/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    addBlk.cpp
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
 *    Local class method to add a new block of memory.
 *
 * Call format:
 *
 *    res = addBlk(reqLen)
 *
 *       res      (int)          - Function result, 0 indicates success
 *       reqLen   (size_t)       - Size of request which must be satisfied
 *
 * Errors:
 *
 *    Errors are signalled with a negative return value.
 *
 * Method:
 *
 *    A new block of memory is allocated and appended to the tail of the list.
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

#include <memory.h>

#include "MSA.h"

// Define extra added to guarantee allocation of requested size

#define MSA_BLK_HDR_LENGTH (sizeof(SegBlk) -\
                            MSA_MAX_BLK_LEN * sizeof(unsigned char))

int MSA::addBlk(size_t reqLen) {
   size_t   len;
   SegBlk*  blk;

// Set size for new memory block

   len = _blkLen;

// Ensure allocation is at least equal to the request size */

   if (len < reqLen + MSA_BLK_HDR_LENGTH)
      len = reqLen + MSA_BLK_HDR_LENGTH;

// If limit enabled (> 0), restrict length or return error

   if (_limit != 0 && _totUse + len > _limit) {
      len = _limit - _totUse;
      if (len < reqLen)
         return -1;
   }

// Obtain new memory block

   if ((blk = (SegBlk*)(new unsigned char[len])) == NULL)
      return -1;

// Clear block header

   memset(blk, 0, blk->_data - (unsigned char*)blk);

   blk->_len = len;

// Append to list

   if ((blk->_prev = _tlBlk) != NULL)
      blk->_prev->_next = blk;
   else
      _hdBlk = blk;
   blk->_next = NULL;
   _tlBlk = blk;

// Set up initial segment list

   blk->_hdFreeSeg = (FreeSeg*)blk->_data;

// Plant size of free segment

   blk->_hdFreeSeg->_len = len - MSA_BLK_HDR_LENGTH;
   blk->_maxFreeSegLen = blk->_hdFreeSeg->_len;
   blk->_hdFreeSeg->_next = NULL;

// Amend buffer size

   _totUse += len;

// Increment block count

   _nBlk++;

// Initialise segment counts

   blk->_nAllocSeg = 0;
   blk->_nFreeSeg = 1;

   return 0;
}
