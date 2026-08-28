/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    formatChar.cpp
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
 *    built in function to format a character.
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

#include <stdio.h>
#include <string.h>

#include "ivory/char.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/string.h"

#if (GARBAGE_COLLECTION==1)
static const size_t formatCharFmtDepth = stackFPSizeString;
static const size_t formatCharFrameDepth = formatCharFmtDepth + stackFPSizeChar;

declareLabel(mark$formatChar$fmt);

static Void markBuiltInPAPArg_GC(const String& str, Env& env argN_VM) {
   if (&env.msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(str);
}

static BuiltInFrameDescrSlot formatCharSlots[] = {
   BuiltInFrameDescrSlot(label(mark$formatChar$fmt), NULL_NAME, NULL_TYPE)
};

static BuiltInFrameDescr frameDescr(altEntry(formatChar),
   formatCharFrameDepth, 1, formatCharSlots);

defineLabel(mark$formatChar$fmt) {
   String fmt = frame(formatCharFrameDepth, String);
   Env* fmtEnv = frame(formatCharFrameDepth - stackSlotSize(String), Env*);
   if (fmtEnv != NULL && &fmtEnv->msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(fmt);
   jump(popLabel());
}

#endif

// formatChar :: String -> Char -> String

defineBuiltInFn_2_args(formatChar,
   typeCon(String), typeCon(Char), typeCon(String),
   fmt, String,
   c, Char)

   int nChars = snprintf(NULL, 0, fmt, c);
   if (nChars < 0)
      runTimeError("formatChar: formatting failed");

   size_t len = (size_t)nChars + 1;

#if (GARBAGE_COLLECTION ==1)
   stackAlloc(formatCharFmtDepth);
   stack(0, String) = fmt;
   stack(stackSlotSize(String), Env*) = fmtEnv;
   String ptr = (String)memAlloc_GC(len, altEntry(formatChar), (MSA_GC&)*consMSA n_vm);
   drop(formatCharFmtDepth);
#else
   String ptr = (String)consMSA->alloc(len);
#endif

   if (snprintf(ptr, len, fmt, c) < 0)
      runTimeError("formatChar: formatting failed");

   drop_Stack(formatChar);
   returnString(ptr, consEnv);
endBuiltInFn
