/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    main.cpp
 *
 * Module:  IvoryScript compiler suite driver
 *
 * Author:  A Scott
 *
 * Date:    20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    The main function for an IvoryScript driver suite
 *
 * Call format:
 *
 *    ivorysc <suite filename> <output file>
 *
 * Method:
 *
 *    An application to compile (run) a suite of IovryScript files.
 *
 * Errors:
 *
 *    Errors will be reported to stderr.
 *
 * Notes:
 *
 * Modification history:
 *
 *-----------------------------------------------------------------------------
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

#include <iostream>
using namespace std;

#include "nameTable.h"
#include "ivory/builtInDataCon.h"
#include "ivory/byteCodeModule.h"
#include "ivory/class.h"
#include "ivory/frameDescr.h"
#include "ivory/typeTable.h"
#include "suite.h"
#include <ivory/compiler/compiler.h>

extern struct Linkage linkage_common;
//extern struct Linkage linkage_comms;
//extern struct Linkage linkage_measurement;
//extern struct Linkage linkage_serialIO;
//extern struct Linkage linkage_socket;
//extern struct Linkage linkage_timer;

static Linkage* linkage[] =
{
   &linkage_common,
   //   &linkage_comms,
   //   &linkage_measurement,
   //   &linkage_serialIO,
   //   &linkage_socket,
   //   &linkage_timer,
};

// The following is aimed at forcing
// Linkage to be referenced

extern Linkage** linkage_(Void) {
   return linkage;
}

Void postConstructorInit(Void) {

// Post constructor initialisation

   TypeDescr::initGlobalList();

   Module::initGlobalList();
   TypeCon::initBuiltInList();
   BuiltInDataCon::initGlobalList();
   BuiltInFn::initGlobalList();
   Class::initGlobalLists();

#if (FRAME_DESCRIPTORS==1)
   BuiltInFrameDescr::initGlobalList();
#endif

   builtInNameCount = builtInNameTable().count();
   builtInTypeCount = builtInTypeTable().count();

#ifdef DEBUG_INIT
   for (Type i = 0; i < builtInTypeCount; i++)
   {
      TypeDescr* descr = builtInTypeTable().typeDescr(i);
      traceExpr(descr->_typeSig, Env(builtInNameTable()));
   }
   outStream << "Primitive memory usage\n";
   builtInMSA().info(outStream, 0);
#endif
}

int main(int argc, char* argv[]) {
   postConstructorInit();

   if (argc < 2) {
     cout << "argv[0]: no input file specified\n";
     return 1;
   }

   if (argc > 3) {
      cout << "argv[0]: too many arguments\n";
      return 1;
   }

   return suite(argv[1], argv[2], "scripts/IvoryScript", "data/ivory/");
}
