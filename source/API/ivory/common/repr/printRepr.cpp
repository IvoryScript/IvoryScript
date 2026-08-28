/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    printRepr.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 3 November 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Print representation
 *
 * Call format:
 *
 *    printRepr(repr, os)
 *
 *       repr        (Repr)         - Representation to print
 *       os          (ostream&)     - Output stream
 *
 * Method:
 *
 * Errors:
 *
 *    None.
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

static Void printStructRepr(const StructRepr& structRepr, ostream& os) {
   for (UInt i = 0; i < structRepr.nReprs(); i++) {
      if (i > 0)
         os << ", ";
      printRepr(structRepr.reprV()[i], os);
      os << '[' << structRepr.offsetV()[i] << '/' << structRepr.sizeV()[i] << ']';
   }
}

static Void printUnionRepr(const UnionRepr& unionRepr, ostream& os) {
   for (UInt i = 0; i < unionRepr.nReprs(); i++) {
      if (i > 0)
         os << " | ";
      printRepr(unionRepr.reprV()[i], os);
   }
}

Void printRepr(const Repr repr, ostream& os) {
   switch (repr.tag()) {
      case Repr::REPR_UNKNOWN:
         os << "UNKNOWN";
         break;
         case Repr::REPR_CELL_INFO_PTR:
         os << "CELL_INFO_PTR";
         break;
      case Repr::REPR_VOID:
         os << "VOID";
         break;
      case Repr::REPR_EXPR:
         os << "EXPR";
         break;
      case Repr::REPR_CELL_PTR:
         os << "CELL_PTR";
         break;
      case Repr::REPR_ENV_PTR:
         os << "ENV_PTR";
         break;
      case Repr::REPR_LABEL:
         os << "LABEL";
         break;
      case Repr::REPR_TYPE:
         os << "TYPE";
         break;
      case Repr::REPR_NAME:
         os << "NAME";
         break;
      case Repr::REPR_BYTE:
         os << "BYTE";
         break;
      case Repr::REPR_INT:
         os << "INT";
         break;
      case Repr::REPR_FLOAT:
         os << "FLOAT";
         break;
      case Repr::REPR_DOUBLE:
         os << "DOUBLE";
         break;
      case Repr::REPR_REF:
         os << "REF";
         break;
      case Repr::REPR_CHAR:
         os << "CHAR";
         break;
      case Repr::REPR_STRING:
         os << "STRING";
         break;
      case Repr::REPR_TAG:
         os << "TAG";
         break;
      case Repr::REPR_PTR:
         os << "PTR";
         break;
      case Repr::REPR_STRUCT: {
         os << "STRUCT(";
         printStructRepr(*repr.structRepr(), os);
         os << ')';
         break;
      }
      case Repr::REPR_UNION: {
         os << "UNION(";
         printUnionRepr(*repr.unionRepr(), os);
         os << ")";
         break;
      }
      case Repr::REPR_FILE_HANDLE:
         os << "REPR_FILE_HANDLE";
         break;
      case Repr::REPR_SERIAL_CONTEXT:
         os << "REPR_SERIAL_CONTEXT";
         break;
      case Repr::REPR_UTC:
         os << "REPR_UTC";
         break;
      case Repr::REPR_LABEL_PAIR:
         os << "REPR_LABEL_PAIR";
         break;
      default:
         os << "???";
         break;
   }
}
