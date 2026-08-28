/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    loadTypeSig.cpp
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
 *    Load a type signature from an archive stream.
 *
 * Call format:
 *
 *    res = loadTypeSig(archive, nameTable, MS& msa)
 *
 *       res			(Type)         - Returned type expression
 *       archive		(Archive&)     - Archive
 *       nameTable	(NameTable&)   - Name table
 *       msa			(MSA&)         - Memory storage allocator
 *
 * Method:
 *
 *    Load primitive type constructor or type application etc.
 *
 * Errors:
 *
 *    Errors are signalled by a return of ERROR.
 *
 * Notes:
 *
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

#include "archive.h"
#include "ivory/builtInDataCon.h"
#include "ivory/env.h"
#include "ivory/module.h"
#include "ivory/serialStorage.h"
#include "ivory/typeTable.h"

 // Primitive type constructors

declareTypeCon(Any);

declareTypeCon(Bool);

declareTypeCon(Byte);

declareTypeCon(Char);

declareTypeCon(Float);

declareTypeCon(Double);

declareTypeCon(Int);

declareTypeCon(List);

declareTypeCon(Name);

declareTypeCon(Plain);

declareTypeCon(Ptr);

declareTypeCon(Ref);

declareTypeCon(String);

declareTypeCon(Void);

declareBuiltInDataCon(Ptr);

static TypeSig nameToTypeConSig(Name name, const NameTable& nameTable) {
   UInt hashVal = nameTable.hashVal(name);
   Name builtInName = nameTable.altName(name);
   TypeSig typeSig;
   if (Module::typeConSig(builtInName != NULL_NAME ? builtInName
      : name + builtInNameCount,
      builtInName != NULL_NAME ? builtInNameTable()
      : nameTable,
      hashVal, typeSig))
      return typeSig;
   error("loadTypeSig: missing type constructor");
   return ERROR;
}

TypeSig loadTypeSig(Archive& archive, const NameTable& nameTable, MSA& msa) {
   Byte code;
   archive >> code;
   if (code >= TYPE_SIG_TUPLE_MIN && code <= TYPE_SIG_TUPLE_MAX)
      return TUPLE_MIN + (code - TYPE_SIG_TUPLE_MIN) * ENUM_VAL_INCR;
   else if (code >= TYPE_SIG_TYPE_CON_MIN &&
      code <= TYPE_SIG_TYPE_CON_MAX)
      return nameToTypeConSig(code - TYPE_SIG_TYPE_CON_MIN, nameTable);
   else
      switch (code) {
      case TYPE_SIG_UNKNOWN:
         return UNKNOWN;

      case TYPE_SIG_AP: {
         TypeSig fun = loadTypeSig(archive, nameTable, msa);
         TypeSig arg = loadTypeSig(archive, nameTable, msa);
         return mkAp(fun, arg, msa);
      }

      case TYPE_SIG_ARROW:
         return ARROW;

      case TYPE_SIG_VOID:
         return typeCon(Void);

      case TYPE_SIG_EXP:
         return typeCon(Exp);

      case TYPE_SIG_EXPR:
         return typeCon(Expr);

      case TYPE_SIG_NAME:
         return typeCon(Name);

      case TYPE_SIG_TYPE:
         return typeCon(Type);

      case TYPE_SIG_BYTE:
         return typeCon(Byte);

      case TYPE_SIG_INT:
         return typeCon(Int);

      case TYPE_SIG_FLOAT:
         return typeCon(Float);

      case TYPE_SIG_DOUBLE:
         return typeCon(Double);

      case TYPE_SIG_CHAR:
         return typeCon(Char);

      case TYPE_SIG_STRING:
         return typeCon(String);

      case TYPE_SIG_REF:
         return typeCon(Ref);

      case TYPE_SIG_TUPLE:
         return TUPLE_MIN + TYPE_SIG_MAX_TUPLE_DEGREE + 1 + archive.extractVLU();

      case TYPE_SIG_PLAIN:
         return typeCon(Void);

      case TYPE_SIG_PTR:
         return typeCon(Ptr);

      case TYPE_SIG_ARRAY:
         return typeCon(Array);

      case TYPE_SIG_LIST:
         return typeCon(List);

      case TYPE_SIG_ANY:
         return typeCon(Any);

      case TYPE_SIG_TYPE_CON: {
         Name name;
         archive >> name;
         return nameToTypeConSig(name, nameTable);
      }

      default:
         ivoryError("loadTypeSig: failed");
         break;
      }
   return ERROR;
}
