/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    type.cpp
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
 *    Implementation of 'TypeCheck' class for type resolution and inference
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
#include "ivory/module.h"
#include "ivory/trace.h" 
#include "ivory/compiler/CopyContext.h"
#include "ivory/compiler/class.h"
#include "ivory/compiler/type.h"

#define TRACE
#define TRACE_UNIFICATION				         // Remove comment to trace unification
//#define TRACE_UNIFICATION_IN_DETAIL	         // Remove comment to trace unification in detail
#define TRACE_CONSTRAINT_CHECKING            // Remove comment to trace constraint checking
//#define TRACE_CONSTRAINT_CHECKING_IN_DETAIL  // Remove comment to trace constraint checking in detail

#define MAX_SUBSTITUTION_LEVEL   100         // Maximum nesting call level for substitution

#define typeCheckError error

// Primitive type constructors

declareTypeCon(Any);

declareTypeCon(Bits);

declareTypeCon(Bool);

declareTypeCon(Byte);

declareTypeCon(Char);

declareTypeCon(Float);

declareTypeCon(Double);

declareTypeCon(Int);

declareTypeCon(Name);

declareTypeCon(Plain);

declareTypeCon(Ptr);

declareTypeCon(Ref);

declareTypeCon(String);

declareTypeCon(Void);

declareBuiltInDataCon(Ptr);         // Ptr (and Null) are primitve data constructors

#ifdef TRACE
static Bool traceFlag =             FALSE;
static Int typeVarProbe =           -1; 
static Int inferProbeSeqNo =        -1;
static Int unifyProbeSeqNo =        -1;
static Int constraintProbeSeqNo =   -1;
Void setTraceFlag(Bool q) {
   traceFlag = q;
   traceClassCompilation(q);
}
#endif

TypeVar typeVarSupply = NULL_TYPE_VAR;

static  ConstraintSet* findConstraintSetByTypeVar(TypeVar typeVar,
   ConstraintSet** list,
   Bool remove/* = FALSE*/);

// compTypeSig: Component type signature
// index 1..n
// arity: type constructor

TypeSig compTypeSig(UInt index, TypeSig typeSig, UInt arity) {
   while (index++ < arity)
      typeSig = fun(typeSig);

   return arg(typeSig);
}

// resetTypeVarSupply: Reset the supply of type variables

Void resetTypeVarSupply(Void) {
   typeVarSupply = NULL_TYPE_VAR;
}

// newTypeVar: Supply a unique type variable

TypeVar newTypeVar(Void) {

//	This probe can be useful when debugging to find out where a type variable has been introduced
#ifdef TRACE
	if (traceFlag && typeVarSupply == 1 - typeVarProbe)
      outStream << "*** Type variable probe hit\n";
#endif

	if (typeVarSupply == -(NAME_MAX / 2))
		error("newTypeVar: Exceeded type variable supply");

	return --typeVarSupply;
}

// isExprTypeSig: Predicate to test that a type signature is an expression type
//                given a set of substitutions

TypePredicate isExprTypeSig(TypeSig typeSig, Subst* substs/* = NULL*/) {
   switch (formOf(typeSig)) {
      case AP:
         return fun(typeSig) == typeCon(Exp)
            ? TYPE_PRED_TRUE
            : TYPE_PRED_FALSE;

      case TYPE_CON:
         return typeSig == typeCon(Expr)           
            ? TYPE_PRED_TRUE
            : TYPE_PRED_FALSE;

      case NAME: {
         Subst* subst = Subst::lookUp(toTypeVar(typeSig), substs);
         return subst != NULL
            ? isExprTypeSig(subst->typeSig(), substs)
            : TYPE_PRED_UNDECIDABLE;
      }

      default:
         return TYPE_PRED_FALSE;
   }
}

// denotedTypeSig: Return the denoted type signature of a given expression type signature

TypeSig denotedTypeSig(TypeSig typeSig) {
 	assert(isExprTypeSig(typeSig, NULL), "denotedTypeSig: Expected expression type signature");
   return typeSig == typeCon(Expr) ? typeCon(Any)
		                             : arg(typeSig);
}

// denoted: Return the evaluated type signature of a given type signature

TypeSig evalTypeSig(TypeSig typeSig) {
   return isExprTypeSig(typeSig) ? evalTypeSig(denotedTypeSig(typeSig))
								         : typeSig;
}

// arrowTypeSig: Form an arrow type signature

TypeSig arrowTypeSig(TypeSig fun, TypeSig arg, MSA& msa) {
   return typeSigAp2(ARROW, fun, arg, msa);
}

// thunkTypeSig: Form a thunk type signature

TypeSig thunkTypeSig(TypeSig typeSig, MSA& msa) {
   return typeSigAp(ARROW, typeSig, msa);
}

// expTypeSig: Form an expression type signature

TypeSig expTypeSig(TypeSig typeSig, MSA& msa) {
   return typeSigAp(typeCon(Exp), typeSig, msa);
}

// ptrTypeSig: Form a pointer type signature

TypeSig ptrTypeSig(TypeSig typeSig, MSA& msa) {
   return typeSigAp(typeCon(Ptr), typeSig, msa);
}

// plainTypeSig: Form a plain type signature

TypeSig plainTypeSig(TypeSig typeSig, MSA& msa) {
   return typeSigAp(typeCon(Plain), typeSig, msa);
}

// addConstraintSetToListByTypeVar: Adds dependent constraint sets to a list for a given type variable

Void addConstraintSetToListByTypeVar(TypeVar typeVar, Void* arg, MSA& msa) {
   ConstraintSet** unresolvedList =    ((AddConstraintSetToListByTypeVar_Args*)arg)->_unresolvedList;
	ConstraintSet** constraintSetList = ((AddConstraintSetToListByTypeVar_Args*)arg)->_constraintSetList;
   const Env* env =                    ((AddConstraintSetToListByTypeVar_Args*)arg)->_env;

#ifdef TRACE
   if (traceFlag && FALSE) {
      outStream << "Searching constraint sets for typeVar ";
      printName(typeVar, outStream, env->nameTable());
      outStream << '\n';
   }
#endif

   ConstraintSet* cs = findConstraintSetByTypeVar(typeVar, unresolvedList, TRUE);
   if (cs != NULL) {
      cs->next() = (*constraintSetList);
      (*constraintSetList) = cs;
   }
}

// hasTypeVar: Predicate to test for inclusion of any type variable in a signature
// T.B.D. Consider replacing as macro.

Bool hasTypeVar(TypeSig typeSig) {
   Bool foo = isAp(typeSig);
   Bool bar = isTypeVar(typeSig);

   return typeSig == TypeSig(UNKNOWN) ||
      (isAp(typeSig)
          ? hasTypeVar(fun(typeSig)) || hasTypeVar(arg(typeSig))
          : isTypeVar(typeSig));
}

// hasTypeVar: Predicate to test for inclusion of any type variable in a signature
// excluding any the given set
// T.B.D. Consider replacing as macro.

Bool hasTypeVar(TypeSig typeSig, TypeVarSet& typeVarSet) {
   return isAp(typeSig)
      ? hasTypeVar(fun(typeSig), typeVarSet) || hasTypeVar(arg(typeSig), typeVarSet)
      : isTypeVar(typeSig) && !typeVarSet.hasElement(toTypeVar(typeSig));
}

// hasTypeVar: Predicate to test for inclusion of any type variable in a signature
//             given a set of substitutions

static Bool hasTypeVar(TypeSig typeSig, const Subst* substs) {
   if (isAp(typeSig))
      return hasTypeVar(fun(typeSig), substs) ||
             hasTypeVar(arg(typeSig), substs);
   else
      if (isTypeVar(typeSig)) {
         const Subst* subst;
         if ((subst = Subst::lookUp(toTypeVar(typeSig), substs)) != NULL)
            return hasTypeVar(subst->typeSig(), substs);
         return TRUE;
      }
      else return FALSE;
}

// typeSigIsTypeVar: Predicate to test if signature is a type variable
//                   given a set of substitutions

Bool typeSigIsTypeVar(TypeSig typeSig, Subst* substs) {
   if (isAp(typeSig))
      return FALSE;
   else
      if (isTypeVar(typeSig)) {
         Subst* subst;
         if ((subst = Subst::lookUp(toTypeVar(typeSig), substs)) != NULL)
            return typeSigIsTypeVar(subst->typeSig(), substs);
         return TRUE;
      }
      else return FALSE;
}

// hasTypeVarInList: Predicate to test for inclusion of any type variable in a list of type signatures

Bool hasTypeVarInList(Expr typeSigList) {
   while (typeSigList != Nil) {
      if (hasTypeVar(hd(typeSigList)))
	      return TRUE; 
	   typeSigList = tl(typeSigList);
   }
   return FALSE;
}

// firstTypeVar: Return first type variab;le encountered in a type signature

TypeVar firstTypeVar(TypeSig typeSig) {
   if (isAp(typeSig)) {
      TypeVar typeVar = firstTypeVar(fun(typeSig));
      if (typeVar != NULL_TYPE_VAR)
         return typeVar;
      return firstTypeVar(arg(typeSig));
   }
   else
      return isTypeVar(typeSig)
         ? toTypeVar(typeSig)
         : NULL_TYPE_VAR;
}

// mapProcTypeVars: Apply procedure to type variables in a type signature

Void mapProcTypeVars(TypeSig typeSig, TYPE_VAR_PROC proc, Void* arg, MSA& msa) {
	if (!isAp(typeSig)) {
		if (isTypeVar(typeSig))
			(*proc)(toTypeVar(typeSig), arg, msa);
	} else {
		mapProcTypeVars(fun(typeSig), proc, arg, msa);
		mapProcTypeVars(arg(typeSig), proc, arg, msa);
	}
}

// mapProcSubstTypeVars: Apply procedure to type variables in a set of substitutions

Void mapProcSubstTypeVars(Subst* substs,
                          const BidirNameMap* typeVarMap,
                          TYPE_VAR_PROC proc, Void* arg, MSA& msa) {
   Subst* subst = substs;
   while (subst != NULL) {
      TypeVar typeVar = subst->typeVar();
      if (typeVarMap != NULL)
        typeVar = typeVarMap->invLookUp(typeVar);
      (*proc)(typeVar, arg, msa);
      subst = subst->next();
   }
}

// typeVarOccursInTypeSig: Predicate to check whether a given type variable occurs
// in a type signature

Bool typeVarOccursInTypeSig(TypeVar typeVar, TypeSig typeSig) {
	return isAp(typeSig)
      ? typeVarOccursInTypeSig(typeVar, fun(typeSig)) ||
        typeVarOccursInTypeSig(typeVar, arg(typeSig))
      : isTypeVar(typeSig) && toTypeVar(typeSig) == typeVar;
}

// typeVarOccursInTypeSig: Predicate to check whether a given type variable occurs
// in a type signature, given a pair of sets of substitutions

Bool typeVarOccursInTypeSig(TypeVar typeVar, TypeSig typeSig, const Subst* substs, const Subst* extSubsts) {
	if (isAp(typeSig))
		return typeVarOccursInTypeSig(typeVar, fun(typeSig), substs, extSubsts) ||
             typeVarOccursInTypeSig(typeVar, arg(typeSig), substs, extSubsts);
   else
      if (isTypeVar(typeSig)) {
         const Subst* subst;
         if ((subst = Subst::lookUp(toTypeVar(typeSig), substs)) != NULL ||
			    (subst = Subst::lookUp(toTypeVar(typeSig), extSubsts)) != NULL)
            return typeVarOccursInTypeSig(typeVar, subst->typeSig(), substs, extSubsts);
         else
            return toTypeVar(typeSig) == typeVar;
      } else
         return FALSE;
}

Bool typeVarOccursInTypeSig(TypeVar typeVar, TypeSig typeSig, CopyContext& cc, MSA& msa) {
   return isAp(typeSig)
      ? typeVarOccursInTypeSig(typeVar, fun(typeSig)) ||
      typeVarOccursInTypeSig(typeVar, arg(typeSig))
      : isTypeVar(typeSig) && mapTypeVar(toTypeVar(typeSig), cc, msa) == typeVar;
}

// typeVarsInTypeSig: Add type variables in signature to set if not already present

Void typeVarsInTypeSig(TypeSig typeSig, TypeVarSet& typeVars, MSA& msa) {
   if (!isAp(typeSig)) {
		if (isTypeVar(typeSig)) {
         TypeVar typeVar = toTypeVar(typeSig);
         if (!typeVars.hasElement(typeVar))
            typeVars.addElement(typeVar, msa);
		}
	} else {
      typeVarsInTypeSig(fun(typeSig), typeVars, msa);
      typeVarsInTypeSig(arg(typeSig), typeVars, msa);
	}
}

// insTypeSig: Instantiate a type signature by substituting new type variables for those
// not already present in the type environment.

Void insTypeSig(TypeSig& typeSig, TypeEnv* typeEnv, TypeVarSet* typeVars, MSA& msa) {
   if (typeSig == TypeSig(UNKNOWN)) {
      TypeVar typeVar = newTypeVar();
      typeSig = fromTypeVar(typeVar);
      if (typeVars != NULL)
         typeVars->addElement(typeVar, msa);
   } else
      if (!isAp(typeSig)) {
         if (isTypeVar(typeSig)) {
            TypeVar typeVar = toTypeVar(typeSig);
            if (typeEnv != NULL) {
               const Subst* subst = typeEnv->lookUp(typeVar);
               if (subst != NULL) {
                  typeSig = subst->typeSig();
                  return;
               }
            }  
            TypeVar ntv = newTypeVar();
            if (typeEnv != NULL)
               typeEnv->addSubst(new(msa) Subst(typeVar, (typeSig = fromTypeVar(ntv)), NULL));
            if (typeVars != NULL)
               typeVars->addElement(ntv, msa);
         }
      }
      else {
         insTypeSig(fun(typeSig), typeEnv, typeVars, msa);
         insTypeSig(arg(typeSig), typeEnv, typeVars, msa);
      }
}

// mapTypeVar: Map a type variable

TypeVar mapTypeVar(TypeVar typeVar, const CopyContext& cc, MSA& msa) {
   return cc.typeVarMap()->use(typeVar,
                               cc.nameTable(), 
                               newTypeVar,
                               msa);
}

// mapTypeSig: Map a type signature

Void mapTypeSig(TypeSig& typeSig, const CopyContext& cc, MSA& msa) {
   if isAp(typeSig) {
      mapTypeSig(fun(typeSig), cc, msa);
      mapTypeSig(arg(typeSig), cc, msa);
   }
   else if isTypeVar(typeSig)
      typeSig = fromTypeVar(mapTypeVar(toTypeVar(typeSig), cc, msa));
}

// mutableTypeSig: Return a mutable mapped type signature

TypeSig mutableTypeSig(TypeSig typeSig, MSA& msa) {
   return isAp(typeSig)
      ? (!hasTypeVar(typeSig)
         ? typeSig
         : typeSigAp(mutableTypeSig(fun(typeSig), msa),
                     mutableTypeSig(arg(typeSig), msa), msa))
      : typeSig;
}

TypeSig mutableTypeSig(TypeSig typeSig, const CopyContext& cc, MSA& msa) {
   return isAp(typeSig)
      ? (!hasTypeVar(typeSig)
         ? typeSig
         : typeSigAp(mutableTypeSig(fun(typeSig), cc, msa),
                     mutableTypeSig(arg(typeSig), cc, msa), msa))
      : (isTypeVar(typeSig) && cc.moduleDefn() != NULL
         ? fromTypeVar(mapTypeVar(toTypeVar(typeSig), cc, msa))
         : typeSig);
}

// assignTypeSigMappedVars: Assign mapped variables for type signature

Void assignTypeSigMappedVars(TypeSig typeSig, const CopyContext& cc, MSA& msa) {
   if isAp(typeSig) {
      assignTypeSigMappedVars(fun(typeSig), cc, msa);
      assignTypeSigMappedVars(arg(typeSig), cc, msa);
   }
   else if isTypeVar(typeSig)
      (Void)mapTypeVar(toTypeVar(typeSig), cc, msa);
}

// assignTypeSigListMappedVars: Allocate mapped variables for type signature list

Void assignTypeSigListMappedVars(Expr typeSigList, const CopyContext& cc, MSA& msa) {
   if (typeSigList != Nil) {
      assignTypeSigMappedVars(hd(typeSigList), cc, msa);
      assignTypeSigListMappedVars(tl(typeSigList), cc, msa);
   }
}

// Minimal form of a pair of type signatures

TypeSigPair minimalForm(TypeSigPair typeSigPair) {
   if (isAp(typeSigPair._x) && isAp(typeSigPair._y)) {
      TypeSig fun_x = fun(typeSigPair._x);
      TypeSig arg_x = arg(typeSigPair._x);
      TypeSig fun_y = fun(typeSigPair._y);
      TypeSig arg_y = arg(typeSigPair._y);
      if (isAp(fun_x) && fun(fun_x) == TypeSig(ARROW) &&
          isAp(fun_y) && fun(fun_y) == TypeSig(ARROW)) {
         TypeSigPair x = minimalForm({ arg(fun_x), arg(fun_y) });
         TypeSigPair y = minimalForm({ arg_x, arg_y });
         if (eqTypeSig(x._x, y._x) &&
             eqTypeSig(x._y, y._y))
            return { x._x , x._y };
      } else {
         if (fun_x == fun_y)
            return minimalForm({ arg_x, arg_y });
      }
   }
   return typeSigPair;
};

// packTypeSig: Pack a type signature into a buffer
// If the buffer size is exceeded, the required length is returned.
// with the buffer pointer set to NULL.

// In-built types have special codes to minimise the space required in
// the name table

char base64encode(unsigned char n) {
   return (n < 26) ?    ('A' + n)
      :   (n < 52) ?    ('a' + (n - 26))
      :   (n < 62) ?    ('0' + (n - 52))
      :   (n == 62) ?   '+'
      :   (n == 63) ?   '/'
      :   '=';
}

static ConstString decorTypeVarIdent(TypeVar typeVar, Code& code,
                                     char fallback[32]) {
   ConstString ident = nameString_(typeVar, code.nameTable());
   if (ident != NULL)
      return ident;

   sprintf(fallback, "v%d", typeVar < 0 ? -typeVar : typeVar);
   return fallback;
}

size_t packTypeSig(TypeSig typeSig, unsigned char* buffer, size_t size,
                   Code& code, Bool useNames/* = TRUE*/) {
   unsigned char uIntBuff[MAX_VLU_LENGTH];
   if (buffer != NULL)
      if (size != 0)
         size--;
      else
         buffer = NULL;      // Minimum 1 byte
   UInt data;
   switch (formOf(typeSig)) {
      case UNKNOWN:
         data = TYPE_SIG_UNKNOWN;
         break;

      case AP: {
         if (buffer != NULL)
            *buffer++ = useNames ? TYPE_SIG_AP : base64encode(TYPE_SIG_AP);
         UInt len = packTypeSig(fun(typeSig), buffer, size, code, useNames);
         if (buffer != NULL)
            if (len <= size) {
               buffer += len;
               size -= len;
            }
            else
               buffer = NULL;
         return 1 + len + packTypeSig(arg(typeSig), buffer, size, code, useNames);
      }

      case ARROW:
         data = TYPE_SIG_ARROW;
         break;

      case NAME: {
         if (buffer != NULL)
            *buffer++ = useNames
               ? TYPE_SIG_TYPE_VAR
               : base64encode(TYPE_SIG_TYPE_VAR);
         UInt len;
         if (useNames) {
            len = packVLU(code.addNameConst(toName(typeSig)),
               uIntBuff);
            if (buffer != NULL && len <= size)
               memcpy(buffer, uIntBuff, len);
         } else {
            char fallback[32];
            ConstString ident = decorTypeVarIdent(toTypeVar(typeSig), code, fallback);
            UInt identLen = strlen(ident);
            len = 1 + identLen;
            if (buffer != NULL && len <= size) {
               *buffer++ = base64encode(identLen);
               memcpy(buffer, ident, identLen);
            }
         }
         return 1 + len;
      }

      case TYPE_CON:
         if (typeSig == typeCon(Void))
            data = TYPE_SIG_VOID;
         else if (typeSig == typeCon(Exp))
            data = TYPE_SIG_EXP;
         else if (typeSig == typeCon(Expr))
            data = TYPE_SIG_EXPR;
         else if (typeSig == typeCon(Name))
            data = TYPE_SIG_NAME;
         else if (typeSig == typeCon(Type))
            data = TYPE_SIG_TYPE; 
         else if (typeSig == typeCon(Byte))
            data = TYPE_SIG_BYTE;
         else if (typeSig == typeCon(Bits))
            data = TYPE_SIG_BITS;
         else if (typeSig == typeCon(Int))
            data = TYPE_SIG_INT;
         else if (typeSig == typeCon(Float))
            data = TYPE_SIG_FLOAT;
         else if (typeSig == typeCon(Double))
            data = TYPE_SIG_DOUBLE;
         else if (typeSig == typeCon(Char))
            data = TYPE_SIG_CHAR;
         else if (typeSig == typeCon(String))
            data = TYPE_SIG_STRING;
         else if (typeSig == typeCon(Ref))
            data = TYPE_SIG_REF;
         else if (typeSig == typeCon(Ptr))
            data = TYPE_SIG_PTR;
         else if (typeSig == typeCon(Array))
            data = TYPE_SIG_ARRAY;
         else if (typeSig == typeCon(List))
            data = TYPE_SIG_LIST;
         else if (typeSig == typeCon(Any))
            data = TYPE_SIG_ANY;  
         else {
            TypeCon& typeCon = toBody(typeSig, TypeCon);
            if (useNames) {
               data = TYPE_SIG_TYPE_CON_MIN + code.addNameConst(typeCon._name);
               if (data > TYPE_SIG_TYPE_CON_MAX) {
                  if (buffer != NULL)
                     *buffer++ = TYPE_SIG_TYPE_CON;
                  UInt len = packVLU(data - (TYPE_SIG_TYPE_CON_MAX - 1), uIntBuff);
                  if (buffer != NULL && len <= size)
                     memcpy(buffer, uIntBuff, len);
                  return 1 + len;
               }
            } else {
               if (buffer != NULL)
                  *buffer++ = base64encode(TYPE_SIG_TYPE_CON);
               Name typeConName = typeCon._name;
               ConstString ident = typeCon.nameTable().string(
                  typeConName < builtInNameCount ? typeConName : typeConName - builtInNameCount);
               char fallback[32];
               if (ident == NULL) {
                  sprintf(fallback, "t%d", typeConName);
                  ident = fallback;
               }
               UInt len = strlen(ident);
               UInt uLen = packVLU(len, uIntBuff);
               UInt packedLen = (uLen * 2) + len;
               if (buffer != NULL && packedLen <= size) {
                  for (UInt i = 0; i < uLen; i++) {
                     *buffer++ = base64encode(uIntBuff[i] & 0x3f);
                     *buffer++ = base64encode((uIntBuff[i] >> 4) & 0x3f);
                  }
                  memcpy(buffer, ident, len);
               }
               return 1 + packedLen;
            }
         }
         break;

      case TUPLE:{
         data = TYPE_SIG_TUPLE_MIN + toTupleCon(typeSig);
         if (data > TYPE_SIG_TUPLE_MAX) {
            if (buffer != NULL)
               *buffer++ = TYPE_SIG_TUPLE;
            UInt len = packVLU(toTupleCon(typeSig) - (TYPE_SIG_TUPLE_MAX + 1),
                               uIntBuff);
            if (buffer != NULL && len <= size)
               memcpy(buffer, uIntBuff, len);
            return 1 + len;
         }

         break;
      }

      default:
         error("packTypeSig: unexpected");
         return 0;
   }
   if (buffer != NULL)
      *buffer = useNames ? data : base64encode(data);

   return 1;
}

// TypeCheck::insOccTypeSig: Instantiate occurrence type signature

Void TypeCheck::insOccTypeSig(TypeSig& typeSig, const TypeVarSet& schematicTypeVars,
                              TypeEnv* typeEnv, TypeVarSet* typeVars) {
   if (typeSig == TypeSig(UNKNOWN)) {
      TypeVar typeVar = newTypeVar(typeVars);
      typeSig = fromTypeVar(typeVar);
   }
   else
      if (!isAp(typeSig)) {
         if (isTypeVar(typeSig)) {
            TypeVar typeVar = toTypeVar(typeSig);
            const Subst* subst = typeEnv->lookUp(typeVar);
            if (subst != NULL)
               typeSig = subst->typeSig();
            else {
               if (schematicTypeVars.hasElement(typeVar)) {
                  TypeVar ntv = newTypeVar(typeVars);
                  typeEnv->addSubst(new(msa()) Subst(typeVar, (typeSig = fromTypeVar(ntv)), NULL));
               }
               else
                  assert(!(typeVar >= 0), " insTypeSig: unexpected type variable");
            }
         }
      }
      else {
         insOccTypeSig(fun(typeSig), schematicTypeVars, typeEnv, typeVars);
         insOccTypeSig(arg(typeSig), schematicTypeVars, typeEnv, typeVars);
      }
}

// TypeCheck::instantiate: Instantiate type signature and associated constraints

Void TypeCheck::instantiate(TypeSig& typeSig,
                            TypeVarSet& schematicTypeVars,
                            Bool withConstraints,
                            TypedVal* typedVal,
                            TypeInsts* typeInsts,
                            const ModuleDefn* moduleDefn,
                            TypeEnv* typeEnv) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Instantiating type signature for ";
      printTypeSig(typeSig, 0, FALSE, outStream, _env);
      if (!schematicTypeVars.isEmpty()) {
         outStream << '{';
         schematicTypeVars.print(outStream, _env);
         outStream << '}';
      }
      outStream << " (potentially mapped)\n";
      outStream.flush();
   }
