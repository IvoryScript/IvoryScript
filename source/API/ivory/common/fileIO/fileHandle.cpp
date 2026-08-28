/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    fileHandle.cpp
 *
 * Module:  Ivory common (file I/O)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 9 October 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of FileHandle primitive data type
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

#include <string.h>
#include "ivory/fileIO.h"

defineTypeCon(FileHandle);

defineTypeCon(FileHandleEntry);

static Void init$FileHandle(TypeDescr* typeDescr);
defineType_n(1, FileHandle, typeCon(FileHandle), init$FileHandle);

// FileHandle type methods.

Void evalFn$FileHandle(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   if (sizeof(FileHandle) > rStructSize) {
      consMSA->free(rStruct);
      rStruct = static_cast<Byte*>(consMSA->alloc(sizeof(FileHandle)));
      rStructSize = sizeof(FileHandle);
   }
   memcpy(rStruct, ptr, sizeof(FileHandle));
}

Void init$FileHandle(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(FileHandle);
   typeDescr->setRepr(Repr::REPR_FILE_HANDLE);
   typeDescr->_evalFn = evalFn$FileHandle;
}
