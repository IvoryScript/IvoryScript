/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    addEntry.cpp
 *
 * Module:  Segment Table Class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 5 July 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Internal method to add a segment entry.
 *
 * Call format:
 *
 *    addEntry(id, hashVal, segment, len, refCount)
 *
 *       res      (bool)         - Return value, true if successful
 *       id       (SegmentId)    - Segment id
 *       hashVal  (UInt)         - Hash value
 *       segment  (const Byte *) - Segment
 *       len      (size_t)       - Segment length
 *       refCount (UInt)         - Reference count
 *
 * Method:
 *
 *    Creates a new entry and links it into the hash table slot list
 *    before adding it to the segment vector.
 *     
 * Errors:
 *
 *    Errors are signalled with a return value of false.
 *
 * Notes:
 *
 *    The memory for the identifier string is allocated by the calling function.
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

SegmentTable::SegmentEntry* SegmentTable::addEntry(SegmentId id, UInt hashVal,
                                                   const Byte* segment,
                                                   size_t len,
                                                   UInt refCount) {
   SegmentEntry* entry = newEntry();
   if ((entry = newEntry()) == NULL) {
      error("SegmentTable::addEntry: no memory");
      return NULL;
   }
   SegmentEntry* next = lockHash(hashVal % _nSlots);
   if ((entry->_next = next) != NULL)
      next->_prev = entry;
   entry->_prev = NULL;
   entry->_id = id;
   entry->_segment = segment;
   entry->_len = len;
   entry->_hashVal = hashVal;
   entry->_refCount = refCount;
   _hashTab[hashVal % _nSlots] = entry;
   _vec.put(id, entry, _msa);
   _count++;
	
   return entry;
}
