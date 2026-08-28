/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    unpackVLI.c
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
 *    Function to extract variable length integer from byte stream.
 *
 * Call format:
 *
 *    res = unpackVLI(buf)
 *
 *       res   (unsigned long int)  - Function result, extracted integer
 *       buf   (unsigned char**)    - Address of pointer to buffer
 *
 * Errors:
 *
 *    None
 *
 * Method:
 *
 *    Transfers a variable length signed integer in Big-Endian order to
 *    host INT32.
 *    Reads 1,2, 3 or 4 bytes depending on the following encoding:
 *
 *    00xxxxxx                             range 0..63
 *    11xxxxxx                             range -1..-64
 *    010xxxxx xxxxxxxx                    range 0..8191
 *    101xxxxx xxxxxxxx                    range -1..-8192
 *    0110xxxx xxxxxxxx xxxxxxxx           range 0..1048575
 *    1001xxxx xxxxxxxx xxxxxxxx           range -1..-1048576
 *    01110xxx xxxxxxxx xxxxxxxx xxxxxxxx  range 0..134217727
 *    10001xxx xxxxxxxx xxxxxxxx xxxxxxxx  range -1..-134217728
 *
 * Notes:
 *
 *    The pointer at the address given by buf is incremented past the bytes
 *    consumed.
 *
 *    Implemented with pattern matching for reasons of execution speed.
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

long int unpackVLI(unsigned char **buf)
{
   long res;
   unsigned char *ptr = *buf;

   if ((*ptr & 0xc0) == 0x00 || (*ptr & 0xC0) == 0xC0)
   {
      res = (char) *ptr;
      (*buf) += 1;
   }
   else if ((*ptr & 0xe0) == 0x40 || (*ptr & 0xe0) == 0xa0)
   {
      res = (short) (((unsigned short) ptr[0] ^ 0x40) << 8 |
		     (unsigned short) ptr[1]);
      (*buf) += 2;
   }
   else if ((*ptr & 0xf0) == 0x60 || (*ptr & 0xf0) == 0x90)
   {
      res = (long) (((*ptr & 0x80) == 0 ? 0 : 0xff000000) |
		    ((unsigned long) ptr[0] ^ 0x60) << 16 |
		    (unsigned long) ptr[1] << 8 |
		    (unsigned long) ptr[2]);
      (*buf) += 3;
   }
   else if ((*ptr & 0xf8) == 0x70 || (*ptr & 0xf8) == 0x88)
   {
      res = (long) (((unsigned long) ptr[0] ^ 0x70) << 24 |
		    (unsigned long) ptr[1] << 16 |
		    (unsigned long) ptr[2] << 8 |
		    (unsigned long) ptr[3]);
      (*buf) += 4;
   }

   return res;
}
