/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    class.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory classs.
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

#ifndef IVORY_COMPILER_CLASS_H_DEFINED
#define IVORY_COMPILER_CLASS_H_DEFINED

#include "code.h"
#include "tran.h"
#include "type.h"

class ClassMethod;
class BindingSet;
class InstanceDefn;
class Subst;
class Tran;

// Class definition

class ClassDefn {
public:
   ClassDefn(Expr superClasses, Name name, Expr typeSigs, Expr decls);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Name name(Void) const { return _name;}
   inline Expr superClasses(Void) const { return _superClasses; }
   inline Expr typeSigs(Void) const { return _typeSigs; }

   Void tran(Tran& tran);
   Void tranDeclOrDefn(Expr& declOrDefn, Tran& tran);
   Void addBinding(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran);
   Void addBindings(BindingSet& bindingEnv, Tran& tran);
   Void tranDeclOrDefn(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran);
   Void tran(BindingSet& bindingEnv, Tran& tran);
   Void insDeclOrDefnTypeSig(Expr declOrDefn, TypeCheck& typeCheck);
   Void insTypeSigs(TypeCheck& typeCheck);
   Void typeCheckDeclOrDefn(Expr declOrDefn, TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void typeCheck(TypeCheck& typeCheck);
   Void resetDeclOrDefnNeedsInstance(Expr declOrDefn, TypeCheck& typeCheck);
   Void resetNeedsInstance(TypeCheck& typeCheck);
   TypedVal* method(Name name) const;

   Void preAnalyseDeclOrDefn(Expr declOrDefn, Analyser& analyser);
   Void preAnalyse(Analyser& analyser);
   Void analyseDeclOrDefn(Expr declOrDefn, Analyser& analyser);
   Void analyse(Analyser& analyser);

   Void reserveDeclOrDefn(Expr declOrDefn, Code& code);
   Void reserve(Code& code);
   Void genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab, Code& code);
   Void gen(CodeLabel& errLab, Code& code);
   Void print(ostream& os, const Env& env, Bool pretty) const;

protected:
   Name					_name;
   Expr              _superClasses;     // superclass list
   Expr					_typeSigs;				// type signature list
   Expr					_declOrDefns;			// method declaration or definition list 
   ClassDefn*        _next;					// next in list

   friend class ClassMethodDecl;
   friend class ClassMethodDefn;
   friend class ClassObjectDefn;
   friend class ModuleDefn;
   friend class InstanceDefn;
   friend class InstanceMethodDefn;
	friend class NameOcc;
   friend class TypedVal;
};

// ClassDefnRef: A reference to a class definition

class ClassDefnRef {
public:
   ClassDefnRef(Name name, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Name className(Void) const { return _className; }
   const ClassDefn& classDefn(Void) const { return *_classDefn; }
   const ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }

protected:
   Name              _className;
   const ClassDefn*  _classDefn;
   const ModuleDefn* _moduleDefn;
};

// ClassMethodDecl: Class method declaration value 

class ClassMethodDecl : public Decl {
public:
   ClassMethodDecl(const ClassDefn& classDefn);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   inline const ClassDefn& classDefn(Void) { return _classDefn; }

   virtual Void insTypeSig(TypedVal& typedVal, TypeCheck& typeCheck);
   virtual Void print(ostream& os, const Env& env,
                      Bool pretty = FALSE, UInt indent = 0) const;

public:
   const ClassDefn& _classDefn;

	friend class NameOcc;
};

// ClassMethodDefn: Class method definition value

class ClassMethodDefn : public Defn {
public:
   ClassMethodDefn(Expr expr, const ClassDefn& _classDefn, Bool inlineFlag);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   inline const ClassDefn& classDefn_(Void) const { return _classDefn; }
   Expr typeSigs(Void) const { return _classDefn._typeSigs; }
   inline virtual UInt32 prevDeclForm(Void) const { return CLASS_METHOD_DECL; };

