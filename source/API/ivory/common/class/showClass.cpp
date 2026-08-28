/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showClass.cpp
 *
 * Module:  Ivory class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    A class to show a value
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

#include "ivory/list.h"
#include "ivory/show.h"
#include "ivory/void.h"

// class Show a where
//    show      :: a -> Void

/*----------------------------------------------------------------------------*/

classMethodRef(showMethod, "show");

declareAltEntry(show);

declareEntry(dispatch_show);

classMethodDecl_n(1, show, entry(dispatch_show),
                  builtInAp2(ARROW,
                             fromName(builtInName(a)),
                             typeCon(Void)))
#define stackDepth  0
{
   checkArgs("show");
   jump(altEntry(show));
}

// Entry, skipping argument checks.

defineAltEntry(show)
{
   push(cell, Cell*);
   pushLabel(altEntry(show));
   Env* t_env = pop(Env*);
   Expr t = pop(Expr);              // Drop arg as no longer live
   emptyStack();
   enter(t, t_env);
}

// Special dispatch entry point

//defineAltEntry(show)
//{
//   jump(cellBody(*pop(Cell*), ClassMethod).dispatch(rType));
//}

defineEntry(dispatch_show)
{
   jump(toBody(classMethod(showMethod), ClassMethod).dispatch(rType));
}

/*----------------------------------------------------------------------------*/

defineClass(Nil,
            Show,
            a,
            builtInCons(methodDecl_1,
                        Nil),
            Nil);
