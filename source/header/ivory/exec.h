/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    exec.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
*
*-----------------------------------------------------------------------------
*
* Description:
*
*    Definitions for use with Ivory byte code interpreter.
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

#ifndef IVORY_EXEC_H_DEFINED
#define IVORY_EXEC_H_DEFINED

#include "./segmentTable.h"
#include "ivory/instructions.h"
#include "ivory/machine.h"
#include "ivory/trace.h"
#include "ivory/type.h"

#define rEnvPtr rEnv

#ifdef TRACE_EXTRA
#define traceMsg(s) if(traceFlag){outStream<<s<<'\n';outStream.flush();}
#define traceLoad(x) if(traceFlag){outStream<<"Loaded "<<(x)<<'\n';outStream.flush();}
#define traceStore(x) if(traceFlag){outStream<<"Stored "<<(x)<<'\n';outStream.flush();}
#define traceOffset(offset) if(traceFlag){outStream<<"Offset "<<(offset)<<'\n';outStream.flush();}
#define traceAddr(addr) if(traceFlag){outStream<<"Address "<<(Void*)(addr)<<'\n';outStream.flush();}
#else
#define traceLoad(x)
#define traceStore(x)
#define traceOffset(offset)
#define traceAddr(addr)
#endif

#define IVORY_STACK_SIZE 16*1024

#define ICODE_DEBUG 1

#define fetchByte(pc)      (*(pc)++)

#ifdef TRACE_EXTRA
#ifndef TRACE_FLAG_DEFINED
static Bool traceFlag = FALSE;
#define TRACE_FLAG_DEFINED
#endif
UInt traceAddrMode(Byte arg) {
   UInt mode = (arg & ADDR_MODE_MASK) >> ADDR_MODE_OFFSET;
   if (traceFlag) {
      switch (mode) {
      case IMM_ADDR_MODE:     outStream << "Immediate"; break;
      case MEM_ADDR_MODE:     outStream << "Memory relative"; break;
      case STACK_ADDR_MODE:   outStream << "Stack relative"; break;
      case CELL_ADDR_MODE:    outStream << "Cell relative"; break;
      case RACK_ADDR_MODE:    outStream << "Rack relative"; break;
      case REG_ADDR_MODE:     outStream << "Register"; break;
      case IND_ADDR_MODE:     outStream << "Indirect"; break;
      case EXT_ADDR_MODE:
         switch ((arg & EXT_ADDR_MODE_MASK) >> EXT_ADDR_MODE_OFFSET) {
         case IMM_1_EXT_ADDR_MODE:     outStream << "Immediate(1)"; break;
         case IMM_2_EXT_ADDR_MODE:     outStream << "Immediate(2)"; break;
         case IMM_4_EXT_ADDR_MODE:     outStream << "Immediate(4)"; break;
         case IMM_8_EXT_ADDR_MODE:     outStream << "Immediate(8)"; break;

         case MEM_1_EXT_ADDR_MODE:     outStream << "Memory relative(1)"; break;
         case MEM_2_EXT_ADDR_MODE:     outStream << "Memory relative(2)"; break;
         case MEM_4_EXT_ADDR_MODE:     outStream << "Memory relative(4)"; break;
         case MEM_8_EXT_ADDR_MODE:     outStream << "Memory relative(8)"; break;

         case STACK_1_EXT_ADDR_MODE:   outStream << "Stack relative(1)"; break;
         case STACK_2_EXT_ADDR_MODE:   outStream << "Stack relative(2)"; break;
         case STACK_4_EXT_ADDR_MODE:   outStream << "Stack relative(4)"; break;
         case STACK_8_EXT_ADDR_MODE:   outStream << "Stack relative(8)"; break;

         case CELL_1_EXT_ADDR_MODE:    outStream << "Cell relative(1)"; break;
         case CELL_2_EXT_ADDR_MODE:    outStream << "Cell relative(2)"; break;
         case CELL_4_EXT_ADDR_MODE:    outStream << "Cell relative(4)"; break;
         case CELL_8_EXT_ADDR_MODE:    outStream << "Cell relative(8)"; break;

#if (FRAME_DESCRIPTORS==1)
         case FRAME_1_EXT_ADDR_MODE:   outStream << "Frame relative(1)"; break;
         case FRAME_2_EXT_ADDR_MODE:   outStream << "Frame relative(2)"; break;
         case FRAME_4_EXT_ADDR_MODE:   outStream << "Frame relative(4)"; break;
         case FRAME_8_EXT_ADDR_MODE:   outStream << "Frame relative(8)"; break;
#endif

         case RACK_1_EXT_ADDR_MODE:    outStream << "Rack relative(1)"; break;
         case RACK_2_EXT_ADDR_MODE:    outStream << "Rack relative(2)"; break;
         case RACK_4_EXT_ADDR_MODE:    outStream << "Rack relative(4)"; break;
         case RACK_8_EXT_ADDR_MODE:    outStream << "Rack relative(8)"; break;

         case IND_1_EXT_ADDR_MODE:     outStream << "Indirect(1)"; break;
         case IND_2_EXT_ADDR_MODE:     outStream << "Indirect(2)"; break;
         case IND_4_EXT_ADDR_MODE:     outStream << "Indirect(4)"; break;
         case IND_8_EXT_ADDR_MODE:     outStream << "Indirect(8)"; break;

         case INDEXED_1_EXT_ADDR_MODE: outStream << "Indexed(1)"; break;
         case INDEXED_2_EXT_ADDR_MODE: outStream << "Indexed(2)"; break;
         case INDEXED_4_EXT_ADDR_MODE: outStream << "Indexed(4)"; break;
         case INDEXED_8_EXT_ADDR_MODE: outStream << "Indexed(8)"; break;

         default:                      outStream << "Invalid extended address mode"; break;
         }
         break;
      }
      outStream << '\n';
   }
   return mode;
}

