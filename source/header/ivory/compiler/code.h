/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    code.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Header file for code generation 
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

#ifndef IVORY_COMPILER_CODE_H
#define IVORY_COMPILER_CODE_H

#include "AST.h"

#if (GARBAGE_COLLECTION==1)
#include <ivory/frameDescr.h>
#endif

class BackPatchElement;
class BasicBlk;
class ByteCode;
class ByteCodeSection; 

#if (GARBAGE_COLLECTION==1)
class GC_StubInstruction;
class FrameDescrInstruction;
#endif

class ICode;
class Instruction;
class Register; 
class SourceCode;
class TypeInsts;
class VarElement;

// TypeElement: An element of a list of types

class TypeElement {
public:
   TypeElement(TypeElement* next, Type type, Bool genMethods)
      : _next(next), _type(type), _genMethods(genMethods) {}
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline TypeElement*& next(Void) { return _next; }
   inline Type& type(Void) { return _type; }
   inline Bool genMethods(Void) const { return _genMethods; }
   inline Void setGenMethods(Void) { _genMethods = TRUE; }

protected:
   TypeElement*   _next;
   Type           _type;
   Bool           _genMethods;
};


class VarSet {
public:
   VarSet(Void) : _varElements(NULL) {}
   VarSet(const VarSet& src, MSA& msa);
   VarElement* addElement(Var& var, MSA& msa);
   VarElement* findElement(const Var& var) const;
   Void removeElement(const Var& var, MSA& msa);
   Void moveToHead(VarElement& varElement);
   UInt count(const Code& code);
   Void empty(Void) { _varElements = NULL; }
   Void empty(MSA& msa);
   Void join(VarSet& vars, MSA& msa);
   inline Bool isEmpty(Void) const { return _varElements == NULL; }
   inline VarElement* head(Void) const { return _varElements; }

protected:
   VarElement* _varElements;

   friend class LabelOperand;
   friend class Code;
   friend class ICode;
   friend class SourceCode;
   friend class ByteCode;
   friend class ConditionalInstruction;
   friend class BasicBlk;
   friend class Loc;
};

// Loc: Abstract class to represent a data location

class Loc {
public:
   enum Kind {
      UNKNOWN_LOC,      // Unknown
      REGISTER_LOC,     // Register
      NATIVE_LOCAL_LOC, // Native local
      SPECIAL_LOC,      // Special (e.g. code env.)
      STATIC_LOC,       // Static
      STACK_LOC,        // Stack
      COMP_LOC,         // Component
      CELL_COMP_LOC,    // Cell component
      IND_LOC,          // Indirect 
      INDEXED_LOC       // Indexed
   };

protected:
   Loc(Kind kind, Repr repr, Loc* next = NULL);
   Loc(const Loc& src, Bool full, MSA& msa);

public:
   inline Repr repr(Void) const { return _repr; };
   inline Kind kind(Void) const { return _kind; };
   inline Loc* next(Void) { return _next; };
   inline const Loc* next(Void) const { return _next; };
   inline VarSet& vars(Void) { return _vars; }
   inline Bool readyToKill(Void) const { return _readyToKill; };
   inline Bool reserved(Void) const { return _reserved; };

   inline Void setReserved(Bool q) { _reserved = q; }

   virtual Loc* parent(Void);
   virtual Loc* clone(Loc* parent, Bool full, MSA& msa) const = 0;
   Loc* cloneChildren(Bool full, MSA& msa);
   virtual Bool registerBased(Void) const { return FALSE; };
   virtual Register* reg(Void) { return NULL; }
   Void addChild(Loc* loc);
   Void empty(MSA& msa);
   Bool inUse(Bool notReadyToKill = TRUE) const;
   virtual Bool requiresFlush(Void) const;
   virtual Void setReadyToKill(Void);
   virtual Void insert(ostream& os, const ICode& code) const;
   virtual Void genByteCode(Int offset, ostream& os, ByteCode& code);
   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void genDstSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void print(ostream& os, const Env& env, UInt depth = 0, Bool isDep = FALSE) const;
   virtual Void printCondRepr(ostream& os, const Env& env, Bool reprFlag, UInt depth, Bool isDep = FALSE) const;

   static
      Bool matchBetween(const Loc& x, const Loc& y);

protected:
   Kind        _kind;
   Repr        _repr;
   Loc*        _next;
   Loc*        _firstChild;   // Structure component or indirect child list
   VarSet      _vars;
   Bool        _readyToKill;
   Bool        _reserved;

static
   Void printLocs(const Loc* hdLoc, ostream& os, const Env& env, UInt depth = 0);

   friend class BasicBlk;
   friend class ICode;
   friend class SourceCode;
   friend class Var;
   friend class LocSet;
   friend class LocOperand;
   friend class MonadicInstruction;
   friend class DeconsInstruction;
   friend Void addVarLoc(Var& var, Loc& loc, MSA& msa);
   friend Void remVarLoc(Var& var, Loc& loc, MSA& msa);

   friend class ContinuationInstruction;
};

// LocElement: element of a location set

class LocElement {
public:
   LocElement(LocElement* next, Loc& loc) : _next(next), _loc(loc) {}
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline Loc& loc(Void) { return _loc; }
   inline LocElement* next(Void) { return _next; }

protected:
   LocElement* _next;
   Loc&        _loc;
 
   friend class LocSet;
   friend class Code;
   friend class ICode;
   friend class SourceCode;
   friend class Var;
   friend class VarSet;
   friend class VarOperand;
   friend class MonadicInstruction;
   friend class ConditionalInstruction;
};

// LocSet: A set of locations

class LocSet {
public:
   LocSet(Void) : _locElements(NULL) {}

   Void copyOf(const LocSet&, MSA& msa);
   LocElement* addElement(Loc& loc, MSA& msa);
   LocElement* findElement(const Loc& loc) const;
   Loc* matchingElement(const Loc& loc) const;
   Void removeElement(const Loc& loc, MSA& msa);
   UInt count(Repr repr) const;
   Void empty(MSA& msa);
   Bool hasNonRegisterLoc(Repr repr) const;
   Loc* firstRegisterBasedLoc(Repr repr);
   Void print(ostream& os, const Env& env, UInt depth = 0) const;
   inline LocElement* head(Void) const { return _locElements; }
   inline Bool isEmpty(Void) const { return _locElements == NULL; }

protected:
   LocElement* _locElements;

   friend class Var;
   friend class VarSet;
   friend class VarOperand;
   friend class Code;
   friend class ICode;
   friend class SourceCode;
   friend class BasicBlk;
   friend class MoveInstruction;
   friend class ConditionalInstruction;
};



class BasicBlkElement {
public:
   BasicBlkElement(BasicBlkElement* next, BasicBlk& basicBlk);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline BasicBlk& basicBlk(Void) const { return _basicBlk; }

protected:
   BasicBlkElement*  _next;
   BasicBlk&         _basicBlk;
   UInt              _count;

   friend class Code;
   friend class ICode;
   friend class BasicBlk;
   friend class BasicBlkSet;
   friend class MoveInstruction;
};

class BasicBlkSet {
public:
   BasicBlkSet(Void) : _basicBlkElements(NULL) {}
   BasicBlkElement* addElement(BasicBlk& basicBlk, MSA& msa);
   BasicBlkElement* findElement(const BasicBlk& basicBlk) const;
   Void join(const BasicBlkSet& basicBlks, MSA& msa);
   Void removeElement(const BasicBlk& basicBlk, MSA& msa);
   inline Void empty(Void) { _basicBlkElements = NULL; }
   inline Bool isEmpty(Void) const { return _basicBlkElements == NULL; }
   inline BasicBlkElement* head(Void) const { return _basicBlkElements; }

protected:
   BasicBlkElement* _basicBlkElements;

   friend class Code;
   friend class BasicBlk;
};

// Basic Block

class BasicBlk {
public:
   BasicBlk(Void);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   Instruction* firstIns (Void) const { return _firstIns; }
   Instruction*& firstIns(Void) { return _firstIns; }
   Instruction* lastIns(Void)   { return _lastIns; }
   UInt sd_(Void) const { return _sd; }
   UInt& sd_(Void) { return _sd; }
   UInt sp_(Void) const { return _sp; }
   UInt& sp_(Void) { return _sp; }

   inline Bool locCodeGenerated(Void) { return _locCodeGenerated; }
   inline VarSet& vars(Void) { return _vars; }
   inline BasicBlkSet& successors(Void) { return _successors; }

   Void addSuccessor(BasicBlk&basicBlk, MSA& msa);
   Void removeSuccessor(BasicBlk& basicBlk, MSA& msa);
   Bool hasSuccessor(const BasicBlk& basicBlk) const;
   BasicBlk* implicitSuccessor(Void) const;
   Void removeInstruction(Instruction* ins, Code& code, Bool destroy = TRUE);
   Void setUD(Code& code);
   Bool varIsInput(const Var& var) const;
   Bool varIsInputToContinuation(const Var& var) const;
   Void liveVarInfo(const Code& code) const;
   Bool localVar(const Var& var);
   Void genLocCode(ICode& code);
   Void saveLocInfo(ICode& code, Bool flushNonLocal);
   Loc* matchLoc(Loc& loc);
   Void printLiveVars(Bool killPendingFlag, const Env& env);

protected:
   BasicBlk*      _next;
   BasicBlk*      _prev;
   Instruction*   _firstIns;
   Instruction*   _lastIns;
   Bool           _locCodeGenerated;
   Bool           _defer;
   Bool           _saveFlag;
   VarSet         _vars;
   BasicBlkSet    _successors;
   UInt           _sd;
   UInt           _sp;

   friend class CodeLabel;
   friend class CellInfo_;
   friend class VarSet;
   friend class Var;
   friend class CodeNode;
   friend class FnAp;
   friend class Lambda;
   friend class Code;
   friend class ICode;
   friend class ModuleDefn;
   friend class Order;
   friend class ReturnState;
   friend class Code;
   friend class SourceCode;
   friend class ByteCode;
   friend class LabelOperand;
   friend class LabelInstruction;
   friend class EntryInstruction;
   friend class CellInfoInstruction;
   friend class CellInfoOperand;
   friend class MonadicInstruction;
   friend class MoveInstruction;
   friend class GoToInstruction;
   friend class ArgCheckInstruction;
   friend class EnterCellInstruction;
   friend class ConditionalInstruction;
   friend class CaseInstruction;
   friend class LabelInstruction;
   friend class LabelBackPatchElement;
};

// Code label

class CodeLabel {
public:
   CodeLabel(BasicBlk* basicBlk, Bool head = FALSE, Lambda* lambda = NULL);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline Bool withBasicBlock(Void) const { return _basicBlk != NULL; }
   inline BasicBlk& basicBlk(Void) { assert(_basicBlk != NULL, ""); return *_basicBlk; }
   inline BasicBlk const& basicBlk(Void) const { assert(_basicBlk != NULL, "");  return *_basicBlk; }
   inline Lambda* lambda(Void) { return _lambda; }
   inline UInt seqNo(Void) { return _seqNo; }

   inline Bool isHead(Void) const { return _head; }
   inline UInt useCount(Void) const { return _useCount; }

   Void incRef(Void);
   Void decRef(Void);

   Void insert(ostream& os, const Code& code) const;
   Void genByteCode(ostream& os, ByteCode& code);
   Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   BasicBlk*         _basicBlk;
   Bool              _head;
   Lambda*           _lambda;
   CodeLabel*        _headCodeLabel;
   UInt              _seqNo;
   UInt              _useCount;
   Bool              _saveEntryFlag;
   UInt              _bytePos;
   ByteCodeSection*  _section;

   friend class Lambda;
   friend class ByteCode;
   friend class SourceCode;
   friend class CellInfo_;
   friend class CellInfoOperand;
   friend class Code;
   friend class ModuleDefn;
   friend class BasicBlk;
   friend class GoToInstruction;
   friend class LabelInstruction;
   friend class LabelBackPatchElement;
};

// Abstract constant

