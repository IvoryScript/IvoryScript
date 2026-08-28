/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    numClass.cpp
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
 *    Declaration of Num class
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

 // class (Eq a) => Num a where
 //    (+) :: a -> a -> a
 //    (-) :: a -> a -> a
 //    (*) :: a -> a -> a
 //    (/) :: a -> a -> a
 //    negate :: a -> a

 /*----------------------------------------------------------------------------*/

classMethodRef(addMethod, "(+)");

declareEntry(add$);
declareEntry(add$$);
declareEntry(dispatch_add);

classOperatorDecl_n(1, "(+)", entry(dispatch_add),
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
   checkArgs("(+)");
   jump(entry(add$));
}

// Entry, skipping argument checks.

defineEntry(add$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(add$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(add$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_add)
{
   jump(toBody(classMethod(addMethod), ClassMethod).dispatch(rType));
}

#undef y
#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

classMethodRef(subMethod, "(-)");

declareEntry(sub$);
declareEntry(sub$$);
declareEntry(dispatch_sub);

classOperatorDecl_n(2, "(-)", entry(dispatch_sub),
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
   checkArgs("(-)");
   jump(entry(sub$));
}

// Entry, skipping argument checks.

defineEntry(sub$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(sub$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(sub$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_sub)
{
   jump(toBody(classMethod(subMethod), ClassMethod).dispatch(rType));
}

#undef y
#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

classMethodRef(multMethod, "(*)");

declareEntry(mult$);
declareEntry(mult$$);
declareEntry(dispatch_mult);

classOperatorDecl_n(3, "(*)", entry(dispatch_mult),
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
   checkArgs("(*)");
   jump(entry(mult$));
}

// Entry, skipping argument checks.

defineEntry(mult$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(mult$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(mult$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_mult)
{
   jump(toBody(classMethod(multMethod), ClassMethod).dispatch(rType));
}

#undef y
#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

classMethodRef(divMethod, "(/)");

declareEntry(div$);
declareEntry(div$$);
declareEntry(dispatch_div);

classOperatorDecl_n(4, "(/)", entry(dispatch_div),
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
   checkArgs("(/)");
   jump(entry(div$));
}

// Entry, skipping argument checks.

defineEntry(div$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(div$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(div$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_div)
{
   jump(toBody(classMethod(addMethod), ClassMethod).dispatch(rType));
}

#undef y
#undef x
#undef x_env
#undef stackDepth

/*----------------------------------------------------------------------------*/

classMethodRef(negMethod, "negate");

declareEntry(neg$);
declareEntry(neg$$);
declareEntry(dispatch_neg);

classMethodDecl_n(5, negate, entry(dispatch_neg),
   builtInAp2(ARROW,
      fromName(builtInName(a)),
      fromName(builtInName(a))))

#define stackDepth   0
{
   checkArgs("negate");
   jump(entry(neg$));
}

// Entry, skipping argument checks.

defineEntry(neg$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(neg$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(neg$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_neg)
{
   jump(toBody(classMethod(negMethod), ClassMethod).dispatch(rType));
}

#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

declareClass(Eq);

defineClass(builtInCons(class_(Eq), Nil),
   Num,
   a,
   builtInCons(methodDecl_1,
      builtInCons(methodDecl_2,
         builtInCons(methodDecl_3,
            builtInCons(methodDecl_4,
               builtInCons(methodDecl_5,
                     Nil))))),
   Nil);
