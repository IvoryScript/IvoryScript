/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    type.h
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
 *    Declaration of 'TypeCheck' class and associated type inference functions
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

#ifndef IVORY_COMPILER_TYPE_H_DEFINED
#define IVORY_COMPILER_TYPE_H_DEFINED

// The following definition enables or disables instance priority dependent
// on the minimum number of subordinate casts (or coercion casts if equal)
//#define SUBORDINATE_AND_INSTANCE_CAST_PRIORITY

#include <string.h>
#include "ivory/tuple.h"
#include "AST.h"

#define N_SUBST_HASH_TABLE_SLOTS       997
#define N_SUBST_REF_HASH_TABLE_SLOTS   997

#define TYPE_VAR                          NAME 
#define NULL_TYPE_VAR                     NULL_NAME 
#define toTypeVar                         toName
#define fromTypeVar                       fromName
#define isTypeVar                         isName
#define typeSigAp(fun,arg,msa)            ((TypeSig)(ap(fun,arg,msa)))
#define typeSigAp2(fun,arg1,arg2,msa)     ((TypeSig)(ap2(fun,arg1,arg2,msa)))

#define mapAlphaTypeVar(name, src, dst)   mapName(name, src, dst)

enum TypePredicate {
   TYPE_PRED_FALSE,      
   TYPE_PRED_TRUE, 
   TYPE_PRED_UNDECIDABLE
};

#ifndef TYPE_VAR_DEFINED
typedef Name            TypeVar;
#define TYPE_VAR_DEFINED
#endif

struct TypeSigPair {
   TypeSig  _x;
   TypeSig  _y;
};

// Predicate function on a type variable

typedef Bool (*TYPE_VAR_PRED)(TypeVar, Void*);

// Procedure on a type variable

typedef Void (*TYPE_VAR_PROC)(TypeVar, Void*, MSA& msa);


// Arguments for: addConstraintSetToListByTypeVar: Adds dependent constraint sets to a list for a given type variable

typedef 	struct {
   ConstraintSet** _unresolvedList;
   ConstraintSet** _constraintSetList;
   const Env* _env;
} AddConstraintSetToListByTypeVar_Args;

class ClassConstraintTransform;
class ClassConstraint;
class Code;
class CoercionConstraint;
class CoercionConstraintTransform;
class Constraint;
class ConstraintSet;
class ConstraintTransform;
class CopyContext;
class ExcludedConstraint;
class ModuleDefn;
class ReductionConstraint;
class SchematicConstraint;
class SchematicInstanceConstraint;
class Subst;
class SubstSet;
class TypeCheck;
class TypeQual;
class TypeVarElement;
class TypeVarSet;

/*----------------------------------------------------------------------------*/

// TypeEquation: Represents a type equation
// For types x and y: x = y

class TypeEquation {
public:
   TypeEquation(TypeSig x, TypeSig y, TypeEquation* next) : _next(next), _x(x), _y(y) {}
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   Void subst(Name name, TypeSig ts);

protected:
   TypeEquation*	_next;
   TypeSig			_x;
   TypeSig			_y;

   friend class TypeCheck;
};

/*----------------------------------------------------------------------------*/

// Subst: Represents a mapping from a type variable to a type signature

class Subst {
public:
   Subst(TypeVar typeVar, TypeSig typeSig, Subst* next = NULL)
      : _next(next), _typeVar(typeVar), _typeSig(typeSig) {}
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   Void subst(Name name, TypeSig typeSig);
   inline Void subst(TypeVar typeVar) { _typeVar = typeVar; };
   inline Subst* next(Void) const { return _next; }
   inline Subst*& next(Void) { return _next; }
   inline TypeVar typeVar(Void) const { return _typeVar; }
   inline TypeVar& typeVar(Void) { return _typeVar; }
   inline TypeSig typeSig(Void) const { return _typeSig; }
   inline TypeSig& typeSig(Void) { return _typeSig; }
protected:
   Subst*  _next;
   TypeVar  _typeVar;
   TypeSig  _typeSig;

public:
   // TypeVarSet::nElement: Return no. elements in set

