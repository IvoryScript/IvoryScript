/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    class.cpp
 *
 * Module:  IvoryScript compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of IvoryScript 'class' translation and type checking
 *    functions.
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

#include "ivory/any.h"
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/trace.h"
#include "ivory/compiler/analyser.h"
#include "ivory/compiler/class.h"
#include "ivory/compiler/CopyContext.h"
#include "ivory/compiler/type.h"
#include "ivory/compiler/tran.h"

extern DataCon& dataConFromExpr(Expr dataCon);

#define TRACE

#define TRACE_CONSTRAINT_CHECKING             // Remove comment to trace constraint checking
//#define TRACE_CONSTRAINT_CHECKING_IN_DETAIL   // Remove comment to trace constraint checking in detail

#ifdef TRACE
static Bool traceFlag = FALSE;
#define toggle(flag) ((flag)=!(flag))
#endif

Void traceClassCompilation(Bool flag) {
   traceFlag = flag;
}

// equalTypeSigs: Match two type signatures
//                given a set of substitutions

static TypePredicate equalTypeSigs(TypeSig typeSig1, TypeSig typeSig2, const Subst* substs) {
   TypePredicate isAp1 = TYPE_PRED_UNDECIDABLE;
   TypePredicate isAp2 = TYPE_PRED_UNDECIDABLE;
   do {
      if (isAp1 == TYPE_PRED_UNDECIDABLE)
         isAp1 = isAp(typeSig1) ? TYPE_PRED_TRUE : TYPE_PRED_FALSE;
      if (isAp2 == TYPE_PRED_UNDECIDABLE)
         isAp2 = isAp(typeSig2) ? TYPE_PRED_TRUE : TYPE_PRED_FALSE;

      if (isAp1 == TYPE_PRED_TRUE) {
         if (isAp2 == TYPE_PRED_TRUE)
            switch (equalTypeSigs(fun(typeSig1), fun(typeSig2), substs)) {
               case TYPE_PRED_FALSE: return TYPE_PRED_FALSE;
               case TYPE_PRED_TRUE: return equalTypeSigs(arg(typeSig1), arg(typeSig2), substs);
               case TYPE_PRED_UNDECIDABLE:
                  return equalTypeSigs(arg(typeSig1), arg(typeSig2), substs) == TYPE_PRED_FALSE
                     ? TYPE_PRED_FALSE
                     : TYPE_PRED_UNDECIDABLE;
            }
         else {
            if (!isTypeVar(typeSig2))
               return TYPE_PRED_FALSE;
            const Subst* subst;
            if ((subst = Subst::lookUp(toTypeVar(typeSig2), substs)) == NULL)
               return TYPE_PRED_UNDECIDABLE;
            typeSig2 = subst->typeSig();
            isAp2 = TYPE_PRED_UNDECIDABLE;
         }
      } else if (isAp2 == TYPE_PRED_TRUE) {
         if (!isTypeVar(typeSig1))
            return TYPE_PRED_FALSE;
         const Subst* subst;
         if ((subst = Subst::lookUp(toTypeVar(typeSig1), substs)) == NULL)
            return TYPE_PRED_UNDECIDABLE;
         typeSig1 = subst->typeSig();
         isAp1 = TYPE_PRED_UNDECIDABLE;
      } else {
         if (typeSig1 == typeSig2)
            return TYPE_PRED_TRUE;
         if (isTypeVar(typeSig1)) {
            const Subst* subst;
            if ((subst = Subst::lookUp(toTypeVar(typeSig1), substs)) == NULL)
               return TYPE_PRED_UNDECIDABLE;
            typeSig1 = subst->typeSig();
            isAp1 = TYPE_PRED_UNDECIDABLE;
         } else
            if (isTypeVar(typeSig2)) {
               const Subst* subst;
               if ((subst = Subst::lookUp(toTypeVar(typeSig2), substs)) == NULL)
                  return TYPE_PRED_UNDECIDABLE;
               typeSig2 = subst->typeSig();
               isAp2 = TYPE_PRED_UNDECIDABLE;
            } else
               return TYPE_PRED_FALSE;
      }
   } while (TRUE);
}

static const ClassDefn* classMethodClassDefn(TypedVal* typedVal) {
   switch (formOf(typedVal->val())) {
      case CLASS_METHOD_DECL:
         return &toBody(typedVal->val(), ClassMethodDecl).classDefn();

      case CLASS_METHOD_DEFN:
         return &toBody(typedVal->val(), ClassMethodDefn).classDefn_();

      default:
         return NULL;
   }
}

static ClassDefn* lookUpClassFrom(Name name, const ModuleDefn* moduleDefn,
                                  const ModuleDefn*& moduleDefnRes,
                                  Tran& tran) {
   moduleDefnRes = NULL;
   if (moduleDefn != NULL) {
      ClassDefn* classDefn = moduleDefn->lookUpClass(name, moduleDefn->env());
      if (classDefn != NULL) {
         moduleDefnRes = moduleDefn;
         return classDefn;
      }

      ConstString nameStr = nameString_(name, moduleDefn->nameTable());
      if (nameStr == NULL)
         return NULL;
      name = tran.nameTable().lookUp(nameStr);
      if (name == NULL_NAME)
         return NULL;
   }
   return tran.lookUpClass(name, moduleDefnRes);
}

static Bool isSuperClassOf(const ClassDefn& classDefn,
                           const ModuleDefn* moduleDefn,
                           const ClassDefn& superClassDefn,
                           Tran& tran) {
   for (Expr superClasses = classDefn.superClasses();
        superClasses != Nil;
        superClasses = tl(superClasses)) {
      Expr superClass = hd(superClasses);
      const ModuleDefn* directSuperClassModuleDefn = NULL;
      ClassDefn* directSuperClassDefn =
         lookUpClassFrom(toName(fst(superClass)),
                         moduleDefn,
                         directSuperClassModuleDefn,
                         tran);

      if (directSuperClassDefn == &superClassDefn ||
          directSuperClassDefn != NULL &&
          isSuperClassOf(*directSuperClassDefn,
                         directSuperClassModuleDefn,
                         superClassDefn,
                         tran))
         return TRUE;
   }
   return FALSE;
}

static Bool isClassMethodOf(TypedVal* typedVal,
                            const ClassDefn& classDefn,
                            const ModuleDefn* moduleDefn,
                            Tran& tran) {
   const ClassDefn* methodClassDefn = classMethodClassDefn(typedVal);
   return methodClassDefn != NULL &&
          methodClassDefn == &classDefn ||
          isSuperClassOf(classDefn, moduleDefn, *methodClassDefn, tran);
}

static Bool isInstanceMethodOf(TypedVal* typedVal, const InstanceDefn& instanceDefn) {
   switch (formOf(typedVal->val())) {
      case INSTANCE_METHOD_DECL:
         return &toBody(typedVal->val(), InstanceMethodDecl)._instanceDefn == &instanceDefn;

      case INSTANCE_METHOD_DEFN:
         return &toBody(typedVal->val(), InstanceMethodDefn).classDefn_() == &instanceDefn;

      default:
         return FALSE;
   }
}

ClassDefn* Tran::lookUpClass(Name name, const ModuleDefn*& moduleDefnRes) {
   ClassDefn* classDefn;
   if (_moduleDefn != NULL) {
      classDefn = _moduleDefn->lookUpClass(name, _moduleDefn->env());
      if (classDefn != NULL) {
         moduleDefnRes = NULL;
         return classDefn;
      }
   }
   ModuleDefn* extModuleDefn = ModuleDefn::workingSet;
   while (extModuleDefn != NULL) {
      ClassDefn* classDefn = extModuleDefn->lookUpClass(name, *_env);
      if (classDefn != NULL) {
         moduleDefnRes = extModuleDefn;
         return classDefn;
      }
      extModuleDefn = extModuleDefn->next();
   }
   ivoryError1("No class definition for %s", nameString_(name, env().nameTable()));
   return NULL;
}

// ModuleDefn::addClassDefn: Add class definition to list

Void ModuleDefn::addClassDefn(ClassDefn& classDefn) {

   // Append to class definition list

   ClassDefn** classDefnPtr = &_classDefns;
   while ((*classDefnPtr) != NULL)
      classDefnPtr = &(*classDefnPtr)->_next;
   *classDefnPtr = &classDefn;
}

// Return pointer to class definition for given class name.

ClassDefn* ModuleDefn::lookUpClass(Name name, const Env& env) const {
   if (!(isBuiltInName(name) || &env == _env)) {
      const char* str = nameString_(name, env.nameTable());
      if (str == NULL || (name = _env->nameTable().lookUp(str)) == NULL_NAME)
         return NULL;
      name += builtInNameCount;
   }

   for (ClassDefn* classDefn = _classDefns; classDefn != NULL; classDefn = classDefn->_next)
      if (classDefn->name() == name)
         return classDefn;
   return NULL;
}

// ModuleDefn::addInstanceDefn: Add instance definition to list

Void ModuleDefn::addInstanceDefn(InstanceDefn& instanceDefn) {
   Name className = instanceDefn.className();

   ClassInstancesMapEntry* mapEntry = _classInstancesMap.lookUp(className, className);
   if (mapEntry == NULL) {
      instanceDefn._next = NULL;
      ClassInstancesMapEntry mapEntry(className, &instanceDefn);
      _classInstancesMap.add(mapEntry, className, _env->msa());
   } else {

//--- Append to class definition list

      InstanceDefn** instanceDefnPtr = &(mapEntry->_instanceDefns);
      while ((*instanceDefnPtr) != NULL)
         instanceDefnPtr = &(*instanceDefnPtr)->_next;
      *instanceDefnPtr = &instanceDefn;
   }
} 

// Return pointer to instance definitions for given class name.

InstanceDefn* ModuleDefn::lookUpInstances(Name className, const Env& env) const {
   if (!(isBuiltInName(className) || &env == _env)) {
      const char* str = nameString_(className, env.nameTable());
      if (str == NULL || (className = _env->nameTable().lookUp(str)) == NULL_NAME)
         return NULL;
      className += builtInNameCount;
   }
   ClassInstancesMapEntry* mapEntry = _classInstancesMap.lookUp(className, className);

   return mapEntry != NULL ? mapEntry->_instanceDefns : NULL;
}

/*----------------------------------------------------------------------------*/

// ClassDefn

ClassDefn::ClassDefn(Expr superClasses, Name name, Expr typeSigs, Expr declOrDefns)
   : _name(name),
     _superClasses(superClasses),
     _typeSigs(typeSigs),
     _declOrDefns(declOrDefns),
	  _next(NULL) {
}

Void ClassDefn::tran(Tran& tran) {
   mapProc1(tranDeclOrDefn, _declOrDefns, tran);
}

Void ClassDefn::tranDeclOrDefn(Expr& declOrDefn, Tran& tran) {
	Bool isInline = FALSE;

	if (isPair(declOrDefn) && fst(declOrDefn) == Expr(INLINE)) {
		isInline = TRUE;
		declOrDefn = snd(declOrDefn);
	}

	if (isPair(declOrDefn)) {
		assert(formOf(fst(declOrDefn)) == DECL, "ClassDefn::tranDeclOrDefn: expected 'DECL'");
      Cell& cell_ = *new(sizeof(ClassMethodDecl), tran.msa()) Cell(CLASS_METHOD_DECL);
      new(cell_.body()) ClassMethodDecl(*this);
		declOrDefn = tran.nameTypedValBinding(snd(declOrDefn), fromCell(cell_));
	} else if (isTriple(declOrDefn)) {
      if (formOf(fst3(declOrDefn)) == DEFN) {
         Cell& cell_ = *new(sizeof(ClassMethodDefn), tran.msa()) Cell(CLASS_METHOD_DEFN);
         new(cell_.body()) ClassMethodDefn(thd3(declOrDefn), *this, isInline);
         declOrDefn = tran.nameTypedValBinding(snd3(declOrDefn), fromCell(cell_));
      } else if (formOf(fst3(declOrDefn)) == OBJECT_DEFN) {
         Cell& cell_ = *new(sizeof(ClassObjectDefn), tran.msa()) Cell(CLASS_OBJECT_DEFN);
         new(cell_.body()) ClassObjectDefn(toName(snd3(declOrDefn)), thd3(declOrDefn), *this, tran);
         cellBody(cell_, ClassObjectDefn).postConstruct(tran);
         declOrDefn = fromCell(cell_);
         cellBody(cell_, ClassObjectDefn).tran(declOrDefn, tran);
      } else
          error("ClassDefn::tranDeclOrDefn: expected 'DEFN'");
	}
}

// ClassDefn::addBinding: Add binding to environment

Void ClassDefn::addBinding(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran) {
   if (formOf(declOrDefn) == NAME_TYPED_VAL_BINDING) {
      NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
      TypedVal* typedVal = bindingEnv.lookUp(binding.name(), FALSE);

#ifdef TRACE
      if (traceFlag) {
         outStream << "ClassDefn::addBinding: ";
         binding.print(FALSE, outStream, tran.env());
         outStream << '\n';
         outStream.flush();
      }
#endif

      if (typedVal != NULL) {
         Bool inconsistent = FALSE;
         switch (formOf(binding.val().val())) {
            case CLASS_METHOD_DECL:
               switch (formOf(typedVal->val())) {
                  case CLASS_METHOD_DECL:
                     toBody(binding.val().val(), ClassMethodDecl)._prevDeclOrDefn = typedVal;
                     break;

                  case CLASS_METHOD_DEFN:
                     toBody(binding.val().val(), ClassMethodDecl)._prevDeclOrDefn = typedVal;
                     break;
                  default:
                     inconsistent = TRUE;
                     break;
               }
               break;

            case CLASS_METHOD_DEFN:
               switch (formOf(typedVal->val())) {
                  case CLASS_METHOD_DECL:
                     toBody(binding.val().val(), ClassMethodDefn)._prevDeclOrDefn = typedVal;
                     return;
                  case CLASS_METHOD_DEFN: {
                     Char buf[256];
                     sprintf(buf, "ClassDefn::addBinding: Duplicate definition of method %s",
                        binding.nameString(tran.env()));
                     error(buf);
                     return;
                  }
                  default:
                     inconsistent = TRUE;
                     break;
               }
               break;

            default:
               inconsistent = TRUE;
               break;
         }

         if (inconsistent) {
            Char buf[256];
            sprintf(buf, "ClassDefn::addBinding: Inconsistent with previous declaration or definition of %s",
               binding.nameString(tran.env()));
            error(buf);
            return;
         }
      }

      bindingEnv.add(binding);
   } else {
      assert(formOf(declOrDefn) == CLASS_OBJECT_DEFN, "Tran::addBinding: expected object definition");
      toBody(declOrDefn, ClassObjectDefn).addBindings(bindingEnv, tran);
   }
}

