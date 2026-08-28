/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    code.cpp
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
 *    Implementation of base code generation class.
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
#include "ivory/bits.h"
#include "ivory/int.h"
#include "ivory/double.h"
#include "ivory/env.h"
#include "ivory/list.h"
#include "ivory/ref.h"
#include "ivory/string.h"
#include "ivory/trace.h"
#include "ivory/tuple.h"
#include "ivory/compiler/code.h"

#define DEFAULT_MAX_PEEPHOLE_OPTIMISATION_PASSES 20

#define SELECT_PREFIX "SEL-"   // Prefix for select instruction

#define TRACE
#define TRACE_EXTRA
#define PEEPHOLE_OPTIMISE
//#define REARRANGE_BLOCKS

#ifdef TRACE
static Bool traceFlag = FALSE;
#define toggle(flag) ((flag)=!(flag))
static Int instructionProbeSeqNo = -1;
#endif

declareTypeCon(Char);
declareTypeCon(Int);
declareTypeCon(Name);
declareTypeCon(Ref);

declareDataCon(Null);

declareType(CellInfo);
declareType(Label);

// Constructor and enviroment constants

Const_consEnv     const_consEnv;
Const_NULL_Env    const_NULL_Env;

Void insertEscape(char c, ostream& os) {
   char escape;
   switch (c) {
   case '\n': {  escape = 'n';    break;   };
   case '\t': {  escape = 't';    break;   };
   case '\v': {  escape = 'v';    break;   };
   case '\b': {  escape = 'b';    break;   };
   case '\r': {  escape = 'r';    break;   };
   case '\f': {  escape = 'f';    break;   };
   case '\a': {  escape = 'a';    break;   };
   case '\\': {  escape = '\\';   break;   };
   case '\?': {  escape = '\?';   break;   };
   case '\'': {  escape = '\'';   break;   };
   case '\"': {  escape = '\"';   break;   };
   default: {  os << c;          return;  };
   }
   os << '\\' << escape;
}

Void insertEscapedString(ConstString s, ostream& os) {
   os << '\"';
   for (UInt i = 0; i < strlen(s); i++)
      insertEscape(s[i], os);
   os << '\"';
}

CodeLabel::CodeLabel(BasicBlk* basicBlk, Bool head/* = FALSE*/, Lambda* lambda/* = NULL*/)
 : _basicBlk(basicBlk), _head(head), _lambda(lambda),
   _headCodeLabel(NULL),
   _seqNo(0), _useCount(0),
   _saveEntryFlag(FALSE),
   _bytePos(0), _section(NULL) {
}

Void CodeLabel::incRef(Void) {
   _useCount++;
}

Void CodeLabel::decRef(Void) {
    assert(_useCount > 0, "CodeLabel::decRef: unexpected");
   _useCount--;
}

Void CodeLabel::insert(ostream& os, const Code& code) const {
   os << "l" << _seqNo;
}

// Var

Void Var::insert(ostream& os, const Code& code) const {
   if (closure() != NULL) {
      if (closure()->name() >= 0)
         printName(closure()->name(), os, code.nameTable());
      else if (closure()->name() == NULL_NAME)
         os << "NULL_NAME";
      else
         os << 'v' << -closure()->name();
      os << '\'';
   }
   if (_name >= 0)
      printName(_name, os, code.nameTable());
   else if (_name == NULL_NAME)
      os << "NULL_NAME";
   else
      os << 'v' << -_name;

/*
   os << '(' << (Void*)this << ')';
*/

   os << "::";
   printTypeSig(_typeSig, 0, FALSE, os, code.env());
   os << '/' << kindMnemonic();
   if (isLive())
      os << "/live";
}

Void Var::adjRefCount(Int diff) {
   _refCount += diff;
}

Void Var::setNextUse(Bool nextUse) {
   _nextUse = nextUse;
}

Void Var::setLastNextUse(Bool lastNextUse) {
   _lastNextUse = lastNextUse;
}

Bool Var::isLive(Void) const {
   return _nextUse || kind() == Var::GLOBAL_VAR;
}

Void Var::checkLive(Code& code) {
   if (!isLive())
      code.notLive(*this);
}

Var* Var::lambdaArgVar(Void) const {
   return _parentLambda != NULL &&
      _index < _parentLambda->_nFormalParams&&
      formOf(_parentLambda->_formalParamV[_index].typedVal().val()) == VAR
      ? &toBody(_parentLambda->_formalParamV[_index].typedVal().val(), Var)
      : NULL;
}

// Define locally due to issues of header file nesting

extern Void printTypeSig(TypeSig typeSig, UInt prec, Bool isRight, ostream& os, const Env& env);

const char* Var::kindMnemonic(Void) const {
   switch (_kind) {
   case GLOBAL_VAR:        return "G";
   case EXT_GLOBAL_VAR:    return "EG";
   case FORMAL_PARAM_VAR:  return "FP";
   case EXTRA_FORMAL_PARAM_VAR:
                           return "EFP";
   case ARG_VAR:           return "PA";
   case EXTRA_ARG_VAR:     return "SA";

   case LOCAL_ARG_VAR:     return "LA";
   case LOCAL_VAR:         return "L";
   case CLOSURE_VAR:       return "CL";
   case NON_GLOBAL_FREE_VAR:
                           return "F";
   case CONTINUATION_VAR:  return "CO";

#if (CLOSURE_UPDATE==1)
   case UPDATE_CELL_VAR:   return "UC";
#endif

   case RESULT_VAR:        return "RV";
   default:                return "??";
   }
}

Void Var::printType(ostream& os, const Env& env) const {
   ::printTypeSig(typeSig(), 0, FALSE, os, env);
}

Void Var::printRepr(ostream& os, const Env& env) const {
   //   if (_structTemplate == NULL)
   printType(os, env);
   //   else
   //      os << "struct S" << _structTemplate->_id << '*';
}

Void Var::printLocs(ostream& os, const Env& env) const {
   printName(_name, os, env.nameTable());
   os << " assigned to\n";
   _locs.print(os, env);
   os.flush();
}

// FreeVarAssoc

FreeVarAssoc::FreeVarAssoc(FreeVarAssoc* next,
                           FreeVarAssoc* parent,
                           TypedVal* typedVal, const ModuleDefn* moduleDefn, 
                           Cell* closedVar,
                           Bool isGlobal,
                           Bool needsClosure,
                           Bool selfReferential)
 : _next(next),
   _parent(parent),
   _typedVal(typedVal),
   _val(typedVal->val()),
   _moduleDefn(moduleDefn),
   _closedVar(closedVar),
   _isGlobal(isGlobal),
   _needsClosure(needsClosure),
   _selfReferential(selfReferential) {
}

// BasicBlkElement

BasicBlkElement::BasicBlkElement(BasicBlkElement* next, BasicBlk& basicBlk)
 : _next(next), _basicBlk(basicBlk), _count(0) {
}

// BasicBlkSet

BasicBlkElement* BasicBlkSet::addElement(BasicBlk& basicBlk, MSA& msa) {
   BasicBlkElement** basicBlkElementPtr = &_basicBlkElements;

   while (*basicBlkElementPtr != NULL) {
      if (&(*basicBlkElementPtr)->_basicBlk == &basicBlk)
         break;
      basicBlkElementPtr = &(*basicBlkElementPtr)->_next;
   }
   if (*basicBlkElementPtr == NULL) {
      BasicBlkElement* basicBlkElement = new (msa) BasicBlkElement(NULL, basicBlk);
      *basicBlkElementPtr = basicBlkElement;
   }
   (*basicBlkElementPtr)->_count++;
   return *basicBlkElementPtr;
}

BasicBlkElement* BasicBlkSet::findElement(const BasicBlk& basicBlk) const {
   BasicBlkElement* basicBlkElement = _basicBlkElements;

   while (basicBlkElement != NULL) {
      if (&basicBlkElement->_basicBlk == &basicBlk)
         return basicBlkElement;
      basicBlkElement = basicBlkElement->_next;
   }
   return NULL;
}

Void BasicBlkSet::join(const BasicBlkSet& basicBlks, MSA& msa) {
   BasicBlkElement* basicBlkElement = basicBlks._basicBlkElements;
   while (basicBlkElement != NULL) {
      BasicBlkElement* joinedElement =
         addElement(basicBlkElement->_basicBlk, msa);
      joinedElement->_count += basicBlkElement->_count - 1;
      basicBlkElement = basicBlkElement->_next;
   }
}

Void BasicBlkSet::removeElement(const BasicBlk& basicBlk, MSA& msa) {
   BasicBlkElement** basicBlkElementPtr = &_basicBlkElements;

   while (*basicBlkElementPtr != NULL) {
      if (&(*basicBlkElementPtr)->_basicBlk == &basicBlk) {
         BasicBlkElement* basicBlkElement = *basicBlkElementPtr;
         if (--(*basicBlkElementPtr)->_count == 0) {
            *basicBlkElementPtr = (*basicBlkElementPtr)->_next;
            msa.free(basicBlkElement);
         }
         return;
      }
      basicBlkElementPtr = &(*basicBlkElementPtr)->_next;
   }
}

Void BasicBlk::addSuccessor(BasicBlk& basicBlk, MSA& msa) {
   _successors.addElement(basicBlk, msa);
}

Void BasicBlk::removeSuccessor(BasicBlk& basicBlk, MSA& msa) {
   _successors.removeElement(basicBlk, msa);
}

Bool BasicBlk::hasSuccessor(const BasicBlk& basicBlk) const {
   return _successors.findElement(basicBlk) != NULL;
}


Void BasicBlk::removeInstruction(Instruction* ins, Code& code, Bool destroy/* = TRUE*/) {
   if (ins == _lastIns)
      _lastIns = ins->_prev;
   code.removeInstruction(ins, destroy);
}

// BasicBlk::BasicBlk: Basic block of instructions

BasicBlk::BasicBlk(Void)
 : _next(NULL), _prev(NULL),
   _firstIns(NULL), _lastIns(NULL),
   _locCodeGenerated(FALSE), _defer(FALSE), _saveFlag(FALSE),
   _sd(0), _sp(0) {
}

BasicBlk* BasicBlk::implicitSuccessor(Void) const {
   switch (_lastIns->kind()) {
      case Instruction::GOTO_INSTR:
      case Instruction::CASE_INSTR:
      case Instruction::ENTER_CELL_INSTR:

#if (GARBAGE_COLLECTION==1)
      case Instruction::MARK_CELL_INSTR:
#endif

      case Instruction::EXCEPTION_INSTR:
      case Instruction::RETURN_INSTR:
         return NULL;

      default:
         if (_lastIns->_next != NULL) {
            assert(_lastIns->_next->kind() == Instruction::LABEL_INSTR,
                  "BasicBlk::implicitSuccessor: unexpected");
            return &((LabelInstruction&)*_lastIns->_next)._codeLabel.basicBlk();
         } else
            return NULL;
   }
}

Void BasicBlk::setUD(Code& code) {
   Instruction* ins = _firstIns;
   while (ins != NULL) {
      ins->setUD(*this, code.msa());
      ins->setSuccessor(*this, code.msa());
      if (ins == _lastIns) {
         BasicBlk* impSuccBlk;
         if ((impSuccBlk = implicitSuccessor()) != NULL)
            addSuccessor(*impSuccBlk, code.msa());
         break;
      }
      ins = ins->_next;
   }
}

/*
1. Initialises counts and next use

1. Sets next use information for instruction operands.
*/

Bool BasicBlk::varIsInput(const Var& var) const {
   VarElement* varElement = _vars.findElement(var);
   return varElement != NULL &&
      varElement->testFlags(VarElement::I) &&
      !varElement->testFlags(VarElement::D);
}

Bool BasicBlk::varIsInputToContinuation(const Var& var) const {
   VarElement* varElement = _vars._varElements;
   while (varElement != NULL) {
      if (varElement->_var.kind() == Var::CONTINUATION_VAR &&
         varElement->_var._contLabel->basicBlk().varIsInput(var))
         return TRUE;
      varElement = varElement->_next;
   }
   return FALSE;
}

Void BasicBlk::liveVarInfo(const Code& code) const {
   BasicBlk* impSucc = implicitSuccessor();

// For each variable, set next use true if either:

// 1. Marked for export
// 2. Input to an immediate successor
// 3. Input to a continuation
//    (note that, by definition, all continuation variables are live at the last exit)

   VarElement* varElement = _vars._varElements;
   while (varElement != NULL) {
      Var& var = varElement->_var;
      var._killPending = FALSE;
      var._nextUse = var._kind == Var::CONTINUATION_VAR ||
         varElement->testFlags(VarElement::X) ||
         varElement->testFlags(VarElement::O) &&
         (impSucc != NULL && impSucc->varIsInput(var) ||
            varIsInputToContinuation(var));
      var._lastNextUse = var._nextUse;
      varElement = varElement->_next;
   }

// Starting at the end of the block

   Instruction* ins = _lastIns;

// Step back to the beginning, setting the next use
   Bool lastUseSet = impSucc != NULL;
   while (ins != NULL) {
      ins->setNextUse(code.msa());

      if (!lastUseSet) {
         VarElement* varElement = _vars._varElements;
         while (varElement != NULL) {
            Var& var = varElement->_var;
            var._lastNextUse = var._nextUse;
            varElement = varElement->_next;
         }
         lastUseSet = TRUE;
      }

      if (ins == _firstIns)
         break;
      ins = ins->_prev;
   }
}

UInt CellInfo_::_count = 0;

CellInfo_::CellInfo_(CodeLabel& entryLabel, CodeLabel& altEntryLabel,
                     CodeLabel* copyFnLabel,
                     CodeLabel* extractBinFnLabel, CodeLabel* insertBinFnLabel


#if (GARBAGE_COLLECTION==1)
                   , CodeLabel* gcFnLabel

#endif
                   )
 : _seqNo(++_count), _useCount(0),
   _entryLabel(entryLabel), _altEntryLabel(altEntryLabel),
   _copyFnLabel(copyFnLabel),
   _extractBinFnLabel(extractBinFnLabel), _insertBinFnLabel(insertBinFnLabel),

#if (GARBAGE_COLLECTION==1)
   _gcFnLabel(gcFnLabel),
#endif

   _rackOffset(NULL) {
}

Void CellInfo_::incRef(Void) {
   if (_useCount++ == 0) {
      _entryLabel.incRef();
      _altEntryLabel.incRef();
      if (_copyFnLabel != NULL)
         _copyFnLabel->incRef();
      if (_extractBinFnLabel != NULL)
         _extractBinFnLabel->incRef();
      if (_insertBinFnLabel != NULL)
         _insertBinFnLabel->incRef();

#if (GARBAGE_COLLECTION==1)
      if (_gcFnLabel != NULL)
         _gcFnLabel->incRef();
#endif

   }
}

Void CellInfo_::decRef(Void) {
   assert(_useCount > 0, "CellInfo_::decRef: unexpected");

   if (--_useCount == 0) {
      _entryLabel.decRef();
      _altEntryLabel.decRef();
      if (_copyFnLabel != NULL)
         _copyFnLabel->decRef();
      if (_extractBinFnLabel != NULL)
         _extractBinFnLabel->decRef();
      if (_insertBinFnLabel != NULL)
         _insertBinFnLabel->decRef();

#if (GARBAGE_COLLECTION==1)
      if (_gcFnLabel != NULL)
         _gcFnLabel->decRef();
#endif

   }
}

Void CellInfo_::insert(ostream& os, Code& code) const {
   _entryLabel.insert(os, code);
   os << ',';
   _altEntryLabel.insert(os, code);
   if (_copyFnLabel != NULL) {
      os << ',';
      _copyFnLabel->insert(os, code);
   }
   if (_extractBinFnLabel != NULL) {
      os << ',';
      _extractBinFnLabel->insert(os, code);
   }
   if (_insertBinFnLabel != NULL) {
      os << ',';
      _insertBinFnLabel->insert(os, code);
   }

#if (GARBAGE_COLLECTION==1)
   os << ',';
   if (_gcFnLabel != NULL) 
      _gcFnLabel->insert(os, code);
#endif

}
// VarElement

VarElement::VarElement(VarElement* next, Var& var)
 : _next(next), _var(var), _nextUse(FALSE), _flags((Flags)0) {
}

// VarSet

VarSet::VarSet(const VarSet& src, MSA& msa) {
   VarElement* varElement = src.head();
   VarElement** varElementPtr = &_varElements;
   while (varElement != NULL) {
      *varElementPtr = new(msa) VarElement(NULL, varElement->var());
      varElementPtr = &((*varElementPtr)->_next);
      varElement = varElement->_next;
   }
}

VarElement* VarSet::addElement(Var& var, MSA& msa) {
   VarElement** varElementPtr = &_varElements;

   while (*varElementPtr != NULL) {
      if (&(*varElementPtr)->_var == &var)
         break;
      varElementPtr = &(*varElementPtr)->_next;
   }
   if (*varElementPtr == NULL) {
      VarElement* varElement = new (msa) VarElement(NULL, var);
      *varElementPtr = varElement;
   }
   return *varElementPtr;
}

VarElement* VarSet::findElement(const Var& var) const {
   VarElement* varElement = _varElements;

   while (varElement != NULL) {
      if (&varElement->_var == &var)
         return varElement;
      varElement = varElement->_next;
   }
   return NULL;
}

Void VarSet::removeElement(const Var& var, MSA& msa) {
   VarElement** varElementPtr = &_varElements;

   while (*varElementPtr != NULL) {
      if (&(*varElementPtr)->_var == &var) {
         VarElement* varElement = *varElementPtr;
         *varElementPtr = (*varElementPtr)->_next;
         msa.free(varElement);
         return;
      }
      varElementPtr = &(*varElementPtr)->_next;
   }
}

Void VarSet::moveToHead(VarElement& varElement) {
   VarElement** varElementPtr = &_varElements;

   while (*varElementPtr != NULL) {
      if (*varElementPtr == &varElement) {
         *varElementPtr = (*varElementPtr)->_next;
         break;
      }
      varElementPtr = &(*varElementPtr)->_next;
   }
   varElement._next = _varElements;
   _varElements = &varElement;
}

UInt VarSet::count(const Code& code) {
   UInt res = 0;
   VarElement* varElement = _varElements;

   while (varElement != NULL) {
      Var& var = varElement->_var;

      outStream << ' ';
      if (varElement->_var._name >= 0)
         printName(var._name, outStream, code.nameTable());
      else
         outStream << 'v' << -var._name;

      outStream << '-';
      outStream << var.kindMnemonic();

      outStream << ':';

      if (varElement->testFlags(VarElement::D))
         outStream << 'D';
      if (varElement->testFlags(VarElement::I))
         outStream << 'I';
      if (varElement->testFlags(VarElement::U))
         outStream << 'U';
      if (varElement->testFlags(VarElement::O))
         outStream << 'O';
      if (varElement->testFlags(VarElement::X))
         outStream << 'X';

      varElement = varElement->_next;
      res++;
   }
   return res;
}

Void VarSet::empty(MSA& msa) {
   VarElement* varElement;

   while ((varElement = _varElements) != NULL) {
      _varElements = varElement->_next;
      msa.free(varElement);
   }
}

Void VarSet::join(VarSet& vars, MSA& msa) {
   VarElement* varElement = vars._varElements;
   while (varElement != NULL) {
      addElement(varElement->_var, msa)->setFlags(varElement->_flags);
      varElement = varElement->_next;
   }
}

// Operator

Operator::Operator(Kind kind, Type type, UInt arity)
 : _kind(kind),
   _type(type),
   _arity(arity) {
}

