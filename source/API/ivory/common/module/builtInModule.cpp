/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    builtInModule.cpp
 *
 * Module:  Ivory common (module)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 18 November 2004
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Module initialisation
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

#include <string.h>

#include "ivory/builtInModule.h"
#include "ivory/type.h"

//#define DEBUG_INIT

#ifdef DEBUG_INIT
   #include "ivory/trace.h"
#endif

BuiltInModule::BuiltInModule(const char* identifiers,  Name nameV[],
                             unsigned char typeData[], UInt typeDataLen, Type typeV[])
   : Module(), _nameV(nameV),
     _typeData(typeData), _typeDataLen(typeDataLen), _typeV(typeV) {

   const char* ptr = identifiers;
   const char* ePtr;
   UInt i = 0;
   while ((ePtr = strchr(ptr, '\0')) - ptr > 0) {
      nameV[i] = builtInName$(ptr);
      if (i++ == 0)
         _name = nameV[0];          // Module name is first name
      ptr = ePtr + 1;
   }
}

// Post constructor initialisation
// Assignes type array slots

void BuiltInModule::init() {

   #ifdef DEBUG_INIT
      outStream << "Assigning type array slots for Module :";
      printName(_name, outStream, Env(builtInNameTable()));
      outStream << '\n';
      outStream.flush();
   #endif

   UInt typeIndex = 0;
   unsigned char* data = _typeData;
/*
   while (data < _typeData + _typeDataLen) {
      TypeSig typeSig = unpackTypeSig(data, _nameV, builtInNameTable(), builtInMSA());

#ifdef DEBUG_INIT
      printExpr(typeSig, outStream, Env(builtInNameTable()));
      outStream << '\n';
      outStream.flush();
#endif

      _typeV[typeIndex++] = useType(typeSig, builtInTypeTable());
   }
*/
}

IAddress BuiltInModule::lookUpImport(Name name, const Env& env) {
   return NULL; // T.B.D.
}
