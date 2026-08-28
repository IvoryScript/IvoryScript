/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    load.cpp
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
 *    Method to load a serialized set of names from an archive stream.
 *
 * Call format:
 *
 *    load(archive)
 *
 *       archive  (Archive&)     - Archive stream
 *
 * Method:
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

void NameTable::load(Archive& archive) {
   UInt count = archive.extractVLU();

   for (UInt i = 0; i < count; i++) {
      Name name = (Name)archive.extractVLU();
      while (_maxName < name) {
         _vec.put(_maxName, NULL, _msa);
         _maxName++;
      }
      size_t len = archive.extractVLU();
      char* ident = (char*)_msa.alloc(len + 1);
      for (UInt j = 0; j < len; j++)
         archive >> ident[j];
      ident[len] = '\0';
      UInt32 hashVal;
      archive >> hashVal;
      UInt refCount = archive.extractVLU();
      Entry* entry = addEntry(name, hashVal, ident, refCount);
      if (entry != NULL) {
         onLoadEntry(entry);
         _vec.put(name, entry, _msa);
         _maxName = name + 1;
      } else {
         _msa.free(ident);
         error("NameTable::load no memory");
      } 
   }
}

void NameTable::onLoadEntry(Entry* entry) {
}
