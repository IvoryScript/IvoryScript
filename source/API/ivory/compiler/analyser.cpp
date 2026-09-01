/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    analyser.cpp
 *
 * Module:  IvoryScript compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 01 December 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of 'Analyser' class and associated analysis functions.
 *
 *    Notes:
 * 
 *    Intended for basic structural analysis of a fully transformed AST
 *    (with one exception) prior to code generation.
 * 
 *    Currently its main purpose is to determine whether or not lambda functions
 *    need a closure on reduction, or can be reduced with a direct
 *    jump to their alternative entry point. This is based on simple criteria
 *    which cover the majority of lambda-lifted inner functions.
 *
 *    It also includes the following transformation of lazy values:
 *
 *    e => #!(fromThunk #!(\ -> #!e))
 *
 * Modification history:
 *
 *------------------------------------------------------------------------------
 *
 * License: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 *-----------------------------------------------------------------------------
 */

#include "OptionsParser.h"
#include "ivory/list.h"
#include "ivory/trace.h"
#include "ivory/compiler/analyser.h"
#include "ivory/compiler/class.h"
#include "ivory/compiler/type.h"

#define TRACE

#ifdef TRACE
static Bool traceFlag = FALSE;
static Int probeSeqNo = -1;
#endif

declareTypeCon(Void);

/*----------------------------------------------------------------------------*/

static Lambda* directReducedLambda(Expr expr);

enum RecReservationReason {
   REC_RESERVE_NONE,
   REC_RESERVE_SELF,
   REC_RESERVE_SELF_CLOSURE,
   REC_RESERVE_FORWARD_VALUE,
   REC_RESERVE_FORWARD_CLOSURE
};

static ConstString reservationReasonString(RecReservationReason reason) {
   switch (reason) {
      case REC_RESERVE_SELF:
         return "self";
      case REC_RESERVE_SELF_CLOSURE:
         return "self closure";
      case REC_RESERVE_FORWARD_VALUE:
         return "forward value";
      case REC_RESERVE_FORWARD_CLOSURE:
         return "forward closure";
      case REC_RESERVE_NONE:
      default:
         return "none";
   }
}

static Bool recInitDefnForm(ExprEnumVal form) {
   return form == DEFN ||
          form == CLASS_METHOD_DEFN ||
          form == INSTANCE_METHOD_DEFN;
}

static Bool validReservedRecInit(TypeSig typeSig) {
   return isArrowTypeSig(typeSig) ||
          typeSig == typeCon(Expr) ||
          (isAp(typeSig) && fun(typeSig) == typeCon(Exp));
}

static Defn::RecInitStrategy recInitStrategy(Defn& defn,
                                             TypeSig typeSig,
                                             RecReservationReason reason) {
   switch (reason) {
      case REC_RESERVE_SELF_CLOSURE:
         return directReducedLambda(defn.expr()) != NULL
            ? Defn::REC_INIT_RESERVED
            : Defn::REC_INIT_INDIRECTION;

      case REC_RESERVE_FORWARD_CLOSURE:
         return Defn::REC_INIT_INDIRECTION;

      case REC_RESERVE_SELF:
         return directReducedLambda(defn.expr()) != NULL
            ? Defn::REC_INIT_RESERVED
            : isArrowTypeSig(typeSig)
               ? Defn::REC_INIT_INDIRECTION
               : Defn::REC_INIT_RESERVED;

      case REC_RESERVE_FORWARD_VALUE:
      case REC_RESERVE_NONE:
         return Defn::REC_INIT_RESERVED;
   }

   return Defn::REC_INIT_RESERVED;
}

static Void markAssocClosureValue(Lambda& lambda,
                                  FreeVarAssoc& freeVarAssoc);

static Void propagateDirectLambdaFreeVars(Lambda& lambda,
                                          Analyser& analyser);

static Void forceDirectLambdaClosure(Lambda* lambda,
                                     Bool needsPartialAps,
                                     Analyser& analyser) {
   if (lambda == NULL)
      return;
   if (needsPartialAps)
      lambda->setNoPartialAps(FALSE);
   if (!lambda->needsClosure()) {
      lambda->setNeedsClosure(TRUE);
      propagateDirectLambdaFreeVars(*lambda, analyser);
   }
}

static Void forceClosure(Defn& defn, Analyser& analyser) {
   forceDirectLambdaClosure(defn.directLambda(), FALSE, analyser);
}

