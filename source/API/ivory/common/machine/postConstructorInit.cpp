/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    postConstructorInit.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Perform any post constructor initialisation.
 *
 * Call format:
 *
 *    postConstructorInit()
 *
 * Method:
 *
 * Errors:
 *
 * Notes:
 *
 *    If DEBUG_INIT is defined, diagnostic information about built-in memory
 *    usage will be reported to 'outStream'.
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

#include "nameTable.h"
#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/class.h"
#include "ivory/dataCon.h"

#if (FRAME_DESCRIPTORS==1)
#include "ivory/frameDescr.h"
#endif

#include "ivory/module.h"
#include "ivory/trace.h"
#include "ivory/typeTable.h"

//#define DEBUG_INIT

Void postConstructorInit(Void) {

// Post constructor initialisation

   TypeDescr::initGlobalList();
   Class::initGlobalLists();
   Module::initGlobalList();
   TypeCon::initBuiltInList();
   BuiltInDataCon::initGlobalList();
   BuiltInFn::initGlobalList();

#if (FRAME_DESCRIPTORS==1)
   BuiltInFrameDescr::initGlobalList();
#endif

   builtInNameCount = builtInNameTable().count();
   builtInTypeCount = builtInTypeTable().count();

#ifdef DEBUG_INIT
   for (Type i = 0; i < builtInTypeCount; i++)
   {
      TypeDescr* descr = builtInTypeTable().typeDescr(i);
      traceExpr(descr->_typeSig, Env(builtInNameTable()));
   }
   outStream << "Primitive memory usage\n";
   builtInMSA().info(outStream, 0);
#endif
}
