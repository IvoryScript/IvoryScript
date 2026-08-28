/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    int.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Int natve data type
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

#ifndef IVORY_INT_H_DEFINED
#define IVORY_INT_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfInt sizeof(Int)

#define typeSpecInt Int

#define argRegInt rInt

declareTypeCon(Int);

declareType(Int);

#define stackFPSizeInt stackSlotSize(Int)

#define loadArgRegInt(name) Int name=rInt

#define loadArgInt(name,off) Int name=stack(off,Int)
#define storeArgInt(name,off) stack(off,Int)=name

#define argRegFromCellInt(s,c) rInt=cellBody(*cell,s).c

#define map_Int(name,srcEnv,dstEnv) name

#define returnInt(i){\
rInt=(i);\
jump(popLabel());}

// Transfer functions

extern Expr fromInt(Int val, MSA& msa);

extern Int toInt(Expr expr);

declareBuiltInFn(extractBinInt);

declareBuiltInFn(fetchInt);

declareBuiltInFn(insertBinInt);


declareBuiltInFn(showInt);

declareBuiltInFn(insertTxtInt);

// StrictCast class

declareBuiltInFn(fromBitsInt);

// Eq class

declareBuiltInFn(eqInt);

declareBuiltInFn(nEqInt);

// Ord class

declareBuiltInFn(compareInt);

declareBuiltInFn(ltInt);

declareBuiltInFn(ltEqInt);

declareBuiltInFn(gtEqInt);

declareBuiltInFn(gtInt);

// Num class

declareBuiltInFn(addInt);

declareBuiltInFn(subInt);

declareBuiltInFn(mulInt);

declareBuiltInFn(divInt);

declareBuiltInFn(modInt);

declareBuiltInFn(negInt);

// Bits class

declareBuiltInFn(andInt);

declareBuiltInFn(orInt);

declareBuiltInFn(xorInt);

declareBuiltInFn(lShiftInt);

declareBuiltInFn(rShiftInt);

declareBuiltInFn(notInt);



declareBuiltInFn(fact);

declareBuiltInFn(formatInt);

declareBuiltInFn(randomInt);

declareBuiltInFn(seedRandomInt);

#endif /* IVORY_INT_H_DEFINED */
