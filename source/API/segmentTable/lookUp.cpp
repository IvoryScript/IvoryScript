/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    lookUp.cpp
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
 *    Methods to look up segments.
 *
 * Call format:
 *
 *    segmentId = lookup(segment, len);
 *
 *    segmentId - (Segment)         - Returned segment id
 *    segment -   (const Byte *)    - Segment
 *    len -       (size_t)          - Segment length
 *
 *    segmentId = lookup(segment, len, hashVal);
 *
 *    segmentId - (Segment)         - Returned segment id
 *    segment -   (const Byte *)    - Segment
 *    len -       (size_t)          - Segment length
 *    hashVal -   (unsigned int)    - Hash value
 *
 * Method:
 *
 *    Public and local table lookup based on linear search of list
 *    of segments with the same hash value.
 *     
 * Errors:
 *
 *    Errors are signalled with a return value of NULL_SEGMENT_ID or NULL.
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
#include <string.h>

SegmentId SegmentTable::lookUp(const Byte * segment, size_t len) {
   unsigned int hashVal = hashDJB2(segment, len);
   SegmentEntry* entry;
   if ((entry = lookUp(segment, len, hashVal)) == NULL)
      return NULL_SEGMENT_ID;

   return entry->_id;
}

SegmentTable::SegmentEntry* SegmentTable::lookUp(const Byte *segment,
                                                 size_t len,
                                                 unsigned int hashVal) {
   for (SegmentEntry* entry = lockHash(hashVal % _nSlots);
        entry != NULL;
        entry = nextHashEntry(entry))
      if (match(entry->_segment, entry->_len, segment, len) == 1)
           return entry;
   return NULL;
}
