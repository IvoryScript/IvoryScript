/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    typeTable.cpp
 *
 * Module:  Ivory common (type)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 5 July 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of TypeTable class
 *
 * Notes:
 *
 *    The TypeTable class is primarily designed to type information
 *    for an environment.
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
 *    02/07/03 -  AJS   Now supports new types after C++ constructor phase.
 *
 */

#include "archive.h"
#include "ivory/bool.h"
#include "ivory/streams.h"
#include "ivory/trace.h"
#include "ivory/type.h"
#include "ivory/typeTable.h"

TypeTable::TypeTable(UInt nSlots, UInt nPerSeg, MSA& msa)
   : _nSlots(nSlots), _nPerSeg(nPerSeg),
   _msa(msa),
   _vec(nPerSeg), _count(0),
   _minFreeType(0), _maxType(0) {
   _hashTab = (TypeDescr**)msa.alloc(nSlots * sizeof(TypeDescr*));
   memset(_hashTab, 0, nSlots * sizeof(TypeDescr*));
}

TypeTable::~TypeTable(Void) {
   for (Type i = 0; i < _maxType; i++) {
      TypeDescr* descr;
      if ((descr = _vec.get(i)) != NULL) {
         destroyTypeSig(descr->_typeSig, _msa);
         _msa.free(descr);
      }
   }
   _msa.free(_hashTab);
   _vec.preDestruct(_msa);
}

// T.B.D. Make following inline

Void* TypeTable::operator new(size_t size, MSA& msa) {
   return msa.alloc(size);
}

#ifdef DELETE_NEEDS_NEW_ARG
Void TypeTable::operator delete(Void* ptr, MSA& msa) {
   msa.free(ptr);
}
#endif
Void TypeTable::operator delete(Void* ptr) {
   ((TypeTable*)ptr)->_msa.free(ptr);
}

TypeDescr* TypeTable::typeDescr(Type type) const {
   return _vec.get(type);
}

UInt TypeTable::hashVal(Type type) const {
   TypeDescr* descr;
   if (type >= _maxType || (descr = _vec.get(type)) == NULL) {
      error("TypeTable::hashVal: type not in use");
      return 0;
   }
   return descr->_hashVal;
}

Type TypeTable::altType(Type type) const {
   TypeDescr* descr;
   if (type >= _maxType || (descr = _vec.get(type)) == NULL) {
      error("TypeTable::altType: type not in use");
      return NULL_TYPE;
   }
   return descr->_altType;
}

Type TypeTable::lookUp(TypeSig typeSig) const {
   UInt hashVal = hashTypeSig(typeSig);
   const TypeDescr* descr;
   if ((descr = lookUp(typeSig, hashVal)) == NULL)
      return NULL_TYPE;
   return descr->_type;
}

TypeDescr* TypeTable::lookUp(const TypeSig typeSig, UInt hashVal) const {
   TypeDescr* descr;
   for (descr = _hashTab[hashVal % _nSlots]; descr != NULL; descr = descr->_next)
      if (eqTypeSig(typeSig, descr->_typeSig))
         return descr;
   return NULL;
}

Type TypeTable::lookUpByAlt(Type altType, UInt hashVal) const {
   TypeDescr* descr;
   for (descr = _hashTab[hashVal % _nSlots]; descr != NULL; descr = descr->_next)
      if (altType == descr->_altType)
         return descr->_type;
   return NULL_TYPE;
}

TypeSig TypeTable::typeSig(Type type) const {
   TypeDescr* descr;
   if (type >= _maxType || (descr = _vec.get(type)) == NULL) {
      error("TypeTable::typeSig: type not in use");
      return ERROR;
   }
   return descr->_typeSig;
}

TypeDescr* TypeTable::addType(TypeSig typeSig, int hashVal) {
   Type type = nextType();
   return addDescr(type, hashVal, copyTypeSig(typeSig, _msa), 1);
}

Type TypeTable::addDescr(TypeDescr* descr) {
   descr->_type = nextType();
   descr->_refCount = 1;
   _vec.put(descr->_type, descr, _msa);
   _count++;
	return descr->_type;
}

static Ptr mapFn(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Cell* cell_ = *(Cell**)ptr;

   CellCopyFn copyCellFn = ((CellInfo*)(cell_->tag()))->cellCopyFn();
   if (copyCellFn != NULL) {
      Ptr ptr = dstEnv.msa().alloc(sizeof(Ptr));
      *(Cell**)ptr = (*copyCellFn)(*cell_, ptrEnv, dstEnv, dstEnv.msa() n_vm);
      return ptr;
   } else {
      throw(RunTimeError("mapFn: Cell can\'t be copied"));
      return NULL;
   }
}

#if (GARBAGE_COLLECTION==1)
static Void markFn(TypeDescr* typeDescr, Ptr ptr, Env& ptrEnv argN_VM) {
   Cell* cell_ = *(Cell**)ptr;

   Label label = ((CellInfo*)(cell_->tag()))->cellGC_MarkFn();

   if (label != NULL) {
      cell = cell_;
      cellEnv = &ptrEnv;
      pushLabel(NULL);
      callSequence$(label n_vm);
   }
}
#endif

static Void insertTxtFn(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   putString(*os, osEnv, "*** Function ***" n_vm);
}

