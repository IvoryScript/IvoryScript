/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    SerialIO_platform.h
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
 *    Common platform related definitions for use with serial I/O.
 *
 *    SerialPortInstance is an abstract base class for
 *    platform specific serial I/O.
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
 *    14/04/03 -  AJS   Added predicate functions to X.25 SVC instance
 *
 */

#ifndef SERIAL_IO_PLATFORM_H_DEFINED
#define SERIAL_IO_PLATFORM_H_DEFINED

#include "MSA.h"

#define DEFAULT_X_25_VC_DELIVERY_CONFIRMATION_BIT  FALSE
#define DEFAULT_X_25_VC_END_TO_END                 FALSE
#define DEFAULT_X_25_VC_QUEUE_LIMIT                7

class SerialPortInstance
{
public:
   enum Parity
   {
      IVORY_PARITY_UNDEFINED,
      IVORY_PARITY_EVEN,
      IVORY_PARITY_MARK,
      IVORY_PARITY_NO,
      IVORY_PARITY_ODD,
      IVORY_PARITY_SPACE,
   };
   enum StopBits
   {
      IVORY_STOP_BITS_UNDEFINED,
      IVORY_STOP_BITS_1,
      IVORY_STOP_BITS_1_5,
      IVORY_STOP_BITS_2
   };

   virtual ~SerialPortInstance(void);

#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
   {
   }
#endif
   inline void operator delete(void* ptr)
   {
   }

   virtual bool open(const char*  name,
                     unsigned int baudRate,
                     unsigned int dataBits,
                     Parity       parity,
                     StopBits     stopBits,
                     int          handshake_RTS_CTS,
                     int          handshake_XON_XOFF) = 0;
   virtual bool close(void) = 0;
   virtual int read(unsigned char *buffer, size_t bufferLen) = 0;
   virtual int write(const unsigned char *buffer, size_t bufferLen) = 0;
   virtual bool setDTR(bool state) = 0;
   virtual bool isOpen() const = 0 ;
};

extern SerialPortInstance* makeSerialPortInstance(MSA& msa);

//------------------------------------------------------------------------------
// Abstract X.25 Service instance
//------------------------------------------------------------------------------

class X25VCInstance;
class X25ServiceInstance
{
public:
   virtual ~X25ServiceInstance(void);

#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
   {
   }
#endif
   inline void operator delete(void* ptr)
   {
   }

   virtual bool callRequest(X25VCInstance& x25VCInstance) = 0;
   virtual bool hangUp(bool immediate, X25VCInstance& x25VCInstance) = 0;
   virtual void poll(X25VCInstance& x25VCInstance) = 0;
   virtual int send(const unsigned char* buffer, size_t length,
                    X25VCInstance& x25VCInstance) = 0;
   virtual bool cancelSend(unsigned int seqId, X25VCInstance& x25VCInstance) = 0;

   void*    _adsi;
   UInt32   _ref;
};

extern X25ServiceInstance* makeX25ServiceInstance(MSA& msa);

//------------------------------------------------------------------------------
// Abstract X.25 Virtual Circuit instance
//------------------------------------------------------------------------------

class X25VCInstance
{
protected:
   X25VCInstance(const char*   localNUA,
                 const char*   remoteNUA,
                 unsigned int  port,
                 bool          endToEnd,
                 bool          deliveryConfirmationBit,
                 unsigned int  receiveBufferSize,
                 unsigned int  queueLimit)
    : _localNUA(localNUA), _remoteNUA(remoteNUA),
                _port(port),
                _endToEnd(endToEnd), _deliveryConfirmationBit(deliveryConfirmationBit),
                _receiveBufferSize(receiveBufferSize),
                _queueLimit(queueLimit)
   {
   }

public:
   virtual ~X25VCInstance(void);

#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
   {
   }
#endif
   inline void operator delete(void* ptr)
   {
   }

   virtual bool readyToCallRequest(void) const = 0;
   virtual bool callRequestInProgress(void) const = 0;
   virtual bool readyToHangUp(void) const = 0;
   virtual bool hangUpInProgress(void) const = 0;
   virtual bool readyToSend(void) const = 0;
   virtual bool sendInProgress(void) const = 0;

   void onCallCompleted(bool success);
   void onCallCleared(void);
   void onHangUpCompleted(bool success);
   void onReceived(void* buffer, size_t length);
   void onReadyToSend(void);
   void onSent(int seqId);
   void onSendFailed(unsigned int seqId);

   void*    _adsi;
   UInt32   _ref;

#if (GLOBAL_VM == 0)
   void*   _vm;
#endif
   const char*    _localNUA;
   const char*    _remoteNUA;
   unsigned int   _port;
   bool           _endToEnd;
   bool           _deliveryConfirmationBit;
   unsigned int   _receiveBufferSize;
   unsigned int   _queueLimit;
};

extern X25VCInstance* makeX25VCInstance(const char*   localNUA,
                                        const char*   remoteNUA,
                                        unsigned int  port,
                                        bool          endToEnd,
                                        bool          deliveryConfirmationBit,
                                        unsigned int  receiveBufferSize,
                                        unsigned int  queueLimit,
                                        MSA& msa);

#endif /* SERIAL_IO_PLATFORM_H_DEFINED */




