/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    allocGC.cpp
 *
 * Module:  Memory Storage Allocator with Garbage Collection class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Class method to allocate a memory segment for GC.
 *
 * Call format:
 *
 *    seg = allocGC(reqLen)
 *
 *       seg      (void*)        - Returned pointer to segment
 *       reqLen   (size_t)       - Requested length in bytes
 *
 * Errors:
 *
 *    Errors are signalled with a NULL return value.
 *
 * Method:
 *
 *    Simply calls alloc, flags the segment and returns it.
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

#include "ivory/trace.h"

void* MSA_GC::allocGC(size_t reqLen) {
   void* seg = alloc(reqLen);

   if (seg != NULL)
      ((SegDescr*)((unsigned char*)seg - sizeof(SegDescr)))->_len |=
         MSA_GC_FLAG;

   _nGC_Segs++;


 //  outStream << "Allocated " << seg << " (" << reqLen << ')' << endl;

   return seg;
}
