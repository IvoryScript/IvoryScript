/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    bitwiseBits.cpp
 *
 * Module:  Ivory common (bits)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class instance methods for type Bits.
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
#include "ivory/bits.h"
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/monadicOp.h"

#define dyadicOp_Bits_Bits_n(n,op,name,e)\
dyadicOp_n_BUILT_IN_PAP(n,op,name,Bits,Bits,Bits,e)

// instance Bitwise Bits where
//   (\/) x y  = andBits x y
//   (/\) x y  = orBits x y
//   (<<) x n = lShiftBits x n
//   (>>) x n = rShiftBits x n

dyadicOp_Bits_Bits_n(1, "(#&)", and,     x & y);

dyadicOp_Bits_Bits_n(2, "(#|)", or,      x | y);

dyadicOp_Bits_Bits_n(3, "(#^)", xor,     x ^ y);

dyadicOp_Bits_Bits_n(4, "(<<)",  lShift,  x << y);

dyadicOp_Bits_Bits_n(5, "(>>)",  rShift,  x >> y);

#define monadicOpBits_Bits_n(n,op,name,e)\
monadicOp_n(n,op,name,Bits,Bits,Bits,e)

monadicOpBits_Bits_n(6, "(#¬)", not, ~x);

/*----------------------------------------------------------------------------*/

declareClass(Bitwise);

defineInstance(Bitwise_Bits,
   Bitwise,
   typeCon(Bits),
   builtInCons(methodDefn_1,
      builtInCons(methodDefn_2,
         builtInCons(methodDefn_3,
            builtInCons(methodDefn_4,
               builtInCons(methodDefn_5,
                  builtInCons(methodDefn_6,
                     Nil)))))));