Operator::Operator(const char* fnStr) {
   struct PrimOpMapping table[] = {
      { "fromThunk",       FROM_THUNK_OP,    OP_TYPE_EXPR,     1 },

#if (CLOSURE_UPDATE==1)  
      { "update",          UPDATE_OP,        OP_TYPE_CLOSURE,  1 },
#endif

      { "typeOf",          TYPE_OF_OP,       OP_TYPE_POLY,     1 },
      { "castPtr",         CAST_PTR_OP,      OP_TYPE_PTR,      1 },
      { "envOf",           ENV_OF_OP,        OP_TYPE_POLY,     1 },
      { "setEnv",          SET_ENV_OP,       OP_TYPE_VOID,     1 },
      { "fromPlain",       FROM_PLAIN_OP,    OP_TYPE_POLY,     2 },
      { "sizeOf",          SIZE_OF_OP,       OP_TYPE_TYPE,     1 },
      { "objectType",      OBJECT_TYPE_OP,   OP_TYPE_REF,      1 },

      { "cellInfoOf",      CELL_INFO_OF_OP,  OP_TYPE_CELL_INFO,
                                                               1 },

      { "tagOf",           TAG_OF_OP,        OP_TYPE_TAG ,     1 },
      { "castTag",         CAST_TAG_OP,      OP_TYPE_POLY ,    1 },
      { "constructorString",
                           CONSTRUCTOR_STRING_OP,
                                             OP_TYPE_STRING,   1 },
      { "constructFromName",
                           CONSTRUCT_FROM_NAME_OP,
                                             OP_TYPE_POLY,      1 },
      { "constructFromString",
                           CONSTRUCT_FROM_STRING_OP,
                                             OP_TYPE_POLY,      1 },

      { "mapClosure",      MAP_CLOSURE_OP, OP_TYPE_POLY,       1 },
      { "extractFreeVars", EXTRACT_FREE_VARS_OP,
                                             OP_TYPE_POLY,     2 },
      { "insertFreeVars",  INSERT_FREE_VARS_OP,
                                             OP_TYPE_POLY,     2 },

      { "eqEnv",           EQ_OP,            OP_TYPE_PTR,      2 },
      { "neqEnv",          NEQ_OP,           OP_TYPE_PTR,      2 },

      { "mapName",         MAP_OP,           OP_TYPE_NAME,     2 },
      { "mapType",         MAP_OP,           OP_TYPE_TYPE,     2 },
      { "mapFn",           MAP_OP,           OP_TYPE_CLOSURE,  2 },
      { "mapThunk",        MAP_OP,           OP_TYPE_CLOSURE,  2 },
      { "mapExpr",         MAP_OP,           OP_TYPE_EXPR,     2 },
      { "mapString",       MAP_OP,           OP_TYPE_STRING,   2 },

#if (SERIALISATION==1)
      { "extractBinFn",    EXTRACT_OP,       OP_TYPE_CLOSURE,  1 },
      { "insertBinFn",     INSERT_OP,        OP_TYPE_CLOSURE,  2 },
      { "extractBinThunk", EXTRACT_OP,       OP_TYPE_CLOSURE,  1 },
      { "insertBinThunk",  INSERT_OP,        OP_TYPE_CLOSURE,  2 },
      { "extractBinExpr",  EXTRACT_OP,       OP_TYPE_EXPR,     1 },
      { "insertBinExpr",   INSERT_OP,        OP_TYPE_EXPR,     2 },
#endif

      { "mapExpr",         MAP_OP,           OP_TYPE_EXPR,     2 },
      { "mapString",       MAP_OP,           OP_TYPE_STRING,   2 },


      { "eqName",          EQ_OP,            OP_TYPE_NAME,     2 },
      { "nEqName",         NEQ_OP,           OP_TYPE_NAME,     2 },

      { "eqType",          EQ_OP,            OP_TYPE_TYPE,     2 },
      { "nEqType",         NEQ_OP,           OP_TYPE_TYPE,     2 },

      { "eqTag",           EQ_OP,            OP_TYPE_TAG,     2 },

      { "negInt",          NEG_OP,           OP_TYPE_INT,      1 },

      { "addInt",          ADD_OP,           OP_TYPE_INT,      2 },
      { "subInt",          SUB_OP,           OP_TYPE_INT,      2 },
      { "mulInt",          MUL_OP,           OP_TYPE_INT,      2 },
      { "divInt",          DIV_OP,           OP_TYPE_INT,      2 },
      { "modInt",          MOD_OP,           OP_TYPE_INT,      2 },

      { "notBits",         NOT_OP,           OP_TYPE_BITS,     1 },
      { "andBits",         AND_OP,           OP_TYPE_BITS,     2 },
      { "orBits",          OR_OP,            OP_TYPE_BITS,     2 },
      { "xorBits",         XOR_OP,           OP_TYPE_BITS,     2 },
      { "lShiftBits",      L_SHIFT_OP,       OP_TYPE_BITS,     2 },
      { "rShiftBist32",    R_SHIFT_OP,       OP_TYPE_BITS,     2 },

      { "ltInt",           LT_OP,            OP_TYPE_INT,      2 },
      { "ltEqInt",         LTEQ_OP,          OP_TYPE_INT,      2 },
      { "eqInt",           EQ_OP,            OP_TYPE_INT,      2 },
      { "nEqInt",          NEQ_OP,           OP_TYPE_INT,      2 },
      { "neqInt",          NEQ_OP,           OP_TYPE_INT,      2 },
      { "gtEqInt",         GTEQ_OP,          OP_TYPE_INT,      2 },
      { "gtInt",           GT_OP,            OP_TYPE_INT,      2 },

      { "fromStringInt",   FROM_STRING_OP,   OP_TYPE_INT,      1 },

      { "negFloat",        NEG_OP,           OP_TYPE_FLOAT,    1 },

      { "addFloat",        ADD_OP,           OP_TYPE_FLOAT,    2 },
      { "subFloat",        SUB_OP,           OP_TYPE_FLOAT,    2 },
      { "mulFloat",        MUL_OP,           OP_TYPE_FLOAT,    2 },
      { "divFloat",        DIV_OP,           OP_TYPE_FLOAT,    2 },

      { "ltFloat",         LT_OP,            OP_TYPE_FLOAT,    2 },
      { "ltEqFloat",       LTEQ_OP,          OP_TYPE_FLOAT,    2 },
      { "eqFloat",         EQ_OP,            OP_TYPE_FLOAT,    2 },
      { "nEqFloat",        NEQ_OP,           OP_TYPE_FLOAT,    2 },
      { "neqFloat",        NEQ_OP,           OP_TYPE_FLOAT,    2 },
      { "gtEqFloat",       GTEQ_OP,          OP_TYPE_FLOAT,    2 },
      { "gtFloat",         GT_OP,            OP_TYPE_FLOAT,    2 },

      { "fromIntFloat",    FROM_INT_OP,      OP_TYPE_FLOAT,    1 },
      { "fromDoubleFloat", FROM_DOUBLE_OP,   OP_TYPE_FLOAT,    1 },

      { "negDouble",       NEG_OP,           OP_TYPE_DOUBLE,   1 },

      { "addDouble",       ADD_OP,           OP_TYPE_DOUBLE,   2 },
      { "subDouble",       SUB_OP,           OP_TYPE_DOUBLE,   2 },
      { "mulDouble",	      MUL_OP,           OP_TYPE_DOUBLE,   2 },
      { "divDouble",       DIV_OP,           OP_TYPE_DOUBLE,   2 },

      { "ltDouble",        LT_OP,            OP_TYPE_DOUBLE,   2 },
      { "ltEqDouble",      LTEQ_OP,          OP_TYPE_DOUBLE,   2 },
      { "eqDouble",        EQ_OP,            OP_TYPE_DOUBLE,   2 },
      { "nEqDouble",       NEQ_OP,           OP_TYPE_DOUBLE,   2 },
      { "neqDouble",       NEQ_OP,           OP_TYPE_DOUBLE,   2 },
      { "gtEqDouble",      GTEQ_OP,          OP_TYPE_DOUBLE,   2 },
      { "gtDouble",        GT_OP,            OP_TYPE_DOUBLE,   2 },

      { "fromIntByte",     FROM_INT_OP,      OP_TYPE_BYTE,     1 },
      { "fromIntBits",     FROM_INT_OP,      OP_TYPE_BITS,     1 },
      { "fromIntFloat",    FROM_INT_OP,      OP_TYPE_FLOAT,    1 },
      { "fromIntDouble",   FROM_INT_OP,      OP_TYPE_DOUBLE,   1 },
      { "fromFloatDouble", FROM_FLOAT_OP,    OP_TYPE_DOUBLE,   1 },
      { "fromStringDouble",
                           FROM_STRING_OP,   OP_TYPE_DOUBLE,   1 },

      { "ltTAG",           LT_OP,            OP_TYPE_TAG,      2 },
      { "ltEqTAG",         LTEQ_OP,          OP_TYPE_TAG,      2 },
      { "eqTAG",           EQ_OP,            OP_TYPE_TAG,      2 },
      { "nEqTAG",          NEQ_OP,           OP_TYPE_TAG,      2 },
      { "neqTAG",          NEQ_OP,           OP_TYPE_TAG,      2 },
      { "gtEqTAG",         GTEQ_OP,          OP_TYPE_TAG,      2 },
      { "gtTAG",           GT_OP,            OP_TYPE_TAG,      2 },

      { "eqBool",          EQ_OP,            OP_TYPE_TAG,      2 },
      { "nEqBool",         NEQ_OP,           OP_TYPE_TAG,      2 },
      { "neqBool",         NEQ_OP,           OP_TYPE_TAG,      2 },

      { "ltChar",          LT_OP,            OP_TYPE_CHAR,     2 },
      { "ltEqChar",        LTEQ_OP,          OP_TYPE_CHAR,     2 },
      { "eqChar",          EQ_OP,            OP_TYPE_CHAR,     2 },
      { "nEqChar",         NEQ_OP,           OP_TYPE_CHAR,     2 },
      { "neqChar",         NEQ_OP,           OP_TYPE_CHAR,     2 },
      { "gtEqChar",        GTEQ_OP,          OP_TYPE_CHAR,     2 },
      { "gtChar",          GT_OP,            OP_TYPE_CHAR,     2 },

      { "lengthString",    LENGTH_STRING_OP, OP_TYPE_STRING,   1 },
      { "getAtString",     GET_AT_STRING_OP, OP_TYPE_STRING,   2 },
      { "putAtString",     PUT_AT_STRING_OP, OP_TYPE_STRING,   3 },

      { "eqPtr",           EQ_OP,            OP_TYPE_PTR,     2 },
      { "neqPtr",          NEQ_OP,           OP_TYPE_PTR,     2 },

      { "eqPlainPtr",      EQ_OP,            OP_TYPE_PTR,     2 },

      { "ltUTC",           LT_OP,            OP_TYPE_INT,      2 },
      { "ltEqUTC",         LTEQ_OP,          OP_TYPE_INT,      2 },
      { "eqUTC",           EQ_OP,            OP_TYPE_INT,      2 },
      { "nEqUTC",          NEQ_OP,           OP_TYPE_INT,      2 },
      { "neqUTC",          NEQ_OP,           OP_TYPE_INT,      2 },
      { "gtEqUTC",         GTEQ_OP,          OP_TYPE_INT,      2 },
      { "gtUTC",           GT_OP,            OP_TYPE_INT,      2 },

      { "allocPtrArray",   ALLOC_PTR_ARRAY_OP,
                                             OP_TYPE_PTR_ARRAY, 1 },
      { "copyPtrArray",    COPY_PTR_ARRAY_OP,
                                             OP_TYPE_PTR_ARRAY, 5 },
      { "getAtPtrArray",   GET_AT_PTR_ARRAY_OP,
                                             OP_TYPE_PTR_ARRAY, 2 },
      { "putAtPtrArray",   PUT_AT_PTR_ARRAY_OP,
                                             OP_TYPE_PTR_ARRAY, 3 },
      { "getAtPlainPtrArray",
                           GET_AT_PLAIN_PTR_ARRAY_OP,
                                              OP_TYPE_PTR_ARRAY, 2 },
      { "putAtPlainPtrArray",
                           PUT_AT_PLAIN_PTR_ARRAY_OP,
                                             OP_TYPE_PTR_ARRAY, 3 },

      { "revRefs",         REV_REFS_OP,      OP_TYPE_REFLIST,  1 },

      { "timeDiff",        SUB_OP,           OP_TYPE_UTC,      2 },

      { "assignAtPrim",    ASSIGN_AT_OP,     OP_TYPE_VOID,     2 },

#if (GARBAGE_COLLECTION==1)
      { "markFreeVars_GC", MARK_FREE_VARS_GC_OP,
                                             OP_TYPE_POLY,     1 },
      { "markPtr_GC",      MARK_PTR_GC_OP,   OP_TYPE_PTR,      1 },
      { "markExprPtr_GC",  MARK_EXPR_PTR_GC_OP,
                                             OP_TYPE_EXPR,     1 },
      { "markExpr_GC",     MARK_EXPR_GC_OP,  OP_TYPE_EXPR,     1 },
      { "markCell_GC",     MARK_CELL_GC_OP,  OP_TYPE_CLOSURE,  1 },
      { "markString_GC",   MARK_STRING_GC_OP,
                                             OP_TYPE_STRING,   1 },
#endif

      { "error",           ERROR_OP,         OP_TYPE_VOID,     1 },
      { "trace",           TRACE_OP,         OP_TYPE_VOID,     1 } };
                  
   UInt max = sizeof(table) / (sizeof(struct PrimOpMapping));

   if (fnStr != NULL)
      for (UInt i = 0; i < max; i++)
         if (strcmp(table[i]._fnStr, fnStr) == 0)
         {
            _kind = table[i]._kind;
            _type = table[i]._type;
            _arity = table[i]._arity;
            return;
         }

   _kind = UNKNOWN_OP;
   _type = OP_TYPE_UNKNOWN;
   _arity = 0;
}

Bool Operator::mapNeedsGCFrame(Void) const {
   if (_kind != MAP_OP)
      return FALSE;

   switch (_type) {
      case OP_TYPE_CLOSURE:
      case OP_TYPE_EXPR:
      case OP_TYPE_STRING:
         return TRUE;

      default:
         return FALSE;
   }
}

const char* Operator::mnemonic(Void) const {
   switch (_kind) {   
      case FROM_THUNK_OP:        return "fromThunk";

#if (CLOSURE_UPDATE==1)  
      case UPDATE_OP:            return "update";
#endif

      case MAP_CLOSURE_OP:       return "mapClosure";

      case MAP_OP:               return "map";
      case EXTRACT_FREE_VARS_OP: return "extractFreeVars";
      case INSERT_FREE_VARS_OP:  return "insertFreeVars";

#if (SERIALISATION==1)
      case EXTRACT_OP:           return "extract";
      case INSERT_OP:            return "insert";
#endif

      case NOT_OP:               return "not";

      case AND_OP:               return "and";
      case OR_OP:                return "or";
      case XOR_OP:               return "xor";
      case L_SHIFT_OP:           return "lShift";
      case R_SHIFT_OP:           return "rShift";

      case LT_OP:                return "lt";
      case LTEQ_OP:              return "lteq";
      case EQ_OP:                return "eq";
      case NEQ_OP:               return "neq";
      case GTEQ_OP:              return "gteq";
      case GT_OP:                return "gt";

      case NEG_OP:               return "negate";

      case ADD_OP:               return "add";
      case SUB_OP:               return "sub";
      case MUL_OP:               return "mul";
      case DIV_OP:               return "div";
      case MOD_OP:               return "mod";

      case FROM_INT_OP:          return "fromInt";
      case FROM_DOUBLE_OP:       return "fromDouble";

      case CELL_INFO_OF_OP:      return "cellInfoOf";
      case TYPE_OF_OP:           return "typeOf";
      case CAST_PTR_OP:          return "castPtr";
      case SIZE_OF_OP:           return "sizeOf"; 
      case TAG_OF_OP:            return "tagOf";
      case CAST_TAG_OP:          return "castTag";
      case CONSTRUCTOR_STRING_OP:
                                  return "constructorString";
      case CONSTRUCT_FROM_NAME_OP:
                                  return "constructFromName";
      case CONSTRUCT_FROM_STRING_OP:
                                  return "constructFromString";

      case ENV_OF_OP:            return "envOf";
      case FROM_PLAIN_OP:        return "fromPlain";

      case OBJECT_TYPE_OP:       return "objectType";

      case EQ_NULL_OP:           return "eqNULL";
      case NEQ_NULL_OP:          return "neqNULL";

      case CONS_OP:              return "cons";

      case LENGTH_STRING_OP:     return "lengthString";
      case GET_AT_STRING_OP:     return "getAtString";
      case PUT_AT_STRING_OP:     return "putAtString";

      case ALLOC_PTR_ARRAY_OP:   return "allocPtrArray";
      case COPY_PTR_ARRAY_OP:    return "copyPtrArray";
      case GET_AT_PTR_ARRAY_OP:  return "getAtPtrArray";
      case PUT_AT_PTR_ARRAY_OP:  return "putAtPtrArray";
      case GET_AT_PLAIN_PTR_ARRAY_OP:  
                                 return "getAtPlainPtrArray";
      case PUT_AT_PLAIN_PTR_ARRAY_OP:
                                 return "putAtPlainPtrArray";



#if (GARBAGE_COLLECTION==1) 
      case MARK_FREE_VARS_GC_OP: return "markFreeVars_GC";
      case MARK_PTR_GC_OP:       return "markPtr_GC";
      case NOT_MARK_PTR_GC_OP:   return "notMarkPtr_GC";
      case MARK_EXPR_PTR_GC_OP:  return "markExprPtr_GC";
      case NOT_MARK_EXPR_PTR_GC_OP:   
                                 return "notMarkExprPtr_GC";
      case MARK_EXPR_GC_OP:      return "markExpr_GC";
      case MARK_CELL_GC_OP:      return "markCell_GC";
      case MARK_STRING_GC_OP:    return "markString_GC";
#endif

      case ASSIGN_AT_OP:         return "assignAtPrim";
      case ERROR_OP:             return "error";
      case TRACE_OP:             return "trace";

      case SET_ENV_OP:           return "setEnv";

      case SPECIAL_EXPR_OP:      return "special";
      case ENTER_SPECIAL_EXPR_OP:      
                                 return "enterSpecial";

      case REV_REFS_OP:          return "revRefs";

      default:                   return "??";
   }
}

// Operator::inverse: Return logical inverse

Operator Operator::inverse(Void) const {
   switch (_kind) {
      case LT_OP:       return Operator(GTEQ_OP,      _type, 2);
      case LTEQ_OP:     return Operator(GT_OP,        _type, 2);
      case EQ_OP:       return Operator(NEQ_OP,       _type, 2);
      case NEQ_OP:      return Operator(EQ_OP,        _type, 2);
      case GTEQ_OP:     return Operator(LT_OP,        _type, 2);
      case GT_OP:       return Operator(LTEQ_OP,      _type, 2);

      case LTZ_OP:      return Operator(GTEQZ_OP,     _type, 1);
      case LTEQZ_OP:    return Operator(GTZ_OP,       _type, 1);
      case EQZ_OP:      return Operator(NEQZ_OP,      _type, 1);
      case NEQZ_OP:     return Operator(EQZ_OP,       _type, 1);
      case GTEQZ_OP:    return Operator(LTZ_OP,       _type, 1);
      case GTZ_OP:      return Operator(LTEQZ_OP,     _type, 1);

      case EQ_NULL_OP:  return Operator(NEQ_NULL_OP,  _type, 2);
      case NEQ_NULL_OP: return Operator(EQ_NULL_OP,   _type, 2);

#if (GARBAGE_COLLECTION==1)
      case MARK_PTR_GC_OP:
                        return Operator(NOT_MARK_PTR_GC_OP, _type, 1);
      case NOT_MARK_PTR_GC_OP: 
                        return Operator(MARK_PTR_GC_OP, _type, 1);
      case MARK_EXPR_PTR_GC_OP:
                        return Operator(NOT_MARK_EXPR_PTR_GC_OP, _type, 1);
      case NOT_MARK_EXPR_PTR_GC_OP:
                        return Operator(MARK_EXPR_PTR_GC_OP, _type, 1);
#endif

      default:          return Operator(UNKNOWN_OP, OP_TYPE_UNKNOWN, 0);
   }
}

// Const & ConstT

Void Const::allocateData(Code& code) const {
}

template<>
TypeSig ConstT<Name>::typeSig(Void) const {
   return typeCon(Name);
}

template<>
TypeSig ConstT<TypeConst>::typeSig(Void) const {
   return typeCon(Type);
}

template<>
TypeSig ConstT<Bits>::typeSig(Void) const {
   return typeCon(Bits);
}

template<>
TypeSig ConstT<Int>::typeSig(Void) const {
   return typeCon(Int);
}

template<>
TypeSig ConstT<Double>::typeSig(Void) const {
   return typeCon(Double);
}

template<>
TypeSig ConstT<Char>::typeSig(Void) const {
   return typeCon(Char);
}

template<>
TypeSig ConstT<String>::typeSig(Void) const {
   return typeCon(String);
}

template<>
TypeSig ConstT<Tag>::typeSig(Void) const {
   return TAG;
}

TypeSig Const_codeEnv::typeSig(Void) const {
   return ENV_PTR;
}

TypeSig Const_consEnv::typeSig(Void) const {
   return ENV_PTR;
}

TypeSig Const_NULL_Ptr::typeSig(Void) const {
   return _typeSig;
}

TypeSig Const_NULL_Env::typeSig(Void) const {
   return typeCon(Env);
}

TypeSig Const_NULL_REF::typeSig(Void) const {
   return typeCon(Ref);
}

template<>
Void ConstT<Name>::allocateData(Code& code) const {
   code.addNameConst(_value);
}

template<>
Void ConstT<TypeConst>::allocateData(Code& code) const {
   code.addTypeConst(_value._type);
}

template<>
Void ConstT<Bits>::allocateData(Code& code) const {
}

template<>
Void ConstT<Int>::allocateData(Code& code) const {
}

template<>
Void ConstT<Double>::allocateData(Code& code) const {
}

template<>
Void ConstT<Char>::allocateData(Code& code) const {
}

template<>
Void ConstT<String>::allocateData(Code& code) const {
}

template<>
Void ConstT<Tag>::allocateData(Code& code) const {
}


template<>
Void ConstT<Name>::print(ostream& os, const Env& env) const {
   printName(_value, os, env.nameTable());
}

template<>
Void ConstT<TypeConst>::print(ostream& os, const Env& env) const {
   printType(_value._type, os, env);
}

template<>
Void ConstT<Bits>::print(ostream& os, const Env& env) const {
   os << hex << _value << dec;
}

template<>
Void ConstT<Int>::print(ostream& os, const Env& env) const {
   os << _value;
}

template<>
Void ConstT<Double>::print(ostream& os, const Env& env) const {
   os << _value;
}

template<>
Void ConstT<Char>::print(ostream& os, const Env& env) const {
   os << '\'';
   insertEscape(_value, os);
   os << '\'';
}

template<>
Void ConstT<String>::print(ostream& os, const Env& env) const {
   insertEscapedString(_value, os);
}

template<>
Void ConstT<Tag>::print(ostream& os, const Env& env) const {
   os << "#" << _value;
}

Void Const_NULL_Ptr::print(ostream& os, const Env& env) const {
   os << "Null";
}

Void Const_NULL_Env::print(ostream& os, const Env& env) const {
   os << "Null";
}

template<>
Void ConstT<Name>::insert(ostream& os, const Code& code) const {
   os << "#";
   print(os, code.env());
}

template<>
Void ConstT<TypeConst>::insert(ostream& os, const Code& code) const {
   os << "#::";
   print(os, code.env());
}

template<>
Void ConstT<Bits>::insert(ostream& os, const Code&) const {
   os << "#0x" << hex << _value << dec;
}

template<>
Void ConstT<Int>::insert(ostream& os, const Code&) const {
   os << "#" << _value;
}

template<>
Void ConstT<Double>::insert(ostream& os, const Code&) const {
   os << "#" << _value;
}

template<>
Void ConstT<Char>::insert(ostream& os, const Code&) const {
   os << '\'';
   insertEscape(_value, os);
   os << '\'';
}

template<>
Void ConstT<String>::insert(ostream& os, const Code&) const {
   os << '#';
   insertEscapedString(_value, os);
}

template<>
Void ConstT<Tag>::insert(ostream& os, const Code&) const {
   os << "#" << _value;
}

Void Const_codeEnv::insert(ostream& os, const Code&) const {
   os << "@0";
}

Void Const_consEnv::insert(ostream& os, const Code&) const {
   os << "consEnv";
}

Void Const_NULL_Ptr::insert(ostream& os, const Code&) const {
   os << "#" << "NULL";
}

Void Const_NULL_Env::insert(ostream& os, const Code&) const {
   os << "#" << "NULL";
}

Void Const_NULL_REF::insert(ostream& os, const Code&) const {
   os << "#" << "NULL_REF";
}


// Operand

Operand::Operand(Kind kind)
 : _kind(kind) {
}

Operand::~Operand(Void) {
}

Operand* Operand::clone(MSA& msa) const {
   assert(FALSE, "Operand::clone: unexpected");
   return NULL;
}

Void Operand::destroy(MSA& msa) {
}

UInt Operand::varRefCount(Void) const {
   return 0;
}