static Bool recNameOccNeedsClosure(Defn& defn,
                                   TypedVal* typedVal,
                                   Analyser& analyser) {
   TypedVal* recInitTypedVal = analyser.recInitTypedVal();
   if (recInitTypedVal != NULL) {
      UInt closureDepth = recInitTypedVal == typedVal
         ? analyser.recInitSelfValueLambdaDepth()
         : analyser.recInitLambdaDepth();
      return analyser.lambdaDepth() > closureDepth;
   }

   Lambda* directLambda = defn.directLambda();
   if (directLambda != NULL && analyser.lambda() == directLambda)
      return FALSE;
   return analyser.lambda() != defn.parentLambda();
}

static Void markReserved(Name name,
                         TypedVal* typedVal,
                         Defn& defn,
                         RecReservationReason reason,
                         Analyser& analyser) {
   if (defn.recInitStrategy() == Defn::REC_INIT_DEFAULT) {
      TypeSig typeSig = typedVal->typeSig();
      if (!validReservedRecInit(typeSig)) {
         Char buf[256];
         sprintf(buf, "Recursive initialisation of '%s' requires an expression or function type",
                 nameString_(name, analyser.nameTable()));
         defn.setRecInitStrategy(Defn::REC_INIT_INVALID);
         analyser.error(buf);
#ifdef TRACE
         if (traceFlag) {
            outStream << "Rec init: ";
            printName(name, outStream, analyser.nameTable());
            outStream << " -> invalid (" << reservationReasonString(reason) << ")\n";
         }
#endif
         return;
      }
      defn.setRecInitStrategy(recInitStrategy(defn, typeSig, reason));
      if (reason == REC_RESERVE_SELF_CLOSURE ||
          reason == REC_RESERVE_FORWARD_CLOSURE)
         forceClosure(defn, analyser);
#ifdef TRACE
      if (traceFlag) {
         outStream << "Rec init: ";
         printName(name, outStream, analyser.nameTable());
         outStream << (defn.recInitStrategy() == Defn::REC_INIT_INDIRECTION
            ? " -> indirection ("
            : " -> reserved (")
            << reservationReasonString(reason) << ")\n";
      }
#endif
   }
   else if (defn.recInitStrategy() == Defn::REC_INIT_RESERVED &&
            (reason == REC_RESERVE_SELF_CLOSURE ||
             reason == REC_RESERVE_FORWARD_CLOSURE)) {
      forceClosure(defn, analyser);
      if (reason == REC_RESERVE_FORWARD_CLOSURE ||
          directReducedLambda(defn.expr()) == NULL)
         defn.setRecInitStrategy(Defn::REC_INIT_INDIRECTION);
#ifdef TRACE
      if (traceFlag) {
         outStream << "Rec init: ";
         printName(name, outStream, analyser.nameTable());
         outStream << (defn.recInitStrategy() == Defn::REC_INIT_INDIRECTION
            ? " -> indirection (additional cause: "
            : " retains reserved (additional cause: ")
                   << reservationReasonString(reason) << ")\n";
      }
#endif
   }
#ifdef TRACE
   else if (traceFlag && defn.recInitStrategy() == Defn::REC_INIT_RESERVED) {
      outStream << "Rec init: ";
      printName(name, outStream, analyser.nameTable());
      outStream << " retains reserved (additional cause: "
                << reservationReasonString(reason) << ")\n";
   }
   else if (traceFlag && defn.recInitStrategy() == Defn::REC_INIT_INDIRECTION) {
      outStream << "Rec init: ";
      printName(name, outStream, analyser.nameTable());
      outStream << " retains indirection (additional cause: "
                << reservationReasonString(reason) << ")\n";
   }
   else if (traceFlag && defn.recInitStrategy() == Defn::REC_INIT_INVALID) {
      outStream << "Rec init: ";
      printName(name, outStream, analyser.nameTable());
      outStream << " retains invalid (additional cause: "
                << reservationReasonString(reason) << ")\n";
   }
#endif
}

static Void markCurrentRecBinding(RecReservationReason reason,
                                  TypedVal* typedVal,
                                  Analyser& analyser) {
   TypedVal* recInitTypedVal = analyser.recInitTypedVal();
   if (recInitTypedVal == NULL || recInitTypedVal == typedVal)
      return;

   Expr val = recInitTypedVal->val();
   ExprEnumVal form = formOf(val);
   if (!recInitDefnForm(form))
      return;

   Defn& defn = toBody(val, Defn);
   if (defn.outermost() || !defn.recInitAnalysing())
      return;

   markReserved(analyser.recInitName(),
                recInitTypedVal,
                defn,
                reason,
                analyser);
}

static Void markAssocClosureValue(Lambda& lambda,
                                  FreeVarAssoc& freeVarAssoc) {
   if (!freeVarAssoc.needsClosure()) {
      freeVarAssoc.setNeedsClosure(TRUE);
      if (!freeVarAssoc.isGlobal() &&
          !freeVarAssoc.selfReferential())
         lambda._nNonGlobalFree++;
   }
}

