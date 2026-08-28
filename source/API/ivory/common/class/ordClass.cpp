/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ord.cpp
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
 *    Declaration of Ord class
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
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/ordering.h"

// class Ord a where
//    compare  :: a -> a -> Ordering
//    (<)      :: a -> a -> Bool
//    (<=)     :: a -> a -> Bool
//    (>=)     :: a -> a -> Bool
//    (>)      :: a -> a -> Bool

/*----------------------------------------------------------------------------*/

classMethodRef(compareMethod, "compare");

declareEntry(compare$);
declareEntry(compare$$);
declareEntry(dispatch_compare);

classOperatorDecl_n(1, "compare", entry(dispatch_compare),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Ordering))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("compare");
   jump(entry(compare$));
}

// Entry, skipping argument checks.

defineEntry(compare$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(compare$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(compare$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_compare)
{
   jump(toBody(classMethod(compareMethod), ClassMethod).dispatch(rType));
}

#undef y
#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

classMethodRef(ltMethod, "(<)");

declareEntry(lt$);
declareEntry(lt$$);
declareEntry(dispatch_lt);

classOperatorDecl_n(2, "(<)", entry(dispatch_lt),
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
   checkArgs("(<)");
   jump(entry(lt$));
}

// Entry, skipping argument checks.

defineEntry(lt$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(lt$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(lt$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_lt)
{
   jump(toBody(classMethod(ltMethod), ClassMethod).dispatch(rType));
}

/*----------------------------------------------------------------------------*/

classMethodRef(ltEqMethod, "(<=)");

declareEntry(ltEq$);
declareEntry(ltEq$$);
declareEntry(dispatch_ltEq);

classOperatorDecl_n(3, "(<=)", entry(dispatch_ltEq),
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
   checkArgs("(<=)");
   jump(entry(ltEq$));
}

// Entry, skipping argument checks.

defineEntry(ltEq$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(ltEq$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(ltEq$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_ltEq)
{
   jump(toBody(classMethod(ltEqMethod), ClassMethod).dispatch(rType));
}

/*----------------------------------------------------------------------------*/

classMethodRef(gtEqMethod, "(>=)");

declareEntry(gtEq$);
declareEntry(gtEq$$);
declareEntry(dispatch_gtEq);

classOperatorDecl_n(4, "(>=)", entry(dispatch_gtEq),
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
   checkArgs("(>=)");
   jump(entry(gtEq$));
}

// Entry, skipping argument checks.

defineEntry(gtEq$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(gtEq$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(gtEq$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_gtEq)
{
   jump(toBody(classMethod(gtEqMethod), ClassMethod).dispatch(rType));
}

/*----------------------------------------------------------------------------*/

classMethodRef(gtMethod, "(>)");

declareEntry(gt$);

declareEntry(gt$$);
declareEntry(dispatch_gt);

classOperatorDecl_n(5, "(>)", entry(dispatch_gt),
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
   checkArgs("(>)");
   jump(entry(gt$));
}

// Entry, skipping argument checks.

defineEntry(gt$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(gt$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(gt$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_gt)
{
   jump(toBody(classMethod(gtMethod), ClassMethod).dispatch(rType));
}

/*----------------------------------------------------------------------------*/

declareClass(Eq);

defineClass(builtInCons(class_(Eq), Nil),
            Ord,
            a,
            builtInCons(methodDecl_1,
               builtInCons(methodDecl_2,
                  builtInCons(methodDecl_3,
                     builtInCons(methodDecl_4,
                        builtInCons(methodDecl_5,
                           Nil))))),
            Nil);
