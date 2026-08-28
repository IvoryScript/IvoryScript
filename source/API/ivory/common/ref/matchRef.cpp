/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    matchRef.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Evaluates the top two items on the stack.  Tests that they are
 *    both of type Ref and compares them
 *
 * Method:
 *
 *    matchRef r1 r2 = case r1 of
 *                         Ref #r1 ->
 *                            case r2 of
 *                               Ref #r2 -> #r1 = #r2
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

#include "ivory/bool.h"
#include "ivory/ref.h"

// matchRef :: Expr -> Expr -> Bool

defineBuiltInFn(matchRef,
                builtInAp2(ARROW,
                           typeCon(Expr),
                           builtInAp2(ARROW,
                                      typeCon(Expr),
                                      typeCon(Bool))))
#define r2           0
#define r1           (r2 + 1)
#define stackDepth  0
{
   checkArgs("matchRef");
   jump(altEntry(matchRef));
}

declareLabel(matchRef_l1);

defineAltEntry(matchRef)
{
   pushLabel(label(matchRef_l1));
   emptyStack();
//   enterLocal(r1);
   jump(NULL);
}

declareLabel(matchRef_l2);

defineLabel(matchRef_l1)
{
   Ref r = checkRef();
   push(rEnv, Env*);
   push(r, Ref);
   pushLabel(label(matchRef_l2));
   emptyStack();
//   enterLocal(r2);
   jump(NULL);
}

defineLabel(matchRef_l2)
{
/*
   Ref r2_ = checkRef();
   Ref r1_ = pop(Ref);
   Env* r1_env = pop(Env*);

   if (r1_env == rEnv && r1_ == r2_)
   {

      returnTrue();
   }
   else
   {

      returnFalse();
   }
*/
   jump(NULL);
}
