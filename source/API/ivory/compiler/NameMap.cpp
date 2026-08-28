/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    NameMap.cpp   
 *
 * Module:  IvoryScript compiler  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2019
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class to maintain a bidirectional mapping of names
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

#include "ivory/compiler/NameMap.h"
#include "ivory/compiler/type.h"

NameMap::NameMap(UInt nSlots)
   : HashTable<Void*, NameMapEntry, Name>(nSlots, NULL) {
}

Void NameMap::add(NameMapEntry& entry, UInt hashVal, MSA& msa) {
   HashTable<Void*, NameMapEntry, Name>::add(entry, hashVal, msa);
}

Void* NameMap::allocNodePtr(size_t size, MSA& msa, Void*& link) {
   link = msa.alloc(size);
   return link;
}

Void BidirNameMap::add(Name fromName, Name toName, MSA& msa) {
   UInt fromHashVal =   NameMap::hash(fromName);
   UInt toHashVal =     NameMap::hash(toName);
   NameMapEntry fwdEntry = NameMapEntry(fromName, toName);
   NameMapEntry invEntry = NameMapEntry(toName, fromName);
   _fwdMap.add(fwdEntry,  fromHashVal,   msa);
   _invMap.add(invEntry,  toHashVal,     msa);
}

Name BidirNameMap::lookUp(Name name) const {
   const NameMapEntry* entry = _fwdMap.lookUp(name, NameMap::hash(name));
   return entry != NULL
      ? entry->_toName
      : NULL_NAME;
}

Name BidirNameMap::invLookUp(Name name) const {
   const NameMapEntry* entry = _invMap.lookUp(name, NameMap::hash(name));
   return entry != NULL
      ? entry->_toName
      : NULL_NAME;
}

Name BidirNameMap::use(Name name, NameTable& nameTable,
                       Name (*newName)(Void),
                       MSA& msa) {
   Name res = lookUp(name);
   if (res != NULL_NAME)
      return res;
   else {
      res = name >= 0
         ? mapName(name, _nameTable, nameTable)
         : newName();
      add(name, res, msa);
      return res;
   }
}

BidirNameMap* getBirNameMap(NameTable& nameTable, BidirNameMap*& nameMaps, MSA& msa) {
   BidirNameMap* nameMap = nameMaps;
   while (nameMap != NULL) {
      if (&nameMap->_nameTable == &nameTable)
         return nameMap;
      nameMap = nameMap->_next;
   }
   nameMap = new (msa) BidirNameMap(nameMaps, nameTable, NAME_MAP_N_SLOTS);
   nameMaps = nameMap;
   return nameMap;
}

const BidirNameMap* getBirNameMap(const NameTable& nameTable, const BidirNameMap* nameMaps) {
   const BidirNameMap* nameMap = nameMaps;
   while (nameMap != NULL) {
      if (&nameMap->_nameTable == &nameTable)
         return nameMap;
      nameMap = nameMap->_next;
   }
   return NULL;
}
