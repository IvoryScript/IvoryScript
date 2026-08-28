/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    double.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Double data type
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

#ifndef IVORY_DOUBLE_H_DEFINED
#define IVORY_DOUBLE_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfDouble sizeof(Double)

#define typeSpecDouble Double

#define argRegDouble rDouble

declareTypeCon(Double);

declareType(Double);

// Transfer functions

extern Cell* makeDouble(Double val, MSA& msa);

extern Double toDouble(Expr expr);

#define fromDouble(d,msa) (Expr(makeDouble(d,msa)))

#define stackFPSizeDouble stackSlotSize(Double)

#define loadArgRegDouble(name) Double name=rDouble

#define loadArgDouble(name,off) Double name=stack(off,Double)
#define storeArgDouble(name,off) stack(off,Double)=name

#define argRegFromCellDouble(s,c) rDouble=cellBody(*cell,s).c

#define map_Double(name,srcEnv,dstEnv) name

#define returnDouble(d){\
rDouble = d;\
jump(popLabel());}

declareBuiltInFn(extractBinDouble);

declareBuiltInFn(fetchDouble);

declareBuiltInFn(insertBinDouble);

declareBuiltInFn(showDouble);

declareBuiltInFn(insertTxtDouble);

// StrictCast class

declareBuiltInFn(fromIntDouble);

declareBuiltInFn(fromFloatDouble);

declareBuiltInFn(fromStringDouble);

// Eq class

declareBuiltInFn(eqDouble);

declareBuiltInFn(nEqDouble);

// Ord class

declareBuiltInFn(compareDouble);

declareBuiltInFn(ltDouble);

declareBuiltInFn(ltEqDouble);

declareBuiltInFn(gtEqDouble);

declareBuiltInFn(gtDouble);

// Num class

declareBuiltInFn(addDouble);

declareBuiltInFn(subDouble);

declareBuiltInFn(mulDouble);

declareBuiltInFn(negDouble);

// Fractional Classs

declareBuiltInFn(divDouble);

declareBuiltInFn(fromDoubleDouble);

// Maths functions

declareBuiltInFn(acos);

declareBuiltInFn(asin);

declareBuiltInFn(atan);

declareBuiltInFn(atan2);

declareBuiltInFn(cos);

declareBuiltInFn(cosh);

declareBuiltInFn(deg);

declareBuiltInFn(degrees);

declareBuiltInFn(exp);

declareBuiltInFn(log);

declareBuiltInFn(log10);

declareBuiltInFn(pow);

declareBuiltInFn(rad);

declareBuiltInFn(radians);

declareBuiltInFn(sin);

declareBuiltInFn(sinh);

declareBuiltInFn(sqrt);

declareBuiltInFn(tan);

declareBuiltInFn(tanh);


declareBuiltInFn(formatDouble);

#endif /* IVORY_DOUBLE_H_DEFINED */
