/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    main.cpp
 *
 * Module:  Ivory system session
 *
 * Author:  A Scott
 *
 * Date:    20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    The main function for an Ivory System session.
 *
 * Call format:
 *
 *    iss filename ...
 *
 * Method:
 *
 *    If a file with the given filename exists, it is expected to contain an
 *    active store which will be activated, otherwise an empty active store
 *    will be created.
 *
 *    IvoryScript orders will then be evaluated from standard input
 *    and displayed on standard output if they evaluate to a type for
 *    which 'show t' is defined.
 *
 *    On exit, the active store will be closed.
 *
 * Errors:
 *
 *    Errors will be reported to stderr.
 *
 * Notes:
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

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/capability.h>
#include <sys/syscall.h>

using namespace std;

#ifndef MINIMAL_IOSTREAM
#include <sstream>
using namespace std;
#endif

#define READ_BUFFER_SIZE    1024
#define NL_ESCAPE "\\\n"
#define NL_ESCAPE_WITH_CR "\\\r\n"

#define FILE_DIRECTIVE  ":file"
#define ISS_FILE_RUN_OPTIONS "run=\"memoryLimit=134217728\""
#define ISS_SCRIPT_RUN_OPTIONS "script run=\"memoryLimit=134217728\""

#include "nameTable.h"
#include "ivory/builtInDataCon.h"
#include "ivory/byteCodeModule.h"
#include "ivory/class.h"
#include "ivory/frameDescr.h"
#include "ivory/linkage.h"
#include "ivory/string.h"
#include "ivory/typeTable.h"
#include "ivory/void.h"
#include "ivory/compiler/compiler.h"

declareBuiltInFn(activateSandbox);

extern struct Linkage linkage_common;
//extern struct Linkage linkage_comms;
//extern struct Linkage linkage_measurement;
//extern struct Linkage linkage_serialIO;
//extern struct Linkage linkage_socket;
//extern struct Linkage linkage_timer;

static bool sessionActive = FALSE;

static bool dropSysChrootCapability(void) {
   struct __user_cap_header_struct hdr;
   struct __user_cap_data_struct data[2];   // 2 entries needed for 64-bit capability sets

   memset(&hdr, 0, sizeof(hdr));
   hdr.version = _LINUX_CAPABILITY_VERSION_3;
   hdr.pid = 0;   // current process

   if (syscall(SYS_capget, &hdr, data) != 0)
      return FALSE;

   unsigned int capIndex = CAP_SYS_CHROOT / 32;
   unsigned int capBit = CAP_SYS_CHROOT % 32;

   data[capIndex].effective &= ~(1U << capBit);
   data[capIndex].permitted &= ~(1U << capBit);

   return syscall(SYS_capset, &hdr, data) == 0;
}

extern "C" bool activateSandbox(const char* path) {
   if (sessionActive)
      return TRUE;

   if (path == NULL || path[0] != '/' || path[1] == '\0') {
      errno = EINVAL;
      return FALSE;
   }

   int rootFD = open(path, O_RDONLY | O_DIRECTORY | O_CLOEXEC | O_NOFOLLOW);
   if (rootFD < 0)
      return FALSE;

   bool ok =
      fchdir(rootFD) == 0 &&
      chroot(".") == 0 &&
      chdir("/") == 0;

   close(rootFD);

   if (!ok)
      return FALSE;

   dropSysChrootCapability();

   (void)setenv("HOME", "/", 1);
   (void)setenv("TMPDIR", "/tmp", 1);
   sessionActive = TRUE;

   return TRUE;
}

// activateSandbox :: String -> Void

defineBuiltInFn_1_arg(activateSandbox,
   typeCon(String), typeCon(Void),
   path, String)
   if (!activateSandbox(path))
      _exit(1);
   returnVoid();
endBuiltInFn

static Cell* builtInFns_iss[] = {
   builtInFn(activateSandbox)
};

static struct Linkage linkage_iss = {
   0, NULL,
   0, NULL,
   0, NULL,
   0, NULL,
   sizeof(builtInFns_iss) / sizeof(Cell*), builtInFns_iss,
};

static Linkage* linkage[] =
{
   &linkage_common,
   &linkage_iss,
//   &linkage_comms,
//   &linkage_measurement,
//   &linkage_serialIO,
//   &linkage_socket,
//   &linkage_timer,
};

// Void dummyModuleInit(Void);

declareEntry(raiseEvent$);

// The following is aimed at forcing
// Linkage to be referenced

extern Linkage** linkage_(Void) {
   return linkage;
}

Void postConstructorInit(Void) {

// Post constructor initialisation

   TypeDescr::initGlobalList();
   Module::initGlobalList();
   TypeCon::initBuiltInList();
   BuiltInDataCon::initGlobalList();
   BuiltInFn::initGlobalList();
   Class::initGlobalLists();

#if (FRAME_DESCRIPTORS==1)
   BuiltInFrameDescr::initGlobalList();
#endif

   builtInNameCount = builtInNameTable().count();
   builtInTypeCount = builtInTypeTable().count();

#ifdef DEBUG_INIT
   for (Type i = 0; i < builtInTypeCount; i++)
   {
      TypeDescr* descr = builtInTypeTable().typeDescr(i);
      traceExpr(descr->_typeSig, Env(builtInNameTable()));
   }
   outStream << "Primitive memory usage\n";
   builtInMSA().info(outStream, 0);
#endif
}