// ClassDefn::addBindings: Add bindings to environment

Void ClassDefn::addBindings(BindingSet& bindingEnv, Tran& tran) {
   mapProc2(addBinding, _declOrDefns, bindingEnv, tran);
}

// ClassDefn::tranDeclOrDefn: Translate class declaration or definition

Void ClassDefn::tranDeclOrDefn(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran) {
   if (formOf(declOrDefn) != CLASS_OBJECT_DEFN)
      tran.tranDeclOrDefn(declOrDefn, bindingEnv);
   else
      toBody(declOrDefn, ClassObjectDefn).tran(declOrDefn, bindingEnv, tran);
}

// ClassDefn::tranDeclOrDefn: Translate

Void ClassDefn::tran(BindingSet& bindingEnv, Tran& tran) {
	if (tran.pass() > 1)
		Subst::substTypeSigList(_typeSigs, tran.substs());
   mapProc2(tranDeclOrDefn, _declOrDefns, bindingEnv, tran);
}

// ClassDefn::insTypeSigs: Instantiate type signatures

Void ClassDefn::insTypeSigs(TypeCheck& typeCheck) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Class - type signatures for: ";
      printName(name(), outStream, typeCheck.nameTable());
      outStream << '\n';
   }
#endif

   mapProc(typeCheck.bindTypeConNames, _typeSigs); 
   mapProc1(insDeclOrDefnTypeSig, _declOrDefns, typeCheck);
}

// ClassDefn::insDeclOrDefnTypeSig: Instantiate a class method declaration or definition type signature

Void ClassDefn::insDeclOrDefnTypeSig(Expr declOrDefn, TypeCheck& typeCheck) {
   if (formOf(declOrDefn) == NAME_TYPED_VAL_BINDING) {
      NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
      switch (formOf(binding.typedVal().val())) {
         case CLASS_METHOD_DECL:
         case CLASS_METHOD_DEFN:
            binding.insTypeSig(NULL, typeCheck);
            break;

         default:
            error("ClassDefn::insDeclOrDefnTypeSig: expected method declaration or definition");
            break;
      }
   } else {
      if (formOf(declOrDefn) == CLASS_OBJECT_DEFN)
         toBody(declOrDefn, ClassObjectDefn).insTypeSigs(typeCheck);
      else
         assert(FALSE, "ClassDefn::insDeclOrDefnTypeSig: unexpected form");
   }
}

// ClassDefn::typeCheck: Type check list of declarations or definitions

Void ClassDefn::typeCheckDeclOrDefn(Expr declOrDefn, TypeEnv* typeEnv,
   TypeCheck& typeCheck) {
   if (formOf(declOrDefn) != CLASS_OBJECT_DEFN)
      typeCheck.typeCheckDeclOrDefn(declOrDefn, typeEnv, ERROR);
   else
      toBody(declOrDefn, ClassObjectDefn).typeCheck(typeCheck);
}

Void ClassDefn::typeCheck(TypeCheck& typeCheck) {
   mapProc2(typeCheckDeclOrDefn, _declOrDefns, NULL, typeCheck);
}

// ClassDefn::resetNeedsInstance: Reset needs instance flag for class method desclarations and definitions

Void ClassDefn::resetDeclOrDefnNeedsInstance(Expr declOrDefn, TypeCheck& typeCheck) {
   if (formOf(declOrDefn) != CLASS_OBJECT_DEFN)
      typeCheck.resetDeclOrDefnNeedsInstance(declOrDefn, NULL);
   else
      toBody(declOrDefn, ClassObjectDefn).resetNeedsInstance(typeCheck);
}

Void ClassDefn::resetNeedsInstance(TypeCheck& typeCheck) {
   mapProc1(resetDeclOrDefnNeedsInstance, _declOrDefns, typeCheck);
}

// ClassDefn::method: return defined value for method or NULL.

TypedVal* ClassDefn::method(Name name) const {
   for (Expr declOrDefns = _declOrDefns; declOrDefns != Nil; declOrDefns = tl(declOrDefns)) {
      Expr declOrDefn = hd(declOrDefns);
		assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "ClassDefn::method: Expected binding");
		NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
		if (formOf(binding.typedVal().val()) == CLASS_METHOD_DEFN &&
         binding.id() == name)
			return &binding.typedVal();
   }

// T.B.D. search superclasses

	return NULL;
}

// ClassDefn::analyse:  Analyse class definition

// ClassDefn::analyse:  Analyse class definition

Void ClassDefn::preAnalyseDeclOrDefn(Expr declOrDefn, Analyser& analyser) {
   if (formOf(declOrDefn) != CLASS_OBJECT_DEFN)
      analyser.preAnalyseDeclOrDefn(declOrDefn);
}

Void ClassDefn::preAnalyse(Analyser& analyser) {
   mapProc1(preAnalyseDeclOrDefn, _declOrDefns, analyser);
}


Void ClassDefn::analyseDeclOrDefn(Expr declOrDefn, Analyser& analyser) {
   if (formOf(declOrDefn) != CLASS_OBJECT_DEFN)
      analyser.analyseDeclOrDefn(declOrDefn);
}

Void ClassDefn::analyse(Analyser& analyser) {
   mapProc1(analyseDeclOrDefn, _declOrDefns, analyser);
}

// ClassDefn::reserve: Generate reserved code for a class definition

Void ClassDefn::reserveDeclOrDefn(Expr declOrDefn, Code& code) {
   if (formOf(declOrDefn) != CLASS_OBJECT_DEFN)
      code.reserveDeclOrDefn(declOrDefn);
}

Void ClassDefn::reserve(Code& code) {
   mapProc1(reserveDeclOrDefn, _declOrDefns, code);
}

// ClassDefn::gen:  Generate code for a class definition

Void ClassDefn::genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab, Code& code) {
   if (formOf(declOrDefn) != CLASS_OBJECT_DEFN)
      code.genDeclOrDefn(declOrDefn, errLab);
}

Void ClassDefn::gen(CodeLabel& errLab, Code& code) {
   mapProc2(genDeclOrDefn, _declOrDefns, errLab, code);
}

Void ClassDefn::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/) const {
   os << "CLASS,";
   printName(name(), os, env.nameTable());
   os << ',';
   printTypeSigList(_typeSigs, os, env);
   os << ',';
   printTranList(_declOrDefns, FALSE, os, env, pretty, 1, pretty);
}

/*----------------------------------------------------------------------------*/

ClassDefnRef::ClassDefnRef(Name name, Tran& tran)
 : _className(name) {
   _classDefn = tran.lookUpClass(name, _moduleDefn);
}

/*----------------------------------------------------------------------------*/


ClassMethodDecl::ClassMethodDecl(const ClassDefn& classDefn)
   : Decl(TRUE), _classDefn(classDefn) {
}

Void ClassMethodDecl::insTypeSig(TypedVal& typedVal, TypeCheck& typeCheck) {
   typeCheck.bindTypeConNames(typedVal.typeSig());
   typeVarsInTypeSig(typedVal.typeSig(), typedVal.schematicTypeVars(), typeCheck.msa());
}

Void ClassMethodDecl::print(ostream& os, const Env& env,
                            Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "CLASS_METHOD_DECL(";
   os << nameString_(_classDefn.name(), env.nameTable());
   os << ')';
}

/*----------------------------------------------------------------------------*/

// ClassMethodDefn: Class method definition

ClassMethodDefn::ClassMethodDefn(Expr expr, const ClassDefn& classDefn, Bool isInline)
   : Defn(expr, TRUE, FALSE, isInline, FALSE), _classDefn(classDefn) {
   _unifyWithPrevDeclOrDefn = FALSE;   // 
}

TypedVal* ClassMethodDefn::lookUp(Name name, const Subst* substs, const Tran& tran) const {

// Look up occurrence unique type variable

   TypeVar tv = Subst::lookUp(hd(typeSigs()), substs);

// Get substitution for type

   const Subst* subst;
   if (tv == NULL_NAME || (subst = tran.substs().lookUp(tv)) == NULL) {
      error("ClassMethod::lookUp: no substitution found");
      return NULL;
   }
//	const InstanceDefn* instanceDefn = _classDefn->instanceDefn(useType(subst->typeSig(),
//                                                           tran.typeTable()));

	const InstanceDefn* instanceDefn = NULL;
   if (instanceDefn != NULL)
      return instanceDefn->method(name);
   else {
      outStream << "Type: ";
      printTranExpr(subst->typeSig(), FALSE, outStream, tran.env());
      outStream << " no instance for method: ";
      print(outStream, tran.env());
      outStream << endl;
      return NULL;
   }
}

Void ClassMethodDefn::gen(Expr& expr, Name name, TypeSig typeSig, CodeLabel& errLab, Code& code) {
}

/*----------------------------------------------------------------------------*/

// ClassObjectDefn: Class object definition

ClassObjectDefn::ClassObjectDefn(Name name, Expr dataConDecl,
                                 const ClassDefn& classDefn, Tran& tran)
   : TypeDefn(name, Nil, cons(dataConDecl, Nil, tran.msa()), tran, TRUE),
     _classDefn(classDefn) {
}


Expr ClassObjectDefn::allocDataCon(Name name, Tag tag, TypeSig typeSig, MSA& msa) {
   Cell& cell_ = *new(sizeof(ClassDataCon), msa) Cell(CLASS_DATA_CON);

   new(cell_.body()) ClassDataCon(name, tag, typeSig, _classDefn);
   return  fromCell(cell_);
}

Void ClassObjectDefn::insDataConTypeSig(Expr dataCon, TypeCheck& typeCheck) {
   NameTypedValBinding& binding = toBody(dataCon, NameTypedValBinding);
   TypedVal& typedVal = binding.typedVal();

   typeCheck.bindTypeConNames(typedVal.typeSig());
   typeVarsInTypeSig(typedVal.typeSig(), typedVal.schematicTypeVars(), typeCheck.msa());
}

Void ClassObjectDefn::insTypeSigs(TypeCheck& typeCheck) {
   mapProc1(insDataConTypeSig, _dataConDecls, typeCheck);
}

/*----------------------------------------------------------------------------*/

ClassDataCon::ClassDataCon(Name name, Tag tag, TypeSig typeSig,
                                       const ClassDefn& classDefn)
   : DataCon(name, tag, typeSig, TypeSig(UNKNOWN)), _classDefn(classDefn) {
}

/*----------------------------------------------------------------------------*/

// T.B.D. at present the syntax for an instance is a list of declarations
// or definitions.  This might be restricted to definitions.

InstanceDefn::InstanceDefn(Name className, Bool subordinate,
                           Expr typeSigs, Expr typeQuals, Expr declOrDefns)
   : _className(className), _subordinate(subordinate),
	  _typeSigs(typeSigs), _typeQuals(typeQuals), _declOrDefns(declOrDefns),
     _classDefn(NULL), _moduleDefn(NULL),
     _next(NULL) {
}

Void InstanceDefn::tran(Tran& tran) {
   _classDefn = tran.lookUpClass(_className, _moduleDefn);
   mapProc1(tranTypeQual, _typeQuals, tran);
   mapProc1(tranDeclOrDefn, _declOrDefns, tran);
}

Void InstanceDefn::tranTypeQual(Expr& typeQual, Tran& tran) {
   if (isPair(typeQual) && fst(typeQual) == (Expr)NEGATION)
      tranTypeQual(snd(typeQual), tran);
   else if (isTriple(typeQual)) {
      if (fst3(typeQual) == (Expr)INSTANCE) {
         Cell& cell_ = *new(sizeof(ClassDefnRef), tran.msa()) Cell(CLASS_DEFN_REF);
         new(cell_.body()) ClassDefnRef(toName(snd3(typeQual)), tran);
         snd3(typeQual) = fromCell(cell_);
      } else {
         tranTypeQual(snd3(typeQual), tran);
         tranTypeQual(thd3(typeQual), tran);
      }
   }
}

Void InstanceDefn::tranDeclOrDefn(Expr& declOrDefn, Tran& tran) {
   Bool isInline = FALSE;

   if (isPair(declOrDefn) && fst(declOrDefn) == Expr(INLINE)) {
      isInline = TRUE;
      declOrDefn = snd(declOrDefn);
   }

   if (isPair(declOrDefn)) {
      assert(formOf(fst(declOrDefn)) == DECL, "InstanceDefn::tranDeclOrDefn: expected 'DECL'");
      Cell& cell_ = *new(sizeof(InstanceMethodDecl), tran.msa()) Cell(INSTANCE_METHOD_DECL);
      new(cell_.body()) InstanceMethodDecl(*this);
      declOrDefn = tran.nameTypedValBinding(snd(declOrDefn), fromCell(cell_));
      return;
   }
   else if (isTriple(declOrDefn)) {
      if (formOf(fst(declOrDefn)) == DEFN) {
         Cell& cell_ = *new(sizeof(InstanceMethodDefn), tran.msa()) Cell(INSTANCE_METHOD_DEFN);
         new(cell_.body()) InstanceMethodDefn(thd3(declOrDefn), *this, isInline);
         declOrDefn = tran.nameTypedValBinding(snd3(declOrDefn), fromCell(cell_));
         return;
      } else {
         switch (formOf(fst(declOrDefn))) {
            case DATA_CON_DECL: {
               Cell& cell_ = *new(sizeof(InstanceDataConDecl), tran.msa()) Cell(INSTANCE_DATA_CON_DECL);
               new(cell_.body()) DataConDecl(toName(snd3(declOrDefn)), thd3(declOrDefn), tran);
               declOrDefn = fromCell(cell_);
               ((DataConDecl&)cellBody(cell_, InstanceDataConDecl)).tran(tran);
               return;
            }

            case DATA_CON_DEFN: {
               Cell& cell_ = *new(sizeof(InstanceDataConDefn), tran.msa()) Cell(INSTANCE_DATA_CON_DEFN);
               new(cell_.body()) InstanceDataConDefn(snd3(declOrDefn), thd3(declOrDefn),
                  FALSE, *this, isInline);
               declOrDefn = fromCell(cell_);
               ((DataConDefn&)cellBody(cell_, InstanceDataConDefn)).tran(tran);
               return;
            }

            default:
               break;
         }
      } 
   }
   assert(FALSE, "InstanceDefn::tranDeclOrDefn: unexpected");
}

