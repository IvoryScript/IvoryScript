/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    repr.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory data representation
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

#ifndef IVORY_REPR_H_DEFINED
#define IVORY_REPR_H_DEFINED

#include "ivory/common.h"

class Repr;
class PtrRepr;
class StructRepr;
class UnionRepr;
class ArrayRepr;

// Repr: Data representation

class Repr {
public:
   enum Tag {
      REPR_UNKNOWN,
      REPR_STACK_MARKER,
      REPR_CELL_PTR,
      REPR_CELL_INFO_PTR,
      REPR_ENV_PTR,
      REPR_LABEL,
      REPR_I_ADDRESS,
      REPR_LABEL_PAIR,
      REPR_TYPE,
      REPR_VOID,
      REPR_EXPR,
      REPR_NAME,
      REPR_INT,
      REPR_BYTE,
      REPR_BITS,
      REPR_FLOAT,
      REPR_DOUBLE,
      REPR_STRING,
      REPR_REF,
      REPR_CHAR,
      REPR_TAG,
      REPR_UTC,
      REPR_DIR,
      REPR_DIR_ENTRY,
      REPR_PLAIN,
      REPR_PTR,
      REPR_STRUCT,
      REPR_UNION,
      REPR_ARRAY,
      REPR_SERIAL_CONTEXT,
      REPR_FILE_HANDLE
   };
   Repr()
      : _tag(REPR_UNKNOWN) { _ext._null = NULL; }
   Repr(Repr::Tag tag)
      : _tag(tag) { _ext._null = NULL; }
   Repr(PtrRepr* ptrRepr)
      : _tag(REPR_PTR) { _ext._ptrRepr = ptrRepr; }
   Repr(StructRepr* structRepr)
      : _tag(REPR_STRUCT)  { _ext._structRepr = structRepr; }
   Repr(UnionRepr* unionRepr)
      : _tag(REPR_UNION) { _ext._unionRepr = unionRepr; }
   Repr(ArrayRepr* arrayRepr)
      : _tag(REPR_ARRAY) { _ext._arrayRepr = arrayRepr; }

   inline Repr::Tag tag(Void) const { return _tag; }
   inline PtrRepr* ptrRepr(Void) const { return _ext._ptrRepr; }
   inline StructRepr* structRepr(Void) const { return _ext._structRepr; }
   inline UnionRepr*  unionRepr(Void) const { return _ext._unionRepr; }
   inline ArrayRepr*  arrayRepr(Void) const { return _ext._arrayRepr; }

   inline Bool isUnknown(Void) const { return _tag == REPR_UNKNOWN; };

#if (GARBAGE_COLLECTION==1)
   Bool markableForGC(Void) const;
#endif

protected:
   Tag         _tag;
   union {
      Void*       _null;         // For tidy initialisation
      PtrRepr*    _ptrRepr;
      StructRepr* _structRepr;
      UnionRepr*  _unionRepr;
      ArrayRepr*  _arrayRepr;
  } _ext;

   friend bool operator == (const Repr l, const Repr r);
   friend inline bool operator != (const Repr l, const Repr r) { return !(l == r); }
};

// Pointer: Pointer representation
//          Its main use is to provide an allocation size when copying

class PtrRepr {
public:
   PtrRepr(Repr repr) :
      _id(0), _repr(repr) {}

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline UInt id(Void) const { return _id; }
   inline UInt& id(Void) { return _id; }
   inline Repr repr(Void) const { return _repr; }
   inline Repr& repr(Void) { return _repr; }

protected:
   UInt     _id;
   Repr     _repr;
};

// An instance of a pointer representation
// Used to support self-referential pointer types

typedef struct PtrReprInstance_tag {
   const PtrReprInstance_tag* _next;
   const PtrRepr*             _ptrRepr;
   UInt                       _id;
} PtrReprInstance;

// StructRepr: Structure representation

class StructRepr {
public:
   StructRepr(Void) :
      _id(0),
      _nReprs(0), _reprV(NULL),
      _size(0), _sizeV(NULL), _offsetV(NULL),
      _maxCompSize(0) {}

