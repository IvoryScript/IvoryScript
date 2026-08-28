/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byteCodeModule.h
 *
 *
 * Author:  Alasdair Scott
 *
 * Original date: 18 November 2004
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory ByteCodeModule
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

#ifndef IVORY_BYTE_CODE_MODULE_H_DEFINED
#define IVORY_BYTE_CODE_MODULE_H_DEFINED

#include "ivory/module.h"
#include "ivory/ExportMap.h"

class ByteCodeModule : protected Module {
public:
   ByteCodeModule(Name name, Env* env,
                  NameExprMap* typeConNameMap, 
                  ExportMap* _exportMap,
                  IAddress segment, UInt rackOrigin);

   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
  
   virtual Bool lookUpTypeCon(Name name, const NameTable& nameTable, UInt hashVal,
                              TypeSig& typeSig);
   virtual IAddress lookUpImport(Name name, const Env& env);

protected:
   void init(Void);

protected:
   Name           _name;
   Env*           _env;
   NameExprMap*   _typeConNameMap;
   IAddress       _segment;
   UInt           _rackOrigin;
};

#endif /* IVORY_BYTE_CODE_MODULE_H_DEFINED */