   TypedVal* lookUp(Name name, const Subst* substs, const Tran& tran) const;
   Void gen(Expr& expr, Name name, TypeSig typeSig, CodeLabel& errLab, Code& code);

protected:
   const ClassDefn&	_classDefn;

	friend class NameOcc;
	friend class Tran;
   friend class TypeCheck;
};

// ClassObjectDefn: Class object definition

class ClassObjectDefn : public TypeDefn {
public:
   ClassObjectDefn(Name name, Expr dataConDecl,
                   const ClassDefn& classDefn, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline const ClassDefn& classDefn_(Void) const { return _classDefn; }
   Expr typeSigs(Void) const { return _classDefn._typeSigs; }
   
   virtual Expr allocDataCon(Name name, Tag tag, TypeSig typeSig, MSA& msa);

   Void insDataConTypeSig(Expr dataCon, TypeCheck& typeCheck);
   Void insTypeSigs(TypeCheck& typeCheck);

   //   Void print(ostream& os, const Env& env) const;
protected:
   const ClassDefn& _classDefn;

   friend class NameOcc;
   friend class Tran;
   friend class TypeCheck;
};

/*----------------------------------------------------------------------------*/

class ClassDataCon : public DataCon {
public:
   ClassDataCon(Name name, Tag tag, TypeSig typeSig,
                const ClassDefn& classDefn);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline const ClassDefn& classDefn(Void) { return _classDefn; }

protected:
   const ClassDefn& _classDefn;
};

// InstanceDefn: An instance definition

class InstanceDefn {
public:
   InstanceDefn(Name className, Bool subordinate,
                Expr typeSigs, Expr typeQuals, Expr declOrDefns);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Name className(Void) const { return _className; }
   inline Bool subordinate(Void) const { return _subordinate; }
   inline Expr typeSigs(Void) const { return _typeSigs; }
   const ClassDefn& classDefn(Void) const { return *_classDefn; }
   const ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }

	Void tran(Tran& tran);
   Void tranTypeQual(Expr& typeQual, Tran& tran);
	Void tranDeclOrDefn(Expr& declOrDefn, Tran& tran);
   Void addBinding(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran);
	Void tran(BindingSet& bindingEnv, Tran& tran);
	Void tranDeclOrDefn(Expr& declOrDefn, BindingSet& bindingEnv, Tran& tran);
   Void insDeclOrDefnTypeSig(Expr declOrDefn, TypeCheck& typeCheck);
   Void insTypeQualTypeSigs(Expr typeQual, TypeCheck& typeCheck);
   Void insTypeSigs(TypeCheck& typeCheck);
   Subst* substs(TypeCheck& typeCheck) const;
   Void typeCheckDeclOrDefn(Expr declOrDefn, TypeCheck& typeCheck);
   Void typeCheck(TypeCheck& typeCheck);
   Void resetNeedsInstance(TypeCheck& typeCheck, TypeEnv* typeEnv);

   Void preAnalyseDeclOrDefn(Expr declOrDefn, Analyser& analyser);
   Void preAnalyse(Analyser& analyser);
   Void analyseDeclOrDefn(Expr declOrDefn, Analyser& analyser);
   Void analyse(Analyser& analyser);

   Void reserveDeclOrDefn(Expr declOrDefn, Code& code);
   Void reserve(Code& code);

   Void genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab, Code& code);
   Void gen(CodeLabel& errLab, Code& code);

   Void printTypeQual(Expr typeQual, ostream& os, const Env& env, Bool pretty = FALSE) const;
   Void print(ostream& os, const Env& env, Bool pretty = FALSE) const;

   TypedVal* method(Name name) const;

