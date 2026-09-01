/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    codeGen.cpp
 *
 * Module:  IovryScript compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    IvoryScript intermediate code generator
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

#include <string.h>

#include "OptionsParser.h"
#include "ivory/bool.h"
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/ref.h"
#include "ivory/string.h"
#include "ivory/trace.h"
#include "ivory/compiler/analyser.h"
#include "ivory/compiler/class.h"
#include "ivory/compiler/code.h"
#include "ivory/compiler/tran.h"
#include "ivory/compiler/type.h"

#define TRACE

#define USE_INSTR_SEQ

declareTypeCon(Byte);

declareTypeCon(Char);

declareTypeCon(Env);

declareTypeCon(InputStream);

declareTypeCon(Int);

declareTypeCon(Name);

declareTypeCon(OutputStream);

declareTypeCon(Ptr);

declareTypeCon(Ref);

declareTypeCon(String);

declareTypeCon(Type);

declareTypeCon(Void);

declareBuiltInDataCon(Void);

static Name closureName = builtInName(closure__);

#ifdef TRACE
static Bool traceFlag = FALSE;
#endif

static Bool isDefnBindingForm(ExprEnumVal form) {
   return form == DEFN ||
          form == CLASS_METHOD_DEFN ||
          form == INSTANCE_METHOD_DEFN;
}

Var::Var(Name name, TypeSig typeSig,
   Kind kind, Lambda* lambda /* = NULL */)
 : _name(name), _typeSig(typeSig), _kind(kind),
   _parentLambda(lambda), _lambda(NULL),
   _reprInfo(ReprInfo { Repr::REPR_UNKNOWN ,FALSE }),
   _reservedDefn(NULL), _reserved(FALSE),
   _nextUse(FALSE), _lastNextUse(FALSE), _killPending(FALSE),
   _structTemplate(NULL), _closure(NULL), _index(0),
   _refCount(0), _entrySkipCount(0), _contLabel(NULL)

#if (GARBAGE_COLLECTION==1)
   , _gcLab(NULL)
   , _gcStackOffset(0)
   , _gcEnvStackOffset(0)
#endif

{
}

Var& Code::newVar(TypeSig typeSig, Var::Kind kind) {
   return  *new(_msa) Var(newName(), typeSig, kind, _lambda);
}

Void Code::reserve(Var& var) {
   if (var.kind() != Var::NON_GLOBAL_FREE_VAR)
      addInstruction(new(_msa) ReserveInstruction(varOperand(var)));
}

// TypedVal::reserve: Generate code to reserve a TypedVal

Void TypedVal::reserve(Name name, Code& code) {
   switch (formOf(_val)) {
      case DEFN: {
         toBody(_val, Defn).reserve(_val, name, _typeSig, code);
         break;
      }

      case INSTANCE_METHOD_DEFN: {
         toBody(_val, InstanceMethodDefn).reserve(_val, name, _typeSig, code);
         break;
      }

      default:
         break;
   }
}

// TypedVal::gen: Generate code for a TypedVal

Void TypedVal::gen(Name name, CodeLabel& errLab, Code& code) {
   switch (formOf(_val)) {
      case DEFN: {
         toBody(_val, Defn).gen(_val, name, _typeSig, errLab, code);
         break;
      }

      case DECL:
         toBody(_val, Decl).gen(name, _typeSig, errLab, code);
         break;

      case INSTANCE_METHOD_DEFN: {
         toBody(_val, InstanceMethodDefn).gen(_val, name, _typeSig, errLab, code);
         break;
      }

      case VAR:
         toBody(_val, Var).gen(errLab, code);
         break;

      case VAR_REF:
         toBody(_val, VarRef).var().gen(errLab, code);
         break;

      default:
         break;
   }
}

// NameTypedValBinding::needsRecReservation: Return whether analysis selected
// an early slot for a recursive binding.

Bool NameTypedValBinding::needsRecReservation(Void) const {
   Expr val = _val.val();
   ExprEnumVal form = formOf(val);
   if (form == DEFN ||
      form == CLASS_METHOD_DEFN ||
      form == INSTANCE_METHOD_DEFN)
      return toBody(val, Defn).recInitStrategy() == Defn::REC_INIT_RESERVED ||
      toBody(val, Defn).recInitStrategy() == Defn::REC_INIT_INDIRECTION;
   return FALSE;
}

// NameTypedValBinding::reserve: Generate code to reserve a Name/TypedVal binding

Void NameTypedValBinding::reserve(Code& code) {
   _val.reserve(name(), code);
}

// NameTypedValBinding::reserveRec: Generate code to reserve an analysed recursive binding

Void NameTypedValBinding::reserveRec(Code& code) {
   assert(needsRecReservation(),
      "NameTypedValBinding::reserveRec: Expected analysed recursive reservation");
   Expr oldVal = _val.val();
   _val.reserve(name(), code);
   Expr newVal = _val.val();
   if (oldVal != newVal && isDefnBindingForm(formOf(oldVal))) {
      Lambda* lambda = toBody(oldVal, Defn).directLambda();
      if (lambda != NULL)
         lambda->rebindFreeVarAssoc(oldVal, newVal);
   }
}

// NameTypedValBinding::gen: Generate code for a Name/TypedVal binding

Void NameTypedValBinding::gen(CodeLabel& errLab, Code& code) {
   Expr oldVal = _val.val();
   _val.gen(name(), errLab, code);
   Expr newVal = _val.val();
   if (oldVal != newVal && isDefnBindingForm(formOf(oldVal)) &&
      code.lambda() != NULL)
      code.lambda()->rebindFreeVarAssoc(oldVal, newVal);
}

// ModuleDefn::gen: Generate code for a module definition
// T.B.D. Consider introducing a lambda function to avoid the code duplication

Bool ModuleDefn::gen(Code& code) {

#ifdef TRACE
   OptionsParser optionsParser(code._options);
   traceFlag = optionsParser.hasKey("trace") &&
               optionsParser.hasKey("gen");
#endif

   code._moduleDefn = this;

	CodeLabel& codeLabel = code.newLabel(TRUE);
   codeLabel._saveEntryFlag = TRUE;
	codeLabel.incRef();  // Inhibit removal of redundant code
   code.genLabel(codeLabel);

   code._errLab = &code.newLabel(TRUE);

   mapProc1(reserveDeclOrDefn, _declOrDefns, code);
   mapProc2(genDeclOrDefn, _declOrDefns, *code._errLab, code);
   if (code.nErrors() > 0)
      return FALSE;

   code.genTypeMethods(*code._errLab);
   if (code.nErrors() > 0)
      return FALSE;

   code.ret();

   if (code._errLab->useCount() > 0) {
      code.genLabel(*code._errLab);
      code.addInstruction(new(code.msa()) ExceptionInstruction());
   }
   else
      code.removeBasicBlk(code._errLab->basicBlk());

   if (code._instrSeq->_appendSeq != NULL)
      code._instrSeq->append(*code._instrSeq->_appendSeq);

   code._lambda = NULL;
   return TRUE;
}

Void ModuleDefn::reserveDeclOrDefn(Expr declOrDefn, Code& code) {
   switch (formOf(declOrDefn)) {
      case TYPE_DEFN:
      case DATA_CON_DECL:
         break;

      case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).reserve(code);
         break;

      case CLASS_DEFN:
         toBody(declOrDefn, ClassDefn).reserve(code);
         break;

      case INSTANCE_DEFN:
         toBody(declOrDefn, InstanceDefn).reserve(code);
         break;

      default:
         code.reserveDeclOrDefn(declOrDefn);
         break;
   }
}

Void ModuleDefn::genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab, Code& code) {     
   switch (formOf(declOrDefn)){
      case TYPE_DEFN:
      case DATA_CON_DECL:
         break;

		case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).gen(errLab, code);
         break;

		case CLASS_DEFN:
         toBody(declOrDefn, ClassDefn).gen(errLab, code);
         break;
      
      case INSTANCE_DEFN:
         toBody(declOrDefn, InstanceDefn).gen(errLab, code);
         break;

		default:
			code.genDeclOrDefn(declOrDefn, errLab);
         break;
   }
}

Bool Order::gen(Code& code) {

#ifdef TRACE
   OptionsParser optionsParser(code._options);
   traceFlag = optionsParser.hasKey("trace") &&
               optionsParser.hasKey("gen");
#endif

   assert(formOf(_sequence) == LAMBDA, "Order::gen: Expected lambda");
   toBody(_sequence, Lambda)._isOrder = TRUE;
   code._errLab = &code.newLabel(TRUE);
	toBody(_sequence, Lambda).gen(NULL, *code._errLab, code, FALSE, FALSE);
   if (code.nErrors() > 0)
      return FALSE;

   if (code._types != NULL) {
      CodeLabel& codeLabel = code.newLabel(TRUE);
      codeLabel.incRef();  // Inhibit removal of redundant code
      code.genLabel(codeLabel);
      code.genTypeMethods(*code._errLab);
      if (code.nErrors() > 0)
         return FALSE;

      code.ret();
   }

   if (code._errLab->useCount() > 0) {
      code.genLabel(*code._errLab);
      code.addInstruction(new(code.msa()) ExceptionInstruction());
   }
   else
      code.removeBasicBlk(code._errLab->basicBlk());
   return TRUE;
}

Expr AST_Node::reduce(CodeLabel& failLab, Code& code) {
   notOverloaded("reduce", code.env());
   return ERROR;
}

Bool AST_Node::isConstReduced(Expr& expr, const Code& code) const {
   notOverloaded("isConstReduced", code.env());
   return FALSE;
}

Void AST_Node::genVar(Var& dst, CodeLabel& failLab, Code& code) {
   notOverloaded("genVar", code.env());
}

Void AST_Node::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   notOverloaded("genVarReduced", code.env());
}

Operand& AST_Node::genOperand(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   notOverloaded("genOperand", code.env());
   return *(Operand*)NULL;
}

Operand& AST_Node::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(typeSig, Var::LOCAL_VAR);
   genVarReduced(var, failLab, code);
   return code.varOperand(var);
}

Void AST_Node::genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   notOverloaded("genEnter", code.env());
}

Void AST_Node::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   notOverloaded("genEnterReduced", code.env());
}

Void AST_Node::genCond(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                         Bool divergent, CodeLabel& failLab, Code& code) {
   notOverloaded("genCond", code.env());
}

Void AST_Node::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                                Bool divergent, CodeLabel& failLab, Code& code) {
   notOverloaded("genCondReduced", code.env());
}

Void AST_Node::genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
   notOverloaded("genSelect", code.env());
}

Void AST_Node::genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
   notOverloaded("genSelectReduced", code.env());
}

Void AST_Node::genVoidReduced(CodeLabel& failLab, Code& code) {
   notOverloaded("genVoid", code.env());
}

Void AST_Node::genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   notOverloaded("genReturn", code.env());
}

Void AST_Node::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   notOverloaded("genReturnReduced", code.env());
}

Void DataConDefn::reserve(Code& code) {
   toBody(_defn, NameTypedValBinding).reserve(code);
   code.reserveDeclOrDefns(_selectors);
}

Void DataConDefn::gen(CodeLabel& errLab, Code& code) {
	toBody(_defn, NameTypedValBinding).gen(errLab, code);
   code.genDeclOrDefns(_selectors, errLab);
}

// Decl::gen: Generate code for an imported declaration

Void Decl::gen(Name name, TypeSig typeSig, CodeLabel& errLab, Code& code) {
   if (_outermost)
	   _var = new(code.msa()) Var(name, typeSig, Var::GLOBAL_VAR);
}

Void Decl::genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   _var->genEnter(fnAp, code);
}

Void Decl::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   _var->genEnterReduced(fnAp, code);
}

// Defn::reserve: Generate code to reserve a definition

Void Defn::reserve(Expr& expr, Name name, TypeSig typeSig, Code& code) {
   if (!(_isInline || _overridden)) {
      Lambda* lambda = directLambda();

      Bool needsStorage =
         lambda == NULL ||
         lambda->needsClosure() ||
         _recInitStrategy == REC_INIT_RESERVED ||
         _recInitStrategy == REC_INIT_INDIRECTION;

      if (needsStorage && !(lambda != NULL && !lambda->needsClosure())) {

#ifdef TRACE
         if (traceFlag) {
            outStream << "Defn::reserve: ";
            printName(name, outStream, code.nameTable());
            print(outStream, code.env());
            outStream << endl;
         }
#endif

         Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
         new(cell_.body()) Var(name, typeSig,
            _outermost ? Var::GLOBAL_VAR : Var::LOCAL_VAR,
            code._lambda);

         Var& var = cellBody(cell_, Var);
         var.setReservedDefn(this);
         expr = fromCell(cell_);
         _boundVal = expr;

         if (_recInitStrategy == REC_INIT_INDIRECTION) {
            code.addIndirectionAllocInstruction(code.varOperand(var));
         }
         else {
            code.reserve(var);
         }
      }
   }
}

// Defn::gen: Generate code for a definition

Void Defn::gen(Expr& expr, Name name, TypeSig typeSig, CodeLabel& errLab, Code& code) {
   if (!(_isInline || _overridden)) {

#ifdef TRACE
      if (traceFlag) {
         outStream << "Defn::gen: ";
         printName(name, outStream, code.nameTable());
         print(outStream, code.env());
         outStream << endl;
      }
#endif

      Lambda* lambda = directLambda();
      if (formOf(expr) == VAR && _recInitStrategy == REC_INIT_RESERVED &&
         (lambda == NULL || lambda->needsClosure())) {
         _boundVal = expr;
         code.genVar(_expr, toBody(expr, Var), errLab);
      }
      else if (lambda == NULL || lambda->needsClosure()) {
         if (formOf(_expr) != VAR) {
            Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
            new(cell_.body()) Var(name, typeSig,
               _outermost ? Var::GLOBAL_VAR : Var::LOCAL_VAR,
               code._lambda);
            expr = fromCell(cell_);
            _boundVal = expr;
            code.genVar(_expr, cellBody(cell_, Var), errLab);
         }
         else {
            expr = _expr;
            _boundVal = expr;
         }
      } else {
         Cell& cell_ = *new(sizeof(DirectLambda), code.msa()) Cell(DIRECT_LAMBDA);
         new(cell_.body()) DirectLambda(lambda);
         expr = fromCell(cell_);
         _boundVal = expr;
         lambda->gen(NULL, errLab, code, FALSE);
      }
   }
}

Expr Defn::reduce(CodeLabel& failLab, Code& code) {
   return code.reduce(_expr, failLab);
}

Void Defn::genVar(Var& dst, CodeLabel& failLab, Code& code){
  code.genVar(_expr, dst, failLab);
}

Void Defn::genVarReduced(Var& dst, CodeLabel& failLab, Code& code){
  code.genVarReduced(_expr, dst, failLab);
}

Operand& Defn::genOperand(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   return code.genOperand(_expr, typeSig, failLab);
}

Operand& Defn::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   return code.genOperandReduced(_expr, typeSig, failLab);
}

Void Defn::genVoidReduced(CodeLabel& failLab, Code& code) {
	code.genVoidReduced(_expr, failLab);
}

Void Defn::genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
	code.genSelect(_expr, typeSig, byPtr, index, dst, failLab); 
}

Void Defn::genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
   code.genSelectReduced(_expr, typeSig, byPtr, index, dst, failLab);
}

Void Defn::genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   code.genEnter(_expr, typeSig, fnAp, failLab);
}

Void Defn::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   code.genEnterReduced(_expr, typeSig, fnAp, failLab);
}


Void Defn::genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   code.genReturn(_expr, typeSig, failLab);
}

Void Defn::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   code.genReturn(_expr, typeSig, failLab);
}

static FreeVarAssoc& rootFreeVarAssoc(FreeVarAssoc& freeVarAssoc) {
   FreeVarAssoc* rootAssoc = &freeVarAssoc;
   while (rootAssoc->parent() != NULL)
      rootAssoc = rootAssoc->parent();
   return *rootAssoc;
}

static Expr resolvedDefnBindingVal(Expr val) {
   if (isDefnBindingForm(formOf(val))) {
      Expr boundVal = toBody(val, Defn).boundVal();
      if (boundVal != (Expr)ERROR)
         return boundVal;
   }
   return val;
}

static Expr freeVarAssocVal(FreeVarAssoc& freeVarAssoc) {
   FreeVarAssoc& rootAssoc = rootFreeVarAssoc(freeVarAssoc);
   Expr freeVarVal = rootAssoc.val();
   TypedVal* typedVal = rootAssoc.typedVal();
   if (typedVal != NULL) {
      Expr typedValExpr = typedVal->val();
      if (typedValExpr != (Expr)ERROR) {
         Expr resolvedVal = resolvedDefnBindingVal(typedValExpr);
         if (formOf(resolvedVal) == VAR || resolvedVal != typedValExpr) {
            rootAssoc.setVal(resolvedVal);
            return resolvedVal;
         }
      }
   }
   Expr resolvedVal = resolvedDefnBindingVal(freeVarVal);
   if (resolvedVal != freeVarVal) {
      rootAssoc.setVal(resolvedVal);
      freeVarVal = resolvedVal;
   }
   return freeVarVal;
}

static Bool freeVarAssocNeedsStorage(const FreeVarAssoc& freeVarAssoc) {
   return freeVarAssoc.isGlobal() ||
          freeVarAssoc.needsClosure() ||
          freeVarAssoc.selfReferential();
}

// Lambda code generation

Void Lambda::rebindFreeVarAssoc(Expr oldVal, Expr newVal) {
   for (FreeVarAssoc* freeVarAssoc = _freeVarAssocs;
      freeVarAssoc != NULL;
      freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->val() == oldVal)
         freeVarAssoc->setVal(newVal);
   }
}

// Lambda::allocFreeVars: allocate locally rebound free variables
// Returns a unique cell template

StructTemplate& Lambda::allocFreeVars(UInt nSlots, Code& code, CodeLabel& failLab) {
   StructTemplateAllocator structTemplateAllocator(nSlots, code);
   UInt i = 0;
   FreeVarAssoc* freeVarAssoc = _freeVarAssocs;
   while (freeVarAssoc != NULL) {
      if (!freeVarAssocNeedsStorage(*freeVarAssoc)) {
         freeVarAssoc = freeVarAssoc->next();
         continue;
      }
      Expr freeVarVal = freeVarAssocVal(*freeVarAssoc);
      if (freeVarAssoc->isGlobal()) {
         assert(formOf(freeVarVal) == VAR, "Lambda::allocFreeVars: Expected VAR");
         Var& var = toBody(freeVarVal, Var);
         Name name = var.name();
         if (freeVarAssoc->moduleDefn() != NULL) {

//------ Map variable name if in external module
//------ Note that for class methods this may be decorated
//------ and so not equal to the name occurrence as a string

            name = useName(nameString_(var.name(),
               freeVarAssoc->moduleDefn()->nameTable()),
               code.nameTable());
         }

         Cell* cell_ = new(sizeof(Var), code.msa()) Cell(VAR);
         freeVarAssoc->closedVar() = cell_;
         new(cell_->body()) Var(name, var.typeSig(), Var::EXT_GLOBAL_VAR);
      } else {
         assert(formOf(freeVarVal) == VAR, "Lambda::allocFreeVars: Expected VAR");
         Var& var = toBody(freeVarVal, Var);
         Cell* cell_ = new(sizeof(Var), code.msa()) Cell(VAR);
         freeVarAssoc->closedVar() = cell_;
         new(cell_->body()) Var(var.name(),
                                var.typeSig(),
                                Var::NON_GLOBAL_FREE_VAR, this);
         if (!freeVarAssoc->selfReferential()) {
            structTemplateAllocator.slot(i, code.useType(var.typeSig()));
            cellBody(*cell_, Var)._index = ++i;
         }
      }
      freeVarAssoc = freeVarAssoc->next();
   }
   return structTemplateAllocator.structTemplate();
}

// Lambda::genFreeVars: map free variables to constructor environment

Void Lambda::genFreeVars(UInt nSlots, Var** slotVars,
                         CodeLabel& failLab, Code& code) const {
   for (FreeVarAssoc* freeVarAssoc = _freeVarAssocs;
      freeVarAssoc != NULL;
      freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->closedVar() == NULL)
         continue;

      Var& closedVar = cellBody(*freeVarAssoc->closedVar(), Var);

      if (closedVar.kind() == Var::NON_GLOBAL_FREE_VAR &&
         !freeVarAssoc->selfReferential()) {

//------ Either free value, or parent closure value

         Expr freeVal = freeVarAssoc->parent() == NULL
            ? freeVarAssocVal(*freeVarAssoc)
            : (Expr)freeVarAssoc->parent()->closedVar();

         while (formOf(freeVal) == DEFN)
            freeVal = toBody(freeVal, Defn).expr();

         Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
         new(cell_.body()) Snippet(
            mkPair(REDUCE,
               ap(mkPair(REDUCE,
                  ap(fromName(useName("condMapToEnv", code.nameTable())),
                     freeVal,
                     code.msa()),
                  code.msa()),
                  fromName(useName("consEnv", code.nameTable())),
                  code.msa()),
               code.msa()));

         Expr snippet = fromCell(cell_);

         UInt nPrevErrors = code.nErrors();
         if (Tran(1, "", code.env()).tran(snippet) &&
            TypeCheck(
#ifdef TRACE
               traceFlag ? "trace" :
#endif
               "", code.env(), code.moduleDefn()).typeCheck(snippet) &&
            Tran(2,
#ifdef TRACE
               traceFlag ? "pass=2 printAST" :
#endif
               "", code.env()).tran(snippet)) {

            Analyser("", code.env(), code.msa()).analyse(snippet);
            slotVars[closedVar._index - 1] = &code.newVar(closedVar.typeSig(), Var::LOCAL_VAR);

            code.genVar(toBody(snippet, Snippet).expr(),
                        *slotVars[closedVar._index - 1], failLab);
         }
         else {
            code.adjErrorCount(1);
         }

         if (code.nErrors() != nPrevErrors) {
            error("Failed to generate code to map closure free variable");
            printTypeSig(closedVar.typeSig(), 0, FALSE, outStream, code.env());
         }
      }
   }
}

// Lambda::copyFreeVars: populate closure slots

Void Lambda::copyFreeVars(UInt nSlots, Var** slotVars,
                          Var& closure, Code& code) const {
   for (UInt i = 0; i < nSlots; ++i) {
      Var* temp = slotVars[i];
  
      Var& var = code.newVar(slotVars[i]->typeSig(), Var::NON_GLOBAL_FREE_VAR);
      var._closure = &closure;
      var._index = i + 1;

      code.addBinaryInstruction(Instruction::MOVE_INSTR,
         code.varOperand(*slotVars[i]),
         code.varOperand(var));
   }
}

// Lambda::genCopyMethod: Generate method to copy
// a closure from one environment to another