Void InstanceDefn::addBinding(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran) {
   if (formOf(declOrDefn) == NAME_TYPED_VAL_BINDING) {
      NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
      TypedVal* typedVal;
      const ModuleDefn* moduleDefn = NULL;
      Name mappedName;
      if ((typedVal = bindingEnv.lookUp(binding.name(), FALSE)) == NULL &&
         (typedVal = tran.lookUpExtBinding(binding.name(), FALSE, moduleDefn, mappedName)) == NULL &&
         (typedVal = tran.lookUpBuiltInBinding(binding.name())) == NULL) {
         char s[255];
         sprintf(s, "\'%s\' not class method",
            nameString_(binding.name(), tran.nameTable()));
         tran.error(s);
         return;
      }
      if (_classDefn == NULL ||
          (!isClassMethodOf(typedVal, *_classDefn, _moduleDefn, tran) &&
           !isInstanceMethodOf(typedVal, *this))) {
         char s[255];
         sprintf(s, "\'%s\' not method of class \'%s\'",
                 nameString_(binding.name(), tran.nameTable()),
                 nameString_(_className, tran.nameTable()));
         tran.error(s);
         return;
      }
      Bool inconsistent = FALSE;
      switch (formOf(binding.val().val())) {
         case INSTANCE_METHOD_DECL:
            switch (formOf(typedVal->val())) {
               case CLASS_METHOD_DECL:
               case CLASS_METHOD_DEFN:
               case INSTANCE_METHOD_DECL:
                  toBody(binding.val().val(), InstanceMethodDecl)._prevDeclOrDefn = typedVal;
                  toBody(binding.val().val(), InstanceMethodDecl)._moduleDefn = moduleDefn;
                  break;

               default:
                  inconsistent = TRUE;
                  break;
            }
            break;

         case INSTANCE_METHOD_DEFN:
            switch (formOf(typedVal->val())) {
               case CLASS_METHOD_DECL:
               case CLASS_METHOD_DEFN:
               case INSTANCE_METHOD_DECL:
                  toBody(binding.val().val(), InstanceMethodDefn)._prevDeclOrDefn = typedVal;
                  toBody(binding.val().val(), InstanceMethodDefn)._moduleDefn = moduleDefn;
                  break;

               case INSTANCE_METHOD_DEFN: {
                  Char buf[256];
                  sprintf(buf, "InstanceDefn::tranDeclOrDefn: Duplicate definition of method %s",
                     binding.nameString(tran.env()));
                  error(buf);
                  return;
               }
               default:
                  inconsistent = TRUE;
                  break;
            }
            break;

         default:
            inconsistent = TRUE;
            break;
      }
      if (inconsistent) {
         Char buf[256];

         sprintf(buf, "InstanceDefn::tranDeclOrDefn: Inconsistent with previous declaration or definition of %s",
            binding.nameString(tran.env()));
         error(buf);
         return;
      }
      bindingEnv.add(binding);
   } else
      if (formOf(declOrDefn) == INSTANCE_DATA_CON_DECL)
         toBody(declOrDefn, InstanceDataConDecl).addBinding(bindingEnv, tran);
      else        
         toBody(declOrDefn, InstanceDataConDefn).addBindings(declOrDefn, bindingEnv, tran);
}

Void InstanceDefn::tran(BindingSet& bindingEnv, Tran& tran) {
   BindingSet localBindingEnv(&bindingEnv);
   mapProc2(addBinding, _declOrDefns, localBindingEnv, tran);
   if (tran.pass() > 1)
      Subst::substTypeSigList(_typeSigs, tran.substs());

// Note that any occurrence of the LHS name is bound to the class method, not the instance

   mapProc2(tranDeclOrDefn, _declOrDefns, bindingEnv, tran);
}

Void InstanceDefn::tranDeclOrDefn(Expr& declOrDefn, BindingSet& bindingEnv, Tran& tran) {
   switch (formOf(declOrDefn)) {
      case INSTANCE_DATA_CON_DECL:
         ((DataConDecl&)toBody(declOrDefn, InstanceDataConDecl)).tran(bindingEnv, tran);
         break;

      case INSTANCE_DATA_CON_DEFN:
         toBody(declOrDefn, InstanceDataConDefn).tran(bindingEnv, tran);
         break;

      default:
         tran.tranDeclOrDefn(declOrDefn, bindingEnv);
         break;
   }
}

// InstanceDefn::insTypeSigs: Instantiate type signatures

Void InstanceDefn::insDeclOrDefnTypeSig(Expr declOrDefn, TypeCheck& typeCheck) {
    switch (formOf(declOrDefn)) {
      case INSTANCE_DATA_CON_DECL:
         toBody(declOrDefn, InstanceDataConDecl).insTypeSig(typeCheck);
         break;

      case INSTANCE_DATA_CON_DEFN:
         toBody(declOrDefn, InstanceDataConDefn).insTypeSigs(typeCheck);
         break;

      default:
         typeCheck.insDeclOrDefnTypeSig(declOrDefn, NULL);
         break;
   }
}

Void InstanceDefn::insTypeQualTypeSigs(Expr typeQual, TypeCheck& typeCheck) {
   if (isPair(typeQual)) {
      if (fst(typeQual) == (Expr)STRICT)
         typeCheck.bindTypeConNames(snd(typeQual));
      else if (fst(typeQual) == (Expr)NEGATION)
         insTypeQualTypeSigs(snd(typeQual), typeCheck);
   } else if (isTriple(typeQual)) {
      Expr f = fst3(typeQual);
      if (f == (Expr)INSTANCE)
         typeCheck.bindTypeConNamesInList(thd3(typeQual));
      else if (f == (Expr)EQUALITY ||
               f == (Expr)INEQUALITY) {
         typeCheck.bindTypeConNames(snd3(typeQual));
         typeCheck.bindTypeConNames(thd3(typeQual));
      } else if (f == (Expr)CONJUNCTION ||
                 f == (Expr)DISJUNCTION) {
         insTypeQualTypeSigs(snd3(typeQual), typeCheck);
         insTypeQualTypeSigs(thd3(typeQual), typeCheck);
      }
   }
}

Void InstanceDefn::insTypeSigs(TypeCheck& typeCheck) {
   mapProc(typeCheck.bindTypeConNames, _typeSigs);

   Expr typeQuals = _typeQuals;
   while (typeQuals != Nil) {
      Expr typeQual = hd(typeQuals);
      insTypeQualTypeSigs(typeQual, typeCheck);
      typeQuals = tl(typeQuals);
   };

   mapProc1(insDeclOrDefnTypeSig, _declOrDefns, typeCheck);
}


#ifdef localConst
#undef localConst
#endif
#define localConst const

Subst* InstanceDefn::substs(TypeCheck& typeCheck) const {
   TypeEnv typeEnv(NULL);
   Expr classTypeSigs = _classDefn->typeSigs();
   Expr typeSigs = _typeSigs;
   while (classTypeSigs != Nil) {
      TypeSig classTypeSig = hd(classTypeSigs);
      if (_moduleDefn == NULL)
         classTypeSig = mutableTypeSig(classTypeSig, typeCheck.msa());
      else {
         CopyContext cc(_moduleDefn, typeCheck.nameTable(),
            typeCheck.predicates().varNameMaps(),
            typeCheck.predicates().typeVarMaps(),
            typeCheck.msa());
         classTypeSig = mutableTypeSig(classTypeSig, typeCheck.msa());
      }
       ::insTypeSig(classTypeSig, &typeEnv, NULL, typeCheck.msa());
       TypeSig typeSig = mutableTypeSig(hd(typeSigs), typeCheck.msa());
       ::insTypeSig(typeSig, &typeEnv, NULL, typeCheck.msa());
      Subst* substs = NULL;
      typeCheck.unify(classTypeSig, typeSig, NULL, &substs, TRUE, TRUE);
      Subst::substSubsts(typeEnv.substs(), substs);
      classTypeSigs = tl(classTypeSigs);
      typeSigs = tl(typeSigs);
   }
   return typeEnv.substs();
}

#undef localConst
#define localConst

// InstanceDefn::typeCheck: Type check list of declarations or definitions

Void InstanceDefn::typeCheckDeclOrDefn(Expr declOrDefn, TypeCheck& typeCheck) {
   switch (formOf(declOrDefn)) {
      case INSTANCE_DATA_CON_DECL:
         break;

      case INSTANCE_DATA_CON_DEFN:
         toBody(declOrDefn, InstanceDataConDefn).typeCheck(typeCheck);
         break;

      default:
         typeCheck.typeCheckDeclOrDefn(declOrDefn, NULL, ERROR);
         break;
   }
}

Void InstanceDefn::typeCheck(TypeCheck& typeCheck) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Instance type check: ";
      print(outStream, typeCheck.env(), false);
      outStream << '\n';
   }
#endif

   mapProc1(typeCheckDeclOrDefn, _declOrDefns, typeCheck);
}

// InstanceDefn::resetNeedsInstance: Reset needs instance flag for a list of method declarations and definitions

Void InstanceDefn::resetNeedsInstance(TypeCheck& typeCheck, TypeEnv* typeEnv) {
   typeCheck.resetDeclOrDefnsNeedsInstance(_declOrDefns, typeEnv);
}

// InstanceDefn::method: return defined value for method or NULL.

TypedVal* InstanceDefn::method(Name name) const {
   for (Expr declOrDefns = _declOrDefns; declOrDefns != Nil; declOrDefns = tl(declOrDefns)) {
		Expr declOrDefn = hd(declOrDefns);
      switch (formOf(declOrDefn)) {
         case NAME_TYPED_VAL_BINDING: {
            NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
            UInt32 forms[] = { INSTANCE_METHOD_DEFN, VAR };
            if (binding.id() == name && formExistsIn(binding.typedVal().val(), 2, forms))
               return &binding.typedVal();
            break;
         }

         case INSTANCE_DATA_CON_DECL:
            break;

         case INSTANCE_DATA_CON_DEFN: {
            InstanceDataConDefn& instanceDataConDefn = toBody(declOrDefn, InstanceDataConDefn);
            NameTypedValBinding& binding = toBody(instanceDataConDefn._defn, NameTypedValBinding);
            if (binding.id() == name)
               return &binding.typedVal();
            break;
         }

         default:
            error("InstanceDefn::method: Unexpected form");
            break;
      }

   }

//	return _classDefn->method(name);

   return NULL;
}

// InstanceDefn::analyse:  Analyse instance definition

Void InstanceDefn::preAnalyseDeclOrDefn(Expr declOrDefn, Analyser& analyser) {
   switch (formOf(declOrDefn)) {
   case INSTANCE_DATA_CON_DECL:
      break;

   case INSTANCE_DATA_CON_DEFN:
      toBody(declOrDefn, InstanceDataConDefn).preAnalyse(analyser);
      break;

   default:
      analyser.preAnalyseDeclOrDefn(declOrDefn);
      break;
   }
}

Void InstanceDefn::preAnalyse(Analyser& analyser) {
   mapProc1(preAnalyseDeclOrDefn, _declOrDefns, analyser);
}


Void InstanceDefn::analyseDeclOrDefn(Expr declOrDefn, Analyser& analyser) {
   switch (formOf(declOrDefn)) {
   case INSTANCE_DATA_CON_DECL:
      break;

   case INSTANCE_DATA_CON_DEFN:
      toBody(declOrDefn, InstanceDataConDefn).analyse(analyser);
      break;

   default:
      analyser.analyseDeclOrDefn(declOrDefn);
      break;
   }
}

Void InstanceDefn::analyse(Analyser& analyser) {
   mapProc1(analyseDeclOrDefn, _declOrDefns, analyser);
}

// InstanceDefn::reserveDeclOrDefn: Generate code to reserve an instance declaration or definition

Void InstanceDefn::reserveDeclOrDefn(Expr declOrDefn, Code& code) {
   switch (formOf(declOrDefn)) {
      case INSTANCE_DATA_CON_DECL:
         break;

      case INSTANCE_DATA_CON_DEFN:
         toBody(declOrDefn, InstanceDataConDefn).reserve(code);
         break;

      default:
         code.reserveDeclOrDefn(declOrDefn);
         break;
   }
}

// InstanceDefn::gen:  Generate code for instance definition

Void InstanceDefn::genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab, Code& code) {
   switch (formOf(declOrDefn)) {
      case INSTANCE_DATA_CON_DECL:
         break;

      case INSTANCE_DATA_CON_DEFN:
         toBody(declOrDefn, InstanceDataConDefn).gen(errLab, code);
         break;

      default:
         code.genDeclOrDefn(declOrDefn, errLab);
          break;
   }
}

Void InstanceDefn::reserve(Code& code) {
   mapProc1(reserveDeclOrDefn, _declOrDefns, code);
}

Void InstanceDefn::gen(CodeLabel& errLab, Code& code) {
   mapProc2(genDeclOrDefn, _declOrDefns, errLab, code);
}

Void InstanceDefn::printTypeQual(Expr typeQual, ostream& os, const Env& env, Bool pretty/* = FALSE*/) const {
   if (isPair(typeQual)) {
      Expr f = fst(typeQual);
      Expr s = snd(typeQual);
      if (f == (Expr)STRICT) {
         os << '!';
         printTypeSig(s, 0, FALSE, os, env);
      }
      else if (f == (Expr)NEGATION) {
         os << '¬';
         printTypeQual(s, os, env, pretty);
      }
   }
   else if (isTriple(typeQual)) {
      Expr f = fst3(typeQual);
      Expr s = snd3(typeQual);
      Expr t = thd3(typeQual);
      if (f == (Expr)INSTANCE) {
         os << "instance (";
         if (isName(s))
            printName(isName(s)
               ? toName(s)
               : toBody(s, ClassDefnRef).className(),
               os, env.nameTable());
         os << ' ';
         printTypeSigList(t, os, env);
      } else if (f == (Expr)EQUALITY ||
                 f == (Expr)INEQUALITY) {
         printTypeSig(s, 0, FALSE, os, env);
         os << (f == (Expr)EQUALITY ? "=" : "¬=");
         printTypeSig(t, 0, FALSE, os, env);
      } else if (f == (Expr)CONJUNCTION ||
               f == (Expr)DISJUNCTION) {
         os << '(';
         printTypeQual(s, os, env, pretty);
         os << (f == (Expr)CONJUNCTION ? '&' : '|');
         printTypeQual(t, os, env, pretty);
         os << ')';
      }
   }
}

