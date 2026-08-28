/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    addBlk_GC.cpp
 *
 * Module:  Memory Storage Allocator with Garbage Collection
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Local class method to add a new block of memory.
 *
 * Call format:
 *
 *    res = addBlk(reqLen)
 *
 *       res      (int)          - Function result, 0 indicates success
 *       reqLen   (size_t)       - Size of request which must be satisfied
 *
 * Errors:
 *
 *    Errors are signalled with a negative return value.
 *
 * Method:
 *
 *    Consitionally calls base class method to add new block     
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

#include "MSA_GC.h"

int MSA_GC::addBlk(size_t reqLen) {
   if (!_enabled || _enableAddBlk) {
      int res = MSA::addBlk(reqLen);
      if (_enableAddBlk)
         _enabled = 1;
      return res;
   } else
      return -1;
}