   static UInt length(const Subst* substs);
   static Subst* concat(Subst* substs1, Subst* substs2);
   static Subst* lookUp(TypeVar typeVar, Subst* substs);
   static const Subst* lookUp(TypeVar typeVar, const Subst* substs);
   static const Subst* lookUpRHS(TypeVar typeVar, const Subst* substs);
   static TypeVar lookUp(TypeSig typeSig, const Subst* substs);
   static Bool typeVarInSubsts(TypeVar, const Subst* substs);
   static Bool typeVarOccursInTypeSig(const Subst* substs, TypeSig typeSig);
   static Bool isTypeVarUnderSubsts(TypeSig typeSig, const Subst* substs);
   static Bool hasTypeVarUnderSubsts(TypeSig typeSig, const Subst* substs);
   static TypeVar firstTypeVar(TypeSig typeSig, TYPE_VAR_PRED pred, Void* arg);
   static TypeVar firstTypeVarInList(Expr typeSigList, TYPE_VAR_PRED pred, Void* arg);
   static Expr uniqueTypeSig(TypeSig typeSig, const CopyContext& cc,
                             TypeEnv* typeEnv, MSA& msa,
                             Bool instantiate = TRUE);
   static Expr uniqueTypeSigList(Expr typeSigList, const CopyContext& cc,
                                 TypeEnv* typeEnv, MSA& msa,
                                 Bool instantiate = TRUE);
   static Expr uniqueTypeSigLists(Expr typeSigLists, const CopyContext& cc,
                                  TypeEnv* typeEnv, MSA& msa);
   static Void mapTypeVars(Subst* substs, const CopyContext& cc, MSA& msa);
   static TypeSig copyTypeSig(TypeSig typeSig, CopyContext& cc, MSA& msa, const Subst* substs);
   static Void mapSubsts(TypeSig ts1, CopyContext& cc1, TypeSig ts2, CopyContext& cc2, Subst* substs, MSA& msa);
   static Void substTypeVar(TypeVar typeVar, TypeSig substTS, TypeSig& typeSig);
   static Subst* substSubsts(Subst* substs);
   static Void substSubsts(Subst* substs1, const Subst* substs2);
   static Void substSubsts(Subst* subst1, const SubstSet& subst2);
   static Void substTypeVars(TypeSig& typeSig, const Subst* substs);
   static Void substTypeVars(TypeSig& typeSig, const SubstSet& substs);
   static Void substTypeSigList(Expr typeSigList, const Subst* substs);
   static Void substTypeSigLists(Expr typeSigLists, const Subst* substs);
   static Void substTypeSigList(Expr typeSigList, const SubstSet& substs);
   static Void swap(Subst* substs);
   static Subst* copySubsts(const Subst* substs, MSA& msa);
   static Subst* copySubsts(const Subst* substs, const CopyContext& cc,
                            MSA& msa, Bool typeVarId = FALSE);
   static Subst* insSubsts(const Subst* substs, const CopyContext& cc,
                           TypeEnv* typeEnv, MSA& msa);
   static Subst* inverseSubsts(const Subst* substs, MSA& msa);
   static Void printSubstLines(const Subst* substs, ostream& os, const Env& env,
                               UInt& n, Bool shortForm = FALSE);
   static Void printSubsts(const Subst* substs, ostream& os, const Env& env,
                           Bool shortForm = FALSE, Bool noHeader = FALSE);

   friend class SubstIndex;
   friend class SubstRefIndex;
   friend class SubstSet;
   friend class TypeCheck;
   friend class TypeEnv;
   friend class NameTypedValBinding;
};

/*----------------------------------------------------------------------------*/

// TypeVarSubstElement: An element of a set of substitutions for a given
//                      type variable

class TypeVarSubstElement {
public:
   TypeVarSubstElement(TypeVar typeVar, Subst* subst)
      : _typeVar(typeVar), _subst(subst) {}
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   static TypeVarElement* lookUp(TypeVar typeVar, TypeVarElement* typeVars);

protected:
   TypeVar	_typeVar;
   Subst*   _subst;

friend class SubstRefIndex;
};

/*----------------------------------------------------------------------------*/

// SubstSet: A set of substitutions

class SubstIndex : public BasicChainedHashTable<Subst*, Subst, TypeVar> {
public:
   SubstIndex(UInt nSlots);

protected:
   virtual Subst* allocNode(Subst& entry, Subst* next, MSA& msa);
   virtual Subst* allocNode(size_t size, MSA& msa, Subst*& link);
   virtual Subst& linkToNode(Subst* link) const {
      return *(Subst*)link;
   }
   Subst* lookUp(TypeVar typeVar) const;
   virtual Subst* chain(Subst& Node, Subst* next) const;

   virtual Subst* next(Subst& entry) const;

protected:
   virtual Name keyOf(const Subst& entry) const { return entry._typeVar; }

friend class SubstSet;
};

class SubstRefIndex : public HashTable<Void*, TypeVarSubstElement, TypeVar> { 
public:
   SubstRefIndex(UInt nSlots);
   Void addTypeSig(TypeSig typeSig, Subst* subst, MSA& msa);
   Void addSubst(Subst* subst, MSA& msa);
   Bool lookUp(TypeVar typeVar, Subst* subst) const;
   Void lookUpAndSave(TypeVar typeVar);
   Void substPending(Subst* substs, Bool nested, MSA& msa);

protected:
   Void addUnique(TypeVar, Subst* subst, MSA& msa);
   virtual Void* allocNodePtr(size_t size, MSA& msa, Void*& link);
   virtual HashTableNode<Void*, TypeVarSubstElement, TypeVar>& linkToNode(Void* link) const {
      return *(HashTableNode<Void*, TypeVarSubstElement, TypeVar>*)link;
   }

   Void freeNode(HashTableNode<Void*, TypeVarSubstElement, TypeVar>* node);

protected:
   inline TypeVar keyOf(const TypeVarSubstElement& entry) const { return entry._typeVar; }

protected:
   HashTableNode<Void*, TypeVarSubstElement, TypeVar>* _nodeSupply;
   HashTableNode<Void*, TypeVarSubstElement, TypeVar>* _pendingNodes;
};

