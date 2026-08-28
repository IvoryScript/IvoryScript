/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    segment.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
*
*-----------------------------------------------------------------------------
*
* Description:
*
*    Definitions for use with Ivory byte code segments
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

#ifndef IVORY_SEGMENT_H_DEFINED
#define IVORY_SEGMENT_H_DEFINED

#include "./segmentTable.h"
#include "ivory/common.h"

#define SEGMENT_WORD                   UInt32
#define SEGMENT_WORD_SIZE              (sizeof(SEGMENT_WORD))

#define SEGMENT_LENGTH_OFFSET          0
#define SEGMENT_RACK_LENGTH_OFFSET     (SEGMENT_LENGTH_OFFSET+SEGMENT_WORD_SIZE)
#define SEGMENT_HASH_OFFSET            (SEGMENT_RACK_LENGTH_OFFSET+SEGMENT_WORD_SIZE)
#define SEGMENT_INIT_CODE_OFFSET       (SEGMENT_HASH_OFFSET+SEGMENT_WORD_SIZE)

#define SEGMENT_HEADER_SIZE            (SEGMENT_INIT_CODE_OFFSET+SEGMENT_WORD_SIZE)

#define extSegWord                     extBits32

#define insSegWord(iAddr,val)          (insBits32(iAddr,val))

extern Void assignSegmentImportSection(IAddress baseAddr, Env& env,
                                       IAddress namesAddr, IAddress rackAddr);
extern Void assignSegmentImports(IAddress segment, Env& env);
extern SegmentId copySegment(const IAddress segment, Env& env);
extern Bool eqSegment(IAddress seg1, IAddress seg2);
extern SEGMENT_WORD hashSegment(IAddress segment, Bool verify = TRUE);
extern Void initSegment(const IAddress segment, Env& env);
extern Void loadSegment(Archive& archive, IAddress segment,
                        SEGMENT_WORD len,
                        const NameTable& nameTable,
                        const TypeTable& typeTable);
extern Env* segmentEnv(IAddress segment);
extern SEGMENT_WORD segmentHash(IAddress segment);
extern Void segmentInfo(IAddress segment);
extern SEGMENT_WORD segmentLength(IAddress segment);
extern Void* segmentRackSlotPtr(IAddress segment, UInt offset);
extern Void storeSegment(Archive& archive, const IAddress segment,
                         const NameTable& nameTable,
                         const TypeTable& typeTable);

#endif /* IVORY_SEGEMENT_H_DEFINED */
