/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    MSA.cpp
 *
 * Module:  Memory Storage Allocator
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 1999
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Constructor and destructor
 *
 * Call formats:
 *
 *    MSA(blkLen);
 *
 *    blkLen -  (size_t)          - Buffer block size in bytes
 *
 *    ~MSA();
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

#include "MSA.h"

// Set WARN_LEAKS to 0 or 1 as required

#define WARN_LEAKS 0

#define MSA_BLK_HDR_LENGTH (sizeof(SegBlk) -\
                            MSA_MAX_BLK_LEN * sizeof(unsigned char))

MSA::MSA(size_t blkLen)
   : _nBlk(0), _nSeg(0),
     _limit(0), _totUse(0), _maxUse(0), _currUse(0),
     _hdBlk(NULL), _tlBlk(NULL), _currBlk(NULL), _errCode(0) {

// Round default block size up to highest multiple of SGM_QUANT bytes

   size_t dataLen = blkLen - MSA_BLK_HDR_LENGTH;

   _blkLen = MSA_BLK_HDR_LENGTH + 
               MSA_QUANT * ((dataLen + MSA_QUANT - 1) / MSA_QUANT);
}

MSA::~MSA() {
   SegBlk*  blk;

#if (WARN_LEAKS==1)
   if (_nSeg != 0)
      error("~MSA: segment count not zero");
#endif

// Do reverse loop over segment blocks, releasing memory

   while ((blk = _tlBlk) != NULL) {
      _tlBlk = blk->_prev;

//--- Release memory

      delete blk;
   }
}