class SubstSet {
public:
   SubstSet(Void);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline
   const Subst* lookUp(TypeVar typeVar) const {
      return _index.lookUp(typeVar);
   }
   Void preSubst(Subst* substs);
   Void addSubsts(Subst* substs, MSA& msa);
   Void print(ostream& os, const Env& env, Bool shortForm = FALSE) const;

protected:
   SubstIndex     _index;
   SubstRefIndex  _refIndex;
   UInt           _nestingLevel;
};

/*----------------------------------------------------------------------------*/

// TypeEnv:  A type environment to map type variables

class TypeEnv {
public:
   TypeEnv(TypeEnv* next = NULL, Subst* const substs = NULL);

   inline const TypeEnv* next(Void) const { return _next; }
   inline Subst* substs(Void) const { return _substs; }
   inline Subst*& substs(Void) { return _substs; }

   const Subst* lookUp(TypeVar typeVar) const;
   Void addSubst(Subst* subst);
   Void substSubsts(const SubstSet& substs);
   Void addTypeSig(TypeSig typeSig, MSA& msa);
   Void removeTypeSig(TypeSig typeSig, MSA& msa);
   Void substTypeVars(const Subst* substs, MSA& msa);
   Bool typeVarOccurs(TypeVar typeVar) const;

   Void printSubsts(ostream& os, const Env& env) const;
   Void printTypeVars(ostream& os, const Env& env) const;
   Void print(ostream& os, const Env& env) const;

protected:
   TypeEnv*    _next;            // Outer type environment
   Subst*      _substs;
   TypeVarSet  _typeVars;        // Assumptions type variables

   friend class TypedVal;
   friend class TypeCheck;
   friend class NameOcc;
   friend class DataConPattern;
   friend class Subst;
};

/*----------------------------------------------------------------------------*/

// ConstraintTransform: Abstract transformation for a satisfied constraint

class ConstraintTransform {

public:
   ConstraintTransform(ConstraintTransform* next, Constraint* constraint)
      : _next(next), _constraint(constraint) {
   }

   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   ConstraintTransform*& next(Void) { return _next; }
   Constraint& constraint(Void) const { return *_constraint; }
   Constraint*& constraint(Void) { return _constraint; }

   virtual Void apply(const Subst* substs, Constraint*& constraints,
                      TypeEnv* typeEnv, TypeCheck& typeCheck) = 0;
   virtual Void print(ostream& os, const Env& env, Bool shortForm) const {}

protected:
   ConstraintTransform* _next;
   Constraint*          _constraint;
};

/*----------------------------------------------------------------------------*/

// Constraint: Abstract type constraint

extern UInt nextConstraintSeqNo(Bool step = TRUE);

class Constraint {

public:
   enum Kind {
      SCHEME_CONSTRAINT,               // Type scheme
      SCHEMATIC_INSTANCE_CONSTRAINT,   // Schematic instance
      REDUCTION_CONSTRAINT,            // Reduction
      COERCION_CONSTRAINT,             // Coercion
      CLASS_CONSTRAINT,                // Class
      CASE_CONSTRAINT                  // Case
   };


   class Result {
   public:
      enum Tag {
         SetIsEmpty,
         Unresolved,
         Satisfied,
         Fails
      };

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
      Result(Tag tag) {
         _tag = tag;
      }
#else
      Result(Tag tag, UInt nCast = 0, UInt nSubordinate = 0) {
         _tag = tag;
         _nCast = nCast;
         _nSubordinate = nSubordinate;
      }
#endif

      inline Tag tag(Void) const { return _tag; }

#ifdef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
      inline UInt nCast(Void) const { return _nCast; }
      inline UInt nSubordinate(Void) const { return _nSubordinate; }
#endif

   protected:
      Tag   _tag;

#ifdef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
      UInt  _nCast;
      UInt  _nSubordinate;
#endif

   };

public:
   Constraint(Kind kind, Bool indep = TRUE);

   inline Constraint*& next(Void) { return _next; }
   inline Kind kind(Void) const { return _kind; }
   inline Bool& indep(Void) { return _indep; }
   inline UInt seqNo(Void) const { return _seqNo; }
   inline ConstraintTransform* transforms(Void) { return _transforms; }

   virtual Void init(TypeCheck& typeCheck);

   Void appendTransforms(ConstraintTransform* transforms);
   virtual Void appendTransforms(Constraint& constraint);

   virtual Bool matches(const Constraint& constraint) = 0;
   virtual Void onMatched(const Constraint& constraint);
   virtual Void substTypeVars(const Subst* substs) = 0;
   virtual Void addConstraintSetTypeVars(Void* arg, MSA& msa) = 0;
   virtual Bool dependsOnSubsts(const Subst* substs) const = 0;
   virtual Bool isClassMethod(Name name) const { return FALSE; };

   virtual Constraint* instantiate(const ModuleDefn* moduleDefn,
                                   TypeEnv* typeEnv,
                                   TypeInsts* typeInsts,
                                   Bool unresolved,
                                   TypeCheck& typeCheck) = 0;
   virtual Void preCheck(Void);
   virtual Result check(Bool fix,
                        const ConstraintSet& constraintSet,
                        Subst** substs, // Not const due to unify
                        const ExcludedConstraint* excludes,
                        TypeEnv* typeEnv,
                        TypeCheck& typeCheck,
                        Constraint*& resolved) = 0;