Bool Operand::referencesVar(const Var& var) const {
   return FALSE;
}

Void Operand::setUD(Bool dest, BasicBlk& basicBlk, MSA& msa) {
}

Void Operand::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
}

Void Operand::setNextUse(Bool dest, MSA& msa) {
}

Void Operand::setNextUse(Code& code) const {
}

Void Operand::setKillPending(Void) const {
}

Void Operand::setReadyToKill(Bool envOf) const {
}

Void Operand::allocateData(Code& code) const {
}

Void Operand::insert(ostream& os, const Code& code) const {
}


// LabelOparand:

LabelOperand::LabelOperand(CodeLabel& codeLabel)
 : Operand(LABEL_OPERAND), _codeLabel(codeLabel) {
   _codeLabel.incRef();
}

LabelOperand::~LabelOperand(Void) {
   _codeLabel.decRef();
}

Void LabelOperand::destroy(MSA& msa) {
   _codeLabel.decRef(); // temporarily
                        //   operator delete(this, msa);
}

Void LabelOperand::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   if (_codeLabel.withBasicBlock())
      basicBlk.addSuccessor(_codeLabel.basicBlk(), msa);
}

Void LabelOperand::setNextUse(Bool dest, MSA& msa) {
   if (_codeLabel.withBasicBlock()) {
      VarElement* varElement = _codeLabel.basicBlk()._vars.head();
      while (varElement != NULL) {
         if (varElement->testFlags(VarElement::I) &&
            !varElement->testFlags(VarElement::D)) {
            Var& var = varElement->_var;
            _nextUses.addElement(var, msa)->_nextUse = var._nextUse;
            var.setNextUse(TRUE);
         }

         varElement = varElement->_next;
      }
   }
}

Void LabelOperand::setNextUse(Code& code) const {
   if (_codeLabel.withBasicBlock()) {
      VarElement* varElement = _codeLabel.basicBlk()._vars.head();
      while (varElement != NULL) {
         if (varElement->testFlags(VarElement::I) &&
            !varElement->testFlags(VarElement::D)) {
            Var& var = varElement->_var;
            var.setNextUse(_nextUses.findElement(var)->_nextUse);
            var.checkLive(code);
         }
         varElement = varElement->_next;
      }
   }
}

Void LabelOperand::insert(ostream& os, const Code& code) const {
   _codeLabel.insert(os, code);
}


// ConstOperand: Constant operand

Operand* ConstOperand::clone(MSA& msa) const {
   return new(msa) ConstOperand(_k);
}

Void ConstOperand::allocateData(Code& code) const {
   _k.allocateData(code);
}

Void ConstOperand::insert(ostream& os, const Code& code) const {
   _k.insert(os, code);
}

// VarOperand

VarOperand::VarOperand(Var& var, Operand::Kind kind/* = VAR_OPERAND*/, Bool reused/* = FALSE*/)
 : Operand(kind), _var(var),
   _needsEnvKnown(FALSE), _needsEnv(FALSE),
   _nextUse(FALSE), _closureNextUse(FALSE),
   _reused(reused),
   _reserved(var.reserved()) {
   var.adjRefCount(1);
   if (var.closure() != NULL)
      var.closure()->adjRefCount(1);
   if (_var._structTemplate != NULL)
      _var._structTemplate->adjUseCount(1);
}

Operand* VarOperand::clone(MSA& msa) const {
   return new(msa) VarOperand(_var, _kind);
}

Void VarOperand::destroy(MSA& msa) {
   _var.adjRefCount(-1);
   if (_var.kind() == Var::NON_GLOBAL_FREE_VAR)
       _var.closure()->adjRefCount(-1);
   if (_var.kind() == Var::CLOSURE_VAR &&
       _var.lambda() != NULL &&
       _var.lambda()->closure() == &_var)
      _var.adjEntrySkipCount(-1);
   if (_var._structTemplate != NULL)
      _var._structTemplate->adjUseCount(-1);
   //   operator delete (this, msa);
}

UInt VarOperand::varRefCount(Void) const {
   return _var.refCount();
}

Var* VarOperand::var(Void) const {
   return &_var;
}

Bool VarOperand::referencesVar(const Var& var) const {
   return &_var == &var;
}

// VarOperand::setU: Set U & D flags for operand

Void VarOperand::setUD(Bool dest, BasicBlk& basicBlk, MSA& msa) {
   if (_var.kind() != Var::GLOBAL_VAR) {
      VarElement* element = basicBlk.vars().addElement(_var, msa);
      if (_var.kind() == Var::NON_GLOBAL_FREE_VAR) {
         VarElement* element = basicBlk.vars().addElement(*_var.closure(), msa);
         element->setFlags(VarElement::U);
      }
      if (!dest)
         element->setFlags(VarElement::U);
      else {
         if (!element->testFlags(VarElement::U)) {
            if (_reused || _reserved)
               element->setFlags(VarElement::U);
            else
               element->setFlags(VarElement::D);
         }

         switch (_var.kind()) {
            case Var::EXTRA_ARG_VAR:
            case Var::ARG_VAR:
            case Var::CLOSURE_VAR:
            case Var::RESULT_VAR:
               element->setFlags(VarElement::X);
               break;

            default:
               break;
         }
      }
   }
}

Void VarOperand::setNextUse(Bool dest, MSA& msa) {
   _nextUse = _var._nextUse;
   _var.setNextUse(!dest || _reused || _reserved);
   if (_var.kind() == Var::NON_GLOBAL_FREE_VAR) {
      _closureNextUse = _var.closure()->_nextUse;
      _var.closure()->setNextUse(TRUE);
   }
}

Void VarOperand::setNextUse(Code& code) const {
   _var._nextUse = _nextUse;
   _var.checkLive(code);
   if (_var.kind() == Var::NON_GLOBAL_FREE_VAR) {
      _var.closure()->_nextUse = _closureNextUse;
      _var.closure()->checkLive(code);
   }
}

Void VarOperand::setKillPending(Void) const {
   if (!_nextUse)
      _var.setKillPending();
   if (_var.kind() == Var::NON_GLOBAL_FREE_VAR) {
      if (!_closureNextUse)
         _var.closure()->setKillPending();
   }
}

Void VarOperand::setReadyToKill(Bool envOf) const {
   if (_var._killPending) {
      _var.setReadyToKill(envOf);
      if (_var.kind() == Var::NON_GLOBAL_FREE_VAR &&
         _var.closure()->_killPending)
         _var.closure()->setReadyToKill(envOf);
   }
}

Void VarOperand::insert(ostream& os, const Code& code) const {
   _var.insert(os, code);
}

// StructOperand: Structure operand (an array of component operands)
// (allows nesting)

StructOperand::StructOperand(UInt nComps, Operand** comps, StructTemplate& structTemplate, TypeSig typeSig)
 : Operand(STRUCT_OPERAND), _nComps(nComps), _comps(comps),
   _structTemplate(structTemplate), _typeSig(typeSig) {
   _structTemplate.adjUseCount(1);
}

Operand* StructOperand::clone(MSA& msa) const {
   Operand** comps = (Operand**)msa.alloc(sizeof(Operand*) * _nComps);
   for (UInt i = 0; i < _nComps; i++)
      comps[i] = _comps[i]->clone(msa);
   return new(msa) StructOperand(_nComps, comps, _structTemplate, copyTypeSig(_typeSig, msa));
}

Void StructOperand::destroy(MSA& msa) {
   _structTemplate.adjUseCount(-1);
}

Bool StructOperand::referencesVar(const Var& var) const {
   for (UInt i = 0; i < _nComps; i++)
      if (_comps[i]->referencesVar(var))
         return TRUE;
   return FALSE;
}

// StructOperand::setU: Set U flag for components
// Always source

Void StructOperand::setUD(Bool dest, BasicBlk& basicBlk, MSA& msa) {
   for (UInt i = 0; i < _nComps; i++)
      _comps[i]->setUD(FALSE, basicBlk, msa);
}

Void StructOperand::setNextUse(Bool dest, MSA& msa) {
   for (UInt i = 0; i < _nComps; i++)
      _comps[i]->setNextUse(dest, msa);
}

Void StructOperand::setKillPending(Void) const {
   for (UInt i = 0; i < _nComps; i++)
      _comps[i]->setKillPending();
}

Void StructOperand::setReadyToKill(Bool envOf) const {
   for (UInt i = 0; i < _nComps; i++)
      _comps[i]->setReadyToKill(envOf);
}

Void StructOperand::setNextUse(Code& code) const {
   for (UInt i = 0; i < _nComps; i++)
      _comps[i]->setNextUse(code);
}

Void StructOperand::insert(ostream& os, const Code& code) const {
   for (UInt i = 0; i < _nComps; i++) {
      os << (i == 0 ? "(" : ", ");
      _comps[i]->insert(os, code);
   }
   os << ')';
}

// SelectOperand: Tuple component (or reference) operand

SelectOperand::SelectOperand(UInt index, Operand& operand,
                             TypeSig typeSig, Bool byPtr/* = FALSE*/)
 : Operand(SELECT_OPERAND), _index(index), _operand(&operand),
   _typeSig(typeSig),
   _byPtr(byPtr),
   _reprInfo(ReprInfo{ Repr::REPR_UNKNOWN, FALSE }) {
}


Operand* SelectOperand::clone(MSA& msa) const {
   return new(msa) SelectOperand(_index, *_operand->clone(msa), _typeSig, _byPtr);
}

Void SelectOperand::destroy(MSA& msa) {
   _operand->destroy(msa);
}

UInt SelectOperand::varRefCount(Void) const {
   return _operand->varRefCount();
}

Bool SelectOperand::referencesVar(const Var& var) const {
   return _operand->referencesVar(var);
}

// StructOperand::setUD: Set U & D flags for components
// Note: Only applies to the U flag of the tuple

Void SelectOperand::setUD(Bool dest, BasicBlk& basicBlk, MSA& msa) {
   _operand->setUD(FALSE, basicBlk, msa);
}

Void SelectOperand::setNextUse(Bool dest, MSA& msa) {
   _operand->setNextUse(dest, msa);
}

Void SelectOperand::setKillPending(Void) const {
   _operand->setKillPending();
}

Void SelectOperand::setReadyToKill(Bool envOf) const {
   _operand->setReadyToKill(envOf);
}

Void SelectOperand::setNextUse(Code& code) const {
   _operand->setNextUse(code);
}

Void SelectOperand::insert(ostream& os, const Code& code) const {
   if (_byPtr)
      os << '@';
   os << SELECT_PREFIX << _index << '(';
   _operand->insert(os, code);
   os << ')';
}

// DePtrOperand: Ptr pattern match operand

DePtrOperand::DePtrOperand(Operand& ptrOperand)
 : Operand(DEPTR_OPERAND),
   _operand(&ptrOperand)
{
}

Operand* DePtrOperand::clone(MSA& msa) const {
   return new(msa) DePtrOperand(*_operand->clone(msa));
}

Void DePtrOperand::destroy(MSA& msa) {
   _operand->destroy(msa);
}

UInt DePtrOperand::varRefCount(Void) const {
   return _operand->varRefCount();
}

Bool DePtrOperand::referencesVar(const Var& var) const {
   return _operand->referencesVar(var);
}

// DePtrOperand::setU: Set U flag for components
// Same comment applies as for SelectOperand

Void DePtrOperand::setUD(Bool dest, BasicBlk& basicBlk, MSA& msa) {
   _operand->setUD(FALSE, basicBlk, msa);
}

Void DePtrOperand::setNextUse(Bool dest, MSA& msa) {
   _operand->setNextUse(FALSE, msa);
}

Void DePtrOperand::setKillPending(Void) const {
   _operand->setKillPending();
}

Void DePtrOperand::setReadyToKill(Bool envOf) const {
   _operand->setReadyToKill(envOf);
}

Void DePtrOperand::setNextUse(Code& code) const {
   _operand->setNextUse(code);
}

Void DePtrOperand::insert(ostream& os, const Code& code) const {
   os << "DE_PTR(";
   _operand->insert(os, code);
   os << ')';
}

// ArrayIndexOperand: Array index operand

ArrayIndexOperand::ArrayIndexOperand(Operand& arrayOperand, Operand& indexOperand)
 : Operand(ARRAY_INDEX_OPERAND),
   _arrayOperand(&arrayOperand), _indexOperand(&indexOperand) {
}

Operand* ArrayIndexOperand::clone(MSA& msa) const {
   return new(msa) ArrayIndexOperand(*_arrayOperand->clone(msa),
      *_indexOperand->clone(msa));
}

Void ArrayIndexOperand::destroy(MSA& msa) {
   _arrayOperand->destroy(msa);
   _indexOperand->destroy(msa);
}

UInt ArrayIndexOperand::varRefCount(Void) const {
   return _arrayOperand->varRefCount() + _indexOperand->varRefCount();
}

Bool ArrayIndexOperand::referencesVar(const Var& var) const {
   return _arrayOperand->referencesVar(var) || _indexOperand->referencesVar(var);
}

// ArrayIndexOperand::setU: Set U flag for components
// Same comment applies as for SelectOperand

Void ArrayIndexOperand::setUD(Bool dest, BasicBlk& basicBlk, MSA& msa) {
   _arrayOperand->setUD(FALSE, basicBlk, msa);
   _indexOperand->setUD(FALSE, basicBlk, msa);
}

Void ArrayIndexOperand::setNextUse(Bool dest, MSA& msa) {
   _arrayOperand->setNextUse(FALSE, msa);
   _indexOperand->setNextUse(FALSE, msa);
}

Void ArrayIndexOperand::setKillPending(Void) const {
   _arrayOperand->setKillPending();
   _indexOperand->setKillPending();
}

Void ArrayIndexOperand::setReadyToKill(Bool envOf) const {
   _arrayOperand->setReadyToKill(envOf);
   _indexOperand->setReadyToKill(envOf);
}

Void ArrayIndexOperand::setNextUse(Code& code) const {
   _arrayOperand->setNextUse(code);
   _indexOperand->setNextUse(code);
}

Void ArrayIndexOperand::insert(ostream& os, const Code& code) const {
   Bool needsB = !(_arrayOperand->kind() == VAR_OPERAND ||
      _arrayOperand->kind() == CONST_OPERAND);
   if (needsB)
      os << '(';
   _arrayOperand->insert(os, code);
   os << '[';
   _indexOperand->insert(os, code);
   os << ']';
   if (needsB)
      os << ')';
}

#ifdef localConst
#undef localConst
#endif
#define localConst const


// BuiltInFnOperand: Built in function operand

BuiltInFnOperand::BuiltInFnOperand(Cell* builtInFn,
   Bool asClosure/* = TRUE */, Bool altEntryFlag/* = TRUE*/)
 : Operand(BUILT_IN_FN_OPERAND), _builtInFn(builtInFn),
   _asClosure(asClosure), _altEntryFlag(altEntryFlag) {
}

Operand* BuiltInFnOperand::clone(MSA& msa) const {
   return new(msa) BuiltInFnOperand(_builtInFn, _altEntryFlag);
}

Void BuiltInFnOperand::insert(ostream& os, const Code& code) const {
   os << builtInNameTable().string(toBody(_builtInFn, BuiltInFn).name());
}

// BuiltInDataConOperand: Built in data constructor operand

Operand* BuiltInDataConOperand::clone(MSA& msa) const {
   return new(msa) BuiltInDataConOperand(_dataCon);
}

Void BuiltInDataConOperand::insert(ostream& os, const Code& code) const {
   os << builtInNameTable().string(toBody(_dataCon, BuiltInDataCon).name());
}

#undef localConst
#define localConst

// ThisOperand: This operand

ThisOperand::ThisOperand(Void)
 : Operand(THIS_OPERAND) {
}

Void ThisOperand::insert(ostream& os, const Code& code) const {
   os << "this";
}

// CellInfoOperand: Cell info operand

CellInfoOperand::CellInfoOperand(CellInfo* cellInfo, CellInfo_* cellInfo_)
 : Operand(CELL_INFO_OPERAND),
   _cellInfo(cellInfo),
   _cellInfo_(cellInfo_) {
   if (cellInfo_ != NULL)
      cellInfo_->incRef();
}

Void CellInfoOperand::destroy(MSA& msa) {
   if (_cellInfo_ != NULL)
      _cellInfo_->decRef();
}

Void CellInfoOperand::insert(ostream& os, const Code& code) const {
   os << "cellInfo";
   if (_cellInfo != NULL) {
      os << '@' << hex << "0x" << _cellInfo << dec;
   }
   else {
      os << '(' << _cellInfo_->_seqNo << ')';
   }
}

// InstructionTrait

InstructionTrait::InstructionTrait(Void) {
}

InstructionTrait::~InstructionTrait(Void) {
}

Void InstructionTrait::destroy(MSA& msa) {
}

Bool InstructionTrait::usesVar(const Var& var) const {
   return FALSE;
}

Bool InstructionTrait::definesVar(const Var& var) const {
   return FALSE;
}

Void InstructionTrait::setUD(BasicBlk& basicBlk, MSA& msa) {
}

Void InstructionTrait::setNextUse(MSA& msa) {
}

Void InstructionTrait::insertSuffix(ostream& os, Code& Code) const {
}


Void InstructionTrait::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
}

Void InstructionTrait::allocateData(Code& code) const {
}



// Instruction (abstract)

Instruction::Instruction(Kind kind)
 : _kind(kind),
   _prev(NULL), _next(NULL),
   _seqNo(0),
   _lambda(NULL),
   _backPatchList(NULL) {
}

Void Instruction::insert(ostream& os, Code& code) const {
   //   if (_flags && FIRST)
   //      os << '*';
/*
   os << '(';
   if (_lambda != NULL)
      os << _lambda->_parent << ", ";
   os << _lambda << ") ";
*/
   os << _seqNo << ": ";
   os << mnemonic() << " ";
   insertSuffix(os, code);
   os << "\n";
   os.flush();
}



// InstrSeq - Instruction sequence

InstrSeq::InstrSeq(Void)
 : _first(NULL), _last(NULL), _appendSeq(NULL) {
}

Void InstrSeq::append(Instruction* ins) {
   if (_first != NULL) {
      _last->_next = ins;
      ins->_prev = _last;
   }
   else {
      _first = ins;
      ins->_prev = NULL;
   }
   ins->_next = NULL;
   _last = ins;
}

Void InstrSeq::insert(const InstrSeq& instrSeq, Instruction* beforeIns) {
   if (instrSeq._first != NULL) {
      if (beforeIns->_prev == NULL)
         _first = instrSeq._first;
      else {
         beforeIns->_prev->_next = instrSeq._first;
         instrSeq._first->_prev = beforeIns->_prev;
      }
 
      instrSeq._last->_next = beforeIns;
      beforeIns->_prev = instrSeq._last;
   }
}


Void InstrSeq::append(const InstrSeq& instrSeq) {
   if (_first != NULL) {
      if ((_last->_next = instrSeq._first) != NULL) {
         instrSeq._first->_prev = _last;
         _last = instrSeq._last;
      }
   }
   else {
      _first = instrSeq._first;
      _last = instrSeq._last;
   }
}


Void InstrSeq::append(InstrSeq* instrSeq) {
   InstrSeq** appendSeq = &_appendSeq;

   while (*appendSeq != NULL)
      appendSeq = &(*appendSeq)->_appendSeq;
   *appendSeq = instrSeq;
}

Void InstrSeq::append(Void) {
   while (_appendSeq != NULL) {
      append(*_appendSeq);
      _appendSeq = _appendSeq->_appendSeq;
   }
}

Void InstrSeq::insert(Instruction* ins, Instruction* beforeIns) {
   ins->_next = beforeIns;
   if ((ins->_prev = beforeIns->_prev) == NULL)
      _first = ins;
   else
      beforeIns->_prev->_next = ins;
   beforeIns->_prev = ins;
}

Void InstrSeq::append(Instruction* ins, Instruction* afterIns) {
   ins->_prev = afterIns;
   if ((ins->_next = afterIns->_next) == NULL)
      _last = ins;
   else
      afterIns->_next->_prev = ins;
   afterIns->_next = ins;
}

Void InstrSeq::remove(Instruction* ins) {
   if (ins->_prev == NULL)
      _first = ins->_next;
   else
      ins->_prev->_next = ins->_next;
   if (ins->_next != NULL)
      ins->_next->_prev = ins->_prev;
   else
      _last = ins->_prev;
}

// Compound data instruction trait

CompoundDataInstructionTrait::CompoundDataInstructionTrait(UInt nComps, Operand** comps, Int slot,
   StructTemplate& structTemplate)
 : InstructionTrait(), _nComps(nComps), _comps(comps), _slot(slot),
   _structTemplate(structTemplate),
   _next(NULL), _nests(NULL) {
   _structTemplate.adjUseCount(1);
}

CompoundDataInstructionTrait* CompoundDataInstructionTrait::getNest(UInt slot) const {
   for (CompoundDataInstructionTrait* nest = _nests;
      nest != NULL; nest = (CompoundDataInstructionTrait*)nest->_next)
      if (slot == _slot)
         return nest;
   return NULL;
}

Void CompoundDataInstructionTrait::destroy(MSA& msa) {
   _structTemplate.adjUseCount(-1);
}

Bool CompoundDataInstructionTrait::usesVar(const Var& var) const {
   for (UInt i = 0; i < _nComps; i++) {
      CompoundDataInstructionTrait* nest;
      if (_comps[i]->kind() == Operand::VAR_OPERAND &&
         (nest = getNest(i + 1)) != NULL &&
         nest->usesVar(var))
         return TRUE;
      else if (_comps[i]->referencesVar(var))
         return TRUE;
   }
   return FALSE;
}

Bool CompoundDataInstructionTrait::instructionDefinesVar(Instruction* ins) const {
   for (UInt i = 0; i < _nComps; i++)
      if (_comps[i]->kind() == Operand::VAR_OPERAND) {
         Var& var = ((VarOperand*)_comps[i])->var();
         CompoundDataInstructionTrait* nest = getNest(i + 1);
         if (nest == NULL && ins->definesVar(var) ||
            nest != NULL &&
            nest->instructionDefinesVar(ins))
            return TRUE;
      }
   return FALSE;
}

Void CompoundDataInstructionTrait::setUD(Bool dest, BasicBlk& basicBlk, MSA& msa) {
   for (UInt i = 0; i < _nComps; i++) {
      CompoundDataInstructionTrait* nest;
      if (_comps[i]->kind() == Operand::VAR_OPERAND &&
         (nest = getNest(i + 1)) != NULL)
         nest->setNextUse(msa);
      else
         _comps[i]->setUD(FALSE, basicBlk, msa);
   }
}