#define addrMode           traceAddrMode
#else
#define addrMode(arg)      (((arg)&ADDR_MODE_MASK)>>ADDR_MODE_OFFSET)
#endif
#define extAddrMode(arg)   (((arg)&EXT_ADDR_MODE_MASK)>>EXT_ADDR_MODE_OFFSET)
#define immVal(arg)        (((arg)&IMM_VAL_MASK)>>IMM_VAL_OFFSET)
#define regId(arg)         (((arg)&REG_ID_MASK)>>REG_ID_OFFSET)

#define extEnvPtr(addr)    ((EnvPtr)extBits32(addr))

#define immExtEnvPtr4(pc)  ((EnvPtr)extBits32(pc))

#define extExpr(addr)      ((Expr)extBits32(addr))

#define immExtExpr4(pc)    ((Expr)extBits32(pc))

#define immExtByte(pc)     fetchByte(pc)
#define immExtByte1(pc)    immExtByte(pc)

#define extByte(addr)      fetchByte(pc)

#define extBits(addr)      fetchByte(pc)

#define immExtBits(pc)     fetchByte(pc)
#define immExtBits1(pc)    extBits1(pc)
#define immExtBits2(pc)    extBits2(pc)
#define immExtBits4(pc)    extBits4(pc)
#define immExtBits8(pc)    extBits8(pc)

#define extBits1(pc)       ((Bits)fetchByte(pc))
#define extBits2(pc)       ((Bits)extBits16(pc))
#define extBits4(pc)       ((Bits)extBits32(pc))
#define extBits8(pc)       ((Bits)extBits64(pc))

#define immUInt(arg)       ((UInt)immVal(arg))

#define extUInt1(pc)       ((UInt)fetchByte(pc))
#define extUInt2(pc)       ((UInt)extBits16(pc))
#define extUInt4(pc)       ((UInt)extBits32(pc))
#define extUInt8(pc)       ((UInt64)extBits64(pc))

#define extUInt(addr)      ((UInt)extBits32(addr))

#define immExtUInt1(pc)    extUInt1(pc)
#define immExtUInt2(pc)    extUInt2(pc)
#define immExtUInt4(pc)    extUInt4(pc)
#define immExtUInt8(pc)    extUInt8(pc)

#define extInt1(pc)        ((Int)(Int8)fetchByte(pc))
#define extInt2(pc)        ((Int)(Int16)extBits16(pc))
#define extInt4(pc)        ((Int)(Int32)extBits32(pc))
#define extInt8(pc)        ((Int64)extBits64(pc))

