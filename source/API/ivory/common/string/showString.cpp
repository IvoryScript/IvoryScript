/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showString.cpp
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
 *    Implementation of showString function
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
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/string.h"
#include "ivory/trace.h"
#include "ivory/void.h"

// showString :: String -> Void

defineBuiltInFn_1_arg(showString,
   typeCon(String), typeCon(Void),
   s, String)
   *outStrm << (String)(s);
   size_t len = strlen((String)(s));
   if (len > 0 && ((String)(s))[len - 1] == '\n')
      outStrm->flush();
   returnVoid();
endBuiltInFn

/*----------------------------------------------------------------------------*/

// instance Show String where
//    show s = showString s

classMethodDefn_n(1, show, showString)
{
   push((String)rPtr, String);
   jump(altEntry(showString));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_String,
               Show,
               typeCon(String),
               builtInCons(methodDefn_1,
                           Nil));



