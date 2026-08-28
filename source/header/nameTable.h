/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    nameTable.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the NameTable class.
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

#ifndef NAME_TABLE_H_DEFINED
#define NAME_TABLE_H_DEFINED

#include "archive.h"
#include "MSA.h"
#include "../header/vector.h"

#ifndef NULL_NAME
#define NULL_NAME -1
#endif

typedef short int Name;

class NameTable {
public:
   class Entry;

public:
   NameTable(unsigned int nSlots, unsigned int nPerSeg, MSA& msa);
   virtual ~NameTable();
   void* operator new(size_t size, MSA& msa);
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr);
   void operator delete(void* ptr, MSA& msa);
#else
   void operator delete(void* ptr);
#endif

   inline unsigned int nSlots(void)    const { return _nSlots; }
   inline unsigned int nPerSeg(void)   const { return _nPerSeg; }
   inline unsigned int count(void)     const { return _count; }

   Name lookUp(const char* string) const;
   Entry* lookUp(const char* ident, unsigned int hashVal) const;
   Entry* lookUpByAlt(Name altName, unsigned int hashVal) const;
   Entry* addName(const char* string, unsigned int hashVal);
   void addName(Name name);
   void dropName(Name name);
   const char* string(Name name) const;
   unsigned int hashVal(Name name) const;

   Name altName(Name name) const;
   Void setAltName(Name name, Name altName);

   void load(Archive& archive);
   void store(Archive& archive) const;

   virtual Entry* lockName(Name name) const;

protected:
 
   const Entry* lookUp_(const char* string, unsigned int hashVal) const;
   
   virtual Entry* nextHashEntry(const NameTable::Entry* entry) const;
   virtual Entry* newEntry(void) const;
   virtual Entry* addEntry(Name name, UInt hashVal, char* string, UInt refCount);

   virtual void onLoadEntry(Entry* entry);

   virtual Name nextName();

   virtual Entry* lockHash(unsigned int index) const;

   virtual int adjustRefCount(Entry* entry, int diff);

public:
   class Entry {
   public:
      inline Name name(void) const { return _name; }
      inline unsigned int hashVal(void) const { return _hashVal; }

   public:
      Entry*         _next;
      Entry*         _prev;
      Name           _name;
      Name           _altName;
      char*          _string;
      unsigned int   _hashVal;
      int            _refCount;

      friend class NameTable;
   };

static
   UInt hash(const char* string);

protected:
   unsigned int   _nSlots;
   unsigned int   _nPerSeg;
   MSA&           _msa;
   Entry**        _hashTab;
   vector<Entry*> _vec;
   unsigned int   _count;
   Name           _minFreeName;
   Name           _maxName;
};

#endif /* NAME_TABLE_H_DEFINED */
