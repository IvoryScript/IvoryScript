/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    info.cpp
 *
 * Module:  Memory Storage Allocator
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Class method to insert diagnostic information into an output stream.
 *
 * Call format:
 *
 *    info(os, indent)
 *
 *       os       (ostream&)        - output stream
 *       indent   (unsigned int)    - indentation amount
 *
 * Errors:
 *
 *    None.
 *
 * Method:
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

#define N_ITEMS   6

void MSA::info(ostream& os, unsigned int indent) const {
   for (unsigned int j = 0; j < N_ITEMS; j++) {
      for (unsigned int i = 0; i < indent; i++)
         os << ' ';
      switch (j) {
         case 0:
            os << "No. segment(s) allocated = " << _nSeg << "\n";
            break;

         case 1:
            os << "Total memory allocated = " << _totUse << " bytes\n";
            break;

         case 2:
            os << "Maximum memory allocated = " << _maxUse << " bytes\n";
            break;

         case 3:
            os << "Memory in use = " << _currUse << " bytes\n";
            break;

         case 4:
            os << "Block count = " << _nBlk << "\n";
            break;

         default:
            break;
      }
   }
}
