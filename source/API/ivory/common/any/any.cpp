/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    any.cpp
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
 *    Support for in-built type Any
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
 *
 * Notes:
 *
 *    '*' is a synonym for Any
 *
 */

#include "ivory/any.h"

defineTypeCon(Any);

/*----------------------------------------------------------------------------*/

Any mapAny_(const Any& any, const Env& srcEnv, Env& dstEnv argN_VM) {
   if (&srcEnv == &dstEnv)
      return any;

   Type type = any.type_();
   TypeDescr* typeDescr = type < builtInTypeCount
      ? builtInTypeTable().typeDescr(type)
      : srcEnv.typeTable().typeDescr(type - builtInTypeCount);
   assert(typeDescr != NULL && typeDescr->_mapFn != NULL,
          "mapAny_: type descriptor missing or no map method");

   Type dstType = mapType(type, srcEnv, dstEnv);
   Ptr dstPtr = typeDescr->_mapFn(typeDescr, any.valPtr(),
                                  srcEnv, dstEnv n_vm);
   return Any(dstType, dstPtr);
}

/*----------------------------------------------------------------------------*/

Any::Any(const Any& src, const Env& srcEnv, Env& env, MSA& msa) {
// T.B.D.
}

Void Any::destroy(Env& env, MSA& msa) {
   // T.B.D.
}

Void Any::print(ostream& os, const Env& env) const {
   os << "Any::print: T.B.D.\n";
}


/*----------------------------------------------------------------------------*/
