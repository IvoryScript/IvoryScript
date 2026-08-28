/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    sourceCode.cpp
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
 *    IvoryScript source code generation
 *		Generates C++ source code from the intermediate code.
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

#include "OptionsParser.h"
#include <string.h>
#include "ivory/repr.h"
#include "ivory/trace.h"
#include "ivory/compiler/sourceCode.h"
#include "ivory/compiler/tran.h"
#include "ivory/compiler/type.h"

#if (GARBAGE_COLLECTION==1)
#include "ivory/frameDescr.h"
#endif

#define TRACE

declareType(ByteString);

declareType(Expr);

declareType(NameAnyBinding);

declareType(Int);

declareType(Label);

declareType(ExprList);

declareType(Pair);


declareType(AddPropertyEvent);
declareType(UpdatePropertyEvent);
declareType(AddRefEvent);
declareType(RemoveRefEvent);
declareType(DestroyObjectEvent);
declareType(RefList);
declareType(ReadyToSendEvent);

declareTypeCon(ByteString);

#define TRACE

#ifdef TRACE
static Bool traceFlag = FALSE;
#define toggle(flag) ((flag)=!(flag))
static Int instructionProbeSeqNo = -1;
#endif


SourceCodeLabelElement* SourceCodeLabelSet::addElement(CodeLabel& codeLabel,
                                                       MSA& msa) {
   SourceCodeLabelElement** elementPtr = &_elements;;

   while (*elementPtr != NULL) {
      if (&(*elementPtr)->_codeLabel == &codeLabel)
         return *elementPtr;
      elementPtr = &(*elementPtr)->_next;
   }
   SourceCodeLabelElement* element = new (msa)
      SourceCodeLabelElement(NULL, codeLabel);
   return (*elementPtr = element);
}


SourceCodeLabelElement* SourceCodeLabelSet::findElement(const CodeLabel& codeLabel) const {
   SourceCodeLabelElement* element = _elements;

   while (element != NULL) {
      if (&element->_codeLabel == &codeLabel)
         return element;
      element = element->_next;
   }
   return NULL;
}

Void SourceCodeLabelSet::removeElement(const CodeLabel& codeLabel, MSA& msa) {
   SourceCodeLabelElement** elementPtr = &_elements;

   while (*elementPtr != NULL) {
      if (&(*elementPtr)->_codeLabel == &codeLabel) {
         SourceCodeLabelElement* element = *elementPtr;
         *elementPtr = (*elementPtr)->_next;
         msa.free(element);
         return;
      }
      elementPtr = &(*elementPtr)->_next;
   }
}

Void SourceCodeLabelSet::merge(const SourceCodeLabelSet& sourceCodeLabelSet,
                               MSA& msa) {
   SourceCodeLabelElement* element = sourceCodeLabelSet._elements;

   while (element != NULL) {
      if (findElement(element->_codeLabel) == NULL)
         addElement(element->_codeLabel, msa);
      element = element->_next;
   }
}


SourceCode::SourceCode(ConstString options, Env& env, MSA& msa)
   : ICode(options, env, msa), _nativeLocals(NULL), _inBody(FALSE), _sourceCodeBlock(NULL)  {
}

Bool SourceCode::builtInNameComp(Expr x, Expr y) const {
//   return FALSE;
   return isPair(x) && fst(x) == Expr(LITERAL) ||
          isPair(y) && fst(y) == Expr(LITERAL);
}

Bool SourceCode::builtInTypeComp(Expr x, Expr y) const {
//   return FALSE;
   return isPair(x) && fst(x) == Expr(LITERAL) ||
          isPair(y) && fst(y) == Expr(LITERAL);
}