Void CompoundDataInstructionTrait::setNextUse(MSA& msa) {
   for (UInt i = 0; i < _nComps; i++) {
      CompoundDataInstructionTrait* nest;
      if (_comps[i]->kind() == Operand::VAR_OPERAND &&
         (nest = getNest(i + 1)) != NULL)
         nest->setNextUse(msa);
      else
         _comps[i]->setNextUse(FALSE, msa);
   }
}

Void CompoundDataInstructionTrait::setNextUse(Code& code) {
   for (UInt i = 0; i < _nComps; i++) {
      CompoundDataInstructionTrait* nest;
      if (_comps[i]->kind() == Operand::VAR_OPERAND &&
         (nest = getNest(i + 1)) != NULL)
         nest->setNextUse(code);
      else
         _comps[i]->setNextUse(code);
   }
}

Void CompoundDataInstructionTrait::insertSuffix(Int index, ostream& os, Code& code) const {
   if (index < 0)
      os << '(';
   for (UInt i = 0; i < _nComps; i++) {
      if (i > 0)
         os << ',';
      CompoundDataInstructionTrait* nest;
      if (_comps[i]->kind() == Operand::VAR_OPERAND &&
         (nest = getNest(i + 1)) != NULL)
         nest->insertSuffix((index < 0 ? 0 : index) + i, os, code);
      else {
         _comps[i]->setNextUse(code);
         _comps[i]->insert(os, code);
      }
   }
   if (index < 0)
      os << ')';
}

Void CompoundDataInstructionTrait::insertSuffix(ostream& os, Code& code) const {
   if (_nComps > 0)
      insertSuffix(-1, os, code);
}

// TypeDescrInstruction: Assign type descriptor methods

TypeDescrInstruction::TypeDescrInstruction(ConstOperand& type,
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
                                          )


 : Instruction(TYPE_DESCR_INSTR),
   _type(type),
   _denotedType(denotedType),
   _evalFn(evalFn),
   _mapFn(mapFn),
   _assignFn(assignFn),
   _insertTxtFn(insertTxtFn)

#if (SERIALISATION==1)
 , _extractBinFn(extractBinFn),
   _insertBinFn(insertBinFn)
#endif

#if (GARBAGE_COLLECTION==1)
 , _markFn(markFn)
#endif

{
}

const char* TypeDescrInstruction::mnemonic(Void) const {
   return "typeDescr";
}

Void TypeDescrInstruction::destroy(MSA& msa) {
   _type.destroy(msa);
   _denotedType.destroy(msa);
   if (_evalFn != NULL)
      _evalFn->destroy(msa);
   if (_mapFn != NULL)
      _mapFn->destroy(msa);
   if (_assignFn != NULL)
      _assignFn->destroy(msa);
   if (_insertTxtFn != NULL)
      _insertTxtFn->destroy(msa);

#if (SERIALISATION==1)
   if (_extractBinFn != NULL)
      _extractBinFn->destroy(msa);
   if (_insertBinFn != NULL)
      _insertBinFn->destroy(msa);
#endif

#if (GARBAGE_COLLECTION==1)
   if (_markFn != NULL)
      _markFn->destroy(msa);
#endif

}

Void TypeDescrInstruction::insertSuffix(ostream& os, Code& code) const {
   _type.insert(os, code);
   os << ',';
   _denotedType.insert(os, code);
   os << ',';
   if (_evalFn != NULL)
      _evalFn->insert(os, code);
   os << ',';
   if (_mapFn != NULL)
      _mapFn->insert(os, code);
   os << ',';
   if (_assignFn != NULL)
      _assignFn->insert(os, code);
   os << ',';
   if (_insertTxtFn != NULL)
      _insertTxtFn->insert(os, code);

#if (SERIALISATION==1)
   os << ',';
   if (_extractBinFn != NULL)
      _extractBinFn->insert(os, code);
   os << ',';
   if (_insertBinFn != NULL)
      _insertBinFn->insert(os, code);
#endif

#if (GARBAGE_COLLECTION==1)
   os << ',';
   if (_markFn != NULL)
      _markFn->insert(os, code);
#endif

}

// EntryInstuction: Not currently used, but included because it may be needed
//                  to define a source code built-in function

EntryInstruction::EntryInstruction(Name name, Type type, CodeLabel& codeLabel)
 : Instruction(ENTRY_INSTR),
   _name(name), _type(type), _codeLabel(codeLabel) {
   _codeLabel.incRef();
}

const char* EntryInstruction::mnemonic(Void) const {
   return "entry";
}

Void EntryInstruction::allocateData(Code& code) const {
   code.addNameConst(_name);
   code.addTypeConst(_type);
}

Void EntryInstruction::insertSuffix(ostream& os, Code& code) const {
   os << nameString_(_name, code.nameTable()) << ",";
   _codeLabel.insert(os, code);
}

ReserveInstruction::ReserveInstruction(VarOperand& x)
 : Instruction(RESERVE_INSTR), _x(&x), _x_env(NULL) {
   x.var().setReserved();
   x.setReserved(FALSE);

// Override the default reference count increment

   x.var()._refCount--;
}

const char* ReserveInstruction::mnemonic(Void) const {
   return "reserve";
}

Bool ReserveInstruction::usesVar(const Var& var) const {
   return FALSE;
}

Bool ReserveInstruction::definesVar(const Var& var) const {
   return _x->referencesVar(var);
}

Void ReserveInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(TRUE, basicBlk, msa);
}

Void ReserveInstruction::setNextUse(MSA& msa) {
   _x->setNextUse(TRUE, msa);
}

Void ReserveInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
}

// AssignInstruction: assign a value to a destination

AssignInstruction::AssignInstruction(Instruction::Kind kind, Operand& dst)
 : Instruction(kind), _dst(&dst) {
}

Void AssignInstruction::destroy(MSA& msa) {
   if (_dst != NULL)
      _dst->destroy(msa);
   //   operator delete(this, msa);
}

Bool AssignInstruction::usesVar(const Var& var) const {
   return _dst->referencesVar(var);
}

Bool AssignInstruction::definesVar(const Var& var) const {
   return _dst->kind() == Operand::VAR_OPERAND &&
      &((VarOperand&)*_dst).var() == &var;
}

Void AssignInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _dst->setUD(FALSE, basicBlk, msa);
   _dst->setUD(TRUE, basicBlk, msa);
}

Void AssignInstruction::setNextUse(MSA& msa) {
   _dst->setNextUse(TRUE, msa);
}

Void AssignInstruction::insertSuffix(ostream& os, Code& code) const {
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// MonadicInstruction: destination a function of a single argument

MonadicInstruction::MonadicInstruction(Instruction::Kind kind,
   Operand& src, Operand& dst)
 : Instruction(kind), _src(&src), _src_env(NULL), _dst(&dst) {
}

Void MonadicInstruction::destroy(MSA& msa) {
   if (_src != NULL)
      _src->destroy(msa);
   if (_dst != NULL)
      _dst->destroy(msa);
   //   operator delete(this, msa);
}

Void MonadicInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   if (_src->kind() == Operand::LABEL_OPERAND &&
      ((VarOperand*)_dst)->_var.kind() == Var::CONTINUATION_VAR) {
      BasicBlk& targetBasicBlk = ((LabelOperand*)_src)->_codeLabel.basicBlk();
      basicBlk.addSuccessor(targetBasicBlk, msa);
      targetBasicBlk._defer = TRUE;
   }
}

Bool MonadicInstruction::usesVar(const Var& var) const {
   return _src->referencesVar(var);
}

Bool MonadicInstruction::definesVar(const Var& var) const {
   return _dst->referencesVar(var);
}

Void MonadicInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _src->setUD(FALSE, basicBlk, msa);
   _dst->setUD(TRUE, basicBlk, msa);
}

Void MonadicInstruction::setNextUse(MSA& msa) {
   _dst->setNextUse(TRUE, msa);
   _src->setNextUse(FALSE, msa);
}

Void MonadicInstruction::insertSuffix(ostream& os, Code& code) const {
   _src->setNextUse(code);
   _src->insert(os, code);
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// MoveInstruction: monadic identity

MoveInstruction::MoveInstruction(Operand& src, Operand& dst)
 : MonadicInstruction(MOVE_INSTR, src, dst), _isConstructor(FALSE) {
}

MoveInstruction::MoveInstruction(Instruction::Kind kind,
   Operand& src, Operand& dst)
 : MonadicInstruction(kind, src, dst), _isConstructor(FALSE) {
}

const char* MoveInstruction::mnemonic(Void) const {
   return "move";
}

Void MoveInstruction::allocateData(Code& code) const {
   _src->allocateData(code);
}

// LVP_Instruction: monadic left value pointer

LVP_Instruction::LVP_Instruction(Operand& src, Operand& dst)
 : MoveInstruction(LVP_INSTR, src, dst) {
}

const char* LVP_Instruction::mnemonic(Void) const {
   return "lvp";
}

// ContinuationInstruction: create a continuation variable

ContinuationInstruction::ContinuationInstruction(LabelOperand& src,
                                                 VarOperand& dst

#if (CLOSURE_UPDATE==1)
                                                 , Bool forUpdate
#endif

#if (GARBAGE_COLLECTION==1)
                                                  , Bool gcFlag
#endif
)
 : MoveInstruction(CONTINUATION_INSTR, src, dst)

#if (CLOSURE_UPDATE==1)
   , _forUpdate(forUpdate)
#endif

#if (GARBAGE_COLLECTION==1)
   , _gcFlag(gcFlag)
#endif

{
}

const char* ContinuationInstruction::mnemonic(Void) const {
   return "continuation";
}

// ConsInstruction:

ConsInstruction::ConsInstruction(Operand& src, VarOperand& dst)
 : MoveInstruction(CONS_INSTR, src, dst) {
}

const char* ConsInstruction::mnemonic(Void) const {
   return "cons";
}

Void ConsInstruction::insertSuffix(ostream& os, Code& code) const {
   _src->setNextUse(code);
   _src->insert(os, code);
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// Cons_S_Instruction:

Cons_S_Instruction::Cons_S_Instruction(Operand& src, Operand& tag, VarOperand& dst)
 : MonadicInstruction(CONS_S_INSTR, src, dst), _tag(&tag) {
}

const char* Cons_S_Instruction::mnemonic(Void) const {
   return "cons_s";
}

Void Cons_S_Instruction::insertSuffix(ostream& os, Code& code) const {
   _src->setNextUse(code);
   _src->insert(os, code);
   os << ",";
   _tag->insert(os, code);
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// Cons_A_S_Instruction:

Cons_A_S_Instruction::Cons_A_S_Instruction(Operand& src, VarOperand& dst)
 : MoveInstruction(CONS_A_S_INSTR, src, dst) {
}

const char* Cons_A_S_Instruction::mnemonic(Void) const {
   return "cons_a_s";
}

// DeconsInstruction:

DeconsInstruction::DeconsInstruction(Operand& src, Bool byPtr, VarOperand& dst)
 : MoveInstruction(DECONS_INSTR, src, dst), _byPtr(byPtr) {
}

const char* DeconsInstruction::mnemonic(Void) const {
   return "decons";
}

Void DeconsInstruction::insertSuffix(ostream& os, Code& code) const {
   if (_byPtr)
      os << '@';
   _src->setNextUse(code);
   _src->insert(os, code);
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// Decons_S_Instruction:

Decons_S_Instruction::Decons_S_Instruction(Operand& src, Tag tag, Bool byPtr,
   VarOperand& dst)
 : MoveInstruction(DECONS_S_INSTR, src, dst), _tag(tag), _byPtr(byPtr) {
}

const char* Decons_S_Instruction::mnemonic(Void) const {
   return "decons_s";
}

Void Decons_S_Instruction::insertSuffix(ostream& os, Code& code) const {
   if (_byPtr)
      os << '@';
   _src->setNextUse(code);
   _src->insert(os, code);
   os << "," << _tag << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// FromPlainInstruction:

FromPlainInstruction::FromPlainInstruction(Operand& src, Operand& env,
                                           VarOperand& dst)
   : MoveInstruction(FROM_PLAIN_INSTR, src, dst), _env(&env) {
}

const char* FromPlainInstruction::mnemonic(Void) const {
   return "fromPlain";
}


Bool FromPlainInstruction::usesVar(const Var& var) const {
   return _env->referencesVar(var) ||
          MoveInstruction::usesVar(var);
}

Void FromPlainInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _env->setUD(FALSE, basicBlk, msa);
   MoveInstruction::setUD(basicBlk, msa);
}

Void FromPlainInstruction::setNextUse(MSA& msa) {
   MoveInstruction::setNextUse(msa);
   _env->setNextUse(FALSE, msa);
}

Void FromPlainInstruction::insertSuffix(ostream& os, Code& code) const {
   _src->setNextUse(code);
   _src->insert(os, code);
   _env->setNextUse(code);
   os << ",";
   _env->insert(os, code);
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// EnvOfInstruction:

EnvOfInstruction::EnvOfInstruction(Operand& src, VarOperand& dst)
 : MoveInstruction(ENV_OF_INSTR, src, dst) {
}

const char* EnvOfInstruction::mnemonic(Void) const {
   return "envOf";
}

// TagOfInstruction:

TagOfInstruction::TagOfInstruction(Operand& src, VarOperand& dst)
 : MoveInstruction(TAG_OF_INSTR, src, dst) {
}

const char* TagOfInstruction::mnemonic(Void) const {
   return "tagOf";
}

// CastTagInstruction:

CastTagInstruction::CastTagInstruction(Operand& src, VarOperand& dst)
 : MoveInstruction(CAST_TAG_INSTR, src, dst) {}

const char* CastTagInstruction::mnemonic(Void) const {
   return "castTag";
}

// ConstructFromNameInstruction:

ConstructFromNameInstruction::ConstructFromNameInstruction(Operand& src,
                                                           TypeSig typeSig,
                                                           VarOperand& dst)
 : MoveInstruction(CONSTRUCT_FROM_NAME_INSTR, src, dst), _typeSig(typeSig) {
}

const char* ConstructFromNameInstruction::mnemonic(Void) const {
   return "constructFromName";
}

Bool ConstructFromNameInstruction::needsSrcEnvLoc(Void) const {
   return TRUE;
}

Void ConstructFromNameInstruction::insertSuffix(ostream& os, Code& code) const {
   _src->setNextUse(code);
   _src->insert(os, code);
   os << ",";
   printTypeSig(_typeSig, 0, FALSE, os, code.env());
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// ConstructFromStringInstruction:

ConstructFromStringInstruction::ConstructFromStringInstruction(Operand& src,
                                                               TypeSig typeSig,
                                                               VarOperand& dst)
 : MoveInstruction(CONSTRUCT_FROM_STRING_INSTR, src, dst), _typeSig(typeSig) {
}

const char* ConstructFromStringInstruction::mnemonic(Void) const {
   return "constructFromString";
}

Void ConstructFromStringInstruction::insertSuffix(ostream& os, Code& code) const {
   _src->setNextUse(code);
   _src->insert(os, code);
   os << ",";
   printTypeSig(_typeSig, 0, FALSE, os, code.env());
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// PtrArrayAssignInstruction: Assign value at index of a pointer to an array

PtrArrayAssignInstruction::PtrArrayAssignInstruction(Operand& src, Operand& dst)
 : AssignInstruction(PTR_ARRAY_ASSIGN_INSTR, dst), _src(&src) {
}

const char* PtrArrayAssignInstruction::mnemonic(Void) const {
   return "putAtPtrArray";
}

Void PtrArrayAssignInstruction::destroy(MSA& msa) {
   if (_src != NULL)
      _src->destroy(msa);
   AssignInstruction::destroy(msa);
}

Bool PtrArrayAssignInstruction::usesVar(const Var& var) const {
   return _src->referencesVar(var) ||
      AssignInstruction::usesVar(var);
}

Void PtrArrayAssignInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _src->setUD(FALSE, basicBlk, msa);
   AssignInstruction::setUD(basicBlk, msa);
}

Void PtrArrayAssignInstruction::setNextUse(MSA& msa) {
   AssignInstruction::setNextUse(msa);
   _src->setNextUse(FALSE, msa);
}

Void PtrArrayAssignInstruction::insertSuffix(ostream& os, Code& code) const {
   _src->setNextUse(code);
   _src->insert(os, code);
   AssignInstruction::insertSuffix(os, code);
}

// StructInstruction: Construct compound data

StructInstruction::StructInstruction(StructOperand& src, Operand& dst)
 : Instruction(STRUCT_INSTR), _src(src),
   _dst(&dst) {
}

const char* StructInstruction::mnemonic(Void) const {
   return "struct";
}

Bool StructInstruction::usesVar(const Var& var) const {
   return _src.referencesVar(var);
}

Bool StructInstruction::definesVar(const Var& var) const {
   return _dst->referencesVar(var);
}

Void StructInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _src.setUD(FALSE, basicBlk, msa);
   _dst->setUD(TRUE, basicBlk, msa);
}

Void StructInstruction::setNextUse(MSA& msa) {
   _dst->setNextUse(TRUE, msa);
   _src.setNextUse(FALSE, msa);
}


Void StructInstruction::insertSuffix(ostream& os, Code& code) const {
   _src.setNextUse(code);
   _src.insert(os, code);
   os << ',';
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// EnterCellInstruction: Reduce a cell

EnterCellInstruction::EnterCellInstruction(Operand& x,
                                           Bool altEntryFlag/* = FALSE*/,
                                           Var* contVar/* = NULL*/

#if (CLOSURE_UPDATE==1)
                                         , Var* updateCellVar/* = NULL*/
#endif
                                          )
 : Instruction(ENTER_CELL_INSTR), _x(&x),
   _altEntryFlag(altEntryFlag),
   _contVar(contVar)

#if (CLOSURE_UPDATE==1)
 , _updateCellVar(updateCellVar)
#endif

   {
}

EnterCellInstruction::EnterCellInstruction(Instruction::Kind kind, Operand& x,
                                           Bool altEntryFlag/* = FALSE*/,
                                           Var* contVar/* = NULL*/

#if (CLOSURE_UPDATE==1)
                                         , Var* updateCellVar/* = NULL*/
#endif
                                          )
 : Instruction(kind), _x(&x),
   _altEntryFlag(altEntryFlag),
   _contVar(contVar)

#if (CLOSURE_UPDATE==1)
 , _updateCellVar(updateCellVar)
#endif

   {
}


const char* EnterCellInstruction::mnemonic(Void) const {
   return "enterCell";
}

Bool EnterCellInstruction::usesVar(const Var& var) const {
   return _x->referencesVar(var);
}

Void EnterCellInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(FALSE, basicBlk, msa);

   if (_contVar != NULL)
      basicBlk.vars().addElement(*_contVar, msa)->setFlags(VarElement::X);

#if (CLOSURE_UPDATE==1)
   if (_updateCellVar != NULL)
      basicBlk.vars().addElement(*_updateCellVar, msa)->setFlags(VarElement::X);
#endif
 
}

Void EnterCellInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   if (_contVar != NULL)
      basicBlk.addSuccessor(_contVar->_contLabel->basicBlk(), msa);
}

Void EnterCellInstruction::setNextUse(MSA& msa) {
   _x->setNextUse(FALSE, msa);
}

Void EnterCellInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
}


#if (GARBAGE_COLLECTION==1)
MarkCellInstruction::MarkCellInstruction(VarOperand& x, Var* contVar/* = NULL*/)
   : EnterCellInstruction(MARK_CELL_INSTR, x, TRUE, contVar) {
}

const char* MarkCellInstruction::mnemonic(Void) const {
   return "markCell";
}
#endif



// RSS_Instruction: Set return structure size

RSS_Instruction::RSS_Instruction(UInt size)
 : Instruction(RSS_INSTR), _size(size) {
}

const char* RSS_Instruction::mnemonic(Void) const {
   return "rss";
}

// ReturnInstruction: Return to previous continuation

ReturnInstruction::ReturnInstruction(Form form/* = GENERAL */,
                                     Var* contVar/* = NULL*/

#if (CLOSURE_UPDATE==1)
                                   , Var* updateCellVar/* = NULL*/
#endif
                                     )
 : Instruction(RETURN_INSTR), _form(form),
   _contVar(contVar)

#if (CLOSURE_UPDATE==1)
 , _updateCellVar(updateCellVar)
#endif

   {
}

const char* ReturnInstruction::mnemonic(Void) const {
   return "return";
}

Void ReturnInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   if (_contVar != NULL)
      basicBlk.vars().addElement(*_contVar, msa)->setFlags(VarElement::X);

#if (CLOSURE_UPDATE==1)
   if (_updateCellVar != NULL)
      basicBlk.vars().addElement(*_updateCellVar, msa)->setFlags(VarElement::X);
#endif

}

Void ReturnInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   if (_contVar != NULL)
      basicBlk.addSuccessor(_contVar->_contLabel->basicBlk(), msa);
}

// GoToInstruction

GoToInstruction::GoToInstruction(Operand& target,
                                 Bool altEntryFlag/* = FALSE*/,
                                 Var* contVar/* = NULL*/

#if (CLOSURE_UPDATE==1)
                               , Var* updateCellVar/* = NULL*/
#endif

                                )
 : Instruction(GOTO_INSTR), _target(&target),
   _altEntryFlag(altEntryFlag),
   _contVar(contVar)

#if (CLOSURE_UPDATE==1)
 , _updateCellVar(updateCellVar)
#endif

   {
}

GoToInstruction::~GoToInstruction(Void) {
}

Void GoToInstruction::destroy(MSA& msa) {
   if (_target != NULL)
      _target->destroy(msa);
   //   operator delete(this, msa);
}

const char* GoToInstruction::mnemonic(Void) const {
   return "goTo";
}

Bool GoToInstruction::usesVar(const Var& var) const {
   return target().referencesVar(var);
}

Void GoToInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   target().setUD(FALSE, basicBlk, msa);

   if (_contVar != NULL)
      basicBlk.vars().addElement(*_contVar, msa)->setFlags(VarElement::X);

#if (CLOSURE_UPDATE==1)
   if (_updateCellVar != NULL)
      basicBlk.vars().addElement(*_updateCellVar, msa)->setFlags(VarElement::X);
#endif

}

Void GoToInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   target().setSuccessor(basicBlk, msa);

   if (_contVar != NULL)
      basicBlk.addSuccessor(_contVar->_contLabel->basicBlk(), msa);
}