#endif
 
   TypeVarSet typeVars;
   insOccTypeSig(typeSig, schematicTypeVars, typeEnv, &typeVars);

   if (withConstraints) {
      // Instantiate any constraints dependent on newly instantiated 
      // type variables

      ConstraintSet** unresolvedList = moduleDefn == NULL
         ? &_predicates._unresolvedList
         : (ConstraintSet**)(void**)&((ModuleDefn*)(Void*)moduleDefn)->constraints();
      const Env& env = moduleDefn == NULL ? _env : moduleDefn->env();
      ConstraintSet* constraintSets = NULL;
      AddConstraintSetToListByTypeVar_Args args = { unresolvedList,
         &constraintSets,
         &env };
      TypeVarElement* typeVarElement = typeVars.elements();
      while (typeVarElement != NULL) {
         TypeVar typeVar = Subst::lookUpRHS(typeVarElement->_typeVar,
            typeEnv->substs())->_typeVar;
         if (moduleDefn != NULL)
            typeVar = getBirNameMap(moduleDefn->nameTable(),
                                     predicates().typeVarMaps(),
                                     msa())->invLookUp(typeVar);
         addConstraintSetToListByTypeVar(typeVar, &args, msa());
         typeVarElement = typeVarElement->_next;
      }

#ifdef TRACE
      UInt count = 0;
#endif

      Constraint* constraints = NULL;
      while (constraintSets != NULL) {
         ConstraintSet* constraintSet = constraintSets;
         constraintSets = constraintSets->_next;
         ConstraintElement* constraintElement = constraintSet->elements();
         while (constraintElement != NULL) {
            if (constraintElement->constraint()->kind() != Constraint::SCHEME_CONSTRAINT &&
                constraintElement->constraint()->kind() != Constraint::SCHEMATIC_INSTANCE_CONSTRAINT) {
               Constraint* constraint = constraintElement->constraint()->instantiate(moduleDefn,
                                                                  typeEnv,
                                                                  typeInsts,
                                                                  TRUE,
                                                                  *this);
               constraint->next() = constraints;
               constraints = constraint;
            }

#ifdef TRACE
            count++;
#endif

            constraintElement = constraintElement->next();
         }
         constraintSet->_next = *unresolvedList;
         *unresolvedList = constraintSet;
      }
      while (constraints != NULL) {
         Constraint* constraint = constraints;
         constraints = constraint->next();
         addConstraintToSet(*constraint, TRUE);
      }

#ifdef TRACE
      if (traceFlag)
         outStream << count << " constraints instantiated\n";
#endif

   }

   if (typedVal != NULL)
      typedVal->addInstance(typeSig, typeInsts, typeEnv, *this);

#ifdef TRACE
   if (traceFlag) {
      outStream << "Instantiated as: ";
      printTypeSig(typeSig, 0, FALSE, outStream, _env);
      outStream << endl;
   }
#endif

}

/*----------------------------------------------------------------------------*/

// TypeEquation::subst: Substitute a type variable in a type equation

Void TypeEquation::subst(TypeVar typeVar, TypeSig ts) {
   Subst::substTypeVar(typeVar, ts, _x);
   Subst::substTypeVar(typeVar, ts, _y);
}

/*----------------------------------------------------------------------------*/

// Subst::subst: Substitute a type variable in a substitution

Void Subst::subst(TypeVar typeVar, TypeSig typeSig) {
   substTypeVar(typeVar, typeSig, _typeSig);
}

// Subst::lookUp: Look up the substitution for a type variable in a set

Subst* Subst::lookUp(TypeVar typeVar, Subst* substs) {
   Subst* subst = substs;
   while (subst != NULL) {
      if (subst->_typeVar == typeVar)
         return subst;
      subst = subst->_next;
   }
   return NULL;
}

// Subst::lookUp: Look up the substitution for a type variable in a set
// (constant version)

const Subst* Subst::lookUp(TypeVar typeVar, const Subst* substs) {
   const Subst* subst = substs;
   while (subst != NULL) {
      if (subst->_typeVar == typeVar)
         return subst;
      subst = subst->_next;
   }
   return NULL;
}

// Subst::lookUpRHS: Look up the substitution for a RHS type variable in a set
// (constant version)

const Subst* Subst::lookUpRHS(TypeVar typeVar, const Subst* substs) {
   const Subst* subst = substs;
   while (subst != NULL) {
      if (isTypeVar(subst->_typeSig) &&
          toTypeVar(subst->_typeSig) == typeVar)
         return subst;
      subst = subst->_next;
   }
   return NULL;
}

// Subst::lookUp: Return the first substitution with an equal type signature
// T.B.D. Check whether this function is a) valid, and b) necessary

TypeVar Subst::lookUp(TypeSig typeSig, const Subst* substs) {
   const Subst* subst = substs;
   while (subst != NULL) {
      if (eqTypeSig(typeSig, subst->_typeSig))
         return subst->_typeVar;
      subst = subst->_next;
   }
   return NULL_TYPE_VAR;
}

// Subst::typeVarInSubsts: Predicate to test for the existence of a substitution
// for a given type variable.  Could be macro.

Bool Subst::typeVarInSubsts(TypeVar typeVar, const Subst* substs) {
	return lookUp(typeVar, substs) != NULL;
}

// Subst::typeVarOccursInTypeSig: Predicate to test whether at least one
// substitution type variable occurs in a type signature.

Bool Subst::typeVarOccursInTypeSig(const Subst* substs, TypeSig typeSig) {
	const Subst* subst = substs;
	while (subst != NULL) {
		if (::typeVarOccursInTypeSig(subst->_typeVar, typeSig))
			return TRUE;
		subst = subst->_next;
	}
	return FALSE;
}

// Predicate to test that a type signature is a type variable
// Given a set of substitutions

Bool Subst::isTypeVarUnderSubsts(TypeSig typeSig, const Subst* substs) {
   if (!isTypeVar(typeSig))
      return FALSE;
   const Subst* subst = Subst::lookUp(toTypeVar(typeSig), substs);
   return subst == NULL || isTypeVar(subst->typeSig());
}

Bool Subst::hasTypeVarUnderSubsts(TypeSig typeSig, const Subst* substs) {
   if (isAp(typeSig))
      return hasTypeVarUnderSubsts(fun(typeSig), substs) ||
             hasTypeVarUnderSubsts(arg(typeSig), substs);
   else
      if (isTypeVar(typeSig)) {
         const Subst* subst = Subst::lookUp(toTypeVar(typeSig), substs);
         return subst == NULL || hasTypeVar(subst->typeSig());
      } else
         return FALSE;
}




// Subst::firstTypeVar Return first type variable in a type signature where a given predicate
// on the type var is not satisfied.
//	Returns NULL_TYPE_VAR if none exists

TypeVar Subst::firstTypeVar(TypeSig typeSig, TYPE_VAR_PRED pred, Void* arg) {
   if (!isAp(typeSig))
      if (isTypeVar(typeSig)) {
			TypeVar typeVar = toTypeVar(typeSig);
			return (*pred)(typeVar, arg) ? NULL_TYPE_VAR
			                             : typeVar;
      }
      else
         return NULL_TYPE_VAR;
   else {
      Type typeVar = firstTypeVar(fun(typeSig), pred, arg);
      return typeVar != NULL_TYPE_VAR 
         ? typeVar
         : firstTypeVar(arg(typeSig), pred, arg);
   }
}

// firstTypeVarInList: firstTypeVar extended to list of signatures

TypeVar Subst::firstTypeVarInList(Expr typeSigList, TYPE_VAR_PRED pred, Void* arg) {
	if (typeSigList == Nil)
		return NULL_TYPE_VAR;
	else {
		TypeVar typeVar = firstTypeVar(hd(typeSigList), pred, arg);
      return typeVar != NULL_TYPE_VAR 
         ? typeVar
         : firstTypeVarInList(tl(typeSigList), pred, arg);
   }
}

// Subst::substTypeVar: Substitute all occurences of given type variable

Void Subst::substTypeVar(TypeVar typeVar, TypeSig substTS, TypeSig& typeSig) {
   if (isAp(typeSig)) {
      substTypeVar(typeVar, substTS, fun(typeSig));
      substTypeVar(typeVar, substTS, arg(typeSig));
   } else if (isTypeVar(typeSig) && typeVar == toTypeVar(typeSig))
      typeSig = substTS;
}

// Subst::substSubsts: Apply nested substitutions

Subst* Subst::substSubsts(Subst* substs) {
   Subst* fixedSubsts = NULL;

   while (substs != NULL) {
      Subst** substPtr = &substs;
      do {
         Subst* subst = *substPtr;
         Bool fixed = FALSE;
         if (!hasTypeVar(subst->typeSig()))
            fixed = TRUE;
         else {
            substTypeVars(subst->_typeSig, fixedSubsts);
            if (!typeVarOccursInTypeSig(substs, subst->typeSig()))
               fixed = TRUE;
         }

         if (fixed) {
            *substPtr = subst->_next;
            subst->_next = fixedSubsts;
            fixedSubsts = subst;
         } else
            substPtr = &subst->_next;

      } while (*substPtr != NULL);
   }
   return fixedSubsts;
}

// Subst::substSubsts: Apply substitutions to a set of substitutions

Void Subst::substSubsts(Subst* subst1, const Subst* subst2) {
	while (subst1 != NULL) {
      Subst::substTypeVars(subst1->_typeSig, subst2);
		subst1 = subst1->_next;
	}
}

Void Subst::substSubsts(Subst* subst1, const SubstSet& subst2) {
	while (subst1 != NULL) {
      Subst::substTypeVars(subst1->_typeSig, subst2);
		subst1 = subst1->_next;
	}
}

// Subst::uniqueTypeSig: Return a unique type signature

Expr Subst::uniqueTypeSig(TypeSig typeSig, const CopyContext& cc,
                          TypeEnv* typeEnv, MSA& msa,
                          Bool instantiate/* = TRUE*/) {
   if (hasTypeVar(typeSig)) {
      typeSig = mutableTypeSig(typeSig, cc, msa);
      if (instantiate)
         insTypeSig(typeSig, typeEnv, NULL, msa);
   }
   return typeSig;
}

// Subst::uniqueTypeSigList: Return a unique type signature list

Expr Subst::uniqueTypeSigList(Expr typeSigList, const CopyContext& cc, 
                              TypeEnv* typeEnv, MSA& msa,
                              Bool instantiate/* = TRUE */) {
   if (typeSigList == Nil)
      return Nil;
   else
      if (!hasTypeVarInList(typeSigList))
         return typeSigList;
      else {
         TypeSig typeSig = uniqueTypeSig(hd(typeSigList), cc, typeEnv, msa, instantiate);
         return cons(typeSig, uniqueTypeSigList(tl(typeSigList), cc, typeEnv, msa, instantiate), msa);
      }
}

// Subst::uniqueTypeSigLists: Return a unique list of type signature lists

Expr Subst::uniqueTypeSigLists(Expr typeSigLists, const CopyContext& cc, 
                               TypeEnv* typeEnv, MSA& msa) {
   if (typeSigLists == Nil)
      return Nil;
   else
      return cons(uniqueTypeSigList(hd(typeSigLists), cc, typeEnv, msa),
                  uniqueTypeSigLists(tl(typeSigLists), cc, typeEnv, msa), msa);
}

// Subst::mapTypeVars: Map type variables in list of substitutions

Void Subst::mapTypeVars(Subst* substs, const CopyContext& cc, MSA& msa) {
   Subst* subst = substs;
   while (subst != NULL) {
      subst->_typeVar = mapTypeVar(subst->_typeVar, cc, msa);
      mapTypeSig(subst->_typeSig, cc, msa);
      subst = subst->next();
   }
}

// Subst::copyTypeSig: Return a unique type signature

Expr Subst::copyTypeSig(TypeSig typeSig, CopyContext& cc, MSA& msa, const Subst* substs) {
   if (hasTypeVar(typeSig) &&
       (cc.moduleDefn() != NULL || Subst::typeVarOccursInTypeSig(substs, typeSig))) {
		typeSig = mutableTypeSig(typeSig, cc, msa);
      Subst::substTypeVars(typeSig, substs);
      return typeSig;
	}
	return typeSig;
}

// Subst::mapSubsts: Map substitions for a mapped type signature

Void Subst::mapSubsts(TypeSig ts1, CopyContext& cc1, TypeSig ts2, CopyContext& cc2, Subst* substs, MSA& msa) {
   if (isAp(ts1)) {
      mapSubsts(fun(ts1), cc1, fun(ts2), cc2, substs, msa);
      mapSubsts(arg(ts1), cc1, arg(ts2), cc2, substs, msa);
   } else if (isTypeVar(ts1)) {
      TypeVar tv1 = cc1.moduleDefn() == NULL ? toTypeVar(ts1) : mapTypeVar(toTypeVar(ts1), cc1, msa);
      Subst* subst = substs;
      while (subst != NULL) {
         if (subst->_typeVar == tv1)
            subst->_typeVar = mapTypeVar(toTypeVar(ts2), cc2, msa);
         subst = substs->_next;
      }
   }
}

// Subst::substTypeVars: Apply substitutions to a type signature

Void Subst::substTypeVars(TypeSig& typeSig, const Subst* substs) {
   if (isAp(typeSig)) {
      substTypeVars(fun(typeSig), substs);
      substTypeVars(arg(typeSig), substs);
   } else if (isTypeVar(typeSig)) {
      const Subst* subst;
      if ((subst = Subst::lookUp(toTypeVar(typeSig), substs)) != NULL)
         typeSig = subst->_typeSig;
   }
}

#ifdef TRACE
static UInt subsLevel = 0;
#endif

Void Subst::substTypeVars(TypeSig& typeSig, const SubstSet& substs) {

#ifdef TRACE
   assert(subsLevel < MAX_SUBSTITUTION_LEVEL, "Subst::substTypeVars: Maximum level exceeded");
#endif

   if (isAp(typeSig)) {

#ifdef TRACE
      subsLevel++;
#endif

      substTypeVars(fun(typeSig), substs);
      substTypeVars(arg(typeSig), substs);

#ifdef TRACE
      subsLevel--;
#endif

   } else if (isTypeVar(typeSig)) {
      const Subst* subst;
      if ((subst = substs.lookUp(toTypeVar(typeSig))) != NULL)
         typeSig = subst->_typeSig;
   }
}

// Subst::substTypeSigList: Return a list of type signatures with type variables substituted.

Void Subst::substTypeSigList(Expr typeSigList, const Subst* substs) {
	while (typeSigList != Nil) {
		Subst::substTypeVars(hd(typeSigList), substs);
		typeSigList = tl(typeSigList);
	}
}

Void Subst::substTypeSigLists(Expr typeSigLists, const Subst* substs) {
   while (typeSigLists != Nil) {
      Subst::substTypeSigList(hd(typeSigLists), substs);
      typeSigLists = tl(typeSigLists);
   }
}

Void Subst::substTypeSigList(Expr typeSigList, const SubstSet& substs) {
	while (typeSigList != Nil) {
		Subst::substTypeVars(hd(typeSigList), substs);
		typeSigList = tl(typeSigList);
	}
}

// Subst::swap: Swap the type variables and type signatures in substitutions
// T.B.D. This function may now be obsolete

Void Subst::swap(Subst* substs) {
	while (substs != NULL) {
		assert(isTypeVar(substs->typeSig()), "Subst::swap: expected type variable");
		TypeVar typeVar = substs->_typeVar;
		substs->_typeVar = toTypeVar(substs->_typeSig);
		substs->_typeSig = fromTypeVar(typeVar);
		substs = substs->_next;
	}
}

// Subst::copySubsts: Return a copy of a set of substutions

Subst* Subst::copySubsts(const Subst* substs, MSA& msa) {
   Subst* resSubsts = NULL;
   const Subst* subst = substs;
   while (subst != NULL) {
      resSubsts = new (msa) Subst(subst->_typeVar,
                                  hasTypeVar(subst->_typeSig)
                                     ? mutableTypeSig(subst->_typeSig, msa)
                                     : subst->_typeSig,
                                  resSubsts);
      subst = subst->_next;
   }
   return resSubsts;
}

Subst* Subst::copySubsts(const Subst* substs, const CopyContext& cc,
                         MSA& msa, Bool typeVarId/* = FALSE*/) {
   Subst* resSubsts = NULL;
   const Subst* subst = substs;
   while (subst != NULL) {
      resSubsts = new (msa) Subst(cc.moduleDefn() == NULL || typeVarId
                                     ? subst->_typeVar
                                     : mapTypeVar(subst->_typeVar, cc, msa),
                                  hasTypeVar(subst->_typeSig)
                                     ? mutableTypeSig(subst->_typeSig, cc, msa)
                                     : subst->_typeSig,
                                  resSubsts);
      subst = subst->_next;
   }
   return resSubsts;
}

// Subst::insSubsts: Instantiate substitutions with copy context

Subst* Subst::insSubsts(const Subst* substs, const CopyContext& cc,
                        TypeEnv* typeEnv, MSA& msa) {
   Subst* resSubsts = NULL;
   const Subst* subst = substs;
   while (subst != NULL) {
      TypeSig typeSig = mutableTypeSig(subst->_typeSig, cc, msa);
      if (hasTypeVar(typeSig))
         ::insTypeSig(typeSig, typeEnv, NULL, msa);
      resSubsts = new (msa) Subst(cc.moduleDefn() == NULL
                                     ? subst->_typeVar
                                     : mapTypeVar(subst->_typeVar, cc, msa),
                                  typeSig,
                                  resSubsts);
      subst = subst->_next;
   }
   return resSubsts;
}

// Subst::inverseSubsts: Return the inverse of a set of substutions
// e.g. a = b => b = a where b is a type variable

Subst* Subst::inverseSubsts(const Subst* substs, MSA& msa) {
   Subst* resSubsts = NULL;
	while (substs != NULL) {
      TypeSig typeSig = substs->_typeSig;
      assert(isTypeVar(typeSig), "Subst::inverseSubsts: Expected type variable on RHS");
      resSubsts = new (msa) Subst(toTypeVar(typeSig), fromTypeVar(substs->_typeVar), resSubsts);
      substs = substs->_next;
   }
   return resSubsts;
}

// Subst::printSubstLines: Print substitutions in multiple lines

Void Subst::printSubstLines(const Subst* substs, ostream& os, const Env& env,
                            UInt& n, Bool shortForm/* = FALSE*/) {
   const Subst* subst = substs;
   while (subst != NULL) {
      if (n++ > 0)
         if (!shortForm)
            os << ';' << ((n - 1) % 8 == 0 ? '\n' : ' ');
         else
            os << ',';
      printTranName(subst->_typeVar, os, env);
      os << '=';
      printTypeSig(subst->_typeSig, 0, FALSE, outStream, env);
      subst = subst->_next;
   }
}

// Subst::printSubsts: Print set of substitutions

Void Subst::printSubsts(const Subst* substs, ostream& os, const Env& env,
                        Bool shortForm/* = FALSE*/, Bool noHeader/* = FALSE*/) {
   if (substs != NULL) {
      if (!(shortForm || noHeader)) {
         os << "Substitutions\n";
         os << "-------------\n";
      }
      else
         os << '{';
      UInt n = 0;
      printSubstLines(substs, os, env, n, shortForm);
      if (!shortForm) {
         if (n > 0)
            os << '\n';
      }
      else
         os << '}';
      os.flush();
   }
}

// Subst::concat: Concatenate substitutions

Subst* Subst::concat(Subst* substs1, Subst* substs2) {
   Subst** substs;
   for (substs = &substs1;
        *substs != NULL;
        substs = &(*substs)->_next)
      ;
   *substs = substs2;
   return substs1;
}

/*----------------------------------------------------------------------------*/

SubstIndex::SubstIndex(UInt nSlots)
   : BasicChainedHashTable<Subst*, Subst, TypeVar>(nSlots, NULL) {  
}

Subst* SubstIndex::allocNode(Subst& entry, Subst* next, MSA& msa) {
   entry._next = (Subst*)next;
   return &entry;
}

Subst* SubstIndex::allocNode(size_t size, MSA& msa, Subst*& link) {
   assert(FALSE, "SubstIndex::allocNode: unexpected");
   return NULL;
}

Subst* SubstIndex::lookUp(TypeVar typeVar) const {
   if (_slots != NULL) {
      Subst* subst = (Subst*)_slots[typeVar % _nSlots];
      while (subst != NULL) {
         if (subst->_typeVar == typeVar)
            return subst;
         subst = subst->_next;
      }
   }
   return NULL;
}

Subst* SubstIndex::chain(Subst& node, Subst* next) const {
   node._next = next;
   return &node;
}

Subst* SubstIndex::next(Subst& node) const {
   return node.next();
}

SubstRefIndex::SubstRefIndex(UInt nSlots)
   : HashTable<Void*, TypeVarSubstElement, TypeVar>(nSlots, NULL),
   _nodeSupply(NULL), _pendingNodes(NULL) {
}

Void* SubstRefIndex::allocNodePtr(size_t size, MSA& msa, Void*& link) {
   if (_nodeSupply != NULL) {
      HashTableNode<Void*, TypeVarSubstElement, TypeVar>* node = _nodeSupply;
      _nodeSupply = (HashTableNode<Void*, TypeVarSubstElement, TypeVar>*)nodeNext(*node);
      return (link = node);
   } else
      return (link = msa.alloc(size));
}

Void SubstRefIndex::freeNode(HashTableNode<Void*, TypeVarSubstElement, TypeVar>* node) {
   nodeNext(*node) = _nodeSupply;
   _nodeSupply = node;
}

Void SubstRefIndex::addUnique(TypeVar typeVar, Subst* subst, MSA& msa) {
   if (_slots == NULL)
      allocSlots(msa);
   else if (lookUp(typeVar, subst))
      return;
   UInt index = typeVar % _nSlots;
   TypeVarSubstElement element(typeVar, subst);
   _slots[index] = HashTable<Void*, TypeVarSubstElement, TypeVar>::allocNode(
      element, _slots[index], msa);
}

Void SubstRefIndex::addTypeSig(TypeSig typeSig, Subst* subst, MSA& msa) {
   if (isAp(typeSig)) {
      addTypeSig(fun(typeSig), subst, msa);
      addTypeSig(arg(typeSig), subst, msa);
   } else if (isTypeVar(typeSig))
      addUnique(toTypeVar(typeSig), subst, msa);
}

Void SubstRefIndex::addSubst(Subst* subst, MSA& msa) {
   addTypeSig(subst->_typeSig, subst, msa);
}

Bool SubstRefIndex::lookUp(TypeVar typeVar, Subst* subst) const {
   if (_slots != NULL) {
      HashTableNode<Void*, TypeVarSubstElement, TypeVar>* node = (HashTableNode<Void*, TypeVarSubstElement, TypeVar>*)_slots[typeVar % _nSlots];
      while (node != NULL) {
         if (nodeEntry(*node)._typeVar == typeVar &&
             nodeEntry(*node)._subst   == subst)
            return TRUE;
         node = (HashTableNode<Void*, TypeVarSubstElement, TypeVar>*)nodeNext(*node);
      }
   }
   return FALSE;
}

// Look up and save any referencing substitution element for type variable

Void SubstRefIndex::lookUpAndSave(TypeVar typeVar) {
   if (_slots != NULL) {
      HashTableNode<Void*, TypeVarSubstElement, TypeVar>** nodePtr = (HashTableNode<Void*, TypeVarSubstElement, TypeVar>**)&_slots[typeVar % _nSlots];
      while (*nodePtr != NULL) {
         const TypeVarSubstElement& entry = nodeEntry(**nodePtr);
         if (typeVar == entry._typeVar) {
            HashTableNode<Void*, TypeVarSubstElement, TypeVar>* node = *nodePtr;
            *nodePtr = (HashTableNode<Void*, TypeVarSubstElement, TypeVar>*)nodeNext(*node);
            nodeNext(*node) = _pendingNodes;
            _pendingNodes = node;
         } else
            nodePtr = &(HashTableNode<Void*, TypeVarSubstElement, TypeVar>*&)nodeNext(**nodePtr);
      }
   }
}

Void SubstRefIndex::substPending(Subst* substs, Bool nested, MSA& msa) {
   HashTableNode<Void*, TypeVarSubstElement, TypeVar>* node = _pendingNodes;
   while (node != NULL) {
      Subst* indexSubst = nodeEntry(*node)._subst;
      Subst* subst = substs;
      while (subst != NULL) {
         Subst::substTypeVar(subst->_typeVar, subst->_typeSig, indexSubst->_typeSig);
         subst = subst->_next;
      }
      if (!nested) { 
         _pendingNodes = (HashTableNode<Void*, TypeVarSubstElement, TypeVar>*)nodeNext(*node);
         nodeNext(*node) = _nodeSupply;
         node = _pendingNodes;
         addSubst(indexSubst, msa);  // Re-index for any type variables in substitution
      } else
         node = (HashTableNode<Void*, TypeVarSubstElement, TypeVar>*)nodeNext(*node);
   }
}

SubstSet::SubstSet(Void)
   : _index(N_SUBST_HASH_TABLE_SLOTS), _refIndex(N_SUBST_REF_HASH_TABLE_SLOTS),
     _nestingLevel(0) {
}

// SubstSet::preSubst: Prepare for subsequent substitution              

Void SubstSet::preSubst(Subst* substs) {
   Subst* subst = substs;
   while (subst != NULL) {
      _refIndex.lookUpAndSave(subst->_typeVar);
      subst = subst->_next;
   }
   _nestingLevel++;
}

