/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    queryBlk.cpp
 *
 * Module:  Memory Storage Allocator
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Local class method to locate the block containing a given segment.
 *
 * Call format:
 *
 *    blk = queryBlk(seg)
 *
 *       blk -    (SegBlk*)   - Returned pointer to memory block
 *       descr -  (void *)    - Segment descriptor
 *
 * Method:
 *
 *    The segment is inspected to see if it is contained in the current block,
 *    otherwise the block list is searched.
 *
 * Notes:
 *
 * Errors:
 *
 *    Errors are signalled with a NULL return value.
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

#include "MSA.h"

MSA::SegBlk* MSA::queryBlk(SegDescr* descr) {
   SegBlk*  blk;

   if (_currBlk->_data <= ((void*)descr) &&
       (void*)descr < (void*)((unsigned char*)_currBlk + _currBlk->_len))
      return _currBlk;

// Search list in reverse order until found or beginning of list

   for (blk = _tlBlk; blk != NULL; blk = blk->_prev)
      if ((void*)blk->_data <= (void*)descr &&
          (void*)descr < (void*)((unsigned char*)blk + blk->_len))
	   return blk;

// Return NULL if no matching block found

   return NULL;
}
