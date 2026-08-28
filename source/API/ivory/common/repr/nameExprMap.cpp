/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    nameExprMap.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of NameExprMap class.
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

#include <string.h>
#include "ivory/bool.h"
#include "ivory/nameExprMap.h"

#define BUILT_IN_NAME_MAP_N_SLOTS            127
#define BUILT_IN_TYPE_CON_NAME_MAP_N_SLOTS   127

NameExprMap::NameExprMap(UInt nSlots)
   : Name_T_Map<Void*, Expr>(nSlots, NULL) {
}

Void* NameExprMap::allocNodePtr(size_t size, MSA& msa, Void*& link) {
   return (link = msa.alloc(size));
}

Void NameExprMap::add(NameExprBinding entry, UInt hashVal, MSA& msa) {
   Name_T_Map<Void*, Expr>::add(entry, hashVal, msa);
}

Bool NameExprMap::lookUp(Name name, UInt hashVal, Expr& res) const {
   const NameExprBinding* binding = Name_T_Map<Void*, Expr>::lookUp(name, hashVal);

   if (binding != NULL) {
      res = binding->val();
      return TRUE;
   }
   else
      return FALSE;
}

// Built-in name map

NameExprMap* _builtInNameMap = NULL;

NameExprMap& builtInNameMap() {
   if (_builtInNameMap == NULL)
      _builtInNameMap = new(builtInMSA())
         NameExprMap(BUILT_IN_NAME_MAP_N_SLOTS);
   return *_builtInNameMap;
}

// Built-in type constructor name map

NameExprMap* _builtInTypeConNameMap = NULL;

NameExprMap& builtInTypeConNameMap() {
   if (_builtInTypeConNameMap == NULL)
      _builtInTypeConNameMap = new(builtInMSA())
         NameExprMap(BUILT_IN_TYPE_CON_NAME_MAP_N_SLOTS);
   return *_builtInTypeConNameMap;
}
