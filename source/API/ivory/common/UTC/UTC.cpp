/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    UTC.cpp
 *
 * Module:  Ivory common (UTC)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of UTC type
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

#include "ivory/class.h"
#include "ivory/UTC.h"

#define TIME_FORMAT "%d/%m/%y %H:%M:%S"

defineTypeCon(UTC);

static Void init$UTC(TypeDescr* typeDescr);
defineType(UTC, typeCon(UTC), init$UTC);

declareLabel(enterUTC);

defineLabel(enterUTC)
{
   returnUTC(toBody(cell, UTC));
}

Ptr mapFn$UTC(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   UTC* ptrRes = (UTC*)dstEnv.msa().alloc(sizeof(UTC));
   *ptrRes = *((UTC*)ptr);
   return ptrRes;
}

Void insertTxtFn$UTC(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<UTC*>(ptr), UTC);
   callSequence$(altEntry(insertTxtUTC) n_vm);
}

Void evalFn$UTC(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rUTC = *static_cast<UTC*>(ptr);
}

Void init$UTC(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(UTC);
   typeDescr->_mapFn = mapFn$UTC;
   typeDescr->_evalFn = evalFn$UTC;
   typeDescr->_insertTxtFn = insertTxtFn$UTC;
}