protected:
   Name		         _className;	   // class name
   Bool              _subordinate;  // True if subordinate, False otherwise
   Expr				   _typeSigs;     // type signature list
	Expr				   _typeQuals;		// type qualifiers
   Expr				   _declOrDefns;  // method declarations and definitions
   ClassDefn*        _classDefn;    // Class definition
   const ModuleDefn* _moduleDefn;   // Class module definition

   InstanceDefn*     _next;         // next in list of instances

   friend class ClassDefn;
	friend class ClassConstraint;
	friend class CMConstraintResult;
   friend class InstanceMethodDecl;
   friend class InstanceMethodDefn;
   friend class InstanceDataConDefn;
   friend class ModuleDefn;
   friend class NameOcc;
   friend class Tran;
};

// InstanceMethodDecl: Instance method declaration value 

class InstanceMethodDecl : public Decl {
public:
   InstanceMethodDecl(const InstanceDefn& instanceDefn);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual Void insTypeSig(TypedVal& typedVal, TypeCheck& typeCheck);

public:
   const InstanceDefn& _instanceDefn;
};

// InstanceMethodDefn: Instance method definition value

class InstanceMethodDefn : public Defn {
public:
   InstanceMethodDefn(Expr expr, const InstanceDefn& instanceDefn, Bool inlineFlag);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline const InstanceDefn& classDefn_(Void) const { return _instanceDefn; }
   inline virtual UInt32 prevDeclForm(Void) const { return INSTANCE_METHOD_DECL; };

   virtual Void insTypeSig(TypedVal& typedVal, TypeCheck& typeCheck);
   Void genDecoratedName(Name name, TypeSig typeSig, Code& code);
   Void reserve(Expr& expr, Name name, TypeSig typeSig, Code& code);
   Void gen(Expr& expr, Name name, TypeSig typeSig, CodeLabel& errLab, Code& code);
   //   Void print(ostream& os, const Env& env) const;
protected:
   const InstanceDefn&	_instanceDefn;
   Name                 _decoratedName;
};

// InstanceDataConDecl:  Instance data constructor declaration

class InstanceDataConDecl : public DataConDecl {
public:
   InstanceDataConDecl(Name name, TypeSig typeSig,
                       const InstanceDefn& instanceDefn,
                       Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif
   Void insTypeSig(TypeCheck& typeCheck);
   virtual Void print(ostream& os, const Env& env,
                      Bool pretty = FALSE, UInt indent = 0) const;

protected:
   const InstanceDefn& _instanceDefn;
};

// InstanceDataConDefn:  Instance data constructor definition

class InstanceDataConDefn : public DataConDefn {
public:
   InstanceDataConDefn(Expr reprDefn, Expr selectors, Bool isImplicit,
                       const InstanceDefn& _classDefn,
                       Bool inlineFlag);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void tran(BindingSet& bindingEnv, Tran& tran);
   Void insTypeSigs(TypeCheck& typeCheck);
   Void typeCheck(TypeCheck& typeCheck);

   Void reserve(Code& code);
   Void gen(CodeLabel& errLab, Code& code);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   const InstanceDefn& _instanceDefn;
};





/*----------------------------------------------------------------------------*/

// CMConstraintInstance: Instance types for a class method constraint

class ClassConstraintInstance {
public:
   ClassConstraintInstance(Expr typeSigs, TypeQual* typeQuals,
                           Subst* substs,
                           const InstanceDefn& instanceDefn,
                           const ModuleDefn* moduleDefn,
                           ClassConstraintInstance* next);

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline ClassConstraintInstance* next(Void) { return _next; };
   inline Subst* substs(Void) const { return _substs; }
   inline const InstanceDefn& instanceDefn(Void) const { return _instanceDefn; }
   inline const ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }
   inline Bool& failed(Void) { return _failed; }

   Void substTypeVars(const Subst* substs);

   Void print(ostream& os, const Env&) const;

protected:
   Expr                       _typeSigs;     // Type signature list
   TypeQual*                  _typeQuals;
   Subst*                     _substs;
   const InstanceDefn&        _instanceDefn;
   const ModuleDefn*          _moduleDefn;   // Instance module
   ClassConstraintInstance*   _next;
   Bool                       _failed;

