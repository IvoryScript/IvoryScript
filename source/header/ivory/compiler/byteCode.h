/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byteCode.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
*
*------------------------------------------------------------------------------
*
* Description:
*
*    Header file for I-machine byte code generation
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

#ifndef IVORY_COMPILER_BYTE_CODE_H
#define IVORY_COMPILER_BYTE_CODE_H

#include "ICode.h"

#define SEGMENT_HEADER_SECTION_ID               1
#define RACK_SECTION_ID                         (SEGMENT_HEADER_SECTION_ID+1)
#define NAME_TABLE_SECTION_ID                   (RACK_SECTION_ID+1)
#define TYPE_TABLE_SECTION_ID                   (NAME_TABLE_SECTION_ID+1)
#define CHILD_SEGMENT_TABLE_SECTION_ID          (TYPE_TABLE_SECTION_ID+1)
#define IMPORT_TABLE_SECTION_ID                 (CHILD_SEGMENT_TABLE_SECTION_ID+1)
#define DATA_SECTION_ID                         (IMPORT_TABLE_SECTION_ID+1)
#define INITIALISATION_CODE_SECTION_ID          (DATA_SECTION_ID+1)
#define MAIN_CODE_SECTION_ID                    (INITIALISATION_CODE_SECTION_ID+1)

#define HIGHEST_RESERVED_SECTION_ID             MAIN_CODE_SECTION_ID

// Import record

struct ImportRecord {
   UInt16   _reserved;    // Reserved (index into local name table for module name?)
   UInt16   _nameIndex;   // Index into local name table for imported name
   UInt32   _offset;      // Segment offset for imported value or reference
} ;

class ByteCodeSegment;

// ByteCodeSection: A subdivision of a byte code segment

class ByteCodeSection {
public:
   ByteCodeSection(UInt id
      
#if (EXTERNAL_RACK==0)      
      , Bool align = FALSE
#endif   
   
   );
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr)
   {
   }
#endif
   inline UInt id(Void) const { return _id; }

#if (EXTERNAL_RACK==0)
   inline Bool align(Void) const { return _align; }
#endif

   Byte getByte(UInt pos) const;
   Void putByte(Byte val, MSA& msa);
   Void putByteAt(Byte val, UInt pos, MSA& msa);
   UInt bytePos(Void) const {
      return _bytePos;
   }
   UInt origin(Void) const {
      return _origin;
   }
   const ByteCodeSegment* segment(Void) const {
      return _segment;
   }
   Void pad(UInt size, MSA& msa);
   Void reserve(UInt size, MSA& msa);

protected:
   UInt              _id;        // identifer

#if (EXTERNAL_RACK==0)
   Bool              _align;     // TRUE if aligned to word boundary, FALSE otherwise
#endif

   ByteCodeSection*  _next;      // next in chain
   ByteVector        _byteVec;   // local byte vector
   UInt              _bytePos;   // local byte position
   UInt              _origin;    // origin relative to segment
   IAddress          _bytePtr;   // absolute origin
   ByteCodeSegment*  _segment;   // Owning segment
  
   friend class ByteCodeSegment;
   friend class ByteCode;
};

// ByteCodeSegment: A segment of byte code - typically associated with a lambda function.

class ByteCodeSegment {
public:
   ByteCodeSegment(ByteCodeSegment* parent, ByteCode& code);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr)
   {
   }
#endif
   const ByteCodeSegment* next(void) const { return _next; }
   UInt index(void) const { return _index; }
   const ByteCodeSegment* firstChild(void) const { return _firstChild; }
   const ByteCodeSection* sections(void) const { return _sections; }
   const ByteCodeSection* lastSection(void) const { return _lastSection; }

   ByteCodeSection* getSection(UInt id) const;
   Void addChild(ByteCodeSegment* segment);
   UInt childCount(Void) const;
   Void appendSection(ByteCodeSection* section, ByteCodeSegment* segment);
   Int nameConstIndex(Name name) const;
   UInt addNameConst(Name name, MSA& msa);
   Int typeConstIndex(Type type) const;
   UInt addTypeConst(Type type, MSA& msa);
   const struct ImportRecord* importRec(UInt16 nameIndex) const;
   Void addImportRec(UInt16 nameIndex, UInt offset, MSA& msa);
   Void putNameConsts(ByteCode& code);
   Void putTypeConsts(ByteCode& code);
   Void putImports(ByteCode& code);
