/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    iCode.cpp
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
 *    Implementation of I-machine code generation
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
#include "ivory/trace.h"
#include "ivory/compiler/ICode.h"
#include "ivory/compiler/tran.h"

#define TRACE

#ifdef TRACE
static Bool traceFlag = FALSE;
#define toggle(flag) ((flag)=!(flag))
static Int instructionProbeSeqNo = -1;
#endif

declareTypeCon(Ptr);

// The following macro controls whether or not stack structures are
// allocated to overlay any variables which are no longer live after
// a structure instruction

//#define ALLOW_STRUCT_OVERLAY

Bool Const::suppressEnvMove(Void) const {
   return FALSE;
}

Void Const::print(ostream& os, const Env& env) const {
   os << "Const::print: not overlaoded\n";
}

template <>
Repr ConstT<Name>::repr(Void) const {
   return Repr::REPR_NAME;
}

template <>
Bool ConstT<Name>::needsEnv(Bool dst) const {
   return TRUE;
}

template <>
Loc* ConstT<Name>::loc(Repr repr, const ICode& code) const {
   return repr == Repr::REPR_ENV_PTR ? &code.codeEnv() : NULL;
}

template <>
Bool ConstT<Name>::suppressEnvMove(Void) const {
   //   return _value < builtInNameCount;
   return TRUE;
}

template <>
Repr ConstT<TypeConst>::repr(Void) const {
   return Repr::REPR_TYPE;
}

template <>
Bool ConstT<TypeConst>::needsEnv(Bool dst) const {
   return TRUE;
}

template <>
Loc* ConstT<TypeConst>::loc(Repr repr, const ICode& code) const {
   return repr == Repr::REPR_ENV_PTR ? &code.codeEnv() : NULL;
}

template <>
Bool ConstT<TypeConst>::suppressEnvMove(Void) const {
   return _value._type < builtInTypeCount;
}


template <>
Repr ConstT<Bits>::repr(Void) const {
   return Repr::REPR_BITS;
}

template <>
Bool ConstT<Bits>::needsEnv(Bool dst) const {
   return FALSE;
}

template <>
Loc* ConstT<Bits>::loc(Repr repr, const ICode& code) const {
   return NULL;
}

template <>
Bool ConstT<Bits>::suppressEnvMove(Void) const {
   return FALSE;
}


template <>
Repr ConstT<Int>::repr(Void) const {
   return Repr::REPR_INT;
}

template <>
Bool ConstT<Int>::needsEnv(Bool dst) const {
   return FALSE;
}

template <>
Loc* ConstT<Int>::loc(Repr repr, const ICode& code) const {
   return NULL;
}

template <>
Bool ConstT<Int>::suppressEnvMove(Void) const {
   return FALSE;
}


template <>
Repr ConstT<Double>::repr(Void) const {
   return Repr::REPR_DOUBLE;
}

template <>
Bool ConstT<Double>::needsEnv(Bool dst) const {
   return FALSE;
}

template <>
Loc* ConstT<Double>::loc(Repr repr, const ICode& code) const {
   return NULL;
}

template <>
Bool ConstT<Double>::suppressEnvMove(Void) const {
   return FALSE;
}

template <>
Repr ConstT<Char>::repr(Void) const {
   return Repr::REPR_CHAR;
}

template <>
Bool ConstT<Char>::needsEnv(Bool dst) const {
   return FALSE;
}

template <>
Loc* ConstT<Char>::loc(Repr repr, const ICode& code) const {
   return NULL;
}

template <>
Bool ConstT<Char>::suppressEnvMove(Void) const {
   return FALSE;
}

template <>
Repr ConstT<String>::repr(Void) const {
   return Repr::REPR_STRING;
}

template <>
Bool ConstT<String>::needsEnv(Bool dst) const {
   return TRUE;
}

template <>
Loc* ConstT<String>::loc(Repr repr, const ICode& code) const {
   return repr == Repr::REPR_ENV_PTR ? &code.codeEnv() : NULL;
}

template <>
Bool ConstT<String>::suppressEnvMove(Void) const {
   return FALSE;
}

template <>
Repr ConstT<Tag>::repr(Void) const {
   return Repr::REPR_TAG;
}

template <>
Bool ConstT<Tag>::needsEnv(Bool dst) const {
   return FALSE;
}

template <>
Loc* ConstT<Tag>::loc(Repr repr, const ICode& code) const {
   return NULL;
}

template <>
Bool ConstT<Tag>::suppressEnvMove(Void) const {
   return FALSE;
}

Repr Const_codeEnv::repr(Void) const {
   return Repr::REPR_PTR;
}

Bool Const_codeEnv::needsEnv(Bool dst) const {
   return FALSE;
}

Loc* Const_codeEnv::loc(Repr repr, const ICode& code) const {
   return &code.codeEnv();
}


Repr Const_consEnv::repr(Void) const {
   return Repr::REPR_ENV_PTR;
}

Bool Const_consEnv::needsEnv(Bool dst) const {
   return FALSE;
}

Loc* Const_consEnv::loc(Repr repr, const ICode& code) const {
   return &code.reg(REG_consEnv);
}


Repr Const_NULL_Ptr::repr(Void) const {
   return Repr::REPR_PTR;
}

Bool Const_NULL_Ptr::needsEnv(Bool dst) const {
   return TRUE;
}

Loc* Const_NULL_Ptr::loc(Repr repr, const ICode& code) const {
   return NULL;
}


Repr Const_NULL_Env::repr(Void) const {
   return Repr::REPR_PTR;
}

Bool Const_NULL_Env::needsEnv(Bool dst) const {
   return FALSE;
}

Loc* Const_NULL_Env::loc(Repr repr, const ICode& code) const {
   return NULL;
}


Repr Const_NULL_REF::repr(Void) const {
   return Repr::REPR_REF;
}

Bool Const_NULL_REF::needsEnv(Bool dst) const {
   return TRUE;
}

Loc* Const_NULL_REF::loc(Repr repr, const ICode& code) const {
   return NULL;
}


Loc::Loc(Kind kind, Repr repr, Loc* next/* = NULL*/)
 : _kind(kind), _repr(repr), _next(next), _firstChild(NULL),
     _readyToKill(FALSE), _reserved(FALSE) {
}

Loc::Loc(const Loc& src, Bool full, MSA& msa)
 : _kind(src._kind), _repr(src._repr),
   _next(NULL), _firstChild(NULL),
   _readyToKill(FALSE), _reserved(src._reserved)  {
   if (full) {
      VarElement* varElement = src._vars.head();
      while (varElement != NULL) {
         addVarLoc(varElement->_var, *this, msa);
         varElement = varElement->_next;
      }
   }
}


static Repr registerRepr(UInt r) {
   switch (r) {
      case REG_consEnv:    return Repr::REPR_ENV_PTR;
      case REG_sb:
      case REG_sp:         return Repr::REPR_PTR;
      case REG_cell:       return Repr::REPR_CELL_PTR;
      case REG_cellEnv:    return Repr::REPR_ENV_PTR;
      case REG_rEnv:       return Repr::REPR_ENV_PTR;
      case REG_rName:      return Repr::REPR_NAME;
      case REG_rType:      return Repr::REPR_TYPE;
      case REG_rCell:      return Repr::REPR_CELL_PTR;
      case REG_rExpr:      return Repr::REPR_EXPR;
      case REG_rByte:      return Repr::REPR_BYTE;
      case REG_rBits:      return Repr::REPR_BITS;
      case REG_rInt:       return Repr::REPR_INT;
      case REG_rFloat:     return Repr::REPR_FLOAT;
      case REG_rDouble:    return Repr::REPR_DOUBLE;
      case REG_rTag:       return Repr::REPR_TAG;
      case REG_rChar:      return Repr::REPR_CHAR;
      case REG_rStruct:    return Repr::REPR_STRUCT;
      case REG_rPtr:       return Repr::REPR_PTR;
      case REG_rRef:       return Repr::REPR_REF;
      case REG_rUTC:       return Repr::REPR_UTC;
 
      default:
         break;
   }
   return Repr::REPR_UNKNOWN;
}

Loc* stackLoc(Repr repr, LocSet& locSet) {
   LocElement* locElement = locSet.head();
   while (locElement != NULL) {
      Loc& loc = locElement->loc();
      if ((repr == Repr::REPR_UNKNOWN || repr == loc.repr()) &&
          loc.kind() == Loc::STACK_LOC)
         return &loc;
      locElement = locElement->next();
   }
   return NULL;
}

Loc* varStackLoc(Var& var, Repr repr, VarElement* varElements) {
   VarElement* varElement = varElements;
   while (varElement != NULL) {
      if (&varElement->var() == &var) {
         Loc* loc = stackLoc(repr, varElement->locSet());
         if (loc != NULL)
            return loc;
      }
      varElement = varElement->next();
   }
   return NULL;
}

Register::Register(UInt id)   
 : Loc(REGISTER_LOC, registerRepr(id), NULL), _id(id) {
}

Register::Register(const Register& src)
 : Loc(src._kind, src._repr, NULL),
    _id(src._id) {
}

SpecialLoc::SpecialLoc(Repr repr)
 : Loc(SPECIAL_LOC, repr) {
}

StaticLoc::StaticLoc(Repr repr, Bool imported, Bool exported, Name name/* = NULL_NAME*/)
 : Loc(STATIC_LOC, repr),
     _name(name),
     _imported(imported), _exported(exported),
     _allocated(FALSE), _rackPos(0) {
}

StaticLoc::StaticLoc(const StaticLoc& src)
 : Loc(src._kind, src._repr, NULL),
     _name(src._name), _imported(src._imported), _exported(src._exported),
     _allocated(FALSE), _rackPos(src._rackPos) {
}

// StackLoc: Stack location

StackLoc::StackLoc(Repr repr, StackLoc* next,
                   UInt offset, size_t size)   
 : Loc(Loc::STACK_LOC, repr, next), _offset(offset), _size(size)

#if (GARBAGE_COLLECTION==1)
   , _origin(SP_REL)
#endif

{
}

StackLoc::StackLoc(const StackLoc& src)
 : Loc(src._kind, src._repr, NULL),
   _offset(src._offset),
   _size(src._size)

#if (GARBAGE_COLLECTION==1)
   , _origin(src._origin)
#endif

{
   _reserved = src._reserved;
}

CompLoc::CompLoc(Loc::Kind kind, Repr repr, Loc& loc, const StructRepr& structRepr, UInt index,
                 Bool addToParent/* = TRUE*/)
 : Loc(kind, repr),  _loc(loc), _structRepr(structRepr), _index(index) {
   if (addToParent)
      loc.addChild(this);
}

CompLoc::CompLoc(const CompLoc& src, Loc* parent, Bool full, MSA& msa)
 : Loc(src, full, msa),
     _loc(parent != NULL ? *parent : src._loc),
     _structRepr(src._structRepr),
     _index(src._index) {
   _loc.addChild(this);
}


// IndLoc: A location representing the value of a selected pointer

IndLoc::IndLoc(Repr repr, Loc& loc, Bool addToParent/* = TRUE*/)
 : Loc(IND_LOC, repr, NULL), _loc(loc) {
   if (addToParent)
      _loc.addChild(this);
}

IndLoc::IndLoc(const IndLoc& src, Loc* parent, Bool withVars, MSA& msa)
 : Loc(src, withVars, msa),
   _loc(parent != NULL ? *parent : src._loc) {
   _loc.addChild(this);
}

Loc* IndLoc::parent(Void) {
   return &_loc;
}

Loc* IndLoc::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) IndLoc(*this, parent, full, msa);
}

Void IndLoc::print(ostream& os, const Env& env, UInt depth/* = 0 */, Bool isDep/* = FALSE*/) const {
   os << '@';
   _loc.print(os, env, depth, TRUE);
   Loc::print(os, env, depth, isDep);
}


// An indexed loc (for array access)

IndexedLoc::IndexedLoc(Repr repr, Loc& loc, const Const& index, Bool addToParent/* = TRUE*/)
 : Loc(INDEXED_LOC, repr, NULL),
     _loc(loc) {
   if (addToParent)
      _loc.addChild(this);
   _isConst = TRUE;
   _index._const = &index;
}

IndexedLoc::IndexedLoc(Repr repr, Loc& loc, Loc& index, Bool addToParent/* = TRUE*/)
 : Loc(INDEXED_LOC, repr, NULL),
     _loc(loc), _isConst(FALSE) {
   if (addToParent)
      _loc.addChild(this);
   _index._loc = &index;
}

IndexedLoc::IndexedLoc(const IndexedLoc& src, Loc* parent, Bool full, MSA& msa)
 : Loc(src, full, msa),
     _isConst(src._isConst),
     _index(src._index),
     _loc(parent != NULL ? *parent : src._loc) {
   _loc.addChild(this);
}


Loc* Loc::parent(Void) {
   return NULL;
}

// Loc::cloneChildren: Clone children of location

Loc* Loc::cloneChildren(Bool full, MSA& msa) {
   Loc* res;
   Loc** locPtr = &res;
   Loc* child = _firstChild;
   while (child != NULL) {
      *locPtr = child->clone(this, full, msa);
      locPtr = &((*locPtr)->_next);
      child = child->_next;
   }
   return res;
}

Void Loc::addChild(Loc* loc) {
   loc->_next = _firstChild;
   _firstChild = loc;
}

Void Loc::empty(MSA& msa) {
   do {
      VarElement* varElement = _vars._varElements;
      if (varElement == NULL)
         break;
      remVarLoc(varElement->_var, *this, msa);
   } while (TRUE);

   while (_firstChild != NULL) {
       _firstChild->empty(msa);
      _firstChild = _firstChild->_next;
   }
}

// Loc::inUse:

// Returns true if location (including any child) contains a variable
// which has 'killPending' False
//
// For any variable with 'killPending' True,
// then the location is in use if 'notReadyToKill' and not '_readyToKill'

Bool Loc::inUse(Bool notReadyToKill/* = TRUE*/) const {
   VarElement* varElement = _vars._varElements;
   while (varElement != NULL) {
      if (!varElement->_var.killPending())
         return TRUE;
      else {
         if (notReadyToKill && !_readyToKill)
            return TRUE;
      }
      varElement = varElement->_next;
   }
   for (Loc* loc = _firstChild; loc != NULL; loc = loc->_next)
      if (loc->inUse(notReadyToKill))
         return TRUE;
   return FALSE;
}

// Returns true if location (including any child) contains a live variable
// which is not linked to an alternative stack location.
//
// For any variable with 'killPending' True,
// then the location is in use if 'notReadyToKill' and not '_readyToKill'

Bool Loc::requiresFlush(Void) const {
   if (_reserved)
      return FALSE;

   VarElement* varElement = _vars._varElements;
   while (varElement != NULL) {
      Var& var = varElement->var();
      if (var.isLive() &&
         stackLoc(repr(), var.locSet()) == NULL) {
         if (!var.killPending() || !_readyToKill)
            return TRUE;
      }
      varElement = varElement->_next;
   }
   for (Loc* loc = _firstChild; loc != NULL; loc = loc->_next)
      if (loc->requiresFlush())
         return TRUE;
   return FALSE;
}

Void Loc::setReadyToKill() {
   _readyToKill = TRUE;
}

Void Loc::insert(ostream & os, const ICode& code) const {
}

Void Loc::print(ostream & os, const Env& env, UInt depth/* = 0 */, Bool isDep/* = FALSE */) const {
   printCondRepr(os, env, TRUE, depth, isDep);
}

Void Loc::printCondRepr(ostream & os, const Env& env, Bool reprFlag, UInt depth, Bool isDep/* = FALSE */) const {
   if (!isDep) {
      if (reprFlag && _firstChild == NULL) {
         os << " <";
         printRepr(_repr, os);
         os << ">";
      }
      os << " {";
      VarElement* varElement = _vars.head();
      UInt n = 0;
      while (varElement != NULL) {
         if (n++ != 0)
            os << ',';
         printTranName(varElement->_var._name, os, env);
         varElement = varElement->_next;
      }
      os << '}';
      if (_firstChild != NULL) {
         os << " ( \n";
         printLocs(_firstChild, os, env, depth + 1);
         for (UInt i = 0; i < depth * 2; i++)
            os << ' ';
         os << ")\n";
      } else
         os << '\n';
   }
}

Void Loc::printLocs(const Loc* hdLoc, ostream& os, const Env& env, UInt depth/* = 0*/) {
   for (const Loc* loc = hdLoc; loc != NULL; loc = loc->_next)
      loc->print(os, env, depth);
}

Bool Loc::matchBetween(const Loc& x, const Loc& y) {
   if (x._kind == y._kind &&
       x._repr == y._repr) {
      switch (x._kind) {
         case Loc::REGISTER_LOC: 
            return ((Register&)x)._id == ((Register&)y)._id;
         case Loc::STATIC_LOC:
            return ((StaticLoc&)x)._name == ((StaticLoc&)y)._name;
         case Loc::STACK_LOC:
            return ((StackLoc&)x)._offset == ((StackLoc&)y)._offset &&
                   ((StackLoc&)x)._size == ((StackLoc&)y)._size
               
#if (GARBAGE_COLLECTION==1)
                   && ((StackLoc&)x)._origin == ((StackLoc&)y)._origin
#endif

                   ;
         case Loc::CELL_COMP_LOC:
 
            // Drop through
         case Loc::COMP_LOC:
            return ((CompLoc&)x)._index == ((CompLoc&)y)._index &&
                   matchBetween(((CompLoc&)x)._loc, ((CompLoc&)y)._loc);
          case Loc::IND_LOC:
            return matchBetween(((IndLoc&)x)._loc, ((IndLoc&)y)._loc);
         case Loc::INDEXED_LOC:
            return matchBetween(((IndexedLoc&)x)._loc, ((IndexedLoc&)y)._loc) &&
                (((IndexedLoc&)x)._isConst
                 ? ((IndexedLoc&)y)._isConst &&
                   ((IndexedLoc&)x)._index._const == ((IndexedLoc&)y)._index._const
               : matchBetween(*((IndexedLoc&)x)._index._loc, *((IndexedLoc&)y)._index._loc));
      }

   }
   return FALSE;
}

// Register

Loc* Register::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) Register(*this);
}

Void Register::insert(ostream & os, const ICode& code) const {
   os << regCodeToString(_id);
}

Void Register::print(ostream & os, const Env& env, UInt depth/* = 0*/, Bool isDep/* = FALSE*/) const {
   os << "Reg " << regCodeToString(_id);
   Loc::print(os, env, depth, isDep);
}

Loc* SpecialLoc::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) SpecialLoc(*this);
}

Void SpecialLoc::insert(ostream & os, const ICode& code) const {
   os << "special";
}

