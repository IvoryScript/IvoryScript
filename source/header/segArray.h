/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    segArray.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 3 July 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of segmented array template class
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

#ifndef SEG_ARRAY_H_DEFINED
#define SEG_ARRAY_H_DEFINED

#include <memory.h>
#include "general.h"
#include "MSA.h"

template <class T>
class segArray {
public:
   segArray() : _segTbl(NULL) {
   }

   int resize(UINT nCurr, UINT nReq,
              UINT nPerSeg, bool initZero,
              MSA& msa) {
      UINT  i;
      T**   segTbl = NULL;   
      
      if (nReq > nCurr) {
 
//------ Obtain new segment table

         if ((segTbl = (T**)msa.alloc(nReq * sizeof(T*))) == NULL)
            return -1;
      
//------ Copy existing segment table pointers

         memcpy(segTbl, _segTbl, nCurr * sizeof(T*));

//------ Allocate new segments and clear if requested

         for (i = nCurr; i < nReq; i++) {
            if ((segTbl[i] = (T*)msa.alloc(sizeof(T) * nPerSeg)) == NULL)
	            return -1;
            if (initZero)
               memset(segTbl[i], 0, sizeof(T) * nPerSeg);
         }
      } else if (nReq < nCurr){
      
//------ Free up segments no longer required

         for (i = nCurr - 1;; i--) {
            if (msa.free(_segTbl[i] ) < 0 )
               return -1;
            if (i <= nReq)
               break;
         }
            
//------ Obtain new segment table

         if (nReq > 0 && (segTbl = (T**)msa.alloc(nReq * sizeof(T*))) == NULL)
            return -1;
      
//------ Copy existing segment table pointers

         memcpy(segTbl, _segTbl, nReq * sizeof(T*));
      }

//--- Free up existing indirection table (if present)

      if (msa.free(_segTbl) < 0)
         return -1;

      _segTbl = segTbl;

      return 0;
   };

   T& lv(unsigned int seg, unsigned int offset) {
      return _segTbl[seg][offset];
   };
   const T& lv(unsigned int seg, unsigned int offset) const {
      return _segTbl[seg][offset];
   };

protected:
   T** _segTbl;
};

#endif /* SEG_ARRAY_H_DEFINED */