Void SourceCode::genTarget(ostream& os) {

#ifdef TRACE
   OptionsParser options(_options);
   if (options.hasKey("source")) {
      traceFlag = options.hasKey("trace");
      options.getNum("probe", instructionProbeSeqNo);
}
   else
      traceFlag = FALSE;
#endif

   if (_moduleDefn != NULL)
      addNameConst(_moduleDefn->name());

   init(TRUE);
   do
   {
      headCodeLabels();
   } while (flagIndirectContinuations());

   unsigned char buffer[1024];
   size_t len = 0;
   for (UInt i = 0; i < _nTypeConsts; i++)
   {
/*
      printTypeSig(typeSignature(_typeConstV.get(i), typeTable()), 0, FALSE, outStream, env());
      outStream << '\n';
      outStream.flush();
*/
      len += packTypeSig(typeSignature(_typeConstV.get(i), typeTable()),
                         buffer + len, 1024 - len, *this);
   }

   if (_nNameConsts > 0)   // Superfluous test
   {
      os << "static Name nameArr$[" << _nNameConsts << "];\n";

      os << "#define stringifiers$ \\\n\"";
      UInt pos = 1;
      for (UInt i = 0; i < _nNameConsts; i++)
      {
         Name name = _nameConstV.get(i);
         if (name >= 0)
         {
            const char* s = nameString_(_nameConstV.get(i), nameTable());
            size_t len = strlen(s) + 2;
            if (pos + len > 80)
            {
               os << "\"\\\n\"";
               pos = 1;
            }
            os << s << "\\0";
            pos += len;
         }
      }
      os << "\"\n";
   }

   if (_nTypeConsts > 0)
   {
      os << "static unsigned char typeSigs$[]={\n";
      for (UInt i = 0; i < len; i++)
      {
         if (i > 0)
            os << ',';
         if (i != 0 && i % 16 == 0)
            os << '\n';
         os << "0x" << hex << (unsigned int)buffer[i] << dec;
      }
      os << "};\n";
      os << "static Type typeArr$[" << _nTypeConsts << "];\n";
   }

   os << "static Module module(" <<
      (_nNameConsts > 0 ? "stringifiers$,nameArr$"
                        : "NULL,NULL") << ',' <<
      (_nTypeConsts > 0 ? "typeSigs$"
                        : "NULL") << ',' <<
      (_nTypeConsts > 0 ? len
                        : 0) << ',' <<
      (_nTypeConsts > 0 ? "typeArr$"
                        : "NULL") << ");\n";

   declareLabels(os);

   genLocCode();

   declareStructures(os);

   const Instruction* ins = _instrSeq->first();
   while (ins != NULL) {

#ifdef TRACE
      if (traceFlag) {
         outStream << "Generating source code for instruction " << ins->_seqNo << '\n';
         outStream.flush();
      }
      if (ins->_seqNo == instructionProbeSeqNo) {
         outStream << "sourceCode::genTarget: probe hit\n";
         outStream.flush();
         traceFlag = TRUE;
      }
#endif

      ins->genSourceCode(os, *this);
      os.flush();
      ins = ins->_next;
   }

   if (_inBody)
      endBody(os);
}

Void SourceCode::printPtrRepr(const PtrRepr& ptrRepr, ostream& os) const {
   printRepr(ptrRepr.repr(), os);
   os << '*';
}

Void SourceCode::printStructRepr(const StructRepr& structRepr, Bool full, ostream& os) const {
   os << "struct S" << structRepr.id();
   if (full && structRepr.id() > 0) {
      os  << " {\n";
      for (UInt i = 0; i < structRepr.nReprs(); i++) {
         Repr repr = structRepr.reprV()[i];
         printRepr(repr, os);
         os << " c" << i + 1 << ";\n";
      }
      os << "};\n";
   }
}

Void SourceCode::printUnionRepr(const UnionRepr& unionRepr, ostream& os) const {
   os << "union S" << unionRepr.id() << " {\n";
   for (UInt i = 0; i < unionRepr.nReprs(); i++) {
      Repr repr = unionRepr.reprV()[i];
      printRepr(repr, os);
      os << " c" << i + 1 << ";\n";
   }
   os << "};\n";
}


Void SourceCode::printRepr(Repr repr, ostream& os) const {
   switch (repr.tag()) {
      case Repr::REPR_UNKNOWN:
         os << "Unknown";
         break;

      case Repr::REPR_VOID:
         os << "Void";
         break;

      case Repr::REPR_NAME:
         os << "Name";
         break;

      case Repr::REPR_TYPE:
         os << "Type";
         break;

      case Repr::REPR_CHAR:
         os << "Char";
         break;

      case Repr::REPR_INT:
         os << "Int";
         break;

      case Repr::REPR_DOUBLE:
         os << "Double";
         break;

      case Repr::REPR_FLOAT:
         os << "Float";
         break;

      case Repr::REPR_REF:
         os << "Ref";
         break;

      case Repr::REPR_STRING:
         os << "String";
         break;

      case Repr::REPR_TAG:
         os << "Tag";
         break;

      case Repr::REPR_ENV_PTR:
         os << "Env*";
         break;

      case Repr::REPR_LABEL:
         os << "Label";
         break;

      case Repr::REPR_EXPR:
         os << "Expr";
         break;

      case Repr::REPR_CELL_PTR:
         os << "Cell*";
         break;

      case Repr::REPR_CELL_INFO_PTR:
         os << "CellInfo*";
         break;

      case Repr::REPR_UTC:
         os << "UTC";
         break;

      case Repr::REPR_ARRAY:
         os << "Array";
         break;

      case Repr::REPR_SERIAL_CONTEXT:
         os << "SerialContext*";
         break;

      case Repr::REPR_PTR: {
 /*        PtrRepr* ptrRepr = repr.ptrRepr();
         if (ptrRepr != NULL)
            printPtrRepr(*repr.ptrRepr(), os);
         else */
            os << "Void*";
         break;
      }

      case Repr::REPR_STRUCT: {
         printStructRepr(*repr.structRepr(), FALSE, os);
         break;
      }
      case Repr::REPR_UNION: {
         printUnionRepr(*repr.unionRepr(), os);
         break;
      }

      default:
         assert(FALSE, "SourceCode::printRepr: Invalid representation");
         break;
   }
}

Void SourceCode::declareStructure(StructRepr& structRepr, UInt& structN, ostream& os) {
   for (UInt i = 0; i < structRepr.nReprs(); i++) {
      Repr repr = structRepr.reprV()[i];
      if (repr.tag() == Repr::REPR_STRUCT)
         declareStructure(*repr.structRepr(), structN, os);


   }
      
   structRepr.id() = ++structN;

   printStructRepr(structRepr, TRUE, os);
}

