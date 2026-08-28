/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    MSA_GC.cpp
 *
 * Module:  Memory Storage Allocator with Garbage Collection
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Constructor and Destructor
 *
 * Call formats:
 *
 *    MSA_GC(blkLen);
 *
 *    blkLen -  (size_t)          - Buffer block size in bytes
 *
 *    ~MSA_GC();
 *
 * Method:
 *
 *    Constructor initialises all variables and pointers to 0 or NULL.
 *    Destructor frees any blocks of memory.
 *     
 * Errors:
 *
 *    None.
 *
 * Notes:
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

#include "MSA_GC.h"

MSA_GC::MSA_GC(size_t blkLen)
 : MSA(blkLen), _enabled(1), _enableAddBlk(1),
   _nGC_Segs(0), _markCount(0) {
}

void MSA_GC::setEnabled(Bool q) {
   _enabled = q;
   _enableAddBlk = q;
}

MSA_GC::~MSA_GC() {
}
