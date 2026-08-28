/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    objStore.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the ObjStore class.
 *
 * Notes:
 *
 *    The OID data type is designed to represent an obect identifier either as
 *    a direct pointer or indirectly via a table.  The bottom bit indicates
 *    the form (1 = indirect) and the next bit is reserved for use by
 *    object store applications.
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

#ifndef OBJSTORE_H_DEFINED
#define OBJSTORE_H_DEFINED

// Set PER_CLUSER_MSA to 1 for a separate MSA per cluster, 0 otherwise

#define PER_CLUSTER_MSA 0

#include "general.h"
#include "MSA.h"

// An object table segment is a segment of the object table

typedef  void* TblSeg;

// OID is formed from cluster table index:object table segment index

#define TSI_FLD_POS  2
#define TSI_FLD_LEN  14
#define TSI_FLD_MASK (((1<<TSI_FLD_LEN)-1)<<TSI_FLD_POS)
#define CI_FLD_POS   16
#define CI_FLD_LEN   16
#define CI_FLD_MASK  (((1<<CI_FLD_LEN)-1)<<CI_FLD_POS)

#define OID_IND_TAG  1
                     
#define NULL_OID 0

class OID {
public:
   inline OID() : _repr(NULL_OID) {}
   inline OID(UInt16 ci, UInt16 tsi)
      : _repr(ci << CI_FLD_POS | tsi << TSI_FLD_POS | OID_IND_TAG) {
   }
   inline OID(UInt32 raw) : _repr(raw) {}
   inline UInt16 ci() {
      return (UInt16)((_repr & CI_FLD_MASK) >> CI_FLD_POS);
   }
   inline UInt16 tsi() {
      return (UInt16)((_repr & TSI_FLD_MASK) >> TSI_FLD_POS);
   }
   inline UInt32 repr() {
      return _repr;
   }
   inline UInt32 repr() const {
      return _repr;
   }

protected:
   UInt32 _repr;
   
   friend inline bool operator == (const OID l, const OID r) {
      return l._repr == r._repr;
   }
   friend inline bool operator != (const OID l, const OID r) {
      return l._repr != r._repr;
   }

   friend inline ostream& operator << (ostream& os, const OID oid) {
      os << oid._repr;
      return os;
   };
};

class ObjStore {
public:
   ObjStore(unsigned int objsPerSeg, size_t blkLen, MSA& msa);
   ~ObjStore();
   void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr, MSA& msa);
#else
   void operator delete(void* ptr);
#endif

   OID alloc(size_t size);
   void* allocOID(OID oid, size_t size);
   void free(OID oid);

   virtual void* lock(OID oid) {
      return (int)oid.repr() != NULL_OID
         ? _clustTbl[oid.ci()]._tblSeg[oid.tsi()]
         : NULL;
   }
   virtual Void unlock(OID oid) {
   }

   inline MSA* queryMSA(OID oid) {
      #if (PER_CLUSTER_MSA==1)
         return &_clustTbl[(int)oid.ci()]._csc->_msa;
      #else
         return &_msa;
      #endif
   }

   inline unsigned int getObjsPerSeg(void) const {
      return _objsPerSeg;
   }
   inline void setObjsPerSeg(unsigned int objsPerSeg) {
      _objsPerSeg = objsPerSeg;
   }
   inline unsigned int nObjects(void) {
      return _nObj;
   }

   class Iterator {
   public:
      Iterator(ObjStore& objStore);
      bool endOfSequence();
      OID  next();

   protected:
      ObjStore&            _objStore;
      unsigned short int   _ci;
      unsigned short int   _tsi;
      unsigned int         _count;
   };

protected:
   int resizeClusterTable(UINT nCurr, UINT nReq);

// A Cluster Storage Controller handles the storage aspects of a cluster

   class CSC {
      CSC(unsigned int objsPerSeg, size_t blkLen)
         : _nFreeObj(objsPerSeg), _minFreeIndex(0)
#if (PER_CLUSTER_MSA==1)
         , _msa(blkLen)
#endif
      {
      }
      inline void* operator new(size_t size, MSA& msa) {
         return msa.alloc(size);
      }
      inline
#ifdef DELETE_NEEDS_NEW_ARG
      void operator delete(void *ptr, MSA& msa)
#else
      void operator delete(void *ptr, size_t size)
#endif
      {
      }

   protected:
      unsigned int   _nFreeObj;        // No. of free objects
      int            _minFreeIndex;    // Lowest free 
#if (PER_CLUSTER_MSA==1)
      MSA            _msa;             // Memory storage allocator
#endif

      friend class ObjStore;
   };

// A cluster is a pool of objects

   class Cluster {
   protected:
      CSC*     _csc;                   // Cluster Storage Controller
      TblSeg*  _tblSeg;                // Object table segment

      friend class ObjStore;
      friend class Iterator;
   };

protected:
   unsigned int   _objsPerSeg;         // No. of objects per table segment
   size_t         _blkLen;             // Cluster MSA block size in bytes
   MSA&           _msa;                // Storage allocator
   Cluster*       _clustTbl;           // Cluster table
   unsigned int   _nClusters;          // No. of clusters
   int            _minFreeIndex;       // Lowest cluster index with free entries
   unsigned int   _nObj;               // No. of objects in use
   unsigned int   _maxNObj;            // Maximum no. of objects used

   friend class Iterator;
};

#endif /* OBJSTORE_H_DEFINED */
