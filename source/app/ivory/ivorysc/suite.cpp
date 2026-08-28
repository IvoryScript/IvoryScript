/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    suite.cpp
 *
 * Module:  IvoryScript compiler suite driver
 *
 * Author:  A Scott
 *
 * Date:    20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Main function to process commands to invoke the compiler and verify test
 *    results.
 *
 * Notes:
 *
 * Modification history:
 *
 * /home/lavm/ivory/luc/src/icode/exec.c created by Lavm on winch on Wed Jan  8 13:09:49 2003 
 *  $Id: code_tester.cpp,v 1.4 2003/07/02 19:55:59 lavm Exp $
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

#include <stdio.h>
#include <stdlib.h>
#ifdef unix // AJS
#include <unistd.h>
#endif  // AJS
#include <sstream>
#include <string.h>
#include <sys/stat.h>
#ifdef unix
#include <dirent.h>
#else
#include <direct.h>
#include <io.h>
#endif

#include "OptionsParser.h"
#include <ivory/machine.h>
#include <ivory/common.h>
#include <ivory/int.h>
#include <ivory/type.h>
#include <ivory/tuple.h>
#include <ivory/repr.h>
#include "ivory/ads.h"
#include <ivory/register.h>
#include <ivory/root.h>
#include <ivory/compiler/compiler.h>
#include <ivory/compiler/parser.h>

#include "suite.h"


#define TMP_BUFFER_SIZE 1024
#define IVORY_LINE_MAX 1024

/**********************************************************************

            ivory vm setup for testing purpose

**********************************************************************/

extern int execScript(const char* s);

#include <fstream>
#include <sstream>
#include <string>

#include "ivory/compiler/compiler.h"  // for compilerSequence

// Simple prefix check
static bool startsWith(const char* s, const char* prefix) {
   while (*prefix) {
      if (*s++ != *prefix++) return false;
   }
   return true;
}

// Run an ORDER script file through the compiler/VM and capture stdout-like output
// into `output`. Returns non-zero on success.
static int runOrderScriptToString(const char* target,
                                  const char* sourceFilename,
                                  const char* options,
                                  std::string& output) {
   std::ostringstream out;
   std::istream* inStream = NULL;
   std::ostream* outStream = &out;

   if (!compilerSequence("bytes", sourceFilename, options, inStream, outStream))
      return 0;

   output = out.str();
   return 1;
}

// Create all directories in the path leading to filePath
static void makeDirs(const std::string& filePath) {
   std::string::size_type pos = 0;
   while ((pos = filePath.find_first_of("/\\", pos + 1)) != std::string::npos) {
      std::string dir = filePath.substr(0, pos);
#ifdef unix
      mkdir(dir.c_str(), 0755);
#else
      _mkdir(dir.c_str());
#endif
   }
}

// Return an expected-output filename from a script path and a base directory.
static std::string makeExpectedPath(const char* scriptFilename,
   const char* expectedDir) {
   std::string script(scriptFilename);
   std::string dir(expectedDir);

// Extract script directory and basename
   std::string scriptDir;
   std::string base = script;

   std::string::size_type pos =
      script.find_last_of("/\\");
   if (pos != std::string::npos) {
      scriptDir = script.substr(0, pos + 1);
      base = script.substr(pos + 1);
   }

   std::string path = dir;

   if (!path.empty() &&
      path[path.size() - 1] != '/' &&
      path[path.size() - 1] != '\\')
      path += "/";

   if (base.size() > 3 && base.substr(base.size() - 3) == ".is")
      base = base.substr(0, base.size() - 3);

   path += scriptDir;
   path += base;
   path += ".out";

   return path;
}

