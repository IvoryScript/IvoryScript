/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    nameMap.h  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    An abstract class to represent a map of names to generic type values.
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

#ifndef IVORY_NAME_MAP_H_DEFINED
#define IVORY_NAME_MAP_H_DEFINED

#include "hashTable.h"
#include "ivory/nameBinding.h"

template <class Link_T, class T>
class Name_T_Map : public HashTable<Link_T, NameBinding<T>, Name> {
public:
   Name_T_Map(UInt nSlots, Link_T nullLink)
      : HashTable<Link_T, NameBinding<T>, Name>(nSlots, nullLink) {
   }
   Name_T_Map(const Name_T_Map<Link_T, T>& src)
      : HashTable<Link_T, NameBinding<T>, Name>(src) {
   }

protected:
   virtual Name keyOf(const NameBinding<T>& entry) const { return entry.name(); }

};

#endif /* IVORY_NAME_MAP_H_DEFINED */
