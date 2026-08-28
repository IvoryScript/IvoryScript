/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    closure.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Functionality related to closure update.
 *
 * Notes:
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

declareLabel(enterInd);

defineLabel(enterInd) {

// Use locals due to semantics of enter macro

//   Expr expr = toBody(cell, EnvExpr).expr();
//   Env* env = toBody(cell, EnvExpr).env();
//   enter(expr, env);
   jump(NULL);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopyClosure(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return Expr(&src, srcEnv, env, msa n_vm);
}

#undef localConst
#define localConst

static CellInfo infoInd(INDIRECT, sizeof(Cell*), label(enterInd),
                        cellCopyClosure, NULL, NULL);
