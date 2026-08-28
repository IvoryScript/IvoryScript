/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    nameAnyMap.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    An abstract class to represent a map of names to Any values.
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

#ifndef IVORY_NAME_ANY_MAP_H_DEFINED
#define IVORY_NAME_ANY_MAP_H_DEFINED

#include "ivory/any.h"
#include "ivory/nameAnyBinding.h"
#include "ivory/nameMap.h"

class NameAnyMap : public Name_T_Map<Void*, Any> {
public:
   NameANyMap(UInt nSlots);

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
   Void add(NameAnyBinding entry, UInt hashVal, MSA& msa);
   Bool lookUp(Name nameId, UInt hashVal, NameAnyBinding& res) const;

protected:
   Void* allocNode(size_t size, MSA& msa, Void*& link);
};

#endif /* IVORY_NAME_ANY_MAP_H_DEFINED */
