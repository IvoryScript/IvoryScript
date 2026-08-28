/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    typeCon.cpp
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
 *    Implementation of functions relating to type constructors
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
#include "ivory/list.h"
#include "ivory/nameExprMap.h"
#include "ivory/type.h"

//#define DEBUG_INIT

#ifdef DEBUG_INIT
#include "ivory/trace.h"
#endif

// Built-in type constructor list

TypeCon* TypeCon::builtInList = NULL;

// Type constructor C++ constructor

TypeCon::TypeCon(Name name, UInt hashVal, const NameTable& nameTable,
                 Expr (*mkTypeVars)(Void),
                 Bool objectFlag/* = FALSE*/, Bool isDynamic/* = FALSE*/)
   : _name(name), _hashVal(hashVal),
     _nameTable(nameTable),
     _mkTypeVars(mkTypeVars),
     _objectFlag(objectFlag),
     _isDynamic(isDynamic),
     _n(0), _dataCons(NULL) {
}

void TypeCon::init() {
   _typeVars = _mkTypeVars != NULL ? _mkTypeVars()
                                   : Nil;

#ifdef DEBUG_INIT
   outStream << "TypeCon::init: " << builtInNameTable().string(_name) << '\n';
   printExpr(_typeVars, outStream, Env(builtInNameTable()));
   outStream << '\n';
   outStream.flush();
#endif
}

// Make a built-in type constructor cell

#include "ivory/trace.h"

TypeSig mkTypeCon(const char* ident, Expr (*mkTypeVars)(Void)/* = NULL*/, Bool objectFlag/* = FALSE*/) {
   Cell& cell_ = *new(sizeof(TypeCon), builtInMSA()) Cell(TYPE_CON);
   Name name = builtInName$(ident);
   UInt hashVal = builtInNameTable().hashVal(name);
   new(cell_.body()) TypeCon(name, hashVal, builtInNameTable(), mkTypeVars, objectFlag);

// Link into built-in list for type variable initialisation

   cellBody(cell_, TypeCon)._next = TypeCon::builtInList;
   TypeCon::builtInList = &cellBody(cell_, TypeCon);

// Add to built-in map from name to type constructor cell

   builtInTypeConNameMap().add(NameExprBinding(name, fromCell(cell_)),
                               hashVal,
                               builtInMSA());

   return fromCell(cell_);
}

Void TypeCon::addDataCon(DataCon* dataCon) {

// Add data constructor to list in tag order

   DataCon** dataConPtr = &_dataCons;

   while (*dataConPtr != NULL && (*dataConPtr)->tag() < dataCon->tag()) 
      dataConPtr = &(*dataConPtr)->next();
   dataCon->next() = *dataConPtr;

   *dataConPtr = dataCon;
   _n++;
}

void TypeCon::initBuiltInList(Void) {
   TypeCon* typeCon;
   while ((typeCon = builtInList) != NULL) {
      builtInList = typeCon->_next;
      typeCon->init();
   }
}
