/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    repr.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Functions related to graph representation.
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

#include "ivory/repr.h"

// Built-in storage allocator block length

#define BUILT_IN_MSA_BLK_LEN 16384

static MSA* _builtInMSA = NULL;

MSA& builtInMSA() {
   if (_builtInMSA == NULL)
      _builtInMSA = new MSA(BUILT_IN_MSA_BLK_LEN);
   return *_builtInMSA;
}

// Identity copy expression method for use with built-in cells

Cell* copyCellId(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return &src;
}

// Common cell info method for simple cell destruction

Void destroyCell(Cell& cell_, Env& env, MSA& msa argN_VM) {
   msa.free(&cell_);
}

// Comparison operator

bool operator == (const Repr l, const Repr r) {
   if (l._tag == r._tag) {
      switch (l._tag) {
         case Repr::REPR_PTR:
/*
           if (l._ext._ptrRepr != NULL &&
               r._ext._ptrRepr != NULL &&
               l._ext._ptrRepr != r._ext._ptrRepr)
                 return FALSE;
*/

            break;

         case Repr::REPR_STRUCT:
            if (l._ext._structRepr->nReprs() != r._ext._structRepr->nReprs())
               return FALSE;
            for (UInt i = 0; i < l._ext._structRepr->nReprs(); i++) {
               Repr foo = l._ext._structRepr->reprV()[i];
               Repr bar = r._ext._structRepr->reprV()[i];

               if (foo != bar)
                  return FALSE;
            }
  
            break;

         case Repr::REPR_UNION:
            if (l._ext._unionRepr->nReprs() != r._ext._unionRepr->nReprs())
               return FALSE;
            for (UInt i = 0; i < l._ext._unionRepr->nReprs(); i++)
               if (l._ext._unionRepr->reprV()[i] != r._ext._unionRepr->reprV()[i])
                  return FALSE;
            break;

         case Repr::REPR_ARRAY:
            if (l._ext._arrayRepr->nElements()   != r._ext._arrayRepr->nElements() ||
                l._ext._arrayRepr->elementRepr() != r._ext._arrayRepr->elementRepr())
               return FALSE;
            break;

         default:
            break;
      }
      return TRUE;
   } else
      return FALSE;
}


#if (GARBAGE_COLLECTION==1)
Bool Repr::markableForGC(Void) const {
   switch (_tag) {
      case REPR_VOID:
      case REPR_CELL_INFO_PTR:
      case REPR_ENV_PTR:
      case REPR_LABEL:
      case REPR_TYPE:
      case REPR_NAME:
      case REPR_INT:
      case REPR_FLOAT:
      case REPR_DOUBLE:
      case REPR_REF:
      case REPR_CHAR:
      case REPR_TAG:
      case REPR_UTC:
      case REPR_DIR:
      case REPR_DIR_ENTRY:
      case REPR_SERIAL_CONTEXT:
      case REPR_LABEL_PAIR: 
      case REPR_FILE_HANDLE:
         return FALSE;

      case REPR_CELL_PTR:
      case REPR_EXPR:
      case REPR_PTR:
      case REPR_STRING:
         return TRUE;

      case REPR_STRUCT:
         return _ext._structRepr->markableForGC();

      case REPR_ARRAY:
         return _ext._arrayRepr->markableForGC();

      case REPR_UNION:
         return _ext._unionRepr->markableForGC();

      default:
         assert(FALSE, "Repr::markableForGC: unexpected tag");
         return FALSE;
   }
}
#endif

StructRepr::StructRepr(UInt nReprs, Repr* reprV,
   size_t* sizeV/* = NULL*/, size_t* offsetV/* = NULL*/) :
   _id(0),
   _nReprs(nReprs), _reprV(reprV),
   _size(0), _sizeV(sizeV), _offsetV(offsetV) {
   assignComponentSizesAndOffsets();
}
/*
static inline size_t alignUp(size_t x, size_t a) {
   return a > 1 ? (x + (a - 1)) & ~(a - 1) : x;
}

Void StructRepr::assignComponentSizesAndOffsets(Void) {
   size_t offset = 0;
   size_t maxCompSize = 0;
   size_t maxAlign = 1;

   for (UInt i = 0; i < _nReprs; ++i) {
      size_t compSize =    sizeOfRepr(_reprV[i]);
      size_t compAlign =   alignOfRepr(_reprV[i]);
      if (compAlign == 0) compAlign = 1;

      offset = alignUp(offset, compAlign);
      if (_sizeV != NULL)
         _sizeV[i] = compSize;
      if (_offsetV = NULL)
         _offsetV[i] = offset;

      if (compSize > maxCompSize)
         maxCompSize = compSize;
      if (compAlign > maxAlign)
         maxAlign = compAlign;
      offset += compSize;
   }

   _size = alignUp(offset, maxAlign);
   _maxCompSize = maxCompSize;
   _maxAlign = maxAlign;  // if stored

// Pad to largest component

   _size = maxCompSize > 0
      ? ((offset + (maxCompSize - 1)) / maxCompSize) * maxCompSize
      : offset;

   _maxCompSize = maxCompSize;
}
*/


Void StructRepr::assignComponentSizesAndOffsets(Void) {
   size_t offset = 0;
   size_t maxCompSize = 0;
   for (UInt i = 0; i < _nReprs; i++) {
      size_t compSize = sizeOfRepr(_reprV[i]);
      size_t alignSize = alignOfRepr(_reprV[i]);
      if (alignSize == 1) {
         //------ No alignment needed
      }
      else if (alignSize == 2)
         offset = ((offset + 1) / 2) * 2;    // Align to 2 byte boundary
      else if (alignSize == 4)
         offset = ((offset + 3) / 4) * 4;    // Align to 4 byte boundary
      else if (alignSize == 8)
         offset = ((offset + 7) / 8) * 8;    // Align to 8 byte boundary
      if (compSize > maxCompSize)
         maxCompSize = compSize;

      _sizeV[i] = compSize;
      _offsetV[i] = offset;
      offset += compSize;
   }

   // Pad to largest component

   _size = maxCompSize > 0
      ? ((offset + (maxCompSize - 1)) / maxCompSize) * maxCompSize
      : offset;

   _maxCompSize = maxCompSize;
   _maxAlign = _maxCompSize;
}

#if (GARBAGE_COLLECTION==1)
Bool StructRepr::markableForGC(Void) const {
   for (UInt i = 0; i < _nReprs; i++)
      if (_reprV[i].markableForGC())
         return TRUE;
   return FALSE;
}
#endif

Bool StructRepr::matches(UInt nReprs, const Repr* reprV) const {
   if (nReprs != _nReprs)
      return FALSE;
   for (UInt i = 0; i < _nReprs; i++)
      if (reprV[i] != _reprV[i])
         return FALSE;
   return TRUE;
}


#if (GARBAGE_COLLECTION==1)
Bool UnionRepr::markableForGC(Void) const {
   for (UInt i = 0; i < _nReprs; i++)
      if (_reprV[i].markableForGC())
         return TRUE;
   return FALSE;
}
#endif

Bool UnionRepr::matches(UInt nReprs, const Repr* reprV) const {
   if (nReprs != _nReprs)
      return FALSE;
   for (UInt i = 0; i < _nReprs; i++)
      if (reprV[i] != _reprV[i])
         return FALSE;
   return TRUE;
}

#if (GARBAGE_COLLECTION==1)
Bool ArrayRepr::markableForGC(Void) const {
   return _elementRepr.markableForGC();
}

#endif
