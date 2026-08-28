/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    DAG_Map.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2019
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definition of class for a map of DAG vertices
 *    from one environment to another
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

#ifndef IVORY_COMPILER_DAG_MAP_H_DEFINED
#define IVORY_COMPILER_DAG_MAP_H_DEFINED

#include "hashTable.h"
#include "ivory/common.h"

class DAG_MapEntry {
public:
   DAG_MapEntry(const Void* fromAddr, Void* toAddr)
      : _fromAddr(fromAddr), _toAddr(toAddr) {}

public:
   const Void* _fromAddr;
   Void*       _toAddr;

friend class DAG_Map;
};


class DAG_Map : public HashTable<Void*, DAG_MapEntry, const Void*> {
public:
   DAG_Map(UInt nSlots);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

protected:
   virtual Void* allocNode(size_t size, MSA& msa, Void*& link);

protected:
   inline const Void* keyOf(const DAG_MapEntry& entry) const { return entry._fromAddr; }

public:
   static UInt hash(const Void* vertex) {
      return (uintptr_t)vertex >> 2;
   }
};

#endif /* IVORY_COMPILER_DAG_MAP_H_DEFINED */
