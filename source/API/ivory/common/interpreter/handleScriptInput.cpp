/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    handleScriptInput.cpp
 *
 * Module:  Ivory common  (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 29 May 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of handleScriptInput primitive
 *
 * Method:
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

#ifndef MINIMAL_IOSTREAM
   #include <strstream>
   using namespace std;
#endif

#include "ivory/basicADS.h"
#include "ivory/object.h"
#include "ivory/socket.h"

extern int compilerSequence(const char* target, bool immFlag, bool listFlag,
                            const char* s, const char* options,
                            ostream* oStream);


declareBuiltInFn(handleScriptInput);

// handleScriptInput :: Ref -> ByteString -> Int

defineBuiltInFn(handleScriptInput,
                builtInAp2(ARROW,
                           typeCon(Ref),
                           builtInAp2(ARROW,
                                      typeCon(ByteString),
                                      typeCon(Int))))
#define data         sizeof(ByteString)
#define ref_env      (data+sizeof(Env*))
#define ref          (ref_env+sizeof(Ref))
#define stackDepth  ref
{
   checkArgs("handleScriptInput");
   jump(altEntry(handleScriptInput));
}

static const char* subString(const char* s, const char* pattern, size_t max)
{
   size_t pLen = strlen(pattern);
   if (pLen > max)
      return NULL;
   for (unsigned int i = 0; i < max - pLen + 1; i++)
      if (strncmp(s + i, pattern, pLen) == 0)
         return s + i;
   return NULL;
}

	typedef std::char_traits<char>::int_type int_type;


static Int execScript(const char* sPos, const char* ePos,
                      SocketRepr& socketRepr argN_VM)
{
   size_t sLen = (ePos - sPos) + 1;
   char* buf = (char*)consMSA->alloc(sLen + 1);
   strncpy(buf, sPos, sLen);
   buf[sLen] = '\0';

   strstream outStream;

   (void)compilerSequence("bytes", TRUE, TRUE, buf, NULL, &outStream);

   consMSA->free(buf);

   char* resStr = outStream.str();
   size_t resLen = (size_t)outStream.pcount();
   outStream.rdbuf()->freeze(false);

   ByteString bs = (ByteString)consMSA->alloc(
      sizeof(ByteString_tag) + resLen - MAX_BYTE_STRING_LENGTH);
   bs->_len = resLen;
   memcpy(bs->_data, resStr, resLen);


   socketRepr.send(bs n_vm);
   consMSA->free(bs);

   return sLen;
}

defineEntry(handleScriptInput$)
{
   const char* s        = (const char*)local(data, ByteString)->_data;
   size_t      max      = local(data, ByteString)->_len;
   const char* begPos   = subString(s, ":{", max);
   const char* endPos   = subString(s, ":}", max);
   const char* nlPos    = subString(s, "\n", max);

// Assume ref is socket   

   SocketRepr& socketRepr = objectBody(*lockObject(local(ref, Ref),
                                       *local(ref_env, BasicADS_Instance*)),
                                       SocketRepr);


   Int res;

   if (begPos != NULL)
   {
      if (begPos != s)
         res = execScript(s, begPos - 1, socketRepr n_vm);
      else
         if (endPos == NULL)
            res = 0;
         else
            res = execScript(begPos + 2, endPos - 1, socketRepr n_vm) + 4;
   }
   else
      if (nlPos != NULL)
         res = execScript(s, nlPos, socketRepr n_vm);
      else
         res = 0;

   dropStack();
   returnInt(res);
}
