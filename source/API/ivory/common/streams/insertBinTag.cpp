/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertBinTag.cpp
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
#include "ivory/typeTable.h"
#include "ivory/type.h"
#include "ivory/void.h"

defineBuiltInFn_3_args(insertBinTag,
   builtInAp(typeCon(OutputStream), typeCon(Byte)), typeCon(Type), typeCon(Tag), typeCon(Void),
   os, OutputStream_Byte,
   type, Type,
   tag, Tag)

   TypeSig typeSig = typeSignature(type, typeEnv->typeTable());
   assert(formOf(typeSig) == TYPE_CON, "insertBinTag: expected type constructor");
   TypeCon& typeCon = toBody(typeSig, TypeCon);
   DataCon* dataCons = typeCon._dataCons;
   while (dataCons != NULL) {
      if (dataCons->tag() == tag)
         break;
      dataCons = dataCons->next();
   }
   assert(dataCons != NULL, "insertBinTag: expected data constructor");
   InsertArchive ia(os->_outputFn, osEnv);
   if (insertBinSerialString(ia, *os->_serialContext,
                          typeCon.nameTable().string(dataCons->name())))
      ia.insertVLU(tag);

   drop_Stack(insertBinTag);
   returnVoid();
endBuiltInFn
