/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    exec_GC.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory byte code interpreter garbage collection.
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

#if (INTERPRETER==1 && GARBAGE_COLLECTION==1)

#ifndef IVORY_EXEC_GC_H_DEFINED
#define IVORY_EXEC_GC_H_DEFINED

#include "MSA_GC.h"
#include "ivory/frameDescr.h"

extern Void* execMemAlloc_GC(size_t reqLen, IAddress pc, MSA_GC& msa argN_VM);

extern Void execMarkContinuations_GC(MSA& msa argN_VM);

extern Void execMarkFrame_GC(IAddress pc argN_VM);

#endif /* IVORY_EXEC_GC_H_DEFINED */

#endif
