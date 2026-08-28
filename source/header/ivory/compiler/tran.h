/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    tran.h
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
 *    Declaration of 'Tran' class and associated translation functions
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

#ifndef IVORY_COMPILER_TRAN_H_DEFINED
#define IVORY_COMPILER_TRAN_H_DEFINED

#include "AST.h"

#define PRINT_NAME_OCC_INST_SUBSTS           // If set, name occurrence instance substitutions are printed

declareTypeCon(Plain);

// Tran: AST translation.

class Tran {
public:
   Tran(UInt pass, ConstString options, Env& env);
   inline UInt pass(Void) const { return _pass; }
   inline const SubstSet& substs(Void) const { return *_substs; }
   inline Env& env(Void) { return *_env; }
   inline const Env& env(Void) const { return *_env; }
   inline NameTable& nameTable(Void) const { return _env->nameTable(); }
   inline TypeTable& typeTable(Void) const { return _env->typeTable(); }
   inline MSA& msa(Void) const { return _env->msa(); }
   TypedVal* lookUpExtBinding(Name name, Bool dataCon, const ModuleDefn*& moduleDefnRes,
      Name& mappedNameRes);
   TypedVal* lookUpBuiltInBinding(Name name);
   Void lookUpDataCon(Expr& dataCon, BindingSet& bindingEnv,
      TypedVal*& typedVal, const ModuleDefn*& moduleDefnRes);
   ClassDefn* lookUpClass(Name name, const ModuleDefn*& moduleDefnRes);
   Name useName(ConstString nameString) const;
   Expr fnAp(Expr f, Expr arg, TypeSig typeSig = UNKNOWN, Bool updatable = TRUE) const;
   Expr fnAp2(Expr f, Expr arg1, Expr arg2, TypeSig typeSig = UNKNOWN, Bool updatable = TRUE) const;
   Expr fnAp(Expr f, Expr* argV, UInt nArgs, TypeSig typeSig = UNKNOWN) const;
   Bool coerceToApply(Expr& expr, Bool noCast) const;
   Expr coerce(Expr expr, Bool force = FALSE) const;
   Expr mapToEnv(Expr src, Expr dstEnv) const;
   Expr reduce(Expr expr) const;
   Expr condReduce(Expr expr, Bool flag) const;
   Bool hasNameOcc(Name name, Expr expr) const;
   Bool hasNameOccInList(Name, Expr list) const;
   UInt nameOccCount(Expr expr, const TypedVal* typedVal) const;
   UInt nameOccCountInList(Expr list, const TypedVal* typedVal) const;
   Void alphaConv(Expr expr, const TypedVal* binding, Name toName) const;
   Void alphaConvList(Expr list, const TypedVal* binding, Name toName) const;
   Void substitutePair(Expr pair, Expr val, TypedVal* binding);
   Void substituteList(Expr list, Expr val, TypedVal* binding);
   Void substituteExpr(Expr& expr, Expr val, TypedVal* binding);
   TypeSig copyTypeSig(TypeSig typeSig, CopyContext* cc, const Subst* substs);
   Expr copyList(const Expr list, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs);
   Expr copyPair(const Expr src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs);
   Expr copyTriple(const Expr src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs);
   Expr copyExpr(const Expr src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs);
   Expr simpleLet(Expr signedId, Expr expr, Expr body, Bool substitutable = FALSE);
   Expr defn(Expr lhs, Expr rhs, Bool outermost = FALSE, Bool redefinable = FALSE,
      Bool isInline = FALSE, Bool exclusive = FALSE);
   Expr let(Expr declOrDefns, Expr body, Bool recursive = FALSE, Bool suppressEval = FALSE);
   Expr unpackDataCon(Expr dataCon, UInt arity,
      Bool isPlainDecon, Bool isPtrDecon, Bool copyArg,
      TypeSig typeSig, TypeSig reprTypeSig,
      Subst* substs, const ModuleDefn* moduleDefn, Expr arg);
   Expr cond(Expr arg, Expr eTrue, Expr eFalse, Bool negate, TypeSig typeSig) const;
   Expr condTypeSig(TypeSig typeSig) const;
   Expr condTestTypeSig(TypeSig typeSig) const;
   Expr condEq(Expr arg, Expr eqOcc, Expr val, Expr expr, TypeSig typeSig) const;
   Expr condTag(Expr arg, Expr tag, Expr expr, TypeSig argTypeSig, TypeSig typeSig) const;
   Expr condNULL(Expr arg, Expr e1, Expr e2, Bool negate, TypeSig argTypeSig, TypeSig typeSig) const;
   Void tranPattern(Expr pattern, NameTypedValBinding* bindings, UInt& i);
   NameTypedValBinding* tranPatterns(UInt nPatterns, Expr patterns);
   Expr lambda(Expr patterns, Expr body, TypeSig typeSig = UNKNOWN, Bool updatable = TRUE);
   Expr lambdaAp(Expr patterns, Expr body, Expr args);
   Expr caseExpr(Case::Kind kind, Expr arg, TypeSig argTypeSig,
      UInt n, Expr alts, TypeSig typeSig, Bool closed);
   Expr tranDefault(Expr expr, Bool hasDefault, Expr otherwise);
   Expr fatBarSeq(Expr alts, Expr args, TypeSig patTypeSig = UNKNOWN);

