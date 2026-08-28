/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinString.cpp
 *
 * Module:  Ivory common (string)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 November 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of extractBinString function. Binary serialisation of a
 *    string.
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

#if (GARBAGE_COLELCTION==1)
#include <MSA_GC.h>
#else
#include <MSA.h>
#endif

#include "ivory/byte.h"
#include "ivory/streams.h"
#include "ivory/string.h"

defineBuiltInFn_1_arg(extractBinString,
   builtInAp(typeCon(InputStream), typeCon(Byte)), typeCon(String),
   is, InputStream_Byte)

   ExtractArchive ea(is->_inAct, isEnv);
   UInt len = ea.extractVLU();

#if (GARBAGE_COLELCTION==1)
   String res = static_cast<String>(((MSA_GC&)*consMSA).allocGC(len + 1));
#else
   String res = static_cast<String>(((MSA&)*consMSA).alloc(len + 1));
#endif

   for (UInt i = 0; i < len; i++)
      ea >> res[i];
   res[len] = '\0';

   returnString(res, consEnv);

endBuiltInFn