#define extInt(addr)       ((Int)(Int32)extBits32(addr))

#define immExtInt1(pc)     extInt1(pc)
#define immExtInt2(pc)     extInt2(pc)
#define immExtInt4(pc)     extInt4(pc)
#define immExtInt8(pc)     extInt8(pc)

#define immExtChar(pc)     ((Char)fetchByte(pc))
#define immExtChar1(pc)    immExtChar(pc)

#define extChar(addr)      ((Char)fetchByte(addr))

#define immExtDouble8(pc)  extDouble(pc)

#define immExtFloat4(pc)   ((Float)extBits32(pc))

#define extFloat(addr)     ((Float)extBits32(addr))

#define immExtName2(pc)    ((Name)extBits16(pc))

#define extName(addr)      ((Name)extBits16(addr))

#define immExtType2(pc)    ((Name)extBits16(pc))

#define extType(addr)      ((Type)extBits16(addr))

#define immTag(arg)        ((Tag)immVal(arg))

#define extTag1(pc)        ((Tag)fetchByte(pc))
#define extTag2(pc)        ((Tag)extBits16(pc))

#define immExtTag2(pc)     extTag(pc)

#define extTag(addr)       ((Tag)extBits16(addr))

#define extLabel(pc)       ((Label)extBits32(pc))

#define extIAddress(pc)    ((IAddress)extBits32(pc))


#define immExtLabel4(pc)   extLabel(pc)

#define immExtIAddress4(pc)   extIAddress(pc)


#define extRef(pc)         ((Ref)extBits32(pc))

#define immExtRef4(pc)     extRef(pc)

#define extPtr(addr)       ((Ptr)extBits32(addr))

#define immExtPtr4(pc)     ((Ptr)extBits32(pc))

#define retNameReg(arg)    return rName
#define retTypeReg(arg)    return rType
#define retCellReg(arg)    return rCell
#define retExprReg(arg)    return rExpr
#define retByteReg(arg)    return rByte
#define retBitsReg(arg)    return rBits
#define retIntReg(arg)     return rInt
#define retFloatReg(arg)   return rFloat
#define retDoubleReg(arg)  return rDouble
#define retTagReg(arg)     return rTag
#define retCharReg(arg)    return rChar
#define retRefReg(arg)     return rRef

#define retRefReg(arg)     return rRef

#define retPtrReg(arg)\
switch (arg){\
case REG_consMSA:    return *((Void**)&consMSA);\
case REG_consEnv:    return *((Void**)&consEnv);\
case REG_sb:         return *((Void**)&sb);\
case REG_sp:         return *((Void**)&sp);\
case REG_rack:       return *((Void**)&rack);\
case REG_cell:       return *((Void**)&cell);\
case REG_cellEnv:    return *((Void**)&cellEnv);\
case REG_rCell:      return *((Void**)&rCell);\
case REG_rStruct:    return *((Void**)&rStruct);\
case REG_rPtr:       return *((Void**)&rPtr);\
case REG_rEnv:       return *((Void**)&rEnv);\
default:return*((Void**)NULL);}\

#define retLabelPairReg(arg) return (*(LabelPair*)NULL)

#define retUIntReg(arg)

#define retLabelReg(arg)

#define retIAddressReg(arg)

#define immExt(n,type)\
case IMM_##n##_EXT_ADDR_MODE:\
return immExt##type##n(pc);

#define immExtAddrModeLabel()\
immExt(4,Label);

#define immExtAddrModeIAddress()\
immExt(4,IAddress);

#define immExtAddrModeEnvPtr()\
immExt(4,EnvPtr);

#define immExtAddrModeName()\
immExt(2,Name);

#define immExtAddrModeType()\
immExt(2,Type);

#define immExtAddrModeTag()\
immExt(2,Tag);\

#define immExtAddrModePtr()\
immExt(4,Ptr);

#define immExtAddrModeExpr()\
immExt(4,Expr);

#define immExtAddrModeChar()\
immExt(1,Char);\

#define immExtAddrModeByte()\
immExt(1,Byte);

#if (IVORY_64_BIT==1)
#define immExtAddrModeBits()\
immExt(1,Bits);\
immExt(2,Bits);\
immExt(4,Bits);\
immExt(8,Bits);
#else
#define immExtAddrModeBits()\
immExt(1,Bits);\
immExt(2,Bits);\
immExt(4,Bits);
#endif

