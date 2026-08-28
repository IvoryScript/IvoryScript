/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built-in type table functions.
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
 *
 *    02/07/03 -  AJS   Now supports new types after C++ constructor phase.
 *
 * Notes:
 *
 */

#include <string.h>
#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/name.h"
#include "ivory/list.h"
#include "ivory/type.h"
#include "ivory/typeTable.h"

//#define DEBUG_INIT

#ifdef DEBUG_INIT
   #include "ivory/trace.h"
#endif

#define BUILT_IN_TYPE_TABLE_N_SLOTS     199
#define BUILT_IN_TYPE_TABLE_N_PER_SEG   64

defineBuiltInTypeCon(Type);

declareBuiltInTypeCon(Any);

static Void init$Type(TypeDescr* typeDescr);
defineType(Type, typeCon(Type), init$Type);

declareLabel(enterType);

defineLabel(enterType) {
   returnType(toBody(cell, Type), cellEnv);
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

static Cell* cellCopyFn(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return &src;
}

#undef localConst
#define localConst

CellInfo info_Type(fromType(builtInType(Type)), 0,
                   label(enterType),
                   cellCopyFn, NULL, NULL);

/*----------------------------------------------------------------------------*/

Ptr mapFn$Type(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM) {
   Type* ptrRes = (Type*)dstEnv.msa().alloc(sizeof(Type));
   *ptrRes = mapType(*((Type*)ptr), ptrEnv, dstEnv);
   return ptrRes;
}

Void insertTxtFn$Type(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(static_cast<Env*>((Void*)&env), Env*);
   stackAlloc(4);
   stack(0, Type) = *static_cast<Type*>(ptr);
   callSequence$(altEntry(insertTxtType) n_vm);
}

#if (SERIALISATION==1)
Ptr extractBinFn$Type(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM) {
   rPtr = is;
   rEnv = static_cast<Env*>((Void*)&isEnv);
   pushLabel(NULL);
   callSequence$(altEntry(extractBinType) n_vm);
   Type* ptrRes = (Type*)dstEnv.msa().alloc(sizeof(Type));
   *ptrRes = rType;
   return ptrRes;
}
#endif

#if (SERIALISATION==1)
Void insertBinFn$Type(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(static_cast<Env*>((Void*)&env), Env*);
   stackAlloc(4);
   stack(0, Type) = *static_cast<Type*>(ptr);
   callSequence$(altEntry(insertBinType) n_vm);
}
#endif

Void evalFn$Type(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rType = *static_cast<Type*>(ptr);
   rEnv = &env;
}

Void init$Type(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Type);
   typeDescr->_mapFn = mapFn$Type;
   typeDescr->_evalFn = evalFn$Type;

   typeDescr->_insertTxtFn = insertTxtFn$Type;

#if (SERIALISATION==1)
   typeDescr->_extractBinFn = extractBinFn$Type;
   typeDescr->_insertBinFn = insertBinFn$Type;
#endif
}

/*----------------------------------------------------------------------------*/

static Void evalFn(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   Repr repr = typeDescr->repr();
   switch (repr.tag()) {
      case Repr::REPR_VOID:
         break;
      case Repr::REPR_CELL_PTR:
         rCell = *static_cast<Cell**>(ptr);
         break;
      case Repr::REPR_ENV_PTR:
         rEnv = *static_cast<Env**>(ptr);
         break;
      case Repr::REPR_TYPE:
         rType = *static_cast<Type*>(ptr);
         break;
      case Repr::REPR_NAME:
         rName = *static_cast<Name*>(ptr);
         break;
      case Repr::REPR_EXPR:
         rExpr = *static_cast<Expr*>(ptr);
         break;
      case Repr::REPR_BYTE:
         rByte = *static_cast<Byte*>(ptr);
         break;
      case Repr::REPR_BITS:
         rBits = *static_cast<Bits*>(ptr);
         break;
      case Repr::REPR_INT:
         rInt = *static_cast<Int*>(ptr);
         break;
      case Repr::REPR_FLOAT:
         rFloat = *static_cast<Float*>(ptr);
         break;
      case Repr::REPR_DOUBLE:
         rDouble = *static_cast<Double*>(ptr);
         break;
      case Repr::REPR_TAG:
         rTag = *static_cast<Tag*>(ptr);
         break;
      case Repr::REPR_CHAR:
         rChar = *static_cast<Char*>(ptr);
         break;
      case Repr::REPR_REF:
         rRef = *static_cast<Ref*>(ptr);
         break;
      case Repr::REPR_UTC:
         rUTC = *static_cast<UTC*>(ptr);
         break;
      case Repr::REPR_CELL_INFO_PTR:
      case Repr::REPR_DIR:
      case Repr::REPR_DIR_ENTRY:
      case Repr::REPR_SERIAL_CONTEXT:
      case Repr::REPR_STRING:
      case Repr::REPR_PTR:
         rPtr = *static_cast<Ptr*>(ptr);
         break;
      case Repr::REPR_FILE_HANDLE:
      case Repr::REPR_STRUCT: {
         size_t size = sizeOfRepr(repr);
         if (size > rStructSize) {
            consMSA->free(rStruct);
            rStruct = static_cast<Byte*>(consMSA->alloc(size));
            rStructSize = size;
         }
         memcpy(rStruct, ptr, size);
         break;
      }
      default:
         throw(RunTimeError("evalFn: unsupported representation"));
   }

   if (typeDescr->needsEnv())
      rEnv = &env;
}

