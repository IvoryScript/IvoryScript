/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    segmentId.cpp
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
 *    Method to return the segment id associated with a segment.
 *
 * Call format:
 *
 *    res = segmentId(segment, hashVal);
 *
 *    res      (SegmentId)   - Returned segment id
 *    segment  (const Byte*)  - Segment
 *    hashVal  (unsigned int) - Hash value
 *
 * Method:
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

SegmentId SegmentTable::segmentId(const Byte* segment, unsigned int hashVal) {
   for (SegmentEntry* entry = lockHash(hashVal % _nSlots);
      entry != NULL;
      entry = nextHashEntry(entry))
      if (entry->_segment == segment)
         return entry->_id;
   return NULL_SEGMENT_ID;
}
