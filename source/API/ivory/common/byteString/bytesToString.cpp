/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    bytesToString.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of bytesToString function
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

#include <string.h>
#include "ivory/byteString.h"
#include "ivory/string.h"

// bytesToString :: ByteString -> String

defineBuiltInFn(bytesToString,
                builtInAp2(ARROW,
                           typeCon(ByteString),
                           typeCon(String)))
#define bs          sizeof(ByteString)
#define stackDepth  bs
{
   checkArgs("bytesToString");
   jump(altEntry(bytesToString));
}

defineAltEntry(bytesToString)
{
   String s = (Char*)consMSA->alloc(local(bs, ByteString)->_len + 1);
   strncpy(s, (const char*)local(bs, ByteString)->_data, local(bs, ByteString)->_len);
   s[local(bs, ByteString)->_len] = '\0';
   dropStack();
   returnString(s, consEnv);
}

