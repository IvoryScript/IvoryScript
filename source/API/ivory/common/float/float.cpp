/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    float.cpp
 *
 * Module:  Ivory common (float)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Float type
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

#include "ivory/float.h"

defineTypeCon(Float);

static Void init$Float(TypeDescr* typeDescr);
defineType(Float, typeCon(Float), init$Float);

declareLabel(enter$Float);

defineLabel(enter$Float) {
#define stackDepth 0
   checkArgs("enterFloat");
   returnFloat(toBody(cell, Float));
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopy$Float(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return makeFloat(cellBody(src, Float), msa);
}

static Void cellShow$Float(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << cellBody(cell_, Float);
}

#undef localConst
#define localConst

CellInfo info$Float(fromType(builtInType(Float)), 0,
                    label(enter$Float),
                    cellCopy$Float,

#if (SERIALISATION==1)
                    NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                    NULL,
#endif

                    destroyCell, cellShow$Float);

// Return Float cell

Cell* makeFloat(Float val, MSA& msa) {
   Cell& cell_ = *new(sizeof(Float), msa) Cell(&info$Float);
   cellBody(cell_, Float) = val;
   return &cell_; 
}

// Return value of Expr as type Float

Float toFloat(Expr expr) {
   return toBody(expr, Float);
}

Ptr mapFn$Float(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Float* ptrRes = (Float*)dstEnv.msa().alloc(sizeof(Float));
   *ptrRes = *((Float*)ptr);
   return ptrRes;
}

Void insertTxtFn$Float(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Float*>(ptr), Float);
   callSequence$(altEntry(insertTxtFloat) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Float(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   callSequence$(altEntry(extractBinFloat) n_vm);
   Float* ptrRes = (Float*)dstEnv.msa().alloc(sizeof(Float));
   *ptrRes = rFloat;
   return ptrRes;
}

Void insertBinFn$Float(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Float*>(ptr), Float);
   callSequence$(altEntry(insertBinFloat) n_vm);
}
#endif

Void evalFn$Float(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rFloat = *static_cast<Float*>(ptr);
}

Void init$Float(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Float);
   typeDescr->_mapFn = mapFn$Float;
   typeDescr->_evalFn = evalFn$Float;
   typeDescr->_insertTxtFn = insertTxtFn$Float;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$Float;
   typeDescr->_insertBinFn = insertBinFn$Float;
#endif

}
