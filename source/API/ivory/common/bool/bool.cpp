/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    bool.cpp
 *
 * Module:  Ivory common (bool)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Bool data type.
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

#include "ivory/bool.h"
#include "ivory/env.h"
#include "ivory/tag.h"
#include "ivory/type.h"

defineBuiltInTypeCon(Bool);

static Void init_Bool(TypeDescr* typeDescr);
defineBuiltInType(Bool, typeCon(Bool), init_Bool);


Expr fromBool(Bool b) {
   switch (b) {
      case FALSE:
         return False;
      case TRUE:
         return True;
   }
   return ERROR;              // Included to satisfy Visual C++
}

// Bool type methods.

Void returnCopy_Bool(Void* ptr, Env& env, MSA& msa argN_VM) {
   *(Bool*)ptr = rTag == TRUE;
}


Ptr mapFn$Bool(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Bool* ptrRes = (Bool*)dstEnv.msa().alloc(sizeof(Bool));
   *ptrRes = *((Bool*)ptr);
   return ptrRes;
}

Void insertTxtFn$Bool(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   stackAlloc(stackFPSizeBool);
   stack(0, Bool) = *static_cast<Bool*>(ptr);
   callSequence$(altEntry(insertTxtBool) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Bool(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   stackAlloc(stackFPSizeType);
   stack(0, Type) = builtInType(Bool);
   stack(stackSlotSize(Type), Env*) = &builtInEnv();
   callSequence$(altEntry(extractBinTag) n_vm);
   Bool* ptrRes = (Bool*)dstEnv.msa().alloc(sizeof(Bool));
   *ptrRes = rTag == TRUE;
   return ptrRes;
}

Void insertBinFn$Bool(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   stackAlloc(stackFPSizeType + stackFPSizeTag);
   stack(0, Type) = builtInType(Bool);
   stack(stackSlotSize(Type), Env*) = &builtInEnv();
   stack(stackFPSizeType, Tag) = *static_cast<Bool*>(ptr) ? TRUE : FALSE;
   callSequence$(altEntry(insertBinTag) n_vm);
}
#endif

Void evalFn$Bool(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rTag = *static_cast<Bool*>(ptr) ? TRUE : FALSE;
}

Void init_Bool(TypeDescr* typeDescr) {
   typeDescr->_size =         sizeof(Bool);

   typeDescr->_size = sizeof(Bool);
   typeDescr->_mapFn = mapFn$Bool;
   typeDescr->_evalFn = evalFn$Bool;
   typeDescr->_insertTxtFn = insertTxtFn$Bool;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$Bool;
   typeDescr->_insertBinFn = insertBinFn$Bool;
#endif

}

#undef False
#undef True

defineNullaryDataCon(False, 0, Bool);
defineNullaryDataCon(True,  1, Bool);

CellInfo* cellInfoTable_Bool[] = {&cellInfo$False, &cellInfo$True};
