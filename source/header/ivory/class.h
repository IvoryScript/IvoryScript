/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    class.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory classs.
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

#ifndef IVORY_CLASS_H_DEFINED
#define IVORY_CLASS_H_DEFINED

#include "ivory/common.h"

class Instance;

// Class

class Class {
protected:
   Class(Expr (*mkSuperclasses)(Void), const char* name, TypeSig typeVar,
         Expr (*mkDecls)(Void), Expr (*mkDefns)(Void));
   Class(Name name, TypeSig typeVar, Expr decls, Expr defns);

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

   Void init(Void);
public:
   inline Name name(Void) { return _name; }
   TypeSig typeVar(Void) const { return _typeVar; }
protected:
   Void initDecl(Expr decl) const;
   Void enterMethods(Type type) const;
   Void enterMethod(Expr method, Type type) const;
   Void enterSuperclassMethods(Expr superClass, Type type) const;
   Bool isSubclass(const Class* super) const;
public:
   Bool method(Name name, Expr& res) const;
   Bool method(Name name, Type type, Expr& res) const;

protected:
   Expr         (*_mkSuperclasses)(Void);
   Expr           _superclasses;    // superclass list
   Name           _name;
   TypeSig        _typeVar;         // type variable
   Expr         (*_mkDecls)(Void);
   Expr           _decls;           // method declaration list
   Expr         (*_mkDefns)(Void);
   Expr           _defns;           // method definition list

   Class*         _next;            // next in list
   static Class*  globalList;       // global class list

   static Class* lookUp(Name name);
   static Void initGlobalLists(Void);
   friend class ClassMethod;
   friend class Instance;

   friend Expr mkClass(Expr (*mkSuperclasses)(Void), const char* name,
                       TypeSig typeVar,
                       Expr (*mkDecls)(Void), Expr (*mkDefns)(Void));
   friend Void postConstructorInit(Void);
   friend Void printExpr(Expr expr, ostream& os, const Env& env);
};

// Class method

struct DispatchSlot;
class ClassMethod {
protected:
   ClassMethod(Name method, Label entryLabel, TypeSig (*mkTypeSig)(Void));

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
   Void init(Void);
   Void sortDispatchTable();

public:
   inline const Class* class__(Void) const { return _class; }
   inline Name name(Void) const { return _name; }
   inline TypeSig typeSig(Void) const { return _typeSig; }
   inline Type type_(Void) const { return _type; }
   inline UInt arity(Void) const { return _arity; }
   inline Label entryLabel(Void) const { return _entryLabel; }
   Label dispatch(Type type);

protected:
   const Class*         _class;
   Name                 _name;
   TypeSig              (*_mkTypeSig)(Void);
   TypeSig              _typeSig;
   Type                 _type;                  // type
   UInt                 _arity;
   UInt                 _nDispatchSlots;
   struct DispatchSlot* _dispatchTable;
   Label                _entryLabel;
 
protected:
   ClassMethod*         _next;            // next in list
   static ClassMethod*  globalList;

   static Void initGlobalList(Void);
   static Void sortDispatchTables(Void);

   friend class Class;
   friend class Instance;
   friend Expr mkClassMethod(Name name, Label entryLabel, TypeSig (*mkTypeSig)(Void),
                             const CellInfo& cellInfo);
   friend Void printExpr(Expr expr, ostream& os, const Env& env);
};

// Instance

class Instance {
protected:
   Instance(Expr (*mkClass)(Void),
            TypeSig (*mkTypeSig)(Void), Expr (*mkDefns)(Void));

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
   Void init(Void);
   Void enterMethod(Expr method) const;

protected:
   Expr     (*_mkClass)(Void);
   Class*    _class;                // class
   TypeSig  (*_mkTypeSig)(Void);
   TypeSig  _typeSig;
   Type     _type;                  // type
   Expr     (*_mkDefns)(Void);
   Expr     _defns;                 // method definition list

   Instance*      _next;            // next in list of instances
   static Instance* globalList;

   static Void initGlobalList(Void);

   friend class Class;
   friend Expr mkInstance(Expr (*mkClass)(Void), TypeSig (*mkTypeSig)(Void),
                          Expr (*mkDefns)(Void));
};

// Type method

class TypeMethod {
protected:
   TypeMethod(Name method, Label label, Expr val);

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
   inline Name name(Void) { return _name; }

protected:
   Name           _name;
   Label          _label;
   Expr           _val;

