/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    copyICellInfo.cpp
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
 *    Function to a copy a byte code cell information structure and its
 *    associated segment to a destination environment.
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
#include "ivory/exec.h"
#include "ivory/segment.h"
#include "ivory/iSegmentTable.h"

const ICellInfo* copyICellInfo(const ICellInfo* cellInfo, Env& env) {
   const IAddress segment = cellInfo->_segment;
   IAddress segmentCopy = segment;
   if (env.segmentTable() != NULL) {
      SegmentId segmentId = copySegment(segment, env);
      segmentCopy = (IAddress)env.segmentTable()->segment(segmentId);
      UInt32 hashVal = segmentHash(segment);
      ICellInfoMapEntry* entry = env.cellInfoMap()->lookUp(segmentId, hashVal);
      if (entry == NULL) {
         ICellInfo* res  = new(env.msa()) ICellInfo(cellInfo->size(),
            cellInfo->_byteCodeEntry + (segmentCopy - segment),
            cellInfo->_byteCodeAltEntry + (segmentCopy - segment),
            cellInfo->_byteCodeCopyFnEntry != 0
            ? cellInfo->_byteCodeCopyFnEntry + (segmentCopy - segment) : 0,
            cellInfo->_byteCodeExtractFnEntry != 0
            ? cellInfo->_byteCodeExtractFnEntry + (segmentCopy - segment) : 0,
            cellInfo->_byteCodeInsertFnEntry != 0
            ? cellInfo->_byteCodeInsertFnEntry + (segmentCopy - segment) : 0,

#if (GARBAGE_COLLECTION==1)
            cellInfo->_byteCodeGC_MarkCellEntry != 0
            ? cellInfo->_byteCodeGC_MarkCellEntry + (segmentCopy - segment) : 0,
#endif     

            segmentCopy);

         ICellInfoMapEntry mapEntry = ICellInfoMapEntry(segmentId, res);
         env.cellInfoMap()->add(mapEntry, hashVal, env.msa());
         return res;
      }
      else
         return entry->_cellInfo;

   }
   else
      return cellInfo;  
}