Void SpecialLoc::print(ostream & os, const Env& env, UInt depth/* = 0*/, Bool isDep/* = FALSE*/) const {
   os << "special";
   Loc::print(os, env, depth, isDep);
}

Loc* StaticLoc::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) StaticLoc(*this);
}

Void StaticLoc::insert(ostream & os, const ICode& code) const {
   os << "data";
}

Void StaticLoc::print(ostream & os, const Env& env, UInt depth/* = 0*/, Bool isDep/* = FALSE*/) const {
   os << "rack[" << _rackPos << ']';
   Loc::print( os, env, depth, isDep);
}

Void StructTemplateAllocator::slot(UInt index, Type type_) {
   if (_match) {
      StructTemplate* structTemplate = _structTemplate == NULL ? _code._structTemplates : _structTemplate;
      while (structTemplate != NULL) {
         if (_nSlots == structTemplate->_nSlots && structTemplate->_slotTypes[index] == type_) {
            _structTemplate = structTemplate;
            return;
         }
         structTemplate = structTemplate->_next;
      }
      _structTemplate = new(_code._msa) StructTemplate(_nSlots, _structTemplate, _code._msa);
      _code.addCellTemplate(_structTemplate);
      _match = FALSE;
   }
   _structTemplate->_slotTypes[index] = type_;
}

StructTemplate& StructTemplateAllocator::structTemplate(Void) {
   if (_structTemplate == NULL) {
      _structTemplate = _code._structTemplates;
      while (_structTemplate != NULL) {
         if (_structTemplate->_nSlots == 0)
            break;
         _structTemplate = _structTemplate->_next;
      }
      if (_structTemplate == NULL) {
         _structTemplate = new(_code._msa) StructTemplate(0, NULL, _code._msa);
         _code.addCellTemplate(_structTemplate);
      }
   }
   return *_structTemplate;
}

StructTemplate::StructTemplate(UInt nSlots, const StructTemplate* last, MSA& msa)
 : _next(NULL), _useCount(0), _id(0), _nSlots(nSlots), _repr(NULL) {
   _slotTypes = (Type*)msa.alloc(_nSlots * sizeof(Type));
   if (last != NULL)
      for (UInt i = 0; i < last->_nSlots; i++)
         _slotTypes[i] = last->_slotTypes[i];
}

Void StructTemplate::adjUseCount(Int diff) {
   assert((Int)_useCount + diff >= 0, "StructTemplate::adjUseCount: Invalid use count");
   _useCount += diff;
}

Void addVarLoc(Var& var, Loc& loc, MSA& msa) {
   var._locs.addElement(loc, msa);
   loc._vars.addElement(var, msa);
}

Void remVarLoc(Var& var, Loc& loc, MSA& msa) {
   var._locs.removeElement(loc, msa);
   loc._vars.removeElement(var, msa);
}


// StackLoc: A stack location

Loc* StackLoc::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) StackLoc(*this);
}

Void StackLoc::setReadyToKill(Void) {
   if (_kind == Loc::STACK_LOC) 
      _readyToKill = TRUE;
}

Void StackLoc::insert(StackLoc& loc, StackLoc** stackLocPtr) {
   while (*stackLocPtr != NULL && loc._offset < (*stackLocPtr)->_offset)
      stackLocPtr = (StackLoc**)&(*stackLocPtr)->_next;
   loc._next = *stackLocPtr;
   *stackLocPtr = &loc;
}

Void StackLoc::extract(StackLoc& loc, StackLoc** stackLocPtr) {
   while (*stackLocPtr != NULL && (*stackLocPtr) != &loc)
      stackLocPtr = (StackLoc**)&(*stackLocPtr)->_next;
   *stackLocPtr = (StackLoc*)loc._next;
}

Void StackLoc::insert(ostream& os, const ICode& code) const {
   os << "sp[" << (Int)code._sp - (Int)_offset << ']';
}

Void StackLoc::print(ostream & os, const Env& env, UInt depth/* = 0 */, Bool isDep/* = FALSE*/) const {
   os << "SP" << '[' << _offset;
   if (!isDep && _kind == Loc::STACK_LOC)
      os << '/' << _size << " bytes";
   os << ']';
   Loc::printCondRepr(os, env, TRUE, depth, isDep);
}

// CompLoc:: A component of a tuple location

Loc* CompLoc::parent(Void) {
   return &_loc;
}

Loc* CompLoc::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) CompLoc(*this, parent, full, msa);
}

Void CompLoc::print(ostream & os, const Env& env, UInt depth/* = 0 */, Bool isDep/* = FALSE*/) const {
   if (!isDep)
     for (UInt i = 0; i < depth * 2; i++)
        os << ' ';
   _loc.print(os, env, depth, TRUE);
   os << '[' << _index << ']';
   Loc::print(os, env, depth, isDep);
}

// CellCompLoc: A cell component (free variable location)

Loc* CellCompLoc::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) CellCompLoc(*this, parent, full, msa);
}

Void CellCompLoc::print(ostream & os, const Env& env, UInt depth/* = 0 */, Bool isDep/* = FALSE*/) const {
   _loc.print(os, env, depth, TRUE);
   os << ' ' << _index;
   Loc::print(os, env, depth, isDep);
}

// IndexedLoc: An element of an array location

Loc* IndexedLoc::parent(Void) {
   return &_loc;
}

Loc* IndexedLoc::clone(Loc* parent, Bool full, MSA& msa) const {
   return new(msa) IndexedLoc(*this);
}

Void IndexedLoc::print(ostream & os, const Env& env, UInt depth/* = 0 */, Bool isDep/* = FALSE*/) const {
   _loc.print(os, env, depth, TRUE);
   os << '[';
//   _index.print(os, env, depth, isDep);
   os << ']';
}


LocElement* LocSet::addElement(Loc& loc, MSA& msa) {
   LocElement** locElementPtr = &_locElements;

   while (*locElementPtr != NULL) {
      if (&(*locElementPtr)->_loc == &loc)
         return *locElementPtr;
      locElementPtr = &(*locElementPtr)->_next;
   }
   LocElement* locElement = new (msa) LocElement(NULL, loc);
   return (*locElementPtr = locElement);
}

LocElement* LocSet::findElement(const Loc& loc) const {
   LocElement* locElement = _locElements;

   while (locElement != NULL) {
      if (&locElement->_loc == &loc)
         return locElement;
      locElement = locElement->_next;
   }
   return NULL;
}

// Match a given location with a set of locations

Loc* LocSet::matchingElement(const Loc& loc) const {
   LocElement* locElement = _locElements;
   while (locElement != NULL) {
      Loc& loc2 = locElement->_loc;
      if (Loc::matchBetween(loc, loc2))
         return &loc2;
      Loc* parent = loc2.parent();
      if (parent != NULL &&
         Loc::matchBetween(loc, *parent))
         return parent;
      Loc* child = loc2._firstChild;
      while (child != NULL) {
         if (Loc::matchBetween(loc, *child))
            return child;
         child = child->_next;
      }

      locElement = locElement->_next;
   }
   return NULL;
}

Void LocSet::removeElement(const Loc& loc, MSA& msa) {
   LocElement** locElementPtr = &_locElements;

   while (*locElementPtr != NULL) {
      if (&(*locElementPtr)->_loc == &loc) {
         LocElement* locElement = *locElementPtr;
         *locElementPtr = (*locElementPtr)->_next;
         msa.free(locElement);
         return;
      }
      locElementPtr = &(*locElementPtr)->_next;
   }
}

Void LocSet::empty(MSA& msa) {
   do {
      LocElement* locElement = _locElements;
      if (locElement == NULL)
         return;
      _locElements = locElement->_next;
      msa.free(locElement);
   } while (TRUE);
}

UInt LocSet::count(Repr repr) const {
   UInt res = 0;
   LocElement* locElement = _locElements;

   while (locElement != NULL) {
      if (repr == Repr::REPR_UNKNOWN || locElement->_loc._repr == repr)
         res++;
      locElement = locElement->_next;
   }
   return res;
}

Bool LocSet::hasNonRegisterLoc(Repr repr) const {
   LocElement* locElement = _locElements;
   while (locElement != NULL) {
      if ((repr == Repr::REPR_UNKNOWN || locElement->_loc._repr == repr) &&
         !locElement->_loc.registerBased())
         return TRUE;
      locElement = locElement->_next;
   }
   return FALSE;
}

Loc* LocSet::firstRegisterBasedLoc(Repr repr) {
   LocElement* locElement = _locElements;
   while (locElement != NULL) {
      if ((repr == Repr::REPR_UNKNOWN || locElement->_loc._repr == repr) &&
         !locElement->_loc._kind == Loc::REGISTER_LOC)
         return &locElement->_loc;
      locElement = locElement->_next;
   }
   return NULL;
}

Void LocSet::print(ostream& os, const Env& env, UInt depth/* = 0*/) const {
   LocElement* locElement = _locElements;

   Int count = 0;
   while (locElement != NULL) {
      for (UInt i = 0; i < depth * 2; i++)
         os << ' ';
      locElement->_loc.print(os, env, depth);
      if (depth > 0)
         os << '\n';
      locElement = locElement->_next;
   }
}

Void LocSet::copyOf(const LocSet& locSet, MSA& msa) {
   _locElements = NULL;
   LocElement* locElement = locSet._locElements;
   while (locElement != NULL) {
      _locElements = new (msa) LocElement(_locElements,
                                          *locElement->_loc.clone(NULL, TRUE, msa));
      _locElements->_loc._vars.empty();
      locElement = locElement->_next;
   }
}



Repr LabelOperand::repr(ICode& code) {
   return Repr::REPR_LABEL;
}



Repr Operand::repr(ICode& code) {
   error("Operand::repr: unexpected");
   return Repr::REPR_UNKNOWN;
}

Repr Operand::repr(Void) {
   error("Operand::repr: unexpected");
   return Repr::REPR_UNKNOWN;
}

Bool Operand::needsEnv(Bool dst) {
   return FALSE;
}

Loc::Kind Operand::locKind(Repr repr, ICode& code) {
   return Loc::Kind::UNKNOWN_LOC;
}

Loc* Operand::loc(Repr repr, ICode& code) {
   //   assert(FALSE, "Operand::loc: unexpected");
   return NULL;
}

Loc* Operand::locEnv(ICode& code) {
   return loc(Repr::REPR_ENV_PTR, code);
}

Loc* Operand::allocLoc(Repr repr, Loc::Kind kind, Bool& reused, ICode& code) {
   return loc(repr, code);
}

Operand* Operand::genLocCode(Repr repr, ICode& code) {
   return this;
}

Operand* Operand::genLocEnvCode(ICode& code) {
   return genLocCode(Repr::REPR_ENV_PTR, code);
}

Bool Operand::suppressEnvMove(Void) const {
   return FALSE;
}

Repr VarOperand::repr(ICode& code) {
   return _var.repr(code);
}

Repr VarOperand::repr(Void) {
   return _var.repr();
}


Bool VarOperand::needsEnv(Bool dst) {
   return _var.needsEnv();
}

Loc::Kind VarOperand::locKind(Repr repr, ICode& code) {
   return code.locKind(_var, repr);
}

Loc* VarOperand::loc(Repr repr, ICode& code) {
   Loc* loc = _var.loc(repr, code);
   if (loc != NULL && loc == &code.reg(REG_rStruct))
      loc = code.indLoc(repr, *loc);
   return loc;
}

Loc* VarOperand::allocLoc(Repr repr, Loc::Kind kind, Bool& reused, ICode& code) {
   return code.allocLoc(repr, _var, reused, kind);
}

Operand* VarOperand::genLocCode(Repr repr, ICode& code) {
   Loc* loc_ = loc(repr, code);
   if (loc_ != NULL) {
      return &code.locOperand(*loc_);
   } else
      return this;
}


Repr BuiltInFnOperand::repr(ICode& code) {
   return Repr::REPR_CELL_PTR;
}

const Repr BuiltInFnOperand::repr(ICode& code) const {
   return Repr::REPR_CELL_PTR;
}

Bool BuiltInFnOperand::needsEnv(Bool dst) {
   return TRUE;
}

Operand* BuiltInFnOperand::genLocCode(Repr repr, ICode& code) {
   return this;
}

Operand* BuiltInFnOperand::genLocEnvCode(ICode& code) {
   return &code.locOperand(code.codeEnv());
}


Repr BuiltInDataConOperand::repr(ICode& code) {
   return Repr::REPR_CELL_PTR;
}

const Repr BuiltInDataConOperand::repr(ICode& code) const {
   return Repr::REPR_CELL_PTR;
}

Bool BuiltInDataConOperand::needsEnv(Bool dst) {
   return FALSE;
}

Operand* BuiltInDataConOperand::genLocCode(Repr repr, ICode& code) {
   return this;
}

Operand* BuiltInDataConOperand::genLocEnvCode(ICode& code) {
   return NULL;
}



Operand* EnvOperand::genLocCode(Repr repr, ICode& code) {
   return _operand.genLocEnvCode(code);
}

Repr ConstOperand::repr(ICode& code) {
   return _k.repr();
}

Repr ConstOperand::repr(Void) {
   return _k.repr();
}

Bool ConstOperand::needsEnv(Bool dst) {
   return _k.needsEnv(dst);
}

Bool ConstOperand::suppressEnvMove(Void) const {
   return _k.suppressEnvMove();
}

Loc* ConstOperand::loc(Repr repr, ICode& code) {
   return _k.loc(repr, code);
}

// ConstOperand::genLocCode: Identity or environment constant

Operand* ConstOperand::genLocCode(Repr repr, ICode& code) {
   Loc* loc = _k.loc(repr, code);
   return loc != NULL
      ? &(Operand&)code.locOperand(*loc)
    : this;
}

Operand* ConstOperand::genLocEnvCode(ICode& code) {
   Loc* loc = _k.loc(Repr::REPR_ENV_PTR, code);
   return loc != NULL
      ? &(Operand&)code.locOperand(*loc)
    : &(Operand&)code.constOperand(const_NULL_Env);
}


LocOperand::LocOperand(Loc& loc, Bool reserved/* = FALSE */)
 : Operand(LOC_OPERAND), _loc(loc), _reserved(reserved) {
}

Void LocOperand::insert(ostream& os, const Code& code) const {
   _loc.insert(os, (ICode&)code);
}

Repr LocOperand::repr(ICode& code) {
   return _loc.repr();
}

Repr LocOperand::repr(Void) {
   return _loc.repr();
}

Void LocOperand::setReadyToKill(Bool envOf) const {
   assert(envOf == (_loc.repr() == Repr::REPR_ENV_PTR), "LocOperand::setReadyToKill: unexpected");
   _loc.setReadyToKill();
}


Bool ThisOperand::needsEnv(Bool dst) const {
   return TRUE;
}


Repr StructOperand::repr(ICode& code) {
   return Repr(&_structTemplate.repr());
}

// DePtrOperand

Repr DePtrOperand::repr(ICode& code) {
   return (_operand->repr(code)).ptrRepr()->repr();
}

Repr DePtrOperand::repr(Void) {
   return (_operand->repr()).ptrRepr()->repr();
}

Bool DePtrOperand::needsEnv(Bool dst) {
   return !dst && _operand->needsEnv(dst);
}

Loc::Kind DePtrOperand::locKind(Repr repr, ICode& code) {
   return repr != Repr::REPR_ENV_PTR
      ? Loc::IND_LOC
    : _operand->locKind(repr, code);
}

Loc* DePtrOperand::loc(Repr repr, ICode& code) {
   return code.indLoc(repr, *_operand->loc(_operand->repr(code), code));
}

Loc* DePtrOperand::locEnv(ICode& code) {
   return _operand->locEnv(code);
}

Loc* DePtrOperand::allocLoc(Repr repr, Loc::Kind kind, Bool& reused, ICode& code) {
   return repr != Repr::REPR_ENV_PTR
      ? loc(repr, code)
      : locEnv(code);
}

Operand* DePtrOperand::genLocCode(Repr repr, ICode& code) {
   return &code.locOperand(*loc(repr, code));
}

Operand* DePtrOperand::genLocEnvCode(ICode& code) {
   return &code.locOperand(*locEnv(code));
}

// ArrayIndexOperand

Loc::Kind ArrayIndexOperand::locKind(Repr repr, ICode& code) {
   return Loc::Kind::INDEXED_LOC;
}

Repr ArrayIndexOperand::repr(ICode& code) {
   ArrayRepr arrayRepr = *_arrayOperand->repr(code).arrayRepr();
   return arrayRepr.elementRepr();
}

Loc* ArrayIndexOperand::loc(Repr repr, ICode& code) {
   Loc& loc = *_arrayOperand->loc(_arrayOperand->repr(code), code);
   return _indexOperand->kind() == CONST_OPERAND
      ? code.indexedLoc(repr, loc, ((ConstOperand*)_indexOperand)->k())
    : code.indexedLoc(repr, loc, *_indexOperand->loc(Repr::REPR_INT, code));
}

Loc* ArrayIndexOperand::locEnv(ICode& code) {
   return _arrayOperand->locEnv(code);
}

Operand* ArrayIndexOperand::genLocCode(Repr repr, ICode& code) {
   return &code.locOperand(*loc(repr, code));
}

Operand* ArrayIndexOperand::genLocEnvCode(ICode& code) {
   return _arrayOperand->genLocEnvCode(code);
}


Void LabelInstruction::genLocCode(ICode& code) {
   if (_codeLabel.withBasicBlock()) {
      BasicBlk& basicBlk = _codeLabel.basicBlk();
      if (!basicBlk.locCodeGenerated()) {
         code.emptyRegisters();
         code.initLocs(basicBlk);
         code.recoverStack();
      }
   }
}

Void GoToInstruction::genTargetLocCode(CodeLabel& codeLabel, ICode& code) {
   code.adjustStackPointer(this);
   codeLabel.basicBlk().saveLocInfo(code, TRUE);
}

Void GoToInstruction::genLocCode(ICode& code) {
   switch (target().kind()) {
      case Operand::VAR_OPERAND: {
         VarOperand& target = *(VarOperand*)_target;
         code.adjustStackPointer(this);
         _target = _target->genLocCode(Repr::REPR_UNKNOWN, code);
         target.setNextUse(code);
         break;
      }

      case Operand::LABEL_OPERAND: {
         genTargetLocCode(((LabelOperand&)*_target).codeLabel(), code);
         break;
      }

      case Operand::BUILT_IN_FN_OPERAND:
      case Operand::BUILT_IN_DATA_CON_OPERAND:
         code.adjustStackPointer(this);
         break;

      default:
         error("GoToInstruction::genLocCode: unexpected");
         break;
   }
   code.recoverStack();
}

