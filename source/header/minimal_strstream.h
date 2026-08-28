/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    minimal_strstream.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 15 June 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with a minimal IOStream library.
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

#ifndef MINIMAL_STRSTREAM_H_DEFINED
#define MINIMAL_STRSTREAM_H_DEFINED

#include "minimal_iostream.h"

class streambuf
{
public:
   char* gptr() const; 
   char* egptr() const; 
   char* gbase() const; 
   char* pptr() const; 
   char* epptr() const; 
   char* pbase() const; 
   void setg(char* eb, char* g, char *eg);
   void setp(char* p, char* ep);
   int out_waiting();
   void gbump(int n);
   void pbump(int n);
   virtual int sync(void);
   virtual int underflow(void);
   virtual int overflow(int);
   virtual streampos seekoff(streamoff, ios::seek_dir, int mode=ios::in|ios::out);

   char* _IO_read_base; /* Start of get area. */
   char* _IO_read_ptr;	/* Current get pointer. */
   char* _IO_read_end;	/* End of get area. */

   char* _IO_write_base;/* Start of put area. */
   char* _IO_write_ptr;	/* Current put pointer. */
   char* _IO_write_end;	/* End of put area. */

   friend class strstreambuf;
};

class strstreambuf : public streambuf
{
public:
   strstreambuf(void);

public:
};

#endif /* MINIMAL_STRSTREAM_H_DEFINED*/
