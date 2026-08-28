/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    basicDataStore.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Abstract class for use with basic data storage
 * 
 *    Currently, it includes transient and persistent forms
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
 *
 */

#ifndef IVORY_BASIC_DATA_STORE_H_DEFINED
#define IVORY_BASIC_DATA_STORE_H_DEFINED

class BasicDataStore {
public:
   BasicDataStore(size_t        blkLen);
   ~BasicDataStore();

   void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr, MSA& msa)
#else
   void operator delete(void* ptr)
#endif
   {
   }
   inline MSA& msa(Void) { return _msa; }

public:
   MSA         _msa;       // Common MSA for environment and object store
};

#endif /* IVORY_BASIC_DATA_STORE_H_DEFINED */
