/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    mark.cpp
 *
 * Module:  Memory Storage Allocator with Garbage Collection
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Class method to mark a memory segment.
 *
 * Call format:
 *
 *    res = mark(seg)
 *
 *       res - (int)          - Function result, 1 indicates success
 *                                               0 if already marked
 *       seg - (void*)        - Pointer to segment
 *
 * Errors:
 *
 *    An error is signalled by a negative return value.
 *
 * Method:
 *
 *    Simply sets the segment's mark flag, rasing an error if the segment
 *    GC flag isn't set.
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

#include <ivory/trace.h>

int MSA_GC::mark(void* seg) {
   SegDescr* segDescr;

// Return 0 for NULL arg

   if (seg == NULL)
      return 0;

// Pointer to segment descriptor

   segDescr = ((SegDescr*)((unsigned char*)seg - sizeof(SegDescr)));

// Return with error if segment status not as expected

   if ((segDescr->_len & MSA_ALLOC_FLAG) == 0 ||
       (segDescr->_len & MSA_GC_FLAG)    == 0)
      return -1;

// Return if already marked

   if ((segDescr->_len & MSA_MARK_FLAG) != 0) {

//--- outStream << "Segment already marked: " << (Void*)seg << endl; // Potential diagnostics

      return 0;
   }

// Set flag bit

   segDescr->_len |= MSA_MARK_FLAG;
   _markCount++;

 //--- outStream << "Marked segment: " << (Void*)seg << endl; // Potential diagnostics

   return 1;
}
