/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    isSumType.cpp
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
 *    Predicates to test for sum type.
 *
 * Call format:
 *
 *    res = isSumType(Type type)
 *
 *       type	(Type)		- Type
 *       res	(Bool)		- True if sum type, False otherwise
 *
 *    res = isSumTypeSig(TypeSig typeSig)
 *
 *       type	(TypeSig)   - Type signature
 *       res	(Bool)		- True if sum type, False otherwise
 *
 * Method:
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

Bool isSumType(Type type, const TypeTable& typeTable){
   return isSumTypeSig(typeSignature(type, typeTable));
}

Bool isSumTypeSig(TypeSig typeSig) {
   typeSig = headForm(typeSig);
   switch (formOf(typeSig)) {
      case TYPE_CON:
         return toBody(typeSig, TypeCon)._n > 1;

      default:
         return FALSE;
   }
}
