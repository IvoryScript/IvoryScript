/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    MSA_GC.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 7 January 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Memory Storage Allocator
 *    with Garbage Collection class
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

#ifndef MSA_GC_H_DEFINED
#define MSA_GC_H_DEFINED

#include "MSA.h"

// Segment descriptor flags

#define MSA_GC_FLAG     2
#define MSA_MARK_FLAG   4

class MSA_GC : public MSA {

public:
   MSA_GC(size_t blkLen);
   virtual ~MSA_GC();

   inline Bool isEnabled(void) const { return _enabled; }
   void setEnabled(Bool q);
   inline void setEnableAddBlk(Bool q) { _enableAddBlk = q; }

   void* allocGC(size_t reqLen);
   int mark(void* seg);
   void sweep(void);

   void check(void);

protected:
   virtual int addBlk(size_t rqLen);

protected:
   Bool  _enabled;
   Bool  _enableAddBlk;
   UInt  _nGC_Segs;
   UInt  _markCount;
};

#endif /* MSA_GC_H_DEFINED */
