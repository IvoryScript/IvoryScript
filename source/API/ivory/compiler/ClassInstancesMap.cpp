/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ClassInstancesMap.cpp
 *
 * Module:  Ivory Compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 11 March 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of C++ class to maintain a mapping of Ivory class name
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

#include "ivory/compiler/ClassInstancesMap.h"

ClassInstancesMap::ClassInstancesMap(UInt nSlots)
   : HashTable<Void*, ClassInstancesMapEntry, Name>(nSlots, NULL) {
}

Void ClassInstancesMap::add(ClassInstancesMapEntry& entry, UInt hashVal, MSA& msa) {
   HashTable<Void*, ClassInstancesMapEntry, Name>::add(entry, hashVal, msa);
}

Void* ClassInstancesMap::allocNodePtr(size_t size, MSA& msa, Void*& link) {
   link = msa.alloc(size);
   return link;
}
