/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    resultType.cpp
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
 *    Function to return the result type signature of a function type sugnature
 *
 * Call format:
 *
 *    res = resultTypeSig(TypeSig typeSig, UInt nArgs = 1)
 *
 *       typeSig  (TypeSig)   - Type signature
 *       nArgs    (UInt)      - No. of args (defaults to 1)
 *       res      (TypeSig)   - Returned result type signature
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

#include "ivory/type.h"

TypeSig resultTypeSig(TypeSig typeSig, UInt nArgs/* = 1*/) {
	do {
       assert(isArrowTypeSig(typeSig), "resultTypeSig: not function type");
      typeSig = arg(typeSig);
      if (nArgs == 0)
         break;
   }
   while (--nArgs > 0);

   return typeSig;
}

Type resultType(Type type, UInt nArgs, TypeTable& typeTable,
                                       NameTable& nameTable) {
   return useType(resultTypeSig(typeSignature(type, typeTable), nArgs),
                  typeTable,
                  nameTable);
}