Void SourceCode::declareStructures(ostream& os)  {
   ReprElement* reprElement = _reprList;
   UInt ptrN =    0;
   UInt structN = 0;
   UInt unionN =  0;
   while (reprElement != NULL) {
      Bool toPrint = FALSE;
      switch (reprElement->repr().tag()) {
         case Repr::REPR_PTR: {
            PtrRepr* ptrRepr = reprElement->repr().ptrRepr();
            if (ptrRepr != NULL)
               ptrRepr->id() = ++ptrN;
            break;
         }

         case Repr::REPR_STRUCT:
            declareStructure(*reprElement->repr().structRepr(), structN, os);
            break;

         case Repr::REPR_UNION: {
            toPrint = FALSE;
            reprElement->repr().unionRepr()->id() = ++unionN;
            break;
         }

         default:

            break;
      }
 
      reprElement = reprElement->next();
   }
}

Void SourceCode::declareLabels(ostream& os) {
   const Instruction* ins = _instrSeq->first();
   UInt count = 0;
   while (ins != NULL) {
      if (ins->_kind == Instruction::LABEL_INSTR) {
         CodeLabel& codeLabel = ((LabelInstruction&)*ins)._codeLabel;
         if (codeLabel._head)
         {
            os << "declareLabel";
            genLabel(codeLabel, os);
            os << ";\n";
            count++;
         }
      }
      ins = ins->_next;
   }
   if (count > 0)
      os << '\n';
}

Void SourceCode::genLabel(const CodeLabel& codeLabel, ostream& os,
                          Bool modPrefixFlag/* = TRUE*/,
                          Bool parenFlag/* = TRUE*/,
                          Bool altFlag/* = FALSE*/) const
{
   if (parenFlag)
      os << '(';
   if (modPrefixFlag && _moduleDefn != NULL) {
      printName(_moduleDefn->name(), os, nameTable());
      os << '_';
   }
   codeLabel.insert(os, *this);
   if (altFlag)
      os << '$';
   if (parenFlag)
      os << ')';
}

NativeLocal& SourceCode::allocNativeLocal(Repr repr) {
   NativeLocal& loc = *new (_msa) NativeLocal(repr, _nativeLocals);
   _nativeLocals = &loc;
   return loc;
}

Void SourceCode::flushNativeLocals(Void) {
   NativeLocal* loc = _nativeLocals;
   while (loc != NULL) {
      NativeLocal* next = (NativeLocal*)loc->next();
      if (loc->inUse()) {
         if (loc->requiresFlush())
            saveLoc(*loc);
         loc->empty(msa());
      }
      loc = next;
   }
}

Void SourceCode::flushNonStack(Void) {
   ICode::flushNonStack();
   flushNativeLocals();
}

// Predicate to test whether code label is local

Bool SourceCode::localLabel(const CodeLabel& codeLabel) const {
   return !codeLabel._head;
}

Void SourceCode::endBody(ostream& os)
{
   if (_inBody)
   {
      os << "}\n\n";
      _inBody = false;
   }
}

Bool SourceCode::insertJumpsToHeadLabels(Void) const {
   return TRUE;
}

Bool SourceCode::isBlockLabel(const CodeLabel& codeLabel) const {
   return _sourceCodeBlock->_labels.findElement(codeLabel) != NULL;
}

Bool SourceCode::isOuterLabel(const CodeLabel& codeLabel,
                              const SourceCodeBlock& sourceCodeBlock) const {
   return sourceCodeBlock._outer == NULL
      ? FALSE
      : (sourceCodeBlock._outer->_labels.findElement(codeLabel) != NULL
            ? TRUE
            : isOuterLabel(codeLabel, *sourceCodeBlock._outer));
}

Void SourceCode::removeBlockLabel(CodeLabel& codeLabel) {
   _sourceCodeBlock->_labels.removeElement(codeLabel, msa());
}

Bool SourceCode::anyForwardLabels(Void) const {
   return !_sourceCodeBlock->_labels.isEmpty();
}

Repr SourceCode::labelRepr(Bool simple/* = FALSE*/) const {
   return Repr::REPR_LABEL;
}

Void SourceCode::genJump(Operand& target, ostream& os) {
   os << "jump(";
   target.genSourceCode(os, *this);
   os << ");\n";
}

Void SourceCode::genGoTo(Operand& target, ostream& os) {
   assert(target._kind == Operand::LABEL_OPERAND,
          "SourceCode::genGoTo not label operand"); {
      CodeLabel& codeLabel = ((LabelOperand&)target)._codeLabel;
      if (_labelSet.findElement(codeLabel) == NULL)
         _sourceCodeBlock->_labels.addElement(codeLabel, msa());
      os << "goto ";
      genLabel(codeLabel, os, FALSE, FALSE);
      os << ";\n";
   }
}

Void SourceCode::createBlock(ostream& os) {
   _sourceCodeBlock = new(msa()) SourceCodeBlock(_sourceCodeBlock);
   os << '{';
}

Void SourceCode::destroyBlock(ostream& os)
{
   _sourceCodeBlock->_outer->_labels.merge(_sourceCodeBlock->_labels, msa());
   _sourceCodeBlock = _sourceCodeBlock->_outer;
   os << '}';
}

