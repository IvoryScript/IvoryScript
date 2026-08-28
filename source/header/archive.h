/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    archive.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 May 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Archive stream class for serialization.
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

#ifndef ARCHIVE_H_DEFINED
#define ARCHIVE_H_DEFINED

#include "general.h"

class Archive {
public:

	Archive& operator << (const char c);
	Archive& operator >> (char& c);

	Archive& operator << (const Byte b);
	Archive& operator >> (Byte& b);

	Archive& operator << (const Int8 i);
	Archive& operator >> (Int8& i);

	Archive& operator << (const Int16 i);
	Archive& operator >> (Int16& i);

	Archive& operator << (const UInt16 i);
	Archive& operator >> (UInt16& i);

	Archive& operator << (const Int32 i);
	Archive& operator >> (Int32& i);

	Archive& operator << (const UInt32 i);
	Archive& operator >> (UInt32& i);

	Archive& operator << (const float f);
	Archive& operator >> (float& f);

	Archive& operator << (const double d);
	Archive& operator >> (double& rd);

   void insertVLU(unsigned long val);
   unsigned long extractVLU(void);

   virtual Byte get(void) = 0;
   virtual void put(Byte c) = 0;
   virtual void get(Byte& c) = 0;

	virtual void getBytes(Byte byteV[], UInt n);
	virtual void putBytes(Byte byteV[], UInt n);
};

#endif // ARCHIVE_H_DEFINED