   friend class Class;
   friend class Instance;
   friend Expr mkTypeMethod(Name name, Label label, Expr val);
};

// Class method dispatch table slot

struct DispatchSlot {
   Type           _type;                  // type
   Label          _label;                 // entry label
};

// Class method reference

class ClassMethodRef {
public:
   ClassMethodRef(Expr& cm, const char* name);
protected:
   Void init(Void);

protected:
   ClassMethodRef*   _next;
   Expr&             _classMethod;
   Name              _name;

   static ClassMethodRef* globalList;

   static Void initGlobalList(Void);

   friend class Class;
};

// Class macros

#define declareClass(C) extern Expr class_##C

#define defineClass(superClasses,C,typeVar,decls,defns)\
static Expr mkSuperclasses_##C(Void){return(superClasses);};\
static Expr mkDecls_##C(Void){return(decls);};\
static Expr mkDefns_##C(Void){return(defns);};\
Expr class_##C=mkClass(mkSuperclasses_##C,#C,fromName(builtInName(typeVar)),\
mkDecls_##C,mkDefns_##C)

#define class_(C) class_##C

// Method declaration macros

#define classMethodDeclName_n(n,name,lab,TS)\
declareLabel(methodDeclEntry_##n);\
static CellInfo cellInfo$methodDecl$##n(CLASS_METHOD,0,label(methodDeclEntry_##n),copyCellId,NULL,NULL);\
static TypeSig mkTypeSigMethodDecl_##n(Void){return(TS);};\
static Expr methodDecl_##n=mkClassMethod(name,lab,mkTypeSigMethodDecl_##n,cellInfo$methodDecl$##n);\
defineLabel(methodDeclEntry_##n)

#define classMethodDecl_n(n,name,lab,TS) classMethodDeclName_n(n,builtInName(name),lab,TS)
#define classMethodDecl(name,lab,TS) classMethodDecl_n(1,name,lab,TS)

#define classOperatorDecl_n(n,op,lab,TS) classMethodDeclName_n(n,builtInName$(op),lab,TS)
#define classOperatorDecl(op,lab,TS) classOperatorDecl_n(1,op,lab,TS)

// Instance macros

#define declareInstance(i) extern Expr instance_##i

// name -   unique name for instance
// C    -   class name
// TS   -   type signure of instance type
// defns -  method definitions

#define defineInstance(name,C,TS,defns)\
static Expr mkInstanceClass_##name(Void){return(class_(C));}\
static Expr mkInstanceDefns_##name(Void){return(defns);}\
static TypeSig mkInstanceTypeSig_##name(Void){return(TS);}\
Expr instance_##name=mkInstance(mkInstanceClass_##name,mkInstanceTypeSig_##name,mkInstanceDefns_##name)

#define instance(i) instance_##i

// Method definition macros

#define classMethodDefnName_n(n,name,val)\
declareLabel(methodDefnEntry_##n);\
static Expr methodDefn_##n=mkTypeMethod(name,label(methodDefnEntry_##n),fromName(builtInName(val)));\
defineLabel(methodDefnEntry_##n)

#define classMethodDefn_n(n,name,val) classMethodDefnName_n(n,builtInName(name),val)
#define classMethodDefn(name,val) classMethodDefn_n(1,name,val)

#define classOperatorDefn_n(n,op,val) classMethodDefnName_n(n,builtInName$(op),val)
#define classOperatorDefn(op,val) classOperatorDefn_n(1,op,val)

#define classMethodRef(cm,name)\
static Expr classMethod_##cm=ERROR;\
static ClassMethodRef classMethodRef_##cm(classMethod_##cm,name)

#define classMethod(cm) classMethod_##cm

extern Expr mkClass(Expr (*mkSuperclasses)(Void), const char* name, TypeSig typeVar,
                    Expr (*mkDecls)(Void), Expr (*mkDefns)(Void));

extern Expr mkClassMethod(Name name, Label lab, TypeSig (*mkTypeSig)(Void),
                          const CellInfo& cellInfo);

extern Expr mkInstance(Expr (*mkClass)(Void), TypeSig (*mkTypeSig)(Void),
                       Expr (*mkDefns)(Void));

extern Expr mkTypeMethod(Name name, Label label, Expr val);

#endif /* IVORY_CLASS_H_DEFINED */