// Predicate to test whether a variable is local to head code label

Bool BasicBlk::localVar(const Var& var) {
   VarElement* varElement = _vars.findElement(var);
   BasicBlkElement* basicBlkElement = _successors.head();
   CodeLabel* headCodeLabel = ((LabelInstruction&)*_firstIns).codeLabel()._headCodeLabel;
   while (basicBlkElement != NULL) {
      BasicBlk& basicBlk = basicBlkElement->basicBlk();
      CodeLabel& codeLabel = ((LabelInstruction&)*basicBlk._firstIns).codeLabel();
      if (codeLabel._head || codeLabel._headCodeLabel != headCodeLabel) {
         varElement = basicBlk._vars.findElement(var);
         if (varElement != NULL && varElement->testFlags(VarElement::I))
            return FALSE;
      }
      basicBlkElement = basicBlkElement->_next;
   }
   return TRUE;
}

Void InstructionTrait::genSourceCode(ostream& os, SourceCode& code) const {
}

Void TypeDescrInstruction::genSourceCode(ostream& os, SourceCode& code) const {
}

Void ReserveInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _x->genDstSourceCode(os, code);
   os << ";\n";
}

Void EntryInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   code.endBody(os);
   os << "defineBuiltInFnExt(";
   printTranName(_name, os, code.env());
   os << ',';
   os << "nameArr$[" << code.nameConstIndex(_name);
   os << "],";
   os << "typeArr$[" << code.typeConstIndex(_type);
   os << "]";
   os << ");\n\n";
}

Void LabelInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   if (_codeLabel._head) {
      code.endBody(os);
      os << "defineLabel";
      code.genLabel(_codeLabel, os);
      os << " {\n";
      code._labelSet.setEmpty();
      code._sourceCodeBlock = new(code.msa()) SourceCodeBlock(NULL);
   }
   else {
      code._labelSet.addElement(_codeLabel, code.msa());
      while (TRUE) {
         if (code.isBlockLabel(_codeLabel))
            code.removeBlockLabel(_codeLabel);
         if (!code.isOuterLabel(_codeLabel, *code._sourceCodeBlock))
            break;
         code.destroyBlock(os);
      }
      code.genLabel(_codeLabel, os, FALSE, FALSE);
      os << ":\n";
   }

   if (_codeLabel.withBasicBlock()) {
      code._sp = _codeLabel.basicBlk()._sp;
      code._inBody = TRUE;
      code._basicBlk = &_codeLabel.basicBlk();
   }
}

Void GoToInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   if (target().kind() == Operand::LABEL_OPERAND) {
      CodeLabel& codeLabel = ((LabelOperand&)*_target)._codeLabel;
      if (code.localLabel(codeLabel)) {
         code.genGoTo(target(), os);
         return;
      }
   }
   code.genJump(target(), os);
}

Void MoveInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = ";
   _src->genSourceCode(os, code);
   os << ";\n";
}

Void LVP_Instruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = &(";
   _src->genSourceCode(os, code);
   os << ");\n";
}

Void Decons_S_Instruction::genSourceCode(ostream& os, SourceCode& code) const {
//   assert(FALSE, "DeconstructInstruction::genSourceCode: T.B.D.");

   outStream << "DeconstructInstruction::genSourceCode: T.B.D.";
}

Void ConditionalInstruction::genSourceCode(ostream& os, SourceCode& code) const
{
   os << "if(";
   _x->genSourceCode(os, code);
   os << _op.sourceSymbol();
   if (_y != NULL)
      _y->genSourceCode(os, code);
   os << ')';
   if (code.localLabel(_z->_codeLabel))
       code.genGoTo(*_z, os);
   else
      code.genJump(*_z, os);
}

Void MonadicOpActionInstruction::genSourceCode(ostream& os, SourceCode& code) const {

#if (GARBAGE_COLLECTION==1)
   if (_op.kind() != Operator::MARK_STRING_GC_OP) {
      os << _op.sourceSymbol();
      os << '(';
      _x->genSourceCode(os, code);
   }
   else {
#endif

      os << "if (&";
      _x_env->genSourceCode(os, code);
      os << "->msa() == consMSA)\n";
         os << "   (Void)((MSA_GC*)consMSA)->mark(";
      _x->genSourceCode(os, code);

#if (GARBAGE_COLLECTION==1)
   }
#endif

   os << ");\n";
}

Void MonadicOpInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = ";
   if (_op.kind() == Operator::LENGTH_STRING_OP) {
      os << "strlen(";
      _src->genSourceCode(os, code);
      os << ')'; 
   } else if (_op.kind() == Operator::ALLOC_PTR_ARRAY_OP) {
       size_t size = sizeOfRepr(_dst->repr(code).ptrRepr()->repr().arrayRepr()->elementRepr());
      os << "consMSA->alloc(" << size << '*';
      _src->genSourceCode(os, code);
      os << ')';
   } else if (_op.kind() == Operator::REV_REFS_OP) {
      os << " = &lockObject(";
      os << ",*(ADSInstance*)";
      os << ")->_revRefs";
   } else if (_op.kind() == Operator::CELL_INFO_OF_OP) {
      os << "(CellInfo*)(";
      _src->genSourceCode(os, code);
      os << ")->tag()";
   } else { 
      os << _op.sourceSymbol();
      if (_op.kind() == Operator::TYPE_OF_OP ||
         _op.kind() == Operator::OBJECT_TYPE_OP)
         os << '(';
      _src->genSourceCode(os, code);
   }
   os << ";\n";
}

