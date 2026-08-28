/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertTxtByteString.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 July 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built in function to insert a byte stream into a text stream.
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
#include "ivory/char.h"
#include "ivory/env.h"
#include "ivory/streams.h"
#include "ivory/void.h"

static Char hexChar(Byte b) {
   return (Char)(b < 10 ? '0' + b : 'a' + b - 10);
}

// insertTxtByteString :: OutputStream Char -> ByteString -> Void

defineBuiltInFn_2_args(insertTxtByteString,
   builtInAp(typeCon(OutputStream), typeCon(Char)), typeCon(ByteString), typeCon(Void),
   os, OutputStream_Char,
   bs, ByteString)

   for (UInt i = 0; i < bs->_len; i++) {
      if (i > 0)
         putChar(*os, *osEnv, ' ' n_vm);
      putChar(*os, *osEnv, hexChar(bs->_data[i] / 16) n_vm);
      putChar(*os, *osEnv, hexChar(bs->_data[i] % 16) n_vm);
   }

   drop_Stack(insertTxtByteString);
   returnVoid();
endBuiltInFn