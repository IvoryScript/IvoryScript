/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    builtInFn.cpp
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
 *    Built-in function implementation.
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
#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/nameExprMap.h"
#include "ivory/typeTable.h"

//#define DEBUG_INIT

#ifdef DEBUG_INIT
   #include "ivory/trace.h"
#endif

// Global built-in function list

BuiltInFn* BuiltInFn::globalList = NULL;

BuiltInFn::BuiltInFn(Name name, TypeSig (*mkTypeSig)(void))
   : _name(name), _mkTypeSig(mkTypeSig),
     _namePtr(NULL), _typePtr(NULL),
     _type(NULL_TYPE), _arity(0)
{

// Link into global list

   _next_      = globalList;
   globalList  = this;
}

BuiltInFn::BuiltInFn(Name& name, Type& type)
   : _namePtr(&name), _typePtr(&type),
     _type(NULL_TYPE), _arity(0)
{

// Link into global list

   _next_      = globalList;
   globalList  = this;
}

// Function to make a built-in function (or cell

Cell* mkBuiltInFn(const char* ident, TypeSig (*mkTypeSig)(void),
                  const CellInfo& cellInfo) {
   Cell& cell_ = *new(sizeof(BuiltInFn), builtInMSA()) Cell(&cellInfo);
   Name name = builtInName$(ident);

   new(cell_.body()) BuiltInFn(name, mkTypeSig);
   builtInNameMap().add(NameExprBinding(name, fromCell(cell_)),
                               builtInNameTable().hashVal(name),
                               builtInMSA());
  
   builtInExportTable().addCell(ident, cell_, cellInfo);

   return &cell_;
}

Cell* mkBuiltInFnExt(Name& name, Type& type, const CellInfo& cellInfo) {
   Cell& cell_ = *new(sizeof(BuiltInFn), builtInMSA()) Cell(&cellInfo);
   new(cell_.body()) BuiltInFn(name, type);
   return &cell_;
}

// Initialisation 

void BuiltInFn::init() {
   if (_namePtr != NULL) {
      _name = *_namePtr;
      builtInNameMap().add(NameExprBinding(_name, fromCell(*(Cell*)((unsigned char*)this - sizeof(Expr)))),
                                  builtInNameTable().hashVal(_name),
                                  builtInMSA());
   }
   if (_typePtr != NULL) {
      _type = *_typePtr;
      _typeSig = builtInTypeTable().typeSig(_type);
   }
   else
      _type = useBuiltInType(_typeSig = _mkTypeSig());
   if (isArrowTypeSig(_typeSig)) {
      _arity = ::arity(_typeSig);
      _resultType = useBuiltInType(_resultTypeSig = ::resultTypeSig(_typeSig, _arity));
   } else {
      _arity = 0;
      _resultTypeSig = _typeSig;
      _resultType = _type;
   }

#ifdef DEBUG_INIT
   outStream << "BuiltInFn::init: " << builtInNameTable().string(_name) << ' ' <<
                "arity " <<_arity << '\n';
   printExpr(_typeSig, outStream, Env(builtInNameTable()));
   outStream << '\n';
   outStream.flush();
#endif
}

void BuiltInFn::initGlobalList(Void) {
   for (BuiltInFn* builtInFn = globalList;
        builtInFn != NULL;
        builtInFn = builtInFn->_next_)
      builtInFn->init();
}

// Identity copy function

Cell* copyBuiltInFn(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return &src;
}
