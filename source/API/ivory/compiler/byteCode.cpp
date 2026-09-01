/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    byteCode.cpp
 *
 * Module:  IvoryScript compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    IvoryScript byte code generation
 *		Generates I-machine byte code from the intermediate code.
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

#include <cstdint>
#include "OptionsParser.h"
#include "ivory/builtIn.h"
#include "ivory/builtInDataCon.h"
#include "ivory/exec.h"

#if (FRAME_DESCRIPTORS==1)
#include "ivory/frameDescr.h"
#endif

#include "ivory/ref.h"
#include "ivory/segment.h"
#include "ivory/compiler/byteCode.h"

#define TRACE

declareType(Env);

#ifdef TRACE
static Bool traceFlag = FALSE;
#define toggle(flag) ((flag)=!(flag))
static Int instructionProbeSeqNo = -1;
#endif

ByteCodeSection::ByteCodeSection(UInt id
   
#if (EXTERNAL_RACK==0)   
   , Bool align/* = FALSE*/
#endif
   )
 : _id (id),
   
#if (EXTERNAL_RACK==0)   
   _align(align),
#endif

    _next(NULL), _byteVec(256), _bytePos(0),
   _origin(0), _bytePtr(0), _segment(NULL) {
}

Byte ByteCodeSection::getByte(UInt pos) const {
   return _byteVec.get(pos);
}

Void ByteCodeSection::putByte(Byte val, MSA& msa) {
   _byteVec.put(_bytePos++, val, msa);
}

Void ByteCodeSection::putByteAt(Byte val, UInt pos, MSA& msa) {
   _byteVec.put(pos, val, msa);
}

// Pad to appropriate boundary for size

Void ByteCodeSection::pad(UInt size, MSA& msa) {
   if (size == 2 && _bytePos % 2 == 1 ||
      size > 2 && _bytePos % 4 != 0) {
      UInt n = size == 2 ? 1 : 4 - _bytePos % 4;
      for (UInt i = 0; i < n; i++)
         putByte(0, msa);
   }
}

Void ByteCodeSection::reserve(UInt size, MSA& msa) {
   for (UInt i = 0; i < size; i++)
      putByte(0, msa);
}

ByteCodeSegment::ByteCodeSegment(ByteCodeSegment* parent, ByteCode& code)
   : _next(NULL), _parent(parent), _index(0), _firstChild(NULL),
     _sections(NULL), _lastSection(NULL),
     _nNameConsts(0), _nameConstV(16),
     _nTypeConsts(0), _typeConstV(16),
     _nImportRecs(0), _importRecV(16) {
   if (parent != NULL)
      parent->addChild(this);
   for (UInt i = 0; i < HIGHEST_RESERVED_SECTION_ID; i++)
      appendSection(code.addSection(i == 0), this);
}

Void ByteCodeSegment::addChild(ByteCodeSegment* segment) {
   UInt index = 0;
   // Append to list

   ByteCodeSegment** segmentPtr = &_firstChild;
   while (*segmentPtr != NULL) {
      segmentPtr = &(*segmentPtr)->_next;
      index++;
   }
   segment->_next = NULL;
   segment->_index = index;
   *segmentPtr = segment;
}

UInt ByteCodeSegment::childCount(Void) const {
   UInt res = 0;
   for (ByteCodeSegment* segment = _firstChild; segment != NULL; segment = segment->_next)
      res++;
   return res;
}

Void ByteCodeSegment::appendSection(ByteCodeSection* section,
                                     ByteCodeSegment* segment) {
   section->_segment = segment;
   if (_sections == NULL)
      _sections = section;
   for (ByteCodeSegment* seg = this; seg != NULL;
      seg = seg->_parent) {
      seg->_lastSection = section;
   }
}

ByteCodeSection* ByteCodeSegment::getSection(UInt id) const {
   for (ByteCodeSection* section = _sections; section != NULL; section = section->_next)
      if (section->_id == id + (_sections->_id - 1))
         return section;
   return NULL;
}

// Linear seach will suffice for the following functions for now

Int ByteCodeSegment::nameConstIndex(Name name) const {
   for (UInt i = 0; i < _nNameConsts; i++)
      if (_nameConstV.get(i) == name)
         return i;
   return -1;
}

UInt ByteCodeSegment::addNameConst(Name name, MSA& msa) {
   Int res;
   if ((res = nameConstIndex(name)) >= 0)
      return (UInt)res;
   _nameConstV.put(_nNameConsts, name, msa);
   return _nNameConsts++;
}

Int ByteCodeSegment::typeConstIndex(Type type) const {
   for (UInt i = 0; i < _nTypeConsts; i++)
      if (_typeConstV.get(i) == type)
         return i;
   return -1;
}

UInt ByteCodeSegment::addTypeConst(Type type, MSA& msa) {
   Int res;
   if ((res = typeConstIndex(type)) >= 0)
      return (UInt)res;
   _typeConstV.put(_nTypeConsts, type, msa);
   return _nTypeConsts++;
}

const struct ImportRecord* ByteCodeSegment::importRec(UInt16 nameIndex) const {
   for (UInt i = 0; i < _nImportRecs; i++) {
      const struct ImportRecord& impRec = _importRecV.get(i);
      if (impRec._nameIndex == nameIndex)
         return &impRec;
   }
   return NULL;
}

Void ByteCodeSegment::addImportRec(UInt16 nameIndex, UInt offset, MSA& msa) {
   if (importRec(nameIndex) == NULL) {
      struct ImportRecord impRec = { 0, nameIndex, offset };
      _importRecV.put(_nImportRecs, impRec, msa);
      _nImportRecs++;
   }
}

Void ByteCodeSegment::putNameConsts(ByteCode& code) {
   code.selectSection(NAME_TABLE_SECTION_ID);
   code.putBits16(_nNameConsts);
   for (UInt i = 0; i < _nNameConsts; i++)
      code.putBits16(_nameConstV.get(i));
}

Void ByteCodeSegment::putTypeConsts(ByteCode& code) {
   code.selectSection(TYPE_TABLE_SECTION_ID);
   code.putBits16(_nTypeConsts);
   for (UInt i = 0; i < _nTypeConsts; i++)
      code.putBits16(_typeConstV.get(i));
}

Void ByteCodeSegment::putImports(ByteCode& code) {
   code.selectSection(IMPORT_TABLE_SECTION_ID);
   code.putBits16(_nImportRecs);
   for (UInt i = 0; i < _nImportRecs; i++) {
      struct ImportRecord& importRec = _importRecV.get(i);
      code.putBits16(0);                     // Reserved
      code.putBits16(importRec._nameIndex);  // Imported name
      code.putBits32(importRec._offset);     // Offset
   }
}

Void ByteCode::appendSection(ByteCodeSection* section) {
   if (_sections == NULL)
      _sections = section;
   else
      _lastSection->_next = section;
   _lastSection = section;
}

ByteCodeSection* ByteCode::addSection(Bool align/* = FALSE */) {
   ByteCodeSection* section = new(msa()) ByteCodeSection(++_sectionId, align);
   appendSection(section);
   _currSection = section;
   return section;
}

// ByteCode::segmentLambda: Return the segment for a given lambda function

Lambda* ByteCode::segmentLambda(Lambda* lambda) {
   Lambda* segLambda = lambda;

   while (segLambda != NULL && segLambda->refCount() == 0)
      segLambda = segLambda->_parent;

   return segLambda;
}

ByteCodeSegment* ByteCode::getSegment(Lambda* lambda, UInt insSeqNo) {
   Lambda* segLambda = segmentLambda(lambda);

   ByteCodeSegment* segment = segLambda == NULL ? _rootSegment
                                                : segLambda->_segment;
   if (segment == NULL) {
      ByteCodeSegment* parentSegment = NULL;
      if (segLambda != NULL) {
         Lambda* parSegLambda = segmentLambda(segLambda->parent());
         parentSegment = parSegLambda == NULL
            ? _rootSegment
            : parSegLambda->_segment;
      }


      segment = new(msa()) ByteCodeSegment(parentSegment, *this);
 
      _currSegment = segment;

//--- Reserve space for the segment header:
//---    length, rack length, hash, offset to initialisation code and

      reserveBytes(4 * SEGMENT_WORD_SIZE, SEGMENT_HEADER_SECTION_ID);

//--- Reserve a slot for the code environment
// 
      selectSection(RACK_SECTION_ID);
      putBits32(0);

//--- Initialise the child segment table

      selectSection(CHILD_SEGMENT_TABLE_SECTION_ID);
      putBits32(0);

      selectSection(DATA_SECTION_ID);
      putBits32((Bits32)static_cast<OID>(NULL_REF).repr());

      _currSegment = parentSegment;

      if (parentSegment == NULL)
         _rootSegment = segment;
      else {

//------ Update the segment count and reserve space for the segment offset

         selectSection(CHILD_SEGMENT_TABLE_SECTION_ID);
         putBits32At(parentSegment->childCount(), 0);
         reserveBytes(sizeof(UInt32));
      }
   }
   if (lambda != NULL && lambda->_segment == NULL)
      lambda->_segment = segment;
   return segment;
}

ByteCode::ByteCode(ConstString options, Env& env, MSA& msa)
   : ICode(options, env, msa),
    _rootSegment(NULL), _currSegment(NULL),
    _sectionId(0), _sections(NULL), _lastSection(NULL), _currSection(NULL),
    _codeSection(NULL), _branchBackPatchFlag(FALSE),
    _backPatchList(NULL) {
}

TypeDescr* ByteCode::allocTypeDescr(TypeSig typeSig, MSA& msa) const {
   return new(msa) ITypeDescr(typeSig);
}