Void ArgCheckInstruction::genLocCode(ICode& code) {

// Ensure alternative entry code generated to obtain fully applied stack depth

   BasicBlk& basicBlk = _altEntryLab.basicBlk();
   if (!basicBlk.locCodeGenerated())
      code.initLocs(basicBlk);

   _partialDepths = (UInt*)code.msa().alloc(_n * sizeof(UInt));
   for (UInt i = 0; i < _n; i++) {
      BasicBlk& basicBlk = _labels[i]->codeLabel().basicBlk();
      if (!basicBlk.locCodeGenerated())
         code.initLocs(basicBlk);
      _partialDepths[i] = basicBlk._sp;
   }
}

Void CaseInstruction::genLocCode(ICode& code) {
   Operand& x = *_x;
   _x = x.genLocCode(x.repr(code), code);
   if (x.needsEnv(FALSE) && x.repr() != Repr::REPR_STRING)
      _x_env = x.genLocEnvCode(code);

   for (UInt i = 0; i < _n; i++)
      _labels[i]->codeLabel().basicBlk().saveLocInfo(code, TRUE);
   if (_otherwise != NULL)
     _otherwise->codeLabel().basicBlk().saveLocInfo(code, TRUE);
 
   x.setNextUse(code);
   code.recoverStack();
}
 
Void ConditionalInstruction::genLocCode(ICode& code) {
   Operand* x = _x;
   Operand* y = _y;

   _x = x->genLocCode(x->repr(code), code);

   if (_op.type_() == Operator::OP_TYPE_NAME ||
       _op.type_() == Operator::OP_TYPE_TYPE

#if (GARBAGE_COLLECTION==1)
      || _op.kind() == Operator::MARK_PTR_GC_OP
      || _op.kind() == Operator::NOT_MARK_PTR_GC_OP
      || _op.kind() == Operator::MARK_EXPR_PTR_GC_OP
      || _op.kind() == Operator::NOT_MARK_EXPR_PTR_GC_OP
#endif

      )
      _x_env = x->genLocEnvCode(code);

   if (y != NULL)
      _y = y->genLocCode(x->repr(code), code);

   if (_op.type_() == Operator::OP_TYPE_NAME ||
       _op.type_() == Operator::OP_TYPE_TYPE)
      _y_env = y->genLocEnvCode(code);

   BasicBlk& basicBlk = _z->_codeLabel.basicBlk();

   basicBlk.saveLocInfo(code, _z->_codeLabel.isHead() || !_divergent);

   if (!_divergent)
      code.adjustStackPointer(basicBlk, this);

    x->setNextUse(code);
    if (y != NULL)
       y->setNextUse(code);
   _z->setNextUse(code);
   code.recoverStack();
}

Void CellAllocInstruction::genLocCode(ICode& code) {

#if (GARBAGE_COLLECTION==1)
   FrameDescrInstruction* frameDescrIns = (FrameDescrInstruction*)_prev;
#endif

   Operand& cellInfo = *_cellInfo;
   Operand* env = _env;
   Operand& dst = *_dst;
   Instruction* nextIns = _next;

#if (GARBAGE_COLLECTION==1)

// Ensure that all live variables are in stack locations

   code.flushNonStack();

#endif

   Loc& loc = genDstLoc(code, nextIns);
   _cellInfo = _cellInfo->genLocCode(Repr::REPR_UNKNOWN, code);

#if (GARBAGE_COLLECTION==1)
   if (frameDescrIns != NULL) {
      loc.setReserved(TRUE);
      code.adjustStackPointer(this);
      code.genLiveVarsGC_Code(FALSE, code.basicBlk(), code.sd_(), *frameDescrIns);
      loc.setReserved(FALSE);
   }
#endif

   cellInfo.setNextUse(code);
   if (env != NULL)
      env->setNextUse(code);
   dst.setNextUse(code);

   code.recoverStack();
}

Void IndirectionAllocInstruction::genLocCode(ICode& code) {

#if (GARBAGE_COLLECTION==1)
   FrameDescrInstruction* frameDescrIns =
      _prev != NULL && _prev->kind() == Instruction::FRAME_DESCR_INSTR
      ? (FrameDescrInstruction*)_prev
      : NULL;
#endif

   Operand& dst = *_dst;
   Var& dstVar = ((VarOperand*)_dst)->var();
   Instruction* nextIns = _next;

#if (GARBAGE_COLLECTION==1)

// Ensure that all live variables are in stack locations

   code.flushNonStack();

#endif

   Loc& loc = genDstLoc(code, nextIns);

#if (GARBAGE_COLLECTION==1)
   if (frameDescrIns != NULL) {
       loc.setReserved(TRUE);
 
      code.adjustStackPointer(this);
      code.genLiveVarsGC_Code(FALSE, code.basicBlk(), code.sd_(), *frameDescrIns);
      loc.setReserved(FALSE);
   }
#endif

// Skip over inserted moves

   code.currentIns() = nextIns->prev();

   dst.setNextUse(code);
   code.recoverStack();
}

size_t HeapAllocInstruction::size(Repr& repr, ICode& code) const {
   return 0;
}

// Allocate a heap location for a given destination operand

Loc& HeapAllocInstruction::genDstLoc(ICode& code, Instruction* nextIns) {
   Operand& dst = *_dst;
   Var& dstVar = ((VarOperand*)_dst)->var();

   Loc* loc = NULL;
   Repr repr = dstVar.repr(code);
   switch (dst.locKind(repr, code)) {
      case Loc::NATIVE_LOCAL_LOC:
      case Loc::STATIC_LOC:

//--- Common with Loc::REGISTER_LOC

      case Loc::REGISTER_LOC: {
         loc = &code.assignLoc(repr, dstVar, _dst);
         Loc& dstEnvLoc = code.assignLoc(Repr::REPR_ENV_PTR, dstVar, _dstEnv);
         Loc* envLoc = NULL;
         if (_env != NULL) {
            envLoc = _env->loc(Repr::REPR_ENV_PTR, code);
            _env = &code.locOperand(*envLoc);
         } else
            envLoc = &code.reg(REG_consEnv);
         code.insertMove(code.locOperand(*envLoc),
            code.locOperand(dstEnvLoc),
            nextIns->next());

//------ Skip over frame label and inserted move

         code.setCurrentIns(nextIns->next());
         break;
      }

      case Loc::STACK_LOC: {
         Loc& dstEnvLoc = code.assignLoc(Repr::REPR_ENV_PTR, dstVar, _dstEnv);
         Loc* envLoc = NULL;
         if (_env != NULL) {
            envLoc = _env->loc(Repr::REPR_ENV_PTR, code);
            _env = &code.locOperand(*envLoc);
         } else
            envLoc = &code.reg(REG_consEnv);    
         code.insertMove(code.locOperand(*envLoc),
                         code.locOperand(dstEnvLoc));
         loc = &code.assignLoc(repr, dstVar, _dst);
         break;
      }

      default:
         error("HeapAllocInstruction::genDstCode: unexpected");
         break;
   }

   return *loc;
}

Loc* PtrInstruction::allocDst(Loc::Kind kind, Operand& dst, Var& dstVar, Instruction* beforeIns, ICode& code) {
   Bool reused;

   Loc* loc = code.allocLoc(beforeIns == NULL ? Repr::REPR_PTR
                                            : Repr::REPR_ENV_PTR,
                            dstVar, reused);
   if (beforeIns == NULL)
      _dst = &code.locOperand(*loc);
   else {
      if (kind == Loc::STACK_LOC)
         code.insertMove(code.locOperand(code.reg(REG_consEnv)),
            code.locOperand(*loc), beforeIns);
      else
         code.appendMove(code.locOperand(code.reg(REG_consEnv)),
            code.locOperand(*loc));
   }
   addVarLoc(dstVar, *loc, code.msa());
 
   return loc;
}

Void PtrInstruction::genLocCode(ICode& code) {

#if (GARBAGE_COLLECTION==1)
   FrameDescrInstruction* frameDescrIns = (FrameDescrInstruction*)_prev;
#endif

   Operand& dst = *_dst;
   Var& dstVar = ((VarOperand*)_dst)->var();
   Repr repr;
   if ((repr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "PtrInstruction::genLocCode: Destination representation unknown");
      return;
   }

   _size = sizeOfRepr(repr.ptrRepr()->repr());

#if (GARBAGE_COLLECTION==1)

// Ensure that all live variables are in stack locations

   code.flushNonStack();

#endif

   Bool needsEnv = _dst->needsEnv(TRUE);
   Loc::Kind kind = dst.locKind(repr, code);

   Instruction* nextIns = _next;
   Loc* loc = NULL;
   switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:
      case Loc::REGISTER_LOC:
      case Loc::CELL_COMP_LOC:
      case Loc::STATIC_LOC:
         loc = allocDst(kind, dst, dstVar, NULL, code);
         if (needsEnv)
            (Void)allocDst(kind, dst, dstVar, this, code);
         break;

      case Loc::STACK_LOC:
         if (needsEnv)
            (Void)allocDst(kind, dst, dstVar, this, code);
         loc = allocDst(kind, dst, dstVar, NULL, code);
         break;

      default:
         break;
   }

   assert(loc != NULL, "PtrInstruction::genLocCode: Null location");

#if (GARBAGE_COLLECTION==1)
   if (frameDescrIns != NULL) {
      loc->setReserved(TRUE);
      code.adjustStackPointer(this);
      code.genLiveVarsGC_Code(FALSE, code.basicBlk(), code.sd_(), *frameDescrIns);
      loc->setReserved(FALSE);
   }
#endif

// Skip over any inserted moves

   code._currentIns = nextIns->_prev;

#ifdef TRACE
   if (traceFlag) {
      outStream << "PtrInstruction::genLocCode: Destination ";
      dstVar.printLocs(outStream, code.env());
   }
#endif

   dst.setNextUse(code);
   code.recoverStack();
}

Loc* PtrArrayAllocInstruction::allocDst(Loc::Kind kind, Operand& n, Operand& dst, Var& dstVar, Instruction* beforeIns, ICode& code) {
   Bool reused;

   n.setReadyToKill(beforeIns != NULL);
   Loc* loc = code.allocLoc(beforeIns == NULL ? Repr::REPR_PTR
      : Repr::REPR_ENV_PTR,
      dstVar, reused);
   if (beforeIns == NULL) {

      _n = n.genLocCode(Repr::REPR_INT, code);
      _dst = &code.locOperand(*loc);
   } 
   else {
      if (kind == Loc::STACK_LOC)
         code.insertMove(code.locOperand(code.reg(REG_consEnv)),
            code.locOperand(*loc), beforeIns);
      else
         code.appendMove(code.locOperand(code.reg(REG_consEnv)),
            code.locOperand(*loc));
   }
   addVarLoc(dstVar, *loc, code.msa());

   return loc;
}

Void PtrArrayAllocInstruction::genLocCode(ICode& code) {
   Operand& n = *_n;
   _n->setKillPending();
   
#if (GARBAGE_COLLECTION==1)
   FrameDescrInstruction* frameDescrIns = (FrameDescrInstruction*)_prev;
#endif

   Operand& dst = *_dst;
   Var& dstVar = ((VarOperand*)_dst)->var();
   Repr repr;
   if ((repr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "PtrInstruction::genLocCode: Destination representation unknown");
      return;
   }
   _size = sizeOfRepr(repr.ptrRepr()->repr().arrayRepr()->elementRepr());
   
#if (GARBAGE_COLLECTION==1)

   // Ensure that all live variables are in stack locations

   code.flushNonStack();

#endif

   Bool needsEnv = _dst->needsEnv(TRUE);
   Loc::Kind kind = dst.locKind(repr, code);

   Instruction* nextIns = _next;
   Loc* loc = NULL;
   switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:
      case Loc::REGISTER_LOC:
      case Loc::CELL_COMP_LOC:
      case Loc::STATIC_LOC:
         loc = allocDst(kind, n, dst, dstVar, NULL, code);
         if (needsEnv)
            (Void)allocDst(kind, n, dst, dstVar, this, code);
         break;

      case Loc::STACK_LOC:
         if (needsEnv)
            (Void)allocDst(kind, n, dst, dstVar, this, code);
         loc = allocDst(kind, n, dst, dstVar, NULL, code);
         break;

      default:
         break;
   }

   assert(loc != NULL, "PtrArrayAllocInstruction::genLocCode: Null location");

#if (GARBAGE_COLLECTION==1)
   if (frameDescrIns != NULL) {
      loc->setReserved(TRUE);
      code.adjustStackPointer(this);
      code.genLiveVarsGC_Code(FALSE, code.basicBlk(), code.sd_(), *frameDescrIns);
      loc->setReserved(FALSE);
   }
#endif

   // Skip over any inserted moves

   code._currentIns = nextIns->_prev;

#ifdef TRACE
   if (traceFlag) {
      outStream << "PtrInstruction::genLocCode: Destination ";
      dstVar.printLocs(outStream, code.env());
   }
#endif

   n.setNextUse(code);
   dst.setNextUse(code);
   code.recoverStack();
}

Void EnterCellInstruction::genLocCode(ICode& code) {
   code.adjustStackPointer(this);
   VarOperand& x = (VarOperand&)*_x;
   _x = x.genLocCode(x.repr(code), code);
   x.setNextUse(code);
}



Void ReturnInstruction::genLocCode(ICode& code) {
   if (_form != ReturnInstruction::EXIT)
      code.adjustStackPointer(this);
}


// BasicBlk::saveLocInfo: Save location information for a basic block.

Void BasicBlk::saveLocInfo(ICode& code, Bool flushNonLocal) {
   _defer = FALSE;

   if (_locCodeGenerated)
      return;

#ifdef TRACE
   if (traceFlag) {
      outStream << "Saving var info for instruction " << _firstIns->_seqNo << '\n';
      outStream.flush();
   }
#endif

   code.saveVarLocs(*this, flushNonLocal);

   CodeLabel& firstLabel = ((LabelInstruction*)_firstIns)->codeLabel();
   if (firstLabel._lambda == NULL) {
      if (firstLabel._head) {
         _sp = code._sd;
         _sd = code.inputStackDepth(code._stackLocs, _vars);
      } else {
         _sd = code._sd;
         _sp = code._sp;
      }
      _saveFlag = TRUE;

#ifdef TRACE
      if (traceFlag) {
         outStream << ", d =  " << _sd  << '\n';
         outStream << ", sp = " << _sp << '\n';
         outStream.flush();
      }
#endif
   }
}

// Print live variables for a basic block
// 'killPendingFlag' determines inclusion/exclusion for any marked 'killPending'

Void BasicBlk::printLiveVars(Bool killPendingFlag, const Env& env) {
   VarElement* varElement = _vars.head();

   UInt n = 0;
   while (varElement != NULL) {
      Var& var = varElement->var();
      if (var.kind() != Var::EXT_GLOBAL_VAR &&
         var.isLive() &&
         !(killPendingFlag && var.killPending())) {
         LocElement* locElement = var.locSet().head();
         UInt locCount = 0;
         while (locElement != NULL) {
            Loc& loc = locElement->loc(); 
            if (locCount == 0) {
               printName(var.name(), outStream, env.nameTable());
               outStream << " :: ";
               printTypeSig(var.typeSig(), 0, FALSE, outStream, env);
               outStream << '\n';
            }
            outStream << "   ";
            loc.print(outStream, env);
            n++;
            locCount++;
            locElement = locElement->next();
         }
      }
      varElement = varElement->_next;
   }
}

Loc* BasicBlk::matchLoc(Loc& loc) {
   VarElement* varElement = _vars.head();
   while (varElement != NULL) {
      Loc* matchingLoc = varElement->_locs.matchingElement(loc);
      if (matchingLoc != NULL)
         return matchingLoc;
      varElement = varElement->_next;
   }
   return NULL;
}



Void BasicBlk::genLocCode(ICode& code) {
   Instruction* ins = _firstIns;

   while (ins != NULL) {
      code._currentIns = ins;

#ifdef TRACE
      if (traceFlag)
         outStream << "Generating location code for instruction " << ins->_seqNo << '\n';
      if (ins->_seqNo == instructionProbeSeqNo) {
         outStream << "BasicBlk::genLocCode: probe hit\n";
         outStream.flush();
         traceFlag = TRUE;
      }
#endif

      ins->genLocCode(code);
      if (code._currentIns != ins) {

//------ An instruction has been appended, check if last

         if (ins == _lastIns)
            _lastIns = code._currentIns;
         ins = code._currentIns;
      }
      if (ins == _lastIns) {
         BasicBlk* basicBlk;
         if ((basicBlk = implicitSuccessor()) != NULL) {
            CodeLabel& codeLabel = ((LabelInstruction&)*ins->_next).codeLabel();
            if (codeLabel._head && code.insertJumpsToHeadLabels()) {
               Instruction* goToIns =
                  new(code.msa()) GoToInstruction(code.labelOperand(codeLabel));
               ((Code)code).insertInstruction(goToIns, ins->_next);
               code._currentIns = goToIns;
               goToIns->genLocCode(code);
            } else {
               if (codeLabel._head) {
                  code.adjustStackPointer(ins->_next);
                  basicBlk->saveLocInfo(code, FALSE);
                  if (codeLabel._head && basicBlk->_saveFlag)
                     code._sd = basicBlk->_sp;
               } else
                  if (!basicBlk->locCodeGenerated()) {
                     code.adjustStackPointer(ins->_next);
                     basicBlk->saveLocInfo(code, TRUE);
                  } else
                     code.adjustStackPointer(*basicBlk, ins->_next);
            }
         }
         break;
      }
      ins = ins->_next;
   }
}

Loc::Kind Var::locKind(Repr repr) {
   switch (_kind) {
      case Var::EXTRA_FORMAL_PARAM_VAR:
      case Var::EXTRA_ARG_VAR:
      case Var::CONTINUATION_VAR:

#if (CLOSURE_UPDATE==1)
      case Var::UPDATE_CELL_VAR:
#endif

         return Loc::STACK_LOC;

      case Var::NON_GLOBAL_FREE_VAR:
         return Loc::CELL_COMP_LOC;

      case Var::CLOSURE_VAR:
         return Loc::REGISTER_LOC;

      case Var::LOCAL_VAR:
         return Loc::STACK_LOC;
 
      case Var::FORMAL_PARAM_VAR:
      case Var::LOCAL_ARG_VAR:
      case Var::ARG_VAR:
      case Var::RESULT_VAR:
         return !(repr.tag() == Repr::REPR_STRUCT ||
                  repr.tag() == Repr::REPR_UNION ||
                  repr.tag() == Repr::REPR_FILE_HANDLE)
            ? Loc::REGISTER_LOC
          : Loc::IND_LOC;         // Structures and unions ae by reference

      case Var::GLOBAL_VAR:
      case Var::EXT_GLOBAL_VAR:
         return Loc::STATIC_LOC;

      default:
         error("Var::locKind: unknown kind");
         return Loc::UNKNOWN_LOC;
   }
}

// Return best location for variable representation