// SubstSet::addSubsts: Extend set of substitutions

Void SubstSet::addSubsts(Subst* substs, MSA& msa) {
   Bool nested = --_nestingLevel > 0;
   Subst* subst = substs;
   _refIndex.substPending(subst, nested, msa);
   while (subst != NULL) {
      assert(_index.lookUp(subst->_typeVar) == NULL, "Substitution already present");
      Subst* next = subst->_next;
      _index.add(subst, subst->_typeVar, msa);
      _refIndex.addSubst(subst, msa);
      subst = next;
   }
}

// Subst::printSubsts: Print set of substitutions

Void SubstSet::print(ostream& os, const Env& env, Bool shortForm/* = FALSE*/) const {
   if (!shortForm) {
      os << "Substitutions\n";
      os << "-------------\n";
   } else
      os << '{';
   UInt n = 0;
   if (_index._slots != NULL)
      for (UInt i = 0; i < _index.nSlots(); i++)
         Subst::printSubstLines((Subst*)_index._slots[i], os, env, n, shortForm);
   if (!shortForm) {
      if (n > 0)
         os << '\n';
   } else
      os << '}';
   os.flush();
}

/*----------------------------------------------------------------------------*/

TypeEnv::TypeEnv(TypeEnv* next/* = NULL*/,
                 Subst* const substs/* = NULL*/)
   : _next(next), _substs(substs) {
}

// TypeEnv::addSubst: Add substitution

Void TypeEnv::addSubst(Subst* subst) {
   subst->_next = _substs;
   _substs = subst;
}

// TypeEnv::lookUp: Find substitution in a type environment

const Subst* TypeEnv::lookUp(TypeVar typeVar) const {
   const Subst* subst = Subst::lookUp(typeVar, _substs);
   return subst != NULL
      ? subst
      : (_next != NULL
         ? _next->lookUp(typeVar)
         : NULL);
}

// TypeEnv::substSubsts: Substitute type variables in substitutions

Void TypeEnv::substSubsts(const SubstSet& substs) {
   Subst::substSubsts(_substs, substs);
   if (_next != NULL)
      _next->substSubsts(substs);
}

// TypeEnv::addTypeSig: Add any type variables
// not already occurring in assumptions

Void TypeEnv::addTypeSig(TypeSig typeSig, MSA& msa) {
   if (isAp(typeSig)) {
      addTypeSig(fun(typeSig), msa);
      addTypeSig(arg(typeSig), msa);
   } else if (isTypeVar(typeSig)) {
      TypeVar typeVar = toTypeVar(typeSig);
      if (!typeVarOccurs(typeVar))
         _typeVars.addElement(typeVar, msa);
   }
}

// TypeEnv::removeTypeSig: Remove any type variables
// occuring in the given type signature 

Void TypeEnv::removeTypeSig(TypeSig typeSig, MSA& msa) {
   if (isAp(typeSig)) {
      removeTypeSig(fun(typeSig), msa);
      removeTypeSig(arg(typeSig), msa);
   } else if (isTypeVar(typeSig)) {
      TypeVar typeVar = toTypeVar(typeSig);
      if (_typeVars.hasElement(typeVar))
         _typeVars.removeElement(typeVar, msa);
   }
}

// TypeEnv::substTypeVars: Substitute type variables in
// Assumption set

Void TypeEnv::substTypeVars(const Subst* substs, MSA& msa) {
   const Subst* subst = substs;
   while (subst != NULL) {
      TypeEnv* typeEnv = this;
      do {
         if (typeEnv->_typeVars.hasElement(subst->typeVar())) {
            TypeEnv* typeEnv2 = this;
            while (typeEnv2 != typeEnv) {
               typeEnv2->removeTypeSig(subst->typeSig(), msa);
               typeEnv2 = typeEnv2->_next;
            }
            typeEnv->_typeVars.removeElement(subst->typeVar(), msa);
            typeEnv->addTypeSig(subst->typeSig(), msa);
            break;
         }
         typeEnv = typeEnv->_next;
      } while (typeEnv != NULL);
      subst = subst->next();
   }
}

// TypeEnv::typeVarOccurs:
// Predicate to test for occurence of type variable in assumptions

Bool TypeEnv::typeVarOccurs(TypeVar typeVar) const {
   return
      _typeVars.hasElement(typeVar) ||
      (_next != NULL
         ? _next->typeVarOccurs(typeVar)
         : FALSE);
}

// TypeEnv::printSubsts: Print substitutions

Void TypeEnv::printSubsts(ostream& os, const Env& env) const {
   Subst::printSubsts(_substs, os, env, TRUE);
   if (_next != NULL)
      _next->printSubsts(os, env);
}

// TypeEnv::printTypeVars: Print type variables

Void TypeEnv::printTypeVars(ostream& os, const Env& env) const {
   os << '{';
   _typeVars.print(os, env);
   if (_next != NULL)
      _next->printTypeVars(os, env);
   os << '}';
}

// TypeEnv::print: Print 

Void TypeEnv::print(ostream& os, const Env& env) const {
   printSubsts(os, env);
   outStream << '\n';
   printTypeVars(os, env);
   os << '\n';
}

/*----------------------------------------------------------------------------*/

TypeVarSet::TypeVarSet(const TypeVarSet& src, CopyContext* cc, MSA& msa) {
   TypeVarElement* element = src._typeVarElements;
   _typeVarElements = NULL;
   TypeVarElement** pElement = &_typeVarElements;
   while (element != NULL) {
      *pElement = new(msa) TypeVarElement(NULL, cc == NULL
         ? element->_typeVar
         : mapTypeVar(element->_typeVar, *cc, msa));
      pElement = &(*pElement)->_next;
      element = element->_next;
   }
}

// TypeVarSet::addElement: Add a new type variable to the set

Void TypeVarSet::addElement(TypeVar typeVar, MSA& msa) {
   if (!hasElement(typeVar))
      _typeVarElements = new (msa) TypeVarElement(_typeVarElements, typeVar);
}

// TypeVarSet::removeElement: Remove a type variable to the set

Void TypeVarSet::removeElement(TypeVar typeVar, MSA& msa) {
   TypeVarElement** typeVarElementPtr = &_typeVarElements;
   while (*typeVarElementPtr != NULL) {
      if ((*typeVarElementPtr)->_typeVar == typeVar) {
         *typeVarElementPtr = (*typeVarElementPtr)->_next;
         return;
      }
      typeVarElementPtr = &(*typeVarElementPtr)->_next;
   }
}

// TypeVarSet::lookUp: Find type variable in set

TypeVarElement* TypeVarSet::lookUp(TypeVar typeVar) const {
	TypeVarElement* element = _typeVarElements;
	while (element != NULL) {
		if (element->_typeVar == typeVar)
			return element;
		element = element->_next;
	}
	return NULL;
}

// TypeVarSet::hasElement: Predicate to test for existence of type variable in set

Bool TypeVarSet::hasElement(TypeVar typeVar) const {
	return lookUp(typeVar) != NULL;
}

// TypeVarSet::nElement: Return no. elements in set

UInt TypeVarSet::nElements(Void) const {
	UInt n = 0;
	TypeVarElement* element = _typeVarElements;
	while (element != NULL) {
		n++;
		element = element->_next;
	}
	return n;
}

// TypeVarSet::print: Print set of type variables

Void TypeVarSet::print(ostream& os, const Env& env) const {
	TypeVarElement* element = _typeVarElements;
	while (element != NULL) {
      if (element != _typeVarElements)
         os << ',';
		printTranName(element->_typeVar, os, env);
		element = element->_next;
	}
}

// TypeVarSet::intersects: True if at least one elemetn in common, False otherwise

Bool TypeVarSet::intersects(const TypeVarSet& tvs1, const TypeVarSet& tvs2) {
   TypeVarElement* element = tvs1._typeVarElements;
   while (element != NULL) {
      if (tvs2.hasElement(element->typeVar()))
         return TRUE;      
      element = element->_next;
   }
   return FALSE;
}

// TypeVarSet::merge: Merge first into second

Void TypeVarSet::merge(const TypeVarSet& src, TypeVarSet& dst) {
	TypeVarElement* srcElements = src._typeVarElements;
	TypeVarElement* element;
	while ((element = srcElements) != NULL) {
		srcElements = element->_next;
		if (!dst.hasElement(element->_typeVar)) {
			element->_next = dst._typeVarElements;
			dst._typeVarElements = element;
		}
	}
}

/*----------------------------------------------------------------------------*/

// Constraint

Constraint::Constraint(Kind kind, Bool indep/* = TRUE*/)
   : _next(NULL),
     _kind(kind),
     _indep(indep),
     _seqNo(nextConstraintSeqNo()),
     _transforms(NULL),
     _replacement(NULL) {
}

Void Constraint::init(TypeCheck& typeCheck) {
}

// Constraint::appendTransforms: Append transformation(s) to end of list

Void Constraint::appendTransforms(ConstraintTransform* transforms) {
   ConstraintTransform** transformPtr = (ConstraintTransform**)&_transforms;
   while ((*transformPtr) != NULL)
      transformPtr = &(*transformPtr)->next();

   *transformPtr = transforms;
   while (transforms != NULL) {
      transforms->constraint() = this;
      transforms = transforms->next();
   }
}

Void Constraint::appendTransforms(Constraint& constraint) {
   appendTransforms(constraint._transforms);
}

Void Constraint::onMatched(const Constraint& constraint) {
}

Void Constraint::preCheck(Void) {
}

Void Constraint::revertSolution(Void) {
}

Void Constraint::onSatisfied(ConstraintSet& constraintSet, const Subst* substs,
                             TypeCheck& typeCheck) {
}

// Constraint::print: Default print method fron constraint class

Void Constraint::print(ostream& os, const Env& env, Bool shortForm) const {
	os << "CONSTRAINT";
}

/*----------------------------------------------------------------------------*/

ConstraintSet::ConstraintSet(ConstraintSet* next/* = NULL*/)
   : _next(next),
   _typeVarSet(),
   _elements(NULL),

   _nCoercion(0),
   _nSchematic(0) {
}

// ConstraintSet::add: Add constraint to set
// Ordered by increasing sequence no.

Void ConstraintSet::add(Constraint& constraint, MSA& msa) {
   UInt seqNo = constraint._seqNo;

	ConstraintElement** elementPtr = &_elements;
	while (*elementPtr != NULL &&
          (*elementPtr)->constraint()->_seqNo < seqNo)
//          (*elementPtr)->constraint()->_seqNo > seqNo)
   elementPtr = &(*elementPtr)->next();
 
   ConstraintElement* element;
   if (_freeConstraintElements != NULL) {
      element = _freeConstraintElements;
      _freeConstraintElements = element->next();
      element->next() = *elementPtr;
      element->constraint() = &constraint;
   }
   else
      element = new (msa) ConstraintElement(*elementPtr, &constraint);
   *elementPtr = element;
   switch (constraint.kind()) {
      case Constraint::COERCION_CONSTRAINT:
         if (!((CoercionConstraint&)constraint).noFix())
            _nCoercion++;
         break;
      case Constraint::SCHEME_CONSTRAINT:
      case Constraint::SCHEMATIC_INSTANCE_CONSTRAINT:
         _nSchematic++;
         break;
      default:
         break;
   }
}

Void ConstraintSet::remove(Constraint& constraint) {
	ConstraintElement** elementPtr = &_elements;
   while (*elementPtr != NULL) {
      if ((*elementPtr)->constraint() == &constraint) {
         ConstraintElement* element = *elementPtr;
         ConstraintElement* next = element->next();
         element->next() = _freeConstraintElements;
         _freeConstraintElements = element;
         *elementPtr = next;
         switch (constraint.kind()) {
            case Constraint::COERCION_CONSTRAINT:
               _nCoercion--;
               break;

            case Constraint::SCHEME_CONSTRAINT:
                _nSchematic--;
               break;

            default:
               break;
         }
         return;
      }
      elementPtr = &(*elementPtr)->next();
   }

	assert(FALSE, "ConstraintSet::remove: Constraint not found");
}

// ConstraintSet::seqNoInRange: Predicate to test if at least one member constraint
//                              is within a given range

Bool ConstraintSet::seqNoInRange(UInt minSeqNo, UInt maxSeqNo) const {
   ConstraintElement* constraintElement = _elements;
   while (constraintElement != NULL) {
      const Constraint& constraint = *constraintElement->constraint();
      if (constraint.kind() != Constraint::SCHEME_CONSTRAINT &&
          constraint.kind() != Constraint::SCHEMATIC_INSTANCE_CONSTRAINT &&
          constraint.seqNo() > minSeqNo &&
          constraint.seqNo() <= maxSeqNo)
         return TRUE;
      constraintElement = constraintElement->next();
   }
   return FALSE;
}

// ConstraintSet::print: Print set of constraints

Void ConstraintSet::print(ostream& os, const Env& env,
                          Bool shortForm/* = FALSE */) const {
   if (!shortForm) {
      outStream << "ConstraintSet ";
      _typeVarSet.print(os, env);
      os << '\n';
   }
   ConstraintElement* constraintElement = _elements;
	while (constraintElement != NULL) {
		os << "---";
      constraintElement->constraint()->print(os, env, shortForm);
		os << '\n';
      constraintElement = constraintElement->next();
	}
}

// ConstraintSet::merge: Mere first into second

Void ConstraintSet::merge(ConstraintSet& cs1, ConstraintSet& cs2, MSA& msa) {
	TypeVarSet::merge(cs1._typeVarSet, cs2._typeVarSet);
	ConstraintElement* element;
	while ((element = cs1.elements()) != NULL) {
		cs1.elements() = element->next();
      cs2.add(*element->constraint(), msa);
	}
}

/*----------------------------------------------------------------------------*/

UInt nextConstraintSeqNo(Bool step/* = TRUE*/) {
   static UInt seqNo = 0;
   return step ? ++seqNo : seqNo;
}

/*----------------------------------------------------------------------------*/

SchemeConstraint::SchemeConstraint(TypeSig typeSig, 
                                   TypeEnv* typeEnv,
                                   MSA& msa)
   : Constraint(Constraint::SCHEME_CONSTRAINT, NULL),
   _typeSig(mutableTypeSig(typeSig, msa)),
   _instances(NULL),
   _typeEnv(typeEnv),
   _substFlag(FALSE) {
}

// Add schematic instance constraint to list

Void SchemeConstraint::addInstance(SchematicInstanceConstraint& constraint) {
   SchematicInstanceConstraint** tailPtr = &_instances;

   while (*tailPtr != NULL)
      tailPtr = &(*tailPtr)->nextInstance();

   *tailPtr = &constraint;
}

// Remove schematic instance constraint from list

Void SchemeConstraint::removeInstance(SchematicInstanceConstraint& constraint) {
   SchematicInstanceConstraint** tailPtr = &_instances;
   while (*tailPtr != NULL) {
      if (*tailPtr == &constraint) {
         *tailPtr = constraint.nextInstance();
         break;
      }
      tailPtr = &(*tailPtr)->nextInstance();
   }
}

// SchematicConstraint::matches: Match type signatures

Bool SchemeConstraint::matches(const Constraint& constraint) {
   return eqTypeSig(_typeSig,
      static_cast<const SchemeConstraint&>(constraint)._typeSig);
}

// SchematicConstraint::substTypeVars: Apply a set of substitutions

Void SchemeConstraint::substTypeVars(const Subst* substs) {
   Subst::substTypeVars(_typeSig, substs);
   _substFlag = TRUE;
}

// SchematicConstraint::addConstraintSetTypeVars: Add to constraint set by type vars

Void SchemeConstraint::addConstraintSetTypeVars(Void* arg, MSA& msa) {
   mapProcTypeVars(_typeSig, addConstraintSetTypeVar, arg, msa);
}

// SchematicConstraint::dependsOnSubsts: Predicate for dependency on substitutions

Bool SchemeConstraint::dependsOnSubsts(const Subst* substs) const {
   return Subst::typeVarOccursInTypeSig(substs, _typeSig);
}

// SchematicConstraint::onSchemeSubst: On a type scheme substitution,
// for each instance:
//
// 1. Instantiate a fresh instance of the scheme.
// 
// 2. Unify with occurence instance.
//    (T.B.D. match type signature form to reduce the number of newly introduced type variables)
//
// 3. Instantiate constraints associated with the type scheme.
//    (T.B.D. for mutually recursive calls, any existing constraints should be removed)

Void SchemeConstraint::onSchemeSubst(TypeEnv* typeEnv, Constraint*& constraints, TypeCheck& typeCheck) {

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
   if (traceFlag) {
      outStream << "*** On type scheme substitution : ";
      print(outStream, typeCheck.env(), FALSE);
      outStream << '\n';
   }
#endif
#endif

   SchematicInstanceConstraint* instance = _instances;
   while (instance != NULL) {
      Subst* instanceSubsts = NULL;
      if (!(typeCheck.unify(_typeSig, instance->typeSig(), typeEnv, &instanceSubsts, TRUE, TRUE) &&
            instanceSubsts == NULL)) {
         TypeSig typeSig = mutableTypeSig(_typeSig, typeCheck.msa());
         TypeEnv localTypeEnv(NULL, NULL);
         TypeVarSet schematicTypeVars;
         typeCheck.resetSchematicTypeVars(typeSig, schematicTypeVars, &localTypeEnv);
         typeCheck.insOccTypeSig(typeSig, schematicTypeVars, &localTypeEnv, NULL);
         instanceSubsts = localTypeEnv.substs();
         typeCheck.deferUnification(typeSig, instance->typeSig());
      }

      TypeEnv localTypeEnv(typeEnv, instanceSubsts);
      for (UInt i = 0; i < 2; i++) {
         ConstraintSet* constraintSets = NULL;
         ConstraintSet** unresolvedList = i == 0
            ? &typeCheck.predicates().unresolvedList()
            : &typeCheck.predicates().workList();

         AddConstraintSetToListByTypeVar_Args args = {
           unresolvedList,
           &constraintSets,
           &typeCheck.env() };
         mapProcTypeVars(_typeSig,
            addConstraintSetToListByTypeVar,
            &args,
            typeCheck.msa());
         while (constraintSets != NULL) {
            ConstraintSet* constraintSet = constraintSets;
            constraintSets = constraintSets->next();
            ConstraintElement* constraintElement = constraintSet->elements();
            while (constraintElement != NULL) {
               if (constraintElement->constraint()->kind() != Constraint::SCHEME_CONSTRAINT &&
                   constraintElement->constraint()->kind() != Constraint::SCHEMATIC_INSTANCE_CONSTRAINT) {
                  Constraint* constraint = constraintElement->constraint()->instantiate(NULL,
                     &localTypeEnv,
                     instance->typeInsts(),
                     TRUE,
                     typeCheck);
                  constraint->next() = constraints;
                  constraints = constraint;
               }
               constraintElement = constraintElement->next();
            }
            constraintSet->next() = *unresolvedList;
            *unresolvedList = constraintSet;
         }
      }
      instance = instance->nextInstance();
   }
   _substFlag = FALSE;
}

Constraint* SchemeConstraint::instantiate(const ModuleDefn* moduleDefn,
                                          TypeEnv* typeEnv,
                                          TypeInsts* typeInsts,
                                          Bool unresolved,
                                          TypeCheck& typeCheck) {
   return NULL;
}

// SchematicConstraint::check: Check with given substitutions


Constraint::Result SchemeConstraint::check(Bool fix,
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
   if (traceFlag) {
      outStream << "Checking " << '[' << ++probe << "]: ";
      Subst::printSubsts(*substs, outStream, typeCheck.env(), TRUE);
      outStream << '\n';
      print(outStream, typeCheck.env(), FALSE);
      outStream << '\n';
   }
#endif

// When fixing coercions, verify all instantiated type signatures under the given substitutuons.
/*
   if (fix) {
      SchematicInstanceConstraint* instance = _instances;
      while (instance != NULL) {
         Subst* tSubsts = *substs;
         if (!typeCheck.unify(_typeSig, instance->typeSig(), typeEnv, &tSubsts, TRUE, TRUE))
            return Result::Fails;
         instance = instance->nextInstance();
      }
   }
*/
   const ExcludedConstraint exclude(excludes, this);
   return typeCheck.checkFirstConstraint(fix, constraintSet, substs, &exclude, typeEnv, resolved);
}

Bool SchemeConstraint::checkInstances(Subst** substs,
   TypeEnv* typeEnv,
   TypeCheck& typeCheck,
   UInt& nI, UInt& nU) {
   SchematicInstanceConstraint* instance = _instances;
   while (instance != NULL) {
      nI++;
      Subst* tSubsts = *substs;
      if (typeCheck.unify(_typeSig, instance->typeSig(), typeEnv, &tSubsts, TRUE, TRUE))
         nU++;
      instance = instance->nextInstance();
   }

   return nI == nU || nI > 0 && nU == 0;
}

Void SchemeConstraint::minimise(Void) {
   SchematicInstanceConstraint* instance = _instances;
   TypeSig schemeMinForm;
   while (instance != NULL) {
      TypeSigPair minForm = minimalForm({ _typeSig, instance->typeSig() });
      if (instance == _instances)
         schemeMinForm = minForm._x;
      else
         if (!eqTypeSig(schemeMinForm, minForm._x))
            return;
      instance = instance->nextInstance();
   }
   _typeSig = schemeMinForm;
   instance = _instances;
   while (instance != NULL) {
      TypeSigPair minForm = minimalForm({ _typeSig, instance->typeSig() });
      instance->typeSig() = minForm._y;
      instance = instance->nextInstance();
   }
}

// SchematicConstraint::print: Print

Void SchemeConstraint::print(ostream& os, const Env& env, Bool shortForm) const {
   os << "Schematic (" << _seqNo << ") ";
   printTypeSig(_typeSig, 0, FALSE, os, env);
   os << " instances: ";
   SchematicInstanceConstraint* instance = _instances;
 
   while (instance != NULL) {
      if (instance != _instances)
         os << ", ";
      printTypeSig(instance->typeSig(), 0, FALSE, os, env);
      instance = instance->nextInstance();
   }
}


/*----------------------------------------------------------------------------*/

SchematicInstanceConstraint::SchematicInstanceConstraint(
   SchemeConstraint* schemeConstraint,
   TypeSig typeSig,
   TypeInsts* typeInsts)
: Constraint(Constraint::SCHEMATIC_INSTANCE_CONSTRAINT, NULL),
   _schemeConstraint(schemeConstraint),
   _nextInstance(NULL),
   _typeSig(typeSig),
   _typeInsts(typeInsts) {
}

// SchematicInstanceConstraint::matches: Match type signatures

Bool SchematicInstanceConstraint::matches(const Constraint& constraint) {
   return
      _schemeConstraint == 
         static_cast<const SchematicInstanceConstraint&>(constraint)._schemeConstraint &&
      eqTypeSig(_typeSig,
      static_cast<const SchematicInstanceConstraint&>(constraint)._typeSig);
}

// SchematicInstanceConstraint::onMatched: On matched, remove instance from scheme constraint

Void SchematicInstanceConstraint::onMatched(const Constraint& constraint) {
   if (_schemeConstraint != NULL)
      _schemeConstraint->removeInstance(*this);
}

// SchematicConstraint::substTypeVars: Apply a set of substitutions

Void SchematicInstanceConstraint::substTypeVars(const Subst* substs) {
   Subst::substTypeVars(_typeSig, substs);
}

// SchematicConstraint::addConstraintSetTypeVars: Add to constraint set by type vars

Void SchematicInstanceConstraint::addConstraintSetTypeVars(Void* arg, MSA& msa) {
   mapProcTypeVars(_typeSig, addConstraintSetTypeVar, arg, msa);
}

// SchematicConstraint::dependsOnSubsts: Predicate for dependency on substitutions

Bool SchematicInstanceConstraint::dependsOnSubsts(const Subst* substs) const {
   return Subst::typeVarOccursInTypeSig(substs, _typeSig);
}

Constraint* SchematicInstanceConstraint::instantiate(const ModuleDefn* moduleDefn,
   TypeEnv* typeEnv,
   TypeInsts* typeInsts,
   Bool unresolved,
   TypeCheck& typeCheck) {
   return NULL;
}

// SchematicInstanceConstraint::check: With given substitutions
// ensure instance of scheme

Constraint::Result SchematicInstanceConstraint::check(Bool fix,
                                                      const ConstraintSet& constraintSet,
                                                      Subst** substs,
                                                      const ExcludedConstraint* excludes,
                                                      TypeEnv* typeEnv,
                                                      TypeCheck& typeCheck,
                                                      Constraint*& resolved) {

/*
   Subst* tSubsts = *substs;
   if (!typeCheck.unify(_schemeConstraint->typeSig(), _typeSig, typeEnv, &tSubsts, TRUE, TRUE))
      return Result::Fails;
*/

   const ExcludedConstraint exclude(excludes, this);
   return typeCheck.checkFirstConstraint(fix, constraintSet, substs, &exclude, typeEnv, resolved);
}

// SchematicInstanceConstraint::print: Print

Void SchematicInstanceConstraint::print(ostream& os, const Env& env, Bool shortForm) const {
   os << "SchematicInstance (" << _seqNo << ") ";
   printTypeSig(_typeSig, 0, FALSE, os, env);
   os << " of type scheme ";
   if (_schemeConstraint != NULL)
      _schemeConstraint->print(os, env, shortForm);
}

/*----------------------------------------------------------------------------*/

Bool ReductionConstraint::matches(const Constraint& constraint) {
   return
      eqTypeSig(_x, static_cast<const ReductionConstraint&>(constraint)._x)
      &&
      eqTypeSig(_y, static_cast<const ReductionConstraint&>(constraint)._y);
}

