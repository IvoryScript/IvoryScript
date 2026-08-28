/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    storeSegment.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Method to store a serialized segment to an archive stream.
 *
 * Call format:
 *
 *    storeSegment(archive)
 *
 *       archive  (Archive&)  - Archive stream
 *
 * Method:
 * 
 *    Essentially a bytewise dump, except that any built-in names and types
 *    are first mapped to their local ones in the given name and type tables.
 *
 * Errors:
 *
 *    None.
 *
 * Notes:
 *
 *    A number of length fields could be stored in a shorter form as
 *    variable length integers.  However, because the potential gain is
 *    unlikely to be significant, this approach hasn't been adopted.
 *    Thus easing comparison of, say, an archived segment with another in
 *    memory.
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
#include "ivory/segment.h"

Void storeSegment(Archive& archive, const IAddress segment,
                  const NameTable& nameTable,
                  const TypeTable& typeTable) {
   IAddress iAddr = segment;

   archive.putBytes(((Byte*)iAddr), SEGMENT_HEADER_SIZE);

   SEGMENT_WORD segLen = extSegWord(iAddr);
   SEGMENT_WORD rackLen = extSegWord(iAddr);
   iAddr += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE + rackLen;

   for (UInt i = 0; i < rackLen; i++)
      archive << (Byte)0;

   UInt16 nNames = extBits16(iAddr);
   archive << nNames;
   for (UInt i = 0; i < nNames; i++) {
      Name name = extBits16(iAddr);
      UInt hashVal = nameHashVal(name, nameTable);
      archive << (Name)(name >= builtInNameCount
         ? name - builtInNameCount
         : nameTable.lookUpByAlt(name, hashVal)->_name);
   }

   UInt16 nTypes = extBits16(iAddr);
   archive << nTypes;
   for (UInt i = 0; i < nTypes; i++) {
      Type type = extBits16(iAddr);
      UInt hashVal = typeHashVal(type, typeTable);
      archive << (Type)(type >= builtInTypeCount
         ? type - builtInTypeCount
         : typeTable.lookUpByAlt(type, hashVal));
   }

// Store child segments and intervening sections

   SEGMENT_WORD nChildSegs = extSegWord(iAddr);
   archive << nChildSegs;
   IAddress childSegTblAddr = iAddr;

   for (UInt i = 0; i < nChildSegs; i++)
      archive << (SEGMENT_WORD)extSegWord(iAddr);

   for (UInt i = 0; i < nChildSegs; i++) {
      SEGMENT_WORD childOffset = extSegWord(childSegTblAddr);
      archive.putBytes(((Byte*)iAddr), childOffset - (iAddr - segment));
      storeSegment(archive, segment + childOffset,
                   nameTable, typeTable);
 
      iAddr += segmentLength(segment + childOffset); 
   }

   archive.putBytes(((Byte *)iAddr), (segment + segLen) - iAddr);
}
