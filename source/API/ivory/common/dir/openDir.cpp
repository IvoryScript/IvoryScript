/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    openDir.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of openDir function
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

#include "ivory/dir.h"
#include "ivory/maybe.h"
#include "ivory/string.h"

#if defined(IVORY_SANDBOX_FILE_IO)

#include "sandboxPath.h"

static Dir sandboxOpenDir(const char* path) {
   int rootFD = ivorySandboxOpenSessionDir();
   if (rootFD < 0)
      return NULL;

   int dirFD = ivorySandboxOpenDirFDAt(rootFD, path);
   int savedErrno = errno;
   close(rootFD);
   errno = savedErrno;

   if (dirFD < 0)
      return NULL;

   DIR* dir = fdopendir(dirFD);
   if (dir == NULL) {
      savedErrno = errno;
      close(dirFD);
      errno = savedErrno;
   }

   return dir;
}

#endif

// openDir :: String -> Maybe Dir

defineBuiltInFn_1_arg(openDir,
   typeCon(String), builtInAp(typeCon(Maybe), typeCon(Dir)),
   s, String)
#if defined(IVORY_SANDBOX_FILE_IO)
   Dir dir = sandboxOpenDir(s);
#else
   Dir dir = opendir(s);
#endif
   if (dir != NULL) {
      returnJustDir(dir);
   }
   else {
      returnNothing(8);
   }
endBuiltInFn
