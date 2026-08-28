/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    common.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common definitions for use with Ivory.
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

#ifndef IVORY_COMMON_H_DEFINED
#define IVORY_COMMON_H_DEFINED

#include "general.h"
#include "objStore.h"
#include <vector.h>

// The following definition is related to multi-threading.
// If there is only one instance of the virtual machine for a process then
// the library can be built with it set to 1, otherwise it must be set to 0.

//#define GLOBAL_VM 1

// The following definition is related to garbage collection.
// It is set to 1 if garbage collection is being carried out, otherwise 0.

//#define GARBAGE_COLLECTION 1

// The following definition enables or disables run-time type checking.
// It should only be set to 0 for a fixed trusted application.

//#define RUN_TIME_TYPE_CHECKS 0

// The minimum length of a closure to allow for in-place updates.
// Excludes tag. 

#define MIN_CLOSURE_SIZE   8

#define FALSE              0
#define TRUE               1

// Variable length cell limits, arbitrary

#define MAX_STRING_LENGTH	65536
#define MAX_TUPLE_DEGREE   256
#define MAX_CELL_BODY_SIZE 65536

#define MAX_NUM_TYPES	   65536
#define MAX_NUM_NAMES	   65536
#define NUM_TAGS           256 
#define NUM_CHARS          256 

//------------------------------------------------------------------------------
// Primary expression tags
//------------------------------------------------------------------------------
 
#define TAG_BITS           1
#define TAG_MASK           ((1<<TAG_BITS)-1)
#define TAG_INCR           (1<<TAG_BITS)

#define PTR_TAG            0
#define ENUM_VAL_TAG       1          
#define ENUM_VAL_INCR      (1<<TAG_BITS)

#define ERROR              (Expr(static_cast<ExprEnumVal>(0)))

#define NEXT_TAG           ENUM_VAL_TAG

// Special expression enumerations