protected:
   ByteCodeSegment*  _next;         // Next sibling segment
   ByteCodeSegment*  _parent;       // Parent segment
   UInt              _index;
   ByteCodeSegment*  _firstChild;   // First child segment   
//   UInt              _sectionId;    // Id of first section
   ByteCodeSection*  _sections;     // Head of section list
   ByteCodeSection*  _lastSection;  // Tail of section list
   UInt              _nNameConsts;
   vector<Name>      _nameConstV;   // Name id constants  
   UInt              _nTypeConsts;
   vector<Type>      _typeConstV;   // Type id constants
   UInt              _nImportRecs;
   vector<struct ImportRecord>
                     _importRecV;
//   UInt              _rackPos;      // Rack position

   friend class ByteCode;
};

// BackPatchElement: Abstract element of a back-patch list

class BackPatchElement {
public:
   BackPatchElement(UInt pos,        ByteCodeSection& posSection,
                    UInt origin = 0, ByteCodeSection* originSection = NULL);
   UInt origin(Void) const;
   virtual UInt target(Void) const = 0;
   virtual Bool branchCrossesSegments(Void) const;

protected:
   BackPatchElement* _next;
   UInt              _pos;             // Position to backpatch
   ByteCodeSection&  _posSection;      // Position section
   UInt              _origin;          // Origin
   ByteCodeSection*  _originSection;   // Origin section

   friend class ByteCode;
   friend class Instruction;
};

// SimpleBackPatchElement: Simple back-patch element where the target address is known

class SimpleBackPatchElement : public BackPatchElement {
public:
   SimpleBackPatchElement(UInt pos,        ByteCodeSection& posSection,
                          UInt origin = 0, ByteCodeSection* originSection = NULL,
                          UInt target = 0, ByteCodeSection* targetSection = NULL);
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
   virtual UInt target(Void) const;

protected:
   UInt              _binding;          // Target address
   ByteCodeSection*  _bindingSection;   // Target address section
};

// LabelBackPatchElement: Code label back-patch element

class LabelBackPatchElement : public BackPatchElement {
public:
   LabelBackPatchElement(UInt pos, ByteCodeSection& posSection,
                          UInt origin, ByteCodeSection& originSection, 
                          CodeLabel& codeLabel, Bool branchFlag = FALSE);
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa){
      msa.free(ptr);
   }
#else
   inline Void operator delete(Void* ptr) {
   }
#endif
   virtual UInt target(Void) const;
   virtual Bool branchCrossesSegments(Void) const;

protected:
   CodeLabel&  _codeLabel;
   Bool        _branchFlag;
};

// SegmentBackPatchElement: Segment address back-patch element

class SegmentBackPatchElement : public BackPatchElement {
public:
   SegmentBackPatchElement(UInt pos, ByteCodeSection& posSection, Lambda& lambda);
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
   virtual UInt target(Void) const;

protected:
   Lambda& _lambda;
};

// ByteCode: Methods and state for byte code generation

class ByteCode : public ICode {
public:
   ByteCode(ConstString options, Env& env, MSA& msa);

   virtual TypeDescr* allocTypeDescr(TypeSig typeSig, MSA& msa) const;

   virtual Void genTarget(ostream& os);

   virtual Bool insertJumpsToHeadLabels(Void) const;

   Lambda* segmentLambda(Lambda* lambda);
   ByteCodeSegment* getSegment(Lambda* lambda, UInt insSeqNo);

   Void appendSection(ByteCodeSection* section);
   ByteCodeSection* addSection(
      
#if (EXTERNAL_RACK==0)
      Bool align = FALSE
#else
      Void
#endif

   );

   Void terminateSegments(ByteCodeSegment* segment, ByteCodeSegment* parent);
   UInt setSectionOrigins(Void) const;
   Void setSegmentLengths(ByteCodeSegment* segment);
   Void copySections(Byte* bytes) const;
   Void segmentInfo(const ByteCodeSegment* segments, Byte* base, UInt level, ostream& os) const;
   Void sectionInfo(const ByteCodeSection* sections, const ByteCodeSection* lastSection,
                    Byte* base, UInt level, ostream& os) const;

   Void genJump(Operand& target);
   Void enterCell(LocOperand& target);
   Void genGoTo(Operand& target);
   Void declareStructures(ByteVector& byteVec, unsigned int& pos) ;
   Void declareLabels(ByteVector& byteVec, unsigned int& pos);

