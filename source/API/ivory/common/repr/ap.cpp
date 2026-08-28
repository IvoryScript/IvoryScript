/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ap.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of function application.
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

#include "ivory/machine.h"

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

declareLabel(enterAp);

static Void cellDestroyAp(Cell& cell_, Env& env, MSA& msa argN_VM) {
   fun(fromCell(cell_)).destroy(env, msa n_vm);
   arg(fromCell(cell_)).destroy(env, msa n_vm);
   msa.free(&cell_);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopyAp(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return mkAp(Expr(fun(fromCell(src)), srcEnv, env, msa n_vm),
               Expr(arg(fromCell(src)), srcEnv, env, msa n_vm), msa);
}


#undef localConst
#define localConst      

static CellInfo cellInfo_ap(AP, sizeof(Expr) * 2, label(enterAp), cellCopyAp,
   
#if (SERIALISATION==1)
                            NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                            NULL,
#endif   
   
                            cellDestroyAp, NULL);

Cell* mkBuiltInAp(Expr fun, Expr arg) {
//   Cell& cell_ = *new(max(sizeof(Expr) * 2, MIN_CLOSURE_SIZE), builtInMSA()) Cell(&cellInfo$ap);
   Cell& cell_ = *new(max(sizeof(Expr) * 2, MIN_CLOSURE_SIZE), builtInMSA()) Cell(AP);
   Expr* ptr = (Expr*)cell_.body();
   ptr[0] = fun; ptr[1] = arg;
   return &cell_;
}

Cell* mkAp(Expr fun, Expr arg, MSA& msa) {
   Cell& cell_ = *new(max(sizeof(Expr) * 2, MIN_CLOSURE_SIZE), msa) Cell(AP);
   Expr* ptr = (Expr*)cell_.body();
   ptr[0] = fun; ptr[1] = arg;
   return &cell_;
}

Cell* mkReducibleAp(Expr fun, Expr arg, MSA& msa) {
   Cell& cell_ = *new(max(sizeof(Expr) * 2, MIN_CLOSURE_SIZE), msa) Cell(&cellInfo_ap);
   Expr* ptr = (Expr*)cell_.body();
   ptr[0] = fun; ptr[1] = arg;
   return &cell_;
}

// Update in place

// Note that expressions in an ADS are not shared.  As all IvoryScript application
// closures are currently updatable, a non-local entry results in a copy prior to
// reduction.

defineLabel(enterAp) {
   if (cellEnv != consEnv) {
      cell = mkReducibleAp(Expr(fun(cell), *cellEnv, *consEnv, *consMSA n_vm),
                           Expr(arg(cell), *cellEnv, *consEnv, *consMSA n_vm), *consMSA);

      cellEnv = consEnv;
   }

   saveStackBase();
   push(cell, Cell*);
   emptyStack();
   push(arg(cell), Expr);
   Expr expr = fun(cell);
   while (formOf(expr) == AP) {
      push(arg(expr), Expr);
      expr = fun(expr);
   };
   if (!isPtr(expr))
      enter(expr, cellEnv);
   else {
      cell = expr;
      jump(((CellInfo*)(toCell(expr).tag()))->entry_());
   }
}
