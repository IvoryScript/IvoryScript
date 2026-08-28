/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    general.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common general definitions
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

#ifndef GENERAL_H_DEFINED
#define GENERAL_H_DEFINED

#include <stdio.h>

#ifndef NULL
   #define NULL 0
#endif

//#define BIG_ENDIAN   0

#ifndef __GNUC__

// Some C++ compilers require an argument for delete

#define DELETE_NEEDS_NEW_ARG

#endif

#define GEN_BUF_SIZE 2048

#define MAX_VLI_LENGTH  10    // Sufficient for a 64 bit signed integer
#define MAX_VLU_LENGTH  10    // Sufficient for a 64 bit unsigned integer

// Assertion

#ifdef assert
#undef assert
#endif
#ifdef ENABLE_ASSERT
#define assert(pred,msg)\
if(!(pred)){::error(msg);\
throw(RunTimeError("assertion failed"));}
#else
#define assert(pred,msg)
#endif

#define error1(fmt,arg)\
{char errBuf[GEN_BUF_SIZE];\
sprintf(errBuf,(fmt),(arg));\
::error(errBuf);}

#define trace1(fmt,arg)\
{char traceBuf[GEN_BUF_SIZE];\
sprintf(traceBuf,(fmt),(arg));\
::trace(traceBuf);}

#define trace2(fmt,arg1, arg2)\
{char traceBuf[GEN_BUF_SIZE];\
sprintf(traceBuf,(fmt),(arg1),(arg2));\
::trace(traceBuf);}

typedef void Void;

// Common integral definitions

#include <cstdint>

#if defined(WIN32)
typedef                 int   BOOL;
#else
typedef                 bool  BOOL;
#endif
typedef                 bool  Bool;
typedef  unsigned       int   UINT;
typedef  unsigned int   UInt;

typedef  int8_t         INT8;
typedef  int8_t         Int8;
typedef  uint8_t        UINT8;
typedef  uint8_t        UInt8;
typedef  uint8_t        Byte;

typedef  int16_t        INT16;
typedef  int16_t        Int16;
typedef  uint16_t       UINT16;
typedef  uint16_t       UInt16;

typedef  int32_t        INT32;
typedef  int32_t        Int32;
typedef  uint32_t       UINT32;
typedef  uint32_t       UInt32;
typedef  int64_t        INT64;
typedef  int64_t        Int64;
typedef  uint64_t       UINT64;
typedef  uint64_t       UInt64;

//------------------------------------------------------------------------------
// RunTimeError: Runtime error information
//------------------------------------------------------------------------------

class RunTimeError {
public:
   RunTimeError(const char* reason);

   inline const char* reason(void) { return _reason; }
protected:
   const char* _reason;
};

//------------------------------------------------------------------------------
// HashDJB2: Simple hash builder
//------------------------------------------------------------------------------

class HashDJB2 {
public:
   HashDJB2();
   void gen(Byte byte);
   void gen(const Byte* bytes, size_t len);
   inline unsigned long hash(void) const { return _hash; }
   inline void setHash(unsigned long hash) { _hash = hash; }
protected:
   unsigned long _hash;
};

// Logging

extern bool          errorLogFlag;
extern const char*   errorLogFileName;

extern bool          traceLogFlag;
extern const char*   traceLogFileName;

// Function prototypes

extern void byteSwap(void* ptr, size_t size);

extern void error(const char* s);

extern unsigned long hashDJB2(const Byte* bytes, size_t len);

extern unsigned int hashString(const char* s);

extern unsigned int packVLI(long int val, unsigned char *buf);

extern unsigned int packVLU(unsigned long val, unsigned char *buf);

extern long int unpackVLI(unsigned char **buf);

extern unsigned long int unpackVLU(unsigned char **buf);

extern void trace(const char* s);

#endif /* GENERAL_H_DEFINED */