Loc* Var::loc(Repr repr, ICode& code) const {
   switch (_kind) {
      case Var::NON_GLOBAL_FREE_VAR:
         if (repr != Repr::REPR_ENV_PTR) {
            Loc& closureLoc = *closure()->bestLocOrNULL(Repr::REPR_CELL_PTR);
            return &code.allocCellCompLoc(*this, repr);
         } else
            return closure()->bestLocOrNULL(Repr::REPR_ENV_PTR);

       default:
         if (repr == Repr::REPR_UNKNOWN)
            repr = code.typeReprInfo(typeSig())._repr;
         
         return bestLocOrNULL(repr);
   }
}

// Var::bestLocOrNULL: return best location or NULL if none available
// regFlag, True if a register may be returned, False otherwise

Loc* Var::bestLocOrNULL(Repr repr, Bool regFlag/* = TRUE*/) const {
   LocElement* locElement = _locs._locElements;
   Loc* best = NULL;
   while (locElement != NULL) {
      Loc& loc = locElement->_loc;
      if (repr == loc._repr &&
         (regFlag || !loc.registerBased()) &&
         (best == NULL || loc._kind < best->_kind))
         best = &loc;
      locElement = locElement->_next;
   }
   return best;
}

// Var::bestLoc: return best location
// regFlag, True if a register may be returned, False otherwise


Loc& Var::bestLoc(Repr repr, Bool regFlag/* = TRUE*/) const {
   Loc* loc = bestLocOrNULL(repr, regFlag);
   if (loc == NULL)
      error("Var::bestLoc: No available location");
   return *loc;
}

Bool Var::hasReservedLocs(Void) const {
   LocElement* locElement = _locs._locElements;
   while (locElement != NULL) {
      if (locElement->loc().reserved())
         return TRUE;
      locElement = locElement->_next;
   }
   return FALSE;
}

Loc& Var::headLoc(Void) const {
   assert(!_locs.isEmpty(), "Var::headLoc: unexpected");
   return _locs.head()->_loc;
}

UInt Var::stackOffset(Void) {
   LocElement* locElement = _locs._locElements;
   while (locElement != NULL) {
      if (locElement->_loc._kind == Loc::STACK_LOC)
         return ((StackLoc&)locElement->_loc)._offset;
      locElement = locElement->_next;
   }
   error("Var::stackOffset: end of list");
   return 0;
}



Bool Var::inLoc(Loc& loc, Repr repr) {
   LocElement* locElement = _locs._locElements;
   while (locElement != NULL) {
      if (Loc::matchBetween(locElement->_loc, loc))
         return TRUE;
      locElement = locElement->_next;
   }
   return FALSE;
}

Void Var::addLoc(Loc* loc, MSA& msa) {
   _locs.addElement(*loc, msa);
}

Repr Var::repr(ICode& code) {
   if (_reprInfo._repr.isUnknown())
      _reprInfo = code.typeReprInfo(_typeSig);
   return _reprInfo._repr;
}

Repr Var::repr(Void) const {
   assert(!_reprInfo._repr.isUnknown(), "Var::repr: Unknown representation");
   return _reprInfo._repr;
}

Bool Var::needsEnv(Void) {
   assert(!_reprInfo._repr.isUnknown(), "Var::needsEnv Unknown if needs env");
   return _reprInfo._needsEnv;
//   return _kind != Var::NON_GLOBAL_FREE_VAR && _reprInfo._needsEnv;
}

// Var::setReadyToKill: Set kill pending on all locations for a given representation

Void Var::setReadyToKill(Bool envOf) {
   LocElement* locElement = _locs._locElements;
   while (locElement != NULL) {
      Loc& loc = locElement->_loc;
      if (envOf == (loc.repr() == Repr::REPR_ENV_PTR))
         loc.setReadyToKill();
      locElement = locElement->_next;
   }
}


Void CompoundDataInstructionTrait::assignComps(Repr repr, Loc* dstLoc, UInt index,
                                               Instruction* beforeIns, ICode& code) {
   size_t size = sizeOfRepr(repr);  // Force calculation of offsets

   for (UInt i = 1; i <= _nComps; i++) {
      Repr compRepr = _comps[i]->repr(code);
      size_t offset = repr.structRepr()->offset(i);
      CompoundDataInstructionTrait* nest = NULL;
      if (_comps[i]->kind() == Operand::VAR_OPERAND) {
         nest = getNest(i + 1);
         if (nest != NULL) {
            dstLoc = code.compLoc(repr, *dstLoc, _structTemplate.repr(), i);
            nest->assignComps(compRepr, dstLoc, i, beforeIns, code);
            return;
         } 
      }

      if (nest == NULL)
         code.insertMove(*_comps[i]->genLocCode(compRepr, code),
                         code.compLocOperand(compRepr, *dstLoc, _structTemplate.repr(), i),
                         beforeIns);
      _comps[i]->setNextUse(code);
   }
}

Void CompoundDataInstructionTrait::assignComps(StructTemplate& structTemplate, Loc* dstLoc,
   Instruction* beforeIns, ICode& code) {

   for (UInt i = 0; i < _nComps; i++) {
      Type type = structTemplate._slotTypes[i];
      Repr compRepr = typeRepr(type, code.typeTable(), code.nameTable());
      CompoundDataInstructionTrait* nest = NULL;
      if (_comps[i]->kind() == Operand::VAR_OPERAND) {
         nest = getNest(i + 1);
         if (nest != NULL) {
            dstLoc = code.compLoc(compRepr, *dstLoc, _structTemplate.repr(), i + 1);
            nest->assignComps(compRepr, dstLoc, i, beforeIns, code);
            return;
         }
      }

      if (nest == NULL) {
         Type type = structTemplate._slotTypes[i];
         code.insertMove(*_comps[i]->genLocCode(compRepr, code),
            code.compLocOperand(compRepr, *dstLoc, _structTemplate.repr(), i + 1),
            beforeIns);
      }
      _comps[i]->setNextUse(code);
   }
}



Loc* ReserveInstruction::allocLoc(Repr repr, Var& var, Loc::Kind kind, ICode& code) {
   Bool reused;
   Loc* loc = loc = code.allocLoc(repr, var, reused, kind);
   addVarLoc(var, *loc, code.msa());
   loc->setReserved(TRUE);
   return loc;
}

Void ReserveInstruction::genLocCode(ICode& code) {
   Operand& x = *_x;
   Var& var = ((VarOperand*)_x)->var();
   Repr repr = var.repr(code);
   Bool needsEnv = var.needsEnv();

   Loc* loc     = NULL;
   Loc* envLoc = NULL;

// A reserve instruction indicates conditional code
// So ensure that all live variables are in stack locations

   code.flushNonStack();

   Loc::Kind kind = _x->locKind(repr, code);
   Bool reused = FALSE;
   switch (kind) {
      case Loc::STATIC_LOC:
      case Loc::NATIVE_LOCAL_LOC:
      case Loc::CELL_COMP_LOC:
      case Loc::REGISTER_LOC:
      case Loc::IND_LOC:
         loc = allocLoc(repr, var, kind, code);
         if (needsEnv)
            envLoc = allocLoc(Repr::REPR_ENV_PTR, var, kind, code);
         break;

      case Loc::STACK_LOC:
        if (needsEnv)
            envLoc = allocLoc(Repr::REPR_ENV_PTR, var, kind, code);
        loc = allocLoc(repr, var, kind, code);
        break;

      default:
         break;
   }
   assert(loc != NULL, "ReserveInstruction::genLocCode: Null location");
   _x = &code.locOperand(*loc);
   if (envLoc != NULL)
      _x_env = &code.locOperand(*envLoc);

   x.setNextUse(code);
   code.recoverStack();
}

Void AssignInstruction::assignLoc(Operand& dst, Repr repr, Loc::Kind kind,
                                  Instruction* beforeIns,
                                  LocCodeGenState& state, ICode& code) {
   setSrcReadyToKill(beforeIns, state, code);

   Bool reused;
   Loc& loc = *dst.allocLoc(beforeIns == NULL ? repr : Repr::REPR_ENV_PTR, kind,
                            reused, code);

   if (beforeIns == NULL) {
      genSrcLocCode(beforeIns, state, code);
      _dst = &code.locOperand(loc);
   } else
      genSrcLocCode(beforeIns, state, code);
}


Void AssignInstruction::genLocCode(LocCodeGenState& state, ICode& code) {
   Operand& dst = *_dst;
   Repr repr;
   if ((repr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "AssignInstruction::genLocCode: Destination representation unknown");
      return;
   }
   Bool needsEnv = dst.needsEnv(TRUE);
   Loc::Kind kind = dst.locKind(repr, code);

   Instruction* nextIns = _next;
   switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:

//--- Common with Loc::REGISTER_LOC
      case Loc::REGISTER_LOC:
         assignLoc(dst, repr, kind, NULL, state, code);
         if (needsEnv)
            assignLoc(dst, Repr::REPR_ENV_PTR, kind, this, state, code);
         break;

      case Loc::STACK_LOC:
         if (needsEnv)
            assignLoc(dst, Repr::REPR_ENV_PTR, kind, this, state, code);
         assignLoc(dst, repr, kind, NULL, state, code);
         break;

      default:
         assignLoc(dst, repr, kind, NULL, state, code);
         break;
   }

   setSrcNextUse(state, code);
   dst.setNextUse(code);

   code.recoverStack();
}

// PtrArrayAssignInstruction:  T.B.D. remove and use move instead


PtrArrayAssignLocCodeGenState::PtrArrayAssignLocCodeGenState(Operand& src, ICode& code)
 : _src(src) {
   _srcRepr = src.repr(code);
}


Void PtrArrayAssignInstruction::setSrcReadyToKill(Instruction* beforeIns, LocCodeGenState& state, ICode& code) {
   _src->setReadyToKill(beforeIns == NULL);
}

Void PtrArrayAssignInstruction::genSrcLocCode(Instruction* beforeIns, LocCodeGenState& state, ICode& code) {
   _src = _src->genLocCode(((PtrArrayAssignLocCodeGenState&)state)._srcRepr, code);
}

Void PtrArrayAssignInstruction::setSrcNextUse(LocCodeGenState& state, ICode& code) {
   ((PtrArrayAssignLocCodeGenState&)state)._src.setNextUse(code);
}


Void PtrArrayAssignInstruction::genLocCode(ICode& code) {
   PtrArrayAssignLocCodeGenState state(*_src, code);
   AssignInstruction::genLocCode(state, code);
}

Bool MonadicInstruction::usesRegisters(Void) const {
   return FALSE;
}

Bool MonadicInstruction::needsSrcEnvLoc(Void) const {
   return FALSE;
}

Bool LVP_Instruction::needsSrcEnvLoc(Void) const {
   return TRUE;
}

Loc& MonadicInstruction::allocLoc(Repr repr, Operand& dst, Loc::Kind kind,
                                  Bool& reused, ICode& code) const {
   return *dst.allocLoc(repr, kind, reused, code);
}

Operand* MonadicInstruction::genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code) {
   return src.genLocCode(repr, code);
}

Operand* LVP_Instruction::genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code) {
   assert(src.kind() == Operand::SELECT_OPERAND &&
          ((SelectOperand&)src).byPtr(),
          "LVP_Instruction::genSrcLocCode: expected by-pointer select source");
   return src.genLocCode(repr, code);
}

Operand* MonadicInstruction::genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code) {
   return src.genLocEnvCode(code);
}

Operand* MonadicInstruction::genDstLocCode(Loc& loc, ICode& code) {
   return &code.locOperand(loc.kind() != Loc::STATIC_LOC
      ? *loc.clone(NULL, FALSE, code.msa())
    : loc);
}

Operand* MonadicInstruction::genDstLocEnvCode(Operand& src, Var* srcVar, Loc& loc, ICode& code) {
   if (_kind != Instruction::FROM_PLAIN_INSTR) {

#if (SERIALISATION==1)
      if (((MonadicOpInstruction&)*this).op().kind() != Operator::EXTRACT_OP) {
         Loc* srcLoc = src.locEnv(code);
         return (!(srcLoc != NULL && Loc::matchBetween(*srcLoc, loc)))
            ? genSrcLocEnvCode(src, srcVar, code)
            : NULL;
      } else
#endif

         return &code.locOperand(code.reg(REG_consEnv));
   } else
      return((FromPlainInstruction*)this)->env()->genLocCode(Repr::REPR_ENV_PTR, code);
}


Void MonadicInstruction::assignLoc(Operand& src, Repr srcRepr, Var* srcVar,
                                   Operand& dst, Repr repr, Loc::Kind kind, Var* dstVar,
                                   Bool& needsEnv, Bool& suppressed,
                                   Instruction* beforeIns,
                                   ICode& code) {
Bool reused;

#ifdef TRACE
   if (traceFlag && srcVar != NULL) {
      outStream << "MonadicInstruction::assignLoc: Source ";
      srcVar->printLocs(outStream, code.env());
   }
#endif

   if (srcVar != NULL) {

//--- Note that the following 'this->'  is required for disambiguation

      if (this->kind() == Instruction::MOVE_INSTR && dstVar != NULL &&
          srcRepr == repr &&
          ((srcVar->kind() == Var::FORMAL_PARAM_VAR ||
            srcVar->kind() == Var::EXTRA_FORMAL_PARAM_VAR ||
            srcVar->kind() == Var::LOCAL_VAR ||
            srcVar->kind() == Var::LOCAL_ARG_VAR ||
            srcVar->kind() == Var::RESULT_VAR) &&
           dstVar->kind() == Var::LOCAL_VAR ||
           dstVar->kind() == Var::LOCAL_ARG_VAR ||
           srcVar->locSet().firstRegisterBasedLoc(repr) != NULL && dstVar->kind() == Var::RESULT_VAR) &&
          repr != Repr::REPR_LABEL && !dstVar->hasReservedLocs()) {

//------ Simply assign source locations to destination for simple local variable moves

         LocElement* locElement = srcVar->_locs.head();
         while (locElement != NULL) {
            addVarLoc(*dstVar, locElement->_loc, code.msa());
            locElement = locElement->_next;
         }
         if (beforeIns == NULL)
            suppressed = TRUE;

#ifdef TRACE
         if (traceFlag) {
            outStream << "MonadicInstruction::assignLoc: Suppressed move, destination ";
            dstVar->printLocs(outStream, code.env());
         }
#endif

         return;
      }
   }

   src.setReadyToKill(beforeIns != NULL);
   Loc& loc = allocLoc(repr, dst, kind, reused, code);

   // Suppress env move for built-in names

   if (!(beforeIns != NULL && code.suppressEnvMove() &&
      _src->suppressEnvMove())) {
      if (!(_kind == Instruction::MOVE_INSTR && srcVar != NULL && srcVar->inLoc(loc, repr))) {
         if (beforeIns == NULL) {
            _src = genSrcLocCode(src, srcRepr, srcVar, code);
            _dst = genDstLocCode(loc, code);
         } else {
            Operand* operand = genDstLocEnvCode(src, srcVar, loc, code);
            if (operand != NULL)
               code.insertMove(*operand, code.locOperand(loc), beforeIns);
         }
      } else
         if (beforeIns == NULL)
            suppressed = TRUE;
   }
   if (dstVar != NULL)
      addVarLoc(*dstVar, loc, code.msa());

// The following is a minor optimisation mainly affecting
// cell register set-up and entry

   if (Instruction::kind() == Instruction::MOVE_INSTR &&
       srcVar != NULL && srcVar->isLive() &&
      loc.registerBased())
      addVarLoc(*srcVar, loc, code.msa());
}

Void MonadicInstruction::genLocCode(ICode& code) {
   Operand& src = *_src;
   Operand& dst = *_dst;
   Var* dstVar = _dst->kind() == Operand::VAR_OPERAND
      ? &((VarOperand*)_dst)->var()
      : NULL;

   Repr dstRepr;
   if ((dstRepr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "MonadicInstruction::genLocCode: Destination representation unknown");
      return;
   }

#if (CLOSURE_UPDATE==1)

   // Semantics of update ensures update thunk and original cell in same environment

   Bool isUpdate =
      _kind == Instruction::UPDATE_INSTR ||
      _kind == Instruction::MONADIC_OP_INSTR &&
         ((MonadicOpInstruction&)*this).op().kind() == Operator::UPDATE_OP;
#endif

#if (GARBAGE_COLLECTION==1)
   FrameDescrInstruction* updateFrameDescrIns =
      _kind == Instruction::UPDATE_INSTR &&
      _prev != NULL && _prev->kind() == Instruction::FRAME_DESCR_INSTR
      ? (FrameDescrInstruction*)_prev
      : NULL;
#endif

   Bool needsEnv =

#if (CLOSURE_UPDATE==1)  
      !isUpdate &&
#endif

      dst.needsEnv(TRUE) && (dstVar == NULL || dstVar->kind() != Var::NON_GLOBAL_FREE_VAR);

   Var* srcVar = src.kind() == Operand::VAR_OPERAND
      ? &((VarOperand&)src).var()
      : NULL;

   src.setKillPending();
   if (!needsEnv

#if (GARBAGE_COLLECTION==1)
       && updateFrameDescrIns == NULL
#endif

      )
      src.setReadyToKill(TRUE);

   Loc::Kind kind = dst.locKind(dstRepr, code);
   Instruction* nextIns = _next;
   Bool suppressed = FALSE;
   Repr srcRepr;
   if ((srcRepr = src.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "MonadicInstruction::genLocCode: Source representation unknown");
      return;
   }
   Bool srcNeedsEnv =
      needsSrcEnvLoc()

#if (CLOSURE_UPDATE==1)  
      && !isUpdate
#endif

      ;

#if (CLOSURE_UPDATE==1)
   Loc* updateDstEnvLoc = isUpdate && dstVar != NULL && dst.needsEnv(TRUE)
      ? dst.locEnv(code)
      : NULL;
#endif

   if (usesRegisters()

#if (GARBAGE_COLLECTION==1)
       || updateFrameDescrIns != NULL
#endif

      ) {
      code.flushNonStack();
      code.adjustStackPointer(this);
   }

   switch (kind) {
      case Loc::STATIC_LOC:
      case Loc::NATIVE_LOCAL_LOC:
      case Loc::CELL_COMP_LOC:
      case Loc::REGISTER_LOC:
      case Loc::IND_LOC:
         assignLoc(src, srcRepr, srcVar,
                   dst, dstRepr, kind, dstVar, needsEnv, suppressed, NULL, code);
#if (CLOSURE_UPDATE==1)
         if (updateDstEnvLoc != NULL && !dstVar->inLoc(*updateDstEnvLoc, Repr::REPR_ENV_PTR))
            addVarLoc(*dstVar, *updateDstEnvLoc, code.msa());
#endif
         if (srcNeedsEnv)
            _src_env = src.genLocEnvCode(code);
         if (needsEnv)
            assignLoc(src, Repr::REPR_ENV_PTR, srcVar,
                      dst, Repr
               ::REPR_ENV_PTR,
                      dst.locKind(Repr::REPR_ENV_PTR, code), dstVar,
                                  needsEnv, suppressed, nextIns, code);
         break;

      case Loc::COMP_LOC:
      case Loc::STACK_LOC:
         if (srcNeedsEnv)
            _src_env = src.genLocEnvCode(code);
         if (needsEnv)
            assignLoc(src, Repr::REPR_ENV_PTR, srcVar,
               dst, Repr::REPR_ENV_PTR,
               dst.locKind(Repr::REPR_ENV_PTR, code), dstVar,
               needsEnv, suppressed, this, code);
          assignLoc(src, srcRepr, srcVar,
                   dst, dstRepr, kind, dstVar, needsEnv, suppressed, NULL, code);
#if (CLOSURE_UPDATE==1)
         if (updateDstEnvLoc != NULL && !dstVar->inLoc(*updateDstEnvLoc, Repr::REPR_ENV_PTR))
            addVarLoc(*dstVar, *updateDstEnvLoc, code.msa());
#endif
         break;

      default:
         error("MonadicInstruction::genLocCode: unexpected");
         break;
   }

#if (GARBAGE_COLLECTION==1)
   if (updateFrameDescrIns != NULL)
      code.genLiveVarsGC_Code(FALSE, code.basicBlk(), code.sd_(), *updateFrameDescrIns);
#endif

   if (suppressed)
      code.removeInstruction(this);

// Skip over any inserted moves

   code._currentIns = nextIns->_prev;

   src.setNextUse(code);
   dst.setNextUse(code);

#ifdef TRACE
   if (traceFlag && dstVar != NULL) {
      outStream << "MonadicInstruction::genLocCode: Destination ";
      dstVar->printLocs(outStream, code.env());
   }
#endif

   code.recoverStack();
}

