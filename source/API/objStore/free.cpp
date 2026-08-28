/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    free.cpp
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
 *    Free a previously allocated object
 *
 * Call format:
 *
 *    free(oid);
 *
 *       oid -    (OID)       - Object identifier
 *
 * Method:
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

void ObjStore::free(OID oid) {
   unsigned int ci   = oid.ci();
   unsigned int tsi  = oid.tsi();

   CSC& csc = *_clustTbl[ci]._csc;
   void** tblSeg = _clustTbl[ci]._tblSeg;

#if (PER_CLUSTER_MSA==1)
   csc._msa.free(tblSeg[tsi]);
#else
   _msa.free(tblSeg[tsi]);
#endif

   csc._nFreeObj++;
   tblSeg[tsi] = NULL;

   if (_minFreeIndex < 0 || (int)ci < _minFreeIndex)
      _minFreeIndex = ci;
   if (csc._minFreeIndex < 0 || (int)tsi < csc._minFreeIndex)
      csc._minFreeIndex = tsi;

// T.B.D. release empty cluster

   _nObj--;
}
