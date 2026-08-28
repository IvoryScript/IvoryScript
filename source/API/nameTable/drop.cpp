/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    drop.cpp
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
 *    Method to drop a name.
 *
 * Call format:
 *
 *    drop(name);
 *
 *       name -   (Name)   - Previously added name
  *
 * Method:
 *
 *    Verifies that the name is valid.  If so, the entry use count is
 *    decremented and the name entry removed if there are no further references.
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

void NameTable::dropName(Name name) {
   Entry* entry;
   if (name >= _maxName || (entry = lockName(name)) == NULL) {
      error("NameTable::drop: name not in use");
      return;
   }
   if (adjustRefCount(entry, -1) == 0) {
      if (entry->_next != NULL)
         entry->_next->_prev = entry->_prev;
      if (entry->_prev != NULL)
         entry->_prev->_next = entry->_next;
      else
         _hashTab[entry->_hashVal % _nSlots] = entry->_next;
      _msa.free(entry->_string);
      _msa.free(entry);
      _vec.put(name, (NameTable::Entry*)NULL, _msa);
      if (name < _minFreeName)
         _minFreeName = name;
      _count--;
   }     
}