   virtual Void revertSolution(Void);
   virtual Void onSatisfied(ConstraintSet& constraintSet, const Subst* substs,
                            TypeCheck& typeCheck);

   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:
   Constraint*          _next;
   Kind                 _kind;
   Bool                 _indep;
   UInt                 _seqNo;
   ConstraintTransform* _transforms;
   Constraint*          _replacement;

   friend class TypeCheck;
   friend class ConstraintSet;
   friend class NameOcc;
   friend class CoercionConstraintTransform;
   friend class AppLambdaCoercionConstraintTransform;
};

/*----------------------------------------------------------------------------*/

// ExcludedConstraint: An excluded constraint list element

class ExcludedConstraint {
public:
   ExcludedConstraint(const ExcludedConstraint* next, Constraint* constraint) :
      _next(next), _constraint(constraint) {}
protected:
   const ExcludedConstraint*	_next;
   Constraint*				      _constraint;

   friend class TypeCheck;
   friend class ClassConstraint;
};

/*----------------------------------------------------------------------------*/

// ConstraintElement: An element of a set of constraints

class ConstraintElement {
public:
   ConstraintElement(ConstraintElement* next, Constraint* constraint)
      : _next(next), _constraint(constraint) {}
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline const ConstraintElement* next(Void) const { return _next; }
   inline ConstraintElement*& next(Void) { return _next; }
   inline Constraint* constraint(Void) const { return _constraint; }
   inline Constraint*& constraint(Void) { return _constraint; }

protected:
   ConstraintElement*   _next;
   Constraint*		      _constraint;
};

/*----------------------------------------------------------------------------*/

static ConstraintElement* _freeConstraintElements = NULL;

// ConstraintSet: A set of constraints associated with a set of type variables.

class ConstraintSet {
public:
   ConstraintSet(ConstraintSet* next = NULL);
 
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline ConstraintSet*& next(Void) { return _next; }
   inline TypeVarSet& typeVarSet(Void) { return _typeVarSet; }
   inline ConstraintElement*& elements(Void) { return _elements; }
   inline ConstraintElement* const & elements(Void) const { return _elements; }
   inline UInt& nCoercion(Void) { return _nCoercion; }
   inline UInt nSchematic(Void) const { return _nSchematic; }
   inline UInt& nSchematic(Void) { return _nSchematic; }

   Void add(Constraint& constraint, MSA& msa);
   Void remove(Constraint& constraint);
   Bool seqNoInRange(UInt minSeqNo, UInt maxSeqNo) const;

   Void print(ostream& os, const Env& env, Bool shortForm = FALSE) const;

   static Void merge(ConstraintSet& cs1, ConstraintSet& cs2, MSA& msa);

protected:
   ConstraintSet*		   _next;
   TypeVarSet			   _typeVarSet;
   ConstraintElement*   _elements;
   UInt                 _nCoercion;
   UInt                 _nSchematic;

   friend class TypeCheck;
   friend class ModuleDefn;
   friend class Order;
   friend class CMConstraintTransform;
};


/*----------------------------------------------------------------------------*/

// SchemeConstraint: Type scheme constraint
// In combination with SchematicInstanceConstraint
// Constrains instances of type schemes

class SchemeConstraint : public Constraint {
public:
   SchemeConstraint(TypeSig typeSig, TypeEnv* typeEnv, MSA& msa);

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeSig& typeSig(Void) { return _typeSig; }
   inline SchematicInstanceConstraint* instances(Void) const { return _instances; }
   Bool& substFlag(Void) { return _substFlag; }

   Void addInstance(SchematicInstanceConstraint& instance);
   Void removeInstance(SchematicInstanceConstraint& instance);

   virtual Bool matches(const Constraint& constraint);

   Void substTypeVars(const Subst* substs);
   Void onSchemeSubst(TypeEnv* typeEnv, Constraint*& constraints, TypeCheck& typeCheck);
   Void addConstraintSetTypeVars(Void* arg, MSA& msa);
   virtual Bool dependsOnSubsts(const Subst* substs) const;
   virtual Constraint* instantiate(const ModuleDefn* moduleDefn,
                                   TypeEnv* typeEnv,
                                   TypeInsts* typeInsts,
                                   Bool unresolved,
                                   TypeCheck& typeCheck);
   virtual Result check(Bool fix,
                        const ConstraintSet& constraintSet,
                        Subst** substs,
                        const ExcludedConstraint* excludes,
                        TypeEnv* typeEnv,
                        TypeCheck& typeCheck,
                        Constraint*& resolved);

   Bool checkInstances(Subst** substs,
      TypeEnv* typeEnv,
      TypeCheck& typeCheck,
      UInt& nI, UInt& nU);

   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:

   Void minimise(Void);

   TypeSig				            _typeSig;
   SchematicInstanceConstraint*  _instances;
   TypeEnv*                      _typeEnv;

// The following flag is set True on a substitution for one or more type
// variables of the type scheme.

   Bool                          _substFlag;
};

/*----------------------------------------------------------------------------*/

// SchematicInstanceConstraint: Represents an instance of a type scheme

