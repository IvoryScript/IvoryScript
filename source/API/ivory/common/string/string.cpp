/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    string.cpp
 *
 * Module:  Ivory common (string)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of String type
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
#include "ivory/env.h"
#include "ivory/int.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/string.h"
#include "ivory/trace.h"
#include "ivory/type.h"

defineTypeCon(String);

static Void init$String(TypeDescr* typeDescr);
defineType(String, typeCon(String), init$String);

// Structure for closure update

struct StringIndirection {
#if (GARBAGE_COLLECTION==1)
   Cell*          _cell;
#endif
   String         _string;
};

declareLabel(enter$String);

declareLabel(enter$IndString);

#if (GARBAGE_COLLECTION==1)
declareLabel(mark$IndString_GC);
#endif

#ifdef localConst
#undef localConst
#endif
#define localConst const

Cell* cellCopy$String1(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return fromString((String)src.body(), msa);
}

Void cellShow$String1(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << (String)cell_.body();
}

Cell* cellCopy$String2(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return fromString(cellBody(src, StringIndirection)._string, msa);
}

Void cellShow$String2(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << cellBody(cell_, StringIndirection)._string;
}

#undef localConst
#define localConst


CellInfo cellInfo$String1(fromType(builtInType(String)), 0,
                          label(enter$String),
                          cellCopy$String1,

#if (SERIALISATION==1)
                          NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                          NULL,
#endif

                          destroyCell, cellShow$String1);


CellInfo cellInfo$String2(fromType(builtInType(String)), 0,
                          label(enter$String),
                          cellCopy$String2,

#if (SERIALISATION==1)
                          NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                          label(mark$IndString_GC),
#endif

                          destroyCell, cellShow$String2);

defineLabel(enter$String) {
//   checkArgs(0, 0, "String");
   rType = builtInType(String);

#if (GARBAGE_COLLECTION==1)
   rCell = cell;
#endif

   rPtr = cell->body();
   jump(popLabel());
}

defineLabel(enter$IndString) {
//   checkArgs(0, 0, "String");
   rType = builtInType(String);

#if (GARBAGE_COLLECTION==1)
   rCell = toBody(cell, StringIndirection)._cell;
#endif

   rPtr = toBody(cell, StringIndirection)._string;
   jump(popLabel());
}

#if (GARBAGE_COLLECTION==1)
defineLabel(mark$IndString_GC) {
   Cell* stringCell = toBody(cell, StringIndirection)._cell;
   if (stringCell != NULL && ((MSA_GC*)consMSA)->mark(stringCell) == 1) {
      Label label = static_cast<const CellInfo*>(stringCell->tag())->cellGC_MarkFn();
      if (label != NULL) {
         cell = stringCell;
         jump(label);
      }
   }
   jump(popLabel());
}
#endif

/*----------------------------------------------------------------------------*/

// String type methods

Ptr mapFn$String(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   if (&ptrEnv == &dstEnv)
      return ptr;
   else {
      String* ptrRes = (String*)dstEnv.msa().alloc(sizeof(String));
      *ptrRes = mapString$(*(String*)ptr, ptrEnv, dstEnv);
      return ptrRes;
   }
}

Void insertTxtFn$String(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(static_cast<Env*>((Void*)&env), Env*);
   push(*static_cast<String*>(ptr), String);
   callSequence$(altEntry(insertTxtString) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$String(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   callSequence$(altEntry(extractBinString) n_vm);
   String* ptrRes = (String*)dstEnv.msa().alloc(sizeof(String));
   *ptrRes = (String)rPtr;
   return ptrRes;
}

Void insertBinFn$String(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(static_cast<Env*>((Void*)&env), Env*);
   push(*static_cast<String*>(ptr), String);
   callSequence$(altEntry(insertBinString) n_vm);
}
#endif

#if (GARBAGE_COLLECTION==1)
static Void markFn$String(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   if (&env.msa() == consMSA)
      (Void)((MSA_GC*)consMSA)->mark(*(String*)ptr);
}
#endif

Void evalFn$String(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rPtr = *static_cast<String*>(ptr);
   rEnv = &env;
}

Void init$String(TypeDescr* typeDescr) {
   typeDescr->_size =      sizeof(String);
   typeDescr->_mapFn =     mapFn$String;
   typeDescr->_evalFn = evalFn$String;
   typeDescr->_insertTxtFn =    insertTxtFn$String;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$String;
   typeDescr->_insertBinFn =  insertBinFn$String;
#endif

#if (GARBAGE_COLLECTION==1)
   typeDescr->_markFn = markFn$String;
#endif

}

// Return value of String as type Expr

Expr fromString(const char* val, MSA& msa) {
   Cell& cell_ = *new(strlen(val) + 1, msa) Cell(&cellInfo$String1);
   strcpy((String)cell_.body(), val);
   return fromCell(cell_); 
}

Expr fromString(const char* val, size_t len, MSA& msa) {
   Cell& cell_ = *new(len + 1, msa) Cell(&cellInfo$String1);
   strncpy((String)cell_.body(), val, len);
   ((String)cell_.body())[len] = '\0';
   return fromCell(cell_); 
}

/*----------------------------------------------------------------------------*/

// Helper function included for reason of efficiency

// mutableString :: Int -> String

#if (GARBAGE_COLLECTION==1)
// No active roots, arbitrary association label
static BuiltInFrameDescr frameDescr(altEntry(mutableString), 0, 0, NULL);
#endif

defineBuiltInFn(mutableString,
   builtInAp2(ARROW,
      typeCon(Int),
      typeCon(String)))

#define stackDepth   0
{
   checkArgs("mutableString");
   jump(altEntry(mutableString));
}

defineAltEntry(mutableString) {

#if (GARBAGE_COLLECTION ==1)
   UInt len = rInt;
   String res = (String)memAlloc_GC(rInt + 1, altEntry(mutableString), (MSA_GC&)*consMSA n_vm);
   rInt = len;
#else
   String res = (String)consMSA->alloc(rInt + 1);
#endif

   memset(res, 0, rInt + 1);
   returnString(res, consEnv);
}

#undef stackDepth
#undef i