#if (IVORY_64_BIT==1)
#define immExtAddrModeUInt()\
immExt(1,UInt);\
immExt(2,UInt);\
immExt(4,UInt);\
immExt(8,UInt);
#else
#define immExtAddrModeUInt()\
immExt(1,UInt);\
immExt(2,UInt);\
immExt(4,UInt);
#endif

#if (IVORY_64_BIT==1)
#define immExtAddrModeInt()\
immExt(1,Int);\
immExt(2,Int);\
immExt(4,Int);\
immExt(8,Int);
#else
#define immExtAddrModeInt()\
immExt(1,Int);\
immExt(2,Int);\
immExt(4,Int);
#endif

#define immExtAddrModeFloat()\
immExt(4,Float);

#define immExtAddrModeDouble()\
immExt(8,Double);

#define immExtAddrModeRef()\
immExt(4,Ref);

// Stack extended 

#define stackExt(n,type)\
case STACK_##n##_EXT_ADDR_MODE:\
{Int64 offset=extInt##n(pc);\
traceOffset(offset);\
return stack(offset,type);}

#define stackExtAddrMode(type)\
stackExt(1,type);\
stackExt(2,type);\
stackExt(4,type);\
stackExt(8,type);

// Memory extended

#define memExt(n,type)\
case MEM_##n##_EXT_ADDR_MODE:{\
Int64 offset=extInt##n(pc);\
IAddress addr=pc+offset;\
traceOffset(offset);\
traceAddr(addr);\
return ext##type(addr);}

#define memExtAddrMode(type)\
memExt(1,type);\
memExt(2,type);\
memExt(4,type);\
memExt(8,type);

#define memLVExt(n,type)\
case MEM_##n##_EXT_ADDR_MODE:{\
Int64 offset=extInt##n(pc);\
traceOffset(offset);\
traceAddr(pc + offset);\
return indexed(pc,offset,type);}

#define memLVExtAddrMode(type)\
memLVExt(1,type);\
memLVExt(2,type);\
memLVExt(4,type);\
memLVExt(8,type);

// Cell extended 

#define cellExt(n,type)\
case CELL_##n##_EXT_ADDR_MODE:\
{UInt64 offset = extUInt##n(pc);\
traceOffset(offset);\
return indexed(cell,offset,type);}\

#define cellExtAddrMode(type)\
cellExt(1,type);\
cellExt(2,type);\
cellExt(4,type);\
cellExt(8,type);

// Rack extended

#define rackExt(n,type)\
case RACK_##n##_EXT_ADDR_MODE:{\
UInt64 offset=extUInt##n(pc);\
traceOffset(offset);\
return indexed(rack,offset,type);}

#define rackExtAddrMode(type)\
rackExt(1,type);\
rackExt(2,type);\
rackExt(4,type);\
rackExt(8,type);

#if (FRAME_DESCRIPTORS==1)
// Frame extended 

#define frameExt(n,type)\
case FRAME_##n##_EXT_ADDR_MODE:\
{UInt64 offset=extUInt##n(pc);\
traceOffset(offset);\
return frame(offset,type);}

#define frameExtAddrMode(type)\
frameExt(1,type);\
frameExt(2,type);\
frameExt(4,type);\
frameExt(8,type);
#else
#define frameExtAddrMode(type)
#endif

extern Ptr loadPtr(IAddress& pc argN_VM);
extern Int loadInt(IAddress& pc argN_VM);

// Indirect 

#define ind(type){\
UInt offset = immUInt(arg);\
return indexed(loadPtr(pc n_vm),offset,type);}

#define indExt(n,type)\
case IND_##n##_EXT_ADDR_MODE:{\
UInt64 offset=extUInt##n(pc);\
return indexed(loadPtr(pc n_vm),offset,type);}

#define indExtAddrMode(type)\
indExt(1,type);\
indExt(2,type);\
indExt(4,type);\
indExt(8,type);

// Indexed extended 
// INDEXED_<n>_EXT_ADDR_MODE,<offset>,<element size>,<base address mode>,<index address mode>, 

