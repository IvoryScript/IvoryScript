/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    copySegment.cpp
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
 *    Function to a copy a byte code segment to a destination environment.
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
#include "ivory/segment.h"
#include "ivory/iSegmentTable.h"

//#define TRACE_MAPPING            // If defined, traces mapping of names and types

// Map data for segment
// 1. Recurse through children
// 2. Environment pointer
// 3. Name and type tables

static Void mapData(IAddress segment, Env& env, MSA& msa) {

// Segment length 

   IAddress iAddr = segment + SEGMENT_RACK_LENGTH_OFFSET;
   SEGMENT_WORD rackSize = extSegWord(iAddr);

   iAddr += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE;
   IAddress rackAddr = iAddr;

// Replace environment pointer

   Env* segEnv = (Env*)extSegWord(iAddr);
   insBits32(iAddr - SEGMENT_WORD_SIZE, (Bits32)&env);
   iAddr += rackSize - SEGMENT_WORD_SIZE;

// Map name constants

   UInt nNames = extBits16(iAddr);
   IAddress namesAddr = iAddr;
   for (UInt i = 0; i < nNames; i++) {
      Name name = extBits16(iAddr);
      Name mappedName = mapName(name, segEnv->nameTable(), env.nameTable());

#ifdef TRACE_MAPPING
      outStream << "Mapped name: ";
      printName(name, outStream, *segEnv);
      outStream << " from " << name << " to " << mappedName << endl;
#endif

      insBits16(iAddr - sizeof(Bits16), mappedName);  
   }

// Map type constants

   UInt nTypes = extBits16(iAddr);
   for (UInt i = 0; i < nTypes; i++) {
      Type type = extBits16(iAddr);
      Type mappedType = mapType(type, *segEnv, env);

#ifdef TRACE_MAPPING
      outStream << "Mapped type: ";
      printType(type, outStream, *segEnv);
      outStream << " from " << type << " to " <<  mappedType << endl;
#endif
 
      insBits16(iAddr - sizeof(Bits16), mappedType);
   }

// Map data for child segments

   UInt n = extSegWord(iAddr);
   UInt i;
   for (i = 0; i < n; i++)
      mapData(segment + extSegWord(iAddr), env, msa);

   assignSegmentImportSection(iAddr, env, namesAddr, rackAddr);
}

// Copy a byte code segment from one environment to another

SegmentId copySegment(const IAddress segment, Env& env) {
   if (env.segmentTable() != NULL) {

      IAddress iAddr = segment;
      UInt len = extSegWord(iAddr);
      UInt rackSize = extSegWord(iAddr);
      UInt checkSum = extSegWord(iAddr);

      SegmentId segmentId = env.segmentTable()->lookUp(segment, len, checkSum);
      if (segmentId != NULL_SEGMENT_ID) {

//         outStream << "Found segment " << (void*)segment << endl; //???

         return segmentId;
      }

//      outStream << "Adding segment" << endl; //???

      segmentId = env.segmentTable()->addSegment((const Byte*)segment, len, checkSum);
      const IAddress segmentCopy = (const IAddress)env.segmentTable()->segment(segmentId);
        
      mapData(segmentCopy, env, env.msa());

//--- Finally, execute initialisation code with a minimal VM
//--- Typically to re-allocate any cell info structures etc.

      initSegment(segmentCopy, env);

      return segmentId;
   }
   return NULL_SEGMENT_ID;
}
