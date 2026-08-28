/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    machine.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory virtual machine.
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

#ifndef IVORY_MACHINE_H_DEFINED
#define IVORY_MACHINE_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/common.h"
#include "ivory/repr.h"
//#include "ivory/streams.h"

typedef Void*  StackPtr;
typedef time_t UTC;

struct InputStream_Byte;
struct OutputStream_Byte;

class VirtualMachine {
public:
   istream*    _inStrm;       // Standard input stream
   ostream*    _outStrm;      // Standard output stream

   MSA*        _consMSA;      // Constructor memory segment allocator
   Env*        _consEnv;      // Constructor environment

   StackPtr    _sb;           // Stack base register
   StackPtr    _sp;

#if (FRAME_DESCRIPTORS==1)
   StackPtr    _fp;           // Frame pointer
#endif

   Void*       _rack;         // Unused at present, future use planned for volatile static data

   Cell*       _cell;         // Current cell pointer
   Env*        _cellEnv;      // Current cell environment pointer

// Registers

   Env*        _rEnv;         // Associated environment pointer for name, type, cell, expr & struct
   Name        _rName;        // Name
   Type        _rType;        // Type
   Cell*       _rCell;        // Cell pointer
   Expr        _rExpr;        // Expression
   Byte        _rByte;        // Byte
   Bits32      _rBits;        // Bits
   Int         _rInt;         // Integer
   Float       _rFloat;       // Floating pointer number
   Double      _rDouble;      // Double precision floating point number
   Tag         _rTag;         // Sum type tag
   Char        _rChar;        // Character
   Byte*       _rStruct;      // Structured value    
   size_t      _rStructSize;  // Current structure size
   Void*       _rPtr;         // Pointer
   Ref         _rRef;         // Object reference
   UTC         _rUTC;         // UTC
};

#if GLOBAL_VM == 1

extern istream*   inStrm;
extern ostream*   outStrm;

extern MSA*       consMSA;
extern Env*       consEnv;

extern StackPtr   sb;
extern StackPtr   sp;

#if (FRAME_DESCRIPTORS==1)
extern StackPtr   fp;
#endif

extern Cell*      cell;
extern Env*       cellEnv;
extern Cell*      updatePtr;

extern Void*      rack;

extern Env*       rEnv;

extern Name       rName;
extern Type       rType;
extern Cell*      rCell;
extern Expr       rExpr;
extern Byte       rByte;
extern Bits       rBits;
extern Int        rInt;
extern Float      rFloat;
extern Double     rDouble;
extern Tag        rTag;
extern Char       rChar;
extern Byte*      rStruct;
extern size_t     rStructSize;
extern Void*      rPtr;
extern Ref        rRef;
extern UTC        rUTC;

#else

#define inStrm    (vm._inStrm)
#define outStrm   (vm._outStrm)

#define consMSA   (vm._consMSA)
#define consEnv   (vm._consEnv)

#define sb        (vm._sb)
#define sp        (vm._sp)

#define cellEnv   (vm._cellEnv)
#define cell      (vm._cell)

#define updatePtr (vm._updatePtr)

#define rack      (vm._rack)

//#define rLabel    (vm._rLabel)

#define rEnv      (vm._rEnv)

#define rName     (vm._rName)
#define rType     (vm._rType)
#define rCell     (vm._rCell)
#define rExpr     (vm._rExpr)
#define rByte     (vm._rByte)
#define rBits     (vm._rBits)
#define rInt      (vm._rInt)
#define rFloat    (vm._rFloat)
#define rDouble   (vm._rDouble)
#define rTag      (vm._rTag)
#define rChar     (vm._rChar)
#define rStruct   (vm._rStruct)
#define rStructSize (vm._rStructSize)
#define rPtr      (vm._rPtr)
#define rRef      (vm._rRef)
#define rUTC      (vm._rUTC)

#endif

#define jump(label) return (label)

#define push(x,t) (*(--*(t**)(&sp))=(x))
#define pop(t) (*(*((t**)(&sp)))++)
#define tos(t) (*((t*)sp))
#define stackAlloc(n) (*((unsigned char**)&sp)-=(n))
#define dropStack() (*((unsigned char**)&sp)+=(stackDepth))
#define drop(n) (*((unsigned char**)&sp)+=(n))