Void GoToInstruction::setNextUse(MSA& msa) {
   ((LabelOperand&)target()).setNextUse(FALSE, msa);
}

Void GoToInstruction::insertSuffix(ostream& os, Code& code) const {
   target().insert(os, code);
}

// CaseInstruction:: Multiple branch

CaseInstruction::CaseInstruction(Operand& x, UInt n, ConstOperand** consts,
   LabelOperand** labels, LabelOperand* otherwise)
 : Instruction(CASE_INSTR), _x(&x), _x_env(NULL),
   _n(n), _consts(consts), _labels(labels),
   _otherwise(otherwise) {
}


Void CaseInstruction::destroy(MSA& msa) {
   if (_x != NULL)
      _x->destroy(msa);
   for (UInt i = 0; i < _n; i++)
      _labels[i]->destroy(msa);
   if (_otherwise != NULL)
      _otherwise->destroy(msa);
}

const char* CaseInstruction::mnemonic(Void) const {
   return "case";
}

Bool CaseInstruction::usesVar(const Var& var) const {
   return _x->referencesVar(var);
}

Void CaseInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(FALSE, basicBlk, msa);
   for (UInt i = 0; i < _n; i++)
      _labels[i]->setUD(FALSE, basicBlk, msa);
   if (_otherwise != NULL)
      _otherwise->setUD(FALSE, basicBlk, msa);
}

Void CaseInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   for (UInt i = 0; i < _n; i++)
      _labels[i]->setSuccessor(basicBlk, msa);
   if (_otherwise != NULL)
      _otherwise->setSuccessor(basicBlk, msa);
}

Void CaseInstruction::setNextUse(MSA& msa) {
   _x->setNextUse(FALSE, msa);
   for (UInt i = 0; i < _n; i++)
      _labels[i]->setNextUse(FALSE, msa);
   if (_otherwise != NULL)
      _otherwise->setNextUse(FALSE, msa);
}

Void CaseInstruction::allocateData(Code& code) const {
   for (UInt i = 0; i < _n; i++)
      _consts[i]->allocateData(code);
}

Void CaseInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
   for (UInt i = 0; i < _n; i++) {
      os << ",(";
      _consts[i]->insert(os, code);
      os << ',';
      _labels[i]->insert(os, code);
      os << ')';
   }
   if (_otherwise != NULL) {
      os << ' ';
      _otherwise->insert(os, code);
   }
}

// MonadicOpInstruction: Monadic operator

MonadicOpInstruction::MonadicOpInstruction(Operator op, Operand& src, Operand& dst)
 : MonadicInstruction(MONADIC_OP_INSTR, src, dst), _op(op) {
}

const char* MonadicOpInstruction::mnemonic(Void) const {
   return _op.mnemonic();
}

// UpdateInstruction: Closure update

UpdateInstruction::UpdateInstruction(Operand& src, Operand& dst)
 : MonadicInstruction(UPDATE_INSTR, src, dst) {
}

const char* UpdateInstruction::mnemonic(Void) const {
   return "update";
}

Bool UpdateInstruction::usesVar(const Var& var) const {
   return _src->referencesVar(var) || _dst->referencesVar(var);
}

Void UpdateInstruction::genSourceCode(ostream& os, SourceCode& code) const {
   os << "update(";
   _src->genSourceCode(os, code);
   os << ", ";
   _dst->genSourceCode(os, code);
   os << ");\n";
}

// MonadicOpActionInstruction: Void operator action

MonadicOpActionInstruction::MonadicOpActionInstruction(Operator op, Operand& x)
 : Instruction(MONADIC_OP_ACTION_INSTR), _op(op), _x(&x), _x_env(NULL) {
}

const char* MonadicOpActionInstruction::mnemonic(Void) const {
   return _op.mnemonic();
}

Bool MonadicOpActionInstruction::usesVar(const Var& var) const {
   return _x->referencesVar(var);
}

Void MonadicOpActionInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(FALSE, basicBlk, msa);
}

Void MonadicOpActionInstruction::setNextUse(MSA& msa) {
   _x->setNextUse(FALSE, msa);
}

Void MonadicOpActionInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
}

// DyadicOpInstruction: Dyadic operator instruction

DyadicOpInstruction::DyadicOpInstruction(Operator op,
   Operand& x, Operand& y, Operand& dst)
 : Instruction(DYADIC_OP_INSTR), _op(op),
   _x(&x), _x_env(NULL), _y(&y), _y_env(NULL), _dst(&dst) {
}

MapGCInstruction::MapGCInstruction(Operator op,
   Operand& x, Operand& y, Operand& dst)
 : DyadicOpInstruction(op, x, y, dst) {
}

const char* DyadicOpInstruction::mnemonic(Void) const {
   return _op.mnemonic();
}

Void DyadicOpInstruction::destroy(MSA& msa) {
   if (_x != NULL)
      _x->destroy(msa);
   if (_y != NULL)
      _y->destroy(msa);
   if (_dst != NULL)
      _dst->destroy(msa);
// operator delete(this, msa);
}

Bool DyadicOpInstruction::usesVar(const Var& var) const {
   return _x->referencesVar(var) || _y->referencesVar(var);
}

Bool DyadicOpInstruction::definesVar(const Var& var) const {
   return _dst->referencesVar(var);
}

Void DyadicOpInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(FALSE, basicBlk, msa);
   _y->setUD(FALSE, basicBlk, msa);
   _dst->setUD(TRUE, basicBlk, msa);
}

Void DyadicOpInstruction::setNextUse(MSA& msa) {
   _dst->setNextUse(TRUE, msa);
   _y->setNextUse(FALSE, msa);
   _x->setNextUse(FALSE, msa);
}

Void DyadicOpInstruction::allocateData(Code& code) const {
   _x->allocateData(code);
   _y->allocateData(code);
}

Void DyadicOpInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
   os << ",";
   _y->setNextUse(code);
   _y->insert(os, code);
   os << ",";
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// DyadicOpActionInstruction: Void triadic operator

DyadicOpActionInstruction::DyadicOpActionInstruction(Operator op,
                                                     Operand& x, Operand& y)
 : Instruction(DYADIC_OP_ACTION_INSTR), _op(op),
   _x(&x), _x_env(NULL), _y(&y), _y_env(NULL) {
}

const char* DyadicOpActionInstruction::mnemonic(Void) const {
   return _op.mnemonic();
}

Bool DyadicOpActionInstruction::usesVar(const Var& var) const {
   return _x->referencesVar(var) ||
      _y->referencesVar(var);
}

Bool DyadicOpActionInstruction::definesVar(const Var& var) const {
   return FALSE;
}

Void DyadicOpActionInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(FALSE, basicBlk, msa);
   _y->setUD(FALSE, basicBlk, msa);
}

Void DyadicOpActionInstruction::setNextUse(MSA& msa) {
   _y->setNextUse(FALSE, msa);
   _x->setNextUse(FALSE, msa);
}

Void DyadicOpActionInstruction::allocateData(Code& code) const {
   _x->allocateData(code);
   _y->allocateData(code);
}

Void DyadicOpActionInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
   os << ",";
   _y->setNextUse(code);
   _y->insert(os, code);
}

// TriadicOpActionInstruction: Void triadic operator

TriadicOpActionInstruction::TriadicOpActionInstruction(Operator op,
   Operand& x, Operand& y, Operand& z)
 : Instruction(TRIADIC_OP_ACTION_INSTR), _op(op),
   _x(&x), _y(&y), _z(&z) {
}

const char* TriadicOpActionInstruction::mnemonic(Void) const {
   return _op.mnemonic();
}

Bool TriadicOpActionInstruction::usesVar(const Var& var) const {
   return _x->referencesVar(var) ||
      _y->referencesVar(var) ||
      _z->referencesVar(var);
}

Bool TriadicOpActionInstruction::definesVar(const Var& var) const {
   return FALSE;
}

Void TriadicOpActionInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(FALSE, basicBlk, msa);
   _y->setUD(FALSE, basicBlk, msa);
   _z->setUD(FALSE, basicBlk, msa);
}

Void TriadicOpActionInstruction::setNextUse(MSA& msa) {
   _z->setNextUse(FALSE, msa);
   _y->setNextUse(FALSE, msa);
   _x->setNextUse(FALSE, msa);
}

Void TriadicOpActionInstruction::allocateData(Code& code) const {
   _x->allocateData(code);
   _y->allocateData(code);
   _z->allocateData(code);
}

Void TriadicOpActionInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
   os << ",";
   _y->setNextUse(code);
   _y->insert(os, code);
   os << ",";
   _z->setNextUse(code);
   _z->insert(os, code);
}

// QuinadicOpActionInstruction: Void quinary operator

QuinadicOpActionInstruction::QuinadicOpActionInstruction(Operator op, Operand& v, Operand& w,
   Operand& x, Operand& y, Operand& z)
 : Instruction(QUINADIC_OP_ACTION_INSTR), _op(op),
   _v(&v), _v_env(NULL),
   _w(&w), _w_env(NULL),
   _x(&x), _x_env(NULL),
   _y(&y), _y_env(NULL),
   _z(&z), _z_env(NULL) {
}

const char* QuinadicOpActionInstruction::mnemonic(Void) const {
   return _op.mnemonic();
}

Bool QuinadicOpActionInstruction::usesVar(const Var& var) const {
   return _v->referencesVar(var) ||
      _w->referencesVar(var) ||
      _x->referencesVar(var) ||
      _y->referencesVar(var) ||
      _z->referencesVar(var);
}

Bool QuinadicOpActionInstruction::definesVar(const Var& var) const {
   return FALSE;
}

Void QuinadicOpActionInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _v->setUD(FALSE, basicBlk, msa);
   _w->setUD(FALSE, basicBlk, msa);
   _x->setUD(FALSE, basicBlk, msa);
   _y->setUD(FALSE, basicBlk, msa);
   _z->setUD(FALSE, basicBlk, msa);
}

Void QuinadicOpActionInstruction::setNextUse(MSA& msa) {
   _z->setNextUse(FALSE, msa);
   _y->setNextUse(FALSE, msa);
   _x->setNextUse(FALSE, msa);
   _w->setNextUse(FALSE, msa);
   _v->setNextUse(FALSE, msa);
}

Void QuinadicOpActionInstruction::allocateData(Code& code) const {
   _v->allocateData(code);
   _w->allocateData(code);
   _x->allocateData(code);
   _y->allocateData(code);
   _z->allocateData(code);
}

Void QuinadicOpActionInstruction::insertSuffix(ostream& os, Code& code) const {
   _v->setNextUse(code);
   _v->insert(os, code);
   os << ",";
   _w->setNextUse(code);
   _w->insert(os, code);
   os << ",";
   _x->setNextUse(code);
   _x->insert(os, code);
   os << ",";
   _y->setNextUse(code);
   _y->insert(os, code);
   os << ",";
   _z->setNextUse(code);
   _z->insert(os, code);
}

// ConditionalInstruction: Conditional branch instruction

ConditionalInstruction::ConditionalInstruction(Operator op, Operand& x,
                                               LabelOperand& z, Bool divergent, MSA& msa)
   : Instruction(COND_INSTR), _op(op),
   _x(&x), _x_env(NULL),
   _y(NULL), _y_env(NULL),
   _z(&z),
   _divergent(divergent),
   _opCodeStr(NULL) {
   setOpCodeStr(msa);
}

ConditionalInstruction::ConditionalInstruction(Operator op, Operand& x, Operand& y,
                                               LabelOperand& z, Bool divergent, MSA& msa)
 : Instruction(COND_INSTR), _op(op),
   _x(&x), _x_env(NULL),
   _y(&y), _y_env(NULL),
   _z(&z),
   _divergent(divergent),
   _opCodeStr(NULL) {
   setOpCodeStr(msa);
}

Void ConditionalInstruction::setOpCodeStr(MSA& msa) {
   if (_opCodeStr != NULL)
      msa.free(_opCodeStr);
   stringstream strStream;
   strStream << 'b' << _op.mnemonic();
   _opCodeStr = (String)msa.alloc(strlen(strStream.str().c_str()) + 1);
   strcpy(_opCodeStr, strStream.str().c_str());
}

Void ConditionalInstruction::destroy(MSA& msa) {
   _x->destroy(msa);
   if (_y != NULL)
      _y->destroy(msa);

   if (_z != NULL)
      _z->destroy(msa);

//   operator delete(this, msa);
}

const char* ConditionalInstruction::mnemonic(Void) const {
   return _opCodeStr;
}

Bool ConditionalInstruction::usesVar(const Var& var) const {
   return _x->referencesVar(var) ||
      (_y != NULL && _y->referencesVar(var));
}

Void ConditionalInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _x->setUD(FALSE, basicBlk, msa);
   if (_y != NULL)
      _y->setUD(FALSE, basicBlk, msa);
}

Void ConditionalInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   _z->setSuccessor(basicBlk, msa);
}

Void ConditionalInstruction::setNextUse(MSA& msa) {
   _z->setNextUse(FALSE, msa);
   assert(_z->kind() == Operand::LABEL_OPERAND,
      "ConditionalInstruction::setNextUse: Expected label operand");
   if (_y != NULL)
      _y->setNextUse(FALSE, msa);
   _x->setNextUse(FALSE, msa);
}

Void ConditionalInstruction::allocateData(Code& code) const {
   _x->allocateData(code);
   if (_y != NULL)
      _y->allocateData(code);
}

Void ConditionalInstruction::insertSuffix(ostream& os, Code& code) const {
   _x->setNextUse(code);
   _x->insert(os, code);
   if (_y != NULL) {
      os << ",";
      _y->setNextUse(code);
      _y->insert(os, code);
   }
   os << ",";
   _z->setNextUse(code);
   _z->insert(os, code);
}

// LabelInstruction: A labelled instruction point

LabelInstruction::LabelInstruction(CodeLabel& codeLabel)
 : Instruction(LABEL_INSTR), _codeLabel(codeLabel) {
   _codeLabel.incRef();
}

Void LabelInstruction::destroy(MSA& msa) {
   _codeLabel.decRef();
//    operator delete(this, msa);
}

const char* LabelInstruction::mnemonic(Void) const {
   return "label";
}

Void LabelInstruction::insertSuffix(ostream& os, Code& code) const {
   if (_codeLabel.withBasicBlock()) {
      BasicBlk& basicBlk = _codeLabel.basicBlk();
      code._basicBlk = &basicBlk;
      if (!basicBlk._locCodeGenerated)
         basicBlk.liveVarInfo(code);
   }
   _codeLabel.insert(os, code);
   if (_codeLabel._head)
      os << "/head";
}

#if (GARBAGE_COLLECTION==1)

// FrameDescriptorInstruction: Provides information for an activation frame

FrameDescrInstruction::FrameDescrInstruction(LabelOperand& labOperand,
                                             GC_StubInstruction& gcStubInstr)
 : Instruction(FRAME_DESCR_INSTR), 
   _labOperand(labOperand),
   _gcStubInstr(gcStubInstr) {
}

const char* FrameDescrInstruction::mnemonic(Void) const {
   return "frameDescriptor";
}

Void FrameDescrInstruction::destroy(MSA& msa) {
   _labOperand.destroy(msa);
   _gcStubInstr.kill();
}

Void FrameDescrInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _labOperand.setUD(FALSE, basicBlk, msa);
}

Void FrameDescrInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   _labOperand.setSuccessor(basicBlk, msa);
}

Void FrameDescrInstruction::setNextUse(MSA& msa) {
   _labOperand.setNextUse(FALSE, msa);
}

Void FrameDescrInstruction::insertSuffix(ostream& os, Code& code) const {
   _labOperand.setNextUse(code);
   _labOperand.insert(os, code);
}


#endif

// HeapAllocInstruction: Allocate heap data

HeapAllocInstruction::HeapAllocInstruction(VarOperand& dst,
                                           Operand* env/* = NULL*/)
 : Instruction(HEAP_ALLOC_INSTR), _env(env), _dst(&dst),
   _dstEnv(NULL) {
}

HeapAllocInstruction::HeapAllocInstruction(Kind kind, size_t size,
                                           VarOperand& dst,
                                           Operand* env/* = NULL*/)
 : Instruction(kind), _size(size), _env(env), _dst(&dst),
   _dstEnv(NULL) {
}

Void HeapAllocInstruction::destroy(MSA& msa) {
   if (_env != NULL)
      _env->destroy(msa);
   if (_dst != NULL)
      _dst->destroy(msa);
   if (_dstEnv != NULL)
      _dstEnv->destroy(msa);
//   operator delete(this, msa);
}

Bool HeapAllocInstruction::usesVar(const Var& var) const {
   return _env != NULL &&
          _env->referencesVar(var);
}

Bool HeapAllocInstruction::definesVar(const Var& var) const {
   return _dst->referencesVar(var);
}

Void HeapAllocInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   if (_env != NULL)
      _env->setUD(FALSE, basicBlk, msa);
   _dst->setUD(TRUE, basicBlk, msa);
}

Void HeapAllocInstruction::setNextUse(MSA& msa) {
   _dst->setNextUse(TRUE, msa);
   if (_env != NULL)
      _env->setNextUse(FALSE, msa);
}

Void HeapAllocInstruction::insertSuffix(ostream& os, Code& code) const {
   if (_env != NULL) {
      _env->setNextUse(code);
      _env->insert(os, code);
       os << ',';
   }
   _dst->setNextUse(code);
   _dst->insert(os, code);
}

// CellInfoInstruction: Allocate cell info structure

CellInfoInstruction::CellInfoInstruction(CellInfo_& cellInfo, StructTemplate& structTemplate)
 : Instruction(CELL_INFO_INSTR), _cellInfo(cellInfo), _structTemplate(structTemplate) {
   cellInfo.incRef();
   structTemplate.adjUseCount(1);
}

Void CellInfoInstruction::destroy(MSA& msa) {
   _cellInfo.decRef();
   _structTemplate.adjUseCount(-1);
   //   operator delete(this, msa);
}

const char* CellInfoInstruction::mnemonic(Void) const {
   return "cellInfo";
}

Void CellInfoInstruction::insertSuffix(ostream& os, Code& code) const {
   os << _cellInfo._seqNo << ',';
   _cellInfo.insert(os, code);
}

// CellAllocInstruction: Allocate cell

CellAllocInstruction::CellAllocInstruction(StructTemplate& structTemplate,
                                           Operand& cellInfo,
                                           VarOperand& dst,
                                           Operand* env/* = NULL */)
 : HeapAllocInstruction(CELL_ALLOC_INSTR, 0, dst,
                        env),
   _structTemplate(structTemplate), _cellInfo(&cellInfo),
   _lambda(dst.var().lambda()) {
   structTemplate.adjUseCount(1);

   if (_lambda != NULL)
      _lambda->adjRefCount(1);
}

Void CellAllocInstruction::destroy(MSA& msa) {
   if (_lambda != NULL) 
      _lambda->adjRefCount(-1);
   _cellInfo->destroy(msa);
   _structTemplate.adjUseCount(-1);
   HeapAllocInstruction::destroy(msa);
}

const char* CellAllocInstruction::mnemonic(Void) const {
   return "cellAlloc";
}

Bool CellAllocInstruction::usesVar(const Var& var) const {
   return _cellInfo->referencesVar(var);
}

Void CellAllocInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _cellInfo->setUD(FALSE, basicBlk, msa);
   HeapAllocInstruction::setUD(basicBlk, msa);
}

Void CellAllocInstruction::setNextUse(MSA& msa) {
   _cellInfo->setNextUse(FALSE, msa);
   HeapAllocInstruction::setNextUse(msa);
}

Void CellAllocInstruction::insertSuffix(ostream& os, Code& code) const {
   os << _structTemplate._id;
   os << ',';
   _cellInfo->setNextUse(code);
   _cellInfo->insert(os, code);
   HeapAllocInstruction::insertSuffix(os, code);
}

// PtrInstruction: Ptr constructor

PtrInstruction::PtrInstruction(Kind kind, VarOperand& dst)
 : HeapAllocInstruction(kind, 0, dst, NULL) {
}

const char* PtrInstruction::mnemonic(Void) const {
   return "ptr";
}

// PtrArrayInstruction: Pointer array allocation

PtrArrayAllocInstruction::PtrArrayAllocInstruction(Operand& n, VarOperand& dst)
   : PtrInstruction(PTR_ARRAY_ALLOC_INSTR, dst), _n(&n) {}

const char* PtrArrayAllocInstruction::mnemonic(Void) const {
   return "ptrArrayAlloc";
}

Void PtrArrayAllocInstruction::destroy(MSA& msa) {
   if (_n != NULL)
      _n->destroy(msa);
   PtrInstruction::destroy(msa);
   //   operator delete(this, msa);
}

Bool PtrArrayAllocInstruction::usesVar(const Var& var) const {
   return _n->referencesVar(var);
}

Bool PtrArrayAllocInstruction::definesVar(const Var& var) const {
   return PtrInstruction::definesVar(var);
}

Void PtrArrayAllocInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   _n->setUD(FALSE, basicBlk, msa);
   PtrInstruction::setUD(basicBlk, msa);
}

Void PtrArrayAllocInstruction::setNextUse(MSA& msa) {
   PtrInstruction::setNextUse(msa);
   _n->setNextUse(FALSE, msa);
}

Void PtrArrayAllocInstruction::insertSuffix(ostream& os, Code& code) const {
   _n->setNextUse(code);
   _n->insert(os, code);
   os << ",";
   PtrInstruction::insertSuffix(os, code);
}

// IndirectionAllocInstruction: Allocate indirection cell

IndirectionAllocInstruction::IndirectionAllocInstruction(VarOperand& dst)
   : HeapAllocInstruction(INDIRECTION_ALLOC_INSTR, 0, dst, NULL) {}

const char* IndirectionAllocInstruction::mnemonic(Void) const {
   return "indirectionAlloc";
}

// TagInstruction: Tag native values to expression

TagInstruction::TagInstruction(Operand& src, VarOperand& dst)
 : MoveInstruction(TAG_INSTR, src, dst), _srcRepr(Repr::REPR_UNKNOWN) {
}

const char* TagInstruction::mnemonic(Void) const {
   return "tag";
}

Void TagInstruction::allocateData(Code& code) const {
   _src->allocateData(code);
}

// ToCellInstruction: Cast from expression to cell

ToCellInstruction::ToCellInstruction(VarOperand& src, VarOperand& dst)
 : MonadicInstruction(TO_CELL_INSTR, src, dst) {
}

const char* ToCellInstruction::mnemonic(Void) const {
   return "toCell";
}

