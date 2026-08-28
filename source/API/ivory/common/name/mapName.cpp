/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    mapName.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Functions to map a name from one environment to another.
 *
 * Call format:
 *
 *    res = mapName$(name, srcNameTable, dstNameTable)
 *                     
 *       res            (Name)         - Returned name mapped to destination
 *       name           (Name)         - Name to map
 *       srcNameTable   (NameTable&)   - Source name table
 *       dstNameTable   (NameTable&)   - Destination name table
 *
 * Method:
 *
 *    Simply returns the given name if the name the name tables match.
 *    Otherwise looks up the source identifier in the destination name table
 *    and returns the result.
 * 
 *    'mappedName' is similar except that no new name is added.
 *
 * Note:
 * 
 *    For a built-in name, an entry is added to the destination table to
 *    ensure persistence consistency.
 *    (i.e. for a change to the built-in set)
 *
 * Errors:
 *
 *    Errors are signalled by a ERROR return value
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
#include "ivory/env.h"
#include "ivory/name.h"

declareTypeCon(Name);

Name mapName$(Name name, const NameTable& srcNameTable,
                               NameTable& dstNameTable) {
   assert(&srcNameTable != &dstNameTable, "mapName$: invalid precondition");
   if (name < builtInNameCount) {
      (Void)useName(builtInNameTable().string(name),
                    dstNameTable);
      return name;
   } else {
      NameTable::Entry& srcEntry = *srcNameTable.lockName(name - builtInNameCount);
      NameTable::Entry* dstEntry = dstNameTable.lookUp(srcEntry._string, srcEntry._hashVal);
      if (dstEntry == NULL)
         dstEntry = dstNameTable.addName(srcEntry._string, srcEntry._hashVal);
      return builtInNameCount + dstEntry->_name;
   }
}

Name mappedName$(Name name, const NameTable& srcNameTable,
                            const NameTable& dstNameTable) {
   if (name < builtInNameCount || &srcNameTable == &dstNameTable)
      return name;
   const char* ident = srcNameTable.string(name - builtInNameCount);
   unsigned int hashVal = srcNameTable.hashVal(name - builtInNameCount);
   NameTable::Entry* entry = dstNameTable.lookUp(ident, hashVal);
   return entry != NULL
      ? builtInNameCount + entry->_name
      : NULL_NAME;
}

// mapName :: Name -> Env -> Name

defineBuiltInFn(mapName,
   builtInAp2(ARROW,
      typeCon(Name),
      builtInAp2(ARROW,
         typeCon(Env),
         typeCon(Name))))
#define arg2         sizeof(Env*)
#define arg1         (arg2+sizeof(Name))
#define stackDepth  arg1
{
   checkArgs("mapName");
   jump(altEntry(mapName));
}

defineAltEntry(mapName) {
   Name name = local(arg1, Name);
   Env* env = local(arg2, Env*);

   dropStack();
   returnName(mapName(name, consEnv->nameTable(), env->nameTable()), env);
}
