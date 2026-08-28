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
 *    Implementation of openFile function
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
#include "ivory/string.h"

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
#define IVORY_SANDBOX_FILE_MODE       0600

static bool invalidSandboxPathComponent(const char* component, size_t len) {
   return len == 0 ||
      (len == 1 && component[0] == '.') ||
      (len == 2 && component[0] == '.' && component[1] == '.');
}

static int openFlagsForMode(IO_Mode mode) {
   switch (mode) {
      case IO_READ_TAG:
         return O_RDONLY;
      case IO_WRITE_TAG:
         return O_WRONLY | O_CREAT | O_TRUNC;
      case IO_APPEND_TAG:
         return O_WRONLY | O_CREAT | O_APPEND;
      case IO_READ_WRITE_TAG:
         return O_RDWR | O_CREAT | O_TRUNC;
      default:
         errno = EINVAL;
         return -1;
   }
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

static int openSandboxPathAt(int rootFD, const char* path,
   int openFlags, mode_t mode) {

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

      if (invalidSandboxPathComponent(component, componentLen)) {
         close(dirFD);
         errno = EINVAL;
         return -1;
      }

      if (componentLen > NAME_MAX) {
         close(dirFD);
         errno = ENAMETOOLONG;
         return -1;
      }

      char name[NAME_MAX + 1];
      memcpy(name, component, componentLen);
      name[componentLen] = '\0';

      if (slash == NULL) {
         int fd = openat(dirFD, name,
            openFlags | O_CLOEXEC | O_NOFOLLOW, mode);
         int savedErrno = errno;
         close(dirFD);
         errno = savedErrno;

         if (fd >= 0) {
            struct stat st;
            if (fstat(fd, &st) != 0) {
               savedErrno = errno;
               close(fd);
               errno = savedErrno;
               return -1;
            }

            if (S_ISDIR(st.st_mode)) {
               close(fd);
               errno = EISDIR;
               return -1;
            }
         }

         return fd;
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

static FILE* sandboxOpenFile(const char* path, IO_Mode mode) {
   const char* modeString = io_ModeString(mode);
   if (modeString == NULL) {
      errno = EINVAL;
      return NULL;
   }

   int openFlags = openFlagsForMode(mode);
   if (openFlags < 0)
      return NULL;

   int rootFD = openSandboxSessionDir();
   if (rootFD < 0)
      return NULL;

   int fd = openSandboxPathAt(rootFD, path, openFlags,
      IVORY_SANDBOX_FILE_MODE);
   int savedErrno = errno;
   close(rootFD);
   errno = savedErrno;

   if (fd < 0)
      return NULL;

   FILE* fp = fdopen(fd, modeString);
   if (fp == NULL) {
      savedErrno = errno;
      close(fd);
      errno = savedErrno;
   }

   return fp;
}

#endif

defineBuiltInFn_2_args(openFile,
   typeCon(String), typeCon(IO_Mode), typeCon(FileHandle),
   path, String,
   mode, IO_Mode)

#if defined(IVORY_SANDBOX_FILE_IO)
   FILE* fp = sandboxOpenFile(path, mode);
#else
   FILE* fp = fopen(path, io_ModeString(mode));
#endif

   drop_Stack(openFile);
   FileHandle fh(fp);
   returnFileHandle(fh);
endBuiltInFn