#if (SERIALISATION==1)
static Ptr extractBinFn(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   Cell* cell_ = extractBinCell(*is, *static_cast<Env*>((Void*)&isEnv));

   Ptr ptr = dstEnv.msa().alloc(sizeof(Ptr));
   *(Cell**)ptr = cell_;
   return ptr;
}

static Void insertBinFn(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   Cell* cell_ = *(Cell**)ptr;
   insertBinCell(*os, *static_cast<Env*>((Void*)&osEnv), *cell_, env argN_VM);
}
#endif



TypeDescr* TypeTable::addDescr(Type type, UInt hashVal, TypeSig typeSig,
                               UInt refCount) {
   TypeDescr* descr = new(_msa) TypeDescr(typeSig);
   if (descr == NULL) {
      error("TypeTable::add: no memory");
      return NULL;
   }
   TypeDescr* next = _hashTab[hashVal % _nSlots];
   if ((descr->_next = next) != NULL)
      next->_prev = descr;
   descr->_prev = NULL;
   descr->_typeSig = typeSig;
   descr->_hashVal = hashVal;
   descr->_type = type;
   descr->_refCount = refCount;

   if (isArrowTypeSig(descr->_typeSig)) {
      descr->_mapFn = mapFn;

#if (GARBAGE_COLLECTION==1)
      descr->_markFn = markFn;
#endif

      descr->_insertTxtFn =     insertTxtFn;

#if (SERIALISATION==1)
      descr->_insertBinFn = insertBinFn;
      descr->_extractBinFn = extractBinFn;
#endif
 
   }

   _hashTab[hashVal % _nSlots] = descr;
   _vec.put(descr->_type, descr, _msa);
   _count++;
   return descr;
}

Void TypeTable::enterDescr(TypeDescr* descr) {
   descr->_hashVal = hashTypeSig(descr->_typeSig);
   if ((descr->_next = _hashTab[descr->_hashVal % _nSlots]) != NULL)
      descr->_next->_prev = descr;
   descr->_prev = NULL;
   _hashTab[descr->_hashVal % _nSlots] = descr;
}

Void TypeTable::dropType(Type type) {
   TypeDescr* descr;
   if (type >= _maxType || (descr = _vec.get(type)) == NULL) {
      error("TypeTable::drop: type not in use");
      return;
   }
   if (--descr->_refCount == 0) {
      if (descr->_next != NULL)
         descr->_next->_prev = descr->_prev;
      if (descr->_prev != NULL)
         descr->_prev->_next = descr->_next;
      else
         _hashTab[descr->_hashVal % _nSlots] = descr->_next;
       _msa.free(descr);
      _vec.put(type, (TypeDescr*)NULL, _msa);
      if (type < _minFreeType)
         _minFreeType = type;
      _count--;
   }     
}

Type TypeTable::nextType() {
   for (;_minFreeType < _maxType; _minFreeType++)
      if (_vec.get(_minFreeType) == NULL)
         return _minFreeType++;

   return _minFreeType++, _maxType++;
}

Void TypeTable::load(Archive& archive, NameTable& nameTable, MSA& msa) {
   _count = archive.extractVLU();

   for (UInt i = 0; i < _count; i++) {
      Type type = (Int)archive.extractVLU();
      while (_maxType < type) {
         _vec.put(_maxType, NULL, _msa);
         _maxType++;
      }
      Byte kind;
      archive >> kind;
      const TypeDescr::Loader* loader = TypeDescr::Loader::loader((TypeDescr::Kind)kind);
      TypeDescr* descr = loader != NULL
         ? loader->load(archive, nameTable, msa)
         : NULL;
      descr->_type = type;
      TypeDescr* builtInDescr = builtInTypeTable().lookUp(descr->typeSig(), descr->hashVal());
      if (builtInDescr != NULL)
         descr->setAltType(builtInDescr->type());
      descr->_refCount = archive.extractVLU();
      UInt slot = descr->_hashVal % _nSlots;
      if ((descr->_next = _hashTab[slot]) != NULL)
         descr->_next->_prev = descr;
      descr->_prev = NULL;
      _hashTab[slot] = descr;
      _vec.put(type, descr, _msa);
      _maxType = type + 1;
   }
   _minFreeType = 0;
}

// TypeTable::postLoad: Invoked after segments loader to adjust label offsets

Void TypeTable::postLoad(const SegmentTable& segmentTable) {
   for (UInt i = 0; i < (UInt)_maxType; i++) {
      TypeDescr* descr = _vec.get(i);
      if (descr != NULL)
         descr->postLoad(segmentTable);
   }
}

Void TypeTable::store(Archive& archive, const NameTable& nameTable) const {
   archive.insertVLU(_count);
   UInt archiveCount = 0;
   if (_count > 0)
      for (UInt i = 0; i < (UInt)_maxType; i++) {
         TypeDescr* descr;
         if ((descr = _vec.get(i)) != NULL) {
            archive.insertVLU(i);
            archive << (Byte)descr->kind();
            descr->store(archive, nameTable);
            archive.insertVLU(descr->_refCount);
            archiveCount++;
         }
      }
   if (_count != archiveCount)
      error("TypeTable::store: count mismatch");
}
