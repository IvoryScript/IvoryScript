/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    object.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with basic objects
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

#ifndef IVORY_BASIC_OBJECT_H_DEFINED
#define IVORY_BASIC_OBJECT_H_DEFINED

#include "ivory/ads.h"
#include "ivory/refList.h"

#define MAX_BASIC_OBJECT_BODY_SIZE 65536

class BasicObject {
public:
   BasicObject(Type type, Type reprType)
      : _type(type), _reprType(reprType), _revRefs() {}

   void* operator new(size_t size, void* ptr);
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr1, void* ptr2)
#else
   void operator delete(void* ptr)
#endif
   {}

public:
   Type     _type;
   Type     _reprType;
   RefList  _revRefs;
   unsigned char _body[MAX_BASIC_OBJECT_BODY_SIZE];
};

#define objectBody(obj,t) (*(t*)((obj)._body))

#define objectMSA(r,adsi)\
((adsi).objStore().queryMSA(r))

#define lockObject(r,adsi)\
((BasicObject*)((adsi).objStore().lock(r)))

#define objectType(r,adsi) (lockObject((r),(adsi))->_type)

declareBuiltInFn(destroyObject);

declareBuiltInFn(destroyObjectExt);

declareBuiltInFn(mapProcObjects);

#endif /* IVORY_BASIC_OBJECT_H_DEFINED */
