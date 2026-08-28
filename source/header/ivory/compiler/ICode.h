/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    iCode.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Header file for I-machine code generation
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

#ifndef IVORY_COMPILER_I_CODE_H
#define IVORY_COMPILER_I_CODE_H

#include "code.h"
#include "type.h"
#include "ivory/bool.h"
#include "ivory/iMachine.h"
#include "ivory/instructions.h"

class Operand;

#if (GARBAGE_COLLECTION==1)
// An association between a pointer representation
// and a garbage collection function label
// Used to support self-referential pointer types

typedef struct PtrReprLabel_tag {
   const PtrReprLabel_tag* _next;
   const PtrRepr* _ptrRepr;
   CodeLabel* _lab;
} PtrReprLabel;
#endif

// ReprElement: An element of a list of representations

class ReprElement {
public:
   ReprElement(ReprElement* next, Repr repr)
      : _next(next), _repr(repr) {}
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline ReprElement*& next(Void) { return _next; }
   inline Repr& repr(Void) { return _repr; }

protected:
   ReprElement* _next;
   Repr         _repr;
};

class StackAdjustInstruction : public Instruction {
public:
   StackAdjustInstruction(Int diff);
   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#endif
   inline Void operator delete(Void* ptr) {}

   virtual const char* mnemonic(Void) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;

protected:
   Int         _diff;

   friend class Code;
};

// Register

class Register : public Loc {
public:
   Register(UInt id);
   Register(const Register& src);
   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;

   virtual Bool registerBased(Void) const { return TRUE; }
   virtual Register* reg(Void) { return this; }
   inline UInt id() const {return _id;}
   virtual Void insert(ostream& os, const ICode& code) const;
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;

protected:
   UInt  _id;

   friend class Var;
   friend class ICode;
   friend class GoToInstruction;

   friend Bool Loc::matchBetween(const Loc& x, const Loc& y);
};

// SpecialLoc: A special location

// Cuurently only used to access the code environment

class SpecialLoc : public Loc {
public:
   SpecialLoc(Repr repr);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;

   virtual Void insert(ostream& os, const ICode& code) const;
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;

};

// StaticLoc: A statically allocated location

class StaticLoc : public Loc {
public:
   StaticLoc(Repr repr, Bool imported, Bool exported, Name name = NULL_NAME);
   StaticLoc(const StaticLoc& src);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;

   virtual Void insert(ostream& os, const ICode& code) const;
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;

protected:
   Bool  _imported;     // True if exported, False otherwise
   Bool  _exported;     // True if exported, False otherwise
   Name  _name;         // For named slots, null otherwise
   Bool  _allocated;    // True if allocated, False otherwise
   UInt  _rackPos;      // For byte code static location

   friend class ByteCode;
   friend Bool Loc::matchBetween(const Loc& x, const Loc& y);
};

// StackLoc: A stack location

class StackLoc : public Loc {

#if (GARBAGE_COLLECTION==1)
public:
   enum Origin {
      SP_REL,  // Stack pointer relative
      FP_REL   // Frame pointer relative
   };
#endif

public:
   StackLoc(Repr repr, StackLoc* next, UInt offset, size_t size);
   StackLoc(const StackLoc& src);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;

   inline UInt& offset(Void) { return _offset; };
   inline size_t& size(Void) { return _size; };

#if (GARBAGE_COLLECTION==1)
   inline Origin origin(Void) const { return _origin; }
   inline Void setFrameRelative(Void) { _origin = FP_REL; }
#endif

   virtual Void setReadyToKill(Void);
   virtual Void insert(ostream& os, const ICode& code) const;
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;
   virtual Bool isRegisterBased(Void) const { return FALSE; }

protected:
   UInt     _offset;
   size_t   _size;

#if (GARBAGE_COLLECTION==1)
   Origin   _origin;
#endif

   static Void insert(StackLoc& loc, StackLoc** stackLocPtr);
   static Void extract(StackLoc& loc, StackLoc** stackLocPtr);

