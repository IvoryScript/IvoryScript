/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    seqString.cpp
 *
 * Module:  Ivory common (string)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Seq instance methods for type String.
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
#include "ivory/char.h"
#include "ivory/class.h"

#if (GARBAGE_COLLECTION==1)
#include <ivory/frameDescr.h>
#endif

#include "ivory/int.h"
#include "ivory/list.h"

#if (GARBAGE_COLLECTION==1)
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/name.h"
#include "ivory/string.h"
#include "ivory/void.h"

#define tX String
#define tY String

// lengthString :: String -> Int

defineBuiltInFn(lengthString,
                builtInAp2(ARROW,
                           typeCon(String),
                           typeCon(Int)))
#define stackDepth   0
{
   checkArgs("lengthString");
   jump(altEntry(lengthString));
}

defineAltEntry(lengthString) {
   Int length = strlen((String)rPtr);
   dropStack();
   returnInt(length);
}

#undef stackDepth
#undef s
#undef s_env

/*----------------------------------------------------------------------------*/

// getAtString :: String -> Int -> Char

defineBuiltInFn(getAtString,
                builtInAp2(ARROW,
                           typeCon(String),
                           builtInAp2(ARROW,
                                      typeCon(Int),
                                      typeCon(Char))))
#define i            sizeof(Int)
#define stackDepth   i
{
#ifdef CHECK_ARGS
   if (depth() < stackDepth)
      runTimeError("invalid application to getAtString");
#endif
   jump(altEntry(getAtString));
}

defineAltEntry(getAtString) {
   if (local(i, Int) < 0 || local(i, Int) >= (Int)strlen((String)rPtr))
      runTimeError("getAtString: index out of range");
   Char res = ((String)rPtr)[local(i, Int)];
   dropStack();
   returnChar(res);
}

#undef stackDepth
#undef s
#undef s_env
#undef i

/*----------------------------------------------------------------------------*/

// putAtString :: String -> Int -> Char -> Void

defineBuiltInFn(putAtString,
   builtInAp2(ARROW,
      typeCon(String),
      builtInAp2(ARROW,
         typeCon(Int),
         builtInAp2(ARROW,
            typeCon(Char),
            typeCon(Void)))))
#define c            sizeof(Char)+(4-sizeof(Char))
#define i            (c+sizeof(Int))
#define stackDepth   i
{
#ifdef CHECK_ARGS
   if (depth() < stackDepth)
      runTimeError("invalid application to putAtString");
#endif
   jump(altEntry(putAtString));
}

defineAltEntry(putAtString) {
   ((String)rPtr)[local(i, Int)] = local(c, Char);
   dropStack();
   returnVoid();
}

#undef stackDepth
#undef i
#undef c

/*----------------------------------------------------------------------------*/

// concatString :: String -> String -> String

#if (GARBAGE_COLLECTION==1)
// No active roots, arbitrary association label
static BuiltInFrameDescr frameDescr1(altEntry(concatString), 0, 0, NULL);
#endif

defineBuiltInFn_2_args(concatString,
   typeCon(String), typeCon(String), typeCon(String),
   x, String,
   y, String)
   size_t l1 = strlen(x);
   size_t l2 = strlen(y);

   drop_Stack(concatString);

#if (GARBAGE_COLLECTION ==1)
   String res = (String)memAlloc_GC(l1 + l2 + 1, altEntry(concatString), (MSA_GC&)*consMSA n_vm);
#else
   String res = (String)consMSA->alloc(l1 + l2 + 1);
#endif

   memcpy(res, x, l1);
   memcpy(res + l1, y, l2 + 1);
   returnString(res, consEnv);
endBuiltInFn

/*----------------------------------------------------------------------------*/

// reverseString :: String -> String

#if (GARBAGE_COLLECTION==1)
// No active roots, arbitrary association label
static BuiltInFrameDescr frameDescr2(altEntry(reverseString), 0, 0, NULL);
#endif

defineBuiltInFn_1_arg(reverseString,
   typeCon(String), typeCon(String),
   s, String)
   UInt len = strlen(s);

#if (GARBAGE_COLLECTION ==1)
   String res = (String)memAlloc_GC(len + 1, altEntry(reverseString), (MSA_GC&)*consMSA n_vm);
#else
   String res = (String)consMSA->alloc(len + 1);
#endif

   for (UInt i = 0; i < len; i++)
      res[len - i - 1] = s[i];
   res[len] = s[len];
   returnString(res, consEnv);
endBuiltInFn
