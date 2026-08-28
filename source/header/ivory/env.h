/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    env.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Env data type
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

#ifndef IVORY_ENV_H_DEFINED
#define IVORY_ENV_H_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/type.h"

#define sizeOfEnv sizeof(Env*)

extern Env& builtInEnv(Void);

declareTypeCon(Env);

declareType(Env);

#define stackFPSizeEnv stackSlotSize(Env)

#define loadArgRegEnv(name) Env* name=rEnv

#define loadArgEnv(name,off) Env* name=stack(off,Env*)
#define storeArgEnv(name,off) stack(off,Env*)=name

#define argRegFromCellEnv(s,c) rEnv=cellBody(*cell,s).c

#define mappedEnv(name,srcEnv,dstEnv) name

#define returnEnv(env){\
rEnv=env;\
jump(popLabel());}

declareBuiltInDataCon(Env);

declareBuiltInFn(destroyEnv);

declareBuiltInFn(extractBinEnv);

declareBuiltInFn(insertBinEnv);

declareBuiltInFn(mkEnv);

declareBuiltInFn(setEnv);

#endif /* IVORY_ENV_H_DEFINED */
