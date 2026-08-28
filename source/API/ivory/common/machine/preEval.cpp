/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    preEval.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to initialise the virtual machine prior to evaluation.
 *
 * Call format:
 *
 *    preEval$(iStream, oStream, memLimit n_vm) 
 *       iStream  (istream*)  - Input stream
 *       oStream  (ostream*)  - Output stream
 *       memLimit (Int)       - Memory allocation limit (bytes)
 *       n_vm     (argN_VM)   - Virtual Machine context
 *
 * Method:
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

//#define DEFAULT_BLK_LEN       (64*1024*1024)
//#define DEFAULT_BLK_LEN       (4*1024*1024)
//#define DEFAULT_BLK_LEN       (1024*1024)
//#define DEFAULT_BLK_LEN       65536
//#define DEFAULT_BLK_LEN       32768
#define DEFAULT_BLK_LEN       16384 // The current block size
//#define DEFAULT_BLK_LEN       8192
//#define DEFAULT_BLK_LEN       4096
//#define DEFAULT_BLK_LEN       2048
//#define DEFAULT_BLK_LEN       1024
//#define DEFAULT_BLK_LEN       512
//#define DEFAULT_BLK_LEN       256
//#define DEFAULT_BLK_LEN       128
//#define DEFAULT_BLK_LEN       64
#define DEFAULT_NAME_SLOTS    47
#define DEFAULT_NAMES_PER_SEG 16
#define DEFAULT_TYPE_SLOTS    47
#define DEFAULT_TYPES_PER_SEG 16

#define DEFAULT_STACK_SIZE    (1024*1024) // The current stack size

#if (GARBAGE_COLLECTION==1)
#include "MSA_GC.h"
#endif

#include "ivory/machine.h"
#include "ivory/nameTable.h"
#include "ivory/typeTable.h"
#include "ivory/trace.h"

Void preEval$(istream* iStream, ostream* oStream,
              Int memLimit argN_VM) {

#if (GLOBAL_VM == 1)
   if (vmLevel++ == 0) {
#endif

      inStrm = iStream != NULL
         ? iStream
         : &inStream;

      outStrm = oStream != NULL
         ? oStream
         : &outStream;


#if (GARBAGE_COLLECTION==1)
      consMSA = new MSA_GC(DEFAULT_BLK_LEN);
#else
      consMSA = new MSA(DEFAULT_BLK_LEN);
#endif

      consEnv = new(*consMSA) Env(*consMSA,
         *new(*consMSA) INameTable(DEFAULT_NAME_SLOTS,
            DEFAULT_NAMES_PER_SEG,
            *consMSA),
         NULL, NULL,
         *new(*consMSA) TypeTable(DEFAULT_TYPE_SLOTS,
            DEFAULT_TYPES_PER_SEG,
            *consMSA));

      Void* stackLo = consMSA->alloc(DEFAULT_STACK_SIZE);
      Void* stackHi = (unsigned char*)stackLo + DEFAULT_STACK_SIZE;

      sb = (StackPtr)stackHi;
      sp = sb;

//      outStream << "Initial stack pointer sp = " << sp << '\n'; //???

      rStructSize = 0;
      rStruct = NULL;

#if (GLOBAL_VM == 1)
   }
#endif

   if (memLimit > 0)
      consMSA->setLimit(memLimit);

// Pad stack B (see partial function update)

   push(NULL, Cell*);
   push(NULL, Label);

   emptyStack();
}
