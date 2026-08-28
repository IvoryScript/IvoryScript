/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    alloc.cpp
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
 *    Allocate an object
 *
 * Call format:
 *
 *    oid = stackAlloc(size);
 *
 *       size -   (size_t)       - Object size
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

OID ObjStore::alloc(size_t size) {
   unsigned int ci;
   unsigned int tsi;
   void** tblSeg;
   CSC* csc;

   if (_minFreeIndex >= 0){
      ci = _minFreeIndex;
      csc = _clustTbl[ci]._csc;
      tblSeg = _clustTbl[ci]._tblSeg;
      tsi = csc->_minFreeIndex;
      if (--csc->_nFreeObj > 0)
         for (; ++csc->_minFreeIndex, tblSeg[csc->_minFreeIndex] != NULL;)
            ;
      else {
         csc->_minFreeIndex = -1;
         do {
            if ((unsigned int)++_minFreeIndex >= _nClusters) {
               _minFreeIndex = -1;
               break;
            }
         } while (_clustTbl[_minFreeIndex]._csc->_nFreeObj == 0);
      }
   } else {
      if (resizeClusterTable(_nClusters, _nClusters + 1) < 0)
         return NULL_OID;
      ci = _nClusters++;
      csc = _clustTbl[ci]._csc;
      tblSeg = _clustTbl[ci]._tblSeg;
      tsi = 0;
      if (--csc->_nFreeObj > 0) {
         _minFreeIndex = ci;
         csc->_minFreeIndex = 1;
      }
   }
#if (PER_CLUSTER_MSA==1)
   if ((tblSeg[tsi] = csc->_msa.alloc(size)) == NULL)
      return NULL_OID;
#else
   if ((tblSeg[tsi] = _msa.alloc(size)) == NULL)
      return NULL_OID;
#endif

   if (++_nObj > _maxNObj)
      _maxNObj = _nObj;
   return OID(ci, tsi);
}
