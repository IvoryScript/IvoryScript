/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    basicADS.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with basic active data storage
 * 
 *    Currently, it includes transient and persistent forms
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

#ifndef IVORY_BASIC_ADS_H_DEFINED
#define IVORY_BASIC_ADS_H_DEFINED

#include "objStore.h"
#include "ivory/ads.h"
#include "ivory/basicDataStore.h"
#include "ivory/propertySet.h"

class BasicADS_Instance : public ADS_Instance {
public:
   BasicADS_Instance(const ADSRef& host);
   ~BasicADS_Instance();

   void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr, MSA& msa)
#else
   void operator delete(void* ptr)
#endif
   {
   }
   virtual BasicDataStore& dataStore(Void) = 0;
   virtual ObjStore& objStore(Void) = 0;

   void info(ostream& os, UInt indent);

public:
   Ref               _root;
};


class BasicADS_Repr : public PropertySet {
public:
   BasicADS_Repr(Void)
      : PropertySet(), _ins(NULL) {
   }
   BasicADS_Repr(const BasicADS_Repr& src, const Env& srcEnv, Env& env, MSA& msa);
   void BasicADS_Repr$(argVM);
   Void destroy(Env& env, MSA& msa);
   ADS_Instance* ins(void) { return _ins; };

protected:
   Void instantiate(size_t blkLen,
      NameTable* nameTbl, TypeTable* typeTbl,
      UInt objsPerSeg, size_t clusterBlkLen, MSA& msa);
   Void instantiate(Ref host, ADS_Instance& env, MSA& msa argN_VM);

protected:
   ADS_Instance* _ins;

   friend ADS_Instance& lockADSInstance(Ref ads, ADS_Instance& adsi argN_VM);
   friend Label createObjectADS$altEntry(argVM);
   friend Label createObjectADS_l1(argVM);
   friend Label createObjectADS_l2(argVM);
   friend Label mapProcObjectsADS$altEntry(argVM);
   friend Label mapProcObjectsADS_l1(argVM);
   friend Label addRegisterEntry_ADS$altEntry(argVM);
   friend Label selectADS_l1(argVM);
   friend Label hasRegisterEntry_ADS$altEntry(argVM);
   friend Label removeRegisterEntry_ADS$altEntry(argVM);
};

declareTypeCon(ADS_Instance);

declareType(ADS_Instance);

#endif /* IVORY_BASIC_ADS_H_DEFINED */