CodeLabel* Lambda::genMapMethod(Var& closure, CodeLabel& failLab, Code& code) {
   Name name = useName("copyClosure", code.nameTable());

// Instantiate mapClosure

   Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
   new(cell_.body()) Snippet(
      mkTriple(CONSTRAIN,
         mkPair(COERCE,
                fromName(name), code.msa()),
                arrowTypeSig(
                    typeCon(Env),
                    closure.typeSig(), code.msa()), code.msa()));
   Expr snippet = fromCell(cell_);

   if (!(Tran(1, "", code.env()).tran(snippet) &&
      TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
      Tran(2,

#ifdef TRACE
         traceFlag ? "pass=2 printAST" :
#endif

         "", code.env()).tran(snippet))) {
      code.error("Failed to generate \'mapClosure\' method");
   }
   Analyser("", code.env(), code.msa()).analyse(snippet);
   Lambda& lambda = toBody(snd(toBody(snippet, Snippet).expr()), Lambda);
   lambda._simple = TRUE;
   lambda.gen(NULL, failLab, code, FALSE, TRUE);

   return lambda._altEntryLabel;
}

#if (SERIALISATION==1)

// Lambda::genSerialiseMethod: Generate method to serialise
// a closure's free variables

CodeLabel* Lambda::genSerialiseMethod(Var& closure, Name argName, Name fnName,
                                      TypeSig typeSig, CodeLabel& failLab, Code& code) {

// Instantiate extractClosure or insertClosure

   Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
   new(cell_.body())
      Snippet(mkPair(REDUCE,
         mkTriple(LAMBDA,
            cons(mkTriple(CONSTRAIN,
               fromName(argName),
               ap(typeSig,
                  typeCon(Byte), code.msa()),
               code.msa()),
               Nil, code.msa()),
            mkPair(REDUCE,
               ap(mkPair(REDUCE,
                  ap(fromName(fnName),
                     fromName(argName), code.msa()),
                  code.msa()),
                  mkTriple(CONSTRAIN,
                     fromName(builtInName(closure__)),
                     closure.typeSig(), code.msa()),
                  code.msa()),
               code.msa()),
            code.msa()),
         code.msa()));
   Expr snippet = fromCell(cell_);

   if (!(Tran(1, "", code.env()).tran(snippet) &&
      TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
      Tran(2,

#ifdef TRACE
         traceFlag ? "pass=2 printAST" :
#endif

         "", code.env()).tran(snippet))) {
      code.error("Failed to generate closure serialise method");
   }
   Analyser("", code.env(), code.msa()).analyse(snippet);
   Lambda& lambda = toBody(snd(toBody(snippet, Snippet).expr()), Lambda);
   lambda._simple = TRUE;
   lambda._updatable = FALSE;
   lambda.gen(NULL, failLab, code, FALSE, TRUE);
   return lambda._altEntryLabel;
}
#endif

#if (GARBAGE_COLLECTION==1)

// Lambda::genMarkMethod_GC: Generate method to mark
// the memory segments of a closure's free variables

CodeLabel* Lambda::genMarkMethod_GC(Var& closure, CodeLabel& failLab, Code& code) {
   Name name = useName("markFreeVars_GC", code.nameTable());

// Instantiate markClosure_GC

   Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
   new(cell_.body()) Snippet(mkPair(REDUCE,
                                    mkTriple(LAMBDA,
                                             Nil,
                                             mkPair(REDUCE,
                                                    ap(fromName(name),
                                                       mkTriple(CONSTRAIN,
                                                                fromName(builtInName(closure__)),
                                                                closure.typeSig(),
                                                                code.msa()),
                                                       code.msa()),
                                                code.msa()),
                                             code.msa()),
                                    code.msa()));
   Expr snippet = fromCell(cell_);

   if (!(Tran(1, "", code.env()).tran(snippet) &&
      TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
      Tran(2,

#ifdef TRACE
         traceFlag ? "pass=2 printAST" :
#endif

         "", code.env()).tran(snippet))) {
      code.error("Failed to generate closure mark method");
   }
   Analyser("", code.env(), code.msa()).analyse(snippet);
   Lambda& lambda = toBody(snd(toBody(snippet, Snippet).expr()), Lambda);
   lambda._simple = TRUE;
   lambda._updatable = FALSE;
   lambda.gen(NULL, failLab, code, FALSE, TRUE);
   return lambda._altEntryLabel;
}

#endif

static Expr nameOcc(Name name, TypedVal* typedVal, Code& code) {
   Cell& cell_ = *new(sizeof(NameOcc), code.msa()) Cell(NAME_OCC);
   new(cell_.body()) NameOcc(name, typedVal, (TypeSig)UNKNOWN);
   return fromCell(cell_);
}

static Expr fnAp(Expr fun, Expr* argV, UInt nArgs, Code& code) {
   Cell& cell_ = *new(sizeof(FnAp), code.msa()) Cell(FN_AP);
   new(cell_.body()) FnAp(fun, argV, nArgs, (TypeSig)UNKNOWN, code.msa(), FALSE);
   return fromCell(cell_);
}

static Void importVar(CodeLabel& codeLabel, Var& var, Code& code) {
   codeLabel.basicBlk().vars().addElement(var, code.msa())->setFlags(
      (VarElement::Flags)(VarElement::D | VarElement::I));
}

static Void importFormalParameters(CodeLabel& codeLabel,
                                   NameTypedValBinding* formalParamV,
                                   UInt n,
                                   Code& code) {
   for (UInt i = n; i > 0;)
      importVar(codeLabel, toBody(formalParamV[--i].typedVal().val(), Var), code);
}

static Void importClosureAndFreeVars(CodeLabel& codeLabel,
                                     Var* closure,
                                     FreeVarAssoc* freeVarAssocs,
                                     Bool includeFreeVars,
                                     Code& code) {
   if (closure == NULL)
      return;

   importVar(codeLabel, *closure, code);

   if (!includeFreeVars)
      return;

   for (FreeVarAssoc* freeVarAssoc = freeVarAssocs;
        freeVarAssoc != NULL;
        freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->selfReferential())
         continue;
      if (freeVarAssoc->closedVar() == NULL)
         continue;
      Var& var = cellBody(*freeVarAssoc->closedVar(), Var);
      if (var.kind() != Var::GLOBAL_VAR &&
         var.kind() != Var::EXT_GLOBAL_VAR)
         var._closure = closure;
      importVar(codeLabel, var, code);
   }
}

NameTypedValBinding* Lambda::allocCurried(UInt arity, 
                                          CodeLabel* mainLabel,
                                          Code& code) {
   importFormalParameters(*mainLabel, _formalParamV, _nFormalParams, code);

   UInt nCurriedFormalParams = arity - _nFormalParams;
   NameTypedValBinding* curriedFormalParamV =
   (NameTypedValBinding*)code.msa().alloc(nCurriedFormalParams *
      sizeof(NameTypedValBinding));

   for (UInt i = 0; i < nCurriedFormalParams; i++) {
      Name name = newName();
      TypeSig typeSig = argTypeSig(_typeSig, _nFormalParams + i + 1);

      Cell& varCell = *new(sizeof(Var), code.msa()) Cell(VAR);
      new(varCell.body()) Var(name,
         typeSig,
         Var::EXTRA_FORMAL_PARAM_VAR,
         this);

      new(&curriedFormalParamV[i]) NameTypedValBinding(name,
         fromCell(varCell),
         typeSig,
         code.msa());
   }
   return curriedFormalParamV;
}

Void Lambda::setupClosureCell(StructTemplate* structTemplate,
                              CodeLabel* mainLabel, Code& code) {

// Use locally bound free variable if recursive

//--- Use locally bound free variable if recursive

   for (FreeVarAssoc* freeVarAssoc = _freeVarAssocs;
      freeVarAssoc != NULL;
      freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->selfReferential()) {
         _closureCell = freeVarAssoc->closedVar();
         _closure = &cellBody(*_closureCell, Var);
         _closure->_kind = Var::CLOSURE_VAR;
         break;
      }
   }

   if (_closureCell == NULL) {
      _closureCell = new(sizeof(Var), code.msa()) Cell(VAR);
      new(_closureCell->body()) Var(newName(), _typeSig, Var::CLOSURE_VAR, this);
      _closure = &cellBody(*_closureCell, Var);
   }

   _closure->_lambda = this;
   _closure->_structTemplate = structTemplate;

   if (_entryLabel != NULL)
      importClosureAndFreeVars(*_entryLabel,
         _closure, _freeVarAssocs, TRUE, code);

   importClosureAndFreeVars(mainLabel == NULL ? *_altEntryLabel
      : *mainLabel,
      _closure, _freeVarAssocs, TRUE, code);
}

// Lambda::genPartial: Generate partial application closure for a given number
// of arguments.  Originally this was just a thunk to wrap partially applied
// arguments and reenter the function.  Now it forms a lambda of 1 argument
// which (by definition) must be present.  The need for this was due to a
// change to the calling convention whereby the first argument is passed in a
// register. The formal parameter now ensures that its kind is EXTRA_ARG_VAR for the tail call.

Expr Lambda::partial(UInt nArgs, TypedVal* closure, CodeLabel& codeLab, Code& code) {
   Cell& lambdaCell = *new(sizeof(Lambda), code.msa()) Cell(LAMBDA);

   Cell& fpCell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(fpCell_.body()) Var(_formalParamV[nArgs].name(), (TypeSig)UNKNOWN,
      Var::FORMAL_PARAM_VAR,
      &cellBody(lambdaCell, Lambda));
   NameTypedValBinding* formalParamV = new (code.msa())
      NameTypedValBinding(_formalParamV[nArgs].name(),
         fromCell(fpCell_), (TypeSig)UNKNOWN, code.msa());

   Expr* argV = (Expr*)code.msa().alloc((nArgs + 1) * sizeof(Expr));
   for (UInt i = 0; i < nArgs + 1; i++)
      argV[i] = nameOcc(_formalParamV[i].name(),
         i < nArgs ? &_formalParamV[i].typedVal() : NULL,
         code);

   Expr fun = nameOcc(toBody(closure->val(), Var).name(),
      closure,
      code);

   Expr ap = fnAp(fun, argV, nArgs + 1, code);

   new(lambdaCell.body()) Lambda(1, formalParamV,
      mkPair(REDUCE, ap, code.msa()),
      (TypeSig)UNKNOWN);

   cellBody(lambdaCell, Lambda).setNoPartialAps(FALSE);

   return fromCell(lambdaCell);
}

// Lambda::genPartialApFn: Generate partial function application

Void Lambda::genPartialApFn(CodeLabel& codeLabel,
                            UInt nArgs,
                            CodeLabel& failLab,
                            Code& code) {
   codeLabel.basicBlk().vars().addElement(*_closure, code.msa())->setFlags(
                                          (VarElement::Flags)(VarElement::D | VarElement::I));

   importFormalParameters(codeLabel, _formalParamV, nArgs, code);
   importClosureAndFreeVars(codeLabel, _closure, _freeVarAssocs, TRUE, code);

// The following provides a binding for the function name

   TypedVal closureTypedVal(fromCell(*_closureCell), _closure->typeSig());
   Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
   new(cell_.body()) Snippet(partial(nArgs, &closureTypedVal, codeLabel, code));
   Expr snippet = fromCell(cell_);

   if (!(Tran(1, "", code.env()).tran(snippet) &&
      TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
      Tran(2,

#ifdef TRACE                  
         traceFlag ? "pass=2 printAST" :
#endif

         "", code.env()).tran(snippet))) {
      code.error("Failed to generate partial closure");
   }
   Lambda& lambda = toBody(toBody(snippet, Snippet).expr(), Lambda);
   Analyser analyser("", code.env(), code.msa(), this);
   lambda.analyse(lambda.typeSig(), analyser);
   lambda.genReturnReduced(lambda.typeSig(), failLab, code);
}

// Lambda::curried: Returns a surrogate lambda for a curried function application

Expr Lambda::curried(UInt nArgs,
                     NameTypedValBinding* curriedFormalParamV,
                     CodeLabel& mainLabel,
                     Code& code) {
   assert(nArgs > _nFormalParams, "Lambda::curried: expected over-supply");

   UInt nExtra = nArgs - _nFormalParams;

   Cell& lambdaCell = *new(sizeof(Lambda), code.msa()) Cell(LAMBDA);
   NameTypedValBinding* formalParamV =
      (NameTypedValBinding*)code.msa().alloc(nArgs * sizeof(NameTypedValBinding));

   for (UInt i = 0; i < _nFormalParams; i++)
      formalParamV[i] = _formalParamV[i];

   for (UInt i = 0; i < nExtra; i++)
      formalParamV[_nFormalParams + i] = curriedFormalParamV[i];

   Expr* baseArgs = (Expr*)code.msa().alloc(_nFormalParams * sizeof(Expr));
   for (UInt i = 0; i < _nFormalParams; i++)
      baseArgs[i] = nameOcc(formalParamV[i].name(),
         &formalParamV[i].typedVal(),
         code);

   Cell& entryCell =
      *new(sizeof(CurriedLambda), code.msa()) Cell(CURRIED_LAMBDA);
   new(entryCell.body()) CurriedLambda(this, &mainLabel);

   Expr baseAp =
      fnAp(fromCell(entryCell),
         baseArgs,
         _nFormalParams,
         code);
   toBody(baseAp, FnAp).setNoCurry(TRUE);

   Expr* extraArgs = (Expr*)code.msa().alloc(nExtra * sizeof(Expr));
   for (UInt i = 0; i < nExtra; i++)
      extraArgs[i] = nameOcc(formalParamV[_nFormalParams + i].name(),
         &formalParamV[_nFormalParams + i].typedVal(),
         code);

   Expr body =
      fnAp(mkPair(REDUCE, baseAp, code.msa()),
         extraArgs,
         nExtra,
         code);
   toBody(body, FnAp).setNoCurry(TRUE);

   new(lambdaCell.body()) Lambda(nArgs,
      formalParamV,
      mkPair(REDUCE, body, code.msa()),
      (TypeSig)UNKNOWN);

   for (UInt i = 0; i < nArgs; i++)
      toBody(formalParamV[i].typedVal().val(), Var).setParentLambda(this);

   cellBody(lambdaCell, Lambda).setNoPartialAps(TRUE);

   return fromCell(lambdaCell);
}

// Lambda::genCurriedApFn: Generate curried function application

Void Lambda::genCurriedApFn(UInt nArgs,
                            NameTypedValBinding* curriedFormalParamV,
                            CodeLabel& codeLabel,
                            CodeLabel& mainLabel,
                            CodeLabel& failLab,
                            Code& code) {
   Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
   new(cell_.body()) Snippet(curried(nArgs, curriedFormalParamV, mainLabel, code));
   Expr snippet = fromCell(cell_);

   if (!(Tran(1, "", code.env()).tran(snippet) &&
      TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
      Tran(2,
#ifdef TRACE
         traceFlag ? "pass=2 printAST" :
#endif
         "", code.env()).tran(snippet))) {
      code.error("Failed to generate curried entry");
   }

   Lambda& lambda = toBody(toBody(snippet, Snippet).expr(), Lambda);

   importFormalParameters(codeLabel,
      lambda._formalParamV,
      lambda._nFormalParams,
      code);

   code.genReturn(lambda._body,
      resultTypeSig(lambda._typeSig, lambda._nFormalParams),
      *code._errLab);
}

CodeLabel* Lambda::genUpdateContinuation(Cell* updateCell,
                                     Code& code) {
   new(updateCell->body()) Var(newName(), _closure->typeSig(), Var::UPDATE_CELL_VAR);
   code._updateCellVar = &cellBody(*updateCell, Var);

   code.addBinaryInstruction(Instruction::MOVE_INSTR,
      code.varOperand(*_closure),
      code.varOperand(cellBody(*updateCell, Var)));

   CodeLabel* label = &code.newLabel(TRUE);

   code._updateContVar = &code.newVar(LABEL, Var::CONTINUATION_VAR);
   code._updateContVar->_contLabel = label;

#if (GARBAGE_COLLECTION==1)
   if (!code.gcFlag())
      code.addInstruction(new (code.msa()) FrameDescrInstruction(code.labelOperand(*label),
         code.genGC_stub()));
#endif

   code.addContinuationInstruction(code.labelOperand(*label),
      code.varOperand(*code._updateContVar), TRUE

#if (GARBAGE_COLLECTION==1)
      , code.gcFlag()
#endif      
   );
   return label;
}

// Lambda::genPartialApEntries: Generate partial function application entries
// i.e from 1 to (arity - 1) arguments

Void Lambda::genPartialApEntries(UInt arity,
                                 NameTypedValBinding* curriedFormalParamV,
                                 CodeLabel* mainLabel,
                                 CodeLabel& failLab,
                                 Code& code) {
   if (arity == 2) {
      code.genLabel(*_entryLabel);
      genPartialApFn(*_entryLabel, 1, failLab, code);
   }
   else {
      UInt nPartialApFns = arity - 1;
      LabelOperand** labels = (LabelOperand**)code.msa().alloc(nPartialApFns * sizeof(LabelOperand*));
      for (UInt i = 0; i < nPartialApFns; i++) {
          if (i < _nFormalParams - 1) {
            CodeLabel& codeLabel = code.newLabel(TRUE);
            code.genLabel(codeLabel);
            labels[i] = new(code.msa()) LabelOperand(codeLabel);
            genPartialApFn(codeLabel, i + 1, failLab, code);
         }
         else {
             if (i == _nFormalParams - 1)
                labels[i] = new(code.msa()) LabelOperand(*mainLabel);
             else {
                CodeLabel& codeLabel = code.newLabel(TRUE);
                code.genLabel(codeLabel);
                genCurriedApFn(i + 1, curriedFormalParamV, codeLabel , *mainLabel, failLab, code);
                labels[i] = new(code.msa()) LabelOperand(codeLabel);
             }
         }
      }
      code.genLabel(*_entryLabel);
      code.addInstruction(new(code.msa()) ArgCheckInstruction(nPartialApFns, labels, *_altEntryLabel));
   }
}

// Lambda::genUpdate: Generate closure update

Void Lambda::genUpdate(Cell* updateCell,
                       CodeLabel& updateLabel,
                       CodeLabel& failLab,
                       Code& code) {
   code.genLabel(updateLabel);

   Name name = newName();
   Cell& resultCell = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(resultCell.body()) Var(name, arg(_typeSig), Var::RESULT_VAR, code.lambda());
   updateLabel.basicBlk().vars().addElement(cellBody(resultCell, Var), code.msa())->setFlags(
      (VarElement::Flags)(VarElement::D | VarElement::I));

   Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
   new(cell_.body()) Snippet(mkPair(REDUCE,
      ap(mkPair(REDUCE,
         ap(fromName(useName("genUpdateThunk", code.nameTable())),
            fromCell(*updateCell), code.msa()), code.msa()),
         fromCell(resultCell), code.msa()), code.msa()));
   Expr snippet = fromCell(cell_);

   if (!(Tran(1, "", code.env()).tran(snippet) &&
      TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
      Tran(2,

#ifdef TRACE            
         traceFlag ? "pass=2 printAST" :
#endif

         "", code.env()).tran(snippet))) {
      code.error("Failed to generate closure update");
   }
   Analyser(

#ifdef TRACE            
      traceFlag ? "trace" :
#endif         

      "", code.env(), code.msa(), this).analyse(snippet);
   Var& thunk = code.newVar(_typeSig, Var::LOCAL_VAR);
   code.genVar(toBody(snippet, Snippet).expr(), thunk, failLab);

   code.addUpdateInstruction(code.varOperand(thunk),
      code.varOperand(cellBody(*updateCell, Var), TRUE));

   cellBody(resultCell, Var).genReturn(code);
}

// Lambda::genCellInfo: Generate cell information and methods

CellInfo_* Lambda::genCellInfo(Var& var, StructTemplate* structTemplate,
   CodeLabel& failLab, Code& code) {
   var._structTemplate = structTemplate;

   CellInfo_* cellInfo = &code.newCellInfo(_entryLabel != NULL
      ? *_entryLabel
      : *_altEntryLabel,
      *_altEntryLabel,
      NULL, NULL, NULL

#if (GARBAGE_COLLECTION==1)
      , NULL
#endif

   );
   var._cellInfo = cellInfo;

   if (_needsClosure && _closure != NULL) {
      _closure->_cellInfo = cellInfo;

      if (_nNonGlobalFree != 0) {
         cellInfo->_copyFnLabel = genMapMethod(*_closure, failLab, code);

#if (SERIALISATION==1)
         cellInfo->_extractBinFnLabel = genSerialiseMethod(*_closure,
            useName("is", code.nameTable()),
            useName("extractFreeVars", code.nameTable()),
            typeCon(InputStream), failLab, code);

         cellInfo->_insertBinFnLabel = genSerialiseMethod(*_closure,
            useName("os", code.nameTable()),
            useName("insertFreeVars", code.nameTable()),
            typeCon(OutputStream), failLab, code);
#endif

#if (GARBAGE_COLLECTION==1)
         cellInfo->_gcFnLabel = genMarkMethod_GC(*_closure, failLab, code);
#endif

      }
   }
   return cellInfo;
}

// Lambda::genClosure: Generate closure (allocate and assign free variables)

Void Lambda::genClosure(Var& var, UInt nSlots, CellInfo_* cellInfo,
                                   StructTemplate& structTemplate,
                                   CodeLabel& failLab, Code& code) {
   Var** slotVars = NULL;
   if (!_simple && _freeVarAssocs != NULL) {
      slotVars = (Var**)code.msa().alloc(nSlots * sizeof(Var*));
      genFreeVars(nSlots, slotVars, failLab, code);
   }

   code.addInstruction(new(code.msa()) CellInfoInstruction(*cellInfo, structTemplate));

#if (GARBAGE_COLLECTION==1)
   CodeLabel& gcLabel = code.newLabel(FALSE, NULL, FALSE);
   FrameDescrInstruction* frameDescrIns = new (code.msa())
      FrameDescrInstruction(code.labelOperand(gcLabel),
                            code.genGC_stub());
   code.addInstruction(frameDescrIns);
#endif

   code.addCellAllocInstruction(structTemplate,
                                code.cellInfoOperand(NULL, cellInfo),
                                code.varOperand(var),
                                NULL);

#if (GARBAGE_COLLECTION==1)
   code.genLabel(gcLabel);
#endif

   if (!_simple && _freeVarAssocs != NULL)
      copyFreeVars(nSlots, slotVars, var, code);
}

// Lambda::gen: Generate a lambda function

Void Lambda::gen(Var* var, CodeLabel& failLab, Code& code,
                 Bool withArgCheck/* = TRUE*/,
                 Bool simpleWithClosure/* = FALSE*/) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Lambda::gen: ";
      if (var != NULL) {
         printName(var->name(), outStream, code.nameTable());
         outStream << ' ';
      }
      outStream << '(' << _nFormalParams << ") ";
      printTypeSig(_typeSig, 0, FALSE, outStream, code.env());
      outStream << endl;
   }
#endif

   _parent = code._lambda;
   ReturnState* parentReturnState = code._returnState;

