/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    char.cpp
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
 *    Implementation of Char type.
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

#include "ivory/char.h"

defineTypeCon(Char);

static Void init$Char(TypeDescr* typeDescr);
defineType(Char, typeCon(Char), init$Char);

declareLabel(enter$Char);

defineLabel(enter$Char) {
   returnChar(toBody(cell, Char));
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopy$Char(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return makeChar(cellBody(src, Char), msa);
}

static Void cellShow$Char(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << cellBody(cell_, Char);
}

#undef localConst
#define localConst


CellInfo cellInfo$Char(fromType(builtInType(Char)), 0,
                       label(enter$Char),
                       cellCopy$Char,

#if (SERIALISATION==1)
                       NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                       NULL,
#endif

                       destroyCell, cellShow$Char);

// Return Char cell

Cell* makeChar(Char val, MSA& msa) {
   Cell& cell_ = *new(sizeof(Char), msa) Cell(&cellInfo$Char);
   cellBody(cell_, Char) = val;
   return &cell_;
}

Ptr mapFn$Char(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Char* ptrRes = (Char*)dstEnv.msa().alloc(sizeof(Char));
   *ptrRes = *((Char*)ptr);
   return ptrRes;
}

Void insertTxtFn$Char(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   stackAlloc(4);
   stack(0, Char) = *static_cast<Char*>(ptr);
   callSequence$(altEntry(insertTxtChar) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Char(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   callSequence$(altEntry(extractBinChar) n_vm);
   Char* ptrRes = (Char*)dstEnv.msa().alloc(sizeof(Char));
   *ptrRes = rChar;
   return ptrRes;
}

Void insertBinFn$Char(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   stackAlloc(4);
   stack(0, Char) = *static_cast<Char*>(ptr);
   callSequence$(altEntry(insertBinChar) n_vm);
}
#endif

Void evalFn$Char(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rChar = *static_cast<Char*>(ptr);
}

Void init$Char(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Char);
   typeDescr->_mapFn = mapFn$Char;
   typeDescr->_evalFn = evalFn$Char;
   typeDescr->_insertTxtFn = insertTxtFn$Char;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$Char;
   typeDescr->_insertBinFn = insertBinFn$Char;
#endif
}
