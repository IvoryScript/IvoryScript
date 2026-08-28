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
 *    Definitions for use with Serial I/O
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

#ifndef IVORY_SERIAL_IO_H_DEFINED
#define IVORY_SERIAL_IO_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/byteString.h"
#include "ivory/class.h"
#include "ivory/propertySet.h"
#include "ivory/serialIO_platform.h"

declareNullaryType(Parity);

declareInstance(Show_Parity);

declareNullaryDataCon(EvenParity);
declareNullaryDataCon(MarkParity);
declareNullaryDataCon(NoParity);
declareNullaryDataCon(OddParity);
declareNullaryDataCon(SpaceParity);

#define checkParity()checkParity$(vm)
#define updateParity()\
if(updatePtr!=NULL){updatePtr->tag()=cellInfoTable$Parity[rTag];updatePtr=NULL;}

declareBuiltInFn(showParity);

/*----------------------------------------------------------------------------*/

declareNullaryType(StopBits);

declareInstance(Show_StopBits);

declareNullaryDataCon(StopBits);
declareNullaryDataCon(StopBits_1);
declareNullaryDataCon(StopBits_1_5);
declareNullaryDataCon(StopBits_2);

#define checkStopBits()checkStopBits$(vm)
#define updateStopBits()\
if(updatePtr!=NULL){updatePtr->tag()=cellInfoTable$StopBits[rTag];updatePtr=NULL;}

declareBuiltInFn(showStopBits);

/*----------------------------------------------------------------------------*/

declareTypeCon(SerialPort);

declareType(SerialPort);

declareType(SerialPortRepr);

declareDataCon(SerialPort);

declareInstance(PropertySetBasedObject_SerialPort);

class SerialPortRepr : public PropertySet
{
public:
   SerialPortRepr(Void)
      : PropertySet(), _ins(NULL)
   {
   }
   SerialPortRepr(const SerialPortRepr& src, const Env& srcEnv, Env& env, MSA& msa);
   void SerialPortRepr$(argVM);
   Void destroy(Env& env, MSA& msa);

   Bool open(const Env& env argN_VM);
   ByteString read(MSA& msa);
   Bool close(Void);
   Bool isOpen(Void) const;
   Bool setDTR(Bool state);
   Int write(const ByteString data);

protected:
   SerialPortInstance*   _ins;

   friend Label entry_dtrSerialPort$(argVM);
   friend Label entry_writeSerialPort$(argVM);
   friend Void insSerialPort(ADSRef& adsr argN_VM);
   friend Void desSerialPort(ADSRef& adsr);
};

declareBuiltInFn(depleteInput);

declareBuiltInFn(selectSerialPort);

declareBuiltInFn(monitorSerialPort);

declareBuiltInFn(pollSerial);

extern void logSerialOutput(const unsigned char* data, unsigned int len);

extern void logSerialInput(const unsigned char* data, unsigned int len);

/*----------------------------------------------------------------------------*/

declareTypeCon(X25Service);

declareType(X25Service);

declareDataCon(X25Service);

declareInstance(PropertySetBasedObject_X25Service);

class X25ServiceRepr : public PropertySet
{
public:
   X25ServiceRepr(Void)
      : PropertySet(), _ins(NULL)
   {
   }
   X25ServiceRepr(const X25ServiceRepr& src, const Env& srcEnv, Env& env, MSA& msa);
   void X25ServiceRepr$(argVM);
   Void destroy(Env& env, MSA& msa);

protected:
   X25ServiceInstance*   _ins;

   friend class X25VCRepr;

   friend Void insX25Service(ADSRef& adsr argN_VM);
   friend Void desX25Service(ADSRef& adsr);
};

declareBuiltInFn(selectX25Service);

declareBuiltInFn(monitorX25);

/*----------------------------------------------------------------------------*/

declareTypeCon(X25VC);

declareType(X25VC);

declareDataCon(X25VC);

declareInstance(PropertySetBasedObject_X25VC);

class X25VCRepr : public PropertySet
{
public:
   X25VCRepr(Void)
      : PropertySet(), _ins(NULL)
   {
   }
   X25VCRepr(const X25VCRepr& src, const Env& srcEnv, Env& env, MSA& msa);
   void X25VCRepr$(argVM);
   Void destroy(Env& env, MSA& msa);

   Bool callRequest(const Env& env argN_VM);
   Bool hangUp(const Env& env argN_VM);
   Void poll(const Env& env argN_VM);
   Int send(const ByteString data, const Env& env argN_VM);
   Bool cancelSend(unsigned int seqId, const Env& env argN_VM);

   Bool readyToCallRequest(argVM) const;
   Bool callRequestInProgress(argVM) const;
   Bool readyToHangUp(argVM) const;
   Bool hangUpInProgress(argVM) const;
   Bool readyToSend(argVM) const;
   Bool sendInProgress(argVM) const;

protected:
   X25VCInstance*   _ins;

   friend Void insX25VC(ADSRef& adsr argN_VM);
   friend Void desX25VC(ADSRef& adsr);
};

declareBuiltInFn(selectX25VC);

declareBuiltInFn(pollX25VC);

#endif /* IVORY_SERIAL_IO_H_DEFINED */