class SchematicInstanceConstraint : public Constraint {
public:
   SchematicInstanceConstraint(SchemeConstraint* schemeConstraint,
                               TypeSig typeSig, TypeInsts* typeInsts);

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline SchemeConstraint*& scheme(Void) { return _schemeConstraint; }
   inline SchematicInstanceConstraint* nextInstance(Void) const { return _nextInstance; }
   inline SchematicInstanceConstraint*& nextInstance(Void) { return _nextInstance; }
   inline TypeSig& typeSig(Void) { return _typeSig; }
   inline TypeInsts* typeInsts(Void) { return _typeInsts; }

   virtual Bool matches(const Constraint& constraint);
   virtual Void onMatched(const Constraint& constraint);
   Void substTypeVars(const Subst* substs);
   virtual Void addConstraintSetTypeVars(Void* arg, MSA& msa);
   virtual Bool dependsOnSubsts(const Subst* substs) const;

   virtual Constraint* instantiate(const ModuleDefn* moduleDefn,
                                   TypeEnv* typeEnv,
                                   TypeInsts* typeInsts,
                                   Bool unresolved,
                                   TypeCheck& typeCheck);
   virtual Result check(Bool fix,
                        const ConstraintSet& constraintSet,
                        Subst** substs, // Not const due to unify
                        const ExcludedConstraint* excludes,
                        TypeEnv* typeEnv,
                        TypeCheck& typeCheck,
                        Constraint*& resolved);

   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:
   SchemeConstraint*             _schemeConstraint;
   SchematicInstanceConstraint*  _nextInstance;
   TypeSig                       _typeSig;
   TypeInsts*                    _typeInsts;
};


/*----------------------------------------------------------------------------*/

// ReductionConstraint: Reduction constraint
// x reduces or evaluates to y

class ReductionConstraint : public Constraint {
public:
   ReductionConstraint(TypeSig x, TypeSig y, Bool evalFlag)
      : Constraint(Constraint::REDUCTION_CONSTRAINT), _x(x), _y(y), _evalFlag(evalFlag) {}

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeSig& x(Void) { return _x; }
   inline TypeSig& y(Void) { return _y; }

   virtual Bool matches(const Constraint& constraint);

   Void substTypeVars(const Subst* substs);
   Void addConstraintSetTypeVars(Void* arg, MSA& msa);
   virtual Bool dependsOnSubsts(const Subst* substs) const;
   virtual Constraint* instantiate(const ModuleDefn* moduleDefn,
                                   TypeEnv* typeEnv,
                                   TypeInsts* typeInsts,
                                   Bool unresolved,
                                   TypeCheck& typeCheck);
   virtual Result check(Bool fix,
                        const ConstraintSet& constraintSet,
                        Subst** substs,
                        const ExcludedConstraint* excludes,
                        TypeEnv* typeEnv,
                        TypeCheck& typeCheck,
                        Constraint*& resolved);

   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:
   TypeSig				_x;
   TypeSig				_y;
   Bool					_evalFlag;

   friend class TypeCheck;
};
/*----------------------------------------------------------------------------*/

// CoercionConstraint: Coercion constraint
// Constrains x to unify with y
// or [x, y] to be an instance of Cast

class CoercionConstraint : public Constraint {
public:
   CoercionConstraint(TypeSig x, TypeSig y, ClassConstraint* classConstraint);

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeSig& x(Void) {return _x; }
   inline TypeSig& y(Void) {return _y; }
   inline ClassConstraint& classConstraint(Void) { return *_classConstraint; }
   inline Bool applyCast(Void) const { return _applyCast; }
   inline Bool noFix(Void) const { return _noFix; }
   inline Void setNoFix(Void) { _noFix = TRUE; }

   virtual Void appendTransforms(Constraint& constraint);
   virtual Bool matches(const Constraint& constraint);
   virtual Void substTypeVars(const Subst* substs);
   virtual Void addConstraintSetTypeVars(Void* arg, MSA& msa);
   virtual Bool dependsOnSubsts(const Subst* substs) const;
   virtual Constraint* instantiate(const ModuleDefn* moduleDefn,
                                   TypeEnv* typeEnv,
                                   TypeInsts* typeInsts,
                                   Bool unresolved,
                                   TypeCheck& typeCheck);
   virtual Result check(Bool fix,
                        const ConstraintSet& constraintSet,
                        Subst** substs,
                        const ExcludedConstraint* excludes,
                        TypeEnv* typeEnv,
                        TypeCheck& typeCheck,
                        Constraint*& resolved);
   virtual Void revertSolution(Void);
   virtual Void onSatisfied(ConstraintSet& constraintSet, const Subst* substs,
                            TypeCheck& typeCheck);

   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:
   TypeSig			   _x;
   TypeSig			   _y;
   ClassConstraint*  _classConstraint;
   Bool              _applyCast;
   Bool              _prevApplyCast;
   Bool              _noFix;

friend class TypeCheck;
friend class Coercion;
friend class CoercionConstraintTransform;
friend class CoercionConstraintTransform;
};

/*----------------------------------------------------------------------------*/

// AbstractCoercionConstraintTransform: Result data for an abstract coercion constraint

