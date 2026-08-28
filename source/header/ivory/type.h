/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    type.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory types.
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
 * Note:
 *
 *    The type methods are currently implemented as callbacks rather than
 *    virtual functions.  The original reason for this was to simplify
 *    porting to a C only execution platform.
 *
 */

#ifndef IVORY_TYPE_H_DEFINED
#define IVORY_TYPE_H_DEFINED

#include "archive.h"
#include "nameTable.h"
#include "../segmentTable.h"
#include "ivory/builtInFn.h"
#include "ivory/common.h"
#include "ivory/dataCon.h"
#include "ivory/repr.h"

// Set TYPE_CHECKS to 0 or 1 as required

#define TYPE_CHECKS 1

#define NULL_TYPE -1

// Packed type signature and hash codes

#define TYPE_SIG_MAX_TUPLE_DEGREE 8

#define TYPE_SIG_UNKNOWN       0
#define TYPE_SIG_AP            (TYPE_SIG_UNKNOWN+1)
#define TYPE_SIG_ARROW         (TYPE_SIG_AP+1)
#define TYPE_SIG_TYPE_VAR      (TYPE_SIG_ARROW+1)
#define TYPE_SIG_VOID          (TYPE_SIG_TYPE_VAR+1)
#define TYPE_SIG_EXP           (TYPE_SIG_VOID+1)
#define TYPE_SIG_EXPR          (TYPE_SIG_EXP+1)
#define TYPE_SIG_NAME          (TYPE_SIG_EXPR+1)
#define TYPE_SIG_TYPE          (TYPE_SIG_NAME+1)
#define TYPE_SIG_BYTE          (TYPE_SIG_TYPE+1)
#define TYPE_SIG_BITS          (TYPE_SIG_BYTE+1)
#define TYPE_SIG_INT           (TYPE_SIG_BITS+1)
#define TYPE_SIG_FLOAT         (TYPE_SIG_INT+1)
#define TYPE_SIG_DOUBLE        (TYPE_SIG_FLOAT+1)
#define TYPE_SIG_CHAR          (TYPE_SIG_DOUBLE+1)
#define TYPE_SIG_STRING        (TYPE_SIG_CHAR+1)
#define TYPE_SIG_REF           (TYPE_SIG_STRING+1)
#define TYPE_SIG_TUPLE         (TYPE_SIG_REF+1)
#define TYPE_SIG_TUPLE_MIN     (TYPE_SIG_TUPLE+1)
#define TYPE_SIG_TUPLE_MAX     (TYPE_SIG_TUPLE_MIN+TYPE_SIG_MAX_TUPLE_DEGREE)
#define TYPE_SIG_PLAIN         (TYPE_SIG_TUPLE_MAX+1)
#define TYPE_SIG_PTR           (TYPE_SIG_PLAIN+1)
#define TYPE_SIG_ARRAY         (TYPE_SIG_PTR+1)
#define TYPE_SIG_LIST          (TYPE_SIG_ARRAY+1)
#define TYPE_SIG_ANY           (TYPE_SIG_LIST+1)
#define TYPE_SIG_TYPE_CON      (TYPE_SIG_ANY+1)
#define TYPE_SIG_TYPE_CON_MIN  (TYPE_SIG_TYPE_CON+1)
#define TYPE_SIG_TYPE_CON_MAX  63

#define sizeOfType (sizeof(Type))

#define typeSpecType Type

// Type constructor
//
// Note: 'nameTable' is required only to map from name to string