class Const {
public:
   virtual TypeSig typeSig(Void) const = 0;
   virtual Repr repr(Void) const = 0;
   virtual Bool needsEnv(Bool dst) const = 0;
   virtual Loc* loc(Repr repr, const ICode& code) const = 0;
   virtual Void print(ostream& os, const Env& env) const;

protected:
   virtual Void allocateData(Code& code) const;
   virtual Void insert(ostream& os, const Code& code) const = 0;
   virtual Void genByteCode(ostream& os, ByteCode& code) const = 0;
   virtual Void genByteCodeData(ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const = 0;
   virtual Bool suppressEnvMove(Void) const;

public:
   friend class ConstOperand;
   friend class IndexedLoc;
   friend class ByteCode;
};

// Const_NULL_Ptr: Used to respresent Null

class Const_NULL_Ptr : public Const {
public:
   Const_NULL_Ptr(TypeSig typeSig) : _typeSig(typeSig) {}
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual TypeSig typeSig(Void) const;
   virtual Repr repr(Void) const;
   virtual Bool needsEnv(Bool dst) const;
   virtual Loc* loc(Repr repr, const ICode& code) const;
   virtual Void print(ostream& os, const Env& env) const;

protected:
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
 
protected:
   TypeSig _typeSig;

   friend class ConstOperand;
};

// Const_NULL_Env: Used to respresent a null environment
// Primarily for security - to avoid exploits on stale values

class Const_NULL_Env : public Const {
public:
   Const_NULL_Env(Void) {}
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual TypeSig typeSig(Void) const;
   virtual Repr repr(Void) const;
   virtual Bool needsEnv(Bool dst) const;
   virtual Loc* loc(Repr repr, const ICode& code) const;
   virtual Void print(ostream& os, const Env& env) const;

protected:
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   friend class ConstOperand;
};

class Const_NULL_REF : public Const {
public:
   virtual TypeSig typeSig(Void) const;
   virtual Repr repr(Void) const;
   virtual Bool needsEnv(Bool dst) const;
   virtual Loc* loc(Repr repr, const ICode& code) const;

protected:
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

   friend class ConstOperand;
};

// Const_codeEnv: represents the code enviroment

class Const_codeEnv : public Const {
public:
   Const_codeEnv(Void) {}
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual TypeSig typeSig(Void) const;
   virtual Repr repr(Void) const;
   virtual Bool needsEnv(Bool dst) const;
   virtual Loc* loc(Repr repr, const ICode& code) const;

protected:
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:

   friend class ConstOperand;
};

// Const_consEnv: represents the constructor enviroment

class Const_consEnv : public Const {
public:
   Const_consEnv(Void) {}
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual TypeSig typeSig(Void) const;
   virtual Repr repr(Void) const;
   virtual Bool needsEnv(Bool dst) const;
   virtual Loc* loc(Repr repr, const ICode& code) const;

protected:
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:

   friend class ConstOperand;
};



template <class T>
class ConstT : public Const {
public:
   ConstT(T value) : _value(value) {}
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   virtual const T& value(Void) const { return _value; };

   virtual TypeSig typeSig(Void) const;
   virtual Repr repr(Void) const;
   virtual Bool needsEnv(Bool dst) const;
   virtual Loc* loc(Repr repr, const ICode& code) const;
   virtual Void allocateData(Code& code) const;
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genByteCodeData(ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
   virtual Bool suppressEnvMove(Void) const;

   virtual Void print(ostream& os, const Env& env) const;

protected:
   T  _value;
};
/*
template <class T>
inline Void Immediate<T>::insert(ostream& os, const Code& Code) const
{
   os << "#" << _value;
}
*/

class TypeConst {
public:
   TypeConst(Type type) { _type = type; }
   Type _type;
};

class Operand {
public:
   enum Kind {
      LABEL_OPERAND,
      CONST_OPERAND,
      VAR_OPERAND,
      STRUCT_OPERAND,
      SELECT_OPERAND,
      DEPTR_OPERAND,
      ARRAY_INDEX_OPERAND,
      BUILT_IN_FN_OPERAND,
      BUILT_IN_DATA_CON_OPERAND,
      THIS_OPERAND,
      CELL_INFO_OPERAND,
      UPDATE_RET_OPERAND,
      LOC_OPERAND,
   };

protected:
   Operand(Kind kind);
   virtual ~Operand(Void);
public:
   inline Kind kind(Void) const { return _kind; }

   virtual Operand* clone(MSA& msa) const;
   virtual Void destroy(MSA& msa);
   virtual UInt varRefCount(Void) const;
   virtual Bool referencesVar(const Var& var) const;
   virtual Void setUD(Bool dest, BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(Bool dest, MSA& msa);
   virtual Void setNextUse(Code& code) const;
   virtual Void allocateData(Code& code) const;
   virtual Void insert(ostream& os, const Code& Code) const = 0;
   virtual Repr repr(ICode& code);
   virtual Repr repr(Void);
   virtual Bool needsEnv(Bool dst);
   virtual Loc::Kind locKind(Repr repr, ICode& code);
   virtual Loc* loc(Repr repr, ICode& code);
   virtual Loc* locEnv(ICode& code);
   virtual Loc* allocLoc(Repr repr, Loc::Kind kind, Bool& reused, ICode& code);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Operand* genLocEnvCode(ICode& code);
   virtual Void setKillPending(Void) const;
   virtual Void setReadyToKill(Bool envOf) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
   virtual Void genDstSourceCode(ostream& os, SourceCode& code) const;

   virtual Bool suppressEnvMove(Void) const;

protected:
   Kind  _kind;

   friend class FnAp;

   friend class Code;
   friend class ICode;
   friend class SourceCode;
};

class LabelOperand : public Operand {
public:
   LabelOperand(CodeLabel& codeLabel);
   ~LabelOperand(Void);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline CodeLabel& codeLabel(Void) { return _codeLabel; }
   inline const CodeLabel& codeLabel(Void) const { return _codeLabel; }

