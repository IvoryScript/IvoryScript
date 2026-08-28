/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    free.cpp
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
 *    Class method to free a previously allocated memory segment.
 *
 * Call format:
 *
 *    res = free(seg)
 *
 *       res - (int)          - Function result, 0 indicates success
 *       seg - (void*)        - Pointer to segment
 *
 * Errors:
 *
 *    Errors are signalled by a negative return value.
 *
 * Method:
 *
 *    The segment is marked free, if the last in a block then the block
 *    is released.
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

#include "MSA.h"

int MSA::free(void* seg) {
   FreeSeg* freeSeg;
   SegBlk*           blk;
   FreeSeg* prev;
   FreeSeg* cur;

// Return without error for NULL arg

   if (seg == NULL)
      return 0;

// Cast to free segment

   freeSeg = ((FreeSeg*)((unsigned char*)seg - sizeof(SegDescr)));

// Return with error if segment status not as expected

   if ((freeSeg->_len & MSA_ALLOC_FLAG) == 0)
      return -1;

// Clear flag bit

   freeSeg->_len &= ~MSA_ALLOC_FLAG;

// Find block containing segment

   if ((blk = queryBlk(freeSeg)) == NULL)
      return -1;

// Update buffer use monitors

   _nSeg--;
   _currUse -= freeSeg->_len;

// If releasing only segment in block, free up block

   if (--blk->_nAllocSeg == 0) {

//--- Amend buffer size

      _totUse -= blk->_len;

//--- Decrement block count

      _nBlk--;

//--- Unlink from list

      if (blk->_prev != NULL)
	      blk->_prev->_next = blk->_next;
      else
	      _hdBlk = blk->_next;
      if (blk->_next != NULL)
	      blk->_next->_prev = blk->_prev;
      else
	      _tlBlk = blk->_prev;

      if (_currBlk == blk)
	      _currBlk = _hdBlk;

      delete blk;

      return 0;
   }

// Insert into free list and concatanate any adjacent free segment

   prev = NULL;
   cur = blk->_hdFreeSeg;
   while (cur != NULL && cur < freeSeg) {
      prev = cur;
      cur = cur->_next;
   }

   if ((unsigned char*)((void*)freeSeg) + freeSeg->_len == (unsigned char*)((void*)cur)) {
      freeSeg->_len += cur->_len;
      freeSeg->_next = cur->_next;
   }
   else {
      freeSeg->_next = cur;
      blk->_nFreeSeg++;
   }

   if (prev != NULL &&
       (unsigned char *)prev + prev->_len == (unsigned char *)freeSeg) {
      if ((prev->_len += freeSeg->_len) > blk->_maxFreeSegLen)
	      blk->_maxFreeSegLen = prev->_len;
      prev->_next = freeSeg->_next;
      blk->_nFreeSeg--;
   } else {
      if (prev != NULL)
	      prev->_next = freeSeg;
      else
	      blk->_hdFreeSeg = freeSeg;

      if (freeSeg->_len > blk->_maxFreeSegLen)
	      blk->_maxFreeSegLen = freeSeg->_len;
   }

   return 0;
}
