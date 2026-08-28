/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    minimal_iostream.h
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

#ifndef MINIMAL_IOSTREAM_H_DEFINED
#define MINIMAL_IOSTREAM_H_DEFINED

#define WRITE_BUFFER_SIZE 256

#define _IO_INPUT	1
#define _IO_OUTPUT	2
#define _IO_BIN		128

#define _IO_DEC 020
#define _IO_HEX 0100

class istream;
class ostream;
class streambuf;
typedef ostream& (*__omanip)(ostream&);
typedef long streampos;
typedef long streamoff;

class ios_base
{
   ios_base();

   streambuf *_strbuf;
   int _flags;

   friend class ios;
   friend class istream;
   friend class ostream;
   friend class iostream;
};

class ios : public ios_base
{
public:
   ios();

    enum open_mode {
	in = _IO_INPUT,
	out = _IO_OUTPUT,
	bin = _IO_BIN,
	binary = _IO_BIN };
   enum { dec=_IO_DEC, hex=_IO_HEX };
    enum seek_dir { beg, cur, end};
    typedef enum seek_dir seekdir;

   friend class istream;
   friend class ostream;
   friend class iostream;
};

class ostream& hex(ostream&);
class ostream& dec(ostream&);

class ostream : virtual public ios    // Output stream
{
public:
   ostream(streambuf* strbuf);
   ostream(void);

public:
   ios_base& hex(ios_base& str);

   ostream& put(char c);

   ostream& write(const char* buf, int n);

   ostream& write(const unsigned char* buf, int n);

  	ostream& seekp(streampos);

   ostream& flush(void);

   ostream& operator<<(bool b);           // boolean

   ostream& operator<<(char c);           // characters

   ostream& operator<<(int i);            // integers

   ostream& operator<<(unsigned int ui);  // unsigned integers

   ostream& operator<<(long int li);      // long integers

   ostream& operator<<(void *ptr);        // pointers

   ostream& operator<<(float f);          // floats

   ostream& operator<<(double d);         // double floats

   ostream& operator<<(const char *s);    // strings

   ostream& operator<<(__omanip func) { return (*func)(*this); }
};

class istream : virtual public ios    // Input stream
{
public:
   istream(streambuf* strbuf);
   istream(void);

public:
   ios_base& hex(ios_base& str);

   istream& get(char& c);

   istream& read(char* buf, int n);

   istream& read(unsigned char* buf, int n);

   istream& operator>>(bool& b);           // boolean

   istream& operator>>(char& c);           // characters

   istream& operator>>(int& i);            // integers

   istream& operator>>(unsigned int& ui);  // unsigned integers

   istream& operator>>(long int& li);      // long integers

   istream& operator>>(void*& ptr);        // pointers

   istream& operator>>(float& f);          // floats

   istream& operator>>(double& d);         // double floats

   istream& operator>>(const char*& s);    // strings

//   istream& operator>>(__omanip func) { return (*func)(*this); }
};

class iostream : public istream, public ostream
{
public:
	iostream(streambuf*);
   iostream(void);
protected:
};

#endif /* MINIMAL_IOSTREAM_H_DEFINED*/
