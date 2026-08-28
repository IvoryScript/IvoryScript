/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    formatString.cpp
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
 *    Implementation of formatString function
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

#include "ivory/string.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

// formatString :: String -> String -> String

#if (GARBAGE_COLLECTION==1)
static const size_t formatStringArgsDepth = stackFPSizeString + stackFPSizeString;
static const size_t formatStringFrameDepth = formatStringArgsDepth + stackFPSizeString;

declareLabel(mark$formatString$args);

static Void markBuiltInPAPArg_GC(const String& str, Env& env argN_VM) {
   if (&env.msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(str);
}

static BuiltInFrameDescrSlot formatStringSlots[] = {
   BuiltInFrameDescrSlot(label(mark$formatString$args), NULL_NAME, NULL_TYPE)
};

static BuiltInFrameDescr frameDescr(altEntry(formatString),
   formatStringFrameDepth, 1, formatStringSlots);

defineLabel(mark$formatString$args) {
   String fmt = frame(formatStringFrameDepth, String);
   Env* fmtEnv = frame(formatStringFrameDepth - stackSlotSize(String), Env*);
   String s = frame(formatStringFrameDepth - stackFPSizeString, String);
   Env* sEnv = frame(formatStringFrameDepth - stackFPSizeString - stackSlotSize(String), Env*);
   if (fmtEnv != NULL && &fmtEnv->msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(fmt);
   if (sEnv != NULL && &sEnv->msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(s);
   jump(popLabel());
}

#endif

defineBuiltInFn_2_args(formatString,
   typeCon(String), typeCon(String), typeCon(String),
   fmt, String,
   s, String)

   int nChars = snprintf(NULL, 0, fmt, s);
   if (nChars < 0)
      runTimeError("formatString: formatting failed");

   size_t len = (size_t)nChars + 1;

#if (GARBAGE_COLLECTION ==1)
   stackAlloc(formatStringArgsDepth);
   stack(0, String) = fmt;
   stack(stackSlotSize(String), Env*) = fmtEnv;
   stack(stackFPSizeString, String) = s;
   stack(stackFPSizeString + stackSlotSize(String), Env*) = sEnv;
   String ptr = (String)memAlloc_GC(len, altEntry(formatString), (MSA_GC&)*consMSA n_vm);
   drop(formatStringArgsDepth);
#else
   String ptr = (String)consMSA->alloc(len);
#endif

   if (snprintf(ptr, len, fmt, s) < 0)
      runTimeError("formatString: formatting failed");

   drop_Stack(formatString);
   returnString(ptr, consEnv);
endBuiltInFn
