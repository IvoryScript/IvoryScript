/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    store.cpp
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
 *    Method to store a serialized set of segments to an archive stream.
 *
 * Call format:
 *
 *    store(Archive& archive)
 *
 *       archive  (Archive&)  - Archive stream
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

void SegmentTable::store(Archive& archive) const {
   archive.insertVLU(_count);
   UInt archiveCount = 0;
   if (_count > 0)
      for (UInt i = 0; i < (UInt)_maxId; i++) {
         SegmentEntry* entry;
         if ((entry = _vec.get(i)) != NULL) {
            archive.insertVLU(i);
            archive.insertVLU(entry->_len);
            for (UInt i = 0; i < entry->_len; i++)
               archive << entry->_segment[i];
            archive << (UInt32)entry->_hashVal;
            archive.insertVLU(entry->_refCount);
            archiveCount++;
         }
      }
   if (_count != archiveCount)
      error("SegmentTable::store: count mismatch");
}