Void ByteCode::genTarget(std::ostream& os)  {

#ifdef TRACE
   OptionsParser options(_options);
   if (options.hasKey("bytes")) {
      traceFlag = options.hasKey("trace");
      options.getNum("probe", instructionProbeSeqNo);
   } else
      traceFlag = FALSE;
#endif

   if (_moduleDefn != NULL)
      addNameConst(_moduleDefn->name());

   init(FALSE);
/*
   do {
      headBasicBlocks();
   } while (flagIndirectContinuations());
*/

   genLocCode();

   Instruction* ins = _instrSeq->first();

   while (ins != NULL) {
      if (ins->_kind != ins->RESERVE_INSTR

#if (GARBAGE_COLLECTION==1)
         && ins->_kind != ins->GC_STUB_INSTR
#endif

         ) {

#ifdef TRACE
         if (traceFlag) {
            outStream << "Generating byte code for instruction " << ins->_seqNo
                      << " segment " << (Void*)_currSegment
                      << " section ";
            if (_currSection != NULL)
               outStream << _currSection->id()
                         << " pos " << _currSection->bytePos();
            else
               outStream << "NULL pos NULL";
            outStream
                      << '\n';
            outStream.flush();
         }
         if (ins->_seqNo == instructionProbeSeqNo) {
            outStream << "ByteCode::genTarget: probe hit" << endl;
            traceFlag = TRUE;
         }
#endif

         ByteCodeSegment* segment = getSegment(ins->_lambda, ins->_seqNo);
         if (segment != _currSegment) {
            _currSegment = segment;
            _currSegment->appendSection(addSection(), _currSegment);
         }

         _currentIns = ins;
         if (ins->_kind != ins->CELL_INFO_DATA_INSTR)
            os << ins->_seqNo << ": ";
         ins->genByteCode(os, *this);
         if (ins->_kind != ins->CELL_INFO_DATA_INSTR)
            os << endl;

#ifdef TRACE
         if (traceFlag) {
            outStream << "Generated byte code for instruction " << ins->_seqNo
                      << " segment " << (Void*)_currSegment
                      << " section ";
            if (_currSection != NULL)
               outStream << _currSection->id()
                         << " pos " << _currSection->bytePos();
            else
               outStream << "NULL pos NULL";
            outStream
                      << '\n';
            outStream.flush();
         }
#endif

         BackPatchElement* element;
         while ((element = ins->_backPatchList) != NULL) {

//--------- Set origin for any targets relative to end of instruction

            ins->_backPatchList = element->_next;
            element->_origin =
               element->_originSection != NULL ? element->_originSection->_bytePos
               : _currSection->_bytePos;
            addBackPatch(element);
         }
      }

      ins = ins->_next;
   }

// Terminate initialisation sections and name & type constant tables

   terminateSegments(_rootSegment, NULL);

// Set section origins

   UInt size = setSectionOrigins();

// Set segment lengths

   setSegmentLengths(_rootSegment);

// Apply back-patches

   BackPatchElement* element;
   while ((element = _backPatchList) != NULL) {
      _backPatchList = element->_next;
      if (element->branchCrossesSegments())
         ivoryError("ByteCode::genTarget: BRA_INS crosses byte code segments");
      selectSection(&element->_posSection);
      putBits32At((Int)element->target() - (Int)element->origin(), element->_pos);
      msa().free(element);
   }

   Byte* bytes = (Byte*)msa().alloc(size);
   copySections(bytes);

#ifdef TRACE
   if (traceFlag) {
      os << "Total bytes = " << size << '\n';
      os.flush();
      segmentInfo(_rootSegment, bytes, 0, os);
   }
#endif

}

Bool ByteCode::insertJumpsToHeadLabels(Void) const {
   return FALSE;
}

Void ByteCode::terminateSegments(ByteCodeSegment* segment, ByteCodeSegment* parent) {
   while (segment != NULL) {
      terminateSegments(segment->_firstChild, segment);

      _currSegment = segment;

//--- Dump name and type tables

      segment->putNameConsts(*this);
      segment->putTypeConsts(*this);
      segment->putImports(*this);

      if (parent != NULL) {

//------ Insert a 'pcb' instruction into parent initialisation code

         _currSegment = parent;
         selectInitCodeSection();

         putByte(PCB_INS);
         putBits32(0);
         addBackPatch(bytePos() - sizeof(Bits32), *_currSection,
                      bytePos(), _currSection,
                      0, segment->getSection(INITIALISATION_CODE_SECTION_ID),
                      FALSE);
         _currSegment = segment;
      }
      selectInitCodeSection();

//--- And return

      putByte(RTN_INS);
      segment = segment->_next;
   }
}

UInt ByteCode::setSectionOrigins(Void) const {
   UInt origin = 0;
   ByteCodeSection* section = _sections;

   while (section != NULL) {

#if (EXTERNAL_RACK==0)
      if (section->align() &&
         origin % 4 != 0)

//------ Pad to 4 byte boundary

         origin += 4 - origin % 4;
#endif
  
      section->_origin = origin;
      origin += section->_bytePos;
      section = section->_next;
   }
   return origin;
}

Void ByteCode::setSegmentLengths(ByteCodeSegment* segment) {
   while (segment != NULL) {
      setSegmentLengths(segment->_firstChild);
      _currSegment = segment;
      selectSection(SEGMENT_HEADER_SECTION_ID);

//--- Segment length

      putBits32At((segment->_lastSection->origin() +
                  segment->_lastSection->_bytePos) -
                  segment->_sections->origin(), 0);

//--- Rack size

      putBits32At(segment->getSection(RACK_SECTION_ID)->bytePos(), sizeof(UInt32));

//--- Clear hash

      putBits32At(0, 2 * sizeof(UInt32));

//--- Offset to initialisation code

      putBits32At(segment->getSection(INITIALISATION_CODE_SECTION_ID)->origin() -
                                      segment->_sections->origin(), 3 * sizeof(UInt32));

 //--- Update offset to child segment

      if ((_currSegment = segment->_parent) != NULL) {
         selectSection(CHILD_SEGMENT_TABLE_SECTION_ID);
         putBits32At(segment->_sections->origin() - 
                     _currSegment->_sections->origin(),
                     sizeof(Bits32) + segment->_index * sizeof(Bits32));
      }

      segment = segment->_next;
   }
}

Void ByteCode::copySections(Byte* bytes) const {
   for (ByteCodeSection* section = _sections; section != NULL; section = section->_next) {
      if (section->align()) {
           while ((reinterpret_cast<std::uintptr_t>(bytes) & 0x3) != 0)
            *bytes++ = 0;
      }
      section->_bytePtr = bytes;
      for (UInt i = 0; i < section->_bytePos; i++) 
         *bytes++ = section->_byteVec.get(i);
   }
}

Void ByteCode::segmentInfo(const ByteCodeSegment* segments, Byte* bytes, UInt level, ostream& os) const {
   UInt n = 0;
   for (const ByteCodeSegment* segment = segments; segment != NULL; segment = segment->next()) {
      for (UInt i = 0; i < level; i++)
         os << "  ";
      os << "Segment " << (void*)segment << ' ' << level << ':' << ++n << '(' << segmentLength(segment->sections()->_bytePtr) << ") [" << endl;
      sectionInfo(segment->sections(), segment->lastSection(), bytes, level, os);
      segmentInfo(segment->firstChild(), bytes, level + 1, os);
      for (UInt i = 0; i < level; i++)
         os << "  ";
      os << level << ':' << n << ']' << endl;
   }
}

Void ByteCode::sectionInfo(const ByteCodeSection* sections, const ByteCodeSection* lastSection,
                           Byte* bytes, UInt level, ostream& os) const {

   for (const ByteCodeSection* section = sections; section != NULL; section = section->_next) {
      for (UInt i = 0; i < level; i++)
         os << "  ";
      os << " Section "
         << section->_id
         << ", length = "
         << section->_bytePos
         << ", address = "
         << (Void*)section->_bytePtr << '(' << reinterpret_cast<uintptr_t>(section->_bytePtr)  << ')'
         << ", offset = "
         << section->_bytePtr - bytes
         << endl;
      if (section == lastSection)
         break;
   }
}

Loc::Kind ByteCode::locKind(Var& var, Repr repr) {
   switch (var.kind()) {
      case Var::LOCAL_VAR:
         if (repr == Repr::REPR_LABEL &&_basicBlk->localVar(var))
            return Loc::STACK_LOC;
         break;

      default:
         break;
   }
   return var.locKind(repr);
}

Bool ByteCode::suppressEnvMove(Void) const {
   return FALSE;
}

Void ByteCode::genOpCode(UInt opCode, ostream& os) {
   os << opCodeToString(opCode) << ' ';
   putByte(opCode);
}

Void ByteCode::genOperand(Operand& operand, Bool sepFlag, ostream& os) {
   if (sepFlag)
      os << ',';
   operand.genByteCode(os, *this);
}

Void ByteCode::genConstData(Operand& operand, Bool sepFlag, ostream& os) {
   if (operand.kind() == Operand::CONST_OPERAND) {
      const Const& k = ((ConstOperand&)operand).k();
      if (sepFlag)
         os << ',';
      k.genByteCodeData(*this);
      k.insert(os, *this);
   }
   else
      genOperand(operand, sepFlag, os);
}

Void ByteCode::putByte(Byte val) {
   _currSection->putByte(val, msa());
}

Void ByteCode::putByteAt(Byte val, UInt pos) {
   _currSection->putByteAt(val, pos, msa());
}

Void ByteCode::putBits16(Bits16 val) {
   putByte((Byte)((val >>  0) & 0xff));
   putByte((Byte)((val >>  8) & 0xff));
}

Void ByteCode::putBits32(Bits32 val) {
   putByte((Byte)((val >>  0) & 0xff));
   putByte((Byte)((val >>  8) & 0xff));
   putByte((Byte)((val >> 16) & 0xff));
   putByte((Byte)((val >> 24) & 0xff));
}

Void ByteCode::putBits64(Bits64 val) {
   putByte((Byte)((val >>  0) & 0xff));
   putByte((Byte)((val >>  8) & 0xff));
   putByte((Byte)((val >> 16) & 0xff));
   putByte((Byte)((val >> 24) & 0xff));
   putByte((Byte)((val >> 32) & 0xff));
   putByte((Byte)((val >> 40) & 0xff));
   putByte((Byte)((val >> 48) & 0xff));
   putByte((Byte)((val >> 56) & 0xff));
}

Void ByteCode::putBits32At(Bits32 val, UInt pos) {
   putByteAt((Byte)((val >>  0) & 0xff), pos);
   putByteAt((Byte)((val >>  8) & 0xff), pos + 1);
   putByteAt((Byte)((val >> 16) & 0xff), pos + 2);
   putByteAt((Byte)((val >> 24) & 0xff), pos + 3);
}

Void ByteCode::putBytes(Byte* buffer, UInt n) {
   for (UInt i = 0; i < n; i++)
      putByte(buffer[i]);
}

Void ByteCode::putVLU(unsigned long int val) {
   unsigned char buffer[MAX_VLU_LENGTH];
   putBytes(buffer, packVLU(val, buffer));
}

Void ByteCode::putVLI(long int val) {
   unsigned char buffer[MAX_VLI_LENGTH];
   putBytes(buffer, packVLI(val, buffer));
}

Void ByteCode::putImmUInt(UInt val) {
   if (0 <= val && val < 32)
      putByte((Byte)val & IMM_VAL_MASK | IMM_ADDR_MODE_VAL);
   else if (32 <= val && val < 256) {
      putByte(IMM_1_EXT_ADDR_MODE_VAL);
      putByte((Byte)val);
   } else if (256 <= val && val < 65536) {
      putByte(IMM_2_EXT_ADDR_MODE_VAL);
      putBits16((Bits16)val);
   } else {
      putByte(IMM_4_EXT_ADDR_MODE_VAL);
      putBits32((Bits32)val);
   }
}

