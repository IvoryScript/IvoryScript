/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showByteString.cpp
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
 *    Built in function to show a ByteString value.
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
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/trace.h"
#include "ivory/void.h"

// showByteString :: ByteString -> Void

defineBuiltInFn(showByteString,
                builtInAp2(ARROW,
                           typeCon(ByteString),
                           typeCon(Void)))
#define bs_          sizeof(ByteString)
#define stackDepth  bs_
{
   checkArgs("showByteString");
   jump(altEntry(showByteString));
}

defineAltEntry(showByteString)
{
   ByteString bs = local(bs_, ByteString);
   Char* buf;
   if ((buf = (char*)malloc(bs->_len * 3 + 1)) != NULL)
   {
      char* ptr = buf;
      for (UInt i = 0; i < bs->_len; i++)
         ptr += sprintf(ptr, "%s%02x", i == 0 ? "" : " ", bs->_data[i]);
      *outStrm << buf;
      free(buf);
   }
   dropStack();
   returnVoid();
}

/*----------------------------------------------------------------------------*/

// instance Show ByteString where
//    show s = showByteString s

classMethodDefn_n(1, show, showByteString) {
//   pushB((ByteString)rPtr, ByteString);
   jump(altEntry(showByteString));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_ByteString,
               Show,
               typeCon(ByteString),
               builtInCons(methodDefn_1,
                           Nil));



