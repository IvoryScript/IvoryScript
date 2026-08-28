/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    lookUp.cpp
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
 *    Methods to look up names.
 *
 * Call format:
 *
 *    name = lookup(ident);
 *
 *    name -      (Name)         - Returned name
 *    ident -     (const char*)  - Name identifier
 *
 *    name = lookup(ident, hashVal);
 *
 *    entry -     (NameEntry)    - Returned name entry
 *    ident -     (const char*)  - Name identifier
 *    hashVal -   (unsigned int) - Hash value
 *
 * Method:
 *
 *    Public and local table lookup based on linear search of list
 *    of names with the same hash value.
 *     
 * Errors:
 *
 *    Errors are signalled with a return value of NAME_NULL or NULL.
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

Name NameTable::lookUp(const char *ident) const {
   unsigned int hashVal = hashString(ident);
   const Entry* entry;
   if ((entry = lookUp(ident, hashVal)) == NULL)
      return NULL_NAME;

   return entry->_name;
}

NameTable::Entry* NameTable::lookUp(const char* ident, unsigned int hashVal) const {
   for (Entry* entry = lockHash(hashVal % _nSlots);
      entry != NULL;
      entry = nextHashEntry(entry))
      if (strcmp(ident, entry->_string) == 0)
         return entry;
   return NULL;
}

const NameTable::Entry* NameTable::lookUp_(const char* ident, unsigned int hashVal) const {
   for (const Entry* entry = lockHash(hashVal % _nSlots);
      entry != NULL;
      entry = nextHashEntry(entry))
      if (strcmp(ident, entry->_string) == 0)
         return entry;
   return NULL;
}


NameTable::Entry* NameTable::lookUpByAlt(Name altName, unsigned int hashVal) const {
   for (Entry* entry = _hashTab[hashVal % _nSlots];
        entry != NULL;
        entry = nextHashEntry(entry))
      if (altName == entry->_altName)
         return entry;
   return NULL;
}
