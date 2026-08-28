/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    altName.cpp
 *
 * Module:  Name Table Class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 10 November 2025
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Method to return the alternative name.
 *
 * Call format:
 *
 *    res = altName(name);
 *
 *    res   (Name) - Associated alternative name
 *
 * Method:
 *
 *    Verifies that the name is valid.
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

Name NameTable::altName(Name name) const {
   Entry* entry;
   if (name >= _maxName || (entry = lockName(name)) == NULL) {
      error("NameTable::altName: name not in use");
      return NULL_NAME;
   }
   return entry->_altName;
}
