/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    comms.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 29 May 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with comms module.
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
 */

#ifndef IVORY_COMMS_H_DEFINED
#define IVORY_COMMS_H_DEFINED

#include "ivory/builtInFn.h"

#include "ivory/class.h"
#include "ivory/propertySet.h"
#include "ivory/ref.h"

#define NOT_IN_CONTACT_TAG          0
#define REQUESTING_CONTACT_ACK_TAG  1
#define CONTACT_REQUEST_FAILED_TAG  2
#define CONTACT_REQUEST_TIMEOUT_TAG 3
#define WAIT_CONTACT_ACK_TAG        4
#define IN_CONTACT_TAG              5

typedef UInt ContactState;

declareTypeCon(ContactState);

declareType(ContactState);

declareInstance(Show_ContactState);

declareAtomicDataCon(NotInContact);
declareAtomicDataCon(RequestingContactACK);
declareAtomicDataCon(ContactRequestFailed);
declareAtomicDataCon(ContactRequestTimeout);
declareAtomicDataCon(WaitContactACK);
declareAtomicDataCon(InContact);

extern Tag checkContactState$(argVM);
#define checkContactState() checkContactState$(vm)

extern CellInfo* cellInfoTable_ContactState[];

declareBuiltInFn(showContactState);

#define updateContactState()\
if(updatePtr!=NULL){updatePtr->_tag=cellInfoTable_ContactState[rTag];updatePtr=NULL;}

extern Expr fromContactState(ContactState contactState);

/*----------------------------------------------------------------------------*/

#define IP_ADDRESS_TAG              0
#define ACCESS_NUMBER_TAG           1

typedef UInt AddressType;

declareTypeCon(AddressType);

declareType(AddressType);

declareInstance(Show_AddressType);

declareAtomicDataCon(IPAddress);
declareAtomicDataCon(AccessNumber);

extern Tag checkAddressType$(argVM);
#define checkAddressType() checkAddressType$(vm)

extern CellInfo* cellInfoTable_AddressType[];

#define updateAddressType()\
if(updatePtr!=NULL){updatePtr->_tag=cellInfoTable_AddressType[rTag];updatePtr=NULL;}

declareBuiltInFn(showAddressType);

extern Expr fromAddressType(AddressType addressType);

/*----------------------------------------------------------------------------*/

extern Expr mkAnyAddress(void);

declareTypeCon(AnyAddress);

declareType(AnyAddress);

declareInstance(Show_AnyAddress);

extern Expr builtIn_showAnyAddress;
declareEntry(f);declareEntry(showAnyAddress$);

declareDataCon(AnyAddress);

#define AnyAddress dataCon(AnyAddress)

#define returnAnyAddress(){\
rType = type(AnyAddress);\
jump(popLabel());}

extern Void checkAnyAddress$(argVM);
#define checkAnyAddress() checkAnyAddress$(vm)

extern CellInfo cellInfo$AnyAddress;

#define updateAnyAddress()\
if(updatePtr!=NULL){updatePtr->_tag=&cellInfo$AnyAddress;updatePtr=NULL;}

declareBuiltInFn(showAnyAddress);

/*----------------------------------------------------------------------------*/

#define GPRS_TAG           0
#define INTER_PROCESS_TAG  1
#define LAN_TAG            2
#define WIFI_TAG		      3

typedef UInt ConnectionType;

declareTypeCon(ConnectionType);

declareType(ConnectionType);

declareInstance(Show_ConnectionType);

declareAtomicDataCon(GPRS);
declareAtomicDataCon(InterProcess);
declareAtomicDataCon(LAN);
declareAtomicDataCon(WIFI);

extern Tag checkConnectionType$(argVM);
#define checkConnectionType() checkConnectionType$(vm)

extern CellInfo* cellInfoTable_ConnectionType[];

#define updateConnectionType()\
if(updatePtr!=NULL){updatePtr->_tag=cellInfoTable_ConnectionType[rTag];updatePtr=NULL;}

declareBuiltInFn(showConnectionType);

extern Expr fromConnectionType(ConnectionType connectionType);

/*----------------------------------------------------------------------------*/

declareTypeCon(ReadyToSendEvent);

declareType(ReadyToSendEvent);

declareDataCon(ReadyToSendEvent);

class ReadyToSendEvent
{
public:
   ReadyToSendEvent(const ADSRef& adsr);
   ReadyToSendEvent(const ReadyToSendEvent& src, const Env& srcEnv, Env& dstEnv, MSA& msa);

   inline
   void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& adsr()
   {
      return _adsr;
   }
   inline Type type_()
   {
      return _type;
   }

protected:
   ADSRef   _adsr;
   Type     _type;
};

extern ReadyToSendEvent& checkReadyToSendEvent$(argVM);
#define checkReadyToSendEvent() checkReadyToSendEvent$(vm)

