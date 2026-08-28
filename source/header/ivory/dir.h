/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    dir.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Dir and DirEntry data types.
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

#ifndef IVORY_DIR_H_DEFINED
#define IVORY_DIR_H_DEFINED

#ifndef LINKAGE

// Suppress for linkage

#ifdef __linux__
#include "unix/dirent.h"
#else
#include "Win32/dirent.h"
#endif

#ifdef CONST
#undef CONST
#endif

#ifdef STRICT
#undef STRICT
#endif

#ifdef VOID
#undef VOID
#endif

typedef DIR* Dir;
typedef struct dirent* DirEntry;

#endif

#include "ivory/builtInFn.h"
//#include "ivory/machine.h"
#include "ivory/type.h"

#define sizeOfDir sizeof(Dir)

#define sizeOfDirEntry sizeof(DirEntry)

declareTypeCon(Dir);

declareTypeCon(DirEntry);


declareType(DirEntry);

declareType(DirEntry);

#define stackFPSizeDir stackSlotSize(Dir)

#define stackFPSizeDirEntry stackSlotSize(Dir)

#define loadArgRegDir(name) Dir name=(Dir)rPtr

#define loadArgRegDirEntry(name) DirEntry name=(DirEntry)rPtr

#define loadArgDir(name,off) Dir name=stack(off,Dir)

#define loadArgDirEntry(name,off) DirEntry name=stack(off,DirEntry)

#define storeArgDir(name,off) stack(off,Dir)=name

#define storeArgDirEntry(name,off) stack(off,DirEntry)=name

#define argRegFromCellDir(s,c) rPtr=cellBody(*cell,s).c

#define argRegFromCellDirEntry(s,c) rPtr=cellBody(*cell,s).c

#define returnDir(dir){\
rPtr = dir;\
jump(popLabel());}

#define returnJustDir(dir){\
if(8>=rStructSize){\
consMSA->free(rStruct);\
rStruct=(Byte*)consMSA->alloc(8);\
rStructSize=8;\
}\
rPtr=rStruct;\
*((Tag*)rPtr)=1;\
*((DIR**)(((Byte*)rPtr)+4))=dir;\
jump(popLabel());}

#define returnDirEntry(entry){\
rPtr = entry;\
jump(popLabel());}

#define returnJustDirEntry(entry){\
if(8>=rStructSize){\
consMSA->free(rStruct);\
rStruct=(Byte*)consMSA->alloc(8);\
rStructSize=8;\
}\
rPtr=rStruct;\
*((Tag*)rPtr)=1;\
*((struct dirent**)(((Byte*)rPtr)+4))=entry;\
jump(popLabel());}

declareBuiltInFn(closeDir);

declareBuiltInFn(dirEntryName);

declareBuiltInFn(makeDir);

declareBuiltInFn(openDir);

declareBuiltInFn(nextDirEntry);

declareBuiltInFn(removeDir);

#endif /* IVORY_DIR_H_DEFINED */
