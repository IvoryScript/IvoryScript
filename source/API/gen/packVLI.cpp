/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    packVLI.c
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
 *    Function to pack a variable length integer into a buffer.
 *
 * Call format:
 *
 *    res = packVLI(val, buf)
 *
 *       res   (int)                - Function result, length of encoded data
 *       val   (long int)           - Integer value
 *       buf   (unsigned char*)     - Pointer to buffer
 *
 * Errors:
 *
 *    None
 *
 * Method:
 *
 *      Inserts a variable length byte sequence depending on the value of
 *      iv.  The number of bytes is encoded in the first byte as a bit
 *      string immediately following the sign bit.  It is complemented
 *      for negative values of iv.
 *
 *      Examples:
 *
 *      00xxxxxx                             range 0..63
 *      11xxxxxx                             range -1..-64
 *      010xxxxx xxxxxxxx                    range 0..8191
 *      101xxxxx xxxxxxxx                    range -1..-8192
 *      0110xxxx xxxxxxxx xxxxxxxx           range 0..1048575
 *      1001xxxx xxxxxxxx xxxxxxxx           range -1..-1048576
 *      01110xxx xxxxxxxx xxxxxxxx xxxxxxxx  range 0..134217727
 *      10001xxx xxxxxxxx xxxxxxxx xxxxxxxx  range -1..-134217728
 *
 *      Notes:
 *
 *      Doesn't support more bytes than 'sizeof(long)'.
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

unsigned int packVLI(long int val, unsigned char *buf) {
   unsigned char v[MAX_VLI_LENGTH];
   unsigned int n = 0;
   unsigned int b;

   do {
      n++;
      b = n * 8 - n - 1;
   }
   while (-(1l << b) > val || val >= (1l << b));

   unsigned int i;
   for (i = 0; i < n; i++)
      v[i] = (unsigned char) ((unsigned long int) val >> (n - i - 1) * 8)
	 & 0xff;

   unsigned char c = 0x80;
   for (i = 0; i < n - 1; i++)
      v[0] ^= (c >>= 1);

   for (i = 0; i < n; i++)
      *buf++ = v[i];

   return n;
}
