/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertTxtTag.cpp
 *
 * Module:  Ivory common (streams)
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of insertTxtTag function. Text insertion of an atomic sum tag.
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

#include "ivory/char.h"
#include "ivory/env.h"
#include "ivory/streams.h"
#include "ivory/tag.h"
#include "ivory/type.h"
#include "ivory/void.h"

// insertTxtTag :: OutputStream Char -> Type -> Tag -> Void

defineBuiltInFn_3_args(insertTxtTag,
   builtInAp(typeCon(OutputStream), typeCon(Char)), typeCon(Type), typeCon(Tag), typeCon(Void),
   os, OutputStream_Char,
   type, Type,
   tag, Tag)

   TypeSig typeSig = headForm(typeSignature(type, typeEnv->typeTable()));
   assert(formOf(typeSig) == TYPE_CON, "insertTxtTag: expected type constructor");

   TypeCon& typeCon = toBody(typeSig, TypeCon);
   DataCon* dataCon = typeCon._dataCons;
   while (dataCon != NULL) {
      if (dataCon->tag() == tag)
         break;
      dataCon = dataCon->next();
   }

   assert(dataCon != NULL, "insertTxtTag: expected data constructor");
   assert(dataCon->arity() == 0, "insertTxtTag: expected atomic data constructor");

   putString(*os, *osEnv, typeCon.nameTable().string(dataCon->name()) n_vm);

   drop_Stack(insertTxtTag);
   returnVoid();
endBuiltInFn