Void InstanceDefn::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/) const {
   os << "INSTANCE,";
   printName(_className, os, env.nameTable());
   os << ',';
   printTypeSigList(_typeSigs, os, env);

   if (_typeQuals != Nil) {
      os << '|';
      UInt n = 0;
      Expr typeQuals = _typeQuals;
      while (typeQuals != Nil) {
         if (n++ > 0) 
            os << ',';
         printTypeQual(hd(typeQuals), os, env, pretty);
         typeQuals = tl(typeQuals);
      }
	}
   os << ',';

   printTranList(_declOrDefns, FALSE, os, env, pretty, 1, pretty);
}

/*----------------------------------------------------------------------------*/

InstanceMethodDecl::InstanceMethodDecl(const InstanceDefn& instanceDefn)
   : Decl(TRUE), _instanceDefn(instanceDefn) {
}

// InstanceMethodDecl::insTypeSig: Instantiate a type signature for a declaration

Void InstanceMethodDecl::insTypeSig(TypedVal& typedVal, TypeCheck& typeCheck) {
   ExprEnumVal forms[] = { CLASS_METHOD_DECL, CLASS_METHOD_DEFN };
   if (_prevDeclOrDefn != NULL && formExistsIn(_prevDeclOrDefn->val(), 2, forms))
      _unifyWithPrevDeclOrDefn = FALSE;

   TypeEnv typeEnv(NULL, 
                   _prevDeclOrDefn != NULL
                      ? _prevDeclOrDefn->substs()
                      : _instanceDefn.substs(typeCheck));
   TypeSig& typeSig = typedVal.typeSig();
   Bool insFlag = TRUE;
   if (typeSig == TypeSig(UNKNOWN) &&
      _prevDeclOrDefn != NULL) { // Test for useless declaration
      typeSig = _prevDeclOrDefn->typeSig();
      typedVal.substs() = _prevDeclOrDefn->substs();
      typedVal.schematicTypeVars() = _prevDeclOrDefn->schematicTypeVars();
   } else {
      typeCheck.bindTypeConNames(typeSig);
      if (hasTypeVar(typeSig)) {
         typeSig = mutableTypeSig(typeSig, typeCheck.msa());
         ::insTypeSig(typeSig, &typeEnv, NULL, typeCheck.msa());
         typedVal.substs() = typeEnv.substs();
         typeVarsInTypeSig(typeSig, typedVal.schematicTypeVars(), typeCheck.msa());
      }
   }

   typedVal.typeSig() = typeSig;
}

/*----------------------------------------------------------------------------*/

// InstanceMethodDefn: Instance method definition

InstanceMethodDefn::InstanceMethodDefn(Expr expr, const InstanceDefn& instanceDefn, Bool isInline)
 : Defn(expr, TRUE, FALSE, isInline, FALSE),
   _instanceDefn(instanceDefn), _decoratedName(NULL_NAME) { 
}

Void InstanceMethodDefn::insTypeSig(TypedVal& typedVal, TypeCheck& typeCheck) {
   ExprEnumVal forms[] = { CLASS_METHOD_DECL, CLASS_METHOD_DEFN };
   if (_prevDeclOrDefn != NULL && formExistsIn(_prevDeclOrDefn->val(), 2, forms))
      _unifyWithPrevDeclOrDefn = FALSE;

   TypeEnv typeEnv(NULL,
      _prevDeclOrDefn != NULL && _unifyWithPrevDeclOrDefn
      ? _prevDeclOrDefn->substs()
      : _instanceDefn.substs(typeCheck));
   TypeSig typeSig = typedVal.typeSig();
   Bool insFlag = TRUE;
   Bool typeSigIsMutable = FALSE;
   if (typeSig == TypeSig(UNKNOWN)) {
      assert(_prevDeclOrDefn != NULL, "InstanceMethodDefn::insTypeSig: Not bound");
      if (formOf(_prevDeclOrDefn->val()) == INSTANCE_METHOD_DECL) {
         typeSig = _prevDeclOrDefn->typeSig();
         typedVal.substs() = _prevDeclOrDefn->substs();
         typedVal.schematicTypeVars() = _prevDeclOrDefn->schematicTypeVars();
         _unifyWithPrevDeclOrDefn = FALSE;
         insFlag = FALSE;
      } else {
         typeSig = _prevDeclOrDefn->typeSig();
         if (_moduleDefn != NULL) {
            CopyContext cc(_moduleDefn, typeCheck.nameTable(),
               typeCheck.predicates().varNameMaps(),
               typeCheck.predicates().typeVarMaps(),
               typeCheck.msa());
            typeSig = mutableTypeSig(typeSig, cc, typeCheck.msa());
            typeSigIsMutable = TRUE;
         }
      }
   } else
      typeCheck.bindTypeConNames(typeSig);
   if (insFlag && hasTypeVar(typeSig)) {
      if (!typeSigIsMutable)
         typeSig = mutableTypeSig(typeSig, typeCheck.msa());
      ::insTypeSig(typeSig, &typeEnv, NULL, typeCheck.msa());
      typeVarsInTypeSig(typeSig, typedVal.schematicTypeVars(), typeCheck.msa());
      typedVal.substs() = typeEnv.substs();
   }
   typedVal.typeSig() = typeSig;
   typedVal.needsInstance() = hasTypeVar(typeSig);
}

static Name decoratedName(Name name, TypeSig typeSig, Code& code) {
   char fallback[32];
   ConstString ident = nameString_(name, code.nameTable());
   if (ident == NULL) {
      sprintf(fallback, "n%d", name);
      ident = fallback;
   }

   size_t sLen = strlen(ident);
   size_t packedLen = packTypeSig(typeSig, NULL, 0, code, FALSE);
   char* buffer = (char*)code.msa().alloc(sLen + 1 + packedLen + 1);

   memcpy(buffer, ident, sLen);
   buffer[sLen] = '$';

   size_t actualLen = packTypeSig(typeSig,
      (unsigned char*)(buffer + sLen + 1),
      packedLen, code, FALSE);
   assert(actualLen == packedLen, "decoratedName: inconsistent packed length");
   buffer[sLen + 1 + actualLen] = '\0';

   return useName(buffer, code.nameTable());
}

Void InstanceMethodDefn::genDecoratedName(Name name, TypeSig typeSig, Code& code) {
   _decoratedName = decoratedName(name, typeSig, code);
}

Void InstanceMethodDefn::reserve(Expr& expr, Name name, TypeSig typeSig, Code& code) {
   if (!_isInline) {
      if (_decoratedName == NULL_NAME)
         genDecoratedName(name, typeSig, code);
      Defn::reserve(expr, _decoratedName, typeSig, code);
   }
}

Void InstanceMethodDefn::gen(Expr& expr, Name name, TypeSig typeSig, CodeLabel& errLab, Code& code) {
   if (!_isInline) {
      if (_decoratedName == NULL_NAME)
         genDecoratedName(name, typeSig, code);
       Defn::gen(expr, _decoratedName, typeSig, errLab, code);
   }
}

/*----------------------------------------------------------------------------*/

// InstanceDataConDecl: Instance data constructor declaration

InstanceDataConDecl::InstanceDataConDecl(Name name, TypeSig typeSig,
                                         const InstanceDefn& instanceDefn,
                                         Tran& tran) 
   : DataConDecl(name, typeSig, tran),
   _instanceDefn(instanceDefn) {
}

// InstanceDataConDecl::insTypeSig : Instantiate a type signature for a
// class instance data constructor declaration

Void InstanceDataConDecl::insTypeSig(TypeCheck& typeCheck) {
   NameTypedValBinding& binding = toBody(_reprDecl, NameTypedValBinding);
   TypedVal& typedVal = binding.typedVal();
   TypeSig& typeSig = typedVal.typeSig();
   TypeEnv typeEnv(NULL);
   typeCheck.bindTypeConNames(typeSig);
   if (hasTypeVar(typeSig)) {
      typeSig = mutableTypeSig(typeSig, typeCheck.msa());
      TypeVarSet typeVars;
      ::insTypeSig(typeSig, &typeEnv, &typeVars, typeCheck.msa());
      typedVal.substs() = typeEnv.substs();
      typedVal.schematicTypeVars() = typeVars;
   }
}

// // DataConDecl::print

Void InstanceDataConDecl::print(ostream& os, const Env& env,
                                Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "INSTANCE_DATA_CON_DECL";
   os << (pretty ? '\n' : ',');
   printTranExpr(_reprDecl, TRUE, os, env, pretty, 0);
}

/*----------------------------------------------------------------------------*/

// InstanceDataConDefn: Instantiate a type signature for a
// class instance data constructor definition

InstanceDataConDefn::InstanceDataConDefn(Expr reprDefn, Expr selectors,
                                         Bool isImplicit, 
                                         const InstanceDefn& instanceDefn,
                                         Bool isInline)
   : DataConDefn(reprDefn, selectors, isImplicit, isInline),
   _instanceDefn(instanceDefn) {
}

Void InstanceDataConDefn::tran(BindingSet& bindingEnv, Tran& tran) {
   BindingSet localBindingEnv(&bindingEnv);
   tran.addBinding(_reprDefn, localBindingEnv);
   DataConDefn::tran(localBindingEnv, tran);
}

Void InstanceDataConDefn::insTypeSigs(TypeCheck& typeCheck) {
   /*
   TypeEnv typeEnv(NULL, NULL, _dataConBinding->substs());
   NameTypedValBinding& reprDefnBinding = toBody(_reprDefn, NameTypedValBinding);
   TypedVal& reprTypedVal = reprDefnBinding.typedVal();
   reprTypedVal.setSchematic(FALSE);
   typeCheck.insDeclOrDefnTypeSig(_reprDefn, &typeEnv);

   NameTypedValBinding& defnBinding = toBody(_defn, NameTypedValBinding);
   TypedVal& defnTypedVal = defnBinding.typedVal();
   TypeSig typeSig = _dataConBinding->typeSig();
   if (typeSig != TypeSig(UNKNOWN))
      typeCheck.bindTypeConNames(typeSig);

   if (hasTypeVar(typeSig)) {
      typeSig = mutableTypeSig(typeSig, typeCheck.msa());
      TypeVarSet typeVars;
      ::insTypeSig(typeSig, &typeEnv, &typeVars, typeCheck.msa());
      defnTypedVal.substs() = typeEnv.substs();
      defnTypedVal.schematicTypeVars() = typeVars;
   }
   defnTypedVal.setNeedsInstance();
   defnTypedVal.typeSig() = typeSig;


   const ModuleDefn* moduleDefn = NULL;

   TypeSig cmTypeSig = _dataConBinding->typeSig();

   if (moduleDefn != NULL) {
      CopyContext cc(moduleDefn, typeCheck.nameTable(),
                     typeCheck.predicates().varNameMaps(),
                     typeCheck.predicates().typeVarMaps(),
                     typeCheck.msa());
      cmTypeSig = mutableTypeSig(cmTypeSig, cc, typeCheck.msa());
   }

   Subst* extSubsts = NULL;
   typeCheck.unify(typeSig, cmTypeSig, &typeEnv, &extSubsts, TRUE, TRUE);

   Subst* subst = _instanceDefn._substs;
   while (subst != NULL) {
      defnTypedVal.substs() = new(typeCheck.msa()) Subst(subst->typeVar(),
         subst->typeSig(),
         defnTypedVal.substs());
      subst = subst->next();
   }

   while (extSubsts != NULL) {
      Subst* next = extSubsts->next();
      if (Subst::lookUp(extSubsts->typeVar(), _instanceDefn._substs) == NULL) {
         extSubsts->next() = defnTypedVal.substs();
         defnTypedVal.substs() = extSubsts;
      }
      extSubsts = next;
   }
   */
}


Void InstanceDataConDefn::typeCheck(TypeCheck& typeCheck) {
   TypeEnv typeEnv(NULL, _dataConBinding->substs());

   typeCheck.typeCheckDeclOrDefn(_reprDefn, &typeEnv, ERROR);

   // Assign CONSTRUCT application signature

   DataCon& dataCon = toBody(_dataConBinding->val(), DataCon);

   NameTypedValBinding& defnBinding = toBody(_defn, NameTypedValBinding);
   FnAp& fnAp = toBody(snd(toBody(toBody(defnBinding.typedVal().val(), Defn)._expr, Lambda)._body), FnAp);

   dataCon.reprTypeSig() = fromTypeVar(::newTypeVar());
   TypeSig typeSig = arg(_dataConBinding->typeSig());
   NameTypedValBinding& reprDefnBinding = toBody(_reprDefn, NameTypedValBinding);
   dataCon.reprTypeSig() = reprDefnBinding.typedVal().typeSig();

   if (dataCon.arity() > 0) {
      typeSig = resultTypeSig(typeSig, dataCon.arity());
      dataCon.reprTypeSig() = resultTypeSig(dataCon.reprTypeSig(), dataCon.arity());;
   }

   fnAp._typeSig = !dataCon.sumType()
      ? ap2(ARROW, dataCon.reprTypeSig(), typeSig, typeCheck.msa())
      : ap2(ARROW, dataCon.reprTypeSig(),
         ap2(ARROW, TAG, typeSig, typeCheck.msa()), typeCheck.msa());

   typeCheck.typeCheckDeclOrDefn(_defn, &typeEnv, ERROR);

   UInt index = 0;
   mapProc4(typeCheckSelector, _selectors, index, arg(_dataConBinding->typeSig()), &typeEnv, typeCheck);

   // Reset schematic type variables for both declaration and definition

   Subst::substTypeVars(_dataConBinding->typeSig(), *typeCheck.substs());
   Subst::substTypeVars(dataCon.reprTypeSig(), *typeCheck.substs());
   typeCheck.setSchematicTypeVars(_dataConBinding->typeSig(), _dataConBinding->schematicTypeVars(), NULL);

   TypedVal& defnTypedVal = toBody(_defn, NameTypedValBinding).typedVal();
   Subst::substTypeVars(defnTypedVal.typeSig(), *typeCheck.substs());
   typeCheck.setSchematicTypeVars(defnTypedVal.typeSig(), defnTypedVal.schematicTypeVars(), NULL);
}

