/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    persistentNameTable.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the PersistentNameTable class.
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

#ifndef PERSISTENT_NAME_TABLE_H_DEFINED
#define PERSISTENT_NAME_TABLE_H_DEFINED

#include "nameTable.h"
#include "PSA.h"

#define CONTROL_REC_N_SLOTS_OFFSET  0
#define CONTROL_REC_HASH_PID_OFFSET (CONTROL_REC_N_SLOTS_OFFSET+PID_SIZE)
#define CONTROL_REC_VEC_PID_OFFSET  (CONTROL_REC_HASH_PID_OFFSET+PID_SIZE)
#define CONTROL_REC_COUNT_OFFSET    (CONTROL_REC_VEC_PID_OFFSET+PID_SIZE)
#define CONTROL_REC_MIN_FREE_OFFSET (CONTROL_REC_COUNT_OFFSET+PERSIST_NAME_SIZE)
#define CONTROL_REC_MAX_NAME_OFFSET (CONTROL_REC_MIN_FREE_OFFSET+PERSIST_NAME_SIZE)
#define CONTROL_REC_SIZE            (CONTROL_REC_MAX_NAME_OFFSET+PERSIST_NAME_SIZE)

#define PERSIST_NAME_SIZE           (PERSIST_UINT16_SIZE)

class PersistentNameTable : public NameTable
{

public:
   PersistentNameTable(unsigned int nSlots, unsigned int nPerSeg, MSA& msa, PSA& psa);
   ~PersistentNameTable();
   PID create(void);
   int open(PID pid);
   void flush(void);


//   void loadEntry(PID);

//   virtual NameEntry* newEntry(void);
   NameEntry* addEntry(Name name, UInt hashVal, char* ident, UInt refCount);

   class NameEntry : public NameTable::NameEntry
   { 
   protected:
      PID   _pid;
      PID   _nextPID;
      PID   _prevPID;

      friend class PersistentNameTable;
   };

protected:
   virtual NameTable::NameEntry* newEntry(void) const;
   virtual NameTable::NameEntry* lockHash(unsigned int index);
   virtual NameTable::NameEntry* lockName(Name name);
   virtual NameTable::NameEntry* nextHashEntry(NameTable::NameEntry* entry);
   virtual Name nextName(void);
   NameEntry* loadEntry(PID pid);

public:
   PSA&     _psa;
   PID      _controlRecPID;
   PID      _hashPID;
   PID      _vecPID;
   bool     _controlRecordWriteFlag;
};

#endif /* PERSISTENT_NAME_TABLE_H_DEFINED */
