/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byteString.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with ByteString data type
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

#ifndef IVORY_BYTE_STRING_H_DEFINED
#define IVORY_BYTE_STRING_H_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/type.h"

declareTypeCon(ByteString);

declareType(ByteString);

#define MAX_BYTE_STRING_LENGTH	65536

typedef struct ByteString_tag
{
   size_t         _len;
   unsigned char  _data[MAX_BYTE_STRING_LENGTH];
}* ByteString;

#define sizeOfByteString sizeof(ByteString)

#define typeSpecByteString ByteString

#define argRegByteString rPtr

#define stackFPSizeByteString (stackSlotSize(ByteString)+stackSlotSize(Env))

#define loadArgRegByteString(name)\
Env* name##Env=rEnv;\
ByteString name=(ByteString)rPtr

#define loadArgByteString(name,off)\
Env* name##Env=stack(off+stackSlotSize(ByteString),Env*);\
ByteString name=stack((off),ByteString)

#define storeArgByteString(name,off)\
stack(off+stackSlotSize(ByteString),Env*)=name##Env;\
stack(off,ByteString)=name

#define argRegFromCellByteString(s,c)\
rPtr=cellBody(*cell,s).c;\
rEnv=cellEnv

#define mappedByteString(name,srcEnv,dstEnv) mapByteString(name,*(srcEnv),*(dstEnv))
// Transfer functions

extern Cell* fromByteString(const ByteString val, MSA& msa);

extern Cell* fromByteString(const unsigned char* data, size_t len, MSA& msa);

extern Void printByteString(const ByteString& byteString, ostream& os);

#define toByteString(e) ((ByteString)toCell(e).body())

extern ByteString mapByteString(ByteString str, const Env& srcEnv, Env& dstEnv);

declareBuiltInFn(mapByteString);

#define returnByteString(bs,env){\
rPtr=(bs);\
rEnv=(env);\
jump(popLabel());}

declareDataCon(ByteString);

declareBuiltInFn(bytesToString);

declareBuiltInFn(showByteString);

declareBuiltInFn(insertTxtByteString);

declareBuiltInFn(eqByteString);

declareBuiltInFn(nEqByteString);

// Seq class

declareBuiltInFn(lengthByteString);

declareBuiltInFn(getAtByteString);

declareBuiltInFn(concatByteString);

#endif /* IVORY_BYTE_STRING_H_DEFINED */
