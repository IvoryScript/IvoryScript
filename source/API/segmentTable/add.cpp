/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    add.cpp
 *
 * Module:  Segment Table Class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Method to add a segment.
 *
 * Call format:
 *
 *    segmentId = add(segment, len, hashVal);
 *
 *       segmentId   (SegmentId)    - returned segment id
 *       segment     (const Byte*)  - segment
 *       len         (size_t)       - segment length
 *       hashVal     (unsigned int) - hash value
 *
 *
 *    add(sigmentId);
 *
 *       segmentId   (SegmentId)    - segmentId
 *
 * Method:
 *
 *    First form:
 *    Looks up the given segment.  If present, returns the id
 *    after incrementing the entry reference count.  Otherwise a new
 *    entry is added to the table.
 *
 *    Second form:
 *    Simply increments the entry reference count.
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

SegmentId SegmentTable::addSegment(const Byte* segment, size_t len,
                                   unsigned int hashVal) {
   SegmentEntry* entry;
   if ((entry = lookUp(segment, len, hashVal)) != NULL) {
      adjustRefCount(entry, 1);
      return entry->_id;
   }

   Byte* segmentCopy;
   if ((segmentCopy = (Byte*)_msa.alloc(len)) == NULL) {
      error("SegmentTable::add: no memory");
      return NULL_SEGMENT_ID;
   }
   memcpy(segmentCopy, segment, len);
   if ((entry = addEntry(nextSegment(), hashVal, segmentCopy, len, 1)) == NULL) {
      _msa.free(segmentCopy);
      error("SegmentTable::add: no memory");
      return NULL_SEGMENT_ID;
   }
	
   return entry->_id;
}

void SegmentTable::addSegment(SegmentId id) {
   SegmentEntry* entry;
   if (id >= _maxId || (entry = lockSegment(id)) == NULL) {
      error("SegmentTable::add: segment id not in use");
      return;
   }
   entry->_refCount++;
   return;
}
