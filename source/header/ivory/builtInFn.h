/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    builtInFn.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common definitions for use with Ivory built-in functions.
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

#ifndef IVORY_BUILT_IN_FN_H_DEFINED
#define IVORY_BUILT_IN_FN_H_DEFINED

#include "hashTable.h"
#include "ivory/builtIn.h"
#include "ivory/common.h"

// Built-in function

class BuiltInFn {
public:
   BuiltInFn(Name name, TypeSig (*mkTypeSig)(Void));
   BuiltInFn(Name& name, Type& type);

   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   virtual Void init(Void);

   inline Name name(Void) const { return _name; }
   inline Name& name(Void) { return _name; }
   inline TypeSig typeSig(Void) const { return _typeSig; }
   inline Type type_(Void) const { return _type; }
   inline UInt arity(Void) const { return _arity; }
   inline Type resultType(Void) const { return _resultType; }
   inline TypeSig resultTypeSig(Void) const { return _resultTypeSig; }


protected:
   Name              _name;
   TypeSig         (*_mkTypeSig)(Void);
   Name*             _namePtr;
   Type*             _typePtr;
   TypeSig           _typeSig;
   Type              _type;
   UInt              _arity;
   TypeSig           _resultTypeSig;
   Type              _resultType;

   BuiltInFn*        _next_;        // next in global list
   static BuiltInFn* globalList;    // global built in function list

   static Void initGlobalList(Void);

   friend Void postConstructorInit(Void);
   friend Label enter$(Expr expr, Env* env argN_VM);
   friend Void printExpr(Expr expr, ostream& os, const Env& env);
};

// Built-in function macros

#define fn_TS(argTS,resTS) builtInAp2(ARROW,(argTS),(resTS))
#define fn_2_TS(arg1TS,arg2TS,resTS) fn_TS(arg1TS,fn_TS(arg2TS,(resTS)))
#define fn_3_TS(arg1TS,arg2TS,arg3TS,resTS)\
fn_TS(arg1TS,fn_2_TS(arg2TS,arg3TS,(resTS)))

#define declareBuiltInFn(f) extern Cell* f##$fn;\
declareEntry(f);declareAltEntry(f)

/*
 * CellInfo helpers
 *
 * The CellInfo constructor arguments vary with SERIALISATION and
 * GARBAGE_COLLECTION.  The ordinary built-in function and the PAP
 * closure are kept consistent by funnelling both through the helpers
 * below.
 */

#if (SERIALISATION==1) && (GARBAGE_COLLECTION==1)

#define defineBuiltInFnCellInfo(f)\
CellInfo f##$cellInfo(BUILT_IN_FN,0,entry(f),altEntry(f),\
                      copyBuiltInFn,\
                      NULL,NULL,\
                      NULL,\
                      NULL,NULL)

