/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    builtIn.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common definitions for use with Ivory built-in export table.
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

#ifndef IVORY_BUILT_IN_H_DEFINED
#define IVORY_BUILT_IN_H_DEFINED

#include "hashTable.h"
#include "ivory/common.h"

#define BUILT_IN_CLOSURE_SUFFIX        "$fn"
#define BUILT_IN_ENTRY_SUFFIX          "$entry"
#define BUILT_IN_ALT_ENTRY_SUFFIX      "$altEntry"
#define BUILT_IN_PAP_CELL_INFO_SUFFIX  "$PAP_cellInfo"

#define BUILT_IN_EXPORT_TABLE_N_SLOTS  47

class BuiltInExportEntry {
public:
   BuiltInExportEntry(Name name, IAddress addr)
      : _name(name), _addr(addr) {}

public:
   Name     _name;
   IAddress _addr;

   friend class ClassInstances;
};

class BuiltInExportTable : public HashTable<Void*, BuiltInExportEntry, Name> {
public:
   BuiltInExportTable(UInt nSlots);

   inline
      void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   Void operator delete(Void* ptr, MSA& msa) {
#else
   Void operator delete(Void* ptr) {
#endif
   }

   Void add(ConstString ident, IAddress addr);
   Void addCell(ConstString ident, const Cell& cell_, const struct CellInfo& cellInfo);
   Void addPAPCellInfo(ConstString ident, const struct CellInfo& cellInfo);
   Name nameOfAddr(IAddress addr) const;

protected:
   virtual Void* allocNodePtr(size_t size, MSA& msa, Void*& link);
   virtual HashTableNode<Void*, BuiltInExportEntry, Name>& linkToNode(Void* link) const {
      return *(HashTableNode<Void*, BuiltInExportEntry, Name>*)link;
   }

protected:
   virtual Name keyOf(const BuiltInExportEntry& entry) const { return entry._name; }

public:
   static UInt hash(Name name) {
      return name;
   }
};

extern BuiltInExportTable& builtInExportTable(Void);

#endif /* IVORY_BUILT_IN_H_DEFINED */