static Void propagateDirectLambdaAssocClosure(Lambda& lambda,
                                              FreeVarAssoc& freeVarAssoc,
                                              Analyser& analyser) {
   if (freeVarAssoc.closedVar() != NULL ||
       freeVarAssoc.typedVal() == NULL)
      return;

   Expr freeVal = freeVarAssoc.typedVal()->val();
   if (formOf(freeVal) != DEFN)
      return;

   Defn& defn = toBody(freeVal, Defn);
   Lambda* directLambda = defn.directLambda();
   if (directLambda == NULL)
      return;

   Bool propagateBinding = lambda.parent() != NULL &&
      !freeVarAssoc.needsClosure();
   markAssocClosureValue(lambda, freeVarAssoc);
   forceDirectLambdaClosure(directLambda, FALSE, analyser);
   analyser.markRecNameOcc(freeVarAssoc.name(),
                           freeVarAssoc.typedVal(),
                           TRUE);

   if (propagateBinding)
      lambda.parent()->analyseBinding(NULL,
                                      freeVarAssoc.typedVal(),
                                      NULL,
                                      defn.parentLambda(),
                                      FALSE,
                                      TRUE,
                                      TRUE,
                                      freeVarAssoc.parent(),
                                      analyser);
}

static Void propagateDirectLambdaFreeVars(Lambda& lambda,
                                          Analyser& analyser) {
   FreeVarAssoc* freeVarAssoc = lambda.freeVarAssocs();
   while (freeVarAssoc != NULL) {
      propagateDirectLambdaAssocClosure(lambda, *freeVarAssoc, analyser);
      freeVarAssoc = freeVarAssoc->next();
   }
}

static Lambda* directReducedLambda(Expr expr) {
   if (isPair(expr)) {
      Expr& f = fst(expr);
      Expr& s = snd(expr);
      if (f == Expr(REDUCE) && formOf(s) == LAMBDA)
         return &toBody(s, Lambda);
   }
   return NULL;
}

/*----------------------------------------------------------------------------*/

// Default analysis of a syntax form

Void AST_Node::analyseValOf(TypeSig typeSig, Analyser& analyser) {
}

/*----------------------------------------------------------------------------*/

// Analysis of a typed value

Void TypedVal::preAnalyse(Analyser& analyser) {
   switch (formOf(_val)) {
      case DEFN: {
         toBody(_val, Defn).preAnalyse(analyser);
         break;
      }

      case INSTANCE_METHOD_DEFN: {
         toBody(_val, InstanceMethodDefn).preAnalyse(analyser);
         break;
      }

      default:
         break;
   }
}

Void TypedVal::analyse(Analyser& analyser) {
   switch (formOf(_val)) {
      case DEFN: {
         toBody(_val, Defn).analyse(_typeSig, analyser);
         break;
      }

      case INSTANCE_METHOD_DEFN: {
         toBody(_val, InstanceMethodDefn).analyse(_typeSig, analyser);
         break;
      }

      default:
         break;
   }
}

/*----------------------------------------------------------------------------*/

// Analysis of a name/typed value binding

Void NameTypedValBinding::preAnalyse(Analyser& analyser) {
   _val.preAnalyse(analyser);
}

// NameTypedValBinding::analyse: Analyse a Name/TypedVal binding

Void NameTypedValBinding::analyse(Analyser& analyser) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "NameTypedValBinding::analyse ";
      printName(name(), outStream, analyser.nameTable());
      outStream << '\n';
   }
#endif

   Expr val = _val.val();
   ExprEnumVal form = formOf(val);
   Bool recInitBinding = recInitDefnForm(form) &&
      toBody(val, Defn).recInitPending();
   Name prevRecInitName = analyser.recInitName();
   TypedVal* prevRecInitTypedVal = analyser.recInitTypedVal();
   UInt prevRecInitLambdaDepth = analyser.recInitLambdaDepth();
   UInt prevRecInitSelfValueLambdaDepth =
      analyser.recInitSelfValueLambdaDepth();

   if (recInitBinding) {
      UInt recInitLambdaDepth = analyser.lambdaDepth();
      UInt recInitSelfValueLambdaDepth = recInitLambdaDepth;
      if (directReducedLambda(toBody(val, Defn).expr()) != NULL)
         recInitSelfValueLambdaDepth = recInitLambdaDepth + 1;
      analyser.setRecInitBinding(name(), &_val,
                                 recInitLambdaDepth,
                                 recInitSelfValueLambdaDepth);
   }

   _val.analyse(analyser);

   if (recInitBinding)
      analyser.setRecInitBinding(prevRecInitName, prevRecInitTypedVal,
                                 prevRecInitLambdaDepth,
                                 prevRecInitSelfValueLambdaDepth);
}