#define AP                 NEXT_TAG
#define CLOSURE            (AP+ENUM_VAL_INCR)
#define PARTIAL_AP         (CLOSURE+ENUM_VAL_INCR)
#define INDIRECT           (PARTIAL_AP+ENUM_VAL_INCR)
#define TYPE_CON           (INDIRECT+ENUM_VAL_INCR)
#define TYPE_METHOD        (TYPE_CON+ENUM_VAL_INCR)
#define DATA_CON           (TYPE_METHOD+ENUM_VAL_INCR)
#define BUILT_IN_FN        (DATA_CON+ENUM_VAL_INCR)
#define BUILT_IN_DATA_CON  (BUILT_IN_FN+ENUM_VAL_INCR)
#define CLASS              (BUILT_IN_DATA_CON+ENUM_VAL_INCR)
#define CLASS_DEFN_REF     (CLASS+ENUM_VAL_INCR)
#define CLASS_METHOD       (CLASS_DEFN_REF+ENUM_VAL_INCR)
#define CLASS_METHOD_DECL  (CLASS_METHOD+ENUM_VAL_INCR)
#define CLASS_METHOD_DEFN  (CLASS_METHOD_DECL+ENUM_VAL_INCR)
#define CLASS_OBJECT_DEFN  (CLASS_METHOD_DEFN+ENUM_VAL_INCR)
#define CLASS_DATA_CON     (CLASS_OBJECT_DEFN+ENUM_VAL_INCR)
#define INSTANCE           (CLASS_DATA_CON+ENUM_VAL_INCR)
#define INSTANCE_METHOD_DECL     (INSTANCE+ENUM_VAL_INCR)
#define INSTANCE_METHOD_DEFN     (INSTANCE_METHOD_DECL+ENUM_VAL_INCR)
#define INSTANCE_DATA_CON_DECL   (INSTANCE_METHOD_DEFN+ENUM_VAL_INCR)
#define INSTANCE_DATA_CON_DEFN   (INSTANCE_DATA_CON_DECL+ENUM_VAL_INCR)
#define STRICT_TYPE_QUAL   (INSTANCE_DATA_CON_DEFN+ENUM_VAL_INCR)
#define INEQUALITY_TYPE_QUAL  (STRICT_TYPE_QUAL+ENUM_VAL_INCR)
#define INSTANCE_TYPE_QUAL (INEQUALITY_TYPE_QUAL+ENUM_VAL_INCR)
#define CONST		   (INSTANCE_TYPE_QUAL+ENUM_VAL_INCR)
#define DATA               (CONST+ENUM_VAL_INCR)
#define VAR                (DATA+ENUM_VAL_INCR)
#define VAR_REF            (VAR+ENUM_VAL_INCR)
#define TUPLE              (VAR_REF+ENUM_VAL_INCR)
#define NAME_TYPED_VAL_BINDING (TUPLE+ENUM_VAL_INCR)
#define NAME_OCC           (NAME_TYPED_VAL_BINDING+ENUM_VAL_INCR)
#define FAIL               (NAME_OCC+ENUM_VAL_INCR)
#define LET                (FAIL+ENUM_VAL_INCR)
#define FN_AP              (LET+ENUM_VAL_INCR)
#define DYADIC_OP          (FN_AP+ENUM_VAL_INCR)
#define MONADIC_OP         (DYADIC_OP+ENUM_VAL_INCR)
#define SEQ                (MONADIC_OP+ENUM_VAL_INCR)
#define COND               (SEQ+ENUM_VAL_INCR)
#define PTR_CONSTRUCTOR    (COND+ENUM_VAL_INCR)
#define CONSTRUCT          (PTR_CONSTRUCTOR+ENUM_VAL_INCR)
#define DECONSTRUCT        (CONSTRUCT+ENUM_VAL_INCR)
#define DECONSTRUCT_PTR    (DECONSTRUCT+ENUM_VAL_INCR)
#define SELECT             (DECONSTRUCT_PTR+ENUM_VAL_INCR)
#define SEL_PTR            (SELECT+ENUM_VAL_INCR)
#define FAT_BAR            (SEL_PTR+ENUM_VAL_INCR)
#define TAG_OF             (FAT_BAR+ENUM_VAL_INCR)
#define EQ_TAG_FN          (TAG_OF+ENUM_VAL_INCR)
#define EQ_NULL_FN         (EQ_TAG_FN+ENUM_VAL_INCR)
#define RETURN_STATE       (EQ_NULL_FN+ENUM_VAL_INCR)
#define SHARED             (RETURN_STATE+ENUM_VAL_INCR)
#define SHARED_EXPR        (SHARED+ENUM_VAL_INCR)
#define SHARED_PATTERN_AP  (SHARED_EXPR+ENUM_VAL_INCR)
#define EXCEPTION          (SHARED_PATTERN_AP+ENUM_VAL_INCR)
#define DISPATCH           (EXCEPTION+ENUM_VAL_INCR)
#define CONST_PATTERN      (DISPATCH+ENUM_VAL_INCR)
#define VAR_PATTERN        (CONST_PATTERN+ENUM_VAL_INCR)
#define APP_LAMBDA         (VAR_PATTERN+ENUM_VAL_INCR)
#define FAT_BAR_SEQ        (APP_LAMBDA+ENUM_VAL_INCR)
#define SNIPPET            (FAT_BAR_SEQ+ENUM_VAL_INCR)
#define DIRECT_LAMBDA      (SNIPPET+ENUM_VAL_INCR)
#define CURRIED_LAMBDA     (DIRECT_LAMBDA+ENUM_VAL_INCR)
#define BUILT_IN_PAP       (CURRIED_LAMBDA+ENUM_VAL_INCR)

#define NEXT_TAG_1         (100*ENUM_VAL_INCR+ENUM_VAL_TAG)

//------------------------------------------------------------------------------
// Syntactic tags
//------------------------------------------------------------------------------