#define indexedExt(n, type)\
case INDEXED_##n##_EXT_ADDR_MODE:{\
UInt64 offset=extUInt##n(pc);\
UInt elemSize=unpackVLU(&pc);\
Byte* base = &lvByte(pc n_vm);\
Int i=loadInt(pc n_vm);\
traceOffset(offset);\
traceLoad(i);\
traceAddr(base+i*elemSize+offset);\
return indexed(base,i*elemSize+offset,type);}

#define indexedExtAddrMode(type)\
indexedExt(1,type);\
indexedExt(2,type);\
indexedExt(4,type);\
indexedExt(8,type);

#define loadT(type)\
type load##type(IAddress& pc argN_VM){\
unsigned char arg;\
switch(addrMode((arg=fetchByte(pc)))){\
case IMM_ADDR_MODE:\
return imm##type(arg);\
case STACK_ADDR_MODE:\
{Int offset = immInt(arg);\
traceOffset(offset); \
return stack(offset,type);}\
case CELL_ADDR_MODE:\
{UInt offset=immUInt(arg);\
traceOffset(offset);\
return indexed(cell,offset,type);}\
case RACK_ADDR_MODE:\
return indexed(rack,immUInt(arg),type);\
case REG_ADDR_MODE:\
ret##type##Reg(regId(arg));\
case IND_ADDR_MODE:\
ind(type);\
default:\
case EXT_ADDR_MODE:\
switch (extAddrMode(arg)){\
indexedExtAddrMode(type);\
immExtAddrMode##type();\
memExtAddrMode(type);\
stackExtAddrMode(type);\
cellExtAddrMode(type);\
frameExtAddrMode(type);\
rackExtAddrMode(type);\
default:\
indExtAddrMode(type);}}}


// Left value type macro

#define lvT(type)\
type& lv##type(IAddress& pc argN_VM){\
unsigned char arg;\
switch (addrMode(arg=fetchByte(pc))){\
case STACK_ADDR_MODE:\
{Int offset = immInt(arg); \
traceOffset(offset); \
return stack(offset, type); }\
case CELL_ADDR_MODE:\
return indexed(cell,immUInt(arg),type);\
case RACK_ADDR_MODE:\
return indexed(rack,immUInt(arg),type);\
case REG_ADDR_MODE:\
ret##type##Reg(regId(arg));\
case IND_ADDR_MODE:\
ind(type);\
default:\
case EXT_ADDR_MODE:\
switch (extAddrMode(arg)){\
indexedExtAddrMode(type);\
memLVExtAddrMode(type);\
stackExtAddrMode(type);\
frameExtAddrMode(type);\
cellExtAddrMode(type);\
rackExtAddrMode(type);\
default:\
indExtAddrMode(type);}}}

// lvT_ExclReg: Similar to lvT but excludes register

#define lvT_ExclReg(type)\
type& lv##type(IAddress& pc argN_VM){\
unsigned char arg;\
switch (addrMode(arg=fetchByte(pc))){\
case STACK_ADDR_MODE:\
return stack(immInt(arg),type);\
case CELL_ADDR_MODE:\
return indexed(cell,immUInt(arg),type);\
case RACK_ADDR_MODE:\
return indexed(rack,immUInt(arg),type);\
case IND_ADDR_MODE:\
ind(type);\
default:\
case EXT_ADDR_MODE:\
switch (extAddrMode(arg)){\
memLVExtAddrMode(type);\
stackExtAddrMode(type);\
cellExtAddrMode(type);\
frameExtAddrMode(type);\
rackExtAddrMode(type);\
indexedExtAddrMode(type);\
default:\
indExtAddrMode(type);}}}

#define storeT(type)\
Void store##type(type val, IAddress& pc argN_VM){\
lv##type(pc n_vm) = val;\
traceStore(val);}

#define execMove(type){\
type src=load##type(pc n_vm);\
traceLoad(src);\
store##type(src,pc n_vm);\
break;}

#define execCast(srcType,dstType){\
srcType x=load##srcType(pc n_vm);\
traceLoad(x);\
store##dstType((dstType)x,pc n_vm);\
break;}

// Monadic operator execution

#define execMonadic(op,type){\
type x=load##type(pc n_vm);\
traceLoad(x);\
store##type(op x,pc n_vm);\
break;}

