/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    exec.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Ivory byte code interpreter.
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

#if (INTERPRETER==1)

#define TRACE
#ifndef __unix__
#define TRACE_EXTRA     // If defined, adds additional operand tracing
#endif

#ifdef TRACE
static unsigned int contCount = 1000; // A probe to ensure matching continuations
#endif

#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nameTable.h"
#include "ivory/any.h"
#include "ivory/basicADS.h"
#include "ivory/bits.h"
#include "ivory/byteString.h"
#include "ivory/common.h"
#include "ivory/double.h"
#include "ivory/exec.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/exec_GC.h"
#include "ivory/iFrameDescr.h"
#endif

#include "ivory/float.h"
#include "ivory/iMachine.h"
#include "ivory/int.h"
#include "ivory/name.h"
#include "ivory/object.h"
#include "ivory/ref.h"
#include "ivory/repr.h"
#include "ivory/string.h"
#include "ivory/type.h"

#ifdef TRACE
#ifndef TRACE_FLAG_DEFINED
static Bool traceFlag = FALSE;
#define TRACE_FLAG_DEFINED
#endif
#define toggle(flag) ((flag)=!(flag))

Void traceInterpreter(Bool flag) {
   traceFlag = flag;
}

Bool interpreterTraceFlag(Void) {
   return traceFlag;
}

static Void* checkMemAlloc(Void* addr) {
   if (addr == NULL)
      throw(RunTimeError("Failed to allocate memory"));

   return addr;
}



static Void showMemory(const Void* addr, Int offset, size_t len) {
   outStream << hex << setw(8) << setfill('0') << addr << dec << endl;
   for (Int i = 0; i < (Int)len; i++) {
      if (i > 0) {
         outStream << (i % 16 == 0 ? '\n' : ' ');
      }
     // outStream << hex << (UInt8)((Byte*)addr)[i + offset] << dec;
      outStream << hex << setw(2) << setfill('0') << (Int)((Byte*)addr)[i + offset] << dec;
   }
   if (len != 0)
      outStream << endl;
}
#else

#define checkMemAlloc(addr) (addr)

Void traceInterpreter(Bool flag) {
}

#endif

static Void* execMemAlloc(size_t reqLen,

#if (GARBAGE_COLLECTION==1)
   IAddress pc,
#endif

   MSA& msa) {

#if (GARBAGE_COLLECTION==1)
   Void* addr = execMemAlloc_GC(reqLen, pc, dynamic_cast<MSA_GC&>(msa));
#else
   Void* addr = msa.alloc(reqLen);
#endif

   if (addr == NULL)
      throw(RunTimeError("Failed to allocate memory"));

   return addr;
}


// Insert bits

Void insBits16(IAddress iAddr, Bits16 val) {
   iAddr[0] = ((Byte)((val >> 0) & 0xff));
   iAddr[1] = ((Byte)((val >> 8) & 0xff));
}

Void insBits32(IAddress iAddr, Bits32 val) {
   iAddr[0] = ((Byte)((val >> 0) & 0xff));
   iAddr[1] = ((Byte)((val >> 8) & 0xff));
   iAddr[2] = ((Byte)((val >> 16) & 0xff));
   iAddr[3] = ((Byte)((val >> 24) & 0xff));
}

Void insBits64(IAddress iAddr, Bits64 val) {
   iAddr[0] = ((Byte)((val >> 0) & 0xff));
   iAddr[1] = ((Byte)((val >> 8) & 0xff));
   iAddr[2] = ((Byte)((val >> 16) & 0xff));
   iAddr[3] = ((Byte)((val >> 24) & 0xff));
   iAddr[4] = ((Byte)((val >> 32) & 0xff));
   iAddr[5] = ((Byte)((val >> 40) & 0xff));
   iAddr[6] = ((Byte)((val >> 48) & 0xff));
   iAddr[7] = ((Byte)((val >> 56) & 0xff));
}

EnvPtr immEnvPtr(unsigned char arg) {
   assert(FALSE, "Illegal address mode for Env*");
   return NULL;
}

Expr immExpr(unsigned char arg) {
   return (Expr)ERROR;
}

Char immChar(unsigned char arg) {
   return '\0';
}

Int immByte(unsigned char arg) {
   unsigned char val = immVal(arg);
   return (Byte)val;
}

Bits immBits(unsigned char arg) {
   unsigned char val = immVal(arg);
   return (Bits)val;
}

Int immInt(unsigned char arg) {
   Int val = immVal(arg);
   if ((val & IMM_VAL_SIGN_MASK) != 0)
      val |= ~IMM_VAL_MASK;
   return val;
}

Label immLabel(unsigned char arg) {
   return NULL;
}

IAddress immIAddress(unsigned char arg) {
   return NULL;
}

Ref immRef(unsigned char arg) {
   return NULL_REF;
}

Type immType(unsigned char arg) {
   return NULL_TYPE;
}

Name immName(unsigned char arg) {
   return NULL_NAME;
}

Double immDouble(unsigned char arg) {
   return 0.0;
}

Ptr immPtr(unsigned char arg) {
   return NULL;
}

// extBits16: Extract 16 bits from given address.

Bits16 extBits16(IAddress& iAddr) {
   Bits16 res = (Bits16)iAddr[0] | (Bits16)iAddr[1] << 8;
   iAddr += 2;
   return res;
}

// extBits32: Extract bits from given address
// Could expand out to increase speed

Bits32 extBits32(IAddress& iAddr) {
   Bits32 ls = (Bits32)extBits16(iAddr);
   Bits32 ms = (Bits32)extBits16(iAddr);
   return ls | (ms << 16);
}

// extBits64: Extract bits from given address

Bits64 extBits64(IAddress& iAddr) {
   Bits64 ls = (Bits64)extBits32(iAddr);
   Bits64 ms = (Bits64)extBits32(iAddr);
   return ls | (ms << 32);
}

// extBits32: Extract double from given address

Double extDouble(IAddress& iAddr) {
   double d;
   memcpy(&d, iAddr, sizeof(double));
   iAddr += sizeof(double);

#if (BIG_ENDIAN==0)
   byteSwap(&d, sizeof(double));
#endif

   return d;
}

Void* extPtr1(IAddress& iAddr) {
   return NULL;
}

Void* extPtr2(IAddress& iAddr) {
   return NULL;
}

Void* extPtr4(IAddress& iAddr) {
   return NULL;
}

Float immFloat(unsigned char arg) {
   return 0.0;
}

static Byte& lvByte(IAddress& iAddr argN_VM);

lvT(Label);
lvT(LabelPair);
lvT(Name);
lvT(Type);
lvT(Expr);
lvT(Byte);
lvT(Bits);
lvT(Int);
lvT(Float);
lvT(Double);
lvT(Tag);
lvT(Char);
lvT(Ref);
lvT(Ptr);

loadT(Label);
loadT(IAddress);
loadT(Name);
loadT(Type);
loadT(Expr);
loadT(Byte);
loadT(Bits);
loadT(Int);
loadT(UInt);
loadT(Float);
loadT(Double);
loadT(Tag);
loadT(Char); 
loadT(Ref);
loadT(Ptr);

storeT(Label); 
storeT(Name);

Void storeExpr(Expr val, IAddress& iAddr argN_VM) {
   lvExpr(iAddr n_vm) = val;
}

storeT(Type);
storeT(Byte);
storeT(Bits);
storeT(Int);
storeT(Float);
storeT(Double);
storeT(Tag);
storeT(Char);
storeT(Ptr);

Void storeRef(Ref val, IAddress& iAddr argN_VM) {
   lvRef(iAddr n_vm) = val;
}

Void alignToBoundary(IAddress& iAddr) {
   if ((UInt32)iAddr % 4 != 0)
      iAddr += 4 - (UInt32)iAddr % 4;
}

IAddress initCodeAddr(IAddress segment) {
   IAddress iAddr = segment + 3 * sizeof(UInt32);
   return segment + extBits32(iAddr);
}

Void callIContinuation(IAddress pc argN_VM) {
   push(pc, IAddress);

#ifdef TRACE
   if (traceFlag)
      outStream << "Call continuation (" << ++contCount << "): address " << (Void*)pc << endl;
#endif

   callSequence$(entry(iContinuation) n_vm);
}

// Byte code type descriptor methods

declareLabel(typeDescrCallback$cont);

#if (GARBAGE_COLLECTION==1)
static BuiltInFrameDescr typeDescrCallback$frameDescr(label(typeDescrCallback$cont),
   sizeof(Label), 0, NULL);
#endif

defineLabel(typeDescrCallback$cont) {
   return NULL;
}

Void evalFnMethod(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   pushLabel(label(typeDescrCallback$cont));

#ifdef TRACE
   if (traceFlag)
      contCount++;
#endif

   rEnv = &env;
   rPtr = ptr;
   iCallSequence(((ITypeDescr*)typeDescr)->_evalFnLab);
}

