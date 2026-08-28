/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    module.h  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 18 November 2004
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory modules.
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

#ifndef IVORY_MODULE_H_DEFINED
#define IVORY_MODULE_H_DEFINED

#include "ivory/common.h"
#include "ivory/ExportMap.h"
#include "ivory/nameExprMap.h"

class Module {

public:
   static Bool typeConSig(Name name, const NameTable& nameTable, UInt hashVal,
                          TypeSig& typeSig);
   static IAddress importNameAddr(Name name, const Env& env);

protected:
   Module(Void);
   Module(ExportMap* exportMap);
   virtual Void init(Void) = 0;

protected:
   virtual Bool lookUpTypeCon(Name name, const NameTable& nameTable, UInt hashVal,
                              TypeSig& typeSig) = 0;
   virtual IAddress lookUpImport(Name name, const Env& env) = 0;

protected:
   Module* _next;               // next in list
   ExportMap* _exportMap;

   static Module* globalList;       // global module list
   static Void initGlobalList(Void);

   friend Void postConstructorInit(Void);
   friend Void dummyModuleInit(Void);
};

#endif /* IVORY_MODULE_H_DEFINED */
