/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    Socket_platform.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 7 November 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common platform related definitions for use with sockets.
 *
 *    SocketInstance is an abstract base class for
 *    platform specific socket interface.
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

#ifndef SOCKET_PLATFORM_H_DEFINED
#define SOCKET_PLATFORM_H_DEFINED

#include "MSA.h"

class SocketInstance {
public:
   enum Mode {
      LISTENER,
      ACCEPED,
      CONNECT,
   };

   virtual ~SocketInstance(void);

#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
   {
   }
#endif
   inline void operator delete(void* ptr)
   {
   }

   void onAccept(void);
   void onConnected(bool success);
   void onReceive(void);
   bool onClose(void);

   virtual bool init(enum SocketInstance::Mode, unsigned int port) = 0;
   virtual bool accept(SocketInstance& instance) = 0;
   virtual bool close(void) = 0;
   virtual bool connect(const char* host, int port) = 0;
   virtual int receive(unsigned char* buffer, size_t bufferLen) = 0;
   virtual int send(unsigned char* buffer, size_t bufferLen) = 0;

   virtual const char* peerName(MSA& msa) = 0;

   virtual bool readyToConnect(void) = 0;
   virtual bool readyToClose(void) = 0;
   virtual bool readyToSend(void) = 0;

   void*    _adsi;
   UInt32   _ref;

#if (GLOBAL_VM == 0)
   void*   _vm;
#endif
};

extern SocketInstance* makeSocketInstance(MSA& msa);

#endif /* SOCKET_PLATFORM_H_DEFINED */




