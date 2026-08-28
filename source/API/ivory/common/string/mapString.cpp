/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    mapString.cpp
 *
 * Module:  Ivory common (string)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to map a string from one environment to another.
 *
* Call format:
 *
 *    res = mapString(str, srcEnv, dstEnv)
 *
 *       res      (String)       - Returned string copied to destination
 *       str      (String)       - String to map
 *       srcEnv   (const Env&)   - Source environment
 *       dstEnv   (Env&)         - Destination environment
 *
 * Method:
 *
 *    Simply returns the given string if the source and destination
 *    environments match, otherwise copies the string.
 *
 * Errors:
 *
 *    Errors are signalled by a ERROR return value
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

#include "ivory/env.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#include <ivory/memAlloc_GC.h>
#endif

#include "ivory/string.h"

#if (GARBAGE_COLLECTION==1)
 // No active roots, arbitrary association label
static BuiltInFrameDescr frameDescr(altEntry(mapString), 0, 0, NULL);
#endif

String mapString$(String str, const Env& srcEnv, Env& dstEnv) {
   if (&srcEnv == &dstEnv)
      return str;
   size_t sLen = strlen(str);

#if (GARBAGE_COLLECTION ==1)
   String res = (String)memAlloc_GC(sLen + 1, altEntry(mapString), (MSA_GC&)dstEnv.msa() n_vm);
#else
   String res = (String)dstEnv.msa().alloc(sLen + 1);
#endif

   strcpy(res, str);
   return res;
}

// mapString :: String -> Env -> String

defineBuiltInFn_2_args(mapString,
   typeCon(String), typeCon(Env), typeCon(String),
   s, String,
   env, Env)
   drop_Stack(mapString);
   returnString(mapString$(s, *sEnv, *env), env);
endBuiltInFn