#if (CLOSURE_UPDATE==1)
   Var* updateCellVar = code._updateCellVar;
   Var* updateContVar = code._updateContVar;   
   code._updateCellVar = NULL;
   code._updateContVar = NULL;
#endif

   UInt arity = ::arity(_typeSig);

   if (var != NULL)
      var->_lambda = this;

   _entryLabel = !_noPartialAps && _needsClosure && withArgCheck &&
      (_nFormalParams > 1 || (_nFormalParams > 0 && arity > _nFormalParams))
      ? &code.newLabel(TRUE, this)
      : NULL;

   _altEntryLabel = &code.newLabel(TRUE, this);

//------------------------------------------------------------------------
// Conditionally allocate curried main label and psuedo formal parameters
//------------------------------------------------------------------------
 
   NameTypedValBinding* curriedFormalParamV = NULL;
   CodeLabel* mainLabel = NULL;
   if (simpleWithClosure || arity <= _nFormalParams)
      importFormalParameters(*_altEntryLabel, _formalParamV, _nFormalParams, code);
   else {
      mainLabel = &code.newLabel(TRUE, this);
      curriedFormalParamV = allocCurried(arity, mainLabel, code);
   }

#ifdef USE_INSTR_SEQ
   InstrSeq* instrSeq = code._instrSeq;
   Bool execSeqFlag = code._execSeqFlag;
   if (execSeqFlag)
      code._instrSeq = new(code.msa()) InstrSeq();
   code._execSeqFlag = FALSE;
#else
   CodeLabel* skipLab = code._execSeqFlag ? &code.newLabel(TRUE) : NULL;
   if (skipLab != NULL)
      code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(*skipLab));
#endif

   FreeVarAssoc* closureAssoc = NULL;

   UInt nSlots = _nNonGlobalFree;
   StructTemplate* structTemplate =
      _needsClosure
      ? &allocFreeVars(nSlots, code, failLab)
      : NULL;

//---------------------------------------------------
// Allocate closure structure and setup closure cell
//---------------------------------------------------

   if (!_simple && _needsClosure || _simple && simpleWithClosure)
      setupClosureCell(structTemplate, mainLabel, code);
 
   code._lambda = this;
   code._returnState = NULL;

   BasicBlk* savedBasicBlk = code._basicBlk;

   if (_entryLabel != NULL)
      genPartialApEntries(arity,
                          curriedFormalParamV,
                          mainLabel,
                          failLab,
                          code);

   code.genLabel(*_altEntryLabel);
   if (_isOrder) {
      _altEntryLabel->_saveEntryFlag = TRUE;
      _altEntryLabel->incRef();
   } 
   if (mainLabel != NULL) {
      genCurriedApFn(arity, curriedFormalParamV, *_altEntryLabel, *mainLabel, failLab, code);
      code.genLabel(*mainLabel);
   }

#if (CLOSURE_UPDATE==1)

//----------------------------------------
// Set up continuation for closure update
//----------------------------------------

   CodeLabel* updateLabel = NULL;
   Cell* updateCell = NULL;
   if (_updatable && _nFormalParams == 0 && arg(_typeSig) != typeCon(Void)) {
      updateCell = new(sizeof(Var), code.msa()) Cell(VAR);
      updateLabel = genUpdateContinuation(updateCell, code); 
   }
#endif

//-------------------------
// Generate body of lambda
//-------------------------

// Use error label for fail label for now
// T.B.D. Add dynamic binding to built-in error label in
//        case closure is copied.

   TypeSig resTypeSig = _nFormalParams > 0
      ? resultTypeSig(_typeSig, _nFormalParams)
      : arg(_typeSig);

   code.genReturn(_body, resTypeSig, *code._errLab);

#if (CLOSURE_UPDATE==1)

//--------------------------------------
// Generate update if closure updateble
//--------------------------------------

   code._updateCellVar = NULL;
   code._updateContVar = NULL;
   if (updateLabel != NULL)
      genUpdate(updateCell, *updateLabel, failLab, code);
#endif

//------------------
// Cell information
//------------------

   CellInfo_* cellInfo = var != NULL
      ? genCellInfo(*var, structTemplate, failLab, code)
      : NULL;
  
//------------------------
// Restore parent context
//------------------------

   code._lambda = _parent;
   code._returnState = parentReturnState;

#ifdef USE_INSTR_SEQ
   if (execSeqFlag) {
      if (instrSeq->_appendSeq == NULL)
         instrSeq->_appendSeq = code._instrSeq;
      else
         instrSeq->_appendSeq->append(code._instrSeq);
      code._instrSeq = instrSeq;
      code._execSeqFlag = execSeqFlag;
      code._basicBlk = savedBasicBlk;
   }
#else
   if (skipLab != NULL)
      code.genLabel(*skipLab);
#endif

//----------------------------------
// Build closure and free variables
//----------------------------------

   if (_needsClosure && var != NULL)
      genClosure(*var, nSlots, cellInfo,
                            *structTemplate,
                            failLab, code);

#if (CLOSURE_UPDATE==1)
   code._updateCellVar = updateCellVar;
   code._updateContVar = updateContVar;
#endif

}

// Lambda::genVarReduced: Generate a lambda variable

Void Lambda::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   gen(&dst, failLab, code);
}

// Lambda::genOperandReduced: Generate a lambda operand

Operand& Lambda::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {     
   Var& var = code.newVar(typeSig, Var::LOCAL_VAR);
   gen(&var, failLab, code);
   return code.varOperand(var);
}

// Lambda::genVoidReduced: Generate a void value

Void Lambda::genVoidReduced(CodeLabel& failLab, Code& code) {
   if (!_simple) {
      Var& var = code.newVar(typeCon(Void), Var::LOCAL_VAR);
      gen(&var, failLab, code);
      code.addUnaryInstruction(Instruction::GOTO_INSTR, code.varOperand(var));
   } else {
      gen(NULL, failLab, code);
      code.addUnaryInstruction(Instruction::GOTO_INSTR,
                               code.labelOperand(*_altEntryLabel));
   }
}

// Lambda::genEnter: Direct entry to alternative label

Void Lambda::genEnter(const FnAp* fnAp, Code& code) const {

   code.addInstruction(new(code.msa()) EmptyArgsInstruction(FALSE));
   if (fnAp != NULL)
      fnAp->copyArgs(code);

   code.addGoToInstruction(code.labelOperand(*_altEntryLabel),
                           TRUE,
                           code._returnState != NULL
                              ? code._returnState->_contVar
                              :

#if (CLOSURE_UPDATE== 1)
                                code._updateContVar, code._updateCellVar);
#else
                                NULL);
#endif

}

// Lambda::genEnterReduced

Void Lambda::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(_typeSig, Var::LOCAL_VAR);
   gen(&var, failLab, code);
   var.genEnter(fnAp, code);
}

// Lambda::genReturnReduced

Void Lambda::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(_typeSig, Var::LOCAL_VAR);
   gen(&var, failLab, code);
   var.genReturn(code);
}

// Lambda::reduce

Expr Lambda::reduce(CodeLabel& failLab, Code& code) {
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), _typeSig, Var::LOCAL_VAR, code._lambda);
   gen(&cellBody(cell_, Var), failLab, code);
   return  fromCell(cell_);
}

// 'Let' code generation

Void Let::gen(CodeLabel& failLab, Code& code) {
   if (_recursive)
      code.reserveRecDeclOrDefns(_declOrDefns);
	code.genDeclOrDefns(_declOrDefns, failLab);
}

Expr Let::reduce(CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   return code.valOf(_expr, failLab);
}

Void Let::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   code.genVar(_expr, dst, failLab);
}

Operand& Let::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   return code.genOperand(_expr, typeSig, failLab);
}

Void Let::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                         Bool divergent, CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   code.genCond(_expr, reduceN, labT, labF, divergent, failLab);
}

Void Let::genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   code.genSelect(_expr, typeSig, byPtr, index, dst, failLab);
}

Void Let::genVoidReduced(CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   code.genVoid(_expr, failLab);
}

Void Let::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   code.genEnter(_expr, typeSig, fnAp, failLab);
}

Void Let::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   gen(failLab, code);
   code.genReturn(_expr, typeSig, failLab);
}

// Var:: reduce: Reduce a variable of type (Exp a)
//               This is complicated by special unboxed constants
//               for a small range of types.

Expr Var::reduceCell(TypeSig typeSig, Code& code) {
   Expr returnState = code.returnState(typeSig, NULL);

   Var* cellVar = &code.newVar(CELL_PTR, Var::CLOSURE_VAR);

   // T.B.D. Consider replacing with plain move

   code.addBinaryInstruction(Instruction::TO_CELL_INSTR,
      code.varOperand(*this),
      code.varOperand(*cellVar));

   code.addInstruction(new(code.msa())
      EnterCellInstruction(code.varOperand(*cellVar),
                           TRUE,
                           code._returnState != NULL
                              ? code._returnState->_contVar
                              :

#if (CLOSURE_UPDATE== 1)
                                code._updateContVar, code._updateCellVar)        
#else
                                NULL)
#endif

                      );
   return returnState;
}

// If dropping through to a bytecode head label would be invalid,
// this slightly more expensive form (which adds jumps)
// should be used instead.
// T.B.D. Base on suitable macro definition

Expr Var::reduce(Code& code) {
   TypeSig typeSig = denotedTypeSig(_typeSig);
   if (!code.hasUnboxedRepr(typeSig)) {
      Expr returnState = reduceCell(typeSig, code);
      toBody(returnState, ReturnState).genContinuation(code);
      return returnState;
   } else {
      Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
      new(cell_.body()) Var(newName(), typeSig, Var::LOCAL_VAR, code._lambda);
      Var& var = cellBody(cell_, Var);
 
      code.reserve(var);
      CodeLabel& lab1 = code.newLabel();
      CodeLabel& lab2 = code.newLabel();
      code.addUnaryConditionalInstruction(Operator(Operator::SPECIAL_EXPR_OP,
                                                   Operator::OP_TYPE_POLY, 1),
                                          code.varOperand(*this), lab1, FALSE);

      Expr returnState = reduceCell(typeSig, code);
      toBody(returnState, ReturnState).genContinuation(code);
      code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab2));

      code.genLabel(lab1);
      code.addInstruction(new(code.msa())
         MonadicOpActionInstruction(Operator(Operator::ENTER_SPECIAL_EXPR_OP, Operator::OP_TYPE_EXPR, 1),
                                    code.varOperand(*this)));
      code.genLabel(lab2);
      Var& resVar = code.newVar(typeSig, Var::RESULT_VAR);
      code.basicBlk()._vars.addElement(resVar,
         code.msa())->setFlags((VarElement::Flags)(VarElement::D | VarElement::I));
      code.genMove(resVar, var);
      return (Expr)&cell_;
   }
}
/*

Expr Var::reduce(Code& code) {
   CodeLabel* lab = NULL;
   TypeSig typeSig = denotedTypeSig(_typeSig);
   Bool unboxed = code.hasUnboxedRepr(typeSig);
   if (unboxed) {
      lab = &code.newLabel();
      code.addUnaryConditionalInstruction(Operator(Operator::SPECIAL_EXPR_OP, Operator::OP_TYPE_POLY, 1),
         code.varOperand(*this), *lab);
   }
   Expr returnState = reduceCell(typeSig, code);
   if (unboxed) {
      code.genLabel(*lab);
      Var& resVar = code.newVar(typeSig, Var::RESULT_VAR);
      code.addInstruction(new(code.msa())
         EnterSpecialInstruction(code.varOperand(*this),
            code.varOperand(resVar)));
   }
   toBody(returnState, ReturnState).genContinuation(code);

   return returnState;
}
*/

Void Var::gen(CodeLabel& errLab, Code& code) {
   if (_reservedDefn != NULL) {
      if (_reservedDefn->recInitStrategy() == Defn::REC_INIT_INDIRECTION) {

#if (CLOSURE_UPDATE==1)
         Var& initVar = code.newVar(_typeSig, Var::LOCAL_VAR);
         code.genVar(_reservedDefn->expr(), initVar, errLab);
         code.addUpdateInstruction(code.varOperand(initVar),
            code.varOperand(*this, TRUE));
#else
         code.error("Recursive indirection initialisation requires closure update");
#endif

      } else
         code.genVar(_reservedDefn->expr(), *this, errLab);
   }
}

Void Var::genVar(Var& dst, Code& code) {
   code.addBinaryInstruction(Instruction::MOVE_INSTR,
                             code.varOperand(*this),
                             code.varOperand(dst));
}

Void Var::reduceVar(Var& dst, Code& code) {
   ReturnState& returnState = toBody(reduce(code), ReturnState);
   code.addBinaryInstruction(Instruction::MOVE_INSTR,
                             code.varOperand(toBody(returnState.var(_typeSig, code), Var)),
                             code.varOperand(dst));
}


Void Var::genExpr(Var& dst, Code& code) {
   if (!isExprTypeSig(_typeSig))
      code.addBinaryInstruction(Instruction::TAG_INSTR,
                                code.varOperand(*this), code.varOperand(dst));
   else
      code.addBinaryInstruction(Instruction::MOVE_INSTR,
                                code.varOperand(*this),
                                code.varOperand(dst));
}

Operand& Var::genOperand(TypeSig typeSig, Code& code) {
   return code.varOperand(*this);
}
/* ???
Void Var::genSelect(Bool byPtr, UInt index, Var& dst, Code& code) {
   Operand& srcOperand = code.varOperand(*this);
   VarOperand& dstOperand = code.varOperand(dst);
   code.addMoveInstruction(*(index > 0
      ? (Operand*)&code.selectOperand(index, &srcOperand, dst.typeSig(), byPtr)
      : (Operand*)&code.dePtrOperand(srcOperand)),
      dstOperand);
}
*/
Void Var::genSelect(Bool byPtr, UInt index, Var& dst, Code& code) {
   Operand& srcOperand = code.varOperand(*this);
   VarOperand& dstOperand = code.varOperand(dst);
   code.addMoveInstruction(*(index > 0
      ? (Operand*)&code.selectOperand(index, &srcOperand, dst.typeSig(), byPtr)
      : (Operand*)&code.dePtrOperand(srcOperand)),
      dstOperand);
}

Void Var::genCond(CodeLabel& labT, CodeLabel& labF,
                  Bool divergent, Code& code) {
   if (typeSig() != typeCon(Bool)) {
      code.error("T.B.D. handle different data types");
      printTypeSig(typeSig(), 0, FALSE, outStream, code.env());
      outStream << endl;
   }
   code.genConditionalInstruction(Operator("eqTAG"),
      code.varOperand(*this),
      code.constOperand(0),
      labF, labT, divergent);
}

Void Var::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                         Bool divergent, CodeLabel& failLab, Code& code) {
   Expr returnState = reduce(code);
   Var& var = toBody(toBody(returnState, ReturnState).var(typeCon(Bool), code), Var);
   code.genConditionalInstruction(Operator("eqTAG"),
      code.varOperand(var),
      code.constOperand(0),
      labF, labT, FALSE);
}

Void Var::genEnter(FnAp* fnAp, Code& code) {
   Bool fullyApplied = fnAp == NULL || fnAp->nArgs() == arity(_typeSig);

   Operand* varOperand = _kind == Var::GLOBAL_VAR ||
                         _kind == Var::EXT_GLOBAL_VAR
      ? &code.varOperand(*this)
      : NULL;

// Entry label if known 
// Note that the descendancy test is included to ensure that an entry label
// is contained within a child segment

   CodeLabel* entryLabel =
      _lambda != NULL &&
      !_lambda->needsClosure() &&
      (_lambda->closure() == NULL ||
         (code.lambda() != NULL &&
            _lambda == code.lambda() ||
            _lambda->hasAscendant(code.lambda())))
      ? (!fullyApplied
         ? _lambda->_entryLabel
         : _lambda->_altEntryLabel)
      : NULL;

   Bool entrySkip = varOperand != NULL || entryLabel != NULL;

   code.addInstruction(new(code.msa()) EmptyArgsInstruction(!fullyApplied && (fnAp == NULL ||
      !fnAp->knownPartial())));
   if (fnAp != NULL)
      fnAp->copyArgs(code);

   Var* cellVar = NULL;
   if (!fullyApplied || !(entrySkip && (_lambda == NULL ||
                                        !_lambda->needsClosure() ||
                                        _lambda->closure() == NULL))) {
      cellVar = &code.newVar(_typeSig, Var::CLOSURE_VAR);
      cellVar->_lambda = _lambda;
      code.addBinaryInstruction(Instruction::MOVE_INSTR,
         code.varOperand(*this),
         code.varOperand(*cellVar));
   }

   if (entrySkip) {

      if (cellVar != NULL)
         adjEntrySkipCount(1);

      code.addGoToInstruction(varOperand != NULL
         ? *varOperand
         : code.labelOperand(*entryLabel),
         fullyApplied,
         code._returnState != NULL
         ? code._returnState->_contVar
         :

#if (CLOSURE_UPDATE== 1)
         code._updateContVar, code._updateCellVar);
#else
         NULL);
#endif

   }
   else
      code.addInstruction(new(code.msa())
         EnterCellInstruction(code.varOperand(*cellVar),
                              fullyApplied,
                              code._returnState != NULL
                                 ? code._returnState->_contVar
                                 :

#if (CLOSURE_UPDATE== 1)
                                   code._updateContVar, code._updateCellVar)
#else
                                   NULL)
#endif
      
                         );
}

// Var::genEnterReduced:

Void Var::genEnterReduced(FnAp* fnAp, Code& code) {
   TypeSig typeSig = denotedTypeSig(_typeSig);
   Expr returnState = reduceCell(typeSig, code);
   toBody(returnState, ReturnState).genContinuation(code);
   toBody(toBody(returnState, ReturnState).var(typeSig, code), Var).genEnter(fnAp, code);
}

// Var::genReturn: Return from closure with value of variable

Void Var::genReturn(Code& code) {
   if (_typeSig != typeCon(Void))
      genVar(code.newVar(_typeSig, Var::RESULT_VAR), code);
   code.ret();
}

// Var::genReturnReduced: Return from closure with reduced value of variable

Void Var::genReturnReduced(Code& code) {
   TypeSig typeSig = denotedTypeSig(_typeSig);
   if (!code.hasUnboxedRepr(typeSig)) {
      Var& var = code.newVar(CELL_PTR, Var::LOCAL_VAR);
      code.addBinaryInstruction(Instruction::TO_CELL_INSTR,
         code.varOperand(*this),
         code.varOperand(var));
      var.genEnter(NULL, code);
   }
   else {
      CodeLabel& lab = code.newLabel();

      code.addUnaryConditionalInstruction(Operator(Operator::SPECIAL_EXPR_OP, Operator::OP_TYPE_EXPR, 1),
                                          code.varOperand(*this), lab, TRUE);

      Var& var = code.newVar(CELL_PTR, Var::LOCAL_VAR);
      code.addBinaryInstruction(Instruction::TO_CELL_INSTR,
         code.varOperand(*this),
         code.varOperand(var));

      var.genEnter(NULL, code);

      code.genLabel(lab);
      code.addInstruction(new(code.msa())
         MonadicOpActionInstruction(Operator(Operator::ENTER_SPECIAL_EXPR_OP, Operator::OP_TYPE_EXPR, 1), code.varOperand(*this)));
      code.ret();
   }
}



ReturnState::ReturnState(TypeSig typeSig)
 : _typeSig(typeSig), _contLabel(NULL), _contVar(NULL), _vars(Nil),
   _returnState(NULL) {
}

Void ReturnState::genContinuation(Code& code) {
   code._returnState = _returnState;
   code.genLabel(*_contLabel);
}

Void ReturnState::genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
}

Void ReturnState::genCond(CodeLabel& labT, CodeLabel& labF,
                          Bool divergent, Code& code) {
   code.genConditionalInstruction(Operator("eqTAG"),
      code.varOperand(toBody(var(TAG, code), Var)),
      code.constOperand(0),
      labF, labT, divergent);
}

Expr ReturnState::var(TypeSig typeSig, Code& code) {
   Expr vars = _vars;
   while (vars != Nil) {
      if (eqTypeSig(toBody(hd(vars), Var).typeSig(), typeSig))
         return hd(vars);
      vars = tl(vars);
   }
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), typeSig, Var::RESULT_VAR, code._lambda);
   _vars = cons(fromCell(cell_), _vars, code.msa());

   _contLabel->basicBlk()._vars.addElement(cellBody(cell_, Var),
      code.msa())->setFlags((VarElement::Flags)(VarElement::D | VarElement::I));

   return fromCell(cell_);
}

Void ReturnState::genVar(Var& dst, Code& code) {
   code.addBinaryInstruction(Instruction::MOVE_INSTR,
                             code.varOperand(toBody(var(dst.typeSig(), code), Var)),
                             code.varOperand(dst));
}

Void ReturnState::genExpr(Var& dst, Code& code) {
   code.addBinaryInstruction(Instruction::TAG_INSTR,
                             code.varOperand(toBody(var(dst.typeSig(), code), Var)),
                             code.varOperand(dst));
}

Expr ReturnState::reduce(Code& code) {
   return toBody(var(_typeSig, code), Var).reduce(code);
}

Operand& ReturnState::genOperand(TypeSig typeSig, Code& code) {
   return code.varOperand(toBody(var(typeSig, code), Var));
}

Void ReturnState::genEnter(TypeSig typeSig, FnAp* fnAp, Code& code) {
   toBody(var(typeSig, code), Var).genEnter(fnAp, code);
}

Void ReturnState::genEnterReduced(TypeSig typeSig, FnAp* fnAp, Code& code) {
   toBody(var(_typeSig, code), Var).genEnterReduced(fnAp, code);
}

// NameOcc::val: Return value or external global reference

Expr NameOcc::val(Code& code) const {
   if (code.lambda() == NULL ||
      !code.lambda()->needsClosure() ||
      freeVarAssoc() == NULL ||
      freeVarAssoc()->closedVar() == NULL) {
      Expr val = this->val();
      ExprEnumVal form = formOf(val);
      if (_moduleDefn == NULL ||
         _moduleDefn == code.moduleDefn() ||
         form == DECL) {
      return !(form == DECL && name() == closureName)
         ? val
         : fromCell(*code.lambda()->_closureCell);
   }
   else {
         if (form == CLASS_METHOD_DECL) {
            printName(name(), outStream, code.nameTable());
            code.error(": unresolved class method declaration");
            return ERROR;
         }
         else {
            assert(form == VAR, "Code::localOrExtVal: Expected variable");
            Var& var = toBody(val, Var);
            if (var.kind() == Var::EXT_GLOBAL_VAR)
               return val;
            Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
            new(cell_.body()) Var(mapName(var.name(),
               _moduleDefn->nameTable(),
               code.nameTable()),
               _typeSig,
               Var::EXT_GLOBAL_VAR, NULL);
            return fromCell(cell_);
         }
      }
   }
   else
      return freeVarAssoc()->closedVar();
}
// NameOcc::reduce: Reduce a name occurrence.

Expr NameOcc::reduce(CodeLabel& failLab, Code& code) {
	return code.reduce(val(code), failLab);
}

