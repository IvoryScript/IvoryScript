/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    int.cpp
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
 *    Implementation of Int type
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

#include "ivory/int.h"

defineTypeCon(Int);

static Void init$Int(TypeDescr* typeDescr);
defineType(Int, typeCon(Int), init$Int);

declareLabel(enter$Int);

defineLabel(enter$Int) {
//   checkArgs(0, 0, "enter$Int");
   returnInt(toBody(cell, Int));
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopy$Int(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return fromInt(cellBody(src, Int), msa);
}

static Void cellShow$Int(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << cellBody(cell_, Int);
}

#undef localConst
#define localConst

CellInfo cellInfo$Int(fromType(builtInType(Int)), 0,
                      label(enter$Int),
                      cellCopy$Int,
   
#if (SERIALISATION==1)
                     NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                     NULL,
#endif
   
                     destroyCell, cellShow$Int);

// Return value of Int as type Expr

Expr fromInt(Int val, MSA& msa) {
	if (val >= MIN_SMALL_INT && val <= MAX_SMALL_INT)
		return ((val + (INT_ZERO >> TAG_BITS)) << TAG_BITS) + ENUM_VAL_TAG;
	else {
      Cell& cell_ = *new(sizeof(Int), msa) Cell(&cellInfo$Int);
      cellBody(cell_, Int) = val;
      return fromCell(cell_); 
   }
}

// Return value of Expr as type Int

Int toInt(Expr expr) {
// ASSERT(typeOf(expr) == typeCon(Int));

   return isPtr(expr)
      ? cellBody(toCell(expr), Int)
      : (static_cast<Int>(expr) >> TAG_BITS) -
        (static_cast<Int>(INT_ZERO) >> TAG_BITS);
}

Ptr mapFn$Int(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Int* ptrRes = (Int*)dstEnv.msa().alloc(sizeof(Int));
   *ptrRes = *((Int*)ptr);
   return ptrRes;
}

Void insertTxtFn$Int(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Int*>(ptr), Int);
   callSequence$(altEntry(insertTxtInt) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Int(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   callSequence$(altEntry(extractBinInt) n_vm);
   Int* ptrRes = (Int*)dstEnv.msa().alloc(sizeof(Int));
   *ptrRes = rInt;
   return ptrRes;
}

Void insertBinFn$Int(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Int*>(ptr), Int);
   callSequence$(altEntry(insertBinInt) n_vm);
}
#endif

Void evalFn$Int(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rInt = *static_cast<Int*>(ptr);
}

Void init$Int(TypeDescr* typeDescr) {
   typeDescr->_size =         sizeof(Int);
   typeDescr->_mapFn =        mapFn$Int;
   typeDescr->_evalFn = evalFn$Int;
   typeDescr->_insertTxtFn =       insertTxtFn$Int;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn =    extractBinFn$Int;
   typeDescr->_insertBinFn =     insertBinFn$Int;
#endif
}

/*----------------------------------------------------------------------------*/
