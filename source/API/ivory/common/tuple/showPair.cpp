/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showPair.cpp
 *
 * Module:  Ivory common (tuple)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of showPair function
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
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/trace.h"
#include "ivory/tuple.h"
#include "ivory/void.h"

// showPair :: (Pair (Exp*) (Exp *)) -> Void

defineBuiltInFn(showPair,
                builtInAp2(ARROW,
                           builtInAp2(PAIR,
                                      builtInAp(typeCon(Exp), typeCon(Any)),
                                      builtInAp(typeCon(Exp), typeCon(Any))),
                           typeCon(Void)))
#define pair_env  sizeof(Env*)
#define pair      (pair_env+sizeof(Void*))
#define stackDepth  pair
{
   checkArgs("showPair");
   jump(altEntry(showPair));
}

defineAltEntry(showPair)
{
   *outStrm << '(';
   printExpr((local(pair, Expr*))[0], *outStrm, *local(pair_env, Env*));
   *outStrm << ',';
   printExpr((local(pair, Expr*))[1], *outStrm, *local(pair_env, Env*));
   *outStrm << ')';
   dropStack();
   returnVoid();
}

/*----------------------------------------------------------------------------*/

// instance Show Pair where
//    show (x,y) = ...

classMethodDefn_n(1, show, showPair)
{
   push(rEnv, Env*);
   push(rPtr, Void*);
   jump(altEntry(showPair));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_Pair,
               Show,
               builtInAp2(PAIR,
                          builtInAp(typeCon(Exp), typeCon(Any)),
                          builtInAp(typeCon(Exp), typeCon(Any))),
               builtInCons(methodDefn_1,
                           Nil));