// Regression check for a single file.
// `actualPath` is the file to compile/run
// `localPath` is the key used to build the expected-output path
// (relative to the source root when called from a directory walk,
//  identical to actualPath for a direct file call).
static void checkOutputFile(const char* target,
   const char* actualPath,
   const char* localPath,
   const char* options,
   const char* defaultExpectedDir,
   FILE* resultFile) {
   const char* expectedDir = defaultExpectedDir;

   if (options != NULL && options[0] != '\0') {
      OptionsParser opt(options);
      const char* expOpt = opt.getString("expected");
      if (expOpt != NULL && expOpt[0] != '\0')
         expectedDir = expOpt;
   }

   std::string actual;
   if (!runOrderScriptToString(target, actualPath, options, actual)) {
      fprintf(resultFile, "CHECK-OUTPUT %s: compile or run error\n", localPath);
      fflush(resultFile);
      return;
   }

   std::string expectedPath = makeExpectedPath(localPath, expectedDir);

   std::ifstream expIn(expectedPath.c_str(), std::ios::in | std::ios::binary);
   if (!expIn) {
      makeDirs(expectedPath);
      std::ofstream expOut(expectedPath.c_str(),
         std::ios::out | std::ios::binary | std::ios::trunc);

      if (!expOut) {
         fprintf(resultFile,
            "CHECK-OUTPUT %s: cannot create baseline (%s)\n",
            localPath, expectedPath.c_str());
         fflush(resultFile);
         return;
      }

      expOut << actual;
      expOut.close();

      fprintf(resultFile,
         "CHECK-OUTPUT %s: baseline created (%s)\n",
         localPath, expectedPath.c_str());
      fflush(resultFile);
      return;
   }

   std::ostringstream buf;
   buf << expIn.rdbuf();
   std::string expected = buf.str();

   if (expected == actual) {
      fprintf(resultFile, "CHECK-OUTPUT %s: OK\n", localPath);
   }
   else {
      fprintf(resultFile,
         "CHECK-OUTPUT %s: OUTPUT MISMATCH (%s)\n",
         localPath, expectedPath.c_str());
   }
   fflush(resultFile);
}

// Recursively walk dirPath, running checkOutputFile for every .is file found.
// localPrefix is the accumulated relative path from the original sourcePath root.
static void checkOutputDir(const char* target,
   const char* dirPath,
   const char* localPrefix,
   const char* options,
   const char* defaultExpectedDir,
   FILE* resultFile) {
#ifdef unix
   DIR* d = opendir(dirPath);
   if (!d) {
      fprintf(resultFile, "CHECK-OUTPUT %s: cannot open directory\n", dirPath);
      fflush(resultFile);
      return;
   }
   struct dirent* entry;
   while ((entry = readdir(d)) != NULL) {
      if (entry->d_name[0] == '.')
         continue;

      std::string fullPath = std::string(dirPath) + "/" + entry->d_name;
      std::string localPath = (*localPrefix)
         ? std::string(localPrefix) + "/" + entry->d_name
         : entry->d_name;

      struct stat st;
      if (stat(fullPath.c_str(), &st) != 0)
         continue;

      if (S_ISDIR(st.st_mode)) {
         checkOutputDir(target, fullPath.c_str(), localPath.c_str(),
            options, defaultExpectedDir, resultFile);
      }
      else {
         size_t len = strlen(entry->d_name);
         if (len > 3 && strcmp(entry->d_name + len - 3, ".is") == 0)
            checkOutputFile(target, fullPath.c_str(), localPath.c_str(),
               options, defaultExpectedDir, resultFile);
      }
   }
   closedir(d);
#else
   std::string pattern = std::string(dirPath) + "/*";
   struct _finddata_t fd;
   intptr_t handle = _findfirst(pattern.c_str(), &fd);
   if (handle == -1) {
      fprintf(resultFile, "CHECK-OUTPUT %s: cannot open directory\n", dirPath);
      fflush(resultFile);
      return;
   }
   do {
      if (fd.name[0] == '.')
         continue;

      std::string fullPath = std::string(dirPath) + "/" + fd.name;
      std::string localPath = (*localPrefix)
         ? std::string(localPrefix) + "/" + fd.name
         : std::string(fd.name);

      if (fd.attrib & _A_SUBDIR) {
         checkOutputDir(target, fullPath.c_str(), localPath.c_str(),
            options, defaultExpectedDir, resultFile);
      }
      else {
         size_t len = strlen(fd.name);
         if (len > 3 && strcmp(fd.name + len - 3, ".is") == 0)
            checkOutputFile(target, fullPath.c_str(), localPath.c_str(),
               options, defaultExpectedDir, resultFile);
      }
   } while (_findnext(handle, &fd) == 0);
   _findclose(handle);
#endif
}

// Dispatcher: if sourcePath is a directory, recurse; otherwise run a single check.
// `localRoot` is the path used to key expected-output files — typically the
// suite-relative filename, without any scriptDir prefix.
static void checkOutput(const char* target,
   const char* sourcePath,
   const char* localRoot,
   const char* options,
   const char* defaultExpectedDir,
   FILE* resultFile) {
   struct stat st;
   if (stat(sourcePath, &st) != 0) {
      fprintf(resultFile, "CHECK-OUTPUT %s: path not found\n", localRoot);
      fflush(resultFile);
      return;
   }

#ifdef unix
   if (S_ISDIR(st.st_mode))
#else
   if (st.st_mode & _S_IFDIR)
#endif
   {
      checkOutputDir(target, sourcePath, localRoot, options, defaultExpectedDir, resultFile);
   }
   else {
      checkOutputFile(target, sourcePath, localRoot,
         options, defaultExpectedDir, resultFile);
   }
}

