/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byte.cpp
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
 *    Implementation of Byte type
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

#include <sstream>
#include "ivory/byte.h"
#include "ivory/streams.h"

defineTypeCon(Byte);

static Void init$Byte(TypeDescr* typeDescr);
defineType(Byte, typeCon(Byte), init$Byte);

declareLabel(enter$Byte);

defineLabel(enter$Byte) {
//   checkArgs(0, 0, "enter_Byte");
   returnByte(toBody(cell, Byte));
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopy$Byte(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return fromByte(cellBody(src, Byte), msa);
}

static Void cellShow$Byte(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << cellBody(cell_, Byte);
}

#undef localConst
#define localConst

CellInfo cellInfo_Byte(fromType(builtInType(Byte)), 0,
                       label(enter$Byte), 
                       cellCopy$Byte,

#if (SERIALISATION==1)
                       NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                       NULL,
#endif

                       destroyCell, cellShow$Byte);

// Return value of Byte as type Expr

Expr fromByte(Byte val, MSA& msa) {
	if (val >= MIN_SMALL_INT && val <= MAX_SMALL_INT)
		return ((val + (INT_ZERO >> TAG_BITS)) << TAG_BITS) + ENUM_VAL_TAG;
	else {
      Cell& cell_ = *new(sizeof(Byte), msa) Cell(&cellInfo_Byte);
      cellBody(cell_, Byte) = val;
      return fromCell(cell_); 
   }
}

// Return value of Expr as type Byte

Byte toByte(Expr expr) {
// ASSERT(typeOf(expr) == typeCon(Byte));

   return isPtr(expr)
      ? cellBody(toCell(expr), Byte)
      : (static_cast<Byte>(expr) >> TAG_BITS) -
        (static_cast<Byte>(INT_ZERO) >> TAG_BITS);
}

// Byte type methods.

Void returnCopy$Byte(Void* ptr, Env& env, MSA& msa argN_VM) {
   *(Byte*)ptr = rInt;
}

Ptr mapFn$Byte(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Byte* ptrRes = (Byte*)dstEnv.msa().alloc(sizeof(Byte));
   *ptrRes = *((Byte*)ptr);
   return ptrRes;
}

Void insertTxtFn$Byte(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   std::ostringstream strm;
   strm << *((Byte*)ptr);
   const std::string str = strm.str();
   putString(*os, osEnv, str.c_str() n_vm);
}

Void evalFn$Byte(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rByte = *static_cast<Byte*>(ptr);
}

Void init$Byte(TypeDescr* typeDescr) {
   typeDescr->_size =         sizeof(Byte);
   typeDescr->_mapFn =        mapFn$Byte;
   typeDescr->_evalFn = evalFn$Byte;
   typeDescr->_insertTxtFn =       insertTxtFn$Byte;
}

/*----------------------------------------------------------------------------*/
