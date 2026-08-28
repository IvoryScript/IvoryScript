/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    env.cpp
 *
 * Module:  Ivory common (env)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Env Type
 *
 * Notes:
 * 
 *    T.B.D. Support partial applications
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

#include "MSA_GC.h"
#include "ivory/env.h"
#include "ivory/ICellInfoMap.h"
#include "ivory/int.h"
#include "ivory/iFrameDescr.h"
#include "ivory/nameTable.h"
#include "ivory/iSegmentTable.h"
#include "ivory/typeTable.h"
#include "ivory/void.h"

static Env* _builtInEnv = NULL;

Env& builtInEnv() {
   if (_builtInEnv == NULL)
      _builtInEnv = new (builtInMSA()) Env(builtInNameTable(), NULL, NULL, builtInTypeTable());
   return *_builtInEnv;
}

defineTypeCon(Env);

defineType_n(1, Env, typeCon(Env), NULL);

/*----------------------------------------------------------------------------*/

// Env :: Int -> Int -> Int -> Int -> Int -> Int -> Int -> Env

// Args: 1. Storage allocation block size
//       2. Name hash table slots
//       3. Name vector segment size
//       4. Segment hash table slots
//       5. Segment vector segment size
//       6. CellInfo hash table slots
//       7. Type hash table slots
//       8. Type vector segment size

defineDataCon(Env, 1, typeCon(Env),
   builtInAp2(ARROW,
      typeCon(Int),
      builtInAp2(ARROW,
         typeCon(Int),
         builtInAp2(ARROW,
            typeCon(Int),
            builtInAp2(ARROW,
               typeCon(Int),
               builtInAp2(ARROW,
                  typeCon(Int),
                  builtInAp2(ARROW,
                     typeCon(Int),
                     builtInAp2(ARROW,
                        typeCon(Int),
                        builtInAp2(ARROW,
                           typeCon(Int),
                           typeCon(Env))))))))))
#define arg8         sizeof(Int)
#define arg7         (arg8+sizeof(Int))
#define arg6         (arg7+sizeof(Int))
#define arg5         (arg6+sizeof(Int))
#define arg4         (arg5+sizeof(Int))
#define arg3         (arg4+sizeof(Int))
#define arg2         (arg3+sizeof(Int))
#define stackDepth  arg2
{
   checkArgs("Env");
   jump(altEntry(Env));
}

defineAltEntry(Env) {
   MSA msa(rInt);
   Env* env = new(msa) Env(*new(msa) INameTable(local(arg2, Int), local(arg3, Int), msa),
                            new(msa) ISegmentTable(local(arg4, Int), local(arg5, Int), msa),
                            new(msa) ICellInfoMap(local(arg6, Int)),
                           *new(msa) TypeTable(local(arg7, Int), local(arg7, Int), msa));

   dropStack();
   rEnv = env;
   rType = builtInType(Env);
   jump(popLabel());
}

#undef stackDepth

/*----------------------------------------------------------------------------*/

// setEnv :: Env -> Void

defineBuiltInFn(setEnv,
   builtInAp2(ARROW,
      typeCon(Env),
      typeCon(Void)))
#define stackDepth  0
{
   checkArgs("setEnv");
   jump(altEntry(setEnv));
}

defineAltEntry(setEnv) {
   consEnv = rEnv;
   consMSA = &consEnv->msa();
   dropStack();
   returnVoid();
}
#undef stackDepth

defineBuiltInFn(mkEnv,
  builtInAp2(ARROW,
      typeCon(Int),
      builtInAp2(ARROW,
         typeCon(Int),
         builtInAp2(ARROW,
            typeCon(Int),
            builtInAp2(ARROW,
               typeCon(Int),
               builtInAp2(ARROW,
                  typeCon(Int),
                  builtInAp2(ARROW,
                     typeCon(Int),
                     builtInAp2(ARROW,
                        typeCon(Int),
                        builtInAp2(ARROW,
                           typeCon(Int),
                           typeCon(Env))))))))))
#define arg8         sizeof(Int)
#define arg7         (arg8+sizeof(Int))
#define arg6         (arg7+sizeof(Int))
#define arg5         (arg6+sizeof(Int))
#define arg4         (arg5+sizeof(Int))
#define arg3         (arg4+sizeof(Int))
#define arg2         (arg3+sizeof(Int))
#define stackDepth  arg2 
{
   checkArgs("Env");
   jump(altEntry(mkEnv));
}

defineAltEntry(mkEnv) {
   MSA_GC& msa = *new MSA_GC(rInt);
   msa.setEnabled(FALSE);     // T.B.D. Enable GC for independent environments
   Env* env_ = new(msa) Env(msa,
                            *new(msa) INameTable(local(arg2, Int), local(arg3, Int), msa),
                             new(msa) ISegmentTable(local(arg4, Int), local(arg5, Int), msa),
                             new(msa) ICellInfoMap(local(arg6, Int)),
                            *new(msa) TypeTable(local(arg7, Int), local(arg8, Int), msa));

   dropStack();
   rEnv = env_;
   jump(popLabel());
}

defineBuiltInFn_1_arg(destroyEnv,
   typeCon(Env), typeCon(Void),
   env, Env)

   MSAFrameDescrMapAssoc::remove(&(env->msa()));

   delete &(env->msa());

returnVoid();
endBuiltInFn

#undef stackDepth
