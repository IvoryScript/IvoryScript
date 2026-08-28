/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    integralClass.cpp
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
 *    Declaration of Integral class
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

// class (Num a) => Integral a where
//    (mod) :: a -> a -> a

/*----------------------------------------------------------------------------*/

classMethodRef(modMethod, "(mod)");

declareEntry(mod$);

declareEntry(mod$$);
declareEntry(dispatch_mod);

classOperatorDecl_n(1, "(mod)", entry(dispatch_mod),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          fromName(builtInName(a)))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(mod)");
   jump(entry(mod$));
}

// Entry, skipping argument checks.

defineEntry(mod$) {
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(mod$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(mod$$) {
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_mod) {
   jump(toBody(classMethod(modMethod), ClassMethod).dispatch(rType));
}

#undef y
#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

declareClass(Num);

defineClass(builtInCons(class_(Num), Nil),
            Integral,
            a,
            builtInCons(methodDecl_1,
                            Nil),
            Nil);