#define MODULE             NEXT_TAG_1
#define ORDER              (MODULE+ENUM_VAL_INCR)
#define SEQUENCE           (ORDER+ENUM_VAL_INCR)
#define TYPE_DEFN          (SEQUENCE+ENUM_VAL_INCR)
#define OBJECT_DEFN        (TYPE_DEFN+ENUM_VAL_INCR)
#define DECL               (OBJECT_DEFN+ENUM_VAL_INCR)
#define DEFN               (DECL+ENUM_VAL_INCR)
#define DATA_CON_DECL      (DEFN+ENUM_VAL_INCR)
#define DATA_CON_DEFN      (DATA_CON_DECL+ENUM_VAL_INCR)
#define CONCRETE_DATA_CON  (DATA_CON_DEFN+ENUM_VAL_INCR)
#define PLAIN_DATA_CON     (CONCRETE_DATA_CON+ENUM_VAL_INCR)
#define LAMBDA             (PLAIN_DATA_CON+ENUM_VAL_INCR)
#define LET_REC            (LAMBDA+ENUM_VAL_INCR)
#define CASE               (LET_REC+ENUM_VAL_INCR)
#define OTHERWISE          (CASE+ENUM_VAL_INCR)
#define LISTCOMP           (OTHERWISE+ENUM_VAL_INCR)
#define GENERATOR          (LISTCOMP+ENUM_VAL_INCR)
#define FILTER             (GENERATOR+ENUM_VAL_INCR)
#define PRIMITIVE          (FILTER+ENUM_VAL_INCR)
#define STRICT             (PRIMITIVE+ENUM_VAL_INCR)
#define TYPE_SIG           (STRICT+ENUM_VAL_INCR)
#define CONSTRAIN          (TYPE_SIG+ENUM_VAL_INCR)
#define COERCE             (CONSTRAIN+ENUM_VAL_INCR)
#define NO_COERCE          (COERCE+ENUM_VAL_INCR)
#define MAP_TO_ENV         (NO_COERCE+ENUM_VAL_INCR)
#define NO_MAP_TO_ENV      (MAP_TO_ENV+ENUM_VAL_INCR)
#define REDUCE             (NO_MAP_TO_ENV+ENUM_VAL_INCR)
#define INLINE             (REDUCE+ENUM_VAL_INCR)
#define IF                 (INLINE+ENUM_VAL_INCR)
#define RETURN             (IF+ENUM_VAL_INCR)
#define WILDCARD_PATTERN   (RETURN+ENUM_VAL_INCR)
#define TUPLE_PATTERN      (WILDCARD_PATTERN+ENUM_VAL_INCR)
#define DATA_CON_PATTERN   (TUPLE_PATTERN+ENUM_VAL_INCR)
#define CONCRETE_PATTERN   (DATA_CON_PATTERN+ENUM_VAL_INCR)
#define PTR_DECON_PATTERN  (CONCRETE_PATTERN+ENUM_VAL_INCR)
#define PLAIN_PATTERN      (PTR_DECON_PATTERN+ENUM_VAL_INCR)
#define CLASS_DEFN         (PLAIN_PATTERN+ENUM_VAL_INCR)
#define INSTANCE_DEFN      (CLASS_DEFN+ENUM_VAL_INCR)
#define SUBORDINATE        (INSTANCE_DEFN+ENUM_VAL_INCR)
#define EQUALITY           (SUBORDINATE+ENUM_VAL_INCR)
#define INEQUALITY         (EQUALITY+ENUM_VAL_INCR)
#define CONJUNCTION	   (INEQUALITY+ENUM_VAL_INCR)
#define DISJUNCTION	   (CONJUNCTION+ENUM_VAL_INCR)
#define NEGATION		   (DISJUNCTION+ENUM_VAL_INCR)
#define DYNAMIC            (NEGATION+ENUM_VAL_INCR)
#define CONSTANT           (DYNAMIC+ENUM_VAL_INCR)
#define VARIABLE           (CONSTANT+ENUM_VAL_INCR)
#define INDISPENSABLE      (VARIABLE+ENUM_VAL_INCR)
#define TRANSIENT          (INDISPENSABLE+ENUM_VAL_INCR)
#define PERSIST            (TRANSIENT+ENUM_VAL_INCR)
#define PRIVATE            (PERSIST+ENUM_VAL_INCR)
#define PUBLIC             (PRIVATE+ENUM_VAL_INCR)
#define LITERAL            (PUBLIC+ENUM_VAL_INCR)
#define THIS               (LITERAL+ENUM_VAL_INCR)

#define NEXT_TAG_2         (200*ENUM_VAL_INCR+ENUM_VAL_TAG)

//------------------------------------------------------------------------------
// Pre-defined types and expression forms
//------------------------------------------------------------------------------

#define VOID               NEXT_TAG_2
#define ARROW              (VOID+ENUM_VAL_INCR)
#define LABEL              (ARROW+ENUM_VAL_INCR)
#define CELL_INFO          (LABEL+ENUM_VAL_INCR)
#define CELL_PTR           (CELL_INFO+ENUM_VAL_INCR)
#define POINTER            (CELL_PTR+ENUM_VAL_INCR)
#define ENV_PTR            (POINTER+ENUM_VAL_INCR)
#define TAG                (ENV_PTR+ENUM_VAL_INCR)
#define UNKNOWN            (TAG+ENUM_VAL_INCR)
#define NAME               (UNKNOWN+ENUM_VAL_INCR)
#define TYPE               (NAME+ENUM_VAL_INCR)
#define INT                (TYPE+ENUM_VAL_INCR)
#define CHAR               (INT+ENUM_VAL_INCR)
#define BYTE_STRING        (CHAR+ENUM_VAL_INCR)
#define REF                (BYTE_STRING+ENUM_VAL_INCR)

