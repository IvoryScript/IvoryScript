/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    lowercaseString.cpp
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
 *    Implementation of lowercaseString function
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

// lowercaseString :: String -> String

#if (GARBAGE_COLLECTION==1)
// No active roots, arbitrary association label
static BuiltInFrameDescr frameDescr1(altEntry(lowercaseString), 0, 0, NULL);
#endif

defineBuiltInFn_1_arg(lowercaseString,
   typeCon(String), typeCon(String),
   s, String)
   UInt len = strlen(s) + 1;

#if (GARBAGE_COLLECTION ==1)
   String res = (String)memAlloc_GC(len, altEntry(lowercaseString), (MSA_GC&)*consMSA n_vm);
#else
   String res = (String)consMSA->alloc(len);
#endif

   for (UInt i = 0; i < len; i++)
      res[i] = tolower(s[i]);
   returnString(res, consEnv);
endBuiltInFn
