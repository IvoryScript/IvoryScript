/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    openFile.cpp
 *
 * Module:  Ivory common  (file I/O)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of readFileByte function
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

#ifdef __linux__
#include <unistd.h>
#elif _WIN32
#include <io.h>
#endif

#include "ivory/byte.h"
#include "ivory/fileIO.h"
#include "ivory/void.h"

defineBuiltInFn_1_arg(readFileByte,
   typeCon(FileHandle), typeCon(Byte),
   fileHandle, FileHandle)

   Byte byte;
   if (fileHandle.kind() == FileHandle::Kind::FH_FP) {
      if (fileHandle.state() == FileHandle::State::FH_READY) { 
         if (fread(&byte, sizeof(Byte), 1, fileHandle.fp()) != 1) {
            if (feof(fileHandle.fp()))
               fileHandle.setEOF();
            else
               fileHandle.setError();
         }
      }
   } else if (fileHandle.kind() == FileHandle::Kind::FH_FD) {

#ifdef _WIN32
      int res = _read(fileHandle.fd(), &byte, 1);
#else
      ssize_t res = read(fileHandle.fd(), &byte, 1);
#endif 

      if (res != 1) {
         if (res == 0)
            fileHandle.setEOF();
         else
            fileHandle.setError();
      }
   }

   returnByte(byte);
endBuiltInFn