#if (GARBAGE_COLLECTION==1)
#define nextFrame(n) (*((unsigned char**)&fp)+=(n))
#endif

#define pushLabel(l) push((l),Label)
#define popLabel() pop(Label)

#define addressOf(p,i) (((unsigned char*)(p))+(i))
#define indexed(p,i,t) (*(t*)addressOf(p,i))

#define local(i,t) indexed(sp,stackDepth-(i),t)
#define stack(i,t) indexed(sp,(i),t)

#define stackWord          (sizeof(void*))
#define stackAlign(x,a)    (((x)+((a)-1))&~((a)-1))
#define stackSlotSize(t)   stackAlign(sizeOf##t, stackWord)

#define stackFPSize(t)     stackFPSize##t

#if (FRAME_DESCRIPTORS==1)
#define frame(i,t) indexed(fp,-((int)(i)),t)
#endif

#define argReg(t) argReg##t

#define loadArgReg(name,t) loadArgReg##t(name)

#define loadArgRegClosure(name)\
Cell* name=rCell;\
Env* name##Env=rEnv

#define loadArgRegExpr(name)\
Expr name=rExpr;\
Env* name##Env=rEnv

#define loadArgRegTag(name) Tag name=rTag

#define stackFPSizeTag stackSlotSize(Tag)

#define loadArgTag(name, off) Tag name = stack(off,Tag)

#define storeArgTag(name,off) stack(off,Tag)=name

#define argRegFromCellTag(s,c) rTag=cellBody(*cell,s).c

#define mappedTag(name,srcEnv,dstEnv) name

#define returnTag(t){\
rTag=(t);\
jump(popLabel());}

#define loadArg(name,R,off)   loadArg##R(name,(off))

#define storeArg(name,R,off)   storeArg##R(name,(off))

#define argRegFromCell(R,s,c) argRegFromCell##R(s,c);


#define defineStackDepth(name,d)\
enum class _SD_##name:size_t{depth=(size_t)(d)}
#define stack_Depth(name)  static_cast<size_t>(_SD_##name::depth)
#define drop_Stack(name)  drop(stack_Depth(name))

// Closure macros

#define mkClosure_max(a,b) ((a)>(b)?(a):(b))
#define mkClosure(cellInfo,size,msa)\
(fromCell(*new(mkClosure_max((size),MIN_CLOSURE_SIZE),(msa))Cell(cellInfo)))

#define freeVar(c,i,t) (*(t*)(((unsigned char*)toCell(c).body())+(i)))

#if (GLOBAL_VM == 0)
   #define preEval(iStream,oStream,memLimit)\
   VirtualMachine vm_;\
   VirtualMachine& vm = vm_;\
   preEval$(iStream,oStream,memLimit n_vm)
#else
   #define preEval(iStream,oStream,memLimit) preEval$(iStream,oStream,memLimit n_vm)
#endif

#define postEval(verbose) postEval$(verbose n_vm)

#if (GLOBAL_VM == 0)
   #define saveVM()\
   VirtualMachine* prev_vm=(VirtualMachine*)(_ins->_vm);\
   _ins->_vm=&vm;
#else
   #define saveVM()
#endif

#if (GLOBAL_VM == 0)
   #define restoreVM()\
   if (_ins!=NULL)\
     _ins->_vm=prev_vm;
#else
   #define restoreVM()
#endif

#define enter(expr,env)\
if ((expr).tag()==PTR_TAG){\
cell=(expr);\
cellEnv=(env);\
jump(static_cast<const CellInfo*>(toCell(expr).tag())->entry_());}\
else jump(enter$((expr),(env)n_vm))

#define enterLocal(e,env) enter(local(e, Expr),local(env, Env*))

#define saveStackBase(){\
push(sb,StackPtr);}

#define restoreStackBase(){\
sb = pop(StackPtr);}

#define stackSaveSize (sizeof(StackPtr))

#define depth() ((size_t)((unsigned char*)(sb)-(unsigned char*)(sp)))