Void DyadicOpInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = ";
   if (_op.kind() == Operator::MAP_OP) {
      os << " mapClosure(";
      _x->genSourceCode(os, code);
      os << ", ";
      _x_env->genSourceCode(os, code);
      os << ", ";
      _y->genSourceCode(os, code);
      os << " n_vm)";
   }
   else if (_op.kind() == Operator::GET_AT_STRING_OP) {
      os << "(";
      _x->genSourceCode(os, code);
      os << ")[";
      _y->genSourceCode(os, code);
      os << ']';
   } else {
      _x->genSourceCode(os, code);
      os << ' ' << _op.sourceSymbol() << ' ';
      _y->genSourceCode(os, code);
   }
   os << ";\n";
}

Void DyadicOpActionInstruction::genSourceCode(ostream& os, SourceCode& code) const {
/* ???
   if (_op.kind() == Operator::INSERT_OP) {
      os << "insertBinExpr(";
      _x->genSourceCode(os, code);
      os << ", ";  _x_env->genSourceCode(os, code);
      os << ", ";  _y->genSourceCode(os, code);
      os << ", ";  _y_env->genSourceCode(os, code);
      os << " n_vm);\n";
   } else
      assert(FALSE, "DyadicOpActionInstruction::genSourceCode: unexpected operator");
*/
}

Void TriadicOpActionInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   if (_op.kind() == Operator::PUT_AT_STRING_OP) {
      os << '(';
      _x->genSourceCode(os, code);
      os << ")[";
      _y->genSourceCode(os, code);
      os << " = ";
      _z->genSourceCode(os, code);
   }
   else
     assert(FALSE, "TriadicOpActionInstruction::genSourceCode: unexpected operator");
   os << ";\n";
}

Void CaseInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   Bool multiWayIfFlag = _x->repr(code) == Repr::REPR_TYPE;
   if (!multiWayIfFlag) {
      os << "switch(";
      _x->genSourceCode(os, code);
      os << "){\n";
   }

   for (UInt i = 0; i < _n; i++) {
      if (multiWayIfFlag) {
         os << "if (";
         _x->genSourceCode(os, code);
         os << " == ";
      } else 
         os << "case ";
      _consts[i]->genSourceCode(os, code);
      if (multiWayIfFlag)
         os << ')';
      else
         os << ":\n";

      CodeLabel& codeLabel = _labels[i]->_codeLabel;
      if (code.localLabel(codeLabel))
         code.genGoTo(*_labels[i], os);
      else
         code.genJump(*_labels[i], os);
   }
   if (_otherwise != NULL)
   {
      if (!multiWayIfFlag)
         os << "default:\n";
      if (code.localLabel(_otherwise->_codeLabel))
         code.genGoTo(*_otherwise, os);
      else
         code.genJump(*_otherwise, os);
   }
   if (!multiWayIfFlag)
      os << "}\n";
}


Void ArgCheckInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << "T.B.D\n";
}



Void CellInfoInstruction::genSourceCode(ostream& os, SourceCode& code) const {
//   code.endBody(os);
   os << "static CellInfo cellInfo" << _cellInfo._seqNo << "(label";
   code.genLabel(_cellInfo._entryLabel, os);
   os << ", ";
   code.genLabel(_cellInfo._altEntryLabel, os);
   os << ", CLOSURE);\n";   
}



#if (GARBAGE_COLLECTION==1)
Void FrameDescrInstruction::genSourceCode(ostream& os, SourceCode& code) const {
/*
   for (UInt i = 0; i < _frameDescr.nSlots(); i++) {
      os << "static BuiltInFrameDescrSlot slot" << i + 1;

      const FrameDescrSlot<CodeLabel*>& slot = _frameDescr.slot(i);
      slot.address()->genSourceCode(os, code);
      os << ", builtInName(";
      os << nameString(slot.name(), code.env().nameTable());
      os << ", builtInType(";
      os << "));\n";
   };
   os << "static BuiltInFrameDescrSlot slots[" << _frameDescr.nSlots() << "] = {";
   for (UInt i = 0; i < _frameDescr.nSlots(); i++) {
      if (i > 0)
         os << ", ";
      os << "slot" << i;
   };
   os << "}\n";
   os << "static BuiltInFrameDescr frameDescr(altEntry(concatString), " << code.sd_();
   os << ", " << _frameDescr.nSlots() << ", slots)\n";
*/
}
#endif

Void HeapAllocInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = (";
//   _dst->printRepr(os, code.env());
   os << ')';
   os << "consMSA->alloc(_size));\n";
}