Void ByteCode::putImmInt(Int val) {
   if (-16 <= val && val < 16)
      putByte((Byte)val & IMM_VAL_MASK | IMM_ADDR_MODE_VAL);
   else if (-128 <= val && val < 128) {
      putByte(IMM_1_EXT_ADDR_MODE_VAL);
      putByte((Byte)(Int8)val);
   } else if (-32768 <= val && val < 32768) {
      putByte(IMM_2_EXT_ADDR_MODE_VAL);
      putBits16((Bits16)(Int16)val); 
	} else {
      putByte(IMM_4_EXT_ADDR_MODE_VAL);
      putBits32((Bits32)val);
   }
}

Void ByteCode::putRegAddrMode(UInt id) {
   putByte(id << REG_ID_OFFSET | REG_ADDR_MODE_VAL);
}

Void ByteCode::putStackAddrMode(Int64 offset) {
   if (-16 <= offset && offset < 16)
      putByte((Byte)offset & IMM_VAL_MASK | STACK_ADDR_MODE_VAL);
   else
      if (-128 <= offset && offset < 128) {
         putByte(STACK_1_EXT_ADDR_MODE_VAL);
         putByte((Byte)(Int8)offset);
      } else if (-32768 <= offset && offset < 32768) {
         putByte(STACK_2_EXT_ADDR_MODE_VAL);
         putBits16((Bits16)(Int16)offset);
      } else if (-2147483647LL - 1 <= offset && offset < 2147483648LL) {
         putByte(STACK_4_EXT_ADDR_MODE_VAL);
         putBits32((Bits32)(Int32)offset);
      } else {
         putByte(STACK_8_EXT_ADDR_MODE_VAL);
         putBits64((Bits64)offset);
      }
}

Void ByteCode::putFrameAddrMode(UInt64 offset) {
   if (offset < 256) {
      putByte(FRAME_1_EXT_ADDR_MODE_VAL);
      putByte((Byte)offset);
   }
   else if (256 <= offset && offset < 65536) {
      putByte(FRAME_2_EXT_ADDR_MODE_VAL);
      putBits16((Bits16)offset);
   }
   else if (65536 <= offset && offset < 0x100000000ULL) {
      putByte(FRAME_4_EXT_ADDR_MODE_VAL);
      putBits32((Bits32)offset);
   }
   else {
      putByte(FRAME_8_EXT_ADDR_MODE_VAL);
      putBits64((Bits64)offset);
   }
}

Void ByteCode::putCellAddrMode(UInt64 offset) {
   if (offset < 32)
      putByte((Byte)offset & IMM_VAL_MASK | CELL_ADDR_MODE_VAL);
   else
      if (32 <= offset && offset < 256) {
         putByte(CELL_1_EXT_ADDR_MODE_VAL);
         putByte((Byte)offset);
      } else if (256 <= offset && offset < 65536) {
         putByte(CELL_2_EXT_ADDR_MODE_VAL);
         putBits16((Bits16)offset);
      } else if (65536 <= offset && offset < 0x100000000ULL) {
         putByte(CELL_4_EXT_ADDR_MODE_VAL);
         putBits32((Bits32)offset);
      } else {
         putByte(CELL_8_EXT_ADDR_MODE_VAL);
         putBits64((Bits64)offset);
      }
}


Void ByteCode::putRackAddrMode(UInt64 offset) {
   if (offset < 32) {
      putByte((Byte)offset | RACK_ADDR_MODE_VAL);
   } else if (offset < 256) {
      putByte(RACK_1_EXT_ADDR_MODE_VAL);
      putByte((Byte)offset);
   } else if (256 <= offset && offset < 65536) {
      putByte(RACK_2_EXT_ADDR_MODE_VAL);
      putBits16((Bits16)offset);
   } else if (65536 <= offset && offset < 0x100000000ULL) {
      putByte(RACK_4_EXT_ADDR_MODE_VAL);
      putBits32((Bits32)offset);
   } else {
      putByte(RACK_8_EXT_ADDR_MODE_VAL);
      putBits64((Bits64)offset);
   }
}

Void ByteCode::putIndAddrMode(Int64 offset) {
   if (0 <= offset && offset < 32)
      putByte((Byte)offset | IND_ADDR_MODE_VAL);
   else
      if (32 <= offset && offset < 256) {
         putByte(IND_1_EXT_ADDR_MODE_VAL);
         putByte((Byte)offset);
      } else if (256 <= offset && offset < 65536) {
         putByte(IND_2_EXT_ADDR_MODE_VAL);
         putBits16((Bits16)offset);
      } else if (65536 <= offset && offset < 0x100000000ULL) {
         putByte(IND_4_EXT_ADDR_MODE_VAL);
         putBits32((Bits32)offset);
      } else {
         putByte(IND_8_EXT_ADDR_MODE_VAL);
         putBits64((Bits64)offset);
      }
}

Void ByteCode::putIndexedAddrMode(UInt64 offset) {
   if (offset < 256) {
      putByte(INDEXED_1_EXT_ADDR_MODE_VAL);
      putByte((Byte)offset);
   } else if (256 <= offset && offset < 65536) {
      putByte(INDEXED_2_EXT_ADDR_MODE_VAL);
      putBits16((Bits16)offset);
   } else if (65536 <= offset && offset < 0x100000000ULL) {
      putByte(INDEXED_4_EXT_ADDR_MODE_VAL);
      putBits32((Bits32)offset);
   } else {
      putByte(INDEXED_8_EXT_ADDR_MODE_VAL);
      putBits64((Bits64)offset);
   }
}

Void ByteCode::putCodeEnv(Void) {
   putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = bytePos();
   putBits32(0);
   addTargetBackPatch(pos, 0, RACK_SECTION_ID, FALSE);
}

Void ByteCode::putTypeConst(Type type, ostream& os) {

//--- Generate memory indirection to segment type table entry

   UInt index = currSegment()->addTypeConst(type, msa());
   putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = bytePos();
   putBits32(0);
   addTargetBackPatch(pos,
      sizeof(Bits16) + index * sizeof(Bits16),
      TYPE_TABLE_SECTION_ID,
      FALSE);
}

Repr ByteCode::labelRepr(Bool simple/* = FALSE*/) const {
   return !simple ? Repr::REPR_LABEL_PAIR
      : Repr::REPR_I_ADDRESS;
}

Void InstructionTrait::genByteCode(ostream& os, ByteCode& code) const {
}

Void TypeDescrInstruction::genByteCode(ostream& os, ByteCode& code) const {
   ByteCodeSection* section = code.selectInitCodeSection();
   code.genOpCode(ATD_INS, os);
   _type.genByteCode(os, code);
   code.putCodeEnv();
   os << ',';
   _denotedType.genByteCode(os, code);
   os << ',';
   Type type = ((ConstT<Type>&)_type.k()).value();
   size_t size = sizeOfRepr(typeDescriptor(type, code.typeTable()).repr());
   code.putVLU(size);
   os << size << ',';

   if (_evalFn != NULL) {
      UInt pos = code.bytePos();
      pos = code.bytePos();
      code.putBits32(0);
      code.addSegmentBackPatch(pos, *_evalFn->codeLabel().lambda());
      _evalFn->genByteCode(os, code);
   } else {
      code.putBits32(0);
      code.putBits32(0);
   }

   os << ',';
   if (_mapFn != NULL) {
      UInt pos = code.bytePos();
      pos = code.bytePos();
      code.putBits32(0);
      code.addSegmentBackPatch(pos, *_mapFn->codeLabel().lambda());
      _mapFn->genByteCode(os, code);
   } else {
      code.putBits32(0);
      code.putBits32(0);
   }

   os << ',';
   if (_assignFn != NULL) {
      UInt pos = code.bytePos();
      pos = code.bytePos();
      code.putBits32(0);
      code.addSegmentBackPatch(pos, *_assignFn->codeLabel().lambda());
      _assignFn->genByteCode(os, code);
   }
   else {
      code.putBits32(0);
      code.putBits32(0);
   }

   os << ',';
   if (_insertTxtFn != NULL) {
      UInt pos = code.bytePos();
      pos = code.bytePos();
      code.putBits32(0);
      code.addSegmentBackPatch(pos, *_insertTxtFn->codeLabel().lambda());
      _insertTxtFn->genByteCode(os, code);
   }
   else {
      code.putBits32(0);
      code.putBits32(0);
   }

#if (SERIALISATION==1)
   os << ',';
   if (_extractBinFn != NULL) {
      UInt pos = code.bytePos();
      pos = code.bytePos();
      code.putBits32(0);
      code.addSegmentBackPatch(pos, *_extractBinFn->codeLabel().lambda());
      _extractBinFn->genByteCode(os, code);
   }
   else {
      code.putBits32(0);
      code.putBits32(0);
   }

   os << ',';
   if (_insertBinFn != NULL) {
      UInt pos = code.bytePos();
      pos = code.bytePos();
      code.putBits32(0);
      code.addSegmentBackPatch(pos, *_insertBinFn->codeLabel().lambda());
      _insertBinFn->genByteCode(os, code);
   }
   else {
      code.putBits32(0);
      code.putBits32(0);
   }
#endif

#if (GARBAGE_COLLECTION==1)
   os << ',';
   if (_markFn != NULL) {
      UInt pos = code.bytePos();
      pos = code.bytePos();
      code.putBits32(0);
      code.addSegmentBackPatch(pos, *_markFn->codeLabel().lambda());
      _markFn->genByteCode(os, code);
   }
   else {
      code.putBits32(0);
      code.putBits32(0);
   }
#endif

   code.selectSection(section);
}

Void EntryInstruction::genByteCode(ostream& os, ByteCode& code) const {
}

Void LabelInstruction::genByteCode(ostream& os, ByteCode& code) const {
   CodeLabel& codeLabel = _codeLabel;
   codeLabel._bytePos = code.bytePos();
   codeLabel._section = code._currSection;

   _codeLabel.insert(os, code); os << ':';
   if (_codeLabel.withBasicBlock()) {
      BasicBlk& basicBlk = _codeLabel.basicBlk();
      code._basicBlk = &basicBlk;
      code._sp = basicBlk._sp;
   }

   if (_codeLabel._saveEntryFlag)
      code.saveEntryPoint();
}

Void GoToInstruction::genByteCode(ostream& os, ByteCode& code) const {
   switch (target().kind()) {
      case Operand::BUILT_IN_FN_OPERAND:
      case Operand::BUILT_IN_DATA_CON_OPERAND:
         code.genOpCode(JMP_INS, os);
         break;

      case Operand::LOC_OPERAND:
         code.genOpCode(!_altEntryFlag ? ENT_P_INS : ENT_A_P_INS, os);
         break;

      case Operand::LABEL_OPERAND:
         code.genOpCode(BRA_INS, os);
         code.setBranchBackPatchFlag(TRUE);
         target().genByteCode(os, code);
         code.setBranchBackPatchFlag(FALSE);
         return;

      default:
         assert(FALSE, "GoToInstruction::genByteCode: Unexpected target");
         break;
   }
   target().genByteCode(os, code);
}

