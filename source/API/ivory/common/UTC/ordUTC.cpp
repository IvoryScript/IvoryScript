/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ordUTC.cpp
 *
 * Module:  Ivory common (UTC)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Ord instance methods for type UTC.
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
#include "ivory/bool.h"
#include "ivory/list.h"
#include "ivory/ordering.h"
#include "ivory/UTC.h"

#define dyadicOp_UTC_Ordering_n(n,op,name,e)\
dyadicOp_n(n,op,name,UTC,Ordering,Tag,e)

#define dyadicOp_UTC_Bool_n(n,op,name,e)\
dyadicOp_n(n,op,name,UTC,Bool,Tag,e)

//   compare x y = compareUTC x y
//   (<)  x y =    ltUTC  x y
//   (<=) x y =    ltEqUTC x y
//   (>=) x y =    gtEqUTC x y
//   (>)  x y =    gtUTC x y

dyadicOp_UTC_Ordering_n(1, "compare",  compare,
                        x == y ? EQ_TAG : (x < y ? LT_TAG : GT_TAG));

dyadicOp_UTC_Bool_n(2, "(<)",  lt,   x <  y);

dyadicOp_UTC_Bool_n(3, "(<=)", ltEq, x <= y);

dyadicOp_UTC_Bool_n(4, "(>=)", gtEq, x >= y);

dyadicOp_UTC_Bool_n(5, "(>)",  gt,   x >  y);

/*----------------------------------------------------------------------------*/

declareClass(Ord);

defineInstance(Ord_UTC,
   Ord,
   typeCon(UTC),
   builtInCons(methodDefn_1,
      builtInCons(methodDefn_2,
         builtInCons(methodDefn_3,
            builtInCons(methodDefn_4,
               builtInCons(methodDefn_5,
                  Nil))))));