Ptr mapFnMethod(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   pushLabel(label(typeDescrCallback$cont));

#ifdef TRACE
   if (traceFlag)
      contCount++;
#endif

#define _dst_env    sizeof(Env*)
#define stackDepth   _dst_env
   stackAlloc(stackDepth);
   local(_dst_env, Env*) = &dstEnv;
   rEnv = (Env*)(Void*)& ptrEnv;
   rPtr = ptr;
   iCallSequence(((ITypeDescr*)typeDescr)->_mapFnLab);
   return rPtr;
}
#undef _dst_env
#undef stackDepth

Void assignFnMethod(TypeDescr* typeDescr, Ptr src, const Env& srcEnv,
                  Ptr dst, Env& dstEnv argN_VM) {
   pushLabel(label(typeDescrCallback$cont));

#ifdef TRACE
   if (traceFlag)
      contCount++;
#endif

#define _dst_ptr_env sizeof(Env*)
#define _dst_ptr     (_dst_ptr_env+sizeof(Ptr))
#define stackDepth   _dst_ptr
   stackAlloc(stackDepth);
   local(_dst_ptr_env, const Env*) = &dstEnv;
   local(_dst_ptr, Ptr) = dst;
   rEnv = (Env*)(Void*)&srcEnv;
   rPtr = src;
   iCallSequence(((ITypeDescr*)typeDescr)->_assignFnLab);
}

#undef _dst_ptr
#undef _dst_ptr_env
#undef stackDepth

Void insertTxtFnMethod(TypeDescr* typeDescr, OutputStream_Char* os,
                       const Env& osEnv, Ptr ptr,
                       const Env& env argN_VM) {
   pushLabel(label(typeDescrCallback$cont));

#ifdef TRACE
   if (traceFlag)
      contCount++;
#endif

   rEnv = static_cast<Env*>((Void*)&osEnv);
   rPtr = os;
   push((Env*)(Void*)&env, Ptr);
   push(ptr, Ptr);
   iCallSequence(((ITypeDescr*)typeDescr)->_insertTxtFnLab);
}

#if (SERIALISATION==1)
Ptr extractBinFnMethod(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   pushLabel(label(typeDescrCallback$cont));

#ifdef TRACE
   if (traceFlag)
      contCount++;
#endif

   rEnv = static_cast<Env*>((Void*)&isEnv);
   rPtr = is;
   iCallSequence(((ITypeDescr*)typeDescr)->_extractBinFnLab);
   return rPtr;
}

Void insertBinFnMethod(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   pushLabel(label(typeDescrCallback$cont));

#ifdef TRACE
   if (traceFlag)
      contCount++;
#endif

   rEnv = static_cast<Env*>((Void*)&osEnv);
   rPtr = os;
   push(&env, Ptr);
   push(ptr, Ptr);
   iCallSequence(((ITypeDescr*)typeDescr)->_insertBinFnLab);
}
#endif

#if (GARBAGE_COLLECTION==1)
Void markFnMethod(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   pushLabel(label(typeDescrCallback$cont));

#ifdef TRACE
   if (traceFlag)
      contCount++;
#endif

   rEnv = static_cast<Env*>((Void*)&env);
   rPtr = ptr;
   iCallSequence(((ITypeDescr*)typeDescr)->_markFnLab);
}
#endif

ITypeDescr::ITypeDescr(TypeSig typeSig)
   : TypeDescr(typeSig),
   _evalFnSegId(NULL_SEGMENT_ID),      _evalFnLab(0),    _evalFnSeg(0),
   _mapFnSegId(NULL_SEGMENT_ID),       _mapFnLab(0),     _mapFnSeg(0),
   _assignFnSegId(NULL_SEGMENT_ID),    _assignFnLab(0),  _assignFnSeg(0),
   _insertTxtFnSegId(NULL_SEGMENT_ID), _insertTxtFnLab(0), _insertTxtFnSeg(0)

#if (SERIALISATION==1)
 , _extractBinFnSegId(NULL_SEGMENT_ID),   _extractBinFnLab(0), _extractBinFnSeg(0),
   _insertBinFnSegId(NULL_SEGMENT_ID),    _insertBinFnLab(0),  _insertBinFnSeg(0)
#endif

#if (GARBAGE_COLLECTION==1)
 , _markFnSegId(NULL_SEGMENT_ID), _markFnLab(0), _markFnSeg(0)
#endif

   {
   _evalFn = evalFnMethod;
   _mapFn =  mapFnMethod;
   _assignFn = assignFnMethod;
   _insertTxtFn = insertTxtFnMethod;

#if (SERIALISATION==1)
   _extractBinFn = extractBinFnMethod;
   _insertBinFn = insertBinFnMethod;
#endif

#if (GARBAGE_COLLECTION==1)
   _markFn = markFnMethod;
#endif

}

TypeDescr::Kind ITypeDescr::kind(Void) const {
   return TYPE_DESCR_BYTE_CODE;
}

Void ITypeDescr::postLoad(const SegmentTable& segmentTable) {
   if (_evalFnSegId != NULL_SEGMENT_ID) {
      _evalFnSeg = (IAddress)segmentTable.segment(_evalFnSegId);
      _evalFnLab = _evalFnSeg + (_evalFnLab - (IAddress)0);
   }
   if (_mapFnSegId != NULL_SEGMENT_ID) {
      _mapFnSeg = (IAddress)segmentTable.segment(_mapFnSegId);
      _mapFnLab = _mapFnSeg + (_mapFnLab - (IAddress)0);
   }
   if (_assignFnSegId != NULL_SEGMENT_ID) {
      _assignFnSeg = (IAddress)segmentTable.segment(_assignFnSegId);
      _assignFnLab = _assignFnSeg + (_assignFnLab - (IAddress)0);
   }
   if (_insertTxtFnSegId != NULL_SEGMENT_ID) {
      _insertTxtFnSeg = (IAddress)segmentTable.segment(_insertTxtFnSegId);
      _insertTxtFnLab = _insertTxtFnSeg + (_insertTxtFnLab - (IAddress)0);
   }

#if (SERIALISATION==1)
   if (_extractBinFnSegId != NULL_SEGMENT_ID) {
      _extractBinFnSeg = (IAddress)segmentTable.segment(_extractBinFnSegId);
      _extractBinFnLab = _extractBinFnSeg + (_extractBinFnLab - (IAddress)0);
   }
   if (_insertBinFnSegId != NULL_SEGMENT_ID) {
      _insertBinFnSeg = (IAddress)segmentTable.segment(_insertBinFnSegId);
      _insertBinFnLab = _insertBinFnSeg + (_insertBinFnLab - (IAddress)0);
   }
#endif

#if (GARBAGE_COLLECTION==1)
   if (_markFnSegId != NULL_SEGMENT_ID) {
      _markFnSeg = (IAddress)segmentTable.segment(_markFnSegId);
      _markFnLab = _markFnSeg + (_markFnLab - (IAddress)0);
   }
#endif

}

Void ITypeDescr::store(Archive& archive, const NameTable& nameTable) const {
   archive.insertVLU(_size);
   storeTypeSig(archive, _typeSig, nameTable);
   archive << (UInt32)_hashVal;
   archive << (UInt32)_denotedType;
   archive << _evalFnSegId;
   archive << (UInt32)(_evalFnLab - _evalFnSeg);
   archive << _mapFnSegId;
   archive << (UInt32)(_mapFnLab - _mapFnSeg);
   archive << _assignFnSegId;
   archive << (UInt32)(_assignFnLab - _assignFnSeg);
   archive << _insertTxtFnSegId;
   archive << (UInt32)(_insertTxtFnLab - _insertTxtFnSeg);

#if (SERIALISATION==1)
   archive << _extractBinFnSegId;
   archive << (UInt32)(_extractBinFnLab - _extractBinFnSeg);
   archive << _insertBinFnSegId;
   archive << (UInt32)(_insertBinFnLab - _insertBinFnSeg);
#endif

#if (GARBAGE_COLLECTION==1)
   archive << _markFnSegId;
   archive << (UInt32)(_markFnLab - _markFnSeg);
#endif

}

