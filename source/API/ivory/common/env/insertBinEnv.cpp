/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertBinEnv.cpp
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
 *    Implementation of insertBinEnv function. Binary serialisation of an
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

#include "ivory/byte.h"
#include "ivory/env.h"
#include "ivory/ICellInfoMap.h"
#include "ivory/streams.h"
#include "ivory/void.h"
#include "ivory/iSegmentTable.h"
Void traceInterpreter(Bool flag);
Bool interpreterTraceFlag(Void);

defineBuiltInFn_2_args(insertBinEnv,
   builtInAp(typeCon(OutputStream), typeCon(Byte)), typeCon(Env), typeCon(Void),
   os, OutputStream_Byte,
   env, Env)

   Bool traceFlag = interpreterTraceFlag();
   traceInterpreter(FALSE);

   InsertArchive ia(os->_outputFn, osEnv);
   ia.insertVLU(env->msa().queryBlkLen());

   ia.insertVLU(env->nameTable().nSlots());
   ia.insertVLU(env->nameTable().nPerSeg());
   env->nameTable().store(ia);
  
   ia.insertVLU(env->typeTable().nSlots());
   ia.insertVLU(env->typeTable().nPerSeg());
   env->typeTable().store(ia, env->nameTable());

   if (env->segmentTable() != NULL) {
      ia.insertVLU(env->segmentTable()->nSlots());
      ia.insertVLU(env->segmentTable()->nPerSeg());
      env->segmentTable()->store(ia, env->nameTable(),
                                     env->typeTable());

      env->cellInfoMap()->store(ia, *env->segmentTable());
   } else
      ia.insertVLU(0);

   if (traceFlag)
      traceInterpreter(TRUE);


   drop_Stack(insertBinEnv);
   returnVoid();
endBuiltInFn
