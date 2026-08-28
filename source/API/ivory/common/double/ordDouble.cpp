/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ordDouble.cpp
 *
 * Module:  Ivory common (double)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Ord instance methods for type Double.
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
#include "ivory/double.h"
#include "ivory/dyadicOp.h"
#include "ivory/list.h"
#include "ivory/ordering.h"

#define dyadicOp_Double_Ordering_n(n,op,name,e)\
dyadicOp_n(n,op,name,Double,Ordering,Tag,e)

#define dyadicOp_Double_Bool_n(n,op,name,e)\
dyadicOp_n(n,op,name,Double,Bool,Tag,e)

// instance Ord Double where
//   compare x y = compareDouble x y
//   (<)     x y = ltDouble  x y
//   (<=)    x y = ltEqDouble x y
//   (>=)    x y = gtEqDouble  x y
//   (>)     x y = gtDouble x y

dyadicOp_Double_Ordering_n(1, "compare", compare,
                          x == y ? EQ_TAG : (x < y ? LT_TAG : GT_TAG));

dyadicOp_Double_Bool_n(2, "(<)",  lt,    x <  y);

dyadicOp_Double_Bool_n(3, "(<=)", ltEq,  x <= y);

dyadicOp_Double_Bool_n(4, "(>=)", gtEq,  x >= y);

dyadicOp_Double_Bool_n(5, "(>)",  gt,    x >  y);

/*----------------------------------------------------------------------------*/

declareClass(Ord);

defineInstance(Ord_Double,
   Ord,
   typeCon(Double),
   builtInCons(methodDefn_1,
      builtInCons(methodDefn_2,
         builtInCons(methodDefn_3,
            builtInCons(methodDefn_4,
               builtInCons(methodDefn_5,
                  Nil))))));

