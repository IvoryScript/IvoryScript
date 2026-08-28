/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    byteSwap.cpp
 *
 * Module:  General
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Byte swap function
 *
 * Call format:
 *
 *    byteSwap(ptr, size);
 *
 *    ptr -    (void*)     - Pointer to bytes
 *    size -   (size_t)    - Size of object
 *
 * Method:
 *
 *    Reverse the byte ordering in a buffer.
 *     
 * Errors:
 *
 *    None.
 *
 * Notes:
 *
 *    Size must be even.
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

#include "general.h"

void byteSwap(void* ptr, size_t size) {
   unsigned char* sptr = (unsigned char*)ptr;
   unsigned char* eptr = ((unsigned char*)ptr) + size;

   while (sptr < eptr) {
      unsigned char t = *--eptr;
      *eptr = *sptr;
      *sptr++ = t;
   }
}
