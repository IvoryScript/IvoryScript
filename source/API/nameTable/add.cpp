/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    add.cpp
 *
 * Module:  Name Table Class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Method to add a name.
 *
 * Call format:
 *
 *    name = add(ident, hashVal);
 *
 *       name     (Name)         - returned name
 *       ident    (const char*)  - identifier
 *       hashVal  (unsigned int) - hash value
 *
 *
 *    add(name);
 *
 *       name     (Name)         - Name
 *
 * Method:
 *
 *    First form:
 *    Looks up the given identifier.  If present, returns the name
 *    after incrementing the entry reference count.  Otherwise a new
 *    name entry is added to the table.
 *
 *    Second form:
 *    Simply increments the entry reference count.
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

#include "nameTable.h"
#include <string.h>

NameTable::Entry* NameTable::addName(const char* ident, unsigned int hashVal) {
   Entry* entry;

   char* identCopy;
   if ((identCopy = (char*)_msa.alloc(strlen(ident) + 1)) == NULL) {      
      error("NameTable::add: no memory");
      return NULL;
   }
   strcpy(identCopy, ident);
   if ((entry = addEntry(nextName(), hashVal, identCopy, 1)) == NULL) {
      _msa.free(identCopy);
      error("NameTable::add: no memory");
      return NULL;
   }
	
   return entry;
}

void NameTable::addName(Name name) {
   Entry* entry;
   if (name >= _maxName || (entry = lockName(name)) == NULL) {
      error("NameTable::add: name not in use");
      return;
   }
   entry->_refCount++;
   return;
}
