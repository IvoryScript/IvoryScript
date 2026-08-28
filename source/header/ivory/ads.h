/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ads.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with active data storage.
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

#ifndef IVORY_ADS_H_DEFINED
#define IVORY_ADS_H_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/env.h"
#include "ivory/ref.h"
#include "ivory/type.h"

// Common supertype for active data store instances

class ADS_Instance : public Env {
public:
   ADS_Instance(const ADSRef& host);

   ~ADS_Instance();

   void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr, MSA& msa)
#else
   void operator delete(void* ptr)
#endif
   {
   }

public:
   ADSRef _host;
};


declareBuiltInFn(createObjectADS);

declareBuiltInFn(host);

declareBuiltInFn(mapProcObjectsADS);

declareBuiltInFn(selectADS);

declareTypeCon(ADS);

declareType(ADS);

declareEntry(addRegisterEntry_ADS);

declareAltEntry(addRegisterEntry_ADS);

declareEntry(hasRegisterEntry_ADS);

declareAltEntry(hasRegisterEntry_ADS);

declareEntry(removeRegisterEntry_ADS);

declareAltEntry(removeRegisterEntry_ADS);

declareBuiltInDataCon(ADS);

declareBuiltInFn(adsiToEnv);

extern Expr hostADS(const ADS_Instance& adsi, MSA& msa);

#endif /* IVORY_ADS_H_DEFINED */