// NameOcc:genVal: code generation value

Void NameOcc::genExpr(Var& dst, CodeLabel& failLab, Code& code) {
   code.genExpr(val(code), dst, failLab);
}

Void NameOcc::genVar(Var& dst, CodeLabel& failLab, Code& code) {
   code.genVar(val(code), dst, failLab);
}

Void NameOcc::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   code.genVarReduced(val(code), dst, failLab);
}

Operand& NameOcc::genOperand(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   return code.genOperand(val(code), typeSig, failLab);
}

Operand& NameOcc::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   return code.genOperandReduced(val(code), typeSig, failLab);
}

Void NameOcc::genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   code.genEnter(val(code), typeSig, fnAp, failLab);
}

Void NameOcc::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   code.genEnterReduced(val(code), typeSig, fnAp, failLab);
}

Void NameOcc::genCond(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                      Bool divergent, CodeLabel& failLab, Code& code) {
   code.genCond(val(code), reduceN, labT, labF, divergent, failLab);
}

Void NameOcc::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                             Bool divergent, CodeLabel& failLab, Code& code) {
   code.genCondReduced(val(code), reduceN, labT, labF, divergent, failLab);
}

Void NameOcc::genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
	code.genSelect(val(code), typeSig, byPtr, index, dst, failLab); 
}

Void NameOcc::genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
   code.genSelectReduced(val(code), typeSig, byPtr, index, dst, failLab);
}

Void NameOcc::genVoidReduced(CodeLabel& failLab, Code& code) {
   code.genVoidReduced(val(code), failLab);
}

Void NameOcc::genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   code.genReturn(val(code), typeSig, failLab);
}

Void NameOcc::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   code.genReturnReduced(val(code), typeSig, failLab);
}

// Cond::Cond: C++ constructor for conditional expression

// The different forms include a test for a constant predicate
// to minimise the generation of unreachable code

Cond::Cond(TypeSig typeSig, Expr ePred, Expr eTrue, Expr eFalse)
   : _typeSig(typeSig), _ePred(ePred), _eTrue(eTrue), _eFalse(eFalse) {
}

Bool Cond::isConstReduced(Expr& val, const Code& code) const {
   return code.isConst(_ePred, val)
      ? (val == fromTag(0)
         ? code.isConst(_eFalse, val)
         : code.isConst(_eTrue, val))
      : FALSE;
}

Expr Cond::reduce(CodeLabel& failLab, Code& code) {
   if (resultTypeSig(_typeSig, 3) == typeCon(Void)) {
      genVoidReduced(failLab, code);
      return VOID;
   }

   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), resultTypeSig(_typeSig, 3), Var::LOCAL_VAR);
   Expr constVal;
   if (code.isConst(_ePred, constVal))
      code.genVar(constVal == fromTag(0) ? _eFalse : _eTrue, cellBody(cell_, Var), failLab);
   else {
      CodeLabel& labT = code.newLabel();
      CodeLabel& labF = code.newLabel();
      CodeLabel& lab = code.newLabel();
      code.genCond(_ePred, 0, labT, labF, FALSE, failLab);
      code.genLabel(labT);
      code.genVar(_eTrue, cellBody(cell_, Var), failLab);
      if (code._execSeqFlag)
         code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab));
      code.genLabel(labF);
      code.genVar(_eFalse, cellBody(cell_, Var), failLab);
      code.genLabel(lab);
   }
   return fromCell(cell_);
}

Void Cond::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                          Bool divergent, CodeLabel& failLab, Code& code) {
   Expr constVal;
   if (code.isConst(_ePred, constVal))
      code.genCond(constVal == fromTag(0) ? _eFalse : _eTrue,
                   reduceN, labT, labF, divergent, failLab);
   else {

//--- Skip after _eTrue not required

      CodeLabel& labT_ = code.newLabel();
      CodeLabel& labF_ = code.newLabel();
      code.genCond(_ePred, 0, labT_, labF_, FALSE, failLab);
      code.genLabel(labT_);
      code.genCond(_eTrue, reduceN, labT, labF, divergent, failLab);
      code.genLabel(labF_);
      code.genCond(_eFalse, reduceN, labT, labF, divergent, failLab);
   }
}

Void Cond::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Expr constVal;
   if (code.isConst(_ePred, constVal))
      code.genVar(constVal == fromTag(0) ? _eFalse : _eTrue, dst, failLab);
   else {
      if (!dst.reserved())
         code.reserve(dst);
      CodeLabel& labT = code.newLabel();
      CodeLabel& labF = code.newLabel();
      CodeLabel& lab = code.newLabel();
      code.genCond(_ePred, 0, labT, labF, FALSE, failLab);
      code.genLabel(labT);
      code.genVar(_eTrue, dst, failLab);
      if (code._execSeqFlag)
         code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab));
      code.genLabel(labF);
      code.genVar(_eFalse, dst, failLab);

      code.genLabel(lab);
   }
}

Operand& Cond::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(typeSig, Var::LOCAL_VAR);
   code.reserve(var);
   genVarReduced(var, failLab, code);
   return code.varOperand(var);
}

Void Cond::genVoidReduced(CodeLabel& failLab, Code& code) {
   Expr constVal;
   if (code.isConst(_ePred, constVal))
      code.genVoid(constVal == fromTag(0) ? _eFalse : _eTrue, failLab);
   else {
      CodeLabel& labT = code.newLabel();
      CodeLabel& labF = code.newLabel();
      CodeLabel& lab = code.newLabel();
      code.genCond(_ePred, 0, labT, labF, FALSE, failLab);
      code.genLabel(labT);
      code.genVoid(_eTrue, failLab);
      if (code._execSeqFlag)
         code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab));
      code.genLabel(labF);
      code.genVoid(_eFalse, failLab);
      code.genLabel(lab);
   }
}

Void Cond::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   Expr constVal;
   if (code.isConst(_ePred, constVal))
      code.genEnter(constVal == fromTag(0) ? _eFalse : _eTrue,
                    constVal == fromTag(0) ? argTypeSig(_typeSig, 3) : argTypeSig(_typeSig, 2),
                    fnAp, failLab);
   else {
      CodeLabel& labT = code.newLabel();
      CodeLabel& labF = code.newLabel();
      code.genCond(_ePred, 0, labT, labF, TRUE, failLab);
      code.genLabel(labT);
      code.genEnter(_eTrue, argTypeSig(_typeSig, 2), fnAp, failLab);
      code.genLabel(labF);
      code.genEnter(_eFalse, argTypeSig(_typeSig, 3), fnAp, failLab);
   }
}

Void Cond::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   Expr constVal;
   if (code.isConst(_ePred, constVal))
      code.genReturn(constVal == fromTag(0) ? _eFalse : _eTrue, typeSig, failLab);
   else {
      CodeLabel& labT = code.newLabel();
      CodeLabel& labF = code.newLabel();
      code.genCond(_ePred, 0, labT, labF, TRUE, failLab);
      code.genLabel(labT);
      code.genReturn(_eTrue, typeSig, failLab);
      code.genLabel(labF);
      code.genReturn(_eFalse, typeSig, failLab);
   }
}


CodeLabel* Case::genInit(ConstOperand**& consts, LabelOperand**& labels,
   CodeLabel& failLab, Code& code) {
   consts = (ConstOperand**)code.msa().alloc(_n * sizeof(ConstOperand*));
   labels = (LabelOperand**)code.msa().alloc(_n * sizeof(LabelOperand*));

   for (UInt i = 0; i < _n; i++) {
      consts[i] = &code.constOperand(toBody(code.makeConst(fst(_altV[i]), FALSE), Const));
      labels[i] = new(code.msa()) LabelOperand(code.newLabel());
   }

// If there is a default label, generate a unique GOTO instruction
// So that the stack may be adjusted before jumping to the common
// fail label.  The peephole optimiser may be able to short-circuit this.

   CodeLabel* otherwiseLabel = !_closed ? &code.newLabel()
                                        : NULL;

   CodeLabel& caseFailLab = otherwiseLabel == NULL
      ? failLab
      : *otherwiseLabel;

   Operand& operand = code.genOperand(_arg, _argTypeSig, caseFailLab);
   code.addInstruction(new(code.msa())
      CaseInstruction(operand, _n, consts, labels,
                      otherwiseLabel == NULL
                        ? NULL
                        : new(code.msa()) LabelOperand(*otherwiseLabel)));
   return otherwiseLabel;
}

Void Case::genFinal(CodeLabel* otherwiseLabel, CodeLabel& failLab, Code& code) {
   if (otherwiseLabel != NULL) {
      code.genLabel(*otherwiseLabel);
      code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(failLab));
   }
}

Expr Case::reduce(CodeLabel& failLab, Code& code) {
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), denotedTypeSig(_typeSig), Var::LOCAL_VAR);
   genVarReduced(cellBody(cell_, Var), failLab, code);
   return fromCell(cell_);
}

Void Case::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   ConstOperand** consts;
   LabelOperand** labels;
   CodeLabel* otherwiseLabel = genInit(consts, labels, failLab, code);
   CodeLabel& lab = code.newLabel();
   for (UInt i = 0; i < _n; i++) {
      code.genLabel(labels[i]->_codeLabel);
      code.genVar(snd(_altV[i]), dst,
                  otherwiseLabel == NULL ? failLab : *otherwiseLabel);
      code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab));
   }
   genFinal(otherwiseLabel, failLab, code);
   code.genLabel(lab);
}

Operand& Case::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(typeSig, Var::LOCAL_VAR);
   code.reserve(var);
   genVarReduced(var, failLab, code);
   return code.varOperand(var);
}

Void Case::genVoidReduced(CodeLabel& failLab, Code& code) {
   ConstOperand** consts;
   LabelOperand** labels;
   CodeLabel* otherwiseLabel = genInit(consts, labels, failLab, code);
   CodeLabel& lab = code.newLabel();
   for (UInt i = 0; i < _n; i++) {
      code.genLabel(labels[i]->_codeLabel);
      code.genVoid(snd(_altV[i]),
                   otherwiseLabel == NULL ? failLab : *otherwiseLabel);
      code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab));
    }
   genFinal(otherwiseLabel, failLab, code);
   code.genLabel(lab);
}


Void Case::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
   Bool divergent, CodeLabel& failLab, Code& code) {
   ConstOperand** consts;
   LabelOperand** labels;
   CodeLabel* otherwiseLabel = genInit(consts, labels, failLab, code);
   for (UInt i = 0; i < _n; i++) {
      code.genLabel(labels[i]->_codeLabel);
      code.genCond(snd(_altV[i]), reduceN, labT, labF, divergent,
                   otherwiseLabel == NULL ? failLab : *otherwiseLabel);
   }
   genFinal(otherwiseLabel, failLab, code);
}

Void Case::genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
   ConstOperand** consts;
   LabelOperand** labels;
   CodeLabel* otherwiseLabel = genInit(consts, labels, failLab, code);
   for (UInt i = 0; i < _n; i++) {
      code.genLabel(labels[i]->_codeLabel);
      code.genSelect(snd(_altV[i]), typeSig, byPtr, index, dst,
                     otherwiseLabel == NULL ? failLab : *otherwiseLabel);
   }
   genFinal(otherwiseLabel, failLab, code);
}

Void Case::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   ConstOperand** consts;
   LabelOperand** labels;
   CodeLabel* otherwiseLabel = genInit(consts, labels, failLab, code);
   for (UInt i = 0; i < _n; i++) {
      code.genLabel(labels[i]->_codeLabel);
      code.genEnter(snd(_altV[i]), typeSig, fnAp,
                    otherwiseLabel == NULL ? failLab : *otherwiseLabel);
   }
   genFinal(otherwiseLabel, failLab, code);
}

Void Case::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   ConstOperand** consts;
   LabelOperand** labels;
   CodeLabel* otherwiseLabel = genInit(consts, labels, failLab, code);
   for (UInt i = 0; i < _n; i++) {
      code.genLabel(labels[i]->_codeLabel);
      code.genReturn(snd(_altV[i]), typeSig,
                     otherwiseLabel == NULL ? failLab : *otherwiseLabel);
    }
   genFinal(otherwiseLabel, failLab, code);
}


Expr CodeNode::reduce(CodeLabel& failLab, Code& code) {
   notOverloaded("reduce", code.env());
   return ERROR;
}

Bool CodeNode::isConstReduced(Expr& val, const Code& code) const {
   return FALSE;
}

Bool CodeNode::genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code) {
   notOverloaded("genExpr", code.env());
   return FALSE;
}

Operand& CodeNode::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(typeSig, Var::LOCAL_VAR);
   genVarReduced(var, failLab, code);
   return code.varOperand(var);
}

Void CodeNode::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   notOverloaded("genVarReduced", code.env());
}

Void CodeNode::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                              Bool divergent, CodeLabel& failLab, Code& code) {
   notOverloaded("genCondReduced", code.env());
}

Void CodeNode::genVoidReduced(CodeLabel& failLab, Code& code) {
   notOverloaded("genVoidReduced", code.env());
}

Void CodeNode::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(typeSig, Var::LOCAL_VAR);
   genVarReduced(var, failLab, code);
   var.genEnter(fnAp, code);
}

Void CodeNode::genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   notOverloaded("genReturn", code.env());
}

Void CodeNode::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   if (typeSig != typeCon(Void))
      genVarReduced(code.newVar(typeSig, Var::RESULT_VAR), failLab, code);
   else
      genVoidReduced(failLab, code);
   code.ret();
}

Void CodeNode::notOverloaded(ConstString methodStr, const Env& env) const {
   outStream << endl;
   static Char buf[256];
   sprintf(buf, "CodeForm::%s: not overloaded", methodStr);
   assert(FALSE, buf);
}

// AllocClosure: allocates a copy of a the current closure in a given environment              

AllocClosure::AllocClosure(Expr env)
   : _env(env) {
}

Void AllocClosure::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {

   const Var& closure = *code.lambda()->parent()->closure();

   Var& srcClosure = *code.lambda()->closure();
   Var& cellInfoVar = code.newVar(CELL_INFO, Var::LOCAL_VAR);
   code.addInstruction(new(code.msa()) MonadicOpInstruction(
      Operator(Operator::CELL_INFO_OF_OP, Operator::OP_TYPE_CELL_INFO, 1),
      code.varOperand(srcClosure), code.varOperand(cellInfoVar)));

#if (GARBAGE_COLLECTION==1)
   CodeLabel& gcLabel = code.newLabel(FALSE, NULL, FALSE);
   FrameDescrInstruction* frameDescrIns = new (code.msa()) FrameDescrInstruction(code.labelOperand(gcLabel),
      code.genGC_stub());
   code.addInstruction(frameDescrIns);
#endif

   code.addCellAllocInstruction(*closure.structTemplate(),
                                code.varOperand(cellInfoVar),
                                code.varOperand(dst),
                                &code.genOperand(_env, typeCon(Env), failLab));

#if (GARBAGE_COLLECTION==1) 
   code.genLabel(gcLabel);
#endif

}

// MapClosure:  Returns a coy Maps the free variables of one closure
//               into a copy of the closure

MapClosure::MapClosure(Expr env)
   : _env(env) {
}

Void MapClosure::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Lambda& lambda = *code.lambda()->parent();
   Var& srcClosure = *code.lambda()->closure();
   const Var& closure = *lambda.closure();

   StructTemplate* structTemplate = closure.structTemplate();
   srcClosure._lambda = &lambda;
   srcClosure._structTemplate = structTemplate;
   dst._lambda = &lambda;
   dst._structTemplate = structTemplate;

   lambda._closure->_structTemplate->adjUseCount(2);

   UInt nSlots = lambda._nNonGlobalFree;
   Var** slotVars = NULL;

   if (nSlots != 0) {
      slotVars = (Var**)code.msa().alloc(nSlots * sizeof(Var*));
      for (UInt i = 0; i < nSlots; ++i)
         slotVars[i] = NULL;
   }

//-- Generate mapped free variables in advance

   for (FreeVarAssoc* freeVarAssoc = lambda._freeVarAssocs;
      freeVarAssoc != NULL;
      freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->closedVar() == NULL)
         continue;

      Var& localVar = cellBody(*freeVarAssoc->closedVar(), Var);
      if (localVar.kind() == Var::NON_GLOBAL_FREE_VAR &&
         !freeVarAssoc->selfReferential()) {

//--------- Unique source free variable selected from source closure

         Cell& varCell = *new(sizeof(Var), code.msa()) Cell(VAR);
         new(varCell.body()) Var(localVar.name(),
            localVar.typeSig(),
            Var::NON_GLOBAL_FREE_VAR);

         Var& srcSlotVar = cellBody(varCell, Var);
         srcSlotVar._closure = &srcClosure;
         srcSlotVar._lambda = &lambda;
         srcSlotVar._index = localVar._index;

         code.basicBlk().vars().addElement(
            srcSlotVar,
            code.msa())->setFlags(
               (VarElement::Flags)(VarElement::D | VarElement::I));

//------ mapToEnv <srcSlotVar> (envOf dst)

         Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
         new(cell_.body()) Snippet(
            mkPair(REDUCE,
                   ap(mkPair(REDUCE,
                             ap(fromName(useName("mapToEnv", code.nameTable())),
                                fromCell(varCell),
                                code.msa()),
                             code.msa()),                
                      _env,
                      code.msa()),
                   code.msa()));

         Expr snippet = fromCell(cell_);

         if (!(Tran(1, "", code.env()).tran(snippet) &&
            TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
            Tran(2,

#ifdef TRACE
               traceFlag ? "pass=2 printAST" :
#endif

               "", code.env()).tran(snippet))) {
            code.error("Failed to generate code to copy free variable");
         }

         Analyser("", code.env(), code.msa()).analyse(snippet);

         slotVars[localVar._index - 1] =
            &code.newVar(localVar.typeSig(), Var::LOCAL_VAR);

         code.genVar(toBody(snippet, Snippet).expr(),
            *slotVars[localVar._index - 1],
            failLab);
      }
   }

   //------ Allocate destination closure after all mapped values are safe

   Var& cellInfoVar = code.newVar(CELL_INFO, Var::LOCAL_VAR);
   code.addInstruction(new(code.msa()) MonadicOpInstruction(
      Operator(Operator::CELL_INFO_OF_OP, Operator::OP_TYPE_CELL_INFO, 1),
      code.varOperand(srcClosure), code.varOperand(cellInfoVar)));

#if (GARBAGE_COLLECTION==1)
   CodeLabel& gcLabel = code.newLabel(FALSE, NULL, FALSE);
   FrameDescrInstruction* frameDescrIns =
      new (code.msa()) FrameDescrInstruction(code.labelOperand(gcLabel),
         code.genGC_stub());
   code.addInstruction(frameDescrIns);
#endif

   code.addCellAllocInstruction(*closure.structTemplate(),
      code.varOperand(cellInfoVar),
      code.varOperand(dst),
      &code.genOperand(_env, typeCon(Env), failLab));

#if (GARBAGE_COLLECTION==1)
   code.genLabel(gcLabel);
#endif

   //------ Copy pre-generated free variables into destination closure

   lambda.copyFreeVars(nSlots, slotVars, dst, code);
}

// ExtractFreeVars: Extract the free variables of a closure

ExtractFreeVars::ExtractFreeVars(Expr is, Expr closure)
   : _is(is), _closure(closure) {
}

Void ExtractFreeVars::genVoidReduced(CodeLabel& failLab, Code& code) {
   Lambda& lambda = *code.lambda()->parent();
   code.lambda()->closure()->_structTemplate = lambda.closure()->structTemplate();

   for (FreeVarAssoc* freeVarAssoc = lambda._freeVarAssocs;
      freeVarAssoc != NULL;
      freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->closedVar() == NULL)
         continue;
      Var& localVar = cellBody(*freeVarAssoc->closedVar(), Var);
      if (localVar.kind() == Var::NON_GLOBAL_FREE_VAR &&
         !freeVarAssoc->selfReferential()) {

//------ Extract free variable

         Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
         new(cell_.body()) Snippet(
            mkTriple(CONSTRAIN,
                     mkPair(REDUCE,
                            ap(fromName(useName("extract", code.nameTable())),
                               code.lambda()->formalParamV()[0].typedVal().val(), code.msa()),
                            code.msa()),
                     localVar.typeSig(), code.msa()));

         Expr snippet = fromCell(cell_);
         if (!(Tran(1, "", code.env()).tran(snippet) &&
            TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
            Tran(2,

#ifdef TRACE                              
               traceFlag ? "pass=2 printAST" :
#endif

               "", code.env()).tran(snippet))) {
            code.error("Failed to generate code to extract free variable");
         }

         Analyser("", code.env(), code.msa(), code.lambda()).analyse(snippet);

//------ Unique copy of free variable in closure

         Cell& varCell = *new(sizeof(Var), code.msa()) Cell(VAR);
         new(varCell.body()) Var(localVar.name(),
            localVar.typeSig(),
            Var::NON_GLOBAL_FREE_VAR);
         cellBody(varCell, Var)._closure = code.lambda()->closure();
         cellBody(varCell, Var)._index = localVar._index;

         code.basicBlk().vars().addElement(
            cellBody(varCell, Var),
            code.msa())->setFlags((VarElement::Flags)(VarElement::D |
               VarElement::I));

         code.genVar(toBody(snippet, Snippet).expr(), cellBody(varCell, Var), failLab);
      }
   }
}

// InsertFreeVars:  Insert the free variables of a closure

InsertFreeVars::InsertFreeVars(Expr os, Expr closure)
 : _os(os), _closure(closure) {
}

Void InsertFreeVars::genVoidReduced(CodeLabel& failLab, Code& code) {
   Lambda& lambda = *code.lambda()->parent();
   code.lambda()->closure()->_structTemplate = lambda.closure()->structTemplate();

   for (FreeVarAssoc* freeVarAssoc = lambda.freeVarAssocs();
      freeVarAssoc != NULL;
      freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->closedVar() == NULL)
         continue;
      Var& localVar = cellBody(*freeVarAssoc->closedVar(), Var);
      if (localVar.kind() == Var::NON_GLOBAL_FREE_VAR &&
         !freeVarAssoc->selfReferential()) {

//------ Unique copy of free variable in closure

         Cell& varCell = *new(sizeof(Var), code.msa()) Cell(VAR);
         new(varCell.body()) Var(localVar.name(),
            localVar.typeSig(),
            Var::NON_GLOBAL_FREE_VAR);
         cellBody(varCell, Var)._closure = code.lambda()->closure();
         cellBody(varCell, Var)._index = localVar._index;

         code.basicBlk().vars().addElement(
            cellBody(varCell, Var),
            code.msa())->setFlags((VarElement::Flags)(VarElement::D |
               VarElement::I));

//------ Insert free variable

         Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
         new(cell_.body()) Snippet(
            mkPair(REDUCE,
               ap(mkPair(REDUCE, ap(fromName(useName("insert", code.nameTable())),
                  code.lambda()->formalParamV()[0].typedVal().val(), code.msa()), code.msa()),
                  fromCell(varCell),
                  code.msa()),
               code.msa()));

         Expr snippet = fromCell(cell_);
         if (!(Tran(1, "", code.env()).tran(snippet) &&
            TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
            Tran(2,

#ifdef TRACE                              
               traceFlag ? "pass=2 printAST" :
#endif

               "", code.env()).tran(snippet))) {
            code.error("Failed to generate code to insert free variable");
         }
         Analyser("", code.env(), code.msa(), code.lambda()).analyse(snippet);
         code.genVoid(toBody(snippet, Snippet).expr(), failLab);
      }
   }
}

