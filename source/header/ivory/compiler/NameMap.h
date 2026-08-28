/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    NameMap.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2019
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Declaration of class to maintain a mapping of non-alphabetic names
 *    from one environment to another.
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

#ifndef IVORY_COMPILER_NAME_MAP_H_DEFINED
#define IVORY_COMPILER_NAME_MAP_H_DEFINED

#define NAME_MAP_N_SLOTS   47

#include "hashTable.h"
#include "ivory/common.h"

class CopyContext;

class NameMapEntry {
public:
   NameMapEntry(Name fromName, Name toName)
      : _fromName(fromName), _toName(toName) {}

public:
   Name  _fromName;
   Name  _toName;

friend class NameMap;
};

class NameMap : public HashTable<Void*, NameMapEntry, Name> {
public:
   NameMap(UInt nSlots);
   Void add(NameMapEntry& entry, UInt hashVal, MSA& msa);

protected:
   virtual Void* allocNodePtr(size_t size, MSA& msa, Void*& link);
   virtual HashTableNode<Void*, NameMapEntry, Name>& linkToNode(Void* link) const {
      return *(HashTableNode<Void*, NameMapEntry, Name>*)link;
   }

protected:
   virtual Name keyOf(const NameMapEntry& entry) const { return entry._fromName; }

public:
   static UInt hash(Name name) {
      return -name;
   }
};

class BidirNameMap {
public:
   BidirNameMap(BidirNameMap* next, NameTable& nameTable, UInt nSlots)
      : _next(next),
        _nameTable(nameTable),
        _fwdMap(nSlots), _invMap(nSlots) {}
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   Void add(Name fromName, Name toName, MSA& msa);

   Name lookUp(Name name) const;
   Name invLookUp(Name name) const;
   Name use(Name name, NameTable& nameTable,
            Name(*newName)(Void),
            MSA& msa);

protected:
   BidirNameMap*     _next;
   const NameTable&  _nameTable;
   NameMap           _fwdMap;
   NameMap           _invMap;

   friend BidirNameMap* getBirNameMap(NameTable& nameTable, BidirNameMap*& nameMaps, MSA& msa);
   friend const BidirNameMap* getBirNameMap(const NameTable& nameTable, const BidirNameMap* nameMaps);
};

extern BidirNameMap* getBirNameMap(NameTable& nameTable, BidirNameMap*& nameMaps, MSA& msa);
extern const BidirNameMap* getBirNameMap(const NameTable& nameTable, const BidirNameMap* nameMaps);


#endif /* IVORY_COMPILER_NAME_MAP_H_DEFINED */