#define NEXT_TAG_3         (300*ENUM_VAL_INCR+ENUM_VAL_TAG)

//------------------------------------------------------------------------------
// Primitive structure tags
//------------------------------------------------------------------------------

#define CONS               NEXT_TAG_3
#define CONS_ENV           (CONS+ENUM_VAL_INCR)

// Tuple constructors and structure tags

#define TUPLE_MIN          (CONS_ENV+ENUM_VAL_INCR)
#define TUPLE_MAX          (TUPLE_MIN+MAX_TUPLE_DEGREE*ENUM_VAL_INCR)

#define PAIR               TUPLE_MIN
#define TRIPLE             TUPLE_MIN+ENUM_VAL_INCR

//------------------------------------------------------------------------------
// Name values of type of type tag(Name, name)
// Negative names have no name table entry
//------------------------------------------------------------------------------

#ifdef NAME_MIN
   #undef NAME_MIN
#endif
#define NAME_MIN            (TUPLE_MAX+ENUM_VAL_INCR)
#ifdef NAME_MAX
   #undef NAME_MAX
#endif
#define NAME_MAX            (NAME_MIN+(MAX_NUM_NAMES-1)*ENUM_VAL_INCR)
#define NAME_ZERO           ((((NAME_MIN>>TAG_BITS)/2+(NAME_MAX>>TAG_BITS)/2)\
                            <<TAG_BITS)+ENUM_VAL_TAG)

//------------------------------------------------------------------------------
// Type values of type of type tag(Type, type)
//------------------------------------------------------------------------------

#define TYPE_MIN           (NAME_MAX+ENUM_VAL_INCR)
#define TYPE_MAX           (TYPE_MIN+(MAX_NUM_TYPES-1)*ENUM_VAL_INCR)

//------------------------------------------------------------------------------
// Constructor tag values of type tag(Tag, t)
//------------------------------------------------------------------------------

#ifdef TAG_MIN
#undef TAG_MIN
#endif
#define TAG_MIN           (TYPE_MAX+ENUM_VAL_INCR)
#ifdef TAG_MAX
#undef TAG_MAX
#endif
#define TAG_MAX           (TAG_MIN+(NUM_TAGS-1)*ENUM_VAL_INCR)

//------------------------------------------------------------------------------
// Character values of type tag(Char, c)
//------------------------------------------------------------------------------

#ifdef CHAR_MIN
   #undef CHAR_MIN
#endif
#define CHAR_MIN           (TAG_MAX+ENUM_VAL_INCR)
#ifdef CHAR_MAX
   #undef CHAR_MAX
#endif
#define CHAR_MAX           (CHAR_MIN+(NUM_CHARS-1)*ENUM_VAL_INCR)

//------------------------------------------------------------------------------
// Small integer values of type tag(Int, i)
//------------------------------------------------------------------------------

#ifdef INT_MIN
   #undef INT_MIN
#endif
#define INT_MIN            (CHAR_MAX+ENUM_VAL_INCR)
#ifdef INT_MAX
   #undef INT_MAX
#endif
#define INT_MAX            ((0x3ffffffeU<<TAG_BITS)+ENUM_VAL_TAG)
#define INT_ZERO           ((((INT_MIN>>TAG_BITS)/2+(INT_MAX>>TAG_BITS)/2)<<TAG_BITS)+ENUM_VAL_TAG)

//------------------------------------------------------------------------------
// Small integer values of type Int
//------------------------------------------------------------------------------

#define MIN_SMALL_INT      ((Int)(INT_MIN/ENUM_VAL_INCR)-(Int)(INT_ZERO/ENUM_VAL_INCR))
#define MAX_SMALL_INT      ((Int)(INT_MAX/ENUM_VAL_INCR)-(Int)(INT_ZERO/ENUM_VAL_INCR))

// Function argument flags

#define STRICT_FLAG        (1<<0)
#define TYPE_VAR_FLAG      (1<<1)

#ifndef NULL_NAME
#define NULL_NAME -1
#endif

#ifdef GCC2
#define Void void
#else
typedef void               Void;
#endif
typedef short int          Name;
typedef short int          Type;
typedef unsigned char      Bits8;
typedef unsigned short     Bits16;
typedef unsigned long      Bits;
typedef unsigned long      Bits32;
typedef UInt64             Bits64;
typedef Bits8              Byte;
typedef int                Int;
typedef unsigned int       UInt;
typedef float              Float;
typedef double             Double;
typedef unsigned short int Tag;
typedef char               Char;
typedef bool               Bool;
typedef char*              String;
typedef const char*        ConstString;
typedef vector<Byte>       ByteVector;
typedef Void*              Ptr;
typedef UInt32             ExprRepr;
typedef UInt32             ExprEnumVal;

