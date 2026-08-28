/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    TransientADS.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with transient active data storage.
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
 */

#ifndef IVORY_TRANSIENT_ADS_H_DEFINED
#define IVORY_TRANSIENT_ADS_H_DEFINED

#include "objStore.h"
#include "ivory/basicADS.h"
#include "ivory/machine.h"
#include "ivory/transientDataStore.h"

class TransientADS_Instance : public BasicADS_Instance {
public:
   TransientADS_Instance(const ADSRef& host,
      size_t blkLen,
      UInt nameSlots, UInt namesPerSeg,
      UInt typeSlots, UInt typesPerSeg,
      UInt objsPerSeg, size_t clusterBlkLen,
      UInt registerSlots
      argN_VM);
   TransientADS_Instance(size_t blkLen,
      NameTable* nameTbl, TypeTable* typeTable,
      UInt objsPerSeg, size_t clusterBlkLen,
      UInt registerSlots
      argN_VM);
   ~TransientADS_Instance();

   Void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   Void operator delete(Void* ptr, MSA& msa)
#else
   Void operator delete(Void* ptr)
#endif
   {
   }

   virtual BasicDataStore& dataStore(Void) { return (BasicDataStore&)_dataStore; }
   virtual ObjStore& objStore(Void) { return _objStore; }

   Void createRegister(UInt registerSlots argN_VM);

   Void load(Archive& archive);
   Void store(Archive& archive);
   Void print(ostream& os, UInt indent);
   Void info(ostream& os, UInt indent);

public:
   TransientDataStore   _dataStore;
   ObjStore             _objStore;
};

declareTypeCon(TransientADS_Instance);

declareType(TransientADS_Instance);

declareBuiltInDataCon(TransientADS_Instance);

declareBuiltInFn(createTransientObject);

declareBuiltInFn(transientADSItoADS_Instance);

#endif /* IVORY_TRANSIENT_ADS_H_DEFINED */
