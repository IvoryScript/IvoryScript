/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    fractionalDouble.cpp
 *
 * Module:  Ivory common  (double)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Fractional instance methods for type Double.
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

#include "ivory/double.h"
#include "ivory/dyadicOp.h"
#include "ivory/list.h"
#include "ivory/monadicOp.h"

 // instance Fractional Double where
 //   (/) x y  = divDouble x y

#define y_           sizeof(Double)
#define x_           (y_+sizeof(Double))
#define stackDepth   x_

#define dyadicOp_Double_Double_n(n,op,name,e)\
dyadicOp_n(n,op,name,Double,Double,Double,e)

dyadicOp_Double_Double_n(1, "(/)", div, x / y);

/*----------------------------------------------------------------------------*/

declareClass(Fractional);

defineInstance(Fractional_Double,
   Fractional,
   typeCon(Double),
   builtInCons(methodDefn_1,
      Nil));


