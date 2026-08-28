/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    markAny_GC.cpp
 *
 * Module:  Ivory common (Any)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 10 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of markAny_GC built in function
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

#if (GARBAGE_COLLECTION==1)
#include "MSA_GC.h"
#endif
#include "ivory/any.h"
#include "ivory/list.h"
#include "ivory/trace.h"
#include "ivory/void.h"

#if (GARBAGE_COLLECTION==1)
Void markAny_GC_(const Any& any, Env& env argN_VM) {
   Type type = any.type_();
   TypeDescr* typeDescr = type < builtInTypeCount
      ? builtInTypeTable().typeDescr(type)
      : env.typeTable().typeDescr(type - builtInTypeCount);

   assert(typeDescr != NULL, "markAny_GC: type descriptor missing");
   if (&env.msa() == consMSA &&
      ((MSA_GC*)consMSA)->mark(any.valPtr()) == 1 &&
      typeDescr->_markFn != NULL)
      typeDescr->_markFn(typeDescr, any.valPtr(), env n_vm);
}

Void markBuiltInPAPArg_GC(const Any& any, Env& env argN_VM) {
   markAny_GC_(any, env n_vm);
}
#endif

// markAny_GC :: Any -> Void

defineBuiltInFn_1_arg(markAny_GC,
   typeCon(Any), typeCon(Void),
   any, Any)
#if (GARBAGE_COLLECTION==1)
   markAny_GC_(any, *rEnv n_vm);
#else
   assert(FALSE, "markAny_GC: GARBAGE_COLLECTION required");
#endif

   returnVoid();
endBuiltInFn
