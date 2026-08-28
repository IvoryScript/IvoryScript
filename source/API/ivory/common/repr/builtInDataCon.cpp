/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    builtInDataCon.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built-in data constructor implementation.
 *
 * Notes:
 *
 *    If DEBUG_INIT is defined, diagnostic information will be reported
 *    to 'outStream'.
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
#include "ivory/builtIn.h"
#include "ivory/builtInDataCon.h"
#include "ivory/nameExprMap.h"
#include "ivory/typeTable.h"

//#define DEBUG_INIT

#ifdef DEBUG_INIT
   #include "ivory/trace.h"
#endif

// Global built-in data constructor list

BuiltInDataCon* BuiltInDataCon::globalList = NULL;

BuiltInDataCon::BuiltInDataCon(Name name, Tag tag, TypeSig(*mkTypeSig)(Void), TypeSig(*mkReprTypeSig)(Void))
   : DataCon(name, tag, UNKNOWN, UNKNOWN),
   _mkTypeSig(mkTypeSig),
   _mkReprTypeSig(mkTypeSig) {

// Link into global list

   _next      = globalList;
   globalList  = this;
}

// Function to make a built-in data constructor

Expr mkBuiltInDataCon(const char* ident, Tag tag,
                      TypeSig (*mkTypeSig)(void), TypeSig(*mkReprTypeSig)(void),
                      const struct CellInfo& cellInfo) {
   Cell& cell_ = *new(sizeof(BuiltInDataCon), builtInMSA()) Cell(&cellInfo);
   Name name = builtInName$(ident);

   new(cell_.body()) BuiltInDataCon(name, tag, mkTypeSig, mkReprTypeSig);
   builtInNameMap().add(NameExprBinding(name, fromCell(cell_)),
                               builtInNameTable().hashVal(name),
                               builtInMSA());

   builtInExportTable().addCell(ident, cell_, cellInfo);

   return fromCell(cell_);
}

// Initialisation 

Void BuiltInDataCon::init() {
   _type = builtInTypeTable().lookUp(_typeSig = _mkTypeSig());
   _arity = ::arity(_typeSig);

#ifdef DEBUG_INIT
   outStream << "BuiltInDataCon::init: " << builtInNameTable().string(_name) << ' ' <<
                "arity " <<_arity << '\n';
   printExpr(_typeSig, outStream, Env(builtInNameTable()));
   outStream << '\n';
   outStream.flush();
#endif

   toBody(headForm(_arity == 0 ? _typeSig : resultTypeSig(_typeSig, _arity)), TypeCon).addDataCon(this);
}


void BuiltInDataCon::initGlobalList(Void) {
   for (BuiltInDataCon* builtInDataCon = globalList;
        builtInDataCon != NULL;
        builtInDataCon = builtInDataCon->_next)
      builtInDataCon->init();
}
