/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ptr.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Ivory primitive polymorphic Ptr data type.
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

#ifndef IVORY_PTR_H_DEFINED
#define IVORY_PTR_H_DEFINED

#include "ivory/type.h"

declareBuiltInTypeCon(Ptr);

declareBuiltInDataCon(Null);
declareBuiltInDataCon(Ptr);

#define NULL_TAG  0
#define PTR_TAG_  1

#define isPtrType(typeSig) (isAp(typeSig)&&(fun(typeSig)==typeCon(Ptr)))

#define loadArgReg_PTR(name)\
Env* name##Env=rEnv;\
Ptr name=rPtr

#define returnNull(){\
rPtr=NULL;\
rTag=NIL_TAG;\
jump(popLabel());}

#define returnPtr(ptr,env){\
rPtr=ptr;\
rEnv=env;\
jump(popLabel());}

declareBuiltInFn(eqPtr);

declareBuiltInFn(eqPlainPtr);

#endif /* IVORY_PTR_H_DEFINED */
