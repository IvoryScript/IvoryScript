/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    packVLU.c
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
 *    Function to pack a variable length unsigned integer into a buffer.
 *
 * Call format:
 *
 *    res = packVLU(val, buf)
 *
 *       res   (int)                - Function result, length of encoded data
 *       val   (unsigned long)      - Unsigned integer value
 *       buf   (unsigned char*)     - Pointer to buffer
 *
 * Errors:
 *
 *    None
 *
 * Method:
 *
 *    Inserts a variable length byte sequence depending on the value of
 *    val.  The number of bytes is encoded as a string of ms bits in the
 *    first byte.
 *
 * Examples:
 *
 *    0xxxxxxx                             range 0..127
 *    10xxxxxx xxxxxxxx                    range 0..16383
 *    110xxxxx xxxxxxxx xxxxxxxx           range 0..2097151
 *    1110xxxx xxxxxxxx xxxxxxxx xxxxxxxx  range 0..268435455
 *    ...
 *
 * Notes:
 *
 *    Currently, the inverse function 'unpackVLU' is limited to 4 bytes.
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

unsigned int packVLU(unsigned long val, unsigned char *buf)
{
   unsigned char v[MAX_VLU_LENGTH];
   unsigned int n = 0;
   unsigned int b;
   do
   {
      n++;
      b = n * 8 - n;
   }
   while (val >= (unsigned long)(1l << b));

   unsigned int i;
   for (i = 0; i < n; i++)
      v[i] = (unsigned char)((unsigned long int)val >> (n - i - 1) * 8)
         & 0xff;

   for (i = 0; i < n - 1; i++)
      v[0] ^= (0x80 >> i);

   for (i = 0; i < n; i++)
      *buf++ = v[i];

   return n;
}
