/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    vector.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the vector template Class
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

#ifndef VECTOR_H_DEFINED
#define VECTOR_H_DEFINED

#include "segArray.h"

template <class T>
class vector {
public:
   vector(int blkSize)
      : _nPerSeg(blkSize), _upb(0) {
   };
   void preDestruct(MSA& msa) {
      _segArr.resize(_upb / _nPerSeg, 0, _nPerSeg, false, msa);
   };
   T& get(unsigned int i) {
      return _segArr.lv(i / _nPerSeg, i % _nPerSeg);
   }
   const T& get(unsigned int i) const {
      return _segArr.lv(i / _nPerSeg, i % _nPerSeg);
   }
   void put(unsigned int i, const T& val, MSA& msa) {

//--- Check within bounds

      if (i >= _upb) {

//------ Calculate sizes

         UINT nCurr = _upb / _nPerSeg;
         UINT nReq = (i + _nPerSeg) / _nPerSeg;

//------ Resize segmented array

         if (_segArr.resize(nCurr, nReq, _nPerSeg, false, msa) < 0) {

//--------- T.B.D raise exception

         }

//------ Save new upperbound index

         _upb = nReq * _nPerSeg;
      }

      _segArr.lv(i / _nPerSeg, i % _nPerSeg) = val;
   };

protected:
   int            _nPerSeg;
   unsigned int   _upb;
   segArray<T>    _segArr;
};

#endif
