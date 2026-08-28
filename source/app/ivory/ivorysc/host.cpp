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
 *    Minimal host functions for the IvoryScript compiler.
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
#ifdef unix
#include <unistd.h>
#endif
#include <time.h>

#ifndef MINIMAL_IOSTREAM
   #include <iostream>
   using namespace std;
#else
   #include "minimal_iostream.h"
   #include "minimal_strstream.h"
#endif

// some functions to be provided by the host of the ivory system

istream& inStream =  cin;
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
