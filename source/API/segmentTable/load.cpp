/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    load.cpp
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
 *    Method to load a serialised set of segments from an archive stream.
 *
 * Call format:
 *
 *    load(archive)
 *
 *       archive  (Archive&)     - Archive stream
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

void SegmentTable::load(Archive& archive) {
   UInt count = archive.extractVLU();

   for (UInt i = 0; i < count; i++) {
      SegmentId id = (SegmentId)archive.extractVLU();
      while (_maxId < id) {
         _vec.put(_maxId, NULL, _msa);
         _maxId++;
      }
      size_t len = archive.extractVLU();
      Byte * segment = (Byte *)_msa.alloc(len);
      for (UInt j = 0; j < len; j++)
         archive >> segment[j];
      UInt32 hashVal;
      archive >> hashVal;
      UInt refCount = archive.extractVLU();

      if (addEntry(id, hashVal, segment, len, refCount) == NULL) {
         _msa.free(segment);
         error("SegmentTable::load no memory");
      }
      else
         _maxId = id + 1;
   }
}