#define updateReadyToSendEvent()\
if(updatePtr!=NULL){update_ReadyToSendEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(ProcessMessageEvent);

declareType(ProcessMessageEvent);

declareDataCon(ProcessMessageEvent);

class ProcessMessageEvent
{
public:
   ProcessMessageEvent(const ADSRef& adsr);
   ProcessMessageEvent(const ProcessMessageEvent& src, const Env& srcEnv, Env& dstEnv, MSA& msa);

   inline
   void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& adsr()
   {
      return _adsr;
   }
   inline Type type_()
   {
      return _type;
   }

protected:
   ADSRef   _adsr;
   Type     _type;
};

extern ProcessMessageEvent& checkProcessMessageEvent$(argVM);
#define checkProcessMessageEvent() checkProcessMessageEvent$(vm)

#define updateProcessMessageEvent()\
if(updatePtr!=NULL){update_ProcessMessageEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(Account);

declareType(Account);

declareInstance(PropertySetBasedObject_Account);

declareDataCon(Account);

/*----------------------------------------------------------------------------*/

declareTypeCon(AccountSet);

declareType(AccountSet);

declareInstance(PropertySetBasedObject_AccountSet);

declareDataCon(AccountSet);

/*----------------------------------------------------------------------------*/

declareTypeCon(AddressMapping);

declareType(AddressMapping);

declareInstance(PropertySetBasedObject_AddressMapping);

declareDataCon(AddressMapping);

/*----------------------------------------------------------------------------*/

declareTypeCon(AddressMap);

declareType(AddressMap);

declareInstance(PropertySetBasedObject_AddressMap);

declareDataCon(AddressMap);

/*----------------------------------------------------------------------------*/

declareTypeCon(BackOff);

declareType(BackOff);

declareInstance(PropertySetBasedObject_BackOff);

declareDataCon(BackOff);

/*----------------------------------------------------------------------------*/

declareTypeCon(BackOffSet);

declareType(BackOffSet);

declareInstance(PropertySetBasedObject_BackOffSet);

declareDataCon(BackOffSet);

/*----------------------------------------------------------------------------*/

declareTypeCon(DataGenerator);

declareType(DataGenerator);

declareInstance(PropertySetBasedObject_DataGenerator);

declareDataCon(DataGenerator);

/*----------------------------------------------------------------------------*/

declareTypeCon(DataGeneratorGroup);

declareType(DataGeneratorGroup);

declareInstance(PropertySetBasedObject_DataGeneratorGroup);

declareDataCon(DataGeneratorGroup);

/*----------------------------------------------------------------------------*/

declareTypeCon(MessageField);

declareType(MessageField);

declareInstance(PropertySetBasedObject_MessageField);

declareDataCon(MessageField);

/*----------------------------------------------------------------------------*/

declareTypeCon(MessageTemplate);

declareType(MessageTemplate);

declareInstance(PropertySetBasedObject_MessageTemplate);

declareDataCon(MessageTemplate);

/*----------------------------------------------------------------------------*/

declareTypeCon(RetryStage);

declareType(RetryStage);

declareInstance(PropertySetBasedObject_RetryStage);

declareDataCon(RetryStage);

/*----------------------------------------------------------------------------*/

declareTypeCon(RetryStageSet);

declareType(RetryStageSet);

declareInstance(PropertySetBasedObject_RetryStageSet);

declareDataCon(RetryStageSet);

/*----------------------------------------------------------------------------*/

declareTypeCon(ContactWatch);

declareType(ContactWatch);

declareInstance(PropertySetBasedObject_ContactWatch);

declareDataCon(ContactWatch);

/*----------------------------------------------------------------------------*/


declareTypeCon(ContactWatchSet);

declareType(ContactWatchSet);

declareInstance(PropertySetBasedObject_ContactWatchSet);

declareDataCon(ContactWatchSet);

/*----------------------------------------------------------------------------*/

declareBuiltInFn(addRoute);

declareBuiltInFn(fieldText);

declareBuiltInFn(genData);

declareBuiltInFn(getRoute);

declareBuiltInFn(compareAddress);

declareBuiltInFn(lookUpBackOff);

declareBuiltInFn(lookUpContactWatch);

declareBuiltInFn(lookUpRoute);

declareBuiltInFn(messageText);

declareBuiltInFn(monitorBackOff);

declareBuiltInFn(monitorBackOffTimeouts);

declareBuiltInFn(monitorContactWatch);

declareBuiltInFn(monitorContactWatchBackOff);

declareBuiltInFn(monitorContactWatchRoute);

declareBuiltInFn(monitorContactWatchTimeouts);

declareBuiltInFn(monitorPacketStateContactWatch);

declareBuiltInFn(monitorInboundPacketBackOff);

declareBuiltInFn(monitorInboundPacketRoute);

declareBuiltInFn(monitorOutboundPacket);

declareBuiltInFn(monitorPacketBackOff);

declareBuiltInFn(monitorPacketGroupLimit);

declareBuiltInFn(monitorPacketStateBackOff);

declareBuiltInFn(monitorRoute);

declareBuiltInFn(routeGPRS);

declareBuiltInFn(routeTrue);

declareBuiltInFn(routeType);

declareBuiltInFn(setRoute);

#endif /* IVORY_COMMS_H_DEFINED */