ExceptionInstruction::ExceptionInstruction(Void)
 : Instruction(EXCEPTION_INSTR) {
}

const char* ExceptionInstruction::mnemonic(Void) const {
   return "exception";
}

// ArgCheckInstruction: Check for partial application

ArgCheckInstruction::ArgCheckInstruction(UInt n, LabelOperand** labels, CodeLabel& altEntryLab)
 : Instruction(ARG_CHECK_INSTR), _n(n), _labels(labels), _altEntryLab(altEntryLab),
   _partialDepths(NULL) {
}

const char* ArgCheckInstruction::mnemonic(Void) const {
   return "argCheck";
}

Bool ArgCheckInstruction::usesVar(const Var& var) const {
   return FALSE;
}

Void ArgCheckInstruction::setUD(BasicBlk& basicBlk, MSA& msa) {
   for (UInt i = 0; i < _n; i++)
      _labels[i]->setUD(FALSE, basicBlk, msa);
}

Void ArgCheckInstruction::setSuccessor(BasicBlk& basicBlk, MSA& msa) {
   for (UInt i = 0; i < _n; i++)
      _labels[i]->setSuccessor(basicBlk, msa);

}

Void ArgCheckInstruction::setNextUse(MSA& msa) {
   for (UInt i = 0; i < _n; i++)
      _labels[i]->setNextUse(FALSE, msa);
}

Void ArgCheckInstruction::allocateData(Code& code) const {
   /*
      for (UInt i = 0; i < _n; i++)
         _consts[i]->allocateData(code);
   */
}

Void ArgCheckInstruction::insertSuffix(ostream& os, Code& code) const {
   os << _n;
   if (_n > 0) {
      os << ",[";
      for (UInt i = 0; i < _n; i++) {
         if (i > 0)
            os << ',';
         _labels[i]->insert(os, code);

      }
      os << ']';
   }
}

// EmptyArgsInstruction: Empty argument stack

EmptyArgsInstruction::EmptyArgsInstruction(Bool needsArgBase)
 : Instruction(EMPTY_ARGS_INSTR),
   _needsArgBase(needsArgBase), _diff(0) {
}

const char* EmptyArgsInstruction::mnemonic(Void) const {
   return "emptyArgs";
}

// SqueezeInstruction: Empty argument stack

SqueezeInstruction::SqueezeInstruction(UInt from, UInt to, UInt size)
 : Instruction(SQUEEZE_INSTR), _from(from), _to(to), _size(size) {
}


const char* SqueezeInstruction::mnemonic(Void) const {
   return "squeeze";
}

#if (GARBAGE_COLLECTION==1)

// GC_StubInstruction: Placeholder for garbage collection code

GC_StubInstruction::GC_StubInstruction(Void)
 : Instruction(GC_STUB_INSTR), _isLive(TRUE) {
}

const char* GC_StubInstruction::mnemonic(Void) const {
   return "gc_stub";
}

#endif

// Code

Code::Code(ConstString options, Env& env, MSA& msa)
 : _options(options),
   _env(env), _msa(msa),
   _nErrors(0), _nWarnings(0),
   _moduleDefn(NULL),
   _anyType(NULL_TYPE),
   _types(NULL),
   _structTemplates(NULL), _lambda(NULL), _returnState(NULL),   
   _basicBlks(NULL), _basicBlk(NULL),

#if (CLOSURE_UPDATE==1)
   _updateCellVar(NULL),
   _updateContVar(NULL),
#endif

   _instrSeqNo(0), _labelSeqNo(0),
   _execSeqFlag(FALSE),
   _nameConstV(16), _nNameConsts(0), _typeConstV(16), _nTypeConsts(0),
   _errLab(NULL),
   _directMainEntryContext(NULL)

#if (GARBAGE_COLLECTION==1)
   , _gcFlag(FALSE)
#endif

{

#ifdef TRACE
   OptionsParser optionsParser(_options);
   if (optionsParser.hasKey("code")) {
      traceFlag = optionsParser.hasKey("trace");
      optionsParser.getNum("probe", instructionProbeSeqNo);
   }
   else
      traceFlag = FALSE;
#endif

   _instrSeq = new(msa) InstrSeq();
}

Void Code::init(Bool extendSuccessorsFlag) {

#ifdef PEEPHOLE_OPTIMISE
   OptionsParser options(_options);
   Int maxPeepholeOptimisationPasses;

   if (!options.getNum("maxPeep", maxPeepholeOptimisationPasses))
      maxPeepholeOptimisationPasses = DEFAULT_MAX_PEEPHOLE_OPTIMISATION_PASSES;
#endif

#ifdef REARRANGE_BLOCKS
   UInt n;
   if ((n = rearrangeBlocks()) > 0) {
      outStream << n << " block(s) moved\n";
      outStream.flush();
   }
#endif

#ifdef PEEPHOLE_OPTIMISE
   UInt nPasses = 0;
   while (nPasses < static_cast<UInt>(maxPeepholeOptimisationPasses)
          && peepholeOptimise(++nPasses)) {};
   
   if (traceFlag) {
      outStream << ">>>>>> " << nPasses << " peephole optimisation pass" << (nPasses == 1 ? "" : "es");
      outStream << '\n';
      outStream.flush();
   }

#endif

   BasicBlk* basicBlk = _basicBlks;
   while (basicBlk != NULL) {
      basicBlk->setUD(*this);
      basicBlk = basicBlk->_next;
   }
   /*
   do {
   headBasicBlocks();
   } while (flagIndirectContinuations());
   */
   allocateDataAndLabels();

#ifdef TRACE_EXTRA
   if (traceFlag)
      printBlockInfo();
#endif

   setInOut();

   if (extendSuccessorsFlag) {

#ifdef TRACE
      if (traceFlag) {
         outStream << "Before extendSuccessors\n";
         outStream.flush();
      }
#endif

      extendSuccessors();

#ifdef TRACE
      if (traceFlag) {
         outStream << "After extendSuccessors\n";
         outStream.flush();
      }
#endif

   }

#ifdef TRACE
   if (traceFlag)
      printBlockInfo();
#endif

}

// Code::error: Increment the error count and report an error

Void Code::error(const char* s) {
   _nErrors++;
   ivoryError(s);
}

Void Code::genTarget(ostream& os) {

#ifdef TRACE
   OptionsParser options(_options);
   if (options.hasKey("code")) {
      traceFlag = options.hasKey("trace");
      options.getNum("probe", instructionProbeSeqNo);
   }
   else
      traceFlag = FALSE;
#endif

   init(TRUE);

   listStructTemplates(os);

   for (Instruction* ins = _instrSeq->first(); ins != NULL; ins = ins->_next) {

#ifdef TRACE
      if (ins->_seqNo == instructionProbeSeqNo) {
         outStream << "Code::genTarget: probe hit\n";
         outStream.flush();
         traceFlag = TRUE;
      }

#endif

      ins->insert(os, *this);
   }
}

Void Code::listStructTemplates(ostream& os) {
   StructTemplate* structTemplate = _structTemplates;

   UInt count = 0;
   while (structTemplate != NULL) {
      structTemplate->_id = count++;
      os << "structTemplate " << count;
      for (UInt i = 0; i < structTemplate->_nSlots; i++) {
         os << ",";
         printType(structTemplate->_slotTypes[i], os);
      }
      structTemplate = structTemplate->_next;
      os << '\n';
   }
}

Void Code::printType(Type type, ostream& os) const {
   if (type == builtInType(Label))
      os << "Label";
   else  if (type == builtInType(CellInfo))
      os << "CellInfo*";
   else if (type != NULL_TYPE)
      ::printType(type, os, env());
   else
      os << "Unknown";
}

// Allocate an appropriate type descriptor for target

TypeDescr* Code::allocTypeDescr(TypeSig typeSig, MSA& msa) const {
   return new(msa) TypeDescr(typeSig);
}

LabelOperand& Code::labelOperand(CodeLabel& codeLabel) const {
   return *new(_msa) LabelOperand(codeLabel);
}

ConstOperand& Code::constOperand(Const& const_) const {
   return *new(_msa) ConstOperand(const_);
}

ConstOperand& Code::constOperand(Tag tag) const {
   return constOperand(*new(_msa) ConstT<Tag>(tag));
}

VarOperand& Code::varOperand(Var& var, Bool reused/* = FALSE*/) const {
   return *new(_msa) VarOperand(var, Operand::VAR_OPERAND, reused);
}

StructOperand& Code::structOperand(UInt nComps, Operand** comps, StructTemplate& structTemplate, TypeSig typeSig) const {
   return *new(_msa) StructOperand(nComps, comps, structTemplate, typeSig);
}

SelectOperand& Code::selectOperand(UInt index, Operand* operand,
                                   TypeSig typeSig, Bool byPtr/* = FALSE*/) const {
   return *new(_msa) SelectOperand(index, *operand, typeSig, byPtr);
}

DePtrOperand& Code::dePtrOperand(Operand& ptrOperand) const {
   return *new(_msa) DePtrOperand(ptrOperand);
}

ArrayIndexOperand& Code::arrayIndexOperand(Operand& arrayOperand, Operand& indexOperand) const {
   return *new(_msa) ArrayIndexOperand(arrayOperand, indexOperand);
}

ThisOperand& Code::thisOperand(Void) const {
   return *new(_msa) ThisOperand();
}

CellInfoOperand& Code::cellInfoOperand(CellInfo* cellInfo, CellInfo_* cellInfo_) const {
   return *new(_msa) CellInfoOperand(cellInfo, cellInfo_);
}


Void Code::addInstruction(Instruction* ins) {
   ins->_seqNo = ++_instrSeqNo;

#ifdef TRACE
   if (_instrSeqNo == instructionProbeSeqNo) {
      outStream << "*** Code::addInstruction: Probe hit\n";
      outStream.flush();
      traceFlag = TRUE;
   }
   if (traceFlag) {
      outStream << "Added instruction: " << _instrSeqNo << '\n';
      outStream.flush();
   }
#endif

   ins->_lambda = _lambda;
   _instrSeq->append(ins);
   switch (ins->kind()) {
      case Instruction::GOTO_INSTR:
      case Instruction::RETURN_INSTR:
      case Instruction::CASE_INSTR:
      case Instruction::ENTER_CELL_INSTR:

#if (GARBAGE_COLLECTION==1)
      case Instruction::MARK_CELL_INSTR:
#endif

      case Instruction::EXCEPTION_INSTR:
         _execSeqFlag = FALSE;
         _instrSeq->append();
         break;

      default:
         break;
   }
   if (_basicBlk != NULL)
      _basicBlk->_lastIns = ins;
}

Void Code::insertInstruction(Instruction* ins, Instruction* beforeIns) {

// Assume not block termination kind

   ins->_lambda = beforeIns->_lambda;
   _instrSeq->insert(ins, beforeIns);
}

Void Code::appendInstruction(Instruction* ins, Instruction* afterIns) {

// Assume not block termination kind

   ins->_lambda = afterIns->_lambda;
   _instrSeq->append(ins, afterIns);
}

Void Code::removeInstruction(Instruction* ins, Bool destroy/* = TRUE*/) {

#ifdef TRACE
   if (ins->_seqNo == -1) {
      outStream << "*** Code::removeInstruction: Probe hit\n";
      outStream.flush();
      traceFlag = TRUE;
   }
   if (traceFlag) {
      outStream << "Remove instruction: " << ins->_seqNo << '\n';
      outStream.flush();
   }
#endif

   _instrSeq->remove(ins);
   if (destroy)
      ins->destroy(msa());
}

Void Code::removeInstructions(Instruction* fromIns, Instruction* toIns) {
   Instruction* prev = fromIns->_prev;
   Instruction* next = toIns->_next;
   if (prev == NULL)
      _instrSeq->_first = next;
   else
      prev->_next = next;
   if (next == NULL)
      _instrSeq->_last = prev;
   else
      next->_prev = prev;
   for (Instruction* ins = fromIns; ; ins = ins->_next) {
      ins->destroy(msa());
      if (ins == toIns)
         break;
   }
}

Void Code::addGoToInstruction(Operand& target,
                              Bool altEntryFlag/* = FALSE*/,
                              Var* contVar/* = NULL*/

#if (CLOSURE_UPDATE==1)
                              , Var* updateCellVar/* = NULL*/
#endif
                              ) {
   addInstruction(new(msa()) GoToInstruction(target, altEntryFlag, contVar

#if (CLOSURE_UPDATE==1)
                                           , updateCellVar
#endif   
                                             ));
   
}

Void Code::addContinuationInstruction(LabelOperand& src, VarOperand& dst

#if (CLOSURE_UPDATE==1)
                                      , Bool forUpdate
#endif

#if (GARBAGE_COLLECTION==1)
                                      , Bool gcFlag
#endif
   
                                      ) {
   addInstruction(new(msa()) ContinuationInstruction(src, dst
      
#if (CLOSURE_UPDATE==1)      
                                                     , forUpdate
#endif

#if (GARBAGE_COLLECTION==1)
                                                     , gcFlag
#endif
                                                     ));
}

Void Code::addCellAllocInstruction(StructTemplate& structTemplate,
                                   Operand& cellInfo,
                                   VarOperand& dst,
                                   Operand* dstEnv/* = NULL */) {
   addInstruction(new(msa()) 
      CellAllocInstruction(structTemplate, cellInfo,
                           dst,
                           dstEnv));
}

Void Code::addIndirectionAllocInstruction(VarOperand& dst) {
   addInstruction(new(msa()) IndirectionAllocInstruction(dst));
}

#if (CLOSURE_UPDATE==1)
Void Code::addUpdateInstruction(Operand& src, Operand& dst) {

#if (GARBAGE_COLLECTION==1)
   CodeLabel& gcLabel = newLabel(FALSE, NULL, FALSE);
   addInstruction(new(msa()) FrameDescrInstruction(labelOperand(gcLabel), genGC_stub()));
#endif

   addInstruction(new(msa()) UpdateInstruction(src, dst));

#if (GARBAGE_COLLECTION==1)
   genLabel(gcLabel);
#endif
}
#endif

Void Code::addUnaryInstruction(Instruction::Kind kind, Operand& x) {
   Instruction* ins;
   switch (kind) {
   case Instruction::GOTO_INSTR:
      ins = new(msa()) GoToInstruction(x);
      break;

   case Instruction::ENTER_CELL_INSTR:
      ins = new(msa()) EnterCellInstruction((VarOperand&)x);
      break;

   default:
      error("Code::addUnaryInstruction: unexpected");
      return;
   }
   addInstruction(ins);
}

Void Code::addBinaryInstruction(Instruction::Kind kind,
                                Operand& x, Operand& y) {
   Instruction* ins;
   switch (kind) {
      case Instruction::MOVE_INSTR:
         ins = new(msa()) MoveInstruction(x, (VarOperand&)y);
         break;

      case Instruction::CONS_A_S_INSTR:
         ins = new(msa()) Cons_A_S_Instruction((ConstOperand&)x, (VarOperand&)y);
         break;

      case Instruction::ENV_OF_INSTR:
         ins = new(msa()) EnvOfInstruction(x, (VarOperand&)y);
         break;

      case Instruction::TAG_OF_INSTR:
         ins = new(msa()) TagOfInstruction(x, (VarOperand&)y);
         break;

      case Instruction::CONSTRUCT_FROM_NAME_INSTR:
         error("Code::addBinaryInstruction: constructFromName requires a type");
         return;

      case Instruction::CONSTRUCT_FROM_STRING_INSTR:
         error("Code::addBinaryInstruction: constructFromString requires a type");
         return;

      case Instruction::TAG_INSTR:
         ins = new(msa()) TagInstruction(x, (VarOperand&)y);
         break;

      case Instruction::TO_CELL_INSTR:
         ins = new(msa()) ToCellInstruction((VarOperand&)x, (VarOperand&)y);
         break;

      default:
         error("Code::addBinaryInstruction: unexpected");
         return;
   }
   addInstruction(ins);
}

// addMoveInstruction

Void Code::addMoveInstruction(Operand& src, Operand& dst) {
   addInstruction(src.kind() == Operand::SELECT_OPERAND &&
                  ((SelectOperand&)src).byPtr()
      ? (Instruction*)new(msa()) LVP_Instruction(src, dst)
      : (Instruction*)new(msa()) MoveInstruction(src, dst));
}

Void Code::addUnaryConditionalInstruction(Operator op, Operand& x, CodeLabel& codeLabel,
                                          Bool divergent) {
   addInstruction(new(msa()) ConditionalInstruction(op, x, labelOperand(codeLabel),
                                                    divergent, msa()));
}

Void Code::addBinaryConditionalInstruction(Operator op, Operand& x, Operand& y, CodeLabel& codeLabel,
                                           Bool divergent) {
   addInstruction(new(msa()) ConditionalInstruction(op, x, y, labelOperand(codeLabel),
                                                    divergent, msa()));
}

// addPtrArrayAssignInstruction

Void Code::addPtrArrayAssignInstruction(Operand& src, Operand& dst) {
   addInstruction(new(msa()) PtrArrayAssignInstruction(src, dst));
}

Void Code::addCellTemplate(StructTemplate* structTemplate) {
   StructTemplate** pCellTemplate = &_structTemplates;
   while (*pCellTemplate != NULL)
      pCellTemplate = &(*pCellTemplate)->_next;

   structTemplate->_next = NULL;
   *pCellTemplate = structTemplate;
}

StructTemplate& Code::structTemplate(Type type) {
   StructTemplateAllocator structTemplateAllocator = StructTemplateAllocator(1, *this);
   structTemplateAllocator.slot(0, type);
   return structTemplateAllocator.structTemplate();
}

ConstT<TypeConst>& Code::makeTypeConst(Type type) {
   return *new(msa()) ConstT<TypeConst>(type);
}

Expr Code::makeConst(Expr expr, Bool assignable/* = TRUE*/) {
   if (isPair(expr)) {
      if (fst(expr) == Expr(LITERAL))
         expr = snd(expr);
      else if (fst(expr) == Expr(TYPE_SIG))
         expr = fromType(useType((TypeSig)snd(expr), assignable));
   }

   if (/*expr == dataCon(Null) ||*/
      expr == Nil) {
      Cell& cell_ = *new(sizeof(Const_NULL_Ptr), msa()) Cell(CONST);
      new(cell_.body()) Const_NULL_Ptr(ERROR);
      return fromCell(cell_);
   } if (expr == dataCon(NullRef)) {
      Cell& cell_ = *new(sizeof(Const_NULL_REF), msa()) Cell(CONST);
      new(cell_.body()) Const_NULL_REF();
      return fromCell(cell_);
   }
   else switch (formOf(expr)) {
      case NAME: {
         Cell& cell_ = *new(sizeof(ConstT<Name>), msa()) Cell(CONST);
         new(cell_.body()) ConstT<Name>(toName(expr));
         return fromCell(cell_);
      }

      case TYPE: {
         Cell& cell_ = *new(sizeof(ConstT<TypeConst>), msa()) Cell(CONST);
         new(cell_.body()) ConstT<TypeConst>(toType(expr));
         return fromCell(cell_);
      }

      case INT: {
         Cell& cell_ = *new(sizeof(ConstT<Int>), msa()) Cell(CONST);
         new(cell_.body()) ConstT<Int>(toInt(expr));
         return fromCell(cell_);
      }

      case CHAR: {
         Cell& cell_ = *new(sizeof(ConstT<Char>), msa()) Cell(CONST);
         new(cell_.body()) ConstT<Char>(toChar(expr));
         return fromCell(cell_);
      }

      case TAG: {
         Cell& cell_ = *new(sizeof(ConstT<Tag>), msa()) Cell(CONST);
         new(cell_.body()) ConstT<Tag>(toTag(expr));
         return fromCell(cell_);
      }

      case DATA: {
         Type type_ toType(exprCellInfo(expr).form());
         if (type_ == builtInType(Bits)) {
            Cell& cell_ = *new(sizeof(ConstT<Bits>), msa()) Cell(CONST);
            new(cell_.body()) ConstT<Bits>(toBits(expr));
            return fromCell(cell_);
         } else if (type_ == builtInType(Int)) {
            Cell& cell_ = *new(sizeof(ConstT<Int>), msa()) Cell(CONST);
            new(cell_.body()) ConstT<Int>(toInt(expr));
            return fromCell(cell_);
         } else if (type_ == builtInType(Double)) {
            Cell& cell_ = *new(sizeof(ConstT<Double>), msa()) Cell(CONST);
            new(cell_.body()) ConstT<Double>(toDouble(expr));
            return fromCell(cell_);
         } else if (type_ == builtInType(String)) {
            Cell& cell_ = *new(sizeof(ConstT<String>), msa()) Cell(CONST);
            new(cell_.body()) ConstT<String>(toString(expr));
            return fromCell(cell_);
         }
      }
   }
   return (Expr)ERROR;
}

CodeLabel& Code::newLabel(Bool head/* = FALSE*/,
                          Lambda* lambda/* = NULL*/,
                          Bool withBasicBlock/* = TRUE*/) {

   BasicBlk* basicBlk = NULL;

   if (withBasicBlock) {

//--- Append new basic block to list

      basicBlk = new (_msa) BasicBlk();
      BasicBlk** basicBlkPtr = &_basicBlks;
      BasicBlk* prev = NULL;
      while (*basicBlkPtr != NULL) {
         prev = *basicBlkPtr;
         basicBlkPtr = &prev->_next;
      }
      basicBlk->_prev = prev;
      *basicBlkPtr = basicBlk;
   }

   CodeLabel* codeLabel = new (_msa) CodeLabel(basicBlk, head, lambda);

   return *codeLabel;
}

