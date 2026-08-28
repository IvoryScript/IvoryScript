/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showType.cpp
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
 *    Implementation of showType function
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
#include <string>
#include <string.h>

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/class.h"
#include "ivory/char.h"
#include "ivory/list.h"
#include "ivory/string.h"
#include "ivory/trace.h"
#include "ivory/typeTable.h"
#include "ivory/void.h"

declareBuiltInFn(showType);
declareBuiltInFn(typeString);

// showType :: Type -> Void

defineBuiltInFn_1_arg(showType,
   typeCon(Type), typeCon(Void),
   t, Type)

   TypeSig typeSig = t < builtInTypeCount
      ? builtInTypeTable().typeSig(t)
      : rEnv->typeTable().typeSig(t - builtInTypeCount);

   printCanonicalType(t, *outStrm, *rEnv);
   returnVoid();
endBuiltInFn

/*----------------------------------------------------------------------------*/

#if (GARBAGE_COLLECTION==1)
// No active roots, arbitrary association label
static BuiltInFrameDescr typeStringFrameDescr(altEntry(typeString), 0, 0, NULL);
#endif

defineBuiltInFn_1_arg(typeString,
   typeCon(Type), typeCon(String),
   t, Type)
   std::ostringstream strm;
   printCanonicalType(rType, strm, *rEnv);
   std::string str = strm.str();

#if (GARBAGE_COLLECTION ==1)
   Char* ptr = (Char*)memAlloc_GC(str.length() + 1, altEntry(typeString), (MSA_GC&)*consMSA n_vm);
#else
   Char* ptr = (Char*)consMSA->alloc(str.length() + 1);
#endif

   strcpy(ptr, str.c_str());
   returnString(ptr, consEnv);
endBuiltInFn

/*----------------------------------------------------------------------------*/

// instance Show Type where
//    show t = showType t

classMethodDefn_n(1, show, showType) {
   push(rEnv, Env*);
   push(rTag, Int);
   jump(altEntry(showType));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_Type,
               Show,
               typeCon(Type),
               builtInCons(methodDefn_1,
                           Nil));
