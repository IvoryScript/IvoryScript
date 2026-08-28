/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    sizeOfRepr.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Returns the size of a given representation in bytes
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

#include "ivory/fileIO.h"
#include "ivory/repr.h"

size_t StructRepr::offset(UInt i) const {
    return _offsetV[i];
}

size_t StructRepr::size(UInt i) const {
   return _sizeV[i];
}

extern size_t sizeOfRepr(Repr repr) {
   switch (repr.tag()) {
      case Repr::REPR_VOID:
      case Repr::REPR_STACK_MARKER: return 0;
      case Repr::REPR_EXPR:         return sizeof(Expr);
      case Repr::REPR_TYPE:         return sizeof(Type);
      case Repr::REPR_NAME:         return sizeof(Name);
      case Repr::REPR_CHAR:         return sizeof(Char);
      case Repr::REPR_UTC:
      case Repr::REPR_INT:          return sizeof(Int);
      case Repr::REPR_BYTE:         return sizeof(Byte);
      case Repr::REPR_BITS:         return sizeof(Bits);
      case Repr::REPR_FLOAT:        return sizeof(Float);
      case Repr::REPR_DOUBLE:       return sizeof(Double);
      case Repr::REPR_REF:          return sizeof(Ref);
      case Repr::REPR_TAG:          return sizeof(Tag);
      case Repr::REPR_CELL_INFO_PTR:
      case Repr::REPR_CELL_PTR:
      case Repr::REPR_ENV_PTR:
      case Repr::REPR_DIR:
      case Repr::REPR_DIR_ENTRY:
      case Repr::REPR_SERIAL_CONTEXT:
      case Repr::REPR_STRING:
      case Repr::REPR_PTR:
         return sizeof(Void*);
      case Repr::REPR_LABEL:        return sizeof(Label);
      case Repr::REPR_LABEL_PAIR:   return sizeof(LabelPair);

      case Repr::REPR_STRUCT:
         return repr.structRepr()->size();

      case Repr::REPR_UNION: {
         UnionRepr* unionRepr = repr.unionRepr();
         size_t maxSize = 0;
         for (UInt i = 0; i < unionRepr->nReprs(); i++) {
            size_t size = sizeOfRepr(unionRepr->reprV()[i]);
            if (size > maxSize)
               maxSize = size;
         }
         return maxSize;
      }

      case Repr::REPR_ARRAY:
         return 0;            // Direct array not supported

      case Repr::REPR_FILE_HANDLE:
         return sizeof(FileHandle);

      default:
         break;
   }
   error("sizeOfRepr: unknown");
   return 0;
}


extern size_t alignOfRepr(Repr repr) {
   switch (repr.tag()) {
      case Repr::REPR_VOID:
      case Repr::REPR_STACK_MARKER:
      case Repr::REPR_EXPR:
      case Repr::REPR_TYPE:
      case Repr::REPR_NAME: 
      case Repr::REPR_CHAR:   
      case Repr::REPR_UTC:
      case Repr::REPR_INT: 
      case Repr::REPR_BYTE:
      case Repr::REPR_BITS:
      case Repr::REPR_FLOAT: 
      case Repr::REPR_DOUBLE:
      case Repr::REPR_REF:
      case Repr::REPR_TAG:
      case Repr::REPR_CELL_PTR:
      case Repr::REPR_ENV_PTR:
      case Repr::REPR_DIR:
      case Repr::REPR_DIR_ENTRY:
      case Repr::REPR_SERIAL_CONTEXT:
      case Repr::REPR_STRING:
      case Repr::REPR_PTR:
      case Repr::REPR_LABEL:
      case Repr::REPR_LABEL_PAIR:
      case Repr::REPR_FILE_HANDLE:
         return sizeOfRepr(repr);

      case Repr::REPR_STRUCT:
         return repr.structRepr()->maxAlign();

      case Repr::REPR_UNION: {
         UnionRepr* unionRepr = repr.unionRepr();
         size_t maxSize = 0;
         for (UInt i = 0; i < unionRepr->nReprs(); i++) {
            size_t size = alignOfRepr(unionRepr->reprV()[i]);
            if (size > maxSize)
               maxSize = size;
         }
         return maxSize;
      }

      case Repr::REPR_ARRAY:
         return 0;            // Direct array not supported

      default:
         break;
   }
   error("alignOfRepr: unknown");
   return 0;
}