class Cell;
struct CellInfo;
class NameTable;
class ISegmentTable;
class ICellInfoMap;
class TypeTable;

// Define virtual machine arguments

#if (GLOBAL_VM == 0)
   class VirtualMachine;
   #define argVM VirtualMachine& vm
   #define argN_VM , argVM
   #define n_vm , vm
#else
   #define argVM Void
   #define argN_VM
   #define vm
   #define n_vm
#endif
  
//------------------------------------------------------------------------------
// Env: Environment for pointers, names and types
//------------------------------------------------------------------------------

class Env {
public:
   Env(Void)
      : _msa(NULL), _nameTable(NULL),
        _segmentTable(NULL), _cellInfoMap(NULL),
        _typeTable(NULL) {}
   Env(MSA& msa)
      : _msa(&msa), _nameTable(NULL),
        _segmentTable(NULL), _cellInfoMap(NULL),
        _typeTable(NULL) {}
   Env(NameTable& nameTable)
      : _msa(NULL), _nameTable(&nameTable),
        _segmentTable(NULL), _cellInfoMap(NULL),
        _typeTable(NULL) {}
   Env(TypeTable& typeTable)
      : _msa(NULL), _nameTable(NULL),
        _segmentTable(NULL), _cellInfoMap(NULL),
        _typeTable(&typeTable) {}
   Env(NameTable& nameTable,
       ISegmentTable* segmentTable, ICellInfoMap* iCellInfoMap,
       TypeTable& typeTable)
      : _msa(NULL), _nameTable(&nameTable),
        _segmentTable(segmentTable), _cellInfoMap(_cellInfoMap),
        _typeTable(&typeTable) {}
   Env(MSA& msa, NameTable& nameTable,
       ISegmentTable* segmentTable, ICellInfoMap* cellInfoMap,
       TypeTable& typeTable)
      : _msa(&msa), _nameTable(&nameTable),
        _segmentTable(segmentTable), _cellInfoMap(cellInfoMap),
        _typeTable(&typeTable)  {}
   inline Void* operator new(size_t size, MSA& msa) { return msa.alloc(size); }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr, MSA& msa) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   inline MSA& msa(Void)       const { return *_msa; }
   inline NameTable& nameTable(Void) { return *_nameTable; }
   inline const NameTable& nameTable(Void) const { return *_nameTable; }
   inline ISegmentTable* segmentTable(Void) { return _segmentTable; }
   inline const ISegmentTable* segmentTable(Void) const { return _segmentTable; }
   inline ICellInfoMap* cellInfoMap(Void) { return _cellInfoMap; }
   inline const ICellInfoMap* cellInfoMap(Void) const { return _cellInfoMap; }
   inline TypeTable& typeTable(Void) { return *_typeTable; }
   inline const TypeTable& typeTable(Void) const { return *_typeTable; }

protected:
   MSA*           _msa;
   NameTable*     _nameTable;
   ISegmentTable* _segmentTable;
   ICellInfoMap*  _cellInfoMap;
   TypeTable*     _typeTable;
};

typedef Env* EnvPtr;

//------------------------------------------------------------------------------
// Expr: Represents an expression value of type 'Exp *'
//------------------------------------------------------------------------------

class Expr {
public:
   inline Expr(Void) { _repr._raw = 0; };
   inline Expr(ExprEnumVal enumVal) { _repr._enumVal = enumVal; }
   inline Expr(const Expr& src) { _repr._raw = src.repr(); }
   inline Expr(Cell* cellPtr) { _repr._cellPtr = cellPtr; }
   inline Expr(const Cell* cellPtr) { _repr._constCellPtr = cellPtr; }
   inline Expr(CellInfo* cellInfoPtr) { _repr._cellInfoPtr = cellInfoPtr; }
   inline Expr(const CellInfo* cellInfoPtr) { _repr._constCellInfoPtr = cellInfoPtr; }
   Expr(Expr src, const Env& srcEnv, Env& env, MSA& msa argN_VM);
   Void assign(const Expr& src, const Env& srcEnv, Env& env, MSA& msa argN_VM);
   Void destroy(Env& env, MSA& msa argN_VM);

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
	inline Void operator delete(Void* ptr1, Void* ptr2) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   inline operator ExprEnumVal (Void) { return _repr._enumVal; }
   inline operator const ExprEnumVal (Void) const { return _repr._enumVal; }
 	inline operator Cell* (Void) { return _repr._cellPtr; }
   inline operator const Cell* (Void) const { return _repr._cellPtr; }
   inline operator CellInfo* (Void) { return _repr._cellInfoPtr; }
   inline operator const CellInfo* (Void) const { return _repr._constCellInfoPtr; }
   inline UInt tag(Void) const { return _repr._raw & TAG_MASK; }
   inline ExprRepr repr(Void) const { return _repr._raw; }