   virtual Void destroy(MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(Bool dest, MSA& msa);
   virtual Void setNextUse(Code& code) const;
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Repr repr(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   CodeLabel&  _codeLabel;
   VarSet      _nextUses;

   friend class Code;
   friend class SourceCode;
   friend class Case;
   friend class LabelInstruction;
   friend class MonadicInstruction;
   friend class MoveInstruction;
   friend class ConditionalInstruction;
   friend class CaseInstruction;
   friend class GoToInstruction;
};

class VarOperand : public Operand {
public:
   VarOperand(Var& var, Operand::Kind kind = Operand::VAR_OPERAND, Bool reused = FALSE);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline Var& var(Void) { return _var; }
   inline Void setReserved(Bool q) { _reserved = q; }

   virtual Operand* clone(MSA& msa) const;
   Void destroy(MSA& msa);
   virtual UInt varRefCount(Void) const;
   virtual Var* var(Void) const;
   virtual Bool referencesVar(const Var& var) const;
   virtual Void setUD(Bool dest, BasicBlk& vars, MSA& msa);
   virtual Void setNextUse(Bool dest, MSA& msa);
   virtual Void setNextUse(Code& code) const;
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Repr repr(ICode& code);
   virtual Repr repr(Void);
   virtual Bool needsEnv(Bool dst);
   virtual Loc::Kind locKind(Repr repr, ICode& code);
   virtual Loc* loc(Repr repr, ICode& code);
   virtual Loc* allocLoc(Repr repr, Loc::Kind kind, Bool& reused, ICode& code);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Void setKillPending(Void) const;
   virtual Void setReadyToKill(Bool envOf) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Var&  _var;
   Repr  _repr;
   Bool  _needsEnvKnown;
   Bool  _needsEnv;
   Bool  _nextUse;
   Bool  _closureNextUse;
   Bool  _reused;
   Bool  _reserved;

   friend class Var;
   friend class Lambda;
   friend class Code;
   friend class SourceCode;
   friend class MoveInstruction;
   friend class MapGCInstruction;
   friend class StructInstruction;
   friend class MonadicInstruction;
   friend class DyadicOpInstruction;
   friend class GoToInstruction;
   friend class HeapAllocInstruction;
   friend class ClosureAllocInstruction;
   friend class PtrInstruction;
   friend class TagInstruction;
   friend class EnterCellInstruction;
   friend class Cast;
};

class ConstOperand : public Operand {
public:
   ConstOperand(Const& k) : Operand(CONST_OPERAND), _k(k) {}
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline const Const& k(Void) const { return _k; }

   virtual Operand* clone(MSA& msa) const;
   virtual Void allocateData(Code& code) const;
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Repr repr(ICode& code);
   virtual Repr repr(Void);
   virtual Bool needsEnv(Bool dst);
   virtual Bool suppressEnvMove(Void) const;
   virtual Loc* loc(Repr repr, ICode& code);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Operand* genLocEnvCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Const& _k;

   friend class Code;
   friend class ByteCode;
};

// Struct Operand

class StructOperand : public Operand {
public:
   StructOperand(UInt nComps, Operand** comps, StructTemplate& structTemplate, TypeSig typeSig);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   virtual Operand* clone(MSA& msa) const;
   virtual Void destroy(MSA& msa);
   virtual Bool referencesVar(const Var& var) const;
   virtual Void setUD(Bool dest, BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(Bool dest, MSA& msa);
   virtual Void setKillPending(Void) const;
   virtual Void setReadyToKill(Bool envOf) const;
   virtual Void setNextUse(Code& code) const;  
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Repr repr(ICode& code);
   Void checkUsage(Int upToIndex, Repr repr,
                   Loc& dstLoc, StructRepr& dstStructRepr,
                   UInt dstIndex,
                   Instruction* beforeIns, ICode& code);
   Void assignComps(Repr repr, Loc& loc, Instruction* beforeIns, ICode& code);

protected:
   UInt              _nComps;
   Operand**         _comps;
   StructTemplate&   _structTemplate;
   TypeSig           _typeSig;

   friend class Code;
   friend class ICode;
   friend class SourceCode;
};

// Tuple component Operand

class SelectOperand : public Operand {
public:
   SelectOperand(UInt index, Operand& operand,
                 TypeSig typeSig, Bool byPtr = FALSE);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline UInt index(Void) { return _index; }
   inline Operand& operand(Void) const { return *_operand; }
   inline Bool& byPtr(Void) { return _byPtr; }
   inline TypeSig typeSig(Void) const { return _typeSig; }
   inline Void setOperand(Operand* operand) { _operand = operand; }

   virtual Loc::Kind locKind(Repr repr, ICode& code);
   virtual Operand* clone(MSA& msa) const;
   Void destroy(MSA& msa);
   virtual UInt varRefCount(Void) const;
   virtual Bool referencesVar(const Var& var) const;
   virtual Void setUD(Bool dest, BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(Bool dest, MSA& msa);
   virtual Void setKillPending(Void) const;
   virtual Void setReadyToKill(Bool envOf) const;
   virtual Void setNextUse(Code& code) const;
   virtual Repr repr(ICode& code);
   virtual Repr repr(Void);
   virtual Bool needsEnv(Bool dst);
   virtual Void insert(ostream& os, const Code& code) const;
   
   virtual Loc* loc(Repr repr, ICode& code);
   virtual Loc* locEnv(ICode& code);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Operand* genLocEnvCode(ICode& code);

protected:
   UInt        _index;
   Operand*    _operand;
   Bool        _byPtr;
   TypeSig     _typeSig;
   ReprInfo    _reprInfo;

   friend class Code;
   friend class ICode;
   friend class SourceCode;
};

// DePtrOperand: Primitive Ptr pattern match

class DePtrOperand : public Operand {
public:
   DePtrOperand(Operand& operand);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline Operand* operand(Void) { return _operand; }

   virtual Operand* clone(MSA& msa) const;
   Void destroy(MSA& msa);
   virtual UInt varRefCount(Void) const;
   virtual Bool referencesVar(const Var& var) const;
   virtual Void setUD(Bool dest, BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(Bool dest, MSA& msa);
   virtual Void setKillPending(Void) const;
   virtual Void setReadyToKill(Bool envOf) const;
   virtual Void setNextUse(Code& code) const;
   virtual Repr repr(ICode& code);
   virtual Repr repr(Void);
   virtual Bool needsEnv(Bool dst);
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Loc::Kind locKind(Repr repr, ICode& code);
   virtual Loc* loc(Repr repr, ICode& code);
   virtual Loc* locEnv(ICode& code);
   virtual Loc* allocLoc(Repr repr, Loc::Kind kind, Bool& reused, ICode& code);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Operand* genLocEnvCode(ICode& code);

protected:
   Operand* _operand;

   friend class Code;
   friend class ICode;
   friend class SourceCode;
};

// ArrayIndexOperand: Array index operand

class ArrayIndexOperand : public Operand {
public:
   ArrayIndexOperand(Operand& ptrArrayOperand, Operand& indexOperand);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   virtual Operand* clone(MSA& msa) const;
   Void destroy(MSA& msa);
   virtual UInt varRefCount(Void) const;
   virtual Bool referencesVar(const Var& var) const;
   virtual Void setUD(Bool dest, BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(Bool dest, MSA& msa);
   virtual Void setKillPending(Void) const;
   virtual Void setReadyToKill(Bool envOf) const;
   virtual Void setNextUse(Code& code) const;
   virtual Repr repr(ICode& code);
   virtual Void insert(ostream& os, const Code& code) const;
   virtual Loc::Kind locKind(Repr repr, ICode& code);
   virtual Loc* loc(Repr repr, ICode& code);
   virtual Loc* locEnv(ICode& code);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Operand* genLocEnvCode(ICode& code);

protected:
   Operand* _arrayOperand;
   Operand* _indexOperand;
   friend class Code;
   friend class ICode;
   friend class SourceCode;
};

class CellInfoOperand : public Operand {
public:
   CellInfoOperand(CellInfo* cellInfo, CellInfo_* cellInfo_);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   virtual Void destroy(MSA& msa);
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   CellInfo*   _cellInfo;
   CellInfo_*  _cellInfo_;
};

class BuiltInFnOperand : public Operand {
public:
   BuiltInFnOperand(Cell* builtInFn, Bool asClosure = TRUE, Bool altEntryFlag = TRUE);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   virtual Operand* clone(MSA& msa) const;
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Repr repr(ICode& code);
   virtual const Repr repr(ICode& code) const;
   virtual Bool needsEnv(Bool dst);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Operand* genLocEnvCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
 
protected:
   Cell* _builtInFn;
   Bool  _asClosure;
   Bool  _altEntryFlag;

   friend class Code;
   friend class GoToInstruction;
};


class BuiltInDataConOperand : public Operand {
public:
   BuiltInDataConOperand(Expr dataCon, Bool altEntryFlag = TRUE)
      : Operand(BUILT_IN_DATA_CON_OPERAND),
        _dataCon(dataCon),
        _altEntryFlag(altEntryFlag) {}
   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   virtual Operand* clone(MSA& msa) const;
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Repr repr(ICode& code);
   virtual const Repr repr(ICode& code) const;
   virtual Bool needsEnv(Bool dst);
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Operand* genLocEnvCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Expr  _dataCon;
   Bool  _altEntryFlag;

   friend class Code;
   friend class GoToInstruction;
};

// EnvOperand: Environment operand

class EnvOperand : public Operand {
public:
   EnvOperand(Operand& operand);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Operand* genLocCode(Repr repr, ICode& code);
   virtual Void setLocsKillPending(Repr repr) const;
   virtual Repr repr(ICode& code);
   virtual Bool needsEnv(Bool dst);

//   virtual Void genByteCode(ostream& os, ByteCode& code) const;
//   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Operand& _operand;

   friend class Code;
   friend class ICode;
   friend class SourceCode;
};

class ThisOperand : public Operand {
public:
   ThisOperand(Void);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   virtual Bool needsEnv(Bool dst) const;
   virtual Void insert(ostream& os, const Code& Code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
};

// Var: A variable

class Var {
public:
   enum Kind {
      GLOBAL_VAR,             // Global
      EXT_GLOBAL_VAR,         // Global reference 
      FORMAL_PARAM_VAR,       // Formal parameter
      EXTRA_FORMAL_PARAM_VAR, // Extra formal parameter
      ARG_VAR,                // Function argument
      EXTRA_ARG_VAR,          // Function extra argument
      LOCAL_ARG_VAR,          // Local function argument
      LOCAL_VAR,              // Local
      CLOSURE_VAR,            // Closure
      NON_GLOBAL_FREE_VAR,    // Non-global free
      CONTINUATION_VAR,       // Continuation

#if (CLOSURE_UPDATE==1)
      UPDATE_CELL_VAR,
#endif

      RESULT_VAR,             // Result
   };
public:
   Var(Name name, TypeSig typeSig, Kind kind, Lambda* lambda = NULL);
   inline void* operator new(size_t size, MSA& msa) { return msa.alloc(size); }
   inline void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   Void operator delete(Void* ptr, MSA& msa) {}
	Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	Void operator delete(Void* ptr) {}
#endif

   inline Name name(Void) const { return _name; }
   inline Name& name(Void) { return _name; }
   inline TypeSig typeSig(Void) const { return _typeSig; }
   inline Kind kind(Void) const { return _kind; }
   inline const Lambda* parentLambda(Void) const { return _parentLambda; }
   inline Lambda* parentLambda(Void) { return _parentLambda; }
   inline Lambda* lambda(Void) { return _lambda; }
   inline const Lambda* lambda(Void) const { return _lambda; }
   inline Bool reserved(Void) const { return _reserved; }
   inline Bool nextUse(Void) const { return _nextUse; }
   inline Bool lastNextUse(Void) const { return _lastNextUse; }
   inline Bool killPending(Void) const { return _killPending; }
   inline StructTemplate* structTemplate(Void) const {
      return _structTemplate; }
   inline CellInfo_* cellInfo(Void) const { return _cellInfo; }
   inline Var* closure(Void) { return _closure; }
   inline const Var* closure(Void) const { return _closure; }
   inline UInt refCount(Void) const { return _refCount; }
   inline UInt entrySkipCount(Void) const { return _entrySkipCount; }
   inline LocSet& locSet(Void) { return _locs; }

#if (GARBAGE_COLLECTION==1)
   inline CodeLabel* gcLab(Void) const { return _gcLab; }
   inline UInt gcStackOffset(Void) const { return _gcStackOffset; }
   inline UInt gcEnvStackOffset(Void) const { return _gcEnvStackOffset; }

   inline Void gcSetLab(CodeLabel& lab) { _gcLab = &lab; }
   inline Void gcSetStackOffset(UInt offset) { _gcStackOffset = offset; }
   inline Void gcSetEnvStackOffset(UInt offset) { _gcEnvStackOffset = offset; }
#endif

   inline Void setParentLambda(Lambda* lambda) { _parentLambda = lambda; }
   inline Void setReservedDefn(Defn* defn) { _reservedDefn = defn; }

   Void insTypeSig(TypeSig& typeSig, TypeEnv* typeEnv, TypeCheck& typeCheck);

   Expr copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const;

   Expr reduceCell(TypeSig typeSig, Code& code);
   Expr reduce(Code& code);
   Void gen(CodeLabel& errLab, Code& code);
   Void genVar(Var& dst, Code& code);
	Void reduceVar(Var& dst, Code& code);
   Void genExpr(Var& dst, Code& code);
   Operand& genOperand(TypeSig typeSig, Code& code);
   Void genSelect(Bool byPtr, UInt index, Var& dst, Code& code);
   Void genCond(CodeLabel& labT, CodeLabel& labF,
                Bool divergent, Code& code);
   Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                       Bool divergent, CodeLabel& failLab, Code& code);
   Void genEnter(FnAp* fnAp, Code& code);
   Void genEnterReduced(FnAp* fnAp, Code& code);
   Void genReturn(Code& code);
   Void genReturnReduced(Code& code);
   Void insert(ostream& os, const Code& Code) const;
   Repr repr(ICode & code);
   Repr repr(Void) const;
   Bool needsEnv(Void);
   inline Void setReserved(Void) { _reserved = TRUE; }
   Void adjRefCount(Int diff);
   Void adjEntrySkipCount(Int diff) { _entrySkipCount += diff; }
   Void setNextUse(Bool nextUse);
   Void setLastNextUse(Bool lastNextUse);

   Loc::Kind locKind(Repr repr);
   Loc* loc(Repr repr, ICode& code) const;
   Void addLoc(Loc* loc, MSA& msa);
   Loc& headLoc(Void) const;
   Loc* bestLocOrNULL(Repr repr, Bool regFlag = TRUE) const;
   Loc& bestLoc(Repr repr, Bool regFlag = TRUE) const;
   Bool hasReservedLocs(Void) const;
   Var* lambdaArgVar(Void) const;
   UInt stackOffset(Void);
   Bool inLoc(Loc& loc, Repr repr);
   Bool isLive(Void) const;
   Void checkLive(Code& code);
   Void setKillPending(Bool killPending = TRUE) { _killPending = killPending; };
   Void setReadyToKill(Bool envOf);
   const char* kindMnemonic(Void) const;
   Void printType(ostream& os, const Env& env) const;
   Void printRepr(ostream& os, const Env& env) const;
   Void printLocs(ostream& os, const Env& env) const;

public:
   Name              _name;            // Name
   TypeSig           _typeSig;         // Type signature
   Kind              _kind;            // Kind
   Lambda*           _parentLambda;    // Inner enclosing lambda
   Lambda*           _lambda;          // Known lambda for closure or NULL
   ReprInfo          _reprInfo;        // Representation
   Defn*             _reservedDefn;    // Reserved definition for mutually recursive definitions
   Bool              _reserved;        // True if reserved location
   Bool              _nextUse;         // Next use for live variable analysis
   Bool              _lastNextUse;     // 'nextUse' after last instruction
   Bool              _killPending;     // For source variables not live after an instruction
   StructTemplate*   _structTemplate;  // Cell free variable template for closure
   CellInfo_*        _cellInfo;        // Cell information for closure
   Var*              _closure;         // Closure for free variable slot
   UInt              _index;           // Free variable slot index
   UInt              _refCount;        // Reference count 
   UInt              _entrySkipCount;  // Entry count skipped by jump
   CodeLabel*        _contLabel;       // Continuation label if kind is CONTINUATION_VAR
   LocSet            _locs;            // Associated locations

#if (GARBAGE_COLLECTION==1)
   CodeLabel*        _gcLab;              // Garbage collection label
   UInt              _gcStackOffset;      // Associated stack offset
   UInt              _gcEnvStackOffset;   // (Env)
#endif

};

class VarElement {
public:
   enum Flags {
      D = 1,   // Defined in block
      U = 2,   // Used in block
      I = 4,   // Input to block
      O = 8,   // Out from block
      X = 16,  // Exported from block
   };
   VarElement(VarElement* next, Var& var);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline VarElement* next(Void) const { return _next; }
   inline const Var& var(Void) const { return _var; }
   inline Var& var(Void) { return _var; }
   inline LocSet& locSet(Void) { return _locs; };

   inline Flags flags(Void) const { return _flags; }
   inline Void setFlags(enum Flags flags) {
      _flags = (Flags)((unsigned char)_flags | (unsigned char)flags);
   }
   inline Void clearFlags(enum Flags flags) {
      _flags = (Flags)((unsigned char)_flags & ~(unsigned char)flags);
   }
   inline Bool testFlags(enum Flags flags) const {
      return ((unsigned char)_flags & (unsigned char)flags) != 0;
   }

protected:
   VarElement*    _next;
   Var&           _var;
   Bool           _nextUse;
   Flags          _flags;
   LocSet         _locs;

   friend class Code;
   friend class ICode;
   friend class SourceCode;
   friend class BasicBlk;
   friend class VarSet;
   friend class Loc;
   friend class MoveInstruction;
   friend class ConditionalInstruction;
   friend class LabelOperand;
};

class Operator {
public:
   enum Kind {
      UNKNOWN_OP,

      FROM_THUNK_OP,

      CELL_INFO_OF_OP,

      MAP_OP,

#if (SERIALISATION==1)
      EXTRACT_OP,
      INSERT_OP,
#endif

      MAP_CLOSURE_OP,
      EXTRACT_FREE_VARS_OP,
      INSERT_FREE_VARS_OP,

      UPDATE_OP,

      LT_OP,
      LTEQ_OP,
      EQ_OP,
      NEQ_OP,
      GTEQ_OP,
      GT_OP,

      LTZ_OP,
      LTEQZ_OP,
      EQZ_OP,
      NEQZ_OP,
      GTEQZ_OP,
      GTZ_OP,

      NEG_OP,

      ADD_OP,
      SUB_OP,
      MUL_OP,
      DIV_OP,
      MOD_OP,

      FROM_INT_OP,
      FROM_FLOAT_OP,
      FROM_DOUBLE_OP,
      FROM_STRING_OP,

      NOT_OP,

      AND_OP,
      OR_OP,
      XOR_OP,
      L_SHIFT_OP,
      R_SHIFT_OP,

      SPECIAL_EXPR_OP,
      ENTER_SPECIAL_EXPR_OP,

      TYPE_OF_OP,
      CAST_PTR_OP,
      ENV_OF_OP,
      SET_ENV_OP,
      FROM_PLAIN_OP,
      SIZE_OF_OP,
      OBJECT_TYPE_OP,
      TAG_OF_OP,
      CAST_TAG_OP,
      CONSTRUCTOR_STRING_OP,
      CONSTRUCT_FROM_NAME_OP,
      CONSTRUCT_FROM_STRING_OP,

      EQ_NULL_OP,
      NEQ_NULL_OP,

      LENGTH_STRING_OP,
      GET_AT_STRING_OP,
      PUT_AT_STRING_OP,

      CONS_OP,

      ALLOC_PTR_ARRAY_OP,
      COPY_PTR_ARRAY_OP,
      GET_AT_PTR_ARRAY_OP,
      PUT_AT_PTR_ARRAY_OP,
      GET_AT_PLAIN_PTR_ARRAY_OP,
      PUT_AT_PLAIN_PTR_ARRAY_OP,

      REV_REFS_OP,

      ASSIGN_AT_OP,

#if (GARBAGE_COLLECTION==1) 
      MARK_FREE_VARS_GC_OP,
      MARK_PTR_GC_OP,
      NOT_MARK_PTR_GC_OP,
      MARK_EXPR_PTR_GC_OP,
      NOT_MARK_EXPR_PTR_GC_OP,
      MARK_EXPR_GC_OP,
      MARK_CELL_GC_OP,
      MARK_STRING_GC_OP,
#endif

      ERROR_OP,
      TRACE_OP,
   };
   enum Type {
      OP_TYPE_BITS,
      OP_TYPE_BOOLEAN,
      OP_TYPE_BYTE,
      OP_TYPE_CELL_INFO,
      OP_TYPE_CHAR,
      OP_TYPE_CLOSURE,
      OP_TYPE_DOUBLE,
      OP_TYPE_ENV,
      OP_TYPE_EXPR,
      OP_TYPE_FLOAT,
      OP_TYPE_INT,
      OP_TYPE_NAME,  
      OP_TYPE_LIST,
      OP_TYPE_POLY,     // Polymorphic
      OP_TYPE_PTR,
      OP_TYPE_PTR_ARRAY,
      OP_TYPE_REF,
      OP_TYPE_REFLIST,
      OP_TYPE_STRING,
      OP_TYPE_TAG,
      OP_TYPE_TYPE,
      OP_TYPE_UNKNOWN,
      OP_TYPE_UTC,
      OP_TYPE_VOID
   };

public:
   Operator(Kind kind, Type type, UInt arity);
   Operator(const char* fnStr);
   Kind kind() const { return _kind; }
   Type type_() const { return _type; }
   UInt arity_() const { return _arity; }
   Bool mapNeedsGCFrame(Void) const;

   Operator inverse(Void) const;
   const char* mnemonic(Void) const;
   const char* sourceSymbol(Void) const;
   Byte byteCode(Bool constOperand) const;
   Byte branchByteCode(Void) const;

   struct PrimOpMapping {
      const char*    _fnStr;
      Kind           _kind;
      Type           _type;
      UInt           _arity;
   };

protected:
   Kind        _kind;
   Type        _type;
   UInt        _arity;

   friend class ConditionalInstruction;   
};

class CodeNode {
protected:
   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Bool isConstReduced(Expr& val, const Code& code) const;
   virtual Bool genExpr(Expr, Var& dst, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genReturn(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);

   Void notOverloaded(ConstString methodStr, const Env& env) const;

   friend class FnAp;
   friend class Cond;
   friend class Code;
};

class Cond : public CodeNode {
public:
   Cond(TypeSig typeSig, Expr ePred, Expr eTrue, Expr eFalse);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Bool isConstReduced(Expr& val, const Code& code) const;
   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);

protected:
   TypeSig  _typeSig;
   Expr     _ePred;
   Expr     _eTrue;
   Expr     _eFalse;
};

class ReturnState {
public:
   ReturnState(TypeSig typeSig);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif

   Void genContinuation(Code& code);
   Expr var(TypeSig typeSig, Code& code);
   Void genVar(Var& dst, Code& code);
   Operand& genOperand(TypeSig typeSig, Code& code);
   virtual Void genSelect(TypeSig typeSig, Bool byPtr, UInt index, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCond(CodeLabel& labT, CodeLabel& labF,
                        Bool divergent, Code& code);
   virtual Void genEnter(TypeSig typeSig, FnAp* fnAp, Code& code);
   virtual Void genEnterReduced(TypeSig typeSig, FnAp* fnAp, Code& code);
   virtual Void genExpr(Var& dst, Code& code);
   virtual Expr reduce(Code& code);

   TypeSig        _typeSig;
   CodeLabel*     _contLabel;
   Var*           _contVar;
   Expr           _vars;
   ReturnState*   _returnState;  // Saved return state
};

// AllocClosure: Intended only to obtain (statically) the cell template of a closure
// and allocate a copy in the destination environment

class AllocClosure : public CodeNode {
public:
   AllocClosure(Expr env);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);

protected:
   Expr  _env;
};

// MapClosure: Intended only for copying a closure and
// mapping its free variables between environments

class MapClosure : public CodeNode {
public:
   MapClosure(Expr env);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
private:
   Expr _env;
};

// ExtractFreeVars: Intended only to extract the free variables of
// a closure from a byte input stream

class ExtractFreeVars : public CodeNode {
public:
   ExtractFreeVars(Expr is, Expr closure);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);

protected:
   Expr     _is;
   Expr     _closure;
};

// InsertFreeVars: Intended only to insert the free variables of
// a closure into a byte output stream

class InsertFreeVars : public CodeNode {
public:
   InsertFreeVars(Expr os, Expr closure);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);

protected:
   Expr     _os;
   Expr     _closure;
};

#if (GARBAGE_COLLECTION==1)
// MarkFreeVars_GC: Intended only for marking the free variables of
// a closure

class MarkFreeVars_GC : public CodeNode {
public:
   MarkFreeVars_GC(Expr closure);
   inline Void* operator new(size_t size, MSA& msa) { Void* ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);

protected:
   Expr  _closure;
};
#endif

// DirectLambda: A closure-less lambda function entered via a label

class DirectLambda {
public:
   DirectLambda(Lambda* lambda);
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }

#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   Void genEnter(const FnAp* fnAp, Code& code) const;

protected:
   Lambda* _lambda;
};

// Constructor: Implements the construction of a value of a non built-in type

class Constructor : public CodeNode {
public:
   Constructor(const FnAp& fnAp, Bool isSum);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Bool isConstReduced(Expr& val, const Code& code) const;
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Bool genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);

protected:
   const FnAp& _fnAp;
   Bool        _isSum;
   Tag         _tag;

};

// Deconstructor: Deconstructs a value of one type to its representing type

class Deconstructor : public CodeNode {
public:
   Deconstructor(const FnAp& fnAp, Bool isSum, Bool byPtr);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Bool genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code);

protected:
   const FnAp& _fnAp;
   Bool        _isSum;
   Bool        _byPtr;
   Tag         _tag;
};

class PtrConstructor : public CodeNode {
public:
   PtrConstructor(const FnAp& fnAp);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Bool genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code);

protected:
   const FnAp& _fnAp;
};

class TupleConstructor : public CodeNode {
public:
   TupleConstructor(const FnAp& fnAp);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Bool genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code);

protected:
   const FnAp& _fnAp;
};

class Select : public CodeNode {
public:
   Select(TypeSig typeSig, Expr dataCon, Bool byPtr, UInt index, Expr arg);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Bool genExpr(Expr expr, Var& dst, CodeLabel& failLab, Code& code);

protected:
   TypeSig  _typeSig;
   Expr     _dataCon;
   Bool     _byPtr;
   UInt     _index;
   Expr     _arg;
};

class MonadicOp : public CodeNode {
public:
   MonadicOp(Operator op, TypeSig typeSig, Expr x);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);

protected:
   Operator _op;
   TypeSig  _typeSig;
   Expr     _x;