   virtual Bool suppressEnvMove(Void) const;
   virtual Loc::Kind locKind(Var& var, Repr repr);
   Void genOpCode(UInt opCode, ostream& os);
   Void genOperand(Operand& operand, Bool sepFlag, ostream& os);
   Void genConstData(Operand& operand, Bool sepFlag, ostream& os);

   Void putByte(Byte val);
   Void putByteAt(Byte val, UInt pos);
   Void putBits16(Bits16 val);
   Void putBits32(Bits32 val);
   Void putBits64(Bits64 val);
   Void putBits32At(Bits32 val, UInt pos);
   Void putBytes(Byte* buffer, UInt n);
   Void putVLU(unsigned long int val);
   Void putVLI(long int val);
   Void putImmUInt(UInt val);
   Void putImmInt(Int val);
   Void putRegAddrMode(UInt regId);
   Void putStackAddrMode(Int64 offset);
   Void putFrameAddrMode(UInt64 offset);
   Void putCellAddrMode(UInt64 offset);
   Void putRackAddrMode(UInt64 offset);
   Void putIndAddrMode(Int64 offset);
   Void putIndexedAddrMode(UInt64 offset);

   Void putCodeEnv(Void);
   Void putTypeConst(Type type, ostream& os);

   virtual Repr labelRepr(Bool simple/* = FALSE*/) const;
   ByteCodeSection* section(UInt id);
   ByteCodeSection* selectSection(UInt id);
   ByteCodeSection* selectHeaderSection(Void);
   ByteCodeSection* selectDataSection(Void);
   ByteCodeSection* selectInitCodeSection(Void);
   UInt bytePos(Void) const;
   Void reserveBytes(UInt size, UInt sectionId);
   Void reserveBytes(UInt size);
   Void padRack(UInt size);
   UInt rackBytePos(Void) const;
   Void reserveRack(UInt size);
   Void padData(UInt size);
   Void reserveData(UInt size);
   Void addBackPatch(BackPatchElement* element);
   Void addBackPatch(UInt pos,    ByteCodeSection& posSection,
                     UInt origin, ByteCodeSection* originSection,
                     UInt target, ByteCodeSection* targetSection,
                     Bool endOfInsFlag = TRUE);
   Void addLabelBackPatch(UInt pos, CodeLabel& codeLabel,
                          Bool endOfInsFlag = TRUE);
   Void addLabelBackPatch(UInt dataPos, ByteCodeSection& originSection,
                          CodeLabel& codeLabel);
   Void addTargetBackPatch(UInt pos,
                         UInt target, UInt targetSectionId = DATA_SECTION_ID,
                         Bool endOfInsFlag = TRUE);
   Void addSegmentBackPatch(UInt pos, Lambda& lambda,
                             Bool endOfInsFlag = TRUE);
   inline Bool branchBackPatchFlag(Void) const {
      return _branchBackPatchFlag;
   }
   inline Void setBranchBackPatchFlag(Bool flag) {
      _branchBackPatchFlag = flag;
   }
   inline ByteCodeSegment* currSegment(Void) const {
      return _currSegment;
   }
   inline ByteCodeSection* currSection(Void) const {
      return _currSection;
   }

   inline Void selectSection(ByteCodeSection* section) {
      _currSection = section;
   }
   Void saveEntryPoint(Void);
   IAddress codeSegment(Void) const;
   UInt rackOrigin(Void) const;
   IAddress mainEntry(Void) const;

protected:
   ByteCodeSegment*  _rootSegment;
   ByteCodeSegment*  _currSegment;
   UInt              _sectionId;
   ByteCodeSection*  _sections;
   ByteCodeSection*  _lastSection;
   ByteCodeSection*  _currSection;
   ByteCodeSection*  _codeSection;
   ByteCodeSection*  _entrySection;
   UInt              _entryPos;
   Bool              _branchBackPatchFlag;

   BackPatchElement* _backPatchList;
 
   friend class Var;
   friend class GoToInstruction;
   friend class ArgCheckInstruction;
   friend class EnterInstruction;
   friend class ConditionalInstruction;
   friend class CaseInstruction;
   friend class LabelInstruction;
   friend class MoveInstruction;
   friend class DyadicOpInstruction;
   friend class HeapAllocInstruction;
   friend class CellInfoInstruction;
   friend class SqueezeInstruction;

   friend class BasicBlk;
   friend class BackPatchElement;
   friend class LabelBackPatchElement;
   friend class DataBackPatchElement;

   friend class ByteCodeSegment;
};

#endif /* IVORY_COMPILER_BYTE_CODE_H_DEFINED */
