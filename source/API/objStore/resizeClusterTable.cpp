/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    resizeClusterTable.cpp
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
 *    Resize cluster table
 *
 * Call format:
 *
 *    oid = stackAlloc(size);
 *
 *       size -   (size_t)       - Object size
 *
 *    ~ObjStore();
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
#include <memory.h>

int ObjStore::resizeClusterTable(UINT nCurr, UINT nReq) {
   UINT  i;
   Cluster* clustTbl = NULL;  
      
   if (nReq > nCurr) {
 
//--- Obtain new cluster table

      if ((clustTbl = (Cluster*)_msa.alloc(nReq * sizeof(Cluster))) == NULL)
         return -1;
      
//--- Copy existing clusters (assuming no overloaded copy constructor)

      memcpy(clustTbl, _clustTbl, nCurr * sizeof(Cluster));

//--- Allocate new object table segments and clear

      for (i = nCurr; i < nReq; i++) {
         if ((clustTbl[i]._csc =
            new (_msa) CSC(_objsPerSeg, _blkLen)) == NULL)
            return -1;

         if ((clustTbl[i]._tblSeg =
             (void**)_msa.alloc(sizeof(void*) * _objsPerSeg)) == NULL)
	         return -1;
         memset(clustTbl[i]._tblSeg, 0, sizeof(void*) * _objsPerSeg);
      }
   } else if (nReq < nCurr ) {
      
//--- Free up object table segments no longer required

      for (i = nCurr - 1;; i-- ) {
         if (_msa.free(_clustTbl[i]._tblSeg ) < 0 )
            return -1;
         if (i <= nReq)
            break;
      }
            
//--- Obtain new cluster table

      if ((clustTbl = (Cluster*)_msa.alloc(nReq * sizeof(Cluster))) == NULL)
         return -1;
      
//--- Copy existing clusters (assuming no overloaded copy constructor)

      memcpy(clustTbl, _clustTbl, nReq * sizeof(Cluster));
   }

// Free up existing cluster table (if present)

   if (_msa.free(_clustTbl) < 0)
      return -1;

   _clustTbl = clustTbl;

   return 0;
}