#define defineBuiltInPAP_CellInfo_(name, form_, size_)\
static CellInfo name##$PAP_cellInfo(\
   form_,size_,\
   entry(name##$PAP_enter),\
   name##$PAP_copyFn,\
   entry(name##$PAP_extract),\
   entry(name##$PAP_insert),\
   entry(name##$PAP_mark),\
   NULL,NULL\
)

#define defineBuiltInPAP_CellInfo(name)\
defineBuiltInPAP_CellInfo_(name, CLOSURE, 0)

#define defineBuiltInSerializablePAP_CellInfo(name, size_)\
defineBuiltInPAP_CellInfo_(name, BUILT_IN_PAP, size_)

#elif (SERIALISATION==1) && (GARBAGE_COLLECTION==0)

#define defineBuiltInFnCellInfo(f)\
CellInfo f##$cellInfo(BUILT_IN_FN,0,entry(f),altEntry(f),\
                      copyBuiltInFn,\
                      NULL,NULL,\
                      NULL,NULL)

#define defineBuiltInPAP_CellInfo_(name, form_, size_)\
static CellInfo name##$PAP_cellInfo(\
   form_,size_,\
   entry(name##$PAP_enter),\
   name##$PAP_copyFn,\
   entry(name##$PAP_extract),\
   entry(name##$PAP_insert),\
   NULL,NULL\
)

#define defineBuiltInPAP_CellInfo(name)\
defineBuiltInPAP_CellInfo_(name, CLOSURE, 0)

#define defineBuiltInSerializablePAP_CellInfo(name, size_)\
defineBuiltInPAP_CellInfo_(name, BUILT_IN_PAP, size_)

#elif (SERIALISATION==0) && (GARBAGE_COLLECTION==1)

#define defineBuiltInFnCellInfo(f)\
CellInfo f##$cellInfo(BUILT_IN_FN,0,entry(f),altEntry(f),\
                      copyBuiltInFn,\
                      NULL,\
                      NULL,NULL)

#define defineBuiltInPAP_CellInfo_(name, form_, size_)\
static CellInfo name##$PAP_cellInfo(\
   form_,size_,\
   entry(name##$PAP_enter),\
   name##$PAP_copyFn,\
   entry(name##$PAP_mark),\
   NULL,NULL\
)

#define defineBuiltInPAP_CellInfo(name)\
defineBuiltInPAP_CellInfo_(name, CLOSURE, 0)

#define defineBuiltInSerializablePAP_CellInfo(name, size_)\
defineBuiltInPAP_CellInfo_(name, BUILT_IN_PAP, size_)

#else

#define defineBuiltInFnCellInfo(f)\
CellInfo f##$cellInfo(BUILT_IN_FN,0,entry(f),altEntry(f),\
                      copyBuiltInFn,\
                      NULL,NULL)

#define defineBuiltInPAP_CellInfo_(name, form_, size_)\
static CellInfo name##$PAP_cellInfo(\
   form_,\
   size_,\
   entry(name##$PAP_enter),\
   name##$PAP_copyFn,\
   NULL,NULL\
)

#define defineBuiltInPAP_CellInfo(name)\
defineBuiltInPAP_CellInfo_(name, CLOSURE, 0)

#define defineBuiltInSerializablePAP_CellInfo(name, size_)\
defineBuiltInPAP_CellInfo_(name, BUILT_IN_PAP, size_)

#endif

template <class T>
inline Void markBuiltInPAPArg_GC(const T&, Env& argEnv argN_VM) {
   (Void)argEnv;
#if (GLOBAL_VM == 0)
   (Void)vm;
#endif
}

#define defineBuiltInFn(f,TS)\
static TypeSig f##$mkTypeSig(Void){return (TS);}\
defineBuiltInFnCellInfo(f);\
Cell* f##$fn = mkBuiltInFn(#f,f##$mkTypeSig,f##$cellInfo);\
defineEntry(f)

#define defineBuiltInFn_1_arg(name,arg1TS,resTS,\
                              fp1,t1)\
defineBuiltInFn(name,fn_TS(arg1TS,resTS))\
{\
   jump(altEntry(name));\
}\
defineAltEntry(name){\
   loadArgReg(fp1,t1);\

 // Dyadic built-in function 

#define defineBuiltInFn_2_args(name,arg1TS,arg2TS,resTS,fp1,t1,fp2,t2)\
struct name##$PAP{typeSpec##t1 _x;};\
declareEntry(name##$PAP_enter);\
declareEntry(name##$PAP_extract);\
declareEntry(name##$PAP_insert);\
declareEntry(name##$PAP_mark);\
static Cell* name##$PAP_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM);\
defineBuiltInPAP_CellInfo(name);\
/* copy method */\
static Cell* name##$PAP_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM){\
   Cell* dst = new(sizeof(t1),msa) Cell(&name##$PAP_cellInfo);\
   cellBody(*dst,name##$PAP)._x = map_##t1(cellBody(src,name##$PAP)._x,&srcEnv,&env);\
   return dst;\
}\
/* PAP entry: arg1 captured in closure, arg2 supplied */\
defineEntry(name##$PAP_enter){\
   loadArgReg(fp2,t2);\
   stackAlloc(stackFPSize(t2));\
   storeArg(fp2,t2,0);\
   argRegFromCell(t1,name##$PAP,_x);\
   jump(altEntry(name));\
}\
defineEntry(name##$PAP_extract){\
   jump(popLabel());\
}\
defineEntry(name##$PAP_insert){\
   jump(popLabel());\
}\
defineEntry(name##$PAP_mark){\
   markBuiltInPAPArg_GC(cellBody(*cell,name##$PAP)._x,*cellEnv n_vm);\
   jump(popLabel());\
}\
/* Curried entry */\
defineBuiltInFn(name,fn_2_TS(arg1TS,arg2TS,resTS))\
{\
   loadArgReg(fp1,t1);\
   rCell = new(sizeof(t1),*consMSA) Cell(&name##$PAP_cellInfo);\
   cellBody(*rCell,name##$PAP)._x = map_##t1(fp1,fp1##Env,consEnv);\
   rEnv = consEnv;\
   jump(popLabel());\
}\
/* Fully applied entry */\
defineAltEntry(name){\
   defineStackDepth(name,stackFPSize(t2));\
   loadArgReg(fp1,t1);\
   loadArg(fp2,t2,0);

#define exportBuiltInPAP_CellInfo(name)\
static struct name##$PAP_CellInfoExport {\
   name##$PAP_CellInfoExport() {\
      builtInExportTable().addPAPCellInfo(#name, name##$PAP_cellInfo);\
   }\
} name##$PAP_cellInfoExport

#define builtInPAPExtractDouble(archive, dst) (archive) >> (dst)
#define builtInPAPInsertDouble(archive, src)  (archive) << (src)

#define builtInPAPExtractFloat(archive, dst)  (archive) >> (dst)
#define builtInPAPInsertFloat(archive, src)   (archive) << (src)

#define builtInPAPExtractInt(archive, dst) {\
   Int32 int$;\
   (archive) >> int$;\
   (dst) = static_cast<Int>(int$);\
}
#define builtInPAPInsertInt(archive, src) (archive) << static_cast<Int32>(src)

#define builtInPAPExtractChar(archive, dst)   (archive) >> (dst)
#define builtInPAPInsertChar(archive, src)    (archive) << (src)

#define builtInPAPExtractByte(archive, dst)   (archive) >> (dst)
#define builtInPAPInsertByte(archive, src)    (archive) << (src)

#define builtInPAPExtractTag(archive, dst) {\
   UInt16 tag$;\
   (archive) >> tag$;\
   (dst) = static_cast<Tag>(tag$);\
}
#define builtInPAPInsertTag(archive, src) (archive) << static_cast<UInt16>(src)

#define builtInPAPExtractBits(archive, dst) {\
   UInt32 bits$;\
   (archive) >> bits$;\
   (dst) = static_cast<Bits32>(bits$);\
}
#define builtInPAPInsertBits(archive, src) (archive) << static_cast<UInt32>(src)

#define builtInPAPExtractBool(archive, dst) {\
   Byte bool$;\
   (archive) >> bool$;\
   (dst) = bool$ != 0;\
}
#define builtInPAPInsertBool(archive, src) (archive) << static_cast<Byte>((src) ? 1 : 0)

#define defineBuiltInFn_2_args_BUILT_IN_PAP(name,arg1TS,arg2TS,resTS,fp1,t1,fp2,t2)\
struct name##$PAP{typeSpec##t1 _x;};\
declareEntry(name##$PAP_enter);\
declareEntry(name##$PAP_extract);\
declareEntry(name##$PAP_insert);\
declareEntry(name##$PAP_mark);\
static Cell* name##$PAP_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM);\
defineBuiltInSerializablePAP_CellInfo(name, sizeof(name##$PAP));\
exportBuiltInPAP_CellInfo(name);\
static Cell* name##$PAP_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM){\
   Cell* dst = new(sizeof(name##$PAP),msa) Cell(&name##$PAP_cellInfo);\
   cellBody(*dst,name##$PAP)._x = map_##t1(cellBody(src,name##$PAP)._x,&srcEnv,&env);\
   return dst;\
}\
defineEntry(name##$PAP_enter){\
   loadArgReg(fp2,t2);\
   stackAlloc(stackFPSize(t2));\
   storeArg(fp2,t2,0);\
   argRegFromCell(t1,name##$PAP,_x);\
   jump(altEntry(name));\
}\
defineEntry(name##$PAP_extract){\
   InputStream_Byte* is = static_cast<InputStream_Byte*>(rPtr);\
   typeSpec##t1* x = &cellBody(*cell,name##$PAP)._x;\
   ExtractArchive ea(is->_inAct, rEnv);\
   builtInPAPExtract##t1(ea, *x);\
   jump(popLabel());\
}\
defineEntry(name##$PAP_insert){\
   OutputStream_Byte* os = static_cast<OutputStream_Byte*>(rPtr);\
   typeSpec##t1 x = cellBody(*cell,name##$PAP)._x;\
   InsertArchive ia(os->_outputFn, rEnv);\
   builtInPAPInsert##t1(ia, x);\
   jump(popLabel());\
}\
defineEntry(name##$PAP_mark){\
   markBuiltInPAPArg_GC(cellBody(*cell,name##$PAP)._x,*cellEnv n_vm);\
   jump(popLabel());\
}\
defineBuiltInFn(name,fn_2_TS(arg1TS,arg2TS,resTS))\
{\
   loadArgReg(fp1,t1);\
   rCell = new(sizeof(name##$PAP),*consMSA) Cell(&name##$PAP_cellInfo);\
   cellBody(*rCell,name##$PAP)._x = map_##t1(fp1,fp1##Env,consEnv);\
   rEnv = consEnv;\
   jump(popLabel());\
}\
defineAltEntry(name){\
   defineStackDepth(name,stackFPSize(t2));\
   loadArgReg(fp1,t1);\
   loadArg(fp2,t2,0);

#define defineBuiltInPAP2_CellInfo(name)\
static CellInfo name##$PAP2_cellInfo(\
   CLOSURE,0,\
   entry(name##$PAP2_enter),\
   name##$PAP2_copyFn,\
   entry(name##$PAP2_extract),\
   entry(name##$PAP2_insert),\
   entry(name##$PAP2_mark),\
   NULL,NULL\
)

// Triadic built-in function

#define defineBuiltInFn_3_args(name,arg1TS,arg2TS,arg3TS,resTS,\
                               fp1,t1,fp2,t2,fp3,t3)\
struct name##$PAP{typeSpec##t1 _x;};\
struct name##$PAP2{typeSpec##t1 _x;typeSpec##t2 _y;};\
declareEntry(name##$PAP_enter);\
declareEntry(name##$PAP_extract);\
declareEntry(name##$PAP_insert);\
declareEntry(name##$PAP_mark);\
declareEntry(name##$PAP2_enter);\
declareEntry(name##$PAP2_extract);\
declareEntry(name##$PAP2_insert);\
declareEntry(name##$PAP2_mark);\
static Cell* name##$PAP_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM);\
static Cell* name##$PAP2_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM);\
defineBuiltInPAP_CellInfo(name);\
defineBuiltInPAP2_CellInfo(name);\
static Cell* name##$PAP_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM){\
   Cell* dst = new(sizeof(name##$PAP),msa) Cell(&name##$PAP_cellInfo);\
   cellBody(*dst,name##$PAP)._x = map_##t1(cellBody(src,name##$PAP)._x,&srcEnv,&env);\
   return dst;\
}\
static Cell* name##$PAP2_copyFn(Cell& src,const Env& srcEnv,Env& env,MSA& msa argN_VM){\
   Cell* dst = new(sizeof(name##$PAP2),msa) Cell(&name##$PAP2_cellInfo);\
   cellBody(*dst,name##$PAP2)._x = map_##t1(cellBody(src,name##$PAP2)._x,&srcEnv,&env);\
   cellBody(*dst,name##$PAP2)._y = map_##t2(cellBody(src,name##$PAP2)._y,&srcEnv,&env);\
   return dst;\
}\
defineEntry(name##$PAP_enter){\
   loadArgReg(fp2,t2);\
   rCell = new(sizeof(name##$PAP2),*consMSA) Cell(&name##$PAP2_cellInfo);\
   cellBody(*rCell,name##$PAP2)._x = map_##t1(cellBody(*cell,name##$PAP)._x,cellEnv,consEnv);\
   cellBody(*rCell,name##$PAP2)._y = map_##t2(fp2,fp2##Env,consEnv);\
   rEnv = consEnv;\
   jump(popLabel());\
}\
defineEntry(name##$PAP_extract){jump(popLabel());}\
defineEntry(name##$PAP_insert){jump(popLabel());}\
defineEntry(name##$PAP_mark){\
   markBuiltInPAPArg_GC(cellBody(*cell,name##$PAP)._x,*cellEnv n_vm);\
   jump(popLabel());\
}\
defineEntry(name##$PAP2_enter){\
   loadArgReg(fp3,t3);\
   typeSpec##t2 fp2 = cellBody(*cell,name##$PAP2)._y;\
   Env* fp2##Env = cellEnv;\
   stackAlloc(stackFPSize(t2)+stackFPSize(t3));\
   storeArg(fp2,t2,0);\
   storeArg(fp3,t3,stackFPSize(t2));\
   argRegFromCell(t1,name##$PAP2,_x);\
   jump(altEntry(name));\
}\
defineEntry(name##$PAP2_extract){jump(popLabel());}\
defineEntry(name##$PAP2_insert){jump(popLabel());}\
defineEntry(name##$PAP2_mark){\
   markBuiltInPAPArg_GC(cellBody(*cell,name##$PAP2)._x,*cellEnv n_vm);\
   markBuiltInPAPArg_GC(cellBody(*cell,name##$PAP2)._y,*cellEnv n_vm);\
   jump(popLabel());\
}\
defineBuiltInFn(name,fn_3_TS(arg1TS,arg2TS,arg3TS,resTS))\
{\
   loadArgReg(fp1,t1);\
   rCell = new(sizeof(name##$PAP),*consMSA) Cell(&name##$PAP_cellInfo);\
   cellBody(*rCell,name##$PAP)._x = map_##t1(fp1,fp1##Env,consEnv);\
   rEnv = consEnv;\
   jump(popLabel());\
}\
defineAltEntry(name){\
   defineStackDepth(name,stackFPSize(t2)+stackFPSize(t3));\
   loadArgReg(fp1,t1);\
   loadArg(fp2,t2,0);\
   loadArg(fp3,t3,stackFPSize(t2));






#define defineBuiltInFnExt(f,name,type)\
CellInfo f##$cellInfo(BUILT_IN_FN,0,entry(f),altEntry(f),,copyCellId,NULL,NULL);\
Cell* f##$fn = mkBuiltInFnExt(name,type,f##$cellInfo);

#define endBuiltInFn }

#define builtInFn(f) f##$fn

extern Cell* copyBuiltInFn(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM);

extern Cell* mkBuiltInFn(const char* ident, TypeSig (*mkTypeSig)(Void),
                         const CellInfo& cellInfo);

extern Cell* mkBuiltInFnExt(Name& name, Type& type, const CellInfo& cellInfo);

#endif /* IVORY_BUILT_IN_FN_H_DEFINED */
