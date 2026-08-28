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
 * Original date: 7 November 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Win32 specific definitions for use with socket interfacxe
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

#ifndef IVORY_WIN32_SOCKET_H_DEFINED
#define IVORY_WIN32_SOCKET_H_DEFINED

#include <afxsock.h>		// MFC socket extensions

#include "ivory\socket_platform.h"

class Win32SocketInstance;

class Win32Socket : public CAsyncSocket
{
   Win32Socket(void);  
   ~Win32Socket(void);  
 
   bool Win32Socket::Connect(const char* host, int port);
   void Win32Socket::Close();

protected:
   void OnAccept(int nErrorCode);
   void OnConnect(int nErrorCode);
   void OnReceive(int nErrorCode);
   void OnClose(int nErrorCode);

   Win32SocketInstance* _ins;
   bool                 _connectPending;
   bool                 _closePending;

   friend class Win32SocketInstance;
};

class Win32SocketInstance : public SocketInstance
{
public:
   Win32SocketInstance(void);

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

   const char* peerName(MSA& msa);

   bool readyToConnect(void);
   bool readyToClose(void);
   bool readyToSend(void);

protected:
   Win32Socket _socket;
};

#endif /* IVORY_WIN32_SOCKET_H_DEFINED */




