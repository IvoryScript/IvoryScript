/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    dirEntryName.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of dirEntryName built in function
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

#ifdef CONST
#undef CONST
#endif

#ifdef STRICT
#undef STRICT
#endif

#ifdef VOID
#undef VOID
#endif

#include "ivory/dir.h"

#if (GARBAGE_COLLECTION==1)
#include <ivory/frameDescr.h>
#endif

#include "ivory/maybe.h"
#include "ivory/string.h"

#if (GARBAGE_COLLECTION==1)
#include <ivory/memAlloc_GC.h>
#endif

#if (GARBAGE_COLLECTION==1)
 // No active roots, arbitrary association label
static BuiltInFrameDescr frameDescr(altEntry(dirEntryName), 0, 0, NULL);
#endif

// dirEntryName :: DirEntry -> String

defineBuiltInFn_1_arg(dirEntryName,
   typeCon(DirEntry), typeCon(String),
   dirEntry, DirEntry)
   size_t sLen = strlen(dirEntry->d_name);

   #if (GARBAGE_COLLECTION ==1)
   String res = (String)memAlloc_GC(sLen + 1, altEntry(dirEntryName), (MSA_GC&)*consMSA n_vm);
   #else
   String res = (String)consMSA->alloc(sLen + 1);
   #endif

   strcpy(res, dirEntry->d_name);
   returnString(res, consEnv);
endBuiltInFn