Bool Code::flagIndirectContinuations(Void) const {
   const Instruction* ins;
   CodeLabel* headCodeLabel = NULL;
   Bool res = FALSE;
   for (ins = _instrSeq->first(); ins != NULL; ins = ins->_next)
      switch (ins->_kind) {
      case Instruction::LABEL_INSTR: {
         LabelInstruction& labelIns = (LabelInstruction&)*ins;
         if (labelIns._codeLabel._head)
            headCodeLabel = labelIns._codeLabel._headCodeLabel;
         break;
      }

      case Instruction::GOTO_INSTR: {
         GoToInstruction& goToIns = (GoToInstruction&)*ins;
         if (goToIns.target().kind() == Operand::LABEL_OPERAND) {
            CodeLabel& codeLabel = ((LabelOperand&)goToIns.target()).codeLabel();
            if (!codeLabel._head &&
               codeLabel._headCodeLabel != headCodeLabel) {
               res = TRUE;
               codeLabel._head = TRUE;
               return TRUE;;
            }
         }
         break;
      }

      case Instruction::COND_INSTR: {
         CodeLabel& codeLabel = ((ConditionalInstruction&)*ins)._z->codeLabel();
         if (!codeLabel._head && codeLabel._headCodeLabel != headCodeLabel) {
            res = TRUE;
            codeLabel._head = TRUE;
            return TRUE;
         }
         break;
      }

      case Instruction::CASE_INSTR: {
         CaseInstruction& caseIns = (CaseInstruction&)*ins;
         for (UInt i = 0; i < caseIns._n; i++) {
            CodeLabel& codeLabel = caseIns._labels[i]->codeLabel();
            if (!codeLabel._head && codeLabel._headCodeLabel != headCodeLabel) {
               res = TRUE;
               codeLabel._head = TRUE;
               return TRUE;
            }
         }
         if (caseIns._otherwise != NULL) {
            CodeLabel& codeLabel = caseIns._otherwise->codeLabel();
            if (!codeLabel._head && codeLabel._headCodeLabel != headCodeLabel) {
               res = TRUE;
               codeLabel._head = TRUE;
               return TRUE;
            }
         }
         break;
      }

      default:
         break;
      }
   return res;
}

// Set head basic block for all basic blocks

Void Code::headCodeLabels(Void) const {
   const Instruction* ins;
   CodeLabel* headCodeLabel = NULL;
   for (ins = _instrSeq->first(); ins != NULL; ins = ins->_next)
      switch (ins->_kind) {
      case Instruction::LABEL_INSTR: {
         CodeLabel& codeLabel = ((LabelInstruction&)*ins).codeLabel();
         if (codeLabel._head)
            headCodeLabel = &codeLabel;
         codeLabel._headCodeLabel = headCodeLabel;
         break;
      }

      default:
         break;
      }
}

/*
* For given blocks 'i' and 'j' flagged as a head, move any block
* 'k' which is not flagged as a head, but is a successor to 'i' and
* not 'j', so that 'k' precedes 'j'.
*
* If required, any cell info instructions between 'i' and 'k' are also moved
* before 'i'.
*
*/

#ifdef REARRANGE_BLOCKS

UInt Code::rearrangeBlocks(Void) {
   Instruction* i, * j, * k;
   BasicBlk* basicBlk = NULL;
   UInt n = 0;
   i = NULL;

   for (i = _instrSeq->first(); i != NULL; i = i->_next)
      if (i->_kind == Instruction::LABEL_INSTR &&
         ((LabelInstruction*)i)->_basicBlk._head)
         break;
l:
   if (i == NULL)
      return n;

   for (j = i->_next; j != NULL; j = j->_next)
      if (j->_kind == Instruction::LABEL_INSTR &&
         (((LabelInstruction*)j)->_basicBlk._head ||
            ((LabelInstruction*)j)->_basicBlk._lambda != NULL))

         break;
   if (j == NULL)
      return n;
   k = j->_next;
   BasicBlkSet basicBlks;
   for (; k != NULL; k = k->_next)
      if (k->_kind == Instruction::LABEL_INSTR &&
         !((LabelInstruction*)k)->_basicBlk._head &&
         ((LabelInstruction*)k)->_basicBlk.implicitSuccessor() == NULL) {
         BasicBlk& iLab = ((LabelInstruction*)i)->_basicBlk;
         BasicBlk& jLab = ((LabelInstruction*)j)->_basicBlk;
         BasicBlk& kLab = ((LabelInstruction*)k)->_basicBlk;

         if ((iLab.hasSuccessor(kLab) ||
            basicBlks.findElement(kLab) != NULL) &&
            !jLab.hasSuccessor(kLab) &&
            kLab._lastIns != NULL) {
            outStream << "Moving block @ " << kLab._ins->_seqNo << '\n';
            outStream.flush();

            //            moveCellInfoInstructions(iLab._lastIns->_next, k, i);
            Instruction* prev1 = jLab._ins->_prev;
            prev1->_next = kLab._ins;
            jLab._ins->_prev = kLab._lastIns;

            Instruction* prev2 = kLab._ins->_prev;

            Instruction* next = kLab._lastIns->_next;
            prev2->_next = next;
            kLab._ins->_prev = prev1;

            kLab._lastIns->_next = jLab._ins;
            if (next != NULL)
               next->_prev = prev2;
            basicBlks.join(kLab.successors(), msa());
            n++;
         }
      }
   i = j;
   goto l;
}

// Ensure that all cell info instructions precede a head label

Void Code::moveCellInfoInstructions(Instruction* ins1,
   Instruction* ins2,
   Instruction* beforeIns) {
   for (Instruction* ins = ins1; ins != ins2;) {
      Instruction* next = ins->_next;
      if (ins->_kind == Instruction::CELLINFO_INSTR) {
         Instruction* prev = ins->_prev;
         prev->_next = next;
         next->_prev = prev;

         if ((prev = beforeIns->_prev) != NULL)
            prev->_next = ins;
         else
            _instrSeq->_first = ins;
         ins->_prev = prev;
         ins->_next = beforeIns;
         beforeIns->_prev = ins;
      }
      ins = next;
   }
}

#endif

Void Code::removeBasicBlk(BasicBlk& basicBlk) {
   if (&basicBlk == _basicBlks) {
      _basicBlks = basicBlk._next;
      if (basicBlk._next != NULL)
         basicBlk._next->_prev = NULL;
   }
   else {
      basicBlk._prev->_next = basicBlk._next;
      if (basicBlk._next != NULL)
         basicBlk._next->_prev = basicBlk._prev;
   }
}

static Bool unusedFreeVar(const Var* closure, UInt index) {
/* T.B.D. This needs a review
   if (closure != NULL) {
      FreeVarAssoc* freeVarAssoc = closure->lambda()->freeVarAssocs();
      while (freeVarAssoc != NULL) {
         Var& localVar = toBody(freeVarAssoc->localTypedVal().val(), Var);
         if (localVar._index == index)
            return localVar.refCount() == 0;
         freeVarAssoc = freeVarAssoc->next();
      }
   }
*/
   return FALSE;
}

Bool Code::unusedLocal(const VarOperand& dst, Instruction* ins, BasicBlk* basicBlk) {
   if ((dst._var.kind() == Var::LOCAL_VAR ||
        dst._var.kind() == Var::LOCAL_ARG_VAR ||
        dst._var.kind() == Var::NON_GLOBAL_FREE_VAR &&
        unusedFreeVar(dst._var.closure(), dst._var._index)) &&
       dst._var._refCount <= 1) {

#ifdef TRACE
      if (traceFlag) {
         outStream << "Peephole optimise: removing unused instruction " << ins->_seqNo << '\n';
         outStream.flush();
      }
#endif

#if (GARBAGE_COLLECTION==1)
      if (ins->kind() == Instruction::DYADIC_OP_INSTR) {
         DyadicOpInstruction& dyadicOpIns = (DyadicOpInstruction&)*ins;
         Instruction* prevIns = ins->_prev;
         if (dyadicOpIns.op().mapNeedsGCFrame() &&
             prevIns != NULL &&
             prevIns->kind() == Instruction::FRAME_DESCR_INSTR)
            basicBlk->removeInstruction(prevIns, *this);
      }
#endif

      basicBlk->removeInstruction(ins, *this);
      return TRUE;
   }
   return FALSE;
}

UInt Code::peepholeJump(Operand** operand, Bool unique, BasicBlk* prevBasicBlk) {
   if ((*operand)->kind() == Operand::LABEL_OPERAND) {
      CodeLabel& codeLabel = ((LabelOperand*)*operand)->codeLabel();
      Instruction& target_next = *codeLabel.basicBlk()._firstIns->_next;
      if (target_next.kind() == Instruction::GOTO_INSTR &&
         ((GoToInstruction&)target_next).target().kind() == Operand::LABEL_OPERAND) {
         LabelOperand& labOperand = (LabelOperand&)((GoToInstruction&)target_next).target();
         if (!unique || labOperand.codeLabel().useCount() == 1) {

#ifdef TRACE
            if (traceFlag) {
               outStream << "Peephole optimise: skipping unncessary goto " << target_next._seqNo << '\n';
               outStream.flush();
            }
#endif

            (*operand)->destroy(_msa);
            *operand = &labOperand;
            labOperand.codeLabel().incRef();
            return 1;
         }
      } else if (target_next.kind() == Instruction::LABEL_INSTR) {
         LabelInstruction& labIns = (LabelInstruction&)target_next;
         (*operand)->destroy(_msa);
         *operand = new(_msa) LabelOperand(labIns.codeLabel());
         return 1;
      }
      
   }
   return 0;
}

// Conditionally remove redundant move
// src == ins->dst => src = ins->src

// Note that 'ins' is initially always a move instruction

Bool Code::peepholeMoveSrcVar(Operand*& src, MoveInstruction*& ins, VarOperand& dst) {
   if (&((VarOperand*&)src)->_var == &dst._var) {
      src->destroy(msa());
      src = ins->_src->clone(msa());
      return TRUE;
   }
   else
      return FALSE;
}


Bool Code::peepholeMoveSrcSelect(Operand*& src, MoveInstruction*& ins, VarOperand& dst) {
   Var& srcVar = ((VarOperand*)(((SelectOperand*&)src)->_operand))->_var;
   if (&srcVar == &dst._var) {
      ((SelectOperand*&)src)->_operand->destroy(msa());
      ((SelectOperand*&)src)->_operand = ins->_src->clone(msa());
      return TRUE;
   }
   return FALSE;
}

Bool Code::peepholeMoveSrcDePtr(Operand*& src, MoveInstruction*& ins, VarOperand& dst) {
   Var& srcVar = ((VarOperand*)(((DePtrOperand*&)src)->_operand))->_var;
   if (&srcVar == &dst._var) {
      ((DePtrOperand*&)src)->_operand->destroy(msa());
      ((DePtrOperand*&)src)->_operand = ins->_src->clone(msa());
      return TRUE;
   }
   return FALSE;
}

UInt Code::peepholeMoveStructSrc(StructOperand& srcComps, Instruction*& ins, BasicBlk& basicBlk) {
   UInt res = 0;
   for (UInt i = 0; i < srcComps._nComps; i++) {
      Operand*& operand = srcComps._comps[i];
      if (operand->kind() != Operand::STRUCT_OPERAND)
         res += peepholeMoveSrc(operand, ins, basicBlk);
      else
         res += peepholeMoveStructSrc((StructOperand&)*operand, ins, basicBlk);
   }
   return res;
}

UInt Code::peepholeMoveSrc(Operand*& src, Instruction*& ins, BasicBlk& basicBlk) {
   if ((ins->kind() == Instruction::MOVE_INSTR ||
        ins->kind() == Instruction::DECONS_INSTR) &&
       !((MoveInstruction*)ins)->isConstructor()) {
      VarOperand& dst = (VarOperand&)*((MoveInstruction&)*ins)._dst;

      if (src != NULL &&
          (dst.varRefCount() <= 2 || ((MoveInstruction&)*ins).src().kind() == Operand::VAR_OPERAND) &&
          dst.var().kind() != Var::GLOBAL_VAR) {
         Operand::Kind srcKind = src->kind();
         switch (srcKind) {
         case Operand::VAR_OPERAND:
            if (!peepholeMoveSrcVar(src, (MoveInstruction*&)ins, dst))
               return 0;
            break;

         case Operand::SELECT_OPERAND:
            if (!peepholeMoveSrcSelect(src, (MoveInstruction*&)ins, dst))
               return 0;
            break;

         case Operand::DEPTR_OPERAND:
            if (!peepholeMoveSrcDePtr(src, (MoveInstruction*&)ins, dst))
               return 0;
            break;

         default:
            return 0;
         }

         if (dst.varRefCount() == 1) {

//--------- Removing an unecessary move here may save the need for an additional pass 

#ifdef TRACE
            if (traceFlag) {
               outStream << "Peephole optimise: removing unnecessary move " << ins->_seqNo << '\n';
               outStream.flush();
            }
#endif
            basicBlk.removeInstruction(ins, *this);
            ins = ins->_next;
         }

         return 1;
      }
   }
   return 0;
}

Operand* Code::peepholeMoveDstDePtrSelect(SelectOperand* sel) {
   return &selectOperand(sel->index(),
      sel->operand().kind() == Operand::SELECT_OPERAND &&
      ((SelectOperand&)sel->operand()).byPtr()
      ? peepholeMoveDstDePtrSelect((SelectOperand*)&sel->operand())
      : &dePtrOperand(*sel->operand().clone(_msa)), sel->typeSig());
}

Bool Code::peepholeMoveDstDePtr(Operand*& dst, MoveInstruction*& ins, VarOperand& insDst) {
   if (((DePtrOperand*&)dst)->operand()->kind() == Operand::VAR_OPERAND &&
      ins->_src->kind() == Operand::SELECT_OPERAND) {
      SelectOperand* srcSel = (SelectOperand*)ins->_src;
      if (srcSel->byPtr()) {
         Var& dstVar = ((VarOperand*)((DePtrOperand*&)dst)->operand())->var();
         if (&dstVar == &insDst._var) {
            dst->destroy(msa());
            dst = peepholeMoveDstDePtrSelect(srcSel);
            return TRUE;
         }
      }
   }
   return FALSE;
}

Bool Code::peepholeMoveDstSelect(Operand*& dst, MoveInstruction*& ins, VarOperand& insDst) {
   SelectOperand& select = *(SelectOperand*)dst;
   if (select.operand().kind() == Operand::DEPTR_OPERAND &&
      ((DePtrOperand&)select.operand()).operand()->kind() == Operand::VAR_OPERAND) {
      SelectOperand& srcSelect = *(SelectOperand*)ins->_src;
      if (srcSelect.byPtr()) {
         Var& dstVar = ((VarOperand*)((DePtrOperand&)select.operand()).operand())->var();
         if (&dstVar == &insDst._var) {
            dst->destroy(msa());
            srcSelect.byPtr() = FALSE;
            dst = &selectOperand(select.index(), &selectOperand(srcSelect.index(),
                                                                &dePtrOperand(*srcSelect.operand().clone(_msa)),
                                                                srcSelect.typeSig()), select.typeSig());
            return TRUE;
         }
      }
   }
   return FALSE;
}

UInt Code::peepholeMoveDst(Operand*& dst, Instruction*& ins, BasicBlk& basicBlk) {
   MoveInstruction& moveIns = *(MoveInstruction*)ins;
   if (moveIns._dst != NULL && moveIns._dst->kind() == Operand::VAR_OPERAND) {
      VarOperand& insDst = *(VarOperand*)moveIns._dst;
      switch (dst->kind()) {
      case Operand::DEPTR_OPERAND:
         if (!peepholeMoveDstDePtr(dst, (MoveInstruction*&)ins, insDst))
            return 0;
         break;

      case Operand::SELECT_OPERAND:
         if (!peepholeMoveDstSelect(dst, (MoveInstruction*&)ins, insDst))
            return 0;
         break;

      default:
         return 0;
      }

      if (insDst.varRefCount() == 1) {

//--------- Removing an unecessary move here may save the need for an additional pass 

#ifdef TRACE
         if (traceFlag) {
            outStream << "Peephole optimise: removing unnecessary move " << ins->_seqNo << '\n';
            outStream.flush();
         }
#endif
         basicBlk.removeInstruction(ins, *this);
         ins = ins->_next;
      }

      return 1;

   }
   return 0;
}

// Optimise cases following a move instruction
// See notes above for 'ins'

UInt Code::peepholeMovePred(Instruction*& ins, Instruction* nextIns, BasicBlk& basicBlk) {
   switch (nextIns->kind()) {
      case Instruction::MONADIC_OP_INSTR:
      case Instruction::UPDATE_INSTR:
      case Instruction::CONS_INSTR:
      case Instruction::CONS_S_INSTR:
      case Instruction::CONS_A_S_INSTR:
      case Instruction::DECONS_INSTR:
      case Instruction::DECONS_S_INSTR:
      case Instruction::TAG_OF_INSTR:
      case Instruction::TAG_INSTR:
      case Instruction::FROM_CELL_INSTR:
      case Instruction::TO_CELL_INSTR:
      case Instruction::ENV_OF_INSTR:
         return peepholeMoveSrc(((MonadicInstruction&)*nextIns)._src, ins, basicBlk);

      case Instruction::FROM_PLAIN_INSTR: {
         FromPlainInstruction& fromPlainIns = (FromPlainInstruction&)*nextIns;
         return peepholeMoveSrc(fromPlainIns._src, ins, basicBlk) > 0
            ? 1
            : peepholeMoveSrc(fromPlainIns._env, ins, basicBlk);
      }

      case Instruction::MONADIC_OP_ACTION_INSTR: {
         MonadicOpActionInstruction& monadicOpIns = (MonadicOpActionInstruction&)*nextIns;
         return peepholeMoveSrc(((MonadicOpActionInstruction&)*nextIns)._x, ins, basicBlk);
      }

      case Instruction::MOVE_INSTR:
         return peepholeMoveSrc(((MonadicInstruction&)*nextIns)._src, ins, basicBlk) > 0
            ? 1
            : peepholeMoveDst(((MoveInstruction&)*nextIns)._dst, ins, basicBlk) > 0;

      case Instruction::DYADIC_OP_INSTR: {
         DyadicOpInstruction& dyadicOpIns = (DyadicOpInstruction&)*nextIns;
         return peepholeMoveSrc(dyadicOpIns._x, ins, basicBlk) > 0
            ? 1
            : peepholeMoveSrc(dyadicOpIns._y, ins, basicBlk);
      }

      case Instruction::COND_INSTR: {
         ConditionalInstruction& condIns = (ConditionalInstruction&)*nextIns;
         return peepholeMoveSrc(condIns._x, ins, basicBlk) > 0
            ? 1
            : peepholeMoveSrc(condIns._y, ins, basicBlk);
      }

      case Instruction::STRUCT_INSTR:
         return peepholeMoveStructSrc(((StructInstruction&)*nextIns)._src, ins, basicBlk);

      case Instruction::CASE_INSTR:
         return peepholeMoveSrc(((CaseInstruction&)*nextIns)._x, ins, basicBlk);

      default:
         break;
   }
   return 0;
}

// Successor move instruction logic

// ... x; move x, y => ... y

// Note that this optimisation is suppressed for function arguments
// to preserve the semantics of tail call function application

Bool Code::peepholeMoveSucc(Instruction*& ins, Instruction* nextIns,
                            Operand*& dst,
                            BasicBlk* prevBasicBlk) {
   if (nextIns->kind() == Instruction::MOVE_INSTR) {
      MoveInstruction* moveIns = (MoveInstruction*)nextIns;
      if (moveIns->_src->kind() == Operand::VAR_OPERAND &&
         !moveIns->isConstructor()) {
         Var& var = ((VarOperand&)*moveIns->_src)._var;
         if (&var == &((VarOperand*&)dst)->_var &&
              var._refCount == 2 &&
              var.kind() != Var::GLOBAL_VAR &&
              ((VarOperand&)*moveIns->_dst)._var.kind() != Var::ARG_VAR &&
              ((VarOperand&)*moveIns->_dst)._var.kind() != Var::EXTRA_ARG_VAR) {
            dst->destroy(msa());
            dst = moveIns->_dst;
            moveIns->_dst = NULL;

#ifdef TRACE
               if (traceFlag) {
                  outStream << "Peephole optimise: removing unnecessary succeeding move " << moveIns->_seqNo << '\n';
                  outStream.flush();
               }
#endif

               prevBasicBlk->removeInstruction(ins, *this, FALSE);
               insertInstruction(ins, moveIns);
               prevBasicBlk->removeInstruction(moveIns, *this, TRUE);
               return TRUE;
         }
      }
   }
   return FALSE;
}

UInt Code::nestStruct(StructOperand& structOperand, StructInstruction* ins, VarOperand*& dst, BasicBlk* prevBasicBlk) {
   UInt nChanges = 0;
   for (UInt i = 0; i < structOperand._nComps; i++) {
      Operand* operand = structOperand._comps[i];
      switch (operand->kind()) {
      case Operand::STRUCT_OPERAND:
         nChanges += nestStruct((StructOperand&)*operand, ins, dst, prevBasicBlk);
         break;

      case Operand::VAR_OPERAND:
         if (&((VarOperand*)operand)->_var == &dst->_var &&
            dst->_var._refCount == 2) {

#ifdef TRACE
            if (traceFlag) {
               outStream << "Peephole optimise: deferring nested structure " << ins->_seqNo << '\n';
               outStream.flush();
            }
#endif

            structOperand._comps[i] = &ins->_src;

            prevBasicBlk->removeInstruction(ins, *this, FALSE);
            nChanges++;
         }
      }
   }

   return nChanges;
}

/*
Simple peephole optimisations

1. goto l1 ... label l1; goto l2       => goto l2

T.B.D. same for branch

2. prev ... goto l; label l ... next   => prev; next
(provided use count of l == 1)

3. b<op> x,y,l1; goto l2; l1           => b<inv op> l2
(provided use count of l1 == 1)

move x, y
move y, z
=> move x, z

*/