TypeDescr* ITypeDescr::Loader::load(Archive& archive,
                                    const NameTable& nameTable,
                                    MSA& msa) const {
   size_t size = archive.extractVLU();
   TypeSig typeSig = loadTypeSig(archive, nameTable, msa);  
   ITypeDescr* typeDescr = new(msa) ITypeDescr(typeSig);
   typeDescr->_size = size;
   UInt32 hashVal;
   archive >> hashVal;
   typeDescr->_hashVal = hashVal;
   UInt32 denotedType;
   archive >> denotedType;
   typeDescr->_denotedType = (Type)denotedType;
   UInt32 offset;
   archive >> typeDescr->_evalFnSegId;
   archive >> offset;
   typeDescr->_evalFnLab = ((IAddress)0) + offset;
   archive >> typeDescr->_mapFnSegId;
   archive >> offset;
   typeDescr->_mapFnLab = ((IAddress)0) + offset;
   archive >> typeDescr->_assignFnSegId;
   archive >> offset;
   typeDescr->_assignFnLab = ((IAddress)0) + offset;
   archive >> typeDescr->_insertTxtFnSegId;
   archive >> offset;
   typeDescr->_insertTxtFnLab = ((IAddress)0) + offset;

#if (SERIALISATION==1)
   archive >> typeDescr->_extractBinFnSegId;
   archive >> offset;
   typeDescr->_extractBinFnLab = ((IAddress)0) + offset;
   archive >> typeDescr->_insertBinFnSegId;
   archive >> offset;
   typeDescr->_insertBinFnLab = ((IAddress)0) + offset;
#endif

#if (GARBAGE_COLLECTION==1)
   archive >> typeDescr->_markFnSegId;
   archive >> offset;
   typeDescr->_markFnLab = ((IAddress)0) + offset;
#endif

   return typeDescr;
}

static ITypeDescr::Loader loader;

// Byte code cell information structure
// Native entry point for an I-Code continuation on the stack

defineEntry(iContinuation) {
   IAddress pc = stack(0, IAddress);
#define stackDepth sizeof(IAddress)
   dropStack();
#undef stackDepth

#ifdef TRACE
   if (traceFlag) {
      outStream << "\nICode continuing (" << contCount-- << "): " << (Void*)pc << endl;
   }
#endif

   return execByteCode(pc n_vm);
}

//static Int runCount = 0;  // Useful to enable tracing in case of endless loop

defineEntry(iClosure) {

#ifdef TRACE
   if (traceFlag)
      outStream << "\nEntering ICode closure" << endl;
#endif

//   runCount++;
//   if (runCount > 100000)
//      traceFlag = TRUE;

   return execByteCode(static_cast<const ICellInfo&>(*static_cast<const CellInfo*>(cell->tag()))._byteCodeEntry n_vm);
}

