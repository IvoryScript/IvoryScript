/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    MSA.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Memory Storage Allocator Class
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

#ifndef MSA_H_DEFINED
#define MSA_H_DEFINED

#ifndef MINIMAL_IOSTREAM
   #include <ostream>
   using namespace std;
#endif

#include "general.h"

// Set memory allocation increment (bytes)

#define MSA_QUANT 8

// Flag to clear segments on allocation

#define MSA_CLR_FLG 0

// Segment descriptor flags

#define MSA_ALLOC_FLAG  1

// Maximum length of segment (arbitrary)

#define MSA_MAX_SEG_LEN 1048576

// Maximum length of block (arbitrary)

#define MSA_MAX_BLK_LEN 1048576

class MSA {
public:
   class SegDescr;
protected:
   class SegBlk;

public:
   MSA(size_t blkLen);
   virtual ~MSA();

   inline Void* operator new(size_t size) {
      Void *ptr = ::malloc(size);
      return ptr;
   }
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
   inline Void operator delete(Void* ptr) {
      ::free(ptr);
   }

   inline size_t queryLimit() const { return _limit; }
   inline void setLimit(size_t limit) { _limit  = limit; }

   inline size_t queryBlkLen() const { return _blkLen; }

   inline size_t allocLen(size_t reqLen) {
      return MSA_QUANT * ((reqLen + sizeof(SegDescr) + MSA_QUANT - 1)
         / MSA_QUANT);
   }

	void* alloc(size_t len);
   int free(void* seg);

   int check(void) const;

   void info(ostream& os, unsigned int indent) const;

protected:
   virtual int addBlk(size_t rqLen);
   SegBlk* queryBlk(SegDescr *descr);

   public:

public:

// Memory buffer segment descriptor
   class SegDescr {
   protected:
      SegDescr();

      size_t _len;				            // Segment length and flags

      friend class MSA;
      friend class MSA_GC;
      friend class SegBlk;
   };

protected:

// Memory buffer segment (descriptor and data)
   class DataSeg : protected SegDescr {
      protected:

      DataSeg();

      unsigned char     _data[MSA_MAX_SEG_LEN];
													// Data area
      friend class MSA;
      friend class MSA_GC;
   };

// Free Memory segment (descriptor and link to next)
	class FreeSeg : protected SegDescr {
      protected:
      
      FreeSeg*   _next;          // Link to next

      friend class MSA;
      friend class MSA_GC;
      friend class SegBlk;
	};

// Segment block
   class SegBlk {
      SegDescr* fstFit(size_t reqLen);
      int check(void) const;

		size_t         _len;           // Block length in bytes
      size_t         _maxFreeSegLen; // Largest free seg
      UINT           _nAllocSeg;     // No. of segments in use
      UINT           _nFreeSeg;      // No. of free segments
      SegBlk*        _prev;          // Previous block
      SegBlk*        _next;          // Next block
      FreeSeg*       _hdFreeSeg;     // Ptr to first free seg
      unsigned char  _data[MSA_MAX_BLK_LEN];
                                     // Data area

	   friend class MSA;
      friend class MSA_GC;
  	   friend class SegDescr;
	};

   size_t            _blkLen;       // Default block length
   unsigned int      _nBlk;         // No. blocks
public:
   UINT              _nSeg;         // Number of segments
   UINT              _limit;        // Allocation limit (bytes), 0 = unlimited
   UINT              _totUse;       // Total bytes allocated
   UINT              _maxUse;       // Maximum no. bytes allocated
   UINT              _currUse;      // Current no. bytes allocated

protected:
   SegBlk*           _hdBlk;        // First block
   SegBlk*           _tlBlk;        // Last block
   SegBlk*           _currBlk;      // Current block
   int               _errCode;      // Error code
};

#endif /* MSA_H_DEFINED */
