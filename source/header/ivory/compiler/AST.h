/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    AST.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Declarations relating to the AST
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

#ifndef IVORY_COMPILER_AST_H_DEFINED
#define IVORY_COMPILER_AST_H_DEFINED

#include "ivory/common.h"
#include "ivory/ExportMap.h"
#include "ivory/nameBinding.h"
#include "ivory/nameExprMap.h"
#include "ivory/typeTable.h"
#include "ClassInstancesMap.h"

#define N_NAME_BINDING_HASH_TABLE_SLOTS   997

#ifndef TYPE_VAR_DEFINED
typedef Name            TypeVar;
#define TYPE_VAR_DEFINED
#endif

typedef Bool (*ExprPred)(Expr expr);
typedef Bool (*ExprEqFn)(Expr, Expr);

class Analyser;
class BidirNameMap; 
class BindingSet;
class ByteCodeSegment;
class CellInfo_;
class ClassConstraint;
class ClassConstraintInstance;
class ClassDefn;
class Code;
class CodeNode;
class CodeLabel; 
class Coercion;
class CoercionConstraint;
class CoercionInst;
class ConstOperand;
class Constraint;
class ConstraintSet;
class CopyContext;
class DataConPattern;
class FnAp;
class FreeVarAssoc;
class LabelOperand;
class Lambda;
class Link;
class Loc;
class LocElement;
class ModuleDefn;
class NameOcc;
class NameOccInst;
class NameOccInstMap;
class Operand;
class ReturnState;
class SchematicInstanceConstraint;
class SchemeConstraint;
class StructTemplate;
class Subst;
class SubstSet;
class Tran;
class TypeCheck;
class TypeEnv;
class TypeInsts;
class Var;

/*----------------------------------------------------------------------------*/

// TypeVarElement: An element of a set of type variables.

class TypeVarElement {
public:
   TypeVarElement(TypeVarElement* next, TypeVar typeVar)
      : _next(next), _typeVar(typeVar) {}
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline const TypeVarElement* next(Void) const { return _next; }
   inline TypeVarElement* next(Void) { return _next; }
   inline TypeVar typeVar(Void) const { return _typeVar; }

protected:
   TypeVarElement*   _next;
   TypeVar		      _typeVar;

   friend class TypeVarSet;
   friend class TypeCheck;
   friend class SchematicTypeSig;
   friend class ConstraintSet;
};

/*----------------------------------------------------------------------------*/

// TypeVarSet: A set of type variables
//    n.b. On construction the list of type var elements is not copied

class TypeVarSet {
public:
   TypeVarSet(TypeVarSet* next = NULL, TypeVarElement* typeVarElements = NULL)
      : _typeVarElements(typeVarElements) {}
   TypeVarSet(const TypeVarSet& src)
      : _typeVarElements(src._typeVarElements) {}
   TypeVarSet(const TypeVarSet& src, CopyContext* cc, MSA& msa);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   TypeVarElement* elements(Void) { return _typeVarElements; }
   const TypeVarElement* elements(Void) const { return _typeVarElements; }
   Bool isEmpty(Void) const { return _typeVarElements == NULL; };
   Void empty() { _typeVarElements = NULL; }

   Void addElement(TypeVar typeVar, MSA& msa);
   Void removeElement(TypeVar typeVar, MSA& msa);
   TypeVarElement* lookUp(TypeVar typeVar) const;
   Bool hasElement(TypeVar typeVar) const;
   UInt nElements(Void) const;

   Void print(ostream& os, const Env& env) const;

   static Bool intersects(const TypeVarSet& tvs1, const TypeVarSet& tvs2);
   static Void merge(const TypeVarSet& tvs1, TypeVarSet& tvs2);

protected:
   TypeVarElement* _typeVarElements;

   friend class TypeCheck;
   friend class ModuleDefn;
   friend class Order;
   friend class ConstraintSet;
};

/*----------------------------------------------------------------------------*/

// TypedVal: A typed value

class TypedVal {
public:
   TypedVal(Expr val, TypeSig typeSig);
   TypedVal(const TypedVal& src,
            CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
            Tran& tran);

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline const Expr val(Void) const { return _val; };
   inline Expr& val(Void) { return _val; };
   inline const TypeSig typeSig(Void) const { return _typeSig; };
   inline TypeSig& typeSig(Void) { return _typeSig; };
   inline Void setVal(Expr val) { _val = val; };
   inline Void setTypeSig(TypeSig typeSig) { _typeSig = typeSig; };
   Subst* substs(Void) const { return _substs; }
   Subst*& substs(Void) { return _substs; }
   inline Bool schematic(Void) const { return _schematic; }
   inline Void setSchematic(Bool schematic) { _schematic = schematic; }
   inline TypeVarSet& schematicTypeVars(Void) { return _schematicTypeVars; }
   inline Bool needsInstance(Void) const { return _needsInstance; }
   inline Bool& needsInstance(Void) { return _needsInstance; }
   inline Void setNeedsInstance(Void) { _needsInstance = TRUE; }
   inline TypeEnv*& typeEnv(Void) { return _typeEnv; }

public:
   Void tran(BindingSet& bindingEnv, Tran& tran);
   Void addInstance(TypeSig& typeSig, TypeInsts* typeInsts, TypeEnv* typeEnv, TypeCheck& typeCheck);

   Void preAnalyse(Analyser& analyser);
   Void analyse(Analyser& analyser);

   Void reserve(Name name, Code& code);
   Void gen(Name name, CodeLabel& errLab, Code& code);
   Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   Expr			      _val;				         // Value
   TypeSig           _typeSig;		         // Type signature
   Subst*            _substs;			         // Substitutions in type signature
   Bool              _schematic;             // Indicates type is schematic
   Bool              _needsInstance;         // Indicates instance needed for mutual recursion
   SchemeConstraint* _schemeConstraint;      // Scheme constraint
   TypeVarSet        _schematicTypeVars;     // Schematic type variables
   TypeEnv*          _typeEnv;               // Type environment at definition

   friend class Decl;
   friend class Defn;
   friend class ClassDefn;
   friend class InstanceDefn;
   friend class NameTypedValBinding;
   friend class Lambda;
   friend class NameOcc;
   friend class VarPattern;
   friend class DataConPattern;
   friend class ConcreteDataCon;
};

/*----------------------------------------------------------------------------*/

// NameTypedValBinding: Binding between a name and a typed value