Void PtrArrayAssignInstruction::genByteCode(ostream& os, ByteCode& code) const {
   Byte opCode;
   Repr srcRepr = _src->repr(code);
   Repr dstRepr = _dst->repr(code);

   switch (dstRepr.tag()) {

      case Repr::REPR_EXPR:   opCode = MOV_E_INS;
         break;
      case Repr::REPR_NAME:   opCode = MOV_N_INS;
         break;
      case Repr::REPR_TYPE:   opCode = MOV_T_INS;
         break;
      case Repr::REPR_BYTE:   opCode = MOV_B_INS;
         break;
      case Repr::REPR_UTC:
      case Repr::REPR_INT:    opCode = MOV_I_INS;
         break;
      case Repr::REPR_FLOAT:  opCode = MOV_F_INS;
         break;
      case Repr::REPR_DOUBLE: opCode = MOV_D_INS;
         break;
      case Repr::REPR_TAG:    opCode = MOV_O_INS;
         break;
      case Repr::REPR_CHAR:   opCode = MOV_C_INS;
         break;

      case Repr::REPR_STRING: if (_src->kind() == Operand::CONST_OPERAND) {
                                 opCode = MCP_INS;
                                 break;
                              }
   //--------------------------- Drop through
  
      case Repr::REPR_ENV_PTR:
      case Repr::REPR_SERIAL_CONTEXT:
      case Repr::REPR_PTR:    opCode = MOV_P_INS;
         break;

      case Repr::REPR_CELL_PTR:
                              opCode = srcRepr.tag() != Repr::REPR_EXPR
                                 ? MOV_P_INS
                                 : ETC_INS;
         break;


      case Repr::REPR_REF:    opCode = MOV_R_INS;
         break;
      case Repr::REPR_STRUCT:
         opCode = MMV_B_INS;
         break;
      default:                ivoryError("ArrayPtrAssignInstruction::genByteCode: unexpected repr");
         opCode = ILG_INS;
         break;
   }
   code.genOpCode(opCode, os);
   code.genOperand(*_src, FALSE, os);
   code.genOperand(*_dst, TRUE, os);
   if (opCode == MMV_B_INS) {
      size_t size = dstRepr.structRepr()->size();
      code.putVLU(size);
      os << ',' << size;
   }
}

Void MoveInstruction::genByteCode(ostream& os, ByteCode& code) const {
   Byte opCode;
   Repr srcRepr = _src->repr(code);
   Repr dstRepr = _dst->repr(code);
   
   switch (dstRepr.tag()) {
      case Repr::REPR_LABEL:  opCode = MOV_L_INS;
                              break;
      case Repr::REPR_I_ADDRESS:
                              opCode = MOV_I_A_INS;
                              break;
      case Repr::REPR_LABEL_PAIR:
                              opCode = MIL_INS;
                              break;
      case Repr::REPR_EXPR:   opCode = MOV_E_INS;
                              break;
      case Repr::REPR_NAME:   opCode = MOV_N_INS;
                              break;
      case Repr::REPR_TYPE:   opCode = MOV_T_INS;
                              break;
      case Repr::REPR_BYTE:   opCode = MOV_B8_INS;
                              break;
      case Repr::REPR_BITS:   opCode = MOV_B_INS;
                              break;
      case Repr::REPR_UTC:
      case Repr::REPR_INT:    opCode = MOV_I_INS;
                              break;
      case Repr::REPR_FLOAT:  opCode = MOV_F_INS;
                              break;
      case Repr::REPR_DOUBLE: opCode = MOV_D_INS;
                              break;
      case Repr::REPR_TAG:    opCode = MOV_O_INS;
                              break;
      case Repr::REPR_CHAR:   opCode = MOV_C_INS;
                              break;
      case Repr::REPR_STRING: if (_src->kind() == Operand::CONST_OPERAND) {
                                 opCode = MCP_INS;                                       
                                 break;
                              }
//--------------------------- Drop through
 
      case Repr::REPR_ENV_PTR:
      case Repr::REPR_DIR:
      case Repr::REPR_DIR_ENTRY:
      case Repr::REPR_SERIAL_CONTEXT:
      case Repr::REPR_PTR:    opCode = MOV_P_INS;
                              break;

      case Repr::REPR_CELL_PTR:
                              opCode = srcRepr.tag() != Repr::REPR_EXPR
                                 ? MOV_P_INS
                                 : ETC_INS;
                              break;


      case Repr::REPR_REF:    opCode = MOV_R_INS;
                              break;
      case Repr::REPR_FILE_HANDLE:
      case Repr::REPR_STRUCT:
                              opCode = MMV_B_INS;
                              break;
      default:                ivoryError("MoveInstruction::genByteCode: unexpected repr");
                              opCode = ILG_INS;
                              break;
   }
   code.genOpCode(opCode, os);
   code.genOperand(*_src, FALSE, os);
   code.genOperand(*_dst, TRUE, os);
   if (opCode == MMV_B_INS) {
      size_t size = sizeOfRepr(dstRepr);
      code.putVLU(size);
      os << ',' << size;
   }
}

Void LVP_Instruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(LVP_INS, os);
   code.genOperand(*_src, FALSE, os);
   code.genOperand(*_dst, TRUE, os);
}

Void EnterCellInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(!_altEntryFlag ? ENT_P_INS : ENT_A_P_INS, os);
   _x->genByteCode(os, code);
}

#if (GARBAGE_COLLECTION==1)
Void MarkCellInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(MRK_CL_INS, os);
   _x->genByteCode(os, code);
}
#endif

Void RSS_Instruction::genByteCode(ostream& os, ByteCode& code) const {
   if (_size > 0) {
      code.genOpCode(RSS_INS, os);
      code.putVLU(_size);
      os << _size;
   }
}

Void ReturnInstruction::genByteCode(ostream& os, ByteCode& code) const {
   UInt opCode;
   switch (_form) {
      case SIMPLE:   opCode = RET_INS;
                     break;

      case GENERAL:  opCode = RTN_INS;
                     break;

      default:       opCode = EXIT_INS;
   }
   code.genOpCode(opCode, os);
}

Void ConditionalInstruction::genByteCode(ostream& os, ByteCode& code) const {
   UInt opCode = _op.kind() != Operator::SPECIAL_EXPR_OP
      ? _op.branchByteCode()
      : BSE_INS;
   code.genOpCode(opCode, os);
   code.genOperand(*_x, FALSE, os);
   if (_x_env != NULL)
      code.genOperand(*_x_env, TRUE, os);
   if (_y != NULL)
      code.genOperand(*_y, TRUE, os);
   if (_y_env != NULL)
      code.genOperand(*_y_env, TRUE, os);
   code.genOperand(*_z, TRUE, os);
}

Void MonadicOpActionInstruction::genByteCode(ostream& os, ByteCode& code) const {
   Byte opCode; 
   if (_x->kind() == Operand::CONST_OPERAND &&
      _op.kind() == Operator::ERROR_OP) {
      opCode = ECP_INS;
   } else if (_x->kind() == Operand::CONST_OPERAND &&
              _op.kind() == Operator::TRACE_OP) {
      opCode = TCP_INS;
   } else
   opCode = _op.byteCode(_x->kind() == Operand::CONST_OPERAND);
   code.genOpCode(opCode, os);
   code.genOperand(*_x, FALSE, os);
   if (_x_env != NULL)
      code.genOperand(*_x_env, TRUE, os);
}

Void MonadicOpInstruction::genByteCode(ostream& os, ByteCode& code) const {
   Bool isError = _op.kind() == Operator::ERROR_OP;
   code.genOpCode(_op.byteCode(_src->kind() == Operand::CONST_OPERAND), os);
   code.genOperand(*_src, FALSE, os);
   if (_op.kind() != Operator::FROM_STRING_OP && _src_env != NULL)
      code.genOperand(*_src_env, TRUE, os);
   if (_op.kind() == Operator::ALLOC_PTR_ARRAY_OP) {
      os << ',';
      size_t size = sizeOfRepr(_dst->repr(code).ptrRepr()->repr().arrayRepr()->elementRepr());
      code.putVLU(size);
      os << size;
   }
   
   if (!isError)
      code.genOperand(*_dst, TRUE, os);
}

Void UpdateInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(UPD_INS, os);
   code.genOperand(*_src, FALSE, os);
   code.genOperand(*_dst, TRUE, os);
}

Void ConstructFromNameInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(CFN_INS, os);
   code.genOperand(*_src, FALSE, os);
   if (_src_env != NULL)
      code.genOperand(*_src_env, TRUE, os);
   os << ',';
   code.putTypeConst(code.useType(_typeSig), os);
   os << ',';
   code.putCodeEnv();
   os << ',';
   code.genOperand(*_dst, TRUE, os);
}

Void ConstructFromStringInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(CFS_INS, os);
   code.genOperand(*_src, FALSE, os);
   os << ',';
   code.putTypeConst(code.useType(_typeSig), os);
   os << ',';
   code.putCodeEnv();
   os << ',';
   code.genOperand(*_dst, TRUE, os);
}

Void DyadicOpInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(_op.byteCode(_x->kind() == Operand::CONST_OPERAND), os);
   if (_x != NULL)
      code.genOperand(*_x, FALSE, os);
   if (_x_env != NULL)
      code.genOperand(*_x_env, TRUE, os);
   if (_y != NULL)
      code.genOperand(*_y, TRUE, os);
   if (_y_env != NULL)
      code.genOperand(*_y_env, TRUE, os);
   code.genOperand(*_dst, TRUE, os);
}

Void DyadicOpActionInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(_op.byteCode(_x->kind() == Operand::CONST_OPERAND), os);
   code.genOperand(*_x, FALSE, os);
   if (_x_env != NULL)
      code.genOperand(*_x_env, TRUE, os);
   code.genOperand(*_y, TRUE, os);
   if (_y_env != NULL)
      code.genOperand(*_y_env, TRUE, os);
}

Void TriadicOpActionInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(_op.byteCode(_x->kind() == Operand::CONST_OPERAND), os);
   code.genOperand(*_x, FALSE, os);
   code.genOperand(*_y, TRUE, os);
   code.genOperand(*_z, TRUE, os);
}

Void QuinadicOpActionInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(_op.byteCode(_x->kind() == Operand::CONST_OPERAND), os);
   code.genOperand(*_v, FALSE, os);
   if (_v_env != NULL)
      code.genOperand(*_v_env, TRUE, os);
   if (_op.kind() == Operator::COPY_PTR_ARRAY_OP) {
      os << ',';
      size_t size = sizeOfRepr(_w->repr(code).ptrRepr()->repr().arrayRepr()->elementRepr());
//--- Assumes src (w) and dst (y) same type
      code.putVLU(size);
      os << size;
   }
   code.genOperand(*_w, TRUE, os);
   if (_w_env != NULL)
      code.genOperand(*_w_env, TRUE, os);
   code.genOperand(*_x, TRUE, os);
   if (_x_env != NULL)
      code.genOperand(*_x_env, TRUE, os);
   code.genOperand(*_y, TRUE, os);
   if (_y_env != NULL)
      code.genOperand(*_y_env, TRUE, os);
   code.genOperand(*_z, TRUE, os);
   if (_z_env != NULL)
      code.genOperand(*_z_env, TRUE, os);
}

