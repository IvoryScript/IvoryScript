/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ICellInfoMap.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2019
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class to maintain a mapping from a segment id
 *    and cell-info entry offset to a cell information structure.
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

#include "ivory/ICellInfoMap.h" 

ICellInfoMap::ICellInfoMap(UInt nSlots)
   : HashTable<Void*, ICellInfoMapEntry, SegmentId>(nSlots, NULL) {
}

Void ICellInfoMap::add(ICellInfoMapEntry& entry, UInt hashVal, MSA& msa) {
   if (_slots == NULL)
      allocSlots(msa);
   else
      if (lookUp(entry._segmentId, hashVal) != NULL) {
         error("ICellInfoMap::add: key not unique");
         return;
      }

   UInt index = hashVal % _nSlots;
   _slots[index] = allocNode(entry, _slots[index], msa);
}

void* ICellInfoMap::allocNodePtr(size_t size, MSA& msa, Void*& link) {
   return (link = msa.alloc(size));
}

Void ICellInfoMap::loadChain(UInt slotIndex, UInt len, Archive& archive, const SegmentTable& segmentTable, MSA& msa) {
   HashTableNode<Void*, ICellInfoMapEntry, SegmentId>** nodePtr = (HashTableNode<Void*, ICellInfoMapEntry, SegmentId>**)&_slots[slotIndex];
   for (UInt i = 0; i < len; i++) {
      SegmentId segmentId = (SegmentId)archive.extractVLU();
      IAddress segment = (IAddress)segmentTable.segment(segmentId);

      UInt size = archive.extractVLU();

      UInt32 entryOffset;
      archive >> entryOffset;
      UInt32 altEntryOffset;
      archive >> altEntryOffset;
      UInt32 copyFnOffset;
      archive >> copyFnOffset;
      UInt32 extractBinFnOffset;
      archive >> extractBinFnOffset;
      UInt32 insertBinFnOffset;
      archive >> insertBinFnOffset;

#if (GARBAGE_COLLECTION==1)
      UInt32 markCellEntryOffset;
      archive >> markCellEntryOffset;
#endif

      ICellInfo* cellInfo = (ICellInfo*)new(msa) ICellInfo(
         size,
         segment + entryOffset,
         segment + altEntryOffset,
         copyFnOffset != 0
            ? segment + copyFnOffset : 0,
         extractBinFnOffset != 0
            ? segment + extractBinFnOffset : 0,
         insertBinFnOffset != 0
            ? segment + insertBinFnOffset : 0,

#if (GARBAGE_COLLECTION==1)
         markCellEntryOffset != 0
            ? segment + markCellEntryOffset : 0,
#endif     

         segment);

      ICellInfoMapEntry mapEntry(segmentId, entryOffset, cellInfo);
      HashTableNode<Void*, ICellInfoMapEntry, SegmentId>* node = (HashTableNode<Void*, ICellInfoMapEntry, SegmentId>*)allocNode(mapEntry, NULL, msa);
      *nodePtr = node;
      nodePtr = ((HashTableNode<Void*, ICellInfoMapEntry, SegmentId>**)&node->next());
   }
}


UInt ICellInfoMap::chainLen(UInt index) const {
   UInt acc = 0;
   HashTableNode<Void*, ICellInfoMapEntry, SegmentId>* node = slot(index);
   while (node != NULL) {
      acc++;
      node = (HashTableNode<Void*, ICellInfoMapEntry, SegmentId>*)node->next();
   }
   return acc;
}

UInt ICellInfoMap::slotUseCount(Void) const {
   UInt acc = 0;
   if (_slots != NULL)
      for (UInt i = 0; i < _nSlots; i++)
         if (_slots[i] != NULL)
            acc++;
   return acc;
}


void ICellInfoMap::load(Archive& archive, const SegmentTable& segmentTable,
                        MSA& msa) {
   allocSlots(msa);
   UInt slotsInUse = archive.extractVLU();
   for (UInt i = 0; i < slotsInUse; i++) {
      UInt index = archive.extractVLU();
      UInt chainLen = archive.extractVLU();
      loadChain(index, chainLen, archive, segmentTable, msa);
   }
}

// ICellInfoMap::store: Store segment id/cell info map

void ICellInfoMap::store(Archive& archive, const SegmentTable& segmentTable) const {
   archive.insertVLU(_nSlots);
   archive.insertVLU(slotUseCount());
   if (_slots != NULL)
      for (UInt i = 0; i < _nSlots; i++) {
         UInt len = chainLen(i);
         if (len > 0) {
            archive.insertVLU(i);
            archive.insertVLU(len);
            HashTableNode<Void*, ICellInfoMapEntry, SegmentId>* node = slot(i);
            while (node != NULL) {
               ICellInfoMapEntry entry = node->entry_();
               archive.insertVLU(entry._segmentId);
               ICellInfo& cellInfo = *entry._cellInfo;
               IAddress segment = (IAddress)segmentTable.segment(entry._segmentId);

               archive.insertVLU(cellInfo.size());

               archive << (UInt32)(cellInfo._byteCodeEntry - segment);
               archive << (UInt32)(cellInfo._byteCodeAltEntry - segment);
               archive << (cellInfo._byteCodeCopyFnEntry != NULL
                  ? (UInt32)(cellInfo._byteCodeCopyFnEntry - segment) : 0);
               archive << (cellInfo._byteCodeExtractFnEntry != NULL
                  ? (UInt32)(cellInfo._byteCodeExtractFnEntry - segment) : 0);
               archive << (cellInfo._byteCodeInsertFnEntry != NULL
                  ? (UInt32)(cellInfo._byteCodeInsertFnEntry - segment) : 0);

#if (GARBAGE_COLLECTION==1)            
               archive << (cellInfo._byteCodeGC_MarkCellEntry != NULL
                  ? (UInt32)(cellInfo._byteCodeGC_MarkCellEntry - segment) : 0);
#endif 

 
               node = (HashTableNode<Void*, ICellInfoMapEntry, SegmentId>*)node->next();
            }
         }
   }
}