class NameTypedValBinding : public NameBinding<TypedVal> {
public:
   NameTypedValBinding(Name name, Expr val, TypeSig typeSig, MSA& msa);
   NameTypedValBinding(const NameTypedValBinding& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline const NameTypedValBinding* next(Void) const { return _next; };
   inline NameTypedValBinding*& next(Void) { return _next; };
   inline const TypedVal& typedVal(Void) const { return _val; };
   inline TypedVal& typedVal(Void) { return _val; };

public:
   ConstString nameString(const Env& env) const;
   Void addBinding(BindingSet& bindingEnv);
   Void tran(BindingSet& bindingEnv, Tran& tran, Bool addBinding = TRUE);
   Void insTypeSig(TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void unify(TypeSig, TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void typeCheck(TypeEnv* typeEnv, TypeSig failTS, TypeCheck& typeCheck);
   Void resetNeedsInstance(TypeEnv* typeEnv, TypeCheck& typeCheck);
   Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;

   Void preAnalyse(Analyser& analyser);
   Void analyse(Analyser& analyser);
   Bool needsRecReservation(Void) const;
   Void reserve(Code& code);
   Void reserveRec(Code& code);
   Void gen(CodeLabel& errLab, Code& code);
   Void print(Bool valFlag, ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   NameTypedValBinding* _next;		         // Next in binding set
   Bool						_count;              // Used for shared case alternatives

   static
      TypedVal* lookUp(Name name, NameTypedValBinding* nameTypedValBinding);

   friend class BindingSet;
   friend class ModuleDefn;
   friend class Defn;
   friend class Lambda;
   friend class Let;
   friend class NameOcc;
   friend class FnAp;
   friend class Tran;
   friend class TypeCheck;
};

// NameBindingSet: A set of name bindings

class NameBindingSet : public BasicChainedHashTable_<NameTypedValBinding,
                                                     Name, N_NAME_BINDING_HASH_TABLE_SLOTS> {
public:
   NameBindingSet(Void);

   Void print(ostream& os, const Env& env) const;

protected:
   virtual UInt hashVal(Name name) const { return name; }
   virtual NameTypedValBinding* next(NameTypedValBinding& node) const {
      return node.next();
   }
   virtual Name keyOf(const NameTypedValBinding& entry) const {
      return entry.name();
   }

};

class BindingSet {
public:
   BindingSet(BindingSet* next = NULL, Lambda* _lambda = NULL);
public:

   Void add(NameTypedValBinding& binding);
   TypedVal* lookUp(Name name, Bool dataCon, Bool nested = TRUE) const;

   inline Lambda* lambda(Void) { return _lambda; }
   Void print(ostream& os, const Env& env) const;

protected:
   BindingSet*			      _next;
   Lambda*						_lambda;
   NameTypedValBinding*		_bindings;

   NameBindingSet          _bindings_;

   friend class Tran;
   friend class ModuleDefn;
   friend class Lambda;
   friend class FnAp;
   friend class Let;
   friend class FatBarSeq;
   friend class Case;
};

// Name occurrence and coercion instances for a given type 

class TypeInsts {
public:
   TypeInsts(Void) : _nameOccInsts(NULL), _coercionInsts(NULL) {
   }

   NameOccInst*   _nameOccInsts;
   CoercionInst*  _coercionInsts;
};

// Name occurrence instance

class NameOccInst {
public:
   NameOccInst(NameOccInst* next,
               NameOccInst* parent,
               NameOcc* nameOcc, const ModuleDefn* nameOccModuleDefn,
               Subst* substs = NULL);
 
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline NameOccInst*& next(Void) { return _next; }
   inline NameOccInst* parent(Void) const { return _parent; }
   const NameOcc* nameOcc(Void) const { return _nameOcc; }
   NameOcc*& nameOcc(Void) { return _nameOcc; }
   const ModuleDefn* nameOccModuleDefn(Void) const { return _nameOccModuleDefn; }
   const ModuleDefn*& nameOccModuleDefn(Void) { return _nameOccModuleDefn; }
   inline Subst*& substs(Void) { return _substs; }
   inline const Subst* substs(Void) const { return _substs; }
   TypedVal* binding(Void) { return _binding; }
   const ModuleDefn* bindingModuleDefn(Void) const { return _bindingModuleDefn; }
   inline const TypeInsts* typeInsts(Void) const { return &_typeInsts; }
   inline TypeInsts* typeInsts(Void) { return &_typeInsts; }

   Void bind_(TypedVal* typedVal, const ModuleDefn* moduleDefn, Name mappedName);

protected:
   NameOccInst*      _next;
   NameOccInst*      _parent;
   NameOcc*          _nameOcc;
   const ModuleDefn* _nameOccModuleDefn;   // Name occurrence module
   Subst*            _substs;
   TypedVal*         _binding;
   const ModuleDefn* _bindingModuleDefn;   // Binding module
   Name              _mappedName;
   TypeInsts         _typeInsts;
};

/*----------------------------------------------------------------------------*/

// Abstract class for AST node

class AST_Node {
public:
   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrict);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);
   virtual Bool reduce(Expr& expr, Tran& tran);
   virtual Bool innerReduce(Tran& tran);

   virtual Void analyseValOf(TypeSig typeSig, Analyser& analyser);

   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Bool isConstReduced(Expr& val, const Code& code) const;
   virtual Operand& genOperand(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVar(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCond(UInt reduceN, CodeLabel& labT, CodeLabel& labF, 
                        Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genSelectReduced(TypeSig typeSig, Bool byPtr,UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

   Void notOverloaded(ConstString methodStr, const Env& env) const;

public:
};

// Variable reference

class VarRef {
public:
   VarRef(Var& var) : _var(var) {}

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Var& var(Void) { return _var; }

protected:
   Var& _var;
};

// Type definition

class TypeDefn {
public:
   TypeDefn(Name typeConName, Expr typeVars, Expr dataConDecls, Tran& tran, Bool objectFlag = FALSE);
   Void postConstruct(Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {
#else
   inline Void operator delete(Void * ptr) {
#endif
   }

   virtual Expr allocDataCon(Name name, Tag tag, TypeSig typeSig, MSA& msa);
   Expr addDataCon(Expr dataConDecl, TypeSig resTypeSig, Tag& tag, Tran& tran);
   Expr tran(Expr expr, Tran& tran);
   Void addBindings(BindingSet& bindingEnv, Tran& tran);
   Void addNilaryDataConBindings(Expr nilaryDataConDefn, BindingSet& bindingEnv, Tran& tran);
   Expr tran(Expr expr, BindingSet& bindingEnv, Tran& tran);
   Void tranNilaryDataConDefn(Expr nilaryDataConDefn, BindingSet& bindingEnv, Tran& tran);
   Void insTypeSigs(TypeCheck& typeCheck);
   Void insNilaryDataConDefnTypeSigs(Expr nilaryDataConDefn, TypeCheck& typeCheck);
   Void typeCheckNilaryDataConDefn(Expr nilaryDataConDefn, TypeCheck& typeCheck);
   Void typeCheck(TypeCheck& typeCheck);
   Void resetNilaryDataConDefnNeedsInstance(Expr nilaryDataConDefn, TypeCheck& typeCheck);
   Void resetNeedsInstance(TypeCheck& typeCheck);

   Void print(ostream& os, const Env& env, Bool pretty = FALSE) const;

protected:
   TypeSig  _typeCon;
   Bool     _objectFlag;
   Expr     _dataConDecls;
   Expr     _nilaryDataConDefns;    // Base definitions of nilary data constructors
                                    // may be redefined
};

// DataConDecl: A data constructor declaration
//              Essentially provides the type environment for the representation

class DataConDecl : public AST_Node {
public:
   DataConDecl(Name name, TypeSig typeSig, Tran& tran);
   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   virtual Void tran(Tran& tran);
   Void addBinding(BindingSet& bindingEnv, Tran& tran);
   virtual Void tran(BindingSet& bindingEnv, Tran& tran);
   Void insTypeSig(TypeEnv* typeEnv, TypeCheck& typeCheck);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

public:
   Name        _name;
   Expr        _reprDecl;
   TypedVal*   _dataConBinding;  // Data constructor binding
};

// DataConDefn:  Data constructor definition

class DataConDefn : public AST_Node {
public:
   DataConDefn(Expr reprDefn, Expr selectors, Bool isImplicit, Bool isInline);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   Void tranSelector(Expr& selector, Name name, UInt& index, Tran& tran);
   virtual Void tran(Tran& tran);
   virtual Void tran(BindingSet& bindingEnv, Tran& tran);
   Expr vars(UInt n, MSA& msa);
   Expr fnAp(Expr vars, Expr res, MSA& msa);
   Void addBindings(Expr dataConDefn, BindingSet& bindingEnv, Tran& tran);
   Void insTypeSigs(TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void typeCheck(TypeCheck& typeCheck);
   Void typeCheckSelector(Expr selector, UInt& index, TypeSig dataConTypeSig,
                          TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void resetNeedsInstance(TypeCheck& typeCheck);
   Void resetSelectorNeedsInstance(Expr selector, TypeCheck& typeCheck);

   Void preAnalyse(Analyser& analyser);
   Void analyse(Analyser& analyser);

   Void reserve(Code& code); 
   Void gen(CodeLabel& errLab, Code& code);

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

public:
   Expr        _reprDefn;        // Representation definition
   Expr        _selectors;       // Selector functions
   Name        _name;            // Data constructor name
   TypedVal*   _dataConBinding;  // Declaration binding
   Name        _reprName;        // Representation definition name

   Expr        _defn;            // DC = expr
   Bool        _isImplicit;      // True if implicit (redefinable)
	Bool			_isInline;		   // Applied to lambda values
};

// Concrete Data Constructor

class ConcreteDataCon : public AST_Node {
public:
   ConcreteDataCon(Expr dataCon);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   Void instantiate(TypeEnv* typeEnv, TypeCheck& typeCheck);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;
 
protected:
   Expr              _dataCon;            // Data constructor
   TypedVal*         _typedVal;           // Bound declaration unless primitive
   const ModuleDefn* _moduleDefn;         // Module definition if external
   TypeSig           _mappedTypeSig;      // Locally mapped type signature if external
   TypeSig           _typeSig;            // Instantiated type signature
   TypeSig           _reprTypeSig;        // Representation type signature
   TypeSig           _mappedReprTypeSig;  // Locally mapped representation type signature if external
   Subst*	         _substs;             // Substitutions applied
};

// Decl: The value of a name declaration

class Decl : public AST_Node {
public:
   Decl(Bool outermost);
   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   inline virtual UInt32 prevDeclOrDefnForm(Void) const { return DECL; };

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   Void insTypeSig(TypedVal& typedVal, TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void typeCheck(TypedVal& typedVal, TypeEnv* typeEnv, TypeSig failTS,
      TypeCheck& typeCheck);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Void gen(Name name, TypeSig typeSig, CodeLabel& errLab, Code& code);
   virtual Void genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

public:
   Bool              _outermost;
   TypedVal*	      _prevDeclOrDefn;  // Previous declaration or definition or NULL
   const ModuleDefn* _moduleDefn;      // External module or NULL
   Bool              _unifyWithPrevDeclOrDefn;
   Var*              _var;       // Used for an imported declaration
};

// Defn: The value of a definition

class Defn : public AST_Node {
public:
   enum RecInitStrategy {
      REC_INIT_DEFAULT,
      REC_INIT_RESERVED,
      REC_INIT_INVALID,
      REC_INIT_INDIRECTION
   };

   enum RecInitState {
      REC_STATE_READY,
      REC_STATE_PENDING,
      REC_STATE_ANALYSING
   };

   Defn(Expr expr, Bool outermost, Bool redefinable,
        Bool isInline, Bool exclusive);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   inline Expr& expr(Void) { return _expr; }
   inline Expr boundVal(Void) const { return _boundVal; }
   inline Bool outermost(Void) const { return _outermost; }
   inline Bool outer(Void) const { return _outer; }
   Void setOverridden(Void) { _overridden = TRUE; }
   inline virtual UInt32 prevDeclForm(Void) const { return DECL; };
   inline UInt minConstraintSeqNo(Void) const { return _minConstraintSeqNo; };
   inline UInt maxConstraintSeqNo(Void) const { return _maxConstraintSeqNo; };
   inline Lambda* parentLambda(Void) const { return _parentLambda; }
   inline RecInitStrategy recInitStrategy(Void) const { return _recInitStrategy; }
   inline RecInitState recInitState(Void) const { return _recInitState; }
   inline Bool recInitPending(Void) const {
      return _recInitState == REC_STATE_PENDING ||
             _recInitState == REC_STATE_ANALYSING;
   }
   inline Bool recInitAnalysing(Void) const {
      return _recInitState == REC_STATE_ANALYSING;
   }

   inline Void setMinConstraintSeqNo(UInt seqNo) { _minConstraintSeqNo = seqNo; };
   inline Void setMaxConstraintSeqNo(UInt seqNo) { _maxConstraintSeqNo = seqNo; };
   inline Void setRecInitStrategy(RecInitStrategy recInitStrategy) { _recInitStrategy = recInitStrategy; };
   inline Void setRecInitState(RecInitState recInitState) { _recInitState = recInitState; };

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);

   Void insTypeSig(TypedVal& typedVal, TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void typeCheck(TypedVal& typedVal, TypeEnv* typeEnv, TypeSig failTS, TypeCheck& typeCheck);
   Void resetNeedsInstance(TypedVal& typedVal, TypeEnv* typeEnv, TypeCheck& typeCheck);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);
   virtual Expr reduce(CodeLabel& failLab, Code& code);

   Lambda* directLambda(Void) const;
   Void preAnalyse(Analyser& analyser);
   Void analyse(TypeSig typeSig, Analyser& analyser);

   Void reserve(Expr& expr, Name name, TypeSig typeSig, Code& code);
   Void gen(Expr& expr, Name name, TypeSig typeSig, CodeLabel& errLab, Code& code);
   virtual Operand& genOperand(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVar(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

public:
   Expr              _expr;
   Expr              _boundVal;          // Generated binding value, if any
   Bool	            _outermost;          // True for outermost definition, false otherwise
   Bool	            _outer;              // True if outside of any lambda, false otherwise
   Bool	            _isInline;		      // Applied to lambda values
   Bool	            _exclusive;		      // True if limited to a single instance
   TypedVal*	      _prevDeclOrDefn;     // Previous declaration or definition or NULL
   const ModuleDefn* _moduleDefn;         // External module or NULL
   Bool              _unifyWithPrevDeclOrDefn;
   Bool              _redefinable;        // True if redefinable
   Bool              _overridden;         // True if overridden
   Bool              _translated;         // To support forward definitions
   UInt              _minConstraintSeqNo; // Minimum constraint sequence number
   UInt              _maxConstraintSeqNo; // Maximum constraint sequence number
   Lambda*	         _parentLambda;		   // Enclosing lambda
   RecInitStrategy   _recInitStrategy;     // Initialisation strategy for recursive local bindings
   RecInitState      _recInitState;        // Analysis state for recursive local bindings

   friend class InstanceDefn;
};

// Lambda: A lambda abstration

class Lambda : public AST_Node {
public:
   Lambda(UInt nFormalParams, NameTypedValBinding* formalParamV,
          Expr body, TypeSig typeSig = UNKNOWN, Bool updatable = TRUE);
   Lambda(const Lambda& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Lambda* parent(Void) const { return _parent; }
   inline UInt nFormalParams(Void) const { return _nFormalParams; }
   inline NameTypedValBinding* formalParamV(Void) const { return _formalParamV; }
   inline const Expr body(Void) const { return _body; }
   inline const TypeSig typeSig(Void) const { return _typeSig; }
   inline FreeVarAssoc* freeVarAssocs(Void) const { return _freeVarAssocs; }
   inline Var* closure(Void) const { return _closure; }
   inline UInt refCount(Void) const { return _refCount; }
   inline Bool needsClosure(Void) const { return _needsClosure; }
   inline Bool updatable(Void) const { return _updatable; }
  
   inline Void setNeedsClosure(Bool q) { _needsClosure = q; }
   inline Void setNoPartialAps(Bool q) { _noPartialAps = q; }

   Void adjRefCount(Int diff);

   Bool isReduction(Void) const;

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);
   Void substitutePair(Expr& expr, Expr* argV, UInt nArgs, Tran& tran);
   Void substituteList(Expr pair, Expr* argV, UInt nArgs, Tran& tran);
   Void substituteExpr(Expr& expr, Expr* argV, UInt nArgs, Tran& tran);
   Void substitute(Expr* argV, UInt nArgs, Tran& tran);
   Void removeUnusedFormalParams(Void);
   Expr instantiatePair(Expr expr, Expr* argV, UInt nArgs, const Subst* substs, Tran& tran) const;
   Expr instantiateList(Expr pair, Expr* argV, UInt nArgs, const Subst* substs, Tran& tran) const;
   Expr instantiateExpr(Expr expr, Expr* argV, UInt nArgs, const Subst* substs, Tran& tran) const;
   Expr instantiate(Expr* argV, UInt nArgs, TypeSig typeSig, Tran& tran) const;
   Bool reduce(Expr& expr, Tran& tran);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);

   Bool hasAscendant(const Lambda* lambda) const;

   Void analyseBinding(NameOcc* namOcc,
                       TypedVal* typedVal, 
                       const ModuleDefn* moduleDefn,
                       Lambda* lambda,
                       Bool isGlobal,
                       Bool needsClosure,
                       Bool needsClosureValue,
                       FreeVarAssoc*& parentFreeVarAssoc,
                       Analyser& analyser);
 
   Void analyse(TypeSig typeSig, Analyser& analyser);
   virtual Void analyseValOf(TypeSig typeSig, Analyser& analyser);

   Void rebindFreeVarAssoc(Expr oldVal, Expr newVal);
   StructTemplate& allocFreeVars(UInt nSlots, Code& code, CodeLabel& failLab);
   CodeLabel* genMapMethod(Var& closure, CodeLabel& failLab, Code& code);

#if (SERIALISATION==1)
   CodeLabel* genSerialiseMethod(Var& closure, Name argName, Name fnName,
                                 TypeSig typeSig,
                                 CodeLabel& failLab, Code& code);
#endif

#if (GARBAGE_COLLECTION==1)
   CodeLabel* genMarkMethod_GC(Var& closure, CodeLabel& failLab, Code& code);
#endif

   Void genFreeVars(UInt nSlots, Var** slotVars, CodeLabel& failLab, Code& code) const;
   Void copyFreeVars(UInt nSlots, Var** slotVars, Var& closure, Code& code) const;
   NameTypedValBinding* allocCurried(UInt arity, CodeLabel* mainLabel,
                                     Code& code);
   Void setupClosureCell(StructTemplate* structTemplate,
                         CodeLabel* mainLabel, Code& code);
   Expr partial(UInt n, TypedVal* closure, CodeLabel& codeLab, Code& code);
   Void genPartialApFn(CodeLabel& codeLabel,
                       UInt nArgs,
                       CodeLabel& failLab,
                       Code& code);
   Expr curried(UInt nArgs, NameTypedValBinding* curriedFormalParamV,
                CodeLabel& mainLabel, Code& code);
   Void genCurriedApFn(UInt nArgs,
                       NameTypedValBinding* curriedFormalParamV,
                       CodeLabel& codeLabel,
                       CodeLabel& mainLabel,
                       CodeLabel& failLab,
                       Code& code);
   CodeLabel* genUpdateContinuation(Cell* updateCell,
                                Code& code);
   Void genPartialApEntries(UInt arity,
                            NameTypedValBinding* curriedFormalParamV,
                            CodeLabel* mainLabel,
                            CodeLabel& failLab,
                            Code& code);
   Void genUpdate(Cell* updateCell,
                  CodeLabel& updateLabel,
                  CodeLabel& failLab,
                  Code& code);

   CellInfo_* genCellInfo(Var& var, StructTemplate* structTemplate,
      CodeLabel& failLab, Code& code);

   Void genClosure(Var& var, UInt nSlots, CellInfo_* cellInfo,
                   StructTemplate& structTemplate,
                   CodeLabel& failLab, Code& code);
   Void gen(Var* dst, CodeLabel& failLab, Code& code,
            Bool withArgCheck = TRUE,
            Bool simpleWithClosure = FALSE);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genEnter(const FnAp* fnAp, Code& code) const;
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);

//   Var* localFreeVar(const Var& localVar) const;
//   const FreeVarAssoc* freeVarAssoc(const Var& var) const;

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

public:
   Lambda*						_parent;             // Parent lambda
//   ReturnState*            _parentReturnState;  // Parent return state
   UInt							_nFormalParams;      // Number of formal parameters
   NameTypedValBinding* 	_formalParamV;       // Formal parameters
   Expr							_body;               // Body of lambda
   Bool							_isInline;           // Inline flag
   TypeSig						_typeSig;            // Type signature
   FreeVarAssoc*				_freeVarAssocs;      // List of free variable associations
   UInt                    _nNonGlobalFree;     // No. of non-global free variables
   CodeLabel*					_entryLabel;         // Entry label
   CodeLabel*					_altEntryLabel;      // Alternative entry label
   Cell*                   _closureCell;        // Closure cell
   Var*							_closure;            // Closure variable
   UInt                    _refCount;           // Reference count from variables and closures
   Bool                    _needsClosure;       // Flag to indicate need for a closure
   Bool                    _noPartialAps;       // Flag to indicate no partial applications
	Bool                    _updatable;          // Flag to indicate if updatable
   Bool							_simple;             // Simple flag
   Bool                    _isOrder;            // True if order, False otherwise
   ByteCodeSegment*			_segment;            // Code segment
};

// CurriedLambda: Represents a lambda function in a curried context

class CurriedLambda : public AST_Node {
public:
   CurriedLambda(Lambda* lambda, CodeLabel* mainLabel);

   inline Lambda* lambda(Void) const { return _lambda; }
   inline CodeLabel& mainLabel(Void) const { return *_mainLabel; }

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck,
      TypeSig failTS, Bool& nonStrictFlag);
   Void genEnter(const FnAp* fnAp, Code& code) const;
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);

protected:
   Lambda*     _lambda;
   CodeLabel*  _mainLabel;
};

// Let: An expression with local bindings

class Let : public AST_Node {
public:
   Let(Expr declOrDefns, Expr expr, Bool recursive = FALSE, Bool substitutable = FALSE);
   Let(const Let& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);
   virtual Bool reduce(Expr& expr, Tran& tran);
   virtual Bool innerReduce(Tran& tran);

   Void analyseValOf(TypeSig typeSig, UInt nReduce, Analyser& analyser);

   Void gen(CodeLabel& failLab, Code& code);
   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genSelectReduced(TypeSig typeSig, Bool byPtr,UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:

   Bool isSubstitutable(Name name);

   Expr							_declOrDefns;
   Expr							_expr;
	Bool							_recursive;
   Bool							_needsReservation;
   Bool							_substitutable;   // Indicates singleton definition
                                             // substitutable if single bound occurrence
   Lambda*						_lambda;

   friend class Tran;
   friend class Lambda;
   friend class FnAp;
};

class FnAp : public AST_Node {
public:
   FnAp(Expr fun, Expr arg, TypeSig typeSig, MSA& msa,
        Bool knownPartial = FALSE);
   FnAp(Expr fun, Expr* args, UInt nArgs, TypeSig typeSig, MSA& msa,
        Bool knownPartial = FALSE);
   FnAp(const FnAp& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   inline TypeSig typeSig(Void) const { return _typeSig; }
   inline Expr fun_(Void) const { return _fun; }
   inline Expr arg_(Void) const { return _argV[0]; }
   inline UInt nArgs(Void) const { return _nArgs; }
   inline const Expr* argV(Void) const { return _argV; }
   inline Bool knownPartial(Void) { return _knownPartial; }
   inline Void setNoCurry(Bool q) { _noCurry = q; }

   TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);
   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   Bool reduceLambdaAp(Expr& expr, UInt d, Tran& tran);
   virtual Bool innerReduce(Tran& tran);
   Expr tranDispatch(UInt i, BindingSet& bindingEnv, Tran& tran);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);
   virtual Bool reduce(Expr& expr, Tran& tran);

   Bool localFnNonPartial (Analyser& analyser, Bool& needsClosure) const;
   virtual Void analyseValOf(TypeSig typeSig, UInt nReduce, Analyser& analyser);

   Bool isPartial(Void) const;
   CodeNode* builtIn(const Code& code) const;
   Expr reduceNonBuiltIn(CodeLabel& failLab, Code& code);
   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Bool isConstReduced(Expr& val, const Code& code) const;
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genSelectReduced(TypeSig typeSig, Bool byPtr,UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   Void genArgs(Lambda* lambda, CodeLabel& failLab, Code& code);
   Void copyArgs(UInt from, UInt nArgs, Lambda* lambda, Code& code) const;
   Void copyArgs(Code& code) const;
   Void enterPreparedResult(Var& funVar, TypeSig funTypeSig,
      UInt from, UInt nArgs, Code& code) const;
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;
   Void removeArg(UInt index);
   Bool checkNArgs(Tran& tran);

protected:
   Expr        _fun;
   UInt        _nArgs;
   Expr*       _argV;
   TypeSig     _typeSig;
   Lambda*     _lambda;

   Var**       _argVars;
   Lambda*     _fnLambda;
   Bool        _knownPartial; // True for partial function closure (suppresses base register)
   Bool        _noCurry;      // True to suppress currying

   friend class Tran;
   friend class TypeCheck;
   friend class Let;
   friend class Lambda;
   friend class Let;
   friend class FatBarSeq;
   friend class Coerced;
   friend class Constructor;
   friend class Deconstructor;
   friend class DataConDefn;
   friend class InstanceDataConDefn;
   friend class AnyConstructor;
   friend class PtrConstructor;
   friend class TupleConstructor;
};

// CoercionInst: an instance of a coercion

class CoercionInst {
public:
   CoercionInst(CoercionInst* next, Coercion* coercion);

   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

	inline CoercionInst* next(Void) { return _next; }
   inline Coercion* coercion(Void) const { return _coercion; }
   inline Coercion*& coercion(Void) { return _coercion; }
   inline Bool& resolved(Void) { return _resolved; }
	inline Bool& applyCast(Void) { return _applyCast; }
	inline TypeSig& typeSig(Void) { return _typeSig; }

protected:
   CoercionInst*  _next;
   Coercion*      _coercion;
   Bool           _resolved;
   Bool           _applyCast;
   TypeSig        _typeSig;

   friend class Coercion;
   friend class Coerced;
   friend class AppLambda;
};

// Coercion: Abstract coercion

class Coercion : public AST_Node {
public:
   Coercion(Expr castOcc, Bool resolved = FALSE, Bool castFlag = FALSE,
            CoercionInst* instances = NULL);

	inline Expr castOcc(Void) { return _castOcc; }
	inline Bool& resolved(Void)  {return _resolved; }
	inline Bool& castFlag(Void) { return _castFlag; }

   CoercionInst* copyInst(CoercionInst* coercionInsts) const;

protected:
   Expr           _castOcc;      // 'cast' name occurrence
   Bool           _resolved;
   Bool           _castFlag;
	CoercionInst*  _instances;    // A list of instances
};

// Coerced: A coerced term

class Coerced : public Coercion {
public:
   Coerced(Expr expr, Expr castOcc);
   Coerced(const Coerced& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);
 	inline Expr& expr(Void) {return _expr; }

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
	Expr           _expr;
};

// AppLambda: Represents a lambda used in a fully applied context
// \p1 p2 ... pn -> e

// Introduced primarily to record the type information
// Subclassed from 'Coercion' for the applied argument

class AppLambda : public Coercion {
public:
   AppLambda(Expr patterns, Expr body, Expr castOcc, Bool castFlag);
   AppLambda(const AppLambda& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {
#else
      inline Void operator delete(Void* ptr) {
#endif
   }
	inline Expr& patterns(Void) { return _patterns; }
	inline Expr& body(Void) { return _body; }
   TypeSig& patTypeSig(void) { return _patTypeSig; }
   TypeSig typeSig(void) const { return _typeSig; }

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
	virtual TypeSig infer(TypeSig argTS, TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   Expr              _patterns;
   Expr              _body;
   TypeSig           _patTypeSig;
   TypeSig           _typeSig;

static AppLambda& fromExpr(Expr expr);

protected:
   friend class Tran;
	friend class FatBarSeq;
   friend class AppLambdaCoercionConstraint;
   friend class CaseConstraint;
   friend class AppLambdaCoercionConstraintCheckResult;
};

// FatBarSeq

// \(p11 p12 ... p1n.e1) a1 a2 ... an []
// \(p21 p22 ... p2n.e2) a1 a2 ... an []
// ...
// \(pm1 pm2 ... pmn.em) a1 a2 ... an

// Introduced primarily to record the argument type information

class FatBarSeq : public AST_Node {
public:
typedef enum  {SplitByType, SplitByConst, SplitByDataCon} SplitAltsKind;

public:
   FatBarSeq(Expr alts, Expr args, TypeSig patTypeSig = UNKNOWN);
   FatBarSeq(const FatBarSeq& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {
#else
      inline Void operator delete(Void* ptr) {
#endif
   }

   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);

	static Void altsPrefix(Expr alts, ExprPred pred, Expr& alts1, Expr& alts2);
   static Expr extKeyAltsList(Expr alt, Expr key, Expr keyAltsList, ExprEqFn eqFn, MSA& msa);
   static Expr splitAlts(Expr alts, Expr keyAltsList, FatBarSeq::SplitAltsKind kind, Tran& tran);

   Expr varAlt(Expr alt, Expr args, TypeSig typeSig, Tran& tran);
   Expr varAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran);
   Expr commonConst(Expr constPair, Expr args, TypeSig typeSig, Tran& tran);
   Expr constAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran);
   Expr commonDataCon(Expr dataConPair, Expr args, TypeSig typeSig, Tran& tran);
   Expr commonDataConAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran);
   Expr splitSameDataConAltsByHeadType(Expr alts, Tran& tran);
   Expr dataConAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran);
   Expr equalTypeAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran);
   Expr commonType(Expr typePair, Expr args, Tran& tran);
   Expr typeAlts(Expr alts, Expr args, UInt n, Tran& tran);
   Expr defaultLambdaAp(Expr patterns, Expr body, Expr args, Tran& tran);
   Expr defaultAlts(Expr alts, Expr args, Tran& tran);
   Bool mergeReducedArg(Tran& tran, Expr& res);
   Bool matchPatterns(Tran& tran, Expr& res);
   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   Void innerReduceAlt(Expr alt, Tran& tran);
   virtual Bool innerReduce(Tran& tran);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   Expr        _alts;
   Expr        _args;
   TypeSig     _patTypeSig;

protected:
   friend class Tran;
	friend class Lambda;
};

// Case: Represents a case expression

class Case : public AST_Node {
public:
   enum Kind {
      CASE_TYPE,
      CASE_TAG,
      CASE_CONST
   };

   Case(Kind kind, Expr arg, TypeSig argTypeSig, UInt n, Expr alts, TypeSig typeSig, Bool closed, MSA& msa);
   Case(const Case& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }

#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);
   virtual Bool innerReduce(Tran& tran);

   Void analyseValOf(TypeSig typeSig, UInt nReduce, Analyser& analyser);

   CodeLabel* genInit(ConstOperand**& consts, LabelOperand**& labels,
                      CodeLabel& failLab, Code& code);
   Void genFinal(CodeLabel* otherwiseLabel, CodeLabel& failLab, Code& code);
   virtual Expr reduce(CodeLabel& failLab, Code& code);

   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genSelectReduced(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   Kind			_kind;
   Expr			_arg;
   TypeSig		_argTypeSig;
   UInt			_n;
   Expr*			_altV;
   TypeSig		_typeSig;
   Bool			_closed;

	friend class Tran;
};

// NameOcc: A name occurrence

class NameOcc : public AST_Node {
public:
   NameOcc(Name name, TypedVal* typedVal = NULL, TypeSig typeSig = UNKNOWN);
   NameOcc(const NameOcc& src);
   NameOcc(const NameOcc& src, CopyContext
      * cc,
           const TypeInsts* typeInsts,
           TypedVal* typedVal, const ModuleDefn* moduleDefn,
           Bool noRebindOnCopy, 
           TypeSig typeSig,
           Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {
#else
	inline Void operator delete(Void* ptr) {
#endif
   }
	inline Name name(Void) const { return _binding.name(); }
	inline Name& name(Void) { return _binding.name(); }
	inline const TypedVal* typedVal(Void) const { return _binding.val(); }
	inline TypedVal*& typedVal(Void) { return _binding.val(); }
   inline const ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }
   inline const ModuleDefn*& moduleDefn(Void) { return _moduleDefn; }
   inline Name mappedName(Void) const { return _mappedName; }
   inline Subst* substs(Void) const { return _substs; }
   inline const TypeInsts* typeInsts(Void) const { return &_typeInsts; }
   inline TypeInsts* typeInsts(Void) { return &_typeInsts; }
   inline FreeVarAssoc* freeVarAssoc(Void) const { return _freeVarAssoc; }
   inline FreeVarAssoc*& freeVarAssoc(Void) { return _freeVarAssoc; }

	Expr val(Void) const;
	Expr typeSig(Void) const;
   Bool isSchematic(Void) const;

   Bool boundToGlobal(Void) const;

   Bool tranFinal(Expr& expr, BindingSet& bindingEnv, CopyContext* cc, Tran& tran);
   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   NameOccInst* copyInst(NameOccInst* nameOccInsts) const;
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Bool hasNameOcc(Name name, const Tran& tran) const;
   virtual UInt nameOccCount(const TypedVal* typedVal, const Tran& tran) const;
   virtual Void alphaConv(const TypedVal* binding, Name toName, const Tran& tran);
   virtual Void substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran);
   virtual Void substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran);
	ClassConstraint* inferCastMethod(TypeSig fromTS, TypeSig toTS, Bool indep,
                                    TypeEnv* typeEnv, TypeCheck& typeCheck);
   ClassConstraint* inferEqMethod(TypeSig typeSig,
                                  TypeEnv* typeEnv, TypeCheck& typeCheck);
   ClassConstraint* inferMethod(const ClassDefn& classDefn, Bool indep,
                                TypeEnv* typeEnv, TypeCheck& typeCheck);
   TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);

   Void analyse(Bool needsClosure, Bool needsPartialAps, Analyser& analyser);
   Void analyseValOf(Analyser& analyser);

   Expr val(Code& code) const;
	Void genExpr(Var& dst, CodeLabel& failLab, Code& code);
   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVar(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Operand& genOperand(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genEnter(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genCond(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                        Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genSelectReduced(TypeSig typeSig, Bool byPtr,UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

public:
	NameBinding<TypedVal*>	_binding;         // Name and binding
   const ModuleDefn*       _moduleDefn;      // Binding module definition, NULL if local or built-in
   Name                    _mappedName;      // Name mapped to binding module
   Bool                    _noRebindOnCopy;  // Rebind on copy unless true
   TypeSig						_typeSig;         // Type signature
   Subst*				      _substs;          // Substitutions applied
   TypeInsts               _typeInsts;       // Associated name occurrence and coercion instances
   FreeVarAssoc*           _freeVarAssoc;    // Free variable association record
};

// Abstract pattern
// Used primarily as a consequence of delaying case generation until after type checking

class Pattern : public AST_Node {
public:
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   virtual TypeSig typeSig(void) const = 0;
   virtual TypeSig& typeSig(void) = 0;

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) = 0;
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) = 0;
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const = 0;

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const = 0;

   friend class Tran;
};

// VarPattern: A variable pattern

class VarPattern : public Pattern {
public:
   VarPattern(Expr pattern, TypeSig typeSig, MSA& msa);
   VarPattern(const VarPattern& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   Name name(Void) const { return _binding.name(); }
   virtual TypeSig typeSig(void) const { return _binding.typedVal().typeSig(); }
   virtual TypeSig& typeSig(void) { return _binding.typedVal().typeSig(); }
  
   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   Void insTypeSig(TypeSig& typeSig, TypeEnv* typeEnv, TypeCheck& typeCheck);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);

   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   NameTypedValBinding	_binding;

   friend class Tran;
};

// Wildcard pattern

class WildCardPattern : public Pattern {
public:
   WildCardPattern(TypeSig typeSig);
   WildCardPattern(const WildCardPattern& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   virtual TypeSig typeSig(void) const { return _typeSig; }
   virtual TypeSig& typeSig(void) { return _typeSig; }
	
	virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   TypeSig  _typeSig;
};

// Constant pattern

class ConstPattern : public Pattern {
public:
   ConstPattern(Expr k, Expr castOcc, TypeSig typeSig);
   ConstPattern(const ConstPattern& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   Expr k(Void) const { return _k; };
   virtual TypeSig typeSig(void) const { return _typeSig;};
   virtual TypeSig& typeSig(void) { return _typeSig;};
	
	virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);

   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   Expr     _k;
	Expr		_eqOcc;		// equality occurrence for use with type inference
   TypeSig  _typeSig;

   friend class Tran;
	friend class FatBarSeq;
};

// Data constructor pattern

class DataConPattern : public Pattern {
public:
   DataConPattern(Expr dataCon, Expr patterns,
                  Bool isPlainDecon, Bool isPtrDecon,
                  TypeSig typeSig, Tran& tran);
   DataConPattern(const DataConPattern& src, CopyContext* cc,
                  const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
//	virtual Kind kind(Void) const { return DataCon; }
   inline Expr dataCon_(Void) const { return _dataCon; }
   inline Expr& patterns(Void) { return _patterns; }
   inline Bool isPlain(Void) const { return _isPlainDecon; }
   inline Bool isPtrDecon(Void) const { return _isPtrDecon; }

   virtual TypeSig typeSig(void) const { return _typeSig; }
   virtual TypeSig& typeSig(void) { return _typeSig; }
   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   Void instantiate(TypeEnv* typeEnv, TypeCheck& typeCheck);
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);
   TypeSig condTypeSig(TypeSig ts, MSA& msa);
   Expr deconstructP(Expr arg, TypeSig ts, Tran& tran);
   Expr deconstructS(Expr arg, Tag tag, TypeSig ts, Tran& tran);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   Expr              _dataCon;            // Data constructor  
   Expr              _patterns;           // Argument patterns
   Bool              _isPlainDecon;       // True if plain deconstruction, False otherwise
   Bool              _isPtrDecon;         // True if pointer deconstruction, False otherwise
   TypedVal*         _typedVal;           // Bound declaration unless primitive
   const ModuleDefn* _moduleDefn;         // Module definition if external
   TypeSig           _typeSig;            // Instantiated type signature
   TypeSig           _reprTypeSig;        // Instantiated representation type signature
   Subst*	         _substs;             // Substitutions applied
   
   friend class FatBarSeq;
   friend TypeSig patternTypeSig(Expr& pattern, Tran& tran);
};

// Concrete pattern

class ConcretePattern : public DataConPattern {
public:
   ConcretePattern(Expr dataCon, Expr pattern,
                   Bool isPlainDecon, Bool isPtrDecon,
                   TypeSig typeSig, Tran& tran);
   ConcretePattern(const ConcretePattern& src, CopyContext* cc,
                   const TypeInsts* typeInsts, Subst* substs, Tran& tran);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual TypeSig infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag);
   Expr deconstruct(Expr arg, TypeSig typeSig, Tran& tran);
   virtual Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;

   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   friend class FatBarSeq;
   friend TypeSig patternTypeSig(Expr& pattern, Tran& tran);
};

class SharedExpr : public AST_Node {
public:
   SharedExpr(Name name, Expr expr);
   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }
   Void sharedPattern(Expr pattern, MSA& msa);
   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   Name  _id;
   Expr  _expr;
   Expr  _patterns;

   friend class Tran;
   friend class SharedPatternAp;
};

class SharedPatternAp : public AST_Node
{
public:
   SharedPatternAp(SharedExpr& sharedExpr);
   inline Void* operator new(size_t size, Void* ptr)
   {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void tran(Expr& expr, BindingSet& bindingEnv, Tran& tran);
   virtual Void print(ostream& os, const Env& env, Bool pretty = FALSE, UInt indent = 0) const;

protected:
   SharedExpr& _sharedExpr;
};
   
// ModuleDefn: A module definition

class ModuleDefn {
public:
   ModuleDefn(Name name, Expr declOrDefns, Env* env);
   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Env& env(Void) { return *_env; }
   inline const Env& env(Void) const { return *_env; }
   inline Name name(Void) const { return _name; }
   inline NameTable& nameTable(Void) const { return _env->nameTable(); }
   inline const SubstSet& substs(Void) { return *_substs; }
   inline ConstraintSet*& constraints(Void) { return _constraints; }
   inline NameExprMap* typeConNameMap(Void) { return _typeConNameMap; }
   inline ClassInstancesMap& classInstancesMap(Void) { return _classInstancesMap; }

   inline BidirNameMap* varNameMaps(Void) const { return _varNameMaps; }
   inline BidirNameMap* typeVarMaps(Void) const { return _typeVarMaps; }
   inline ExportMap* exportMap(Void) { return _exportMap; }
   inline ModuleDefn* next(Void) { return _next; }

   inline Void setTypeConNameMap(NameExprMap* typeConNameMap) { _typeConNameMap = typeConNameMap; }
   inline Void setExportMap(ExportMap* exportMap) { _exportMap = exportMap;}
 
   Void tran(BindingSet& bindingEnv, Tran& tran);
   Void tranDeclOrDefn(Expr& declOrDefn, Tran& tran);
   Void addBinding(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran) const;
   Void tranDeclOrDefn(Expr& declOrDefn, BindingSet& bindingEnv, Tran& tran);
   Void insTypeSig(Expr declOrDefn, TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void typeCheck(TypeCheck& typeCheck);
   Void typeCheckDeclOrDefn(Expr declOrDefn, TypeEnv* typeEnv, TypeCheck& typeCheck);
   Void resetNeedsInstance(Expr declOrDefn, TypeEnv* typeEnv, TypeCheck& typeCheck);

   Void analyse(Analyser& analyser);
   Void preAnalyseDeclOrDefn(Expr declOrDefn, Analyser& analyser);
   Void analyseDeclOrDefn(Expr declOrDefn, Analyser& analyser);

   Bool gen(Code& code);
   Void reserveDeclOrDefn(Expr declOrDefn, Code& code);
   Void genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab, Code& code);
   Void print(ostream& os, const Env& env, Bool pretty = FALSE) const;

   Bool lookUpTypeCon(Name name, const Env& env, UInt hashVal, TypeSig& typeSig) const;
   TypedVal* lookUp(Name name, Bool dataCon, const Env& env,
                    Name& mappedNameRes) const;

   Void addClassDefn(ClassDefn& classDefn);
   ClassDefn* lookUpClass(Name name, const Env& env) const;
   Void addInstanceDefn(InstanceDefn& instanceDefn);
   InstanceDefn* lookUpInstances(Name className, const Env& env) const;
   Void addGlobal(Name name, UInt pos);

protected:
   Env* _env;
   Name                 _name;
   Expr                 _declOrDefns;

   NameExprMap*         _typeConNameMap;  // Type Constructor name map
   BindingSet           _bindings;        // Exported bindings

   ClassDefn*           _classDefns;      // Class definition list

   ClassInstancesMap    _classInstancesMap;
                                          // Class instances map

   SubstSet*		      _substs;
   ConstraintSet*	      _constraints;

   BidirNameMap*        _varNameMaps;     // List of variable name maps
   BidirNameMap*        _typeVarMaps;     // List of type variable maps

   ExportMap*           _exportMap;       // Exported name map
   
   ModuleDefn*          _next;            // next in list

public:
   static ModuleDefn*    workingSet;   // list of available modules

   friend class Order;
};

class Order {
public:
   Order(Expr sequence, Tran& tran);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   Void tranSequenceExpr(Expr& expr, Tran& tran) const;
   Void tran(BindingSet& bindingEnv, Tran& tran);
   Void typeCheck(TypeCheck& typeCheck);

   Void analyse(Analyser& analyser);

   Bool gen(Code& code);
   Void print(ostream& os, const Env& env, Bool pretty = FALSE) const;

public:
   Expr        _sequence;
	SubstSet*   _substs;
   BidirNameMap* _varNameMaps;      // List of variable name maps
   BidirNameMap* _typeVarMaps;      // List of type variable maps
};

// Code Snippet

class Snippet {
public:
   Snippet(Expr expr);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline Expr expr(Void) { return _expr; };
   inline TypeSig typeSig(Void) { return _typeSig; }

   Void tran(BindingSet& bindingEnv, Tran& tran);

   Void analyse(Analyser& analyser);

   Void typeCheck(TypeCheck& typeCheck);


   Void print(ostream& os, const Env& env, Bool pretty/* = FALSE*/) const;

protected:
   Expr           _expr;
   TypeSig        _typeSig;

   SubstSet* _substs;
   BidirNameMap* _varNameMaps;      // List of variable name maps
   BidirNameMap* _typeVarMaps;      // List of type variable maps
};

#endif /* IVORY_COMPILER_AST_H_DEFINED */