Void CaseInstruction::genByteCode(ostream& os, ByteCode& code) const {

// Default to branch via association table (k0, l0), (k1, l1) ...

   Byte opCode;
   switch (_x->repr(code).tag()) {
      case Repr::REPR_NAME:   opCode = ATB_N_INS;
                              break;
      case Repr::REPR_TYPE:   opCode = ATB_T_INS;
                              break;
      case Repr::REPR_INT:    opCode = ATB_I_INS;
                              break;
      case Repr::REPR_TAG:    opCode = ATB_O_INS;
                              break;
      case Repr::REPR_CHAR:   opCode = ATB_C_INS;
                              break;
      case Repr::REPR_STRING: opCode = _x->kind() == Operand::CONST_OPERAND
                                 ? ATB_CS_INS
                                 : ATB_S_INS;
                              break;
      default:                ivoryError("CaseInstruction::genByteCode: unexpected repr");
                              opCode = ILG_INS;
                              break;
   }
   
   code.genOpCode(opCode, os);
   code.genOperand(*_x, FALSE, os);
   if (_x_env != NULL) {
      code.genOperand(*_x_env, TRUE, os);
      code.putCodeEnv();
   }

   ByteCodeSection* section = code.currSection();
   code._codeSection = section;     // Fix for case where current section is not origin
   code.selectDataSection();
   UInt dataPos = code.bytePos();
   os << ",[";
   for (UInt i = 0; i < _n; i++) {
      if (i > 0)
         os << ',';
      os << '(';
      code.genConstData(*_consts[i], FALSE, os);
      os << ',';
      code.genConstData(*_labels[i], FALSE, os);
      os << ')';
   }
   os << ']';
   if (_otherwise != NULL) {
      os << ',';
      code.genConstData(*_otherwise, FALSE, os);
   }
   code._codeSection = NULL;
   code.selectSection(section);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, dataPos);
   code.putVLU(_n);
}

Void ArgCheckInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(ASC_INS, os);
   code.putVLU(_n);
   if (_n > 0) {
      ByteCodeSection* section = code.currSection();
      code._codeSection = section;     // Fix for case where current section is not origin
      code.selectDataSection();
      UInt dataPos = code.bytePos();
      os << ",[";
      for (UInt i = 0; i < _n; i++) {
         if (i > 0)
            os << ',';
         code.putBits32(_partialDepths[i]);
         os << '(' << _partialDepths[i] << ',';
         code.genConstData(*_labels[i], FALSE, os);
         os << ')';
      }
      os << ']';
      code._codeSection = NULL;
      code.selectSection(section);
      UInt pos = code.bytePos();
      code.putBits32(0);
      code.addTargetBackPatch(pos, dataPos);
   }
}

Void CellInfoInstruction::genByteCode(ostream& os, ByteCode& code) const {
   ByteCodeSection* section = code.selectInitCodeSection();
   code.genOpCode(ACI_INS, os);
   os << _cellInfo._seqNo;

   _cellInfo._rackOffset = code.rackBytePos();
   code.reserveRack(sizeof(Ptr));

   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addLabelBackPatch(pos, _cellInfo._entryLabel);
   os << ',';
   _cellInfo._entryLabel.insert(os, code);

   pos = code.bytePos();
   code.putBits32(0);
   code.addLabelBackPatch(pos, _cellInfo._altEntryLabel);
   os << ',';
   _cellInfo._altEntryLabel.insert(os, code);

   pos = code.bytePos();
   code.putBits32(0);
   os << ',';
   if (_cellInfo._copyFnLabel != NULL) {
      code.addLabelBackPatch(pos, *_cellInfo._copyFnLabel);
       _cellInfo._copyFnLabel->insert(os, code);
   }

   pos = code.bytePos();
   code.putBits32(0);
   os << ',';
   if (_cellInfo._extractBinFnLabel != NULL) {
      code.addLabelBackPatch(pos, *_cellInfo._extractBinFnLabel);
      _cellInfo._extractBinFnLabel->insert(os, code);
   }

   pos = code.bytePos();
   code.putBits32(0);
   os << ',';
   if (_cellInfo._insertBinFnLabel != NULL) {
      code.addLabelBackPatch(pos, *_cellInfo._insertBinFnLabel);
      _cellInfo._insertBinFnLabel->insert(os, code);
   }

#if (GARBAGE_COLLECTION==1)
   pos = code.bytePos();
   code.putBits32(0);
   os << ',';
   if (_cellInfo._gcFnLabel != NULL) {
      code.addLabelBackPatch(pos, *_cellInfo._gcFnLabel); 
      _cellInfo._gcFnLabel->insert(os, code);
   }
#endif

   code.putVLU(_structTemplate.nSlots() > 0
                  ? _structTemplate.repr().size()
                  : 0);

   pos = code.bytePos();
   code.putBits32(0);
   code.addSegmentBackPatch(pos, *_cellInfo._altEntryLabel.lambda());

   pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, _cellInfo._rackOffset, RACK_SECTION_ID);

   code.selectSection(section); 
}



#if GARBAGE_COLLECTION==1

Void FrameDescrInstruction::genByteCode(ostream& os, ByteCode& code) const {
   const FrameDescr<CodeLabel*>& frameDescr = _frameDescr;
   ByteCodeSection* section = code.selectInitCodeSection();
   code.genOpCode(AFD_INS, os);

   _labOperand.genByteCode(os, code);

   code.putVLU(frameDescr.labSeqNo());
   code.putVLU(frameDescr.size());
   os << ',' << frameDescr.size();

   code.putVLU(_frameDescr.nSlots());
   os << ",[";
   for (UInt i = 0; i < frameDescr.nSlots(); i++) {
      if (i > 0)
         os << ',';
      code.putBits16(frameDescr.slot(i).name());
      UInt pos = code.bytePos();
      code.putBits32(0);
      code.addLabelBackPatch(pos, *frameDescr.slot(i).address(), FALSE);
      frameDescr.slot(i).address()->insert(os, code);
   }
   os << ']';

   code.selectSection(section);
}

#endif

Void HeapAllocInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(ALC_INS, os);
   code.putImmUInt(_size);
   os << _size;
   code.genOperand(*_dst, TRUE, os);
}

Void CellAllocInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(_env == NULL ? ACL_INS : ACL_ENV_INS, os);
   size_t size = _structTemplate.nSlots() > 0
      ? _structTemplate._repr->size()
      : 0;
   code.putVLU(size);
   os << size;
   code.genOperand(*_cellInfo, TRUE, os);
   if (_env != NULL)
      code.genOperand(*_env, TRUE, os);
   code.genOperand(*_dst, TRUE, os);
}

Void IndirectionAllocInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(AIN_INS, os);
   code.genOperand(*_dst, TRUE, os);
}

Void PtrArrayAllocInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(APA_INS, os);
   code.genOperand(*_n, FALSE, os);
   os << ',';
   code.putVLU(_size);
   os << _size;
   code.genOperand(*_dst, TRUE, os);
}

Void TagInstruction::genByteCode(ostream& os, ByteCode& code) const {
   Byte opCode;
   switch (_srcRepr.tag()) {
      case Repr::REPR_CELL_PTR:  opCode = EFC_INS;
										   break;
      case Repr::REPR_NAME:		opCode = TAG_N_INS;
										   break;
      case Repr::REPR_TYPE:		opCode = TAG_T_INS;
										   break;
      case Repr::REPR_INT:       opCode = TAG_I_INS;
                                 break;
      case Repr::REPR_FLOAT:		opCode = TAG_F_INS;
										   break;
      case Repr::REPR_DOUBLE:		opCode = TAG_D_INS;
										   break;
      case Repr::REPR_TAG:			opCode = TAG_O_INS;
										   break;
      case Repr::REPR_CHAR:		opCode = TAG_C_INS;
										   break; 
      case Repr::REPR_STRING:    opCode = _src->kind() == Operand::CONST_OPERAND
                                    ? opCode = TSC_INS
                                    : TAG_S_INS;
                                 break;
      case Repr::REPR_REF:			opCode = TAG_R_INS;
										   break;
      default:						   opCode = TAG_INS;
										break;
   }
   code.genOpCode(opCode, os);
   _src->genByteCode(os, code);
   os << ',';
   _dst->genByteCode(os, code);
}

Void ToCellInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(ETC_INS, os);
   code.genOperand(*_src, FALSE, os);
   code.genOperand(*_dst, TRUE, os);
}

Void StackAdjustInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code._sp += _diff;
   code.genOpCode(_diff > 0 ? SUB_SP_INS : ADD_SP_INS, os);
   code.putVLU(abs(_diff));
   os << abs(_diff);
}

Void ExceptionInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(EXC_INS, os);
}

Void EmptyArgsInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(_diff == 0 ? MOV_P_INS : ADD_P_INS, os);
   code.reg(REG_sp).genByteCode(0, os, code);
   if (_diff != 0) {
      os << ',' << _diff;
      code.putImmInt(_diff);
   }
   os << ',';
   code.reg(REG_sb).genByteCode(0, os, code);
}

Void SqueezeInstruction::genByteCode(ostream& os, ByteCode& code) const {
   code.genOpCode(SQZ_INS, os);
   code.putVLI((Int)code._sp - (Int)_from);
   os << ((Int)code._sp - (Int)_from);
   os << ',';
   code.putVLI((Int)code._sp - (Int)_to);
   os << ((Int)code._sp - (Int)_to);
   os << ',';
   code.putVLU(_size);
   os << _size;
}

Void Operand::genByteCode(ostream& os, ByteCode& code) const {
   error("Operand::genByteCode: no overload");
}

Void LabelOperand::genByteCode(ostream& os, ByteCode& code) const {
  _codeLabel.genByteCode(os, code);
}

Void ThisOperand::genByteCode(ostream& os, ByteCode& code) const {
   UInt offset = sizeof(Ref);
   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
}

Void CodeLabel::genByteCode(ostream& os, ByteCode& code) {
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addLabelBackPatch(pos, *this);
   insert(os, code);
}

Void Loc::genByteCode(Int offset, ostream& os, ByteCode& code) {
   assert(FALSE, "Loc::genByteCode: Method not defined for derived class");
}

Void Register::genByteCode(Int offset, ostream& os, ByteCode& code) {
   assert(offset == 0, "Register::genByteCode: Offset not 0");
   code.putRegAddrMode(_id);
   os << regCodeToString(_id);
}

