/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    bits.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with bits data types
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

#ifndef IVORY_BITS_H_DEFINED
#define IVORY_BITS_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfBits sizeof(Bits)

#define typeSpecBits Bits

#define argRegBits rBits

declareTypeCon(Bits);

declareType(Bits);

#define stackFPSizeBits stackSlotSize(Bits)

#define loadArgRegBits(name) Bits name=rBits

#define loadArgBits(name,off) Bits name=stack(off,Bits)

#define loadArgBits(name,off) Bits name=stack(off,Bits)
#define storeArgBits(name,off) stack(off,Bits)=name

#define argRegFromCellBits(s,c) rBits=cellBody(*cell,s).c

#define map_Bits(name,srcEnv,dstEnv) name

#define returnBits(bits){\
rBits=(bits);\
jump(popLabel());}

// Transfer functions

extern Expr fromBits(Bits val, MSA& msa);

extern Bits toBits(Expr expr);

declareBuiltInFn(showBits);

declareBuiltInFn(insertTxtBits);

// StrictCast class

declareBuiltInFn(fromByteBits);

declareBuiltInFn(fromIntBits);

// Eq class

declareBuiltInFn(eqBits);

declareBuiltInFn(nEqBits);

// Bits class

declareBuiltInFn(andBits);

declareBuiltInFn(orBits);

declareBuiltInFn(xorBits);

declareBuiltInFn(lShiftBits);

declareBuiltInFn(rShiftBits);

declareBuiltInFn(notBits);

#endif /* IVORY_BITS_H_DEFINED */
