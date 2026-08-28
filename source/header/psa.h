/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    PSA.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Persistent Segment Allocator class.
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

#ifndef PSA_H_DEFINED
#define PSA_H_DEFINED

#include <memory.h>
#include <string.h>
#include <time.h>
#include "archive.h"
#include "general.h"
#include "MSA.h"

#define WRITE_FLAG   1

#define PSA_MAGIC_NO "d57b29d89485e0c3553e516ccbf9107a"

// Error codes

#define PSA_ERROR_STORE             -1
#define PSA_ERROR_PID               -2
#define PSA_ERROR_OVERRUN           -3
#define PSA_ERROR_CORRUPT           -4

#define PID_SIZE                    4
#define PERSIST_UINT16_SIZE         2
#define PERSIST_UINT32_SIZE         4

#define PAGE_INDEX_FIELD_POS        0
#define PAGE_INDEX_FIELD_LEN        8
#define PAGE_INDEX_FIELD_MASK       (((1<<PAGE_INDEX_FIELD_LEN)-1)<<PAGE_INDEX_FIELD_POS)
#define PAGE_NUM_FIELD_POS          8
#define PAGE_NUM_FIELD_LEN          24
#define PAGE_NUM_FIELD_MASK         (((1<<PAGE_NUM_FIELD_LEN)-1)<<PAGE_NUM_FIELD_POS)

#define PAGE_NUM_SIZE               3

#define NULL_PAGE_NUM               0xfffff
#define NULL_PID                    ((PID)0)

#define PAGE_HEADER_SIZE            2
#define PAGE_NUM_SEG_OFFSET         0
#define PAGE_SEG_DIR_OFFSET         2
#define PAGE_SEG_DIR_ENTRY_SIZE     2

#define PSA_PAGE_FLAGS_MASK         0x000f
#define PSA_PAGE_FLAGS_POS          12
#define PSA_N_SEGMENTS_MASK         0x0fff
#define PSA_N_SEGMENTS_POS          0

#define PSA_RESERVE_1_FLAG          0x0001        
#define PSA_FREE_ENTRY_FLAG         0x0002

#define PSA_SEG_FLAGS_MASK          0x0003
#define PSA_SEG_FLAGS_POS           14
#define PSA_SEG_OFFSET_MASK         0x3fff        
#define PSA_SEG_OFFSET_POS          0

#define PSA_FREE_SEGMENT            0
#define PSA_SHORT_SEGMENT           1
#define PSA_LONG_SEGMENT            2
#define PSA_OVERFLOW_SEGMENT        3

#define PSA_IND_ENTRY_LEN_OFFSET    0
#define PSA_IND_ENTRY_PID_OFFSET    PERSIST_UINT16_SIZE
#define PSA_IND_TABLE_ENTRY_SIZE    (PSA_IND_ENTRY_PID_OFFSET+PID_SIZE)

#define PSA_HDR_SEG_PAGE_NUM_BITS_OFFSET     0
#define PSA_HDR_SEG_PAGE_INDEX_BITS_OFFSET  (PSA_HDR_SEG_PAGE_NUM_BITS_OFFSET+1)
#define PSA_HDR_SEG_PAGE_FREE_LEN_OFFSET    (PSA_HDR_SEG_PAGE_INDEX_BITS_OFFSET+1)
#define PSA_HDR_SEG_PAGE_FREE_PID_OFFSET    (PSA_HDR_SEG_PAGE_FREE_LEN_OFFSET+2)
#define PSA_HDR_SEG_PAGE_KEY_PID_OFFSET     (PSA_HDR_SEG_PAGE_FREE_PID_OFFSET+4)
#define PSA_HDR_SEG_PAGE_SPARE_OFFSET       (PSA_HDR_SEG_PAGE_KEY_PID_OFFSET+4)
#define PSA_HDR_SEG_MAGIC_NO_OFFSET         (PSA_HDR_SEG_PAGE_SPARE_OFFSET+4)
#define PSA_HDR_SEG_LEN                     (PSA_HDR_SEG_MAGIC_NO_OFFSET+16)

#define PSA_FREE_TABLE_ENTRY_SIZE           (2+2*PAGE_NUM_SIZE)

#define PSA_MIN_PAGE_SIZE  (PAGE_HEADER_SIZE+PAGE_SEG_DIR_ENTRY_SIZE+PSA_HDR_SEG_LEN)
#define PSA_MAX_PAGE_SIZE  (PSA_SEG_OFFSET_MASK+1)

