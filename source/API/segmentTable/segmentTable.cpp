/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    segmentTable.cpp
 *
 * Module:  Segment Table Class
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
 * Call format:
 *
 *    SegmentTable(nSlots, nPerSeg, msa);
 *
 *       nSlots   (unsigned int)       - No. slots in hash table
 *       nPerSeg  (unsigned int)       - No. entries per segment in vector
 *       msa      (MSA&)            
 *
 *    ~SegmentTable();
 *
 * Method:
 *
 *    Constructor & destructor functions for 'SegmentTable' class
 *    for shared segments of memory.
 *    Constructor initialises all member variables and allocates hash table.
 *    Destructor releases all allocated memory.
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

#include "segmentTable.h"

SegmentTable::SegmentTable(unsigned int nSlots, unsigned int nPerSeg, MSA& msa)
 : _nSlots(nSlots), _nPerSeg(nPerSeg),
   _msa(msa),
   _vec(nPerSeg), _count(0),
   _minFreeId(0), _maxId(0) {
   _hashTab = (SegmentEntry**)msa.alloc(nSlots * sizeof(SegmentEntry*));
   memset(_hashTab, 0, nSlots * sizeof(SegmentEntry*));
}

SegmentTable::~SegmentTable() {
   for (unsigned int i = 0; i < _nSlots; i++) {
      SegmentEntry * entry;
      while ((entry = _hashTab[i]) != NULL) {
         _hashTab[i] = entry->_next;
         _msa.free((void *)entry->_segment);
         _msa.free(entry);
      }
   }
   _msa.free(_hashTab);
}

void* SegmentTable::operator new(size_t size, MSA& msa) {
   return msa.alloc(size);
}

#ifdef DELETE_NEEDS_NEW_ARG
void SegmentTable::operator delete(void* ptr, MSA& msa) {
}
#endif
void SegmentTable::operator delete(void* ptr) {
}
