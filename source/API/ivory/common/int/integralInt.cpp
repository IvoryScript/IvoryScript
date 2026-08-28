/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    numInt.cpp
 *
 * Module:  Ivory common (int)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Integral instance methods for type Int.
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

#include "ivory/dyadicOp.h"
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/monadicOp.h"

#define dyadicOp_Int_Int_n(n,op,name,e)\
dyadicOp_n(n,op,name,Int,Int,Int,e)

// instance Integral Int where
//   (+) x y   = addInt  x y
//   (-) x y   = subInt  x y
//   (*) x y   = mulInt x y
//   (/) x y   = divInt x y
//   (mod) x y = modInt x y
//   negate x  = negInt  x

dyadicOp_Int_Int_n(1, "(+)",   add, x + y);

dyadicOp_Int_Int_n(2, "(-)",   sub, x - y);

dyadicOp_Int_Int_n(3, "(*)",   mul, x * y);

dyadicOp_Int_Int_n(4, "(/)",   div, x / y);

dyadicOp_Int_Int_n(5, "(mod)", mod, x & y);

#define monadicOpInt_Int_n(n,op,name,e)\
monadicOp_n(n,op,name,Int,Int,Int,e)

monadicOpInt_Int_n(6, "negate", neg, -x);

/*----------------------------------------------------------------------------*/

declareClass(Integral);

defineInstance(Integral_Int,
   Integral,
   typeCon(Int),
   builtInCons(methodDefn_1,
      builtInCons(methodDefn_2,
         builtInCons(methodDefn_3,
            builtInCons(methodDefn_4,
               builtInCons(methodDefn_5,
                  builtInCons(methodDefn_6,
                     Nil)))))));