// ReductionConstraint::substTypeVars: Apply a set of substitutions

Void ReductionConstraint::substTypeVars(const Subst* substs) {
   Subst::substTypeVars(_x, substs);
   Subst::substTypeVars(_y, substs);
}

// ReductionConstraint::addConstraintSetTypeVars: Add to constraint set by type vars

Void ReductionConstraint::addConstraintSetTypeVars(Void* arg, MSA& msa) {
   mapProcTypeVars(_x, addConstraintSetTypeVar, arg, msa);
   mapProcTypeVars(_y, addConstraintSetTypeVar, arg, msa);
}

// ReductionConstraint::dependsOnSubsts: Predicate for dependency on substitutions

Bool ReductionConstraint::dependsOnSubsts(const Subst* substs) const {
   return Subst::typeVarOccursInTypeSig(substs, _x);
}

// ReductionConstraint::instantiate

Constraint* ReductionConstraint::instantiate(const ModuleDefn* moduleDefn,
                                             TypeEnv* typeEnv,
                                             TypeInsts* typeInsts,
                                             Bool unresolved,
                                             TypeCheck& typeCheck) {
   CopyContext cc(moduleDefn, typeCheck.nameTable(),
      typeCheck.predicates().varNameMaps(),
      typeCheck.predicates().typeVarMaps(),
      typeCheck.msa());
   ReductionConstraint* constraint = new(typeCheck.msa())
      ReductionConstraint(Subst::uniqueTypeSig(_x, cc, typeEnv, typeCheck.msa()),
                          Subst::uniqueTypeSig(_y, cc, typeEnv, typeCheck.msa()),
                          _evalFlag);

#ifdef TRACE
   if (traceFlag) {
      outStream << "*** Instantiated reduce constraint\n";
      print(outStream,
            moduleDefn == NULL ? typeCheck.env() : moduleDefn->env(), FALSE);
      outStream << "\n*** as\n";
      constraint->print(outStream, typeCheck.env(), FALSE);
      outStream << "\n";
   }
#endif

   return constraint;
}

// ReductionConstraint::check: Check with given substitutions

Constraint::Result ReductionConstraint::check(Bool fix,
                                              const ConstraintSet& constraintSet,
                                              Subst** substs, // Not const due to unify
                                              const ExcludedConstraint* excludes,
                                              TypeEnv* typeEnv,
                                              TypeCheck& typeCheck,
                                              Constraint*& resolved) {
   TypeSig x = _x;
   const Subst* subst;
   while (isTypeVar(x) && (subst = Subst::lookUp(toTypeVar(x), *substs)) != NULL)
      x = subst->typeSig();

   if (isTypeVar(x))
      return Result::Unresolved;
   else
      if (_evalFlag)
         x = evalTypeSig(x);
      else
         if (!isExprTypeSig(x))
            return Result::Fails;
         else
            x = denotedTypeSig(x);

   if (typeCheck.unify(x, _y, NULL, substs)) {
      const ExcludedConstraint exclude(excludes, this);
      Constraint* tResolved = NULL;
      Result res = typeCheck.checkFirstConstraint(fix, constraintSet, substs, &exclude, typeEnv, tResolved);
      switch (res.tag()) {
         case Result::SetIsEmpty:
         case Result::Unresolved:
            tResolved = NULL;
            //------ drop through

         case Result::Satisfied:
            _next = tResolved;
            resolved = this;

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
            return Result(Result::Satisfied);
#else
            return Result(Result::Satisfied, tResolved != NULL ? res.nCast() : 0,
                                             tResolved != NULL ? res.nSubordinate() : 0);
#endif

         case Result::Fails:
         default: // Satisfy control path return
            return Result::Fails;
      }
   }
   else
      return Result::Fails;
}

// ReductionConstraint::print: Print

Void ReductionConstraint::print(ostream& os, const Env& env, Bool shortForm) const {
   if (_evalFlag)
      os << '!';
   else
      os << "#!";
   os << " (" << _seqNo << ") ";
   printTypeSig(_x, 0, FALSE, os, env);
   os << '=';
   printTypeSig(_y, 0, FALSE, os, env);
}

/*----------------------------------------------------------------------------*/

CoercionConstraint::CoercionConstraint(TypeSig x, TypeSig y,
                                       ClassConstraint* classConstraint)
   : Constraint(COERCION_CONSTRAINT), _x(x), _y(y),
     _classConstraint(classConstraint),
     _applyCast(FALSE), _prevApplyCast(FALSE), _noFix(FALSE) {
}


Void CoercionConstraint::appendTransforms(Constraint& constraint) {
   Constraint::appendTransforms(constraint);
   _classConstraint->appendTransforms(*((CoercionConstraint&)constraint)._classConstraint);
}

Bool CoercionConstraint::matches(const Constraint& constraint) {
   return
      eqTypeSig(_x, static_cast<const CoercionConstraint&>(constraint)._x)
      &&
      eqTypeSig(_y, static_cast<const CoercionConstraint&>(constraint)._y);
}

// CoercionConstraint::substTypeVars: Apply a set of substitutions

Void CoercionConstraint::substTypeVars(const Subst* substs) {
	Subst::substTypeVars(_x, substs);
	Subst::substTypeVars(_y, substs);
   _classConstraint->substTypeVars(substs);
}

// CoercionConstraint::addConstraintSetTypeVars: Add to constraint set by type vars

Void CoercionConstraint::addConstraintSetTypeVars(Void* arg, MSA& msa) {
	mapProcTypeVars(_x, addConstraintSetTypeVar, arg, msa);
	mapProcTypeVars(_y, addConstraintSetTypeVar, arg, msa);
   _classConstraint->addConstraintSetTypeVars(arg, msa);
}

// CoercionConstraint::instantiate

Constraint* CoercionConstraint::instantiate(const ModuleDefn* moduleDefn,
                                            TypeEnv* typeEnv,
                                            TypeInsts* typeInsts,
                                            Bool unresolved,
                                            TypeCheck& typeCheck) {
   CopyContext cc(moduleDefn, typeCheck.nameTable(),
                  typeCheck.predicates().varNameMaps(),
                  typeCheck.predicates().typeVarMaps(),
                  typeCheck.msa());
   CoercionConstraint* constraint = new(typeCheck.msa())CoercionConstraint(
      Subst::uniqueTypeSig(_x, cc, typeEnv, typeCheck.msa()),
      Subst::uniqueTypeSig(_y, cc, typeEnv, typeCheck.msa()),
      (ClassConstraint*)_classConstraint->instantiate(moduleDefn, typeEnv,
                                                      typeInsts,
                                                      unresolved, typeCheck));

   ConstraintTransform* transform = transforms();
   while (transform != NULL) {
      InstantiatedCoercionConstraintTransform* insTransform = new (typeCheck.msa())
         InstantiatedCoercionConstraintTransform(NULL,
            *constraint,
            ((AbstractCoercionConstraintTransform*)transform)->coercion(),
            typeInsts);
      ((Constraint*)constraint)->appendTransforms((ConstraintTransform*)insTransform);
      transform = transform->next();
   }

#ifdef TRACE
	if (traceFlag) {
      outStream << "*** Instantiated coercion constraint\n";
      print(outStream,
            moduleDefn == NULL ? typeCheck.env() : moduleDefn->env(),
            FALSE);
      outStream << "\n*** as\n";
      constraint->print(outStream, typeCheck.env(), FALSE);
      outStream << "\n";
   }
#endif

   return constraint;
}

// CoercionConstraint::dependsOnSubsts: Predicate for dependency on substitutions

Bool CoercionConstraint::dependsOnSubsts(const Subst* substs) const {
	return Subst::typeVarOccursInTypeSig(substs, _x) ||
             Subst::typeVarOccursInTypeSig(substs, _y) ||
          _classConstraint->dependsOnSubsts(substs);
}

// CoercionConstraint::check: Check with given substitutions

Constraint::Result CoercionConstraint::check(Bool fix,
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
   if (traceFlag) {
      outStream << "Checking " << '[' << ++probe << "]: ";
      Subst::printSubsts(*substs, outStream, typeCheck.env(), TRUE);
      outStream << '\n';
      print(outStream, typeCheck.env(), FALSE);
      outStream << '\n';
   }
#endif

   Subst* tSubsts = *substs;

/*
   Bool res = typeCheck.unify(_x, _y, typeEnv, &tSubsts) &&
              (!(fix && !_noFix) || unifySchematic(_x, _y, substs,
                                                   constraintSet,
                                                   typeEnv, typeCheck));
*/
   Bool res = typeCheck.unify(_x, _y, typeEnv, &tSubsts);

   if (res) {
      Bool unresolved = !(fix && !_noFix) && tSubsts != *substs;

      UInt nI = 0;
      UInt nU = 0;
      if ((fix && !_noFix)) {
         ConstraintElement* constraintElement = constraintSet.elements();
         while (constraintElement != NULL) {
            Constraint* constraint = constraintElement->constraint();
            if (constraint->kind() == SCHEME_CONSTRAINT)
               if (!((SchemeConstraint&)*constraint).checkInstances(&tSubsts, typeEnv, typeCheck, nI, nU))
                  return Result::Fails;
            constraintElement = constraintElement->next();
         }
      }

      if (nI == nU) {
		   const ExcludedConstraint exclude(excludes, this);
		   Constraint* tResolved = NULL;
         Constraint::Result constraintRes = typeCheck.checkFirstConstraint((fix && !_noFix), constraintSet, &tSubsts, &exclude, typeEnv, tResolved);
		   switch (constraintRes.tag()) {
			   case Result::SetIsEmpty:
			   case Result::Unresolved:
               tResolved = NULL;
   //------ drop through

            case Result::Satisfied:
               if (unresolved) {

   #ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
                  if (traceFlag)
                     outStream << '(' << _seqNo << "): Unresolved\n";
   #endif

                  return Result::Unresolved;
               } else {
                  _prevApplyCast = _applyCast;
                  _applyCast = FALSE;
                  _next = tResolved;
                  resolved = this;
                  *substs = tSubsts;

   #ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
                  if (traceFlag)
                     outStream << '(' << _seqNo << "): Satisfied\n";
   #endif

   #ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
                  return Result(Result::Satisfied);
   #else
                  return Result(Result::Satisfied, tResolved != NULL ? constraintRes.nCast() : 0,
                                                   tResolved != NULL ? constraintRes.nSubordinate() : 0);
   #endif

               }
         
			   case Result::Fails:
			   default: // Satisfy control path return
				   break;
		   }
      }
   }

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL 
   if (traceFlag)
      outStream << '(' << _seqNo << "): Fails, trying cast\n";
#endif

	Constraint* tResolved = NULL;
   const ExcludedConstraint exclude(excludes, this);
   _classConstraint->init(typeCheck);

   Result ccRes = _classConstraint->check((fix && !_noFix), constraintSet, substs, &exclude, typeEnv, typeCheck, tResolved);
   switch (ccRes.tag()) {
		case Result::SetIsEmpty:
         tResolved = NULL;
//------ drop through
      case Result::Satisfied:
         _prevApplyCast = _applyCast;
         _applyCast = TRUE;
         _next = tResolved;
         resolved = this;

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
         if (traceFlag) {
            outStream << '(' << _seqNo << ") Satisfied with cast: ";
            Subst::printSubsts(*substs, outStream, typeCheck.env(), TRUE);
            outStream << '\n';
         }
#endif

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
         return Result(Result::Satisfied);
#else
         return Result(Result::Satisfied, tResolved != NULL ? ccRes.nCast() + 1 : 0,
                                          tResolved != NULL ?  ccRes.nSubordinate() : 0);
#endif

      case Result::Unresolved: {

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
         if (traceFlag)
            outStream << '(' << _seqNo << "): Unresolved\n";
#endif

         return Result::Unresolved;
      }

		case Result::Fails:

		default: // Satisfy control path return
			break;
   }

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
   if (traceFlag)
      outStream << '(' << _seqNo << "): Fails\n";
#endif

   return Result::Fails;
}

// CoercionConstraint::revertSolution

Void CoercionConstraint::revertSolution(Void) {
   _applyCast = _prevApplyCast;
   _prevApplyCast = FALSE;
}

// CoercedConstraint::onSatisfied: On satisfied action

Void CoercionConstraint::onSatisfied(ConstraintSet& constraintSet, const Subst* substs,
                                     TypeCheck& typeCheck) {
   TypeSig typeSig;

// If applying the cast, but its constraint is unresolved, detach it as independent

   if (_applyCast && _next != _classConstraint) {
      _classConstraint->indep() = TRUE;

      _classConstraint->substTypeVars(substs);
      typeCheck.addConstraintToSet(*_classConstraint);
   }
}

// CoercionConstraint::print: Print

Void CoercionConstraint::print(ostream& os, const Env& env, Bool shortForm) const {
	os << "coerce (" << _seqNo << ") ";
   os << ' ';
	printTypeSig(_x, 0, FALSE, os, env);
	os << " to ";
	printTypeSig(_y, 0, FALSE, os, env);

   os << ' ';
   _classConstraint->print(os, env, FALSE);
}

// CoercedConstraintTransform::apply: On satisfied action

Void AbstractCoercionConstraintTransform::apply(const Subst* substs,
                                                Constraint*& constraints,
                                                TypeEnv* typeEnv,
                                                TypeCheck& typeCheck) {
   CoercionConstraint& constraint = *(CoercionConstraint*)_constraint;

   TypeSig typeSig = !constraint.applyCast()
      ? (TypeSig)UNKNOWN
      : (TypeSig)ap2(ARROW, constraint.x(), constraint.y(), typeCheck.msa());

   resolve(constraint.applyCast(), typeSig, typeCheck.msa());
}

Void CoercionConstraintTransform::resolve(Bool applyCast, TypeSig typeSig, MSA& msa) {
   _coercion.castFlag() = applyCast;
   _coercion.resolved() = TRUE;
}

InstantiatedCoercionConstraintTransform::InstantiatedCoercionConstraintTransform(
   ConstraintTransform* next,
   Constraint& constraint,
   Coercion& coercion,
   TypeInsts* typeInsts)
   : AbstractCoercionConstraintTransform(next, constraint),
   _coercionInst(typeInsts->_coercionInsts, &coercion) {
   typeInsts->_coercionInsts = &_coercionInst;
}

Void InstantiatedCoercionConstraintTransform::resolve(Bool applyCast, TypeSig typeSig, MSA& msa) {
   _coercionInst.resolved() = TRUE;
   _coercionInst.applyCast() = applyCast;
   _coercionInst.typeSig() = typeSig;
}

Void CoercedConstraintTransform::resolve(Bool applyCast, TypeSig typeSig, MSA& msa) {
   if (applyCast) {
      Coerced& coerced = (Coerced&)_coercion;
      Cell& cell_ = *new(sizeof(FnAp), msa) Cell(FN_AP);
      new(cell_.body()) FnAp(coerced.castOcc(), coerced.expr(), typeSig, msa);
      coerced.expr() = mkPair(REDUCE, fromCell(cell_), msa);
   }
   CoercionConstraintTransform::resolve(applyCast, typeSig, msa);
}

/*----------------------------------------------------------------------------*/

Bool CaseConstraint::matches(const Constraint& constraint) {
   if (!eqTypeSig(_x, static_cast<const CaseConstraint&>(constraint)._x))
      return FALSE;

   return eqTypeSig(_xs, static_cast<const CaseConstraint&>(constraint)._xs);
}

// CaseConstraint::substTypeVars: Apply a set of substitutions

Void CaseConstraint::substTypeVars(const Subst* substs) {
   Subst::substTypeVars(_x, substs);
   Expr xs = _xs;
   while (xs != Nil) {
      Subst::substTypeVars(hd(xs), substs);
      xs = tl(xs);
   }
}

// CaseConstraint::addConstraintSetTypeVars: Add to constraint set by type vars

Void CaseConstraint::addConstraintSetTypeVars(Void* arg, MSA& msa) {
   mapProcTypeVars(_x, addConstraintSetTypeVar, arg, msa);
   Expr xs = _xs;
   while (xs != Nil) {
      mapProcTypeVars(hd(xs), addConstraintSetTypeVar, arg, msa);
      xs = tl(xs);
   }
}

// CaseConstraint::dependsOnSubsts: Predicate for dependency on substitutions

Bool CaseConstraint::dependsOnSubsts(const Subst* substs) const {
   Expr xs = _xs;
   while (xs != Nil) {
      if (Subst::typeVarOccursInTypeSig(substs, hd(xs)))
         return TRUE;
      xs = tl(xs);
   }
   return Subst::typeVarOccursInTypeSig(substs, _x);
}

// CaseConstraint::instantiate:

Constraint* CaseConstraint::instantiate(const ModuleDefn* moduleDefn,
                                        TypeEnv* typeEnv,
                                        TypeInsts* typeInsts,
                                        Bool unresolved,
                                        TypeCheck& typeCheck) {
    CopyContext cc(moduleDefn, typeCheck.nameTable(),
                  typeCheck.predicates().varNameMaps(),
                  typeCheck.predicates().typeVarMaps(),
                  typeCheck.msa());

    CaseConstraint* constraint = new(typeCheck.msa())
      CaseConstraint(Subst::uniqueTypeSig(_x, cc, typeEnv, typeCheck.msa()),
                     Subst::uniqueTypeSigList(_xs, cc, typeEnv, typeCheck.msa()));
 
#ifdef TRACE
   if (traceFlag) {
      outStream << "*** Instantiated case constraint\n";
      print(outStream,
            moduleDefn == NULL ? typeCheck.env() : moduleDefn->env(),
            FALSE);
      outStream << "\n*** as\n";
      constraint->print(outStream, typeCheck.env(), FALSE);
      outStream << "\n";
   }
#endif

   return constraint;
}

// CaseConstraint::check: Check with given substitutions

Constraint::Result CaseConstraint::check(Bool fix,
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
   if (traceFlag) {
      outStream << "Checking " << '[' << probe << "]: ";
      Subst::printSubsts(*substs, outStream, typeCheck.env(), TRUE);
      outStream << '\n';
      print(outStream, typeCheck.env(), FALSE);
      outStream << ' ';
   }
#endif

   Subst* tSubsts = *substs;
   Bool res = TRUE;
   Expr xs = _xs;
   while (xs != Nil) {
      if (!typeCheck.unify(_x, hd(xs), typeEnv, &tSubsts)) {
         res = FALSE;
         break;
      }
      xs = tl(xs);
   }

   if (res) {
      Bool unresolved = !fix && tSubsts != *substs/* ||
                                                  fix && hasTypeVar(_x, *substs)*/;

      const ExcludedConstraint exclude(excludes, this);
      Constraint* tResolved = NULL;
      Result res = typeCheck.checkFirstConstraint(fix, constraintSet, &tSubsts, &exclude, typeEnv, tResolved);
      switch (res.tag()) {
         case Result::SetIsEmpty:
         case Result::Unresolved:
            tResolved = NULL;
//--------- drop through

         case Result::Satisfied:
            if (unresolved)
               return Result::Unresolved;
            else {
               _next = resolved;
               resolved = this;
               *substs = tSubsts;

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
               if (traceFlag)
                  outStream << '(' << _seqNo << ") Satisfied\n";
#endif

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
               return Result(Result::Satisfied);
#else
               return Result(Result::Satisfied, tResolved != NULL ? res.nSubordinate() : 0);
#endif

            }

         case Result::Fails:
         default: // Satisfy control path return
            break;
      }
   }

#ifdef TRACE_CONSTRAINT_CHECKING_IN_DETAIL
      outStream << '(' << _seqNo << ")  Fails, trying with * pattern\n";
#endif

   if (typeCheck.unify(_x, typeCon(Any), typeEnv, &tSubsts)) {
      const ExcludedConstraint exclude(excludes, this);
      Constraint* tResolved = NULL;
      Result res = typeCheck.checkFirstConstraint(fix, constraintSet, &tSubsts, &exclude, typeEnv, tResolved);
      switch (res.tag()) {
         case Result::SetIsEmpty:
         case Result::Unresolved:
            tResolved = NULL;
            //------ drop through

         case Result::Satisfied:
            _next = tResolved;
            resolved = this;

#ifndef SUBORDINATE_AND_INSTANCE_CAST_PRIORITY
            return Result(Result::Satisfied);
#else
            return Result(Result::Satisfied, tResolved != NULL ? 0 : res.nSubordinate());
#endif

         case Result::Fails:
         default: // Satisfy control path return
            return Result::Fails;
      }
   }
   else
      return Result::Fails;
}

// CaseConstraint::print: Print

Void CaseConstraint::print(ostream& os, const Env& env, Bool shortForm) const {
   os << "case (" << _seqNo << ") ";
   printTypeSig(_x, 0, FALSE, os, env);
   os << ',';
   printTranExpr(_xs, FALSE, os, env);
}



// findConstraintSet: constraint set for constraint

ConstraintSet* findConstraintSetByConstraint(Constraint* constraint,
                                             ConstraintSet** list) {
   ConstraintSet* constraintSet;
   while ((constraintSet = *list) != NULL) {
      ConstraintElement* element = (*list)->elements();
      while (element != NULL) {
         if (element->constraint() == constraint)
            return constraintSet;
         element = element->next();
      }
      list = &constraintSet->next();
   }
   return NULL;
}

/*----------------------------------------------------------------------------*/

// Var::insTypeSig: Instantiate a type signature for a formal parameter
//                  (only form of variable at this stage)

Void Var::insTypeSig(TypeSig& typeSig, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   typeCheck.bindTypeConNames(typeSig);
   if (hasTypeVar(typeSig)) {
      typeSig = mutableTypeSig(typeSig, typeCheck.msa());
      ::insTypeSig(typeSig, typeEnv, NULL, typeCheck.msa());
   }
}

// TypedVal::addInstance: Add instance to list

Void TypedVal::addInstance(TypeSig& typeSig, TypeInsts* typeInsts, TypeEnv* typeEnv, TypeCheck& typeCheck) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Adding instance ";
      printTypeSig(typeSig, 0, FALSE, outStream, typeCheck.env());
      outStream << " of scheme ";
      printTypeSig(_typeSig, 0, FALSE, outStream, typeCheck.env());
      outStream << "\n";
   }
#endif

   if (_needsInstance) {
      if (_schemeConstraint == NULL) {
         _schemeConstraint = new(typeCheck.msa()) SchemeConstraint(_typeSig, _typeEnv, typeCheck.msa());
         typeCheck.addConstraint(_schemeConstraint, typeEnv, TRUE);
      }
      SchematicInstanceConstraint* constraint = new(typeCheck.msa()) SchematicInstanceConstraint(_schemeConstraint, typeSig, typeInsts);
      _schemeConstraint->addInstance(*constraint);
      typeCheck.addConstraint(constraint, typeEnv, TRUE);

   }
}

/*----------------------------------------------------------------------------*/

// NameTypedValBinding::insTypeSig: Instantiate the binding type signature

Void NameTypedValBinding::insTypeSig(TypeEnv* typeEnv, TypeCheck& typeCheck) { 
   typeCheck.insTypeSig(_val.val(), _val, typeEnv);
}	

// NameTypedValBinding::unify: Unify binding type signature

Void NameTypedValBinding::unify(TypeSig ts, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   typeCheck.unifyOrError(ts, _val.typeSig(), typeEnv,
                          "Failed to unify with previous declaration");
}	

// NameTypedValBinding::typeCheck: Type check binding

Void NameTypedValBinding::typeCheck(TypeEnv* typeEnv, TypeSig failTS, TypeCheck& typeCheck) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "NameTypedValBinding::typeCheck: ";
      printName(name(), outStream, typeCheck.nameTable());
      outStream << '\n';
   }
#endif

   typeCheck.typeCheck(_val.val(), _val, typeEnv, failTS);

#ifdef TRACE
	if (traceFlag) {
		outStream << "NameTypedValBinding: ";
      printName(name(), outStream, typeCheck.nameTable());
      outStream << " :: ";
      printTypeSig(_val.typeSig(), 0, FALSE, outStream, typeCheck.env());
		outStream << endl;
	}
#endif

}

// NameTypedValBinding::resetNeedsInstance: Reset needs instance flag for
// mutually recursive declarations and definitions

Void NameTypedValBinding::resetNeedsInstance(TypeEnv* typeEnv, TypeCheck& typeCheck) {
   typeCheck.resetNeedsInstance(_val.val(), _val, typeEnv);
}

/*----------------------------------------------------------------------------*/

// SyntaxForm::infer: Catch non-overloaded syntax forms

TypeSig AST_Node::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   notOverloaded("infer", typeCheck.env());
   return (Expr)ERROR;
}

/*----------------------------------------------------------------------------*/

Void TypeDefn::insTypeSigs(TypeCheck& typeCheck) {
   typeCheck.insDeclOrDefnsTypeSigs(_dataConDecls, NULL);
   mapProc1(insNilaryDataConDefnTypeSigs, _nilaryDataConDefns, typeCheck);
}

Void TypeDefn::insNilaryDataConDefnTypeSigs(Expr nilaryDataConDefn, TypeCheck& typeCheck) {
   toBody(nilaryDataConDefn, DataConDefn).insTypeSigs(NULL, typeCheck);
}

Void TypeDefn::typeCheckNilaryDataConDefn(Expr nilaryDataConDefn, TypeCheck& typeCheck) {
   toBody(nilaryDataConDefn, DataConDefn).typeCheck(typeCheck);
}

Void TypeDefn::typeCheck(TypeCheck& typeCheck) {
   //   typeCheck.typeCheckDeclOrDefns(_dataConDecls, TRUE, typeEnv, ERROR);
   mapProc1(typeCheckNilaryDataConDefn, _nilaryDataConDefns, typeCheck);
}

