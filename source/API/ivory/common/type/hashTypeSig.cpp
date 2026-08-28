/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    hashTypeSig.cpp
 *
 * Module:  Ivory common (type)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to provide a hash value for a type signature
 *
 * Call format:
 *
 *    res = hashTypeSig(typeSig)
 *
 *       res      (UInt)      - Returned hash value
 *       typeSig  (TypeSig)   - Type signature
 *
 * Method:
 *
 * Errors:
 *
 *    Errors are signalled by a return of ERROR.
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

#include <ctype.h>
#include "nameTable.h"
#include "ivory/machine.h"
#include "ivory/name.h"
#include "ivory/type.h"

UInt hashTypeSig(TypeSig typeSig){
   ExprEnumVal form = formOf(typeSig);
   switch (form) {
      case AP:
         return 3 * AP * hashTypeSig(fun(typeSig)) + hashTypeSig(arg(typeSig));

      case TYPE_CON:
         return toBody(typeSig, TypeCon)._hashVal;

      case NAME: {
         Name name = toName(typeSig);
         return name >= 0 ? builtInNameTable().hashVal(toName(typeSig))
                          : 0;
      }

      default:
         return typeSig.repr();
   }
   return ERROR;
}

Void hashTypeSig_(TypeSig typeSig, const Env& env, HashDJB2& hashDJB2) {
   ExprEnumVal form = formOf(typeSig);
   switch (form) {
      case AP:
         hashDJB2.gen(TYPE_SIG_AP);
         hashTypeSig_(fun(typeSig), env, hashDJB2);
         hashTypeSig_(arg(typeSig), env, hashDJB2);
         break;

      case ARROW:
      case TAG:
         hashDJB2.gen(TYPE_SIG_ARROW);
         break;

      case TYPE_CON: {
         hashDJB2.gen(TYPE_SIG_TYPE_CON);
         const TypeCon& typeCon = toBody(typeSig, TypeCon);
         const String str = nameString_(typeCon.name(), typeCon.nameTable());
         hashDJB2.gen((const Byte*)str, strlen(str));
         break;
      }

      case NAME: {
         hashDJB2.gen(TYPE_SIG_TYPE_VAR);
         Name name = toName(typeSig);
         if (name >= 0) {
            const String str = nameString_(toName(typeSig), env.nameTable());
            hashDJB2.gen((const Byte*)str, strlen(str));
         }
         break;
      }

      case TUPLE: {
         hashDJB2.gen(TYPE_SIG_TUPLE);
         hashDJB2.gen(toTupleCon(typeSig));
         break;
      }

      default:
         ivoryError("hashTypeSig_: invalid type signature");
         traceExpr(typeSig, Env(builtInNameTable()));
         break;
   }
}
