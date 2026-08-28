/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    reprType.cpp
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
 *    Given a type signature, this function returns the type signature of a value
 *    constructed with the specified data constructor.
 *
 * Call format:
 *
 *    TypeSig = reprTypeSig(type, tag, msa)
 *
 *       res      (TypeSig)   - Returned type signature
 *       typeSig  (TypeSig)   - Type signature
 *       tag      (Tag)       - Data constructor tag for sum type
 *       msa      (MSA&)      - Storage allocator
 *
 * Method:
 *
 * Errors:
 *
 *    None
 *
 * Notes:
 *
 *    For product types, tag should be 1
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

#include "ivory/common.h"
#include "ivory/list.h"
#include "ivory/compiler/type.h"

// typeSigParam: Formal parameter type signature
// 1 <= i <= nParams

static TypeSig typeSigParam(TypeSig typeSig, UInt i, UInt nParams) {
   while (i++ < nParams)
      typeSig = fun(typeSig);
   
   return arg(typeSig);
}

// reprTypeSig: Representation type signature of a value of a given type signature
//              and data constructor tag

TypeSig reprTypeSig(TypeSig typeSig, Tag tag, MSA& msa) {
   UInt nParams = 0;
   TypeSig hfTypeSig = typeSig;
   while isAp(hfTypeSig) {
      hfTypeSig = fun(hfTypeSig);
      nParams++;
   }
   assert(formOf(hfTypeSig) == TYPE_CON, "reprTypeSig: Expected type constructor");
   TypeCon& typeCon = toBody(hfTypeSig, TypeCon);
   Expr typeVars = typeCon._typeVars;
   Subst* substs = NULL;
   UInt i = 1;
   while (typeVars != Nil) {
      Expr typeVar = hd(typeVars);
      substs = new (msa) Subst(toTypeVar(typeVar), typeSigParam(typeSig, i++, nParams), substs);
      typeVars = tl(typeVars);
   }
   const DataCon* dataCon = typeCon._dataCons;
   while (dataCon->tag() != tag)
      dataCon = &dataCon->next();
   TypeSig resTypeSig = mutableTypeSig(dataCon->reprTypeSig(), msa);
   Subst::substTypeVars(resTypeSig, substs);
   return resTypeSig;
}
