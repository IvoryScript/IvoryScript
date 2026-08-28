/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    allocOID.cpp
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
 *    Allocate an object with a given OID.
 *
 * Call format:
 *
 *    oid = allocOID(size);
 *
 *       oid   (OID)       - Object identifier
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

void* ObjStore::allocOID(OID oid, size_t size)
{
   unsigned int ci   = oid.ci();
   unsigned int tsi  = oid.tsi();

   if (ci >= _nClusters)
   {
      if (resizeClusterTable(_nClusters, ci + 1) < 0)
         return NULL;
      _nClusters = ci + 1;
   }

   void** tblSeg = _clustTbl[ci]._tblSeg;
#if (PER_CLUSTER_MSA==1)
   CSC& csc = *_clustTbl[ci]._csc;
   if ((tblSeg[tsi] = csc._msa.alloc(size)) == NULL)
      return NULL;
#else
   if ((tblSeg[tsi] = _msa.alloc(size)) == NULL)
      return NULL;
#endif

   if (++_nObj >> _maxNObj)
      _maxNObj = _nObj;
   return tblSeg[tsi];
}