   friend class Var;
   friend class VarSet;
   friend class ICode;
   friend class SourceCode;
   friend class MoveInstruction;

   friend Bool Loc::matchBetween(const Loc& x, const Loc& y);
};

// CompLoc: A component location

class CompLoc : public Loc {
public:
   CompLoc(Loc::Kind kind, Repr repr, Loc& loc,
           const StructRepr& structRepr, UInt index, Bool addToParent = TRUE);
   CompLoc(const CompLoc& src, Loc* parent, Bool withVars, MSA& msa);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline size_t index(Void) { return _index; }
   inline const StructRepr& structRepr(Void) { return _structRepr; }
   inline Loc& loc(Void) { return _loc; }

   Loc* parent(Void);
   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;

   virtual Bool registerBased(Void) const { return _loc.registerBased(); }
   virtual Register* reg(Void) { return _loc.reg(); }
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;

protected:
   Loc&              _loc;
   UInt              _index;
   const StructRepr& _structRepr;

   friend class ICode;

   friend Bool Loc::matchBetween(const Loc& x, const Loc& y);
};

// CellCompLoc: A cell component location

class CellCompLoc : public CompLoc {
public:
   CellCompLoc(Repr repr, Loc& loc, StructRepr& structRepr, UInt index, Bool addToParent);
   CellCompLoc(const CellCompLoc& src, Loc* parent, Bool withVars, MSA& msa);
   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;
   virtual Bool requiresFlush(Void) const { return FALSE; }
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;

   friend class Var;
   friend class ICode;

   friend Bool Loc::matchBetween(const Loc& x, const Loc& y);
};

// IndLoc: An indirect location

class IndLoc : public Loc {
public:
   IndLoc(Repr repr, Loc& loc, Bool addToParent = TRUE);
   IndLoc(const IndLoc& src, Loc* parent, Bool withVars, MSA& msa);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   Loc* parent(Void);
   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;
   virtual Bool registerBased(Void) const { return _loc.registerBased(); }
   virtual Register* reg(Void) { return _loc.reg(); }
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;

protected:
   Loc&  _loc;

   friend class ICode;

   friend Bool Loc::matchBetween(const Loc& x, const Loc& y);
};

// IndexedLoc: An indexed location

class IndexedLoc : public Loc {
public:
   IndexedLoc(Repr repr, Loc& loc, const Const& index, Bool addToParent = TRUE);
   IndexedLoc(Repr repr, Loc& loc, Loc& index, Bool addToParent = TRUE);
   IndexedLoc(const IndexedLoc& src, Loc* parent, Bool withVars, MSA& msa);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   Loc* parent(Void);
   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;

   virtual Bool registerBased(Void) const { return _loc.registerBased(); }
   virtual Register* reg(Void) { return _loc.reg(); }
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;

protected:
   Loc&  _loc;
   Bool  _isConst;   // TRUE if constant, FALSE otherwise
   union {
      const Const*   _const;  // Indexed by a constant   
      Loc*           _loc;    // Indexed by another location
   } _index;
   
   friend class ICode;

   friend Bool Loc::matchBetween(const Loc& x, const Loc& y);
};

// Location Operand

class LocOperand : public Operand {
public:
   LocOperand(Loc& loc, Bool reserved = FALSE);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Repr repr(ICode& code);
   virtual Repr repr(Void);
   Void setReadyToKill(Bool envOf) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
   virtual Void genDstSourceCode(ostream& os, SourceCode& code) const;

   inline Loc& loc(Void) {
      return _loc;
   }

protected:
   Loc&  _loc;
   Bool  _reserved;

   friend class ICode;
   friend class SourceCode;
   friend class SelectOperand;
};

// Abstract I-Machine code class

class ICode : public Code {

public:
   ICode(ConstString options, Env& env, MSA& msa);

