/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    archive.cpp
 *
 * Module:  Archive
 *
 * Author:  Alasdair Scott
 *
 * Original date: 29 May 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of an archive class for serialization.
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

#include "archive.h"

#define MAX_LENGTH   8

/*-------------------------------------------------------------------------*/

Archive& Archive::operator << (const char c) {
    put(c);
    return *this;
}

Archive& Archive::operator >> (char& c) {
   get(*(Byte*)(void*)&c);
   return *this;
}

Archive& Archive::operator << (const Byte b) {
   put(b);
   return *this;
}

Archive& Archive::operator >> (Byte& b) {
   get(b);
   return *this;
}

Archive& Archive::operator << (const Int8 i) {
   put(i);
   return *this;
}

Archive& Archive::operator >> (Int8& i) {
   get(*(Byte*)(void*)&i);
   return *this;
}

Archive& Archive::operator << (const Int16 i) {

#if (BIG_ENDIAN==1)
   Int16 ti = i;
   byteSwap(&ti, sizeof(Int16);
#endif

   for (unsigned int j = 0; j < sizeof(Int16); j++)
      put(((Byte*)(&i))[j]);
    return *this;
}

Archive& Archive::operator >> (Int16& i) {
   for (unsigned int j = 0; j < sizeof(Int16); j++)
      get(((Byte*)(&i))[j]);

#if (BIG_ENDIAN==1)
      byteSwap(&i, sizeof(Int16);
#endif

	return *this;
}

Archive& Archive::operator << (const UInt16 i) {

#if (BIG_ENDIAN==1)
   UInt16 ti = i;
   byteSwap(&ti, sizeof(UInt16);
#endif

   for (unsigned int j = 0; j < sizeof(UInt16); j++)
      put(((Byte*)(&i))[j]);
    return *this;
}

Archive& Archive::operator >> (UInt16& i) {
   for (unsigned int j = 0; j < sizeof(UInt16); j++)
      get(((Byte*)(&i))[j]);

#if (BIG_ENDIAN==1)
      byteSwap(&i, sizeof(UInt16);
#endif

	return *this;
}

Archive& Archive::operator << (const Int32 i) {

#if (BIG_ENDIAN==1)
   Int32 ti = i;
   byteSwap(&ti, sizeof(Int32);
#endif

   for (unsigned int j = 0; j < sizeof(Int32); j++)
      put(((Byte*)(&i))[j]);
    return *this;
}

Archive& Archive::operator >> (Int32& i) {
   for (unsigned int j = 0; j < sizeof(Int32); j++)
      get(((Byte*)(&i))[j]);

#if (BIG_ENDIAN==1)
      byteSwap(&i, sizeof(Int32);
#endif

	return *this;
}

Archive& Archive::operator << (const UInt32 i) {
   UInt32 ti = i;

#if (BIG_ENDIAN==1)
      byteSwap(&ti, sizeof(UInt32);
#endif

   for (unsigned int j = 0; j < sizeof(UInt32); j++)
      put(((char*)(&ti))[j]);
    return *this;
}

Archive& Archive::operator >> (UInt32& i) {
   for (unsigned int j = 0; j < sizeof(UInt32); j++)
      get(((Byte*)(&i))[j]);

#if (BIG_ENDIAN==1)
      byteSwap(&i, sizeof(UInt32);
#endif

	return *this;
}

Archive& Archive::operator << (const float f) {
   float tf = f;

#if (BIG_ENDIAN==1)
      byteSwap(&tf, sizeof(float);
#endif

   for (unsigned int i = 0; i < sizeof(float); i++)
      put(((char*)(&tf))[i]);
   return *this;
}

Archive& Archive::operator >> (float& f) {
   for (unsigned int i = 0; i < sizeof(float); i++)
      get(((Byte*)(&f))[i]);

#if (BIG_ENDIAN==1)
      byteSwap(&d, sizeof(double);
#endif

	return *this;}

Archive& Archive::operator << (const double d) {
   double td = d;

#if (BIG_ENDIAN==1)
      byteSwap(&td, sizeof(double);
#endif

   for (unsigned int i = 0; i < sizeof(double); i++)
      put(((char*)(&td))[i]);
   return *this;
}

Archive& Archive::operator >> (double& d) {
   for (unsigned int i = 0; i < sizeof(double); i++)
      get(((Byte*)(&d))[i]);

#if (BIG_ENDIAN==1)
      byteSwap(&d, sizeof(double);
#endif

	return *this;
}

void Archive::getBytes(Byte byteV[], UInt n) {
   for (UInt i = 0; i < n; i++)
      byteV[i] = get();
}

void Archive::putBytes(Byte byteV[], UInt n) {
   for (UInt i = 0; i < n; i++)
      put(byteV[i]);
}

/*-------------------------------------------------------------------------*/

void Archive::insertVLU(unsigned long val) {
   Byte v[MAX_LENGTH];
   int n, b, i;

   n = 0;
   do
   {
      n++;
      b = n * 8 - n;
   }
   while (val >= (unsigned long)(1l << b));

   for (i = 0; i < n; i++)
      v[i] = (Byte)((unsigned long int)val >> (n - i - 1) * 8)
         & 0xff;

   for (i = 0; i < n - 1; i++)
      v[0] ^= (0x80 >> i);

   for (i = 0; i < n; i++)
      *this << v[i];
}

unsigned long Archive::extractVLU(void) {
   unsigned long res;
   Byte b;

   *this >> b;

   if ((b & 0x80) == 0)
      res = 0;
   else if ((b & 0x40) == 0)
   {
      res = ((unsigned short)(b & 0x7f) << 8);
      *this >> b;
   }
   else if ((b & 0x20) == 0)
   {
      res = ((unsigned long)(b & 0x03f) << 16);
      *this >> b;
      res += ((unsigned long)b << 8);
      *this >> b;
   }
   else
   {
      res = ((unsigned long)(b & 0x1f) << 24);
      *this >> b;
      res += ((unsigned long)b << 16);
      *this >> b;
      res += ((unsigned long)b << 8);
      *this >> b;
   }

   return res + b;
}
