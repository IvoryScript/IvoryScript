/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    loadSegment.cpp
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
 *    loadSegment(archive, segment, len, nameTable, typeTable)
 *
 *       archive     (Archive&)     - Archive stream
 *       segment     (Iaddress)     - Segment address
 *       len         (SEGMENT_WORD) - Segment length (0 if not known)
 *       nameTable   (NameTable&)   - Name table
 *       typeTable   (TypeTable&)   - Type table
 * 
 * Method:
 * 
 *    Essentially a bytewise dump, except that built-in names and types are
 *    first mapped from their local ones in the given name and type tables.
 *
 * Errors:
 *
 *    None.
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
#include "ivory/iSegmentTable.h"
#include "ivory/segment.h"

Void loadSegment(Archive& archive, IAddress segment,
                 SEGMENT_WORD segLen,
                 const NameTable& nameTable,
                 const TypeTable& typeTable) {
   if (segLen == 0)
      archive >> segLen;
   SEGMENT_WORD rackLen;
   archive >> rackLen;

   IAddress iAddr = segment;
   insSegWord(iAddr, segLen);
   iAddr += SEGMENT_WORD_SIZE;
   insSegWord(iAddr, rackLen);
   iAddr += SEGMENT_WORD_SIZE;
   archive.getBytes(((Byte*)iAddr), SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE);
   iAddr += SEGMENT_HEADER_SIZE - 2 * SEGMENT_WORD_SIZE;

// Load zero bytes for the rack

   for (UInt i = 0; i < rackLen; i++)
      archive >> ((Byte*)iAddr)[i];
   iAddr += rackLen;

   UInt16 nNames;
   archive >> nNames;
   insBits16(iAddr, nNames);
   iAddr += 2;
   for (UInt i = 0; i < nNames; i++) {
      Name name;
      archive >> name;
      Name builtInName = nameTable.altName(name);
      insBits16(iAddr, builtInName == NULL_NAME ? name + builtInNameCount : builtInName);
      iAddr += 2;
   }

   UInt16 nTypes;
   archive >> nTypes;
   insBits16(iAddr, nTypes);
   iAddr += 2;
   for (UInt i = 0; i < nTypes; i++) {
      Type type;
      archive >> type;
      Type builtInType = typeTable.altType(type);
      insBits16(iAddr, builtInType == NULL_NAME ? type + builtInTypeCount : builtInType);
      iAddr += 2;
   }

// Load child segments and intervening sections

   SEGMENT_WORD nChildSegs;
   archive >> nChildSegs;
   insSegWord(iAddr, nChildSegs);
   iAddr += SEGMENT_WORD_SIZE;
   IAddress childSegTblAddr = iAddr;
   for (UInt i = 0; i < nChildSegs; i++) {
      SEGMENT_WORD childOffset;
      archive >> childOffset;
      insSegWord(iAddr, childOffset);
      iAddr += SEGMENT_WORD_SIZE;
   }

   for (UInt i = 0; i < nChildSegs; i++) {
      SEGMENT_WORD childOffset = extSegWord(childSegTblAddr);
      archive.getBytes(((Byte*)iAddr), childOffset - (iAddr - segment));
      loadSegment(archive, segment + childOffset,
                  0,
                  nameTable, typeTable);

      iAddr += segmentLength(segment + childOffset); 
   }

   archive.getBytes(((Byte *)iAddr), (segment + segLen) - iAddr);
}
