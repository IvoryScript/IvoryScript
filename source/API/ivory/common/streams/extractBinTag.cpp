/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinTag.cpp
 *
 * Module:  Ivory common (streams)
 *
 *-----------------------------------------------------------------------------
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

#include "ivory/byte.h"
#include "ivory/env.h"
#include "ivory/streams.h"
#include "ivory/tag.h"
#include "ivory/type.h"

defineBuiltInFn_2_args(extractBinTag,
   builtInAp(typeCon(InputStream), typeCon(Byte)), typeCon(Type), typeCon(Tag),
   is,   InputStream_Byte,
   type, Type)

   TypeSig typeSig = typeSignature(type, typeEnv->typeTable());
   assert(formOf(typeSig) == TYPE_CON, "extractBinTag: expected type constructor");
   TypeCon& typeCon = toBody(typeSig, TypeCon);

   ExtractArchive ea(is->_inAct, isEnv);

   Bool isFirst = FALSE;
   const char* dcStr = extractBinSerialString(ea, *is->_serialContext, isFirst);
   DataCon* dataCon = typeCon._dataCons;
   while (dataCon != NULL) {
      if (strcmp(typeCon.nameTable().string(dataCon->name()),
                 dcStr) == 0)
         break;
      dataCon = dataCon->next();
   }
   assert(dataCon != NULL, "extractBinTag: expected data constructor");
   if (isFirst) {
      Tag tag = (Tag)ea.extractVLU();
      if (tag != dataCon->tag())
         error("Tag mismatch on stream extract");
   }
   drop_Stack(extractBinTag);
   returnTag((Tag)dataCon->tag());
endBuiltInFn