class TypeCon {
public:
   TypeCon(Name name, UInt hashVal, const NameTable& nameTable,
           Expr (*mkTypeVars)(Void),
           Bool objectFlag = FALSE, Bool isDynamic = FALSE);
   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {
#else
   inline Void operator delete(Void* ptr) {
#endif
   }
public:
   inline Name name(Void) const { return _name; }
   inline UInt hashVal(Void) const { return _hashVal; }
   inline const NameTable& nameTable(Void) const { return _nameTable; }
   inline Bool isDynamic(Void) const { return _isDynamic; }
   inline UInt n(Void) const { return _n; }
   inline Bool isSumType(Void) const { return _n > 1; }

   Void addDataCon(DataCon* dataCon);

protected:
   Void init(Void);

public:
   Name              _name;
   UInt              _hashVal;
   const NameTable&  _nameTable;
   Expr              (*_mkTypeVars)(Void);
   Expr              _typeVars;
   Bool              _objectFlag;
   Bool              _isDynamic;
   UInt              _n;
   DataCon*          _dataCons;     

   TypeCon*          _next;               // next in built-in list

   static TypeCon*   builtInList;      // built-in type constructor list
   static Void initBuiltInList(Void);

   friend class DataCon;

   friend UInt hashTypeSig(TypeSig typeSig);
   friend Void printExpr(Expr expr, ostream& os, const Env& env);
   friend Void postConstructorInit(Void);
};

// Abstract type descriptor
 
class TypeDescr;
struct InputStream_Byte;
struct OutputStream_Byte;
struct OutputStream_Char;

typedef Void      (*TypeMethod_initFn)(TypeDescr* typeDescr);
typedef Ptr       (*TypeMethod_mapFn)(TypeDescr* typeDescr, Ptr ptr, const Env& ptrEnv, Env& dstEnv argN_VM);
typedef Void      (*TypeMethod_assignFn)(TypeDescr* typeDescr, Ptr src, const Env& srcEnv,
                                                               Ptr dst, Env& dstEnv argN_VM);
typedef Void      (*TypeMethod_evalFn)(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM);

typedef Void(*TypeMethod_insertTxtFn)(TypeDescr* typeDescr, OutputStream_Char* os,
                                      const Env& osEnv, Ptr ptr,
                                      const Env& env argN_VM);

#if (SERIALISATION==1)
typedef Ptr       (*TypeMethod_extractBinFn)(TypeDescr* typeDescr, InputStream_Byte* is, const Env& isEnv, Env& dstEnv argN_VM);
typedef Void      (*TypeMethod_insertBinFn)(TypeDescr* typeDescr, OutputStream_Byte* os, const Env& osEnv, Ptr ptr, Env& env argN_VM);
#endif

#if (GARBAGE_COLLECTION==1)
typedef Void(*TypeMethod_markFn)(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM);
#endif

// TypeDescr: A descriptor for a type instance

class TypeDescr {
public:
   enum Kind { TYPE_DESCR_BUILT_IN, TYPE_DESCR_BYTE_CODE };
public:
   TypeDescr(TypeSig (*mkTypeSig)(Void), TypeMethod_initFn initFn); 
   TypeDescr(TypeSig typeSig); 

   inline Void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline size_t size(Void) const { return _size; }
   inline TypeSig typeSig(Void) const { return _typeSig; }
   inline Type type(Void) const { return _type; }
   inline Type altType(Void) const { return _altType; }
   inline Type denotedType(Void) const { return _denotedType; }
   inline UInt hashVal(Void) const { return _hashVal; }
   inline Repr repr(Void) const { return _repr; }
   inline Void setRepr(Repr val) { _repr = val; }
   inline Bool needsEnv(Void) const { return _needsEnv; }
   inline Void setNeedsEnv(Bool q) { _needsEnv = q; }

   inline Void setAltType(Type altType) { _altType= altType; }
   inline Void setDenotedType(Type denotedType) { _denotedType = denotedType; }

   Void init(Void);

   virtual Kind kind(Void) const;
   virtual Void postLoad(const SegmentTable& segmentTable);
   virtual Void store(Archive& archive, const NameTable& nameTable) const;

public:

   class Loader {
   public:
      Loader(TypeDescr::Kind kind);

      virtual TypeDescr* load(Archive& archive,
                              const NameTable& nameTable,
                              MSA& msa) const;

   protected:
      static Loader*& head() {
         static Loader* _head = 0;
         return _head;
      }
   public:
      static const Loader* loader(TypeDescr::Kind kind);