   inline Register& reg(UInt id) const { return *_register[id]; }
   inline SpecialLoc& codeEnv(Void) const { return *_codeEnv; }
   inline SpecialLoc& cellInfo(Void) const { return *_cellInfo; }
   inline UInt sd_(Void) const { return _sd; }
   inline UInt sp_(Void) const { return _sp; }
   inline const Instruction* currentIns(Void) const { return _currentIns; }
   inline Instruction*& currentIns(Void) { return _currentIns; }

   inline Void setCurrentIns(Instruction* ins) { _currentIns = ins; }

   Void genTarget(ostream& os);

   ReprInfo typeReprInfo(TypeSig typeSig);
   StructRepr* addStructRepr(UInt nComps, Repr* compReprV);

   Void setStructRepr(StructTemplate& structTemplate);

   CompLoc* compLoc(Repr repr, Loc& loc, const StructRepr& structRepr, UInt index, Bool addToParent = TRUE) const;
   CellCompLoc* cellCompLoc(Repr repr, Loc& loc, StructRepr& structRepr, UInt index, Bool addToParent = TRUE) const;
   IndLoc* indLoc(Repr repr, Loc& loc, Bool addToParent = TRUE) const;
   IndexedLoc* indexedLoc(Repr repr, Loc& loc, const Const& index,
                          Bool addToParent = TRUE) const;
   IndexedLoc* indexedLoc(Repr repr, Loc& loc, Loc& index,
                          Bool addToParent = TRUE) const;

   LocOperand& locOperand(Loc& loc, Bool reserved = FALSE) const;
   LocOperand& compLocOperand(Repr repr, Loc& loc, StructRepr& structRepr, UInt index) const;
   LocOperand& indLocOperand(Repr repr, Loc& loc) const;

   static Bool squeezeNeeded(const StackLoc* stackLoc);
   UInt liveStackDepth(const StackLoc* stackLoc);
   UInt inputStackDepth(const StackLoc* stackLoc, const VarSet& vars);

   Void emptyRegisters(Void);
   Register* selectReg(Repr repr);
   Void allocRegisters(Var& var);
   virtual Bool suppressEnvMove(Void) const;

   virtual Void printPtrRepr(const PtrRepr* ptrRepr, ostream& os,
                             const PtrReprInstance* ptrReprs = NULL) const;
   virtual Void printStructRepr(const StructRepr& structRepr, ostream& os,
                                const PtrReprInstance* ptrReprs = NULL) const;
   virtual Void printUnionRepr(const UnionRepr& unionRepr, ostream& os,
                               const PtrReprInstance* ptrReprs = NULL) const;
   virtual Void printArrayRepr(const ArrayRepr& arrayRepr, ostream& os,
                               const PtrReprInstance* ptrReprs = NULL) const;
   virtual Void printRepr(Repr repr, ostream& os,
                          const PtrReprInstance* ptrReprs = NULL) const;

   Void adjustStackPointer(Instruction* ins, Bool recoverFlag = FALSE);
   Void adjustStackPointer(BasicBlk& basicBlk, Instruction* ins);
   Void recoverStackRange(UInt& from, UInt to,
                          StackLoc* stackLocs,
                          UInt& d, UInt& argBase_d, UInt& arg_d,
                          Bool emitCode);
   Void recoverStack(Bool squeeze = FALSE, Bool emitCode = FALSE);
   Void moveLocVars(Loc& fromLoc, Loc& toLoc);
   Void flushLoc(Loc& loc);

   StackLoc& allocStackLoc(Repr repr);

   UInt allocStackOffset(size_t size, Loc::Kind kind, Bool reclaimFlag);
   Loc& allocExtraFormalParameterLoc(Var& var, Repr repr);

   StackLoc& allocStackLoc(Repr repr, Bool argFlag, Bool reclaimFlag);
   StackLoc& allocStackLocAt(Repr repr, size_t size, UInt offset);
   Register& allocReg(Repr repr);
   CellCompLoc& allocCellCompLoc(const Var& var, Repr repr, Bool addToParent = TRUE);

