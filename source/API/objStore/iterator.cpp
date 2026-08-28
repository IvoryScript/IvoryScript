/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    iterator.cpp
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
 * Call format:
 *
 *    ptr = iterator(oid);
 *
 *       ptr -    (void*)     - Returned pointer to object data
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

ObjStore::Iterator::Iterator(ObjStore& objStore)
   : _objStore(objStore), _ci(0), _tsi(0), _count(0) {
}

bool ObjStore::Iterator::endOfSequence() {
   return _count >= _objStore._nObj;
}

OID ObjStore::Iterator::next() {
   while (_ci < _objStore._nClusters) {
      void** tblSeg = _objStore._clustTbl[_ci]._tblSeg;
      while (_tsi < _objStore._objsPerSeg) {
         if (tblSeg[_tsi++] != NULL) {
            _count++;
            return OID(_ci, _tsi - 1);
         }
      }
      _ci++;
      _tsi = 0;
   }
   return NULL_OID;
}