// Dyadic operator execution

#define execDyadic(op,type){\
type x=load##type(pc n_vm);\
traceLoad(x);\
type y=load##type(pc n_vm);\
traceLoad(y);\
store##type(x op y,pc n_vm);\
break;}

#define execCompare(op,type){\
type x=load##type(pc n_vm);\
traceLoad(x);\
type y=load##type(pc n_vm);\
traceLoad(y);\
storeTag(x op y,pc n_vm);\
break;}

#define execUnaryBranch(kBool,type){\
type x=load##type(pc n_vm);\
traceLoad(x);\
Int offset=extInt4(pc);\
if(x==kBool)pc+=offset;\
break;}

#define execCondBranch(op,type){\
type x=load##type(pc n_vm);\
traceLoad(x);\
type y=load##type(pc n_vm);\
traceLoad(y);\
Int offset=extInt4(pc);\
if(x op y)pc+=offset;\
break;}

// Conditional branch

#define execTestBranch(op,y,type){\
type x=load##type(pc n_vm);\
traceLoad(x);\
Int offset=extInt4(pc);\
if(x op y)pc+=offset;\
break;}

// Table based branch

#define tableBranchOffset(type)\
Int tableBranchOffset##type(type index,IAddress table,UInt n){\
IAddress addr=table;\
while(n>0){\
type key=ext##type(addr);\
Int offset=extInt4(addr);\
if(key==index)\
return offset;\
n--;}\
return extInt4(addr);}

#define indirectTableBranchOffset(type)\
Int indirectTableBranchOffset##type(type index,IAddress table,UInt n){\
IAddress addr=table;\
while(n>0){\
int keyOffset = extInt4(addr);\
type key=*((type*)(addr+keyOffset));\
Int offset=extInt4(addr);\
if(key==index)\
return offset;\
n--;}\
return extInt4(addr);}

#define stringTableBranchOffset()\
Int stringTableBranchOffset_(String index,IAddress table,UInt n){\
IAddress addr=table;\
while(n>0){\
String key=(String)(addr);\
addr+=strlen(key)+1;\
Int offset=extInt4(addr);\
if(strcmp(key,(index))==0)\
return offset;\
n--;}\
return extInt4(addr);}

#define execTableBranch(type){\
type x=load##type(pc n_vm);\
Int offset=extInt4(pc);\
UInt n=unpackVLU(&pc);\
pc+=tableBranchOffset##type(x,pc+offset,n);\
break;}

#define execIndirectTableBranch(type,mapSrc,mapDst){\
type x=load##type(pc n_vm);\
Env* env=(Env*)loadPtr(pc n_vm);\
Env* codeEnv=(Env*)loadPtr(pc n_vm);\
Int offset=extInt4(pc);\
UInt n=unpackVLU(&pc);\
pc+=indirectTableBranchOffset##type(map##type(x,mapSrc,mapDst),pc+offset,n);\
break;}

#define execStringTableBranch(){\
String s=(String)loadPtr(pc n_vm);\
Int offset=extInt4(pc);\
UInt n=unpackVLU(&pc);\
pc+=stringTableBranchOffset_(s,pc+offset,n);\
break;}

#define execConstStringTableBranch(){\
Int stringOffset=extInt4(pc);\
String s=(String)pc+stringOffset;\
Int offset=extInt4(pc);\
UInt n=unpackVLU(&pc);\
pc+=stringTableBranchOffset_(s,pc+offset,n);\
break;}

#define iCallSequence(lab) callSequence$(execByteCode((lab) n_vm) n_vm);

struct ITypeDescr : public TypeDescr {
   ITypeDescr(TypeSig typeSig);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr) {
   }
#endif


   virtual Kind kind(Void) const;
   virtual Void postLoad(const SegmentTable& segmentTable);
   virtual Void store(Archive& archive, const NameTable& nameTable) const;

public:
   class Loader : public TypeDescr::Loader {
   public:
      Loader(Void) : TypeDescr::Loader(TYPE_DESCR_BYTE_CODE) {}
   protected:
      virtual TypeDescr* load(Archive& archive,
                              const NameTable& nameTable,
                              MSA& msa) const;
   };