#define emptyStack()\
sb=sp

// Argument check

#ifdef CHECK_ARGS
#define emptyStacksChecked(d) sb=(d)
#define checkArgs(s)\
if(depth()!=(stackDepth))\
runTimeError("invalid application to " s)
#else
#define emptyStacksChecked(d)
#define checkArgs(a)
#endif

// Indirection cell info

struct Indirection {
   Cell* _cell;
};

extern CellInfo indirectionCellInfo;
extern Cell* allocIndirection(Cell* cell, MSA& msa);

// Common error point for partial function update

declareEntry(updatePap$);

#define updatePap entry(updatePap$)

struct PApTypeEntry {
   size_t   _size;
   Type*    _type;
}; 

extern Label returnPaP(struct PApTypeEntry pApTypeTable[] argN_VM);

// Common error point for run-time errors

declareEntry(runTimeError$);

#ifdef NO_EXCEPTIONS
   #define throw(error)
#endif

#define runTimeError(reason)\
{throw(RunTimeError(reason));jump(entry(runTimeError$));}

// Common error point for tag errors

#define tagError(fn)\
{ivoryError1("Tag error in: %s",fn);jump(entry(runTimeError$));}

extern int vmLevel;

extern Label enter$(Expr expr, Env* env argN_VM);

extern Void enterReturn(Expr expr, Env* env argN_VM);

extern Void eval(Expr expr, Env& env argN_VM);

extern Cell* extractBinCell(InputStream_Byte& is, Env& isEnv argN_VM);

extern Expr extractBinExpr(InputStream_Byte& is, Env& isEnv argN_VM);

extern Void insertBinCell(OutputStream_Byte& os, Env& osEnv, Cell& cell_, Env& cellEnv_ argN_VM);

extern Void insertBinExpr(OutputStream_Byte& os, Env& osEnv, Expr expr, Env& exprEnv argN_VM);

extern Void postConstructorInit(Void);

declareBuiltInFn(exit);

Cell* mapClosure(Cell* closure, const Env& srcEnv, Env& dstEnv argN_VM);

extern Name mapName$(Name name, const NameTable& srcNameTable,
                                      NameTable& dstNameTable);
#define mapName(name,srcNameTable,dstNameTable)\
&(srcNameTable)==&(dstNameTable)?(name):mapName$((name),(srcNameTable),(dstNameTable))

extern Type mapType$(Type type, const Env& srcEnv, Env& dstEnv);
#define mapType(type,srcEnv,dstEnv)\
&(srcEnv)==&(dstEnv)?(type):mapType$(type,(srcEnv),(dstEnv))

extern Void preEval$(istream* istresam, ostream* ostream, Int memLimit argN_VM);

extern Void postEval$(Bool verbose argN_VM);

extern Void callSequence$(Label label argN_VM);
#define callSequence(label) callSequence$(label n_vm)

extern Void localCallSequence$(Label label argN_VM);
#define localCallSequence(label) localCallSequence$(label n_vm)

extern Cell* mapClosure(Cell* closure, const Env& srcEnv, Env& dstEnv argN_VM);

extern Void printExpr(Expr expr, ostream& os, const Env& env);

extern Void printList(Expr list, ostream& os, const Env& env);

extern Void printName(Name name, ostream& os, const NameTable& nameTable);

extern Void printRepr(const Repr repr, ostream& os);

extern Void printType(Type type, ostream& os, const Env& env);

extern void printCanonicalType(Type type, ostream& os, Env& env);

extern Void printSchematicTypeSig(TypeSig typeSig, ostream& os, const Env& env);

extern Void printTypeSig(TypeSig typeSig, UInt prec, Bool isRight, ostream& os, const Env& env);

extern Void printCanonicalTypeSig(TypeSig typeSig, UInt prec, Bool isRight, ostream& os, Env& env);

extern Void printTypeSigList(Expr list, ostream& os, const Env& env, Bool inner = FALSE);

extern Bool readGlobal(const char* filename);

extern Void traceExpr(Expr e, const Env& env);

extern Int start(Void);

#endif /* IVORY_MACHINE_H_DEFINED */

