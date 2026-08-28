/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    unpackTypeSig.cpp
 *
 * Module:  Ivory common (type)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 11 January 2005
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Unpack a type signature.
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

#include "general.h"
#include "nameTable.h"
#include "ivory/module.h"
#include "ivory/nameExprMap.h"
#include "ivory/type.h"

declareTypeCon(Any);

declareTypeCon(Name);

declareTypeCon(Void);

declareTypeCon(Char);

declareTypeCon(Float);

declareTypeCon(Double);

declareTypeCon(Int);

declareTypeCon(Ref);

declareTypeCon(String);

declareTypeCon(List);

// Built-in types have special codes to minimise the space required in
// the name table

TypeSig unpackTypeSig(unsigned char*& buffer, const Name nameV[],
                      NameTable& nameTable, MSA& msa) {
   TypeSig typeSig;
   UInt code = *buffer++;
   switch (code) {
      case TYPE_SIG_AP: {
         TypeSig f = unpackTypeSig(buffer, nameV, nameTable, msa);
         TypeSig a = unpackTypeSig(buffer, nameV, nameTable, msa);
         return ap(f, a, msa);
      }

      case TYPE_SIG_ARROW:
         return ARROW;

      case TYPE_SIG_TYPE_VAR:
         return fromName(nameV[unpackVLU(&buffer)]);

      case TYPE_SIG_ANY:
         return typeCon(Any);

      case TYPE_SIG_EXP:
         return typeCon(Exp);

      case TYPE_SIG_EXPR:
         return typeCon(Expr);

      case TYPE_SIG_NAME:
         return typeCon(Name);

      case TYPE_SIG_TYPE:
         return typeCon(Type);

      case TYPE_SIG_VOID:
         return typeCon(Void);

      case TYPE_SIG_INT:
         return typeCon(Int);

      case TYPE_SIG_FLOAT:
         return typeCon(Float);

      case TYPE_SIG_DOUBLE:
         return typeCon(Double);

      case TYPE_SIG_REF:
         return typeCon(Ref);

      case TYPE_SIG_CHAR:
         return typeCon(Char);

      case TYPE_SIG_STRING:
         return typeCon(String);

      case TYPE_SIG_LIST:
         return typeCon(List);

      case TYPE_SIG_ARRAY:
         return typeCon(Array);

      case TYPE_SIG_TUPLE:
          return fromTupleCon(unpackVLU(&buffer) + TYPE_SIG_TUPLE_MAX + 1);
      
      case TYPE_SIG_TYPE_CON: {
         TypeSig res;
         Name typeConName = nameV[unpackVLU(&buffer)];
         if (!builtInTypeConNameMap().lookUp(typeConName, nameTable.hashVal(typeConName), res))
            res = ERROR;
         return res;
      }

      default: {
         if (code >= TYPE_SIG_TUPLE_MIN &&
             code <= TYPE_SIG_TUPLE_MAX)
            return fromTupleCon(code - TYPE_SIG_TUPLE_MIN);
         TypeSig res;
         Name typeConName = nameV[code -  TYPE_SIG_TYPE_CON_MIN];
         if (!builtInTypeConNameMap().lookUp(typeConName, nameTable.hashVal(typeConName), res))
            res = ERROR;
         return res;
      }
   }
}