/*----------------------------------------------------------------------------*/

// Analysis of a definition AST node

// Main purpose is to test for a directly reduced lambda that may
// may not need a closure on function application reduction.
// (i.e. by direct jump)

Lambda* Defn::directLambda(Void) const {
   return !_outermost
      ? directReducedLambda(_expr)
      : NULL;
}

Void Defn::preAnalyse(Analyser& analyser) {
   _parentLambda = analyser.lambda();
   if (_parentLambda == NULL)
      _outer = TRUE;
}

Void Defn::analyse(TypeSig typeSig, Analyser& analyser) {
   Bool recInitAnalysis = _recInitState == REC_STATE_PENDING;
   if (recInitAnalysis) {
      _recInitState = REC_STATE_ANALYSING;
      analyser.enterRecInitAnalysis();
   }

   if (!(_isInline || _overridden)) {
      Lambda* lambda = this->directLambda();
      if (lambda != NULL && lambda->nFormalParams() > 0) {
         Bool prevClosureValueBody = analyser.closureValueBody();
         analyser.setClosureValueBody(FALSE);
         lambda->analyse(typeSig, analyser);
         analyser.setClosureValueBody(prevClosureValueBody);
      }
      else
         analyser.analyseValOf(_expr, typeSig, 0);
   }

   if (recInitAnalysis)
      analyser.leaveRecInitAnalysis();

   if (_recInitState == REC_STATE_ANALYSING)
      _recInitState = REC_STATE_READY;
}

/*----------------------------------------------------------------------------*/

// Analysis of a data constructor definition AST node

Void DataConDefn::preAnalyse(Analyser& analyser) {
   toBody(_defn, NameTypedValBinding).preAnalyse(analyser);
   analyser.preAnalyseDeclOrDefns(_selectors);
}

Void DataConDefn::analyse(Analyser& analyser) {
   toBody(_defn, NameTypedValBinding).analyse(analyser);
   analyser.analyseDeclOrDefns(_selectors);
}

/*----------------------------------------------------------------------------*/

// Analysis of a name occurrence AST node
// Ensures that a closure is used for any name occurrence bound
// to a direct lambda value.

Void NameOcc::analyse(Bool needsClosure, Bool needsPartialAps, Analyser& analyser) {
   Expr val = this->val();
   ExprEnumVal form = formOf(val);
   Bool needsClosureValue = needsClosure || needsPartialAps;
   Bool isClosure__ = FALSE;                                                                                                                                                                              
   Lambda* lambda = NULL;
   Bool isGlobal = FALSE;
   Bool isOuter = FALSE;
   if (form == DECL) {
      if (name() == builtInName(closure__)) {
         assert(analyser.lambda() != NULL, "NameOcc::analyse: unexpected null lambda");
         analyser.lambda()->setNeedsClosure(TRUE);
      }
      return;
   } else if (form == DEFN ||
              form == CLASS_METHOD_DEFN ||
              form == INSTANCE_METHOD_DEFN) {
      Defn& defn = toBody(val, Defn);
      lambda = defn.parentLambda();
      isOuter = defn.outer();
      if (defn.outermost())
         isGlobal = TRUE;
      Lambda* directLambda = defn.directLambda();
      if (directLambda != NULL &&
          needsClosureValue)
         forceDirectLambdaClosure(directLambda, needsPartialAps, analyser);
   } else if (form == VAR) {
      Var& var = toBody(val, Var);
      lambda = var.parentLambda();
      isOuter = lambda == NULL;
      isGlobal = var.kind() == Var::GLOBAL_VAR ||
         var.kind() == Var::EXT_GLOBAL_VAR;
   }
   else
      return;

   analyser.markRecNameOcc(name(), typedVal(), needsClosureValue);

   if (analyser.lambda() != NULL)
      analyser.lambda()->analyseBinding(this,
                                        typedVal(),
                                        moduleDefn(),
                                        lambda,
                                        isGlobal,
                                        needsClosure,
                                        needsClosureValue,
                                        freeVarAssoc(),
                                        analyser);
}

Void NameOcc::analyseValOf(Analyser& analyser) {
   analyse(TRUE, TRUE, analyser);
}
/*----------------------------------------------------------------------------*/

// Analysis of a lambda function AST node

// Lambda::hasAscendant: Predicate to test for ancestor match.

Bool Lambda::hasAscendant(const Lambda* lambda) const {
   return lambda == _parent
      ? TRUE
      : (_parent == NULL
         ? FALSE
         : _parent->hasAscendant(lambda));
}

// Lambda::analyseBinding: Analyse binding 
// Primarily for global and free variables