TypeDescr::TypeDescr(TypeSig (*mkTypeSig)(Void), TypeMethod_initFn initFn)
 : _initFn(initFn),
   _size(0),
   _mapFn(NULL),
   _assignFn(NULL),
   _evalFn(evalFn),
   _insertTxtFn(NULL),

#if (SERIALISATION==1)
   _extractBinFn(NULL),
   _insertBinFn(NULL),
#endif

#if (GARBAGE_COLLECTION==1)
   _markFn(NULL),
#endif

   _next(NULL), _prev(NULL), 
   _mkTypeSig(mkTypeSig), _typeSig(ERROR),
   _type(NULL_TYPE), _denotedType(NULL_TYPE), _altType(NULL_TYPE),
   _hashVal(0),
   _repr(), _needsEnv(FALSE),
   _refCount(0) {
}

TypeDescr::TypeDescr(TypeSig typeSig)
   : _initFn(NULL),
   _size(0),
   _mapFn(NULL),
   _assignFn(NULL),
   _evalFn(evalFn),
   _insertTxtFn(NULL),

#if (SERIALISATION==1)
   _extractBinFn(NULL),
   _insertBinFn(NULL),
#endif

#if (GARBAGE_COLLECTION==1)
   _markFn(NULL),
#endif

   _next(NULL), _prev(NULL),
   _mkTypeSig(NULL), _typeSig(typeSig),
   _type(NULL_TYPE), _denotedType(NULL_TYPE), _altType(NULL_TYPE),
   _hashVal(0),
   _repr(), _needsEnv(FALSE),
   _refCount(0) {
}

// Functions to make a type descriptor

// 1. before 'main'

Type mkType(TypeSig (*mkTypeSig)(Void), TypeMethod_initFn initFn) {
   return builtInTypeTable().addDescr(new(builtInMSA()) TypeDescr(mkTypeSig, initFn));
}

// 2. after 'main'

Type mkType(TypeSig typeSig) {
   return useBuiltInType(typeSig);
 }

// Initialisation 

Void TypeDescr::init() {
   _typeSig = _mkTypeSig();
   _denotedType = _type;
    if (_initFn != NULL)
      _initFn(this);

#ifdef DEBUG_INIT
   outStream << "TypeDescr::init: ";
   printExpr(_typeSig, outStream, Env(builtInNameTable()));
   outStream << '\n';
   outStream.flush();
#endif
}

Void TypeDescr::initGlobalList(Void) {
   for (UInt i = 0; i < builtInTypeTable()._count; i++) {
      TypeDescr* descr = builtInTypeTable().typeDescr(i);
      descr->init();
      builtInTypeTable().enterDescr(descr);
   }
}

enum Kind { TYPE_DESCR_BUILT_IN, TYPE_DESCR_BYTE_CODE };


TypeDescr::Kind TypeDescr::kind(Void) const {
   return TYPE_DESCR_BUILT_IN;
}

Void TypeDescr::postLoad(const SegmentTable& segmentTable) {
}

