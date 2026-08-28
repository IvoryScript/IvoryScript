/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    machine.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Functions related to the virtual machine.
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
 *
 * Notes:
 *
 */

#include "ivory/machine.h"

int vmLevel = 0;

#if GLOBAL_VM == 1

istream* inStrm;
ostream* outStrm;

MSA*        consMSA;
Env*        consEnv;

StackPtr    sb;
StackPtr    sp;

#if (GARBAGE_COLLECTION==1)
StackPtr    fp;
#endif

Void*       rack;

Cell*       cell;
Env*        cellEnv;

Cell*       updatePtr;    // Update pointer (retain for now)

Env*        rEnv; 
Name        rName;
Type        rType;
Cell*       rCell;
Expr        rExpr;
Byte        rByte;   
Bits        rBits;
Int         rInt;
Float       rFloat;
Double      rDouble;
Tag         rTag;
Char        rChar;

Byte*       rStruct;
size_t      rStructSize;
Void*       rPtr;
Ref         rRef;
UTC         rUTC;

#endif

Void emptyStacks$(argVM) {
   push(sb,StackPtr);
   sb = sp;
}
