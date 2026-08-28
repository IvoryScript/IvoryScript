/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    nameExprMap.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    An abstract class to represent a map of names to expdession values.
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

#ifndef IVORY_NAME_EXPR_MAP_H_DEFINED
#define IVORY_NAME_EXPR_MAP_H_DEFINED

#include "MSA.h"
#include "nameExprBinding.h"
#include "nameMap.h"

class NameExprMap : public Name_T_Map<Void*, Expr> {
public:
   NameExprMap(UInt nSlots);

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
   Void add(NameExprBinding entry, UInt hashVal, MSA& msa);
   Bool lookUp(Name nameId, UInt hashVal, Expr& res) const;

protected:
   virtual Void* allocNodePtr(size_t size, MSA& msa, Void*& link);
   virtual HashTableNode<Void*, NameBinding<Expr>, Name>& linkToNode(Void* link) const {
      return *(HashTableNode<Void*, NameBinding<Expr>, Name>*)link;
   }
};

extern NameExprMap& builtInNameMap();

extern NameExprMap* _builtInNameMap;

extern NameExprMap& builtInTypeConNameMap();

#endif /* IVORY_NAME_EXPR_MAP_H_DEFINED */
