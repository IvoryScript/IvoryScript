/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    float.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Float native data type
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

#ifndef IVORY_FLOAT_H_DEFINED
#define IVORY_FLOAT_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfFloat sizeof(Float)

#define typeSpecFloat Float

#define argRegFloat rFloat

declareTypeCon(Float);

declareType(Float);

// Transfer functions

extern Cell* makeFloat(Float val, MSA& msa);

extern Float toFloat(Expr expr);

#define fromFloat(f,msa) (Expr(makeFloat(f,msa)))

#define stackFPSizeFloat stackSlotSize(Float)

#define loadArgRegFloat(name) Float name=rFloat

#define loadArgFloat(name,off) Float name=stack(off,Float)
#define storeArgFloat(name,off) stack(off,Float)=name

#define argRegFromCellFloat(s,c) rFloat=cellBody(*cell,s).c

#define map_Float(name,srcEnv,dstEnv) name

#define returnFloat(d){\
rFloat = d;\
jump(popLabel());}

declareBuiltInFn(extractBinFloat);

declareBuiltInFn(insertBinFloat);

declareBuiltInFn(showFloat);

declareBuiltInFn(insertTxtFloat);

// StrictCast class

declareBuiltInFn(fromIntFloat);

declareBuiltInFn(fromStringFloat);

// Eq class

declareBuiltInFn(eqFloat);

declareBuiltInFn(nEqFloat);

// Ord class

declareBuiltInFn(compareFloat);

declareBuiltInFn(ltFloat);

declareBuiltInFn(ltEqFloat);

declareBuiltInFn(gtEqFloat);

declareBuiltInFn(gtFloat);

// Num class

declareBuiltInFn(addFloat);

declareBuiltInFn(subFloat);

declareBuiltInFn(mulFloat);

declareBuiltInFn(negFloat);

// Fractional Classs

declareBuiltInFn(divFloat);

declareBuiltInFn(fromDoubleFloat);


declareBuiltInFn(formatFloat);


#endif /* IVORY_FLOAT_H_DEFINED */
