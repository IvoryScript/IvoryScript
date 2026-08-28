/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    socket.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 31 January 2003
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    WinCE specific definitions for use with socket interface.
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

#ifndef IVORY_WINCE_SOCKET_H_DEFINED
#define IVORY_WINCE_SOCKET_H_DEFINED

#define WINCE_SOCKET_TIMER 999999
//#define WINCE_SOCKET_PERIOD 50
#define WINCE_SOCKET_PERIOD 50

#include <afxsock.h>		// MFC socket extensions

#include "ivory\socket_platform.h"

void pollSockets(void);

class WinCESocketInstance;

class WinCESocket : public CAsyncSocket
{
   WinCESocket(void);
   ~WinCESocket(void);

   bool WinCESocket::Connect(const char* host, int port);

protected:
   void OnAccept(int nErrorCode);
   void OnConnect(int nErrorCode);
   void OnClose(int nErrorCode);
   void OnReceive(int nErrorCode);

   bool                 _connected;

public:
   WinCESocket*   _next;
   WinCESocketInstance* _ins;

   void WinCESocket::AddToList(void);
   void WinCESocket::DropFromList(void);

   void WinCESocket::PollForReceive(void);

   friend class WinCESocketInstance;
};

class WinCESocketInstance : public SocketInstance
{
public:
   WinCESocketInstance(void);
   ~WinCESocketInstance(void);

   inline void* operator new(size_t size, MSA& msa)
   {
      void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr, MSA& msa)
   {
      msa.free(ptr);
   }
#endif
   inline void operator delete(void* ptr)
   {
   }

   bool init(enum SocketInstance::Mode, unsigned int port);
   bool accept(SocketInstance& instance);
   bool close(void);
   bool connect(const char* host, int port);
   int receive(unsigned char *buffer, size_t bufferLen);
   int send(unsigned char *buffer, size_t bufferLen);

   bool readyToConnect(void);
   bool readyToClose(void);
   bool readyToSend(void);

   bool _closePending;

protected:
   WinCESocket   _socket;
   Mode _mode;

   friend class WinCESocket;
};

#endif /* IVORY_WINCE_SOCKET_H_DEFINED */
