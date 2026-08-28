/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    storeTypeSig.cpp
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
 *    Store type signature to archive stream.
 *
 * Call format:
 *
 *    storeTypeSig(archive, type, nameTbl)
 *
 *       archive  (Archive&)     - Archive
 *       type     (Type)         - Type expression to store
 *       nameTbl  (NameTable&)   - Name table
 *
 * Method:
 *
 *    Store primitive type constructor or type application.
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

#include "nameTable.h"
#include "ivory/builtInDataCon.h"
#include "ivory/serialStorage.h"
#include "ivory/type.h"

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

void storeTypeSig(Archive& archive, TypeSig typeSig, const NameTable& nameTable) {
   Byte code = 0;
   switch (formOf(typeSig)) {
      case UNKNOWN:
         code = (Byte)TYPE_SIG_UNKNOWN;
         break;

      case AP:
         archive << (Byte)TYPE_SIG_AP;
         storeTypeSig(archive, fun(typeSig), nameTable);
         storeTypeSig(archive, arg(typeSig), nameTable);
         return;

      case ARROW:
         code = (Byte)TYPE_SIG_ARROW;
         break;

      case TAG:
         code = (Byte)TAG;
         break;

      case TYPE_CON:
         if (typeSig == typeCon(Void))
            code = (Byte)TYPE_SIG_VOID;
         else if (typeSig == typeCon(Exp))
            code = (Byte)TYPE_SIG_EXP;
         else if (typeSig == typeCon(Expr))
            code = (Byte)TYPE_SIG_EXPR;
         else if (typeSig == typeCon(Name))
            code = (Byte)TYPE_SIG_NAME;
         else if (typeSig == typeCon(Type))
            code = (Byte)TYPE_SIG_TYPE;
         else if (typeSig == typeCon(Byte))
            code = (Byte)TYPE_SIG_BYTE;
         else if (typeSig == typeCon(Int))
            code = (Byte)TYPE_SIG_INT;
         else if (typeSig == typeCon(Float))
            code = (Byte)TYPE_SIG_FLOAT;
         else if (typeSig == typeCon(Double))
            code = (Byte)TYPE_SIG_DOUBLE;
         else if (typeSig == typeCon(Char))
            code = (Byte)TYPE_SIG_CHAR;
         else if (typeSig == typeCon(String))
            code = (Byte)TYPE_SIG_STRING;
         else if (typeSig == typeCon(Ref))
            code = (Byte)TYPE_SIG_REF;
         else if (typeSig == typeCon(Ptr))
            code = (Byte)TYPE_SIG_PTR;
         else if (typeSig == typeCon(Array))
            code = (Byte)TYPE_SIG_ARRAY;
         else if (typeSig == typeCon(List))
            code = (Byte)TYPE_SIG_LIST;
         else if (typeSig == typeCon(Any))
            code = (Byte)TYPE_SIG_ANY;
         else {
            archive << (Byte)TYPE_SIG_TYPE_CON;
            TypeCon& typeCon = toBody(typeSig, TypeCon);
            Name typeConName = typeCon._name;
            const char* ident = typeCon.nameTable().string(
               typeConName < builtInNameCount ? typeConName : typeConName - builtInNameCount);
            NameTable::Entry* entry = nameTable.lookUp(ident, typeCon.hashVal());
            assert(entry != NULL, "storeTypeSig: name missing");
            archive << entry->_name;
            return;
         }
         break;

      case TUPLE: {
         code = (Byte)(TYPE_SIG_TUPLE_MIN + toTupleCon(typeSig));
         if (code > TYPE_SIG_TUPLE_MAX) {
            archive << (Byte)TYPE_SIG_TUPLE;
            archive.insertVLU(toTupleCon(typeSig) - (TYPE_SIG_TUPLE_MAX + 1));
            return;
         }
         break;
      }

      default:
         ivoryError("storeType: invalid type signature");
         break;
   }

   archive << code;
}
