/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    addEntry.cpp
 *
 * Module:  Name Table Class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 5 July 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Internal method to add a name entry.
 *
 * Call format:
 *
 *    addEntry(name, hashVal, ident, refCount)
 *
 *       res      (bool)         - Return value, true if successful
 *       name     (Name)         - Name index
 *       hashVal  (UInt)         - Hash value
 *       ident    (const char*)  - Identifier string
 *       refCount (UInt)         - Reference count
 *       ident -  (const char*)  - Identifier.
 *
 * Method:
 *
 *    Creates a new entry and links it into the hash table slot list
 *    before adding it to the name vector.
 *     
 * Errors:
 *
 *    Errors are signalled with a return value of false.
 *
 * Notes:
 *
 *    The memory for the identifier string is allocated by the calling function.
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

#include "nameTable.h"

NameTable::Entry* NameTable::addEntry(Name name, UInt hashVal,
                                      char* ident, UInt refCount) {

   Entry* entry = newEntry();
   if ((entry = newEntry()) == NULL) {
      error("NameTable::addEntry: no memory");
      return NULL;
   }
   Entry* next = lockHash(hashVal % _nSlots);
    if ((entry->_next = next) != NULL)
      next->_prev = entry;
   entry->_prev = NULL;
   entry->_name = name;
   entry->_altName = NULL_NAME;;
   entry->_string = ident;
   entry->_hashVal = hashVal;
   entry->_refCount = refCount;
   _hashTab[hashVal % _nSlots] = entry;
   _vec.put(entry->_name, entry, _msa);
   _count++;
	
   return entry;
}
