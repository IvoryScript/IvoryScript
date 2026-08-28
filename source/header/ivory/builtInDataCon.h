/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    builtInDataCon.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common definitions for use with Ivory built in data constructors.
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

#ifndef IVORY_BUILT_IN_DATA_CON_H_DEFINED
#define IVORY_BUILT_IN_DATA_CON_H_DEFINED

#include "ivory/dataCon.h"
#include "ivory/machine.h"

// Built-in Data Constructor

class BuiltInDataCon : public DataCon {
public:
   BuiltInDataCon(Name name, Tag tag, TypeSig (*mkTypeSig)(Void), TypeSig (*mkReprTypeSig)(Void));

   inline Void* operator new(size_t size, Void* ptr) { return ptr; }
   inline Void* operator new(size_t size, MSA& msa) { Void *ptr = msa.alloc(size); return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
   inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
   inline Void operator delete(Void* ptr) {}
#endif

   Void init(Void);

protected:
   TypeSig(*_mkTypeSig)(Void);
   TypeSig(*_mkReprTypeSig)(Void);

   BuiltInDataCon*         _next;         // next in global list
   static BuiltInDataCon*  globalList;    // global built in function list

   static Void initGlobalList(Void);

   friend Void postConstructorInit(Void);
};

// Built-in data constructor macros

#define declareBuiltInDataCon(C) extern Expr dataCon$##C;\
declareEntry(C);declareAltEntry(C)

#define declareDataCon(C) declareBuiltInDataCon(C)

#define builtInDataCon(C) dataCon$##C
#define dataCon(C) builtInDataCon(C)

/*
 * CellInfo helpers for built-in data constructors
 */

#if (SERIALISATION==1) && (GARBAGE_COLLECTION==1)

#define defineBuiltInDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0,\
                      entry(C),altEntry(C),\
                      copyCellId,\
                      NULL,NULL,\
                      NULL,\
                      NULL,NULL)

#define defineBuiltInNullaryDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0,\
                      altEntry(C),\
                      cellCopyFn$##C,\
                      NULL,NULL,\
                      NULL,\
                      NULL,cellShowFn$##C)

#elif (SERIALISATION==1) && (GARBAGE_COLLECTION==0)

#define defineBuiltInDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0\
                      entry(C),altEntry(C),\
                      copyCellId,\
                      NULL,NULL,\
                      NULL,NULL)

#define defineBuiltInNullaryDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0,\
                      altEntry(C),\
                      cellCopyFn$##C,\
                      NULL,NULL,\
                      NULL,cellShowFn$##C)

#elif (SERIALISATION==0) && (GARBAGE_COLLECTION==1)

#define defineBuiltInDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0\
                      entry(C),altEntry(C),\
                      copyCellId,\
                      NULL,\
                      NULL,NULL)

#define defineBuiltInNullaryDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0,\
                      altEntry(C),\
                      cellCopyFn$##C,\
                      NULL,\
                      NULL,cellShowFn$##C)

#else

#define defineBuiltInDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0,\
                      entry(C),altEntry(C),\
                      copyCellId,\
                      NULL,NULL)

#define defineBuiltInNullaryDataConCellInfo(C)\
CellInfo cellInfo$##C(BUILT_IN_DATA_CON,0,\
                      altEntry(C),\
                      cellCopyFn$##C,\
                      NULL,cellShowFn$##C)

#endif

#define defineBuiltInDataCon(C,i,TS,reprTS)\
static TypeSig mkTypeSig$##C(Void){return (TS);}\
static TypeSig mkReprTypeSig$##C(Void){return (reprTS);}\
defineBuiltInDataConCellInfo(C);\
Expr dataCon$##C = mkBuiltInDataCon(#C,i,mkTypeSig$##C,mkReprTypeSig$##C,cellInfo$##C);\
defineEntry(C)

#define defineDataCon(C,i,TS,reprTS) defineBuiltInDataCon(C,i,TS,reprTS)

 // Nullary data constructor macros

#define declareBuiltInNullaryDataCon(C) extern Expr dataCon$##C;\
declareAltEntry(C)

#define declareNullaryDataCon(C) declareBuiltInNullaryDataCon(C)

// Argument check removed
//checkArgs(0,0,#C);

#define defineBuiltInNullaryDataCon(C,tag,T)\
static TypeSig mkTypeSig$##C(Void){return typeCon(T);}\
defineAltEntry(C){\
   rTag = tag;\
   rType = builtInType(T);\
   jump(popLabel());\
}\
static Cell* cellCopyFn$##C(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM){\
   return builtInDataCon(C);\
}\
static Void cellShowFn$##C(const Cell& cell_,ostream& os,const Env& env argN_VM){\
   os << #C;\
}\
defineBuiltInNullaryDataConCellInfo(C);\
Expr dataCon$##C = mkBuiltInDataCon(#C,tag,mkTypeSig$##C,NULL,cellInfo$##C)

#define defineNullaryDataCon(C,tag,T) defineBuiltInNullaryDataCon(C,tag,T)

extern Expr mkBuiltInDataCon(const char* ident, Tag tag,
                             TypeSig (*mkTypeSig)(Void), TypeSig (*mkReprTypeSig)(Void),
                             const struct CellInfo& cellInfo);

#endif /* IVORY_BUILT_IN_DATA_CON_H_DEFINED */
