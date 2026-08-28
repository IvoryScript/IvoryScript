/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    dataCon.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common definitions for use with Ivory data constructors.
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

#ifndef IVORY_DATA_CON_H_DEFINED
#define IVORY_DATA_CON_H_DEFINED

#include "ivory/common.h"

// Data Constructor

class DataCon {
public:
   DataCon(Name name, Tag tag, TypeSig typeSig, TypeSig reprTypeSig);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Name name(Void) const { return _name; }
   inline Tag tag(Void) const { return _tag; }
   inline Bool sumType(Void) const { return _tag > 0 || _next != NULL; }
   inline const DataCon& next(Void) const { return *_next; }
   inline DataCon*& next(Void) { return _next; }

   inline const TypeSig typeSig(Void) const { return _typeSig; }
   inline TypeSig& typeSig(Void) { return _typeSig; }

   inline UInt arity(Void) const { return _arity; }

   inline const TypeSig reprTypeSig(Void) const { return _reprTypeSig; }
   inline TypeSig& reprTypeSig(Void) { return _reprTypeSig; }

   static ConstString selectorName(Name name, UInt index,
                                   const NameTable& nameTable, MSA& msa);

protected:
   Name        _name;
   Tag         _tag;
   TypeSig     _typeSig;
   Type        _type;
   UInt        _arity;
   TypeSig     _reprTypeSig;
   Type        _reprType;
   DataCon*    _next;      // next in data constructor list for type
};

#define isDataCon(e) (formOf(e)==BUILT_IN_DATA_CON||formOf(e)==DATA_CON)

#endif /* IVORY_DATA_CON_H_DEFINED */
