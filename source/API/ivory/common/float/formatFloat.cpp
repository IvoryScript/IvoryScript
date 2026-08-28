/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    formatFloat.cpp
 *
 * Module:  Ivory common (float)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of formatFloat function
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

#include "ivory/float.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/string.h"

// formatFloat :: String -> Float -> String

#if (GARBAGE_COLLECTION==1)
static const size_t formatFloatFmtDepth = stackFPSizeString;
static const size_t formatFloatFrameDepth = formatFloatFmtDepth + stackFPSizeFloat;

declareLabel(mark$formatFloat$fmt);

static Void markBuiltInPAPArg_GC(const String& str, Env& env argN_VM) {
   if (&env.msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(str);
}

static BuiltInFrameDescrSlot formatFloatSlots[] = {
   BuiltInFrameDescrSlot(label(mark$formatFloat$fmt), NULL_NAME, NULL_TYPE)
};

static BuiltInFrameDescr frameDescr(altEntry(formatFloat),
   formatFloatFrameDepth, 1, formatFloatSlots);

defineLabel(mark$formatFloat$fmt) {
   String fmt = frame(formatFloatFrameDepth, String);
   Env* fmtEnv = frame(formatFloatFrameDepth - stackSlotSize(String), Env*);
   if (fmtEnv != NULL && &fmtEnv->msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(fmt);
   jump(popLabel());
}

#endif

defineBuiltInFn_2_args(formatFloat,
   typeCon(String), typeCon(Float), typeCon(String),
   fmt, String,
   x, Float)

   int nChars = snprintf(NULL, 0, fmt, x);
   if (nChars < 0)
      runTimeError("formatFloat: formatting failed");

   size_t len = (size_t)nChars + 1;

#if (GARBAGE_COLLECTION ==1)
   stackAlloc(formatFloatFmtDepth);
   stack(0, String) = fmt;
   stack(stackSlotSize(String), Env*) = fmtEnv;
   String ptr = (String)memAlloc_GC(len, altEntry(formatFloat), (MSA_GC&)*consMSA n_vm);
   drop(formatFloatFmtDepth);
#else
   String ptr = (String)consMSA->alloc(len);
#endif

   if (snprintf(ptr, len, fmt, x) < 0)
      runTimeError("formatFloat: formatting failed");

   drop_Stack(formatFloat);
   returnString(ptr, consEnv);
endBuiltInFn

