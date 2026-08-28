/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    tuple.cpp
 *
 * Module:  Ivory common (tuple)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Ivory tuple type
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
#include "ivory/machine.h"
#include "ivory/repr.h"
#include "ivory/tuple.h"

// Structure for closure update

struct TupleIndirection
{
   Type  _type;
#if (GARBAGE_COLLECTION==1)
   Cell* _cell;
#endif
   Ptr   _ptr;
}; 

defineTypeCon(Pair);
static Void init_Pair(TypeDescr* typeDescr);
defineType_n(1, Pair, builtInAp2(PAIR,
                                 builtInAp(typeCon(Exp), typeCon(Any)),
                                 builtInAp(typeCon(Exp), typeCon(Any))), init_Pair);

Cell* mkTuple(UInt degree, const Expr* src, Expr tag, MSA& msa) {
   Cell& _cell = *new(sizeof(Expr) * degree, msa) Cell(tag);
   Expr* ptr = (Expr*)_cell.body();
   for (UInt i = 0; i < degree; i++)
      *ptr++ = src[i];
   return &_cell;
}

// Return value of tuple as type Expr

declareLabel(enter_Tuple);


static CellInfo cellInfo1(DATA, 0,
   label(enter_Tuple),
   NULL, NULL,

#if (SERIALISATION==1)
   NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
   NULL,
#endif

   NULL, NULL);




defineLabel(enter_Tuple)
{
   rType = toBody(cell, struct TupleIndirection)._type;
#if (GARBAGE_COLLECTION==1)
   rCell = toBody(cell, struct TupleIndirection)._cell;
#endif
   rPtr = toBody(cell, struct TupleIndirection)._ptr;
   jump(popLabel());
}

Cell* fromTuple(Type type, Ptr ptr, MSA& msa) {
   Cell& cell_ = *new(sizeof(struct TupleIndirection), msa) Cell(&cellInfo1);
   cellBody(cell_, struct TupleIndirection)._type = type;
   cellBody(cell_, struct TupleIndirection)._ptr  = ptr;
   return &cell_; 
}

declareLabel(enterPair);

static Cell* cellCopyFnPair(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return mkPair(Expr(fst(fromCell(src)), srcEnv, env, msa n_vm),
                 Expr(snd(fromCell(src)), srcEnv, env, msa n_vm), msa);
}