Void InstanceDataConDefn::reserve(Code& code) {
   assert(FALSE, "T.B.D. reserve instance data constructor definition");
}

Void InstanceDataConDefn::gen(CodeLabel& errLab, Code& code) {
   if (!_isInline) {
      NameTypedValBinding& binding = toBody(_defn, NameTypedValBinding);
      Expr& val = binding.typedVal().val();
      TypeSig typeSig = binding.typedVal().typeSig();
      Name name = decoratedName(binding.name(), typeSig, code);
      toBody(val, Defn).gen(val, name, typeSig, errLab, code);
   }
}

// InstanceDataConDefn::print:

Void InstanceDataConDefn::print(ostream& os, const Env& env,
                                Bool pretty/* = FALSE*/,
   UInt indent/* = 0*/) const {
   printTranExpr(_reprDefn, FALSE, os, env, pretty, 0);
   os << (pretty ? '\n' : ',');
   printTranExpr(_defn, FALSE, os, env, pretty, 0);
   if (_selectors != Nil) {
      os << (pretty ? '\n' : ',');
      printTranList(_selectors, FALSE, os, env, pretty, indent = 0, TRUE);
   }
}

/*----------------------------------------------------------------------------*/

ClassConstraint* NameOcc::inferCastMethod(TypeSig fromTS, TypeSig toTS, Bool indep,
                                          TypeEnv* typeEnv, TypeCheck& typeCheck) {
   assert(formOf(val()) == CLASS_METHOD_DECL, "NameOcc::inferCastMethod: expected method declaration");
   const ClassDefn& classDefn = toBody(val(), ClassMethodDecl)._classDefn;

   Subst::substTypeVars(fromTS, *typeCheck.substs());
   Subst::substTypeVars(toTS, *typeCheck.substs());

#ifdef TRACE
   if (traceFlag) {
      outStream << "NameOcc::inferCastMethod: Cast from ";
      printTypeSig(fromTS, 0, FALSE, outStream, typeCheck.env());
      outStream << " to ";
      printTypeSig(toTS, 0, FALSE, outStream, typeCheck.env());
      outStream << endl;
   }
#endif

   Expr typeSigs = classDefn._typeSigs;
   assert(isTypeVar(hd(typeSigs)) && isTypeVar(hd(tl(typeSigs))), "NameOcc::inferCastMethod: expected type variable");
   TypeVar fromTV = toTypeVar(hd(typeSigs));
   TypeVar toTV = toTypeVar(hd(tl(typeSigs)));

   TypeEnv localTypeEnv;
   Subst* fromSubst = new(typeCheck.msa()) Subst(fromTV, fromTS, NULL);
   localTypeEnv.addSubst(fromSubst);
   Subst* toSubst = new(typeCheck.msa()) Subst(toTV, toTS, NULL);
   localTypeEnv.addSubst(toSubst);

   ClassConstraint* constraint = inferMethod(classDefn, indep, &localTypeEnv, typeCheck);

   _typeSig = mutableTypeSig(typeSig(), typeCheck.msa());
   fromSubst->typeVar() = toTypeVar(arg(fun(_typeSig)));
   toSubst->typeVar() = toTypeVar(arg(_typeSig));

   _substs = localTypeEnv.substs();
   Subst::substTypeVars(_typeSig, _substs);

   return constraint;
}

ClassConstraint* NameOcc::inferEqMethod(TypeSig typeSig_,
   TypeEnv* typeEnv, TypeCheck& typeCheck) {
   assert(formOf(val()) == CLASS_METHOD_DECL, "NameOcc::inferEqMethod: expected method declaration");
   const ClassDefn& classDefn = toBody(val(), ClassMethodDecl)._classDefn;

   Subst::substTypeVars(typeSig_, *typeCheck.substs());
   Expr typeSigs = classDefn._typeSigs;
   assert(isTypeVar(hd(typeSigs)), "NameOcc::inferEqMethod: expected type variable");
   TypeVar typeVar = toTypeVar(hd(typeSigs));
   TypeEnv localTypeEnv;
   Subst* subst = new(typeCheck.msa()) Subst(typeVar, typeSig_, NULL);
   localTypeEnv.addSubst(subst);

   ClassConstraint* constraint = inferMethod(classDefn, TRUE, &localTypeEnv, typeCheck);

   _typeSig = mutableTypeSig(typeSig(), typeCheck.msa());
   subst->typeVar() = toTypeVar(arg(fun(_typeSig)));
   Subst::substTypeVars(_typeSig, subst);

   return constraint;
}

// NameOcc::inferMethod: Infer class method

ClassConstraint* NameOcc::inferMethod(const ClassDefn& classDefn,
                                      Bool indep,
                                      TypeEnv* typeEnv,
                                      TypeCheck& typeCheck) {
   CopyContext cc(_moduleDefn, typeCheck.nameTable(),
                  typeCheck.predicates().varNameMaps(),
                  typeCheck.predicates().typeVarMaps(),
                  typeCheck.msa());
   Expr typeSigs = Subst::uniqueTypeSigList(classDefn._typeSigs,
                                            cc,
                                            typeEnv,
                                            typeCheck.msa());

   ClassConstraint* constraint = new(typeCheck.msa())
      ClassConstraint(classDefn, _moduleDefn, typeEnv->substs(), typeSigs, indep);

   constraint->appendTransforms(new (typeCheck.msa())
      CMConstraintTransform(NULL, *constraint, *this));

   return constraint;
}

/*----------------------------------------------------------------------------*/

ClassConstraintInstance::ClassConstraintInstance(Expr typeSigs, TypeQual* typeQuals,
                                                 Subst* substs,
                                                 const InstanceDefn& instanceDefn,
                                                 const ModuleDefn* moduleDefn,
                                                 ClassConstraintInstance* next)
 : _typeSigs(typeSigs), _typeQuals(typeQuals),
   _substs(substs),
   _instanceDefn(instanceDefn),
   _moduleDefn(moduleDefn),
   _next(next),
   _failed(FALSE) {
}

Void ClassConstraintInstance::substTypeVars(const Subst* substs) {
   Subst::substTypeSigList(_typeSigs, substs);
   Subst::substSubsts(_substs, substs);
   TypeQual* typeQual = _typeQuals;
   while (typeQual != NULL) {
      typeQual->substTypeVars(substs);
      typeQual = typeQual->next();
   }

}

Void ClassConstraintInstance::print(ostream& os, const Env& env) const {
   if (_instanceDefn.subordinate())
      os << '(';
   printTypeSigList(_typeSigs, os, env);
   if (_typeQuals != NULL) {
      os << '|';
      TypeQual* typeQual = _typeQuals;
      while (typeQual != NULL) {
         if (typeQual != _typeQuals)
            os << ',';
         typeQual->print(os, env, 0);
         typeQual = typeQual->next();
      }
   }
   if (_instanceDefn.subordinate())
      os << ')';
}

/*----------------------------------------------------------------------------*/

ClassConstraint::ClassConstraint(const ClassDefn&  classDefn,
                                 const ModuleDefn* moduleDefn,
                                 Subst*            substs,
                                 Expr              typeSigs, 
                                 Bool              indep/* = TRUE*/,
                                 Bool              instantiated/* = FALSE*/)
 : Constraint(Constraint::CLASS_CONSTRAINT, indep),
   _classDefn(classDefn), _moduleDefn(moduleDefn),
   _substs(substs),
   _typeSigs(typeSigs),
   _instances(NULL),
   _solInstance(NULL),
   _prevSolInstance(NULL),
   _instantiated(instantiated) {
}

// ClassConstraint::matches: Match type signatures

Bool ClassConstraint::matches(const Constraint& constraint) {
   return &_classDefn == &static_cast<const ClassConstraint&>(constraint)._classDefn &&
      eqTypeSigs(_typeSigs, static_cast<const ClassConstraint&>(constraint)._typeSigs);
}

// ClassConstraint::init: Intialise instances on first use of constraint 

Void ClassConstraint::init(TypeCheck& typeCheck) {
    if (_instances == NULL)
      addInstances(typeCheck);
}

// ClassConstraint::substTypeVars: Apply type signature substitutions

Void ClassConstraint::substTypeVars(const Subst* substs) {
   Subst::substTypeSigList(_typeSigs, substs);
   Subst::substSubsts(_substs, substs);
   ClassConstraintInstance* instance = _instances;
   while (instance != NULL) {
      instance->substTypeVars(substs);
      instance = instance->next();
   }
}

// ClassConstraint::addConstraintSetTypeVars: Add to constraint set by type vars

Void ClassConstraint::addConstraintSetTypeVars(Void* arg, MSA& msa) {
   mapProc3(mapProcTypeVars, _typeSigs, addConstraintSetTypeVar, arg, msa);
}

// ClassConstraint::dependsOnSubsts: Predicate for dependency on substitutions

Bool ClassConstraint::dependsOnSubsts(const Subst* substs) const {
   TypeSig typeSig = _typeSigs;
   while (typeSig != Nil) {
      if (Subst::typeVarOccursInTypeSig(substs, hd(typeSig)))
         return TRUE;
      typeSig = tl(typeSig);
   }
   return FALSE;
}

ClassConstraintInstance* ClassConstraint::newInstance(InstanceDefn* instanceDefn,
   ModuleDefn* moduleDefn,
   TypeCheck& typeCheck) {
   TypeEnv typeEnv(NULL, _substs);

   CopyContext cc(moduleDefn == NULL || typeCheck.moduleDefn() == moduleDefn
      ? NULL
      : moduleDefn, typeCheck.nameTable(),
      typeCheck.predicates().varNameMaps(),
      typeCheck.predicates().typeVarMaps(),
      typeCheck.msa());

   Expr typeSigs = Subst::uniqueTypeSigList(instanceDefn->_typeSigs,
                                            cc,
                                            &typeEnv, typeCheck.msa(),
                                            FALSE);

   TypeQual* typeQuals = uniqueTypeQuals(instanceDefn->_typeQuals,
                                         cc,
                                         &typeEnv,
                                         typeCheck.msa());

   ClassConstraintInstance* constraintInstance = new(typeCheck.msa())
      ClassConstraintInstance(typeSigs, typeQuals,
         typeEnv.substs(),
         *instanceDefn, moduleDefn, NULL);

   return constraintInstance;
}

// ClassConstraint::addInstances

Void ClassConstraint::addInstances(TypeCheck& typeCheck) {
   ClassConstraintInstance** listPtr = &_instances;

   Name className = _classDefn.name();

   // The following nested loop adds instances 
   // firstly from any current module, then external modules

   Bool local = TRUE;
   ModuleDefn* extModuleDefn = typeCheck.moduleDefn();
   do {
      if (extModuleDefn != NULL &&
         (local ||
            typeCheck.moduleDefn() == NULL ||
            typeCheck.moduleDefn() != extModuleDefn)) {
         Name mappedClassName = mapName(className,
            typeCheck.moduleDefn() != NULL && _moduleDefn == NULL
            ? typeCheck.moduleDefn()->nameTable()
            : _moduleDefn->nameTable(),
            extModuleDefn->nameTable());
         ClassInstancesMapEntry* mapEntry =
            extModuleDefn->classInstancesMap().lookUp(mappedClassName, mappedClassName);
         if (mapEntry != NULL) {
            InstanceDefn* instanceDefn = mapEntry->_instanceDefns;
            while (instanceDefn != NULL) {
               *listPtr = newInstance(instanceDefn,
                  local ? NULL : extModuleDefn,
                  typeCheck);
               listPtr = &(*listPtr)->_next;
               instanceDefn = instanceDefn->_next;
            }
         }
      } else
         if (!local)
            break;
      if (local) {
         local = FALSE;
         extModuleDefn = ModuleDefn::workingSet;
      } else
         extModuleDefn = extModuleDefn->next();
   } while (TRUE);

   TypeVar initTypeVarSupply = typeVarSupply;
   TypeVar typeVarHWM = initTypeVarSupply;

   ClassConstraintInstance* instance = _instances;
   while (instance != NULL) {
      TypeEnv typeEnv(NULL, _substs);
      mapProc3(insTypeSig, instance->_typeSigs, &typeEnv, NULL, typeCheck.msa());
      TypeQual* typeQual = instance->_typeQuals;
      while (typeQual != NULL) {
         typeQual->insTypeSigs(&typeEnv, typeCheck.msa());
         typeQual = typeQual->next();
      }
      instance->_substs = typeEnv.substs();

      if (typeVarSupply < typeVarHWM)
         typeVarHWM = typeVarSupply;
      typeVarSupply = initTypeVarSupply;
      instance = instance->next();
   }

   typeVarSupply = typeVarHWM;
}

// ClassConstraint::instantiate