Void CellAllocInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = new(";
   if (_structTemplate.nSlots() > 0)
      os << "sizeof(struct S" << _structTemplate.repr().id() << ')';
   else
      os << '0';
   os << ", *consMSA) Cell(";
   if (_cellInfo != NULL)
      _cellInfo->genSourceCode(os, code);
   os << ");\n";
}


Void IndirectionAllocInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = allocIndirection(NULL, *consMSA);\n";
}
Void PtrInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = consMSA->alloc(" << _size << ");\n";
}

Void TagInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = ";;
   switch (_srcRepr.tag()) {
      case Repr::REPR_NAME:
         os << ",fromName(";
        _src->genSourceCode(os, code);
         os << "))";
         break;

      case Repr::REPR_TYPE:
         os << ",fromType(";
        _src->genSourceCode(os, code);
         os << "))";
         break;

      case Repr::REPR_INT:
         os << "fromInt(";
        _src->genSourceCode(os, code);
         os << ",*consMSA))";
         break;

      case Repr::REPR_TAG:
         os << "fromTag(";
        _src->genSourceCode(os, code);
         os << ",*consMSA))";
         break;

      case Repr::REPR_UTC:
         os << "fromUTC(";
        _src->genSourceCode(os, code);
         os << ",*consMSA))";
         break;

      case Repr::REPR_REF:
         os << ",fromRef(";
        _src->genSourceCode(os, code);
         os << "))";
         break;

      default:
        error("TagInstruction::genSourceCode: unsupported representation");
        break;
   }
   os << ";\n";
}

Void ConstructFromNameInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = constructFromName(";
   _src->genSourceCode(os, code);
   os << ", ";
   _src_env->genSourceCode(os, code);
   os << ", ";
   os << code.useType(_typeSig);
   os << ", consEnv);\n";
}
Void ConstructFromStringInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = constructFromString(";
   _src->genSourceCode(os, code);
   os << ", ";
   os << code.useType(_typeSig);
   os << ", consEnv);\n";
}
Void EnterCellInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << "jump(((CellInfo*)";
   _x->genSourceCode(os, code);
   os << "->_tag)->entry_());\n";
}

#if (GARBAGE_COLLECTION==1)
Void MarkCellInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << "jump(((CellInfo*)";
   _x->genSourceCode(os, code);
   os << "->_tag)->_cellGC_MarkFn());\n";
}
#endif

Void RSS_Instruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << "if (" << _size << ">= rStructSize) {consMSA->free(rStruct);rStruct = (Byte*)consMSA->alloc("
      << _size << ");rStructSize = size;\n";
}

Void ReturnInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << "jump(popLabel());\n";
}

Void ToCellInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   _dst->genDstSourceCode(os, code);
   os << " = &toCell(";
   _src->genSourceCode(os, code);
   os << ");\n";
}

Void SourceCode::adjustStack(Int diff, Loc::Kind kind, ostream& os) const {
   if (diff != 0) {
      os << (diff > 0 ? "alloc" : "drop");
      os << '(' << abs(diff) << ");\n";
   }
}

Void StackAdjustInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << (_diff > 0 ? "alloc" : "drop");
   os << '(' << abs(_diff) << ");\n";
}

Void ExceptionInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << "runTimeError(\"???\");\n";
}

Void EmptyArgsInstruction::genSourceCode(ostream& os, SourceCode& code) const{

   if (_diff != 0)
      os << "(unsigned char*)";
   os << "sp";
   if (_diff != 0)
       os << (_diff > 0 ? '+' : '-') << abs(_diff);
   os << ";\n";
}

Void SqueezeInstruction::genSourceCode(ostream& os, SourceCode& code) const {
}

Loc::Kind SourceCode::locKind(Var& var, Repr repr) {
   switch (var.kind()) {
      case Var::ARG_VAR: {
         Var* lambdaVar = var.lambdaArgVar();
         if (lambdaVar != NULL)
            return locKind(*lambdaVar, repr);
         break;
/*
         if (!(var._lambda != NULL &&
               var._lambda->_label != NULL &&
               !var._lambda->_label->basicBlk()._head &&
               var._lambda->_label->basicBlk()._headBasicBlk == _basicBlk->_headBasicBlk))
            break;
*/
      }
//------ Common with LOCAL_VAR
      case Var::LOCAL_ARG_VAR:
      case Var::LOCAL_VAR:
         if (_basicBlk->localVar(var))
            return Loc::NATIVE_LOCAL_LOC;

      default:
         break;
   }
   return var.locKind(repr);
}

Loc::Kind SourceCode::temporaryKind(Loc::Kind kind) const {
   switch (kind) {
      case Loc::STACK_LOC:
         return Loc::NATIVE_LOCAL_LOC;

      default:
         return ICode::temporaryKind(kind);
   }
}

Loc* SourceCode::allocLoc(Repr repr, Loc::Kind kind, Bool argFlag,
                          Bool reclaimFlag/* = FALSE*/) {
   return kind == Loc::NATIVE_LOCAL_LOC
      ? (Loc*)&allocNativeLocal(repr)
      : ICode::allocLoc(repr, kind, argFlag, reclaimFlag);
}

Void Operand::genSourceCode(ostream& os, SourceCode& code) const {
   insert(os, code);
}