UInt Code::peepholeOptimise(UInt pass) {
   UInt nChanges = 0;
   BasicBlk* prevBasicBlk = NULL;
   Bool inBody = FALSE;

#ifdef TRACE
   if (traceFlag) {
      outStream << "Peephole optimisation: pass " << pass << '\n';
      outStream.flush();
   }
#endif

   for (Instruction* ins = _instrSeq->first(); ins != NULL;) {

#ifdef TRACE
      if (ins->_seqNo == instructionProbeSeqNo) {
         outStream << "*** Code::peepholeOptimise: Probe hit\n";
         outStream.flush();
         traceFlag = TRUE;
      }
#endif
      if (traceFlag) {
         outStream << "Peephole optimise: " << ins->_seqNo << '\n';
         outStream.flush();
      }

      Instruction* next = ins->_next;
      switch (ins->_kind) {
         case Instruction::LABEL_INSTR: {
            CodeLabel& codeLabel = ((LabelInstruction&)*ins).codeLabel();

            if (codeLabel.useCount() > 1 ||
               inBody && codeLabel._lambda != NULL) {
               if (codeLabel._basicBlk != NULL) {
                  prevBasicBlk = &codeLabel.basicBlk();
                  inBody = TRUE;
               }
            } else {
               if (inBody) {

   #ifdef TRACE
                  if (traceFlag) {
                     outStream << "Peephole optimise: removing unused label = " << ins->_seqNo << '\n';
                     outStream.flush(); 
                  }
   #endif

                  if (codeLabel._basicBlk != NULL) {
                     if (codeLabel.basicBlk()._firstIns !=
                         codeLabel.basicBlk()._lastIns)
                        prevBasicBlk->_lastIns = codeLabel.basicBlk()._lastIns;
                     removeBasicBlk(codeLabel.basicBlk());
                  }
                  removeInstruction(ins);
               }
               else {

//--------------- Eliminate dead code

                  if (codeLabel._lambda != NULL &&
                     codeLabel._lambda->_entryLabel == &codeLabel)
                     codeLabel._lambda->_entryLabel = NULL;

                  Instruction* lastIns = codeLabel.basicBlk()._lastIns;
                  removeBasicBlk(codeLabel.basicBlk());
                  next = lastIns->_next;

   #ifdef TRACE
                  if (traceFlag) {
                     outStream << "Peephole optimise: removing dead code from " << ins->_seqNo << " to " << lastIns->_seqNo << '\n';
                     outStream.flush();
                  }
   #endif

                  removeInstructions(ins, lastIns);
               }
               nChanges++;
            }
            break;
         }

         case Instruction::RESERVE_INSTR: {
            ReserveInstruction& reserveIns = (ReserveInstruction&)*ins;
            if (((VarOperand*)reserveIns._x)->var()._refCount == 0) {

   #ifdef TRACE
               if (traceFlag) {
                  outStream << "Peephole optimise: removing unused reserve = " << ins->_seqNo << '\n';
                  outStream.flush();
               }
   #endif

               prevBasicBlk->removeInstruction(ins, *this);
               nChanges++;
               break;
            }
            break;
         }

         case Instruction::DECONS_INSTR:
         case Instruction::MOVE_INSTR: {
            MoveInstruction& moveIns = (MoveInstruction&)*ins;

            if (moveIns.dst().kind() == Operand::VAR_OPERAND) {
               VarOperand* dst = (VarOperand*)(moveIns._dst);

               if (unusedLocal(*dst, ins, prevBasicBlk)) {
                  nChanges++;
                  break;
               }
               BasicBlk* basicBlk;
               if (moveIns._src->_kind == Operand::LABEL_OPERAND) {
                  basicBlk = &((LabelOperand&)*moveIns._src)._codeLabel.basicBlk();
                  Instruction* dst_next;
                  if ((dst_next = basicBlk->_firstIns->_next) != NULL &&
                     dst_next->_kind == Instruction::GOTO_INSTR &&
                     ((GoToInstruction*)dst_next)->target()._kind != Operand::VAR_OPERAND &&
                     FALSE) {

//------------------ This optimisation is currently disabled because it
//------------------ causes incorrect code to be generated where
//------------------ a live variable is lower in the stack.

                     moveIns._src = ((GoToInstruction*)dst_next)->_target;
                     if (moveIns._src->_kind == Operand::LABEL_OPERAND) {
                        ((LabelOperand&)*moveIns._src)._codeLabel.incRef();
                        nChanges++;
                        continue;
                     }
                  }
               }
               if (prevBasicBlk != NULL && ins != prevBasicBlk->_lastIns) {
                  VarOperand* dst = (VarOperand*)((MoveInstruction&)*ins)._dst;
                  for (Instruction* nextIns = next;
                       nextIns != NULL && nextIns->kind() != Instruction::PTR_INSTR;
                       nextIns = nextIns->_next) {
                     UInt n;
                     if ((n = peepholeMovePred(ins, nextIns, *prevBasicBlk)) > 0) {
                        nChanges += n;
                        goto cont;
                     }
                     if (nextIns == prevBasicBlk->_lastIns ||
                        nextIns->usesVar(dst->_var))
                        break;
                  }
               }
            }
            break;
         }

         case Instruction::MONADIC_OP_INSTR:
         case Instruction::ENV_OF_INSTR:
         case Instruction::TAG_INSTR:
//          case Instruction::TAG_OF_INSTR:

         case Instruction::FROM_CELL_INSTR: {
            MonadicInstruction& monadicIns = (MonadicInstruction&)*ins;
            if (monadicIns._dst->kind() == Operand::VAR_OPERAND) {
               VarOperand*& dst = (VarOperand*&)monadicIns._dst;
               if (unusedLocal(*dst, ins, prevBasicBlk)) {
                  nChanges++;
                  break;
               }
               if (prevBasicBlk != NULL &&
                  ins != prevBasicBlk->_lastIns) {
                  Var* src = monadicIns._src->kind() == Operand::VAR_OPERAND
                     ? &((VarOperand*)monadicIns._src)->_var
                     : NULL;
                  for (Instruction* nextIns = next;
                       nextIns != NULL &&
                       !(ins->kind() == Instruction::MONADIC_OP_INSTR

#if (SERIALISATION==1)
                         && ((MonadicOpInstruction&)*ins).op().kind() == Operator::EXTRACT_OP
#endif

                          ) && nextIns->kind() != Instruction::PTR_INSTR; nextIns = nextIns->_next) {
                     if (peepholeMoveSucc(ins, nextIns, monadicIns._dst, prevBasicBlk)) {
                        nChanges++;
                        goto cont;
                     }
                     if (nextIns == prevBasicBlk->_lastIns ||
                        nextIns->usesVar(dst->_var) ||
                        src != NULL && nextIns->definesVar(*src))
                        break;
                  }
               }
            }
            break;
         }

         case Instruction::DYADIC_OP_INSTR: {
            DyadicOpInstruction& dyadicOpIns = (DyadicOpInstruction&)*ins;
            if (dyadicOpIns._dst->kind() == Operand::VAR_OPERAND) {
               VarOperand*& dst = (VarOperand*&)dyadicOpIns._dst;
               if (unusedLocal(*dst, ins, prevBasicBlk)) {
                  nChanges++;
                  break;
               }
               if (prevBasicBlk != NULL &&
                  ins != prevBasicBlk->_lastIns) {
                  Var* x = dyadicOpIns._x->kind() == Operand::VAR_OPERAND
                     ? &((VarOperand*)dyadicOpIns._x)->_var
                     : NULL;
                  Var* y = dyadicOpIns._y->kind() == Operand::VAR_OPERAND
                     ? &((VarOperand*)dyadicOpIns._y)->_var
                     : NULL;

                  for (Instruction* nextIns = next;
                       nextIns != NULL && nextIns->kind() != Instruction::PTR_INSTR;
                       nextIns = nextIns->_next) {
                     if (dyadicOpIns.op().kind() == Operator::MAP_OP &&
                         nextIns->kind() == Instruction::CELL_ALLOC_INSTR)
                        break;
                     if (peepholeMoveSucc(ins, nextIns, dyadicOpIns._dst, prevBasicBlk)) {
                        nChanges++;
                        goto cont;
                     }
                     if (nextIns == prevBasicBlk->_lastIns ||
                        nextIns->usesVar(dst->_var) ||
                        x != NULL && nextIns->definesVar(*x) ||
                        y != NULL && nextIns->definesVar(*y))
                        break;
                  }
               }
            }
            break;
         }

         case Instruction::CELL_INFO_INSTR: {
            CellInfoInstruction& cellInfoIns = *(CellInfoInstruction*)ins;
            if (cellInfoIns._cellInfo._useCount <= 1) {

   #ifdef TRACE
               if (traceFlag) {
                  outStream << "Peephole optimise: removing unused cell info = " << ins->_seqNo << '\n';
                  outStream.flush();
               }
   #endif

               removeInstruction(ins);
               nChanges++;
            }
            break;
         }

         case Instruction::CELL_ALLOC_INSTR: {
            CellAllocInstruction& caIns = *(CellAllocInstruction*)ins;
            Var& dst = ((VarOperand&)*caIns._dst)._var;

            if (//caIns._structTemplate._nSlots == 0 &&
                dst.kind() != Var::GLOBAL_VAR &&
                dst.kind() != Var::RESULT_VAR &&
                dst._refCount == 1) {

//------------ The following could be improved with better data flow analysis

   #ifdef TRACE
               if (traceFlag) {
                  outStream << "Peephole optimise: removing unused cell allocation = " << ins->_seqNo << '\n';
                  outStream.flush();
               }
   #endif

#if (GARBAGE_COLLECTION==1)
               if (ins->_prev != NULL &&
                   ins->_prev->kind() == Instruction::FRAME_DESCR_INSTR) {

#ifdef TRACE
                  if (traceFlag) {
                     outStream << "Peephole optimise: removing unused frame descriptor = " << ins->_prev->_seqNo << '\n';
                     outStream.flush();
                  }
#endif
                  prevBasicBlk->removeInstruction(ins->_prev, *this);
                  nChanges++;
               }
#endif

               prevBasicBlk->removeInstruction(ins, *this);
               nChanges++;
            }
            break;
         }

         case Instruction::GOTO_INSTR: {
            GoToInstruction& goToIns = (GoToInstruction&)*ins;
            if (peepholeJump(&goToIns._target, FALSE, prevBasicBlk) > 0) {
               nChanges++;
               continue;
            }
            if (goToIns._target->kind() == Operand::LABEL_OPERAND) {
               CodeLabel& codeLabel = ((LabelOperand*)goToIns._target)->_codeLabel;
               if (codeLabel._lambda == NULL &&
                  next == codeLabel.basicBlk()._firstIns) {

//--------------- Jumps to closures are not removed because there may be data
//--------------- sections preceding the code

   #ifdef TRACE
                  if (traceFlag) {
                     outStream << "Peephole optimise: removing unnecessary jump = " << ins->_seqNo << '\n';
                     outStream.flush();
                  }
   #endif

                  prevBasicBlk->removeInstruction(ins, *this);
                  nChanges++;
                  break;
               }
            }
            inBody = FALSE;
            break;
         }

//------ Drop through

         case Instruction::RETURN_INSTR:
            inBody = FALSE;
            break;

         case Instruction::COND_INSTR: {
            ConditionalInstruction& condIns = (ConditionalInstruction&)*ins;
            if (peepholeJump((Operand**)&condIns._z, TRUE, prevBasicBlk) > 0) {
               nChanges++;
               continue;
            }
            Instruction* nextButOne;
            if (next != NULL && (nextButOne = next->_next) != NULL) {               
               if (next->kind() == Instruction::GOTO_INSTR) {
                  GoToInstruction& goToIns = (GoToInstruction&)*next;
                  LabelOperand& labOperand = (LabelOperand&)goToIns.target();
                  if (goToIns._target->kind() == Operand::LABEL_OPERAND &&
                     nextButOne->kind() == Instruction::LABEL_INSTR) {
                     LabelInstruction& labelIns = (LabelInstruction&)*nextButOne;
                     CodeLabel* codeLabel = &labelIns._codeLabel;
                     if (&condIns._z->_codeLabel == codeLabel &&
                        labOperand._codeLabel.useCount() <= 4) {
                        condIns._op = condIns._op.inverse();
                        condIns.setOpCodeStr(msa());
                        condIns._z->destroy(msa());
                        condIns._z = &labOperand;
                        goToIns._target = NULL;

   #ifdef TRACE
                        if (traceFlag) {
                           outStream << "Peephole optimise: inverting conditional instruction " << ins->_seqNo << '\n';
                           outStream << "                   removing instruction " << next->_seqNo << '\n';
                           outStream.flush();
                        }
   #endif

                        prevBasicBlk->removeInstruction(next, *this);
                        nChanges++;
                        continue;
                     }
                  }
               }
            }
            break;
         }

         case Instruction::CASE_INSTR: {
            CaseInstruction& caseIns = (CaseInstruction&)*ins;
            for (UInt i = 0; i < caseIns._n; i++)
               nChanges += peepholeJump((Operand**)&caseIns._labels[i], FALSE,
                  prevBasicBlk);
            if (caseIns._otherwise != NULL)
               nChanges += peepholeJump((Operand**)&caseIns._otherwise, TRUE,
                  prevBasicBlk);
            inBody = FALSE;
            break;
         }

         case Instruction::STRUCT_INSTR: {
            StructInstruction& structIns = (StructInstruction&)*ins;
            VarOperand*& dst = (VarOperand*&)structIns._dst;
            if (unusedLocal(*dst, ins, prevBasicBlk)) {
               nChanges++;
               break;
            }
            if (prevBasicBlk != NULL &&
               ins != prevBasicBlk->_lastIns) {
               for (Instruction* nextIns = next;
                                 nextIns != NULL/*&&
                                 nextIns->kind() != Instruction::PTR_INSTR*/; // T.B.D. Consider w.r.t. GC
                                 nextIns = nextIns->_next) {
                  if (peepholeMoveSucc(ins, nextIns, structIns._dst, prevBasicBlk)) {
                     nChanges++;
                     goto cont;
                  }

//--------------- Nested structure analysis

                  if (nextIns->kind() == Instruction::STRUCT_INSTR) {
                     Bool breakFlag = FALSE;
                     StructInstruction& nextStructIns = *(StructInstruction*)nextIns;
                     StructOperand& structOperand = nextStructIns._src;
                     UInt nestCount = nestStruct(structOperand, (StructInstruction*)ins, dst, prevBasicBlk);

                     if (nestCount > 0) {
                        nChanges += nestCount;
                        break;
                     }
                  }

                  if (nextIns == prevBasicBlk->_lastIns ||
                      nextIns->usesVar(dst->_var))
                     break;
               }
            }
            break;
         }

         case Instruction::PTR_INSTR: {
            PtrInstruction& ptrIns = (PtrInstruction&)*ins;
            VarOperand*& dst = (VarOperand*&)ptrIns._dst;
            if (unusedLocal(*dst, ins, prevBasicBlk)) {
               nChanges++;
               break;
            }
            if (prevBasicBlk != NULL &&
               ins != prevBasicBlk->_lastIns) {
               for (Instruction* nextIns = next; nextIns != NULL; nextIns = nextIns->_next) {
                  if (peepholeMoveSucc(ins, nextIns, ptrIns._dst, prevBasicBlk)) {
                     nChanges++;
                     goto cont;
                  }

                  if (nextIns == prevBasicBlk->_lastIns ||
                     nextIns->usesVar(dst->_var))
                     break;
               }
            }
            break;
         }

         case Instruction::ENTER_CELL_INSTR:

#if (GARBAGE_COLLECTION==1)
         case Instruction::MARK_CELL_INSTR:
#endif

         case Instruction::EXCEPTION_INSTR:
            inBody = FALSE;
            break;

         case Instruction::ARG_CHECK_INSTR: {
            ArgCheckInstruction& argCheckInstruction = *(ArgCheckInstruction*)ins;
            if (argCheckInstruction._altEntryLab.lambda()->_simple) {
               for (UInt i = 0; i < argCheckInstruction._n; i++) {
                  argCheckInstruction._labels[i]->destroy(msa());
               }
               argCheckInstruction._n = 0;
            }
            break;
         }

#if (GARBAGE_COLLECTION==1)
         case Instruction::GC_STUB_INSTR: {
            GC_StubInstruction& gc_StubInstruction = *(GC_StubInstruction*)ins;
            if (!gc_StubInstruction.isLive()) {
               prevBasicBlk->removeInstruction(ins, *this);
               nChanges++;
            }
         }
#endif

         default:
            break;
      }
      ins = next;
   cont:;
   }
   return nChanges;
}

Void Code::extendSuccessors(Void) const {
   Bool more;
   do {
      more = FALSE;
      BasicBlk* basicBlk = _basicBlks;
      while (basicBlk != NULL) {
         BasicBlkElement* basicBlkElement = basicBlk->_successors.head();
         while (basicBlkElement != NULL) {
            BasicBlkElement* succBasicBlkElement = basicBlkElement->_basicBlk._successors.head();
            while (succBasicBlkElement != NULL) {
               if (basicBlk->_successors.findElement(succBasicBlkElement->_basicBlk) == NULL) {
                  basicBlk->addSuccessor(succBasicBlkElement->_basicBlk, msa());
                  more = TRUE;
               }
               succBasicBlkElement = succBasicBlkElement->_next;
            }
            basicBlkElement = basicBlkElement->_next;
         }
         basicBlk = basicBlk->_next;
      }
   } while (more);
}

// Set input and output flags according to the standard algorithm
//
// Note the modifications for handling continuations which may be defined but
// are not used locally.

Void Code::setInOut(Void) const {
   Bool more = TRUE;
   while (more) {
      more = FALSE;
      BasicBlk* basicBlk = _basicBlks;
      while (basicBlk != NULL) {

#ifdef TRACE_EXTRA
         if (traceFlag) {
            outStream << "Setting flags for " << basicBlk->_firstIns->_seqNo << '\n';
            outStream << "Successors: ";
         }
#endif

         BasicBlkElement* basicBlkElement = basicBlk->_successors.head();
         while (basicBlkElement != NULL) {

#ifdef TRACE_EXTRA
            if (traceFlag)
               outStream << '<' << basicBlkElement->_basicBlk._firstIns->_seqNo << "> ";
#endif

            VarElement* succVarElement = basicBlkElement->_basicBlk._vars.head();
            while (succVarElement != NULL) {
               Var& succVar = succVarElement->_var;
               if (succVar.kind() != Var::GLOBAL_VAR &&
                  succVarElement->testFlags(VarElement::I) &&
                  !succVarElement->testFlags(VarElement::D)) {
                  VarElement* varElement = basicBlk->_vars.findElement(succVar);
                  if (varElement == NULL)
                     varElement = basicBlk->_vars.addElement(succVar, msa());
                  if (!(varElement->testFlags(VarElement::O))) {
                     varElement->setFlags(VarElement::O);
                     more = TRUE;
                  }
               }
               succVarElement = succVarElement->_next;
            }

            basicBlkElement = basicBlkElement->_next;
         }

#ifdef TRACE_EXTRA
         if (traceFlag)
            outStream << '\n';
#endif

         VarElement* varElement = basicBlk->_vars._varElements;
         while (varElement != NULL) {
            Var& var = varElement->_var;
            VarElement* next = varElement->_next;
            if (var.kind() != Var::GLOBAL_VAR &&
               (varElement->testFlags(VarElement::U) ||
                  varElement->testFlags((VarElement::Flags)(VarElement::O | VarElement::X))) &&
               !varElement->testFlags((VarElement::Flags)(VarElement::D | VarElement::I))) {
               varElement->setFlags(VarElement::I);

               if (traceFlag)
                  outStream << ' '<< _basicBlk->_firstIns->_seqNo << ' ' << var.name() << ' ';
               more = TRUE;
            } else if (var.kind() == Var::EXT_GLOBAL_VAR &&
                       varElement->testFlags(VarElement::U))
               varElement->setFlags((VarElement::Flags)(VarElement::D | VarElement::I));
            varElement = next;
         }
         basicBlk = basicBlk->_next;
      }
   }
}

Void Code::printBlockInfo(Void) {
   BasicBlk* basicBlk = _basicBlks;
   while (basicBlk != NULL) {
      outStream << "Basic block ";
      if (basicBlk->_firstIns != NULL &&
         basicBlk->_lastIns != NULL) {
         outStream << basicBlk->_firstIns->_seqNo
            << '-'
            << basicBlk->_lastIns->_seqNo
            << ' ';
      }
      if (!basicBlk->_successors.isEmpty()) {
         outStream << "[";
         BasicBlkElement* basicBlkElement = basicBlk->_successors.head();
         UInt count = 0;
         while (basicBlkElement != NULL) {
            if (count++ != 0)
               outStream << ',';
            outStream << basicBlkElement->_basicBlk._firstIns->_seqNo;
            basicBlkElement = basicBlkElement->_next;
         }
         outStream << ']';
      }
      basicBlk->_vars.count(*this);
      outStream << '\n';
      basicBlk = basicBlk->_next;
   }
}

Void Code::setKillPending(VarOperand& operand) {
   operand.setKillPending();
}

Void Code::setNextUse(Var& var, Bool nextUse) {
}

Void Code::notLive(Var& var) {
}

// Linear seach will suffice for the following two functions for now

Int Code::nameConstIndex(Name name) const {
   for (UInt i = 0; i < _nNameConsts; i++)
      if (_nameConstV.get(i) == name)
         return i;
   return -1;
}

UInt Code::addNameConst(Name name) {
   Int res;
   if ((res = nameConstIndex(name)) >= 0)
      return (UInt)res;
   _nameConstV.put(_nNameConsts, name, msa());
   return _nNameConsts++;
}

Int Code::typeConstIndex(Type type) const {
   for (UInt i = 0; i < _nTypeConsts; i++)
      if (_typeConstV.get(i) == type)
         return i;
   return -1;
}

UInt Code::addTypeConst(Type type) {
   Int res;
   if ((res = typeConstIndex(type)) >= 0)
      return (UInt)res;
   _typeConstV.put(_nTypeConsts, type, msa());
   return _nTypeConsts++;
}

CellInfo_& Code::newCellInfo(CodeLabel& entryLabel, CodeLabel& altEntryLabel,
                             CodeLabel* copyFnLabel,
                             CodeLabel* extractBinFnLabel, CodeLabel* insertBinFnLabel

#if (GARBAGE_COLLECTION==1)
                           , CodeLabel* gcFnLabel
#endif

                            ) {
   return *new(_msa) CellInfo_(entryLabel, altEntryLabel, copyFnLabel,
                               extractBinFnLabel, insertBinFnLabel

#if (GARBAGE_COLLECTION==1)
                             , gcFnLabel
#endif

                              );
}

Void Code::genLabel(CodeLabel& codeLabel) {
   Instruction* ins = new(_msa) LabelInstruction(codeLabel);
   if (codeLabel._basicBlk != NULL) {
      BasicBlk& basicBlk = *codeLabel._basicBlk;
      _basicBlk = &basicBlk;
      _execSeqFlag = TRUE;
      basicBlk._firstIns = ins;
   }
   addInstruction(ins);
}

Void Code::allocateDataAndLabels(Void) {
   const Instruction* ins = _instrSeq->first();
   while (ins != NULL) {
      ins->allocateData(*this);
      if (ins->_kind == Instruction::LABEL_INSTR) {
         CodeLabel& codeLabel = ((LabelInstruction&)*ins)._codeLabel;
         codeLabel._seqNo = ++_labelSeqNo;
      }
      ins = ins->_next;
   }
}

/*

// Usual useful debugging code snippets

printTranExpr(expr, FALSE, outStream, env());
outStream << '\n';
outStream.flush();

printTypeSig(typeSig, 0, FALSE, outStream, env());
outStream << '\n';

printTypeSig(typeSig, 0, FALSE, outStream, code.env());
outStream << '\n';
*/
