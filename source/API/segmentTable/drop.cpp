/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    drpop.cpp
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
 *    Method to drop a segment.
 *
 * Call format:
 *
 *    drop(id);
 *
 *       id -  (SegmentId)    - Previously added segment
 *
 * Method:
 *
 *    Verifies that the segment id is valid.  If so, the entry use count is
 *    decremented and the segment entry removed if there are no further references.
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

void SegmentTable::dropSegment(SegmentId id) {
   SegmentEntry* entry;
   if (id >= _maxId || (entry = lockSegment(id)) == NULL) {
      error("SegmentTable::drop: segment id not in use");
      return;
   }
   if (adjustRefCount(entry, -1) == 0) {
      if (entry->_next != NULL)
         entry->_next->_prev = entry->_prev;
      if (entry->_prev != NULL)
         entry->_prev->_next = entry->_next;
      else
         _hashTab[entry->_hashVal % _nSlots] = entry->_next;
      _msa.free((void*)entry->_segment);
      _msa.free(entry);
      _vec.put(id, (SegmentTable::SegmentEntry*)NULL, _msa);
      if (id < _minFreeId)
         _minFreeId = id;
      _count--;
   }     
}