   Void print(ostream& os, const Env& env) const;

protected:
   union {
      ExprRepr          _raw;
      ExprEnumVal       _enumVal;
      Cell*             _cellPtr;
      const Cell*       _constCellPtr;
      CellInfo*         _cellInfoPtr;
      const CellInfo*   _constCellInfoPtr;
   } _repr;

   friend inline bool operator == (const Expr l, const Expr r) { return l._repr._raw == r._repr._raw; }
   friend inline bool operator != (const Expr l, const Expr r) { return l._repr._raw != r._repr._raw; }
};

//------------------------------------------------------------------------------
// TypeSig: Type signature
//------------------------------------------------------------------------------

typedef Expr TypeSig;

//------------------------------------------------------------------------------
// Cell: A cell expression
//------------------------------------------------------------------------------

typedef Byte CellBody[MAX_CELL_BODY_SIZE];

class Cell {
public:
   inline Cell(Expr tag) : _tag(tag) {}

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
   inline Void* operator new(size_t size, size_t bodySize, MSA& msa) {
		return msa.alloc(size - MAX_CELL_BODY_SIZE + bodySize);
   }

#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
	inline Void operator delete(Void* ptr, size_t bodySize, MSA& msa) {}
#else
	inline Void operator delete(Void* ptr) {}
#endif
   inline Expr& tag(Void) { return _tag; }
   inline const Expr& tag(Void) const { return _tag; }
   inline CellBody& body(Void) { return _body; }
   inline const CellBody& body(Void) const { return _body; }

protected:
   Expr				_tag;
   CellBody	      _body;
};

//------------------------------------------------------------------------------
// Label: A native continuation label
//------------------------------------------------------------------------------

//typedef Label (*Label)(argVM);    // Not permitted
typedef Void* (*Label)(argVM);      // Return will be cast to Label

typedef Label Entry;

//------------------------------------------------------------------------------
// IAddress: A general address
//------------------------------------------------------------------------------

typedef unsigned char* IAddress;

struct LabelPair {
   Label    _label;
   IAddress _iLabel;
};

#define label(name) ((Label)name)
#define declareLabel(name) static Label name(argVM)
#define defineLabel(name) Label name(argVM)                           

#define entry(name) label(name##$entry)
#define declareEntry(name) extern Label name##$entry(argVM)
#define defineEntry(name) defineLabel(name##$entry)

#define altEntry(name) (label(name##$altEntry))
#define declareAltEntry(name) extern Label name##$altEntry(argVM)
#define defineAltEntry(name) defineLabel(name##$altEntry)

#define declareLocalEntry(name) declareLabel(name##$entry)
#define declareLocalAltEntry(name) declareLabel(name##$altEntry)

//------------------------------------------------------------------------------
// CellInfo: Cell information and dispatch table
//------------------------------------------------------------------------------

// Cell methods

typedef Cell* (*CellCopyFn)(Cell& src, const Env& srcEnv,
                            Env& env, MSA& msa argN_VM);

typedef Void (*CellDestroyFn)(Cell& cell_, Env& env, MSA& msa argN_VM);
typedef Void (*CellShowFn)(const Cell& cell_, ostream& os, const Env& env argN_VM);

struct CellInfo {
   CellInfo(Expr form, size_t size,
            Label entry_,
            CellCopyFn cellCopyFn = NULL,

#if (SERIALISATION==1)
            Label cellExtractFn = NULL,
            Label cellInsertFn = NULL,
#endif

#if (GARBAGE_COLLECTION==1)
            Label cellGC_MarkFn = NULL,
#endif

            CellDestroyFn cellDestroyFn = NULL,
            CellShowFn cellShowFn = NULL);