defineAltEntry(iClosure) {

#ifdef TRACE
   if (traceFlag)
      outStream << "\nEntering ICode closure (alternative)" << endl;
#endif

   //   runCount++;
   //   if (runCount > 100000)
   //      traceFlag = TRUE;

   return execByteCode(static_cast<const ICellInfo&>(*static_cast<const CellInfo*>(cell->tag()))._byteCodeAltEntry n_vm);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const


// allocCellInfo: Allocate an ICode cell info structure

static ICellInfo* allocCellInfo(IAddress entryLab,
                                IAddress altEntryLab,
                                IAddress copyFnLab,


#if (SERIALISATION==1)
                                IAddress extractBinFnLab,
                                IAddress insertBinFnLab,
#endif

#if (GARBAGE_COLLECTION==1)
                                IAddress gcMarkFnLab,     
#endif

                                size_t size,
                                IAddress segment,
                                IAddress pc, MSA& msa) {
   return (ICellInfo*)new(msa) ICellInfo(size, 
                                         entryLab, altEntryLab,
                                         copyFnLab,

#if (SERIALISATION==1)
                                         extractBinFnLab,
                                         insertBinFnLab,
#endif
      
#if (GARBAGE_COLLECTION==1)
                                         gcMarkFnLab,
#endif     
      
                                         segment);
}

// allocClosure: Allocates a closure with the given size and cell info structure.
// Ensures that its size is at least a minimum

Cell* allocClosure(size_t size, const CellInfo* cellInfo, IAddress pc, MSA& msa) {
   Void* ptr = execMemAlloc(sizeof(Cell) - MAX_CELL_BODY_SIZE + mkClosure_max(size, MIN_CLOSURE_SIZE),

#if (GARBAGE_COLLECTION==1)
                             pc,
#endif
                             msa); 

   return new(ptr) Cell(cellInfo);
}

declareLabel(byteCodeCellCopy$cont);

#if (GARBAGE_COLLECTION==1)
static BuiltInFrameDescr byteCodeCellCopy$frameDescr(label(byteCodeCellCopy$cont),
   sizeof(Label), 0, NULL);
#endif

defineLabel(byteCodeCellCopy$cont) {
   jump(popLabel());
}

static Cell* byteCodeCellCopyFn(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   if (&srcEnv == &env)
      return &src;
   const ICellInfo& iCellInfo = static_cast<const ICellInfo&>(*static_cast<const CellInfo*>(src.tag()));

   if (iCellInfo._byteCodeCopyFnEntry != NULL) {

// Invoke cell copy method

      pushLabel(label(byteCodeCellCopy$cont));
      rEnv = &env;
      cell = &src;
      cellEnv = (Env*)(Void*)&srcEnv;
      execByteCode(iCellInfo._byteCodeCopyFnEntry n_vm);
      return rCell;
   }
   else {

//--- Standard miminal cell copy

      Void* ptr = env.msa().alloc(sizeof(Cell) - MAX_CELL_BODY_SIZE + MIN_CLOSURE_SIZE);
      return new(ptr) Cell(copyICellInfo(&iCellInfo, env));
   }
}


defineEntry(iExtractCell) {
   const ICellInfo& iCellInfo = static_cast<const ICellInfo&>(*static_cast<const CellInfo*>(cell->tag()));
   return iCellInfo._byteCodeExtractFnEntry != 0
      ? execByteCode(iCellInfo._byteCodeExtractFnEntry n_vm)
      : NULL;
}

defineEntry(iInsertCell) {
   const ICellInfo& iCellInfo = static_cast<const ICellInfo&>(*static_cast<const CellInfo*>(cell->tag()));
   return iCellInfo._byteCodeInsertFnEntry != 0
      ? execByteCode(iCellInfo._byteCodeInsertFnEntry n_vm)
      : NULL;
}

#if (GARBAGE_COLLECTION==1)
defineEntry(iMarkCell) {
   const ICellInfo& iCellInfo = static_cast<const ICellInfo&>(*static_cast<const CellInfo*>(cell->tag()));
   return iCellInfo._byteCodeGC_MarkCellEntry != 0
      ? execByteCode(iCellInfo._byteCodeGC_MarkCellEntry n_vm)
      : NULL;
}
#endif


static Void byteCodeCellShowFn(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << "Byte code closure";
}

#undef localConst
#define localConst

static Void byteCodeCellDestroyFn(Cell& cell_, Env& env, MSA& msa argN_VM) {
   ICellInfo* cellInfo =
      &static_cast<ICellInfo&>(exprCellInfo(cell_.tag()));

   msa.free(cellInfo->_segment);
   msa.free(cellInfo);
   msa.free(&cell_);
}


ICellInfo::ICellInfo(size_t size,
                     IAddress byteCodeEntry,
                     IAddress byteCodeAltEntry,
                     IAddress byteCodeCopyFnEntry,

#if (SERIALISATION==1)
                     IAddress byteCodeExtractFnEntry,
                     IAddress byteCodeInsertFnEntry,
#endif

#if (GARBAGE_COLLECTION==1)
                     IAddress byteCodeGC_MarkCellEntry,
#endif

                     IAddress segment)
   : CellInfo(CLOSURE, size,
              entry(iClosure),
              altEntry(iClosure),
              byteCodeCellCopyFn,

#if (SERIALISATION==1)
              entry(iExtractCell),
              entry(iInsertCell),
#endif

#if (GARBAGE_COLLECTION==1)
              entry(iMarkCell),
#endif

              byteCodeCellDestroyFn,
              byteCodeCellShowFn),
   _byteCodeEntry(byteCodeEntry),
   _byteCodeAltEntry(byteCodeAltEntry),
   _byteCodeCopyFnEntry(byteCodeCopyFnEntry),

#if (SERIALISATION==1)
   _byteCodeExtractFnEntry(byteCodeExtractFnEntry),
   _byteCodeInsertFnEntry(byteCodeInsertFnEntry),
#endif


#if (GARBAGE_COLLECTION==1)
   _byteCodeGC_MarkCellEntry(byteCodeGC_MarkCellEntry),
#endif

   _segment(segment)
{
}


indirectTableBranchOffset(Name);
indirectTableBranchOffset(Type);
tableBranchOffset(Int);
//tableBranchOffset(Tag);

Int tableBranchOffsetTag(Tag index,IAddress table,UInt n){
IAddress addr=table;
while(n>0){
Tag key=extTag(addr);
Int offset=extInt4(addr);
if(key==index)
return offset;
n--;}
return extInt4(addr);}


tableBranchOffset(Char);
stringTableBranchOffset();

// ByteCode interpreter

Label execByteCode(IAddress pc argN_VM) {
   Byte opCode;
   do {

#ifdef TRACE
      if ((int)pc == 0x0) {
         outStream << "execByteCode: breakpoint at " << (Void*)(pc - 1) << endl;
         traceFlag = true;
      }
#endif

      opCode = fetchByte(pc);

#ifdef TRACE
      if (traceFlag) {
         outStream << "execByteCode: pc = " << (Void*)(pc - 1);
         outStream << ", opCode = " << opCodeToString(opCode);
         outStream << ", sp = " << sp << ", sb = " << sb << endl;

         outStream << "Env: " << rEnv << ' ' << consEnv << endl;
      }
#endif

      switch (opCode) {
         case ATD_INS: {   // assign type descriptor
            Type type = loadType(pc n_vm);
            Env& typeEnv = *(Env*)loadPtr(pc n_vm);
            Type denotedType = loadType(pc n_vm);
            size_t size = unpackVLU(&pc);
            Int evalFnSegOffset =   extInt4(pc);
            Int evalFnLabOffset =   extInt4(pc);
            Int mapFnSegOffset =    extInt4(pc);
            Int mapFnLabOffset =    extInt4(pc);
            Int assignFnSegOffset =   extInt4(pc);
            Int assignFnLabOffset =   extInt4(pc);
            Int insertTxtFnSegOffset =   extInt4(pc);
            Int insertTxtFnLabOffset =   extInt4(pc);

#if (SERIALISATION==1)
            Int extractBinFnSegOffset = extInt4(pc);
            Int extractBinFnLabOffset = extInt4(pc);
            Int insertBinFnSegOffset = extInt4(pc);
            Int insertBinFnLabOffset = extInt4(pc);
#endif

#if (GARBAGE_COLLECTION==1)
            Int markFnSegOffset = extInt4(pc);
            Int markFnLabOffset = extInt4(pc);
#endif

            TypeDescr* typeDesc = typeEnv.typeTable().typeDescr(type - builtInTypeCount);

            ITypeDescr& typeDescr = *static_cast<ITypeDescr*>(typeEnv.typeTable().typeDescr(type - builtInTypeCount));
            typeDescr._size = size;
            typeDescr._denotedType = denotedType;
            typeDescr._evalFnSeg =  evalFnSegOffset != 0 ?  pc + evalFnSegOffset :  0;
            typeDescr._evalFnLab =  evalFnLabOffset != 0 ?  pc + evalFnLabOffset :  0;
            typeDescr._mapFnSeg =   mapFnSegOffset != 0 ?   pc + mapFnSegOffset :   0;
            typeDescr._mapFnLab =   mapFnLabOffset != 0 ?   pc + mapFnLabOffset  :  0;
            typeDescr._assignFnSeg =  assignFnSegOffset != 0 ?  pc + assignFnSegOffset :  0;
            typeDescr._assignFnLab =  assignFnLabOffset != 0 ?  pc + assignFnLabOffset :  0;
            typeDescr._insertTxtFnSeg =  insertTxtFnSegOffset != 0 ?  pc + insertTxtFnSegOffset :  0;
            typeDescr._insertTxtFnLab =  insertTxtFnLabOffset != 0 ?  pc + insertTxtFnLabOffset :  0;

#if (SERIALISATION==1)
            typeDescr._extractBinFnSeg = extractBinFnSegOffset != 0 ? pc + extractBinFnSegOffset : 0;
            typeDescr._extractBinFnLab = extractBinFnLabOffset != 0 ? pc + extractBinFnLabOffset : 0;
            typeDescr._insertBinFnSeg = insertBinFnSegOffset != 0 ? pc + insertBinFnSegOffset : 0;
            typeDescr._insertBinFnLab = insertBinFnLabOffset != 0 ? pc + insertBinFnLabOffset : 0;
#endif

#if (GARBAGE_COLLECTION==1)
            typeDescr._markFnSeg = markFnSegOffset != 0 ? pc + markFnSegOffset : 0;
            typeDescr._markFnLab = markFnLabOffset != 0 ? pc + markFnLabOffset : 0;

#endif

   #ifdef TRACE_EXTRA
               if (traceFlag)
                  outStream << "Allocated type descriptor" << endl;
   #endif
               break;
            }

         case ADD_P_INS: {
            Ptr x = loadPtr(pc n_vm);
            Int y = loadInt(pc n_vm);
            storePtr((unsigned char*)x + y, pc n_vm);
            break;
         }

         case ADD_SP_INS: {
            (*((unsigned char**)&sp) += unpackVLU(&pc));
            break;
         }

         case SUB_SP_INS: {
            (*((unsigned char**)&sp) -= unpackVLU(&pc));
            break;
         }

         case SQZ_INS: {
            Int from =  unpackVLI(&pc);
            Int to =    unpackVLI(&pc);
            UInt size = unpackVLU(&pc);
            memmove(((unsigned char*)sp) + to, ((unsigned char*)sp) + from, size);
            break;
         }

         case MOV_E_INS: {
            Expr src=loadExpr(pc n_vm);
            traceLoad(src);
            storeExpr(src, pc n_vm);
            break;
         }

         case MOV_C_INS:
            execMove(Char);

         case MOV_B8_INS:
            execMove(Byte);

         case MOV_B_INS:
            execMove(Bits);

         case MOV_I_INS:
            execMove(Int);

         case MOV_L_INS:
            execMove(Label);

         case MOV_N_INS:
            execMove(Name);

         case MOV_P_INS:
            execMove(Ptr);

         case LVP_INS: {
            Ptr src = (Ptr)&lvByte(pc n_vm);
            traceLoad(src);
            storePtr(src, pc n_vm);
            break;
         }

         case MOV_T_INS:
            execMove(Type);

         case MOV_R_INS:
            execMove(Ref);

         case MOV_O_INS:
            execMove(Tag);

         case CFN_INS: {
            Name name = loadName(pc n_vm);
            Env& nameEnv = *(Env*)loadPtr(pc n_vm);
            Type type = loadType(pc n_vm);
            Env& typeEnv = *(Env*)loadPtr(pc n_vm);
            storeTag(constructFromName(name, nameEnv, type, typeEnv), pc n_vm);
            break;
         }

         case CFS_INS: {
            String str = (String)loadPtr(pc n_vm);
            Type type = loadType(pc n_vm);
            Env& typeEnv = *(Env*)loadPtr(pc n_vm);
            storeTag(constructFromString(str, type, typeEnv), pc n_vm);
            break;
         }

         case MMV_B_INS: {
            Byte* src = &lvByte(pc n_vm);
            Byte* dst = &lvByte(pc n_vm);
            size_t size = unpackVLU(&pc);
            memmove(dst, src, size);

#ifdef TRACE
            if (traceFlag) {
               outStream << "Moving memory: ";
               showMemory(src, 0, size);
               outStream.flush();
            }
#endif

            break;
         }

         case MCP_INS: {
            int offset = extInt4(pc);
            IAddress dataPtr = pc + offset;
            Ptr& ptr = lvPtr(pc n_vm);
            ptr = dataPtr;
            break;
         }

         case MIL_INS: {
            Int offset = extInt4(pc);
            LabelPair& lp = lvLabelPair(pc n_vm);
            lp._label = entry(iContinuation);
            lp._iLabel = pc + offset;

#ifdef TRACE_EXTRA
            if (traceFlag)
               outStream << "Pushed continuation (" << ++contCount << "): (" << lp._label << ", " << (Void*)(lp._iLabel) << ')' << endl;
#endif

            break;
         }

         case MAP_N_INS: {
            Name src = loadName(pc n_vm);
            traceLoad(src);
            Env& src_env = *(Env*)loadPtr(pc n_vm);
            Env& dst_env = *(Env*)loadPtr(pc n_vm);
            Name dst = mapName(src, src_env.nameTable(), dst_env.nameTable());
            storeName(dst, pc n_vm);
            break;
         }

         case MAP_T_INS: {
            Type src = loadType(pc n_vm);
            traceLoad(src);
            Env& src_env = *(Env*)loadPtr(pc n_vm);
            Env& dst_env = *(Env*)loadPtr(pc n_vm);
            Type dst = mapType(src, src_env, dst_env);

#ifdef TRACE_EXTRA
            if (traceFlag) {
               outStream << "Mapped type ";
               printType(src, outStream, src_env);
               outStream << " to ";
               printType(dst, outStream, dst_env);
               outStream << endl;
            }
#endif

            storeType(dst, pc n_vm);
            break;
         }

         case MAP_CL_INS: {
            Cell* src = (Cell*)loadPtr(pc n_vm);
            traceLoad(src);
            Env* srcEnv = (Env*)loadPtr(pc n_vm);
            Env* dstEnv = (Env*)loadPtr(pc n_vm);
            Ptr& dst = lvPtr(pc n_vm);
            if (src != NULL && srcEnv != dstEnv) {
               push(pc, IAddress);
               pushLabel(entry(iContinuation));
               dst = mapClosure(src, *srcEnv, *dstEnv n_vm);
               drop(sizeof(IAddress) + sizeof(Label));
            } else
               dst = src;
            traceStore(dst);
            break;
         }

         case MAP_E_INS: {
            Expr src = loadExpr(pc n_vm);
            traceLoad(src);
            const Env* srcEnv = (const Env*)loadPtr(pc n_vm);
            Env* dstEnv = (Env*)loadPtr(pc n_vm);
            Expr& dst = lvExpr(pc n_vm);
            if (srcEnv != dstEnv) {
               push(pc, IAddress);
               pushLabel(entry(iContinuation));
               dst = Expr(src, *srcEnv, *dstEnv, dstEnv->msa() n_vm);
               drop(sizeof(IAddress) + sizeof(Label));
            } else
               dst = src;
            traceStore(dst);
            break;
         }

         case MAP_S_INS: {
            String src = (String)loadPtr(pc n_vm);
            traceLoad(src);
            Env* srcEnv = (Env*)loadPtr(pc n_vm);
            Env* dstEnv = (Env*)loadPtr(pc n_vm);
            Ptr& dst = lvPtr(pc n_vm);
            if (srcEnv != dstEnv) {
               push(pc, IAddress);
               pushLabel(entry(iContinuation));
               dst = mapString$(src, *srcEnv, *dstEnv);
               drop(sizeof(IAddress) + sizeof(Label));
            } else
               dst = src;
            traceStore(dst);
            break;
         }

         case MAP_SC_INS: {
            int offset = extInt4(pc);
            IAddress strPtr = pc + offset;
            Env* srcEnv = (Env*)loadPtr(pc n_vm);
            Env* dstEnv = (Env*)loadPtr(pc n_vm);
            Ptr& dst = lvPtr(pc n_vm);
            if (srcEnv != dstEnv) {
               push(pc, IAddress);
               pushLabel(entry(iContinuation));
               dst = mapString$((String)strPtr, *srcEnv, *dstEnv);
               drop(sizeof(IAddress) + sizeof(Label));
            } else
               dst = (String)strPtr;
            traceStore(dst);
            break;
         }

			case PCB_INS: {		// Push continuation and branch
            Int offset = extInt4(pc);
            stack(-(Int)sizeof(IAddress), IAddress) = pc;
            stack(-(Int)(sizeof(IAddress) + sizeof(Label)), Label) = entry(iContinuation);
            stackAlloc(sizeof(IAddress) + sizeof(Label));

#ifdef TRACE
            contCount++;
#endif

            pc += offset;
            break;
         }


         case BEQ_P_INS:
            execCondBranch(==,   Ptr);

         case BNE_P_INS:
            execCondBranch(!= , Ptr);

         case CEQ_P_INS: {
            Ptr x = loadPtr(pc n_vm);
            Ptr y = loadPtr(pc n_vm);
            storeTag(x == y ? 1 : 0, pc n_vm);
            break;
         }

         case CNE_P_INS: {
            Ptr x = loadPtr(pc n_vm);
            Ptr y = loadPtr(pc n_vm);
            storeTag(x != y ? 1 : 0, pc n_vm);
            break;
         }

         case BEQ_N_INS: {
            Name x = loadName(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Name y = loadName(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc); 
            if (eqName_(x, x_env.nameTable(), y, y_env.nameTable()))
               pc += offset;
            break;
         }

         case BNE_N_INS: {
            Name x = loadName(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Name y = loadName(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc);
            if (!eqName_(x, x_env.nameTable(), y, y_env.nameTable()))
               pc += offset;
            break;
         }

         case CEQ_N_INS: {
            Name x = loadName(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Name y = loadName(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            storeTag(eqName_(x, x_env.nameTable(), y, y_env.nameTable()) ? 1 : 0, pc n_vm);
            break;
         }

         case CNE_N_INS: {
            Name x = loadName(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Name y = loadName(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            storeTag(eqName_(x, x_env.nameTable(), y, y_env.nameTable()) ? 0 : 1, pc n_vm);
            break;
         }


         case BEQ_T_INS: {
            Type x = loadType(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Type y = loadType(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc);
            if (eqType_(x, x_env.typeTable(), y, y_env.typeTable()))
               pc += offset;
            break;
         }

         case BNE_T_INS: {
            Type x = loadType(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Type y = loadType(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc);
            if (!eqType_(x, x_env.typeTable(), y, y_env.typeTable()))
               pc += offset;
            break;
         }

         case CEQ_T_INS: {
            Type x = loadType(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Type y = loadType(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            storeTag(eqType_(x, x_env.typeTable(), y, y_env.typeTable()) ? 1 : 0, pc n_vm);
            break;
         }

         case CNE_T_INS: {
            Type x = loadType(pc n_vm);
            Env& x_env = *(Env*)loadPtr(pc n_vm);
            Type y = loadType(pc n_vm);
            Env& y_env = *(Env*)loadPtr(pc n_vm);
            storeTag(eqType_(x, x_env.typeTable(), y, y_env.typeTable()) ? 0 : 1, pc n_vm);
            break;
         }

         case INT_B8_INS:
            execCast(Int, Byte);
            break;

         case INT_B_INS:
            execCast(Int, Bits);
            break;

         case NEG_I_INS:
            execMonadic(-,       Int);

         case ADD_I_INS:
            execDyadic(+,        Int);

         case SUB_I_INS:
         case SUB_U_INS:
            execDyadic(-,        Int);

         case MUL_I_INS:
            execDyadic(*,        Int);

         case DIV_I_INS:
            execDyadic(/,        Int);

         case MOD_I_INS:
            execDyadic(%,        Int);


         case NOT_B_INS:
            execMonadic(~,       Bits);

         case AND_B_INS:
            execDyadic(&,        Bits);

         case IOR_B_INS:
            execDyadic(|,        Bits);

         case XOR_B_INS:
            execDyadic(^,        Bits);

         case LSH_B_INS:
            execDyadic(<<,       Bits);

         case RSH_B_INS:
            execDyadic(>>,       Bits);


         case CLT_I_INS:
            execCompare(<,       Int);

         case CLE_I_INS:
            execCompare(<=,      Int);

         case CEQ_I_INS:
            execCompare(==,      Int);

         case CNE_I_INS:
            execCompare(!=,      Int);

         case CGE_I_INS:
            execCompare(>=,      Int);
         
         case CGT_I_INS:
            execCompare(>,       Int);
         

         case BLT_I_INS:
            execCondBranch(<,    Int);

         case BLE_I_INS:
            execCondBranch(<=,   Int);

         case BEQ_I_INS:
            execCondBranch(==,   Int);

         case BNE_I_INS:
            execCondBranch(!=,   Int);

         case BGE_I_INS:
            execCondBranch(>=,   Int);
         
         case BGT_I_INS:
            execCondBranch(>,    Int);


         case LEN_S_INS: {
            String s = (String)loadPtr(pc n_vm);
            storeInt(strlen(s), pc n_vm);
            break;
         }
         
         case LEN_SC_INS: {
            int offset = extInt4(pc);
            IAddress strPtr = pc + offset;
            storeInt(strlen((String)strPtr), pc n_vm);
            break;
         }

         case GET_AT_S_INS: {
            String s = (String)loadPtr(pc n_vm);
            Int i = loadInt(pc n_vm);
            storeChar(s[i], pc n_vm);
            break;
         }

         case GET_AT_SC_INS: {
            int offset = extInt4(pc);
            IAddress strPtr = pc + offset;
            Int i = loadInt(pc n_vm);
            storeChar(((String)strPtr)[i], pc n_vm);
            break;
         }

         case PUT_AT_S_INS: {
            String s = (String)loadPtr(pc n_vm);
            Int i = loadInt(pc n_vm);
            Char c = loadChar(pc n_vm);
            s[i] = c;
            break;
         }

#if (GARBAGE_COLLECTION==1)

         case MRK_CL_INS: {
            Cell& cell_ = *static_cast<Cell*>(lvPtr(pc n_vm));

#ifdef TRACE
            if (traceFlag)
               outStream << "Marking cell: address " << &cell_ << endl;
#endif

            Label label = static_cast<CellInfo*>(cell_.tag())->cellGC_MarkFn();

            if (label == entry(iMarkCell)) {
               ICellInfo* iCellInfo = (ICellInfo*)static_cast<CellInfo*>(cell_.tag());
               IAddress iLabel = iCellInfo->_byteCodeGC_MarkCellEntry;
               if (iLabel == NULL) {
                  Label label = pop(Label);
                  jump(label);
               }

               pc = iLabel;
               break;
            }
            else {
               if (label == NULL)
                  label = pop(Label);
               jump(label);
            }
         }
 
         case MRK_P_BS_INS: {
            Ptr ptr = loadPtr(pc n_vm);
            traceLoad(ptr);
            Env* env = (Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc);
            if (&env->msa() == consMSA &&
               ((MSA_GC*)consMSA)->mark(ptr) == 1)
               pc += offset;
            break;
         }

         case MRK_P_BNS_INS: {
            Ptr ptr = loadPtr(pc n_vm);
            traceLoad(ptr);
            Env* env = (Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc);
            if (ptr != NULL && &env->msa() != consMSA ||
               ((MSA_GC*)consMSA)->mark(ptr) == 0)
               pc += offset;
            break;
         }

         case MRK_E_BS_INS: {
            Expr src = loadExpr(pc n_vm);
            Env* env = (Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc);
            if (isPtr(src) &&
               &env->msa() == consMSA &&
               ((MSA_GC*)consMSA)->mark((Cell*)src) == 1)
               pc += offset;
            break;
         }

         case MRK_E_BNS_INS: {
            Expr src = loadExpr(pc n_vm);
            Env* env = (Env*)loadPtr(pc n_vm);
            Int offset = extInt4(pc);
            if (!isPtr(src) ||
               &env->msa() != consMSA ||
               ((MSA_GC*)consMSA)->mark((Cell*)src) == 0)
               pc += offset;
            break;
         }

         case MRK_S_INS: {
            Ptr ptr = loadPtr(pc n_vm);
            Env* env = (Env*)loadPtr(pc n_vm);
 
            if (&env->msa() == consMSA)
               (Void)((MSA_GC*)consMSA)->mark(ptr);
            break;
         }

#endif

#if (SERIALISATION==1)
         case EXT_CL_INS: {
            InputStream_Byte& is = *static_cast<InputStream_Byte*>(loadPtr(pc n_vm));
            Env& isEnv = *(Env*)loadPtr(pc n_vm);
            Cell* cell_ = extractBinCell(is, isEnv n_vm);
            storePtr(cell_, pc);
            break;
         }
         case INS_CL_INS: {
            OutputStream_Byte& os = *static_cast<OutputStream_Byte*>(loadPtr(pc n_vm));
            Env& osEnv = *(Env*)loadPtr(pc n_vm);
            Cell& cell_ = *(Cell*)loadPtr(pc n_vm);
            Env& cellEnv_= *(Env*)loadPtr(pc n_vm);
            insertBinCell(os, osEnv, cell_, cellEnv_ n_vm);
            break;
         }
         case EXT_E_INS: {
            InputStream_Byte& is = *static_cast<InputStream_Byte*>(loadPtr(pc n_vm));
            Env& isEnv = *(Env*)loadPtr(pc n_vm);
            storeExpr(extractBinExpr(is, isEnv n_vm), pc n_vm);          
            break;
         }

         case INS_E_INS: {
            OutputStream_Byte& os = *static_cast<OutputStream_Byte*>(loadPtr(pc n_vm));
            Env& osEnv = *(Env*)loadPtr(pc n_vm);
            Expr expr = loadExpr(pc n_vm);
            Env& exprEnv = *(Env*)loadPtr(pc n_vm);
            insertBinExpr(os, osEnv, expr, exprEnv n_vm);
            break;
         }
#endif

         case STR_I_INS: {
            String s = (String)loadPtr(pc n_vm);
            storeInt(atoi(s), pc n_vm);
            break;
         }

         case STR_IC_INS: {
            int offset = extInt4(pc);
            String s = (String)pc + offset;
            storeInt(atoi(s), pc n_vm);
            break;
         }


         case MOV_F_INS:
            execMove(Float);


         case NEG_F_INS:
            execMonadic(-,       Float);

         case ADD_F_INS:
            execDyadic(+,        Float);

         case SUB_F_INS:
            execDyadic(-,        Float);

         case MUL_F_INS:
            execDyadic(*,        Float);

         case DIV_F_INS:
            execDyadic(/,        Float);

         case INT_F_INS:
            execCast(Int,        Float);

         case DBL_F_INS:
            execCast(Double,     Float);

         case CLT_F_INS:
            execCompare(<,       Float);

         case CLE_F_INS:
            execCompare(<=,      Float);

         case CEQ_F_INS:
            execCompare(==,      Float);

         case CNE_F_INS:
            execCompare(!=,      Float);

         case CGE_F_INS:
            execCompare(>=,      Float);
         
         case CGT_F_INS:
            execCompare(>,       Float);


         case BLT_F_INS:
            execCondBranch(<,    Float);

         case BLE_F_INS:
            execCondBranch(<=,   Float);

         case BEQ_F_INS:
            execCondBranch(==,   Float);

         case BNE_F_INS:
            execCondBranch(!=,   Float);

         case BGE_F_INS:
            execCondBranch(>=,   Float);

         case BGT_F_INS :
            execCondBranch(>,    Float);



         case MOV_D_INS:
            execMove(Double);


         case NEG_D_INS:
            execMonadic(-,       Double);

         case ADD_D_INS:
            execDyadic(+,        Double);

         case SUB_D_INS:
            execDyadic(-,        Double);

         case MUL_D_INS:
            execDyadic(*,        Double);

         case DIV_D_INS:
            execDyadic(/,        Double);


         case CLT_D_INS:
            execCompare(<,       Double);

         case CLE_D_INS:
            execCompare(<=,      Double);

         case CEQ_D_INS:
            execCompare(==,      Double);

         case CNE_D_INS:
            execCompare(!=,      Double);

         case CGE_D_INS:
            execCompare(>=,      Double);
         
         case CGT_D_INS:
            execCompare(>,       Double);


         case BLT_D_INS:
            execCondBranch(<,    Double);

         case BLE_D_INS:
            execCondBranch(<=,   Double);

         case BEQ_D_INS:
            execCondBranch(==,   Double);

         case BNE_D_INS:
            execCondBranch(!=,   Double);

         case BGE_D_INS:
            execCondBranch(>=,   Double);

         case BGT_D_INS:
            execCondBranch(>,    Double);


         case INT_D_INS:
            execCast(Int, Double);


         case STR_D_INS: {
            String s = (String)loadPtr(pc n_vm);
            storeDouble(strtod(s, NULL), pc n_vm);
            break;
         }

         case STR_DC_INS: {
            int offset = extInt4(pc);
            String s = (String)pc + offset;
            storeDouble(strtod(s, NULL), pc n_vm);
            break;
         }


         case CLT_O_INS:
            execCompare(<,       Tag);

         case CLE_O_INS:
            execCompare(<=,      Tag);

         case CEQ_O_INS:
            execCompare(==,      Tag);

         case CNE_O_INS:
            execCompare(!=,      Tag);

         case CGE_O_INS:
            execCompare(>=,      Tag);
         
         case CGT_O_INS:
            execCompare(>,       Tag);
         
         case BF_INS:
            execUnaryBranch(FALSE, Tag);

         case BT_INS:
            execUnaryBranch(TRUE, Tag);

         case BLT_O_INS:
            execCondBranch(<,    Tag);

         case BLE_O_INS:
            execCondBranch(<=,   Tag);

         case BEQ_O_INS:
            execCondBranch(==,   Tag);

         case BNE_O_INS:
            execCondBranch(!=,   Tag);

         case BGE_O_INS:
            execCondBranch(>=,   Tag);
         
         case BGT_O_INS:
            execCondBranch(>,    Tag);



         case CLT_C_INS:
            execCompare(<,       Char);

         case CLE_C_INS:
            execCompare(<=,      Char);

         case CEQ_C_INS:
            execCompare(==,      Char);

         case CNE_C_INS:
            execCompare(!=,      Char);

         case CGE_C_INS:
            execCompare(>=,      Char);
         
         case CGT_C_INS:
            execCompare(>,       Char);
         

         case BLT_C_INS:
            execCondBranch(<,    Char);

         case BLE_C_INS:
            execCondBranch(<=,   Char);

         case BEQ_C_INS:
            execCondBranch(==,   Char);

         case BNE_C_INS:
            execCondBranch(!=,   Char);

         case BGE_C_INS:
            execCondBranch(>=,   Char);
         
         case BGT_C_INS:
            execCondBranch(>,    Char);

         case BNP_INS:
            execTestBranch(==,   NULL, Ptr);

         case BNN_INS:
            execTestBranch(!= , NULL, Ptr);

         case BRA_INS: {
            int offset = extInt4(pc);

#ifdef TRACE
            if (traceFlag)
               outStream << "Branch offset " << offset << '\n';
#endif

            pc += offset;
            break;
         }

         case BSE_INS: {
            Expr x  = loadExpr(pc n_vm);
            Int offset = extInt4(pc);
            if (x.tag() != PTR_TAG)
               pc += offset;
            break;
         }

         case EFC_INS: {
            Cell* src = (Cell*)loadPtr(pc n_vm);
            traceLoad(src);
            storeExpr(Expr(src), pc n_vm);
            break;
         }

         case ETC_INS: {
            /* */ Expr expr = loadExpr(pc n_vm);
            Cell* cp = (Cell*)expr;
            storePtr(cp, pc n_vm);
            break;
         }

         case TAG_N_INS: {
            Name src       = loadName(pc n_vm);
            storeExpr(fromName(src), pc n_vm);
            break;
         }

         case TAG_T_INS: {
            Name src = loadName(pc n_vm);
            storeExpr(fromType(src), pc n_vm);
            break;
         }
 
         case TAG_C_INS: {
            Char src = loadChar(pc n_vm);
            storeExpr(fromChar(src), pc n_vm);
            break;
         }

         case TAG_F_INS: {
            Float src = loadFloat(pc n_vm);
            storeExpr(fromFloat(src, *consMSA), pc n_vm);
            break;
         }

         case TAG_D_INS: {
            Double src = loadDouble(pc n_vm);
            storeExpr(fromDouble(src, *consMSA), pc n_vm);
            break;
         }

         case TAG_I_INS: {
            Int src = loadInt(pc n_vm);
            traceLoad(src);
            storeExpr(fromInt(src, *consMSA), pc n_vm);
            break;
         }

         case TSC_INS: {
            int offset = extInt4(pc);
            IAddress strPtr = pc + offset;
            Expr& expr = lvExpr(pc n_vm);
            expr = fromString((String)strPtr, *consMSA);
            break;
         }

         case TAG_B_INS: {
            ByteString src = (ByteString)loadPtr(pc n_vm);
            storeExpr(fromByteString(src, *consMSA), pc n_vm);
            break;
         }

         case ALC_INS: {
            UInt size = loadUInt(pc n_vm);
            Ptr& ptr = lvPtr(pc n_vm);

            ptr = execMemAlloc(size,
               
#if (GARBAGE_COLLECTION==1)               
                               pc,
#endif  

                               *consMSA);

#ifdef TRACE
            if (traceFlag) {
               outStream << "Allocating heap memory: ";
               showMemory(ptr, 0, size);
               outStream.flush();
            }
#endif

            break;
         }

         case ASC_INS: {
            UInt n = unpackVLU(&pc);
            if (n > 0) {
               Int offset = extInt4(pc);
               UInt d = (unsigned char*)sb - (unsigned char*)sp;
               IAddress addr = pc + offset;
               while (n > 0) {
                  UInt pd = extInt4(addr);
                  Int offset = extInt4(addr);
                  if (n == 1 || d == pd) {
                     pc += offset;
                     break;
                  }
                  n--;
               }          
            }
            break;
         }

         case UPD_INS: {
            Cell* src = (Cell*)loadPtr(pc n_vm);
            traceAddr(src);
            Cell* dst = (Cell*)loadPtr(pc n_vm);
            traceAddr(dst);

            CellInfo& src_ci = *static_cast<CellInfo*>(src->tag());
            CellInfo& dst_ci = *static_cast<CellInfo*>(dst->tag());
            if (src_ci.size() <= MIN_CLOSURE_SIZE || src_ci.size() <= dst_ci.size())
               memcpy(dst, src, src_ci.size() + sizeof(Expr));  // In-place update
            else {
               size_t indirectionSize = sizeof(Cell) - MAX_CELL_BODY_SIZE + sizeof(Indirection);
               if (static_cast<CellInfo*>(dst->tag()) == &indirectionCellInfo) {
                  cellBody(*dst, Indirection)._cell = src;
               }
               else {
                  Void* ptr = execMemAlloc(indirectionSize,

#if (GARBAGE_COLLECTION==1)
                     pc,
#endif

                     * consMSA);
                  Cell& cell_ = *new(ptr) Cell(&indirectionCellInfo);
                  cellBody(cell_, Indirection)._cell = src;
                  memcpy(dst, &cell_, indirectionSize);
               }
               if (traceFlag)
                  showMemory(dst, 0, indirectionSize);
            }
            break;
         }

#if (GARBAGE_COLLECTION==1)
         case AFD_INS: {   // Activation frame descriptor
            Int labAssocOffset = extInt4(pc);
            UInt labSeqNo = unpackVLU(&pc); //???
            UInt d = unpackVLU(&pc);
            UInt nSlots = unpackVLU(&pc);   
            FrameDescr<IAddress>* frameDescr = new (*consMSA) FrameDescr<IAddress>();
            FrameDescrSlot<IAddress>* slots = nSlots > 0
               ? (FrameDescrSlot<IAddress>*)consMSA->alloc(sizeof(FrameDescrSlot<IAddress>) * nSlots)
               : NULL;
            frameDescr->setLabSeqNo(labSeqNo); //???
            frameDescr->setSize(d);
            frameDescr->setN_slots(nSlots);
            frameDescr->setSlots(slots);
            for (UInt i = 0; i < nSlots; i++) {
               FrameDescrSlot<IAddress>& slot = frameDescr->slot(i);
               Name name = extBits16(pc);
               slot.setName(name);
               Int offset = extInt4(pc);
               slot.setAddress(pc + offset);
            }
            frameDescr->setAddress(pc + labAssocOffset);

#ifdef TRACE_EXTRA
            if (traceFlag) {
               outStream << "Adding frame descriptor label " << (Void*)(pc + labAssocOffset) << endl;
               outStream << frameDescr->labSeqNo() << endl;
            }
#endif

            MSAFrameDescrMapAssoc* assoc = MSAFrameDescrMapAssoc::lookUp(consMSA);
            if (assoc == NULL)
               assoc = MSAFrameDescrMapAssoc::add(consMSA);
            assoc->frameDescrMap().add(frameDescr, reinterpret_cast<uintptr_t>(pc + labAssocOffset), *consMSA);
            break;
         }
#endif

			case ACI_INS: {   // Allocate CellInfo
            Int entryLabOffset   =     extInt4(pc);
            Int altEntryLabOffset =    extInt4(pc);
            Int copyFnLabOffset =      extInt4(pc);

#if(SERIALISATION==1)
            Int extractBinFnLabOffset =   extInt4(pc);
            Int insertBinFnLabOffset =    extInt4(pc);
#endif

#if (GARBAGE_COLLECTION==1)
            Int gcMarkFnLabOffset = extInt4(pc);
#endif

            UInt size =             unpackVLU(&pc);
            Int segmentOffset =     extInt4(pc);
            Int dataOffset =        extInt4(pc);
            ICellInfo* cellInfo_   = allocCellInfo(pc + entryLabOffset,
                                                   pc + altEntryLabOffset,
                                                   copyFnLabOffset == 0 ? 0 : pc + copyFnLabOffset,

#if (SERIALISATION==1)
                                                   extractBinFnLabOffset == 0 ? 0 : pc + extractBinFnLabOffset,
                                                   insertBinFnLabOffset == 0 ? 0 : pc + insertBinFnLabOffset,
#endif

#if (GARBAGE_COLLECTION==1)
                                                   gcMarkFnLabOffset == 0 ? 0 : pc + gcMarkFnLabOffset,
#endif

                                                   size,
                                                   pc + segmentOffset,
                                                   pc,
                                                   *consMSA);
#ifdef TRACE_EXTRA
            if (traceFlag) {
               outStream << "Allocated cell info:" << endl;
               showMemory(cellInfo_, 0, sizeof(ICellInfo));
               outStream << endl;
            }
     
#endif

            insBits32(pc + dataOffset, (Bits32)cellInfo_);
            break;
         }

         case CIO_INS: {
            Cell* cell_ = (Cell*)loadPtr(pc n_vm);
            traceAddr(cell_);
            storePtr((CellInfo*)cell_->tag(), pc n_vm);
            break;
         }

         case ACL_INS: {   // Allocate closure
            UInt size = unpackVLU(&pc);
            const ICellInfo* cellInfo = (ICellInfo*)loadPtr(pc n_vm);
            Ptr& ptr = lvPtr(pc n_vm);
            ptr = consEnv->segmentTable() != NULL
               ? allocClosure(size, copyICellInfo(cellInfo, *consEnv), pc, *consMSA)
               : allocClosure(size, cellInfo, pc, *consMSA);

#ifdef TRACE
            if (traceFlag) {
               outStream << " Allocating closure (" << size << " bytes) : ";
               showMemory(ptr, 0, sizeof(CellInfo*) + size);
               outStream << endl;
               showMemory(cellInfo, 0, sizeof(ICellInfo));
               outStream.flush();
            }
#endif

            break;
         }

         case ACL_ENV_INS: {   // Allocate closure in given environment
            UInt size      = unpackVLU(&pc);
            const ICellInfo* cellInfo = (ICellInfo*)loadPtr(pc n_vm); 
            Env& env = *(Env*)loadPtr(pc n_vm);
            Ptr& ptr = lvPtr(pc n_vm);
            cellInfo = copyICellInfo(cellInfo, env);
            ptr = allocClosure(size, cellInfo, pc, env.msa());

#ifdef TRACE
            if (traceFlag) {
               outStream << "Allocating closure (" << size << " bytes) : ";
               showMemory(ptr, 0, sizeof(CellInfo*) + size);
               outStream << "Cell info: ";
               showMemory(cellInfo, 0, sizeof(ICellInfo));
               outStream.flush();
            }
#endif

            break;
         }

         case AIN_INS: {
            Ptr& ptr = lvPtr(pc n_vm);
            ptr = execMemAlloc(sizeof(Cell) - MAX_CELL_BODY_SIZE + sizeof(Indirection),

#if (GARBAGE_COLLECTION==1)
                               pc,
#endif

                               *consMSA);
            Cell& cell_ = *new(ptr) Cell(&indirectionCellInfo);
            cellBody(cell_, Indirection)._cell = NULL;

#ifdef TRACE
            if (traceFlag) {
               outStream << "Allocating indirection cell: ";
               showMemory(ptr, 0, sizeof(Cell) - MAX_CELL_BODY_SIZE + sizeof(Indirection));
               outStream.flush();
            }
#endif

            break;
         }

//------ Allocate pointer array

         case APA_INS: {
            Int n = loadInt(pc n_vm);
            UInt size = unpackVLU(&pc);
            Ptr& ptr = lvPtr(pc n_vm);
            size_t reqLen = size * n;
            if (reqLen == 0) {
               ptr = NULL;
               break;
            }

            ptr = execMemAlloc(reqLen,

#if (GARBAGE_COLLECTION==1)               
               pc,
#endif  

               *consMSA);

#ifdef TRACE
            if (traceFlag) {
               outStream << "Allocated pointer array memory: ";
               showMemory(ptr, 0, n * size);
               outStream.flush();
            }
#endif

            break;
         }

         case ENT_E_INS: {
            Expr expr = loadExpr(pc n_vm);
            Env* env = (Env*)loadPtr(pc n_vm);
            enterReturn(expr, env n_vm);
            break;
         }
     
         case ENT_P_INS: {
            Cell& cell_ = *static_cast<Cell*>(lvPtr(pc n_vm));
            CellInfo& ci = *static_cast<CellInfo*>(cell_.tag());

#ifdef TRACE
            if (traceFlag) {
               outStream << "About to enter cell: ";
               showMemory(&cell_, 0, 16);   
               showMemory(&ci, 0, ci.entry_() != entry(iClosure)
                                       ? sizeof(CellInfo)
                                       : sizeof(ICellInfo));
               outStream << "Entry point: "
                  << (ci.entry_() != entry(iClosure)
                        ? (Void*)ci.entry_()
                        : (Void*)(static_cast<ICellInfo&>(ci))._byteCodeEntry)
                  << endl;
            }
#endif

            if (ci.entry_() != entry(iClosure))
               jump(ci.entry_());
            else
               pc = static_cast<ICellInfo&>(ci)._byteCodeEntry; 
            break;
         }

         case ENT_A_P_INS: {
            Cell& cell_ = *static_cast<Cell*>(lvPtr(pc n_vm));
            CellInfo& ci = *static_cast<CellInfo*>(cell_.tag());

#ifdef TRACE
            if (traceFlag) {
               outStream << "About to enter cell: ";
               showMemory(&cell_, 0, 16);
               showMemory(&ci, 0, 16);
               outStream << "Alternative entry point: ";
               if (ci.entry_() != altEntry(iClosure))
                  outStream << ci.altEntry_();
               else
                  outStream << static_cast<Void*>((static_cast<ICellInfo&>(ci))._byteCodeAltEntry);
               outStream << endl;
            }
#endif

            if (ci.altEntry_() != altEntry(iClosure))
               jump(ci.altEntry_());
            else
               pc = static_cast<ICellInfo&>(ci)._byteCodeAltEntry;
            break;
         }

         case JMP_INS: {

#ifdef TRACE
            Label lab = loadLabel(pc n_vm);
            if (traceFlag)
               outStream << "Jump to " << (Void*)lab << endl;
            jump(lab);
#else
            jump(loadLabel(pc n_vm));
#endif

         }

         case RET_INS:
            pc = pop(IAddress);
            break;

         case RTN_INS: {
            Label label = pop(Label);
            if (label == entry(iContinuation)) {
               IAddress iLabel = pop(IAddress);

#ifdef TRACE
               UInt contCount_ = contCount--;
               if (traceFlag) //???
                  outStream << "Return (" << contCount_ << "): address " << (Void*)(iLabel) << endl;
#endif

               pc = iLabel;
               break;
            } else {

#ifdef TRACE
               if (traceFlag) {
                  outStream << "Return: label " << label;
                  outStream << endl;
               }
#endif

               jump(label);
            }
         }

         case EXIT_INS:
            return NULL;

         case ATB_N_INS:
            execIndirectTableBranch(Name, env->nameTable(), codeEnv->nameTable());

         case ATB_T_INS:
            execIndirectTableBranch(Type, *env, *codeEnv);

         case ATB_I_INS:
            execTableBranch(Int);
         
         case ATB_O_INS:
            //execTableBranch(Tag);
         { Tag x = loadTag(pc n_vm);\
            Int offset = extInt4(pc);\
            UInt n = unpackVLU(&pc);\
            pc += tableBranchOffsetTag(x, pc + offset, n);break;}

         case ATB_C_INS:
            execTableBranch(Char);

         case ATB_S_INS:
            execStringTableBranch();

         case ATB_CS_INS:
            execConstStringTableBranch();

         case OTY_INS: {
            Ref src        = loadRef(pc n_vm);
            Env* src_env   = (Env*)loadPtr(pc n_vm); 
            Type& dst      = lvType(pc n_vm);\
            Env** dst_env  = (Env**)&lvPtr(pc n_vm);
            dst = objectType(src, *(BasicADS_Instance*)src_env);
            *dst_env = NULL;
            break;
         }

			case RRL_INS: {	// Reverse reference list
            Ref src        = loadRef(pc n_vm);
            Env* src_env   = (Env*)loadPtr(pc n_vm); 
            RefList** dst  = (RefList**)&lvPtr(pc n_vm);
            Env** dst_env  = (Env**)&lvPtr(pc n_vm);
            *dst = &lockObject(src, *(BasicADS_Instance*)src_env)->_revRefs;
            *dst_env = src_env;
            break;
         }

         case ERR_INS:     // error
            error((String)loadPtr(pc n_vm));
            return NULL;

         case ECP_INS: {   // error (constant string)
            int offset = extInt4(pc);
            error((String)(pc + offset));
            return NULL;
         }

			case TRC_INS: {	// trace
            String s   = (String)loadPtr(pc n_vm);
            trace(s);
            break;
         }

         case TCP_INS: { // trace (constant string)
            int offset = extInt4(pc);
            trace((String)(pc + offset));
            break;
         }

         case SET_ENV_INS: {  // Set constructor environment
            Env* env = (Env*)loadPtr(pc n_vm);
            consEnv = env;
            consMSA = &consEnv->msa();
            break;
         }

          case RSS_INS: {

//--------- Reserve space for structure, not subject to garbage collection

            size_t size = unpackVLU(&pc);
            if (size > rStructSize) {
               consMSA->free(rStruct);
               rStruct = (Byte*)checkMemAlloc(consMSA->alloc(size));
               rStructSize = size;
            }
            break;
         }

         case SIZE_OF_INS: {
            Type type = loadType(pc n_vm);
            Env& env = *(Env*)loadPtr(pc n_vm);
            TypeDescr* typeDescr = type < builtInTypeCount
               ? builtInTypeTable().typeDescr(type)
               : env.typeTable().typeDescr(type - builtInTypeCount);
            storeInt(typeDescr->_size, pc n_vm);
            break;
         }

         case CPA_INS: {
            Int nElem = loadInt(pc n_vm);
            size_t elemSize = unpackVLU(&pc);
            Ptr ptr1 =    loadPtr(pc n_vm);
            Int offset1 = loadInt(pc n_vm);
            Ptr ptr2 =    loadPtr(pc n_vm);
            Int offset2 = loadInt(pc n_vm);
            memcpy((Byte*)ptr2 + offset2 * elemSize,   // Assumes non-overlapping
                     (Byte*)ptr1 + offset1 * elemSize,
                     nElem * elemSize);
            break;
         }

         case EXC_INS:
            runTimeError("execByteCode: exception");

         case ILG_INS:
            runTimeError("execByteCode: illegal instruction");

         default:
            error1("execByteCode: invalid opcode %d", opCode);
      }
   } while (TRUE);
   return NULL;
}

#endif

