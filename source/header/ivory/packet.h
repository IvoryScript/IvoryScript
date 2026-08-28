/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    packet.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 29 May 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Packet types
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

#ifndef IVORY_PACKET_H_DEFINED
#define IVORY_PACKET_H_DEFINED

#include "ivory/class.h"
#include "ivory/propertySet.h"

#define PKT_READY_TAG               0
#define PKT_READY_TIMEOUT_TAG       1
#define PKT_SEND_TAG                2
#define PKT_SEND_TIMEOUT_TAG        3
#define PKT_SENT_TAG                4
#define PKT_FAILED_TO_SEND_TAG      5  
#define PKT_SEND_ERROR_TAG          6
#define PKT_IN_MAILBOX_TAG          7
#define PKT_COMPLETED_TAG           8
#define PKT_DELIVERED_TAG           9
#define PKT_FAILED_TO_DELIVER_TAG   10
#define PKT_DELIVERY_ERROR_TAG      11
#define PKT_BACK_OFF_TAG            12
#define PKT_TIMEOUT_TAG             13
#define PKT_LIMIT_EXCEEDED_TAG      14

typedef Tag PacketState;

declareTypeCon(PacketState);

declareType(PacketState);

declareInstance(Show_PacketState);

declareAtomicDataCon(Pkt_Ready);
declareAtomicDataCon(Pkt_ReadyTimeout);
declareAtomicDataCon(Pkt_Send);
declareAtomicDataCon(Pkt_SendTimeout);
declareAtomicDataCon(Pkt_Sent);
declareAtomicDataCon(Pkt_FailedToSend);
declareAtomicDataCon(Pkt_SendError);
declareAtomicDataCon(Pkt_InMailbox);
declareAtomicDataCon(Pkt_Completed);
declareAtomicDataCon(Pkt_Delivered);
declareAtomicDataCon(Pkt_FailedToDeliver);
declareAtomicDataCon(Pkt_DeliveryError);
declareAtomicDataCon(Pkt_BackOff);
declareAtomicDataCon(Pkt_Timeout);
declareAtomicDataCon(Pkt_LimitExceeded);

extern Tag checkPacketState$(argVM);
#define checkPacketState() checkPacketState$(vm)

extern CellInfo* cellInfoTable_PacketState[];

#define updatePacketState()\
if(updatePtr!=NULL){updatePtr->_tag=cellInfoTable_PacketState[rTag];updatePtr=NULL;}

declareBuiltInFn(showPacketState);

/*----------------------------------------------------------------------------*/

declareTypeCon(Packet);

declareType(Packet);

declareInstance(PropertySetBasedObject_Packet);

declareDataCon(Packet);

/*----------------------------------------------------------------------------*/

declareTypeCon(PacketGroup);

declareType(PacketGroup);

declareInstance(PropertySetBasedObject_PacketGroup);

declareDataCon(PacketGroup);

/*----------------------------------------------------------------------------*/

declareBuiltInFn(monitorOutboundPacketTimeouts);

declareBuiltInFn(monitorPacket);

declareBuiltInFn(monitorPacketStateBasic);

extern Expr fromPacketState(PacketState packetState);

#endif /* IVORY_PACKET_H_DEFINED */
