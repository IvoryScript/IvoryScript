/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    dataCon.cpp
 *
 * Module:  Ivory common.
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Data constructor implementation.
 *
 * Notes:
 *
 *    If DEBUG_INIT is defined, diagnostic information will be reported
 *    to 'outStream'.
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

#include "ivory/dataCon.h"
#include "ivory/repr.h"
#include "ivory/type.h"

//#define DEBUG_INIT

#ifdef DEBUG_INIT
   #include "ivory/trace.h"
#endif

DataCon::DataCon(Name name, Tag tag, TypeSig typeSig, TypeSig reprTypeSig)
  : _name(name), _tag(tag),
    _typeSig(typeSig), _type(NULL_TYPE),
    _reprTypeSig(reprTypeSig), _reprType(NULL_TYPE),
    _next(NULL) {
   _arity = ::arity(typeSig);
}

static UInt digits(UInt n) {
   UInt res = 1;
   while (n >= 10) {
      res++;
      n /= 10;
   }
   return res;
}

ConstString DataCon::selectorName(Name name, UInt index, const NameTable& nameTable, MSA& msa) {
   ConstString nameStr = nameString_(name, nameTable);
   String res = (String)msa.alloc(3 + 1 + strlen(nameStr) + 1 + digits(index) + 1);
   sprintf(res, "SEL$%s$%d", nameStr, index);
   return res;
}
