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
#include "ivory/name.h"
#include "ivory/segment.h"

SEGMENT_WORD hashSegment(IAddress segment, Bool verify/* = TRUE*/) {
   HashDJB2 hashDJB2;

   IAddress iAddr = segment;
   hashDJB2.gen(iAddr, SEGMENT_HEADER_SIZE);

// Segment length 

   SEGMENT_WORD segLen = extSegWord(iAddr);
   SEGMENT_WORD rackSize = extSegWord(iAddr);

   iAddr += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE;

   Env* segEnv = (Env*)extSegWord(iAddr);
   iAddr += rackSize - SEGMENT_WORD_SIZE;

// Hash name constants

   hashDJB2.gen(iAddr, 2);
   UInt nNames = extBits16(iAddr);
   IAddress namesAddr = iAddr;
   for (UInt i = 0; i < nNames; i++) {
      const String str = nameString_(extBits16(iAddr), segEnv->nameTable());
      hashDJB2.gen((const Byte*)str, strlen(str));
   }

// Hash type constants
   hashDJB2.gen(iAddr, 2);
   UInt nTypes = extBits16(iAddr);
   for (UInt i = 0; i < nTypes; i++)
      hashTypeSig_(typeSignature(extBits16(iAddr), segEnv->typeTable()), *segEnv, hashDJB2);

// Hash child segments table and intervening sections

   hashDJB2.gen(iAddr, SEGMENT_WORD_SIZE);
   UInt nChildSegs = extSegWord(iAddr);
   IAddress remAddr = iAddr + nChildSegs * SEGMENT_WORD_SIZE;
   for (UInt i = 0; i < nChildSegs; i++) {
      hashDJB2.gen(iAddr, SEGMENT_WORD_SIZE);
      IAddress childSeg = segment + extSegWord(iAddr);
      hashDJB2.gen(remAddr, childSeg - remAddr);
      SEGMENT_WORD childHash = hashSegment(childSeg, verify);
      hashDJB2.gen((const Byte*)&childHash, SEGMENT_WORD_SIZE);
      remAddr = childSeg + segmentLength(childSeg);
   }

// Remainder of segment

   hashDJB2.gen(remAddr, segment + segLen - remAddr);

   if (!verify)
      insSegWord(segment + SEGMENT_HASH_OFFSET, hashDJB2.hash());

   return hashDJB2.hash();
}
