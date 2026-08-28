/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    host.cpp
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


#include <stdio.h>
#ifdef unix // AJS
#include <unistd.h>
#endif  // AJS
#include <time.h>

#ifndef MINIMAL_IOSTREAM
   #include <sstream>
   #include <iostream>
using namespace std;
#else
   #include "minimal_iostream.h"
   #include "minimal_strstream.h"
#endif

istream& inStream = cin;
ostream& outStream = cout;

void error(const char* s) {
   outStream << s << '\n';
   outStream.flush();
}

void trace(const char* s) {
   time_t        _time;
   struct tm *   _tm;
   char          _buf[128];

   time( &_time );
 
   _tm = localtime( &_time );

   strftime(_buf, 128, "%H:%M:%S %d/%m/%Y", _tm);
   outStream << _buf << ": ";
   outStream << s << '\n';
   outStream.flush();
}

void outbit(const char* s) {
   outStream << s;
}

void outnew() {
   outStream <<'\n';
   outStream.flush();
}