   CellInfo(Expr form, size_t size, Label entry_, Label altEntry_,
            CellCopyFn cellCopyFn = NULL,

#if (SERIALISATION==1)
            Label cellExtractFn = NULL,
            Label cellInsertFn = NULL,
#endif

#if (GARBAGE_COLLECTION==1)
            Label cellGC_MarkFn = NULL,
#endif

            CellDestroyFn cellDestroyFn = NULL,
            CellShowFn cellShowFn = NULL);

   inline const Expr form(Void) const { return _form; }
   inline size_t size(Void) const { return _size; }
   inline Label entry_(Void) const { return _entry; }
   inline Label altEntry_(Void) const { return _altEntry; }
   inline CellCopyFn cellCopyFn(Void) const { return _cellCopyFn; }

#if (SERIALISATION==1)
   inline Label cellExtractFn(Void) const { return _cellExtractFn; }
   inline Label cellInsertFn(Void) const { return _cellInsertFn; }
#endif

#if (GARBAGE_COLLECTION==1)
   inline Label cellGC_MarkFn(Void) const { return _cellGC_MarkFn; }
#endif

   inline CellDestroyFn cellDestroyFn(Void) const { return _cellDestroyFn; }
   inline CellShowFn cellShowFn(Void) const { return _cellShowFn; }
 
protected:
   Expr           _form;
   size_t         _size;
   Label          _entry;
   Label          _altEntry;
   CellCopyFn     _cellCopyFn;

#if (SERIALISATION==1)
   Label          _cellExtractFn;
   Label          _cellInsertFn;
#endif

#if (GARBAGE_COLLECTION==1)
   Label          _cellGC_MarkFn;
#endif

   CellDestroyFn  _cellDestroyFn;
   CellShowFn     _cellShowFn;
};

//------------------------------------------------------------------------------
// Ref: Generalised store reference
//------------------------------------------------------------------------------

class Ref {
public:
   inline Ref(Void) {}
   inline Ref(OID oid) { _repr = oid; }
   Ref(const Ref& src, const Env& srcEnv, Env& dstEnv, MSA& msa) { _repr = src._repr; }
   Void destroy(Env& env, MSA& msa) {}
   Void print(ostream& os, const Env& env) const;
   Void printWithType(ostream& os, const Env& env) const;

   inline operator OID (Void) { return _repr; }
   inline operator const OID (Void) const { return _repr; }

   friend inline bool operator == (const Ref l, const Ref r) {
      return l._repr == r._repr;
   }
   friend inline bool operator != (const Ref l, const Ref r) {
      return l._repr != r._repr;
   }
   friend inline ostream& operator << (ostream& os, Ref ref) {
      os << ref._repr;
      return os;
   };
protected:
   OID  _repr;
};

// Basic storage macros

#ifndef localConst
#define localConst
#endif

#define toEnumVal(expr) (static_cast<ExprEnumVal>(expr))

#define fromCell(cell_) (static_cast<localConst Expr>(&(cell_)))
#define toCell(expr) (*(static_cast<localConst Cell*>(expr)))
#define toConstCell(expr) (*(static_cast<const Cell*>(expr)))
#define cellBody(cell,type) (*(static_cast<localConst type*>(static_cast<localConst Void*>(&(cell).body()))))
#define constCellBody(cell,type) (*(static_cast<const type*>(static_cast<const Void*>(&(cell).body()))))
#define toBody(expr,type) (cellBody(toCell(expr),type))
#define toConstBody(expr,type) (constCellBody(toConstCell(expr),type))

#define exprCellInfo(expr) (*(static_cast<localConst CellInfo*>(toCell(expr).tag())))
#define constExprCellInfo(expr) (*(static_cast<const CellInfo*>(toConstCell(expr).tag())))


#define fun(expr) (fst(expr))
#define arg(expr) (snd(expr))

#define isPtr(expr) ((expr).tag()==PTR_TAG&&(expr)!=ERROR)
#define isValue(expr) ((expr).tag()==ENUM_VAL_TAG)

#define inRange(min,x,max) ((min)<=(x)&&(x)<=(max))

#define isTupleCon(expr) (isValue(expr)&&inRange(TUPLE_MIN,((expr)),TUPLE_MAX))
#define toTupleCon(expr) (((expr)>>TAG_BITS)-(Int)((UInt)TUPLE_MIN>>TAG_BITS))
#define fromTupleCon(tupleCon) ((Expr)(((tupleCon)<<TAG_BITS)+TUPLE_MIN))
#define tupleCard(expr) ((UInt)((static_cast<ExprEnumVal>(expr)-TUPLE_MIN)>>TAG_BITS)+2)

