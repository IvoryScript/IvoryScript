/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    frameDescr.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory stack frame descriptors.
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

#if (FRAME_DESCRIPTORS==1)

#ifndef IVORY_FRAME_DESCR_H_DEFINED
#define IVORY_FRAME_DESCR_H_DEFINED

#include "hashTable.h"
#include "ivory/common.h"

#define N_FRAME_DESCR_TABLE_SLOTS   47

template <class AddressT>
class FrameDescrSlot {
public:
   FrameDescrSlot(Name name, Type type)
    : _name(name), _type(type) {
   }

   inline Name name(Void) const { return _name; }
   inline Type type_(Void) const { return _type; }
   inline AddressT address(Void) const { return _addressGC; }

   inline Void setName(Name name) { _name = name; }
   inline Void setType(Type type_) { _type = type_; }
   inline Void setAddress(AddressT address) { _addressGC = address; }


protected:
   Name     _name;      // Name
   Type     _type;      // Type
   AddressT _addressGC; // Address for garbage collection
};

template <class AddressT>
class FrameDescr {
public:
   FrameDescr() : _next(NULL), _labSeqNo(0), _size(0), _nSlots(0), _slots(NULL) {}

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}


   const AddressT address(Void) const { return _address; }
   inline FrameDescr<AddressT>* next(Void) const { return _next; }
   inline FrameDescr<AddressT>*& next(Void) { return _next; }
   inline UInt labSeqNo(Void) const { return _labSeqNo; }
   inline UInt size(Void) const { return _size; }
   inline UInt nSlots(Void) const { return _nSlots; }
   inline const FrameDescrSlot<AddressT>& slot(UInt i) const { return _slots[i]; }
   inline FrameDescrSlot<AddressT>& slot(UInt i) { return _slots[i]; }

   inline Void setAddress(AddressT address) { _address = address; }
   inline Void setLabSeqNo(UInt n) { _labSeqNo = n; } //???
   inline Void setSize(UInt n) { _size = n; }
   inline Void setN_slots(UInt n) { _nSlots = n; }
   inline Void setSlots(FrameDescrSlot<AddressT>* slots) { _slots = slots;  }

protected:
   FrameDescr<AddressT>*      _next;      // Next in hash table chain
   AddressT                   _address;   // Associated address
   UInt                       _labSeqNo;  // ???
   UInt                       _size;      // Frame size
   UInt                       _nSlots;    // No. slots
   FrameDescrSlot<AddressT>*  _slots;
};

// FrameDescrTable

template <class AddressT>
class FrameDescrTable : public BasicChainedHashTable<FrameDescr<AddressT>*,
                               FrameDescr<AddressT>, AddressT> {
public:
   FrameDescrTable(UInt nSlots) : BasicChainedHashTable<FrameDescr<AddressT>*,
      FrameDescr<AddressT>, AddressT>(nSlots, NULL) {};

protected:
   virtual FrameDescr<AddressT>& linkToNode(FrameDescr<AddressT>* link) const {
      return *link;
   }


protected:
   virtual AddressT keyOf(const FrameDescr<AddressT>& entry) const {
      return entry.address();
   };
};

class BuiltInFrameDescrSlot : public FrameDescrSlot<Label> {
public:
   BuiltInFrameDescrSlot(Label label, Name name, Type type);
};

class BuiltInFrameDescr : public FrameDescr<Label> {
public:
   BuiltInFrameDescr(Label address,
                     size_t size,
                     UInt nSlots, BuiltInFrameDescrSlot* slots);

protected:
   BuiltInFrameDescr* _next;               // next in list

   static BuiltInFrameDescr* globalList;       // global list
   static Void initGlobalList(Void);

   friend Void postConstructorInit(Void);
};


extern FrameDescrTable<Label> builtInFrameDescrMap;

#endif /* IVORY_FRAME_DESCR_H_DEFINED */

#endif
