/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    sourceCode.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
*
*------------------------------------------------------------------------------
*
* Description:
*
*    Header file for I-machine source code generation
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

#ifndef IVORY_COMPILER_SOURCE_CODE_H
#define IVORY_COMPILER_SOURCE_CODE_H

#include "ICode.h"

class SourceCodeLabelElement {
public:
   SourceCodeLabelElement(SourceCodeLabelElement* next,
                          CodeLabel& codeLabel)
      : _next(next), _codeLabel(codeLabel) {
   }
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
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

protected:
   SourceCodeLabelElement* _next;
   CodeLabel&              _codeLabel;

   friend class SourceCodeLabelSet;
};

class SourceCodeLabelSet {
public:
   SourceCodeLabelSet(Void)
      : _elements(NULL) {
   }

   SourceCodeLabelElement* addElement(CodeLabel& codeLabel, MSA& msa);
   SourceCodeLabelElement* findElement(const CodeLabel& codeLabel) const;
   Void removeElement(const CodeLabel& codeLabel, MSA& msa);
   Void merge(const SourceCodeLabelSet& sourceCodeLabelSet, MSA& msa);
   Void empty(MSA& msa);
   inline SourceCodeLabelElement* head(Void) const {
      return _elements;
   }
   inline Bool isEmpty(Void) const {
      return _elements == NULL;
   }
   inline Void setEmpty(Void) {
      _elements = NULL;
   }

protected:
   SourceCodeLabelElement* _elements;;

   friend class SourceCode;
};

class SourceCodeBlock {
public:
   SourceCodeBlock(SourceCodeBlock* outer)
      : _outer(outer)
   {
   }
   inline Void* operator new(size_t size, MSA& msa)
   {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
   {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr)
   {
   }
#endif

protected:
   SourceCodeBlock*     _outer;
   SourceCodeLabelSet   _labels;

   friend class SourceCode;
};

class NativeLocal : public Loc {
public:
   NativeLocal(Repr repr, NativeLocal* next);
   NativeLocal(const NativeLocal& src);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
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

   virtual Loc* clone(Loc* parent, Bool withVars, MSA& msa) const;

   virtual Void genSourceCode(Int offset, ostream& os, SourceCode& code);
   virtual Void genDstSourceCode(Int offset, ostream& os, SourceCode& code);

   virtual Void print(ostream& os, const Env& env, Bool isDep = FALSE) const;

protected:
   UInt  _n;

   static Void insert(NativeLocal& loc, NativeLocal** nativeLocalPtr);

   static UInt _count;

   friend class Var;
   friend class SourceCode;
};

class SourceCodeBlock;

class SourceCode : public ICode {
public:
   SourceCode(ConstString options, Env& env, MSA& msa);
   virtual Bool builtInNameComp(Expr x, Expr y) const;
   virtual Bool builtInTypeComp(Expr x, Expr y) const;

   Void genTarget(ostream& os);

   virtual Bool insertJumpsToHeadLabels(Void) const;

   Bool isBlockLabel(const CodeLabel& codeLabel) const;
   Bool isOuterLabel(const CodeLabel& codeLabel,
                     const SourceCodeBlock& sourceCodeBlock) const;
   Bool anyForwardLabels(Void) const;
   Void removeBlockLabel(CodeLabel& codeLabel);

   virtual Repr labelRepr(Bool simple/* = FALSE*/) const;

   Void genJump(Operand& target, ostream& os);
   Void genGoTo(Operand& target, ostream& os);

   virtual Void printPtrRepr(const PtrRepr& ptrRepr, ostream& os) const;
   virtual Void printStructRepr(const StructRepr& structRepr, Bool full, ostream& os) const;
   virtual Void printUnionRepr(const UnionRepr& unionRepr, ostream& os) const;
   virtual Void printRepr(Repr repr, ostream& os) const;

   Void declareStructure(StructRepr& structRepr, UInt& structN, ostream& os);
   Void declareStructures(ostream& os) ;
   Void declareLabels(ostream& os); 
   Void genLabel(const CodeLabel& codeLabel, ostream& os,
                 Bool modulePrefixFlag = TRUE,
                 Bool parenFlag = TRUE,
                 Bool altFlag = FALSE) const;



   Void createBlock(ostream& os);
   Void destroyBlock(ostream& os);
   Void insertLoc(Loc& loc);
   NativeLocal& allocNativeLocal(Repr repr);
   Void flushNativeLocals(Void);
   virtual Void flushNonStack(Void);
   Void endBody(ostream& os);
   Bool localLabel(const CodeLabel& codeLabel) const;
   Void adjustStack(Int diff, Loc::Kind kind, ostream & os) const;

   virtual Loc::Kind locKind(Var& var, Repr repr);
   virtual Loc::Kind temporaryKind(Loc::Kind kind) const;
   virtual Loc* allocLoc(Repr repr, Loc::Kind kind, Bool argFlag, Bool reclaimFlag = FALSE);

protected:
   NativeLocal*         _nativeLocals;
   Bool                 _inBody;
   SourceCodeLabelSet   _labelSet;
   SourceCodeBlock*     _sourceCodeBlock;

   friend class Var;
   friend class GoToInstruction;
   friend class EnterInstruction;
   friend class ConditionalInstruction;
   friend class CaseInstruction;
   friend class LabelInstruction;
   friend class MoveInstruction;
   friend class DyadicOpInstruction;
   friend class HeapAllocInstruction;
   friend class LocOperand;
};

#endif /* IVORY_COMPILER_SOURCE_CODE_H_DEFINED */
