/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ioMode.cpp
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
 *    Implementation of IO_Mode primitive data type.
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

#include "ivory/fileIO.h"
#include "ivory/class.h"

defineTypeCon(IO_Mode);

defineType(IO_Mode, typeCon(IO_Mode), NULL);

defineBuiltInNullaryDataCon(IO_Read,       IO_READ_TAG,         IO_Mode);
defineBuiltInNullaryDataCon(IO_Write,      IO_WRITE_TAG,        IO_Mode);
defineBuiltInNullaryDataCon(IO_Append,     IO_APPEND_TAG,       IO_Mode);
defineBuiltInNullaryDataCon(IO_ReadWrite,  IO_READ_WRITE_TAG,   IO_Mode);

const String io_ModeString(IO_Mode mode) {
   switch (mode) {
      case IO_READ_TAG:        return "rb";
      case IO_WRITE_TAG:       return "wb";
      case IO_APPEND_TAG:      return "ab";
      case IO_READ_WRITE_TAG:  return "w+b";   /* use "r+b" if non-truncating is desired */
      default:                 return NULL;
   }
}

