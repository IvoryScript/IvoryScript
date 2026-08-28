/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    builtIn.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Ivory built-in export table.
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
#include "ivory/repr.h"

BuiltInExportTable::BuiltInExportTable(UInt nSlots)
   : HashTable<Void*, BuiltInExportEntry, Name>(nSlots, NULL) {
}

Void BuiltInExportTable::add(ConstString ident, IAddress addr) {
   Name name = ::useName(ident, builtInNameTable());
   BuiltInExportEntry entry = BuiltInExportEntry(name, addr);
   HashTable<Void*, BuiltInExportEntry, Name>::add(entry, name, builtInMSA());
}

Void BuiltInExportTable::addCell(ConstString ident, const Cell& cell_, const struct CellInfo& cellInfo) {
   String fnIdent = (String)builtInMSA().alloc(strlen(ident) + strlen(BUILT_IN_CLOSURE_SUFFIX) + 1);
   String entryIdent = (String)builtInMSA().alloc(strlen(ident) + strlen(BUILT_IN_ENTRY_SUFFIX) + 1);
   String altEntryIdent = (String)builtInMSA().alloc(strlen(ident) + strlen(BUILT_IN_ALT_ENTRY_SUFFIX) + 1);

   strcpy(fnIdent, ident);
   strcat(fnIdent, BUILT_IN_CLOSURE_SUFFIX);
   strcpy(entryIdent, ident);
   strcat(entryIdent, BUILT_IN_ENTRY_SUFFIX);
   strcpy(altEntryIdent, ident);
   strcat(altEntryIdent, BUILT_IN_ALT_ENTRY_SUFFIX);

   builtInExportTable().add(fnIdent, (IAddress)&cell_);
   builtInExportTable().add(entryIdent, (IAddress)cellInfo.entry_());
   builtInExportTable().add(altEntryIdent, (IAddress)cellInfo.altEntry_());

   builtInMSA().free(altEntryIdent);
   builtInMSA().free(entryIdent);
   builtInMSA().free(fnIdent);
}

Void BuiltInExportTable::addPAPCellInfo(ConstString ident, const struct CellInfo& cellInfo) {
   String cellInfoIdent = (String)builtInMSA().alloc(strlen(ident) + strlen(BUILT_IN_PAP_CELL_INFO_SUFFIX) + 1);

   strcpy(cellInfoIdent, ident);
   strcat(cellInfoIdent, BUILT_IN_PAP_CELL_INFO_SUFFIX);

   builtInExportTable().add(cellInfoIdent, (IAddress)&cellInfo);

   builtInMSA().free(cellInfoIdent);
}

Name BuiltInExportTable::nameOfAddr(IAddress addr) const {
   for (UInt i = 0; i < nSlots(); ++i) {
      HashTableNode<Void*, BuiltInExportEntry, Name>* node = slot(i);
      while (node != NULL) {
         const BuiltInExportEntry& entry = node->entry_();
         if (entry._addr == addr)
            return entry._name;
         node = static_cast<HashTableNode<Void*, BuiltInExportEntry, Name>*>(node->next());
      }
   }

   return NULL_NAME;
}

void* BuiltInExportTable::allocNodePtr(size_t size, MSA& msa, Void*& link) {
   return (link = msa.alloc(size));
}

// Built-in name map

BuiltInExportTable* _builtInExportTable = NULL;

BuiltInExportTable& builtInExportTable(Void) {
   if (_builtInExportTable == NULL)
      _builtInExportTable = new(builtInMSA())
      BuiltInExportTable(BUILT_IN_EXPORT_TABLE_N_SLOTS);
   return *_builtInExportTable;
}
