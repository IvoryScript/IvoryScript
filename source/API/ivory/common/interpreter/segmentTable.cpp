/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    segmentTable.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 1999
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    IovryScript byte code segment table
 *
 * Call format:
 *
 *    ISegmentTable(nSlots, nPerSeg, msa);
 *
 *       nSlots   (unsigned int)       - No. slots in hash table
 *       nPerSeg  (unsigned int)       - No. entries per segment in vector
 *       msa      (MSA&)            
 *
 *    ~ISegmentTable();
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

#include "ivory/segment.h"
#include "ivory/iSegmentTable.h"

ISegmentTable::ISegmentTable(unsigned int nSlots, unsigned int segSize, MSA& msa)
   : SegmentTable(nSlots, segSize, msa) {

}

SegmentId ISegmentTable::lookUp(const Byte* segment, size_t len, unsigned int hashVal) {
   SegmentEntry* entry;
   if ((entry = SegmentTable::lookUp(segment, len, hashVal)) == NULL)
      return NULL_SEGMENT_ID;

   return entry->_id;
}

int ISegmentTable::match(const Byte* segment1, size_t len1,
                         const Byte* segment2, size_t len2) const {
   return len1 == len2 && eqSegment((IAddress)segment1, (IAddress)segment2);
}

Void ISegmentTable::load(Archive& archive,
                         const NameTable& nameTable,
                         const TypeTable& typeTable,
                         MSA& msa) {
   UInt count = archive.extractVLU();

   for (UInt i = 0; i < count; i++) {
      SegmentId id = (SegmentId)archive.extractVLU();
      for (; _maxId < id; _maxId++) 
         _vec.put(_maxId, NULL, _msa);
      SEGMENT_WORD len;
      archive >> len;
      Byte* segment = (Byte*)_msa.alloc(len);
      loadSegment(archive, segment, len, nameTable, typeTable);
      UInt32 hashVal;
      archive >> hashVal;
      UInt refCount = archive.extractVLU();

      if (addEntry(id, hashVal, segment, len, refCount) == NULL) {
         _msa.free(segment);
         error("SegmentTable::load no memory");
      }
      else
         _maxId = i + 1;
   }
}

Void ISegmentTable::postLoad(Env& env) {
   for (UInt i = 0; i < (UInt)_maxId; i++) {
      IAddress seg = (IAddress)segment(i);
      assignSegmentImports(seg, env);
      initSegment(seg, env);
   }
}

void ISegmentTable::store(Archive& archive,
                          const NameTable& nameTable,
                          const TypeTable& typeTable) const {
   archive.insertVLU(_count);
   UInt archiveCount = 0;
   if (_count > 0)
      for (UInt i = 0; i < (UInt)_maxId; i++) {
         SegmentEntry* entry;
         if ((entry = _vec.get(i)) != NULL) {
            archive.insertVLU(i);
            storeSegment(archive, (IAddress)entry->_segment, nameTable, typeTable);
            archive << (UInt32)entry->_hashVal;
            archive.insertVLU(entry->_refCount);
            archiveCount++;
         }
      }
   if (_count != archiveCount)
      error("SegmentTable::store: count mismatch");
}