   protected:
      Loader* _next;
      Kind    _kind;
   };

public:
   TypeMethod_initFn          _initFn;
   size_t                     _size;
   TypeMethod_mapFn           _mapFn;
   TypeMethod_assignFn        _assignFn;
   TypeMethod_evalFn          _evalFn;

   TypeMethod_insertTxtFn     _insertTxtFn;

#if (SERIALISATION==1)
   TypeMethod_extractBinFn    _extractBinFn;
   TypeMethod_insertBinFn     _insertBinFn;
#endif

#if (GARBAGE_COLLECTION==1)   
   TypeMethod_markFn          _markFn;
#endif

protected:
   TypeDescr*     _next;               // Next in list
   TypeDescr*     _prev;               // Previous in list
   TypeSig      (*_mkTypeSig)(Void);   // Post constructor initialisation callback
   TypeSig        _typeSig;            // Type signature
   Type           _type;               // Type
   Type           _denotedType;        // Fully evaluated type
   Type           _altType;            // Alternative type
   UInt           _hashVal;            // Hash value
   Repr           _repr;               // Representation
   Bool           _needsEnv;           // FALSE if environment independent, FALSE otherwise 
   UInt           _refCount;           // Reference count

   static Void initGlobalList(Void);

   friend class TypeTable;
   friend Type mkType(TypeSig (*mkTypeSig)(Void), TypeMethod_initFn initFn);
   friend Type mkType(TypeSig TypeSig);
   friend Void postConstructorInit(Void);
};

// Type signature predicates

#define isTypeCon(typeSig) ((typeSig).tag()==PTR_TAG&&\
(ExprEnumVal)(toCell(typeSig).tag())==TYPE_CON)

#define isMonadicArrowTypeSig(typeSig) (isAp(typeSig)&&\
formOf(fun(typeSig))==ARROW)

#define isDyadicArrowTypeSig(typeSig) (isAp(typeSig)&&\
isAp(fun(typeSig))&&\
formOf(fun(fun(typeSig)))==ARROW)

#define isArrowTypeSig(typeSig) (isAp(typeSig)&&\
(formOf(fun(typeSig))==ARROW||\
isAp(fun(typeSig))&&\
formOf(fun(fun(typeSig)))==ARROW))

#define isTypeSigEnv(typeSig) (isPtr(typeSig)&&toCell(typeSig)._tag==TYPE_SIG_ENV)

#define declareBuiltInTypeCon(T) extern TypeSig typeCon$##T
#define declareTypeCon(T) declareBuiltInTypeCon(T)

#define defineBuiltInTypeCon(T) TypeSig typeCon$##T=mkTypeCon(#T)
#define defineTypeCon(T) defineBuiltInTypeCon(T)

#define defineBuiltInParamTypeCon(T,mkTypeVars) TypeSig typeCon$##T=mkTypeCon(#T,mkTypeVars)
#define defineParamTypeCon(T,mkTypeVars) defineBuiltInParamTypeCon(T,mkTypeVars)

#define defineBuiltInObjectCon(T) TypeSig typeCon$##T=mkTypeCon(#T,NULL,TRUE)
#define defineObjectCon(T) defineBuiltInObjectCon(T)

#define builtInTypeCon(T) typeCon$##T
#define typeCon(T) typeCon$##T

#define declareBuiltInType(T) extern Type type$##T
#define declareType(T) declareBuiltInType(T)

#define defineBuiltInType_n(n,T,TS,initFn)\
static TypeSig mkTypeSig$##n(Void){return (TS);}\
Type type$##T=mkType(mkTypeSig$##n,initFn)

#define defineType_n(n,T,TS,initFn) defineBuiltInType_n(n,T,TS,initFn)

#define defineBuiltInType(T,TS,initFn) defineBuiltInType_n(1,T,TS,initFn)

#define defineType(T,TS,initFn) defineType_n(1,T,TS,initFn)

#define builtInType(T) type$##T

#define declareNullaryType(T)\
declareTypeCon(T);\
declareType(T);\
extern CellInfo* cellInfoTable$##T[];

#define sizeOfType (sizeof(Type))

#define typeSpecType Type

#define stackFPSizeType (stackSlotSize(Type)+stackSlotSize(Env))

