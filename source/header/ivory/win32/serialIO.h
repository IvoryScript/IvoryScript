/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    serialIO.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 29 May 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Win32 specific definitions for use with serial I/O types
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

#ifndef IVORY_WIN32_SERIAL_IO_H_DEFINED
#define IVORY_WIN32_SERIAL_IO_H_DEFINED

#include "windows.h"
#include "ivory\serialIO_platform.h"

class Win32SerialPortInstance : public SerialPortInstance
{
public:
   Win32SerialPortInstance(void);
   ~Win32SerialPortInstance(void);

   inline void* operator new(size_t size, void* ptr)
   {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
   {
   }
#endif
   inline void operator delete(void* ptr)
   {
   }
   
   bool open(const char*  name,
             unsigned int baudRate,
             unsigned int dataBits,
             Parity       parity,
             StopBits     stopBits,
             int          handshake_RTS_CTS,
             int          handshake_XON_XOFF);
   bool close(void);
   int read(unsigned char *buffer, size_t bufferLen);
   int write(const unsigned char *buffer, size_t bufferLen);
   bool setDTR(bool state);
   bool isOpen() const;


protected:
   HANDLE   _handle;
   Parity   _parity;
   bool      _softParity;

};

#endif /* IVORY_WIN32_SERIAL_IO_H_DEFINED */