Bool MonadicOpInstruction::usesRegisters(Void) const {

#if (SERIALISATION==1)
   return _op.kind() == Operator::EXTRACT_OP;
#else
   return FALSE;
#endif

}

Bool MonadicOpInstruction::needsSrcEnvLoc(Void) const {
   if (_src->needsEnv(FALSE))
      switch (_op.kind()) {
         case Operator::LENGTH_STRING_OP:
         case Operator::FROM_THUNK_OP:
         case Operator::CELL_INFO_OF_OP:
         case Operator::FROM_INT_OP:
            return FALSE;

         default:
            return TRUE;
      }
   else
      return FALSE;
}
Operand* MonadicOpInstruction::genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code) {
   return src.genLocCode(repr, code);
}

Operand* MonadicOpInstruction::genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code) {
   return _op.kind() != Operator::ALLOC_PTR_ARRAY_OP
      
#if (SERIALISATION==1)      
      && _op.kind() != Operator::EXTRACT_OP
#endif

      ? src.genLocCode(Repr::REPR_ENV_PTR, code)
    : &code.locOperand(code.reg(REG_consEnv));
}

Void ContinuationInstruction::genLocCode(ICode& code) {

#if (GARBAGE_COLLECTION==1)
   FrameDescrInstruction* frameDescrIns = !_gcFlag
      ? (FrameDescrInstruction *)_prev
      : NULL; 
#endif

   Operand& dst = *_dst;
   Var* dstVar = &((VarOperand*)_dst)->var();
   Repr repr = code.labelRepr();

   //    code.flushReturnRegs();
   //    code.flushNonLocalVar(code._basicBlk->_vars);
   ((LabelOperand&)*_src).codeLabel().basicBlk().saveLocInfo(code,
      
#if (CLOSURE_UPDATE==1)      
      !_forUpdate
#else
      TRUE
#endif
   
   );

   Bool reused;
   Loc& loc = allocLoc(repr, dst, Loc::STACK_LOC, reused, code);
   _dst = &code.locOperand(loc);

   addVarLoc(*dstVar, loc, code.msa());

   dst.setNextUse(code);

#ifdef TRACE
   if (traceFlag && dstVar != NULL) {
      outStream << "ContinuationInstruction::genLocCode: Destination ";
      dstVar->printLocs(outStream, code.env());
   }
#endif

#if (GARBAGE_COLLECTION==1)
   if (frameDescrIns != NULL)
      code.genLiveVarsGC_Code(TRUE,
                              ((LabelOperand&)*_src).codeLabel().basicBlk(),
                               code.sd_(), *frameDescrIns);
#endif

   code.recoverStack();
}

Void ConsInstruction::assignDst(Operand& src, Repr srcRepr,
   Repr repr, Var& dstVar,
   Instruction* beforeIns, ICode& code) {
   Bool reused;

   Loc* loc = NULL;
   src.setReadyToKill(FALSE);
   if (beforeIns == NULL) {
      loc = code.allocLoc(repr, dstVar, reused);
      _dst = &code.locOperand(*loc);
      _src = src.genLocCode(repr, code);
   } else {
      loc = code.allocLoc(repr, dstVar, reused);
      Operand* srcOperand = src.genLocCode(repr, code);
      if (srcOperand != NULL)
         code.insertMove(*srcOperand, code.locOperand(*loc), beforeIns);
   }

   addVarLoc(dstVar, *loc, code.msa());
}

Void ConsInstruction::genLocCode(ICode& code) {
   Operand& src = *_src;
   Repr srcRepr = src.repr(code);
   src.setKillPending();

   Operand& dst = *_dst;
   Var& dstVar = ((VarOperand*)_dst)->var();

   Repr repr;
   if ((repr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "ConsInstruction::genLocCode: Destination representation unknown");
      return;
   }
   Bool needsEnv = dst.needsEnv(TRUE);
   Loc::Kind kind = dst.locKind(repr, code);

   Instruction* nextIns = _next;
   switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:

         //------ Common with Loc::REGISTER_LOC
      case Loc::REGISTER_LOC:
         assignDst(src, srcRepr, repr, dstVar, NULL, code);
         if (needsEnv)
            assignDst(src, Repr::REPR_ENV_PTR, Repr::REPR_ENV_PTR, dstVar, this, code);
         break;

      case Loc::STACK_LOC:
         if (needsEnv)
            assignDst(src, Repr::REPR_ENV_PTR, Repr::REPR_ENV_PTR, dstVar, this, code);
         assignDst(src, srcRepr, repr, dstVar, NULL, code);
         break;

      default:
         error("ConsInstruction::genLocCode: unexpected");
         break;
   }

   src.setNextUse(code);
   dst.setNextUse(code);

// Skip over any inserted moves

   code._currentIns = nextIns->_prev;

#ifdef TRACE
   if (traceFlag) {
      outStream << "ConsInstruction::genLocCode: Destination ";
      dstVar.printLocs(outStream, code.env());
   }
#endif

   code.recoverStack();
}

Operand* Cons_A_S_Instruction::genDstLocCode(Loc& loc, ICode& code) {
   return loc.repr() == Repr::REPR_TAG
      ? MoveInstruction::genDstLocCode(loc, code)
    : &code.compLocOperand(Repr::REPR_TAG, loc, *loc.repr().structRepr(), 1);
}

Void Cons_S_Instruction::assignDst(Operand& src, Repr srcRepr,
                                   Repr repr, Var& dstVar,
                                   Instruction* beforeIns, ICode& code) {
   Bool reused;

   src.setReadyToKill(beforeIns != NULL);
   Loc& loc= *code.allocLoc(repr, dstVar, reused);
   if (beforeIns == NULL)
      _dst = &code.locOperand(loc);
   else {
      Operand* srcOperand = _src->genLocEnvCode(code);
      if (srcOperand != NULL)
         code.insertMove(*srcOperand, code.locOperand(loc), beforeIns);
   }

   addVarLoc(dstVar, loc, code.msa());
}

Void Cons_S_Instruction::genLocCode(ICode& code) {
   Operand& src = *_src;
   Repr srcRepr = src.repr(code);
   src.setKillPending();

   Operand& dst = *_dst;
   Var& dstVar = ((VarOperand*)_dst)->var();

   Repr repr;
   if ((repr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "Cons_S_Instruction::genLocCode: Destination representation unknown");
      return;
   }
   Bool needsEnv = dst.needsEnv(TRUE);
   Loc::Kind kind = dst.locKind(repr, code);
   
   Instruction* nextIns = _next;
   switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:

//--- Common with Loc::REGISTER_LOC
      case Loc::REGISTER_LOC:
      case Loc::CELL_COMP_LOC:
      case Loc::IND_LOC:
         assignDst(src, srcRepr, repr, dstVar, NULL, code);
         if (needsEnv)
            assignDst(src, Repr::REPR_ENV_PTR, Repr::REPR_ENV_PTR, dstVar, this, code);
         break;

      case Loc::STACK_LOC:
         if (needsEnv)
            assignDst(src, Repr::REPR_ENV_PTR, Repr::REPR_ENV_PTR, dstVar, this, code);
         assignDst(src, srcRepr, repr, dstVar, NULL, code);
         break;

      default:
         error("Cons_S_Instruction::genLocCode: unexpected");
         break;
   }

    code.insertMove(*src.genLocCode(srcRepr, code),
      code.compLocOperand(srcRepr,
      ((LocOperand&)*_dst).loc(), *repr.structRepr(), 2),
      nextIns);
 
   code.insertMove(*_tag->genLocCode(Repr::REPR_TAG, code),
      code.compLocOperand(Repr::REPR_TAG,
      ((LocOperand&)*_dst).loc(), *repr.structRepr(), 1),
      nextIns);

   src.setNextUse(code);
   dst.setNextUse(code);

   code.removeInstruction(this);

// Skip over any inserted moves

   code._currentIns = nextIns->_prev;

#ifdef TRACE
   if (traceFlag) {
      outStream << "Cons_S_Instruction::genLocCode: Destination ";
      dstVar.printLocs(outStream, code.env());
   }
#endif

   code.recoverStack();
}

// Extract environment from source

Operand* EnvOfInstruction::genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code) {
   return src.genLocCode(Repr::REPR_ENV_PTR, code);
}

Operand* EnvOfInstruction::genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code) {
   return &src;
}


// Extract tag for sum type value either directly (if all atomic) or first component of pair

Operand* TagOfInstruction::genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code) {
   return repr == Repr::REPR_TAG
      ? src.genLocCode(repr, code)
    : &code.compLocOperand(Repr::REPR_TAG, *src.loc(repr, code), *repr.structRepr(), 1);
}

Void DeconsInstruction::assignDst(Operand& src, Repr srcRepr,
   Repr repr, Var& dstVar,
   Instruction* beforeIns, ICode& code) {
   Bool reused;

   Loc* loc = NULL;
   src.setReadyToKill(beforeIns != NULL);
   if (beforeIns == NULL) {
      loc = code.allocLoc(repr, dstVar, reused); 
      _dst = &code.locOperand(*loc);       
      _src = src.genLocCode(srcRepr, code);
   } else {
      loc = code.allocLoc(repr, dstVar, reused);
      Loc* srcLoc = src.loc(Repr::REPR_ENV_PTR, code);
      if (!(srcLoc != NULL && Loc::matchBetween(*srcLoc, *loc))) {
         Operand* srcOperand = src.genLocCode(srcRepr, code);
         if (srcOperand != NULL)
            code.insertMove(*srcOperand, code.locOperand(*loc), beforeIns);
      }
   }

   addVarLoc(dstVar, *loc, code.msa());
}

Void DeconsInstruction::genLocCode(ICode& code) {
   if (!_byPtr || TRUE)
      MoveInstruction::genLocCode(code);
   else {
      Operand& src = *_src;
      Repr srcRepr = src.repr(code);
      src.setKillPending();

      Operand& dst = *_dst;
      Var& dstVar = ((VarOperand*)_dst)->var();

      Repr repr;
      if ((repr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
         assert(FALSE, "DeconsInstruction::genLocCode: Destination representation unknown");
         return;
      }
      Bool needsEnv = dst.needsEnv(TRUE);
      Loc::Kind kind = dst.locKind(repr, code);

      Bool suppressed = FALSE;
      Instruction* nextIns = _next;
      switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:

         //--- Common with Loc::REGISTER_LOC

      case Loc::REGISTER_LOC:
         assignDst(src, srcRepr, repr, dstVar, NULL, code);
         if (needsEnv)
            assignDst(src, Repr::REPR_ENV_PTR, Repr::REPR_ENV_PTR, dstVar,
               this, code);
         break;

      case Loc::STACK_LOC:
         if (needsEnv)
            assignDst(src, Repr::REPR_ENV_PTR, Repr::REPR_ENV_PTR, dstVar,
               this, code);
         assignDst(src, srcRepr, repr, dstVar, NULL, code);
         break;

      default:
         error("DeconsInstruction::genLocCode: unexpected");
         break;
      }
      if (suppressed)
         code.removeInstruction(this);

      src.setNextUse(code);
      dst.setNextUse(code);

//--- Skip over any inserted moves

      code._currentIns = nextIns->_prev;

#ifdef TRACE
      if (traceFlag) {
         outStream << "DeconsInstruction::genLocCode: Destination ";
         dstVar.printLocs(outStream, code.env());
      }
#endif

      code.recoverStack();
   }
}


Operand* Decons_S_Instruction::genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code) {
   Repr sndRepr = repr.structRepr()->reprV()[1].unionRepr()->reprV()[_tag];
   Loc* loc = src.loc(repr, code);
   if (loc != NULL)
      return &code.compLocOperand(sndRepr, *loc, *repr.structRepr(), 2);
   else {
      loc = src.loc(sndRepr, code);
      return &code.locOperand(*loc);
   }
}

// TagInstruction: Assign source environment if needed

Operand* TagInstruction::genSrcLocCode(Operand& src, Repr repr, Var* srcVar, ICode& code) {
   _srcRepr = repr;
   return src.genLocCode(repr, code);
}

Operand* TagInstruction::genSrcLocEnvCode(Operand& src, Var* srcVar, ICode& code) {
   return &code.locOperand(code.reg(REG_consEnv));
}

// ToCellInstruction: allocate destination location

// n.b. Assumes only used to prepare for entry to closure

Loc& ToCellInstruction::allocLoc(Repr repr, Var& dstVar, Loc::Kind kind,
                                 Bool& reused, ICode& code) const {
   UInt regId = repr != Repr::REPR_ENV_PTR
      ? REG_cell
    : REG_cellEnv;

   code.flushReg(regId);
   return code.reg(regId);
}

Void MonadicOpActionInstruction::genLocCode(ICode& code) {
   Operand& x = *_x;
   Repr repr = x.repr(code);

   _x = x.genLocCode(repr, code);

   if ((_op.kind() != Operator::ERROR_OP &&
        _op.kind() != Operator::TRACE_OP) &&
       x.needsEnv(TRUE))
      _x_env = x.genLocEnvCode(code);

    x.setNextUse(code);
   code.recoverStack();
}


Void DyadicOpActionInstruction::genLocCode(ICode& code) {
   Operand& x = *_x;

#if (SERIALISATION==1)
      if (_op.kind() == Operator::INSERT_OP) {
      code.flushNonStack();
      code.adjustStackPointer(this);
   }
#endif

   _x = _x->genLocCode(Repr::REPR_UNKNOWN, code);
   if (x.needsEnv(FALSE))
      _x_env = x.genLocEnvCode(code);
   x.setNextUse(code);
   Operand& y = *_y;
   _y = _y->genLocCode(Repr::REPR_UNKNOWN, code);
   if (y.needsEnv(FALSE))
      _y_env = y.genLocEnvCode(code);
   y.setNextUse(code);
   code.recoverStack();
}

Void TriadicOpActionInstruction::genLocCode(ICode& code) {
   Operand& x = *_x;
   _x = _x->genLocCode(Repr::REPR_UNKNOWN, code);
   x.setNextUse(code);
   Operand& y = *_y;
   _y = _y->genLocCode(Repr::REPR_UNKNOWN, code);
   y.setNextUse(code);
   Operand& z = *_z;
   _z = _z->genLocCode(Repr::REPR_UNKNOWN, code);
   z.setNextUse(code);
   code.recoverStack();
}

Void QuinadicOpActionInstruction::genLocCode(ICode& code) {
   if (_v->kind() == Operand::VAR_OPERAND) {
      VarOperand* v = (VarOperand*)_v;
      _v = v->genLocCode(Repr::REPR_UNKNOWN, code);
      v->setNextUse(code);
   }
   if (_w->kind() == Operand::VAR_OPERAND) {
      VarOperand* w = (VarOperand*)_w;
      _w = w->genLocCode(Repr::REPR_UNKNOWN, code);
      w->setNextUse(code);
   }
   if (_x->kind() == Operand::VAR_OPERAND) {
      VarOperand* x = (VarOperand*)_x;
      _x = x->genLocCode(Repr::REPR_UNKNOWN, code);
      x->setNextUse(code);
   }
   if (_y->kind() == Operand::VAR_OPERAND) {
      VarOperand* y = (VarOperand*)_y;
      _y = y->genLocCode(Repr::REPR_UNKNOWN, code);
      y->setNextUse(code);
   }
   if (_z->kind() == Operand::VAR_OPERAND) {
      VarOperand* z = (VarOperand*)_z;
      _z = z->genLocCode(Repr::REPR_UNKNOWN, code);
      z->setNextUse(code);
   }
   code.recoverStack();
}

// StructOperand::checkUsage

// Before assigning a structure component, ensure that any
// live source select operand assigned to the same location
// is moved to a temporary location.

Void StructOperand::checkUsage(Int upToIndex, Repr repr,
                               Loc& dstLoc, StructRepr& dstStructRepr,
                               UInt dstIndex,
                               Instruction* beforeIns, ICode& code) {
   for (Int j = 0; j < upToIndex; j++) {
      if (_comps[j]->kind() == Operand::SELECT_OPERAND) {
         SelectOperand& selOperand = (SelectOperand&)*_comps[j];
         Operand& srcStructOperand = selOperand.operand();
         StructRepr& srcStructRepr = *srcStructOperand.repr(code).structRepr();
         Loc* srcStructLoc = srcStructOperand.loc(srcStructOperand.repr(code), code);
         if (Loc::matchBetween(*srcStructLoc, dstLoc)) {
            size_t srcOffset = srcStructRepr.offset(selOperand.index() - 1);
            size_t srcSize = srcStructRepr.size(selOperand.index() - 1);
            size_t dstOffset = dstStructRepr.offset(dstIndex - 1);
            size_t dstSize = dstStructRepr.size(dstIndex - 1);
            if (srcOffset < dstOffset + dstSize &&
               dstOffset < srcOffset + srcSize) {
               Repr selRepr = selOperand.repr(code);
               StackLoc& tempLoc = code.allocStackLoc(selRepr);
               code.insertMove(code.locOperand(*selOperand.loc(selRepr, code)),
                                 code.locOperand(tempLoc), beforeIns);
               _comps[j] = &code.locOperand(tempLoc);
            }
         }
      }
   }
}