Void TypeDescr::store(Archive& archive, const NameTable& nameTable) const {
   storeTypeSig(archive, _typeSig, nameTable);
   archive << (UInt32)_hashVal;
   archive << (UInt32)_denotedType;
}

TypeDescr::Loader::Loader(TypeDescr::Kind kind)
   : _next(NULL), _kind(kind) {
   _next = head();
   head() = this;
}

const TypeDescr::Loader* TypeDescr::Loader::loader(TypeDescr::Kind kind) {
   TypeDescr::Loader* loader = head();
   while (loader != NULL) {
      if (loader->_kind == kind)
         return loader;
      loader = loader->_next;
   }
   return NULL;
}

TypeDescr* TypeDescr::Loader::load(Archive& archive,
                                   const NameTable& nameTable,
                                   MSA& msa) const {
   TypeSig typeSig = loadTypeSig(archive, nameTable, msa);
   UInt32 hashVal;
   archive >> hashVal;
   UInt32 denotedType;
   archive >> denotedType;
   if (builtInTypeTable().lookUp(typeSig, hashVal) == NULL)
      error("Missing built-in type");
   TypeDescr* descr = new(msa) TypeDescr(typeSig);
   descr->_hashVal = hashVal;
   descr->_denotedType = (Type)denotedType;
   return descr;
}

static TypeDescr::Loader loader(TypeDescr::TYPE_DESCR_BUILT_IN);
      
// built-in type table

static TypeTable* _builtInTypeTable = NULL;

Type builtInTypeCount = 0;

TypeTable& builtInTypeTable() {
   if (_builtInTypeTable == NULL)
      _builtInTypeTable = new (builtInMSA()) TypeTable(BUILT_IN_TYPE_TABLE_N_SLOTS,
                                           BUILT_IN_TYPE_TABLE_N_PER_SEG,
                                           builtInMSA());
   return *_builtInTypeTable;
}

// Enter or look up type in built-in type table

Type useBuiltInType(TypeSig typeSig) {
   UInt hashVal = hashTypeSig(typeSig);
   TypeDescr* typeDescr = builtInTypeTable().lookUp(typeSig,
                                                   hashVal);
   if (typeDescr == NULL)
      typeDescr = builtInTypeTable().addType(typeSig, hashVal);
   if (typeDescr->denotedType() == NULL_TYPE)
      typeDescr->setDenotedType(typeDescr->type());
   return typeDescr->type();
}

// Test whether a given type is built-in

Bool isbuiltInType(TypeSig ts) {
   return builtInTypeTable().lookUp(ts) != NULL_TYPE;
}

// Extend type table for a given type signature
// Adding alternate if built-in

Type useType(TypeSig typeSig, TypeTable& typeTable, NameTable& nameTable) {
   UInt hashVal = hashTypeSig(typeSig);
   TypeDescr* typeDescr = typeTable.lookUp(typeSig, hashVal);
   if (typeDescr == NULL) {
      typeDescr = typeTable.addType(typeSig, hashVal);
      TypeDescr* builtInTypeDescr = builtInTypeTable().lookUp(typeSig, hashVal);
      if (builtInTypeDescr != NULL)
         typeDescr->setAltType(builtInTypeDescr->type());
      addTypeConNames(typeSig, nameTable);
   }

   return typeDescr->altType() != NULL_TYPE
      ? typeDescr->altType()
      : builtInTypeCount + typeDescr->type();
}

Tag constructFromName(Name name, const Env& nameEnv,
                      Type type, const Env& typeEnv) {
   TypeDescr* typeDescr = type < builtInTypeCount
      ? builtInTypeTable().typeDescr(type)
      : typeEnv.typeTable().typeDescr(type - builtInTypeCount);
   TypeSig hfTypeSig = headForm(typeDescr->typeSig());

   if (formOf(hfTypeSig) != TYPE_CON)
      error("constructFromName: expected a type constructor");

   TypeCon& typeCon = toBody(hfTypeSig, TypeCon);
   for (DataCon* dataCon = typeCon._dataCons;
        dataCon != NULL;
        dataCon = dataCon->next()) {
      if (dataCon->arity() == 0 &&
          eqName_(name, nameEnv.nameTable(),
                  dataCon->name(), typeCon.nameTable()))
         return dataCon->tag();
   }

   error("constructFromName: unknown constructor name");
   return 0;
}

