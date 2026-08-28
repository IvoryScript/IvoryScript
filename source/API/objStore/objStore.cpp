/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    ObjStore.cpp
 *
 * Module:  Object store class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 7 December 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Constructor and destructor
 *
 * Call format:
 *
 *    ObjStore(nObjPerSeg, blkLen);
 *
 *       nObjPerSeg -   (unsigned int)    - No. objects per table segment
 *       blkLen -       (size_t)          - Cluster MSA block size
 *
 *    ~ObjStore();
 *
 * Method:
 *
 *    Constructor initialises all member variables.
 *    Destructor releases all allocated memory.
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

#include "objStore.h"

ObjStore::ObjStore(unsigned int objsPerSeg, size_t blkLen, MSA& msa)
   : _objsPerSeg(objsPerSeg), _blkLen(blkLen), _msa(msa),
     _clustTbl(NULL), _nClusters(0),
     _minFreeIndex(-1), _nObj(0), _maxNObj(0) {
}

ObjStore::~ObjStore() {
}

void* ObjStore::operator new(size_t size, MSA& msa) {
   return msa.alloc(size);
}

#ifdef DELETE_NEEDS_NEW_ARG
void ObjStore::operator delete(void* ptr, MSA& msa)
#else
void ObjStore::operator delete(void* ptr)
#endif
{
}