Void TypeDefn::resetNilaryDataConDefnNeedsInstance(Expr nilaryDataConDefn, TypeCheck& typeCheck) {
   toBody(nilaryDataConDefn, DataConDefn).resetNeedsInstance(typeCheck);
}

Void TypeDefn::resetNeedsInstance(TypeCheck& typeCheck) {
   mapProc1(resetNilaryDataConDefnNeedsInstance, _nilaryDataConDefns, typeCheck);
}

/*----------------------------------------------------------------------------*/

Void DataConDecl::insTypeSig(TypeEnv* typeEnv, TypeCheck& typeCheck) {
   TypeEnv localTypeEnv(typeEnv, _dataConBinding->substs());
   typeCheck.insDeclOrDefnTypeSig(_reprDecl, &localTypeEnv);
}

/*----------------------------------------------------------------------------*/

Void DataConDefn::insTypeSigs(TypeEnv* typeEnv, TypeCheck& typeCheck) {
   TypeEnv localTypeEnv(typeEnv, _dataConBinding->substs());
   NameTypedValBinding& reprDefnBinding = toBody(_reprDefn, NameTypedValBinding);
   TypedVal& typedVal = reprDefnBinding.typedVal();
   typedVal.setSchematic(FALSE);
   typeCheck.insDeclOrDefnTypeSig(_reprDefn, &localTypeEnv);
   typeCheck.insDeclOrDefnTypeSig(_defn, &localTypeEnv);
}	

Void DataConDefn::typeCheck(TypeCheck& typeCheck) {
   TypeEnv typeEnv(NULL, _dataConBinding->substs());

   typeCheck.typeCheckDeclOrDefn(_reprDefn, &typeEnv, ERROR);

// Assign CONSTRUCT application signature

   DataCon& dataCon = toBody(_dataConBinding->val(), DataCon);
 
   dataCon.reprTypeSig() = fromTypeVar(newTypeVar());
   TypeSig typeSig = arg(_dataConBinding->typeSig());
   NameTypedValBinding& reprDefnBinding = toBody(_reprDefn, NameTypedValBinding);
   dataCon.reprTypeSig() = reprDefnBinding.typedVal().typeSig();

   if (dataCon.arity() > 0) {
      typeSig = resultTypeSig(typeSig, dataCon.arity());
      dataCon.reprTypeSig() = resultTypeSig(dataCon.reprTypeSig(), dataCon.arity());;
   }

   Expr expr = toBody(toBody(_defn, NameTypedValBinding).typedVal().val(), Defn)._expr;
   toBody(dataCon.arity() == 0
      ? expr
      : snd(toBody(expr, Lambda)._body), FnAp)._typeSig =
         ap2(ARROW, dataCon.reprTypeSig(), typeSig, typeCheck.msa());

   typeCheck.typeCheckDeclOrDefn(_defn, &typeEnv, ERROR);

   UInt index = 0;
   mapProc4(typeCheckSelector, _selectors, index, arg(_dataConBinding->typeSig()), &typeEnv, typeCheck);

// Reset schematic type variables for both declaration and definition

   Subst::substTypeVars(_dataConBinding->typeSig(), *typeCheck.substs());
   Subst::substTypeVars(dataCon.reprTypeSig(), *typeCheck.substs());

   typeCheck.resetSchematicTypeVars(_dataConBinding->typeSig(), _dataConBinding->schematicTypeVars(), NULL);

   TypedVal& defnTypedVal = toBody(_defn, NameTypedValBinding).typedVal();
   Subst::substTypeVars(defnTypedVal.typeSig(), *typeCheck.substs());

   typeCheck.resetSchematicTypeVars(defnTypedVal.typeSig(), defnTypedVal.schematicTypeVars(), NULL);

#ifdef TRACE_
	//if (_traceFlag)
	//	traceFlag = !traceFlag;
#endif

}

Void DataConDefn::resetNeedsInstance(TypeCheck& typeCheck) {
   typeCheck.resetDeclOrDefnNeedsInstance(_reprDefn, NULL);
   typeCheck.resetDeclOrDefnNeedsInstance(_defn, NULL);
   mapProc1(resetSelectorNeedsInstance, _selectors, typeCheck);
}

Void DataConDefn::typeCheckSelector(Expr selector, UInt& index, TypeSig dataConTypeSig,
                                    TypeEnv* typeEnv, TypeCheck& typeCheck) {
   index++;
	assert(formOf(selector) == NAME_TYPED_VAL_BINDING, "DataConDefn::typeCheckSelector: expected binding");
	NameTypedValBinding& binding = toBody(selector, NameTypedValBinding);

// Assign binding type signature

   DataCon& dataCon = toBody(_dataConBinding->val(), DataCon);
   binding.typedVal().typeSig() = ap2(ARROW, dataCon.reprTypeSig(),
                                             argTypeSig(dataConTypeSig, index), typeCheck.msa());
   binding.typeCheck(typeEnv, TypeSig(ERROR), typeCheck);
}

Void DataConDefn::resetSelectorNeedsInstance(Expr selector, TypeCheck& typeCheck) {
   assert(formOf(selector) == NAME_TYPED_VAL_BINDING, "DataConDefn::resetSelectorNeedsInstance: expected binding");
   NameTypedValBinding& binding = toBody(selector, NameTypedValBinding);

   binding.resetNeedsInstance(NULL, typeCheck);
}

/*----------------------------------------------------------------------------*/

Void ConcreteDataCon::instantiate(TypeEnv* typeEnv, TypeCheck& typeCheck) {
   TypeSig typeSig;

// All data constructors have type Exp a ,so obtain a

   typeSig = arg(_typedVal->typeSig());
   Subst::substTypeVars(typeSig, _typedVal->_substs);

   _reprTypeSig = toBody(_dataCon, DataCon).reprTypeSig();

   if (hasTypeVar(typeSig)) {
      TypeEnv localTypeEnv;
         _reprTypeSig = mutableTypeSig(_reprTypeSig, typeCheck.msa());
         Subst::substTypeVars(_reprTypeSig, _typedVal->_substs);
      if (_moduleDefn == NULL)
         typeSig = mutableTypeSig(typeSig, typeCheck.msa());
      else {
         CopyContext cc(_moduleDefn, typeCheck.nameTable(),
                        typeCheck.predicates().varNameMaps(),
                        typeCheck.predicates().typeVarMaps(),
                        typeCheck.msa());
         _mappedTypeSig = mutableTypeSig(typeSig, cc, typeCheck.msa());
         typeSig = copyTypeSig(_mappedTypeSig, typeCheck.msa());
         mapTypeSig(_reprTypeSig, cc, typeCheck.msa());
         _mappedReprTypeSig = copyTypeSig(_mappedReprTypeSig, typeCheck.msa());
      }

      if (_typedVal->_schematic && !_typedVal->_schematicTypeVars.isEmpty()) {
         TypeEnv localTypeEnv(NULL);
         if (_moduleDefn == NULL)
            typeCheck.insOccTypeSig(typeSig, _typedVal->_schematicTypeVars, &localTypeEnv, NULL);
         else {
            CopyContext cc(_moduleDefn, typeCheck.nameTable(),
                           typeCheck.predicates().varNameMaps(),
                           typeCheck.predicates().typeVarMaps(),
                           typeCheck.msa());
            TypeVarSet schematicTypeVars =
               TypeVarSet(_typedVal->_schematicTypeVars, &cc, typeCheck.msa());
            typeCheck.insOccTypeSig(typeSig, schematicTypeVars, &localTypeEnv, NULL);
         }
         insTypeSig(_reprTypeSig, &localTypeEnv, NULL, typeCheck.msa());
         _typedVal->addInstance(typeSig, NULL, typeEnv, typeCheck);

         _substs = localTypeEnv.substs();
      }
   }

   _typeSig = typeSig;
}

TypeSig ConcreteDataCon::infer(TypeEnv* typeEnv, TypeCheck& typeCheck,
                               TypeSig failTS, Bool& nonStrictFlag) {
   instantiate(typeEnv, typeCheck);
   UInt arity = toBody(_dataCon, DataCon).arity();
   return ap2(ARROW,
              _reprTypeSig,
              arity == 0
                 ? _typeSig
                 : resultTypeSig(_typeSig, arity), typeCheck.msa());
}

/*----------------------------------------------------------------------------*/

// Decl::insTypeSig: Instantiate a type signature for a declaration

Void Decl::insTypeSig(TypedVal& typedVal, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   TypeEnv localTypeEnv(typeEnv, _prevDeclOrDefn != NULL ? _prevDeclOrDefn->_substs : NULL);
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
         ::insTypeSig(typeSig, &localTypeEnv, &typedVal.schematicTypeVars(), typeCheck.msa());
         typedVal.substs() = localTypeEnv.substs();
      }
   }

   typedVal.typeSig() = typeSig;
}

// Decl::typeCheck: Type check a declaration

Void Decl::typeCheck(TypedVal& typedVal, TypeEnv* typeEnv, TypeSig failTS,
   TypeCheck& typeCheck) {
   if (_prevDeclOrDefn != NULL && _unifyWithPrevDeclOrDefn)
      typeCheck.unifyOrError(_prevDeclOrDefn->typeSig(), typedVal.typeSig(),
                             typeEnv,
                             "Failed to unify declaration with previous"
                             " declaration or definition");
}

/*----------------------------------------------------------------------------*/

// Defn::insTypeSig: Instantiate a type signature for a definition

Void Defn::insTypeSig(TypedVal& typedVal, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   if (!_overridden) {
      typedVal.typeEnv() = typeEnv;
      TypeEnv localTypeEnv(typeEnv, _prevDeclOrDefn != NULL && _unifyWithPrevDeclOrDefn
         ? _prevDeclOrDefn->_substs
         : NULL);
      TypeSig& typeSig = typedVal.typeSig();
      Bool insFlag = TRUE;
      if (typeSig == TypeSig(UNKNOWN)) {
         if (_prevDeclOrDefn != NULL) {
            typeSig = _prevDeclOrDefn->typeSig();
            if (_unifyWithPrevDeclOrDefn) {
               typedVal.substs() = _prevDeclOrDefn->substs();
               typedVal.schematicTypeVars() = _prevDeclOrDefn->schematicTypeVars();
               _unifyWithPrevDeclOrDefn = FALSE;
               insFlag = FALSE;
            }
         }
      } else
         typeCheck.bindTypeConNames(typeSig);
      if (insFlag && hasTypeVar(typeSig)) {
         typeSig = mutableTypeSig(typeSig, typeCheck.msa());
         ::insTypeSig(typeSig, &localTypeEnv, &typedVal.schematicTypeVars(), typeCheck.msa());
         typedVal.substs() = localTypeEnv.substs();
      }
  
      typedVal.typeSig() = typeSig;
      typedVal._needsInstance = hasTypeVar(typeSig);
   }
}

// Defn::typeCheck: Type check a definition and unify with any previous
//                  declaration

Void Defn::typeCheck(TypedVal& typedVal, TypeEnv* typeEnv, TypeSig failTS,
                     TypeCheck& typeCheck) {
   _minConstraintSeqNo = nextConstraintSeqNo(FALSE);
   if (!_overridden) {
      Bool isSchematic = typedVal.schematic();
      typedVal.setSchematic(FALSE);
      typedVal.schematicTypeVars().empty();
      TypeEnv localTypeEnv(typeEnv, typedVal.substs());
      Subst* subst = typedVal.substs();
      while (subst != NULL) {
         localTypeEnv.addTypeSig(subst->typeSig(), typeCheck.msa());
         subst = subst->next();
      }
 
      Expr expectedExpr = _expr;
      while (isPair(expectedExpr) && fst(expectedExpr) == Expr(REDUCE))
         expectedExpr = snd(expectedExpr);
      if (isPtr(expectedExpr) && formOf(expectedExpr) == LAMBDA) {
         Lambda& lambda = toBody(expectedExpr, Lambda);
         if (lambda._typeSig == TypeSig(UNKNOWN))
            lambda._typeSig = typedVal.typeSig();
      }

      TypeSig typeSig = typeCheck.infer(_expr, &localTypeEnv, failTS);

      Subst::substTypeVars(typedVal.typeSig(), *typeCheck.substs());
      typeCheck.unifyOrError(typedVal.typeSig(), typeSig, &localTypeEnv,
                             "Failed to unify definition");

//--- Unify with previous declaration if appropriate

      if (_prevDeclOrDefn != NULL && _unifyWithPrevDeclOrDefn &&
          formOf(_prevDeclOrDefn->val()) == prevDeclForm()) {
         Subst::substTypeVars(_prevDeclOrDefn->typeSig(), *typeCheck.substs());
         Subst::substTypeVars(typeSig, *typeCheck.substs());
         typeCheck.unifyOrError(_prevDeclOrDefn->typeSig(), typeSig,
                                &localTypeEnv,
                                "Failed to unify definition with previous"
                                " declaration or definition");

      }
      Subst::substTypeVars(typedVal.typeSig(), *typeCheck.substs());
      Subst::substSubsts(typedVal.substs(), *typeCheck.substs());
 
      if (isSchematic && hasTypeVar(typedVal.typeSig())) {
         typedVal.setSchematic(TRUE);
         typeCheck.setSchematicTypeVars(typedVal.typeSig(), typedVal.schematicTypeVars(),
                                        typeEnv != NULL ? typeEnv->next() : NULL);
      } else {
         typedVal.setSchematic(TRUE);
         typedVal._needsInstance = FALSE;
      }
   }
   _maxConstraintSeqNo = nextConstraintSeqNo(FALSE);
}

// Reset the schematic type variables 

Void Defn::resetNeedsInstance(TypedVal& typedVal, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   if (typedVal.schematic()) {
      Subst::substTypeVars(typedVal.typeSig(), *typeCheck.substs());
      Subst::substSubsts(typedVal.substs(), *typeCheck.substs());
      typeCheck.resetSchematicTypeVars(typedVal.typeSig(),
                                       typedVal.schematicTypeVars(),
                                       typeEnv);
   }
   typeCheck.fixCoercions(typedVal.typeSig(), typeEnv, _minConstraintSeqNo, _maxConstraintSeqNo);

   if (_outermost)
      typedVal._needsInstance = FALSE;
}

static const ClassDefn* bindingClassDefn(Expr expr) {
   switch (formOf(expr)) {
      case CLASS_METHOD_DECL:
         return &toBody(expr, ClassMethodDecl).classDefn();

      case CLASS_DATA_CON:
         return &toBody(expr, ClassDataCon).classDefn();

      default:
         return NULL;
   }
}

TypeSig NameOcc::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   Name name = _binding.name();

   TypedVal* tv = typedVal();
   if (tv == NULL) {
      error("NameOcc::infer: not bound");
      return (TypeSig)ERROR;
   }

   _typeSig = tv->typeSig();
    
   if (hasTypeVar(_typeSig)) {
      _typeSig = _moduleDefn == NULL
         ? mutableTypeSig(_typeSig, typeCheck.msa())
         : mutableTypeSig(_typeSig,
                          CopyContext(_moduleDefn,
                                      typeCheck.nameTable(),
                                      typeCheck.predicates().varNameMaps(),
                                      typeCheck.predicates().typeVarMaps(),
                                      typeCheck.msa()),
                          typeCheck.msa());

      if (tv->_schematic && !tv->_schematicTypeVars.isEmpty()) {
         TypeEnv localTypeEnv(NULL);
         const ClassDefn* classDefn = bindingClassDefn(tv->val());

         if (_moduleDefn == NULL)
            typeCheck.instantiate(_typeSig, tv->_schematicTypeVars,
                                  classDefn == NULL,
                                  tv->needsInstance() ? tv : NULL,
                                  &_typeInsts,
                                  _moduleDefn, &localTypeEnv);
         else {
            CopyContext cc(_moduleDefn, typeCheck.nameTable(),
                           typeCheck.predicates().varNameMaps(),
                           typeCheck.predicates().typeVarMaps(),
                           typeCheck.msa());
            TypeVarSet schematicTypeVars =
               TypeVarSet(tv->_schematicTypeVars, &cc, typeCheck.msa());
            typeCheck.instantiate(_typeSig, schematicTypeVars,
                                  classDefn == NULL,
                                  tv->needsInstance() ? tv : NULL,
                                  &_typeInsts,
                                  _moduleDefn, &localTypeEnv);
         }
         _substs = localTypeEnv.substs();

         if (classDefn != NULL)
            typeCheck.addConstraint(inferMethod(*classDefn,
                                                TRUE,
                                                &localTypeEnv, typeCheck),
                                    &localTypeEnv, TRUE);
      } else
         Subst::substTypeVars(_typeSig, *typeCheck.substs());
   }

	return _typeSig;
}

/*----------------------------------------------------------------------------*/

// Lambda::infer: Infer type of lambda function

TypeSig Lambda::infer(TypeEnv* typeEnv, TypeCheck& typeCheck,
                      TypeSig failTS, Bool& nonStrictFlag) {
   TypeEnv localTypeEnv(typeEnv);
   TypeSig expectedTS = _typeSig;
   Bool expectedTypeComplete = expectedTS != TypeSig(UNKNOWN);

   UInt i;
   for (i = 0; i < _nFormalParams; i++) {
      TypedVal& typedVal = _formalParamV[i].typedVal();
      TypeSig& typeSig = typedVal.typeSig();
#ifdef TRACE
      TypeSig traceTypeSig = typeSig;
#endif
      typeCheck.bindTypeConNames(typeSig);
      if (hasTypeVar(typeSig)) {
         typeSig = mutableTypeSig(typeSig, typeCheck.msa());       
         ::insTypeSig(typeSig, &localTypeEnv, NULL, typeCheck.msa());
         localTypeEnv.addTypeSig(typeSig, typeCheck.msa());
      }
      if (expectedTypeComplete && isArrowTypeSig(expectedTS)) {
         TypeSig expectedFormalParamTS = arg(fun(expectedTS));
         Subst::substTypeVars(expectedFormalParamTS, *typeCheck.substs());
         if (!typeCheck.unify(typeSig, expectedFormalParamTS, &localTypeEnv)) {
            typeCheck.error("Failed to unify lambda formal parameter");
            return (TypeSig)ERROR;
         }
         expectedTS = arg(expectedTS);
      } else
         expectedTypeComplete = FALSE;
#ifdef TRACE
      if (traceFlag) {
         TypeSig traceInstTypeSig = typeSig;
         Subst::substTypeVars(traceInstTypeSig, *typeCheck.substs());
         outStream << "Lambda formal parameter: ";
         printName(_formalParamV[i].name(), outStream, typeCheck.nameTable());
         outStream << " :: ";
         printTypeSig(traceInstTypeSig, 0, FALSE, outStream, typeCheck.env());
         if (traceTypeSig != TypeSig(UNKNOWN)) {
            outStream << " from ";
            printTypeSig(traceTypeSig, 0, FALSE, outStream, typeCheck.env());
         }
         outStream << '\n';
      }
#endif
   }

   _typeSig = typeCheck.infer(_body, &localTypeEnv,
                              expectedTypeComplete ? expectedTS : failTS);

   if (_nFormalParams > 0)
      for (i = _nFormalParams; i-- > 0;) {
         TypeSig typeSig = _formalParamV[i].typedVal().typeSig();
         Subst::substTypeVars(typeSig, *typeCheck.substs());
         _typeSig = ap2(ARROW,
                        typeSig,
                        _typeSig, typeCheck.msa());
      } 
   else
      _typeSig = ap(ARROW, _typeSig, typeCheck.msa());

   typeCheck.fixCoercions(_typeSig, typeEnv, NULL, TRUE);

   nonStrictFlag = TRUE;
   return _typeSig;
}

/*----------------------------------------------------------------------------*/

TypeSig CurriedLambda::infer(TypeEnv* typeEnv,
                             TypeCheck& typeCheck,
                             TypeSig failTS,
                             Bool& nonStrictFlag) {
   nonStrictFlag = FALSE;
   return _lambda->typeSig();
}

/*----------------------------------------------------------------------------*/

// Let::infer: Infer the type of a 'let' definition

TypeSig Let::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS,
                   Bool& nonStrictFlag) {
   typeCheck.insDeclOrDefnsTypeSigs(_declOrDefns, typeEnv);
   typeCheck.typeCheckDeclOrDefns(_declOrDefns, typeEnv, failTS);
   TypeSig typeSig = typeCheck.infer(_expr, typeEnv, failTS);
   typeCheck.resetDeclOrDefnsNeedsInstance(_declOrDefns, typeEnv);
   nonStrictFlag = TRUE;
   return typeSig;
}

/*----------------------------------------------------------------------------*/

// FnAp::infer: Infer the type of a function application

TypeSig FnAp::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   UInt minConstraintSeqNo = nextConstraintSeqNo();
   TypeSig resTS = fromTypeVar(newTypeVar());
   TypeSig typeSig = resTS;

	Bool castFlag = FALSE;
	Bool typeConversionFlag = FALSE;
	TypeSig castArgTS;

// Handle a number of special cases
// 1. cast as a minor optimisation
// 2. data construction

	if (_nArgs == 1) {
      NameOcc* nameOcc = NULL;
      UInt32 formOfFun = formOf(_fun);
		if (formOfFun == NAME_OCC) {
			nameOcc = &toBody(_fun, NameOcc);
			if (nameOcc->name() == typeCheck.castName())
				castFlag = TRUE;
      } else if (formOfFun == CONSTRUCT ||  
                 isPair(_fun) && formOf(fst(_fun)) == CONSTRUCT ) {
         TypeSig reprTS = typeCheck.infer(_argV[0], typeEnv, failTS);
         Subst::substTypeVars(arg(fun(_typeSig)), *typeCheck.substs());
		   typeCheck.unifyOrError(arg(fun(_typeSig)), reprTS, typeEnv,
                                "Unexpected failure to unify"
                                " constructor or destructor application");
         resTS = arg(_typeSig);
			typeConversionFlag = TRUE;
      }
      else if (formOfFun == CONCRETE_DATA_CON) {
         _typeSig = typeCheck.infer(_fun, typeEnv, failTS);
         TypeSig reprTS = typeCheck.infer(_argV[0], typeEnv, failTS);
         typeCheck.unifyOrError(arg(fun(_typeSig)), reprTS, typeEnv,
                                "Unexpected failure to unify"
" concrete constructor application");
         resTS = arg(_typeSig);
         typeConversionFlag = TRUE;
      }

		if (castFlag) {
			castArgTS = typeCheck.infer(_argV[0], typeEnv, failTS);
         typeCheck.addConstraint(nameOcc->inferCastMethod(castArgTS, resTS, TRUE, typeEnv, typeCheck),
                                 typeEnv,
                                 TRUE);
			_typeSig = nameOcc->_typeSig;
		}
   }

   if (!castFlag && !typeConversionFlag) {
      _typeSig = typeCheck.infer(_fun, typeEnv, failTS);

      for (UInt i = _nArgs; i-- > 0;)
         typeSig = ap2(ARROW,
            (i > 0 || !castFlag ? typeCheck.infer(_argV[i], typeEnv, failTS) : castArgTS),
            typeSig, typeCheck.msa());

      Subst::substTypeVars(_typeSig, *typeCheck.substs());
      Subst::substTypeVars(typeSig, *typeCheck.substs());

      if (!typeCheck.unify(_typeSig, typeSig, typeEnv)) {
         typeCheck.error("Failed to unify function application");
         print(outStream, typeCheck.env());
         outStream << endl;
         return (Expr)ERROR;
      }
   }

   Subst::substTypeVars(resTS, *typeCheck.substs());

   typeCheck.fixCoercions(resTS, typeEnv, minConstraintSeqNo, nextConstraintSeqNo(FALSE));

   nonStrictFlag = TRUE;
   return resTS;
}

/*----------------------------------------------------------------------------*/

// ConstPattern::infer: Infer the type of a constant pattern

TypeSig ConstPattern::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   _typeSig = typeCheck.infer(_k, typeEnv, failTS, TRUE);
	Constraint* constraint = toBody(_eqOcc, NameOcc).inferEqMethod(_typeSig, typeEnv, typeCheck);

   constraint->appendTransforms(new(typeCheck.msa())
      CMConstraintTransform(NULL, *constraint, toBody(_eqOcc, NameOcc)));
   typeCheck.addConstraint(constraint, typeEnv);

   return _typeSig;
}

// VarPattern::insTypeSig: Instantiate a type signature as for a formal parameter

Void VarPattern::insTypeSig(TypeSig& typeSig, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   typeCheck.bindTypeConNames(typeSig);
   if (hasTypeVar(typeSig)) {
      typeSig = mutableTypeSig(typeSig, typeCheck.msa());
      ::insTypeSig(typeSig, typeEnv, NULL, typeCheck.msa());
   }
}

/*----------------------------------------------------------------------------*/

// VarPattern::infer: Infer the type of a variable pattern

TypeSig VarPattern::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   TypedVal& typedVal = _binding.typedVal();
   typedVal.setSchematic(FALSE);
	_binding.insTypeSig(typeEnv, typeCheck);  
   Subst::substTypeVars(typedVal.typeSig(), *typeCheck.substs());

   assert(typeEnv != NULL, "VarPattern::infer: NULL unexpected");
   typeEnv->addTypeSig(typedVal.typeSig(), typeCheck.msa());

   return typedVal.typeSig();
}

/*----------------------------------------------------------------------------*/

// WildCardPattern::infer: Infer the type of a wildcard pattern