   // Rename the following functions

   Expr doPat(Expr pat);
   Expr doCaseAlt(Expr alt, Bool castFlag);
   Expr doCaseAlts(Expr alts, Bool castFlag);
   Expr nameTypedValBinding(Name name, Expr val, TypeSig typeSig);
   Expr nameTypedValBinding(Expr signedId, Expr val);
   Void tranDeclOrDefn(Expr& declOrDefn, Bool outermost, Bool isInline, Bool exclusive);
   Void tranDeclOrDefns(Expr declOrDefns, Bool outermost);
   Void addBinding(Expr declOrDefn, BindingSet& bindingEnv);
   Void addBindings(Expr declOrDefns, BindingSet& bindingEnv);
   Void tranDeclOrDefn(Expr declOrDefn, BindingSet& bindingEnv);
   Void tranDeclOrDefns(Expr declOrDefns, BindingSet& bindingEnv);
   Expr makeDataConAlt(Expr dataCon, Expr expr) const;

   Void tranReduce(Expr& pair);
   Void tranClosure(Expr& expr, TypeSig typeSig, Bool updatable, Bool force = FALSE);
   Void tranPair(Expr& pair, BindingSet& bindingEnv);
   Void tranTriple(Expr& triple, BindingSet& bindingEnv);
   Void tranCons(Expr& cons, BindingSet& bindingEnv);
   Expr tranSequenceList(Expr list, Bool first);
   Void tranExpr(Expr& expr, BindingSet& bindingEnv);
   Expr tranCase(Expr expr, Expr alts);
   Expr caseAlts(Expr alts, Expr& sharedExprs, Bool& hasDefault, Expr& otherwise);
   Expr tranDefnCmd(Expr defn) const;
   Expr tranUndefnCmd(Expr undefn) const;
   Void tranAp(Expr& expr, Expr f, Expr a, BindingSet& bindingEnv);
   Bool tran(Expr& expr, BindingSet* = NULL);
   Void error(const char* s);
   Void warn(const char* s);

protected:
   Bool probableMissingDeclSemicolon(Name unresolvedName) const;
   Void printMissingDeclSemicolonHint(Name unresolvedName) const;

   UInt			      _nErrors;
   UInt			      _nWarnings;
   UInt			      _pass;
   ConstString	      _options;
   Env* _env;
   BindingSet        _builtInBindingSet;
   SubstSet* _substs;
   BidirNameMap* _varNameMaps;      // List of variable name maps
   BidirNameMap* _typeVarMaps;      // List of type variable maps
   ModuleDefn* _moduleDefn;
   Name _resolverBindingName;
   TypeSig _resolverBindingTypeSig;

   friend class ModuleDefn;
   friend class Order;
   friend class Snippet;
   friend class Defn;
   friend class NameTypedValBinding;
   friend class NameOcc;
   friend class FnAp;
   friend class DataConDecl;
   friend class DataConDefn;
   friend class TypeDefn;
   friend class DataConPattern;
   friend class InstanceDataConDefn;
};

// Macros

#define isReduce(expr) (isPair(expr)&&fst(expr)==Expr(REDUCE))

#define isPlainTypeSig(ts) (isAp(ts)&&(fun(ts)==typeCon(Plain)))

#define unwrapPlainTypeSig(ts) (isPlainTypeSig(ts)?arg(ts):(ts))

// Extern references

extern UInt dataConArity(Expr dataCon);

extern Tag dataConTag(Expr dataCon);

extern Void idForm(Expr signedId, Name& name, TypeSig& typeSig);

extern Void initSyntax(Void);

extern Name newName(Void);

extern Void printTranExpr(Expr expr, Bool bFlag, ostream& os, const Env& env,
   Bool pretty = FALSE, UInt indent = 0);

extern Void printTranList(Expr list, Bool inner, ostream& os, const Env& env,
   Bool pretty = FALSE, UInt indent = 0,
   Bool itemPerLine = FALSE);

extern Void printTranName(Name name, ostream& os, const Env& env);

extern Void resetNameSupply(Void);

#endif /* IVORY_COMPILER_TRAN_H_DEFINED */
