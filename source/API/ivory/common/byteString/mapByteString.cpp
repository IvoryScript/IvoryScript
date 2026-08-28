/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    mapByteString.cpp
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built in Function to map a byte string from one environment to another.
 *
 * Call format:
 *
 *    res = mapByteString(str, srcEnv, dstEnv)
 *                     
 *       res      (ByteString)   - Returned byte string copied to destination
 *       str      (ByteString)   - ByteString to map
 *       srcEnv   (const Env&)   - Source environment
 *       dstEnv   (Env&)         - Destination environment
 *
 * Method:
 *
 *    Simply returns the given byte string if the source and destination
 *    environments match, otherwise copies the byte string.
 *
 * Errors:
 *
 *    Errors are signalled by a ERROR return value
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

#include "ivory/byteString.h"
#include "ivory/env.h"

declareTypeCon(ByteString);

ByteString mapByteString(const ByteString bs, const Env& srcEnv, Env& dstEnv) {
   if (&srcEnv == &dstEnv)
      return bs;
   ByteString res = (ByteString)dstEnv.msa().alloc(sizeof(ByteString_tag) +
      bs->_len - MAX_BYTE_STRING_LENGTH);
   memcpy(res->_data, bs->_data, bs->_len);
   res->_len = bs->_len;
   return res;
}

// mapByteString :: ByteString -> Env -> ByteString

defineBuiltInFn(mapByteString,
   builtInAp2(ARROW,
      typeCon(ByteString),
      builtInAp2(ARROW,
         typeCon(Env),
         typeCon(ByteString))))
#define dstEnv_      sizeof(Env*)
#define bsEnv_       (dstEnv_+sizeof(Env*))
#define bs_           (bsEnv_+sizeof(ByteString))
#define stackDepth   bs_
{
   checkArgs("mapByteString");
   jump(altEntry(mapByteString));
}

defineAltEntry(mapByteString) {
   ByteString bs =      local(bs_, ByteString);
   const Env& bsEnv =   *local(bsEnv_, Env*);
   Env* dstEnv =        local(dstEnv_, Env*);
   dropStack();
   returnByteString(mapByteString(bs, bsEnv, *dstEnv), dstEnv);
}