   friend class TypeCheck;
   friend class NameOcc;
   friend class ClassConstraint;
   friend class CMConstraintTransform;
};

/*----------------------------------------------------------------------------*/

// ClassConstraint: Class constraint
// The list of types given by 'typeSigs' must unify with exactly one list in  _instances.
// Used primarily to satisfy class constraints

class ClassConstraint : public Constraint {
public:
   ClassConstraint(const ClassDefn& classDefn,
                   const ModuleDefn* moduleDefn,
                   Subst* substs,
                   Expr typeSigs,
                   Bool indep = TRUE, Bool instantiated = FALSE);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Expr typeSigs(Void) const  { return _typeSigs; }
   inline const ClassDefn& classDefn(Void) const { return _classDefn; }
   inline const ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }
   inline Subst* substs(Void) const { return _substs; }
   inline const ClassConstraintInstance* solInstance(Void) const { return _solInstance; }

   virtual Bool matches(const Constraint& constraint);
   virtual Void init(TypeCheck& typeCheck);

   virtual Void substTypeVars(const Subst* substs);
   virtual Void addConstraintSetTypeVars(Void* arg, MSA& msa);
   Bool dependsOnSubsts(const Subst* substs) const;

   ClassConstraintInstance* newInstance(InstanceDefn* instanceDefn, ModuleDefn* moduleDefn,
                                        TypeCheck& typeCheck);
   Void addInstances(TypeCheck& typeCheck);

   virtual Constraint* instantiate(const ModuleDefn* moduleDefn,
                                   TypeEnv* typeEnv, 
                                   TypeInsts* typeInsts,
                                   Bool unresolved,
                                   TypeCheck& typeCheck);

   virtual Void preCheck(Void);

   virtual Result checkNew(Bool fix,
                           const ConstraintSet& constraintSet,
                           Subst** substs,
                           const ExcludedConstraint* excludes,
                           TypeEnv* typeEnv,
                           TypeCheck& typeCheck,
                           Constraint*& results);

   virtual Result check(Bool fix,
                        const ConstraintSet& constraintSet,
                        Subst** substs,
                        const ExcludedConstraint* excludes,
                        TypeEnv* typeEnv,
                        TypeCheck& typeCheck,
                        Constraint*& results);
   virtual Void revertSolution(Void);
   TypePredicate instancePredicate(Subst* substs,
                                   TypeEnv* typeEnv,
                                   TypeCheck& typeCheck);
   virtual Void onSatisfied(ConstraintSet& constraintSet, const Subst* substs,
                            TypeCheck& typeCheck);
   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:
   const ClassDefn&                 _classDefn;
   const ModuleDefn*                _moduleDefn;   // class definition module
   Subst*                           _substs;
   Expr                             _typeSigs;
   ClassConstraintInstance*         _instances;
   const ClassConstraintInstance*   _solInstance;
   const ClassConstraintInstance*   _prevSolInstance;
   Bool                             _instantiated;

   friend class TypeCheck;
   friend class NameOcc;
};

/*----------------------------------------------------------------------------*/

// CMConstraintTransform: Abstract result data for a statisfied
//                     class method constraint

class AbstractCMConstraintTransform : public ConstraintTransform {

public:
   AbstractCMConstraintTransform(ConstraintTransform* next, Constraint& constraint)
      : ConstraintTransform(next, &constraint) {
   }

   virtual NameOccInst* nameOccInst(Void) = 0;
   virtual NameOcc& nameOcc(Void) = 0;
   virtual const ModuleDefn* nameOccModuleDefn(Void) = 0;
   virtual TypeInsts* typeInsts(Void) = 0;

   virtual Void apply(const Subst* substs, Constraint*& constraints,
      TypeEnv* typeEnv, TypeCheck& typeCheck);

   virtual Void bind(TypedVal* typedVal,
      const ModuleDefn* moduleDefn,
      Name              mappedName,
      Subst**& substList) = 0;

   virtual TypeSig typeSig(TypeCheck& typeCheck) const = 0;

protected:
};

