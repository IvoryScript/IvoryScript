/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    fileIO.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with IvoryScript primitive file I/O
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

#ifndef IVORY_FILE_IO_DEFINED
#define IVORY_FILE_IO_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/type.h"

#define IO_READ_TAG        0
#define IO_WRITE_TAG       1
#define IO_APPEND_TAG      2
#define IO_READ_WRITE_TAG  3

typedef Tag IO_Mode;

#define sizeOfIO_Mode sizeof(IO_Mode)

#define typeSpecIO_Mode IO_Mode

class FileHandle {
public:
   enum class Kind :  char { FH_NONE, FH_FP, FH_FD };
   enum class State : char { FH_UNKNOWN, FH_READY, FH_EOF, FH_ERROR };

   FileHandle(Void) : _kind(Kind::FH_NONE), _state(State::FH_UNKNOWN) {
      _union._fp = NULL;
   }
   FileHandle(FILE* fp) :  _kind(Kind::FH_FP),
                           _state(fp != NULL ? State::FH_READY : State::FH_ERROR){
      _union._fp = fp; }
   FileHandle(int fd)  :   _kind(Kind::FH_FD)   { _union._fd = fd; }

   inline Kind kind (Void)    const { return _kind; }
   inline State state(Void)   const { return _state; }
   inline FILE* fp(Void)      const { return _union._fp; }
   inline int fd(Void)        const { return _union._fd; }

   inline Void setEOF(Void)   { _state = State::FH_EOF; }
   inline Void setError(Void) { _state = State::FH_ERROR; }

protected:
   Kind  _kind;
   State _state;
   union {
      FILE* _fp;
      int   _fd;
   } _union;
};

#define stackFPSizeIO_Mode stackSlotSize(IO_Mode)

#define typeSpecFileHandle FileHandle
#define typeSpecIO_Mode IO_Mode

#define argRegFileHandle rStruct
#define argRegIO_Mode rTag

#define loadArgRegFileHandle(name) FileHandle name=*(FileHandle*)rStruct
#define loadArgRegIO_Mode(name) IO_Mode name=(IO_Mode)rTag

#define loadArgFileHandle(name, off) FileHandle name=stack(off,FileHandle)
#define loadArgIO_Mode(name, off) IO_Mode name=stack(off,IO_Mode)

#define storeArgFileHandle(name,off) stack(off,FileHandle)=name
#define storeArgIO_Mode(name,off) stack(off,IO_Mode)=name

#define argRegFromCellFileHandle(s,c){\
if(sizeof(FileHandle)>rStructSize){\
consMSA->free(rStruct);\
rStruct=(Byte*)consMSA->alloc(sizeof(FileHandle));\
rStructSize = sizeof(FileHandle);\
};\
memcpy(rStruct,&(cellBody(*cell,s).c),sizeof(FileHandle));}\

#define argRegFromCellIO_Mode(s,c) rTag=cellBody(*cell,s).c

#define map_IO_Mode(name,srcEnv,dstEnv) name

//T.B.D
#define map_FileHandle(name,srcEnv,dstEnv) name

#define returnFileHandle(fh){\
if(sizeof(FileHandle)>rStructSize){\
consMSA->free(rStruct);\
rStruct=(Byte*)consMSA->alloc(sizeof(FileHandle));\
rStructSize = sizeof(FileHandle);\
};\
memcpy(rStruct,&(fh),sizeof(FileHandle));\
jump(popLabel());}

declareTypeCon(FileHandle);

declareTypeCon(IO_Mode);

declareType(IO_Mode);

declareNullaryDataCon(IO_Read);
declareNullaryDataCon(IO_Write);
declareNullaryDataCon(IO_Append);
declareNullaryDataCon(IO_ReadWrite);

declareBuiltInFn(closeFile); 
declareBuiltInFn(deleteFile);
declareBuiltInFn(openFile);
declareBuiltInFn(readFileByte);
declareBuiltInFn(writeFileByte);

extern const String io_ModeString(IO_Mode mode);

#endif /* IVORY_FILE_IO_DEFINED */