   virtual Void notLive(Var& var);

   virtual Loc::Kind locKind(Var& var, Repr repr) = 0;
   virtual Loc::Kind temporaryKind(Loc::Kind kind) const;
   virtual Loc* allocLoc(Repr repr, Loc::Kind kind, Bool argFlag, Bool reclaimFlag = FALSE);
   Loc* allocLoc(Repr repr, Var& var, Bool& reused, Loc::Kind kind = Loc::UNKNOWN_LOC);

   Loc& assignLoc(Repr repr, Var& dstVar, Operand*& dst);

   Void insertInstruction(Instruction* ins);
   Void insertMove(Operand& src, Operand& dst, Instruction* beforeIns = NULL);
   Void insertMove(Repr repr, const Var* src, Loc& dst);
   Void appendInstruction(Instruction* ins);
   Void appendMove(Operand& src, Operand& dst);

   Void saveLoc(Loc& loc);
   Void flushReg(UInt r);
   Void flushRegisters(Void);
   virtual Void flushNonStack(Void);

   Bool hasLoc(const Loc& loc, const Loc* list) const;
   virtual Void insertLoc(Loc& loc);
   Void initLocs(BasicBlk& basicBlk);
   Loc* transferLoc(Loc& loc, BasicBlk& basicBlk);
   Void saveVarLocs(BasicBlk& basicBlk, Bool flushNonLocal);

   Void genLocCode(Void);

   virtual Bool insertJumpsToHeadLabels(Void) const = 0;

   virtual Repr labelRepr(Bool simple = FALSE) const = 0;

#if (GARBAGE_COLLECTION==1)
   Void genLiveVarsGC_Code(Bool isContinuation,
                           BasicBlk& basicBlk,
                           UInt stackSize,
                           FrameDescrInstruction& frameDescrInstr);
   Void genGC_Code(Var& var, StackLoc* loc, StackLoc* loc_env,
                   GC_StubInstruction& gcStubInstr);
#endif

protected:

   ReprElement*   _reprList;           // Unique list of representations of pointer, sructure and union types

   Register*      _register[N_REGS];   // I-machine registers
   SpecialLoc*    _codeEnv;            // Code environment location
   SpecialLoc*    _cellInfo;           // Code cell info

   UInt           _rackPos;            // Rack position

   UInt           _sd;                 // Stack depth
   UInt           _sp;                 // Stack pointer

   Bool           _argFlag;            // True if preparing arguments, false otherwise
   UInt           _argBase_d;          // Argument base depth
   UInt           _arg_d;              // Argument depth
   Bool           _sqzPending;         // True if pending squeeze for 'dead' variables, False otherwise

   Bool           _altEntry;           // True if alternative entry, False otherwise

   Instruction*   _currentIns;         // Current instruction

   StackLoc*      _stackLocs;          // List of stack locations
   
   friend class ContinuationInstruction;
   friend class MonadicInstruction;
   friend class DyadicOpInstruction;
   friend class GoToInstruction;
   friend class ConditionalInstruction;
   friend class LabelInstruction;
   friend class StackAdjustInstruction;
   friend class EnterInstruction;
   friend class CellInfoInstruction;
   friend class ClosureAllocInstruction;
   friend class CellAllocInstruction;
   friend class StructInstruction;
   friend class ConsInstruction;
   friend class Cons_A_S_Instruction;
   friend class Cons_S_Instruction;
   friend class DeconsInstruction;
   friend class SelPtrInstruction;
   friend class PtrInstruction;
   friend class PtrArrayAllocInstruction;
   friend class EmptyArgsInstruction;
   friend class EnterCellInstruction;
   friend class StackLoc;
   friend class StructCompLoc;
   friend class BasicBlk;
   friend class Var;
   friend class ConstT<Name>;
   friend class ConstT<String>;
};

#endif /* IVORY_COMPILER_I_CODE_H_DEFINED */
