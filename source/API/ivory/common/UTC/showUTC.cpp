/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showUTC.cpp
 *
 * Module:  Ivory common (UTC)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of showUTC function
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

#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/trace.h"
#include "ivory/UTC.h"
#include "ivory/void.h"

#define TIME_FORMAT "%d/%m/%y %H:%M:%S"

// showUTC :: UTC -> Void

defineBuiltInFn_1_arg(showUTC,
   typeCon(UTC), typeCon(Void),
   utc, UTC)
   struct tm* _tm = gmtime(&utc);
   char buf[256];

   if (_tm != NULL)
      strftime(buf, 256, TIME_FORMAT, _tm);
   else
      *buf = '\0';
   *outStrm << buf;
   returnVoid();
endBuiltInFn

/*----------------------------------------------------------------------------*/

// instance Show UTC where
//    show utc = showUTC utc

classMethodDefn_n(1, show, showUTC) {
   push(rInt, UTC);
   jump(altEntry(showUTC));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_UTC,
               Show,
               typeCon(UTC),
               builtInCons(methodDefn_1,
                           Nil));
