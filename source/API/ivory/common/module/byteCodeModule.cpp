/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byteCodeModule.cpp
 *
 * Module:  Ivory common (module)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 18 November 2004
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of ByteCodeModule C++ class.
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
 */

#include <string.h>
#include "nameTable.h"
#include "ivory/byteCodeModule.h"
#include "ivory/type.h"

//#define DEBUG_INIT

#ifdef DEBUG_INIT
   #include "ivory/trace.h"
#endif

ByteCodeModule::ByteCodeModule(Name name, Env* env,
                               NameExprMap*  typeConNameMap,
                               ExportMap* exportMap,
                               IAddress segment, UInt rackOrigin)
   : Module(exportMap), _name(name), _env(env),
     _typeConNameMap(typeConNameMap),
     _segment(segment), _rackOrigin(rackOrigin) {
}

void ByteCodeModule::init() {
}

Bool ByteCodeModule::lookUpTypeCon(Name name, const NameTable& nameTable, UInt hashVal,
                                   TypeSig& typeSig) {
   assert(name >= builtInNameCount, "ByteCodeModule::lookUpTypeCon: unexpected");
   if (&nameTable != &_env->nameTable() && name >= builtInNameCount) {
      const char* str = nameTable.string(name - builtInNameCount);
      if (str == NULL || (name = _env->nameTable().lookUp(str)) == NULL_NAME)
         return FALSE;
      name += builtInNameCount;
   }      
   return _typeConNameMap->lookUp(name, hashVal, typeSig);
}

IAddress ByteCodeModule::lookUpImport(Name name, const Env& env) {
   if (&env != _env && name >= builtInNameCount) {
      const char* str = env.nameTable().string(name - builtInNameCount);
      if (str == NULL || (name = _env->nameTable().lookUp(str)) == NULL_NAME)
         return NULL;
      name += builtInNameCount;
   }
   ExportMapEntry* entry = _exportMap->lookUp(name, name);
   if (entry == NULL)
      return NULL;
   return _segment + _rackOrigin + entry->_pos;
}