Void Lambda::analyseBinding(NameOcc* nameOcc,
                            TypedVal* typedVal,
                            const ModuleDefn* moduleDefn,
                            Lambda* lambda,
                            Bool isGlobal,
                            Bool needsClosure,
                            Bool needsClosureValue,
                            FreeVarAssoc*& freeVarAssoc,
                            Analyser& analyser) {
   if (lambda != NULL && !hasAscendant(lambda))
      return;

   Bool selfReferential = FALSE;
   if (!isGlobal && typedVal != NULL) {
      Expr freeVal = typedVal->val();
      if (formOf(freeVal) == DEFN) {
         Defn& defn = toBody(freeVal, Defn);
         Lambda* directLambda = defn.directLambda();
         selfReferential = directLambda == this;
      }
   }

// Test for existing free variable association

   FreeVarAssoc* freeVarAssocs = _freeVarAssocs;
   while (freeVarAssocs != NULL) {
      if (!isGlobal && typedVal == freeVarAssocs->typedVal()) {
         freeVarAssoc = freeVarAssocs;
         if (needsClosureValue)
            markAssocClosureValue(*this, *freeVarAssoc);
         if (needsClosure && !_needsClosure) {
            setNeedsClosure(TRUE);
            propagateDirectLambdaFreeVars(*this, analyser);

//--------- Propagate need for closure up ancestor chain
//--------- Associated ancestor free variables already exist

            for (Lambda* parentLambda = _parent;
               parentLambda != NULL && parentLambda != lambda;
               parentLambda = parentLambda->_parent)
               if (!parentLambda->_needsClosure)
                  parentLambda->setNeedsClosure(TRUE);
         }
         return;
      }
      freeVarAssocs = freeVarAssocs->next();
   }

#ifdef TRACE
   if (traceFlag && nameOcc != NULL) {
      outStream << "Adding " <<
         (isGlobal ? "global" : "free") << " variable: ";
      nameOcc->print(outStream, analyser.env());
      outStream << endl;
   }
#endif

   Cell* cell_ = NULL;

   if (!isGlobal) {
      if (needsClosureValue && !selfReferential)
         _nNonGlobalFree++;
      if (needsClosure && !_needsClosure)
         setNeedsClosure(TRUE);
   }

   _freeVarAssocs = new (analyser.msa())
      FreeVarAssoc(_freeVarAssocs,
          NULL,
          nameOcc != NULL ? nameOcc->name() : NULL_NAME,
          typedVal,
          nameOcc != NULL ? nameOcc->_moduleDefn : NULL,
          cell_,
          isGlobal,
          needsClosureValue,
          selfReferential);

// Link to name occurrence or parent free variable association

   freeVarAssoc = _freeVarAssocs;

   if (!isGlobal && _parent != NULL)

//--- Propagate up the ancestry chain

      _parent->analyseBinding(nameOcc,
         typedVal,
         NULL,
         lambda,
         FALSE,
         needsClosure,
         needsClosureValue,
         _freeVarAssocs->parent(),
         analyser);

   if (_needsClosure)
      propagateDirectLambdaAssocClosure(*this, *_freeVarAssocs, analyser);
}

// Void Lambda::analyse: Basic lambda analysis
// Not forcing closure or partial function formation

Void Lambda::analyse(TypeSig typeSig, Analyser& analyser) {
   if (analyser.variableThunk())
      _updatable = FALSE;
   analyser.setVariableThunk(FALSE);

   Lambda* prevLambda = analyser.lambda();
   UInt prevLambdaDepth = analyser.lambdaDepth();
   _parent = prevLambda;
   analyser.setLambda(this);
   analyser.setLambdaDepth(prevLambdaDepth + 1);

   analyser.analyseValOf(_body,
                         _nFormalParams > 0
                            ? resultTypeSig(_typeSig, _nFormalParams)
                           : arg(_typeSig), 0);

// If this lambda needs a closure, ensure any direct lambdas
// bound to a free variable also need closures.

   if (_needsClosure)
      propagateDirectLambdaFreeVars(*this, analyser);

   analyser.setLambda(prevLambda);
   analyser.setLambdaDepth(prevLambdaDepth);
}

// Void Lambda::analyse: Full lambda analysis

Void Lambda::analyseValOf(TypeSig typeSig, Analyser& analyser) {
   _noPartialAps = FALSE;
   setNeedsClosure(TRUE);
   Bool prevClosureValueBody = analyser.closureValueBody();
   analyser.setClosureValueBody(TRUE);
   analyse(typeSig, analyser);
   analyser.setClosureValueBody(prevClosureValueBody);
}

/*----------------------------------------------------------------------------*/