#define isName(expr) (isValue(expr)&&inRange(NAME_MIN,(expr),NAME_MAX))
#define toName(expr) ((localConst Name)((static_cast<ExprEnumVal>(expr)>>TAG_BITS)-(Int)\
                     ((UInt)NAME_ZERO>>TAG_BITS)))
#define fromName(name) ((localConst Expr)((((name)+(NAME_ZERO>>TAG_BITS))\
                       <<TAG_BITS)+ENUM_VAL_TAG))

#define isType(expr) (isValue(expr)&&inRange(TYPE_MIN,static_cast<ExprEnumVal>(expr),TYPE_MAX))
#define toType(expr) ((localConst Type)((static_cast<ExprEnumVal>(expr)-TYPE_MIN)>>TAG_BITS))
#define fromType(type) ((localConst Expr)(((type)<<TAG_BITS)+TYPE_MIN))

#define isChar(expr) (isValue(expr)&&inRange(CHAR_MIN,(expr),CHAR_MAX))
#define toChar(expr) ((localConst Char)((static_cast<ExprEnumVal>(expr)-CHAR_MIN)>>TAG_BITS))
#define fromChar(c) ((localConst Expr)(((c)<<TAG_BITS)+CHAR_MIN))

#define isTag(expr) (isValue(expr)&&inRange(TAG_MIN,(expr),TAG_MAX))
#define toTag(expr) ((localConst Type)((static_cast<ExprEnumVal>(expr)-TAG_MIN)>>TAG_BITS))
#define fromTag(tag) ((localConst Expr)(((tag)<<TAG_BITS)+TAG_MIN))

#define ivoryError(s) ::error(s)

#define ivoryError1(fmt,arg) error1((fmt),(arg))

#define ivoryTrace(s) ::trace(s)

#define ivoryTrace1(fmt,arg) trace1((fmt),(arg))

// Built-in name macros

#define builtInName(name) (builtInName$(#name))
#define builtInOperator(op) (builtInName$(op))

// Expression predicate

#define isAp(expr) (isPtr(expr)&&toCell(expr).tag()==Expr(AP))

// Function application macros

#define ap(fun,arg,msa) (mkAp((fun),(arg),(msa)))
#define ap2(fun,arg1,arg2,msa) (ap(ap((fun),(arg1),(msa)),(arg2),(msa)))
#define ap3(fun,arg1,arg2,arg3,msa) (ap(ap2((fun),(arg1),(arg2),(msa)),(arg3),\
                                    (msa)))
#define ap4(fun,arg1,arg2,arg3,arg4,msa) (ap(ap3((fun),(arg1),(arg2),(arg3),\
                                         (msa)),(arg4),(msa)))

#define builtInAp(fun,arg) (mkBuiltInAp((fun),(arg)))
#define builtInAp2(fun,arg1,arg2) (builtInAp(builtInAp((fun),(arg1)),(arg2)))
#define builtInAp3(fun,arg1,arg2,arg3) (builtInAp(builtInAp2((fun),(arg1),\
                                       (arg2)),(arg3)))
#define builtInAp4(fun,arg1,arg2,arg3,arg4) (builtInAp(builtInAp3((fun),(arg1),\
                                            (arg2),(arg3)),(arg4)))

// Tuple macros

#define fst(p) (((localConst Expr*)toCell(p).body())[0])
#define snd(p) (((localConst Expr*)toCell(p).body())[1])
#define constFst(p) (((const Expr*)toConstCell(p).body())[0])
#define constSnd(p) (((const Expr*)toConstCell(p).body())[1])



#define fst3(t) (((localConst Expr*)toCell(t).body())[0])
#define snd3(t) (((localConst Expr*)toCell(t).body())[1])
#define thd3(t) (((localConst Expr*)toCell(t).body())[2])

extern MSA& builtInMSA(Void);

extern Name builtInName$(const char* ident);

extern Cell* mkBuiltInAp(Expr fun, Expr arg);

extern Int builtInNameCount;

extern Cell* copyCellId(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM);

extern Void destroyCell(Cell& cell_, Env& env, MSA& msa argN_VM);

extern Cell* mkAp(Expr fun, Expr arg, MSA& msa);

extern Cell* mkPair(Expr fst, Expr snd, MSA& msa);

extern Cell* mkTriple(Expr fst3, Expr snd3, Expr thd3, MSA& msa);

extern Cell* mkTuple(UInt degree, const Expr* src, Expr tag, MSA& msa);

#endif /* IVORY_COMMON_H_DEFINED */