Void Operand::genDstSourceCode(ostream& os, SourceCode& code) const {
   genSourceCode(os, code);
}

Void VarOperand::genSourceCode(ostream& os, SourceCode& code) const {
   if (_var.kind() == Var::GLOBAL_VAR)
      os << "entry(";
   _var.insert(os, code);
   if (_var.kind() == Var::GLOBAL_VAR)
      os << "$)";
}

Void LabelOperand::genSourceCode(ostream& os, SourceCode& code) const {
   _codeLabel.genSourceCode(os, code);
}

Void BuiltInFnOperand::genSourceCode(ostream& os, SourceCode& code) const {
   BuiltInFn& fn = cellBody(*_builtInFn, BuiltInFn);
   os << (!_altEntryFlag ? "entry" : "altEntry") << '(';
   insert(os, code);
   os << ')';
}

Void BuiltInDataConOperand::genSourceCode(ostream& os, SourceCode& code) const {
/*
   if (repr.tag() == Repr::REPR_EXPR)
   {
      os << "builtInDataCon(";
      insert(os, code);
      os << ')';
   }
   else
   {
      os << "entry(";
      insert(os, code);
      os << ')';
   }
*/
}

Void CodeLabel::genSourceCode(ostream& os, SourceCode& code) const {
   os << "label";
   code.genLabel(*this, os);
}

Void Loc::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   assert(FALSE, "Loc::genSourceCode: Method not defined for derived class");
}

Void Loc::genDstSourceCode(Int offset, ostream& os, SourceCode& code) {
   genSourceCode(offset, os, code);
}

Void Register::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   os << regCodeToString(_id);
}

Void SpecialLoc::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   os << "builtInEnv()";
}

Void StaticLoc::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   os << nameString_(_name, code.nameTable());
}

Void StackLoc::genSourceCode(Int offset, ostream& os, SourceCode& code) {

#if (GARBAGE_COLLECTION==1)
   os << (_origin == StackLoc::SP_REL ? "stack" : "frame") << '(';
#else
   os << "stack(";

#endif

   os << (Int)code._sp - ((Int)_offset + (Int)_size);
   os << ", ";
   code.printRepr(_repr, os);
   os << ")";
}

Void CellCompLoc::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   os << "cellBody((";
   _loc.genSourceCode(0, os, code);
   os << "), struct S" << _structRepr.id();
   os << ").c" << this->_index;
}

Void CompLoc::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   _loc.genSourceCode(0, os, code);
   os << ".c" << this->_index;
}


Void IndLoc::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   os << "*(";
   code.printRepr(_repr, os);
   os << "*)(((unsigned char*)(";
   if (_loc.kind() == Loc::REGISTER_LOC &&
      ((Register&)_loc).id() == REG_cell)
      os << "cell";
   else
      _loc.genSourceCode(0, os, code);
   os << "))+(" << offset << "))";
}

Void IndexedLoc::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   os << "*(";
   code.printRepr(_repr, os);
   os << "*)(((unsigned char*)(";
   if (_loc.kind() == Loc::REGISTER_LOC &&
      ((Register&)_loc).id() == REG_cell)
      os << "cell";
   else
      _loc.genSourceCode(0, os, code);
   os << "))+(" << offset << "))";
}

// NativeLocal: Native local location

NativeLocal::NativeLocal(Repr repr, NativeLocal* next)
   : Loc(NATIVE_LOCAL_LOC, repr, next), _n(++NativeLocal::_count) {
}

NativeLocal::NativeLocal(const NativeLocal& src)
   : Loc(src._kind, src._repr, NULL), _n(src._n) {
}

UInt NativeLocal::_count = 0;

Loc* NativeLocal::clone(Loc* parent, Bool withVars, MSA& msa) const {
   return new(msa) NativeLocal(*this);
}

Void NativeLocal::insert(NativeLocal& loc, NativeLocal** nativeLocalPtr) {
   loc._next = *nativeLocalPtr;
   *nativeLocalPtr = &loc;
}

Void NativeLocal::print(ostream& os, const Env& env, Bool isDep/* = FALSE*/) const {
   os << " v" << _n;
   Loc::print(os, env, isDep);
}

Void NativeLocal::genSourceCode(Int offset, ostream& os, SourceCode& code) {
   assert(offset == 0, "NativeLocal::genSourceCode: unexpected offset");
   os << "v" << _n;
}

Void NativeLocal::genDstSourceCode(Int offset, ostream& os, SourceCode& code) {
   code.printRepr(_repr, os);
   os << ' ';
   genSourceCode(offset, os, code);
}

