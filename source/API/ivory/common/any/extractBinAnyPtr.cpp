/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinAnyPtr.cpp
 *
 * Module:  Ivory common (Any)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 9 October 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built in function to extract a generic value from a byte stream.
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
#include "ivory/byte.h"
#include "ivory/env.h"
#include "ivory/ptr.h"
#include "ivory/streams.h"
#include "ivory/type.h"

// extractBinAnyPtr :: InputStream Byte -> type -> Ptr a;

defineBuiltInFn_2_args(extractBinAnyPtr,
                       builtInAp(typeCon(InputStream), typeCon(Byte)),
                       typeCon(Type),
                       builtInAp(typeCon(Ptr), fromName(builtInName(a))),
                       is, InputStream_Byte,
                       t, Type)

   TypeDescr* typeDescr = t < builtInTypeCount
      ? builtInTypeTable().typeDescr(t)
      : tEnv->typeTable().typeDescr(t - builtInTypeCount);
#if (SERIALISATION==1)
   assert(typeDescr != NULL && typeDescr->_extractBinFn != NULL, "extractBinAnyPtr: type descriptor missing or no extract method");
   rPtr = typeDescr->_extractBinFn(typeDescr, is, *isEnv, *consEnv);
#else
   assert(FALSE, "extractBinAnyPtr: SERIALISATION required");
#endif

   drop_Stack(extractBinAnyPtr);
   returnPtr(rPtr, consEnv);

endBuiltInFn