// CMConstraintTransform: Result data for a satisfied class method constraint

class CMConstraintTransform : public AbstractCMConstraintTransform {

public:
   CMConstraintTransform(ConstraintTransform* next, Constraint& constraint,
      NameOcc& nameOcc);

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual NameOccInst* nameOccInst(Void) { return NULL; }
   virtual NameOcc& nameOcc(Void) { return _nameOcc; }
   virtual TypeInsts* typeInsts(Void) { return _nameOcc.typeInsts(); }
   virtual const ModuleDefn* nameOccModuleDefn(Void) {
      return NULL;
   }
   virtual TypeSig typeSig(TypeCheck& typeCheck) const {
      return _nameOcc._typeSig;
   }

   virtual Void bind(TypedVal* typedVal,
      const ModuleDefn* moduleDefn,
      Name              mappedName,
      Subst**& substList);

   virtual Void print(ostream& os, const Env& env, Bool shortFrom) const;

protected:
   NameOcc& _nameOcc;
};

// InstantiatedCMConstraintTransform: Result data for a satisfied instantiated
// class method constraint

class InstantiatedCMConstraintTransform : public AbstractCMConstraintTransform {

public:
   InstantiatedCMConstraintTransform(ConstraintTransform* next,
      Constraint& constraint,
      const ModuleDefn* moduleDefn,
      NameOccInst* parentNameOccInst,
      NameOcc& nameOcc,
      const ModuleDefn* nameOccModuleDefn,
      TypeInsts* typeInsts,
      TypeEnv* typeEnv,
      TypeCheck& typeCheck);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual NameOccInst* nameOccInst(Void) { return &_nameOccInst; }
   virtual NameOcc& nameOcc(Void) { return *_nameOccInst.nameOcc(); }
   virtual const ModuleDefn* nameOccModuleDefn(Void) {
      return _nameOccInst.nameOccModuleDefn();
   }
   virtual TypeInsts* typeInsts(Void) { return _nameOccInst.typeInsts(); }
   virtual TypeSig typeSig(TypeCheck& typeCheck) const;

   virtual Void bind(TypedVal* typedVal,
      const ModuleDefn* moduleDefn,
      Name              mappedName,
      Subst**& substList);

   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:
   NameOccInst _nameOccInst;
};


// TypeQual: Abstract type qualifier

class TypeQual {
public:
   TypeQual(TypeQual* next) {
      _next = next;
   }
   inline TypeQual* next(Void) { return _next; }

   virtual Void substTypeVars(const Subst* substs) = 0;
   virtual Void insTypeSigs(TypeEnv* typeEnv, MSA& msa) = 0;
   virtual TypePredicate check(Subst* substs,
                                     TypeEnv* typeEnv,
                                     TypeCheck& typeCheck) const = 0;
   virtual Void print(ostream& os, const Env& env, UINT prec) const = 0;

protected:
   TypeQual* _next;
};

// StrictTypeQual: A strict type qualifier

class StrictTypeQual : public TypeQual {
public:
   StrictTypeQual(TypeQual* next, TypeSig typeSig);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeSig typeSig(Void) const { return _typeSig; }

   virtual Void substTypeVars(const Subst* substs);
   virtual Void insTypeSigs(TypeEnv* typeEnv, MSA& msa);
   virtual TypePredicate check(Subst* substs,
                                     TypeEnv* typeEnv,
                                     TypeCheck& typeCheck) const;
   virtual Void print(ostream& os, const Env& env, UInt prec) const;

protected:
   TypeSig           _typeSig;
};


// InstanceTypeQual: An instance type qualifier

class InstanceTypeQual : public TypeQual {
public:
   InstanceTypeQual(TypeQual* next, ClassConstraint& constraint);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Constraint& constraint(Void) { return _constraint; }

