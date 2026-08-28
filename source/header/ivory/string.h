/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    String.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Ivory 'String' native data type.
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

#ifndef IVORY_STRING_H_DEFINED
#define IVORY_STRING_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/env.h"
#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfString sizeof(String)

#define typeSpecString String

#define argRegString rPtr

declareTypeCon(String);

declareType(String);

// Transfer functions

extern Expr fromString(const char* val, MSA& msa);

extern Expr fromString(const char* val, size_t len, MSA& msa);

#define toString(e) ((String)toCell(e).body())

#define stackFPSizeString (stackSlotSize(String)+stackSlotSize(Env))

#define loadArgRegString(name)\
Env* name##Env=rEnv;\
String name=(String)rPtr

#define loadArgString(name,off)\
Env* name##Env=stack(off+stackSlotSize(String),Env*);\
String name=stack((off),String)

#define storeArgString(name,off) stack(off,String)=name

#define argRegFromCellString(s,c)\
rPtr=cellBody(*cell,s).c;\
rEnv=cellEnv

#define map_String(name,srcEnv,dstEnv) mapString$(name,*(srcEnv),*(dstEnv))

#define returnString(s,env){\
rPtr=(Void*)(s);\
rEnv=(env);\
jump(popLabel());}

extern String mapString$(String str, const Env& srcEnv, Env& dstEnv);

declareBuiltInFn(mapString);

#if (GARBAGE_COLLECTION==1)
declareBuiltInFn(markString_GC);
#endif

declareBuiltInFn(extractBinString);

declareBuiltInFn(insertBinString);

declareBuiltInFn(mutableString);

declareBuiltInFn(showString);

declareBuiltInFn(insertTxtString);

// Eq class

declareBuiltInFn(eqString);

declareBuiltInFn(nEqString);

// Ord class

declareBuiltInFn(compareString);

declareBuiltInFn(ltString);

declareBuiltInFn(ltEqString);

declareBuiltInFn(gtEqString);

declareBuiltInFn(gtString);

// Seq class

declareBuiltInFn(lengthString);

declareBuiltInFn(getAtString);

declareBuiltInFn(putAtString);

declareBuiltInFn(concatString);



declareBuiltInFn(formatString);

declareBuiltInFn(hashString);

declareBuiltInFn(isSubstring);

declareBuiltInFn(lowercaseString);

declareBuiltInFn(reverseString);

declareBuiltInFn(substring);

declareBuiltInFn(uppercaseString);


#endif /* IVORY_STRING_H_DEFINED */
