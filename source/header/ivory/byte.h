/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byte.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Byte native data type
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

#ifndef IVORY_BYTE_H_DEFINED
#define IVORY_BYTE_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfByte sizeof(Byte)

declareTypeCon(Byte);

declareType(Byte);

#define stackFPSizeByte stackSlotSize(Byte)

#define loadArgRegByte(name) Byte name=(Byte)rByte

#define loadArgByte(name,off) Byte name=stack(off,Byte)
#define storeArgByte(name,off) stack(off,Byte)=name

#define argRegFromCellByte(s,c) rByte=cellBody(*cell,s).c

#define returnByte(i){\
rByte=(i);\
jump(popLabel());}

// Transfer functions

extern Expr fromByte(Byte val, MSA& msa);

extern Byte toByte(Expr expr);

declareBuiltInFn(fromIntByte);

declareBuiltInFn(showByte);

// StrictCast class

declareBuiltInFn(fromBitsByte);


#endif /* IVORY_BYTE_H_DEFINED */
