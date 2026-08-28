/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    unix.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 July 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common platform related definitions for unix/
 *
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
 *
 */

#ifndef IVORY_UNIX_H_DEFINED
#define IVORY_UNIX_H_DEFINED

#include "MSA.h"

// Virtual class for elements of the file descriptor list

class FileDescriptorElement
{
public:
   enum State
   {
      READ = 1,
      WRITE = 2,
      EXCEPT = 4,
   };
public:
   void toList(int fd);
   void fromList(void);
   virtual void onSelect(int state) = 0;

protected:
   FileDescriptorElement*  _next;
   FileDescriptorElement*  _prev;
   int _fd;
   bool _read;
   bool _write;
   bool _except;

   friend void main_unix(void);
};

extern FileDescriptorElement* fileDescriptorList;

extern void main_unix(void);

#endif /* IVORY_UNIX_H_DEFINED */