Void SpecialLoc::genByteCode(Int offset, ostream& os, ByteCode& code) {
   assert(offset == 0, "SpecialLoc::genByteCode: Offset not 0");
   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, 0, RACK_SECTION_ID, FALSE);
   os << "@0";
}

Void StaticLoc::genByteCode(Int offset, ostream& os, ByteCode& code) {
   UInt size = sizeOfRepr(_repr);

   if (!_allocated) {
      Int16 nameIndex = code.currSegment()->nameConstIndex(_name);
      const ImportRecord* impRec = nameIndex != -1
         ? code.currSegment()->importRec(nameIndex)
         : NULL;

      if (impRec != NULL)
         _rackPos = impRec->_offset;
      else {

//------ Allocate statically in rack section

         code.padRack(size);
         _rackPos = code.rackBytePos();
         code.reserveRack(size);
      }
 
      if (_imported && impRec == NULL) {
         nameIndex = code.currSegment()->addNameConst(_name, code.msa());
         code.currSegment()->addImportRec(nameIndex, _rackPos, code.msa());
      }

      if (_exported)
         code.moduleDefn()->addGlobal(_name, _rackPos);
      _allocated = TRUE;
   }

   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, _rackPos, RACK_SECTION_ID, FALSE);
   os << "rack[" << _rackPos << ']';
}

Void StackLoc::genByteCode(Int offset, ostream& os, ByteCode& code) {

#if (GARBAGE_COLLECTION==1)
   if (_origin == StackLoc::SP_REL) {
#endif
      Int stackOffset = (Int)code._sp - ((Int)_offset + (Int)_size);
      code.putStackAddrMode(stackOffset + offset);
      os << "sp[" << stackOffset + offset << ']';

#if (GARBAGE_COLLECTION==1)
   } else {
      UInt stackOffset = _offset + _size;
      code.putFrameAddrMode(stackOffset - offset);
      os << "fp[" << stackOffset - offset << ']';
   }
#endif

}

Void CellCompLoc::genByteCode(Int offset, ostream& os, ByteCode& code) {
   UInt byteOffset = _structRepr.offsetV()[_index - 1] + sizeof(Expr);
   if (_loc.kind() == Loc::REGISTER_LOC &&
      ((Register&)_loc).id() == REG_cell) {
      code.putCellAddrMode(offset + byteOffset);
      os << "cell[" << offset + byteOffset << ']';
   } else {
      os << "@(";
      code.putIndAddrMode(offset + byteOffset);
      _loc.genByteCode(0, os, code);
      os << ")[" << offset + byteOffset << ']';
   }
}

Void CompLoc::genByteCode(Int offset, ostream& os, ByteCode& code) {
//   _loc.genByteCode(offset + _loc.repr().structRepr()->offsetV()[_index - 1], os, code);
     _loc.genByteCode(offset + _structRepr.offsetV()[_index - 1], os, code);
}

Void IndLoc::genByteCode(Int offset, ostream& os, ByteCode& code) {
   os << "@(";
   code.putIndAddrMode(offset);
   _loc.genByteCode(0, os, code);
   os << ")[" << offset << ']';
}

Void IndexedLoc::genByteCode(Int offset, ostream& os, ByteCode& code) {
   code.putIndexedAddrMode(offset);
   UInt elemSize = sizeOfRepr(_repr);
   code.putVLU(elemSize);
   os << "((";
   _loc.genByteCode(0, os, code);
   os << ':';
   os << elemSize;
   os << ')';
   os << '[';
   if (_isConst)
      _index._const->genByteCode(os, code);
   else
      _index._loc->genByteCode(0, os, code);
   os << ']';
   os << ")[" << offset << ']';
}

Void ConstOperand::genByteCode(ostream& os, ByteCode& code) const {
   _k.genByteCode(os, code);
}

Void CellInfoOperand::genByteCode(ostream& os, ByteCode& code) const {
   if (_cellInfo != NULL) {
      code.putByte(IMM_4_EXT_ADDR_MODE_VAL);
      code.putBits32((Bits32)&_cellInfo);
   }
   else {
      code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
      UInt pos = code.bytePos();
      code.putBits32(0);
      code.addTargetBackPatch(pos, _cellInfo_->_rackOffset,
                              RACK_SECTION_ID, FALSE);
   }
   insert(os, code);
}

Void BuiltInFnOperand::genByteCode(ostream& os, ByteCode& code) const {
   ConstString ident = builtInNameTable().string(cellBody(*_builtInFn, BuiltInFn).name());
   ConstString suffix = _asClosure
      ? BUILT_IN_CLOSURE_SUFFIX
      : (!_altEntryFlag ? BUILT_IN_ENTRY_SUFFIX : BUILT_IN_ALT_ENTRY_SUFFIX);

   String extIdent = (String)code.msa().alloc(strlen(ident) + strlen(suffix) + 1);

   strcpy(extIdent, ident);
   strcat(extIdent, suffix);

   Name name = ::useName(extIdent, code.nameTable());
   code.msa().free(extIdent);

   UInt16 nameIndex = code.currSegment()->addNameConst(name, code.msa());
   UInt32 rackPos;

   const struct ImportRecord* impRec = code.currSegment()->importRec(nameIndex);
   if (impRec == NULL) {

//--- Allocate pointer to cell or label in rack on first use

      UInt size = sizeof(Ptr);

      code.padRack(size);
      rackPos = code.rackBytePos();
      code.reserveRack(size);

      code.currSegment()->addImportRec(nameIndex, rackPos, code.msa());
   }
   else
      rackPos = impRec->_offset;

//   os << "@(";
//   code.putIndAddrMode(0);
   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, rackPos, RACK_SECTION_ID, FALSE);
   os << "rack[" << rackPos << ']';
//   os << ")[0]";
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

Void BuiltInDataConOperand::genByteCode(ostream& os, ByteCode& code) const {
   ConstString ident = builtInNameTable().string(toBody(_dataCon, BuiltInDataCon).name());
   ConstString suffix =
      repr(code) == Repr::REPR_CELL_PTR
      ? BUILT_IN_CLOSURE_SUFFIX
      : (!_altEntryFlag ? BUILT_IN_ENTRY_SUFFIX : BUILT_IN_ALT_ENTRY_SUFFIX);

   String extIdent = (String)code.msa().alloc(strlen(ident) + strlen(suffix) + 1);

   strcpy(extIdent, ident);
   strcat(extIdent, suffix);

   Name name = ::useName(extIdent, code.nameTable());
   code.msa().free(extIdent);

   UInt16 nameIndex = code.currSegment()->addNameConst(name, code.msa());
   UInt32 rackPos;

   const struct ImportRecord* impRec = code.currSegment()->importRec(nameIndex);
   if (impRec == NULL) {

      //--- Allocate pointer to cell or label in rack on first use

      UInt size = sizeof(Ptr);

      code.padRack(size);
      rackPos = code.rackBytePos();
      code.reserveRack(size);

      code.currSegment()->addImportRec(nameIndex, rackPos, code.msa());
   }
   else
      rackPos = impRec->_offset;

   //   os << "@(";
   //   code.putIndAddrMode(0);
   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, rackPos, RACK_SECTION_ID, FALSE);
   os << "rack[" << rackPos << ']';
   //   os << ")[0]";
}

Void Const::genByteCodeData(ByteCode& code) const {
}

template<>
Void ConstT<Name>::genByteCode(ostream& os, ByteCode& code) const {
 
// Generate memory indirection to segment name table entry

   UInt index = code.currSegment()->addNameConst(_value, code.msa());
   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos,
                           sizeof(Bits16) + index * sizeof(Bits16),
                           NAME_TABLE_SECTION_ID,
                           FALSE);
   insert(os, code);
}

template<>
Void ConstT<Name>::genByteCodeData(ByteCode& code) const {
   UInt index = code.currSegment()->addNameConst(_value, code.msa());
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos,
                           sizeof(Bits16) + index * sizeof(Bits16),
                           NAME_TABLE_SECTION_ID,
                           FALSE);
}

template<>
Void ConstT<TypeConst>::genByteCode(ostream& os, ByteCode& code) const {

// Generate memory indirection to segment type table entry

   UInt index = code.currSegment()->addTypeConst(_value._type, code.msa());
   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos,
      sizeof(Bits16) + index * sizeof(Bits16),
      TYPE_TABLE_SECTION_ID,
      FALSE);
   insert(os, code);
}

template<>
Void ConstT<TypeConst>::genByteCodeData(ByteCode& code) const {
   UInt index = code.currSegment()->addTypeConst(_value._type, code.msa());
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos,
      sizeof(Bits16) + index * sizeof(Bits16),
      TYPE_TABLE_SECTION_ID,
      FALSE);
}

template<>
Void ConstT<Bits>::genByteCode(ostream& os, ByteCode& code) const {
   code.putImmUInt(_value);
   insert(os, code);
}

template<>
Void ConstT<Bits>::genByteCodeData(ByteCode& code) const {
   code.putBits32((Bits32)_value);
}

template<>
Void ConstT<Int>::genByteCode(ostream& os, ByteCode& code) const {
   code.putImmInt(_value);
   insert(os, code);
}

template<>
Void ConstT<Int>::genByteCodeData(ByteCode& code) const {
   code.putBits32((Bits32)_value);
}

// When compiling with GCC, the order in which the next two
// functions are defined is critical!

template<>
Void ConstT<Double>::genByteCodeData(ByteCode& code) const {
   double d = _value;

#if (BIG_ENDIAN==0)
   byteSwap(&d, sizeof(double));
#endif

   code.putBytes((Byte*)&d, sizeof(double));
}

template<>
Void ConstT<Double>::genByteCode(ostream& os, ByteCode& code) const {
   code.putByte(IMM_8_EXT_ADDR_MODE_VAL);
   genByteCodeData(code);
   insert(os, code);
}

template<>
Void ConstT<Char>::genByteCode(ostream& os, ByteCode& code) const {
   code.putByte(IMM_1_EXT_ADDR_MODE_VAL);
   code.putByte(_value);
   insert(os, code);
}

template<>
Void ConstT<Char>::genByteCodeData(ByteCode& code) const {
   code.putByte((Byte)_value);
}

template<>
Void ConstT<String>::genByteCode(ostream& os, ByteCode& code) const {
   ByteCodeSection* section = code.selectDataSection();
   UInt dataPos = code.bytePos();
   code.putBytes((Byte*)_value, strlen(_value) + 1);
   code.selectSection(section);

   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, dataPos, DATA_SECTION_ID, FALSE);
   insert(os, code);
}

template<>
Void ConstT<String>::genByteCodeData(ByteCode& code) const {
   code.putBytes((Byte*)_value, strlen(_value) + 1);
}

template<>
Void ConstT<Tag>::genByteCode(ostream& os, ByteCode& code) const {
   code.putImmUInt(_value);
   insert(os, code);
}

template<>
Void ConstT<Tag>::genByteCodeData(ByteCode& code) const {
   code.putBits16((Bits16)_value);
}