// The main driver loop

int suite(char* filename, char* resultFilename, char* scriptDir, char* expectedDir) {
   char line[IVORY_LINE_MAX];
   char target[IVORY_LINE_MAX];
   char sourceFilename[IVORY_LINE_MAX];
   char options[IVORY_LINE_MAX];

   FILE* testfile = fopen(filename, "r");
   if (!testfile) {
      fprintf(stderr, "Cannot open suite file %s\n", filename);
      return 1;
   }

   FILE* resultFile = fopen(resultFilename, "w");
   if (!resultFile) {
      fprintf(stderr, "Cannot open result file %s\n", resultFilename);
      fclose(testfile);
      return 1;
   }

   char* result;

   while (1) {
// --- First phase: compile / check-output directives
      while ((result = fgets(line, IVORY_LINE_MAX, testfile)) != NULL) {

//------ Stop directive
         if (strlen(result) >= 4 && strncmp(result, "stop", 4) == 0) {
            fclose(testfile);
            fclose(resultFile);
            return 0;
         }

//------ Skip blank lines
         if (line[0] == '\n' || line[0] == '\r')
            continue;

//------ Skip comment lines beginning with "--"
         if (startsWith(line, "--")) {
            continue;
         }

         if (startsWith(line, "compile")) {
            options[0] = '\0';
            int n = sscanf(line,
               "compile %s %s %[^\n]",
               target,
               sourceFilename,
               options);
            if (n >= 2) {
               std::string actualPath = (scriptDir && scriptDir[0])
                  ? std::string(scriptDir) + "/" + sourceFilename
                  : std::string(sourceFilename);
               fprintf(resultFile, "Compiling file %s (target=%s options=\"%s\")\n",
                  sourceFilename, target, options);
               if (!compilerSequence(target, actualPath.c_str(), options,
                  NULL, NULL)) {
                  fprintf(resultFile,
                     "File %s failed to compile\n",
                     sourceFilename);
               }
               fflush(resultFile);
               continue;
            }
         }

//------ regression directive:
//------ check-output target filename options...
//------ Typically target will be "bytes" and filename an ORDER script.
         if (startsWith(line, "check-output")) {
            options[0] = '\0';
            int n = sscanf(line,
               "check-output %s %s %[^\n]",
               target,
               sourceFilename,
               options);
            if (n >= 2) {
               std::string actualPath = (scriptDir && scriptDir[0])
                  ? std::string(scriptDir) + "/" + sourceFilename
                  : std::string(sourceFilename);
               fprintf(resultFile,
                  "Checking output for %s (target=%s options=\"%s\")\n",
                  sourceFilename, target, options);
               checkOutput(target, actualPath.c_str(), sourceFilename, options,
                  expectedDir, resultFile);
               continue;
            }
         }

//------ Break to allow second phase (apIntInt, etc.) to process it, as in
//------ Luc's original structure.
         break;
      }

//--- Second phase: legacy apIntInt tests
//--- Could be repurposed for other function-level tests.

      char functionName[IVORY_LINE_MAX];
      int inputValue;
      int outputValue;

      while ((result = fgets(line, IVORY_LINE_MAX, testfile)) != NULL) {
         int result2 = sscanf(line,
            "apIntInt %s %d -> %d",
            functionName,
            &inputValue,
            &outputValue);
         if (result2 == 0) break;
         if (result2 == EOF) break;

         char invocation[TMP_BUFFER_SIZE];
         sprintf(invocation, "apIntInt %s %d",
            functionName, inputValue);

         fprintf(resultFile, "Testing %s\n", invocation);
         fflush(resultFile);

//------ Placeholder: execScriptGetInt or similar
//------ int intResult = -1;
//------ execScriptGetInt(invocation, &intResult);

         int intResult = -1; // current stub behaviour

         fprintf(resultFile,
            "Finishing invocation with %d\n",
            intResult);

         if (intResult == outputValue) {
            fprintf(resultFile,
               "%s ---------------- SUCCESS\n",
               line);
         }
         else {
            fprintf(resultFile,
               "%s ---------------- FAILURE\n",
               line);
         }
         fflush(resultFile);
      }

      if (result == NULL)
         break;
   }

   fclose(testfile);
   fclose(resultFile);
   return 0;
}