#if (GARBAGE_COLLECTION==1)

// MarkFreeVars_GC:  Marks the free variables of a closure

MarkFreeVars_GC::MarkFreeVars_GC(Expr closure)
   : _closure(closure) {
}

Void MarkFreeVars_GC::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   Bool gcFlag = code.gcFlag();
   code.setGCFlag(TRUE);

   Lambda& lambda = *code.lambda();
   lambda.closure()->_structTemplate = lambda.parent()->closure()->_structTemplate;

   Expr seqExpr = VOID;
   for (FreeVarAssoc* freeVarAssoc = lambda.parent()->_freeVarAssocs;
      freeVarAssoc != NULL;
      freeVarAssoc = freeVarAssoc->next()) {
      if (freeVarAssoc->closedVar() == NULL)
         continue;
      Var& localVar = cellBody(*freeVarAssoc->closedVar(), Var);
      if (localVar.kind() == Var::NON_GLOBAL_FREE_VAR &&
          !freeVarAssoc->selfReferential()) {

//------ Unique copy of free variable in closure

         Cell& varCell = *new(sizeof(Var), code.msa()) Cell(VAR);
         new(varCell.body()) Var(localVar.name(),
            localVar.typeSig(),
            Var::NON_GLOBAL_FREE_VAR);
         cellBody(varCell, Var)._closure = lambda.closure();
         cellBody(varCell, Var)._index = localVar._index;

         code.basicBlk().vars().addElement(
            cellBody(varCell, Var),
            code.msa())->setFlags((VarElement::Flags)(VarElement::D |
               VarElement::I));

//------ Mark free variable

         Expr markExpr = mkPair(REDUCE,
            ap(fromName(useName("mark_GC", code.nameTable())),
               fromCell(varCell),
               code.msa()),
            code.msa());
         seqExpr = seqExpr != (Expr)VOID
            ? (Expr)mkPair(REDUCE,
                           ap(mkPair(REDUCE,
                                     ap(SEQ,
                                        seqExpr, code.msa()),
                                     code.msa()),
                              markExpr,
                              code.msa()),
                           code.msa())
            : markExpr;
      }
   }

   Cell& cell_ = *new(sizeof(Snippet), code.msa()) Cell(SNIPPET);
   new(cell_.body()) Snippet(seqExpr);
   Expr snippet = fromCell(cell_);
   if (!(Tran(1, "", code.env()).tran(snippet) &&
      TypeCheck("", code.env(), code.moduleDefn()).typeCheck(snippet) &&
      Tran(2,

#ifdef TRACE                              
         traceFlag ? "pass=2 printAST" :
#endif

         "", code.env()).tran(snippet))) {
      code.error("Failed to generate code to mark free variable");
   }
   Analyser("", code.env(), code.msa()).analyse(snippet);
   code.genReturn(toBody(snippet, Snippet).expr(), typeSig, failLab);
   code.setGCFlag(gcFlag);
}
#endif

MonadicOp::MonadicOp(Operator op, TypeSig typeSig, Expr x)
   : _op(op), _typeSig(typeSig), _x(x) {
}

Expr MonadicOp::reduce(CodeLabel& failLab, Code& code) {
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), resultTypeSig(_typeSig, 1), Var::LOCAL_VAR);
   genVarReduced(cellBody(cell_, Var), failLab, code);
   return fromCell(cell_);
}

Void MonadicOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   if (_op.kind() != Operator::ALLOC_PTR_ARRAY_OP)
      code.addInstruction(new(code.msa()) MonadicOpInstruction(_op, x, code.varOperand(dst)));
   else {

#if (GARBAGE_COLLECTION==1)
      CodeLabel& gcLabel = code.newLabel(FALSE, NULL, FALSE);
      FrameDescrInstruction* frameDescrIns = new (code.msa()) FrameDescrInstruction(code.labelOperand(gcLabel),
         code.genGC_stub());
      code.addInstruction(frameDescrIns);
#endif

      code.addInstruction(new(code.msa()) PtrArrayAllocInstruction(x, code.varOperand(dst)));

#if (GARBAGE_COLLECTION==1)
      code.genLabel(gcLabel);
#endif

   } 
}

Void MonadicOp::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   code.genConditionalInstruction(_op, x, labT, labF, divergent);
}

Void MonadicOp::genVoidReduced(CodeLabel& failLab, Code& code) {

#if (GARBAGE_COLLECTION==1)
   if (_op.kind() != Operator::MARK_CELL_GC_OP &&
       _op.kind() != Operator::MARK_EXPR_GC_OP) {
#endif

      TypeSig typeSig = argTypeSig(_typeSig, 1);
      Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
      code.addInstruction(new(code.msa()) MonadicOpActionInstruction(_op, x));

#if (GARBAGE_COLLECTION==1)
   } else {
      Expr returnState = code.returnState(typeCon(Void), NULL);
      if (_op.kind() == Operator::MARK_CELL_GC_OP) {
         Var& cellVar = code.newVar(_typeSig, Var::CLOSURE_VAR);
         code.addBinaryInstruction(Instruction::MOVE_INSTR,
            code.genArgOperand(_x, _typeSig, 1, failLab),
            code.varOperand(cellVar));
         code.addInstruction(new(code.msa()) MarkCellInstruction(code.varOperand(cellVar), toBody(returnState, ReturnState)._contVar));
      } 
      else {
         Var& cellVar = code.newVar(_typeSig, Var::CLOSURE_VAR);
         code.addBinaryInstruction(_op.kind() == Operator::MARK_CELL_GC_OP ? Instruction::MOVE_INSTR
                                                                           : Instruction::TO_CELL_INSTR,
                                   code.genArgOperand(_x, _typeSig, 1, failLab),
                                   code.varOperand(cellVar));
         code.addInstruction(new(code.msa()) MarkCellInstruction(code.varOperand(cellVar), toBody(returnState, ReturnState)._contVar));
      }
       toBody(returnState, ReturnState).genContinuation(code);
   }
#endif

}

Void MonadicOp::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {

#if (GARBAGE_COLLECTION ==1)
   if (_op.kind() != Operator::MARK_CELL_GC_OP &&
       _op.kind() != Operator::MARK_EXPR_GC_OP)
#endif

      CodeNode::genReturnReduced(typeSig, failLab, code);

#if (GARBAGE_COLLECTION ==1)
   else {      
      Var& cellVar = code.newVar(_typeSig, Var::CLOSURE_VAR);
      code.addBinaryInstruction(_op.kind() == Operator::MARK_CELL_GC_OP ? Instruction::MOVE_INSTR
                                                                        : Instruction::TO_CELL_INSTR,
                                code.genArgOperand(_x, _typeSig, 1, failLab),
                                code.varOperand(cellVar));
      code.addInstruction(new(code.msa()) MarkCellInstruction(code.varOperand(cellVar), NULL));      
   }
#endif

}

// TypeOfOp: Monadic operator to return the type of its argument

TypeOfOp::TypeOfOp(Operator op, TypeSig typeSig, Expr x)
   : MonadicOp(op, typeSig, x) {
}

Void TypeOfOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   TypeSig typeSig = argTypeSig(_typeSig, 1);
   code.genMoveConst(toBody(code.makeConst(fromType(code.useType(typeSig, TRUE))), Const), dst);
}

// IdOp: Monadic operator for polymorphic identity

IdOp::IdOp(Operator op, TypeSig typeSig, Expr x)
   : MonadicOp(op, typeSig, x) {
}

Void IdOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   code.addMoveInstruction(x, code.varOperand(dst));
}

// EnvOfOp: Monadic operator to return the environment of its argument

EnvOfOp::EnvOfOp(Operator op, TypeSig typeSig, Expr x)
   : MonadicOp(op, typeSig, x) {
}

Void EnvOfOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   code.addBinaryInstruction(Instruction::ENV_OF_INSTR, x, code.varOperand(dst));
}

// TagOp: Monadic operator to return the tag of its argument

TagOfOp::TagOfOp(TypeSig typeSig, Expr x)
   : MonadicOp(Operator(Operator::TAG_OF_OP, Operator::OP_TYPE_TAG, 1), typeSig, x) {
}

Void TagOfOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   code.addBinaryInstruction(Instruction::TAG_OF_INSTR, x, code.varOperand(dst));
}

ConstructorStringOp::ConstructorStringOp(TypeSig typeSig, Expr x)
   : MonadicOp(Operator(Operator::CONSTRUCTOR_STRING_OP,
                        Operator::OP_TYPE_STRING, 1), typeSig, x) {
}

Void ConstructorStringOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   TypeSig srcTypeSig = argTypeSig(_typeSig, 1);

   if (hasTypeVar(srcTypeSig)) {
      code.error("constructorString: argument type is not statically known");
      return;
   }

   TypeSig hfTypeSig = headForm(srcTypeSig);
   if (formOf(hfTypeSig) != TYPE_CON) {
      code.error("constructorString: expected a type constructor argument");
      return;
   }

   TypeCon& typeCon = toBody(hfTypeSig, TypeCon);
   if (typeCon._dataCons == NULL) {
      code.error("constructorString: type has no data constructors");
      return;
   }

   if (typeCon._dataCons->next() == NULL) {
      Const& c = toBody(code.makeConst(
         fromString(nameString_(typeCon._dataCons->name(),
                               typeCon.nameTable()), code.msa()),
         FALSE), Const);
      code.genMoveConst(c, dst);
      return;
   }

   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   Var& tagVar = code.newVar(TAG, Var::LOCAL_VAR);
   code.addBinaryInstruction(Instruction::TAG_OF_INSTR, x, code.varOperand(tagVar));

   UInt n = typeCon._n;
   ConstOperand** consts =
      (ConstOperand**)code.msa().alloc(n * sizeof(ConstOperand*));
   LabelOperand** labels =
      (LabelOperand**)code.msa().alloc(n * sizeof(LabelOperand*));

   UInt i = 0;
   for (DataCon* dataCon = typeCon._dataCons;
        dataCon != NULL;
        dataCon = dataCon->next(), i++) {
      consts[i] = &code.constOperand(dataCon->tag());
      labels[i] = new(code.msa()) LabelOperand(code.newLabel());
   }

   CodeLabel& lab = code.newLabel();
   code.addInstruction(new(code.msa())
      CaseInstruction(code.varOperand(tagVar), n, consts, labels, NULL));

   i = 0;
   for (DataCon* dataCon = typeCon._dataCons;
        dataCon != NULL;
        dataCon = dataCon->next(), i++) {
      code.genLabel(labels[i]->codeLabel());
      Const& c = toBody(code.makeConst(
         fromString(nameString_(dataCon->name(), typeCon.nameTable()),
                    code.msa()),
         FALSE), Const);
      code.genMoveConst(c, dst);
      code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab));
   }
   code.genLabel(lab);
}

ConstructFromNameOp::ConstructFromNameOp(TypeSig typeSig, Expr x)
   : MonadicOp(Operator(Operator::CONSTRUCT_FROM_NAME_OP,
                        Operator::OP_TYPE_POLY, 1), typeSig, x) {
}

Void ConstructFromNameOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   TypeSig resTypeSig = dst.typeSig();

   if (hasTypeVar(resTypeSig)) {
      code.error("constructFromName: result type is not statically known");
      return;
   }

   ReprInfo reprInfo = typeReprInfo(resTypeSig, code.typeTable(),
                                    code.nameTable());
   if (reprInfo._repr != Repr::REPR_TAG) {
      code.error("constructFromName: result type is not an atomic sum type");
      return;
   }

   TypeSig hfTypeSig = headForm(resTypeSig);
   if (formOf(hfTypeSig) != TYPE_CON) {
      code.error("constructFromName: expected a type constructor result");
      return;
   }

   TypeCon& typeCon = toBody(hfTypeSig, TypeCon);
   for (DataCon* dataCon = typeCon._dataCons;
        dataCon != NULL;
        dataCon = dataCon->next()) {
      if (dataCon->arity() != 0) {
         code.error("constructFromName: constructor is not atomic");
         return;
      }
   }

   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   code.addInstruction(new(code.msa())
      ConstructFromNameInstruction(x, resTypeSig, code.varOperand(dst)));
}

ConstructFromStringOp::ConstructFromStringOp(TypeSig typeSig, Expr x)
   : MonadicOp(Operator(Operator::CONSTRUCT_FROM_STRING_OP,
                        Operator::OP_TYPE_POLY, 1), typeSig, x) {
}

Void ConstructFromStringOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   TypeSig resTypeSig = dst.typeSig();

   if (hasTypeVar(resTypeSig)) {
      code.error("constructFromString: result type is not statically known");
      return;
   }

   ReprInfo reprInfo = typeReprInfo(resTypeSig, code.typeTable(),
                                    code.nameTable());
   if (reprInfo._repr != Repr::REPR_TAG) {
      code.error("constructFromString: result type is not an atomic sum type");
      return;
   }

   TypeSig hfTypeSig = headForm(resTypeSig);
   if (formOf(hfTypeSig) != TYPE_CON) {
      code.error("constructFromString: expected a type constructor result");
      return;
   }

   TypeCon& typeCon = toBody(hfTypeSig, TypeCon);
   for (DataCon* dataCon = typeCon._dataCons;
        dataCon != NULL;
        dataCon = dataCon->next()) {
      if (dataCon->arity() != 0) {
         code.error("constructFromString: constructor is not atomic");
         return;
      }
   }

   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   code.addInstruction(new(code.msa())
      ConstructFromStringInstruction(x, resTypeSig, code.varOperand(dst)));
}

DyadicOp::DyadicOp(Operator op, TypeSig typeSig, Expr x, Expr y)
   : _op(op), _typeSig(typeSig), _x(x), _y(y) {}

Void DyadicOp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   Operand& y = code.genArgOperand(_y, _typeSig, 2, failLab);
   VarOperand& dstOperand = code.varOperand(dst);
   Bool mapNeedsGCFrame = _op.mapNeedsGCFrame();

#if (GARBAGE_COLLECTION==1)
   CodeLabel* gcLabel = NULL;
   if (mapNeedsGCFrame) {
      gcLabel = &code.newLabel(FALSE, NULL, FALSE);
      code.addInstruction(new(code.msa())
         FrameDescrInstruction(code.labelOperand(*gcLabel),
                               code.genGC_stub()));
   }
#endif

   if (_op.kind() == Operator::GET_AT_PTR_ARRAY_OP ||
       _op.kind() == Operator::GET_AT_PLAIN_PTR_ARRAY_OP)
      code.addMoveInstruction(code.arrayIndexOperand(code.dePtrOperand(x), y), dstOperand);
   else if (mapNeedsGCFrame)
      code.addInstruction(new(code.msa()) MapGCInstruction(_op, x, y, dstOperand));
   else
      code.addInstruction(_op.kind() != Operator::FROM_PLAIN_OP
         ? (Instruction*)new(code.msa()) DyadicOpInstruction(_op, x, y, dstOperand)
         : (Instruction*)new(code.msa()) FromPlainInstruction(x, y, dstOperand));

#if (GARBAGE_COLLECTION==1)
   if (gcLabel != NULL)
      code.genLabel(*gcLabel);
#endif
}

Void DyadicOp::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                              Bool divergent, CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   Operand& y = code.genArgOperand(_y, _typeSig, 2, failLab);
   code.genConditionalInstruction(_op, x, y, labT, labF, divergent);
}

Void DyadicOp::genVoidReduced(CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   Operand& y = code.genArgOperand(_y, _typeSig, 2, failLab);
   if (_op.kind() == Operator::ASSIGN_AT_OP)
      code.addMoveInstruction(x, (Operand&)code.dePtrOperand(y));
   else
      code.addInstruction(new(code.msa()) DyadicOpActionInstruction(_op, x, y));
}


TriadicOp::TriadicOp(Operator op, TypeSig typeSig, Expr x, Expr y, Expr z)
   : _op(op), _typeSig(typeSig), _x(x), _y(y), _z(z) {}

Void TriadicOp::genVoidReduced(CodeLabel& failLab, Code& code) {
   Operand& x = code.genArgOperand(_x, _typeSig, 1, failLab);
   Operand& y = code.genArgOperand(_y, _typeSig, 2, failLab);
   Operand& z = code.genArgOperand(_z, _typeSig, 3, failLab);
   if (_op.kind() == Operator::PUT_AT_STRING_OP)
      code.addInstruction(new(code.msa()) TriadicOpActionInstruction(_op, x, y, z));
   else if (_op.kind() == Operator::PUT_AT_PTR_ARRAY_OP)
      code.addPtrArrayAssignInstruction(z, code.arrayIndexOperand(code.dePtrOperand(x), y));
   else
      assert(FALSE, "TriadicOp::genVoidReduced: Unexpected operator");
}

QuinadicOp::QuinadicOp(Operator op, TypeSig typeSig, Expr v, Expr w, Expr x, Expr y, Expr z)
   : _op(op), _typeSig(typeSig), _v(v), _w(w), _x(x), _y(y), _z(z) {}

Void QuinadicOp::genVoidReduced(CodeLabel& failLab, Code& code) {
   Operand& v = code.genArgOperand(_v, _typeSig, 1, failLab);
   Operand& w = code.genArgOperand(_w, _typeSig, 2, failLab);
   Operand& x = code.genArgOperand(_x, _typeSig, 3, failLab);
   Operand& y = code.genArgOperand(_y, _typeSig, 4, failLab);
   Operand& z = code.genArgOperand(_z, _typeSig, 5, failLab);
   code.addInstruction(new(code.msa()) QuinadicOpActionInstruction(_op, v, w, x, y, z));
}


DirectLambda::DirectLambda(Lambda* lambda)
   : _lambda(lambda) {
}

Void DirectLambda::genEnter(const FnAp* fnAp, Code& code) const {
   _lambda->genEnter(fnAp, code);
}

CurriedLambda::CurriedLambda(Lambda* lambda, CodeLabel* mainLabel)
   : _lambda(lambda), _mainLabel(mainLabel) {
}

Void CurriedLambda::genEnter(const FnAp* fnAp, Code& code) const {
   assert(fnAp != NULL, "CurriedLambda::genEnter: expected application");
   assert(fnAp->nArgs() == _lambda->nFormalParams(),
      "CurriedLambda::genEnter: expected physical arity");

   code.addInstruction(new(code.msa()) EmptyArgsInstruction(FALSE));
   fnAp->copyArgs(code);
   code.addUnaryInstruction(Instruction::GOTO_INSTR,
      code.labelOperand(*_mainLabel));
}

Void CurriedLambda::genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code) {
   assert(fnAp != NULL, "CurriedLambda::genEnter: expected application");
   assert(fnAp->nArgs() == _lambda->nFormalParams(),
      "CurriedLambda::genEnter: expected physical arity");

   code.addInstruction(new(code.msa()) EmptyArgsInstruction(FALSE));
   fnAp->copyArgs(code);
   code.addUnaryInstruction(Instruction::GOTO_INSTR,
      code.labelOperand(*_mainLabel));
}

Constructor::Constructor(const FnAp& fnAp, Bool isSum)
   : _fnAp(fnAp), _isSum(isSum) {
   if (_isSum)
      _tag = toTag(snd(fnAp.fun_()));
}


Bool Constructor::isConstReduced(Expr& val, const Code& code) const {
   assert(_fnAp._nArgs == 1 &&
      isPair(_fnAp.fun_()) &&
      fst(_fnAp.fun_()) == (Expr)CONSTRUCT &&
      _fnAp._argV[0] == (Expr)VOID, "Constructor::isConstReduced: Expected boolean constructor");

   val = snd(_fnAp.fun_());

   return TRUE;
}

Void Constructor::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   if (_fnAp._argV[0] == Expr(VOID))
      code.addBinaryInstruction(Instruction::CONS_A_S_INSTR,
                                code.constOperand(_tag),
                                code.varOperand(dst));
   else {
      Instruction* ins;
      if (_isSum && !(isAp(dst.typeSig()) &&
          (fun(dst.typeSig()) == typeCon(Ptr) ||
           fun(dst.typeSig()) == typeCon(List))))
         ins = new(code.msa()) Cons_S_Instruction(code.genOperand(_fnAp._argV[0], argTypeSig(_fnAp._typeSig, 1), failLab),
                                                  code.constOperand(_tag),
                                                  code.varOperand(dst));
      else {
         ins = new(code.msa()) MoveInstruction(code.genOperand(_fnAp._argV[0], argTypeSig(_fnAp._typeSig, 1), failLab),
                                               code.varOperand(dst));
         ((MoveInstruction*)ins)->setIsConstructor();
      }
      code.addInstruction(ins);
   } 
}

Bool Constructor::genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code) {
   code.genExpr(_fnAp._argV[0], dst, failLab);
   return TRUE;
}

Void Constructor::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                                 Bool divergent, CodeLabel& failLab, Code& code) {

// Not reachable now (with constant expression suppression), so could assert anyway

   assert(_fnAp._nArgs == 1 &&
          isPair(_fnAp.fun_()) &&
          fst(_fnAp.fun_()) == (Expr)CONSTRUCT &&
          _fnAp._argV[0] == (Expr)VOID, "Constructor::genCondReduced: Expected boolean constructor");

   code.addUnaryInstruction(Instruction::GOTO_INSTR,
      code.labelOperand(toTag(snd(_fnAp.fun_())) == 0 ? labF : labT));
}

Deconstructor::Deconstructor(const FnAp& fnAp, Bool isSum, Bool byPtr)
   : _fnAp(fnAp), _isSum(isSum), _byPtr(byPtr), _tag(0) {
   if (_isSum)
      _tag = toTag(snd(fnAp.fun_()));
}

Expr Deconstructor::reduce(CodeLabel& failLab, Code& code) {
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), arg(_fnAp.typeSig()), Var::LOCAL_VAR);
   genVarReduced(cellBody(cell_, Var), failLab, code);
   return fromCell(cell_);
}

Void Deconstructor::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(arg(fun(_fnAp.typeSig())), Var::LOCAL_VAR);
   code.genVar(_fnAp._argV[0], var, failLab);
   Bool isList = isListTypeSig(var.typeSig());

   if (!_isSum || isList) {
 /*     if (!_byPtr)
         code.addBinaryInstruction(Instruction::MOVE_INSTR,
                                   code.varOperand(var),
                                   code.varOperand(dst));
     else */
         code.addInstruction(new(code.msa())
            DeconsInstruction(code.varOperand(var),
                              _byPtr,
                              code.varOperand(dst))); 
   } else
      code.addInstruction(new(code.msa())
         Decons_S_Instruction(code.varOperand(var),
                              _tag,
                              _byPtr,
                              code.varOperand(dst)));
}