   friend class Cond;
};

class TypeOfOp : public MonadicOp {
public:
   TypeOfOp(Operator op, TypeSig typeSig, Expr x);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
};

class EnvOfOp : public MonadicOp {
public:
   EnvOfOp(Operator op, TypeSig typeSig, Expr x);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
};

class TagOfOp : public MonadicOp {
public:
   TagOfOp(TypeSig typeSig, Expr x);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
};

class ConstructorStringOp : public MonadicOp {
public:
   ConstructorStringOp(TypeSig typeSig, Expr x);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
};

class ConstructFromNameOp : public MonadicOp {
public:
   ConstructFromNameOp(TypeSig typeSig, Expr x);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
};

class ConstructFromStringOp : public MonadicOp {
public:
   ConstructFromStringOp(TypeSig typeSig, Expr x);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
};


class IdOp : public MonadicOp {
public:
   IdOp(Operator op, TypeSig typeSig, Expr x);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
};

class DyadicOp : public CodeNode {
public:
   DyadicOp(Operator op, TypeSig typeSig, Expr x, Expr y);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);

protected:
   Operator _op;
   TypeSig  _typeSig;
   Expr     _x;
   Expr     _y;

   friend class Cond;
};

class TriadicOp : public CodeNode {
public:
   TriadicOp(Operator op, TypeSig typeSig, Expr x, Expr y, Expr z);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);

protected:
   Operator _op;
   TypeSig  _typeSig;
   Expr     _x;
   Expr     _y;
   Expr     _z;
};

class QuinadicOp : public CodeNode {
public:
   QuinadicOp(Operator op, TypeSig typeSig, Expr v, Expr w, Expr x, Expr y, Expr z);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);

protected:
   Operator _op;
   TypeSig  _typeSig;
   Expr     _v;
   Expr     _w;
   Expr     _x;
   Expr     _y;
   Expr     _z;
};

class FatBar : public CodeNode{
public:
   FatBar(TypeSig typeSig, Expr x, Expr y);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
	inline Void operator delete(Void* ptr) {}
#endif
   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);

protected:
   TypeSig  _typeSig;
   Expr     _x;
   Expr     _y;
};

class Seq : public CodeNode {
public:
   Seq(Expr e1, Expr e2);
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Expr reduce(CodeLabel& failLab, Code& code);
   virtual Void genVoidReduced(CodeLabel& failLab, Code& code);
   virtual Void genReturnReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);
   virtual Void genVarReduced(Var& dst, CodeLabel& failLab, Code& code);
   virtual Void genCondReduced(UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                               Bool divergent, CodeLabel& failLab, Code& code);
   virtual Operand& genOperandReduced(TypeSig typeSig, CodeLabel& failLab, Code& code);

protected:
   Expr     _e1;
   Expr     _e2;
};

// InstructionTrait: Abstract instruction methods

