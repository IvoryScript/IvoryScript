/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertBinICell.cpp
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
 *    Function to insert a byte code cell into a byte output stream.
 *
 * Call format:
 *
 *    insertBinICell(os, osEnv, cell_, cellEnv n_vm)
 *
 *       os       (OutputStream_Byte& os) - Output stream
 *       osEnv    (Env&)                  - Output stream environment
 *       cell_    (Cell*)                 - Cell to insert
 *       cellEnv  (Env&)                  - Cell environment
 *       n_vm                             - Virtual machine or absent
 *
 * Method:
 *
 *    Handles insertion of byte code closures
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
#include "ivory/iSegmentTable.h"
#include "ivory/machine.h"
#include "ivory/segment.h"
#include "ivory/streams.h"


extern Void insertBinICell(OutputStream_Byte& os, Env& osEnv,
                           Cell& cell_, Env& cellEnv_ argN_VM) { 
   const ICellInfo& iCellInfo = static_cast<const ICellInfo&>(*static_cast<const CellInfo*>(cell_.tag()));
   InsertArchive ia(os._outputFn, &osEnv);
   IAddress segment = iCellInfo.segment();
   assert(cellEnv_.segmentTable() != NULL, "insertBinICell: Null segment table");
   SegmentId segId = cellEnv_.segmentTable()->segmentId(segment, segmentHash(segment));
   assert(segId != NULL_SEGMENT_ID, "insertBinICell: Null segment id");
   ia << segId;
//   ia << (UInt32)(iCellInfo._byteCodeEntry - segment);
   if (iCellInfo._byteCodeInsertFnEntry != NULL) {
      pushLabel(NULL);
      cell = &cell_;
      cellEnv = &cellEnv_;
      rEnv = static_cast<Env*>((Void*)&osEnv);
      rPtr = &os;
      callIContinuation(iCellInfo._byteCodeInsertFnEntry n_vm);
   }
}
