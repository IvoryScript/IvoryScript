/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    mapAnyPtr.cpp
 *
 * Module:  Ivory common (Any)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built in function to map a generic value from one environment to another.
 *
 * Notes:
 *
 *    1. Implemented as built-in function to access type methods
 *
 *    2. Only returns the pointer component
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

#include "ivory/any.h"
#include "ivory/env.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#endif

#include "ivory/ptr.h"
#include "ivory/void.h"

static const size_t mapAnyPtrSpillDepth = stackFPSizeAny + stackFPSizeEnv;

declareLabel(mapAnyPtr$cont);

#if (GARBAGE_COLLECTION==1)
declareLabel(mark$mapAnyPtr$any);

static BuiltInFrameDescrSlot mapAnyPtr$slots[] = {
   BuiltInFrameDescrSlot(label(mark$mapAnyPtr$any), NULL_NAME, NULL_TYPE)
};

static BuiltInFrameDescr mapAnyPtr$frameDescr(label(mapAnyPtr$cont),
   sizeof(Label) + mapAnyPtrSpillDepth, 1, mapAnyPtr$slots);

defineLabel(mark$mapAnyPtr$any) {
   Any any = frame(mapAnyPtrSpillDepth, Any);
   Env* anyEnv = frame(mapAnyPtrSpillDepth - stackSlotSize(Any), Env*);
   markAny_GC_(any, *anyEnv n_vm);
   jump(popLabel());
}
#endif

defineLabel(mapAnyPtr$cont) {
   drop(mapAnyPtrSpillDepth);
   jump(popLabel());
}

// mapAnyPtr :: Any -> Env -> Ptr a

defineBuiltInFn_2_args(mapAnyPtr,
   typeCon(Any), typeCon(Env), builtInAp(typeCon(Ptr), fromName(builtInName(a))),
   any,     Any,
   dstEnv,  Env)
   Type any_type = any.type_();
   TypeDescr* typeDescr = any_type < builtInTypeCount
      ? builtInTypeTable().typeDescr(any_type)
      : anyEnv->typeTable().typeDescr(any_type - builtInTypeCount);
   assert(typeDescr != NULL && typeDescr->_mapFn != NULL, "mapAnyPtr: type descriptor missing or no map method");

   stackAlloc(stackFPSizeAny);
   storeArg(any, Any, 0);
   pushLabel(label(mapAnyPtr$cont));
   rPtr = typeDescr->_mapFn(typeDescr, any.valPtr(), *anyEnv, *dstEnv n_vm);
   rEnv = dstEnv;
   jump(popLabel());
endBuiltInFn
