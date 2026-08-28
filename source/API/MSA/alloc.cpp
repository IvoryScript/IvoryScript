/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    alloc.cpp
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
 *    Class method to allocate a memory segment.
 *
 * Call format:
 *
 *    seg = stackAlloc(reqLen)
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
 *    Simple first fit in current block or list of blocks.  A new block
 *    may be implicitly allocated.
 *
 * Notes:
 *
 * Modification history:
 *
 *    04/09/03 -  AJS   Now logs memory allocation errors
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

#include <memory.h>

#include "MSA.h"

// Constants to set efficiency threshold for full search

#define MSA_NUMERATOR   4
#define MSA_DENOMINATOR 5

void* MSA::alloc(size_t reqLen) {
   SegDescr*   descr;
   SegBlk*     blk;
   int         res;
   
// Return immediately if requested length = 0

   if (reqLen == 0)
		return NULL;
 
// Round up to highest multiple of MSA_QUANT bytes

   reqLen = MSA_QUANT * ((reqLen + sizeof(SegDescr) + MSA_QUANT - 1)
                      / MSA_QUANT );
   
   if (_currBlk != NULL) {

//--- Force full search if less than 80% efficiency

      if (_currUse * MSA_DENOMINATOR < _totUse * MSA_NUMERATOR)
         _currBlk = _hdBlk;

//--- Search forwards from current block to tail

      for (blk = _currBlk; blk != NULL; blk = blk->_next)
	      if (blk->_hdFreeSeg != NULL && reqLen <= blk->_maxFreeSegLen &&
	         (descr = blk->fstFit(reqLen)) != NULL)
	         goto lab;

//--- If unsuccesful, search backwards until head is reached

      for (blk = _currBlk->_prev; blk != NULL; blk = blk->_prev)
	      if (blk->_hdFreeSeg != NULL && reqLen <= blk->_maxFreeSegLen &&
	         (descr = blk->fstFit(reqLen)) != NULL)
            goto lab;
   }

// Add new block if necessary

   if ((res = addBlk(reqLen)) < 0) {
 //     error("MSA::alloc: memory allocation error");
      _errCode = res;
      return 0;
   }

   if ((descr = (blk = _tlBlk)->fstFit(reqLen)) == NULL)
      return 0;

// Label to avoid redundant comparison

lab:

// Update current block

   _currBlk = blk;

// Update buffer use monitors

   _nSeg++;
   if ((_currUse += reqLen) > _maxUse)
      _maxUse = _currUse;

// Clear segment if necessary

   if (MSA_CLR_FLG)
      memset(&((DataSeg*)(descr))->_data, 0, reqLen - sizeof(SegDescr));

   return ((DataSeg*)(descr))->_data;
}

//------------------------------------------------------------------------------
//
// Utility method to search for first free segment in block
//
//------------------------------------------------------------------------------

MSA::SegDescr* MSA::SegBlk::fstFit(size_t reqLen) {
   size_t     len;
   size_t     max = 0;
   FreeSeg*   prev;
   FreeSeg*   curr;
   FreeSeg*            next;

   prev = NULL;
   curr = _hdFreeSeg;

// Search for suitable free segment

   do {
      if ((len = curr->_len) >= reqLen)
	      break;
      if (len > max)
	      max = len;
      prev = curr;
      if ((curr = curr->_next) == NULL) {
	      _maxFreeSegLen = max;
	      return NULL;
      }
   }
   while (true);

// See if exact match

   if (reqLen == len) {
      curr->_len |= MSA_ALLOC_FLAG;
      next = curr->_next;
      if (--_nFreeSeg == 0)
         _maxFreeSegLen = 0;
   } else {

//--- No, split block

      curr->_len = reqLen | MSA_ALLOC_FLAG;

      next = (FreeSeg*)((unsigned char*)curr + reqLen);
      next->_len = len - reqLen;
      next->_next = curr->_next;
      if (_nFreeSeg == 1)
         _maxFreeSegLen = next->_len;
   }
   if (prev != NULL)
      prev->_next = next;
   else
      _hdFreeSeg = next;

// Update block segment count

   _nAllocSeg++;

   return (SegDescr*)curr;
}
