/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    bits.cpp
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
 *    Declaration of Bitwise class
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
#include "ivory/bits.h"
#include "ivory/list.h"

// class Bitwise a where
//    (#&) :: a -> a -> a
//    (#|) :: a -> a -> a
//    (#^) :: a -> a -> a
//    (<<) :: a -> Bits -> a
//    (>>) :: a -> Bits -> a
//    (#¬) :: a -> a

/*----------------------------------------------------------------------------*/

classMethodRef(andMethod, "(#&)");

declareEntry(and$);
declareEntry(and$$);
declareEntry(dispatch_and);

classOperatorDecl_n(1, "(#&)", entry(dispatch_and),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Bits))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(#&)");
   jump(entry(and$));
}

// Entry, skipping argument checks.

defineEntry(and$) {
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(and$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(and$$) {
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_and)
{
   jump(toBody(classMethod(andMethod), ClassMethod).dispatch(rType));
}

#undef stackDepth
#undef x
#undef x_env
#undef y
#undef y_env

/*----------------------------------------------------------------------------*/

classMethodRef(orMethod, "(#|)");

declareEntry(or$);
declareEntry(or$$);
declareEntry(dispatch_or);

classOperatorDecl_n(2, "(#|)", entry(dispatch_or),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Bits))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(#|)");
   jump(entry(or$));
}

// Entry, skipping argument checks.

defineEntry(or$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(or$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(or$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_or)
{
   jump(toBody(classMethod(orMethod), ClassMethod).dispatch(rType));
}

#undef stackDepth
#undef x
#undef x_env
#undef y
#undef y_env

/*----------------------------------------------------------------------------*/

classMethodRef(xorMethod, "(#^)");

declareEntry(xor$);
declareEntry(xor$$);
declareEntry(dispatch_xor);

classOperatorDecl_n(3, "(#^)", entry(dispatch_xor),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Bits))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(#^)");
   jump(entry(xor$));
}

// Entry, skipping argument checks.

defineEntry(xor$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(xor$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(xor$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_xor)
{
   jump(toBody(classMethod(xorMethod), ClassMethod).dispatch(rType));
}

#undef stackDepth
#undef x
#undef x_env
#undef y
#undef y_env

/*----------------------------------------------------------------------------*/

classMethodRef(lShiftMethod, "(<<)");

declareEntry(lShift$);
declareEntry(lShift$$);
declareEntry(dispatch_lShift);

classOperatorDecl_n(4, "(<<)", entry(dispatch_lShift),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Bits))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(<<)");
   jump(entry(lShift$));
}

// Entry, skipping argument checks.

defineEntry(lShift$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(lShift$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(lShift$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_lShift)
{
   jump(toBody(classMethod(lShiftMethod), ClassMethod).dispatch(rType));
}

#undef stackDepth
#undef x
#undef y

/*----------------------------------------------------------------------------*/

classMethodRef(rShiftMethod, "(>>)");

declareEntry(rShift$);
declareEntry(rShift$$);
declareEntry(dispatch_rShift);

classOperatorDecl_n(5, "(>>)", entry(dispatch_rShift),
                    builtInAp2(ARROW,
                               fromName(builtInName(a)),
                               builtInAp2(ARROW,
                                          fromName(builtInName(a)),
                                          typeCon(Bits))))
#define y_env        sizeof(Env*)
#define y            y_env+sizeof(Expr)
#define x_env        y+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(>>)");
   jump(entry(rShift$));
}

// Entry, skipping argument checks.

defineEntry(rShift$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(rShift$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(rShift$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_rShift)
{
   jump(toBody(classMethod(rShiftMethod), ClassMethod).dispatch(rType));
}

#undef stackDepth
#undef x
#undef x_env
#undef y
#undef y_env

/*----------------------------------------------------------------------------*/

classMethodRef(notMethod, "(#¬)");

declareEntry(not$);
declareEntry(not$$);
declareEntry(dispatch_not);

classOperatorDecl_n(6, "(#¬)", entry(dispatch_not),
                  builtInAp2(ARROW,
                             fromName(builtInName(a)),
                             fromName(builtInName(a))))
#define x_env        sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("(#¬)");
   jump(entry(not$));
}

// Entry, skipping argument checks.

defineEntry(not$)
{
   push(cell, Cell*);
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   pushLabel(entry(not$$));
   emptyStack();
   enter(t, t_env);
}

defineEntry(not$$)
{
   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
}

defineEntry(dispatch_not)
{
   jump(toBody(classMethod(notMethod), ClassMethod).dispatch(rType));
}

#undef x
#undef stackDepth

/*----------------------------------------------------------------------------*/

defineClass(Nil,
            Bitwise,
            a,
            builtInCons(methodDecl_1,
               builtInCons(methodDecl_2,
                  builtInCons(methodDecl_3,
                     builtInCons(methodDecl_4,
                        builtInCons(methodDecl_5,
                           builtInCons(methodDecl_6,
                              Nil)))))),
            Nil);
