/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    closeFile.cpp
 *
 * Module:  Ivory common (file I/O)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of closeFile function
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

#include "ivory/fileIO.h"
#include "ivory/void.h"

defineBuiltInFn_1_arg(closeFile,
   typeCon(FileHandle), typeCon(Void),
   fileHandle, FileHandle)

   if (fileHandle.kind() == FileHandle::Kind::FH_FP) {
      if (fileHandle.fp() != NULL)
         fclose(fileHandle.fp());
   } else

#ifdef _WIN32
      _close(fileHandle.fd());
#else
      close(fileHandle.fd());
#endif

   returnVoid();
endBuiltInFn