// Analysis of a 'let' AST node

static Void resetRecInitStrategies(Expr declOrDefns) {
   for (; declOrDefns != Nil; declOrDefns = tl(declOrDefns)) {
      NameTypedValBinding& binding = toBody(hd(declOrDefns), NameTypedValBinding);
      Expr val = binding.typedVal().val();
      ExprEnumVal form = formOf(val);
      if (recInitDefnForm(form)) {
         Defn& defn = toBody(val, Defn);
         defn.setRecInitStrategy(Defn::REC_INIT_DEFAULT);
         defn.setRecInitState(Defn::REC_STATE_PENDING);
      }
   }
}

Void Let::analyseValOf(TypeSig typeSig, UInt nReduce, Analyser& analyser) {
   analyser.preAnalyseDeclOrDefns(_declOrDefns);
   if (_recursive) {
      resetRecInitStrategies(_declOrDefns);
      analyser.analyseDeclOrDefns(_declOrDefns);
   } else
      analyser.analyseDeclOrDefns(_declOrDefns);
   analyser.analyseValOf(_expr, typeSig, nReduce - 1);
}

/*----------------------------------------------------------------------------*/

// Analysis of a function application AST node

Bool FnAp::localFnNonPartial(Analyser& analyser,
   Bool& needsClosure) const {
   needsClosure = FALSE;

   if (formOf(_fun) == NAME_OCC) {
      const NameOcc& nameOcc = toConstBody(_fun, NameOcc);
      const TypedVal* typedVal = nameOcc.typedVal();
      Expr boundVal = typedVal->val();

      if (formOf(boundVal) == DEFN) {
         Defn& defn = toBody(boundVal, Defn);
         Lambda* lambda = defn.directLambda();

         if (lambda != NULL) {
            UInt arity = ::arity(lambda->typeSig());

            if (_nArgs < arity)
               lambda->setNoPartialAps(FALSE);

            if (_nArgs == arity) {
               needsClosure =
                  (analyser.closureValueBody() ||
                   (analyser.lambda() != NULL &&
                    analyser.lambda()->needsClosure())) &&
                  recNameOccNeedsClosure(defn, (TypedVal*)typedVal, analyser);
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}

/*

            if (_nArgs != lambda->nFormalParams())
               lambda->setNoPartialAps(FALSE);
            if (_nArgs == lambda->nFormalParams())
               return TRUE;

          if (_nArgs < arity(lambda->typeSig()))
               lambda->setNoPartialAps(FALSE);

            if (_nArgs >= lambda->nFormalParams())
               return TRUE;
*/

Void FnAp::analyseValOf(TypeSig typeSig, UInt nReduce, Analyser& analyser) {
   if (_fun != (Expr)SEQ) {
      Lambda* lambda = directReducedLambda(_fun);
      if (lambda != NULL) {
         if (_nArgs == lambda->nFormalParams())
            lambda->analyse(_typeSig, analyser);
         else {
            NameOcc& funName = toBody(_fun, NameOcc);
            analyser.markRecNameOcc(funName.name(), funName.typedVal(), FALSE);
         }
      }
      else {
         Bool needsClosure = FALSE;

         if (!localFnNonPartial(analyser, needsClosure))
            analyser.analyseValOf(_fun, _typeSig, 0);
         else
            toBody(_fun, NameOcc).analyse(needsClosure, FALSE, analyser);
      }
      for (Int i = (Int)_nArgs; i > 0; --i)
         analyser.analyseValOf(_argV[i - 1], argTypeSig(_typeSig, i), 0);
   } else {
      analyser.analyseValOf(_argV[0], argTypeSig(_typeSig, 1), 0);
      analyser.analyseValOf(_argV[1], argTypeSig(_typeSig, 2), nReduce - 1);
   }
}

/*----------------------------------------------------------------------------*/

Void Case::analyseValOf(TypeSig typeSig, UInt nReduce, Analyser& analyser) {
   analyser.analyseValOf(_arg, _argTypeSig, 0);

   for (UInt i = 0; i < _n; i++) {
      assert(isPair(_altV[i]), "Case::analyseValOf: expected alternative pair");
      analyser.analyseValOf(snd(_altV[i]), typeSig, nReduce - 1);
   }
}

/*----------------------------------------------------------------------------*/

// Analysis of a snippet AST

Void Snippet::analyse(Analyser& analyser) {
   analyser.analyseValOf(_expr, _typeSig, 0);
}

/*----------------------------------------------------------------------------*/

// Analysis of a module AST

Void ModuleDefn::preAnalyseDeclOrDefn(Expr declOrDefn, Analyser& analyser) {
   switch (formOf(declOrDefn)) {
      case TYPE_DEFN:
         break;

      case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).preAnalyse(analyser);
         break;

      case INSTANCE_DATA_CON_DEFN:
         toBody(declOrDefn, InstanceDataConDefn).preAnalyse(analyser);
         break;

      case DATA_CON_DECL:
      case INSTANCE_DATA_CON_DECL:
         break;

      case CLASS_DEFN:
         toBody(declOrDefn, ClassDefn).preAnalyse(analyser);
         break;

      case INSTANCE_DEFN:
         toBody(declOrDefn, InstanceDefn).preAnalyse(analyser);
         break;

      default:
         analyser.preAnalyseDeclOrDefn(declOrDefn);
         break;
   }
}

Void ModuleDefn::analyseDeclOrDefn(Expr declOrDefn, Analyser& analyser) {
   switch (formOf(declOrDefn)) {
      case TYPE_DEFN:
         break;

      case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).analyse(analyser);
         break;

      case INSTANCE_DATA_CON_DEFN:
         toBody(declOrDefn, InstanceDataConDefn).analyse(analyser);
         break;

      case DATA_CON_DECL:
      case INSTANCE_DATA_CON_DECL:
         break;

      case CLASS_DEFN:
         toBody(declOrDefn, ClassDefn).analyse(analyser);
         break;

      case INSTANCE_DEFN:
         toBody(declOrDefn, InstanceDefn).analyse(analyser);
         break;

      default:
         analyser.analyseDeclOrDefn(declOrDefn);
         break;
   }
}

Void ModuleDefn::analyse(Analyser& analyser) {
   mapProc1(preAnalyseDeclOrDefn, _declOrDefns, analyser);
   mapProc1(analyseDeclOrDefn, _declOrDefns, analyser);

#ifdef TRACE
   OptionsParser optionsParser(analyser.options());
   if (optionsParser.hasKey("printAST")) {
      printTranExpr(_declOrDefns, FALSE, outStream, analyser.env(), TRUE, 0);
      outStream << "\n--------------------------------------------" << endl;
   }
#endif
}

/*----------------------------------------------------------------------------*/

// Analysis of an order AST

Void Order::analyse(Analyser& analyser) {
   toBody(_sequence, Lambda).analyseValOf(typeCon(Void), analyser);
}

/*----------------------------------------------------------------------------*/

// Analyser functions

Bool Analyser::condGenThunk(Expr& expr, TypeSig typeSig, UInt nReduce) {
   if (nReduce == 0) {
      Tran(3, traceFlag ? "trace pass=3" : "", _env).tranClosure(expr,
         typeSig,
         !_variableThunk);
      setVariableThunk(FALSE);
      analyseValOf(expr, typeSig, 0);
      return TRUE;
   }
   else 
      return FALSE;
}

Void Analyser::analyseValOf(Expr& expr, TypeSig typeSig, UInt nReduce) {

#ifdef TRACE
   static UInt probe = 0;
   if (++probe == probeSeqNo) {
      outStream << "***** Analyser::analyseValOf: probe hit\n";
      traceFlag = TRUE;
   }
   UInt localProbe = probe;

   if (traceFlag) {
      outStream << "Analyser::analyseValOf: (" << probe << ") : ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env(), TRUE);
      outStream << endl;
   }
#endif

   if (hasTypeVar(typeSig))
      return;

   if (isPair(expr)) {
      Expr& f = fst(expr);
      Expr& s = snd(expr);
      if (f == Expr(REDUCE))
         analyseValOf(s, typeSig, nReduce + 1);
      else if (f == Expr(VARIABLE)) {
         _variableThunk = TRUE;
         expr = s;
         analyseValOf(expr, typeSig, nReduce);
      } else {
         analyseValOf(f, typeSig, nReduce);
         analyseValOf(s, typeSig, nReduce);
      }
   } else
      switch (formOf(expr)) {
         case CLASS_METHOD_DEFN:
         case LAMBDA:
            if (!condGenThunk(expr, typeSig, nReduce))
               toBody(expr, AST_Node).analyseValOf(typeSig, *this);
            break;

         case CASE:
            if (!condGenThunk(expr, typeSig, nReduce))
               toBody(expr, Case).analyseValOf(typeSig, nReduce, *this);
            break;

         case LET:
            if (!condGenThunk(expr, typeSig, nReduce))
               toBody(expr, Let).analyseValOf(typeSig, nReduce, *this);
            break;

         case FN_AP:
            if (!condGenThunk(expr, typeSig, nReduce))
               toBody(expr, FnAp).analyseValOf(typeSig, nReduce, *this);
            break;

         case NAME_OCC:
            toBody(expr, NameOcc).analyseValOf(*this);
            break;

         default:
            if (recInitAnalysis() && isExprTypeSig(typeSig))
               condGenThunk(expr, typeSig, nReduce);
            break;
      }   
}

// Analyser constructor

static UInt analyserLambdaDepth(Lambda* lambda) {
   UInt depth = 0;
   for (; lambda != NULL; lambda = lambda->parent())
      ++depth;
   return depth;
}

Analyser::Analyser(ConstString options, Env& env, MSA& msa, Lambda* lambda/* = NULL*/)
   : _options(options),
     _env(env), _msa(msa),
     _lambda(lambda),
     _lambdaDepth(analyserLambdaDepth(lambda)),
     _variableThunk(FALSE),
     _closureValueBody(FALSE),
     _recInitDepth(0),
     _recInitName(NULL_NAME),
     _recInitTypedVal(NULL),
     _recInitLambdaDepth(0),
     _recInitSelfValueLambdaDepth(0),
     _nErrors(0) {

#ifdef TRACE
   OptionsParser optionsParser(_options);
   traceFlag = optionsParser.hasKey("trace");
   optionsParser.getNum("probe", probeSeqNo);
   traceClassCompilation(traceFlag);
#endif

}

// Mark a pending recursive binding whose value is mentioned during
// declaration analysis.
/*
Void Analyser::markRecNameOcc(Name name, TypedVal* typedVal) {
   if (typedVal == NULL)
      return;

   Expr val = typedVal->val();
   ExprEnumVal form = formOf(val);
   if (!recInitDefnForm(form))
      return;

   Defn& defn = toBody(val, Defn);
   if (defn.outermost() || !defn.recInitPending())
      return;

   Bool needsClosure = recNameOccNeedsClosure(defn, typedVal, *this);
   RecReservationReason reason = defn.recInitAnalysing()
      ? (needsClosure ? REC_RESERVE_SELF_CLOSURE : REC_RESERVE_SELF)
      : (needsClosure ? REC_RESERVE_FORWARD_CLOSURE : REC_RESERVE_FORWARD_VALUE);

   markReserved(name, typedVal, defn, reason, *this);

   if (!defn.recInitAnalysing() && needsClosure)
      markCurrentRecBinding(REC_RESERVE_SELF_CLOSURE, typedVal, *this);
}
*/

Void Analyser::markRecNameOcc(Name name, TypedVal* typedVal,
   Bool needsClosureValue) {
   if (typedVal == NULL)
      return;

   Expr val = typedVal->val();
   ExprEnumVal form = formOf(val);
   if (!recInitDefnForm(form))
      return;

   Defn& defn = toBody(val, Defn);
   if (defn.outermost() || !defn.recInitPending())
      return;

   Bool needsClosure =
      needsClosureValue &&
      recNameOccNeedsClosure(defn, typedVal, *this);

   RecReservationReason reason = defn.recInitAnalysing()
      ? (needsClosure ? REC_RESERVE_SELF_CLOSURE : REC_RESERVE_SELF)
      : (needsClosure ? REC_RESERVE_FORWARD_CLOSURE : REC_RESERVE_FORWARD_VALUE);

   markReserved(name, typedVal, defn, reason, *this);

   if (!defn.recInitAnalysing() && needsClosure)
      markCurrentRecBinding(REC_RESERVE_SELF_CLOSURE, typedVal, *this);
}

Void Analyser::error(ConstString s) {
   _nErrors++;
   ivoryError1("Analysis error: %s", s);
}

// Analyse a list of definitions

Void Analyser::preAnalyseDeclOrDefn(Expr declOrDefn) {
   assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "Analyser::analyseDeclOrDefn: Expected binding");
   toBody(declOrDefn, NameTypedValBinding).preAnalyse(*this);
}

Void Analyser::preAnalyseDeclOrDefns(Expr declOrDefns) {
   mapProc(preAnalyseDeclOrDefn, declOrDefns);
}

Void Analyser::analyseDeclOrDefn(Expr declOrDefn) {
   assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "Analyser::analyseDeclOrDefn: Expected binding");
   toBody(declOrDefn, NameTypedValBinding).analyse(*this);
}

Void Analyser::analyseDeclOrDefns(Expr declOrDefns) {
   mapProc(analyseDeclOrDefn, declOrDefns);
}

// Analyse a topmost AST expression

Void Analyser::analyse(Expr expr) {
   switch (formOf(expr)) {
      case MODULE:
         toBody(expr, ModuleDefn).analyse(*this);
         break;

      case ORDER:
         toBody(expr, Order).analyse(*this);
         break;

      case SNIPPET:
         toBody(expr, Snippet).analyse(*this);
         break;

      default: 
         break;
   }
}

/*----------------------------------------------------------------------------*/
