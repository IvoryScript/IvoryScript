/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    setAltName.cpp
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
 *    Method to set an alternative name.
 *
 * Call format:
 *
 *    setAltName(name, altName);
 *
 *    name     (Name)   - Primary name
 *    altName  (Name)   - Alternative name
 *
 * Method:
 *
 *    Updates the name entry.
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

Void NameTable::setAltName(Name name, Name altName) {
   Entry* entry;
   if (name >= _maxName || (entry = lockName(name)) == NULL) {
      error("NameTable::setAltName: name not in use");
      if (entry->_altName == NULL_NAME)
         entry->_altName = altName;
   }
}
