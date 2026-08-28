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
 * Original date: 29 May 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Sockets.
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

#ifndef IVORY_SOCKET_H_DEFINED
#define IVORY_SOCKET_H_DEFINED

#include "ivory/byteString.h"
#include "ivory/class.h"
#include "ivory/propertySet.h"
#include "ivory/socket_platform.h"

declareTypeCon(SocketMode);

declareType(SocketMode);

declareInstance(Show_SocketMode);

extern Tag checkSocketMode$(argVM);
#define checkSocketMode() checkSocketMode$(vm)

extern CellInfo* cellInfoTable_SocketMode[];

#define updateSocketMode()\
if(updatePtr!=NULL){updatePtr->tag()=cellInfoTable_SocketMode[rTag];updatePtr=NULL;}

declareBuiltInFn(showSocketMode);

#define LISTENER_TAG  0
#define ACCEPTED_TAG  1
#define CONNECT_TAG   2

declareNullaryDataCon(Listener);
declareNullaryDataCon(Accepted);
declareNullaryDataCon(Connect);


declareTypeCon(Socket);

declareType(Socket);

declareType(SocketRepr);

declareDataCon(Socket);

declareInstance(PropertySetBasedObject_Socket);

class SocketRepr : public PropertySet
{
public:
   SocketRepr(Void)
      : PropertySet(), _ins(NULL)
   {
   }
   SocketRepr(const SocketRepr& src, const Env& srcEnv, Env& env, MSA& msa);
   void SocketRepr$(argVM);
   Void destroy(Env& env, MSA& msa);

   Bool accept(SocketRepr& ins) const;
   Bool close(Void) const;
   Bool connect(const Env& env argN_VM) const;
   ByteString receive(MSA& msa argN_VM) const;
   Int send(const ByteString data argN_VM) const;

   const char* peerName(MSA& msa) const;

   Bool readyToConnect(argVM) const;
   Bool readyToClose(argVM) const;
   Bool readyToSend(argVM) const;

protected:
   SocketInstance*   _ins;
   Type              _sendType;
   Cell*             _sendClosure;

   friend Void insSocket(ADSRef& adsr argN_VM);
   friend Void desSocket(ADSRef& adsr);
   friend Label selectSocket$altEntry(argVM);
};

declareBuiltInFn(acceptSocket);

declareBuiltInFn(closeSocket);

declareBuiltInFn(monitorSocket);

declareBuiltInFn(monitorAcceptedSocket);

declareBuiltInFn(selectSocket);

declareBuiltInFn(showReceived);

#endif /* IVORY_SOCKET_H_DEFINED */




