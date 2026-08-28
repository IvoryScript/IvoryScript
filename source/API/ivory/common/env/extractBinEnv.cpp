/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinEnv.cpp
 *
 * Module:  Ivory common (env)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of extractBinEnv function. Binary serialisation of an
 *    environment.
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

#if (GARBAGE_COLLECTION==1)
#include <MSA_GC.h>
#else
#include <MSA.h>
#endif

#include "ivory/byte.h"
#include "ivory/env.h"
#include "ivory/ICellInfoMap.h"
#include "ivory/streams.h"
#include "ivory/iSegmentTable.h"

defineBuiltInFn_1_arg(extractBinEnv,
   builtInAp(typeCon(InputStream), typeCon(Byte)), typeCon(Env),
   is, InputStream_Byte)

   ExtractArchive ea(is->_inAct, isEnv);

   UInt blkLen = ea.extractVLU();

#if (GARBAGE_COLLECTION==1)
   MSA_GC& msa = *new MSA_GC(blkLen);
   msa.setEnabled(FALSE);
#else
   MSA& msa = *new MSA(blkLen);
#endif

   UInt nameTableSlots =   ea.extractVLU();
   UInt nameTableSegSize = ea.extractVLU();
   NameTable& nameTable = *new(msa) INameTable(nameTableSlots, nameTableSegSize, msa);
   nameTable.load(ea);

   UInt typeTableSlots = ea.extractVLU();
   UInt typeTableSegSize = ea.extractVLU();
   TypeTable& typeTable = *new(msa) TypeTable(typeTableSlots, typeTableSegSize, msa);
   typeTable.load(ea, nameTable, msa);

   ISegmentTable* segmentTable = NULL;
   ICellInfoMap* cellInfoMap = NULL;
   UInt segmentTableSlots = ea.extractVLU();
   if (segmentTableSlots > 0) {
      UInt segmentTableSegSize = ea.extractVLU();
      segmentTable = new(msa) ISegmentTable(segmentTableSlots, segmentTableSegSize, msa);
      segmentTable->load(ea, nameTable, typeTable, msa);

      UInt nCellInfoMapSlots = ea.extractVLU();
      cellInfoMap = new(msa) ICellInfoMap(nCellInfoMapSlots);
      cellInfoMap->load(ea, *segmentTable, msa);

      typeTable.postLoad(*segmentTable);
   }

   rEnv = new(msa) Env(msa, nameTable, segmentTable, cellInfoMap, typeTable);
   if (segmentTable != NULL)
      segmentTable->postLoad(*rEnv);

   jump(popLabel());

endBuiltInFn
