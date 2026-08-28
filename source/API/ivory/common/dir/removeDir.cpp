/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    removeDir.cpp
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
 *    Implementation of removeDir built in function
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

static bool sandboxRemoveDir(const char* path) {
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

   struct stat st;
   if (fstatat(parentFD, leafName, &st, AT_SYMLINK_NOFOLLOW) != 0) {
      savedErrno = errno;
      close(parentFD);
      errno = savedErrno;
      return false;
   }

   if (!S_ISDIR(st.st_mode)) {
      close(parentFD);
      errno = EINVAL;
      return false;
   }

   bool ok = unlinkat(parentFD, leafName, AT_REMOVEDIR) == 0;
   savedErrno = errno;
   close(parentFD);
   errno = savedErrno;

   return ok;
}
#endif

#if defined(_WIN32)
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "ivory/dir.h"
#include "ivory/bool.h"
#include "ivory/string.h"

// removeDir :: String -> Bool

defineBuiltInFn_1_arg(removeDir,
   typeCon(String), typeCon(Bool),
   path, String)

#if defined(IVORY_SANDBOX_FILE_IO)
   bool ok = sandboxRemoveDir(path);
#elif defined(_WIN32)
   bool ok = _rmdir(path) == 0;
#else
   bool ok = ::rmdir(path) == 0;
#endif

   returnBool(ok);
endBuiltInFn
