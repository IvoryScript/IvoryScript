/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ICellInfoMap.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 January 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Declarations relating to maintain a mapping between a segment id,
 *    a cell-info entry offset, and a cell information structure.
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

#ifndef IVORY_CELL_INFO_MAP_H_DEFINED
#define IVORY_CELL_INFO_MAP_H_DEFINED

#include "hashTable.h"
#include "../segmentTable.h"
#include "exec.h"

class ICellInfoMapEntry {
public:
   ICellInfoMapEntry(SegmentId segmentId, UInt32 entryOffset,
                     ICellInfo* cellInfo)
      : _segmentId(segmentId), _entryOffset(entryOffset),
        _cellInfo(cellInfo) {}

public:
   SegmentId   _segmentId;
   UInt32      _entryOffset; // Byte-code entry offset within the segment
   ICellInfo*  _cellInfo;

friend class ICellInfoMap;
};

class ICellInfoMap : public HashTable<Void*, ICellInfoMapEntry, SegmentId> {
public:
   ICellInfoMap(UInt nSlots);

   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   Void add(ICellInfoMapEntry& entry, UInt hashVal, MSA& msa);
   Void load(Archive& archive, const SegmentTable& segmentTable, MSA& msa);
   Void store(Archive& archive, const SegmentTable& segmentTable) const;

protected:
   virtual Void* allocNodePtr(size_t size, MSA& msa, Void*& link);
   virtual HashTableNode<Void*, ICellInfoMapEntry, SegmentId>& linkToNode(Void* link) const {
      return *(HashTableNode<Void*, ICellInfoMapEntry, SegmentId>*)link;
   }

   Void loadChain(UInt slotIndex, UInt len, Archive& archive, const SegmentTable& segmentTable, MSA& msa);

   UInt slotUseCount(Void) const;
   UInt chainLen(UInt index) const;

protected:
   virtual SegmentId keyOf(const ICellInfoMapEntry& entry) const { return entry._segmentId; }
};

#endif /* IVORY_CELL_INFO_MAP_H_DEFINED */