Bool Deconstructor::genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code) {
   code.genExpr(_fnAp._argV[0], dst, failLab);
   return TRUE;
}


PtrConstructor::PtrConstructor(const FnAp& fnAp)
   : _fnAp(fnAp) {
}

Expr PtrConstructor::reduce(CodeLabel& failLab, Code& code) {
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), arg(_fnAp.typeSig()), Var::LOCAL_VAR);
   genVarReduced(cellBody(cell_, Var), failLab, code);
   return fromCell(cell_);
}

Void PtrConstructor::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   Operand& operand = code.genOperand(_fnAp._argV[0],
                                      argTypeSig(_fnAp.typeSig()), failLab);

#if (GARBAGE_COLLECTION==1)
   CodeLabel& gcLabel = code.newLabel(FALSE, NULL, FALSE);
   FrameDescrInstruction* frameDescrIns = new (code.msa()) FrameDescrInstruction(code.labelOperand(gcLabel),
      code.genGC_stub());
   code.addInstruction(frameDescrIns);
#endif

   code.addInstruction(new(code.msa()) PtrInstruction(Instruction::PTR_INSTR, code.varOperand(dst)));

#if (GARBAGE_COLLECTION==1)
   code.genLabel(gcLabel);
#endif

   code.addMoveInstruction(operand, (Operand&)code.dePtrOperand(code.varOperand(dst)));
}

Bool PtrConstructor::genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code) {
   assert(FALSE, "PtrConstructor::genExpr:: not implemented");
   return FALSE;
}



TupleConstructor::TupleConstructor(const FnAp& fnAp)
   : _fnAp(fnAp) {
}

Expr TupleConstructor::reduce(CodeLabel& failLab, Code& code) {
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), resultTypeSig(_fnAp.typeSig(), _fnAp._nArgs), Var::LOCAL_VAR);
   genVarReduced(cellBody(cell_, Var), failLab, code);
   return fromCell(cell_);
}

Void TupleConstructor::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   UInt nComps = _fnAp._nArgs;
   Operand** comps = (Operand**)code.msa().alloc(nComps * sizeof(Operand*));

   StructTemplateAllocator structTemplateAllocator(_fnAp.nArgs(), code);
   for (UInt i = 0; i < _fnAp.nArgs(); i++)
      structTemplateAllocator.slot(i, code.useType(argTypeSig(_fnAp._typeSig, i  + 1)));
   StructTemplate& structTemplate = structTemplateAllocator.structTemplate();
   
// Load components in reverse order to satisfy
// function application semantics

   for (UInt i = nComps; i-- > 0; ) {
      Expr comp = _fnAp._argV[i];
      TypeSig typeSig = argTypeSig(_fnAp._typeSig, i + 1);
      comps[i] = &code.genOperand(comp, typeSig, failLab);
   }

   code.addInstruction(new(code.msa()) StructInstruction(code.structOperand(nComps, comps, structTemplate, dst.typeSig()),
                                                         code.varOperand(dst)));
}

Bool TupleConstructor::genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code) {
   return TRUE;
}



Select::Select(TypeSig typeSig, Expr dataCon, Bool byPtr, UInt index, Expr arg)
   : _typeSig(typeSig),
     _dataCon(dataCon), _byPtr(byPtr), _index(index),
     _arg(arg) {
}

Expr Select::reduce(CodeLabel& failLab, Code& code) {
   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), arg(_typeSig), Var::LOCAL_VAR);
   genVarReduced(cellBody(cell_, Var), failLab, code);
   return fromCell(cell_);
}

Void Select::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   code.genSelect(_arg, arg(_typeSig), _byPtr, isTupleCon(_dataCon) ? _index : 0, dst, failLab);
}

Bool Select::genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code) {
   Operand& x = code.genOperand(_arg, _typeSig, failLab);
/*
   code.addInstruction(new(code.msa())
      SelectInstruction(useType(_typeSig, code.typeTable()),
                        dataConTag(_dataCon), _index, x, code.varOperand(dst)));
*/
   return TRUE;
}

// FatBar:

FatBar::FatBar(TypeSig typeSig, Expr x, Expr y)
   : _typeSig(typeSig), _x(x), _y(y) {
}

Void FatBar::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   CodeLabel& lab = code.newLabel();
   CodeLabel& lab2 = code.newLabel();
   code.genVar(_x, dst, lab);
   code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab2));
   code.genLabel(lab);
   code.genVar(_y, dst, failLab);
   code.genLabel(lab2);
}

Expr FatBar::reduce(CodeLabel& failLab, Code& code) {
   TypeSig typeSig = resultTypeSig(_typeSig, 2);
   if (typeSig == typeCon(Void)) {
      genVoidReduced(failLab, code);
      return VOID;
   }

   Cell& cell_ = *new(sizeof(Var), code.msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), typeSig, Var::LOCAL_VAR);
   genVarReduced(cellBody(cell_, Var), failLab, code);
   return fromCell(cell_);
}

Void FatBar::genVoidReduced(CodeLabel& failLab, Code& code) {
   CodeLabel& lab1 = code.newLabel();
   CodeLabel& lab2 = code.newLabel();
   code.genVoidReduced(_x, lab1);
   code.addUnaryInstruction(Instruction::GOTO_INSTR, code.labelOperand(lab2));
   code.genLabel(lab1);
   code.genVoidReduced(_y, failLab);
   code.genLabel(lab2);
}

Void FatBar::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   CodeLabel& lab = code.newLabel(); 
   code.genReturn(_x, typeSig, lab);
   code.genLabel(lab);
   code.genReturn(_y, typeSig, failLab);
}

Seq::Seq(Expr e1, Expr e2)
   : _e1(e1), _e2(e2) {
}

Void Seq::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   code.genVoid(_e1, failLab);
   code.genVar(_e2, dst, failLab);
}

Operand& Seq::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   code.genVoid(_e1, failLab);
   return code.genOperand(_e2, typeSig, failLab);
}

Expr Seq::reduce(CodeLabel& failLab, Code& code) {
   code.genVoid(_e1, failLab);
   return _e2;
}

Void Seq::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                         Bool divergent, CodeLabel& failLab, Code& code) {
   code.genVoid(_e1, failLab);
   code.genCond(_e2, reduceN, labT, labF, divergent, failLab);
}

Void Seq::genVoidReduced(CodeLabel& failLab, Code& code) {
   code.genVoid(_e1, failLab);
   code.genVoid(_e2, failLab);
}

Void Seq::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   code.genVoid(_e1, failLab);
   code.genReturn(_e2, typeSig, failLab);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

Bool FnAp::isPartial(Void) const {
   return _nArgs < arity(_typeSig);
}

CodeNode* FnAp::builtIn(const Code& code) const {
    if (_fun == Expr(CONSTRUCT)) {
      assert(_nArgs == 1, "FnAp::builtIn: expected 1 argument");
      return new(code.msa()) Constructor(*this, FALSE);
   } else if (isPair(_fun) && fst(_fun) == (Expr)CONSTRUCT) {
      return new(code.msa()) Constructor(*this, TRUE);
   } else if (_fun == Expr(DECONSTRUCT) ||
              _fun == Expr(DECONSTRUCT_PTR)) {
      assert(_nArgs == 1, "FnAp::builtIn: expected 1 argument");
      return new(code.msa()) Deconstructor(*this, FALSE, _fun == Expr(DECONSTRUCT_PTR));
   } else if (isPair(_fun) &&
              (fst(_fun) == (Expr)DECONSTRUCT ||
               fst(_fun) == (Expr)DECONSTRUCT_PTR)) {
      assert(_nArgs == 1, "FnAp::builtIn: expected 1 argument");
      return new(code.msa()) Deconstructor(*this, TRUE, fst(_fun) == (Expr)DECONSTRUCT_PTR);
   } else if (_fun == Expr(PTR_CONSTRUCTOR)) {
      assert(_nArgs == 1, "FnAp::builtIn: expected 1 argument");
      return new(code.msa()) PtrConstructor(*this);
   }  else if (isTupleCon(_fun) && tupleCard(_fun) == _nArgs)
      return new(code.msa()) TupleConstructor(*this);
   else if (_fun == Expr(TAG_OF)) {
      assert(_nArgs == 1, "FnAp::builtIn: expected 1 argument");
         return new(code.msa()) TagOfOp(_typeSig, _argV[0]);
   } else if (isTriple(_fun)) {
      if ((fst3(_fun) == Expr(SELECT) || fst3(_fun) == Expr(SEL_PTR))
          && _nArgs == 1)
         return new(code.msa()) Select(snd3(_fun),
                                       fst(thd3(_fun)),
                                       fst3(_fun) == Expr(SEL_PTR),
                                       toInt(snd(thd3(_fun))),
                                       _argV[0]);
   } else switch (formOf(_fun)) {
      case NAME_OCC: {
         const NameOcc& nameOcc = toBody(_fun, NameOcc);
         Name name = NULL_NAME;
         switch (formOf(nameOcc.val())) {
				case DECL:
					name = nameOcc.name();
					break;

            case BUILT_IN_FN:
               name = toBody(nameOcc.val(), BuiltInFn).name();
               break;

            default:
               break;
         }
         if (name < 0)
            break;
         Operator op(nameString_(name, code.nameTable()));

         if (op.kind() != Operator::UNKNOWN_OP &&
             _nArgs == op.arity_())
            switch (_nArgs) {
               case 1:  
                  if (op.kind() == Operator::TYPE_OF_OP)
                     return new(code.msa()) TypeOfOp(op, _typeSig, _argV[0]);
                  else if (op.kind() == Operator::CAST_PTR_OP ||
                           op.kind() == Operator::CAST_TAG_OP)
                     return new(code.msa()) IdOp(op, _typeSig, _argV[0]);
                  else if (op.kind() == Operator::ENV_OF_OP)
                     return new(code.msa()) EnvOfOp(op, _typeSig, _argV[0]);
                  else if (op.kind() == Operator::CONSTRUCTOR_STRING_OP)
                     return new(code.msa()) ConstructorStringOp(_typeSig,
                                                                _argV[0]);
                  else if (op.kind() == Operator::CONSTRUCT_FROM_NAME_OP)
                     return new(code.msa()) ConstructFromNameOp(_typeSig,
                                                               _argV[0]);
                  else if (op.kind() == Operator::CONSTRUCT_FROM_STRING_OP)
                     return new(code.msa()) ConstructFromStringOp(_typeSig,
                                                                  _argV[0]);
                  else if (op.kind() == Operator::MAP_CLOSURE_OP)
                     return new(code.msa()) MapClosure(_argV[0]);

#if (GARBAGE_COLLECTION==1)
                  else if (op.kind() == Operator::MARK_FREE_VARS_GC_OP)
                     return new(code.msa()) MarkFreeVars_GC(_argV[0]);
#endif

                  else
                     return new(code.msa()) MonadicOp(op, _typeSig, _argV[0]);
                  break;

               case 2:
                  if (op.kind() == Operator::EXTRACT_FREE_VARS_OP)
                     return new(code.msa()) ExtractFreeVars(_argV[0], _argV[1]);
                  else if (op.kind() == Operator::INSERT_FREE_VARS_OP)
                     return new(code.msa()) InsertFreeVars(_argV[0], _argV[1]);
                  else
                     return new(code.msa()) DyadicOp(op, _typeSig, _argV[0], _argV[1]);

               case 3:
                  return new(code.msa()) TriadicOp(op, _typeSig, _argV[0], _argV[1], _argV[2]);

               case 5:
                  return new(code.msa()) QuinadicOp(op, _typeSig, _argV[0], _argV[1], _argV[2], _argV[3], _argV[4]);

               default:
                  break;
            }
         break;
      }
            
      case COND:
         if (_nArgs == 3)
            return new(code.msa()) Cond(_typeSig, _argV[0], _argV[1], _argV[2]);
         break;

      case FAT_BAR:
         if (_nArgs == 2)
            return new(code.msa()) FatBar(_typeSig, _argV[0], _argV[1]);
         break;

      case SEQ:
         if (_nArgs == 2)
            return new(code.msa()) Seq(_argV[0], _argV[1]);
         break;

      case EQ_TAG_FN:
         if (_nArgs == 2)
            return new(code.msa()) DyadicOp(Operator(Operator::EQ_OP,
                                                     Operator::OP_TYPE_TAG, 2),
                                            _typeSig, _argV[0], _argV[1]);
         break;

      case EQ_NULL_FN:
         if (_nArgs == 1)
            return new(code.msa()) MonadicOp(Operator(Operator::EQ_NULL_OP,
                                                      Operator::OP_TYPE_LIST, 2),
                                            _typeSig, _argV[0]);
         break;


      case DATA_CON:
         break;

      default:
         break;
   }

   return NULL;
}

#undef localConst
#define localConst

Void FnAp::genArgs(Lambda* lambda, CodeLabel& failLab, Code& code) {
   _argVars = (Var**)code.msa().alloc(_nArgs * sizeof(Var*));
   _fnLambda = lambda;

// Arguments in reverse order to support currying

// Using temporaries here simplifies tail recursive functions.
// The peephole optimiser (or native compiler) will subsequently
// remove and reorder the result to improve the code.

   UInt i;
   for (i = _nArgs; i-- > 0; ) {
      TypeSig typeSig = argTypeSig(_typeSig, i + 1);
      _argVars[i] = &code.newVar(typeSig, i == 0 ? Var::LOCAL_ARG_VAR : Var::LOCAL_VAR);
      code.genVar(_argV[i], *_argVars[i], failLab);
   }
}

Void FnAp::copyArgs(UInt from, UInt nArgs, Lambda* lambda, Code& code) const {
   assert(_argVars != NULL, "FnAp::copyArgs: _argVars = NULL");
   assert(from + nArgs <= _nArgs, "FnAp::copyArgs: invalid argument range");

   for (UInt i = nArgs; i-- > 0; ) {
      UInt srcIndex = from + i;
      Var& var = *new(code.msa()) Var(newName(), _argVars[srcIndex]->typeSig(),
         i == 0 ? Var::ARG_VAR : Var::EXTRA_ARG_VAR, lambda);
      if (lambda != NULL && i < lambda->_nFormalParams)
         var._parentLambda = lambda;
      var._index = i;
      code.addBinaryInstruction(Instruction::MOVE_INSTR,
         code.varOperand(*_argVars[srcIndex]),
         code.varOperand(var));
   }
}

Void FnAp::copyArgs(Code& code) const {
   copyArgs(0, _nArgs, _fnLambda, code);
}

Void FnAp::enterPreparedResult(Var& funVar, TypeSig funTypeSig,
   UInt from, UInt nArgs, Code& code) const {
   Bool fullyApplied = nArgs == arity(funTypeSig);

   code.addInstruction(new(code.msa()) EmptyArgsInstruction(!fullyApplied));
   copyArgs(from, nArgs, NULL, code);

   Var& cellVar = code.newVar(funTypeSig, Var::CLOSURE_VAR);
   code.addBinaryInstruction(Instruction::MOVE_INSTR,
      code.varOperand(funVar),
      code.varOperand(cellVar));

   code.addInstruction(new(code.msa())
      EnterCellInstruction(code.varOperand(cellVar),
         fullyApplied,
         code._returnState != NULL
         ? code._returnState->_contVar
         :
#if (CLOSURE_UPDATE==1)
         code._updateContVar, code._updateCellVar)
#else
      NULL)
#endif
      );
}






Void FnAp::genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code) {
   Var& var = code.newVar(resultTypeSig(_typeSig, _nArgs), Var::LOCAL_VAR);
   genVarReduced(var, failLab, code);
   var.genSelect(byPtr, index, dst, code);
}

// Generate conditional function application

Void FnAp::genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                          Bool divergent, CodeLabel& failLab, Code& code) {
   CodeNode* codeForm;
   if ((codeForm = builtIn(code)) != NULL)
      codeForm->genCondReduced(reduceN, labT, labF, divergent, failLab, code);
   else {
      Expr returnState = reduceNonBuiltIn(failLab, code);
      code.genConditionalInstruction(Operator("eqTAG"),
               code.varOperand(toBody(toBody(returnState, ReturnState).var(TAG, code), Var)),
                               code.constOperand(0),
                               labF, labT, divergent);
   }
}

static Lambda* functionLambda(Expr expr) {
   if (formOf(expr) == VAR)
      return toBody(expr, Var)._lambda;

   if (formOf(expr) == NAME_OCC) {
      Expr val = toBody(expr, NameOcc).val();
      if (formOf(val) == VAR)
         return toBody(val, Var)._lambda;
   }

   if (formOf(expr) == CURRIED_LAMBDA)
      return toBody(expr, CurriedLambda).lambda();

   return NULL;
}

// FnAp::reduceNonBuiltIn: Reduce non built-in function application

Expr FnAp::reduceNonBuiltIn(CodeLabel& failLab, Code& code) {
   Lambda* lambda = functionLambda(_fun);
   TypeSig typeSig = resultTypeSig(_typeSig, _nArgs);
   Expr returnState = code.returnState(typeSig, lambda);
   genArgs(lambda, failLab, code);
   code.genEnter(_fun, typeSig, this, failLab);
   toBody(returnState, ReturnState).genContinuation(code);
   return returnState;
}

// FnAp::reduce: Reduce function application

Expr FnAp::reduce(CodeLabel& failLab, Code& code) {
   CodeNode* codeForm = builtIn(code);
   return codeForm != NULL
      ? codeForm->reduce(failLab, code)
      : reduceNonBuiltIn(failLab, code);
}

Void FnAp::genVarReduced(Var& dst, CodeLabel& failLab, Code& code) {
   CodeNode* codeForm;
   if ((codeForm = builtIn(code)) != NULL)
      codeForm->genVarReduced(dst, failLab, code);
   else {
      Expr returnState = reduceNonBuiltIn(failLab, code);
      toBody(returnState, ReturnState).genVar(dst, code);
   }
}

// FnAp::isConstReduced: Predicate to test for reduced function application

Bool FnAp::isConstReduced(Expr& val, const Code& code) const {
   CodeNode* codeForm = builtIn(code);
   return codeForm != NULL
      ? codeForm->isConstReduced(val, code)
      : FALSE;
}


Operand& FnAp::genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   CodeNode* codeForm;
   if ((codeForm = builtIn(code)) != NULL)
      return codeForm->genOperandReduced(typeSig, failLab, code);
   else {
      Expr returnState = reduceNonBuiltIn(failLab, code);
      return toBody(returnState, ReturnState).genOperand(typeSig, code);
   }
}

Void FnAp::genVoidReduced(CodeLabel& failLab, Code& code) {
   CodeNode* codeForm = builtIn(code);
   if (codeForm != NULL)
      codeForm->genVoidReduced(failLab, code);
   else
      (Void)reduceNonBuiltIn(failLab, code);
}


Void FnAp::genEnterReduced(TypeSig typeSig,
                           FnAp* fnAp,
                           CodeLabel& failLab,
                           Code& code) {

   TypeSig resultTS = resultTypeSig(_typeSig, _nArgs);

   CodeNode* codeForm = builtIn(code);
   if (codeForm != NULL) {
      Var& var = code.newVar(resultTS, Var::LOCAL_VAR);
      codeForm->genVarReduced(var, failLab, code);
      var.genEnter(fnAp, code);
      return;
   }

   Expr returnState = reduceNonBuiltIn(failLab, code);
   toBody(returnState, ReturnState).genEnter(resultTS, fnAp, code);
}

/*

Void FnAp::genEnterReduced(TypeSig typeSig, FnAp* fnAp,
   CodeLabel& failLab, Code& code) {
   CodeForm* codeForm = builtIn(code);
   if (codeForm != NULL)
      codeForm->genEnterReduced(_typeSig, fnAp, failLab, code);
   else
      code.genEnterReduced(_fun, _typeSig, fnAp, failLab);
}

*/

Void FnAp::genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code) {
   CodeNode* codeForm = builtIn(code);
   if (codeForm != NULL)
      codeForm->genReturnReduced(typeSig, failLab, code);
   else {
      genArgs(functionLambda(_fun), failLab, code);
      code.genEnter(_fun, typeSig, this, failLab);
   }
}
Bool Code::hasUnboxedRepr(TypeSig typeSig) {
   return typeSig == typeCon(Name) ||
          typeSig == typeCon(Type) ||
          typeSig == typeCon(Int) ||
          typeSig == (Expr)TAG ||
          typeSig == typeCon(Char) ||
          typeSig == typeCon(Ref);
}

Expr Code::newVarExpr(TypeSig typeSig, Var::Kind kind) {
   Cell& cell_ = *new(sizeof(Var), msa()) Cell(VAR);
   new(cell_.body()) Var(newName(), typeSig, kind, _lambda);
   return fromCell(cell_);
}

Void Code::addVarElement(Var& var, VarElement::Flags flags) {
   _basicBlk->_vars.addElement(var, msa())->setFlags(flags);
}

// Code::useType: Use a data type
//
// A list of fixed known types is formed
// for subsequent generation of type methods to support dynamic values.
// This may use instances of Env and Show which should be
// in the same module as all types that are defined in it.

#undef type

Bool Code::typeGenPending(Type type, Bool genMethods) const {
   TypeElement* typeElement = _types;
   while (typeElement != NULL) {
      if (typeElement->type() == type) {
         if (genMethods && !typeElement->genMethods())
            typeElement->setGenMethods();
         return TRUE;
      }
      typeElement = typeElement->next();
   }
   return FALSE;
}

Type Code::useType(TypeSig typeSig, Bool genMethods/* = FALSE*/) {
   Type type = isExprTypeSig(typeSig) == TYPE_PRED_TRUE
      ? NULL_TYPE
      : builtInTypeTable().lookUp(typeSig);
   if (type == NULL_TYPE) {

#ifdef TRACE
      if (traceFlag) {
         outStream << "Using type signature: ";
         printTypeSig(typeSig, 0, FALSE, outStream, _env);
         if (genMethods)
            outStream << " -- with methods";
         outStream << endl;
      }
#endif

      UInt typeSigHash = hashTypeSig(typeSig);
      TypeDescr* typeDescr = typeTable().lookUp(typeSig, typeSigHash);
      if (typeDescr == NULL) {
         typeDescr = allocTypeDescr(typeSig, _msa);
         type = builtInTypeCount + typeTable().addDescr(typeDescr);
         typeTable().enterDescr(typeDescr);
      } else
         type = builtInTypeCount + typeDescr->type();

      if (typeDescr->denotedType() == NULL_TYPE) {
         typeDescr->setDenotedType(type);  // Guard recursive denotation.
         typeDescr->setDenotedType(useType(evalTypeSig(typeSig)));
      }

      if (!typeGenPending(typeDescr->type(), genMethods) &&
         !hasTypeVar(typeSig) &&
         !(formOf(typeSig) == TYPE_CON &&
            toBody(typeSig, TypeCon)._objectFlag)) {
         TypeElement** typeElementPtr = &_types;
         while (*typeElementPtr != NULL)
            typeElementPtr = &((*typeElementPtr)->next());
         *typeElementPtr = new(_msa) TypeElement(NULL, typeDescr->type(), genMethods);
      }
   }

   return type;
}