static Void cellDestroyFnPair(Cell& cell_, Env& env, MSA& msa argN_VM) {
   fst(fromCell(cell_)).destroy(env, msa n_vm);
   snd(fromCell(cell_)).destroy(env, msa n_vm);
   msa.free(&cell_);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Void cellShowFnPair(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << '(';
   printExpr(fst(fromCell(cell_)), os, env);
   os << ',';
   printExpr(snd(fromCell(cell_)), os, env);
   os << ')';
}

#undef localConst
#define localConst      

defineLabel(enterPair)
{
//   checkArgs(0, 0, "Pair");
   rPtr = cell->body();
   rType = builtInType(Pair);
   rEnv = cellEnv;
   jump(popLabel());
}

static CellInfo cellInfo2(fromType(builtInType(Pair)), 0,
                         label(enterPair),
                          cellCopyFnPair,
   
#if (SERIALISATION==1)
                          NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                          NULL,
#endif   
   
   
                          cellDestroyFnPair, cellShowFnPair);

Void init_Pair(TypeDescr* typeDescr) {
}

Cell* mkPair(Expr fst, Expr snd, MSA& msa) {
   Expr pair[2] = {fst, snd};
	return mkTuple(2, pair, &cellInfo2, msa);
}


defineType_n(2, Triple, builtInAp3(TRIPLE,
                                   typeCon(Expr),
                                   typeCon(Expr),
                                   typeCon(Expr)), NULL);


declareLabel(enterTriple);

static Void destroyTriple(Cell& cell_, Env& env, MSA& msa argN_VM) {
   fst3(fromCell(cell_)).destroy(env, msa n_vm);
   snd3(fromCell(cell_)).destroy(env, msa n_vm);
   thd3(fromCell(cell_)).destroy(env, msa n_vm);
   msa.free(&cell_);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* copyExprTriple(Cell& cell_, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return mkTriple(Expr(fst3(fromCell(cell_)), srcEnv, env, msa n_vm),
                   Expr(snd3(fromCell(cell_)), srcEnv, env, msa n_vm),
                   Expr(thd3(fromCell(cell_)), srcEnv, env, msa n_vm), msa);
}

static Void cellShowFnTriple(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << '(';
   printExpr(fst3(fromCell(cell_)), os, env);
   os << ',';
   printExpr(snd3(fromCell(cell_)), os, env);
   os << ',';
   printExpr(thd3(fromCell(cell_)), os, env);
   os << ')';
}

#undef localConst
#define localConst      

#define stackDepth 0
defineLabel(enterTriple)
{
   checkArgs("Triple");
   rPtr = cell->body();
   rType = builtInType(Triple);
   rEnv = cellEnv;
   jump(popLabel());
}

static CellInfo cellInfo$Triple(fromType(builtInType(Triple)), 0,
                                label(enterTriple),
                                copyExprTriple,
   
#if (SERIALISATION==1)
                                NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                                NULL,
#endif   
   
                                destroyTriple, cellShowFnTriple);

Cell* mkTriple(Expr fst3, Expr snd3, Expr thd3, MSA& msa) {
   Expr triple[3] = {fst3, snd3, thd3};
	return mkTuple(3, triple, &cellInfo$Triple, msa);
}

/*----------------------------------------------------------------------------*/

// Built-in  constructor

// Pair :: (Exp *) -> (Exp *) -> Pair (Exp *) (Exp *)

// Notes:
//    1. components will be copied if necessary to ensure
//       that they match the construction environment.

#undef Pair

defineDataCon(Pair, 1, typeCon(Pair),
              builtInAp2(ARROW,
                         builtInAp(typeCon(Exp), typeCon(Any)),
                         builtInAp2(ARROW,
                                    builtInAp(typeCon(Exp), typeCon(Any)),
                                    builtInAp2(PAIR,
                                               builtInAp(typeCon(Exp), typeCon(Any)),
                                               builtInAp(typeCon(Exp), typeCon(Any))))))
#define c2           0
#define c1           (c2+1)
#define stackDepth  0
{
   checkArgs("Pair");
   jump(altEntry(Pair));
}

defineAltEntry(Pair)
{
   /*
   Expr t1, t2;

#ifdef __GNUC__

//GCC bug

   if (localA(c1).env() == consEnv)
      t1 = localA(c1).expr();
   else
      t1 = Expr(localA(c1).expr(), localA(c1).env(), *consEnv, *consMSA);
   if (localA(c2).env() == consEnv)
      t2 = localA(c2).expr();
   else
      t2 = Expr(localA(c2).expr(), localA(c2).env(), *consEnv, *consMSA);
#else
   t1 = localA(c1).env() == consEnv
      ? localA(c1).expr()
      : Expr(localA(c1).expr(), localA(c1).env(), *consEnv, *consMSA);
   t2 = localA(c2).env() == consEnv
      ? localA(c2).expr()
      : Expr(localA(c2).expr(), localA(c2).env(), *consEnv, *consMSA);
#endif

   rPtr = consMSA->alloc(sizeof(Expr) * 2);
   ((Expr*)rPtr)[0] = t1;
   ((Expr*)rPtr)[1] = t2;
   rEnv = consEnv;
   rType = type(Pair);
*/
   jump(popLabel());
}

#undef stackDepth

/*----------------------------------------------------------------------------*/

declareBuiltInFn(fst);

// fst :: (Expr, Expr) -> *

defineBuiltInFn(fst,
                builtInAp2(ARROW,
                           builtInAp2(TUPLE_MIN,
                                      builtInAp(typeCon(Exp), typeCon(Any)),
                                      builtInAp(typeCon(Exp), typeCon(Any))),
                           typeCon(Any)))
#define pair_env     (sizeof(Env*))
#define pair         (pair_env+sizeof(Void*))
#define stackDepth  pair
{
   checkArgs("head");
   jump(altEntry(fst));
}

defineAltEntry(fst)
{
   Expr expr = local(pair, Expr*)[0];
   Env* env = local(pair_env, Env*);
   dropStack();
   enter(expr, env);
}

declareBuiltInFn(snd);

// snd :: (Expr, Expr) -> *

defineBuiltInFn(snd,
                builtInAp2(ARROW,
                           builtInAp2(TUPLE_MIN,
                                      builtInAp(typeCon(Exp), typeCon(Any)),
                                      builtInAp(typeCon(Exp), typeCon(Any))),
                           typeCon(Any)))
{
   checkArgs("head");
   jump(altEntry(snd));
}

defineAltEntry(snd)
{
   Expr expr = local(pair, Expr*)[1];
   Env* env = local(pair_env, Env*);
   dropStack();
   enter(expr, env);
}
