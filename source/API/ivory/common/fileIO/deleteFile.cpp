/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    deleteFile.cpp
 *
 * Module:  Ivory common (file I/O)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 13 July 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of deleteFile function
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

#ifndef __linux__
#error IVORY_SANDBOX_FILE_IO is supported only on Linux
#endif

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define IVORY_SANDBOX_SESSION_DIR_ENV "ISSS_SESSION_DIR"

static bool invalidSandboxPathComponent(const char* component, size_t len) {
   return len == 0 ||
      (len == 1 && component[0] == '.') ||
      (len == 2 && component[0] == '.' && component[1] == '.');
}

static int openSandboxSessionDir(void) {
   const char* sessionDir = getenv(IVORY_SANDBOX_SESSION_DIR_ENV);
   if (sessionDir == NULL || sessionDir[0] != '/' ||
      sessionDir[1] == '\0') {
      errno = EINVAL;
      return -1;
   }

   return open(sessionDir,
      O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
}

static bool deleteSandboxPathAt(int rootFD, const char* path) {
   if (path == NULL || path[0] == '\0' || path[0] == '/') {
      errno = EINVAL;
      return false;
   }

   int dirFD = dup(rootFD);
   if (dirFD < 0)
      return false;

   const char* component = path;

   while (true) {
      const char* slash = strchr(component, '/');
      size_t componentLen = slash != NULL
         ? (size_t)(slash - component)
         : strlen(component);

      if (invalidSandboxPathComponent(component, componentLen)) {
         close(dirFD);
         errno = EINVAL;
         return false;
      }

      if (componentLen > NAME_MAX) {
         close(dirFD);
         errno = ENAMETOOLONG;
         return false;
      }

      char name[NAME_MAX + 1];
      memcpy(name, component, componentLen);
      name[componentLen] = '\0';

      if (slash == NULL) {
         struct stat st;
         if (fstatat(dirFD, name, &st, AT_SYMLINK_NOFOLLOW) != 0) {
            int savedErrno = errno;
            close(dirFD);
            errno = savedErrno;
            return false;
         }

         if (!S_ISREG(st.st_mode)) {
            close(dirFD);
            errno = EINVAL;
            return false;
         }

         bool ok = unlinkat(dirFD, name, 0) == 0;
         int savedErrno = errno;
         close(dirFD);
         errno = savedErrno;
         return ok;
      }

      int nextDirFD = openat(dirFD, name,
         O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
      int savedErrno = errno;
      close(dirFD);

      if (nextDirFD < 0) {
         errno = savedErrno;
         return false;
      }

      dirFD = nextDirFD;
      component = slash + 1;
   }
}

static bool sandboxDeleteFile(const char* path) {
   int rootFD = openSandboxSessionDir();
   if (rootFD < 0)
      return false;

   bool ok = deleteSandboxPathAt(rootFD, path);
   int savedErrno = errno;
   close(rootFD);
   errno = savedErrno;
   return ok;
}

#elif defined(_WIN32)

#include <io.h>

#else

#include <unistd.h>

#endif

#include "ivory/bool.h"
#include "ivory/fileIO.h"
#include "ivory/string.h"

// deleteFile :: String -> Bool

defineBuiltInFn_1_arg(deleteFile,
   typeCon(String), typeCon(Bool),
   path, String)

#if defined(IVORY_SANDBOX_FILE_IO)
   bool ok = sandboxDeleteFile(path);
#elif defined(_WIN32)
   bool ok = _unlink(path) == 0;
#else
   bool ok = unlink(path) == 0;
#endif

   returnBool(ok);
endBuiltInFn