Void Code::methodTypeError(ConstString s, TypeSig typeSig) {
   outStream << "Method generation failed for: ";
   printTypeSig(typeSig, 0, FALSE, outStream, _env);
   outStream << endl;
   error(s);
}

Void Code::genTypeMethods(CodeLabel& failLab) {
   while (_types != NULL) {
      TypeSig typeSig = typeTable().typeSig(_types->type());
      TypeSig denotedTypeSig_ = evalTypeSig(typeSig);
      Type denotedType = typeTable().typeDescr(_types->type())->denotedType();
      assert(denotedType != NULL_TYPE,
             "Code::genTypeMethods: missing denoted type");

      Lambda* evalFnLambda =     NULL;
      Lambda* mapFnLambda =      NULL;
      Lambda* assignFnLambda =   NULL;
      Lambda* insertTxtFnLambda = NULL;

#if (SERIALISATION==1)
      Lambda* extractBinFnLambda =  NULL;
      Lambda* insertBinFnLambda =   NULL;
#endif

#if (GARBAGE_COLLECTION==1)
      Lambda* markFnLambda = NULL;
#endif

      if (_types->genMethods()) {

#ifdef TRACE
         if (traceFlag) {
            outStream << "Generating type methods for: ";
            printTypeSig(typeSig, 0, FALSE, outStream, _env);
            outStream << endl;
         }
#endif

         TypeSig ptrTS = ap(typeCon(Ptr), typeSig, msa());

//-----  evalGenPtr:: Ptr <typeSig> -> <denotedTypeSig>

         Cell& evalFnSnippetCell = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
         new(evalFnSnippetCell.body()) Snippet(
            mkTriple(CONSTRAIN,
               fromName(useName("evalGenPtr", nameTable())),
               arrowTypeSig(ptrTS, denotedTypeSig_, msa()), msa()));
         Expr evalFnSnippet = fromCell(evalFnSnippetCell);

         if (!(Tran(1, "", _env).tran(evalFnSnippet) &&
            TypeCheck(

#ifdef TRACE
               traceFlag ? "substs" :
#endif

               "", _env, _moduleDefn).typeCheck(evalFnSnippet) &&

            Tran(2,

#ifdef TRACE
               traceFlag ? "pass=2 trace printAST" :
#endif

               "", _env).tran(evalFnSnippet))) {
            methodTypeError("Failed to generate 'eval' type method", typeSig);
            return;
         }

//-----  mapGenPtr:: Ptr <typeSig> -> Env -> Ptr <typeSig>

         Cell& mapFnSnippetCell = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
         new(mapFnSnippetCell.body()) Snippet(
            mkTriple(CONSTRAIN,
               fromName(useName("mapGenPtr", nameTable())),
               arrowTypeSig(
                  ptrTS,
                  arrowTypeSig(
                     typeCon(Env),
                     ptrTS, msa()), msa()), msa()));
         Expr mapFnSnippet = fromCell(mapFnSnippetCell);

         if (!(Tran(1, "", _env).tran(mapFnSnippet) &&
            TypeCheck(

#ifdef TRACE                        
               traceFlag ? "substs" :
#endif

               "", _env, _moduleDefn).typeCheck(mapFnSnippet) &&

            Tran(2,

#ifdef TRACE            
               traceFlag ? "pass=2 trace printAST" :
#endif

               "", _env).tran(mapFnSnippet))) {
            methodTypeError("Failed to generate \'map\' type method", typeSig);
            return;
         }

//-----  assignByPtr:: Ptr <typeSig> -> Ptr <typeSig> -> Void

         Cell& assignFnSnippetCell = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
         new(assignFnSnippetCell.body()) Snippet(
            mkTriple(CONSTRAIN,
               fromName(useName("assignByPtr", nameTable())),
               arrowTypeSig(
                  ptrTS,
                  arrowTypeSig(
                     ptrTS,
                     typeCon(Void), msa()), msa()), msa()));
         Expr assignFnSnippet = fromCell(assignFnSnippetCell);
         if (!(Tran(1, "", _env).tran(assignFnSnippet) &&
            TypeCheck(

#ifdef TRACE                        
               traceFlag ? "trace substs" :
#endif

               "", _env, _moduleDefn).typeCheck(assignFnSnippet) &&
            Tran(2,

#ifdef TRACE            
               traceFlag ? "pass=2 trace printAST" :
#endif

               "", _env).tran(assignFnSnippet))) {
            methodTypeError("Failed to generate \'assign\' type method", typeSig);
            return;
         }

//-----  insertTxtGenPtr:: OutputStream Char -> Ptr <typeSig> -> Void

         Cell& insertTxtFnSnippetCell = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
         new(insertTxtFnSnippetCell.body()) Snippet(
            mkTriple(CONSTRAIN,
               fromName(useName("insertTxtGenPtr", nameTable())),
               arrowTypeSig(
                  ap(typeCon(OutputStream), typeCon(Char), msa()),
                  arrowTypeSig(
                     ptrTS,
                     typeCon(Void), msa()), msa()), msa()));
         Expr insertTxtFnSnippet = fromCell(insertTxtFnSnippetCell);
         if (!(Tran(1, "", _env).tran(insertTxtFnSnippet) &&
            TypeCheck(

#ifdef TRACE                       
               traceFlag ? "trace substs" :
#endif

               "", _env, _moduleDefn).typeCheck(insertTxtFnSnippet) &&
            Tran(2,

#ifdef TRACE            
               traceFlag ? "pass=2 trace printAST" :
#endif

               "", _env).tran(insertTxtFnSnippet))) {
            methodTypeError("Failed to generate \'insertTxt\' type method", typeSig);
            return;
         }

#if (SERIALISATION==1)
//-----  extractBinGenPtr:: (InputStream Byte)-> Ptr <typeSig>;

         Cell& extractBinFnSnippetCell = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
         new(extractBinFnSnippetCell.body()) Snippet(
            mkTriple(CONSTRAIN,
                     fromName(useName("extractBinGenPtr", nameTable())),
                     arrowTypeSig(
                         ap(typeCon(InputStream), typeCon(Byte), msa()),
                         ptrTS, msa()), msa()));


         if (traceFlag)
            outStream << "Extract***\n";

         Expr extractBinFnSnippet = fromCell(extractBinFnSnippetCell);
         if (!(Tran(1, "", _env).tran(extractBinFnSnippet) &&
            TypeCheck(

#ifdef TRACE                        
               traceFlag ? "trace substs" :
#endif

               "", _env, _moduleDefn).typeCheck(extractBinFnSnippet) &&
            Tran(2,

#ifdef TRACE            
               traceFlag ? "pass=2 trace printAST" :
#endif

               "", _env).tran(extractBinFnSnippet))) {
            methodTypeError("Failed to generate \'extract\' type method", typeSig);
            return;
         }

//-----  insertBinGenPtr:: (OutputStream Byte) -> Ptr <typeSig> -> Void

         Cell& insertBinFnSnippetCell = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
         new(insertBinFnSnippetCell.body()) Snippet(
            mkTriple(CONSTRAIN,
               fromName(useName("insertBinGenPtr", nameTable())),
               arrowTypeSig(
                   ap(typeCon(OutputStream), typeCon(Byte), msa()),
                   arrowTypeSig(
                       ptrTS,
                       typeCon(Void), msa()), msa()), msa()));

         Expr insertBinFnSnippet = fromCell(insertBinFnSnippetCell);
         if (!(Tran(1, "", _env).tran(insertBinFnSnippet) &&
            TypeCheck(

#ifdef TRACE                       
               traceFlag ? "trace substs" :
#endif

               "", _env, _moduleDefn).typeCheck(insertBinFnSnippet) &&
            Tran(2,

#ifdef TRACE            
               traceFlag ? "pass=2 trace printAST" :
#endif

               "", _env).tran(insertBinFnSnippet))) {
            methodTypeError("Failed to generate \'insert\' type method", typeSig);
            return;
         }
#endif

#if (GARBAGE_COLLECTION==1)
//-----  markGenPtr:: Ptr <typeSig> -> Void

         Cell& markFnSnippetCell = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
         new(markFnSnippetCell.body()) Snippet(
            mkTriple(CONSTRAIN,
               fromName(useName("markGenPtr", nameTable())),
               arrowTypeSig(ptrTS,
                            typeCon(Void), msa()), msa()));

         Expr markFnSnippet = fromCell(markFnSnippetCell);
         if (!(Tran(1, "", _env).tran(markFnSnippet) &&
            TypeCheck(

#ifdef TRACE                       
               traceFlag ? "trace substs" :
#endif

               "", _env, _moduleDefn).typeCheck(markFnSnippet) &&
            Tran(2,

#ifdef TRACE            
               traceFlag ? "pass=2 trace printAST" :
#endif

               "", _env).tran(markFnSnippet))) {
            methodTypeError("Failed to generate \'mark\' type method", typeSig);
            return;
         }
#endif

         Lambda* savedLambda = _lambda;
         _lambda = NULL;

         Analyser("", env(), msa()).analyse(evalFnSnippet);
         evalFnLambda = &toBody(snd(toBody(evalFnSnippet, Snippet).expr()), Lambda);
         evalFnLambda->_simple = TRUE;
         UInt _nPrevErrors = _nErrors;
         evalFnLambda->gen(NULL, failLab, *this, FALSE, FALSE);
         evalFnLambda->adjRefCount(1);
         if (_nErrors > _nPrevErrors)
            methodTypeError("Failed to generate 'eval' type method", typeSig);

         Analyser("", env(), msa()).analyse(mapFnSnippet);
         mapFnLambda = &toBody(snd(toBody(mapFnSnippet, Snippet).expr()), Lambda);
         mapFnLambda->_simple = TRUE;
         _nPrevErrors = _nErrors;
         mapFnLambda->gen(NULL, failLab, *this, FALSE, FALSE);
         mapFnLambda->adjRefCount(1);
         if (_nErrors > _nPrevErrors)
            methodTypeError("Failed to generate \'map\' type method", typeSig);

         Analyser("", env(), msa()).analyse(assignFnSnippet);
         assignFnLambda = &toBody(snd(toBody(assignFnSnippet, Snippet).expr()), Lambda);
         assignFnLambda->_simple = TRUE;
         _nPrevErrors = _nErrors;
         assignFnLambda->gen(NULL, failLab, *this, FALSE, FALSE);
         assignFnLambda->adjRefCount(1);
         if (_nErrors > _nPrevErrors)
            methodTypeError("Failed to generate \'assign\' type method", typeSig);

         Analyser("", env(), msa()).analyse(insertTxtFnSnippet);
         insertTxtFnLambda = &toBody(snd(toBody(insertTxtFnSnippet, Snippet).expr()), Lambda);
         insertTxtFnLambda->_simple = TRUE;
         _nPrevErrors = _nErrors;
         insertTxtFnLambda->gen(NULL, failLab, *this, FALSE, FALSE);
         insertTxtFnLambda->adjRefCount(1);
         if (_nErrors > _nPrevErrors)
            methodTypeError("Failed to generate \'insertTxt\' type method", typeSig);

#if (SERIALISATION==1)
         Analyser(traceFlag ? "trace" : "", env(), msa()).analyse(extractBinFnSnippet);
         extractBinFnLambda = &toBody(snd(toBody(extractBinFnSnippet, Snippet).expr()), Lambda);
         extractBinFnLambda->_simple = TRUE;
         _nPrevErrors = _nErrors;
         extractBinFnLambda->gen(NULL, failLab, *this, FALSE, FALSE);
         extractBinFnLambda->adjRefCount(1);
         if (_nErrors > _nPrevErrors)
            methodTypeError("Failed to generate \'extract\' type method", typeSig);

         Analyser("", env(), msa()).analyse(insertBinFnSnippet);
         insertBinFnLambda = &toBody(snd(toBody(insertBinFnSnippet, Snippet).expr()), Lambda);
         insertBinFnLambda->_simple = TRUE;
         _nPrevErrors = _nErrors;
         insertBinFnLambda->gen(NULL, failLab, *this, FALSE, FALSE);
         insertBinFnLambda->adjRefCount(1);
         if (_nErrors > _nPrevErrors)
            methodTypeError("Failed to generate \'insert\' type method", typeSig);
#endif

#if (GARBAGE_COLLECTION==1)
         Analyser("", env(), msa()).analyse(markFnSnippet);
         markFnLambda = &toBody(snd(toBody(markFnSnippet, Snippet).expr()), Lambda);
         markFnLambda->_simple = TRUE;
         _nPrevErrors = _nErrors;
         markFnLambda->gen(NULL, failLab, *this, FALSE, FALSE);
         markFnLambda->adjRefCount(1);
         if (_nErrors > _nPrevErrors)
            methodTypeError("Failed to generate \'mark\' type method", typeSig);
#endif

         _lambda = savedLambda;

         addInstruction(new(msa())
            TypeDescrInstruction(constOperand(makeTypeConst(_types->type() + builtInTypeCount)),
               constOperand(makeTypeConst(denotedType)),
               evalFnLambda != NULL ?     &labelOperand(*evalFnLambda->_altEntryLabel) : NULL,
               mapFnLambda != NULL ?      &labelOperand(*mapFnLambda->_altEntryLabel) : NULL,
               assignFnLambda != NULL ?   &labelOperand(*assignFnLambda->_altEntryLabel) : NULL,
               insertTxtFnLambda != NULL ? &labelOperand(*insertTxtFnLambda->_altEntryLabel) : NULL

#if (SERIALISATION==1)
             , extractBinFnLambda != NULL ?  &labelOperand(*extractBinFnLambda->_altEntryLabel) : NULL,
               insertBinFnLambda != NULL ?   &labelOperand(*insertBinFnLambda->_altEntryLabel) : NULL
#endif

#if (GARBAGE_COLLECTION==1)
             , markFnLambda != NULL ? &labelOperand(*markFnLambda->_altEntryLabel) : NULL
#endif   
                        
            ));
      }

      _types = _types->next();
   }  
}

#if (GARBAGE_COLLECTION==1)

// Code::genGC_stub: Generate placeholder for garbage collection code
//
// Assumes execSeqFlag True

GC_StubInstruction& Code::genGC_stub(Void) {
   GC_StubInstruction* ins = new(_msa) GC_StubInstruction();
   ins->_lambda = _lambda;

#ifdef USE_INSTR_SEQ
   InstrSeq* instrSeq = new(msa()) InstrSeq();
   instrSeq->append(ins);
   if (_instrSeq->_appendSeq == NULL)
      _instrSeq->_appendSeq = instrSeq;
   else
      _instrSeq->_appendSeq->append(instrSeq);
#else
   CodeLabel skipLab = newLabel(TRUE);
   addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(*skipLab));
   _basicBlk = NULL;
   addInstruction(*ins);
   genLabel(skipLab);
#endif

   return *ins;
}

//  Code::genGC_MarkSnippet: Generate snippet to mark memory used by variable

CodeLabel& Code::genGC_MarkSnippet(Var& var) {
   Cell& varCell = *new(sizeof(VarRef), msa()) Cell(VAR_REF);
   new(varCell.body()) VarRef(var);

   Cell& cell_ = *new(sizeof(Snippet), msa()) Cell(SNIPPET);
   new(cell_.body()) Snippet(
      mkPair(REDUCE,
         ap(fromName(useName("mark_GC", nameTable())),
            fromCell(varCell),
            msa()),
         msa()));

   Expr snippet = fromCell(cell_);
   if (!(Tran(1, "", env()).tran(snippet) &&
      TypeCheck(traceFlag ? "trace" : "", env(), moduleDefn()).typeCheck(snippet) && 
      Tran(2, traceFlag ? "pass=2 printAST" : "", env()).tran(snippet))) {
      error("Failed to generate GC mark code");
   }
   Analyser("", env(), msa()).analyse(snippet);
   CodeLabel& codeLabel = newLabel(TRUE);
   codeLabel.incRef();  
   genLabel(codeLabel);
   codeLabel.basicBlk().vars().addElement(var, msa())->setFlags(VarElement::I);
   genReturn(toBody(snippet, Snippet).expr(), typeCon(Void), *_errLab);
//   _basicBlk->vars().addElement(var, msa())->setFlags(VarElement::O); // Frame variable remains live.
   return codeLabel;
}
#endif

// Code::ret: Return to previous continuation

Void Code::ret(Void) {
   addInstruction(new(_msa) ReturnInstruction(ReturnInstruction::GENERAL,
                                              _returnState != NULL
                                                 ? _returnState->_contVar
                                                 :

#if (CLOSURE_UPDATE== 1)
                                                   _updateContVar, _updateCellVar)
#else
                                                   NULL)
#endif
                                                   
                                               );
}

Operand& Code::genArgOperand(Expr arg, TypeSig fnTypeSig, UInt i, CodeLabel& failLab) {
   return genOperand(arg, argTypeSig(fnTypeSig, i), failLab);
}

// Ensure that the target of a conditional is a unique label.
// The peephole optimiser will short-circuit this where the
// 'True' label is not shared.

Void Code::genPredicateInstruction(Operator op, Operand& x,
                                   CodeLabel& labT, CodeLabel& labF,
                                   Bool divergent) {
   CodeLabel& lab = newLabel();
   addUnaryConditionalInstruction(op, x, lab, divergent);
   addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(labF));
   genLabel(lab);
   addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(labT));
}

Void Code::genPredicateInstruction(Operator op, Operand& x,  Operand& y,
                                   CodeLabel& labT, CodeLabel& labF,
                                   Bool divergent) {
   CodeLabel& lab = newLabel();
   addBinaryConditionalInstruction(op, x, y, lab, divergent);
   addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(labF));
   genLabel(lab);
   addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(labT));
}

Void Code::genConditionalInstruction(Operator op, Operand& x,
                                     CodeLabel& labT, CodeLabel& labF,
                                     Bool divergent) {
   genPredicateInstruction(op, x, labT, labF, divergent);
}

Void Code::genConditionalInstruction(Operator op, Operand& x, Operand& y,
                                     CodeLabel& labT, CodeLabel& labF,
                                     Bool divergent) {
   genPredicateInstruction(op, x, y, labT, labF, divergent);
}

Expr Code::returnState(TypeSig typeSig, Lambda* lambda) {
   Cell& cell_ = *new(sizeof(ReturnState), msa()) Cell(RETURN_STATE);
   new(cell_.body()) ReturnState(typeSig);
   ReturnState& returnState = cellBody(cell_, ReturnState);
   returnState._returnState = _returnState;
   _returnState = &returnState;
   CodeLabel& contLabel = newLabel(TRUE);
   Var& contVar = newVar(LABEL, Var::CONTINUATION_VAR);
   contVar._parentLambda = lambda;
   contVar._contLabel = &contLabel;

   returnState._contLabel = &contLabel;
   returnState._contVar = &contVar;

#if (GARBAGE_COLLECTION==1)
   if (!_gcFlag)
      addInstruction(new (msa()) FrameDescrInstruction(labelOperand(*returnState._contLabel),
                                                       genGC_stub()));
#endif

   addContinuationInstruction(labelOperand(*returnState._contLabel),
                              varOperand(*returnState._contVar)

#if (CLOSURE_UPDATE==1)
                              , FALSE
#endif

#if (GARBAGE_COLLECTION==1)
                              , _gcFlag
#endif

                              );

   return fromCell(cell_);
}

// Code::reserveDeclOrDefns: Generate code to reserve a declaration or definition binding

Void Code::reserveDeclOrDefn(Expr declOrDefn) {
   assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "Code::reserveDeclOrDefn: Expected binding");
   toBody(declOrDefn, NameTypedValBinding).reserve(*this);
}

// Code::reserveDeclOrDefns: Generate code to reserve a list of declaration or definition bindings

Void Code::reserveDeclOrDefns(Expr declOrDefns) {
   mapProc(reserveDeclOrDefn, declOrDefns);
}

// Code::reserveRecDeclOrDefns: Generate code to reserve a recursive binding
// when analysis selected an early slot.

Void Code::reserveRecDeclOrDefn(Expr declOrDefn) {
   assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "Code::reserveRecDeclOrDefn: Expected binding");
   NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
   if (binding.needsRecReservation())
      binding.reserveRec(*this);
}

// Code::reserveRecDeclOrDefns: Generate code to reserve a list of recursive
// bindings whose initialisation strategy requires an early slot.

Void Code::reserveRecDeclOrDefns(Expr declOrDefns) {
   mapProc(reserveRecDeclOrDefn, declOrDefns);
}

// Code::genDeclOrDefns: Generate code for a declaration or definition binding

Void Code::genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab) {
   assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "Code::genDeclOrDefn: Expected binding");
   toBody(declOrDefn, NameTypedValBinding).gen(errLab, *this);
}

// Code::geDeclOrDefns: Generate code for a list of declaration or definition bindings

Void Code::genDeclOrDefns(Expr declOrDefns, CodeLabel& errLab) {
   mapProc1(genDeclOrDefn, declOrDefns, errLab);
}

Void Code::genMove(Var& src, Var& dst) {
   addBinaryInstruction(Instruction::MOVE_INSTR,
                        varOperand(src),
                        varOperand(dst));
}

// Code::genMoveConst: Generate a move instruction for a constant source

Void Code::genMoveConst(Const& _const, Var& dst) {
   addBinaryInstruction(Instruction::MOVE_INSTR,
                             constOperand(_const),
                             varOperand(dst));
}

// Code::valOf: Value of ezxpression

Expr Code::valOf(Expr expr, CodeLabel& failLab) {
   return !(isPair(expr) && fst(expr) == Expr(REDUCE))
      ? expr
      : reduce(snd(expr), failLab);
}

// Code::reduce: Reduce an expression.

Expr Code::reduce(Expr expr, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::reduce: ";
      printTranExpr(expr, FALSE, outStream, env(), TRUE);
      outStream << endl;
   }
#endif

   if (isPair(expr)) {
      if (fst(expr) == Expr(REDUCE))
         return reduce(reduce(snd(expr), failLab), failLab);
      else
         return ERROR;
   } else
      switch (formOf(expr)) {
         case NAME:
         case TYPE:
         case INT:
         case CHAR:
         case TAG:
         case REF:
         case DATA:
         case BUILT_IN_DATA_CON:
            return expr != dataCon(Void)
               ? makeConst(expr)
               : (Expr)VOID;

         case BUILT_IN_FN: {  // Built-in expression
            Expr returnState = this->returnState(denotedTypeSig(toBody(expr, BuiltInFn).typeSig()), NULL);
            addInstruction(new(msa())
               EnterCellInstruction(*new(msa()) BuiltInFnOperand(expr),
                  TRUE,
                  toBody(returnState, ReturnState)._contVar));
            toBody(returnState, ReturnState).genContinuation(*this);
            return returnState;
         }

         case DEFN:
         case NAME_OCC:
         case LAMBDA:
         case FN_AP:
         case LET:
         case CASE:
            return toBody(expr, AST_Node).reduce(failLab, *this);

         case VAR:
            return toBody(expr, Var).reduce(*this);

         case VAR_REF:
            return toBody(expr, VarRef).var().reduce(*this);
 
         case RETURN_STATE:
            return toBody(expr, ReturnState).reduce(*this);

         default:
			   error("Code::reduce: unrecognised form");
            printTranExpr(expr, FALSE, outStream, env());
            outStream << endl;
            return ERROR;
      }
}