// Assign structure components for given destination variable

Void StructOperand::assignComps(Repr repr, Loc& loc, Instruction* beforeIns, ICode& code) {
   for (Int i = _nComps - 1; i >= 0; i--) {
      Operand& operand = *_comps[i];
      Repr compRepr = operand.repr(code);
      if (_comps[i]->kind() == Operand::STRUCT_OPERAND) {
         CompLoc& compLoc = *code.compLoc(compRepr, loc, _structTemplate.repr(), i + 1);
         ((StructOperand&)operand).assignComps(compRepr, compLoc, beforeIns, code);
      } else {

#ifdef ALLOW_STRUCT_OVERLAY
         operand.setReadyToKill(FALSE);
#endif

         checkUsage(i, repr, loc, _structTemplate.repr(), i + 1, beforeIns, code);

         _comps[i] = operand.genLocCode(compRepr, code);
         code.insertMove(*_comps[i],
                         code.compLocOperand(compRepr, loc, _structTemplate.repr(), i + 1), beforeIns);
         operand.setReadyToKill(FALSE);
         operand.setNextUse(code);
      }
   }
}

// SelectOperand

Repr SelectOperand::repr(ICode& code) {
   if (_reprInfo._repr == Repr::REPR_UNKNOWN)
      _reprInfo = code.typeReprInfo(_typeSig);

   return _reprInfo._repr;
}

Repr SelectOperand::repr(Void) {
   return _reprInfo._repr;
}

Bool SelectOperand::needsEnv(Bool dst) {
   return _reprInfo._needsEnv;
}

Loc::Kind SelectOperand::locKind(Repr repr, ICode& code) {
   return Loc::COMP_LOC;
}

Loc* SelectOperand::loc(Repr repr, ICode& code) {
   Repr operandRepr = _operand->repr(code);
   if (!_byPtr) {
      StructRepr& structRepr = *operandRepr.structRepr();
      return code.compLoc(repr, *_operand->loc(operandRepr, code), structRepr, _index);
   } else {
      StructRepr& structRepr = *operandRepr.ptrRepr()->repr().structRepr();
      Loc& loc = *_operand->loc(operandRepr, code);
      return code.compLoc(structRepr.reprV()[_index - 1],
                          *code.indLoc(Repr(&structRepr), loc), structRepr, _index);
   }
}

Loc* SelectOperand::locEnv(ICode& code) {
   return repr() != Repr::REPR_ENV_PTR
      ? _operand->locEnv(code)
      : loc(Repr::REPR_ENV_PTR, code);
}

Operand* SelectOperand::genLocCode(Repr repr, ICode& code) {
   return &code.locOperand(*loc(repr, code));
}

Operand* SelectOperand::genLocEnvCode(ICode& code) {
   return &code.locOperand(*locEnv(code));
}

Void StructInstruction::assignDst(Repr repr, Operand& dst, Instruction* beforeIns, ICode& code) {
   if (dst.kind() == Operand::VAR_OPERAND) {
      Var& var = ((VarOperand&)dst).var();

      Bool reused;
      Loc* loc = code.allocLoc(repr, var, reused);
      if (beforeIns == NULL)
         _dst = &code.locOperand(*loc);
      else
         code.insertMove(code.locOperand(code.reg(REG_consEnv)),
            code.locOperand(*loc), beforeIns);
      addVarLoc(var, *loc, code.msa());
   } else
      _dst = _dst->genLocCode(repr, code);
}

Void StructInstruction::genLocCode(ICode& code) {
   Operand& dst = *_dst;

#ifdef ALLOW_STRUCT_OVERLAY
   _src.setKillPending();
   _src.setReadyToKill(TRUE);  // Assumes src environment already mapped
#endif

/*

// T.B.D. Could introduce dynamic check here to ensure that
// all source components are already mapped to the destination environment
// For now - make the assumption that mapping is needed

*/
   Repr repr;
   if ((repr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "StructInstruction::genLocCode: Destination representation unknown");
      return;
   }
   Bool needsEnv = dst.needsEnv(TRUE);
   Loc::Kind kind = dst.locKind(repr, code);

   Instruction* nextIns = _next;
   switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:
      case Loc::REGISTER_LOC:
      case Loc::STATIC_LOC:
      case Loc::CELL_COMP_LOC:
      case Loc::IND_LOC:
         assignDst(repr, dst, NULL, code);
         if (needsEnv)
            assignDst(Repr::REPR_ENV_PTR, dst, nextIns, code);
         _src.assignComps(repr, ((LocOperand&)*_dst).loc(), nextIns, code);
         break;

      case Loc::STACK_LOC:
         if (needsEnv)
            assignDst(Repr::REPR_ENV_PTR, dst, nextIns, code);
         assignDst(repr, dst, NULL, code);
         _src.assignComps(repr, ((LocOperand&)*_dst).loc(), nextIns, code);
         break;

      default:
         error("StructInstruction::genLocCode: unexpected");
         break;
   }

   code.removeInstruction(this);

// Skip over any inserted moves

   code._currentIns = nextIns->_prev;

   dst.setNextUse(code);

   code.recoverStack();
}


Void DyadicOpInstruction::insertEnvMove(Operand& src, Operand& dst, Instruction* beforeIns,
                                        ICode& code) const {
   code.insertMove(src, dst, beforeIns);
}

Void DyadicOpInstruction::assignLoc(Operand& x, Repr xRepr,
                                    Operand& y, Repr yRepr,
                                    Operand& dst, Repr repr, Loc::Kind kind, Var* dstVar,
                                    Bool& needsEnv, Bool& suppressed,
                                    Instruction* beforeIns,
                                    ICode& code) {
   Bool reused;
   Bool opNeedsOperandEnv = _op.type_() == Operator::OP_TYPE_NAME ||
                            _op.type_() == Operator::OP_TYPE_TYPE;

// At present, only map operations require an environment for the destination
// If different semantics are needed in future, then a virtual function could
// be added to determine it

   if (beforeIns == NULL) {
      x.setReadyToKill(FALSE);
      y.setReadyToKill(FALSE);
   } else {
      if (_op.kind() != Operator::MAP_OP)
         return;
   }

   Loc& loc = *dst.allocLoc(beforeIns == NULL ? repr : Repr::REPR_ENV_PTR, kind, reused, code);

   if (beforeIns == NULL) {  
      _x = x.genLocCode(xRepr, code);
      if (_op.kind() == Operator::MAP_OP ||
          opNeedsOperandEnv && x.needsEnv(FALSE))
         _x_env = x.genLocEnvCode(code);
      _y = y.genLocCode(yRepr, code);
      if (opNeedsOperandEnv && y.needsEnv(FALSE))
         _y_env = y.genLocEnvCode(code);
      _dst = &code.locOperand(loc);
   } else
      insertEnvMove(*y.genLocEnvCode(code),
         code.locOperand(loc), beforeIns, code);
 
   if (dstVar != NULL)
      addVarLoc(*dstVar, loc, code.msa());
}

#if (GARBAGE_COLLECTION==1)

Bool DyadicOpInstruction::locCodeNeedsStableStack(Void) const {
#if (SERIALISATION==1)
   return _op.kind() == Operator::EXTRACT_OP;
#else
   return FALSE;
#endif
}

FrameDescrInstruction* DyadicOpInstruction::frameDescrInstruction(Void) const {
   return NULL;
}

Void DyadicOpInstruction::genFrameDescrLocCode(FrameDescrInstruction&, ICode&) {
}

Void DyadicOpInstruction::genLocCode(ICode& code) {
   Operand& x = *_x;
   Operand& y = *_y;
   
   x.setKillPending();
   y.setKillPending();

   Operand& dst = *_dst;
   Var* dstVar = _dst->kind() == Operand::VAR_OPERAND
      ? &((VarOperand*)_dst)->var()
      : NULL;

   Repr dstRepr;
   if ((dstRepr = dst.repr(code)) == Repr::REPR_UNKNOWN) {
      assert(FALSE, "DyadicOpInstruction::genLocCode: Destination representation unknown");
      return;
   }
   Bool needsEnv = dst.needsEnv(TRUE);

#if (GARBAGE_COLLECTION==1)
   FrameDescrInstruction* frameDescrIns = frameDescrInstruction();
#endif

   Loc::Kind kind = dst.locKind(dstRepr, code);
   Instruction* nextIns = _next;
   Bool suppressed = FALSE;
   Repr xRepr = x.repr(code);
   Repr yRepr = y.repr(code);

   if (xRepr == Repr::REPR_UNKNOWN || yRepr == Repr::REPR_UNKNOWN) {
      assert(FALSE, "DyadicOpInstruction::genLocCode: Source representation unknown");
      return;
   }

   if (locCodeNeedsStableStack()) {
      code.flushNonStack();
      code.adjustStackPointer(this);
   }
   switch (kind) {
      case Loc::NATIVE_LOCAL_LOC:
      case Loc::CELL_COMP_LOC:
      case Loc::REGISTER_LOC:
      case Loc::STATIC_LOC:
      case Loc::IND_LOC:
         assignLoc(x, xRepr, y, yRepr,
            dst, dstRepr, kind, dstVar, needsEnv, suppressed, NULL, code);
         if (needsEnv)
            assignLoc(x, xRepr, y, yRepr,
               dst, dstRepr, kind, dstVar, needsEnv, suppressed, nextIns, code);
         else
            if (_op.kind() == Operator::MAP_OP)
               _x_env = x.genLocEnvCode(code);
         break;

      case Loc::COMP_LOC:
      case Loc::STACK_LOC:
         if (needsEnv) {
            assignLoc(x, Repr::REPR_ENV_PTR, y, Repr::REPR_ENV_PTR,
               dst, Repr::REPR_ENV_PTR, kind, dstVar, needsEnv, suppressed, this, code);
            x.setReadyToKill(TRUE);
            y.setReadyToKill(TRUE);
         }
         assignLoc(x, xRepr, y, yRepr,
            dst, dstRepr, kind, dstVar, needsEnv, suppressed, NULL, code);
         break;

      default:
         error("DyadicOpInstruction::genLocCode: unexpected");
         break;
   }

#if (GARBAGE_COLLECTION==1)
   if (frameDescrIns != NULL)
      genFrameDescrLocCode(*frameDescrIns, code);
#endif

   if (suppressed)
      code.removeInstruction(this);

   x.setNextUse(code);
   y.setNextUse(code);
   dst.setNextUse(code);

   code.recoverStack();
}


Bool MapGCInstruction::locCodeNeedsStableStack(Void) const {
   return TRUE;
}

FrameDescrInstruction* MapGCInstruction::frameDescrInstruction(Void) const {
   return _prev != NULL && _prev->kind() == Instruction::FRAME_DESCR_INSTR
      ? (FrameDescrInstruction*)_prev
      : NULL;
}


Void MapGCInstruction::insertEnvMove(Operand& src, Operand& dst, Instruction* beforeIns,
   ICode& code) const {
   if (this == beforeIns)
      DyadicOpInstruction::insertEnvMove(src, dst, beforeIns, code);
   else {
      code.insertMove(src, dst, beforeIns->next());
      code.setCurrentIns(beforeIns->next());
   }
}

Void MapGCInstruction::genFrameDescrLocCode(FrameDescrInstruction& frameDescrIns, ICode& code) {
   Loc* dstLoc = _dst->kind() == Operand::LOC_OPERAND
      ? &((LocOperand*)_dst)->loc()
      : NULL;
   if (dstLoc != NULL)
      dstLoc->setReserved(TRUE);
   code.adjustStackPointer(this);
   code.genLiveVarsGC_Code(FALSE, code.basicBlk(), code.sd_(), frameDescrIns);
   if (dstLoc != NULL)
      dstLoc->setReserved(FALSE);
}

#endif

Void EmptyArgsInstruction::genLocCode(ICode& code) {
   if (!code._argFlag) {
      code._argFlag = TRUE;
      code._sqzPending = code.squeezeNeeded(code._stackLocs);
      code._argBase_d = code._arg_d =
         code._sqzPending
         ? code._sd
       : code.liveStackDepth(code._stackLocs);

      _diff = code._sp - code._argBase_d;

      code._altEntry = !_needsArgBase;
   }

   if (!_needsArgBase)
      code.removeInstruction(this);
}



ICode::ICode(ConstString options, Env& env, MSA& msa)
 : Code(options, env, msa),
   _reprList(NULL),
   _rackPos(0),
   _sd(0),
   _sp(0),
   _argFlag(FALSE),
   _argBase_d(0),
   _arg_d(0),
   _sqzPending(FALSE),
   _altEntry(FALSE),
   _currentIns(NULL),
   _stackLocs(NULL)
{
   for (UInt reg = REG_consMSA; reg < N_REGS; reg++)
      _register[reg] = new(_msa) Register(reg);
   _codeEnv = new(_msa) SpecialLoc(Repr::REPR_ENV_PTR);
   _cellInfo = new(_msa) SpecialLoc(Repr::REPR_CELL_INFO_PTR);
}

// T.B.D.

Loc& ICode::assignLoc(Repr repr, Var& dstVar, Operand*& dst) {
   Bool reused;
   Loc& dstLoc = *allocLoc(repr, dstVar, reused);
   dst = &locOperand(dstLoc, dstVar.reserved());
   addVarLoc(dstVar, dstLoc, msa());

#ifdef TRACE
   if (traceFlag) {
      outStream << "assignLoc: Destination ";
      dstVar.printLocs(outStream, env());
   }
#endif

   return dstLoc;
}

// Set variable not live

Void ICode::notLive(Var& var) {
   LocElement* locElement;
   while ((locElement = var._locs._locElements) != NULL) {
      Loc& loc = locElement->_loc;

#ifdef TRACE
      if (traceFlag) {
         outStream << "Removing variable ";
         printName(var._name, outStream, nameTable());
         outStream << " from location ";
         loc.print(outStream, env());
         outStream << '\n';
      }
#endif

      remVarLoc(var, loc, msa());
   }
}

// Insert an instruction before current

Void ICode::insertInstruction(Instruction* ins) {

#ifdef TRACE
   if (_currentIns->_seqNo == -1)
      toggle(traceFlag);
   if (traceFlag)
      outStream << "Inserting instruction before " << _currentIns->_seqNo << '\n';
#endif

   Code::insertInstruction(ins, _currentIns);
}

// Insert a move instruction

Void ICode::insertMove(Operand& src, Operand& dst, Instruction* beforeIns/* = NULL*/) {
   if (beforeIns == NULL)
      beforeIns = _currentIns;

#ifdef TRACE
   if (_currentIns->_seqNo == -1)
      toggle(traceFlag);
   if (traceFlag)
      outStream << "Inserting move before " << beforeIns->_seqNo << '\n';
#endif

   Code::insertInstruction(new (msa()) MoveInstruction(src, dst), beforeIns);
}

Void ICode::insertMove(Repr repr, const Var* src, Loc& dst) {
   insertMove(locOperand(*src->loc(repr, *this)),
      locOperand(dst));
}

// Append an instruction after current

Void ICode::appendInstruction(Instruction* ins) {

#ifdef TRACE
   if (_currentIns->_seqNo == -1)
      toggle(traceFlag);
   if (traceFlag)
      outStream << "Appending instruction after " << _currentIns->_seqNo << '\n';
#endif

   Code::appendInstruction(ins, _currentIns);
   _currentIns = ins;
}

// Append a move instruction

Void ICode::appendMove(Operand& src, Operand& dst) {
   appendInstruction(new (msa()) MoveInstruction(src, dst));
}

declareTypeCon(FileHandle);

Void ICode::setStructRepr(StructTemplate& structTemplate) {
   UInt nComps = structTemplate.nSlots();
   if (nComps > 0) {
      Repr* reprV = (Repr*)msa().alloc(nComps * sizeof(Repr));
      for (UInt i = 0; i < nComps; i++)
         reprV[i] = typeReprInfo(typeSignature(structTemplate.slotTypes()[i],
            typeTable()))._repr;
      structTemplate.setRepr(addStructRepr(nComps, reprV));
   }
}

// Bool ICode::squeezeNeeded

// The purpose of this is to assess if 'dead' variables should be squeezed from a stack
// before function application.  However, squeezing is currently not employed, but it could be useful
// in cases where its cost would be less than the cost of temporary variable relocation.

Bool ICode::squeezeNeeded(const StackLoc* stackLoc) {
   return FALSE;
}

/*
Bool ICode::squeezeNeeded(const StackLoc* stackLoc) {
   Bool headInUse = FALSE;
   while (stackLoc != NULL) {
      Bool isOutput = FALSE;
      if (stackLoc->inUse()) {
         VarElement* varElement = stackLoc->_vars.head();
         while (varElement != NULL) {
            if (varElement->_var._expCount > 0) {
               isOutput = TRUE;
               break;
            }
            varElement = varElement->_next;
         }
      }
      if (isOutput)
         headInUse = TRUE;
      else
         if (headInUse)
            return TRUE;
      stackLoc = (StackLoc*)stackLoc->_next;
   }
   return FALSE;
}
*/
UInt ICode::liveStackDepth(const StackLoc* stackLoc) {
   UInt d = 0;

   while (stackLoc != NULL) {
      if (stackLoc->inUse()) {
         VarElement* varElement = stackLoc->_vars.head();
         while (varElement != NULL) {
            if (varElement->_var._lastNextUse)
               return stackLoc->_offset + stackLoc->_size;
            varElement = varElement->_next;
         }
      }
      stackLoc = (StackLoc*)stackLoc->_next;
   }
   return 0;
}

UInt ICode::inputStackDepth(const StackLoc* stackLoc, const VarSet& vars) {
   UInt d = 0;

   while (stackLoc != NULL) {
      if (stackLoc->inUse()) {
         VarElement* varElement = stackLoc->_vars.head();
         while (varElement != NULL) {
            VarElement* succVarElement = vars.findElement(varElement->_var);
            if (succVarElement != NULL &&
               succVarElement->testFlags(VarElement::I) &&
               !succVarElement->testFlags(VarElement::D))
               return stackLoc->_offset + stackLoc->_size;
            varElement = varElement->_next;
         }
      }
      stackLoc = (StackLoc*)stackLoc->_next;
   }
   return 0;
}

