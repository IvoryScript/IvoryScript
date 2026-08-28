/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    iFrameDescr.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory interpreted code stack frame descriptors.
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

#if (FRAME_DESCRIPTORS==1)

#ifndef IVORY_INTERP_FRAME_DESCR_H_DEFINED
#define IVORY_INTERP_FRAME_DESCR_H_DEFINED

#include "ivory/frameDescr.h"

#define N_FRAME_DESCR_TABLE_SLOTS   47 

// For future use on destruction of a segment

struct SegmentLabelPair {
   IAddress _segment;
   IAddress _iLabel;
};

class MSAFrameDescrMapAssoc {
public:
   MSAFrameDescrMapAssoc(MSAFrameDescrMapAssoc* next, MSA* msa)
      : _next(next),
        _msa(msa),
        _map(N_FRAME_DESCR_TABLE_SLOTS) {}

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}


   inline MSAFrameDescrMapAssoc* next(Void) { return _next; }

   inline FrameDescrTable<IAddress>& frameDescrMap(Void) { return _map; }

   static MSAFrameDescrMapAssoc* lookUp(MSA* msa);
   static MSAFrameDescrMapAssoc* add(MSA* msa);
   static Void remove(MSA* msa);

   static MSAFrameDescrMapAssoc* frameDescrMaps(Void) { return _frameDescrMaps; }

protected:
   MSAFrameDescrMapAssoc*     _next;
   MSA*                       _msa;
   FrameDescrTable<IAddress>  _map;

   static MSAFrameDescrMapAssoc* _frameDescrMaps;
};

#endif /* IVORY_INTERP_FRAME_DESCR_H_DEFINED */

#endif