TypeSig WildCardPattern::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   if (_typeSig == TypeSig(UNKNOWN)) {
      TypeVar typeVar = newTypeVar();
      _typeSig = fromTypeVar(typeVar);
   } else
      if (hasTypeVar(_typeSig)) {
         typeCheck.bindTypeConNames(_typeSig);
	      _typeSig = mutableTypeSig(_typeSig, typeCheck.msa());
	      insTypeSig(_typeSig, typeEnv, NULL, typeCheck.msa());
         Subst::substTypeVars(_typeSig, *typeCheck.substs());
      }
   return _typeSig;
}

/*----------------------------------------------------------------------------*/

Void DataConPattern::instantiate(TypeEnv* typeEnv, TypeCheck& typeCheck) {
   TypeSig typeSig;
   if (_typedVal != NULL) {
      Bool isPrimitive = _dataCon == dataCon$Ptr;

//--- All data constructors have type Exp a ,so obtain a

      typeSig = arg(_typedVal->typeSig());
      Subst::substTypeVars(typeSig, _typedVal->_substs);
      
      if (!isPrimitive)
         _reprTypeSig = toBody(_dataCon, DataCon).reprTypeSig();

      if (hasTypeVar(typeSig)) {
         TypeEnv localTypeEnv;
         if (!isPrimitive) {
            _reprTypeSig = mutableTypeSig(_reprTypeSig, typeCheck.msa());
            Subst::substTypeVars(_reprTypeSig, _typedVal->_substs);
         }
         if (_moduleDefn == NULL)
            typeSig = mutableTypeSig(typeSig, typeCheck.msa());
         else {
            CopyContext cc(_moduleDefn, typeCheck.nameTable(),
                           typeCheck.predicates().varNameMaps(),
                           typeCheck.predicates().typeVarMaps(),
                           typeCheck.msa());
            typeSig = mutableTypeSig(typeSig, cc, typeCheck.msa());
             if (!isPrimitive)
               mapTypeSig(_reprTypeSig, cc, typeCheck.msa());
         }

         if (_typedVal->_schematic &&
             !_typedVal->_schematicTypeVars.isEmpty()) {
            TypeEnv localTypeEnv(NULL);
            if (_moduleDefn == NULL)
               typeCheck.insOccTypeSig(typeSig,
                                       _typedVal->_schematicTypeVars,
                                       &localTypeEnv, NULL);
            else {
               CopyContext cc(_moduleDefn, typeCheck.nameTable(),
                              typeCheck.predicates().varNameMaps(),
                              typeCheck.predicates().typeVarMaps(),
                              typeCheck.msa());
               TypeVarSet schematicTypeVars =
                  TypeVarSet(_typedVal->_schematicTypeVars,
                             &cc,
                             typeCheck.msa());
               typeCheck.insOccTypeSig(typeSig,
                                       schematicTypeVars,
                                       &localTypeEnv, NULL);
            }
            if (!isPrimitive)
               insTypeSig(_reprTypeSig, &localTypeEnv, NULL, typeCheck.msa());

//            _typedVal->addInstance(typeSig, NULL, &localTypeEnv, typeCheck);

            _substs = localTypeEnv.substs();
         }
      }
   }
   else
      typeSig = typeCheck.inferTuple(tupleCard(_dataCon),
                                     _dataCon);
   
   if (_typeSig != TypeSig(UNKNOWN)) {
      if (hasTypeVar(_typeSig)) {
         typeCheck.bindTypeConNames(_typeSig);

         _typeSig = mutableTypeSig(_typeSig, typeCheck.msa());

         insTypeSig(_typeSig, typeEnv, NULL, typeCheck.msa());
         Subst::substTypeVars(_typeSig, *typeCheck.substs());
      }
      typeCheck.unifyOrError(typeSig, _typeSig, typeEnv,
                             "Failed to unify data constructor pattern");
   }
   else
      _typeSig = typeSig;
}

// DataConPattern::infer: Infer the type of a data constructor pattern

TypeSig DataConPattern::infer(TypeEnv* typeEnv, TypeCheck& typeCheck,
   TypeSig failTS, Bool& nonStrictFlag) {
   instantiate(typeEnv, typeCheck);

   UInt i = 0;
   Expr pats = _patterns;
   while (pats != Nil) {
      TypeSig patTS = typeCheck.infer(hd(pats), typeEnv, failTS);
      Subst::substTypeVars(_typeSig, *typeCheck.substs());
      TypeSig argTS = argTypeSig(_typeSig, ++i);
      if (_isPtrDecon)
         argTS = ptrTypeSig(argTS, typeCheck.msa());
      if (_isPlainDecon)
         argTS = plainTypeSig(argTS, typeCheck.msa());
      if (!typeCheck.unify(patTS, argTS, typeEnv)) {
         typeCheck.error("Failed to unify data constructor component pattern");
         return (TypeSig)ERROR;
      }
      pats = tl(pats);
   }
   Subst::substTypeVars(_typeSig, *typeCheck.substs());
   TypeSig typeSig = i == 0 ? _typeSig : resultTypeSig(_typeSig, i);
   TypeSig res = typeSig;
   if (_isPtrDecon)
      res = ptrTypeSig(res, typeCheck.msa());
   else if (_isPlainDecon)
      res = plainTypeSig(res, typeCheck.msa());

   return res;
}

/*----------------------------------------------------------------------------*/

TypeSig ConcretePattern::infer(TypeEnv* typeEnv, TypeCheck& typeCheck,
                               TypeSig failTS, Bool& nonStrictFlag) {
   instantiate(typeEnv, typeCheck);
 
   TypeSig reprTypeSig = _reprTypeSig;

   TypeSig patTS = typeCheck.infer(hd(_patterns), typeEnv, failTS);

   if (_isPtrDecon)
      reprTypeSig = ptrTypeSig(reprTypeSig, typeCheck.msa());

   if (_isPlainDecon)
      reprTypeSig = plainTypeSig(reprTypeSig, typeCheck.msa());

   if (!typeCheck.unify(patTS, reprTypeSig, typeEnv)) {
      typeCheck.error("Failed to unify concrete pattern");
      return (TypeSig)ERROR;
   }

   Subst::substTypeVars(_typeSig, *typeCheck.substs());
   UInt arity = dataConArity(_dataCon);
   TypeSig typeSig = arity == 0 ? _typeSig : resultTypeSig(_typeSig, arity);
   TypeSig res = typeSig;
   if (_isPtrDecon)
      res = ptrTypeSig(res, typeCheck.msa());
   if (_isPlainDecon)
      res = plainTypeSig(res, typeCheck.msa());
   return res;
}

/*----------------------------------------------------------------------------*/

// AppLambda::infer: Infer the type of a lambda abstraction (fully applied)

TypeSig AppLambda::infer(TypeSig argTS, TypeEnv* typeEnv, TypeCheck& typeCheck,
                         TypeSig failTS) {
    TypeEnv localTypeEnv(typeEnv);
	_patTypeSig = typeCheck.infer(hd(_patterns), &localTypeEnv, failTS);
   Subst::substTypeVars(argTS, *typeCheck.substs());
/*
   Constraint* constraint = new(typeCheck.msa())
      CoercionConstraint(argTS, _patTypeSig,
         toBody(_castOcc, NameOcc).inferCastMethod(argTS, _patTypeSig, FALSE, &localTypeEnv, typeCheck));

   constraint->appendTransforms(new(typeCheck.msa())
      CoercionConstraintTransform(NULL, *constraint, *this));
   typeCheck.addConstraint(constraint, typeEnv, TRUE);
*/
   _typeSig = typeCheck.infer(_body,&localTypeEnv, failTS);
   return _typeSig;
}

/*----------------------------------------------------------------------------*/

// FatBarSeq::infer: Infer the type of a sequence of FAT_BAR expressions

TypeSig FatBarSeq::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   assert(length(_args) == 1, "FatBarSeq::infer: unique argument expected");
   Expr arg = hd(_args);
   assert(formOf(arg) == NAME_OCC, "FatBarSeq::infer: expected name occurence");
   TypedVal& typedVal = *toBody(arg, NameOcc).typedVal();
   typedVal.setSchematic(FALSE);

   TypeSig argTS = typeCheck.infer(arg, typeEnv, failTS);

   TypeSig altsTS = UNKNOWN;
   Expr alts = _alts;
   while (alts != Nil) {
      AppLambda& appLambda = AppLambda::fromExpr(hd(alts));
      TypeSig altTS = appLambda.infer(argTS, typeEnv, typeCheck, failTS);
      if (altsTS == TypeSig(UNKNOWN))
         altsTS = altTS;
      else {
         Subst::substTypeVars(altsTS, *typeCheck.substs());
         if (!typeCheck.unify(altTS, altsTS, typeEnv)) {
            typeCheck.error("Failed to unify alternative type");
            return (TypeSig)ERROR;
         }
      }
      alts = tl(alts);
   }

   alts = _alts;

/* Temporarily commented out until support for Any is fully implemented

   Subst::substTypeVars(argTypeSig, *typeCheck.substs());
   Expr xs = Nil;
      while (alts != Nil) {
         TypeSig patTypeSig = AppLambda::fromExpr(hd(alts)).patTypeSig();
         Subst::substTypeVars(patTypeSig, *typeCheck.substs());
         xs = cons(patTypeSig, xs, typeCheck.msa());
         alts = tl(alts);
      }
      typeCheck.addConstraints(new(typeCheck.msa()) CaseConstraint(argTypeSig, xs), typeEnv.typeVars(), TRUE);
 */

   while (alts != Nil) {
      AppLambda& appLambda = AppLambda::fromExpr(hd(alts));
      TypeSig patTypeSig = appLambda.patTypeSig();
      Subst::substTypeVars(argTS, *typeCheck.substs());
      Subst::substTypeVars(patTypeSig, *typeCheck.substs());
      if (!typeCheck.unify(argTS, patTypeSig, typeEnv)) {
         typeCheck.error("Failed to unify case argument with pattern");
         return (TypeSig)ERROR;
      }
      Subst::substTypeVars(patTypeSig, *typeCheck.substs());
      if (_patTypeSig == TypeSig(UNKNOWN))
         _patTypeSig = patTypeSig;
      else {
         Subst::substTypeVars(_patTypeSig, *typeCheck.substs());
         if (!typeCheck.unify(_patTypeSig, patTypeSig, typeEnv)) {
            typeCheck.error("Failed to unify alternative pattern type");
            return (TypeSig)ERROR;
         }
      }

      alts = tl(alts);
   }

   Subst::substTypeVars(_patTypeSig, *typeCheck.substs());
   Subst::substTypeVars(altsTS, *typeCheck.substs());
   return altsTS;
}

/*----------------------------------------------------------------------------*/

// Coerced::infer: Infer the type of a coerced term

TypeSig Coerced::infer(TypeEnv* typeEnv, TypeCheck& typeCheck, TypeSig failTS, Bool& nonStrictFlag) {
   TypeSig typeSig = typeCheck.infer(_expr, typeEnv, failTS);
   TypeSig res = fromTypeVar(::newTypeVar());

   Constraint* constraint = new(typeCheck.msa())
      CoercionConstraint(typeSig, res,
         toBody(_castOcc, NameOcc).inferCastMethod(typeSig, res, FALSE, typeEnv, typeCheck));
   constraint->appendTransforms(new(typeCheck.msa())
      CoercedConstraintTransform(NULL, *constraint, *this));
	typeCheck.addConstraint(constraint, typeEnv, TRUE);
	return res;
}

/*----------------------------------------------------------------------------*/

// ModuleDefn::typeCheck: Check module types

Void ModuleDefn::typeCheck(TypeCheck& typeCheck) {
   typeCheck._moduleDefn = this;
   {
      TypeEnv typeEnv(NULL);
      mapProc2(insTypeSig, _declOrDefns, &typeEnv, typeCheck);
      mapProc2(typeCheckDeclOrDefn, _declOrDefns, &typeEnv, typeCheck);
      typeCheck.removeSchematicConstraints();
      mapProc2(resetNeedsInstance, _declOrDefns, NULL, typeCheck);
   }

   _substs =      typeCheck.predicates()._substs;
   _constraints = typeCheck.predicates()._unresolvedList;
   _typeVarMaps = typeCheck.predicates().typeVarMaps();
/*
   ConstraintSet* constraints = _constraints;
   while (constraints != NULL) {
      constraints->print(outStream, typeCheck.env());
      constraints = constraints->_next;
   }
*/
#ifdef TRACE
   OptionsParser optionsParser(typeCheck._options);
   if (optionsParser.hasKey("printAST")) {
      printTranExpr(_declOrDefns, FALSE, outStream, typeCheck.env(), TRUE, 0);
      outStream << "\n--------------------------------------------" << endl;
   }
#endif

}

// ModuleDefn::insTypeSig: Instantiate type signature for declaration or definition

Void ModuleDefn::insTypeSig(Expr declOrDefn, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   switch (formOf(declOrDefn)) {
		case TYPE_DEFN:
         toBody(declOrDefn, TypeDefn).insTypeSigs(typeCheck);
         break;

		case DATA_CON_DECL:
         toBody(declOrDefn, DataConDecl).insTypeSig(NULL, typeCheck);
         break;
      
      case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).insTypeSigs(NULL, typeCheck);
         break;

      case CLASS_DEFN:
         //traceFlag = !traceFlag;
         toBody(declOrDefn, ClassDefn).insTypeSigs(typeCheck);
         //traceFlag = !traceFlag;
         break;

      case INSTANCE_DEFN:
         //traceFlag = !traceFlag;
         toBody(declOrDefn, InstanceDefn).insTypeSigs(typeCheck);
         //traceFlag = !traceFlag;
         break;

		default: {
			typeCheck.insDeclOrDefnTypeSig(declOrDefn, typeEnv);
         break;
		}
   }
}

// ModuleDefn::typeCheckDeclOrDefn: Check module declaration or definition

Void ModuleDefn::typeCheckDeclOrDefn(Expr declOrDefn, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   switch (formOf(declOrDefn)) {
		case TYPE_DEFN:
         toBody(declOrDefn, TypeDefn).typeCheck(typeCheck);
         break;
      
		case DATA_CON_DECL:
         break;

		case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).typeCheck(typeCheck);
         break;

      case CLASS_DEFN:
         //traceFlag = !traceFlag;
         toBody(declOrDefn, ClassDefn).typeCheck(typeCheck);
         //traceFlag = !traceFlag;
         break;

      case INSTANCE_DEFN:
         toBody(declOrDefn, InstanceDefn).typeCheck(typeCheck);
         break;

		default: {
			typeCheck.typeCheckDeclOrDefn(declOrDefn, typeEnv, ERROR);
         break;
		}
   }
}


// ModuleDefn::typeCheck: Check module declaration or definition

Void ModuleDefn::resetNeedsInstance(Expr declOrDefn, TypeEnv* typeEnv, TypeCheck& typeCheck) {
   switch (formOf(declOrDefn)) {
      case TYPE_DEFN:
         toBody(declOrDefn, TypeDefn).resetNeedsInstance(typeCheck);
         break;

      case DATA_CON_DECL:
         break;

      case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).resetNeedsInstance(typeCheck);
         break;

      case CLASS_DEFN:
         toBody(declOrDefn, ClassDefn).resetNeedsInstance(typeCheck);
         break;

      case INSTANCE_DEFN:
         toBody(declOrDefn, InstanceDefn).resetNeedsInstance(typeCheck, typeEnv);
         break;

      default: {
         typeCheck.resetDeclOrDefnNeedsInstance(declOrDefn, typeEnv);
         break;
      }
   }
}

// ModuleDefn::lookUpTypeCon: Lookup module type constructor for given name

Bool ModuleDefn::lookUpTypeCon(Name name, const Env& env, UInt hashVal, TypeSig& typeSig) const {
   assert(!isBuiltInName(name), "ModuleDefn::lookUpTypeCon: Unexpected built-in name");
   if (!isBuiltInName(name)) {
      const char* str = nameString_(name, env.nameTable());
      if (str == NULL || (name = _env->nameTable().lookUp(str)) == NULL_NAME)
         return FALSE;
      name += builtInNameCount;
   }
   return _typeConNameMap->lookUp(name, hashVal, typeSig);
}

/*----------------------------------------------------------------------------*/

// Order::typeCheck: Type check an 'Order' script

Void Order::typeCheck(TypeCheck& typeCheck) {
   UInt minConstraintSeqNo = nextConstraintSeqNo();
    TypeSig typeSig = typeCheck.infer(_sequence, NULL, ERROR, TRUE);
  	typeCheck.unifyOrError(typeSig, ap(ARROW, typeCon(Void), typeCheck.msa()),
                          NULL,
                          "Order not Void type");
   typeCheck.removeSchematicConstraints();
   typeCheck.fixCoercions(typeSig, NULL, minConstraintSeqNo, nextConstraintSeqNo(FALSE));

/*
   ConstraintSet* constraintSet = typeCheck._predicates._unresolvedList;
   if (constraintSet != NULL) {
      typeCheck.error("Unresolved constraints");
      typeCheck.printConstraints(outStream, TRUE);
   }
*/

   _substs = typeCheck.predicates()._substs;
   _typeVarMaps = typeCheck.predicates().typeVarMaps();

#ifdef TRACE
   OptionsParser optionsParser(typeCheck._options);
   if (optionsParser.hasKey("printAST")) {
      printTranExpr(_sequence, FALSE, outStream, typeCheck.env(), TRUE, 0);
      outStream << "\n--------------------------------------------" << endl;
   }
#endif

}

/*----------------------------------------------------------------------------*/

// Snippet::typeCheck: Type check snippet

Void Snippet::typeCheck(TypeCheck& typeCheck) {
   UInt minConstraintSeqNo = nextConstraintSeqNo();
   _typeSig = typeCheck.infer(_expr, NULL, ERROR, TRUE);
   typeCheck.fixCoercions(_typeSig, NULL,
                          minConstraintSeqNo, nextConstraintSeqNo(FALSE));

   _substs = typeCheck.predicates()._substs;
   _typeVarMaps = typeCheck.predicates().typeVarMaps();
}

/*----------------------------------------------------------------------------*/

// TypeCheck::TypeCheck: Constructor

TypeCheck::TypeCheck(ConstString options, Env& env,
                     ModuleDefn* moduleDefn/* = NULL*/)
   : _options(options), _nErrors(0),
     _env(env), _moduleDefn(moduleDefn),
     _deferredUnifications(NULL) {

   _castName = useName("cast");
   _freeTypeEquationList = NULL;

#ifdef TRACE
   OptionsParser optionsParser(options);
   optionsParser.getNum("typeVarProbe",      typeVarProbe);
   optionsParser.getNum("inferProbe",        inferProbeSeqNo);
   optionsParser.getNum("unifyProbe",        unifyProbeSeqNo);
   optionsParser.getNum("constraintProbe",   constraintProbeSeqNo);
   traceFlag = optionsParser.hasKey("trace");
   if (traceFlag)
      traceClassCompilation(TRUE);
#endif

   _predicates._substs = new (msa()) SubstSet();

   _freeConstraintElements = NULL;
}

// Look up type constructor for given name

TypeSig TypeCheck::typeConSig(Name name) {
   TypeSig typeSig;
   UInt hashVal = nameHashVal(name, env().nameTable());

   if (Module::typeConSig(name, _env.nameTable(), hashVal, typeSig))
      return typeSig;

   if (_moduleDefn != NULL && _moduleDefn->typeConNameMap()->lookUp(name, hashVal, typeSig))
      return typeSig;

   _nErrors++;
   ivoryError1("Undefined type constructor: %s", nameString_(name, env().nameTable()));
   return (TypeSig)ERROR;
}

// TypeCheck::bindTypeConNames: Bind type constructor names in a type signature
// The reason for this is to ensure type constructor comparison by address.

Void TypeCheck::bindTypeConNames(TypeSig& typeSig) {
   if (isName(typeSig)) {
      Name name = toName(typeSig);
      if (name >= 0) {
         ConstString nameStr = nameString_(name, env().nameTable());
         if (strlen(nameStr) > 0 && isupper(nameStr[0]))
            typeSig = typeConSig(name);
      }
   }
 else if (isAp(typeSig)) {
    bindTypeConNames(fun(typeSig));
    bindTypeConNames(arg(typeSig));
   }
}

// TypeCheck::bindTypeConNamesInList: Bind type constructor names
// in a list of type signatures

Void TypeCheck::bindTypeConNamesInList(Expr typeSigs) {
   while (typeSigs != Nil) {
      bindTypeConNames(hd(typeSigs));
      typeSigs = tl(typeSigs);
   }
}

// TypeCheck::bindTypeConNamesInLists: Bind type constructor names
// in a list of type signature lists

Void TypeCheck::bindTypeConNamesInLists(Expr typeSigLists) {
   while (typeSigLists != Nil) {
      bindTypeConNamesInList(hd(typeSigLists));
      typeSigLists = tl(typeSigLists);
   }
}

// TypeCheck::useName: Look up or create a name

Name TypeCheck::useName(ConstString nameString) const {
   return ::useName(nameString, nameTable());
}

// TypeCheck::newTypeVar: Add new type variable and add to set

TypeVar TypeCheck::newTypeVar(TypeVarSet* typeVars) const {
   TypeVar typeVar = ::newTypeVar();
   if (typeVars != NULL)
      typeVars->addElement(typeVar, msa());
   return typeVar;
}

// TypeCheck::typeSignature: Map type to type signature

TypeSig TypeCheck::typeSignature(Type type) const {
   return ::typeSignature(type, typeTable());
}

// TypeCheck::reducedTypeSig: The type resulting from reducing an expression


TypeSig TypeCheck::reducedTypeSig(TypeSig typeSig, Bool evalFlag, TypeEnv* typeEnv) {
   Subst::substTypeVars(typeSig, *substs());
   if (!evalFlag) {
      if (isExprTypeSig(typeSig) == TYPE_PRED_TRUE)
         return denotedTypeSig(typeSig);
      else {
         TypeVar typeVar = ::newTypeVar();
         unifyOrError(typeSig, expTypeSig(fromTypeVar(typeVar), msa()), typeEnv,
                      "Unexpected failure to unify denoted type signature");
         return fromTypeVar(typeVar);
      }
   }
   else {
      typeSig = ::evalTypeSig(typeSig);
      if (!isTypeVar(typeSig))
         return typeSig;
   
      TypeSig res = fromTypeVar(::newTypeVar());
      addConstraint(new(msa()) ReductionConstraint(typeSig, res, evalFlag), typeEnv, TRUE);
      return res;
   }
}

// typeVarOccursInSchematicInstance: Predicate to test if a type variable occurs 
// in any schematic instance
// T.B.D. Computationally expensive, so introduce appropriate maps

static Bool typeVarOccursInSchematicInstance(TypeVar typeVar, ConstraintSet* constraintSet){
   while (constraintSet != NULL) {
      if (constraintSet->nSchematic() > 0) {
         ConstraintElement* element = constraintSet->elements();
         while (element != NULL) {
            if (element->constraint()->kind() == Constraint::SCHEME_CONSTRAINT) {
               SchemeConstraint& constraint = (SchemeConstraint&)*element->constraint();
               SchematicInstanceConstraint* instance = constraint.instances();
               while (instance != NULL) {
                  if (typeVarOccursInTypeSig(typeVar, instance->typeSig()))
                     return TRUE;
                  instance = instance->nextInstance();
               }
            }
            element = element->next();
         }
      }
      constraintSet = constraintSet->next();
   }
   return FALSE;
}

// TypeCheck::fixCoercions: Fixes any local coercions with type variables that are not:
//
// 1. Included in a given type signature (if provided)
//
// 2. Included in the parent type environment
//

Void TypeCheck::fixCoercions(TypeSig typeSig, TypeEnv* typeEnv,
                             UInt minSeqNo, UInt maxSeqNo) {
   ConstraintSet** constraintSetPtr = &_predicates._unresolvedList;
   while (*constraintSetPtr != NULL) {
      Bool fix = TRUE;
      ConstraintSet& constraintSet = **constraintSetPtr;
      if (constraintSet.nCoercion() > 0 &&
          constraintSet.seqNoInRange(minSeqNo, maxSeqNo)) {
         const TypeVarElement* element = constraintSet.typeVarSet().elements();
         while (element != NULL) {
            TypeVar typeVar = element->typeVar();
            if (typeVarOccursInTypeSig(typeVar, typeSig) ||
                (typeEnv != NULL && typeEnv->typeVarOccurs(typeVar))) {
               fix = FALSE;
               break;
            }
            element = element->next();
         }
         if (fix) {
            const TypeVarElement* element = constraintSet.typeVarSet().elements();
            while (element != NULL) {
               TypeVar typeVar = element->typeVar();
               if (typeVarOccursInSchematicInstance(typeVar, _predicates._unresolvedList)) {
                  fix = FALSE;
                  break;
               }
               element = element->next();
            }
         }
      }
      else
         fix = FALSE;

      if (fix) {

#ifdef TRACE
         if (traceFlag) {
            outStream << "Fixing coercions for: ";
            printTypeSig(typeSig, 0, FALSE, outStream, env());
            outStream << '\n';
            if (typeEnv != NULL && typeEnv->next() != NULL)
               typeEnv->next()->print(outStream, env());
            constraintSet.print(outStream, env());
         }
#endif

         * constraintSetPtr = constraintSet.next();
         constraintSet._next = _predicates._workList;
         _predicates._workList = &constraintSet;
         checkConstraintsWorkList(TRUE, typeEnv);
      }
      else
         constraintSetPtr = &constraintSet._next;
   }
}

// TypeCheck::unifyOrError: Unify two type expressions
//                          Error on failure