#define PSA_HDR_PAGE_NUM            0

#define PSA_EMPTY_FREE_SLOT_LIMIT   8

#define P_ARCHIVE_BUFFER_SIZE       256

typedef unsigned short int UInt16;

typedef long int PageNum;

class PArchive;

// Persistent segment identifier

class PID {
public:
   inline PID()
      : _repr(0) {
   }

   inline PID(PageNum pageNum, UINT index)
      : _repr(pageNum << PAGE_NUM_FIELD_POS |
              index << PAGE_INDEX_FIELD_POS) {
   }
   inline PID(UInt32 raw)
      : _repr(raw) {
   }

   inline PageNum pageNum() {
      return ((_repr & PAGE_NUM_FIELD_MASK) >> PAGE_NUM_FIELD_POS);
   }
   inline UInt16 pageIndex() {
      return (UInt16)((_repr & PAGE_INDEX_FIELD_MASK) >> PAGE_INDEX_FIELD_POS);
   }
   inline UInt32 repr() {
      return _repr;
   }

protected:
   UInt32 _repr;  

   friend inline bool operator == (const PID l, const PID r) {
      return l._repr == r._repr;
   }
   friend inline bool operator != (const PID l, const PID r) {
      return l._repr != r._repr;
   }
   friend class PArchive;
};

class PSA {
public:
   PSA(size_t blkLen, UInt bufferLimit);
   ~PSA();
   int free(PID pid);

   PID alloc(size_t len, const void* ptr = NULL);
   int read(PID pid, size_t pos, size_t len, void* ptr);
   int write(PID pid, size_t pos, size_t len, const void* ptr);

   int insert(PID pid, size_t pos, size_t len, void* ptr);
   int remove(PID pid, size_t pos, size_t len);

   int length(PID pid);

   int open(void);
   int create(UInt16 pageSize);
   int close(void);

   virtual PageNum allocPage(void) = 0;
   virtual int readPage(PageNum pageNum, size_t len, void* ptr) = 0;
   virtual int writePage(PageNum pageNum, size_t len, void* ptr) = 0;

protected:
   class BufferManager;

public:
   class PageBuffer {
      inline UInt16 getUInt16(UInt16 offset) {
         return (UInt16)(_data[offset]) |
                (UInt16)(_data[offset + 1]) << 8;
      }
      inline void putUInt16(UInt16 val, UInt16 offset) {
         _data[offset]     =  val       & 0xff;
         _data[offset + 1] = (val >> 8) & 0xff;
      }
      inline UInt32 getUInt32(UInt16 offset) {
         return (UInt32)(_data[offset]) |
                (UInt32)(_data[offset + 1]) << 8 |
                (UInt32)(_data[offset + 2]) << 16 |
                (UInt32)(_data[offset + 3]) << 24;
      }
      inline void putUInt32(UInt32 val, UInt16 offset) {
         _data[offset]     =  val        & 0xff;
         _data[offset + 1] = (val >> 8)  & 0xff;
         _data[offset + 2] = (val >> 16) & 0xff;
         _data[offset + 3] = (val >> 24) & 0xff;
      }
      inline PID getPID(UInt16 offset) {
         return (PID)getUInt32(offset);
      }
      inline void putPID(PID pid, UInt16 offset) {
         putUInt32(pid.repr(), offset);
      }
      inline PageNum getPageNum(UInt16 offset) {
         return (PageNum)(_data[offset]) |
                (PageNum)(_data[offset + 1]) << 8 |
                (PageNum)(_data[offset + 2]) << 16;
      }
      inline void putPageNum(UInt16 offset, PageNum pageNum) {
         _data[offset] = pageNum & 0xff;
         _data[offset + 1] = (pageNum >> 8) & 0xff;
         _data[offset + 2] = (pageNum >> 16) & 0xff;
      }
      inline void * data(UInt16 offset) {
         return _data + offset;
      }
      inline void clear(UInt16 offset, size_t len) {
         memset(_data + offset, 0, len);
      }
      inline void copy(UInt16 offset, size_t len, void* ptr) {
         memcpy(ptr, _data + offset, len);
      }
      inline void copy(size_t len, const void* ptr, UInt16 offset) {
         memcpy(_data + offset, ptr, len);
      }
      inline void move(UInt16 src, UInt16 dst, size_t len) {
         memmove(_data + dst, _data + src, len);
      }
	   inline int getNSegments(void) {
         return getUInt16(PAGE_NUM_SEG_OFFSET) & PSA_N_SEGMENTS_MASK;
      }
	   inline int setNSegments(int n) {
         return getUInt16(PAGE_NUM_SEG_OFFSET) & PSA_N_SEGMENTS_MASK;
      }
      inline UInt16 getEntry(UInt16 index) {
         return getUInt16(PAGE_SEG_DIR_OFFSET +
                          PAGE_SEG_DIR_ENTRY_SIZE * index);
      }
      inline void setEntry(UInt16 index, UInt16 flags, UInt16 offset) {
         putUInt16(flags << PSA_SEG_FLAGS_POS |
                   (offset != PSA_MAX_PAGE_SIZE ? offset : 0) << PSA_SEG_OFFSET_POS,
                   PAGE_SEG_DIR_OFFSET + PAGE_SEG_DIR_ENTRY_SIZE * index);
      }
      UInt16 segLength(UInt16 index) {
         return (index == 0 ? _pageSize
                            : offset(getEntry(index - 1))) - offset(getEntry(index));
      }
      inline PageNum getPrev(void) {
         return getPageNum(PAGE_HEADER_SIZE + _nSegments * PAGE_SEG_DIR_ENTRY_SIZE +
                           (_reserve > 1 ? PAGE_SEG_DIR_ENTRY_SIZE : 0)); 
      }
      inline void putPrev(PageNum prev) {
         putPageNum(PAGE_HEADER_SIZE + _nSegments * PAGE_SEG_DIR_ENTRY_SIZE +
                    (_reserve > 1 ? PAGE_SEG_DIR_ENTRY_SIZE : 0), prev); 
      }
      inline PageNum getNext(void) {
         return getPageNum(PAGE_HEADER_SIZE + _nSegments * PAGE_SEG_DIR_ENTRY_SIZE +
                           (_reserve > 1 ? PAGE_SEG_DIR_ENTRY_SIZE : 0) + 
                           PAGE_NUM_SIZE);
      }
      inline void putNext(PageNum next) {
         putPageNum(PAGE_HEADER_SIZE + _nSegments * PAGE_SEG_DIR_ENTRY_SIZE +
                    (_reserve > 1 ? PAGE_SEG_DIR_ENTRY_SIZE : 0) +
                    PAGE_NUM_SIZE, next);
      }