// Const_codeEnv::genByteCode: Generate byte code for code environment

Void Const_codeEnv::genByteCode(ostream& os, ByteCode& code) const {
   code.putByte(MEM_4_EXT_ADDR_MODE_VAL);
   UInt pos = code.bytePos();
   code.putBits32(0);
   code.addTargetBackPatch(pos, 0, RACK_SECTION_ID, FALSE);
   os << "codeEnv";
}

// Const_consEnv::genByteCode: Generate byte code for consEnv register

Void Const_consEnv::genByteCode(ostream& os, ByteCode& code) const {
   code.reg(REG_consEnv).genByteCode(0, os, code);
}

Void Const_NULL_REF::genByteCode(ostream& os, ByteCode& code) const {
   code.putImmUInt(0);
   os << '#' << "NULL_REF";
}

Void Const_NULL_Ptr::genByteCode(ostream& os, ByteCode& code) const {
   code.putImmUInt(0);
   os << "#NULL";
}

Void Const_NULL_Env::genByteCode(ostream& os, ByteCode& code) const {
   code.putImmUInt(0);
   os << "#NULL";
}

Void LocOperand::genByteCode(ostream& os, ByteCode& code) const {
   _loc.genByteCode(0, os, code);
}

Byte Operator::byteCode(Bool constOperand) const {
   switch (_kind) {
      case FROM_THUNK_OP:                    return EFC_INS;
      case ENTER_SPECIAL_EXPR_OP:            return ENT_E_INS;
      case UPDATE_OP:                        return UPD_INS;
      case CELL_INFO_OF_OP:                  return CIO_INS;
      case SIZE_OF_OP:                       return SIZE_OF_INS;
      case TAG_OF_OP:                        return MOV_O_INS;
      case OBJECT_TYPE_OP:                   return OTY_INS;
      case SET_ENV_OP:                       return SET_ENV_INS;
      case MAP_OP:   switch (_type) {
                        case OP_TYPE_NAME:   return MAP_N_INS;
                        case OP_TYPE_TYPE:   return MAP_T_INS;
                        case OP_TYPE_CLOSURE:
                                             return MAP_CL_INS;
                        case OP_TYPE_EXPR:   return MAP_E_INS;
                        case OP_TYPE_STRING: return !constOperand
                                                ? MAP_S_INS
                                                : MAP_SC_INS;
                        default:             break;
                     }

/*
      case NOT:                              return ???;
*/
      case ADD_OP:   switch (_type) {
                        case OP_TYPE_INT:    return ADD_I_INS;
                        case OP_TYPE_FLOAT:  return ADD_F_INS;
                        case OP_TYPE_DOUBLE: return ADD_D_INS;
                        default:             break;
                     }
      case SUB_OP:   switch (_type) {
                        case OP_TYPE_INT:    return SUB_I_INS;
                        case OP_TYPE_FLOAT:  return SUB_F_INS;
                        case OP_TYPE_DOUBLE: return SUB_D_INS;
                        case OP_TYPE_UTC:    return SUB_U_INS;
                        default:             break;
                     }
      case MUL_OP:   switch (_type) {
                        case OP_TYPE_INT:    return MUL_I_INS;
                        case OP_TYPE_FLOAT:  return MUL_F_INS;
                        case OP_TYPE_DOUBLE: return MUL_D_INS;
                        default:             break;
                     }
      case DIV_OP:   switch (_type) {
                        case OP_TYPE_INT:    return DIV_I_INS;
                        case OP_TYPE_FLOAT:  return DIV_F_INS;
                        case OP_TYPE_DOUBLE: return DIV_D_INS;
                        default:             break;
                     }
      case MOD_OP:   switch (_type) {
                        case OP_TYPE_INT:    return MOD_I_INS;
                        default:             break;
                     }
      case NEG_OP:   switch (_type) {
                        case OP_TYPE_INT:    return NEG_I_INS;
                        case OP_TYPE_FLOAT:  return NEG_F_INS;
                        case OP_TYPE_DOUBLE: return NEG_D_INS;
                        default:             break;
                     }
      case FROM_INT_OP:
                     switch (_type) {
                        case OP_TYPE_BYTE:   return INT_B8_INS;
                        case OP_TYPE_BITS:   return INT_B_INS;
                        case OP_TYPE_FLOAT:  return INT_F_INS;
                        case OP_TYPE_DOUBLE: return INT_D_INS;
                        default:             break;
                     }
      case FROM_DOUBLE_OP:
                     switch (_type) {
                        case OP_TYPE_FLOAT:  return DBL_F_INS;
                        default:             break;
                     }
      case FROM_STRING_OP:
                     switch (_type) {
                        case OP_TYPE_INT:    return !constOperand ? STR_I_INS : STR_IC_INS;
                        case OP_TYPE_DOUBLE: return !constOperand ? STR_D_INS : STR_DC_INS;
                        default:             break;
                     }
      case AND_OP:   switch (_type) {
                        case OP_TYPE_BITS:   return AND_B_INS;
                        default:             break;
                     }
      case OR_OP:    switch (_type) {
                        case OP_TYPE_BITS:   return IOR_B_INS;
                        default:             break;
                     }
      case XOR_OP:   switch (_type) {
                        case OP_TYPE_BITS:   return XOR_B_INS;
                        default:             break;
                     }
      case L_SHIFT_OP:
                     switch (_type) {
                        case OP_TYPE_BITS:   return LSH_B_INS;
                        default:             break;
                     }
      case R_SHIFT_OP:
                     switch (_type) {
                        case OP_TYPE_BITS:   return RSH_B_INS;
                        default:             break;
                     }
      case NOT_OP:   switch (_type) {
                        case OP_TYPE_BITS:   return NOT_B_INS;
                        default:             break;
                     }
      case LT_OP:    switch (_type) {
                        case OP_TYPE_INT:    return CLT_I_INS;
                        case OP_TYPE_FLOAT:  return CLT_F_INS;
                        case OP_TYPE_DOUBLE: return CLT_D_INS;
                        case OP_TYPE_TAG:    return CLT_O_INS;
                        case OP_TYPE_CHAR:   return CLT_C_INS;
                        default:             break;
                     }
      case LTEQ_OP:  switch (_type) {
                        case OP_TYPE_INT:    return CLE_I_INS;
                        case OP_TYPE_FLOAT:  return CLE_F_INS;
                        case OP_TYPE_DOUBLE: return CLE_D_INS;
                        case OP_TYPE_TAG:    return CLE_O_INS;
                        case OP_TYPE_CHAR:   return CLE_C_INS;
                        default:             break;
                     }
      case EQ_OP:    switch (_type) {
                        case OP_TYPE_NAME:   return CEQ_N_INS;
                        case OP_TYPE_TYPE:   return CEQ_T_INS;
                        case OP_TYPE_INT:    return CEQ_I_INS;
                        case OP_TYPE_FLOAT:  return CEQ_F_INS;
                        case OP_TYPE_DOUBLE: return CEQ_D_INS;
                        case OP_TYPE_TAG:    return CEQ_O_INS;
                        case OP_TYPE_CHAR:   return CEQ_C_INS;
                        case OP_TYPE_PTR:    return CEQ_P_INS;
                        default:             break;
                     }
      case NEQ_OP:   switch (_type) {
                        case OP_TYPE_NAME:   return CNE_N_INS;
                        case OP_TYPE_TYPE:   return CNE_T_INS;
                        case OP_TYPE_INT:    return CNE_I_INS;
                        case OP_TYPE_FLOAT:  return CNE_F_INS;
                        case OP_TYPE_DOUBLE: return CNE_D_INS;
                        case OP_TYPE_TAG:    return CNE_O_INS;
                        case OP_TYPE_CHAR:   return CNE_C_INS;
                        case OP_TYPE_PTR:    return CNE_P_INS;
                        default:             break;
                     }
      case GTEQ_OP:  switch (_type) {
                        case OP_TYPE_INT:    return CGE_I_INS;
                        case OP_TYPE_FLOAT:  return CGE_F_INS;
                        case OP_TYPE_DOUBLE: return CGE_D_INS;
                        case OP_TYPE_TAG:    return CGE_O_INS;
                        case OP_TYPE_CHAR:   return CGE_C_INS;
                        default:             break;
                     }
      case GT_OP:    switch (_type) {
                        case OP_TYPE_INT:    return CGT_I_INS;
                        case OP_TYPE_FLOAT:  return CGT_F_INS;
                        case OP_TYPE_DOUBLE: return CGT_D_INS;
                        case OP_TYPE_TAG:    return CGT_O_INS;
                        case OP_TYPE_CHAR:   return CGT_C_INS;
                        default:             break;
                     }

      case LENGTH_STRING_OP:                 return !constOperand
                                                ? LEN_S_INS
                                                : LEN_SC_INS;
      case GET_AT_STRING_OP:                 return !constOperand
                                                ? GET_AT_S_INS
                                                : GET_AT_SC_INS;
      case PUT_AT_STRING_OP:                 if (!constOperand)
                                                return PUT_AT_S_INS;
                                             else {
                                                error("Invalid for constant string (putAtString)");
                                                return ILG_INS;
                                             }

      case ALLOC_PTR_ARRAY_OP:               return APA_INS;
      case COPY_PTR_ARRAY_OP:                return CPA_INS;
      case GET_AT_PTR_ARRAY_OP:
      case GET_AT_PLAIN_PTR_ARRAY_OP:
                                             return GPAV_INS;
      case PUT_AT_PTR_ARRAY_OP:
      case PUT_AT_PLAIN_PTR_ARRAY_OP:
                                             return PPAV_INS;

      case REV_REFS_OP:                      return RRL_INS;

#if (GARBAGE_COLLECTION==1)
      case MARK_PTR_GC_OP:                   return MRK_P_INS;
      case MARK_STRING_GC_OP:                return MRK_S_INS;
#endif

#if (SERIALISATION==1)
      case EXTRACT_OP:  switch (_type) {
                           case OP_TYPE_CLOSURE:
                                             return EXT_CL_INS;
                           case OP_TYPE_EXPR:
                                             return EXT_E_INS;
                           default:             break;
                        }
      case INSERT_OP:  switch (_type) {
                           case OP_TYPE_CLOSURE:
                                             return INS_CL_INS;
                           case OP_TYPE_EXPR:
                                             return INS_E_INS; 
                           default:             break;
                        }

#endif



      case ERROR_OP:                         return !constOperand
                                                ? ERR_INS
                                                : ECP_INS;
      case TRACE_OP:                         return !constOperand
                                                ? TRC_INS
                                                : TCP_INS;

      case UNKNOWN_OP:
      default:                               break;
   }
   return ILG_INS;
}

