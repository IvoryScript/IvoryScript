/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    bits.cpp
 *
 * Module:  Ivory common (bits)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of native Bits type
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

#include "ivory/bits.h"

defineTypeCon(Bits);

static Void init$Bits(TypeDescr* typeDescr);
defineType(Bits, typeCon(Bits), init$Bits);

declareLabel(enter$Bits);

defineLabel(enter$Bits) {
   //   checkArgs(0, 0, "enter$Bits");
   returnBits(toBody(cell, Bits));
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopy$Bits(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return fromBits(cellBody(src, Bits), msa);
}

static Void cellShow$Bits(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << hex << cellBody(cell_, Bits) << dec;
}

#undef localConst
#define localConst

CellInfo cellInfo$Bits(fromType(builtInType(Bits)), 0, label(enter$Bits),
   cellCopy$Bits,

#if (SERIALISATION==1)
   NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
   NULL,
#endif

   destroyCell, cellShow$Bits);

// Return value of Bits as type Expr

Expr fromBits(Bits val, MSA& msa) {
   Cell& cell_ = *new(sizeof(Bits), msa) Cell(&cellInfo$Bits);
   cellBody(cell_, Bits) = val;
   return fromCell(cell_);
}

// Return value of Expr as type Bits

Bits toBits(Expr expr) {
   return toBody(expr, Bits);
}

// Bits type methods.

Void returnCopy$Bits(Void* ptr, Env& env, MSA& msa argN_VM) {
   *(Bits*)ptr = rBits;
}

Ptr mapFn$Bits(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Bits* ptrRes = (Bits*)dstEnv.msa().alloc(sizeof(Bits));
   *ptrRes = *((Bits*)ptr);
   return ptrRes;
}

Void insertTxtFn$Bits(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Bits*>(ptr), Bits);
   callSequence$(altEntry(insertTxtBits) n_vm);
}

Void evalFn$Bits(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rBits = *static_cast<Bits*>(ptr);
}

Void init$Bits(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Bits);
   typeDescr->_mapFn = mapFn$Bits;
   typeDescr->_evalFn = evalFn$Bits;
   typeDescr->_insertTxtFn = insertTxtFn$Bits;
}

/*---------------------------------------------------------------------*/
