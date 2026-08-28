/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinCell.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 9 February 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to extract an expression from a byte input stream.
 *
 * Call format:
 *
 *    extractBinCell(is, isEnv n_vm)
 *
 *       is       (InputStream_Byte&)  - Input stream
 *       isEnv    (Env&)               - Input stream environment
 *       n_vm                          - Virtual machine or absent
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

#include "ivory/builtIn.h"
#include "ivory/exec.h"
#include "ivory/machine.h"
#include "ivory/serialStorage.h"
#include "ivory/streams.h"
#include <string.h>

Cell* extractBinCell(InputStream_Byte& is, Env& isEnv argN_VM) {
   ExtractArchive ea(is._inAct, &isEnv);
   Byte kind;
   ea >> kind;
   if (kind == (Byte)CELL_KIND_BUILT_IN_FN) {
      Bool isFirst;
      const char* fnName = extractBinSerialString(ea, *is._serialContext, isFirst);
      char* nameBuf = (char*)is._serialContext->msa().alloc(
         strlen(fnName) + strlen(BUILT_IN_CLOSURE_SUFFIX) + 1);
      strcpy(nameBuf, fnName);
      strcat(nameBuf, BUILT_IN_CLOSURE_SUFFIX);
      Name name = useName(nameBuf, builtInNameTable());
      BuiltInExportEntry* entry = builtInExportTable().lookUp(name, builtInExportTable().hash(name));
      is._serialContext->msa().free(nameBuf);
      return (Cell*)entry->_addr;
   } else if (kind == (Byte)CELL_KIND_BUILT_IN_PAP) {
      Bool isFirst;
      const char* cellInfoName = extractBinSerialString(ea, *is._serialContext, isFirst);
      Name name = useName(cellInfoName, builtInNameTable());
      BuiltInExportEntry* entry = builtInExportTable().lookUp(name, builtInExportTable().hash(name));
      if (entry == NULL) {
         error("extractBinCell: built-in PAP CellInfo not exported");
         return NULL;
      }

      CellInfo* cellInfo = (CellInfo*)entry->_addr;
      Void* ptr = consMSA->alloc(sizeof(Cell) - MAX_CELL_BODY_SIZE +
                                 mkClosure_max(cellInfo->size(), MIN_CLOSURE_SIZE));
      Cell* cell_ = new(ptr) Cell(cellInfo);
      Label cellExtractFn = cellInfo->cellExtractFn();
      if (cellExtractFn != NULL) {
         pushLabel(NULL);
         cell = cell_;
         cellEnv = consEnv;
         rEnv = static_cast<Env*>((Void*)&isEnv);
         rPtr = &is;
         callSequence(cellExtractFn);
      }

      return cell_;
   }
   return extractBinICell(is, isEnv n_vm);
}
