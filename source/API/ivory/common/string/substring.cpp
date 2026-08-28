/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    substring.cpp
 *
 * Module:  Ivory common (string)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of substring function
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

#if (GARBAGE_COLLECTION==1)
#include <ivory/frameDescr.h>
#endif

#include "ivory/int.h"

#if (GARBAGE_COLLECTION==1)
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/string.h"

// substring :: String -> Int -> Int -> String

#if (GARBAGE_COLLECTION==1)
// No active roots, arbitrary association label
static BuiltInFrameDescr frameDescr(altEntry(substring), 0, 0, NULL);
#endif

defineBuiltInFn(substring,
                builtInAp2(ARROW,
                           typeCon(String),
                           builtInAp2(ARROW,
                                      typeCon(Int),
                                      builtInAp2(ARROW,
                                                 typeCon(Int),
                                                 typeCon(String)))))
#define n_           sizeof(Int)
#define p_           n_+sizeof(Int)
#define stackDepth  p_
{
   checkArgs("substring");
   jump(altEntry(substring));
}

defineAltEntry(substring) {
   UInt len = strlen(String(rPtr));
   UInt p = local(p_, Int);
   UInt n = local(n_, Int);
   if (p < 0 || p >= len || n < 0)
      n = 0;
   else
      if (p + n > len)
         n = len - p;

#if (GARBAGE_COLLECTION ==1)
   String res = (String)memAlloc_GC(n + 1, altEntry(substring), (MSA_GC&)*consMSA n_vm);
#else
   String res = (String)consMSA->alloc(n + 1);
#endif

   for (UInt i = 0; i < n; i++)
      res[i] = (String(rPtr))[p + i];
   res[n] = '\0';
   dropStack();
   returnString(res, consEnv);
}
