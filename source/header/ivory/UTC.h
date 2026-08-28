/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    UTC.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with UTC data type.
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
 *
 */

#ifndef IVORY_UTC_H_DEFINED
#define IVORY_UTC_H_DEFINED

#ifndef _WIN32_WCE
   #include <time.h>
#else
   #include <WinCE/time.h>
#endif

#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

typedef time_t UTC;

#define sizeOfUTC sizeof(UTC)

#define typeSpecUTC UTC

#define argRegUTC rUTC

declareTypeCon(UTC);

declareType(UTC);

#define stackFPSizeUTC stackSlotSize(UTC)

// Transfer functions

extern Expr fromUTC(UTC val, MSA& msa);

#define loadArgRegUTC(name) UTC name=rUTC

#define loadArgUTC(name,off) UTC name=stack(off,UTC)
#define storeArgUTC(name,off) stack(off,UTC)=name

#define argRegFromCellUTC(s,c) rUTC=cellBody(*cell,s).c

#define map_UTC(name,srcEnv,dstEnv) name

#define returnUTC(utc){\
rUTC = utc;\
jump(popLabel());}

declareBuiltInFn(showUTC);

declareBuiltInFn(insertTxtUTC);

declareBuiltInFn(time);

declareBuiltInFn(timeDiff);

// Eq class

declareBuiltInFn(eqUTC);

declareBuiltInFn(nEqUTC);

// Ord class

declareBuiltInFn(compareUTC);

declareBuiltInFn(ltUTC);

declareBuiltInFn(ltEqUTC);

declareBuiltInFn(gtEqUTC);

declareBuiltInFn(gtUTC);


declareBuiltInFn(formatUTC);

#endif /* IVORY_UTC_H_DEFINED */

