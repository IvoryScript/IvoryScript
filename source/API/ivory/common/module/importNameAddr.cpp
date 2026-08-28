/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    lookUpImport.cpp
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
 *    Look up imported name.
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

#include "ivory/builtIn.h"
#include "ivory/byteCodeModule.h"
#include "ivory/type.h"

IAddress Module::importNameAddr(Name name, const Env& env) {
   Module* module = globalList;
   while (module != NULL) {
      IAddress res = module->lookUpImport(name, env);
      if (res != NULL)
         return res;
      module = module->_next;
   }

   if (name < builtInNameCount) {
      BuiltInExportEntry* entry = builtInExportTable().lookUp(name, name);
      if (entry != NULL)
         return entry->_addr;
   }
   return NULL;
}
