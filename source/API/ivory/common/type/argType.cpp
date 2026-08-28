/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    argType.cpp
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
 *    Function to return the argument type of a function type
 *
 * Call format:
 *
 *    res = argType(TypeSig typeSig)
 *
 *       res      (TypeSig)   - Returned argument type signature
 *       typeSig  (Type)      - Type signature
 *
 * Method:
 *
 *
 * Errors:
 *
 *    None
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

#include "ivory/machine.h"
#include "ivory/type.h"

// 1 <= i <= nArgs

Type argType(Type type, UInt i, TypeTable& typeTable, 
                                NameTable& nameTable) {
   return useType(argTypeSig(typeSignature(type, typeTable), i),
                  typeTable, nameTable);
}

TypeSig argTypeSig(TypeSig typeSig, UInt i/*= 1*/) {
   if (i > arity(typeSig)) {
      assert(TRUE, "argType: invalid index");
      return UNKNOWN;
   }
   while (--i > 0)
      typeSig = arg(typeSig);
   
   return arg(fun(typeSig));
}