class AbstractCoercionConstraintTransform : public ConstraintTransform {

public:
   AbstractCoercionConstraintTransform(ConstraintTransform* next,
                                      Constraint& constraint)
      : ConstraintTransform(next, &constraint) {
        }

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual CoercionInst* coercionInst(Void) = 0;
   virtual Coercion& coercion(Void) = 0;

   virtual Void apply(const Subst* substs, Constraint*& constraints,
                      TypeEnv* typeEnv, TypeCheck& typeCheck);

   virtual Void resolve(Bool applyCast, TypeSig typeSig, MSA& msa) = 0;
};

// CoercionConstraintTransform: Result data for an instantiated coercion constraint

class CoercionConstraintTransform : public AbstractCoercionConstraintTransform {
  
public:
   CoercionConstraintTransform(ConstraintTransform* next, Constraint& constraint,
                               Coercion& coercion)
      : AbstractCoercionConstraintTransform(next, constraint), _coercion(coercion) {
        }

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size); return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual CoercionInst* coercionInst(Void) { return NULL; }
   virtual Coercion& coercion(Void) { return _coercion; }

   virtual Void resolve(Bool applyCast, TypeSig typeSig, MSA& msa);

public:
   Coercion& _coercion;
};

// InstantiatedCoercionConstraintTransform: Result data for an instantiated coercion constraint

class InstantiatedCoercionConstraintTransform : public AbstractCoercionConstraintTransform {

public:
public:
   InstantiatedCoercionConstraintTransform(ConstraintTransform* next,
                                           Constraint& constraint,
                                           Coercion& coercion,
                                           TypeInsts* typeInsts);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size); return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual CoercionInst* coercionInst(Void) { return &_coercionInst; }
   virtual Coercion& coercion(Void) { return *_coercionInst.coercion(); }

   virtual Void resolve(Bool applyCast, TypeSig typeSig, MSA& msa);

public:
   CoercionInst _coercionInst;
};


// CoercedConstraintTransform: Result data for an instantiated coerced constraint

class CoercedConstraintTransform : public CoercionConstraintTransform {

public:
   CoercedConstraintTransform(ConstraintTransform* next,
                              Constraint& constraint,
                              Coerced& coerced)
      : CoercionConstraintTransform(next, constraint, coerced) {
   }

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size); return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual CoercionInst* coercionInst(Void) { return NULL; }

   virtual Void resolve(Bool applyCast, TypeSig typeSig, MSA& msa);

};


/*----------------------------------------------------------------------------*/

// CaseConstraint: Case constraint
// Used to coerce the argument to the pattern

class CaseConstraint : public Constraint {
public:
   CaseConstraint(TypeSig x, Expr xs)
      : Constraint(CASE_CONSTRAINT), _x(x), _xs(xs) {
   }

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size); return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeSig& condTypeSig(Void) { return _x; }
   inline Expr& pats(Void) { return _xs; }

   virtual Bool matches(const Constraint& constraint);

   Void substTypeVars(const Subst* substs);
   Void addConstraintSetTypeVars(Void* arg, MSA& msa);
   virtual Bool dependsOnSubsts(const Subst* substs) const;
   virtual Constraint* instantiate(const ModuleDefn* moduleDefn,
                                   TypeEnv* typeEnv,
                                   TypeInsts* typeInsts,
                                   Bool unresolved,
                                   TypeCheck& typeCheck);
   virtual Result check(Bool fix,
                        const ConstraintSet& constraintSet,
                        Subst** substs,
                        const ExcludedConstraint* excludes,
                        TypeEnv* typeEnv,
                        TypeCheck& typeCheck,
                        Constraint*& resolved);

   virtual Void print(ostream& os, const Env& env, Bool shortForm) const;

protected:
   TypeSig  _x;
   Expr	   _xs;

   friend class TypeCheck;
};

/*----------------------------------------------------------------------------*/

// CaseConstraintTransform: Result data for a case constraint

class CaseConstraintTransform : public ConstraintTransform {

public:
   CaseConstraintTransform(ConstraintTransform* next, Constraint& constraint)
      : ConstraintTransform(next, &constraint), _applyCast(FALSE) {
   }

   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void apply(const Subst* substs, Constraint*& constraints,
                      TypeEnv* typeEnv, TypeCheck& typeCheck) = 0;

protected:
   Bool  _applyCast;
};


/*----------------------------------------------------------------------------*/

// Predicates: Type predicates

class Predicates {
public:
   Predicates(Void) :
      _substs(NULL),
      _varNameMaps(NULL), _typeVarMaps(NULL),
      _workList(NULL), _unresolvedList(NULL) {
   }

   inline BidirNameMap*& varNameMaps(Void) { return _varNameMaps; }
   inline BidirNameMap*& typeVarMaps(Void) { return _typeVarMaps; }
   inline ConstraintSet*& workList(Void) { return _workList; }
   inline ConstraintSet*& unresolvedList(Void) { return _unresolvedList; }

protected:
   SubstSet*		   _substs;
   BidirNameMap*     _varNameMaps;     // List of variable name maps
   BidirNameMap*     _typeVarMaps;     // List of type variable maps
   ConstraintSet*	   _workList;
   ConstraintSet*	   _unresolvedList;

   friend class TypeCheck;
   friend class ModuleDefn;
   friend class Order;
   friend class Snippet;
   friend class NameOcc;
   friend class CMConstraintTransform;
   friend class NameTypedValBinding;
};