      void initAlloc(UInt16 len, UInt16 reserve, UInt16& index, UInt16& offset);
      void allocEntry(UInt16 len, UInt16 reserve, UInt16& index, UInt16& offset);
      void resizeEntry(UInt16 index, UInt16 fromLen, UInt16 toLen,
                       UInt16 pos, UInt16& offset);
      void freeEntry(UInt16 index, UInt16 offset, UInt16 len);
      void copy3(UInt16 len1, const void* ptr1,
                 UInt16 len2, const void* ptr2,
                 UInt16 len3, const void* ptr3,
                 UInt16 offset);

      size_t freeSpace(void);
      PID alloc(size_t len);
      void postLoad(void);
      void preStore(void);

      PageBuffer*       _next;
      UInt16            _pageSize;
      PageNum           _pageNum;
      UInt              _seqNo;
      UInt              _lockCount;
      unsigned char*    _data;
      UInt16            _nSegments;
      UInt16            _reserve;
      bool              _isFreeSeg;
      bool              _writePending;

      static inline UInt16 offset(UInt16 entry) {
         UInt16 res = entry >> PSA_SEG_OFFSET_POS & PSA_SEG_OFFSET_MASK;
         return res != 0 ? res : PSA_MAX_PAGE_SIZE;
      }
      static inline UInt16 flags(UInt16 entry) {
         return entry >> PSA_SEG_FLAGS_POS & PSA_SEG_FLAGS_MASK;
      }
      static inline size_t reserve(size_t len) {
         return len >= sizeof(PID) ? 0 : sizeof(PID) - len;
      }

      friend class PSA;
      friend class BufferManager;
      friend class PArchive;
   };

   protected:   
   class BufferManager {
      BufferManager(UInt limit);
      ~BufferManager(void);

      PageBuffer* getPage(PageNum pageNum, bool writeFlag);
      PageBuffer* allocPageBuffer(PageNum pageNum,
                                  bool clearFlag, bool writeFlag);
      int writePage(PageBuffer* pageBuf);
      void flush(void);

      PageBuffer*    _pageBufferPool;
      UInt           _count;
      UInt           _limit;
      UInt           _seqNo;
      PSA*           _psa;

      friend class PSA;
   };