Void ICode::genLocCode(Void) {

#ifdef TRACE
   OptionsParser options(_options);
   if (options.hasKey("iCode")) {
      traceFlag = options.hasKey("trace");
      options.getNum("probe", instructionProbeSeqNo);
   } else
      traceFlag = FALSE;
#endif 

   StructTemplate* structTemplate = _structTemplates;
   while (structTemplate != NULL) {
      setStructRepr(*structTemplate);
      structTemplate = structTemplate->_next;
   }

   UInt nDeferred;
   do {
      nDeferred = 0;
      BasicBlk* basicBlk = _basicBlks;
      while (basicBlk != NULL) {
           Bool deferred = FALSE;
         if (!basicBlk->_locCodeGenerated) {
            if (!(((LabelInstruction*)basicBlk->_firstIns)->codeLabel().isHead() ||
               basicBlk->_saveFlag) || basicBlk->_defer) {

#ifdef TRACE
               if (traceFlag)
                  outStream << basicBlk->_firstIns->_seqNo << " already deferred" << endl;
#endif

               deferred = TRUE;
         } else {
               VarElement* varElement = basicBlk->_vars.head();
               while (varElement != NULL) {
                  if (varElement->_var.kind() != Var::GLOBAL_VAR &&
                     varElement->_var.kind() != Var::EXT_GLOBAL_VAR &&
                     varElement->_locs.isEmpty() &&
                     varElement->testFlags(VarElement::I) &&
                     !varElement->testFlags(VarElement::D)) {

#ifdef TRACE
                     if (traceFlag) {
                        printTranName(varElement->_var._name, outStream, env());
                        outStream << '\n';
                        outStream.flush();
                     }
#endif

                     deferred = TRUE;
                     break;
                  }
                  varElement = varElement->_next;
               }
            }

            /*
            if (basicBlk->_lambda != NULL &&
            basicBlk != basicBlk->_headBasicBlk) {
            for (UInt i = 0; i < basicBlk->_lambda->_nFormalParams; i++)
            if (toBody(basicBlk->_lambda->_formalParamV[i]->val(), Var).hasReservedLocs()) {
            //                if (!toBody(basicBlk->_lambda->_formalParamV[i]->val(), Var).hasReservedLocs()) { T.B.D. address this
            deferred = TRUE;
            goto next;
            }
            }
            */
            if (!deferred) {
               basicBlk->genLocCode(*this);
               basicBlk->_locCodeGenerated = TRUE;
            } else {
               nDeferred++;

#ifdef TRACE
               if (deferred && traceFlag)
                  outStream << "Deferring " << basicBlk->_firstIns->_seqNo << endl;
#endif

            }
         }
         basicBlk = basicBlk->_next;
      }
   } while (nDeferred > 0);
}

// ICode::transferLoc: Transfer a location to a basic block

Loc* ICode::transferLoc(Loc& loc, BasicBlk& basicBlk) {
   Loc* dstLoc = basicBlk.matchLoc(loc);

   if (dstLoc == NULL) {

      //--- If no match, check any child locations match

      Loc* childLoc = loc._firstChild;
      while (childLoc != NULL) {
         Loc* parent = childLoc->parent();
         dstLoc = basicBlk.matchLoc(*parent);
         if (dstLoc != NULL)
            break;
         childLoc = childLoc->_next;
      }
   }

   if (dstLoc == NULL) {
      Loc* parent = loc.parent();
      Loc* cloneParent = NULL;
      if (parent != NULL)
         cloneParent = transferLoc(*parent, basicBlk);
      dstLoc = loc.clone(cloneParent, FALSE, msa());
   }
   return dstLoc;
}

// ICode::saveVarLocs: Save a set of variable locations

Void ICode::saveVarLocs(BasicBlk& basicBlk, Bool flushNonLocal) {

  if (flushNonLocal) {

//--- Save all register variables

//--- Note that if a location is saved, then the loop is exited because
//--- it applies to all other variables sharing the location

      for (UInt r = REG_cell; r <= REG_rRef; r++) {
         Loc* loc = _register[r];

//------ rStruct is treated as a special case
//------ assuming a single indirect first child or NULL

         if (r == REG_rStruct)
            loc = loc->_firstChild;

         if (loc != NULL) {
            VarElement* varElement = loc->_vars.head();
            while (varElement != NULL) {
               Var& var = varElement->var();
               if (basicBlk.varIsInput(var) &&
                  var.bestLocOrNULL(loc->repr(), false) == NULL) {
                  if (loc->reserved()) {
                     Loc& stackLoc = allocStackLoc(loc->repr());
                     moveLocVars(*loc, stackLoc);
                  } else
                     saveLoc(*loc);
                  break;
               }
               varElement = varElement->next();
            }
         }
      }
   }

   VarElement* varElement = basicBlk._vars.head();

#ifdef TRACE
   Int count = 0;
#endif

   while (varElement != NULL) {
      if (varElement->testFlags(VarElement::I) &&
         !varElement->testFlags(VarElement::D)) {
         Var& var = varElement->var();

#ifdef TRACE
         if (traceFlag) {
            if (count++ > 0)
               outStream << ' ';
            printTranName(var._name, outStream, env());
            outStream << ':';
         }
#endif

         LocElement* locElement = var._locs.head();
         while (locElement != NULL) {
            LocElement* nextLocElement = locElement->_next;
            Loc& loc = locElement->loc();
            if (loc.registerBased() && flushNonLocal) {
               Register* reg = loc.reg();
               if (loc.kind() == Loc::IND_LOC &&
                  reg != NULL && reg->id() == REG_rStruct) {
                  if (loc.reserved()) {
                     Loc& stackLoc = allocStackLoc(loc.repr());
                     moveLocVars(loc, stackLoc);
                  } else
                     saveLoc(loc);

                  Loc* savedLoc = var.bestLocOrNULL(loc.repr(), false);
                  if (savedLoc != NULL)
                     varElement->_locs.addElement(*transferLoc(*savedLoc, basicBlk), msa());
               }
            } else
               varElement->_locs.addElement(*transferLoc(loc, basicBlk), msa());
            locElement = nextLocElement;
         }

         var.checkLive(*this);

#ifdef TRACE
         if (traceFlag)
            varElement->_locs.print(outStream, env());
#endif

      }
      varElement = varElement->_next;
   }

#ifdef TRACE
   if (traceFlag && count++ > 0) {
      outStream << '\n';
      outStream.flush();
   }
#endif

}

// Default to suppressing environment pointer move if applicable

Bool ICode::suppressEnvMove(Void) const {
   return TRUE;
}

Void ICode::adjustStackPointer(Instruction* ins, Bool recoverFlag/* = FALSE*/) {
   if (recoverFlag)
      recoverStack(TRUE, TRUE);

   Int diff = (Int)_sd - (Int)_sp;
   _sp = _sd;
   if (diff != 0)
      Code::insertInstruction(new(msa()) StackAdjustInstruction(diff), ins);
}

Void ICode::adjustStackPointer(BasicBlk& basicBlk, Instruction* ins) {
   Int diff = (Int)basicBlk.sp_() - (Int)_sp;
   _sp = basicBlk.sp_();
   if (diff != 0)
      Code::insertInstruction(new(msa()) StackAdjustInstruction(diff), ins);
}


// ICode::allocRegisters: Allocate registers for a result variable

Void ICode::allocRegisters(Var& var) {
   Repr repr = var.repr(*this);
   if (repr.tag() != Repr::REPR_STRUCT &&
       repr.tag() != Repr::REPR_FILE_HANDLE) {
      Register& reg = *selectReg(repr);
      reg._repr = repr;
      reg._readyToKill = FALSE;
      addVarLoc(var, reg, _msa);
   } else {
      Register& reg = *_register[REG_rStruct];
      reg._repr = Repr(new(msa()) PtrRepr(repr));
      reg._readyToKill = FALSE;
      addVarLoc(var, *indLoc(repr, reg), _msa);
   }

   if (var.needsEnv()) {
      Register& reg = *_register[REG_rEnv];
      reg._repr = Repr::REPR_ENV_PTR;
      reg._readyToKill = FALSE;
      addVarLoc(var, reg, _msa);
   }
}

Register* ICode::selectReg(Repr repr) {
   switch (repr.tag()) {
      case Repr::REPR_ENV_PTR:      return _register[REG_rEnv];
      case Repr::REPR_NAME:         return _register[REG_rName];
      case Repr::REPR_TYPE:         return _register[REG_rType];
      case Repr::REPR_CELL_PTR:     return _register[REG_rCell];
      case Repr::REPR_EXPR:         return _register[REG_rExpr];
      case Repr::REPR_BYTE:         return _register[REG_rByte];
      case Repr::REPR_BITS:         return _register[REG_rBits];
      case Repr::REPR_INT:          return _register[REG_rInt];
      case Repr::REPR_FLOAT:        return _register[REG_rFloat];
      case Repr::REPR_DOUBLE:       return _register[REG_rDouble];
      case Repr::REPR_TAG:          return _register[REG_rTag];
      case Repr::REPR_CHAR:         return _register[REG_rChar];
      case Repr::REPR_STRING:
      case Repr::REPR_DIR:
      case Repr::REPR_DIR_ENTRY:
      case Repr::REPR_SERIAL_CONTEXT:
      case Repr::REPR_PTR:          return _register[REG_rPtr];
      case Repr::REPR_FILE_HANDLE:
      case Repr::REPR_STRUCT:       return _register[REG_rStruct];
      case Repr::REPR_REF:          return _register[REG_rRef];
      case Repr::REPR_UTC:          return _register[REG_rUTC];

      default:
         break;
   }
   error("ICode::selectReg: unknown representation");
   return NULL;
}

Void ICode::printPtrRepr(const PtrRepr* ptrRepr, ostream& os,
                         const PtrReprInstance* ptrReprs/* = NULL*/) const {
   if (ptrRepr != NULL) {
      const PtrReprInstance* ptrReprInstPtr = ptrReprs;
      while (ptrReprInstPtr != NULL) {
         if (ptrRepr == ptrReprInstPtr->_ptrRepr) {
            os << "^" << ptrReprInstPtr->_id;
            return;
         }
         ptrReprInstPtr = ptrReprInstPtr->_next;
      }
      PtrReprInstance ptrReprInst = { ptrReprs, ptrRepr, ptrReprs != NULL ? ptrReprs->_id + 1 : 1 };

      os << '^' << ptrReprInst._id << '(';
      printRepr(ptrRepr->repr(), os, &ptrReprInst);
      os << ')';
   } else
      os << "^_";
}

Void ICode::printStructRepr(const StructRepr& structRepr, ostream& os,
                            const PtrReprInstance* ptrReprs/* = NULL*/) const {
   os << "(";
   for (UInt i = 0; i < structRepr.nReprs(); i++) {
      if (i > 0)
         os << ",";
      printRepr(structRepr.reprV()[i], os, ptrReprs);
      os << '[' << structRepr.offsetV()[i] << '/' << structRepr.sizeV()[i] << ']';
   }
   os << ')';
}

Void ICode::printUnionRepr(const UnionRepr& unionRepr, ostream& os,
                           const PtrReprInstance* ptrReprs/* = NULL*/) const {
   os << "(";
   for (UInt i = 0; i < unionRepr.nReprs(); i++) {
      if (i > 0)
         os << " | ";
      printRepr(unionRepr.reprV()[i], os, ptrReprs);
   }
   os << ')';
}


Void ICode::printArrayRepr(const ArrayRepr& arrayRepr, ostream& os,
                           const PtrReprInstance* ptrReprs/* = NULL*/) const {
   os << "[";
   printRepr(arrayRepr.elementRepr(), os, ptrReprs);
   os << "]";
}

Void ICode::printRepr(Repr repr, ostream& os,
                      const PtrReprInstance* ptrReprs/* = NULL*/) const {
   switch (repr.tag()) {
      case Repr::REPR_UNKNOWN:
         os << "?";
         break;

      case Repr::REPR_VOID:
         os << "VOID";
         break;

      case Repr::REPR_NAME:
         os << "NAME";
         break;

      case Repr::REPR_TYPE:
         os << "TYPE";
         break;

      case Repr::REPR_CHAR:
         os << "CHAR";
         break;

      case Repr::REPR_BYTE:
         os << "BYTE";
         break;

      case Repr::REPR_INT:
         os << "INT";
         break;

      case Repr::REPR_DOUBLE:
         os << "DOUBLE";
         break;

      case Repr::REPR_FLOAT:
         os << "FLOAT";
         break;

      case Repr::REPR_STRING:
         os << "STRING";
         break;

      case Repr::REPR_REF:
         os << "REF";
         break;

      case Repr::REPR_TAG:
         os << "TAG";
         break;

      case Repr::REPR_ENV_PTR:
         os << "ENV_PTR";
         break;

      case Repr::REPR_LABEL:
         os << "LABEL";
         break;

      case Repr::REPR_LABEL_PAIR:
         os << "LABEL_PAIR";
         break;

      case Repr::REPR_EXPR:
         os << "EXPR";
         break;

      case Repr::REPR_CELL_INFO_PTR:
         os << "CELL_INFO";
         break;

      case Repr::REPR_CELL_PTR:
         os << "CELL_PTR";
         break;

      case Repr::REPR_UTC:
         os << "UTC";
         break;

      case Repr::REPR_FILE_HANDLE:
         os << "FILE_HANDLE";
         break;

      case Repr::REPR_DIR:
         os << "DIR";
         break;

      case Repr::REPR_SERIAL_CONTEXT:
         os << "SERIAL_CONTEXT";
         break;

      case Repr::REPR_PTR:
         printPtrRepr(repr.ptrRepr(), os, ptrReprs);
         break;

      case Repr::REPR_STRUCT: {
         printStructRepr(*repr.structRepr(), os, ptrReprs);
         break;
      }

      case Repr::REPR_UNION: {
         printUnionRepr(*repr.unionRepr(), os, ptrReprs);
         break;
      }

      case Repr::REPR_ARRAY: {
         printArrayRepr(*repr.arrayRepr(), os, ptrReprs);
         break;
      }

      default:
         assert(FALSE, "ICode::printRepr: Invalid representation");
         break;
   }
}


Void ICode::genTarget(ostream& os) {
   error("ICode::genTarget: Not supported");
}

// ICode::typeRepr: Return representation of a given type signature

ReprInfo ICode::typeReprInfo(TypeSig typeSig) {
   ReprInfo reprInfo = ::typeReprInfo(typeSig, typeTable(), nameTable());

#ifdef TRACE
   if (traceFlag) {
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << " represented as ";
      printRepr(reprInfo._repr, outStream);
      outStream << "\n";
   }
#endif

   ReprElement** reprElementPtr = &_reprList;
   while (*reprElementPtr != NULL) {
      //      ReprElement* reprElement = *reprElementPtr;
      //      if (reprElement->repr() == repr)
      //         return repr;
      reprElementPtr = &((*reprElementPtr)->next());
   }
   *reprElementPtr = new(_msa) ReprElement(NULL, Repr(reprInfo._repr));
   return reprInfo;
}

// ICode::addStructRepr: Add a unique structure representation

StructRepr* ICode::addStructRepr(UInt nReprs, Repr* reprV) {
   ReprElement** reprElementPtr = &_reprList;
   while (*reprElementPtr != NULL) {
      ReprElement* reprElement = *reprElementPtr;
      if (reprElement->repr().tag() == Repr::REPR_STRUCT &&
         reprElement->repr().structRepr()->matches(nReprs, reprV)) {
         msa().free(reprV);
         return (*reprElementPtr)->repr().structRepr();
      }
      reprElementPtr = &((*reprElementPtr)->next());
   }
   size_t* compSizeV = (size_t*)_msa.alloc(nReprs * sizeof(size_t));
   size_t* compOffsetV = (size_t*)_msa.alloc(nReprs * sizeof(size_t));
   StructRepr* structRepr = new(_msa) StructRepr(nReprs, reprV, compSizeV, compOffsetV);
   *reprElementPtr = new(_msa) ReprElement(NULL, Repr(structRepr));
   return structRepr;
}

// ICode::emptyRegisters: clear all register assignments

Void ICode::emptyRegisters(Void) {
   for (UInt reg = REG_sb; reg < N_REGS; reg++) {
      _register[reg]->_vars.empty(msa());
      _register[reg]->_firstChild = NULL;
      _register[reg]->_reserved = FALSE;
      _register[reg]->setReserved(FALSE);
   }
}

Register& ICode::allocReg(Repr repr) {
   Register* reg = selectReg(repr);
   assert(reg != NULL, "ICode::allocReg: No register for representation");
   if (reg != NULL && reg->inUse())
      flushReg(reg->_id);
   reg->_repr = repr;
   reg->_readyToKill = FALSE;
   return *reg;
}

// T.B.D.
// If the register is assigned to more than one variable
// Then only one new stack location or local variable may be needed.

Void ICode::flushReg(UInt r) {
   if (_register[r]->requiresFlush()) {
      flushLoc(*_register[r]);
      _register[r]->empty(msa());
   }
}

// Save all register variables

Void ICode::flushRegisters(Void) {
   for (UInt r = REG_cell; r <= REG_rRef; r++)
      flushReg(r);
}

Void ICode::flushNonStack(Void) {
   flushRegisters();
}

// Recover unused stack locations

Void ICode::recoverStackRange(UInt& from, UInt to,
                         StackLoc* stackLocs,
                         UInt& d, UInt& argBase_d, UInt& arg_d,
                         Bool emitCode) {
   UInt size = from - to;

   StackLoc* prevLoc = stackLocs;
   while (prevLoc != NULL && prevLoc->offset() >= from) {
      prevLoc->offset() -= size;
      prevLoc = (StackLoc*)prevLoc->next();
   }

   UInt n = d - from;
   if (emitCode && n > 0)
      Code::insertInstruction(new (msa()) SqueezeInstruction(from, to, n), _currentIns);

// Adjust stack and argument depths

   d -= size;
   if (_argFlag) {
      if (argBase_d >= from)
         argBase_d -= size;
      if (arg_d >= from)
         arg_d -= size;
   }
   from -= size;
}

Void ICode::recoverStack(Bool squeeze/* = FALSE */, Bool emitCode/* = FALSE */) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Recover stack, d = " << _sd << '\n';
      outStream << '\n';
      Loc::printLocs(_stackLocs, outStream, env());
      outStream << '\n';
      outStream.flush();
   }
#endif

   StackLoc** locPtr = &_stackLocs;
   UInt& d = _sd;
   UInt& argBase_d = _argBase_d;
   UInt& arg_d = _arg_d;
   UInt from = d;
   while ((*locPtr) != NULL) {
      StackLoc& loc = *(*locPtr);
      if (loc.inUse()) {
         if (!squeeze) {
            assert(loc._offset + loc._size == d, "Void ICode::recoverStack: unexpected");
            return;
         }
         UInt to = loc._offset + loc._size;
         if (from != to)
            recoverStackRange(from, to, _stackLocs,
                         d, argBase_d, arg_d,
                         emitCode);
         from = loc._offset;
         locPtr = (StackLoc**)&loc._next;
      } else {
         UInt to;
         StackLoc* nextLoc = (StackLoc*)loc._next;
         do {
            if (nextLoc == NULL) {
               to = 0;
               break;
            }
            to = nextLoc->_offset + nextLoc->_size;
            if (nextLoc->inUse())
               break;
            nextLoc = (StackLoc*)nextLoc->_next;
         } while (TRUE);

         recoverStackRange(from, to, _stackLocs,
                           d, argBase_d, arg_d,
                           squeeze && emitCode);

         (*locPtr) = nextLoc;
         if (!squeeze)
            return;
      }
   }
   if (from > 0)
      recoverStackRange(from, 0, _stackLocs,
                        d, argBase_d, arg_d,
                        emitCode);

