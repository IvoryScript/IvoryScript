/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    FractionalClass.cpp
 *
 * Module:  Ivory class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Declaration of Fractional class
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

#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/machine.h"

// class (Num a) => Fractional a where
//    fromDouble :: Double -> a

/*----------------------------------------------------------------------------*/

classMethodRef(fromDoubleMethod, "fromDouble");

declareEntry(fromDouble$);
declareEntry(fromDouble$$);
declareEntry(dispatch_fromDouble);

declareTypeCon(Double);

classMethodDecl_n(1, fromDouble, entry(dispatch_fromDouble),
                  builtInAp2(ARROW,
                             typeCon(Double),
                             fromName(builtInName(a))))
#define x_env        sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("fromDouble");
   jump(entry(fromDouble$));
}

// Entry, skipping argument checks.

defineEntry(fromDouble$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(fromDouble$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(fromDouble$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_fromDouble)
{
   jump(toBody(classMethod(fromDoubleMethod), ClassMethod).dispatch(rType));
}

#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

declareClass(Num);

defineClass(builtInCons(class_(Num), Nil),
            Fractional,
            a,
            builtInCons(methodDecl_1,
                            Nil),
            Nil);
