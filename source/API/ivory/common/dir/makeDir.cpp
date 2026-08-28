/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    makeDir.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 13 July 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of makeDir built in function
 *
 * Modification history:
 *
 *-----------------------------------------------------------------------------
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

#if defined(IVORY_SANDBOX_FILE_IO)

#include <sys/stat.h>

#include "sandboxPath.h"

static bool sandboxMakeDir(const char* path) {
   int rootFD = ivorySandboxOpenSessionDir();
   if (rootFD < 0)
      return false;

   char leafName[NAME_MAX + 1];
   int parentFD = ivorySandboxOpenParentDirFDAt(rootFD, path,
      leafName, sizeof(leafName));
   int savedErrno = errno;
   close(rootFD);
   errno = savedErrno;

   if (parentFD < 0)
      return false;

   bool ok = mkdirat(parentFD, leafName, IVORY_SANDBOX_DIR_MODE) == 0;
   savedErrno = errno;
   close(parentFD);
   errno = savedErrno;

   return ok;
}

#endif

#include "ivory/dir.h"
#include "ivory/bool.h"
#include "ivory/string.h"

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#endif

// mkdir :: String -> Bool

defineBuiltInFn_1_arg(makeDir,
   typeCon(String), typeCon(Bool),
   path, String)

#if defined(IVORY_SANDBOX_FILE_IO)
   Bool ok = sandboxMakeDir(path);
#else
#if defined(_WIN32)
   Bool ok = _mkdir(path) == 0;
#else
   Bool ok = ::mkdir(path, 0777) == 0;
#endif
#endif

   returnBool(ok);
endBuiltInFn
