/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    fractionalFloat.cpp
 *
 * Module:  Ivory common (float)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Fractional instance methods for type Float.
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
#include "ivory/float.h"
#include "ivory/list.h"
#include "ivory/monadicOp.h"

// instance Fractional Float where
//   (/) x y  = divFloat x y

#define dyadicOp_Float_Float_n(n,op,name,e)\
dyadicOp_n(n,op,name,Float,Float,Float,e)

dyadicOp_Float_Float_n(1, "(/)", div, x / y);

/*----------------------------------------------------------------------------*/

declareClass(Fractional);

defineInstance(Fractional_Float,
   Fractional,
   typeCon(Float),
   builtInCons(methodDefn_1,
      Nil));

