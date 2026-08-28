/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertBinCell.cpp
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
 *    Function to insert a cell into a byte output stream.
 *
 * Call format:
 *
 *    insertBinCell(os, osEnv, cell_, cellEnv_ n_vm)
 *
 *       os       (OutputStream_Byte&) - Output stream
 *       osEnv    (Env&)               - Output stream environment
 *       cell_    (Cell*)              - Cell to insert
 *       cellEnv  (Env&)               - Cell environment
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

#include "ivory/exec.h"
#include "ivory/builtIn.h"
#include "ivory/int.h"
#include "ivory/machine.h"
#include "ivory/serialStorage.h"
#include "ivory/streams.h"

Void insertBinCell(OutputStream_Byte& os, Env& osEnv,
                Cell& cell_, Env& cellEnv_ argN_VM) {

   InsertArchive ia(os._outputFn, &osEnv);

   Expr tag = cell_.tag();
   if (isPtr(tag)) {
      CellInfo& cellInfo = *(CellInfo*)static_cast<CellInfo*>(tag);
      Expr form = cellInfo.form();
      if (form == (Expr)BUILT_IN_FN) {
         ia << (Byte)CELL_KIND_BUILT_IN_FN;
         Name name = cellBody(cell_, BuiltInFn).name();
         insertBinSerialString(ia, *os._serialContext,
                            builtInNameTable().string(name));
      } else if (form == (Expr)BUILT_IN_PAP) {
         Name name = builtInExportTable().nameOfAddr((IAddress)&cellInfo);
         if (name == NULL_NAME)
            error("insertBinCell: built-in PAP CellInfo not exported");

         ia << (Byte)CELL_KIND_BUILT_IN_PAP;
         insertBinSerialString(ia, *os._serialContext,
                            builtInNameTable().string(name));

         Label cellInsertFn = cellInfo.cellInsertFn();
         if (cellInsertFn != NULL) {
            pushLabel(NULL);
            cell = &cell_;
            cellEnv = &cellEnv_;
            rEnv = static_cast<Env*>((Void*)&osEnv);
            rPtr = &os;
            callSequence(cellInsertFn);
         }
      } else if (cellInfo.entry_() == entry(iClosure) &&
                 cellInfo.altEntry_() == altEntry(iClosure)) {
         ia << (Byte)CELL_KIND_ICELL;
         insertBinICell(os, osEnv, cell_, cellEnv_ n_vm);
      } else {
         error("insertBinCell: unsupported native cell");
      }
   }
}
