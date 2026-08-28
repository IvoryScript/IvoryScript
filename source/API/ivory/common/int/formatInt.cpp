/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    formatInt.cpp
 *
 * Module:  Ivory common (int)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of formatInt function
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

#include "ivory/int.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/string.h"

// formatInt :: String -> Int -> String

#if (GARBAGE_COLLECTION==1)
static const size_t formatIntFmtDepth = stackFPSizeString;
static const size_t formatIntFrameDepth = formatIntFmtDepth + stackFPSizeInt;

declareLabel(mark$formatInt$fmt);

static Void markBuiltInPAPArg_GC(const String& str, Env& env argN_VM) {
   if (&env.msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(str);
}

static BuiltInFrameDescrSlot formatIntSlots[] = {
   BuiltInFrameDescrSlot(label(mark$formatInt$fmt), NULL_NAME, NULL_TYPE)
};

static BuiltInFrameDescr frameDescr(altEntry(formatInt),
   formatIntFrameDepth, 1, formatIntSlots);

defineLabel(mark$formatInt$fmt) {
   String fmt = frame(formatIntFrameDepth, String);
   Env* fmtEnv = frame(formatIntFrameDepth - stackSlotSize(String), Env*);
   if (fmtEnv != NULL && &fmtEnv->msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(fmt);
   jump(popLabel());
}

#endif

defineBuiltInFn_2_args(formatInt,
   typeCon(String), typeCon(Int), typeCon(String),
   fmt, String,
   i, Int)

   int nChars = snprintf(NULL, 0, fmt, i);
   if (nChars < 0)
      runTimeError("formatInt: formatting failed");

   size_t len = (size_t)nChars + 1;

#if (GARBAGE_COLLECTION ==1)
   stackAlloc(formatIntFmtDepth);
   stack(0, String) = fmt;
   stack(stackSlotSize(String), Env*) = fmtEnv;
   String ptr = (String)memAlloc_GC(len, altEntry(formatInt), (MSA_GC&)*consMSA n_vm);
   drop(formatIntFmtDepth);
#else
   String ptr = (String)consMSA->alloc(len);
#endif

   if (snprintf(ptr, len, fmt, i) < 0)
      runTimeError("formatInt: formatting failed");

   drop_Stack(formatInt);
   returnString(ptr, consEnv);
endBuiltInFn
