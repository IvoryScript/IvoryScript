/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    sandboxPath.h
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
 *    Helpers for sandboxed directory/file path resolution.
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

#ifndef IVORY_COMMON_DIR_SANDBOX_PATH_H_DEFINED
#define IVORY_COMMON_DIR_SANDBOX_PATH_H_DEFINED

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
#include <unistd.h>

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define IVORY_SANDBOX_SESSION_DIR_ENV "ISSS_SESSION_DIR"
#define IVORY_SANDBOX_DIR_MODE        0700

static bool ivorySandboxInvalidPathComponent(const char* component,
   size_t len) {
   return len == 0 ||
      (len == 1 && component[0] == '.') ||
      (len == 2 && component[0] == '.' && component[1] == '.');
}

static bool ivorySandboxCopyPathComponent(char* dst, size_t dstLen,
   const char* component, size_t componentLen) {
   if (ivorySandboxInvalidPathComponent(component, componentLen)) {
      errno = EINVAL;
      return false;
   }

   if (componentLen > NAME_MAX || componentLen >= dstLen) {
      errno = ENAMETOOLONG;
      return false;
   }

   memcpy(dst, component, componentLen);
   dst[componentLen] = '\0';
   return true;
}

static int ivorySandboxOpenSessionDir(void) {
   const char* sessionDir = getenv(IVORY_SANDBOX_SESSION_DIR_ENV);
   if (sessionDir == NULL || sessionDir[0] != '/' ||
      sessionDir[1] == '\0') {
      errno = EINVAL;
      return -1;
   }

   return open(sessionDir,
      O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
}

static int ivorySandboxOpenDirFDAt(int rootFD, const char* path) {
   if (path == NULL || path[0] == '\0' || path[0] == '/') {
      errno = EINVAL;
      return -1;
   }

   if (strcmp(path, ".") == 0)
      return dup(rootFD);

   int dirFD = dup(rootFD);
   if (dirFD < 0)
      return -1;

   const char* component = path;

   while (true) {
      const char* slash = strchr(component, '/');
      size_t componentLen = slash != NULL
         ? (size_t)(slash - component)
         : strlen(component);

      char name[NAME_MAX + 1];
      if (!ivorySandboxCopyPathComponent(name, sizeof(name),
         component, componentLen)) {
         int savedErrno = errno;
         close(dirFD);
         errno = savedErrno;
         return -1;
      }

      int nextDirFD = openat(dirFD, name,
         O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
      int savedErrno = errno;
      close(dirFD);

      if (nextDirFD < 0) {
         errno = savedErrno;
         return -1;
      }

      if (slash == NULL)
         return nextDirFD;

      dirFD = nextDirFD;
      component = slash + 1;
   }
}

static int ivorySandboxOpenParentDirFDAt(int rootFD, const char* path,
   char* leafName, size_t leafNameLen) {
   if (path == NULL || path[0] == '\0' || path[0] == '/') {
      errno = EINVAL;
      return -1;
   }

   int dirFD = dup(rootFD);
   if (dirFD < 0)
      return -1;

   const char* component = path;

   while (true) {
      const char* slash = strchr(component, '/');
      size_t componentLen = slash != NULL
         ? (size_t)(slash - component)
         : strlen(component);

      char name[NAME_MAX + 1];
      if (!ivorySandboxCopyPathComponent(name, sizeof(name),
         component, componentLen)) {
         int savedErrno = errno;
         close(dirFD);
         errno = savedErrno;
         return -1;
      }

      if (slash == NULL) {
         if (strlen(name) >= leafNameLen) {
            close(dirFD);
            errno = ENAMETOOLONG;
            return -1;
         }

         strcpy(leafName, name);
         return dirFD;
      }

      int nextDirFD = openat(dirFD, name,
         O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
      int savedErrno = errno;
      close(dirFD);

      if (nextDirFD < 0) {
         errno = savedErrno;
         return -1;
      }

      dirFD = nextDirFD;
      component = slash + 1;
   }
}

#endif

#endif /* IVORY_COMMON_DIR_SANDBOX_PATH_H_DEFINED */