   virtual Void substTypeVars(const Subst* substs);
   virtual Void insTypeSigs(TypeEnv* typeEnv, MSA& msa);
   virtual TypePredicate check(Subst* substs,
      TypeEnv* typeEnv,
      TypeCheck& typeCheck) const;
   virtual Void print(ostream& os, const Env& env, UInt prec) const;

protected:
   ClassConstraint& _constraint;
};

// NegationTypeQual: A logical negation type qualifier

class NegationTypeQual : public TypeQual {
public:
   NegationTypeQual(TypeQual* next, TypeQual& typeQual);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline const TypeQual& typeQual(Void) const { return _typeQual; }

   virtual Void substTypeVars(const Subst* substs);
   virtual Void insTypeSigs(TypeEnv* typeEnv, MSA& msa);
   virtual TypePredicate check(Subst* substs,
      TypeEnv* typeEnv,
      TypeCheck& typeCheck) const;
   virtual Void print(ostream& os, const Env& env, UInt prec) const;

protected:
   TypeQual& _typeQual;
};

// EqualityTypeQual: An equality type qualifier

class EqualityTypeQual : public TypeQual {
public:
   EqualityTypeQual(TypeQual* next, TypeSig x, TypeSig y, Bool negate);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeSig x(Void) { return _x; }
   inline TypeSig y(Void) { return _y; }

   virtual Void substTypeVars(const Subst* substs);
   virtual Void insTypeSigs(TypeEnv* typeEnv, MSA& msa);
   virtual TypePredicate check(Subst* substs,
                                     TypeEnv* typeEnv,
                                     TypeCheck& typeCheck) const;
   virtual Void print(ostream& os, const Env& env, UInt prec) const;

protected:
   TypeSig           _x;
   TypeSig           _y;
   Bool              _negate;
};

// LogicalOpTypeQual: A logical operator type qualifier

class LogicalOpTypeQual : public TypeQual {
public:
   LogicalOpTypeQual(TypeQual* next, TypeQual& x, TypeQual& y);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeQual& x(Void) { return _x; }
   inline TypeQual& y(Void) { return _y; }
 
   virtual Void substTypeVars(const Subst* substs);
   virtual Void insTypeSigs(TypeEnv* typeEnv, MSA& msa);
   virtual TypePredicate check(Subst* substs,
      TypeEnv* typeEnv,
      TypeCheck& typeCheck) const = 0;

   virtual Char symbol(Void) const = 0;
   virtual UInt prec(Void) const = 0;
   virtual Void print(ostream& os, const Env& env,UInt prec) const;

protected:
   TypeQual& _x;
   TypeQual& _y;
};

// ConjunctionTypeQual: A logical and type qualifier

class ConjunctionTypeQual : public LogicalOpTypeQual {
public:
   ConjunctionTypeQual(TypeQual* next, TypeQual& x, TypeQual& y);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual TypePredicate check(Subst* substs,
      TypeEnv* typeEnv,
      TypeCheck& typeCheck) const;

   virtual UInt prec(Void) const;
   virtual Char symbol(Void) const; 
};

// DisjunctionTypeQual: A logical or type qualifier

class DisjunctionTypeQual : public LogicalOpTypeQual {
public:
   DisjunctionTypeQual(TypeQual* next, TypeQual& x, TypeQual& y);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual TypePredicate check(Subst* substs,
      TypeEnv* typeEnv,
      TypeCheck& typeCheck) const;

   virtual UInt prec(Void) const;
   virtual Char symbol(Void) const;
};



extern TypedVal* lookUpBuiltInClassMethod(ClassMethod& classMethod,
                                          const Subst* substs, Tran& tran);

extern Void traceClassCompilation(Bool flag);

extern TypeQual* uniqueTypeQual(Expr typeQual, const CopyContext& cc,
                                TypeQual* next,
                                TypeEnv* typeEnv, MSA& msa);

extern TypeQual* uniqueTypeQuals(Expr typeQual, const CopyContext& cc,   
                                 TypeEnv* typeEnv, MSA& msa);

#endif /* IVORY_COMPILER_CLASS_H_DEFINED */
