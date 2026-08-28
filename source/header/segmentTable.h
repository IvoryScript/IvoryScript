/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    segmentTable.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the SegmentTable class.
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

#ifndef SEGMENT_TABLE_H_DEFINED
#define SEGMENT_TABLE_H_DEFINED

#include "archive.h"
#include "MSA.h"
#include "../header/vector.h"

#define NULL_SEGMENT_ID -1

typedef short int SegmentId;

class SegmentTable {
public:
   class SegmentEntry;

public:
   SegmentTable(unsigned int nSlots, unsigned int nPerSeg, MSA& msa);
   virtual ~SegmentTable();
   void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr);
   void operator delete(void* ptr, MSA& msa);
#else
   void operator delete(void* ptr);
#endif

   inline unsigned int nSlots(void)    const { return _nSlots; }
   inline unsigned int nPerSeg(void)   const { return _nPerSeg; }
   inline MSA& msa(void) const { return _msa; }

   SegmentId lookUp(const Byte* segment, size_t len);
   SegmentId addSegment(const Byte* segment, size_t len, unsigned int hashVal);
   void addSegment(SegmentId segmentId);

   void dropSegment(SegmentId segmentId);
   const Byte* segment(SegmentId segmentId) const;
   SegmentId segmentId(const Byte* segment, unsigned int hashVal);

   unsigned int hashVal(SegmentId segmentId) const;
 
   void load(Archive& archive);
   void store(Archive& archive) const;

protected:
   SegmentEntry* lookUp(const Byte* segment, size_t len,
                        unsigned int hashVal);

   virtual int match(const Byte* segment1, size_t len1,
                     const Byte* segment2, size_t len2) const;
   virtual SegmentEntry* nextHashEntry(SegmentTable::SegmentEntry* entry);
   virtual SegmentEntry* newEntry(void) const;
   virtual SegmentEntry* addEntry(SegmentId id, UInt hashVal,
                                  const Byte* segment, size_t len,
                                  UInt refCount);

   virtual SegmentId nextSegment();

   virtual SegmentEntry* lockSegment(SegmentId segmentId) const;
   virtual SegmentEntry* lockHash(unsigned int index);

   virtual int adjustRefCount(SegmentEntry* entry, int diff);

public:
   class SegmentEntry {
   public:
      SegmentEntry*  _next;
      SegmentEntry*  _prev;
      SegmentId      _id;
      size_t         _len;
      const Byte*    _segment;
      UInt32         _hashVal;
      UInt16         _refCount;

      friend class SegmentTable;
   };

static
   UInt hash(const Byte* segment, size_t len);

protected:
   unsigned int            _nSlots;
   unsigned int            _nPerSeg;
   MSA&                    _msa;
   SegmentEntry**          _hashTab;
   vector<SegmentEntry*>   _vec;
   unsigned int            _count;
   SegmentId               _minFreeId;
   SegmentId               _maxId;
};

#endif /* SEGMENT_TABLE_H_DEFINED */