extern stringstream outputStream;


// Respond - copy to standard output

void respond(const char* buf, size_t len) {
   ::write(STDOUT_FILENO, buf, len);
}


char* strnstr(const char* s, const char* find, size_t slen) {
   char c, sc;
   size_t len;

   if ((c = *find++) != '\0') {
      len = strlen(find);
      do {
         do {
            if (slen-- < 1 || (sc = *s++) == '\0')
               return (NULL);
         } while (sc != c);
         if (len > slen)
            return (NULL);
      } while (strncmp(s, find, len) != 0);
      s--;
   }
   return ((char*)s);
}


// String with all occurences of "\\\n" or "\\\r\n" replaced with '\n'

size_t unescape(char* str, size_t len) {
   char* s = str;
   size_t rem = len;
   do {
      char* escPos = strnstr(s, NL_ESCAPE, rem - (s - str));
      if (escPos != NULL) {
         memmove(escPos, escPos + 1, rem - (escPos + 1 - str));
         rem -= 1;
      }
      else {
         escPos = strnstr(s, NL_ESCAPE_WITH_CR, rem - (s - str));
         if (escPos != NULL) {
            memmove(escPos, escPos + 2, rem - (escPos + 2 - str));
            rem -= 2;
         }
         else
            break;
      }
      s = escPos;
   } while (true);

   return rem;
}

class FilterStreambuf : public std::streambuf {

public:

   FilterStreambuf(std::ostream& os)
      : _streambuf(os.rdbuf())
      , _nlPrev(false)
      , _os(&os) {
      _os->rdbuf(this);
   }
   virtual ~FilterStreambuf() {
      if (_nlPrev)
         _streambuf->sputc('\n');

      if (_os != NULL) {
         _os->rdbuf(_streambuf);
      }
   }

protected:

   virtual int overflow(int ch) {
      if (_nlPrev) {
         _streambuf->sputc('\\');
         _streambuf->sputc('\n');
         _nlPrev = false;
      }

      if (ch == '\n') {
         _nlPrev = true;
         return ch;
      }
      else
         return _streambuf->sputc(ch);
   }

private:
   std::streambuf* _streambuf;
   bool              _nlPrev;
   std::ostream* _os;
};

int doCommand(char* command, int len) {
//  {FilterStreambuf filterStreambuf(std::cout);

   size_t unescLen = unescape(command, len);

   char ch = command[unescLen];
   command[unescLen] = '\0';
   if (unescLen > strlen(FILE_DIRECTIVE) &&
      strncmp(command, FILE_DIRECTIVE, strlen(FILE_DIRECTIVE)) == 0) {
      char* startQuote = strchr(command + strlen(FILE_DIRECTIVE), '"');
      char* endQuote = strchr(startQuote + 1, '"');
      *endQuote = '\0';
      compilerSequence("bytes", startQuote + 1, ISS_FILE_RUN_OPTIONS, NULL, NULL);
   }
   else
      compilerSequence("bytes", command, ISS_SCRIPT_RUN_OPTIONS, &cin, &cout);
   command[unescLen] = ch;
   //}

   cout.flush();

   return len;
}

void processInput(int fd) {
   char* buffer = NULL;
   size_t bufferSize = 0;
   size_t bufferLen = 0;
   char readBuffer[READ_BUFFER_SIZE];

   do {
      if (bufferLen == 0) {
         cerr << "> ";
         cerr.flush();
      }

      size_t len = ::read(fd, readBuffer, READ_BUFFER_SIZE);
      if (len <= 0)
         break;

//--- Grow buffer as needed, allowing space for a null terminator

      if (bufferLen + len + 1 > bufferSize) {
         bufferSize = bufferLen + len + 1;
         char* extBuffer = new char[bufferSize];
         memcpy(extBuffer, buffer, bufferLen);
         buffer = extBuffer;
      }

      memcpy(buffer + bufferLen, readBuffer, len);
      buffer[bufferLen + len] = '\0';
      bufferLen += len;

      char* nlPtr = strrchr(buffer, '\n');
      if (nlPtr != NULL &&
         !(nlPtr > buffer && *(nlPtr - 1) == '\\' ||
            nlPtr > buffer + 1 && *(nlPtr - 1) == '\r' && *(nlPtr - 2) == '\\')) {
         int cmd_res = doCommand(buffer, (nlPtr - buffer) + 1);
         if (cmd_res > 0)
            memmove(buffer, nlPtr + 1, bufferLen - cmd_res);
         bufferLen -= cmd_res;
      }
   } while (true);
}


UINT setTimer(UINT nIDEvent, UINT period) {
   return 0;
}

UINT killTimer(UINT nIDEvent) {
   return 0;
}

UINT readDigitalInput(UINT deviceId, UINT channelId) {
   return 0;
}

UINT writeDigitalOutput(UINT deviceId, UINT channelId, UINT level) {
   return 0;
}

void logSerialInput(const unsigned char* data, unsigned int len) {}

void logSerialOutput(const unsigned char* data, unsigned int len) {}

void logInboundPackets(long int count) {}

void logOutboundPackets(long int count) {}

int main(int argc, char* argv[]) {

   setbuf(stdout, NULL);

   postConstructorInit();

   processInput(STDIN_FILENO);
}