#ifdef TRACE
   if (traceFlag) {
      outStream << "Recover stack, d = " << _sd << '\n';
      outStream << '\n';
      Loc::printLocs(_stackLocs, outStream, env());
      outStream << '\n';
      outStream.flush();
   }
#endif
}

Bool ICode::hasLoc(const Loc& loc, const Loc* list) const {
   while (list != NULL) {
      if (&loc == list)
         return TRUE;
      list = list->next();
   }
   return FALSE;
}

Void ICode::insertLoc(Loc& loc) {
   switch (loc.kind()) {
      case Loc::STACK_LOC:

#if (GARBAGE_COLLECTION==1)
         if (((StackLoc&)loc).origin() == StackLoc::FP_REL)
            return;
#endif

         if (!hasLoc(loc, _stackLocs)) {
            StackLoc::insert((StackLoc&)loc, &_stackLocs);
            _sd = _stackLocs->_offset + _stackLocs->_size;
         }
         break;

      case Loc::REGISTER_LOC: {
         Register& reg = (Register&)loc;
         _register[reg.id()] = &reg;
         break;
      }

      case Loc::STATIC_LOC:
      case Loc::COMP_LOC:
      case Loc::CELL_COMP_LOC:
         break;

      case Loc::IND_LOC: {
         insertLoc(((IndLoc&)loc)._loc);
         break;
      }

      default:
         error("ICode::insertLoc: unexpected");
         break;
   }
}

// ICode::initLocs: Initialise locations for basic block

// Implemented in 2 passes
// 1. Assign locations from previous block
// 2. Assigned imported locations; arguments for formal parameters and continuation

Void ICode::initLocs(BasicBlk& basicBlk) {
   _basicBlk = &basicBlk;

   _stackLocs = NULL;
   _sqzPending = FALSE;
   _sd = 0;
   _argFlag = FALSE;
   _altEntry = FALSE;

   basicBlk.liveVarInfo(*this);

#ifdef TRACE
   if (traceFlag) {
      outStream << "Generating var info for instruction " << basicBlk._firstIns->_seqNo << '\n';
      outStream.flush();
   }
#endif

   for (UInt pass = 1; pass <= 3; pass++) {
      VarElement* varElement = basicBlk.vars()._varElements;
      while (varElement != NULL) {
         Var& var = varElement->_var;
         if (var.kind() != Var::GLOBAL_VAR) {
            if (pass == 1) {
               var._locs.empty(msa());
               LocElement* locElement = varElement->_locs._locElements;
               while (locElement != NULL) {
                  Loc* loc = &locElement->_loc;
                  addVarLoc(var, *loc, msa());
                  insertLoc(*loc);
                  locElement = locElement->_next;
               }
            }

//--------- Allocate locations for variables first input to block

            else if (pass == 2) {
               if (varElement->testFlags(VarElement::I) &&
                  varElement->testFlags(VarElement::D)) {
                  assert(var._locs.count(Repr::REPR_UNKNOWN) == 0, "Expected 0 location count");
                  switch (var._kind) {
                     case Var::EXTRA_FORMAL_PARAM_VAR: {
                        Repr repr = var.repr(*this);
                        if (var.needsEnv())
                           addVarLoc(var, allocExtraFormalParameterLoc(var, Repr::REPR_ENV_PTR), _msa);
                        addVarLoc(var, allocExtraFormalParameterLoc(var, repr), _msa);
                        break;
                     }

                     case Var::CONTINUATION_VAR:
                        assert(_stackLocs == NULL, "ICode::initLocs: unexpected");
                        addVarLoc(var, allocStackLoc(labelRepr(), FALSE, FALSE), _msa);
                        break;

                     case Var::FORMAL_PARAM_VAR:
                     case Var::RESULT_VAR:
                        allocRegisters(var);
                        break;

                     case Var::CLOSURE_VAR:
                        var._reprInfo = ReprInfo{ Repr::REPR_CELL_PTR, TRUE };
                        addVarLoc(var, *_register[REG_cell], _msa);
                        addVarLoc(var, *_register[REG_cellEnv], _msa);
                        break;


                     case Var::EXT_GLOBAL_VAR: {
                        Repr repr = var.repr(*this);
                        Repr glbRefRepr;
                        if (!var.needsEnv())
                           addVarLoc(var, *indLoc(repr,
                              *new (_msa) StaticLoc(Repr(new(_msa) PtrRepr(repr)),
                                                    TRUE, FALSE, var.name())), _msa);
                        else {
                           Repr* reprPair = (Repr*)_msa.alloc(sizeof(Repr) * 2);
                           reprPair[0] = repr; reprPair[1] = Repr::REPR_ENV_PTR;
                           StructRepr* glbStruct = addStructRepr(2, reprPair);
                           Loc* loc = new (_msa) StaticLoc(Repr(new(_msa) PtrRepr(Repr(glbStruct))),
                                                           TRUE, FALSE, var.name());
                           addVarLoc(var, *compLoc(repr, *indLoc(Repr(glbStruct), *loc), *glbStruct, 1), _msa);
                           addVarLoc(var, *compLoc(Repr::REPR_ENV_PTR, *indLoc(Repr(glbStruct), *loc), *glbStruct, 2), _msa);
                        }
                        break;
                     }

                     case Var::NON_GLOBAL_FREE_VAR:
                        if (varElement->testFlags(VarElement::U) ||
                            varElement->testFlags(VarElement::O)) {
                           Repr repr = var.repr(*this);
                           addVarLoc(var, allocCellCompLoc(var, repr), _msa);
                           if (var.needsEnv())
                              addVarLoc(var, *_register[REG_cellEnv], _msa);
                        }
                        break;

                     default:
                        break;
                  }
               }
            } else if (pass == 3 && !var.isLive())
               notLive(var);
         }

#ifdef TRACE
         if (traceFlag && pass == 3 && !var._locs.isEmpty())
            var.printLocs(outStream, env());
#endif

         varElement = varElement->_next;
      }
   }

   if (!basicBlk._saveFlag)
      basicBlk._sp = _sp = _sd;
   else
      _sp = basicBlk._sp;

   _basicBlk = &basicBlk;

#ifdef TRACE
   if (traceFlag) {
      outStream << "d = " << _sd;
      outStream << ", sp = " << _sp;
      outStream << '\n';
      outStream.flush();
   }
#endif
}

// ICode::flushNonLocalVar: Save all variables input to basic block
//                          that are not local
//
/*
Void ICode::flushNonLocalVar(Var& var) {
   switch (var._kind) {
      case Var::RESULT_TYPE_VAR:
      case Var::RESULT_VAL_VAR:
      case Var::CLOSURE_VAR:
         if (!_basicBlk->localVar(var) &&
            stackLoc(Repr::REPR_UNKNOWN, var.locSet()) == NULL)
            saveVar(var);
         break;

      default:
         break;
   }
}
*/

//Loc* SourceCode::allocLoc(Repr repr, Loc::Kind kind, Bool argFlag) {

CellCompLoc& ICode::allocCellCompLoc(const Var& var, Repr repr, Bool addToParent/* = TRUE */) {
   return *cellCompLoc(repr,
      var.closure()->bestLoc(Repr::REPR_CELL_PTR),
      var.closure()->_structTemplate->repr(), var._index,
      addToParent);
}

Loc* ICode::allocLoc(Repr repr, Loc::Kind kind, Bool argFlag, Bool reclaimFlag/* = FALSE */) {
   switch (kind) {
      case Loc::STACK_LOC:
         return &allocStackLoc(repr, argFlag, reclaimFlag);

      case Loc::REGISTER_LOC:
         return &allocReg(repr);

      case Loc::IND_LOC: {
         insertInstruction(new (msa()) RSS_Instruction(sizeOfRepr(repr)));
         Register& r = reg(REG_rStruct);
         if (r.inUse())
            flushReg(r._id);
         r._repr = Repr(new(msa()) PtrRepr(repr));
         r._readyToKill = FALSE;
         return indLoc(repr, r);
      }

      default:
         return NULL;
   }
}

Loc* ICode::allocLoc(Repr repr, Var& var, Bool& reused, Loc::Kind kind/* = Loc::UNKNOWN_LOC*/) {
   Bool argFlag = FALSE;
   Bool reclaimFlag = TRUE;
   switch (var.kind()) {
      case Var::NON_GLOBAL_FREE_VAR: {
         Loc* loc = var.bestLocOrNULL(repr);
         if (loc != NULL) {
            reused = TRUE;
            return loc;
         } else if (repr == Repr::REPR_ENV_PTR) {
            loc = var.closure()->bestLocOrNULL(Repr::REPR_ENV_PTR);
            if (loc != NULL) {
               reused = TRUE;
               return loc;
            }
            error("ICode::allocLoc: No closure environment location");
            return NULL;
         } else {
            reused = FALSE;
            return &allocCellCompLoc(var, repr);
         }
      }

      case Var::CLOSURE_VAR: {
         UInt reg = repr == Repr::REPR_CELL_PTR ? REG_cell : REG_cellEnv;
         flushReg(reg);
         return _register[reg];
      }

      case Var::GLOBAL_VAR:
         return new (msa()) StaticLoc(repr, FALSE,
                                            !var._reprInfo._needsEnv ||
                                            repr != Repr::REPR_ENV_PTR, var.name());

      case Var::EXT_GLOBAL_VAR:
         error("ICode::allocLoc: unexpected kind of variable");
         return NULL;

      case Var::ARG_VAR:
      case Var::EXTRA_ARG_VAR:
         argFlag = TRUE;
//------ Drop through

      case Var::CONTINUATION_VAR:
         reclaimFlag = FALSE;
//------ Drop through

      default: {
         Loc* loc = var.bestLocOrNULL(repr);
         if (loc != NULL) {
            if (loc->reserved())
               loc->setReserved(FALSE);
            else
               reused = TRUE;
            return loc;
         } else {
            reused = FALSE;
            return allocLoc(repr,
                            kind != Loc::UNKNOWN_LOC
                               ? kind
                               : var.locKind(repr), argFlag, reclaimFlag);
         }
      }
   }
}

Loc::Kind ICode::temporaryKind(Loc::Kind kind) const {
   return kind == Loc::REGISTER_LOC ? Loc::STACK_LOC : kind;
}

// Save a shared location onto a stack

Void ICode::saveLoc(Loc& loc) {
   Loc& stackLoc = allocStackLoc(loc.repr());
   insertMove(locOperand(loc), locOperand(stackLoc));
   moveLocVars(loc, stackLoc);
}

// UInt ICode::allocStackOffset:  Allocate an offset for a stack location
// A search is made for the lowest unused offset.
// Further investigation is needed to determine if this strategy is better
// than allocating the highest unsed offset less than the stack depth

UInt ICode::allocStackOffset(size_t size, Loc::Kind kind, Bool reclaimFlag) {
   UInt d = _sd;
   if (!reclaimFlag)
      return d;
   StackLoc* stackLocs = _stackLocs;
   UInt minOffset = d;
   UInt from = d;
   UInt to = d;
   while (stackLocs != NULL) {
      StackLoc& loc = *stackLocs;
      if (loc.inUse(FALSE)) {
         from = loc._size + loc._offset;
         if (to > from && (to - from) >= size)
            minOffset = from;
         to = loc._offset;
      } else
         from = loc._offset;
      if (to == d)
         minOffset = from;
      stackLocs = (StackLoc*)stackLocs->_next;
   }
   return to > 0 && to >= size
      ? 0
    : minOffset;
}

StackLoc& ICode::allocStackLoc(Repr repr, Bool argFlag, Bool reclaimFlag) {
   size_t size = ((sizeOfRepr(repr) + 3) / 4) * 4;    // Align to 4 byte boundary
   UInt offset;
   if (!argFlag)
      offset = allocStackOffset(size, Loc::STACK_LOC, reclaimFlag);
   else {
      offset = _arg_d;
      _arg_d += size;
   }
   return allocStackLocAt(repr, size, offset);
}

StackLoc& ICode::allocStackLocAt(Repr repr, size_t size, UInt offset) {
   StackLoc* intersectingLoc;

   if (offset + size > _sd)
      _sd = offset + size;

   Bool exactMatch = FALSE;
   do {
      intersectingLoc = NULL;
      Bool needsFlush = FALSE;
      StackLoc** locPtr = &_stackLocs;
      StackLoc* locToFlush = NULL;
      while ((*locPtr) != NULL && offset < (*locPtr)->_offset + (*locPtr)->_size) {
         StackLoc& loc = **locPtr;
         if (offset + size > loc._offset) { 
            intersectingLoc = &loc;
            if (loc.inUse())
               needsFlush = TRUE;
            if (loc._offset == offset &&
               loc._size == size) {
               exactMatch = TRUE;
            }
            if (!(needsFlush || exactMatch)) {
               if (&loc == _stackLocs)
                  _sd = loc._next != NULL
                     ? ((StackLoc*)(loc._next))->_offset + ((StackLoc*)(loc._next))->_size
                   : 0;
               *locPtr = (StackLoc*)loc._next; 
            }
            break;
         }
         locPtr = ((StackLoc**)&loc._next);
      }

      if (intersectingLoc == NULL)
         break;
      else {
         if (needsFlush)
            flushLoc(*intersectingLoc);
      }
   } while (!exactMatch);

   if (intersectingLoc != NULL)
      StackLoc::extract(*intersectingLoc, &_stackLocs);

   StackLoc* loc = new (_msa) StackLoc(repr, _stackLocs, offset, size);
   StackLoc::insert(*loc, &_stackLocs);

   _sd = _stackLocs->_offset + _stackLocs->_size;

   return *loc;
}

StackLoc& ICode::allocStackLoc(Repr repr) {
   return allocStackLoc(repr, FALSE, TRUE);
}

Loc& ICode::allocExtraFormalParameterLoc(Var& var, Repr repr) {
   Loc* loc = var.bestLocOrNULL(repr);
   return loc != NULL
      ? *loc
      : (Loc&)allocStackLoc(repr);
}

Void InstructionTrait::genLocCode(ICode& code) {
}

// ICode::moveLocVars: Move a saved location's variables and clone children

Void ICode::moveLocVars(Loc& fromLoc, Loc& toLoc) {
   do {
      VarElement* varElement = fromLoc._vars._varElements;
      if (varElement == NULL)
         break;
      Var& var = varElement->_var;
      remVarLoc(var, fromLoc, _msa);
      addVarLoc(var, toLoc, _msa);
   } while (TRUE);

   for (Loc* fromChild = fromLoc._firstChild;
      fromChild != NULL;
      fromChild = fromChild->_next) {
      Loc* toChild = fromChild->clone(&toLoc, FALSE, _msa);
      moveLocVars(*fromChild, *toChild);
   }
}

// ICode::flushLoc: Flush a location to a temporary one

Void ICode::flushLoc(Loc& loc) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Flushing ";
      loc.print(outStream, env());
      outStream << '\n';
      outStream.flush();
   }
#endif

   Bool indLoc = loc.kind() == Loc::REGISTER_LOC && ((Register&)loc).id() == REG_rStruct;

   Loc& fromLoc = !indLoc
      ? loc
      : *loc._firstChild;

   Repr repr = !indLoc
      ? loc.repr()
      : loc.repr().ptrRepr()->repr();

   Loc& toLoc = *allocLoc(repr, temporaryKind(loc._kind), FALSE);
   insertMove(!indLoc ? locOperand(loc) : indLocOperand(repr, loc), locOperand(toLoc));

   moveLocVars(fromLoc, toLoc);
}

StackAdjustInstruction::StackAdjustInstruction(Int diff)
  : Instruction(STACK_ADJUST_INSTR), _diff(diff) {
}

const char* StackAdjustInstruction::mnemonic(Void) const {
   return "stackAdjust";
}

CompLoc* ICode::compLoc(Repr repr, Loc& loc, const StructRepr& structRepr, UInt index,
                        Bool addToParent/* = TRUE*/) const {
   CompLoc* compLoc = new(_msa) CompLoc(Loc::COMP_LOC, repr, loc, structRepr, index);
   return compLoc;
}

CellCompLoc* ICode::cellCompLoc(Repr repr, Loc& loc, StructRepr& structRepr, UInt index,
                                Bool addToParent/* = TRUE*/) const {
   CellCompLoc* cellCompLoc = new(_msa) CellCompLoc(repr, loc, structRepr, index, addToParent);
   return cellCompLoc;
}

IndLoc* ICode::indLoc(Repr repr, Loc& loc, Bool addToParent/* = TRUE*/) const {
   IndLoc* indLoc = new(_msa) IndLoc(repr, loc, addToParent);
   return indLoc;
}

IndexedLoc* ICode::indexedLoc(Repr repr, Loc& loc, const Const& index,
   Bool addToParent/* = TRUE*/) const {
   IndexedLoc* indexedLoc = new(_msa) IndexedLoc(repr, loc, index, addToParent);
   return indexedLoc;
}

IndexedLoc* ICode::indexedLoc(Repr repr, Loc& loc, Loc& index,
                              Bool addToParent/* = TRUE*/) const {
   IndexedLoc* indexedLoc = new(_msa) IndexedLoc(repr, loc, index, addToParent);
   return indexedLoc;
}

LocOperand& ICode::locOperand(Loc& loc, Bool reserved/* = FALSE*/) const {
   return *new(_msa) LocOperand(loc, reserved);
}

LocOperand& ICode::compLocOperand(Repr repr, Loc& loc, StructRepr& structRepr, UInt index) const {
   return *new(_msa) LocOperand(*compLoc(repr, loc, structRepr, index, FALSE));
}

LocOperand& ICode::indLocOperand(Repr repr, Loc& loc) const {
   return *new(_msa) LocOperand(*indLoc(repr, loc, FALSE));
}

CellCompLoc::CellCompLoc(Repr repr, Loc& loc, StructRepr& structRepr, UInt index,
                         Bool addToParent/* = TRUE*/)
 : CompLoc(CELL_COMP_LOC, repr, loc, structRepr, index, addToParent) {
}

CellCompLoc::CellCompLoc(const CellCompLoc& src, Loc* parent, Bool full, MSA& msa)
 : CompLoc(src, parent, full, msa) {
}