Void TypeCheck::unifyOrError(TypeSig ts1, TypeSig ts2, TypeEnv* typeEnv,
                             const char* errStr) {
   if (!unify(ts1, ts2, typeEnv))
      error(errStr);
}

// TypeCheck::unify: Unify two type expressions

Bool TypeCheck::unify(TypeSig ts1, TypeSig ts2,
                      TypeEnv* typeEnv,
                      Subst** const substs/* = NULL*/, 
                      Bool instanceOfScheme/* = FALSE*/,
                      Bool forInstance/* = FALSE*/) {
   Bool fixed = substs == NULL;  // a fixed solution unless substitutions are provided

#ifdef TRACE
static UInt unifyProbe = 0;
   if ((Int)(++unifyProbe) == unifyProbeSeqNo) {
		outStream << "***** Type check unify probe hit\n";
      traceFlag = TRUE;
   }

#endif

	Subst* extSubsts = NULL;		// Extended substitutions
	Subst* extSubstsTail = NULL;	// Tail of list

#ifdef TRACE_UNIFICATION
#ifdef TRACE_UNIFICATION_IN_DETAIL
	if (traceFlag) {
#else
   if (traceFlag && (fixed)) {
#endif
      outStream << "TypeCheck::unify(" << unifyProbe << "): ";
      printTypeSig(ts1, 0, FALSE, outStream, env());
      outStream << "\n                  ";
      printTypeSig(ts2, 0, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   TypeEquation* typeEquationList;
	if (_freeTypeEquationList != NULL) {
		typeEquationList = _freeTypeEquationList;
		_freeTypeEquationList = typeEquationList->_next;
		typeEquationList->_next = NULL;
		typeEquationList->_x = ts1;
		typeEquationList->_y = ts2;
	} else
		typeEquationList = new(msa()) TypeEquation(ts1, ts2, NULL);

   while (typeEquationList != NULL) {
		TypeEquation* typeEquation = typeEquationList;
      TypeSig x = typeEquation->_x;
      TypeSig y = typeEquation->_y;

		Bool reused = FALSE;
      Bool fail = FALSE;
repeatLab:

#ifdef TRACE
#ifdef TRACE_UNIFICATION_IN_DETAIL
    if (traceFlag) {
         outStream << "Trying: ";
         printTypeSig(x, 0, FALSE, outStream, env());
         outStream << "\n        ";
         printTypeSig(y, 0, FALSE, outStream, env());
         outStream << endl;
		} 
#endif
#endif

      if (x != y) {
			if (!(fixed)) {
				Bool repeatFlag = FALSE;
				Subst* subst;

  				if (isTypeVar(x) && ((subst = Subst::lookUp(toTypeVar(x), extSubsts)) != NULL ||
					                  (subst = Subst::lookUp(toTypeVar(x), *substs)) != NULL)) {
					x = subst->typeSig();
					repeatFlag = TRUE;
				}
    			if (isTypeVar(y) && ((subst = Subst::lookUp(toTypeVar(y), extSubsts)) != NULL ||
   			                   (subst = Subst::lookUp(toTypeVar(y), *substs)) != NULL)) {
					y = subst->typeSig();
					repeatFlag = TRUE;
				}
				if (repeatFlag)
					goto repeatLab;
			}
			Bool typeVarFlag = FALSE;
         Bool addSubstFlag = TRUE;
         Bool q = instanceOfScheme && !forInstance;
         if (isTypeVar(x)) {
            typeVarFlag = TRUE;
            if (q)
               addSubstFlag = FALSE;
         }
			if (isTypeVar(y)) {
             if (!typeVarFlag) {
				   TypeSig t = x; x = y; y = t;
               if (!q)
				      typeVarFlag = TRUE;
            }
			}

			if (typeVarFlag) {
            if (addSubstFlag) {
               TypeVar typeVar = toTypeVar(x);
               TypeSig typeSig = y;

               if (fixed) {
                  if (typeVarOccursInTypeSig(typeVar, typeSig)) {
                     outStream << "************** Occurs check failed: ";
                     printName(typeVar, outStream, nameTable());
                     outStream << " in ";
                     printTypeSig(y, 0, FALSE, outStream, env());
                     outStream << '\n';
                     Subst::printSubsts(*substs, outStream, env());
                     Subst::printSubsts(extSubsts, outStream, env());
                     outStream.flush();
                     _nErrors++;
                     return FALSE;
                  }
               }
               else
                  if (typeVarOccursInTypeSig(typeVar, typeSig, *substs, extSubsts))
                     return FALSE;

               if (fixed) {

                  //------------ Substitute in constraint stack

                  TypeEquation* typeEquation = typeEquationList->_next;
                  while (typeEquation != NULL) {
                     typeEquation->subst(typeVar, typeSig);
                     typeEquation = typeEquation->_next;
                  }

                  if (substs != NULL) {

 //----------------- Substitute existing substitutions

                     Subst* subst = *substs;
                     while (subst != NULL) {
                        subst->subst(typeVar, typeSig);
                        subst = subst->_next;
                     }
                  }

//------------ Substitute extension substitutions

                  Subst* subst = extSubsts;
                  while (subst != NULL) {
                     subst->subst(typeVar, typeSig);
                     subst = subst->_next;
                  }
               }
				   extSubsts = new(msa()) Subst(typeVar, typeSig, extSubsts);
				   if (extSubstsTail == NULL)
					   extSubstsTail = extSubsts;
            }
			} else {
				if (isAp(x) && isAp(y)) {
					if (fun(x) != fun(y)) {
						TypeEquation* newTypeEquation;
						if (_freeTypeEquationList != NULL) {
							newTypeEquation = _freeTypeEquationList;
							_freeTypeEquationList = newTypeEquation->_next;
							newTypeEquation->_x = fun(x);
							newTypeEquation->_y = fun(y);
						} else
							newTypeEquation = new(msa()) TypeEquation(fun(x), fun(y), NULL);
						newTypeEquation->_next = typeEquation->_next;
						typeEquation->_next = newTypeEquation;
					}
					typeEquation->_x = arg(x);
					typeEquation->_y = arg(y);
					reused = TRUE;
				} else
					fail = TRUE;
			}
		}
		if (!reused) {
			typeEquationList = typeEquation->_next;
			typeEquation->_next = _freeTypeEquationList;
			_freeTypeEquationList = typeEquation;
		}
		if (fail) {
			if (fixed) {
				outStream << "*** Failed to unify ";
				printTypeSig(x, 0, FALSE, outStream, env());
				outStream << " with ";
				printTypeSig(y, 0, FALSE, outStream, env());
				outStream << endl;
				_nErrors++;
			}
         return FALSE;
      }
   }

	if (fixed) {
      if (extSubsts != NULL)
         applySubsts(extSubsts, typeEnv);
   } else {

//--- Extend substitutions

	   if (extSubstsTail != NULL) {

#ifdef TRACE
#ifdef TRACE_UNIFICATION_IN_DETAIL
         if (traceFlag) {
            outStream << "Extended Trial ";
			   Subst::printSubsts(extSubsts, outStream, env());
         }
#endif
#endif

		   extSubstsTail->_next = *substs;
		   *substs = extSubsts;
	   }
   }

   return TRUE;
}

// TypeCheck::unifyTypeLists: Unify two lists of type signatures

Bool TypeCheck::unifyTypeLists(Expr typeList1, Expr typeList2,
                               TypeEnv* typeEnv,
										 Subst** const substs/* = NULL*/) {
	while (typeList1 != Nil && typeList2 != Nil) {
		if (!unify(hd(typeList1), hd(typeList2), typeEnv, substs))
			return FALSE;
		typeList1 = tl(typeList1), typeList2 = tl(typeList2);
	}
	if (typeList1 != Nil || typeList2 != Nil) {
		error("TypeCheck::unifyTypeLists: Type list mismatch");
		return FALSE;
	}
	return TRUE;
}

// Helper function to prepend one substitution list to another
Subst* prependSubsts(Subst* source, Subst* dest) {
   if (source == NULL) return dest;

   Subst* head = source;
   Subst* tail = source;
   while (tail->next() != NULL) {
      tail = tail->next();
   }
   tail->next() = dest;
   return head;
}

// Helper to find a substitution for a given type variable
TypeSig* findSubstFor(TypeVar var, Subst* substs) {
   for (Subst* s = substs; s != NULL; s = s->next()) {
      if (s->typeVar() == var) {
         return &(s->typeSig());
      }
   }
   return NULL;
}


Void TypeCheck::genInstanceSubsts(TypeSig ts1, TypeSig ts2, TypeEnv* typeEnv, Subst*& substs) {
   if (isAp(ts1)) {
      if (isAp(ts2)) {
         genInstanceSubsts(fun(ts1), fun(ts2), typeEnv, substs);
         genInstanceSubsts(arg(ts1), arg(ts2), typeEnv, substs);
      }
      else {
         TypeEnv localTypeEnv;
         TypeSig typeSig = mutableTypeSig(ts1, msa());
         ::insTypeSig(typeSig, &localTypeEnv, NULL, msa());
         Subst* insSubsts = localTypeEnv.substs();
         while (insSubsts != NULL) {
            Subst* next = insSubsts->_next;
            insSubsts->_next = substs;
            substs = insSubsts;
            insSubsts = next;
         }
         unifyOrError(typeSig, ts2, typeEnv,
            "Unexpected failure to unify instance type signature");
      }
   }
   else {
      if (isTypeVar(ts1)) {
         TypeVar typeVar = toTypeVar(ts1);
         if (!Subst::typeVarInSubsts(typeVar, substs))
            substs = new (msa()) Subst(typeVar, ts2, substs);
      }
      else
         if (ts1 != ts2)
            unifyOrError(ts1, ts2, typeEnv,
               "Unexpected failure to unify instance type signature");
   }
}

Subst* TypeCheck::instanceSubsts(Subst* substs1,
                                 Subst* substs2,
                                 TypeEnv* typeEnv) {
                                 Subst* res = NULL;

   for (Subst* s1 = substs1; s1 != NULL; s1 = s1->_next) {
      TypeVar v = s1->typeVar();
      Subst* s2 = Subst::lookUp(v, substs2);
      if (s2 != NULL) {
         Subst::substTypeVars(s1->_typeSig, *substs());
         Subst::substTypeVars(s2->_typeSig, *substs());

         genInstanceSubsts(s1->typeSig(), s2->typeSig(), typeEnv, res);
      }
   }

   return res;
}


// TypeCheck::deferUnification: Defer unification 

Void TypeCheck::deferUnification(TypeSig ts1, TypeSig ts2) {
   _deferredUnifications = new (msa()) DeferredUnification(_deferredUnifications, ts1, ts2);
}

// TypeCheck::unifyDeferred: Unify deferred list

Void TypeCheck::unifyDeferred(TypeEnv* typeEnv) {

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
   if (traceFlag)
      outStream << "Unifying deferred list\n";
#endif
#endif

     do {
      DeferredUnification* deferredUnification = _deferredUnifications;
      _deferredUnifications = deferredUnification->next();
      TypeSig ts1 = deferredUnification->ts1();
      TypeSig ts2 = deferredUnification->ts2();
      Subst::substTypeVars(ts1, *substs());
      Subst::substTypeVars(ts2, *substs());
      unifyOrError(ts1, ts2, typeEnv,
                   "Failed to unify schematic instance");
     } while (_deferredUnifications != NULL);
}

// TypeCheck::isNonSchematic: Predicate to check if type variable is non-schematic

Bool TypeCheck::isNonSchematic(TypeVar typeVar, const TypeEnv& typeEnv) const {
   return typeEnv.typeVarOccurs(typeVar);
}

// TypeCheck::setSchematicVars: Set schematic type variables in type signature

Void TypeCheck::setSchematicTypeVars(TypeSig typeSig, TypeVarSet& schematicTypeVars,
                                     const TypeEnv* typeEnv) const {
   if (isTypeVar(typeSig)) {
      TypeVar typeVar = toTypeVar(typeSig);
      if (typeEnv != NULL && isNonSchematic(typeVar, *typeEnv)) {
         if (schematicTypeVars.hasElement(typeVar))
            schematicTypeVars.removeElement(typeVar, msa());
      } else {
         if (!schematicTypeVars.hasElement(typeVar))
            schematicTypeVars.addElement(typeVar, msa());
      }
   }
   else
      if (isAp(typeSig)) {
         setSchematicTypeVars(fun(typeSig), schematicTypeVars, typeEnv);
         setSchematicTypeVars(arg(typeSig), schematicTypeVars, typeEnv);
      }
}

// Reset schematic variabes

Void TypeCheck::resetSchematicTypeVars(TypeSig typeSig, TypeVarSet& schematicTypeVars,
                                       const TypeEnv* typeEnv) const {

// Remove schematic type variables no longer present in type signature

   TypeVarElement** typeVarElementPtr = &schematicTypeVars._typeVarElements;
   while (*typeVarElementPtr != NULL) {
      TypeVar typeVar = (*typeVarElementPtr)->_typeVar;
      if (!::typeVarOccursInTypeSig(typeVar, typeSig))
         *typeVarElementPtr = (*typeVarElementPtr)->_next;
      else
         typeVarElementPtr = &(*typeVarElementPtr)->_next;
   }

   setSchematicTypeVars(typeSig, schematicTypeVars, typeEnv);
}


// TypeCheck::schematicInstance: Unify schematic instance
/*
Subst* TypeCheck::schematicInstance(TypeSig instance, TypeSig scheme, TypeEnv* typeEnv)  {
   Subst::substTypeVars(scheme, *substs());
   Subst::substTypeVars(instance, *substs());
   TypeSig typeSig = scheme;
   Bool schemeCopied = FALSE;

#ifdef TRACE
   if (traceFlag) {
      outStream << "*** Unifying instance ";
      printTypeSig(instance, 0, FALSE, outStream, env());
      outStream << " of scheme ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   Subst* extSubsts = NULL;
   Bool repeat;
   do {
      Subst* substs = NULL;
      repeat = FALSE;

      if (extSubsts != NULL) {
         Subst::substTypeVars(scheme, extSubsts);
         Subst::substTypeVars(instance, extSubsts);
      }

      if (!unify(typeSig, instance, typeEnv, &substs, TRUE)) {
         outStream << "*** Failed to unify instance ";
         printTypeSig(instance, 0, FALSE, outStream, env());
         outStream << " with scheme";
         printTypeSig(typeSig, 0, FALSE, outStream, env());
         outStream << endl;
         _nErrors++;
      }

      Subst* subst = substs;
      while (subst != NULL) {
         Subst* next = subst->_next;
         subst->_next = NULL;
         Subst::substSubsts(next, subst);

         Bool inScheme;
         if (inScheme = ::typeVarOccursInTypeSig(subst->_typeVar, typeSig)) {
             if (!schemeCopied) {
               typeSig = mutableTypeSig(scheme, msa());
               schemeCopied = TRUE;
            }
            Subst::substTypeVars(typeSig, subst);
            repeat = TRUE;
          }
         Subst::substTypeVars(instance, subst);
         if (!inScheme || ::typeVarOccursInTypeSig(subst->_typeVar, instance)) {
            subst->_next = extSubsts;
            extSubsts = subst;
         }
         subst = next;
      }
   } while (repeat);

   return extSubsts;
}

Subst* TypeCheck::schematicInstance2(TypeSig instance, TypeSig scheme, TypeEnv* typeEnv) {
  TypeSig typeSig = scheme;
   Bool schemeCopied = FALSE;
   Bool swapped = FALSE;

#ifdef TRACE
   if (traceFlag) {
      outStream << "*** Unifying instance ";
      printTypeSig(instance, 0, FALSE, outStream, env());
      outStream << " of scheme ";
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   Subst* extSubsts = NULL;
   Bool repeat;
   do {
      Subst* substs = NULL;
      repeat = FALSE;

      if (!unify(!swapped ? instance : typeSig, !swapped ? typeSig : instance, typeEnv, &substs)) {
         outStream << "*** Failed to unify instance ";
         printTypeSig(instance, 0, FALSE, outStream, env());
         outStream << " with scheme ";
         printTypeSig(typeSig, 0, FALSE, outStream, env());
         outStream << endl;
         _nErrors++;
      }

      Subst* subst = substs;
      while (subst != NULL) {
         Subst* next = subst->_next;
         subst->_next = NULL;
         Subst::substSubsts(next, subst);

         Bool inInstance;
         if (inInstance = ::typeVarOccursInTypeSig(subst->_typeVar, instance)) {
            swapped = !swapped;
            repeat = TRUE;
            break;
         }
         if (!schemeCopied) {
            typeSig = mutableTypeSig(scheme, msa());
            schemeCopied = TRUE;
         }
         Subst::substTypeVars(typeSig, subst);
         subst->_next = extSubsts;
         extSubsts = subst;
         subst = next;
      }
   } while (repeat);

   return extSubsts;
}
*/

// findConstraintSet: constraint set for type variable
// unlinks from list if found, returns NULL if none found

ConstraintSet* findConstraintSetByTypeVar(TypeVar typeVar,
                                          ConstraintSet** list,
                                          Bool remove/* = FALSE*/) {
	ConstraintSet* constraintSet;
	while ((constraintSet = *list) != NULL) {
		TypeVarElement* tve = (*list)->typeVarSet().elements();
		while (tve != NULL) {
			if (tve->typeVar() == typeVar) {
				if (remove)
					*list = constraintSet->next();
				return constraintSet;
			}
			tve = tve->next();
		}
		list = &constraintSet->next();
	}
	return NULL;
}

// addConstraintSetTypeVar: Add constraint set type variable
// Merges any intersecting sets and removes from list

typedef 	struct {
   TypeCheck*     _typeCheck;
	ConstraintSet* _constraintSet;
   Bool           _unresolved;
   Bool           _modified;
} AddConstraintSetTypeVar_Args;

Void addConstraintSetTypeVar(TypeVar typeVar, Void* arg, MSA& msa) {
	TypeCheck& typeCheck = *((AddConstraintSetTypeVar_Args*)arg)->_typeCheck;
	ConstraintSet* constraintSet = ((AddConstraintSetTypeVar_Args*)arg)->_constraintSet;

	ConstraintSet* unresolvedConstraintSet =
      findConstraintSetByTypeVar(typeVar,
                                 &typeCheck.predicates().unresolvedList(), TRUE);
	if (unresolvedConstraintSet != NULL && unresolvedConstraintSet != constraintSet)
		if (constraintSet != NULL) {
			ConstraintSet::merge(*unresolvedConstraintSet, *constraintSet, msa);
         ((AddConstraintSetTypeVar_Args*)arg)->_modified = TRUE;
		} else {
			constraintSet = unresolvedConstraintSet;
         ((AddConstraintSetTypeVar_Args*)arg)->_unresolved = TRUE;
			((AddConstraintSetTypeVar_Args*)arg)->_constraintSet = constraintSet;
		}

	ConstraintSet* workListConstraintSet =
      findConstraintSetByTypeVar(typeVar,
                                 &typeCheck.predicates().workList(), TRUE);
	if (workListConstraintSet != NULL && workListConstraintSet != constraintSet)
		if (constraintSet != NULL) {
			ConstraintSet::merge(*workListConstraintSet, *constraintSet, msa);
         ((AddConstraintSetTypeVar_Args*)arg)->_modified = TRUE;
		} else {
			constraintSet = workListConstraintSet;
		  ((AddConstraintSetTypeVar_Args*)arg)->_constraintSet = constraintSet;
		}

	if (constraintSet == NULL) {
		constraintSet = new(typeCheck.msa()) ConstraintSet(NULL);
		((AddConstraintSetTypeVar_Args*)arg)->_constraintSet = constraintSet;
      ((AddConstraintSetTypeVar_Args*)arg)->_modified = TRUE;
	}	

   TypeVarElement* tve = constraintSet->typeVarSet().elements();
	while (tve != NULL) {
		if (tve->typeVar() == typeVar)
			break;
		tve = tve->next();
	}

   if (tve == NULL) {
      constraintSet->typeVarSet().addElement(typeVar, typeCheck.msa());
      ((AddConstraintSetTypeVar_Args*)arg)->_modified = TRUE;
   }
}

// TypeCheck::addConstraintToSet: Add constraint to set according to type variables

Void TypeCheck::addConstraintToSet(Constraint& constraint, Bool unresolved/* = FALSE*/) {
	AddConstraintSetTypeVar_Args args = { this, NULL, unresolved, FALSE };
	constraint.addConstraintSetTypeVars(&args, msa());
	ConstraintSet* constraintSet = args._constraintSet;

	if (constraintSet == NULL) {

//--- Type signature list has no variables

		constraintSet = new(msa()) ConstraintSet(_predicates._workList);
		_predicates._workList = constraintSet;
	}
	else {

//    Insert set into unresolved or work list

		ConstraintSet** listPtr = unresolved && args._unresolved
         ? &_predicates._unresolvedList
         : &_predicates._workList;

		constraintSet->_next = *listPtr;
		*listPtr = constraintSet;

		ConstraintElement* constraintElement = constraintSet->elements();
		while (constraintElement != NULL) {
			if (constraintElement->constraint() == &constraint)
				return;

         if (constraint.kind() == constraintElement->constraint()->kind() &&
             constraint.matches(*constraintElement->constraint())) {
            constraintElement->constraint()->appendTransforms(constraint);
            constraint._replacement = constraintElement->constraint();
            constraint.onMatched(*constraintElement->constraint());
            return;
         }
         constraintElement = constraintElement->next();
		}
	}
   constraint.init(*this);
   constraintSet->add(constraint, msa());
}

// TypeCheck::addConstraint: Add a constraint

Void TypeCheck::addConstraint(Constraint* constraint, TypeEnv* typeEnv, 
                              Bool unresolved/* = FALSE*/) {

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
	if (traceFlag) {
		outStream << "Adding constraint\n";
		constraint->print(outStream, env(), FALSE);
      outStream << '\n';
	}
#endif
#endif

	addConstraintToSet(*constraint, unresolved);
   if (typeEnv != NULL)
	   checkConstraintsWorkList(FALSE, typeEnv);
}

// TypeCheck::checkFirstConstraint: Check first constraint in set that is:
// a) Not present in excludes list
// b) Dependent on at least one variable in substitutions

Constraint::Result TypeCheck::checkFirstConstraint(Bool fix,
                                                   const ConstraintSet& constraintSet,
											                  Subst** substs,
											                  const ExcludedConstraint* excludes,
                                                   TypeEnv* typeEnv,
																	Constraint*& resolved) {
 
	ConstraintElement* constraintElement = constraintSet.elements();
	while (constraintElement != NULL) {
		const ExcludedConstraint* exclude = excludes;
		while (exclude != NULL) {
			if (exclude->_constraint == constraintElement->constraint())
				break;
			exclude = exclude->_next;
		}
      if (exclude == NULL && constraintElement->constraint()->dependsOnSubsts(*substs))
         return constraintElement->constraint()->check(fix, constraintSet, substs, excludes, typeEnv, *this, resolved);
      constraintElement = constraintElement->next();
	}
	return Constraint::Result::SetIsEmpty;
}

// TypeCheck::checkConstraints: Check a set of constraints simultaneously

Void TypeCheck::checkConstraints(ConstraintSet& constraintSet, Bool unifyCastFlag, TypeEnv* typeEnv) {

#ifdef TRACE_CONSTRAINT_CHECKING
   static UInt probe = 0;
   if (++probe == constraintProbeSeqNo) {
      outStream << "***** TypeCheck::checkConstraints: probe hit\n";
      setTraceFlag(TRUE);
   }
   if (traceFlag) {
      outStream << "Checking constraint(s)(" << probe << ')' << (unifyCastFlag ? " - unifying coercions" : "") << "...\n";
      constraintSet.print(outStream, env());
   }
#endif

   ConstraintElement* constraintElement = constraintSet.elements();
   while (constraintElement != NULL) {
      constraintElement->constraint()->preCheck();
      constraintElement = constraintElement->next();
   }
   constraintElement = constraintSet.elements();
	while (constraintElement != NULL) {
		Subst* substs = NULL;
      Constraint* result;
      Constraint::Result res = constraintElement->constraint()->check(unifyCastFlag, constraintSet, &substs, NULL, typeEnv,  *this, result);
		switch (res.tag()) {
         case Constraint::Result::SetIsEmpty:
         case Constraint::Result::Unresolved:
            break;

         case Constraint::Result::Satisfied: {

//--------- Apply nested substitutions

            substs = Subst::substSubsts(substs);

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING

            if (traceFlag) {
               outStream << "Constraints satisfied (" << probe << ")";
               if (substs != NULL) {
                  outStream << ", solution ";
                  Subst::printSubsts(substs, outStream, env());
               } else
                  outStream << '\n';
            }
#endif
#endif

//--------- Add to existing substitutions

            _predicates._substs->preSubst(substs);

            Constraint* constraints = NULL;
            while (result != NULL) {

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
               if (traceFlag) {
                  result->print(outStream, env(), FALSE);
                  outStream << endl;
               }
#endif
#endif

               result->onSatisfied(constraintSet, substs, *this);

               ConstraintTransform* transform = result->transforms();
               while (transform != NULL) {
                  transform->apply(substs, constraints, typeEnv, *this);
                  transform = transform->next();
               }


               if (result->indep())
                  constraintSet.remove(*result);

               result = result->next();
            }

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
            if (traceFlag)
               outStream.flush();
#endif									
#endif

//--------- Apply substitution to remainder of set and re-index

            while ((constraintElement = constraintSet.elements()) != NULL) {
               Constraint* constraint = constraintElement->constraint();
               constraintSet.remove(*constraint);
               constraint->substTypeVars(substs);

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
               if (traceFlag) {
                  constraint->print(outStream, env(), FALSE);
                  outStream << '\n';
               }
#endif
#endif

               addConstraintToSet(*constraint);
            }


#ifdef TRACE
            if (traceFlag && substs != NULL) {
               outStream << "Constraint ";
               Subst::printSubsts(substs, outStream, env());
            }
#endif

            if (substs != NULL)
               substConstraints(substs, typeEnv);
            while (constraints != NULL) {
               Constraint* constraint = constraints;
               constraints = constraints->next();
               constraint->substTypeVars(substs);
               addConstraintToSet(*constraint, constraint->kind() == Constraint::SCHEME_CONSTRAINT);
            }

            if (typeEnv != NULL)
               typeEnv->substTypeVars(substs, msa());

#ifdef TRACE
            if (traceFlag && substs != NULL) {
               outStream << "Extended ";
               Subst::printSubsts(substs, outStream, env());
            }
#endif

            _predicates._substs->addSubsts(substs, msa());
            return;
         }

			case Constraint::Result::Fails:
				error("Unable to resolve constraint(s) for:");
				ConstraintElement* constraintElement = constraintSet.elements();
				while (constraintElement != NULL) {
               constraintElement->constraint()->print(outStream, _env, FALSE);
					outStream << '\n';
               constraintElement = constraintElement->next();
				}
				outStream.flush();
				return;
      }
      constraintElement = constraintElement->next();
	}

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
   if (traceFlag)
      outStream << "Constraints checked (" << probe << ")\n";
#endif
#endif

	constraintSet._next = _predicates._unresolvedList;
	_predicates._unresolvedList = &constraintSet;
}

// TypeCheck::checkConstraintsWorkList: Check worklist of constraint sets

Void TypeCheck::checkConstraintsWorkList(Bool unifyCastFlag, TypeEnv* typeEnv) {
   ConstraintSet* constraintSet;

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
   if (traceFlag)
      outStream << "Checking constraints worklist\n";
#endif
#endif

   do {
      while ((constraintSet = _predicates._workList) != NULL) {
         _predicates._workList = constraintSet->_next;
         checkConstraints(*constraintSet, unifyCastFlag, typeEnv);
//         unifyCastFlag = FALSE; ???
      }

      Constraint* constraints = NULL;
      constraintSet = _predicates._unresolvedList;
      while (constraintSet != NULL) {
         if (constraintSet->nSchematic() > 0) {
            ConstraintElement* element = constraintSet->elements();
            while (element != NULL) {
               if (element->constraint()->kind() == Constraint::SCHEME_CONSTRAINT &&
                  ((SchemeConstraint*)element->constraint())->substFlag())
                  ((SchemeConstraint*)element->constraint())->onSchemeSubst(typeEnv, constraints, *this);
               element = element->next();
            }
         }
         constraintSet = constraintSet->next();
      }
      if (constraints != NULL) {
         while (constraints != NULL) {
            Constraint* constraint = constraints;
            constraints = constraints->next();
            addConstraintToSet(*constraint);
         }
      }
      else
         break;
   } while (TRUE);

   if (_deferredUnifications != NULL)
      unifyDeferred(typeEnv);
}

// TypeCheck::substConstraints: Apply substitutions to constraints

Void TypeCheck::substConstraints(Subst* substs, TypeEnv* typeEnv) {
   ConstraintSet** constraintSetPtr = &_predicates._unresolvedList;
   ConstraintSet* constraintSet;
   Constraint* constraints = NULL;

   while ((constraintSet = *constraintSetPtr) != NULL) {
      TypeVarElement* tve = constraintSet->_typeVarSet.elements();
      BOOL dependsOnSubsts = FALSE;
      while (tve != NULL) {
         if (Subst::typeVarInSubsts(tve->_typeVar, substs)) {
            *constraintSetPtr = constraintSet->next();
            ConstraintElement* constraintElement;
            while ((constraintElement = constraintSet->elements()) != NULL) {
               Constraint* constraint = constraintElement->constraint();
               constraint->substTypeVars(substs);

#ifdef TRACE
#ifdef TRACE_CONSTRAINT_CHECKING
               if (traceFlag) {
                  outStream << "*** Substituted constraint: ";
                  constraint->print(outStream, env(), FALSE);
                  outStream << '\n';
               }
#endif
#endif
               constraintSet->remove(*constraint);
               constraint->next() = constraints;
               constraints = constraint;
            }
            dependsOnSubsts = TRUE;
            break;
         }
         tve = tve->_next;
      }
      if (!dependsOnSubsts)
         constraintSetPtr = &(*constraintSetPtr)->next();
   }
   while (constraints != NULL) {
      Constraint* constraint = constraints;
      constraints = constraint->next();
      addConstraintToSet(*constraint);
   }
}

// TypeCheck::applySubsts: Apply a set of substitutions

Void TypeCheck::applySubsts(Subst* extSubsts, TypeEnv* typeEnv) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Extended ";
      Subst::printSubsts(extSubsts, outStream, env());

      if (typeEnv != NULL)
         typeEnv->print(outStream, env());
   }
#endif

   substs()->preSubst(extSubsts);
   if (extSubsts != NULL && typeEnv != NULL)
      typeEnv->substTypeVars(extSubsts, msa());

   substConstraints(extSubsts, typeEnv);
   substs()->addSubsts(extSubsts, msa());
   checkConstraintsWorkList(FALSE, typeEnv);
}

// TypeCheck::primOcc: Primitive name occurrence type signatures

TypeSig TypeCheck::primOcc(const char* id) {
   TypeSig typeSig;
   Expr res;
   if (builtInNameMap().lookUp(builtInNameTable().lookUp(id),
                                       hashString(id),
                                       res)) {
      typeSig = toBody(res, BuiltInFn).typeSig();
   } else {
      outStream << id << endl;
      typeCheckError("Primitive type signature  missing, no prelude included?");
      return (Expr)ERROR;
   }

	if (hasTypeVar(typeSig)) {
		TypeEnv typeEnv;
		typeSig = mutableTypeSig(typeSig, msa());
		::insTypeSig(typeSig, &typeEnv, NULL, msa());
	}
	return typeSig;
}

// TypeCheck::insTypeSig: Instantiate a definition, declaration
//                        or formal parameter type signature

Void TypeCheck::insTypeSig(Expr expr, TypedVal& typedVal, TypeEnv* typeEnv) {

#ifdef TRACE
   TypeSig traceTypeSig = typedVal.typeSig();
#endif

   switch (formOf(expr)) {
      case BUILT_IN_DATA_CON:
      case DATA_CON:
      case CLASS_DATA_CON: {
         TypeEnv typeEnv;
         bindTypeConNames(typedVal.typeSig());
         TypeVarSet typeVars;
         ::insTypeSig(typedVal.typeSig(), &typeEnv, &typeVars, msa());
         typedVal.substs() = typeEnv.substs();
         typedVal.schematicTypeVars() = typeVars;
         break;
      }

      case DECL:
         toBody(expr, Decl).insTypeSig(typedVal, typeEnv, *this);
         break;

      case DEFN:
         toBody(expr, Defn).insTypeSig(typedVal, typeEnv, *this);
         break;

      case VAR:
         assert(typeEnv != NULL, "TypeCheck::insTypeSig: unexpected");
         toBody(expr, Var).insTypeSig(typedVal.typeSig(), typeEnv, *this);
         break;

      case VAR_PATTERN:
         assert(typeEnv != NULL, "TypeCheck::insTypeSig: unexpected");
         toBody(expr, VarPattern).insTypeSig(typedVal.typeSig(), typeEnv, *this);
         break;

      case CLASS_METHOD_DECL:
         toBody(expr, ClassMethodDecl).insTypeSig(typedVal, *this);
         break;

      case CLASS_METHOD_DEFN:
         toBody(expr, ClassMethodDefn).insTypeSig(typedVal, typeEnv, *this);
         break;

      case INSTANCE_METHOD_DECL:
         toBody(expr, InstanceMethodDecl).insTypeSig(typedVal, *this);
         break;

      case INSTANCE_METHOD_DEFN:
         toBody(expr, InstanceMethodDefn).insTypeSig(typedVal, *this);
         break;

      default:
         printTranExpr(expr, FALSE, outStream, env()); //???
         break;
   }


#ifdef TRACE
   if (traceFlag &&
       (formOf(expr) == VAR ||
        formOf(expr) == VAR_PATTERN ||
        !typedVal.schematicTypeVars().isEmpty())) {
      printTypeSig(traceTypeSig, 0, FALSE, outStream, env());
      outStream << " instantiated as: ";
      printTypeSig(typedVal.typeSig(), 0, FALSE, outStream, env());
      outStream << '\n';
      Subst::printSubsts(typedVal.substs(), outStream, env());
      outStream.flush();
   }
#endif

}

// TypeCheck::inferPair: Infer the type of a pair form

TypeSig TypeCheck::inferPair(Expr pair, TypeEnv* typeEnv, TypeSig failTS) {
	Expr f = fst(pair);
	Expr s = snd(pair);
   switch (formOf(f)) {
      case LITERAL:
         return infer(s, typeEnv, failTS, TRUE);

      case TYPE_SIG:
         bindTypeConNames(s);
         if (hasTypeVar(s)) {
            s = mutableTypeSig(s, msa());
            ::insTypeSig(s, typeEnv, NULL, msa());
         }
         snd(pair) = s;
         return typeCon(Type);

      case CONSTANT:
      case VARIABLE:
      case INDISPENSABLE:
         return infer(s, typeEnv, failTS);

		case REDUCE:
			return reducedTypeSig(infer(s, typeEnv, failTS), FALSE, typeEnv);

      case TAG_OF:
         return ap2(ARROW, s, TAG, msa());
     
      default:
         if (infer(f, typeEnv, failTS) == (Expr)ERROR ||
             infer(s, typeEnv, failTS) == (Expr)ERROR)
             return (Expr)ERROR;
         return ap2(PAIR, typeCon(Expr), typeCon(Expr), msa());
   }
}

// TypeCheck::inferTriple: Infer the type of a triple form

TypeSig TypeCheck::inferTriple(Expr triple, TypeEnv* typeEnv, TypeSig failTS) {
   TypeSig typeSig = thd3(triple);

   assert(fst3(triple) == Expr(CONSTRAIN), "TypeCheck::inferTriple: unexpected");

   bindTypeConNames(typeSig);
   if (hasTypeVar(typeSig)) {
      TypeEnv localTypeEnv(typeEnv);
      typeSig = mutableTypeSig(typeSig, msa());
      ::insTypeSig(typeSig, &localTypeEnv, NULL, msa());
   }

   unifyOrError(infer(snd3(triple), typeEnv, failTS), typeSig, typeEnv,
                "Failed to unify constraint");

   Subst::substTypeVars(typeSig, *substs());

   return typeSig;
}

// TypeCheck::infer: Infer the type of a tuple form

TypeSig TypeCheck::inferTuple(UInt n, TypeSig resTS) {
	if (n > 0) {
      TypeSig typeVar = fromTypeVar(::newTypeVar());
      return ap2(ARROW,
                 typeVar,
                 inferTuple(n - 1, typeSigAp(resTS, typeVar, msa())), msa());
   }
   else
      return resTS;
}

// TypeCheck::infer: Infer the type of an expression

TypeSig TypeCheck::infer(Expr expr, TypeEnv* typeEnv, TypeSig failTS, Bool strict/* = FALSE*/) {
	TypeSig res;
	Bool nonStrictFlag = FALSE;

   assert(expr != (Expr)ERROR, "TypeCheck::infer: ERROR unexpected");

#ifdef TRACE
	static UInt probe = 0;
   if (++probe == inferProbeSeqNo) {
		outStream << "***** TypeCheck::infer: probe hit\n";
      setTraceFlag(TRUE);
   }
   UInt localProbe = probe;

   if (traceFlag) {
		ExprRepr forms[] = {DECL, DEFN, CLASS_METHOD_DECL, CLASS_METHOD_DEFN};
		if (!formExistsIn(expr, sizeof(forms) / sizeof(ExprRepr), forms)) {
         Bool printedSubsts = FALSE;
         for (const TypeEnv* traceTypeEnv = typeEnv;
              traceTypeEnv != NULL;
              traceTypeEnv = traceTypeEnv->next())
            if (traceTypeEnv->substs() != NULL) {
               typeEnv->printSubsts(outStream, env());
               printedSubsts = TRUE;
               break;
            }
         if (printedSubsts)
            outStream << '\n';
			outStream << "TypeCheck::infer (" << probe << "): ";
			printTranExpr(expr, FALSE, outStream, env(), TRUE);
			outStream << endl;
		}
   }
#endif

   if (isPair(expr))
      res = inferPair(expr, typeEnv, failTS);
   else if (isTriple(expr))
      res = inferTriple(expr, typeEnv, failTS);
   else switch (formOf(expr)) {
      case RETURN:
      case EXCEPTION:
         res = typeCon(Void);
         break;

      case FAIL:
         res = failTS;
			break;

      case FAT_BAR: { // a -> a -> a
         TypeVar typeVar = ::newTypeVar();
         res = ap2(ARROW,
            fromTypeVar(typeVar),
            ap2(ARROW,
               fromTypeVar(typeVar),
               fromTypeVar(typeVar), msa()), msa());
         break;
      }

      case SEQ: { // Void -> a -> a
         TypeVar typeVar = ::newTypeVar();
         res = ap2(ARROW,
                   typeCon(Void),
                   ap2(ARROW,
                       fromTypeVar(typeVar),
                       fromTypeVar(typeVar), msa()), msa());
			break;
      }

      case COND: { // COND :: Bool -> a -> a -> a
         TypeVar typeVar = ::newTypeVar();
         res = ap2(ARROW,
                   typeCon(Bool),
                   ap2(ARROW,
                       fromTypeVar(typeVar),
                       ap2(ARROW,
                           fromTypeVar(typeVar),
                           fromTypeVar(typeVar), msa()), msa()), msa());
			break;
      }

      case EQ_TAG_FN:
         res =  ap2(ARROW,
                    TAG,
                    ap2(ARROW, TAG, typeCon(Bool), msa()), msa());
			break;
 
      case EQ_NULL_FN:
         res = ap2(ARROW, typeCon(List), typeCon(Bool), msa()); // Why no parameter for List ???
			break;

      case VAR:
         res = toBody(expr, Var).typeSig();
         break;

      case VAR_REF:
         res = toBody(expr, VarRef).var().typeSig();
         break;

      case VOID:
         res = typeCon(Void);
			break;

      case NAME:
         res = typeCon(Name);
			nonStrictFlag = TRUE;
         break;
      
      case TYPE:
         res = typeCon(Type);
			nonStrictFlag = TRUE;
         break;

      case INT:
         res = typeCon(Int);
			nonStrictFlag = TRUE;
         break;
   
      case CHAR:
         res = typeCon(Char);
			nonStrictFlag = TRUE;
         break;

      case CONST:

//------ The only constant at this stage is the constant null pointer
         res = POINTER;
         break;

      case TAG:
         res = TAG;
			nonStrictFlag = TRUE;
         break;

      case REF:
      case THIS:
         res = typeCon(Ref);
			nonStrictFlag = TRUE;
         break;

      case LAMBDA:
      case CURRIED_LAMBDA:
      case FN_AP:
      case LET:
	   case NAME_OCC:
		case DATA_CON_DEFN:
		case DEFN:
      case CONCRETE_DATA_CON:
		case CLASS_METHOD_DEFN:
      case CONST_PATTERN:
      case VAR_PATTERN:
      case WILDCARD_PATTERN:
      case DATA_CON_PATTERN:
      case CONCRETE_PATTERN:
      case FAT_BAR_SEQ:
		case APP_LAMBDA:
      case COERCE:
         if (isPtr(expr))
            res = toBody(expr, AST_Node).infer(typeEnv, *this, failTS, nonStrictFlag);
			else {
				typeCheckError("TypeCheck::infer: untransformed form");
				printTranExpr(expr, FALSE, outStream, env());
				outStream << endl;
				res = ERROR;
			}
			break;
	
      case DATA: {
         Type type = toType(exprCellInfo(expr).form());
         res = typeSignature(type);
			nonStrictFlag = TRUE;
			break;
      }
      case BUILT_IN_DATA_CON:
         res = toBody(expr, BuiltInDataCon).typeSig();
         if (hasTypeVar(res)) {
            TypeEnv localTypeEnv;   // Data constructor scope is global
            res = mutableTypeSig(res, msa());
            ::insTypeSig(res, &localTypeEnv, NULL, msa());
         }
         //      nonStrictFlag = TRUE;
         break;

		case DATA_CON:
         res = toBody(expr, DataCon).typeSig();
			if (hasTypeVar(res)) {
				TypeEnv localTypeEnv;   // Data constructor scope is global
				res = mutableTypeSig(res, msa());
				::insTypeSig(res, &localTypeEnv, NULL, msa());

			}
 //      nonStrictFlag = TRUE;
         break;

		case TUPLE:
         res = inferTuple(tupleCard(expr), expr);
			break;

      case BUILT_IN_FN:
         res = toBody(expr, BuiltInFn).typeSig();
         if (hasTypeVar(res)) {
            TypeEnv localTypeEnv;   // Built-in function scope is global
            res = mutableTypeSig(res, msa());
            ::insTypeSig(res, &localTypeEnv, NULL, msa());
         }
         break;
/*
      case CLASS_METHOD:
         res = toBody(expr, ClassMethod).typeSig();
*/
      default:
			typeCheckError("TypeCheck::infer: unrecognised form - ");
         printTranExpr(expr, FALSE, outStream, env());
         outStream << endl;
		   res = (Expr)ERROR;
			break;
   }

	if (nonStrictFlag && !strict)
		res = expTypeSig(res, msa());

#ifdef TRACE
   if (traceFlag) {
		ExprRepr forms[] = {DECL, DEFN, CLASS_METHOD_DECL, CLASS_METHOD_DEFN};
		if (!formExistsIn(expr, sizeof(forms) / sizeof(ExprRepr), forms)) {
			outStream << "TypeCheck::inferred (" << localProbe << "): ";
			printTranExpr(expr, FALSE, outStream, env(), TRUE);
         if (formOf(expr) != NAME_OCC || TRUE) {
			   outStream << " :: ";
			   printTypeSig(res, 0, FALSE, outStream, env());
         }
			outStream << endl;
		}
   }
#endif

	return res;
}

// TypeCheck::insDeclOrDefnTypeSig: Instantiate a declaration or definition binding type signature

Void TypeCheck::insDeclOrDefnTypeSig(Expr declOrDefn, TypeEnv* typeEnv) {
	assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "TypeCheck::typeCheckBindings: expected binding");
	NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
   binding.insTypeSig(typeEnv, *this);
}

