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
 *    unix specific definitions for use with socket interfacxe
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

#ifndef IVORY_UNIX_SOCKET_H_DEFINED
#define IVORY_UNIX_SOCKET_H_DEFINED

#include "ivory/socket_platform.h"
#include "ivory/unix.h"

class SocketInstance_unix;
class SocketDescriptorElement : public FileDescriptorElement
{
   SocketDescriptorElement::SocketDescriptorElement(void);
   virtual void onSelect(int state);

protected:
   SocketInstance_unix* _ins;

   friend class SocketInstance_unix;
};

class SocketInstance_unix : public SocketInstance
{
public:
   SocketInstance_unix(void);
   ~SocketInstance_unix(void);

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

   bool setReuseAddr(void);
   bool setNonBlocking(void);

protected:
   Mode                    _mode;
   int                     _sd;
   SocketDescriptorElement _sde;
   bool                    _connectPending;
   bool                    _closePending;

   friend class SocketDescriptorElement;
};

#endif /* IVORY_UNIX_SOCKET_H_DEFINED */