/*----------------------------------------------------------------------------*/

// DeferredUnification: Deferred unification

class DeferredUnification {
public:
   DeferredUnification(DeferredUnification* next, TypeSig ts1, TypeSig ts2) :
      _next(next), _ts1(ts1), _ts2(ts2) {
   }

   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline DeferredUnification* next(Void) const { return _next; }
   inline TypeSig ts1(Void) const { return _ts1; }
   inline TypeSig ts2(Void) const { return _ts2; }

protected:
   DeferredUnification* _next;
   TypeSig              _ts1;
   TypeSig              _ts2;
};

/*----------------------------------------------------------------------------*/

// TypeCheck: Type Checker state

class TypeCheck {
public:
   TypeCheck(ConstString options, Env& env, ModuleDefn* moduleDefn = NULL);
   inline Env& env(Void) const { return _env; }
   inline MSA& msa(Void) const { return _env.msa(); }
   inline NameTable& nameTable(Void) const { return _env.nameTable(); }
   inline TypeTable& typeTable(Void) const { return _env.typeTable(); }
   inline Predicates& predicates(Void) { return _predicates; }
   inline SubstSet*& substs(Void) { return _predicates._substs; }
   inline SubstSet* const & substs(Void) const { return _predicates._substs; }
   inline ModuleDefn* moduleDefn(Void) { return _moduleDefn; }
   inline Name castName(Void) const { return _castName; }

   TypeSig typeConSig(Name name);
   Void bindTypeConNames(TypeSig& typeSig);
   Void bindTypeConNamesInList(Expr typeSigs);
   Void bindTypeConNamesInLists(Expr typeSigLists);
   Name useName(ConstString nameString) const;
   TypeVar newTypeVar(TypeVarSet* typeVarSet) const;
   TypeSig typeSignature(Type type) const;
   TypeSig reducedTypeSig(TypeSig typeSig, Bool evalFlag, TypeEnv* typeEnv);
   Void insOccTypeSig(TypeSig& typeSig, const TypeVarSet& schematicTypeVars,
                      TypeEnv* typeEnv, TypeVarSet* typeVars);
   Void instantiate(TypeSig& typeSig,
                    TypeVarSet& schematicTypeVars,
                    Bool withConstraints,
                    TypedVal* typedVal,
                    TypeInsts* typeInsts,
                    const ModuleDefn* moduleDefn, TypeEnv* typeEnv);
   Void fixCoercions(TypeSig typeSig, TypeEnv* typeEnv,
                     UInt minSeqNo, UInt maxSeqNo);
   Void unifyOrError(TypeSig ts1, TypeSig ts2, TypeEnv* typeEnv,
                     const char* errStr);

   Bool unify(TypeSig ts1, TypeSig ts2, TypeEnv* typeEnv,
              Subst** const substs = NULL,
              Bool instanceOfScheme = FALSE, Bool forInstance = FALSE);
   Bool unifyTypeLists(Expr typeList1, Expr typeList2, TypeEnv* typeEnv,
                       Subst** const substs = NULL);
   Void deferUnification(TypeSig ts1, TypeSig ts2);
   Void unifyDeferred(TypeEnv* typeEnv);

   Void genInstanceSubsts(TypeSig ts1, TypeSig ts2, TypeEnv* typeEnv, Subst*& substs);


   Subst* instanceSubsts(Subst* substs1, Subst* substs2, TypeEnv* typeEnv);

   Bool isNonSchematic(TypeVar typeVar, const TypeEnv& typeEnv) const; 
   Void setSchematicTypeVars(TypeSig typeSig, TypeVarSet& schematicTypeVars, const TypeEnv* typeEnv) const;
   Void resetSchematicTypeVars(TypeSig typeSig, TypeVarSet& schematicTypeVars, const TypeEnv* typeEnv) const;
   Bool inAnySchematicInstanceConstraint(TypeVar typeVar) const;

//   Subst* schematicInstance(TypeSig instance, TypeSig scheme, TypeEnv* typeEnv);
//   Subst* schematicInstance2(TypeSig instance, TypeSig scheme, TypeEnv* typeEnv);
   Void addConstraintToSet(Constraint& constraint, Bool unresolved = FALSE);
   Void addConstraint(Constraint* constraint, TypeEnv* typeEnv, Bool unresolved = FALSE);
   Constraint::Result checkFirstConstraint(Bool fix,
                                           const ConstraintSet& constraintSet,
                                           Subst** substs,
                                           const ExcludedConstraint* excludes,
                                           TypeEnv* typeEnv,
                                           Constraint*& resolved);
   Void checkConstraints(ConstraintSet& constraints, Bool unifyCastFlag,
                         TypeEnv* typeEnv);
   Void checkConstraintsWorkList(Bool unifyCastFlag, TypeEnv* typeEnv);
   Void substConstraints(Subst* substs, TypeEnv* typeEnv);
   Void applySubsts(Subst* substs, TypeEnv* typeEnv);
   Void applySubstsToConstraints(Subst* substs);
   TypeSig primOcc(const char* id);
   Void insTypeSig(Expr expr, TypedVal& typedVal, TypeEnv* typeEnv);
   TypeSig inferPair(Expr pair, TypeEnv* typeEnv, TypeSig failTS);
   TypeSig inferTriple(Expr triple, TypeEnv* typeEnv, TypeSig failTS);
   TypeSig inferTuple(UInt n, TypeSig resTS);
   TypeSig infer(Expr expr, TypeEnv* typeEnv, TypeSig failTS, Bool strict = FALSE);
   Void insDeclOrDefnTypeSig(Expr declOrDefn, TypeEnv* typeEnv);
   Void insDeclOrDefnsTypeSigs(Expr declOrDefns, TypeEnv* typeEnv);
   Void typeCheck(Expr expr, TypedVal& typedVal, TypeEnv* typeEnv, TypeSig failTS);
   Void typeCheckDeclOrDefn(Expr declOrDefn, TypeEnv* typeEnv, TypeSig failTS);
   Void typeCheckDeclOrDefns(Expr declOrDefns, TypeEnv* typeEnv, TypeSig failTS);
   Void removeSchematicConstraints(Void);
   Void resetNeedsInstance(Expr expr, TypedVal& typedVal, TypeEnv* typeEnv);
   Void resetDeclOrDefnNeedsInstance(Expr declOrDefn, TypeEnv* typeEnv);
   Void resetDeclOrDefnsNeedsInstance(Expr declOrDefns, TypeEnv* typeEnv);
   Bool typeCheck(Expr expr);

