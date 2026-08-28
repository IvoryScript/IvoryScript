/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    frameDescr.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 27 August 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built-in frame descriptpor
 *
 * Notes:
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

#if (FRAME_DESCRIPTORS==1)

#include <string.h>

#include "ivory/env.h"
#include "ivory/frameDescr.h"

// Global module list

BuiltInFrameDescrSlot::BuiltInFrameDescrSlot(Label label, Name name, Type type_)
 : FrameDescrSlot<Label>(name, type_) {
   setAddress(label);
}

BuiltInFrameDescr* BuiltInFrameDescr::globalList = NULL;

BuiltInFrameDescr::BuiltInFrameDescr(Label address,
                                     size_t size,
                                     UInt nSlots, BuiltInFrameDescrSlot* slots)
 : _next(NULL) {
   _address = address;
   _size = size;
   _nSlots = nSlots;
   _slots = slots;

   _next = globalList;
   globalList = this;
}

// Post constructor initialisation

Void BuiltInFrameDescr::initGlobalList(Void) {
   BuiltInFrameDescr* frameDescr;
   while ((frameDescr = globalList) != NULL) {
      globalList = frameDescr->_next;
      builtInFrameDescrMap.add(frameDescr, reinterpret_cast<uintptr_t>(frameDescr->address()), builtInMSA());
   }
}

#endif
