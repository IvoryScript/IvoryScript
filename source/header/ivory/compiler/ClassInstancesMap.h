/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ClassInstancesMap.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 11 March 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definition of C++ class to maintain a mapping of Ivory class name
 *    to a list of instances.
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

#ifndef IVORY_COMPILER_CLASS_INSTANCES_MAP_H_DEFINED
#define IVORY_COMPILER_CLASS_INSTANCES_MAP_H_DEFINED

#include "hashTable.h"
#include "ivory/common.h"

class InstanceDefn;

class ClassInstancesMapEntry {
public:
   ClassInstancesMapEntry(Name className , InstanceDefn* instanceDefns)
      : _className(className), _instanceDefns(instanceDefns) {}

public:
   Name           _className;
   InstanceDefn*  _instanceDefns;

friend class ClassInstances;
};

class ClassInstancesMap : public HashTable<Void*, ClassInstancesMapEntry, Name> {
public:
   ClassInstancesMap(UInt nSlots);
   Void add(ClassInstancesMapEntry& entry, UInt hashVal, MSA& msa);

protected:
   virtual Void* allocNodePtr(size_t size, MSA& msa, Void*& link);
   virtual HashTableNode<Void*, ClassInstancesMapEntry, Name>& linkToNode(Void* link) const {
      return *(HashTableNode<Void*, ClassInstancesMapEntry, Name>*)link;
   }

protected:
   virtual Name keyOf(const ClassInstancesMapEntry& entry) const { return entry._className; }

public:
   static UInt hash(Name name) {
      return -name;
   }
};

#endif /* IVORY_COMPILER_CLASS_INSTANCES_MAP_H_DEFINED */
