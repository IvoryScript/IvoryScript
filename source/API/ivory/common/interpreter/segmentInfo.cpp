/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    hashSegment.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 1 September 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to a compute a unique byte code segment hash value 
 *
 * Notes:
 * 
 *    2. Unique for name and type constants regardless of mapping.
 * 
 *    3. Unless 'verify' is TRUE, the computed hash is stored.
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
#include "ivory/iSegmentTable.h"
#include "ivory/name.h"
#include "ivory/segment.h"

Void segmentInfo(IAddress segment) {
   IAddress iAddr = segment;

// Segment length 

   SEGMENT_WORD segLen = extSegWord(iAddr);
   SEGMENT_WORD rackSize = extSegWord(iAddr);

   iAddr += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE;

   Env* segEnv = *(Env**)iAddr;
   iAddr += rackSize;

// Hash name constants

   UInt nNames = extBits16(iAddr);
   IAddress namesAddr = iAddr;
   for (UInt i = 0; i < nNames; i++) {
      if (i > 0)
         outStream << ", ";
      outStream << nameString_(extBits16(iAddr), segEnv->nameTable());     
   }
   if (nNames > 0)
      outStream << endl;

// Hash type constants

   UInt nTypes = extBits16(iAddr);
   for (UInt i = 0; i < nTypes; i++) {
      Type type = extBits16(iAddr);
      printTypeSig(segEnv->typeTable().typeSig(type), 0, FALSE, outStream, *segEnv);
      outStream << endl;
   }

   UInt nChildSegs = extSegWord(iAddr);
   IAddress remAddr = iAddr + nChildSegs * SEGMENT_WORD_SIZE;
   for (UInt i = 0; i < nChildSegs; i++)
      segmentInfo(segment + extSegWord(iAddr));

   UInt nImports = extBits16(iAddr);
   for (UInt i = 0; i < nImports; i++) {
      Bits16 b16 = extBits16(iAddr);
      Bits16 index = extBits16(iAddr);
      Bits32 offset = extBits32(iAddr);
      outStream << index << " " << offset << endl;
   }
}
