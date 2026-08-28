/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    sweep.cpp
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
 *    Class method to sweep the list of blocks.
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
 *    None.
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

// #define TRACE // Define to trace released segments

#include "MSA_GC.h"

#ifdef TRACE
#include <ivory/trace.h>
#endif

void MSA_GC::sweep(void) {

   assert(_enabled, "MSA_GC: Expected garbage collection enabled");

   UInt nFree = 0;
   UInt prevN_Free = 0;

// Do reverse loop over segment blocks, freeing up any unmarked garbage
// collectable segments and freeing block if none remain

   SegBlk* blk = _tlBlk;
   SegBlk* fstBlk = NULL;     // First block with a released segment

   while (blk != NULL) {
      DataSeg* dataSeg = (DataSeg*)(blk->_data);
      FreeSeg* nextFreeSeg = blk->_hdFreeSeg;
      FreeSeg* prevFreeSeg = NULL;
      size_t maxFreeSegLen = 0;

      do {
         size_t len = dataSeg->_len;
         if ((len & MSA_GC_FLAG) != 0) {
            if ((len & MSA_MARK_FLAG) != 0) {

//------------ Clear mark flag

               dataSeg->_len &= ~MSA_MARK_FLAG;
               len &= ~(MSA_MARK_FLAG | MSA_GC_FLAG | MSA_ALLOC_FLAG);

               _markCount--;
            }
            else {

//------------ Segment not marked, release

#ifdef TRACE
               outStream << "Releasing segment: " << (Void*)((unsigned char*)dataSeg + sizeof(SegDescr)) << endl;
#endif

               len &= ~(MSA_ALLOC_FLAG | MSA_GC_FLAG);
               dataSeg->_len = len;

               nFree += len;
               _currUse -= len;
               _nSeg--;

               if (prevFreeSeg == NULL && nextFreeSeg == NULL) {
                  maxFreeSegLen = len;
                  blk->_nAllocSeg--;
                  blk->_nFreeSeg++;

                  ((FreeSeg*)dataSeg)->_next = NULL;

                  blk->_hdFreeSeg = (FreeSeg*)dataSeg;

                  prevFreeSeg = (FreeSeg*)dataSeg;
               }
               else {
                  while (nextFreeSeg != NULL && nextFreeSeg < (FreeSeg*)dataSeg) {
                     if (nextFreeSeg->_len > maxFreeSegLen)
                        maxFreeSegLen = nextFreeSeg->_len;
                     prevFreeSeg = nextFreeSeg;
                     nextFreeSeg = nextFreeSeg->_next;
                  }

                  if ((unsigned char*)((void*)dataSeg) + len == (unsigned char*)((void*)nextFreeSeg)) {
                     len += nextFreeSeg->_len;
                     dataSeg->_len = len;
                     ((FreeSeg*)dataSeg)->_next = nextFreeSeg->_next;
                     if (prevFreeSeg == NULL)
                        blk->_hdFreeSeg = prevFreeSeg;
                     else
                        prevFreeSeg->_next = (FreeSeg*)dataSeg;
                     nextFreeSeg = ((FreeSeg*)dataSeg)->_next;
                  }
                  else {
                     ((FreeSeg*)dataSeg)->_next = nextFreeSeg;
                     blk->_nFreeSeg++;
                  }

                  if (prevFreeSeg != NULL &&
                     (unsigned char*)prevFreeSeg + prevFreeSeg->_len == (unsigned char*)dataSeg) {
                     len += prevFreeSeg->_len;
                     if (len > maxFreeSegLen)
                        maxFreeSegLen = len;
                     prevFreeSeg->_len = len;
                     prevFreeSeg->_next = ((FreeSeg*)dataSeg)->_next;
                     dataSeg = (DataSeg*)prevFreeSeg;
                     blk->_nFreeSeg--;
                  }
                  else {
                     if (prevFreeSeg != NULL)
                        prevFreeSeg->_next = (FreeSeg*)dataSeg;
                     else
                        blk->_hdFreeSeg = (FreeSeg*)dataSeg;
                     prevFreeSeg = (FreeSeg*)dataSeg;

                  }
               }
            }
         }
         else
            len &= ~MSA_ALLOC_FLAG;

         if ((unsigned char*)dataSeg + len == (unsigned char*)blk + blk->_len)
            break;
         dataSeg = (DataSeg*)((unsigned char*)dataSeg + len);

      } while (1);

      blk->_maxFreeSegLen = maxFreeSegLen;

      if (nFree >= _blkLen && _markCount == 0) {

//          outStream << "Collected " << nFree << " bytes" << endl;

         _currBlk = blk;
         _enabled = 0;
         return;
      }

      if (blk->_nAllocSeg != 0) {
         if (nFree != prevN_Free) {
            _currBlk = blk;
            prevN_Free = nFree;
         }

         blk = blk->_prev;
      } else {
         _totUse -= blk->_len;

//------ Decrement block count

         _nBlk--;

//------ Unlink from list

         SegBlk* prevBlk = blk->_prev;

         if (prevBlk != NULL)
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

         blk = prevBlk;
      }
   }
//   if (nFree < _blkLen)
      _enabled = 0;

   assert(_markCount == 0, "MSA_GC::sweep: Mark count not zero");

//      outStream << "Collected " << nFree << " bytes" << endl;
   
} 
