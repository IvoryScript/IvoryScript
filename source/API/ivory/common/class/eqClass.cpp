/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    eqClass.cpp
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
 *    Declaration of Eq class
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

#include "ivory/bool.h"
#include "ivory/class.h"
#include "ivory/list.h"

// class Eq a where
//    (=)  :: a -> a -> Bool
//    (¬=) :: a -> a -> Bool

/*----------------------------------------------------------------------------*/

classMethodRef(eqMethod, "(=)");

declareEntry(eq$);

declareEntry(eq$$);

declareEntry(dispatch_eq);

classOperatorDecl_n(1, "(=)", entry(dispatch_eq),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Bool))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(=)");
   jump(entry(eq$));
}

// Entry, skipping argument checks.

defineEntry(eq$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(eq$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(eq$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_eq)
{
   jump(toBody(classMethod(eqMethod), ClassMethod).dispatch(rType));
}


/*----------------------------------------------------------------------------*/

classMethodRef(nEqMethod, "(¬=)");

declareEntry(nEq$);
declareEntry(nEq$$);
declareEntry(dispatch_nEq);

classOperatorDecl_n(2, "(¬=)", entry(dispatch_nEq),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Bool))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(¬=)");
   jump(entry(nEq$));
}

// Entry, skipping argument checks.

defineEntry(nEq$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(nEq$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(nEq$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_nEq)
{
   jump(toBody(classMethod(nEqMethod), ClassMethod).dispatch(rType));
}

/*----------------------------------------------------------------------------*/

defineClass(Nil,
            Eq,
            a,
            builtInCons(methodDecl_1,
               builtInCons(methodDecl_2,
                  Nil)),
            Nil);
