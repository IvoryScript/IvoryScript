/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertAny.cpp
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
 *     Built in function to insert a generic value into a byte stream.
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
#include "ivory/list.h"
#include "ivory/streams.h"
#include "ivory/void.h"

// insertBinAnyPtr :: OutputStream Byte Any -> Void

defineBuiltInFn_2_args(insertBinAnyPtr,
   builtInAp(typeCon(OutputStream), typeCon(Byte)), typeCon(Any), typeCon(Void),
   os, OutputStream_Byte,
   any, Any)

   Type type = any.type_();
   TypeDescr* typeDescr = type < builtInTypeCount
      ? builtInTypeTable().typeDescr(type)
      : anyEnv->typeTable().typeDescr(type - builtInTypeCount);

#if (SERIALISATION==1)
   assert(typeDescr != NULL && typeDescr->_insertBinFn != NULL, "insertBinAnyPtr: type descriptor missing or no insert method");
   typeDescr->_insertBinFn(typeDescr, os, *osEnv, any.valPtr(), *anyEnv);
#else
   assert(FALSE, "insertBinAnyPtr: SERIALISATION required");
#endif

   drop_Stack(insertBinAnyPtr);
   returnVoid();
endBuiltInFn

