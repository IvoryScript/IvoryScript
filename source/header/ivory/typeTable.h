/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the TypeTable class.
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
 *
 * Modification history:
 *
 *    02/07/03 -  AJS   Now supports new types after C++ constructor phase.
 *
 */

#ifndef IVORY_TYPE_TABLE_H_DEFINED
#define IVORY_TYPE_TABLE_H_DEFINED

#include "../header/segmentTable.h"
#include "../header/vector.h"
#include "ivory/type.h"

class TypeDescr;

// TypeTable: A table of types

class TypeTable {
public:
   TypeTable(UInt nSlots, UInt nPerSeg, MSA& msa);
   ~TypeTable(Void);

   Void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   Void operator delete(Void* ptr, MSA& msa);
#endif
   Void operator delete(Void* ptr);

   inline UInt nSlots(Void) const { return _nSlots; }
   inline UInt nPerSeg(Void) const { return _nPerSeg; }

   inline MSA& msa(Void) const { return _msa; }
   inline UInt count(Void) const { return _count; }

   Type lookUp(TypeSig typeSig) const;
   TypeDescr* addType(TypeSig typeSig, int hashVal);
   Void dropType(Type type);
   TypeSig typeSig(Type type) const;
   UInt hashVal(Type type) const;

   Type altType(Type) const;
   Void setAltType(Type type, Type altType);

   Void load(Archive& archive, NameTable& nameTable, MSA& msa);
   Void postLoad(const SegmentTable& segmentTable);

   Void store(Archive& archive, const NameTable& nameTable) const;
   TypeDescr* lookUp(const TypeSig typeSig, UInt hashVal) const;
   Type lookUpByAlt(Type altType, UInt hashVal) const;
   Type addDescr(TypeDescr* descr);
   TypeDescr* addDescr(Type type, UInt hashVal, TypeSig typeSig, UInt refCount);
   Void enterDescr(TypeDescr* descr);
   TypeDescr* typeDescr(Type type) const;
   Type nextType();

protected:
   UInt                 _nSlots;
   UInt                 _nPerSeg;
   MSA&                 _msa;
   TypeDescr**          _hashTab;
   vector<TypeDescr*>   _vec;
   UInt                 _count;
   Type                 _minFreeType;
   Type                 _maxType;

   friend class TypeDescr;
   friend Type mkType(TypeSig (*mkTypeSig)(Void), TypeMethod_initFn initFn);
   friend Type mkType(TypeSig typeSig);
};

#endif /* IVORY_TYPE_TABLE_H_DEFINED */
