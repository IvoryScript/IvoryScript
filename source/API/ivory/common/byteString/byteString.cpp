/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byteString.cpp
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
 *    Implementation of ByteString type
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

#include <ctype.h>
#include <string.h>
#include "ivory/byteString.h"
#include "ivory/string.h"

#define charToHex(val) (isdigit(val)?(val)-'0':10+tolower(val)-'a')

#define hexToChar(val) ((char)((val)<10?'0'+(val):'a'+(val)-10))

defineTypeCon(ByteString);

static Void init$byteString(TypeDescr* typeDescr);
defineType(ByteString, typeCon(ByteString), init$byteString);

// Structure for closure update

struct ByteStringIndirection {
#if (GARBAGE_COLLECTION==1)
   Cell*          _cell;
#endif
   ByteString     _byteString;
};

declareLabel(enter$ByteString);

declareLabel(enter$IndByteString);

static Cell* cellCopy$ByteString1(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return fromByteString(cellBody(src, ByteString)->_data,
                         cellBody(src, ByteString)->_len, msa);
}

static Cell* cellCopy$ByteString2(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return fromByteString(cellBody(src, ByteStringIndirection)._byteString->_data,
                         cellBody(src, ByteStringIndirection)._byteString->_len, msa);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Void cellShow$ByteString1(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   printByteString(cellBody(cell_, ByteString), os);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Void cellShow$ByteString2(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   printByteString(cellBody(cell_, ByteStringIndirection)._byteString, os);
}

#undef localConst
#define localConst

static CellInfo cellInfo$ByteString1(fromType(builtInType(ByteString)), 0,
                                     label(enter$ByteString),
                                     cellCopy$ByteString1,

#if (SERIALISATION==1)
                                     NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                                     NULL,
#endif

                                     destroyCell, cellShow$ByteString1);

static CellInfo cellInfo$ByteString2(fromType(builtInType(ByteString)), 0,
                                     label(enter$ByteString),
                                     cellCopy$ByteString2,

#if (SERIALISATION==1)
                                     NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                                     NULL,
#endif

                                     destroyCell, cellShow$ByteString2);

#define stackDepth 0
defineLabel(enter$ByteString) {
   checkArgs("ByteString");
   rType = builtInType(ByteString);
#if (GARBAGE_COLLECTION==1)
   rCell = cell;
#endif
   rPtr = cell->body();
   jump(popLabel());
}

defineLabel(enter_IndByteString) {
   checkArgs("ByteString");
   rType = builtInType(ByteString);
#if (GARBAGE_COLLECTION==1)
   rCell = toBody(cell, ByteStringIndirection)._cell;
#endif
   rPtr = toBody(cell, ByteStringIndirection)._byteString;
   jump(popLabel());
}
#undef stackDepth

/*----------------------------------------------------------------------------*/

// ByteString type methods

// Copy construct a value from the return state into the given hunk.

Void returnCopy$byteString(Void* ptr, Env& env, MSA& msa argN_VM) {
   ByteString rBS = (ByteString)rPtr;
   ByteString bs = (ByteString)msa.alloc(
      sizeof(ByteString_tag) + rBS->_len / 2 - MAX_BYTE_STRING_LENGTH);
   bs->_len = rBS->_len;
   memcpy(bs->_data, rBS->_data, rBS->_len);
   *(ByteString*)ptr = bs;
}

// Free up any memory on destruction

Void insertTxtFn$byteString(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(static_cast<Env*>((Void*)&env), Env*);
   push(*static_cast<ByteString*>(ptr), ByteString);
   callSequence$(altEntry(insertTxtByteString) n_vm);
}
Void destroy$byteString(Void* ptr, Env& env, MSA& msa) {
   msa.free(((ByteString)ptr)->_data);
}

Void evalFn$ByteString(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rPtr = *static_cast<ByteString*>(ptr);
   rEnv = &env;
}

Void init$byteString(TypeDescr* typeDescr) {
   typeDescr->_size =            sizeof(ByteString);
   typeDescr->_insertTxtFn =     insertTxtFn$byteString;
   typeDescr->_evalFn = evalFn$ByteString;
}

// Return value of ByteString as cell

Cell* fromByteString(const ByteString val, MSA& msa) {
   Cell& cell_ = *new(sizeof(struct ByteStringIndirection), msa)
      Cell(&cellInfo$ByteString2);
   cellBody(cell_, struct ByteStringIndirection)._byteString = val;
   return &cell_; 
}

// Return value of ByteString as type Expr

Cell* fromByteString(const unsigned char* data, size_t len, MSA& msa) {
   Cell& cell_ = *new(sizeof(ByteString_tag) + len - MAX_BYTE_STRING_LENGTH, msa)
      Cell(&cellInfo$ByteString1);
   ((ByteString)cell_.body())->_len = len;

// Allow for data initialisation by caller

   if (data != NULL)
      memcpy(((ByteString)cell_.body())->_data, data, len);
   return &cell_; 
}

Void printByteString(const ByteString& byteString, ostream& os) {
   os << "ByteString \"";
   for (size_t i = 0; i < byteString->_len; i++) {
      os << hexToChar(byteString->_data[i] / 16);
      os << hexToChar(byteString->_data[i] % 16);
   }
   os << '\"';
}
/*----------------------------------------------------------------------------*/

// ByteString :: String -> ByteString

defineDataCon(ByteString, 0,
              builtInAp2(ARROW,
                         typeCon(String),
                         typeCon(ByteString)),
              BYTE_STRING)
#define s_env  sizeof(Env*)
#define s      (s_env+sizeof(String))
#define stackDepth   s
{
   checkArgs("ByteString");
   jump(altEntry(ByteString));
}

defineAltEntry(ByteString) {
   size_t len = strlen(local(s, String));

   ByteString bs = (ByteString)consMSA->alloc(sizeof(ByteString_tag) + len / 2 - MAX_BYTE_STRING_LENGTH);

   bs->_len = len / 2;
   for (unsigned int i = 0; i < len / 2; i++)
      bs->_data[i] = charToHex(local(s, String)[i * 2]) * 16 +
                     charToHex(local(s, String)[i * 2 + 1]);

   dropStack();
   returnByteString(bs, consEnv);
}
