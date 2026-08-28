/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    hashDJB2.cpp
 *
 * Module:  General
 *
 * Author:  Alasdair Scott
 *
 * Original date: 21 February 2024
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    DJB2 hash function
 *
 * Call format:
 *
 *    hashVal = hashDJB2(s);
 *
 *    hashVal -   (unsigned int)       - Returned hash value
 *    bytes -     (const Byte* bytes)  - Byte sequence to hash
 *    len -       (size_t)             - length
 *   
 *
 * Method:
 *
 *    Hash function using Daniel J. Bernstein's method
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

#include "general.h"

#define HASHDJB2_INITIAL_HASH 5381

unsigned long hashDJB2(const Byte* bytes, size_t len) {
   unsigned long hash = HASHDJB2_INITIAL_HASH;
   for (size_t i = 0; i < len; i++)
      hash = ((hash << 5) + hash) + bytes[i]; // hash * 33 + b
   return hash;
}

HashDJB2::HashDJB2(void)
 : _hash(HASHDJB2_INITIAL_HASH) {
};

void HashDJB2::gen(Byte byte) {
   _hash = ((_hash << 5) + _hash) + byte;       // hash * 33 + b
}

void HashDJB2::gen(const Byte* bytes, size_t len) {
   for (size_t i = 0; i < len; i++)
      gen(bytes[i]);
}