#define loadArgRegType(type)\
Type type=rType;\
Env* type##Env=rEnv

#define loadArgType(type,off)\
Env* type##Env=stack(off+stackSlotSize(Type),Env*);\
Type type=stack((off),Type)

#define storeArgType(type, off) stack(off, Type)=type

#define argRegFromType(s,c)\
rType=cellBody(*cell,s).c;\
rEnv=cellEnv

#define argRegFromCellType(s,c)\
rType=cellBody(*cell,s).c;\
rEnv=cellEnv

#define map_Type(type,srcEnv,dstEnv) mapType$(type,*(srcEnv),*(dstEnv))

#define returnType(type,env){\
rType = type;\
rEnv = env;\
jump(popLabel());}

#if (RUN_TIME_TYPE_CHECKS==1)
#define checkType(t)\
if(rType!=builtInType(t)){throw(RunTimeError("expected " #t));}
#else
#define checkType(t)
#endif

declareTypeCon(Expr); // Exp *

declareTypeCon(Exp); // Exp t

declareTypeCon(Type);

declareType(Type);

declareTypeCon(Array);

declareTypeCon(Vector);

declareBuiltInFn(extractBinType);

declareBuiltInFn(insertBinType);

declareBuiltInFn(showType);

declareBuiltInFn(typeString);

declareBuiltInFn(insertTxtType);

extern Type builtInTypeCount;

extern Void addTypeConNames(TypeSig typeSig, NameTable& nameTable);

extern Type argType(Type type, UInt i, TypeTable& typeTable,
                                       NameTable& nameTable);

extern TypeSig argTypeSig(TypeSig typeSig, UInt i = 1);

extern UInt arity(TypeSig typeSig);

extern TypeSig canonicalTypeSig(TypeSig typeSig, NameTable& nameTable, MSA& msa);

extern TypeSig copyTypeSig(TypeSig typeSig, MSA& msa);

extern Void destroyTypeSig(TypeSig typeSig, MSA& msa);

Bool eqType_(Type type1, const TypeTable& typeTable1,
             Type type2, const TypeTable& typeTable2);

extern Bool eqTypeSig(const TypeSig typeSig1, const TypeSig typeSig2);

extern Bool eqTypeSigs(Expr typeSigs1, Expr typeSigs2);

extern UInt hashTypeSig(TypeSig typeSig);

extern Void hashTypeSig_(TypeSig typeSig, const Env& env, HashDJB2& hashDJB2);

extern Bool isSumType(Type type, const TypeTable& typeTable);

extern Bool isSumTypeSig(TypeSig typeSig);

extern TypeSig loadTypeSig(Archive& archive, const NameTable& nameTable, MSA& msa);

extern Type mkType(TypeSig (*mkTypeSig)(Void), TypeMethod_initFn initFn);

extern TypeSig mkTypeCon(const char* ident, Expr (*mkTypeVars)(Void) = NULL,
                         Bool isObject = FALSE);

extern TypeSig resultTypeSig(TypeSig typeSig, UInt nArgs = 1);

extern Type resultType(Type type, UInt nArgs, TypeTable& typeTable,
                                              NameTable& nameTable);

extern Void storeTypeSig(Archive& archivechive, TypeSig typeSig,
                         const NameTable& nameTable);

extern TypeDescr& typeDescriptor(Type type, const TypeTable& typeTable);

extern UInt typeHashVal(Type type, const TypeTable& typeTable);

extern TypeSig typeSignature(Type type, const TypeTable& typeTable);

extern TypeSig unpackTypeSig(unsigned char*& buffer, const Name nameV[],
                             NameTable& nameTable, MSA& msa);

extern Type useBuiltInType(TypeSig typeSig);

extern Type useType(TypeSig typeSig, TypeTable& typeTable, NameTable& nameTable);

extern Tag constructFromName(Name name, const Env& nameEnv,
                             Type type, const Env& typeEnv);
extern Tag constructFromString(String str, Type type, const Env& typeEnv);

#endif /* IVORY_TYPE_H_DEFINED */