Constraint* ClassConstraint::instantiate(const ModuleDefn* moduleDefn,
                                         TypeEnv*          typeEnv, 
                                         TypeInsts*        typeInsts,
                                         Bool              unresolved,
                                         TypeCheck&        typeCheck) {
   CopyContext cc(moduleDefn, typeCheck.nameTable(),
                  typeCheck.predicates().varNameMaps(),
                  typeCheck.predicates().typeVarMaps(),
                  typeCheck.msa());

// Temporarily work-around to provide constant TypeEnv

   ClassConstraint* constraint = new(typeCheck.msa())
      ClassConstraint(_classDefn,
                      _moduleDefn != NULL ? _moduleDefn : moduleDefn,
                      Subst::insSubsts(_substs, cc, typeEnv, typeCheck.msa()),
                      Subst::uniqueTypeSigList(_typeSigs,
                                               cc,
                                               typeEnv,
                                               typeCheck.msa()),
                      _indep, TRUE);

   ConstraintTransform* transform = transforms();
   while (transform != NULL) {
      InstantiatedCMConstraintTransform* insTransform = new (typeCheck.msa())
         InstantiatedCMConstraintTransform(NULL,
            *constraint,
            moduleDefn,
            ((AbstractCMConstraintTransform*)transform)->nameOccInst(),
            ((AbstractCMConstraintTransform*)transform)->nameOcc(),
            ((AbstractCMConstraintTransform*)transform)->nameOccModuleDefn() != NULL
            ? ((AbstractCMConstraintTransform*)transform)->nameOccModuleDefn()
            : moduleDefn,
            typeInsts,
            typeEnv,
            typeCheck);

      constraint->appendTransforms(insTransform);
      transform = transform->next();
   }

#ifdef TRACE
   if (traceFlag) {
      const Env& env = moduleDefn == NULL ? typeCheck.env() : moduleDefn->env();
      outStream << "*** Instantiated class constraint:\n";
      print(outStream, env, FALSE);
      outStream << "\nas\n";
      constraint->print(outStream, typeCheck.env(), FALSE);
      outStream << "\n";
   }
#endif

   return constraint;
}

// ClassConstraint::preCheck: Clear instance failure flags

void ClassConstraint::preCheck(Void) {
   ClassConstraintInstance* instance = _instances;
   while (instance != NULL) {
      instance->failed() = FALSE;
      instance = instance->next();
   }
}

// checkTypeQuals: Check a type qualifier list

static TypePredicate checkTypeQuals(TypeQual* typeQual,
                                    Subst* substs,
                                    TypeEnv* typeEnv,
                                    TypeCheck& typeCheck) {
   Bool unresolved = FALSE;

   while (typeQual != NULL) {
      switch (typeQual->check(substs, typeEnv, typeCheck)) {
         case TYPE_PRED_FALSE:
            return TYPE_PRED_FALSE;

         case TYPE_PRED_UNDECIDABLE:
            unresolved = TRUE;
            break;

         default:
            break;
      }

      typeQual = typeQual->next();
   }

   return unresolved
      ? TYPE_PRED_UNDECIDABLE
      : TYPE_PRED_TRUE;
}

// ClassConstraint::check: Check for a unique solution to a class constraint

Constraint::Result ClassConstraint::checkNew(Bool fix,
                                             const ConstraintSet& constraintSet,
                                             Subst** substs,
                                             const ExcludedConstraint* excludes,
                                             TypeEnv* typeEnv,
                                             TypeCheck& typeCheck,
                                             Constraint*& resolved) {

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
   static UInt probe = 0;
   if (++probe == 0)
      traceFlag = TRUE;
   UInt localProbe = probe;
   if (traceFlag) {
      outStream << "Checking " << '[' << probe << "]: ";
      Subst::printSubsts(*substs, outStream, typeCheck.env(), TRUE);
      outStream << "\n---";
      print(outStream, typeCheck.env(), FALSE);
      outStream << '\n';
   }
#endif

   UInt n = 0;

   Subst* solSubsts = NULL;
   Constraint* solResolved = NULL;
   const ClassConstraintInstance* solInstance = NULL;
   Bool solUnresolved = FALSE;

#ifdef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
   UInt nCast = 0;
   UInt nSubordinate = 0;
#endif

   Bool subordinateRes = FALSE;
   Bool checkSubordinates = FALSE;
   do {
      ClassConstraintInstance* instance = _instances;
      while (instance != NULL) {
         if (instance->_instanceDefn._subordinate == checkSubordinates) {
            Subst* tSubsts = *substs;
            Constraint* tResolved = NULL;
            Bool qualUnresolved = FALSE;

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
            if (traceFlag) {
               outStream << ">>>>>>>Instance (" << _seqNo << ")\n";
               printTypeSigList(_typeSigs, outStream, typeCheck.env());
               outStream << '\n';
               printTypeSigList(instance->_typeSigs, outStream, typeCheck.env());
               outStream << '\n';
               if (tSubsts != NULL) {
                  Subst::printSubsts(tSubsts, outStream, typeCheck.env(), TRUE);
                  outStream << '\n';
               }
            }
#endif

            Bool res = !instance->failed() &&
               typeCheck.unifyTypeLists(_typeSigs, instance->_typeSigs, typeEnv, &tSubsts);

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
            if (traceFlag) {
               if (res) {
                  Subst::printSubsts(tSubsts, outStream, typeCheck.env(), TRUE);
                  outStream << '\n';
               }
               else
                  outStream << "Unification fails\n";
            }
#endif

            if (res) {
               switch (checkTypeQuals(instance->_typeQuals, tSubsts, typeEnv, typeCheck)) {
                  case TYPE_PRED_FALSE:
                     res = FALSE;
                     break;

                  case TYPE_PRED_UNDECIDABLE:
#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
                     //                  if (traceFlag)
                     //                     outStream << "Unresolved qualifier\n";
#endif
                     qualUnresolved = TRUE;
                     break;

                  default:
                     break;
               }
            }

            if (res) {
               const ExcludedConstraint exclude(excludes, this);
               Subst* iSubsts = tSubsts;
               Result constraintRes = typeCheck.checkFirstConstraint(fix, constraintSet, &tSubsts, &exclude, typeEnv, tResolved);
               Bool dependentUnresolved = constraintRes.tag() == Result::Unresolved;
               switch (constraintRes.tag()) {
                  case Result::SetIsEmpty:
                  case Result::Unresolved:
                     tResolved = NULL;
                     tSubsts = iSubsts;
//------------------ drop through
                  case Result::Satisfied: 
                     if (instance->_typeQuals != NULL) {
                        TypePredicate qualPred = checkTypeQuals(instance->_typeQuals, tSubsts, typeEnv, typeCheck);
                        switch (qualPred) {
                           case TYPE_PRED_FALSE: {
                              Constraint* constraint = tResolved;
                              while (constraint != NULL) {
                                 constraint->revertSolution();
                                 constraint = constraint->next();
                              }
                              break;
                           }

                           case TYPE_PRED_TRUE:
                              qualUnresolved = FALSE;
                              break;

                           case TYPE_PRED_UNDECIDABLE:
                              qualUnresolved = TRUE;
                              break;
                        }

                        if (qualPred == TYPE_PRED_FALSE)
                           break;

                        if (dependentUnresolved)
                           qualUnresolved = TRUE;
                     }

                     if (n > 0) {

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
                        if (traceFlag)
                           outStream << '(' << _seqNo << ") Unresolved\n";
#endif

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
                        return Result::Unresolved;
#else

/* The intention of the code below is to prioritise ambiguous constraint set solutions
   to one involving the smallest number of casts or subordinate instances (if same number of casts)
*/

                        if (tResolved != NULL) {
                           UInt nSubord = nSubordinate +(!checkSubordinates ? 0 : 1);
                           if (constraintRes.nSubordinate() > nSubord ||
                               constraintRes.nSubordinate() == nSubord &&
                               constraintRes.nCast() > nCast) {
                              Constraint* constraint = tResolved;
                              do {
                                 constraint->revertSolution();
                                 constraint = constraint->next();
                              } while (constraint != NULL);
                              break;
                           } else {
                              if (constraintRes.nSubordinate() == nSubord &&
                                    constraintRes.nCast() == nCast) {
                                 tResolved = NULL;
                                 return Result::Unresolved;
                              }
                           }
                        } else {
 //                          if (nCast == 0 &&
 //                                 nSubordinate == 0)
                                 return Result::Unresolved;
                        }
#endif
                     } else
                        n++;
                     solSubsts = tSubsts;
                     solResolved = tResolved;
                     solInstance = instance;
                     solUnresolved = qualUnresolved;
                     subordinateRes = checkSubordinates;

#ifdef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
                     nSubordinate = constraintRes.nSubordinate();
                     nCast = constraintRes.nCast();
#endif

                  break;

                  case Result::Fails:
                     if (excludes == NULL ||
                        excludes != NULL && !_indep && excludes->_next == NULL) {
                        instance->failed() = TRUE;
                     }
                     break;

                  default:
                     break;
               }
            }
            else {
               if (excludes == NULL ||
                  excludes != NULL && !_indep && excludes->_next == NULL) {
                  instance->failed() = TRUE;
               }
            }
         }
         instance = instance->_next;
      }
      checkSubordinates = !checkSubordinates;
   } while (checkSubordinates && n == 0);
   if (n == 1) {
      if (!solUnresolved) {
         *substs = solSubsts;
         _prevSolInstance = _solInstance;
         _solInstance = solInstance;
         _next = solResolved;
         resolved = this;

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
         if (traceFlag) {
            outStream << '(' << _seqNo <<  "): Satisfied";
            if (subordinateRes)
               outStream << " by subordinate instance";
            outStream << ": ";
            Subst::printSubsts(solSubsts, outStream, typeCheck.env(), TRUE);
            outStream << '\n';
         }

#endif        

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
         return Result(Result::Satisfied);
#else
            return Result(Result::Satisfied, nCast, nSubordinate + (!subordinateRes ? 0 : 1));
#endif

      } else


#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
         if (traceFlag) {
            outStream << '(' << _seqNo << "): Unresolved\n";
         }
#endif
         return Result::Unresolved;
   } else {

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
      if (traceFlag)
         outStream << '(' << _seqNo << "): Fails\n";
#endif

      return Result::Fails;
   }
}


Constraint::Result ClassConstraint::check(Bool fix,
                                          const ConstraintSet& constraintSet,
                                          Subst** substs,
                                          const ExcludedConstraint* excludes,
                                          TypeEnv* typeEnv,
                                          TypeCheck& typeCheck,
                                          Constraint*& resolved) {

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
   static UInt probe = 0;
   if (++probe == 0)
      traceFlag = TRUE;
   UInt localProbe = probe;
   if (traceFlag) {
      outStream << "Checking " << '[' << probe << "]: ";
      Subst::printSubsts(*substs, outStream, typeCheck.env(), TRUE);
      outStream << "\n---";
      print(outStream, typeCheck.env(), FALSE);
      outStream << '\n';
   }
#endif

   UInt n = 0;

   Subst* solSubsts = NULL;
   Constraint* solResolved = NULL;
   const ClassConstraintInstance* solInstance = NULL;
   Bool solUnresolved = FALSE;

#ifdef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
   UInt nCast = 0;
   UInt nSubordinate = 0;
#endif

   Bool subordinateRes = FALSE;
   Bool checkSubordinates = FALSE;
   do {
      ClassConstraintInstance* instance = _instances;
      while (instance != NULL) {
         if (instance->_instanceDefn._subordinate == checkSubordinates) {
            Subst* tSubsts = *substs;
            Constraint* tResolved = NULL;
            Bool qualUnresolved = FALSE;

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
            if (traceFlag) {
               outStream << ">>>>>>>Instance (" << _seqNo << ")\n";
               printTypeSigList(_typeSigs, outStream, typeCheck.env());
               outStream << '\n';
               printTypeSigList(instance->_typeSigs, outStream, typeCheck.env());
               outStream << '\n';
               if (tSubsts != NULL) {
                  Subst::printSubsts(tSubsts, outStream, typeCheck.env(), TRUE);
                  outStream << '\n';
               }
            }
#endif

            Bool res = !instance->failed() &&
               typeCheck.unifyTypeLists(_typeSigs, instance->_typeSigs, typeEnv, &tSubsts);

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
            if (traceFlag) {
               if (res) {
                  Subst::printSubsts(tSubsts, outStream, typeCheck.env(), TRUE);
                  outStream << '\n';
               }
               else
                  outStream << "Unification fails\n";
            }
#endif

            if (res) {
               TypeQual* typeQual = instance->_typeQuals;

               while (typeQual != NULL) {
                  switch (typeQual->check(tSubsts, typeEnv, typeCheck)) {
                  case TYPE_PRED_FALSE:
                     res = FALSE;
                     break;

                  case TYPE_PRED_UNDECIDABLE:
                     qualUnresolved = TRUE;
                     break;

                  default:
                     break;
                  }
                  typeQual = typeQual->next();
               }
            }

            if (res) {
               const ExcludedConstraint exclude(excludes, this);
               Subst* iSubsts = tSubsts;
               Result remRes = typeCheck.checkFirstConstraint(fix, constraintSet, &tSubsts, &exclude, typeEnv, tResolved);
               Bool remUnresolved = remRes.tag() == Result::Unresolved;
               switch (remRes.tag()) {
                  case Result::SetIsEmpty:
                  case Result::Unresolved:
                     tResolved = NULL;
                     tSubsts = iSubsts;
//------------------ drop through
                  case Result::Satisfied: 
                     if (remUnresolved && instance->_typeQuals != NULL)
                        qualUnresolved = TRUE;

                     if (n > 0) {

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
                        if (traceFlag)
                           outStream << '(' << _seqNo << ") Unresolved\n";
#endif

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
                        return Result::Unresolved;
#else

/* The intention of the code below is to prioritise ambiguous constraint set solutions
   to one involving the smallest number of casts or subordinate instances (if same number of casts)
*/

                        if (tResolved != NULL) {
                           UInt nSubord = nSubordinate +(!checkSubordinates ? 0 : 1);
                           if (constraintRes.nSubordinate() > nSubord ||
                               constraintRes.nSubordinate() == nSubord &&
                               constraintRes.nCast() > nCast) {
                              Constraint* constraint = tResolved;
                              do {
                                 constraint->revertSolution();
                                 constraint = constraint->next();
                              } while (constraint != NULL);
                              break;
                           } else {
                              if (constraintRes.nSubordinate() == nSubord &&
                                    constraintRes.nCast() == nCast) {
                                 tResolved = NULL;
                                 return Result::Unresolved;
                              }
                           }
                        } else {
 //                          if (nCast == 0 &&
 //                                 nSubordinate == 0)
                                 return Result::Unresolved;
                        }
#endif
                     } else
                        n++;
                     solSubsts = tSubsts;
                     solResolved = tResolved;
                     solInstance = instance;
                     solUnresolved = qualUnresolved;
                     subordinateRes = checkSubordinates;

#ifdef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
                     nSubordinate = constraintRes.nSubordinate();
                     nCast = constraintRes.nCast();
#endif

                  break;

                  case Result::Fails:
                     if (excludes == NULL ||
                        excludes != NULL && !_indep && excludes->_next == NULL) {
                        instance->failed() = TRUE;
                     }
                     break;

                  default:
                     break;
               }
            }
            else {
               if (excludes == NULL ||
                  excludes != NULL && !_indep && excludes->_next == NULL) {
                  instance->failed() = TRUE;
               }
            }
         }
         instance = instance->_next;
      }
      checkSubordinates = !checkSubordinates;
   } while (checkSubordinates && n == 0);
   if (n == 1) {
      if (!solUnresolved) {
         *substs = solSubsts;
         _prevSolInstance = _solInstance;
         _solInstance = solInstance;
         _next = solResolved;
         resolved = this;

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
         if (traceFlag) {
            outStream << '(' << _seqNo <<  "): Satisfied";
            if (subordinateRes)
               outStream << " by subordinate instance";
            outStream << ": ";
            Subst::printSubsts(solSubsts, outStream, typeCheck.env(), TRUE);
            outStream << '\n';
         }

#endif        

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
         return Result(Result::Satisfied);
#else
            return Result(Result::Satisfied, nCast, nSubordinate + (!subordinateRes ? 0 : 1));
#endif

      } else


#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
         if (traceFlag) {
            outStream << '(' << _seqNo << "): Unresolved\n";
         }
#endif
         return Result::Unresolved;
   } else {

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
      if (traceFlag)
         outStream << '(' << _seqNo << "): Fails\n";
#endif

      return Result::Fails;
   }
}