   StructRepr(UInt nReprs, Repr* reprV,
              size_t* sizeV = NULL, size_t* offsetV = NULL);

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline UInt id(Void) const { return _id; }
   inline UInt& id(Void) { return _id; }
   inline UInt nReprs(Void) const { return _nReprs; }
   inline const Repr* reprV(Void) const { return _reprV; }
   inline size_t& size(Void) { return _size; }
   inline size_t size(Void) const { return _size; }
   inline size_t*const & offsetV(Void) const { return _offsetV; }
   inline size_t* const & sizeV(Void) const { return _sizeV; }
   inline size_t*& offsetV(Void) { return _offsetV; }
   inline size_t*& sizeV(Void) { return _sizeV; }
   inline size_t maxCompSize(Void) { return _maxCompSize; }
   inline size_t maxAlign(Void) { return _maxAlign; }

#if (GARBAGE_COLLECTION==1)
   Bool markableForGC(Void) const;
#endif

   size_t offset(UInt i) const;
   size_t size(UInt i) const;

   Bool matches(UInt nReprs, const Repr* reprV) const;

protected:

   Void assignComponentSizesAndOffsets(Void);

   UInt     _id;
   UInt     _nReprs;
   Repr*    _reprV;
   size_t   _size;
   size_t*  _sizeV;
   size_t*  _offsetV;
   size_t   _maxCompSize;
   size_t   _maxAlign;
};

// UnionRepr: Union representation

class UnionRepr {
public:
   UnionRepr(UInt nReprs, Repr* reprV)
      : _id(0), _nReprs(nReprs), _reprV(reprV), _size(0) {}

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline UInt id(Void) const { return _id; }
   inline UInt& id(Void) { return _id; }
   inline UInt nReprs(Void) const { return _nReprs; }
   inline const Repr* reprV(Void) const { return _reprV; }
   inline size_t& size(Void) { return _size; }
   inline size_t size(Void) const { return _size; }

#if (GARBAGE_COLLECTION==1)
   Bool markableForGC(Void) const;
#endif

   Bool matches(UInt nReprs, const Repr* reprV) const;

protected:
   UInt     _id;
   UInt     _nReprs;
   Repr*    _reprV;
   size_t   _size;
};

// ArrayRepr: Array representation

class ArrayRepr {
public:
   ArrayRepr(UInt nElements, Repr elementRepr)
      : _nElements(nElements) , _elementRepr(elementRepr) {}

   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#endif
   inline Void operator delete(Void* ptr) {}

   inline UInt nElements(Void) const { return _nElements; }
   inline Repr elementRepr(Void) const { return _elementRepr; }

#if (GARBAGE_COLLECTION==1)
   Bool markableForGC(Void) const;
#endif

protected:
   UInt  _nElements;
   Repr  _elementRepr;
};

typedef struct ReprInfo_tag {
   Repr _repr;
   Bool _needsEnv;
} ReprInfo;

#define isBuiltInName(name) ((name)<builtInNameCount)

extern size_t alignOfRepr(Repr repr);

extern UInt arity(TypeSig typeSig);

extern Name builtInName$(const char* name);

extern Int builtInNameCount;

extern NameTable& builtInNameTable(Void);

extern TypeTable& builtInTypeTable(Void);

extern Name copyName$(const Name name, const NameTable& srcNameTable,
                                             NameTable& dstNameTable);

#define copyName(name,srcNameTable,dstNameTable)\
((name)<builtInNameCount)?(name):copyName$(name,srcNameTable,dstNameTable)

#define dropName_(name,nameTable)\
if((name)>=builtInNameCount){(nameTable).dropName(name-builtInNameCount);}

extern Bool equalsName(Name name1, NameTable& nameTable1,
                       Name name2, NameTable& nameTable2);

extern Bool formExistsIn(Expr expr, UInt n, UInt32* forms);

extern ExprEnumVal formOf(Expr e);

extern Expr headForm(Expr expr);

extern UInt nameHashVal(Name name, const NameTable& nameTable);

extern Name nameIndex(Name name, const NameTable& nameTable);

extern const String nameString_(Name name, const NameTable& nameTable);

extern size_t sizeOfRepr(Repr repr);

extern TypeSig typeSig(Expr expr);

extern Name useName(const char* string, NameTable& nameTable);

#endif /* IVORY_REPR_H_DEFINED */
