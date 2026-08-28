/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    strictCastDouble.cpp
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
 *    Implementation of strict cast methods for Double
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
#include "ivory/double.h"
#include "ivory/float.h"
#include "ivory/int.h"
#include "ivory/monadicOp.h"
#include "ivory/string.h"

#define monadicOp_Int_Double_n(n,op,name,e)\
monadicOp_n(n,op,name,Int,Double,Double,e)

monadicOp_Int_Double_n(1, "fromInt", fromInt, (Double)x);


#define monadicOp_Float_Double_n(n,op,name,e)\
monadicOp_n(n,op,name,Float,Double,Double,e)

monadicOp_Float_Double_n(2, "fromFloat", fromFloat, (Double)x);

#define monadicOp_String_Double_n(n,op,name,e)\
monadicOp_n(n,op,name,String,Double,Double,e)

monadicOp_String_Double_n(3, "fromString", fromString, strtod(x, NULL));