// ClassConstraint::revertSolution

Void ClassConstraint::revertSolution(Void) {
   _solInstance = _prevSolInstance;
   _prevSolInstance = NULL;
}

// ClassConstraint::onSatisfied: On satisfied action

Void ClassConstraint::onSatisfied(ConstraintSet& constraintSet, const Subst* substs,
   TypeCheck& typeCheck) {
   Subst::substSubsts(_solInstance->substs(), *typeCheck.substs());
   Subst::substSubsts(_solInstance->substs(), substs);
}

// ClassConstraint::instancePredicate:
// Predicate for existential instance under substitutions

TypePredicate ClassConstraint::instancePredicate(Subst* substs,
                                                 TypeEnv* typeEnv,
                                                 TypeCheck& typeCheck) {
   ClassConstraintInstance* instance = _instances;
   while (instance != NULL) {
      Subst* tSubsts = substs;
      if (typeCheck.unifyTypeLists(_typeSigs, instance->_typeSigs,
         typeEnv, &tSubsts)) {
         switch (checkTypeQuals(instance->_typeQuals, tSubsts, typeEnv, typeCheck)) {
            case TYPE_PRED_TRUE:
               return TYPE_PRED_TRUE;

            case TYPE_PRED_UNDECIDABLE:
               return TYPE_PRED_UNDECIDABLE;

            default:
               break;
         }
      }
      instance = instance->_next;
   }
   return TYPE_PRED_FALSE;
}

// ClassConstraint::print: Print

Void ClassConstraint::print(ostream& os, const Env& env, Bool shortForm) const {
   os << nameString_(_classDefn.name(), (_moduleDefn == NULL
      ? env
      : _moduleDefn->env()).nameTable());
   if (!shortForm)
      os << " (" << _seqNo << ") ";
   else
      os << ' ';
   printTypeSigList(_typeSigs, os, env);
   if (!shortForm) {
      Subst::printSubsts(_substs, os, env, TRUE, TRUE);
      os << ", ";
   }
   os << "[";
   ClassConstraintInstance* instance = _instances;
   while (instance != NULL) {
      if (instance != _instances)
         os << ',';
      instance->print(os, env);
      instance = instance->_next;
   }
   os << "], [";
   ConstraintTransform* transform = _transforms;
   while (transform != NULL) {
      if (transform != _transforms)
         os << ',';
      transform->print(os, env, shortForm);
      transform = transform->next();
   }
   os << ']';
}

/*----------------------------------------------------------------------------*/

// AbstractCMConstraintTransform::apply: On satisfied action

Void AbstractCMConstraintTransform::apply(const Subst* substs,
                                          Constraint*& constraints,
                                          TypeEnv* typeEnv,
                                          TypeCheck& typeCheck) {
   const ClassConstraintInstance& solInstance = *((ClassConstraint*)_constraint)->solInstance();
   NameOcc& nameOcc = this->nameOcc();
   const ModuleDefn* nameOccModuleDefn = this->nameOccModuleDefn();
   if (nameOccModuleDefn == NULL) {
      Subst::substTypeVars(nameOcc._typeSig, *typeCheck.substs());
      Subst::substTypeVars(nameOcc._typeSig, substs);
      Subst::substSubsts(nameOcc._substs, *typeCheck.substs());
      Subst::substSubsts(nameOcc._substs, substs);
   }

   TypedVal* typedVal = NULL;
   const ModuleDefn* bindingModuleDefn = NULL;
   Name mappedName = NULL_NAME;
   const char* nameStr = NULL;
   if (solInstance.moduleDefn() == NULL &&
      nameOccModuleDefn == NULL ||
      solInstance.moduleDefn() == nameOccModuleDefn) {
      typedVal = solInstance.instanceDefn().method(nameOcc.name());
      bindingModuleDefn = solInstance.moduleDefn();
   } else {
      Name name = nameOcc.name();
      if (!isBuiltInName(name)) {
         nameStr = nameString_(name,
            nameOccModuleDefn != NULL
            ? nameOccModuleDefn->nameTable()
            : typeCheck.env().nameTable());
         name = nameStr != NULL
            ? (solInstance.moduleDefn() != NULL
               ? solInstance.moduleDefn()->nameTable()
               : typeCheck.env().nameTable()).lookUp(nameStr) + builtInNameCount
            : NULL_NAME;
      }
      typedVal = name != NULL_NAME
         ? solInstance.instanceDefn().method(name)
         : NULL;

      if (typedVal != NULL) {
         mappedName = name;
         bindingModuleDefn = solInstance.moduleDefn();
      }
   }
   if (typedVal == NULL) {

//--- No instance method, search for class method

      bindingModuleDefn = solInstance.instanceDefn().moduleDefn();
      if (bindingModuleDefn == NULL)
         bindingModuleDefn = solInstance.moduleDefn();

      if (bindingModuleDefn == NULL &&
         nameOccModuleDefn == NULL ||
         bindingModuleDefn == nameOccModuleDefn)
         typedVal = solInstance.instanceDefn().classDefn().method(nameOcc.name());
      else {
         Name name = nameOcc.name();
         if (!isBuiltInName(name)) {
            if (nameStr == NULL)
               nameStr = nameString_(name, nameOccModuleDefn != NULL ? nameOccModuleDefn->nameTable() : typeCheck.env().nameTable());
            name = nameStr != NULL
               ? bindingModuleDefn->nameTable().lookUp(nameStr) + builtInNameCount
               : NULL_NAME;
         }
         typedVal = name != NULL_NAME
            ? solInstance.instanceDefn().classDefn().method(name)
            : NULL;

         if (typedVal != NULL)
            mappedName = name;
      }
   }

   if (typedVal != NULL) {
      Subst* bindingSubsts = typedVal->substs();

      TypeSig scheme = typedVal->typeSig();
      if (bindingModuleDefn == NULL)
         scheme = mutableTypeSig(scheme, typeCheck.msa());
      else {
         CopyContext cc(bindingModuleDefn, typeCheck.nameTable(),
            typeCheck.predicates().varNameMaps(),
            typeCheck.predicates().typeVarMaps(),
            typeCheck.msa());
         bindingSubsts = Subst::copySubsts(bindingSubsts, cc, typeCheck.msa());
         scheme = mutableTypeSig(scheme, cc, typeCheck.msa());
      }
      Subst::substSubsts(bindingSubsts, *typeCheck.substs());
      Subst::substTypeVars(scheme, *typeCheck.substs());

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
      if (traceFlag) {
         const Env& nameOccEnv = nameOccModuleDefn != NULL ? nameOccModuleDefn->env() : typeCheck.env();
         outStream << "Instance substitutions: ";
         Subst::printSubsts(solInstance.substs(), outStream, typeCheck.env(), TRUE);
         outStream << "\nName occurrence type signature: ";
         printTypeSig(nameOcc._typeSig, 0, FALSE, outStream, nameOccEnv);
         outStream << '\n';
         Subst::printSubsts(nameOcc._substs, outStream, nameOccEnv, TRUE);
         outStream << "\nMapped to: ";
         const Env& tvEnv = bindingModuleDefn == NULL ? typeCheck.env() : bindingModuleDefn->env();
         typedVal->print(outStream, tvEnv);
         outStream << '\n';
         printTypeSig(typedVal->typeSig(), 0, FALSE, outStream, tvEnv); //???
         Subst::printSubsts(bindingSubsts, outStream, typeCheck.env(), TRUE);
         outStream << endl;
      }
#endif
#endif

      Subst** nameOccSubstList;

//--- Bind either directly to name occurrence, or instance

      bind(typedVal, bindingModuleDefn, mappedName, nameOccSubstList);

      Subst::substSubsts(*nameOccSubstList, *typeCheck.substs());
      Subst::substSubsts(*nameOccSubstList, substs);

      *nameOccSubstList = typeCheck.instanceSubsts(bindingSubsts, *nameOccSubstList, typeEnv);

      Subst::substSubsts(*nameOccSubstList, *nameOccSubstList);

//--- Suppress 'const' for constraint list work

      ConstraintSet* depConstraintSets = NULL;
      ConstraintSet** unresolvedList = bindingModuleDefn == NULL
         ? &typeCheck.predicates().unresolvedList()
         : &((ModuleDefn*)(Void*)bindingModuleDefn)->constraints();
      const Env& env = bindingModuleDefn == NULL ? typeCheck.env() : bindingModuleDefn->env();
      AddConstraintSetToListByTypeVar_Args args = { unresolvedList,
                                                   &depConstraintSets,
                                                   &env };
#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING

      if (traceFlag) {
         outStream << "Finding depending constraints for: ";
         printTypeSig(scheme, 0, FALSE, outStream, env);
         outStream << '\n';
         if (*nameOccSubstList != NULL) {
            Subst::printSubsts(*nameOccSubstList, outStream, env, TRUE);
            outStream << '\n';
         }
         outStream.flush();
      }

#endif
#endif

      mapProcTypeVars(typedVal->typeSig(),
         addConstraintSetToListByTypeVar, &args, typeCheck.msa());

      if (depConstraintSets != NULL) {
         TypeEnv localTypeEnv(NULL, *nameOccSubstList);

         while (depConstraintSets != NULL) {
            ConstraintSet* depConstraintSet = depConstraintSets;
            depConstraintSets = depConstraintSets->next();
            ConstraintElement* depConstraintElement = depConstraintSet->elements();
            while (depConstraintElement != NULL) {
               if (depConstraintElement->constraint()->kind() != Constraint::SCHEME_CONSTRAINT &&
                   depConstraintElement->constraint()->kind() != Constraint::SCHEMATIC_INSTANCE_CONSTRAINT) {
                  Constraint* constraint =
                     depConstraintElement->constraint()->instantiate(bindingModuleDefn,
                        &localTypeEnv,
                        typeInsts(),
                        FALSE,
                        typeCheck);
                  constraint->next() = constraints;
                  constraints = constraint;
               }
               depConstraintElement = depConstraintElement->next();
            }
            depConstraintSet->next() = *unresolvedList;
            *unresolvedList = depConstraintSet;
         }
         *nameOccSubstList = localTypeEnv.substs();
      }
 
      if (typedVal->needsInstance()) {
         assert(bindingModuleDefn == NULL, "AbstractCMConstraintTransform::apply: unexpected external module binding");

// T.B.D. Consider whether or not a method binding requires a schematic instance

//         typedVal->addInstance(nameOcc._typeSig, typeInsts(), NULL, typeCheck); //???
      }

   } else {
      error("No definition for class method found");
      outStream << nameString_(nameOcc.name(), nameOccModuleDefn == NULL ? typeCheck.nameTable() : nameOccModuleDefn->nameTable());
      outStream << endl;
   }
}

/*----------------------------------------------------------------------------*/

CMConstraintTransform::CMConstraintTransform(ConstraintTransform* next,
   Constraint& constraint,
   NameOcc& nameOcc)
   : AbstractCMConstraintTransform(next, constraint),
   _nameOcc(nameOcc) {
}

//--- CMConstraintTransform::bind
//--- Rebind name occurrence to instance definition

Void CMConstraintTransform::bind(TypedVal* typedVal,
   const ModuleDefn* moduleDefn,
   Name              mappedName,
   Subst**& substList) {

   _nameOcc._binding.val() = typedVal;
   _nameOcc._moduleDefn = moduleDefn;
   _nameOcc._mappedName = mappedName;
   _nameOcc._noRebindOnCopy = TRUE;
   substList = &_nameOcc._substs;
}

Void CMConstraintTransform::print(ostream& os, const Env& env,
                                  Bool shortForm) const {
   os << "CM: ";
   _nameOcc.print(os, env);
}

/*----------------------------------------------------------------------------*/

InstantiatedCMConstraintTransform::InstantiatedCMConstraintTransform(
   ConstraintTransform* next,
   Constraint& constraint,
   const ModuleDefn* moduleDefn,
   NameOccInst* parentNameOccInst,
   NameOcc& nameOcc,
   const ModuleDefn* nameOccModuleDefn,
   TypeInsts* typeInsts,
   TypeEnv* typeEnv,
   TypeCheck& typeCheck)
   : AbstractCMConstraintTransform(next, constraint),
   _nameOccInst(typeInsts->_nameOccInsts, parentNameOccInst,
      &nameOcc, nameOccModuleDefn) {

   typeInsts->_nameOccInsts = &_nameOccInst;
   if (parentNameOccInst == NULL) {
      if (nameOccModuleDefn == NULL)
         Subst::substSubsts(nameOcc.substs(), *typeCheck.substs());
      if (((ClassConstraint&)constraint).substs() != NULL)
         Subst::substSubsts(nameOcc.substs(), ((ClassConstraint&)constraint).substs());
      CopyContext cc(nameOccModuleDefn, typeCheck.nameTable(),
         typeCheck.predicates().varNameMaps(),
         typeCheck.predicates().typeVarMaps(),
         typeCheck.msa());
      _nameOccInst.substs() = Subst::insSubsts(nameOcc.substs(), cc, typeEnv, typeCheck.msa());
   }
   else {
      CopyContext cc(moduleDefn, typeCheck.nameTable(),
                     typeCheck.predicates().varNameMaps(),
                     typeCheck.predicates().typeVarMaps(),
                     typeCheck.msa());
      if (moduleDefn == NULL || moduleDefn == typeCheck.moduleDefn())
         Subst::substSubsts(parentNameOccInst->substs(), *typeCheck.substs());
      _nameOccInst.substs() = Subst::insSubsts(parentNameOccInst->substs(), cc, typeEnv, typeCheck.msa());
   }
}

