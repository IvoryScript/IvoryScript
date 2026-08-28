/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    iCode_GC.cpp
 *
 * Module:  IvoryScript compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 10 May 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    I-machine garbage collection code generation
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

#include "ivory/compiler/ICode.h"
#include "ivory/trace.h"

declareTypeCon(List);
declareTypeCon(Ptr);
declareTypeCon(String);

// ICode::genLiveVarsGC_Code:

// Generate code to mark all memory segments associated with live variables
// For both continuations and instructions involving heap allocation.

// Note:  The stack pointer should be set to the stack depth before GC
//        This could be done for every heap allocation or saved if GC is invoked
//        (preferably the latter).

Void ICode::genGC_Code(Var& var, StackLoc* loc, StackLoc* env_loc,
                       GC_StubInstruction& gcStubInstr) {
   Bool gcFlag = _gcFlag;
   setGCFlag(TRUE);

   Lambda* lambda = _lambda;
   _lambda = gcStubInstr._lambda;
   InstrSeq* instrSeq = _instrSeq;
   _instrSeq = new(msa()) InstrSeq();
   BasicBlk* basicBlks = _basicBlks;
   BasicBlk* basicBlk = _basicBlk;

   Bool nextUse = var.nextUse();
   Bool lastNextUse = var.lastNextUse();
   Bool killPending = var.killPending();

   StructTemplate* structTemplates = _structTemplates;
   _structTemplates = NULL;

   _basicBlks = NULL;
   _basicBlk = NULL;

   CodeLabel& lab = genGC_MarkSnippet(var);
   var.gcSetLab(lab);
   var.gcSetStackOffset(loc->offset());
   var.gcSetEnvStackOffset(env_loc->offset());
   
   init(FALSE);

   VarElement* varElement = lab.basicBlk().vars().head();

   LocSet locSet = var.locSet();
   var.locSet() = LocSet();

   loc = (StackLoc*)loc->clone(NULL, TRUE, msa());
   env_loc = (StackLoc*)env_loc->clone(NULL, TRUE, msa());

   loc->setFrameRelative();
   env_loc->setFrameRelative();

   varElement->locSet().addElement(*loc, msa());
   varElement->locSet().addElement(*env_loc, msa());

   UInt sp = _sp;
   UInt sd = _sd;
   Bool argFlag = _argFlag;

   _sp = _sd = 0;
   _argFlag = FALSE;

   Instruction* currentIns = _currentIns;
   StackLoc* stackLocs = _stackLocs;
   _stackLocs = NULL;

   Register* register__[N_REGS];   // I-machine registers
   for (UInt r = REG_cell; r <= REG_rRef; r++) {
      register__[r] = _register[r];
      _register[r] = new(_msa) Register(r);
   }      

   genLocCode();

   for (UInt r = REG_cell; r <= REG_rRef; r++)
      _register[r] = register__[r];

   _stackLocs = stackLocs;
   _currentIns = currentIns;

   _sd = sd;
   _sp = sp;
   _argFlag = argFlag;

   var.locSet().empty(msa());
   var.locSet() = locSet;

   var.setKillPending(killPending);
   var.setNextUse(nextUse);
   var.setLastNextUse(lastNextUse);

   _basicBlks = basicBlks;
   _basicBlk = basicBlk;

// append any structure templates

  _structTemplates = structTemplates;

   instrSeq->insert(*_instrSeq, &gcStubInstr);
  _instrSeq = instrSeq;

   _lambda = lambda;

   setGCFlag(gcFlag);
}

Void ICode::genLiveVarsGC_Code(Bool isContinuation,
                               BasicBlk& basicBlk,
                               UInt frameSize,
                               FrameDescrInstruction& frameDescrInstr) {
   FrameDescr<CodeLabel*>& frameDescr = frameDescrInstr.frameDescr();
   frameDescr.setLabSeqNo(frameDescrInstr.labOperand().codeLabel().seqNo());
   frameDescr.setSize(frameSize);
   for (UInt pass = 1; pass <= 2; pass++) {
      UInt n = 0;
      VarElement* varElement = basicBlk._vars.head();
      while (varElement != NULL){
         if (!isContinuation ||
             isContinuation && varElement->testFlags(VarElement::I)) {
            Var& var = varElement->var();
            if (var.kind() != Var::CONTINUATION_VAR &&
                var.kind() != Var::NON_GLOBAL_FREE_VAR &&
                var.kind() != Var::EXT_GLOBAL_VAR &&
                var.isLive()) {
               Repr repr = var.repr(*this);
               if (var.needsEnv() &&
                   !var.hasReservedLocs() &&
                  repr.markableForGC()) {  // Skip variables not subject to GC
                  if (pass == 1) {
/*
                     printName(var.name(), outStream, _env.nameTable());
                     outStream << "::"; printTypeSig(var.typeSig(), 0, FALSE, outStream, _env);
                     outStream << ": "; printRepr(repr, outStream);
                     outStream << endl;
*/
   
                     Loc* loc = var.bestLocOrNULL(var.repr(), FALSE);
    
                     assert(loc->kind() == Loc::STACK_LOC, "Expected stack location");
                     Loc* env_loc = var.bestLocOrNULL(Repr::REPR_ENV_PTR, FALSE);
                     assert(env_loc->kind() == Loc::STACK_LOC, "Expected stack location");
                     if (var.gcLab() == NULL ||
                        var.gcStackOffset() != ((StackLoc*)loc)->offset() ||
                        var.gcEnvStackOffset() != ((StackLoc*)env_loc)->offset()) {
                        genGC_Code(var, (StackLoc*)loc, (StackLoc*)env_loc,
                           frameDescrInstr.gcStubInstr());
                     }
                     
                  } else {
                     FrameDescrSlot<CodeLabel*>& slot = frameDescr.slot(n);
                     slot.setName(var.name());
                     slot.setType(::useType(var.typeSig(),
                                            env().typeTable(),
                                            env().nameTable()));
                     slot.setAddress(var.gcLab());
                  }
                  n++;
               }
            }
         }
         varElement = varElement->_next;
      }
      if (pass == 1) {
         frameDescr.setN_slots(n);
         FrameDescrSlot<CodeLabel*>* slots = n > 0
            ? (FrameDescrSlot<CodeLabel*>*)_msa.alloc(sizeof(FrameDescrSlot<CodeLabel*>) * n)
            : NULL;
         frameDescr.setSlots(slots);
      }
   }
}
#endif
