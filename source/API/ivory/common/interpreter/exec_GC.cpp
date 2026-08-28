/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    exec_GC.cpp
 *
 * Module:  Ivory common  (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 10 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Ivory byte code interpreter garbage collection.
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

#if (GARBAGE_COLLECTION==1)

// #define TRACE // Define to trace garbage collection

#include <MSA_GC.h>
#include <ivory/exec.h>
#include <ivory/exec_GC.h>
#include <ivory/iFrameDescr.h>
#include <ivory/memAlloc_GC.h>

MSAFrameDescrMapAssoc* MSAFrameDescrMapAssoc::_frameDescrMaps = NULL;

MSAFrameDescrMapAssoc* MSAFrameDescrMapAssoc::lookUp(MSA* msa) {
   MSAFrameDescrMapAssoc* assoc = _frameDescrMaps;
   while (assoc != NULL) {
      if (msa == assoc->_msa)
         return assoc;
      assoc = assoc->_next;
   }
   return NULL;
}

MSAFrameDescrMapAssoc* MSAFrameDescrMapAssoc::add(MSA* msa) {
   _frameDescrMaps = new(*msa) MSAFrameDescrMapAssoc(_frameDescrMaps, msa);
   return _frameDescrMaps;
}

// Remove any map associated with a given MSA

Void MSAFrameDescrMapAssoc::remove(MSA* msa) {
   MSAFrameDescrMapAssoc** assocPtr = &_frameDescrMaps;
   while (*assocPtr != NULL) {
      if (msa == (*assocPtr)->_msa) {
         *assocPtr = (*assocPtr)->_next;
         return;
      }
      assocPtr = &(*assocPtr)->_next;
   }
}

#ifdef TRACE
// Mark variables for given frame

UInt markCount = 0;
#endif

Void execMarkFrame_GC(IAddress pc argN_VM) {

// Associated stack frame descriptor

   MSAFrameDescrMapAssoc* mapAssoc = MSAFrameDescrMapAssoc::frameDescrMaps();
   FrameDescr<IAddress>* frameDescr = NULL;
   while (mapAssoc != NULL) {
      frameDescr = mapAssoc->frameDescrMap().lookUp(pc, reinterpret_cast<uintptr_t>(pc));
      if (frameDescr != NULL)
         break;
      mapAssoc = mapAssoc->next();
   }

   if (frameDescr != NULL) {

#ifdef TRACE
      outStream << "*** GC for label: " << frameDescr->labSeqNo() << endl;
#endif


      nextFrame(frameDescr->size());

#ifdef TRACE
      outStream << "*** Marking interpreted frame with " << frameDescr->nSlots() << " slots: ";
#endif

      for (UInt i = 0; i < frameDescr->nSlots(); i++) {
         const FrameDescrSlot<IAddress>& slot = frameDescr->slot(i);

#ifdef TRACE
         outStream << "*** Marking slot " << i + 1 << " of " << frameDescr->nSlots() << " (" << ++markCount << ") : ";
         if (i > 0)
            outStream << " ";
#endif

         StackPtr sp_ = sp;

         pushLabel(NULL);
         callIContinuation(slot.address() n_vm);

#ifdef TRACE
         outStream << "*** Marked slot" << endl;
#endif

         if (sp_ != sp)
            throw(RunTimeError("Garbage collection stack error"));
      }
   } else {
     // throw(RunTimeError("Garbage collection frame descriptor missing"));

      outStream << "*** GC for continuation byte code label " << (Void*)pc << endl; //???
   }
#ifdef TRACE 
   outStream << endl;
#endif
}

Void execMarkContinuations_GC(MSA& msa argN_VM) {
   IAddress contAddress = frame(-((Int)sizeof(Label)), IAddress);
   execMarkFrame_GC(contAddress n_vm);
}

// Invoke garbage collection for given code point and memory allocator

static Void invokeGC(IAddress pc, MSA& msa argN_VM) {

#ifdef TRACE
   outStream << "*** Marking live variables" << endl;
#endif

//--- Set current stack frame pointer and mark variables and continuations

   fp = sp;

   execMarkFrame_GC(pc n_vm);

   markContinuations_GC(msa n_vm);

   MSA_GC& msa_GC = dynamic_cast<MSA_GC&>(msa);
   msa_GC.sweep();

#ifdef TRACE
      outStream << "Done." << endl;
#endif

}

extern Void traceInterpreter(Bool flag);
 

Void* execMemAlloc_GC(size_t reqLen, IAddress pc, MSA_GC& msa argN_VM) {
   if (msa.isEnabled()) {
      msa.setEnableAddBlk(0);
      Void* addr = msa.allocGC(reqLen);
      msa.setEnableAddBlk(1);
      if (addr == NULL) {
//       traceInterpreter(TRUE);
         invokeGC(pc, msa n_vm);
//       traceInterpreter(FALSE);
         addr = msa.allocGC(reqLen);
      }
      return addr;
   } else
      return msa.allocGC(reqLen);
}

#endif
