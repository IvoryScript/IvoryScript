/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    any.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Any (*) data type
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

#ifndef IVORY_ANY_H_DEFINED
#define IVORY_ANY_H_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/env.h"
#include "ivory/typeTable.h"

class Any {
public:
   Any(Void) 
      : _type(NULL_TYPE), _padding(0), _valPtr(NULL) {}
   Any(Type type, Void* valPtr)
      : _type(type), _padding(0), _valPtr(valPtr) {}
   Any(const Any& src, const Env& srcEnv, Env& env, MSA& msa);
   Void destroy(Env& env, MSA& msa);

   Void print(ostream& os, const Env& env) const;

   inline Type type_(Void) const { return _type; }
   inline Ptr valPtr(Void) const { return _valPtr; }

protected:
   Type     _type;
   UInt16   _padding;
   Ptr      _valPtr;
};

#define sizeOfAny sizeof(Any)

#define typeSpecAny Any

declareTypeCon(Any);

#define stackFPSizeAny (stackSlotSize(Any)+stackSlotSize(Env))

#define loadArgRegAny(name)\
Any name=*static_cast<Any*>((Void*)rStruct);\
Env* name##Env=rEnv

#define loadArgAny(name,off)\
Env* name##Env=stack(off+stackSlotSize(Any),Env*);\
Any name=stack((off),Any)

#define storeArgAny(name,off)\
stack(off+stackSlotSize(Any),Env*) = name##Env;\
stack((off),Any)=name;

#define argRegFromCellAny(s,c){\
if(sizeof(Any)>rStructSize){\
consMSA->free(rStruct);\
rStruct=(Byte*)consMSA->alloc(sizeof(Any));\
rStructSize=sizeof(Any);\
};\
memcpy(rStruct,&(cellBody(*cell,s).c),sizeof(Any));\
rEnv=cellEnv;}

#define map_Any(name,srcEnv,dstEnv) mapAny_(name,*(srcEnv),*(dstEnv) n_vm)

extern Any mapAny_(const Any& any, const Env& srcEnv, Env& dstEnv argN_VM);

// Tag any return value

#define tagAny() builtInTypeTable().typeDescr(rType)->_tagFn(vm)


declareBuiltInFn(evalAny);

declareBuiltInFn(mapAnyPtr);

declareBuiltInFn(markAny_GC);

#if (GARBAGE_COLLECTION==1)
Void markAny_GC_(const Any& any, Env& env argN_VM);
Void markBuiltInPAPArg_GC(const Any& any, Env& env argN_VM);
#endif

declareBuiltInFn(extractBinAnyPtr);

declareBuiltInFn(insertBinAnyPtr);

declareBuiltInFn(showAny);

declareBuiltInFn(insertTxtAny);

#endif /* IVORY_ANY_H_DEFINED */