   Void printConstraints(ostream& os, Bool shortForm = FALSE);

   Void error(const char* s);

protected:
   ConstString          _options;
   UInt                 _nErrors;
   Env&                 _env;
   ModuleDefn*          _moduleDefn;
   Predicates		      _predicates;
   DeferredUnification* _deferredUnifications;
   Name                 _castName;
   TypeEquation*        _freeTypeEquationList;

   friend class ModuleDefn;
   friend class TypeDefn;
   friend class Defn;
   friend class Sequence;
   friend class Order;
   friend class Snippet;
   friend class Constraint;
   friend class NameTypedValBinding;
};

extern TypeVar typeVarSupply;

// Global function declarations

extern Void addConstraintSetToListByTypeVar(TypeVar typeVar, Void* arg, MSA& msa);

extern Void addConstraintSetTypeVar(TypeVar typeVar, Void* arg, MSA& msa);

extern TypeSig arrowTypeSig(TypeSig fun, TypeSig arg, MSA& msa);

extern TypeSig compTypeSig(UInt index, TypeSig typeSig, UInt arity);

extern TypeSig denotedTypeSig(TypeSig typeSig);

extern TypeSig evalTypeSig(TypeSig fun);

extern TypeSig expTypeSig(TypeSig typeSig, MSA& msa);

extern TypeVar firstTypeVar(TypeSig typeSig);

extern Bool hasTypeVar(TypeSig typeSig);

extern Bool hasTypeVarInList(Expr typeSigList);

extern Void insTypeSig(TypeSig& typeSig, TypeEnv* typeEnv, TypeVarSet* typeVars, MSA& msa);

extern TypePredicate isExprTypeSig(TypeSig typeSig, Subst* substs = NULL);

extern Void mapProcTypeVars(TypeSig typeSig, TYPE_VAR_PROC proc, Void* arg, MSA& msa);

extern TypeVar mapTypeVar(TypeVar typeVar, const CopyContext& cc, MSA& msa);

extern TypeSigPair minimalForm(TypeSig typeSig1, TypeSig typeSig2);

extern TypeSig mutableTypeSig(TypeSig typeSig, MSA& msa);

extern TypeSig mutableTypeSig(TypeSig typeSig, const CopyContext& cc, MSA& msa);

extern TypeVar newTypeVar(Void);

extern size_t packTypeSig(TypeSig typeSig, unsigned char* buffer, size_t size,
                          Code& code, Bool useNames = TRUE);

extern TypeSig plainTypeSig(TypeSig typeSig, MSA& msa);

extern TypeSig ptrTypeSig(TypeSig typeSig, MSA& msa);

extern TypeSig reprTypeSig(TypeSig typeSig, Tag tag, MSA& msa);

extern Void resetTypeVarSupply(Void);

extern TypeSig thunkTypeSig(TypeSig typeSig, MSA& msa);

extern Bool typeIsDynamic(TypeSig typeSig);

//extern Bool typeNeedsEnv(Type type, TypeTable& typeTable);

extern Bool typeNeedsEnv(TypeSig typeSig, TypeTable& typeTable,
                                          NameTable& nameTable);

extern Repr typeRepr(TypeSig typeSig, TypeTable& typeTable,
                                      NameTable& nameTable);

extern ReprInfo typeReprInfo(TypeSig typeSig, TypeTable& typeTable,
                                              NameTable& nameTable);

extern Void typeVarsInTypeSig(TypeSig typeSig, TypeVarSet& typeVars, MSA& msa);

extern Bool typeVarOccursInTypeSig(TypeVar typeVar, TypeSig typeSig);

extern Bool typeVarOccursInTypeSig(TypeVar typeVar, TypeSig typeSig, CopyContext& cc, MSA& msa);

#endif /* IVORY_COMPILER_TYPE_H_DEFINED */
