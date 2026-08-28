/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    check.cpp
 *
 * Module:  Memory Storage Allocator
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Class method for a basic corruption check 
 *
 * Call format:
 *
 *    res = check()
 *
 *       res - (int)          - Function result, 0 indicates success
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

#include "MSA.h"

int MSA::check(void) const {
   SegBlk* blk;

   for (blk = _hdBlk; blk != NULL; blk = blk->_next)
      if (blk->check() != 0)
         return -1;

   return 0;
}

//------------------------------------------------------------------------------
//
// Utility method to check for an invalid segment length
//
//------------------------------------------------------------------------------

int MSA::SegBlk::check(void) const {
   size_t     len;
   FreeSeg* curr = _hdFreeSeg;

   if (curr == NULL)
      return 0;

   do {
      len = curr->_len;

      if (len < MSA_QUANT)
         return -1;
 
      if ((curr = curr->_next) == NULL)
         break;
   } while (true);

   return 0;
}
