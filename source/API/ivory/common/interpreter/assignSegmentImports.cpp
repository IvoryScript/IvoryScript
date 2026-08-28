/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    assignSegmentImports.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2019
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Assign a segment's imported references and values.
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

#include "ivory/exec.h"
#include "ivory/module.h"
#include "ivory/segment.h"
//#include "ivory/trace.h"

// assignSegmentImportSection: Assign global imports for section within segment 

Void assignSegmentImportSection(IAddress baseAddr, Env& env, IAddress namesAddr, IAddress rackAddr) {
   IAddress iAddr = baseAddr;

// Imported Names

   UInt nImports = extBits16(iAddr);

   for (UInt i = 0; i < nImports; i++) {
      (Void)extBits16(iAddr);    // Reserved (for module name
      UInt16 nameIndex = extBits16(iAddr);
      IAddress nameAddr = namesAddr + nameIndex * sizeof(Name);
      Name name = extBits16(nameAddr);
      UInt32 offset = extSegWord(iAddr);
/*
      outStream << "Assigning ";
      printName(name, outStream, env.nameTable());
      outStream << " to " << (Void*)rackAddr << '[' << offset << "] " << (Void*)Module::importNameAddr(name, env) << '\n';
*/
      IAddress addr = Module::importNameAddr(name, env);
      if (addr == NULL)
         ivoryError1("Unable to import: %s", nameString_(name, env.nameTable()));
      *(IAddress*)(rackAddr + offset) = addr;
   }
}

// assignSegmentImports: assign all segment imports for a segment 

Void assignSegmentImports(IAddress segment, Env& env) {
   IAddress iAddr = segment + SEGMENT_RACK_LENGTH_OFFSET; 

#if (EXTERNAL_RACK==0)
   UInt32 rackSize = extSegWord(iAddr);
   iAddr += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE;
   IAddress rackAddr = iAddr;
   *(Env**)(iAddr) = &env;

   iAddr += rackSize;
#else
   assert(FALSE, "T.B.D.")
#endif

// Skip over Name constants

   UInt nNames = extBits16(iAddr);
   IAddress namesAddr = iAddr;
   iAddr += nNames * sizeof(Name);

// Skip over type constants

   UInt nTypes = extBits16(iAddr);
   iAddr += nTypes * sizeof(Type);

   UInt n = extSegWord(iAddr);
   UInt i;
   for (i = 0; i < n; i++)
      (Void)assignSegmentImports(segment + extSegWord(iAddr), env);

// Assign child segment imports

   assignSegmentImportSection(iAddr, env, namesAddr, rackAddr);
}