Tag constructFromString(String str, Type type, const Env& typeEnv) {
   TypeDescr* typeDescr = type < builtInTypeCount
      ? builtInTypeTable().typeDescr(type)
      : typeEnv.typeTable().typeDescr(type - builtInTypeCount);
   TypeSig hfTypeSig = headForm(typeDescr->typeSig());

   if (formOf(hfTypeSig) != TYPE_CON)
      error("constructFromString: expected a type constructor");

   TypeCon& typeCon = toBody(hfTypeSig, TypeCon);
   for (DataCon* dataCon = typeCon._dataCons;
        dataCon != NULL;
        dataCon = dataCon->next()) {
      if (dataCon->arity() == 0 &&
          strcmp(str, nameString_(dataCon->name(), typeCon.nameTable())) == 0)
         return dataCon->tag();
   }

   error("constructFromString: unknown constructor name");
   return 0;
}

// Return the type descriptor for a given type

TypeDescr& typeDescriptor(Type type, const TypeTable& typeTable) {
   return type < builtInTypeCount ? *builtInTypeTable().typeDescr(type)
                                  : *typeTable.typeDescr(type - builtInTypeCount);
}

// Return the signature for a given type

TypeSig typeSignature(Type type, const TypeTable& typeTable) {
   return type < builtInTypeCount ? builtInTypeTable().typeSig(type)
                                  : typeTable.typeSig(type - builtInTypeCount);
}

// Return the hash value of a given type

UInt typeHashVal(Type type, const TypeTable& typeTable) {
   return type < builtInTypeCount
      ? builtInTypeTable().hashVal(type)
      : typeTable.hashVal(type - builtInTypeCount);
}

// Compare two types

Bool eqType_(Type type1, const TypeTable& typeTable1,
             Type type2, const TypeTable& typeTable2) {
   return type1 < builtInTypeCount || type2 < builtInTypeCount ||
          &typeTable1 == &typeTable2
      ? type1 == type2
      : eqTypeSig(type1 < builtInTypeCount ? builtInTypeTable().typeSig(type1)
                                           : typeTable1.typeSig(type1 - builtInTypeCount),
                  type2 < builtInTypeCount ? builtInTypeTable().typeSig(type2)
                                           : typeTable2.typeSig(type2 - builtInTypeCount));
}                                          

// Compare two type signatures

// Assumes type variables share a common environment
// (guaranteed when compiling) and all run-time types are concrete (no type variables)

#ifdef localConst
#undef localConst
#endif
#define localConst const

Bool eqTypeSig(const TypeSig typeSig1, const TypeSig typeSig2) {
   return typeSig1 == typeSig2 ||
      isAp(typeSig1) && isAp(typeSig2) &&
      eqTypeSig(fun(typeSig1), fun(typeSig2)) &&
      eqTypeSig(arg(typeSig1), arg(typeSig2));
}

// eqTypeSigs: Compare two lists of type signatures

Bool eqTypeSigs(Expr typeSigs1, Expr typeSigs2) {
   while (typeSigs1 != Nil && typeSigs2 != Nil) {
      if (!eqTypeSig(hd(typeSigs1), hd(typeSigs2)))
         return FALSE;
      typeSigs1 = tl(typeSigs1);
      typeSigs2 = tl(typeSigs2);
   }
   return typeSigs1 == Nil && typeSigs2 == Nil;
}

#undef localConst
#define localConst

// Unconditionally copy a type signature

TypeSig copyTypeSig(TypeSig typeSig, MSA& msa) {
   switch (formOf(typeSig)) {
      case AP:
         return ap(copyTypeSig(fun(typeSig), msa),
                   copyTypeSig(arg(typeSig), msa), msa);

      default:
         return typeSig;
   }
}


// Add type constructor names

Void addTypeConNames(TypeSig typeSig, NameTable& nameTable) {
   switch (formOf(typeSig)) {
      case AP:
         addTypeConNames(fun(typeSig), nameTable);
         addTypeConNames(arg(typeSig), nameTable);
         break;

      case TYPE_CON: {
         TypeCon& typeCon = toBody(typeSig, TypeCon);
         mapName(typeCon.name(), typeCon.nameTable(), nameTable);
         break;
      }
      
      default:
         break;
   }
}

