/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    unpackVLU.c
 *
 * Module:  General
 *
 * Author:  Alasdair Scott
 *
 * Original date: 12 March 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to extract variable length unsigned integer from byte stream.
 *
 * Call format:
 *
 *    res = unpackVLU(buf)
 *
 *       res   (unsigned long)   - Function result, extracted unsigned integer
 *       buf   (unsigned char**) - Address of pointer to buffer
 *
 * Errors:
 *
 *    None
 *
 *  Method:
 *     Transfers a variable length unsigned integer in Big-Endian order to 
 *     host unsigned long int.
 *     Reads 1,2,3 or 4 bytes depending on the following encoding:
 *
 *     0xxxxxxx                             range 0..127
 *     10xxxxxx xxxxxxxx                    range 0..16383
 *     110xxxxx xxxxxxxx xxxxxxxx           range 0..2091751
 *     1110xxxx xxxxxxxx xxxxxxxx xxxxxxxx  range 0..268435455
 *
 * Notes:
 *
 *    The pointer at the address given by buf is incremented past the bytes
 *    consumed.
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

unsigned long unpackVLU(unsigned char **buf)
{
   unsigned long res;
   unsigned char *ptr = *buf;

   if ((*ptr & 0x80) == 0)
   {
      res = *ptr;
      (*buf) += 1;
   }
   else if ((*ptr & 0x40) == 0)
   {
      res = ((unsigned short)(ptr[0] & 0x7f) << 8) +
             (unsigned short) ptr[1];
      (*buf) += 2;
   }
   else if ((*ptr & 0x20) == 0)
   {
      res = ((unsigned long)(ptr[0] & 0x03f) << 16) +
            ((unsigned long) ptr[1] << 8) +
             (unsigned long) ptr[2];
      (*buf) += 3;

   }
   else
   {
      res = ((unsigned long)(ptr[0] & 0x1f) << 24) +
            ((unsigned long) ptr[1] << 16) +
            ((unsigned long) ptr[2] << 8) +
             (unsigned long) ptr[3];
      (*buf) += 4;
   }

   return res;
}
