/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    memAlloc_GC.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory garbage collection.
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

#if (GARBAGE_COLLECTION==1)

#ifndef IVORY_MEM_ALLOC_GC_H_DEFINED
#define IVORY_MEM_ALLOC_GC_H_DEFINED

#include "MSA_GC.h"
#include "ivory/common.h"

extern Void markContinuations_GC(MSA& msa argN_VM);

extern Void* memAlloc_GC(size_t reqLen, Label label, MSA_GC& msa argN_VM);

#endif /* IVORY_MEM_ALLOC_GC_H_DEFINED */

#endif