// Predicate for constant expression

Bool Code::isConst(Expr expr, Expr& val) const {
   return isPair(expr) && fst(expr) == Expr(REDUCE)
      ? isConstReduced(snd(expr), val)
      : FALSE;
}

// Predicate for reduced constant expression

Bool Code::isConstReduced(Expr expr, Expr& val) const {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::isConstReduced: ";
      printTranExpr(expr, FALSE, outStream, env(), TRUE);
      outStream << endl;
   }
#endif

   switch (formOf(expr)) {
      case FN_AP:
         return toBody(expr, FnAp).isConstReduced(val, *this);

      default:
         break;
   }
   return FALSE;
}

Void Code::genExpr(Expr expr, Var& dst, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
		outStream << "Code::genExpr: ";
		printTypeSig(dst.typeSig(), 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env(), TRUE);
      outStream << endl;
   }
#endif
 
   switch (formOf(expr)) {
      case NAME:
      case TYPE:
      case INT:
      case CHAR:
      case REF:
      case DATA:
         addBinaryInstruction(Instruction::TAG_INSTR,
                              constOperand(toBody(makeConst(expr), Const)), varOperand(dst));
         return;

      case THIS:
         addBinaryInstruction(Instruction::TAG_INSTR,
                              thisOperand(), varOperand(dst));
         break;

      case BUILT_IN_FN:
         addInstruction(new(msa())
            MoveInstruction(*new(msa()) BuiltInFnOperand(expr),
                              varOperand(dst)));
         return;

      case DEFN:
      case NAME_OCC:
         toBody(expr, NameOcc).genExpr(dst, failLab, *this);
         break;

      case VAR:
         toBody(expr, Var).genExpr(dst, *this);
         break;

      case VAR_REF:
         toBody(expr, VarRef).var().genExpr(dst, *this);
         break;
       
      case RETURN_STATE:
         toBody(expr, ReturnState).genExpr(dst, *this);
         break;

      default:
			error("Code::genExpr: unrecognised form");
         printTranExpr(expr, FALSE, outStream, env());
         outStream << endl;;
         break;
   }
}

// Code::genOperand: Generate code to return an operand for an expression

// Operand forms are: Variable
//                    Constant
//                    Label

// Unless otherwise overriden, the default is to generate a variable operand

Operand& Code::genOperand(Expr expr, TypeSig typeSig, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genOperand: ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr)) {
      Expr f = fst(expr);
      Expr s = snd(expr);
      if (f == Expr(REDUCE))
         return genOperandReduced(s, typeSig, failLab);
      else if (f == Expr(LITERAL) ||
               f == Expr(TYPE_SIG))
         expr = makeConst(expr);
      else
         assert(FALSE, "Code::genOperand: unexpected pair");
   }
   switch (formOf(expr)) {
      case CONST:
         return constOperand(toBody(expr, Const));

      case CONS_ENV:
         return constOperand(const_consEnv);

      case THIS:
         return thisOperand();

      case FN_AP:
      case LET:
      case CASE: {
         Var& var = newVar(typeSig, Var::LOCAL_VAR);
         genExpr(expr, var, failLab);
         return varOperand(var);
      }

      case DEFN:
      case NAME_OCC:
         return toBody(expr, AST_Node).genOperand(typeSig, failLab, *this);

      case RETURN_STATE:
         return toBody(expr, ReturnState).genOperand(typeSig, *this);

      case VAR:
         return toBody(expr, Var).genOperand(typeSig, *this);

      case VAR_REF:
         return toBody(expr, VarRef).var().genOperand(typeSig, *this);

      default: {
         Var& var = newVar(typeSig, Var::LOCAL_VAR);
         genVar(expr, var, failLab);
         return varOperand(var);
      }
   }
}

// Code::genOperandReduced: Generate code to return an operand for a reduced expression

Operand& Code::genOperandReduced(Expr expr, TypeSig typeSig, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genOperandReduced: ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr)) {
      Expr f = fst(expr);
      Expr s = snd(expr);
      assert(f == Expr(REDUCE), "Code::genOperandReduced: expected REDUCE");
      return genOperandReduced(reduce(s, failLab), typeSig, failLab);
   }
   switch (formOf(expr)) {
      case LAMBDA:
      case FN_AP:
      case LET:
      case CASE:
      case NAME_OCC:
         return toBody(expr, AST_Node).genOperandReduced(typeSig, failLab, *this);

      default:
         return genOperand(reduce(expr, failLab), typeSig, failLab);
   }
}

// Code::genVar: Generate code to assign a value to a variable

Void Code::genVar(Expr expr, Var& dst, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genVar(";
      printName(dst._name, outStream, nameTable());
      outStream << "): ";
      printTypeSig(dst.typeSig(), 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr)) {
      Expr f = fst(expr);
      Expr s = snd(expr);
      if (f == Expr(REDUCE)) {
         genVarReduced(s, dst, failLab);
         return;
      }
      else if (f == Expr(LITERAL) || f == Expr(TYPE_SIG))
         expr = makeConst(expr);
      else if (f == Expr(INDISPENSABLE)) {
         dst.adjRefCount(1);
         genVar(s, dst, failLab);
         return;
      } else
         assert(FALSE, "Code::genVar: unexpected pair");
   }
   switch (formOf(expr)) {
      case CONST:
         genMoveConst(toBody(expr, Const), dst);
         break;

      case CONS_ENV:
         genMoveConst(const_consEnv, dst);
         break;

      case NAME:
      case TYPE:
      case INT:
      case CHAR:
      case TAG:
      case DATA:
      case BUILT_IN_DATA_CON:
      case DATA_CON:
      case REF:
      case CLASS_METHOD_DEFN:
      case FN_AP:
      case LET:
      case CASE:
         genExpr(expr, dst, failLab);
         break;

      case DEFN:
      case NAME_OCC:
      case LAMBDA:
         toBody(expr, AST_Node).genVar(dst, failLab, *this);
         break;

      case THIS:
         addBinaryInstruction(Instruction::MOVE_INSTR,
            thisOperand(),
            varOperand(dst));
         break;

      case BUILT_IN_FN:
         addInstruction(new(msa())
            MoveInstruction(*new(msa()) BuiltInFnOperand(expr),
               varOperand(dst)));
         break;

      case VAR:
         toBody(expr, Var).genVar(dst, *this);
         break;

      case VAR_REF:
         toBody(expr, VarRef).var().genVar(dst, *this);
         break;

      case RETURN_STATE:
         toBody(expr, ReturnState).genVar(dst, *this);
         break;

      case FAIL:
         addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(failLab));
         break;

      case EXCEPTION:
         addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(*_errLab));
         break;

      default:
         error("Code::genVar: unrecognised form");
         printTranExpr(expr, FALSE, outStream, env());
         outStream << endl;
         break;
   }
}

// Code::genVarReduced: Generate code to assign a value to a variable from a reduced expression

Void Code::genVarReduced(Expr expr, Var& dst, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genVarReduced: ";
      printTypeSig(dst.typeSig(), 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env(), TRUE);
      outStream << endl;
   }
#endif

   if (isPair(expr)) {
      Expr f = fst(expr);
      Expr s = snd(expr);
      assert(f == Expr(REDUCE), "Code::genVarReduced: expected REDUCE");
      genVarReduced(reduce(s, failLab), dst, failLab);
   } else
      switch (formOf(expr)) {
         case DEFN:
            genVarReduced(toBody(expr, Defn).expr(), dst, failLab);
            break;

         case BUILT_IN_DATA_CON:
            if (expr != dataCon(NullRef))
               addBinaryInstruction(Instruction::CONS_A_S_INSTR,
                  constOperand(toBody(expr, BuiltInDataCon).tag()),
                  varOperand(dst));
            else
               genMoveConst(toBody(makeConst(expr), Const), dst);
            break;

         case NAME_OCC:
         case LAMBDA:
         case FN_AP:
         case LET:
         case CASE:
            toBody(expr, AST_Node).genVarReduced(dst, failLab, *this);
            break;

         default:
            genVar(reduce(expr, failLab), dst, failLab);
            break;
         }
}

// Code::genCond: Generate code for a conditional jump

Void Code::genCond(Expr expr, UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                   Bool divergent, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
		outStream << "Code::genCond: ";
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (reduceN > 0)
		genCondReduced(expr, reduceN - 1, labT, labF, divergent, failLab);
	else if (isPair(expr) && fst(expr) == Expr(REDUCE))
		genCondReduced(snd(expr), reduceN, labT, labF, divergent, failLab);
   else if (isTriple(expr) && fst3(expr) == Expr(DISPATCH)) {
      Expr retState = returnState(UNKNOWN, NULL);
      genEnterReduced(snd3(expr), typeCon(Bool), NULL, failLab);
      toBody(retState, ReturnState).genContinuation(*this);
      toBody(retState, ReturnState).genCond(labT, labF, divergent, *this);
   } else
      switch (formOf(expr)) {
         case FAIL:
            addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(failLab));
            break;

         case EXCEPTION:
            addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(*_errLab));
            break;

         case RETURN_STATE:
            toBody(expr, ReturnState).genCond(labT, labF, divergent, *this);
            break;
		  
		   case DEFN:
            genCond(toBody(expr, Defn).expr(), reduceN, labT, labF,
                     divergent, failLab);
            break;

         case VAR:
            toBody(expr, Var).genCond(labT, labF, divergent, *this);
            break;

         case VAR_REF:
            toBody(expr, VarRef).var().genCond(labT, labF, divergent, *this);
            break;

         case NAME_OCC:
            toBody(expr, AST_Node).genCond(reduceN, labT, labF,
                                             divergent, failLab, *this);
            break;

         default:
            error("genCond: unexpected");
            printTranExpr(expr, FALSE, outStream, env());
            outStream << endl;
      }
}

// Code::genCondReduced: Generate code for a conditional jump on value of reduced expression

Void Code::genCondReduced(Expr expr, UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                          Bool divergent, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genCondReduced: ";
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   while (isPair(expr) && fst(expr) == Expr(REDUCE)) {
      expr = snd(expr);
      reduceN++;
   }
   switch (formOf(expr)) {
      case VAR:
         toBody(expr, Var).genCondReduced(reduceN, labT, labF,
                                          divergent, failLab, *this);
         break;

      case VAR_REF:
         toBody(expr, VarRef).var().genCondReduced(reduceN, labT, labF,
                                                   divergent, failLab, *this);
         break;

      case NAME_OCC:
      case LET:
      case FN_AP:
      case CASE:
         toBody(expr, AST_Node).genCondReduced(reduceN, labT, labF,
                                                 divergent, failLab, *this);
         break;

      case SEQ:
         toBody(expr, CodeNode).genCondReduced(reduceN, labT, labF,
            divergent, failLab, *this);
         break;


      default:
         error("Code::genCondReduced: unexpected form");
         printTranExpr(expr, FALSE, outStream, env());
         outStream << endl;
   }
}

// Code::genSelect: Generate code to select a component from an expression

Void Code::genSelect(Expr expr, TypeSig typeSig, Bool byPtr, UInt index,
                     Var& dst, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genSelect: ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr) && fst(expr) == Expr(REDUCE))
      genSelectReduced(snd(expr), typeSig, byPtr, index, dst, failLab);
   else
      switch (formOf(expr)) {
         case RETURN_STATE:
            toBody(expr, ReturnState).genSelect(typeSig, byPtr, index, dst, failLab, *this);
            break;

         case DEFN:
         case NAME_OCC:
         case FN_AP:
            toBody(expr, AST_Node).genSelect(typeSig, byPtr, index, dst, failLab, *this);
            break;

         case VAR:
            toBody(expr, Var).genSelect(byPtr, index, dst, *this);
            break;

         case VAR_REF:
            toBody(expr, VarRef).var().genSelect(byPtr, index, dst, *this);
            break;

         default:
            error("genSelect: unexpected");
            printTranExpr(expr, FALSE, outStream, env());
            outStream << endl;
      }
}

// Code::genSelectReduced: Generate code to select a component
//                         from a reduced expression

Void Code::genSelectReduced(Expr expr, TypeSig typeSig, Bool byPtr, UInt index,
                            Var& dst, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genSelectReduced: ";
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr) && fst(expr) == Expr(REDUCE))
      genSelectReduced(reduce(snd(expr), failLab), typeSig, byPtr, index, dst, failLab);
   else
      switch (formOf(expr)) {
         case FN_AP:
         case LAMBDA:
         case LET:
         case CASE:
            toBody(expr, AST_Node).genSelectReduced(typeSig, byPtr, index, dst, failLab, *this);
            break;

         default:
            genSelect(reduce(expr, failLab), typeSig, byPtr, index, dst, failLab);
            break;
      }
}

// Code::genVoid: Generate code for an expression with no value.

Void Code::genVoid(Expr expr, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genVoid: ";
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr) && fst(expr) == Expr(REDUCE))
      return genVoidReduced(snd(expr), failLab);
   else {
      switch (formOf(expr)) {
         case RETURN_STATE:
         case VOID:  // Nothing to do
            break;

         case FAIL:
            addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(failLab));
            break;

         case EXCEPTION:
            addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(*_errLab));
            break;

         case RETURN:
            ret();
            break;

         default:
            error("Code::genVoid: unexpected");
            printTranExpr(expr, FALSE, outStream, env());
            outStream << endl;
            break;
      }
   }
}

// Code::genVoidReduced: Generate code for a reduced expression with no value.

Void Code::genVoidReduced(Expr expr, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genVoidReduced: ";
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr) && fst(expr) == Expr(REDUCE)) {
      Expr rExpr = reduce(expr, failLab);
      genVoidReduced(rExpr, failLab);
   } else
      switch (formOf(expr)) {
         case RETURN_STATE:
         case VOID:
            break;

         case FN_AP:
         case LAMBDA:
         case LET:
         case CASE:
            toBody(expr, AST_Node).genVoidReduced(failLab, *this);
            break;

         default:            
            genVoid(reduce(expr, failLab), failLab);
            break;
      }
}

// Code::genEnter: Generate code to enter an expression

Void Code::genEnter(Expr expr, TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab) {
   Bool altEntryFlag = fnAp == NULL || fnAp->nArgs() >= arity(fnAp->typeSig());

   Operand* goToOperand;

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genEnter: ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      if (formOf(expr) != DIRECT_LAMBDA) {
         printTranExpr(expr, FALSE, outStream, env(), TRUE);
         outStream << endl;
      }
      if (fnAp != NULL) {
         fnAp->print(outStream, env());
         outStream << endl;
      }
   }
#endif

   if (isPair(expr) && fst(expr) == Expr(REDUCE))
      genEnterReduced(snd(expr), typeSig, fnAp, failLab);
   else {
      switch (formOf(expr)) {
         case FAIL:
            addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(failLab));
            return;

         case EXCEPTION:
            addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(*_errLab));
            return;

         case DECL:
         case DEFN:
         case INSTANCE_METHOD_DEFN:
         case NAME_OCC:
            toBody(expr, AST_Node).genEnter(typeSig, fnAp, failLab, *this);
            return;

         case DIRECT_LAMBDA:
            toBody(expr, DirectLambda).genEnter(fnAp, *this);
            return;

         case CURRIED_LAMBDA:
            toBody(expr, CurriedLambda).genEnter(fnAp, *this);
            return;

         case VAR:
            toBody(expr, Var).genEnter(fnAp, *this);
            return;

         case VAR_REF:
            toBody(expr, VarRef).var().genEnter(fnAp, *this);
            return;

         case BUILT_IN_FN:
            goToOperand = new(msa()) BuiltInFnOperand(expr, FALSE, fnAp != NULL && !fnAp->isPartial());
            break;

         case RETURN_STATE:
            toBody(expr, ReturnState).genEnter(typeSig, fnAp, *this);
            return;

         case 0:  // ERROR
            return;

         default:
            error("Code::genEnter: unrecognised form");
            printTranExpr(expr, FALSE, outStream, env());
            outStream << endl;
            return;
      }
      addInstruction(new(msa()) EmptyArgsInstruction(fnAp == NULL || fnAp->isPartial()));
      if (fnAp != NULL)
         fnAp->copyArgs(*this);
      addInstruction(new(msa()) GoToInstruction(*goToOperand,
         altEntryFlag,
         _returnState != NULL
            ? _returnState->_contVar
            :

#if (CLOSURE_UPDATE== 1)
               _updateContVar, _updateCellVar
#else
               NULL
#endif
                    ));

   }
}

// Code::genEnterReduced: Generate code to enter a reduced expression

Void Code::genEnterReduced(Expr expr, TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab) {
   Bool altEntryFlag = fnAp == NULL || fnAp->nArgs() >= arity(fnAp->typeSig());

   Operand* goToOperand;

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genEnterReduced: ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
      if (fnAp != NULL) {
         outStream << "          ";
         printTranExpr(fnAp->typeSig(), FALSE, outStream, env());
         outStream << endl;
      }
   }
#endif

   if (isPair(expr) && fst(expr) == Expr(REDUCE))
      genEnterReduced(reduce(snd(expr), failLab), typeSig, fnAp, failLab);
   else {
      switch (formOf(expr)) {
         case DEFN:
         case NAME_OCC:
         case LAMBDA:
         case FN_AP:
         case LET:
         case CASE:
            toBody(expr, AST_Node).genEnterReduced(typeSig, fnAp, failLab, *this);
            return;

         case DIRECT_LAMBDA:
            toBody(expr, DirectLambda).genEnter(fnAp, *this);
            return;

         case CURRIED_LAMBDA:
            toBody(expr, CurriedLambda).genEnterReduced(typeSig, fnAp, failLab, *this);
            return;

         case VAR:
            toBody(expr, Var).genEnterReduced(fnAp, *this);
            return;

         case VAR_REF:
            toBody(expr, VarRef).var().genEnterReduced(fnAp, *this);
            return;

         case BUILT_IN_FN:
            goToOperand = new(msa()) BuiltInFnOperand(expr, FALSE, altEntryFlag);
            break;

         case BUILT_IN_DATA_CON:
            goToOperand = new(msa()) BuiltInDataConOperand(expr, altEntryFlag);
            addInstruction(new(msa()) EmptyArgsInstruction(fnAp == NULL || fnAp->isPartial()));
            if (fnAp != NULL)
               fnAp->copyArgs(*this);
            addInstruction(new(msa()) GoToInstruction(*goToOperand, altEntryFlag));
            return;

         case RETURN_STATE:
            toBody(expr, ReturnState).genEnterReduced(typeSig, fnAp, *this);
            return;

         default:
            error("Code::genEnterReduced: unrecognised form");
            printTranExpr(expr, FALSE, outStream, env());
            outStream << endl;
            return;
      }

// T.B.D. Investigate the following

      assert(FALSE, "Code::genEnterReduced: unexpected");
      addInstruction(new(msa()) EmptyArgsInstruction(fnAp == NULL || fnAp->isPartial()));
      if (fnAp != NULL)
         fnAp->copyArgs(*this);
//       addInstruction(new(msa()) GoToInstruction(*goToOperand, altEntryFlag, contVar));
   }
}

// Code::genReturn: Generate code to return from closure

Void Code::genReturn(Expr expr, TypeSig typeSig, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
		outStream << "Code::genReturn: ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env(), TRUE);
      outStream << endl;
   }
#endif

   if (isPair(expr))
      genPairReturn(expr, typeSig, failLab);
   else switch (formOf(expr)) {
      case FAIL:
         addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(failLab));
         break;

      case EXCEPTION:
         addUnaryInstruction(Instruction::GOTO_INSTR, labelOperand(*_errLab));
         break;

      case RETURN:
      case RETURN_STATE:
      case VOID:
         ret();
         break;

      case NAME_OCC:
         toBody(expr, NameOcc).genReturn(typeSig, failLab, *this);
         break;

      case NAME:
      case TYPE:
      case INT:
      case CHAR:
      case TAG:
      case REF:
      case DATA:
         genMoveConst(toBody(makeConst(expr), Const), newVar(typeSig, Var::RESULT_VAR));
         break;

      case CONST:
         genMoveConst(toBody(expr, Const), newVar(typeSig, Var::RESULT_VAR));
         ret();
         break;

      case VAR:
         toBody(expr, Var).genReturn(*this);
         break;

      case VAR_REF:
         toBody(expr, VarRef).var().genReturn(*this);
         break;

      case LAMBDA:
         toBody(expr, Lambda).genVar(newVar(typeSig, Var::RESULT_VAR), failLab, *this);
         ret();
         break;

      case FN_AP:
      case LET:
      case CASE:
         genExpr(expr, newVar(typeSig, Var::RESULT_VAR), failLab);
         ret();
         break;

      default:
         error("Code::genReturn: unexpected");
         printTranExpr(expr, FALSE, outStream, env());
         outStream << endl;
         break;
   }
}

// Code::genPairReturn: Closure return for pair form

Void Code::genPairReturn(Expr pair, TypeSig typeSig, CodeLabel& failLab) {
   Expr f = fst(pair);
   Expr s = snd(pair);
   switch (formOf(f)) {
      case LITERAL:
      case TYPE_SIG:
         genReturn(makeConst(pair), typeSig, failLab);
         break;

      case REDUCE:
         genReturnReduced(s, typeSig, failLab);
         break;

      default:
         error("Code::genReturnPair: unexpected");
         printTranExpr(pair, FALSE, outStream, env());
         outStream << endl;
   }
}

// Code::genReturnReduced: Generate code to return from closure with reduced expression

Void Code::genReturnReduced(Expr expr, TypeSig typeSig, CodeLabel& failLab) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Code::genReturnReduced: ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << ' ';
      printTranExpr(expr, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   if (isPair(expr)) {
      Expr f = fst(expr);
      Expr s = snd(expr);
      assert(f == Expr(REDUCE), "Code::genReturnReduced: expected REDUCE");
      genReturnReduced(reduce(s, failLab), typeSig, failLab);
   } else
      switch (formOf(expr)) {
         case LAMBDA:
         case NAME_OCC:
         case LET:
         case FN_AP:
         case CASE:
            toBody(expr, AST_Node).genReturnReduced(typeSig, failLab, *this);
            break;

         case VAR:
            toBody(expr, Var).genReturnReduced(*this);
            break;

         case VAR_REF:
            toBody(expr, VarRef).var().genReturnReduced(*this);
            break;

         default:
            genReturn(reduce(expr, failLab), typeSig, failLab);
            break;
      }
}
