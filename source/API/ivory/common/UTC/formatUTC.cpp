/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    formatUTC.cpp
 *
 * Module:  Ivory common (UTC)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of formatUTC function
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
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/string.h"
#include "ivory/UTC.h"

#define MAX_LENGTH   256

// formatUTC :: String -> UTC -> String

#if (GARBAGE_COLLECTION==1)
static const size_t formatUTCFmtDepth = stackFPSizeString;
static const size_t formatUTCFrameDepth = formatUTCFmtDepth + stackFPSizeUTC;

declareLabel(mark$formatUTC$fmt);

static Void markBuiltInPAPArg_GC(const String& str, Env& env argN_VM) {
   if (&env.msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(str);
}

static BuiltInFrameDescrSlot formatUTCSlots[] = {
   BuiltInFrameDescrSlot(label(mark$formatUTC$fmt), NULL_NAME, NULL_TYPE)
};

static BuiltInFrameDescr frameDescr(altEntry(formatUTC),
   formatUTCFrameDepth, 1, formatUTCSlots);

defineLabel(mark$formatUTC$fmt) {
   String fmt = frame(formatUTCFrameDepth, String);
   Env* fmtEnv = frame(formatUTCFrameDepth - stackSlotSize(String), Env*);
   if (fmtEnv != NULL && &fmtEnv->msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(fmt);
   jump(popLabel());
}

#endif

defineBuiltInFn_2_args(formatUTC,
   typeCon(String), typeCon(UTC), typeCon(String),
   fmt, String,
   utc, UTC)

   struct tm* tm = gmtime(&utc);
   if (tm == NULL)
      runTimeError("formatUTC: invalid UTC");

#if (GARBAGE_COLLECTION ==1)
   stackAlloc(formatUTCFmtDepth);
   stack(0, String) = fmt;
   stack(stackSlotSize(String), Env*) = fmtEnv;
   String ptr = (String)memAlloc_GC(MAX_LENGTH, altEntry(formatUTC), (MSA_GC&)*consMSA n_vm);
   drop(formatUTCFmtDepth);
#else
   String ptr = (String)consMSA->alloc(MAX_LENGTH);
#endif

   if (fmt[0] == '\0')
      ptr[0] = '\0';
   else if (strftime(ptr, MAX_LENGTH, fmt, tm) == 0)
      runTimeError("formatUTC: formatted result too long");

   drop_Stack(formatUTC);
   returnString(ptr, consEnv);
endBuiltInFn
