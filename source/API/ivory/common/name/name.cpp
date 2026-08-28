/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    name.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built-in name table functions.
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
 * Notes:
 *
 *    Updating a name 
 */

#include <string.h>
#include "nameTable.h"
#include "ivory/machine.h"
#include "ivory/name.h"
#include "ivory/repr.h"

#define BUILT_IN_NAME_TABLE_N_SLOTS     1297
#define BUILT_IN_NAME_TABLE_N_PER_SEG   256

defineTypeCon(Name);

static Void init$Name(TypeDescr* typeDescr);
defineType(Name, typeCon(Name), init$Name);

declareLabel(enterName);

defineLabel(enterName) {
   returnName(toBody(cell, Name), cellEnv);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopyFnName(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return makeName(cellBody(src, Name), msa);
}

#undef localConst
#define localConst

CellInfo infoName(fromType(builtInType(Name)), 0,
                  label(enterName),
                  cellCopyFnName, NULL,
   NULL);

// Return Name cell

Cell* makeName(Name name, MSA& msa) {
   Cell& cell_ = *new(sizeof(Float), msa) Cell(&infoName);
   cellBody(cell_, Name) = name;
   return &cell_;
}

// Name type methods.

Void returnCopy$Name(Void* ptr, Env& env, MSA& msa argN_VM) {
   *(Name*)ptr = rName;
}

Ptr mapFn$Name(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Name* ptrRes = (Name*)dstEnv.msa().alloc(sizeof(Name));
   *ptrRes = mapName(*((Name*)ptr), ptrEnv.nameTable(), dstEnv.nameTable());
   return ptrRes;
}

Void insertTxtFn$Name(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(static_cast<Env*>((Void*)&env), Env*);
   stackAlloc(4);
   stack(0, Name) = *static_cast<Name*>(ptr);
   callSequence$(altEntry(insertTxtName) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Name(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   callSequence$(altEntry(extractBinName) n_vm);
   Name* ptrRes = (Name*)dstEnv.msa().alloc(sizeof(Name));
   *ptrRes = rName;
   return ptrRes;
}

Void insertBinFn$Name(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(static_cast<Env*>((Void*)&env), Env*);
   stackAlloc(4);
   stack(0, Name) = *static_cast<Name*>(ptr);
   callSequence$(altEntry(insertBinName) n_vm);
}
#endif

Void evalFn$Name(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rName = *static_cast<Name*>(ptr);
   rEnv = &env;
}

Void init$Name(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Name);
   typeDescr->_mapFn = mapFn$Name;
   typeDescr->_evalFn = evalFn$Name;
   typeDescr->_insertTxtFn = insertTxtFn$Name;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$Name;
   typeDescr->_insertBinFn = insertBinFn$Name;
#endif

}

// built-in name table

static NameTable* _builtInNameTable = NULL;

Int builtInNameCount = 0;

NameTable& builtInNameTable() {
   if (_builtInNameTable == NULL)
      _builtInNameTable = new (builtInMSA()) NameTable(BUILT_IN_NAME_TABLE_N_SLOTS,
                               BUILT_IN_NAME_TABLE_N_PER_SEG,
                               builtInMSA());
   return *_builtInNameTable;
}

// Enter or look up name in built-in name table

Name builtInName$(const char* ident) {
   UInt hashVal = hashString(ident);
   const NameTable::Entry* entry = builtInNameTable().lookUp(ident,
      hashVal);
   if (entry == NULL)
      entry = builtInNameTable().addName(ident, hashVal);
   return entry->_name;
}

// Extend name table for a given identifier
// Adding alternate if built-in

Name useName(const char* ident, NameTable& nameTable) {
   UInt hashVal = hashString(ident);
   NameTable::Entry* entry = nameTable.lookUp(ident, hashVal);
   if (entry == NULL) {
      entry = nameTable.addName(ident, hashVal);
      NameTable::Entry* builtInNameEntry = builtInNameTable().lookUp(ident, hashVal);
      if (builtInNameEntry != NULL)
         entry->_altName = builtInNameEntry->_name;
   }

   return entry->_altName != NULL_NAME
      ? entry->_altName
      : builtInNameCount + entry->_name;
}

// Return the hash value of a given name

UInt nameHashVal(Name name, const NameTable& nameTable) {
   return name < builtInNameCount ? builtInNameTable().hashVal(name)
                                  : nameTable.hashVal(name - builtInNameCount);
}

// Return the identifier for a given name

const String nameString_(Name name, const NameTable& nameTable) {
   return (String)(name >= 0
      ? (name < builtInNameCount ? builtInNameTable().string(name)
                                 : nameTable.string(name - builtInNameCount))
      : NULL);
}

// eqName_:: Test two names for equality

Bool eqName_(Name name1, const NameTable& nameTable1,
   Name name2, const NameTable& nameTable2) {
   if (name1 < builtInNameCount && name2 < builtInNameCount ||
       &nameTable1 == &nameTable2)
      return name1 == name2;
   else {
      const char* s1 = nameString_(name1, name1 < builtInNameCount ? builtInNameTable() : nameTable1);
      const char* s2 = nameString_(name2, name2 < builtInNameCount ? builtInNameTable() : nameTable2);
      return s1 != NULL && s2 != NULL && strcmp(s1, s2) == 0;
   }
}
