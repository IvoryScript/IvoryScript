/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    eqSegment.cpp
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
 *    Function to compare two segments for equivalence.
 *
 * Notes:
 * 
 *    Essentially a bytewise comparison, except that the name and type
 *    constant tables are relative to the host environment, so require a
 *    specific name comparison.
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

Bool eqSegment(IAddress seg1, IAddress seg2) {
   IAddress iAddr1 = seg1;
   IAddress iAddr2 = seg2;

   SEGMENT_WORD seg1len = extSegWord(iAddr1);
   SEGMENT_WORD seg2len = extSegWord(iAddr2);
   if (seg1len != seg2len)
      return FALSE;

   SEGMENT_WORD rack1len = extSegWord(iAddr1);
   SEGMENT_WORD rack2len = extSegWord(iAddr2);
   if (rack1len != rack2len ||
      memcmp(iAddr1, iAddr2, SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE) != 0)
      return FALSE;

   iAddr1 += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE;
   iAddr2 += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE;

   // Environment pointers

   Env* seg1Env = (Env*)extSegWord(iAddr1);
   Env* seg2Env = (Env*)extSegWord(iAddr2);

   iAddr1 += rack1len - SEGMENT_WORD_SIZE;
   iAddr2 += rack2len - SEGMENT_WORD_SIZE;

   UInt nNames1 = extBits16(iAddr1);
   UInt nNames2 = extBits16(iAddr2);
   if (nNames1 != nNames2)
      return FALSE;

   for (UInt i = 0; i < nNames1; i++)
      if (!eqName_(extBits16(iAddr1), seg1Env->nameTable(),
                   extBits16(iAddr2), seg2Env->nameTable()))
         return FALSE;

   UInt nTypes1 = extBits16(iAddr1);
   UInt nTypes2 = extBits16(iAddr2);
   if (nTypes1 != nTypes2)
      return FALSE;

   for (UInt i = 0; i < nTypes1; i++)
      if (!eqType_(extBits16(iAddr1), seg1Env->typeTable(),
                   extBits16(iAddr2), seg2Env->typeTable()))
         return FALSE;

// Match child segments and intervening sections

   UInt nChildSegs1 = extSegWord(iAddr1);
   UInt nChildSegs2 = extSegWord(iAddr2);
   if (nChildSegs1 != nChildSegs2)
      return FALSE;
   IAddress remAddr1 = iAddr1 + nChildSegs1 * SEGMENT_WORD_SIZE;
   IAddress remAddr2 = iAddr2 + nChildSegs2 * SEGMENT_WORD_SIZE;

   for (UInt i = 0; i < nChildSegs1; i++) {
      SEGMENT_WORD child1offset = extSegWord(iAddr1);
      SEGMENT_WORD child2offset = extSegWord(iAddr2);
      if (child1offset != child2offset ||
         memcmp(remAddr1, remAddr2, child1offset - (iAddr1 - seg1)) != 0 ||
         !eqSegment(seg1 + child1offset, seg2 + child2offset))
         return FALSE;
      remAddr1 += segmentLength(seg1 + child1offset);
      remAddr2 += segmentLength(seg2 + child2offset);
   }

// Remainder of segment

   return memcmp(remAddr1, remAddr2, (seg1 + seg1len) - remAddr1) == 0;
}
