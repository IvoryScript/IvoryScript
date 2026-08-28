/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    double.cpp
 *
 * Module:  Ivory common (double)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Double type
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

#include "ivory/double.h"
#include <cmath>
#include <iomanip>

defineTypeCon(Double);

static Void init$Double(TypeDescr* typeDescr);
defineType(Double, typeCon(Double), init$Double);

declareLabel(enter$Double);

defineLabel(enter$Double) {
#define stackDepth 0
   checkArgs("enterDouble");
   returnDouble(toBody(cell, Double));
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopy$Double(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return makeDouble(cellBody(src, Double), msa);
}

static Void cellShow$Double(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << cellBody(cell_, Double);
}

#undef localConst
#define localConst

CellInfo info$Double(fromType(builtInType(Double)), 0,
                     label(enter$Double),
                     cellCopy$Double,

#if (SERIALISATION==1)
                     NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                     NULL,
#endif

                     destroyCell, cellShow$Double);

// Return Double cell

Cell* makeDouble(Double val, MSA& msa) {
   Cell& cell_ = *new(sizeof(Double), msa) Cell(&info$Double);
   cellBody(cell_, Double) = val;
   return &cell_;
}

// Return value of Expr as type Double

Double toDouble(Expr expr) {
   return toBody(expr, Double);
}

Ptr mapFn$Double(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Double* ptrRes = (Double*)dstEnv.msa().alloc(sizeof(Double));
   *ptrRes = *((Double*)ptr);
   return ptrRes;
}

Void insertTxtFn$Double(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Double*>(ptr), Double);
   callSequence$(altEntry(insertTxtDouble) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Double(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   callSequence$(altEntry(extractBinDouble) n_vm);
   Double* ptrRes = (Double*)dstEnv.msa().alloc(sizeof(Double));
   *ptrRes = rDouble;
   return ptrRes;
}

Void insertBinFn$Double(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Double*>(ptr), Double);
   callSequence$(altEntry(insertBinDouble) n_vm);
}
#endif

Void evalFn$Double(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rDouble = *static_cast<Double*>(ptr);
}

Void init$Double(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Double);
   typeDescr->_mapFn = mapFn$Double;
   typeDescr->_evalFn = evalFn$Double;
   typeDescr->_insertTxtFn = insertTxtFn$Double;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$Double;
   typeDescr->_insertBinFn = insertBinFn$Double;
#endif

}
