/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    memAlloc_GC.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Ivory garbage collection.
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

//#define TRACE // Define to trace garbage collection

#if (INTERPRETER==1)
#include "ivory/exec_GC.h"
#endif

#include "ivory/frameDescr.h"
#include "ivory/machine.h"
#include "ivory/memAlloc_GC.h"

//#ifdef TRACE ???
#include "ivory/trace.h"
//#endif

FrameDescrTable<Label> builtInFrameDescrMap(N_FRAME_DESCR_TABLE_SLOTS);

#ifdef TRACE

// Mark variables for given frame

//UInt markCount = 0;
#endif

static Void markFrame_GC(const FrameDescr<Label>& frameDescr argN_VM) {

#ifdef TRACE
   outStream << "*** Marking frame with " << frameDescr.nSlots() << " slots: ";
#endif

   for (UInt i = 0; i < frameDescr.nSlots(); i++) {
      const FrameDescrSlot<Label>& slot = frameDescr.slot(i);

#ifdef TRACE
      //      outStream << "*** Marking slot " << i + 1 << " of " << frameDescr.nSlots() << " (" << ++markCount << ") : ";
      if (i > 0)
         outStream << " ";
      printName(slot.name(), outStream, *slot.env());
#endif

      StackPtr sp_ = sp;

      pushLabel(NULL);
      callSequence$(slot.address() n_vm);

      if (sp_ != sp)
         throw(RunTimeError("Garbage collection stack error"));
   }

#ifdef TRACE 
   outStream << endl;
#endif
}

// Invoke garbage collection for given code point and memory allocator

#if (INTERPRETER==1)
declareEntry(iContinuation);
#endif

Void markContinuations_GC(MSA& msa argN_VM) {

#ifdef TRACE
   outStream << "*** Marking continuation frames" << endl;
#endif


#ifdef TRACE
      UInt n = 0;
#endif

//--- Loop over continuation frames and mark

   do {
      Label contLabel = frame(0, Label);
      if (contLabel == NULL)
         break;

#if (INTERPRETER==1)
      if (contLabel != entry(iContinuation)) {
#endif

         FrameDescr<Label>* frameDescr = builtInFrameDescrMap.lookUp(contLabel, reinterpret_cast<uintptr_t>(contLabel));

         if (frameDescr == NULL)
            throw(RunTimeError("Garbage collection frame descriptor missing"));

//#ifdef TRACE            
 //        outStream << "*** GC for continuation label " << frameDescr->labSeqNo() << " " << static_cast<Void*>(contLabel) << endl;
 //        outStream << "*** GC for continuation label " << static_cast<Void*>(contLabel) << endl;
//#endif

         nextFrame(frameDescr->size());
         markFrame_GC(*frameDescr);

#if (INTERPRETER==1)
      }
      else
         execMarkContinuations_GC(msa n_vm);
#endif

#ifdef TRACE
      n++;
#endif

   } while (TRUE);

#ifdef TRACE
   outStream << "*** " << n << " frames marked." << endl;
#endif

}

// Invoke garbage collection for given code point and memory allocator

static Void invokeGC(Label label, MSA& msa argN_VM) {

#ifdef TRACE
   outStream << "*** Marking live variables" << endl;
#endif

// Associated stack frame descriptor

   FrameDescr<Label>* frameDescr = builtInFrameDescrMap.lookUp(label, reinterpret_cast<uintptr_t>(label));

   if (frameDescr != NULL) {

#ifdef TRACE
      outStream << "*** GC for label: " << frameDescr->labSeqNo() << endl;
#endif

//--- Set current stack frame pointer and mark variables

      fp = sp;
      nextFrame(frameDescr->size());
      markFrame_GC(*frameDescr argN_VM);

      markContinuations_GC(msa n_vm);

      MSA_GC& msa_GC = dynamic_cast<MSA_GC&>(msa);

      msa_GC.sweep();

#ifdef TRACE
      outStream << "Done." << endl;
#endif

   }
}
 
Void* memAlloc_GC(size_t reqLen, Label label, MSA_GC& msa argN_VM) {
   msa.setEnableAddBlk(0);
   Void* addr = msa.allocGC(reqLen);
   msa.setEnableAddBlk(1);
   if (addr == NULL) {
      if (msa.isEnabled())
         invokeGC(label, msa argN_VM);
      addr = msa.allocGC(reqLen);
   }
   return addr;
}

#endif
