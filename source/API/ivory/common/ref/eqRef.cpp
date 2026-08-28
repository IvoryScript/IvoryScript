/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    eqRef.cpp
 *
 * Module:  Ivory common (ref)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Primitive function to compare two references
 *
 * Method:
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
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/ref.h"

// eqRef :: Expr -> Expr -> Bool

defineBuiltInFn(eqRef,
                builtInAp2(ARROW,
                           typeCon(Ref),
                           builtInAp2(ARROW,
                                      typeCon(Ref),
                                      typeCon(Bool))))
#define r2_env       sizeof(Env*)
#define r2           (r2_env+sizeof(Ref))
#define r1_env       (r2+sizeof(Env*))
#define r1           (r1_env+sizeof(Ref))
#define stackDepth  r1
{
   checkArgs("eqRef");
   jump(altEntry(eqRef));
}

defineAltEntry(eqRef)
{
   Bool res = local(r1_env, Env*) == local(r2_env, Env*) &&
              local(r1,     Ref)  == local(r2, Ref);
   dropStack();
   returnBool(res);
}

// instance Eq Ref where
//    (=) x y = eqRef x y

classOperatorDefn_n(1, "(=)", eqRef)
{
   push(rRef, Ref);
   jump(altEntry(eqRef));
}

// nEqRef :: Expr -> Expr -> Bool

defineBuiltInFn(nEqRef,
                builtInAp2(ARROW,
                           typeCon(Ref),
                           builtInAp2(ARROW,
                                      typeCon(Ref),
                                      typeCon(Bool))))
{
   checkArgs("nEqRef");
   jump(altEntry(nEqRef));
}

defineAltEntry(nEqRef)
{
   Bool res = local(r1_env, Env*) != local(r2_env, Env*) ||
              local(r1,     Ref)  != local(r2, Ref);
   dropStack();
   returnBool(res);
}

// instance Eq Ref where
//    (¬=) x y = nEqRef x y

classOperatorDefn_n(2, "(¬=)", nEqRef)
{
   push(rRef, Ref);
   jump(altEntry(nEqRef));
}

/*----------------------------------------------------------------------------*/

declareClass(Eq);

defineInstance(Eq_Ref,
               Eq,
               typeCon(Ref),
               builtInCons(methodDefn_1,
                  builtInCons(methodDefn_2,
                  Nil)));