Byte Operator::branchByteCode(Void) const {
   switch (_kind) {
      case LT_OP:    switch (_type) {
                        case OP_TYPE_INT:    return BLT_I_INS;
                        case OP_TYPE_FLOAT:  return BLT_F_INS;
                        case OP_TYPE_DOUBLE: return BLT_D_INS;
                        case OP_TYPE_TAG:    return BLT_O_INS;
                        case OP_TYPE_CHAR:   return BLT_C_INS;
                        default:             break;
                     }

      case LTEQ_OP:  switch (_type) {
                        case OP_TYPE_INT:    return BLE_I_INS;
                        case OP_TYPE_DOUBLE: return BLE_D_INS;
                        case OP_TYPE_FLOAT:  return BLE_F_INS;
                        case OP_TYPE_CHAR:   return BLE_C_INS;
                        default:             break;
                     }

      case EQ_OP:    switch (_type) {
                        case OP_TYPE_PTR:    return BEQ_P_INS;
                        case OP_TYPE_NAME:   return BEQ_N_INS;
                        case OP_TYPE_TYPE:   return BEQ_T_INS;
                        case OP_TYPE_INT:    return BEQ_I_INS;
                        case OP_TYPE_DOUBLE: return BEQ_D_INS;
                        case OP_TYPE_FLOAT:  return BEQ_F_INS;
                        case OP_TYPE_TAG:    return BEQ_O_INS;
                        case OP_TYPE_CHAR:   return BEQ_C_INS;
                        default:             break;
                     }

      case NEQ_OP:   switch (_type) {
                        case OP_TYPE_PTR:    return BNE_P_INS;
                        case OP_TYPE_NAME:   return BNE_N_INS;
                        case OP_TYPE_TYPE:   return BNE_T_INS;
                        case OP_TYPE_INT:    return BNE_I_INS;
                        case OP_TYPE_FLOAT:  return BNE_F_INS;
                        case OP_TYPE_DOUBLE: return BNE_D_INS;
                        case OP_TYPE_TAG:    return BNE_O_INS;
                        case OP_TYPE_CHAR:   return BNE_C_INS;
                        default:             break;
                     }

      case GTEQ_OP:  switch (_type) {
                        case OP_TYPE_INT:    return BGE_I_INS;
                        case OP_TYPE_DOUBLE: return BGE_D_INS;
                        case OP_TYPE_FLOAT:  return BGE_F_INS;
                        case OP_TYPE_TAG:    return BGE_O_INS;
                        case OP_TYPE_CHAR:   return BGE_C_INS;
                        default:             break;
                     }

      case GT_OP:    switch (_type) {
                        case OP_TYPE_INT:    return BGT_I_INS;
                        case OP_TYPE_FLOAT:  return BGT_F_INS;
                        case OP_TYPE_DOUBLE: return BGT_D_INS;
                        case OP_TYPE_TAG:    return BGT_O_INS;
                        case OP_TYPE_CHAR:   return BGT_C_INS;
                        default:             break;
                     }

      case EQ_NULL_OP:
                     return BNP_INS;

      case NEQ_NULL_OP:
                     return BNN_INS;

      case SPECIAL_EXPR_OP:   
                     switch (_type) {
                        case OP_TYPE_EXPR:   return BSE_INS;
                        default:             break;
                  }

#if (GARBAGE_COLLECTION==1) 
      case MARK_PTR_GC_OP:                   return MRK_P_BS_INS;
      case NOT_MARK_PTR_GC_OP:               return MRK_P_BNS_INS;
      case MARK_EXPR_PTR_GC_OP:              return MRK_E_BS_INS;
      case NOT_MARK_EXPR_PTR_GC_OP:          return MRK_E_BNS_INS;
#endif

      default:       break;
   }
   return ILG_INS;
}

Void ByteCode::addBackPatch(BackPatchElement* element) {
   element->_next =_backPatchList;
   _backPatchList = element;
}

// General backpatch

Void ByteCode::addBackPatch(UInt pos,    ByteCodeSection& posSection,
                            UInt origin, ByteCodeSection* originSection,
                            UInt target, ByteCodeSection* targetSection,
                            Bool endOfInsFlag/* = TRUE */) {
   BackPatchElement* element = new (msa())
      SimpleBackPatchElement(pos, posSection,
                             origin, originSection,
                             target, targetSection);
   if (endOfInsFlag)
      _currentIns->addBackPatch(element);          // Origin is end of instruction
   else
      addBackPatch(element);
}

Void ByteCode::addLabelBackPatch(UInt pos, CodeLabel& codeLabel,
                                 Bool endOfInsFlag /* = TRUE */) {
   BackPatchElement* element = new (msa())
      LabelBackPatchElement(pos, *_currSection,
                             0, *(_codeSection != NULL ? _codeSection
                                                       : _currSection),
                             codeLabel, _branchBackPatchFlag);
 
   if (endOfInsFlag)
      _currentIns->addBackPatch(element);          // Origin is end of instruction
	else {
      element->_origin = _currSection->_bytePos;   // Origin is end of operand
      addBackPatch(element);
   }
}

Void ByteCode::addLabelBackPatch(UInt dataPos, ByteCodeSection& originSection,
                                 CodeLabel& codeLabel) {
   BackPatchElement* element = new (msa())
      LabelBackPatchElement(dataPos, *_currSection,
                            0, originSection, codeLabel);
   addBackPatch(element);
}

// Add backpatch to target in data section

Void ByteCode::addTargetBackPatch(UInt pos,
                                  UInt target, UInt targetSectionId,
                                  Bool endOfInsFlag/* = TRUE */) {
   SimpleBackPatchElement* element = new (msa())
      SimpleBackPatchElement(pos,      *_currSection,
                             0,        _currSection,
                             target,   _currSegment->getSection(targetSectionId));
   if (endOfInsFlag)
      _currentIns->addBackPatch(element);          // Origin is end of instruction
   else {
      element->_origin = _currSection->_bytePos;   // Origin is end of operand
      addBackPatch(element);
   }
}

// Add backpatch for segment address

Void ByteCode::addSegmentBackPatch(UInt pos, Lambda& lambda,
                                   Bool endOfInsFlag /* = TRUE */) {
   BackPatchElement* element = new (msa())
      SegmentBackPatchElement(pos, *_currSection, lambda);
   element->_originSection = _currSection;
   if (endOfInsFlag)
      _currentIns->addBackPatch(element);          // Origin is end of instruction
   else {
      element->_origin = _currSection->_bytePos;   // Origin is end of operand
      addBackPatch(element);
   }
}

Void Instruction::addBackPatch(BackPatchElement* element) {
   element->_next = _backPatchList;
   _backPatchList = element;
}

BackPatchElement::BackPatchElement(UInt pos,    ByteCodeSection& posSection,
                                   UInt origin, ByteCodeSection* originSection)
 : _next(NULL),
   _pos(pos),        _posSection(posSection),
   _origin(origin),  _originSection(originSection) {
}

UInt BackPatchElement::origin(Void) const {
   return _origin +
      (_originSection != NULL ? _originSection->origin()
                               : 0);
}

Bool BackPatchElement::branchCrossesSegments(Void) const {
   return FALSE;
}

SimpleBackPatchElement::SimpleBackPatchElement(UInt pos, ByteCodeSection& posSection,
   UInt origin, ByteCodeSection* originSection,
   UInt target, ByteCodeSection* targetSection)
   : BackPatchElement(pos, posSection, origin, originSection),
   _binding(target), _bindingSection(targetSection) {
}

UInt SimpleBackPatchElement::target(Void) const {
   return _binding +
      (_bindingSection != NULL ? _bindingSection->origin()
                               : 0);
}



LabelBackPatchElement::LabelBackPatchElement(UInt pos, ByteCodeSection& posSection,
                                              UInt origin, ByteCodeSection& originSection,
                                              CodeLabel& codeLabel, Bool branchFlag)
 : BackPatchElement(pos, posSection, origin, &originSection),
    _codeLabel(codeLabel), _branchFlag(branchFlag) {
}

UInt LabelBackPatchElement::target(Void) const {
   return _codeLabel._bytePos + _codeLabel._section->origin();
}

Bool LabelBackPatchElement::branchCrossesSegments(Void) const {
   return _branchFlag &&
      _originSection != NULL && _originSection->segment() != NULL &&
      _codeLabel._section != NULL && _codeLabel._section->segment() != NULL &&
      _originSection->segment() != _codeLabel._section->segment();
}



SegmentBackPatchElement::SegmentBackPatchElement(UInt pos, ByteCodeSection& posSection,
                                                 Lambda& lambda)
   : BackPatchElement(pos, posSection), _lambda(lambda) {
}

UInt SegmentBackPatchElement::target(Void) const {
   return _lambda._segment->getSection(SEGMENT_HEADER_SECTION_ID)->origin();
}



UInt ByteCode::bytePos(Void) const {
   return _currSection->bytePos();
}

Void ByteCode::reserveBytes(UInt size, UInt sectionId) {
   _currSegment->getSection(sectionId)->reserve(size, msa());
}

Void ByteCode::reserveBytes(UInt size) {
   _currSection->reserve(size, msa());
}

UInt ByteCode::rackBytePos(Void) const {
   return _currSegment->getSection(RACK_SECTION_ID)->bytePos();
}

Void ByteCode::padRack(UInt size) {
   _currSegment->getSection(RACK_SECTION_ID)->pad(size, msa());
}

Void ByteCode::reserveRack(UInt size) {
   _currSegment->getSection(RACK_SECTION_ID)->reserve(size, msa());
}



Void ByteCode::padData(UInt size) {
   _currSegment->getSection(DATA_SECTION_ID)->pad(size, msa());
}

Void ByteCode::reserveData(UInt size) {
   _currSegment->getSection(DATA_SECTION_ID)->reserve(size, msa());
}


ByteCodeSection* ByteCode::section(UInt id) {
   return _currSegment->getSection(id);
}

ByteCodeSection* ByteCode::selectSection(UInt id) {
   ByteCodeSection* section = _currSection;
   _currSection = _currSegment->getSection(id);
   return section;
}

ByteCodeSection* ByteCode::selectHeaderSection(Void) {
   return selectSection(SEGMENT_HEADER_SECTION_ID);
}

ByteCodeSection* ByteCode::selectDataSection(Void) {
   return selectSection(DATA_SECTION_ID);
}

ByteCodeSection* ByteCode::selectInitCodeSection(Void) {
   return selectSection(INITIALISATION_CODE_SECTION_ID);
}

IAddress ByteCode::codeSegment(Void) const {
   return _rootSegment->getSection(SEGMENT_HEADER_SECTION_ID)->_bytePtr;
}

UInt ByteCode::rackOrigin(Void) const {
   return _rootSegment->getSection(RACK_SECTION_ID)->origin();
}

IAddress ByteCode::mainEntry(Void) const {
   return _entrySection->_bytePtr + _entryPos;
}

Void ByteCode::saveEntryPoint(Void) {
   _entrySection = _currSection;
   _entryPos = _currSection->bytePos();
}
