/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ordering.cpp
 *
 * Module:  Ivory common (ord)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Ordering data type.
 *
 * Notes:
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

#include "ivory/env.h"
#include "ivory/ordering.h"
#include "ivory/class.h"
#include "ivory/streams.h"
#include <string.h>

defineTypeCon(Ordering);

static Void init$Ordering(TypeDescr* typeDescr);
defineType(Ordering, typeCon(Ordering), init$Ordering);

static const char* constructorName(Tag tag) {
   switch (tag) {
      case LT_TAG: return "LT";
      case EQ_TAG: return "EQ";
      case GT_TAG: return "GT";
   }

   error("constructorName: bad Ordering tag");
   return "LT";
}

static Tag tagFromConstructorName(const char* name) {
   if (strcmp(name, "LT") == 0)
      return LT_TAG;
   if (strcmp(name, "EQ") == 0)
      return EQ_TAG;
   if (strcmp(name, "GT") == 0)
      return GT_TAG;

   error("tagFromConstructorName: bad Ordering constructor name");
   return LT_TAG;
}

static Ptr mapFn$Ordering(TypeDescr* typeDescr, Ptr ptr,
                          const Env& ptrEnv, Env& dstEnv argN_VM) {
   Ordering* ptrRes = (Ordering*)dstEnv.msa().alloc(sizeof(Ordering));
   *ptrRes = *((Ordering*)ptr);
   return ptrRes;
}

static Void insertTxtFn$Ordering(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Ordering*>(ptr), Tag);
   callSequence$(altEntry(insertTxtOrdering) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Ordering(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   push(&builtInEnv(), Env*);
   push(builtInType(Ordering), Int);
   callSequence$(altEntry(extractBinTag) n_vm);
   Int* ptrRes = (Int*)dstEnv.msa().alloc(sizeof(Int));
   *ptrRes = rInt;
   return ptrRes;
}

Void insertBinFn$Ordering(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<Int*>(ptr), Int);
   push(&builtInEnv(), Env*);
   push(builtInType(Ordering), Int);
   callSequence$(altEntry(insertBinTag) n_vm);
}
#endif
Void evalFn$Ordering(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rTag = static_cast<Tag>(*static_cast<Ordering*>(ptr));
}

static Void init$Ordering(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Ordering);
   typeDescr->_mapFn = mapFn$Ordering;
   typeDescr->_evalFn = evalFn$Ordering;
   typeDescr->_insertTxtFn = insertTxtFn$Ordering;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$Ordering;
   typeDescr->_insertBinFn = insertBinFn$Ordering;
#endif
}

defineNullaryDataCon(LT, 0, Ordering);
defineNullaryDataCon(EQ, 1, Ordering);
defineNullaryDataCon(GT, 2, Ordering);

CellInfo* cellInfoTable_Ordering[] = {&cellInfo$LT, &cellInfo$EQ,
                                      &cellInfo$GT};