// TypeCheck::insDeclOrDefnsTypeSigs: Instantiate declaration or definition bindings type signatures

Void TypeCheck::insDeclOrDefnsTypeSigs(Expr declOrDefns, TypeEnv* typeEnv) {
	mapProc1(insDeclOrDefnTypeSig, declOrDefns, typeEnv);
}

// TypeCheck::typeCheck: Type check a definition or declaration

Void TypeCheck::typeCheck(Expr expr, TypedVal& typedVal, TypeEnv* typeEnv, TypeSig failTS) {
   switch (formOf(expr)) {
      case DECL:
         toBody(expr, Decl).typeCheck(typedVal, typeEnv, failTS, *this);
         break;

      case DEFN:
         toBody(expr, Defn).typeCheck(typedVal, typeEnv, failTS, *this);
         break;

      case CLASS_METHOD_DECL:
         toBody(expr, ClassMethodDecl).typeCheck(typedVal, typeEnv, failTS, *this);
         break;

      case CLASS_METHOD_DEFN:
         toBody(expr, ClassMethodDefn).typeCheck(typedVal, typeEnv, failTS, *this);
         break;

      case INSTANCE_METHOD_DECL:
         toBody(expr, InstanceMethodDecl).typeCheck(typedVal, typeEnv, failTS, *this);
         break;

      case INSTANCE_METHOD_DEFN:
         toBody(expr, InstanceMethodDefn).typeCheck(typedVal, typeEnv, failTS, *this);
         break;

      default:
         printTranExpr(expr, FALSE, outStream, env()); //???
         assert(FALSE, "TypeCheck::typeCheck: unexpected form");
         break;
   }
}

// TypeCheck::typeCheckDeclOrDefn: Type check a declaration or definition binding

Void TypeCheck::typeCheckDeclOrDefn(Expr declOrDefn, TypeEnv* typeEnv, TypeSig failTS) {
	assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "TypeCheck::typeCheckDeclOrDefn: expected binding");
	NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);

   binding.typeCheck(typeEnv, failTS, *this);
}

// TypeCheck::typeCheckDeclOrDefns: Type check a list of declaration or definition bindings

Void TypeCheck::typeCheckDeclOrDefns(Expr declOrDefns, TypeEnv* typeEnv, TypeSig failTS) {
   mapProc2(typeCheckDeclOrDefn, declOrDefns, typeEnv, failTS);
}

Void TypeCheck::removeSchematicConstraints(Void) {
   Constraint* constraints = NULL;
   ConstraintSet** constraintSetPtr = &_predicates._unresolvedList;
   while (*constraintSetPtr != NULL) {
      ConstraintSet& constraintSet = **constraintSetPtr;
      if (constraintSet.nSchematic() > 0) {
         do {
            ConstraintElement* element = constraintSet.elements();
            if (element == NULL)
               break;
            Constraint* constraint = element->constraint();
            constraintSet.remove(*constraint);
            if (!(element->constraint()->kind() == Constraint::SCHEME_CONSTRAINT ||
                  element->constraint()->kind() == Constraint::SCHEMATIC_INSTANCE_CONSTRAINT)) {
               constraint->next() = constraints;
               constraints = constraint;
            }
         } while (TRUE);
         *constraintSetPtr = (*constraintSetPtr)->next();
      }
      else
         constraintSetPtr = &constraintSet.next();
   }
   while (constraints != NULL) {
      Constraint* constraint = constraints;
      constraints = constraint->next();
      if (constraint->kind() == Constraint::COERCION_CONSTRAINT)
         ((CoercionConstraint&)*constraint).setNoFix();
      addConstraintToSet(*constraint, TRUE);
   }
}

// TypeCheck::resetNeedsInstance: Reset type schemes

Void TypeCheck::resetNeedsInstance(Expr expr, TypedVal& typedVal, TypeEnv* typeEnv) {
   switch (formOf(expr)) {
      case DEFN:
         toBody(expr, Defn).resetNeedsInstance(typedVal, typeEnv, *this);
         break;

      case CLASS_METHOD_DEFN:
         toBody(expr, ClassMethodDefn).resetNeedsInstance(typedVal, typeEnv, *this);
         break;

      case INSTANCE_METHOD_DEFN:
         toBody(expr, InstanceMethodDefn).resetNeedsInstance(typedVal, typeEnv, *this);
         break;

      default: 
         break;
      }
}


// TypeCheck::resetDeclOrDefnNeedsInstance: Reset needs instance flag for a declaration or definition

Void TypeCheck::resetDeclOrDefnNeedsInstance(Expr declOrDefn, TypeEnv* typeEnv) {
   assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "TypeCheck::resetDeclOrDefnNeedsInstance: expected binding");
   NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
   binding.resetNeedsInstance(typeEnv, *this);
}

// TypeCheck::resetDeclOrDefnsNeedsInstance: Reset needs instance flag for a list of declarations or definitions
// Also removes any schematic constraints

Void TypeCheck::resetDeclOrDefnsNeedsInstance(Expr declOrDefns, TypeEnv* typeEnv) {
   mapProc1(resetDeclOrDefnNeedsInstance, declOrDefns, typeEnv);
}

// TypeCheck::typeCheck: Check types in a script

Bool TypeCheck::typeCheck(Expr expr) {
	switch (formOf(expr)) {
      case MODULE:
         toBody(expr, ModuleDefn).typeCheck(*this);
         break;

      case ORDER:
         toBody(expr, Order).typeCheck(*this);
         break;

      case SNIPPET:
         toBody(expr, Snippet).typeCheck(*this);
         break;

      default: {

         TypeEnv typeEnv;
         TypeSig typeSig = infer(expr, &typeEnv, ERROR, TRUE);

         printTypeSig(typeSig, 0, FALSE, outStream, env());
         outStream << '\n';

         //error("TypeCheck::typeCheck: Unexpected form");
         break;
      }
   }

#ifdef TRACE
   OptionsParser optionsParser(_options);
   if (optionsParser.hasKey("substs")) {
      substs()->print(outStream, env());

      ConstraintSet* constraintSet = _predicates._unresolvedList;
      if (constraintSet != NULL) {
         outStream << "Unresolved constraints:\n";
         printConstraints(outStream);
      }
   }
#endif

   return _nErrors == 0;
}

// TypeCheck::printConstraints: Print unresolved constraints

Void TypeCheck::printConstraints(ostream& os, Bool shortForm/* = FALSE*/) {
	ConstraintSet * constraintSet = _predicates._unresolvedList;
	while (constraintSet != NULL) {
		constraintSet->print(os, env(), shortForm);
		constraintSet = constraintSet->_next;
	}
}

// TypeCheck::error: Signals an error.

Void TypeCheck::error(const char* s) {
   _nErrors++;
   ivoryError1("Type check error: %s", s);
}
/*
Useful debugging code snippets

   printTranExpr(e, FALSE, outStream, env());
   outStream << endl;
 

   printTranExpr(e, FALSE, outStream, typeCheck.env());
   outStream << endl;
 

	printTypeSig(_typeSig, 0, FALSE, outStream, typeCheck.env());
   outStream << endl;

*/
