/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinICell.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 9 February 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to extract a byte code cell from a byte input stream.
 *
 * Call format:
 *
 *    cell_ = extractBinICell(is, isEnv n_vm)
 *
 *       is       (InputStream_Byte&)  - Input stream
 *       isEnv    (Env&)               - Input stream environment
 *       n_vm                          - Virtual machine or absent
 *
 * Method:
 *
 *    Handles extraction of byte code closures.
 *
 * Errors:
 *
 *    None
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

#include "ivory/exec.h"
#include "ivory/ICellInfoMap.h"
#include "ivory/iSegmentTable.h"
#include "ivory/machine.h"
#include "ivory/segment.h"
#include "ivory/streams.h"

extern Cell* extractBinICell(InputStream_Byte& is, Env& isEnv argN_VM) {
   ExtractArchive ea(is._inAct, &isEnv);
   SegmentId segmentId;
   ea >> segmentId;

   IAddress segment = (IAddress)consEnv->segmentTable()->segment(segmentId);
   ICellInfoMapEntry* entry = consEnv->cellInfoMap()->lookUp(segmentId, segmentHash(segment));
   ICellInfo* iCellInfo = entry->_cellInfo;

   Void* ptr = consMSA->alloc(sizeof(Cell) - MAX_CELL_BODY_SIZE + mkClosure_max(iCellInfo->size(), MIN_CLOSURE_SIZE));
   Cell* cell_ = new(ptr) Cell(iCellInfo);
   if (iCellInfo->_byteCodeExtractFnEntry != NULL) {
      pushLabel(NULL);
      cell = cell_;
      cellEnv = consEnv;
      rEnv = static_cast<Env*>((Void*)&isEnv);
      rPtr = &is;
      callIContinuation(iCellInfo->_byteCodeExtractFnEntry n_vm);
   }

   return cell_;
}