protected:
   SegmentId   _evalFnSegId;
   IAddress    _evalFnSeg;
   IAddress    _evalFnLab;
   SegmentId   _mapFnSegId;
   IAddress    _mapFnSeg;
   IAddress    _mapFnLab;
   SegmentId   _assignFnSegId;
   IAddress    _assignFnSeg;
   IAddress    _assignFnLab;
   SegmentId   _insertTxtFnSegId;
   IAddress    _insertTxtFnSeg;
   IAddress    _insertTxtFnLab;

#if (SERIALISATION==1)
   SegmentId   _extractBinFnSegId;
   IAddress    _extractBinFnSeg;
   IAddress    _extractBinFnLab;
   SegmentId   _insertBinFnSegId;
   IAddress    _insertBinFnSeg;
   IAddress    _insertBinFnLab;
#endif

#if (GARBAGE_COLLECTION==1)
   SegmentId   _markFnSegId;
   IAddress    _markFnSeg;
   IAddress    _markFnLab;
#endif

   friend Label execByteCode(IAddress pc argN_VM);
   friend Type mapType$(Type type, const Env& srcEnv, Env& dstEnv);
   friend Void evalFnMethod(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM);
   friend Ptr mapFnMethod(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM);
   friend Void assignFnMethod(TypeDescr* typeDescr, Ptr src, const Env& srcEnv,
                              Ptr dst, Env& dstEnv argN_VM);
   friend Void insertTxtFnMethod(TypeDescr* typeDescr, OutputStream_Char* os,
                                 const Env& osEnv, Ptr ptr,
                                 const Env& env argN_VM);

#if (SERIALISATION==1)
   friend Ptr extractBinFnMethod(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM);
   friend Void insertBinFnMethod(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv,
                              Ptr ptr, Env& env argN_VM);
#endif

#if (GARBAGE_COLLECTION==1)
   friend Void markFnMethod(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM);
#endif

};

struct ICellInfo : public CellInfo {
   ICellInfo(size_t size,
             IAddress byteCodeEntry,
             IAddress byteCodeAltEntry,
             IAddress byteCodeCopyFnEntry,

#if (SERIALISATION==1)
             IAddress byteCodeExtractFnEntry,
             IAddress byteCodeInsertFnEntry,
#endif

#if (GARBAGE_COLLECTION==1)
             IAddress _byteCodeGC_MarkCellEntry,
#endif

             IAddress segment);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr) {
   }
#endif

   inline IAddress segment(Void) const { return _segment; }

   IAddress _byteCodeEntry;
   IAddress _byteCodeAltEntry;
   IAddress _byteCodeCopyFnEntry;
   IAddress _byteCodeExtractFnEntry;
   IAddress _byteCodeInsertFnEntry;

#if (GARBAGE_COLLECTION==1)
   IAddress _byteCodeGC_MarkCellEntry;
#endif

   IAddress    _segment;
};

declareEntry(iClosure);
declareAltEntry(iClosure);

declareEntry(iContinuation);

extern Void alignToBoundary(IAddress& iAddr);
extern Void callIContinuation(IAddress pc argN_VM);
extern const ICellInfo* copyICellInfo(const ICellInfo* cellInfo, Env& env);
extern Label execByteCode(IAddress pc argN_VM);
extern Bits16 extBits16(IAddress& iAddr);
extern Bits32 extBits32(IAddress& iAddr);
extern Bits64 extBits64(IAddress& iAddr);
extern Double extDouble(IAddress& iAddr);
extern Cell* extractBinICell(InputStream_Byte& is, Env& isEnv argN_VM);
extern Int immInt(unsigned char arg);
extern IAddress initCodeAddr(IAddress segment);
extern Void insBits16(IAddress iAddr, Bits16 val);
extern Void insBits32(IAddress iAddr, Bits32 val);
extern Void insBits64(IAddress iAddr, Bits64 val);
extern Void insertBinICell(OutputStream_Byte& os, Env& osEnv, Cell& cell_, Env& cellEnv_ argN_VM);
extern Name mapNameX(Name name, NameTable* srcNameTable, NameTable* dstNameTable);
extern Void traceInterpreter(Bool flag);

#endif /* IVORY_EXEC_H_DEFINED */