Void InstantiatedCMConstraintTransform::bind(TypedVal* typedVal,
   const ModuleDefn* moduleDefn,
   Name                 mappedName,
   Subst**& substList) {
   _nameOccInst.bind_(typedVal, moduleDefn, mappedName);
   substList = &_nameOccInst.substs();
}

TypeSig InstantiatedCMConstraintTransform::typeSig(TypeCheck& typeCheck) const {
   CopyContext cc(_nameOccInst.nameOccModuleDefn(), typeCheck.nameTable(),
      typeCheck.predicates().varNameMaps(),
      typeCheck.predicates().typeVarMaps(),
      typeCheck.msa());
   TypeEnv typeEnv(NULL, (Subst*)((Void*)_nameOccInst.substs()));
   TypeSig typeSig = mutableTypeSig(_nameOccInst.nameOcc()->_typeSig, cc, typeCheck.msa());
   ::insTypeSig(typeSig, &typeEnv, NULL, typeCheck.msa());
   return typeSig;
}

Void InstantiatedCMConstraintTransform::print(ostream& os, const Env& env,
                                              Bool shortForm) const {
   os << "ICM ";
   if (_nameOccInst.nameOccModuleDefn() != NULL)
      os << '(' << nameString_(_nameOccInst.nameOccModuleDefn()->name(),
         _nameOccInst.nameOccModuleDefn()->env().nameTable()) << ')';
   os << ": ";
   _nameOccInst.nameOcc()->print(os, _nameOccInst.nameOccModuleDefn() != NULL ?
      _nameOccInst.nameOccModuleDefn()->env() : env);

#ifdef PRINT_NAME_OCC_INST_SUBSTS
   if (shortForm)
      Subst::printSubsts(_nameOccInst.substs(), os, env, TRUE);
#endif
   Subst::printSubsts(_nameOccInst.substs(), os, env, TRUE);
}

/*----------------------------------------------------------------------------*/

// Type qualifer structures

StrictTypeQual::StrictTypeQual(TypeQual* next, TypeSig typeSig)
   : TypeQual(next), _typeSig(typeSig) {
}

Void StrictTypeQual::substTypeVars(const Subst* substs) {
   Subst::substTypeVars(_typeSig, substs);
} 

Void StrictTypeQual::insTypeSigs(TypeEnv* typeEnv, MSA& msa) {
   insTypeSig(_typeSig, typeEnv, NULL, msa);
}

TypePredicate StrictTypeQual::check(Subst* substs,
                                          TypeEnv* typeEnv,
                                          TypeCheck& typeCheck) const {
   switch (isExprTypeSig(_typeSig, substs)) {
      case TYPE_PRED_FALSE:
         return TYPE_PRED_TRUE;
         break;

      case TYPE_PRED_TRUE:
         return TYPE_PRED_FALSE;
         break;

      default:
         return TYPE_PRED_UNDECIDABLE;
   }
}

Void StrictTypeQual::print(ostream& os, const Env& env, UInt prec) const {
   os << '!';
   printTypeSig(_typeSig, 0, FALSE, os, env);
}


InstanceTypeQual::InstanceTypeQual(TypeQual* next,
   ClassConstraint& constraint)
   : TypeQual(next),
   _constraint(constraint) {
}


Void InstanceTypeQual::substTypeVars(const Subst* substs) {
   _constraint.substTypeVars(substs);
}

Void InstanceTypeQual::insTypeSigs(TypeEnv* typeEnv, MSA& msa) {
   _constraint.substTypeVars(typeEnv->substs());
}

TypePredicate InstanceTypeQual::check(Subst* substs,
   TypeEnv* typeEnv,
   TypeCheck& typeCheck) const {
   _constraint.init(typeCheck);
   return _constraint.instancePredicate(substs, typeEnv, typeCheck);
}


Void InstanceTypeQual::print(ostream& os, const Env& env, UInt prec) const {
   if (prec != 0)
      os << '(';
   _constraint.print(os, env, FALSE);
   if (prec != 0)
      os << ')';
}


NegationTypeQual::NegationTypeQual(TypeQual* next, TypeQual& typeQual)
   : TypeQual(next), _typeQual(typeQual) {
}


Void NegationTypeQual::substTypeVars(const Subst* substs) {
   _typeQual.substTypeVars(substs);
}

Void NegationTypeQual::insTypeSigs(TypeEnv* typeEnv, MSA& msa) {
   _typeQual.insTypeSigs(typeEnv, msa);
}

TypePredicate NegationTypeQual::check(Subst* substs,
                                            TypeEnv* typeEnv,
                                            TypeCheck& typeCheck) const {
   switch (_typeQual.check(substs, typeEnv, typeCheck)) {
      case TYPE_PRED_FALSE:
         return TYPE_PRED_TRUE;
         break;

      case TYPE_PRED_TRUE:
         return TYPE_PRED_FALSE;
         break;

      default:
         return TYPE_PRED_UNDECIDABLE;
      }
}

Void NegationTypeQual::print(ostream& os, const Env& env, UInt prec) const {
   os << '¬';
   _typeQual.print(os, env, 3);
}

EqualityTypeQual::EqualityTypeQual(TypeQual* next,
                                   TypeSig x, TypeSig y,
                                   Bool negate)
   : TypeQual(next), _x(x), _y(y), _negate(negate) {
}


Void EqualityTypeQual::substTypeVars(const Subst* substs) {
   Subst::substTypeVars(_x, substs);
   Subst::substTypeVars(_y, substs);
}

Void EqualityTypeQual::insTypeSigs(TypeEnv* typeEnv, MSA& msa) {
   insTypeSig(_x, typeEnv, NULL, msa);
   insTypeSig(_y, typeEnv, NULL, msa);
}

TypePredicate EqualityTypeQual::check(Subst* substs,
                                            TypeEnv* typeEnv,
                                            TypeCheck& typeCheck) const {
   switch (equalTypeSigs(_x, _y, substs)) {
      case TYPE_PRED_FALSE: return !_negate ? TYPE_PRED_FALSE : TYPE_PRED_TRUE;
      case TYPE_PRED_TRUE:  return !_negate ? TYPE_PRED_TRUE  : TYPE_PRED_FALSE;
      default:
         return TYPE_PRED_UNDECIDABLE;
   }
}

Void EqualityTypeQual::print(ostream& os, const Env& env, UInt prec) const {
   printTypeSig(_x, 0, FALSE, os, env);
   os << (!_negate ? "=" : "¬=");
   printTypeSig(_y, 0, FALSE, os, env);
}


LogicalOpTypeQual::LogicalOpTypeQual(TypeQual* next,
                                     TypeQual& x, TypeQual& y)
   : TypeQual(next), _x(x), _y(y){
}

Void LogicalOpTypeQual::substTypeVars(const Subst* substs) {
   _x.substTypeVars(substs);
   _y.substTypeVars(substs);
}

Void LogicalOpTypeQual::insTypeSigs(TypeEnv* typeEnv, MSA& msa) {
   _x.insTypeSigs(typeEnv, msa);
   _y.insTypeSigs(typeEnv, msa);
}

Void LogicalOpTypeQual::print(ostream& os, const Env& env, UInt prec) const {
   if (prec != 0 && prec < this->prec())
      os << '(';
   _x.print(os, env, this->prec());
   os << symbol();
   _y.print(os, env, this->prec());
   if (prec != 0 && prec < this->prec())
      os << ')';
}

ConjunctionTypeQual::ConjunctionTypeQual(TypeQual* next,
                                         TypeQual& x, TypeQual& y)
   : LogicalOpTypeQual(next, x, y) {
}

TypePredicate ConjunctionTypeQual::check(Subst* substs,
   TypeEnv* typeEnv,
   TypeCheck& typeCheck) const {
   switch (_x.check(substs, typeEnv, typeCheck)) {
      case TYPE_PRED_FALSE:
         return TYPE_PRED_FALSE;

      case TYPE_PRED_TRUE:
         return _y.check(substs, typeEnv, typeCheck);

      default:
         switch (_y.check(substs, typeEnv, typeCheck)) {
            case TYPE_PRED_FALSE:
               return TYPE_PRED_FALSE;

            case TYPE_PRED_TRUE:
            default:
               return TYPE_PRED_UNDECIDABLE;
         }
   }
}

UInt ConjunctionTypeQual::prec(Void) const {
   return 4;
}

Char ConjunctionTypeQual::symbol(Void) const {
   return '&';
}


DisjunctionTypeQual::DisjunctionTypeQual(TypeQual* next,
                                         TypeQual& x, TypeQual& y)
   : LogicalOpTypeQual(next, x, y) {
}

TypePredicate DisjunctionTypeQual::check(Subst* substs,
   TypeEnv* typeEnv,
   TypeCheck& typeCheck) const {
   switch (_x.check(substs, typeEnv, typeCheck)) {
      case TYPE_PRED_FALSE:
         return _y.check(substs, typeEnv, typeCheck);

      case TYPE_PRED_TRUE:
         return TYPE_PRED_TRUE;

      default:
         switch (_y.check(substs, typeEnv, typeCheck)) {
            case TYPE_PRED_TRUE:
               return TYPE_PRED_TRUE;

            case TYPE_PRED_FALSE:
            default:
               return TYPE_PRED_UNDECIDABLE;
         }
   }
}

UInt DisjunctionTypeQual::prec(Void) const {
   return 4;
}

Char DisjunctionTypeQual::symbol(Void) const {
   return '|';
}


// uniqueTypeQual: Add a unique mapped type qualifier
// (Conservatively assumes type qualifers are generally type variables)

TypeQual* uniqueTypeQual(Expr typeQual, const CopyContext& cc,
                         TypeQual* next ,
                         TypeEnv* typeEnv, MSA& msa) {
   if (isPair(typeQual)) {
      Expr f = fst(typeQual);
      Expr s = snd(typeQual);
      if (f == (Expr)STRICT) {
         return new(msa) StrictTypeQual(next,
            Subst::uniqueTypeSig(s,
                                 cc,
                                 typeEnv, msa, FALSE));
      } else if (f == (Expr)NEGATION)
         return new(msa) NegationTypeQual(next,
                                         *uniqueTypeQual(s, cc, NULL, typeEnv, msa));
   }
   else if (isTriple(typeQual)) {
      Expr f = fst3(typeQual);
      Expr s = snd3(typeQual);
      Expr t = thd3(typeQual);
 
      if (f == (Expr)INSTANCE) {
         const ClassDefnRef& classDefnRef = toBody(s, ClassDefnRef);
         const ClassDefn& classDefn = classDefnRef.classDefn();
         const ModuleDefn* moduleDefn = classDefnRef.moduleDefn() != NULL
            ? classDefnRef.moduleDefn()
            : cc.moduleDefn();
         Expr typeSigs = Subst::uniqueTypeSigList(t,
            cc,
            typeEnv,
            msa, FALSE);
         return new(msa) InstanceTypeQual(next,
                                          *new(msa)
                                          ClassConstraint(classDefn,
                                                          moduleDefn,
                                                          typeEnv->substs(),
                                                          typeSigs));
      } else if (f == (Expr)EQUALITY ||
                 f == (Expr)INEQUALITY)
         return new(msa) EqualityTypeQual(next,
            Subst::uniqueTypeSig(snd3(typeQual),
               cc,
               typeEnv, msa, FALSE),
            Subst::uniqueTypeSig(thd3(typeQual),
               cc,
               typeEnv, msa, FALSE),
            f == (Expr)INEQUALITY);
      else if (f == (Expr)CONJUNCTION) 
         return new(msa) ConjunctionTypeQual(next,
            *uniqueTypeQual(s, cc, NULL, typeEnv, msa),
            *uniqueTypeQual(t, cc, NULL, typeEnv, msa));
      else if (f == (Expr)DISJUNCTION)
         return new(msa) DisjunctionTypeQual(next,
            *uniqueTypeQual(s, cc, NULL, typeEnv, msa),
            *uniqueTypeQual(t, cc, NULL, typeEnv, msa));
   }
   return NULL;
}
   // ClassConstraint::uniqueTypeQuals: Return a list of unique type qualifiers

TypeQual* uniqueTypeQuals(Expr typeQualList, const CopyContext & cc,
                          TypeEnv* typeEnv, MSA & msa) {
   if (typeQualList == Nil)
      return NULL;
   else
      return uniqueTypeQual(hd(typeQualList),
                            cc,
                            uniqueTypeQuals(tl(typeQualList),
                                            cc,
                                            typeEnv, msa),
                            typeEnv, msa);
}

/*----------------------------------------------------------------------------*/

TypedVal* lookUpBuiltInClassMethod(ClassMethod& classMethod,
                                    const Subst* substs, Tran& tran) {
   const Class* class_ = classMethod.class__();   

// Look up occurrence unique type variable

   TypeVar tv = Subst::lookUp(class_->typeVar(), substs);

// Get substitution for type

   const Subst* subst;
   if (tv == NULL_NAME || (subst = tran.substs().lookUp(tv)) == NULL) {
      error("ClassMethod::lookUp: no substitution found");
      return NULL;
   }

   Expr res;
   if (class_->method(classMethod.name(), useBuiltInType(subst->typeSig()), res))
      return tran.lookUpBuiltInBinding(toName(res));
         
   if (subst->typeSig() != typeCon(Any)) {
      printTranName(classMethod.name(), outStream, tran.env());
      outStream << ": no instance or class method definition for type: ";
      printTranExpr(subst->typeSig(), FALSE, outStream, tran.env());
      outStream << endl;
   }
   return NULL;
}