class InstructionTrait {
public:
   InstructionTrait(Void);
   virtual ~InstructionTrait(Void);
   virtual Void destroy(MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& Code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void allocateData(Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;
  
protected:
};

class Instruction : public InstructionTrait {
public:
   enum Kind {
      TYPE_DESCR_INSTR,
      LABEL_INSTR,
      RESERVE_INSTR,
      MOVE_INSTR,
      LVP_INSTR,
      CONTINUATION_INSTR,
      UPDATE_INSTR,
      MAP_INSTR,
      RSS_INSTR,
      RETURN_INSTR,
      MONADIC_OP_ACTION_INSTR,
      MONADIC_OP_INSTR,
      DYADIC_OP_INSTR,
      DYADIC_OP_ACTION_INSTR,
      TRIADIC_OP_ACTION_INSTR,
      QUINADIC_OP_ACTION_INSTR,
      GOTO_INSTR,
      COND_INSTR,
      CASE_INSTR,
      END_CASE_INSTR,
      ENTRY_INSTR,
      CELL_INFO_INSTR,
      CELL_INFO_DATA_INSTR,

#if (GARBAGE_COLLECTION==1)
      FRAME_DESCR_INSTR,
#endif

      HEAP_ALLOC_INSTR,
      CELL_ALLOC_INSTR,
      ANY_INSTR,
      PTR_INSTR,
      PTR_ARRAY_ALLOC_INSTR,
      INDIRECTION_ALLOC_INSTR,
      CONS_INSTR,
      CONS_S_INSTR,
      CONS_A_S_INSTR,
      DECONS_INSTR,
      DECONS_S_INSTR,
      FROM_PLAIN_INSTR,
      STRUCT_INSTR,
      ENV_OF_INSTR,
      TAG_OF_INSTR,
      CAST_TAG_INSTR,
      PTR_ARRAY_ASSIGN_INSTR,
      TAG_INSTR,
      CONSTRUCT_FROM_NAME_INSTR,
      CONSTRUCT_FROM_STRING_INSTR,
      FROM_CELL_INSTR,
      STACK_ADJUST_INSTR,
      ENTER_CELL_INSTR,
      ENTER_SPECIAL_INSTR,
      TO_CELL_INSTR,
      EXCEPTION_INSTR,
      ARG_CHECK_INSTR,
      CLOSURE_UPDATE_INSTR,
      EMPTY_ARGS_INSTR,
      SQUEEZE_INSTR

#if (GARBAGE_COLLECTION==1)
     ,GC_STUB_INSTR
     ,MARK_CELL_INSTR
#endif

   };

public:
   Instruction(Kind kind);
   virtual const char* mnemonic(Void) const = 0;
   virtual Void insert(ostream& os, Code& Code) const;

   Void addBackPatch(BackPatchElement* element);

   inline Kind kind(Void) const { return _kind; }
   inline Instruction* prev(Void) const { return _prev; }
   inline Instruction* next(Void) const { return _next; }
   inline UInt seqNo(Void) const { return _seqNo; }
   inline Lambda* lambda(Void) const { return _lambda; }

protected:
   Kind              _kind;
   Instruction*      _prev;
   Instruction*      _next;
   UInt              _seqNo;
   Lambda*           _lambda;          // Containing lambda - used to determine segment
   BackPatchElement* _backPatchList;   // List of backpatches relative to end of instruction fields

   friend class InstrSeq;
   friend class VarSet;
   friend class Code;
   friend class ICode;
   friend class SourceCode;
   friend class ByteCode;
   friend class BasicBlk;
   friend class LabelBackPatchElement;
   friend class MonadicInstruction;
   friend class DyadicOpInstruction;
   friend class StructInstruction;
   friend class ConsInstruction;
   friend class Cons_A_S_Instruction;
   friend class Cons_S_Instruction;
   friend class DeconsInstruction;
   friend class SelPtrInstruction;
   friend class PtrInstruction;
   friend class PtrArrayAllocInstruction;
   friend class CellAllocInstruction;
};

// Abstract class to retain state of location code generation

class LocCodeGenState {
};

// Instruction sequence

class InstrSeq {
public:
   InstrSeq(Void);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Instruction* first(Void) const { return _first; }

   Void append(Instruction* ins);
   Void append(const InstrSeq& instrSeq);
   Void append(InstrSeq* instrSeq);
   Void insert(const InstrSeq& instrSeq, Instruction* beforeIns);
   Void append(Void);
   Void insert(Instruction* ins, Instruction* beforeIns);
   Void append(Instruction* ins, Instruction* afterIns);
   Void remove(Instruction* ins);

protected:
   Instruction*   _first;
   Instruction*   _last;
   InstrSeq*      _appendSeq;

   friend class Code;
   friend class Lambda;
   friend class ReturnState;
	friend class ModuleDefn;
   friend class Order;
};

// An instruction trait for compound data, used for tuples and closures

class CompoundDataInstructionTrait : public InstructionTrait {
public:
   CompoundDataInstructionTrait(UInt nComps, Operand** comps, Int slot, StructTemplate& structTemplate);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   CompoundDataInstructionTrait* getNest(UInt slot) const;

   virtual Void destroy(MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   Bool instructionDefinesVar(Instruction* ins) const;
   Void setUD(Bool dest, BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void setNextUse(Code& code);
   Void insertSuffix(Int index, ostream& os, Code& code) const;
   virtual Void insertSuffix(ostream& os, Code& Code) const;
   Void assignComps(StructTemplate& structTemplate, Loc* dstLoc,
                    Instruction* beforeIns, ICode& code);
   Void assignComps(Repr repr, Loc* dstLoc, UInt index,
                    Instruction* beforeIns, ICode& code);

protected:
   UInt                          _nComps;
   Operand**                     _comps;
   Int                           _slot;
   StructTemplate&               _structTemplate;

// Currently implements directly nested compound data

   CompoundDataInstructionTrait* _next;
   CompoundDataInstructionTrait* _nests;

   friend class Code;
};

// An instruction trait for heap data, used for closures and values of type Ptr

class HeapInstructionTrait : public InstructionTrait {
};

class LabelInstruction : public Instruction {
public:
   LabelInstruction(CodeLabel& label);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline CodeLabel& codeLabel(Void) const { return _codeLabel; }

   virtual Void destroy(MSA& msa);
   virtual const char* mnemonic(Void) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& iCode);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   CodeLabel&  _codeLabel;

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
   friend class GoToInstruction;
};

// TypeDescrInstruction: Allocate a type descriptor

class TypeDescrInstruction : public Instruction {
public:
   TypeDescrInstruction(ConstOperand& type,
                        ConstOperand& denotedType,
                        LabelOperand* evalFn,
                        LabelOperand* mapFn,
                        LabelOperand* assignFn,
                        LabelOperand* insertTxtFn

#if (SERIALISATION==1)
                      , LabelOperand* extractBinFn,
                        LabelOperand* insertBinFn
#endif

#if (GARBAGE_COLLECTION==1)
                      , LabelOperand* markFn
#endif

                       );

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void destroy(MSA& msa);
   virtual const char* mnemonic(Void) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:

   ConstOperand&  _type;
   ConstOperand&  _denotedType;
   LabelOperand*  _evalFn;
   LabelOperand*  _mapFn;
   LabelOperand*  _assignFn;
   LabelOperand*  _insertTxtFn;

#if (SERIALISATION==1)
   LabelOperand*  _extractBinFn;
   LabelOperand*  _insertBinFn;
#endif

#if (GARBAGE_COLLECTION==1)
   LabelOperand* _markFn;
#endif

   friend class Code;
   friend class SourceCode;
};

// ReserveInstruction: Reserve location for variable

class ReserveInstruction : public Instruction {
public:
   ReserveInstruction(VarOperand& x);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;

   Loc* allocLoc(Repr repr, Var& var, Loc::Kind kind, ICode& code);
   virtual Void genLocCode(ICode& code);
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;

protected:
   Operand* _x;
   Operand* _x_env;

   friend class Code;
};

// AssignInstruction: abstract instruction for assignment

class AssignInstruction : public Instruction {
public:
   AssignInstruction(Instruction::Kind kind, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Operand& dst(Void) const { return *_dst; }

   virtual Void destroy(MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void setSrcReadyToKill(Instruction* beforeIns, LocCodeGenState& state, ICode& code) = 0;
   virtual Void genSrcLocCode(Instruction* beforeIns, LocCodeGenState& state, ICode& code) = 0;
   virtual Void assignLoc(Operand& dst, Repr repr, Loc::Kind kind,
                          Instruction* beforeIns, LocCodeGenState& state, ICode& code);
   virtual Void setSrcNextUse(LocCodeGenState& state, ICode& code) = 0;
   Void genLocCode(LocCodeGenState& state, ICode& code);

protected:
   Operand*  _dst;

   friend class Code;
};

// MonadicInstruction: abstract single source/single destination instruction

class MonadicInstruction : public Instruction {
public:
   MonadicInstruction(Kind kind, Operand& src, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Operand& src(Void) const { return *_src; }
   inline Operand& dst(Void) const { return *_dst; }

   virtual Void destroy(MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Bool usesRegisters(Void) const;
   virtual Bool needsSrcEnvLoc(Void) const;
   virtual Loc& allocLoc(Repr repr, Operand& dst, Loc::Kind kind,
                         Bool& reused, ICode& code) const;
   virtual Operand* genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code);
   virtual Operand* genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code);
   virtual Operand* genDstLocCode(Loc& loc, ICode& code);
   virtual Operand* genDstLocEnvCode(Operand& src, Var* srcVar, Loc& loc, ICode& code);
   Void assignLoc(Operand& src, Repr srcRepr, Var* srcVar, 
                  Operand& dst, Repr repr, Loc::Kind kind, Var* dstVar, 
                  Bool& needsEnv, Bool& suppressed,
                  Instruction* beforeIns,
                  ICode& code);

   virtual Void genLocCode(ICode& code);
   virtual Void insertSuffix(ostream& os, Code& code) const;

protected:
   Operand* _src;
   Operand* _src_env;
   Operand* _dst;

   friend class Code;
};

// MoveInstruction : instruction to move from source to destination

class MoveInstruction : public MonadicInstruction {
public:
   MoveInstruction(Operand& src, Operand& dst);
   MoveInstruction(Instruction::Kind kind, Operand& src, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Bool isConstructor(Void) const { return _isConstructor; }
   inline Void setIsConstructor(Void) { _isConstructor = TRUE; }

   virtual const char* mnemonic(Void) const;

   virtual Void allocateData(Code& code) const;

   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;

protected:
   Bool _isConstructor;       // TRUE if move constructs a type, FALSE otherwise
};

// LVP_Instruction : instruction to move the address of a source left value

class LVP_Instruction : public MoveInstruction {
public:
   LVP_Instruction(Operand& src, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Bool needsSrcEnvLoc(Void) const;
   virtual Operand* genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code);

   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;
};

// ContinuationInstruction : instruction to create a continuation variable

class ContinuationInstruction : public MoveInstruction {
public:
   ContinuationInstruction(LabelOperand& src,
      VarOperand& dst

#if (CLOSURE_UPDATE==1)
      , Bool forUpdate
#endif

#if (GARBAGE_COLLECTION==1)
      , Bool gcFlag
#endif
   );

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Void genLocCode(ICode& code);
protected:

#if (CLOSURE_UPDATE==1)
   bool  _forUpdate;
#endif

#if (GARBAGE_COLLECTION==1)
   bool  _gcFlag;
#endif
};

// Cons_Instruction : instruction to construct a value of a product type
//                    from its representing type

class ConsInstruction : public MoveInstruction {
public:
   ConsInstruction(Operand& src, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   Void assignDst(Operand& src, Repr srcRepr,
                  Repr repr, Var& dstVar, Instruction* beforeIns, ICode& code);
   virtual Void genLocCode(ICode& code);

protected:
   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// Cons_S_Instruction : instruction to construct a value of a sum type
//                      from a value of one of its representing types

class Cons_S_Instruction : public MonadicInstruction {
public:
   Cons_S_Instruction(Operand& src, Operand& tag, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   Void assignDst(Operand& src, Repr srcRepr,
                  Repr repr, Var& dstVar, Instruction* beforeIns, ICode& code);
   Void genLocCode(ICode& code);
   virtual Void insertSuffix(ostream& os, Code& code) const;

protected:
   Operand* _tag;

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// Cons_S_A_Instruction : instruction to construct an atomic
//                        value of a sum type

class Cons_A_S_Instruction : public MoveInstruction {
public:
   Cons_A_S_Instruction(Operand& src, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Operand* genDstLocCode(Loc& loc, ICode& code);
 
protected:
   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// Decons_Instruction : instruction to deconstruct a value of a product type
//                      to its representing type

class DeconsInstruction : public MoveInstruction {
public:
   DeconsInstruction(Operand& src, Bool byPtr, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   Void assignDst(Operand& src, Repr srcRepr,
                  Repr repr, Var& dstVar,
                  Instruction* beforeIns, ICode& code);
   virtual Void genLocCode(ICode& code);

protected:
   Bool  _byPtr;

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// Decons_S_Instruction : instruction to deconstruct a value of a sum type
//                        into one of its representining types

class Decons_S_Instruction : public MoveInstruction {
public:
   Decons_S_Instruction(Operand& src, Tag tag, Bool byPtr, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Tag tag(Void) { return _tag; }

   virtual const char* mnemonic(Void) const;

   virtual Void insertSuffix(ostream& os, Code& code) const;

   virtual Operand* genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code);
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;

protected:
   Tag   _tag;
   Bool  _byPtr;

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// FromPlainInstruction : instruction to bind a plain value with an environment

class FromPlainInstruction : public MoveInstruction {
public:
   FromPlainInstruction(Operand& src, Operand& env, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline Operand* env(Void) const { return _env; }

   virtual const char* mnemonic(Void) const;

   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);

   virtual Void insertSuffix(ostream& os, Code& code) const;

protected:
   Operand* _env;

   friend class Code;
   friend class ICode;
};

class StructInstruction : public Instruction {
public:
   StructInstruction(StructOperand& srcComps, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   StructInstruction* getNest(Var& var) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& code);
   Void assignDst(Repr repr, Operand& dst, Instruction* beforeIns, ICode& code);

protected:
   StructOperand& _src;
   Operand*       _dst;

   friend class Code;
   friend class PtrInstruction;
};

// EnvOfInstruction : instruction to extract the environment of its operand

class EnvOfInstruction : public MoveInstruction {
public:
   EnvOfInstruction(Operand& src, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Operand* genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code);
   virtual Operand* genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code);

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// TagOfInstruction : instruction to extract the tag of its operand

class TagOfInstruction : public MoveInstruction {
public:
   TagOfInstruction(Operand& src, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Operand* genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code);
   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// CastTagInstruction : instruction to cast a tag to an atomic value

class CastTagInstruction : public MoveInstruction {
public:
   CastTagInstruction(Operand& src, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};



// ConstructFromNameInstruction : instruction to extract the tag of its operand

class ConstructFromNameInstruction : public MoveInstruction {
public:
   ConstructFromNameInstruction(Operand& src, TypeSig typeSig, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Bool needsSrcEnvLoc(Void) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;

   Void genByteCode(ostream& os, ByteCode& code) const;

   Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   TypeSig  _typeSig;

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};

// ConstructFromStringInstruction : instruction to extract the tag of its operand

class ConstructFromStringInstruction : public MoveInstruction {
public:
   ConstructFromStringInstruction(Operand& src, TypeSig typeSig, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;

   Void genByteCode(ostream& os, ByteCode& code) const;

   Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   TypeSig  _typeSig;

   friend class Code;
   friend class SourceCode;
   friend class BasicBlk;
};


// Ptr Array assign instruction

class PtrArrayAssignLocCodeGenState : public LocCodeGenState {
public:
   PtrArrayAssignLocCodeGenState(Operand& src, ICode& code);
protected:
   Operand& _src;
   Repr     _srcRepr;

   friend class PtrArrayAssignInstruction;
};

class PtrArrayAssignInstruction : public AssignInstruction {
public:
   PtrArrayAssignInstruction(Operand& src, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Void destroy(MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void setSrcReadyToKill(Instruction* beforeIns, LocCodeGenState& state, ICode& code);
   virtual Void genSrcLocCode(Instruction* beforeIns, LocCodeGenState& state, ICode& code);
   virtual Void setSrcNextUse(LocCodeGenState& state, ICode& code);
   virtual Void genLocCode(ICode& code);
   Void genByteCode(ostream& os, ByteCode& code) const;

protected:
   Operand* _src;
};

class EnterCellInstruction : public Instruction {
public:
   EnterCellInstruction(Operand& x, Bool altEntryFlag = FALSE,
                        Var* contVar = NULL

#if (CLOSURE_UPDATE==1)
                      , Var* updateCellVar = NULL
#endif

                        );

   EnterCellInstruction(Instruction::Kind kind,
                        Operand& x, Bool altEntryFlag = FALSE,
                        Var* contVar = NULL

#if (CLOSURE_UPDATE==1)
                      , Var * updateCellVar = NULL
#endif
                        );

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;

protected:
   Operand* _x;
   Bool     _altEntryFlag;
   Var*     _contVar;

#if (CLOSURE_UPDATE==1)
   Var* _updateCellVar;
#endif

   friend class Code;
   friend class SourceCode;
};

#if (GARBAGE_COLLECTION==1)
class MarkCellInstruction : public EnterCellInstruction {
public:
   MarkCellInstruction(VarOperand& x, Var* contVar = NULL);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;
};

#endif

// RSS_Instruction: Return structure size

class RSS_Instruction : public Instruction {
public:
   RSS_Instruction(UInt size);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;
 
protected:
   UInt  _size;
};

class ReturnInstruction : public Instruction {
public:
   enum Form {
      SIMPLE,
      GENERAL,
      EXIT
   };

public:
   ReturnInstruction(Form form = GENERAL,
                     Var* contVar = NULL
   
#if (CLOSURE_UPDATE==1)
                   , Var* updateCellVar = NULL
#endif   
   
                     );

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);

   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& Code) const;

protected:
   Form  _form;
   Var*  _contVar;

#if (CLOSURE_UPDATE==1)
   Var* _updateCellVar;
#endif   
};

class MonadicOpInstruction : public MonadicInstruction {
public:
   MonadicOpInstruction(Operator op, Operand& src, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Operator op(Void) const { return _op; }

   virtual const char* mnemonic(Void) const;
   virtual Bool usesRegisters(Void) const;
   virtual Bool needsSrcEnvLoc(Void) const;
   virtual Operand* genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code);
   virtual Operand* genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Operator  _op;
};

class UpdateInstruction : public MonadicInstruction {
public:
   UpdateInstruction(Operand& src, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
};

class MonadicOpActionInstruction : public Instruction {
public:
   MonadicOpActionInstruction(Operator op, Operand& x);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline Operator op(Void) const { return _op; }
   inline Operand* x(Void) const { return _x; }

   virtual const char* mnemonic(Void) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
 
protected:
   Operator  _op;
   Operand*  _x;
   Operand*  _x_env;

   friend class Code;
};

class DyadicOpInstruction : public Instruction {
public:
   DyadicOpInstruction(Operator op, Operand& x, Operand& y, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline Operator op(Void) const { return _op; }
   inline Operand* x(Void) const { return _x; }
   inline Operand* y(Void) const { return _y; }

   virtual const char* mnemonic(Void) const;
   virtual Void allocateData(Code& code) const;
   virtual Void destroy(MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void insertEnvMove(Operand& src, Operand& dst, Instruction* beforeIns,
                              ICode& code) const;
   Void assignLoc(Operand& x, Repr xRepr,
                  Operand& y, Repr yRepr,
                  Operand& dst, Repr repr, Loc::Kind kind, Var* dstVar,
                  Bool& needsEnv, Bool& suppressed,
                  Instruction* beforeIns,
                  ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
#if (GARBAGE_COLLECTION==1)
   virtual Bool locCodeNeedsStableStack(Void) const;
   virtual FrameDescrInstruction* frameDescrInstruction(Void) const;
   virtual Void genFrameDescrLocCode(FrameDescrInstruction& frameDescrIns, ICode& code);
#endif

   Operator _op;
   Operand* _x;
   Operand* _x_env;
   Operand* _y;
   Operand* _y_env;
   Operand* _dst;

   friend class Code;
};

class MapGCInstruction : public DyadicOpInstruction {
public:
   MapGCInstruction(Operator op, Operand& x, Operand& y, Operand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

protected:

#if (GARBAGE_COLLECTION==1)
   virtual Bool locCodeNeedsStableStack(Void) const;
   virtual FrameDescrInstruction* frameDescrInstruction(Void) const;
   virtual Void insertEnvMove(Operand& src, Operand& dst, Instruction* beforeIns,
                              ICode& code) const;
   virtual Void genFrameDescrLocCode(FrameDescrInstruction& frameDescrIns, ICode& code);
#endif

};

class DyadicOpActionInstruction : public Instruction {
public:
   DyadicOpActionInstruction(Operator op, Operand& x, Operand& y);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void allocateData(Code& code) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Operator _op;
   Operand* _x;
   Operand* _x_env;
   Operand* _y;
   Operand* _y_env;

   friend class Code;
};

class TriadicOpActionInstruction : public Instruction {
public:
   TriadicOpActionInstruction(Operator op, Operand& x, Operand& y, Operand& z);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void allocateData(Code& code) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Operator _op;
   Operand* _x;
   Operand* _y;
   Operand* _z;

   friend class Code;
};

class QuinadicOpActionInstruction : public Instruction {
public:
   QuinadicOpActionInstruction(Operator op, Operand& v, Operand& w, Operand& x, Operand& y, Operand& z);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void allocateData(Code& code) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;

protected:
   Operator _op;
   Operand* _v;
   Operand* _v_env;
   Operand* _w;
   Operand* _w_env;
   Operand* _x;
   Operand* _x_env;
   Operand* _y;
   Operand* _y_env;
   Operand* _z;
   Operand* _z_env;

   friend class Code;
};


class ConditionalInstruction : public Instruction {
public:
   ConditionalInstruction(Operator op, Operand& x, LabelOperand& z,
                          Bool divergent, MSA& msa);
   ConditionalInstruction(Operator op, Operand& x, Operand& y, LabelOperand& z,
                          Bool divergent, MSA& msa);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   Void setOpCodeStr(MSA& msa);

   virtual Void destroy(MSA& msa);
   virtual const char* mnemonic(Void) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void allocateData(Code& code) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
 
protected:
   Operator       _op;
   Operand*       _x;
   Operand*       _x_env;
   Operand*       _y;
   Operand*       _y_env;
   LabelOperand*  _z;
   Bool           _divergent;

   String         _opCodeStr;

   friend class Code;
   friend class SourceCode;
};

class CaseInstruction : public Instruction {
public:
   CaseInstruction(Operand& x, UInt n, ConstOperand** consts,
                   LabelOperand** labels, LabelOperand* otherwise);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void destroy(MSA& msa);
   virtual const char* mnemonic(Void) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void allocateData(Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Operand*          _x;
   Operand*          _x_env;
   UInt              _n;
   ConstOperand**    _consts;
   LabelOperand**    _labels;
   LabelOperand*     _otherwise;

   friend class Code;
   friend class SourceCode;
   friend class LabelInstruction;
};

class GoToInstruction : public Instruction {
public:
   GoToInstruction(Operand& target, Bool AltEntryFlag = FALSE,
                   Var* contVar = NULL

#if (CLOSURE_UPDATE==1)
                 , Var* updateCellVar = NULL
#endif

                   );

   virtual ~GoToInstruction(Void);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Operand& target(Void) const { return *_target; }

   virtual Void destroy(MSA& msa);
   const char* mnemonic(Void) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   Void genTargetLocCode(CodeLabel& codeLabel, ICode& code);
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Operand* _target;
   Bool     _altEntryFlag;
   Var*     _contVar;

#if (CLOSURE_UPDATE==1)
   Var*     _updateCellVar;
#endif

   friend class Code;
   friend class ICode;
   friend class SourceCode;
   friend class Var;
};

// EntryInstruction: T.B.D. - investigate the purpose and need for this

class EntryInstruction : public Instruction {
public:
   EntryInstruction(Name name, Type type, CodeLabel& codeLabel);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void allocateData(Code& code) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Name         _name;
   Type         _type;
   CodeLabel&   _codeLabel;

   friend class Code;
   friend class SourceCode;
};

// CellInfoInstruction: Cell information allocation 

class CellInfoInstruction : public Instruction {
public:
   CellInfoInstruction(CellInfo_& cellInfo, StructTemplate& structTemplate);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void destroy(MSA& msa);
   virtual const char* mnemonic(Void) const;
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   CellInfo_&        _cellInfo;
   StructTemplate&   _structTemplate;

   friend class Code;
   friend class SourceCode;
};

#if (GARBAGE_COLLECTION==1)

// FrameDescrInstruction: Provides information for an activation frame

class FrameDescrInstruction : public Instruction {
public:
   FrameDescrInstruction(LabelOperand& labOperand, 
                         GC_StubInstruction& gcStubInstr);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline LabelOperand& labOperand(Void) { return _labOperand; }
   inline GC_StubInstruction& gcStubInstr(Void) { return _gcStubInstr; }

#if (FRAME_DESCRIPTORS==1)
   inline FrameDescr<CodeLabel*>& frameDescr(Void) { return _frameDescr; }
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void destroy(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;

   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);

   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   LabelOperand&           _labOperand;
   GC_StubInstruction&     _gcStubInstr;

#if (FRAME_DESCRIPTORS==1)
   FrameDescr<CodeLabel*>  _frameDescr;
#endif

};
#endif

// HeapAllocInstruction: Abstract heap allocation

class HeapAllocInstruction : public Instruction {
public:
   HeapAllocInstruction(VarOperand& dst,
                        Operand* env = NULL);
   HeapAllocInstruction(Kind kind, size_t size,
                        VarOperand& dst,
                        Operand* env = NULL);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void destroy(MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   Loc& genDstLoc(ICode& code, Instruction* nextIns);
   virtual size_t size(Repr& repr, ICode& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   size_t      _size;
   Operand*    _env;       // NULL indicates default environment
   Operand*    _dst;
   Operand*    _dstEnv;

   friend class Code;
};

class CellAllocInstruction : public HeapAllocInstruction {
public:
   CellAllocInstruction(StructTemplate& structTemplate, Operand& cellInfo,
                        VarOperand& dst,
                        Operand* env = NULL);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual Void destroy(MSA& msa);
   virtual const char* mnemonic(Void) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Void setNextUse(MSA& msa);
   Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code) const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   StructTemplate&                _structTemplate;
   Lambda*                        _lambda;
   Operand*                       _cellInfo;

   friend class Code;
};

class PtrInstruction : public HeapAllocInstruction {
public:
   PtrInstruction(Kind kind, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
 
   Loc* allocDst(Loc::Kind kind, Operand& dst, Var& dstVar,
                 Instruction* beforeIns, ICode& code);
   virtual Void genLocCode(ICode& code);
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
 
protected:
   friend class Code;
};

class PtrArrayAllocInstruction : public PtrInstruction {
public:
   PtrArrayAllocInstruction(Operand& n, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void destroy(MSA& msa);
   virtual Bool usesVar(const Var& var) const;
   virtual Bool definesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;

   Loc* allocDst(Loc::Kind kind, Operand& n, Operand& dst, Var& dstVar,
                 Instruction* beforeIns, ICode& code);
   virtual Void genLocCode(ICode& code);

   virtual Void genByteCode(ostream& os, ByteCode& code) const;

protected:
   Operand* _n;

protected:
   friend class Code;
};

class IndirectionAllocInstruction : public HeapAllocInstruction {
public:
   IndirectionAllocInstruction(VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;

   virtual Void genLocCode(ICode& code);

   Void genByteCode(ostream& os, ByteCode& code) const;

   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   friend class Code;
};

// TagInstruction

class TagInstruction : public MoveInstruction {
public:
   TagInstruction(Operand& x, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Operand* genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code);
   virtual Operand* genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code);
   virtual Void allocateData(Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Repr     _srcRepr;
   friend class Code;
};

class ToCellInstruction : public MonadicInstruction {
public:
   ToCellInstruction(VarOperand& src, VarOperand& dst);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Loc& allocLoc(Repr repr, Var& dstVar, Loc::Kind kind,
                         Bool& reused, ICode& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   friend class Code;
};

class ExceptionInstruction : public Instruction {
public:
   ExceptionInstruction(Void);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   const char* mnemonic(Void) const;
//   Void insertSuffix(ostream& os, Code& code) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:

   friend class Code;
};

class ArgCheckInstruction : public Instruction {
public:
   ArgCheckInstruction(UInt n, LabelOperand** labels, CodeLabel& altEntryLab);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Bool usesVar(const Var& var) const;
   virtual Void setUD(BasicBlk& basicBlk, MSA& msa);
   virtual Void setSuccessor(BasicBlk& basicBlk, MSA& msa);
   virtual Void setNextUse(MSA& msa);
   virtual Void insertSuffix(ostream& os, Code& code) const;
   virtual Void allocateData(Code& code) const;
   virtual Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   UInt           _n;
   LabelOperand** _labels;
   CodeLabel&     _altEntryLab;
   UInt*          _partialDepths;

   friend class Code;
};

class EmptyArgsInstruction : public Instruction {
public:
   EmptyArgsInstruction(Bool needsArgBase);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   Void genLocCode(ICode& code);
   virtual Void genByteCode(ostream& os, ByteCode& code)  const;
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;

protected:
   Bool  _needsArgBase;
   Int   _diff;
};

class SqueezeInstruction : public Instruction {
public:
   SqueezeInstruction(UInt from, UInt to, UInt size);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   virtual const char* mnemonic(Void) const;
   virtual Void genByteCode(ostream& os, ByteCode& code)  const; 
   virtual Void genSourceCode(ostream& os, SourceCode& code) const;
 
protected:
   UInt  _from;
   UInt  _to;
   UInt  _size;
};

#if (GARBAGE_COLLECTION==1)

// Placeholder for garbage collection code

class GC_StubInstruction : public Instruction {
public:
   GC_StubInstruction(Void);

   inline Void* operator new(size_t size, MSA& msa) {
      Void* ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline Bool isLive(Void) const { return _isLive;  }
   inline Void kill(Void) { _isLive = FALSE; }

   virtual const char* mnemonic(Void) const;

protected:
   Bool  _isLive;
};

#endif

class CellInfo_ {
public:
   CellInfo_(CodeLabel& entryLabel, CodeLabel& altEntryLabel,
             CodeLabel* copyFnLabel,
            CodeLabel* extractBinFnLabel, CodeLabel* insertBinFnLabel

#if (GARBAGE_COLLECTION==1)
           , CodeLabel* gcFnLabel
#endif

            );

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline UInt seqNo(Void) const { return _seqNo; }
   inline UInt useCount(Void) const { return _useCount; }
   inline CodeLabel& entryLabel(Void) const { return _entryLabel; }
   inline CodeLabel& altEntryLabel(Void) const { return _altEntryLabel; }
   inline CodeLabel* copyFnLabel(Void) const { return _copyFnLabel; }
   inline CodeLabel* extractBinFnLabel(Void) const { return _extractBinFnLabel; }
   inline CodeLabel* insertBinFnLabel(Void) const { return _insertBinFnLabel; }

#if (GARBAGE_COLLECTION==1)
   inline CodeLabel* gcFnLabel(Void) const { return _gcFnLabel; }
#endif


   Void incRef(Void);
   Void decRef(Void);
   Void insert(ostream& os, Code& code) const;

protected:
   UInt           _seqNo;
   UInt           _useCount;
   CodeLabel&     _entryLabel;
   CodeLabel&     _altEntryLabel;
   CodeLabel*     _copyFnLabel;
   CodeLabel*     _extractBinFnLabel;
   CodeLabel*     _insertBinFnLabel;

#if (GARBAGE_COLLECTION==1)
   CodeLabel*     _gcFnLabel;
#endif

   UInt           _rackOffset;

   static UInt _count;

   friend class Code;
   friend class SourceCode;
   friend class Lambda;
   friend class CellInfoOperand;
   friend class CellInfoInstruction;
   friend class ClosureAllocInstruction;
};

class StructTemplateAllocator {
public:
   StructTemplateAllocator(UInt nSlots, Code& code)
		: _nSlots(nSlots), _code(code), _structTemplate(NULL), _match(TRUE) {}
   Void slot(UInt index, Type type_);
   StructTemplate& structTemplate(Void);

protected:
   UInt			      _nSlots;
   Code&				   _code;
   StructTemplate*   _structTemplate;
   Bool				   _match;

   friend class Lambda;
};

// StructTemplate: A template for structures

class StructTemplate {
public:
   StructTemplate(UInt nSlots, const StructTemplate* last, MSA& msa);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline StructTemplate* next(Void) { return _next; }
   inline UInt useCount(Void) { return _useCount; }
   inline UInt& id(Void) { return _id; }
   inline UInt nSlots(Void) { return _nSlots; }
   inline Type* slotTypes(Void) { return _slotTypes; }
   inline StructRepr& repr(Void) { return *_repr; }
   inline Void setRepr(StructRepr* repr) { _repr = repr; }

   Void adjUseCount(Int diff);

protected:
   StructTemplate*   _next;
   UInt              _useCount;
   UInt              _id;
   UInt              _nSlots;
   Type*             _slotTypes;
   StructRepr*       _repr;

   friend class Lambda;
   friend class Var;
   friend class VarOperand;
   friend class CellLocCompOperand;
   friend class StructTemplateAllocator;
   friend class Code;
   friend class ICode;
   friend class ByteCode;
   friend class SourceCode;
   friend class CompoundDataInstructionTrait;
   friend class LabelInstruction;
   friend class MoveInstruction;
   friend class HeapAllocInstruction;
   friend class CellInfoInstruction;
   friend class CellAllocInstruction;
};

// FreeVarAssoc: variable association
// (associates free variable binding with locally bound closed value)

class FreeVarAssoc {
public:
   FreeVarAssoc(FreeVarAssoc* next,
                FreeVarAssoc* parent,
                Name name,
                TypedVal* typedVal,
                const ModuleDefn* moduleDefn,
                Cell* closedVar,
                Bool isGlobal,
                Bool needsClosure,
                Bool selfReferential);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif
   inline FreeVarAssoc*& next(Void) { return _next; }
   inline const FreeVarAssoc* next(Void) const { return _next; }
   inline FreeVarAssoc*& parent(Void) { return _parent; }

   inline Name name(Void) const { return _name; }
   inline TypedVal*& typedVal(Void) { return _typedVal; }
   inline const TypedVal* typedVal(Void) const { return _typedVal; }
   inline const Expr val(Void) const { return _parent != NULL ? _parent->val() : _val; }
   inline Void setVal(Expr val) { _val = val; }
   inline const ModuleDefn*& moduleDefn(Void) { return _moduleDefn; }
   inline const ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }
   inline Cell*& closedVar(Void) { return _closedVar; }
   inline const Cell* closedVar(Void) const { return _closedVar; }
   inline Bool isGlobal(Void) const { return _isGlobal; }
   inline Bool needsClosure(Void) const { return _needsClosure; }
   inline Void setNeedsClosure(Bool q) { _needsClosure = q; }
   inline Bool selfReferential(Void) const { return _selfReferential; }

protected:
   FreeVarAssoc*     _next;            // Next in list
   FreeVarAssoc*     _parent;          // Parent lambda association
   Name              _name;            // Free bound value name
   TypedVal*         _typedVal;        // Free bound value
   Expr              _val;             // Effective bound value used by codegen
   const ModuleDefn* _moduleDefn;      // Free binding module definition or NULL
   Cell*             _closedVar;       // Locally bound closed variable
   Bool              _isGlobal;        // True if global, FALS otherwise
   Bool              _needsClosure;    // TRUE if occurrence requires closure value
   Bool              _selfReferential; // TRUE is self-referential, FALSE otherwise
};

// State for curried fu ction application

struct DirectMainEntryContext {
   DirectMainEntryContext(Lambda* lambda = NULL,
      CodeLabel* mainLabel = NULL,
      UInt nBaseArgs = 0)
      : _lambda(lambda),
      _mainLabel(mainLabel),
      _nBaseArgs(nBaseArgs) {}

   Lambda*     _lambda;
   CodeLabel*  _mainLabel;
   UInt        _nBaseArgs;
};

// Abstract code class

class Code {
public:
   Code(ConstString options, Env& env, MSA& msa);

   inline UInt nErrors(Void) const { return _nErrors; }
   inline Void adjErrorCount(Int diff) { _nErrors += diff; }
   inline const Env& env(Void) const { return _env; }
   inline Env& env(Void) { return _env; }
   inline NameTable& nameTable(Void) const { return _env.nameTable(); }
   inline TypeTable& typeTable(Void) const { return _env.typeTable(); }
   inline ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }
   inline MSA& msa(Void) const { return _msa; }
   inline Lambda* lambda(Void) const { return _lambda; }
   inline BasicBlk& basicBlk(Void) const { return *_basicBlk; }

#if (GARBAGE_COLLECTION==1)
   inline Bool gcFlag(Void) const { return _gcFlag; }
   inline Void setGCFlag(Bool q) { _gcFlag = q; }
#endif

   // error: Increment the error count and report an error

   Void error(ConstString s);

   virtual Void genTarget(ostream& os);

   Void init(Bool extendSuccessorsFlag);
   Void listStructTemplates(ostream& os);

   Bool typeGenPending(Type type, Bool genMethods) const;
   Type useType(TypeSig typeSig, Bool genMethods = FALSE);
   Void methodTypeError(ConstString s, TypeSig typeSig);
   Void genTypeMethods(CodeLabel& failLab);

   Void printType(Type type, ostream& os) const;
   virtual TypeDescr* allocTypeDescr(TypeSig typeSig, MSA& msa) const;

   Void reserveDeclOrDefn(Expr declOrDefn);
   Void reserveDeclOrDefns(Expr declOrDefns);
   Void reserveRecDeclOrDefn(Expr declOrDefn);
   Void reserveRecDeclOrDefns(Expr declOrDefns);

   Void genDeclOrDefn(Expr declOrDefn, CodeLabel& errLab);
	Void genDeclOrDefns(Expr declOrDefns, CodeLabel& errLab);

   Expr valOf(Expr expr, CodeLabel& failLab);
   Expr reduce(Expr expr, CodeLabel& failLab);

   Bool isConst(Expr expr, Expr& val) const;
   Bool isConstReduced(Expr expr, Expr& val) const;

   Void genExpr(Expr expr, Var& dst, CodeLabel& failLab);
   Operand& genOperand(Expr expr, TypeSig typeSig, CodeLabel& failLab);
   Operand& genOperandReduced(Expr expr, TypeSig typeSig, CodeLabel& failLab);
   Void genVar(Expr expr, Var& dst, CodeLabel& failLab);
   Void genVarReduced(Expr expr, Var& dst, CodeLabel& failLab);
   Void genCond(Expr expr, UInt reduceN, CodeLabel& labT, CodeLabel& labF,
                Bool divergent, CodeLabel& failLab);
   Void genCondReduced(Expr expr, UInt reduceN,
                       CodeLabel& labT, CodeLabel& labF,
                       Bool divergent, CodeLabel& failLab);
   Void genSelect(Expr expr, TypeSig typeSig, Bool byPtr, UInt index, Var& dst,
                  CodeLabel& failLab);
   Void genSelectReduced(Expr expr, TypeSig typeSig, Bool byPtr, UInt index,
                         Var& dst, CodeLabel& failLab);
   Void genVoid(Expr expr, CodeLabel& failLab);
   Void genVoidReduced(Expr expr, CodeLabel& failLab);
   Void genEnter(Expr expr, TypeSig typeSig, FnAp* fnAp, CodeLabel& failLab);
   Void genEnterReduced(Expr expr, TypeSig typeSig, FnAp* fnAp,
                        CodeLabel& failLab);
   Void genReturn(Expr expr, TypeSig typeSig, CodeLabel& failLab);
   Void genPairReturn(Expr pair, TypeSig typeSig, CodeLabel& failLab);
   Void genReturnReduced(Expr expr, TypeSig typeSig, CodeLabel& failLab);

   Void addInstruction(Instruction* ins);
   Void insertInstruction(Instruction* ins, Instruction* beforeIns);
   Void appendInstruction(Instruction* ins, Instruction* afterIns);
   Void removeInstruction(Instruction* ins, Bool destroy = TRUE);
   Void removeInstructions(Instruction* fromIns, Instruction* toIns);
   Void addGoToInstruction(Operand& lab, Bool altEntryFlag = FALSE,
                           Var* contVar = NULL

#if (CLOSURE_UPDATE==1)
                         , Var* updateCellVar = NULL
#endif
                           );

   Void addContinuationInstruction(LabelOperand& src,
                                   VarOperand& dst

#if (CLOSURE_UPDATE==1)
                                   , Bool forUpdate
#endif

#if (GARBAGE_COLLECTION==1)
                                   , Bool gcFlag
#endif

                                   );       
   Void addCellAllocInstruction(StructTemplate& structTemplate,
                                Operand& cellInfo,
                                VarOperand& dst,
                                Operand* dstEnv = NULL);

   Void addIndirectionAllocInstruction(VarOperand& dst);
 
#if (CLOSURE_UPDATE==1)
   Void addUpdateInstruction(Operand& src, Operand& dst);
#endif

   Void addUnaryInstruction(Instruction::Kind kind, Operand& x);
   Void addBinaryInstruction(Instruction::Kind kind,
                             Operand& x, Operand& y);
   Void addMoveInstruction(Operand& src, Operand& dst);
   Void addUnaryConditionalInstruction(Operator op, Operand& x,
                                       CodeLabel& lab, Bool divergent);
   Void addBinaryConditionalInstruction(Operator op, Operand& x, Operand& y,
                                        CodeLabel& lab, Bool divergent);
   Void addPtrArrayAssignInstruction(Operand& src, Operand& dst);
   Void genPredicateInstruction(Operator op, Operand& x,
                                CodeLabel& labT, CodeLabel& labF,
                                Bool divergent);
   Void genPredicateInstruction(Operator op, Operand& x, Operand& y,
                                CodeLabel& labT, CodeLabel& labF,
                                Bool divergent);
   Void genConditionalInstruction(Operator op, Operand& x,
                                  CodeLabel& labT, CodeLabel& labF,
                                  Bool divergent);
   Void genConditionalInstruction(Operator op, Operand& x, Operand& y,
                                  CodeLabel& labT, CodeLabel& labF,
                                  Bool divergent);
   Void genMove(Var& src, Var& dst);
	Void genMoveConst(Const& _const, Var& dst);
   ConstT<TypeConst>& makeTypeConst(Type type);
   Expr makeConst(Expr expr, Bool assignable = TRUE);
   CodeLabel& newLabel(Bool head = FALSE, Lambda* lambda = NULL, Bool withBasicBlock = TRUE);
   Bool hasUnboxedRepr(TypeSig typeSig);
   Var& newVar(TypeSig typeSig, Var::Kind kind);
   Expr newVarExpr(TypeSig typeSig, Var::Kind kind);
   Void reserve(Var& var);
   Void addVarElement(Var& var, VarElement::Flags flags);
   CellInfo_& newCellInfo(CodeLabel& entryLabel, CodeLabel& altEntryLabel,
                          CodeLabel* copyFnLabel,
                          CodeLabel* extractBinFnLabel, CodeLabel* insertBinFnLabel

#if (GARBAGE_COLLECTION==1)
                        , CodeLabel* gcFnLabel
#endif

                         );
   Void genLabel(CodeLabel& codeLabel);
   LabelOperand& labelOperand(CodeLabel& codeLabel) const;
   ConstOperand& constOperand(Const& const_) const;
   ConstOperand& constOperand(Tag tag) const;
   VarOperand& varOperand(Var& var, Bool reused = FALSE) const;
   StructOperand& structOperand(UInt nComps, Operand** comps, StructTemplate& structTemplate, TypeSig typeSig) const;
   SelectOperand& selectOperand(UInt index, Operand* operand,
                                TypeSig typeSig, Bool byPtr = FALSE) const;
   DePtrOperand& dePtrOperand(Operand& operand) const;
   ArrayIndexOperand& arrayIndexOperand(Operand& arrayOperand, Operand& indexOperand) const;
   ThisOperand& thisOperand(Void) const;
   CellInfoOperand& cellInfoOperand(CellInfo* cellInfo, CellInfo_* cellInfo_) const;

#if (GARBAGE_COLLECTION == 1)
   GC_StubInstruction& genGC_stub(Void);
   CodeLabel& genGC_MarkSnippet(Var& var);
#endif

   Void ret(Void);
   Operand& genArgOperand(Expr arg, TypeSig fnTypeSig, UInt i, CodeLabel& failLab);
   Expr returnState(TypeSig typeSig, Lambda* lambda);
   Void addCellTemplate(StructTemplate* structTemplate);
   StructTemplate& structTemplate(Type type);
   Void allocateDataAndLabels(Void);
   Bool flagIndirectContinuations(Void) const;
   Void headCodeLabels(Void) const;
   Void extendSuccessors(Void) const;
   UInt rearrangeBlocks(Void);
   Void removeBasicBlk(BasicBlk& basicBlk);
   Void moveCellInfoInstructions(Instruction* ins1,
                                 Instruction* ins2,
                                 Instruction* beforeIns);
   Bool unusedLocal(const VarOperand& dst, Instruction* ins, BasicBlk* basicBlk);
   UInt peepholeJump(Operand** operand, Bool unique, BasicBlk* prevCodeLabel);
   UInt peepholeMoveSrc(Operand*& src, Instruction*& ins, BasicBlk& basicBlk);
   Bool peepholeMoveSrcVar(Operand*& src, MoveInstruction*& ins, VarOperand& dst);
   Bool peepholeMoveSrcSelect(Operand*& src, MoveInstruction*& ins, VarOperand& dst);
   Bool peepholeMoveSrcDePtr(Operand*& src, MoveInstruction*& ins, VarOperand& dst);
   UInt peepholeMoveDst(Operand*& dst, Instruction*& ins, BasicBlk& basicBlk);
   Operand* peepholeMoveDstDePtrSelect(SelectOperand* selectOperand);
   Bool peepholeMoveDstDePtr(Operand*& dst, MoveInstruction*& ins, VarOperand& insDst);
   Bool peepholeMoveDstSelect(Operand*& dst, MoveInstruction*& ins, VarOperand& insDst);
   UInt peepholeMoveStructSrc(StructOperand& srcComps, Instruction*& ins, BasicBlk& basicBlk);
   UInt peepholeMovePred(Instruction*& ins, Instruction* nextIns, BasicBlk& basicBlk);
   Bool peepholeMoveSucc(Instruction*& ins, Instruction* nextIns,
                         Operand*& dst, BasicBlk* prevCodeLabel);
   UInt nestStruct(StructOperand& structOperand, StructInstruction* ins, VarOperand*& dst, BasicBlk* prevBasicBlk);
   UInt peepholeOptimise(UInt pass);
   Void setInOut(Void) const;
   Void printBlockInfo(Void);
   Void setKillPending(VarOperand& operand);
   virtual Void setNextUse(Var& var, Bool nextUse);
   virtual Void notLive(Var& var);
   Int nameConstIndex(Name name) const;
   UInt addNameConst(Name name);
   Int typeConstIndex(Type type) const;
   UInt addTypeConst(Type type);

protected:
   ConstString             _options;
   UInt                    _nErrors;
   UInt                    _nWarnings;
   Env&                    _env;
   MSA&                    _msa;

   ModuleDefn*             _moduleDefn;

public:
   Type                    _anyType;

protected:
   TypeElement*            _types;
   StructTemplate*         _structTemplates;
   Lambda*                 _lambda;
   ReturnState*            _returnState;     // Current return state
     
   BasicBlk*               _basicBlks;       // List of basic blocks
   BasicBlk*               _basicBlk;        // Current basic block

#if (CLOSURE_UPDATE==1)
   Var*                    _updateCellVar;   // Update cell variable
   Var*                    _updateContVar;   // Update conytinuation variable
#endif

   UInt                    _instrSeqNo;

   UInt                    _labelSeqNo;

   Bool                    _execSeqFlag;  

   vector<Name>            _nameConstV;
   UInt                    _nNameConsts;
   vector<Type>            _typeConstV;
   UInt                    _nTypeConsts;

   CodeLabel*              _errLab;

   InstrSeq*               _instrSeq;

   DirectMainEntryContext* _directMainEntryContext;

#if (GARBAGE_COLLECTION==1)
   Bool                    _gcFlag;             // TRUE if GC code generation, FALSE otherwise
#endif

   friend class ModuleDefn;
   friend class Order;
   friend class Defn;
   friend class Lambda;
   friend class Var;
   friend class FnAp;
   friend class CodeNode;
   friend class StackLoc;
   friend class StructTemplateAllocator;
   friend class GoToInstruction;
   friend class MonadicInstruction;
   friend class EnterCellInstruction;
   friend class ConditionalInstruction;
   friend class CaseInstruction;
   friend class LabelInstruction;
   friend class VarOperand;
   friend class ReturnState;
   friend class Cond;
   friend class Case;
   friend class FatBar;
   friend class NameOcc;
};

extern Const_consEnv    const_consEnv;    // Constructor environment
//extern Const_codeEnv    const_codeEnv;    // Current code environment
extern Const_NULL_Env   const_NULL_Env;   // Null environment

extern Void addVarLoc(Var& var, Loc& loc, MSA& msa);

extern Void remVarLoc(Var& var, Loc& loc, MSA& msa);

#endif /* IVORY_COMPILER_CODE_H_DEFINED */
