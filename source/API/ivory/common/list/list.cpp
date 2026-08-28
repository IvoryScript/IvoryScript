/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    list.cpp
 *
 * Module:  List data type
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of List type.
 *
 * Notes:
 * 
 *    T.B.D.  Replaced by 'List.is'
 * 
 *    List Expr is now the only built-in type.
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
#include "ivory/list.h"
#include "ivory/ptr.h"

static TypeSig mkTypeVars$Nil(Void) {
   return builtInCons(fromName(builtInName(a)), Nil);
};

defineBuiltInParamTypeCon(List, mkTypeVars$Nil);

static Void init$ExprList(TypeDescr* typeDescr);
defineType(ExprList, builtInAp(typeCon(List), builtInAp(typeCon(Exp), typeCon(Any))), init$ExprList);

#undef Nil

// Nil is treated as a special case

static TypeSig mkTypeSig$Nil(Void) {
   return builtInAp(typeCon(List), fromName(builtInName(a)));
};

// Entry code for []

defineEntry(Nil){
   returnNil();
}

// Entry code for constructor []::List[Exp *]

defineEntry(Nil$ExprList){
rPtr=NULL;
rType = builtInType(ExprList);
jump(popLabel());
}

static Cell* copyExpr$Nil(Cell& src,const Env& srcEnv,Env& env, MSA& msa argN_VM) {
   return builtInDataCon(Nil);
};

static Void cellShowFn$Nil(const Cell& cell_,ostream& os,const Env& env argN_VM) {
   os << "Nil";
};

CellInfo cellInfo$Nil(BUILT_IN_DATA_CON, 0, entry(Nil), copyExpr$Nil,
   
#if (SERIALISATION==1)
                      NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                      NULL,
#endif   
   
                      destroyCell, cellShowFn$Nil);

CellInfo cellInfo$NilExprList(BUILT_IN_DATA_CON, 0, entry(Nil$ExprList), copyExpr$Nil,
   
#if (SERIALISATION==1)
                              NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                              NULL,
#endif      
      
                              destroyCell, cellShowFn$Nil);

Expr dataCon$Nil = mkBuiltInDataCon("Nil", 0, mkTypeSig$Nil, NULL, cellInfo$NilExprList);

#define Nil builtInDataCon(Nil)

declareLabel(CONS$);

static Void cellDestroyCons(Cell& cell_, Env& env, MSA& msa argN_VM) {
   hd(fromCell(cell_)).destroy(env, msa n_vm);
   tl(fromCell(cell_)).destroy(env, msa n_vm);
   msa.free(&cell_);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopyCons(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return mkCons(Expr(hd(fromCell(src)), srcEnv, env, msa n_vm),
                 Expr(tl(fromCell(src)), srcEnv, env, msa n_vm), msa);
}

static Void cellShowCons(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << '[';
   printList(fromCell(cell_), os, env);
   os << ']';
}

#undef localConst
#define localConst

static CellInfo cellInfo$CONS(CONS, 0, 
                              label(CONS$),
                              cellCopyCons,

#if (SERIALISATION==1)
                              NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                              NULL,
#endif      

                              cellDestroyCons,
                              cellShowCons);

Cell* mkCons(Expr x, Expr xs, MSA& msa) {
   Cell& cell_ = *new(sizeof(Cons_<Expr>), msa) Cell(&cellInfo$CONS);
   new(cell_.body()) Cons_<Expr>(x, xs);
   return &cell_;
}

// Entry label for a CONS cell

defineLabel(CONS$) {
//   checkArgs(0, 0, "CONS");
   returnCons(&cellBody(*cell, Cons_<Expr>), cellEnv);
}

Void init$ExprList(TypeDescr* typeDescr) {
}


UInt length(Expr xs) {
    UInt res = 0;
    for (Expr x = xs; x != Nil; x = tl(x))
       res++;
    return res;
}

Expr concat(Expr xs1, Expr xs2, MSA& msa) {
   if (xs1 == Nil)
      return xs2;
   return cons(hd(xs1), concat(tl(xs1), xs2, msa), msa);
}

Expr reverse(Expr xs, MSA& msa) {
   if (xs == Nil)
      return xs;
   return concat(reverse(tl(xs), msa), cons(hd(xs), Nil, msa), msa);
}

/*----------------------------------------------------------------------------*/

// Built-in Cons constructor

// Cons :: (Exp *) -> (Exp (List (Exp *))) -> List (Exp *)
// Cons head tail = returnCons head tail

// Notes:
//    1. non-strict for both the head and tail arguments.
//       Hence not implemented as algebraic type
//
//    2. head and tail will be copied if necessary to ensure
//       that they match the construction environment.

#undef Cons

declareEntry(Cons);

defineBuiltInDataCon(Cons, 1,
                     builtInAp2(ARROW,
                                fromName(builtInName(a)),
                                builtInAp2(ARROW,
                                           builtInAp(typeCon(Exp), builtInAp(typeCon(List), fromName(builtInName(a)))),
                                           builtInAp(typeCon(List), fromName(builtInName(a))))),
                     builtInAp(typeCon(Ptr),
                               builtInAp2(PAIR,
                                          fromName(builtInName(a)),
                                           builtInAp(typeCon(Exp),
                                                     builtInAp(typeCon(List),
                                                               fromName(builtInName(a)))))))
#define tail         0
#define head         (tail+1)
#define stackDepth  0
{
   checkArgs("Cons");
   jump(altEntry(Cons));
}

defineAltEntry(Cons) {
/*
   rEnv = consEnv;
   rType = type(ExprList);
   rTag = CONS_TAG;

   EnvExpr t1 = localA(head);
   EnvExpr t2 = localA(tail);

   Expr h, t;

#ifdef __GNUC__

//GCC bug

   if (localA(head).env() == NULL || localA(head).env() == consEnv)
      h = localA(head).expr();
   else
      h = Expr(localA(head).expr(), localA(head).env(), *consEnv, *consMSA);
   if (localA(tail).env() == NULL || localA(tail).env() == consEnv)
      t = localA(tail).expr();
   else
      t = Expr(localA(tail).expr(), localA(tail).env(), *consEnv, *consMSA);
#else
   h = localA(head).env() == NULL || localA(head).env() == consEnv
      ? localA(head).expr()
      : Expr(localA(head).expr(), localA(head).env(), *consEnv, *consMSA);
   t = localA(tail).env() == NULL || localA(tail).env() == consEnv
      ? localA(tail).expr()
      : Expr(localA(tail).expr(), localA(tail).env(), *consEnv, *consMSA);
#endif
   rPtr = new(*consMSA) ConsExpr(h, t);
   dropStack(stackDepth);
*/
   jump(popLabel());
}
