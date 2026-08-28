/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    char.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Char native data type
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

#ifndef IVORY_CHAR_H_DEFINED
#define IVORY_CHAR_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfChar sizeof(Char)

#define typeSpecChar Char

#define argRegChar rChar

declareTypeCon(Char);

declareType(Char);

// Transfer functions

extern Cell* makeChar(Char val, MSA& msa);

#define stackFPSizeChar stackSlotSize(Char)

#define loadArgRegChar(name) Char name=rChar

#define loadArgChar(name,off) Char name=stack(off,Char)
#define storeArgChar(name,off) stack(off,Char)=name

#define argRegFromCellChar(s,c) rChar=cellBody(*cell,s).c

#define map_Char(name,srcEnv,dstEnv) name

#define returnChar(c){\
rChar = c;\
jump(popLabel());}

declareBuiltInFn(extractBinChar);

declareBuiltInFn(fetchChar);

declareBuiltInFn(insertBinChar);

declareBuiltInFn(showChar);

declareBuiltInFn(insertTxtChar);

// Eq class

declareBuiltInFn(eqChar);

declareBuiltInFn(nEqChar);

// Ord class

declareBuiltInFn(compareChar);

declareBuiltInFn(ltChar);

declareBuiltInFn(ltEqChar);

declareBuiltInFn(gtEqChar);

declareBuiltInFn(gtChar);


declareBuiltInFn(formatChar);


#endif /* IVORY_CHAR_H_DEFINED */
