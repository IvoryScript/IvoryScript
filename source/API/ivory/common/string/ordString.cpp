/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ordString.cpp
 *
 * Module:  Ivory common (string)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Ord instance methods for type String.
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

#include <string.h>
#include "ivory/bool.h"
#include "ivory/dyadicOp.h"
#include "ivory/list.h"
#include "ivory/ordering.h"
#include "ivory/string.h"

#define dyadicOp_String_Ordering_n(n,op,name,e)\
dyadicOp_n(n,op,name,String,Ordering,Tag,e)

#define dyadicOp_String_Bool_n(n,op,name,e)\
dyadicOp_n(n,op,name,String,Bool,Tag,e)

//   compare x y = compareString x y
//   (<)  x y    = ltString  x y
//   (<=) x y    = ltEqString x y
//   (>=) x y    = gtEqString  x y
//   (>)  x y    = gtString x y

static Tag stringOrd(const String x, const String y)
{
   int res = strcmp(x, y);
   return res == 0 ? EQ_TAG : (res < 0 ? LT_TAG : GT_TAG);
}

dyadicOp_String_Ordering_n(1, "compare",  compare, stringOrd(x, y));

dyadicOp_String_Bool_n(2, "(<)",  lt,   strcmp(x, y) <  0);

dyadicOp_String_Bool_n(3, "(<=)", ltEq, strcmp(x, y) <= 0);

dyadicOp_String_Bool_n(4, "(>=)", gtEq, strcmp(x, y) >= 0);

dyadicOp_String_Bool_n(5, "(>)",  gt,   strcmp(x, y) >  0);

/*----------------------------------------------------------------------------*/

declareClass(Ord);

defineInstance(Ord_String,
   Ord,
   typeCon(String),
   builtInCons(methodDefn_1,
      builtInCons(methodDefn_2,
         builtInCons(methodDefn_3,
            builtInCons(methodDefn_4,
               builtInCons(methodDefn_5,
                  Nil))))));