const char* Operator::sourceSymbol(Void) const {
   switch (_kind) {
      case TYPE_OF_OP:     return "typeOf";
      case OBJECT_TYPE_OP: return "objectType";

      case MAP_OP:         return "map";

      case LT_OP:          return "<";
      case LTEQ_OP:        return "<=";
      case EQ_OP:          return "==";
      case NEQ_OP:         return "¬=";
      case GTEQ_OP:        return ">=";
      case GT_OP:          return ">";

      case LTZ_OP:         return "< 0";
      case LTEQZ_OP:       return "<= 0";
      case EQZ_OP:         return "= 0";
      case NEQZ_OP:        return "¬= 0";
      case GTEQZ_OP:       return ">= 0";
      case GTZ_OP:         return "> 0";

      case NEG_OP:         return "-";

      case ADD_OP:         return "+";
      case SUB_OP:         return "-";
      case MUL_OP:         return "*";
      case DIV_OP:         return "/";

      case FROM_INT_OP:    return "";
      case FROM_DOUBLE_OP: return "";

      case SPECIAL_EXPR_OP:
                           return ".expr().tag() != PTR_TAG";

      case ASSIGN_AT_OP:   return "assignAtPrim";
      case ERROR_OP:       return "error";
      case TRACE_OP:       return "trace";

      case EQ_NULL_OP:     return "== NULL";
      case NEQ_NULL_OP:    return "!= NULL";

      case UNKNOWN_OP:
      default:             return "??";
   }
}

Void ConstOperand::genSourceCode(ostream& os, SourceCode& code) const {
   _k.genSourceCode(os, code);
}

Void CellInfoOperand::genSourceCode(ostream& os, SourceCode& code) const{
   if (_cellInfo_ != NULL)
      os << "&cellInfo" << _cellInfo_->_seqNo;
}

template <>
Void ConstT<Name>::genSourceCode(ostream& os, SourceCode& code) const {
/*
      if (repr.tag() == Repr::REPR_EXPR)
         os << "fromName(";
      os << "nameArr$[" << code.nameConstIndex(_value) << ']';
      if (repr.tag() == Repr::REPR_EXPR)
         os << ')';
*/ 
}


template <>
Void ConstT<TypeConst>::genSourceCode(ostream& os, SourceCode& code) const {
/*
   if (repr != Repr::REPR_ENV_PTR) {
      if (repr.tag() == Repr::REPR_EXPR)
         os << "fromType(";
      os << "typeArr$[" << code.typeConstIndex(_value._type) << ']';
      if (repr.tag() == Repr::REPR_EXPR)
         os << ')';
   } else
      os << "NULL";
      */
}


template <>
Void ConstT<Bits>::genSourceCode(ostream& os, SourceCode& code) const {
   os << hex << _value << dec;
}


template <>
Void ConstT<Int>::genSourceCode(ostream& os, SourceCode& code) const {
   os << _value;
}


template <>
Void ConstT<Double>::genSourceCode(ostream& os, SourceCode& code) const {
   os << _value;
}


template <>
Void ConstT<Char>::genSourceCode(ostream& os, SourceCode& code) const {
   os << '\'' << _value << '\'';
}


template <>
Void ConstT<String>::genSourceCode(ostream& os, SourceCode& code) const {
   os << '\"' << _value << '\"';
}

template <>
Void ConstT<Tag>::genSourceCode(ostream& os, SourceCode& code) const {
   os << _value;
}

Void Const_codeEnv::genSourceCode(ostream& os, SourceCode& code) const {
   insert(os, code);
}

Void Const_consEnv::genSourceCode(ostream& os, SourceCode& code) const {
   insert(os, code);
}

Void Const_NULL_Ptr::genSourceCode(ostream& os, SourceCode& code) const {
   os << "NULL";
}

Void Const_NULL_Env::genSourceCode(ostream& os, SourceCode& code) const {
   os << "NULL";
}

Void Const_NULL_REF::genSourceCode(ostream& os, SourceCode& code) const {
/*
   if (repr.tag() == Repr::REPR_EXPR)
      os << "fromRef(";
   os << (repr != Repr::REPR_ENV_PTR ? "NULL_REF" : "NULL");
   if (repr.tag() == Repr::REPR_EXPR)
      os << ')';
      */
}

Void LocOperand::genSourceCode(ostream& os, SourceCode& code) const {
   _loc.genSourceCode(0, os, code);

/*
   if (repr.tag() == Repr::REPR_EXPR && _loc._repr.tag() == Repr::REPR_INT) {
      os << "fromInt(";
   } else if (repr.tag() == Repr::REPR_EXPR && _loc._repr.tag() == Repr::REPR_FLOAT) {
      os << "fromFloat(";
   }

   if (_loc.kind() == Loc::NATIVE_LOCAL_LOC && _defFlag) {
      if (code.anyForwardLabels())
         code.createBlock(os);
      code.printRepr(((NativeLocal&)_loc)._repr, os);
      os << ' ';
   }
   _loc.genSourceCode(0, os, code);
   if (repr.tag() == Repr::REPR_EXPR && (_loc._repr.tag() == Repr::REPR_INT ||
                             _loc._repr.tag() == Repr::REPR_FLOAT)) {
      os << ",*consMSA)";
   }
*/
}

Void LocOperand::genDstSourceCode(ostream& os, SourceCode& code) const {
   !_reserved
      ? _loc.genDstSourceCode(0, os, code)
      : _loc.genSourceCode(0, os, code);
}

Void SourceCode::insertLoc(Loc& loc) {
   if (loc.kind() == Loc::NATIVE_LOCAL_LOC)
      NativeLocal::insert((NativeLocal&)loc, &_nativeLocals);
   else
      ICode::insertLoc(loc);
}