   class FreeTableEntry {
      size_t   _nFree;
      PageNum  _head;
      PageNum  _tail;

      friend class PSA;
   };

   PID allocSegment(UInt16 flags, UInt16 len, const void* ptr,
                    PageBuffer*& pugeBuffer, UINT16& index, UInt16& offset);
   int freeSegment(PID pid);
   PageBuffer* fetch(PID pid,
                     UInt16& index, UInt16&offset, UInt16&flags);
   Int16 lookUpFree(size_t nFree, bool addFlag);
   void compressFree(void);
   void unlinkFree(PageBuffer* pageBuf);
   void linkFree(PageBuffer* pageBuf);

   inline UInt16 maxSegmentLength(void) {
      return _pageSize - PAGE_HEADER_SIZE - PAGE_SEG_DIR_ENTRY_SIZE;
   }

public:

   static inline void packUInt16(UInt16 val, unsigned char* ptr) {
      ptr[0] = val      & 0xff;
      ptr[1] = val >> 8 & 0xff;
   }
   static inline UInt16 unpackUInt16(unsigned char* ptr) {
      return (UInt16)(ptr[0]) |
             (UInt16)(ptr[1]) << 8;
   }
   static inline void packPID(PID pid, unsigned char* ptr) {
      ptr[0] = pid.repr()       & 0xff;
      ptr[1] = pid.repr() >>  8 & 0xff;
      ptr[2] = pid.repr() >> 16 & 0xff;
      ptr[3] = pid.repr() >> 24 & 0xff;
   }
   static inline PID unpackPID(unsigned char* ptr) {
      return (UInt32)(ptr[0]) |
             (UInt32)(ptr[1]) << 8 |
             (UInt32)(ptr[2]) << 16 |
             (UInt32)(ptr[3]) << 24;
   }
   static inline void packPageNum(PageNum pageNum, unsigned char* ptr) {
      ptr[0] = pageNum       & 0xff;
      ptr[1] = pageNum >>  8 & 0xff;
      ptr[2] = pageNum >> 16 & 0xff;
   }
   static inline PageNum unpackPageNum(unsigned char* ptr) {
      return (UInt32)(ptr[0]) |
             (UInt32)(ptr[1]) << 8 |
             (UInt32)(ptr[2]) << 16;
   }

protected:
   MSA               _msa;          // Memory storage allocator
   BufferManager     _bufMan;       // Buffer manager
   UInt16            _pageSize;
   UInt16            _maxSegmentLength;
   UInt16            _lenFreeTable;
   UInt16            _nFreeTable;
   FreeTableEntry*   _freeTable;
   PID               _freePID;

   friend class BufferManager;
   friend class PArchive;
};

class PArchive : public Archive {
public:
   PArchive(PSA& psa, PID pid, UInt16 lim = P_ARCHIVE_BUFFER_SIZE);
   PArchive(PSA& psa);
   ~PArchive(void);

   PID alloc(void);

   virtual unsigned char get(void);
   virtual void get(unsigned char&);
   virtual void put(unsigned char);

   virtual void seek(size_t pos);

	inline PArchive& operator << (const char c) {
      return (PArchive&)((Archive&)*this << c);
   }   
	inline PArchive& operator >> (char& c) {
      return (PArchive&)((Archive&)*this >> c);
   }

	inline PArchive& operator << (const UInt8 u) {
      return (PArchive&)((Archive&)*this << u);
   }
	inline PArchive& operator >> (UInt8& u) {
      return (PArchive&)((Archive&)*this >> u);
   }

	inline PArchive& operator << (const UInt16 u) {
      return (PArchive&)((Archive&)*this << u);
   }
	inline PArchive& operator >> (UInt16& u) {
      return (PArchive&)((Archive&)*this >> u);
   }

	inline PArchive& operator << (const UInt32 u) {
      return (PArchive&)((Archive&)*this << u);
   }
	inline PArchive& operator >> (UInt32& u) {
      return (PArchive&)((Archive&)*this >> u);
   }

	PArchive& operator << (const PID pid);
	PArchive& operator >> (PID& i);

protected:
   PSA&              _psa;
   PID               _pid;
   UInt16            _pos;
   UInt16            _lim;
   UInt16            _offset;
   UInt16            _readCount;
   bool              _writeFlag;
   UInt16            _writeLWM;
   UInt16            _writeHWM;

   unsigned char  _buffer[P_ARCHIVE_BUFFER_SIZE];
};

#endif /* PSA_H_DEFINED */
