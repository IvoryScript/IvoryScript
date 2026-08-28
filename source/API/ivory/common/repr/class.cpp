/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    class.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class mechanism.
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

#include <string.h>
#include "nameTable.h"
#include "ivory/class.h"
#include "ivory/object.h"
#include "ivory/trace.h"
#include "ivory/nameExprMap.h"

#define CLASS_NAME_SPACE_N_SLOTS 31

#define DISPATCH_TABLE_N_SLOTS   127

//#define DEBUG_INIT

// Class name space

static NameExprMap* _classNameSpace = NULL;

NameExprMap& classNameSpace() {
   if (_classNameSpace == NULL)
      _classNameSpace = new(builtInMSA())
                           NameExprMap(CLASS_NAME_SPACE_N_SLOTS);
   return *_classNameSpace;
}

/*----------------------------------------------------------------------------*/

// Class class

Class* Class::globalList = NULL;

Class::Class(Expr (*mkSuperclasses)(Void), const char* name, TypeSig typeVar,
             Expr (*mkDecls)(Void), Expr (*mkDefns)(Void))
   : _mkSuperclasses(mkSuperclasses), _superclasses(ERROR),
     _name(builtInName$(name)), _typeVar(typeVar),
     _mkDecls(mkDecls), _decls(ERROR), _mkDefns(mkDefns), _defns(ERROR) {

// Link into global list

   _next = globalList;
   globalList = this;
}

// Initialise 

Void Class::init() {
   _superclasses  = _mkSuperclasses();
   _decls         = _mkDecls();
   _defns         = _mkDefns();

   mapProc(initDecl, _decls);
}

Void Class::initDecl(Expr decl) const {
   assert(formOf(decl) == CLASS_METHOD, "Class::initDecl: not class method");
   toBody(decl, ClassMethod)._class = this;
}

Void Class::enterMethods(Type type) const {
   mapProc1(enterMethod, _defns, type);
   mapProc1(enterSuperclassMethods, _superclasses, type);
}

Void Class::enterMethod(Expr method, Type type) const {
   assert(formOf(method) == TYPE_METHOD, "Class::enterMethod: expected TYPE_METHOD");
   TypeMethod& typeMethod = toBody(method, TypeMethod);

   Expr val;
   if (!builtInNameMap().lookUp(
          typeMethod.name(),
          builtInNameTable().hashVal(typeMethod.name()), val)) {
      ivoryError("Class::enterMethod: method not declared");

      outStream << builtInNameTable().string(typeMethod.name()) << '\n';
      outStream.flush();

      return;
   }
   assert(formOf(val) == CLASS_METHOD, "Class::enterMethod: not class method");
   ClassMethod& classMethod = toBody(val, ClassMethod);

// Ensure definition relates to instance class or one of its superclasses

   if (this != classMethod._class && !isSubclass(classMethod._class)) {
      ivoryError("Class::enterMethod: definition not declared for class or subclass");
      return;
   }
   
   // Return if method already assigned to type

   UInt nSlots = classMethod._nDispatchSlots;
   if (classMethod._dispatchTable != NULL)
      for (UInt i = 0; i < nSlots; i++)
         if (classMethod._dispatchTable[i]._type == type)
            return;

#ifdef DEBUG_INIT
   outStream << "ClassMethod::enterMethod: adding " <<
      builtInNameTable().string(classMethod._name) << " for type ";
   printType(type, outStream, Env(builtInNameTable(), builtInTypeTable()));
   outStream << " to class " << builtInNameTable().string(_name) << '\n';
   outStream.flush();
#endif

   // Create or extend the table

   classMethod._nDispatchSlots++;
   struct DispatchSlot* dispatchTable =
      (struct DispatchSlot*)builtInMSA().alloc(
            classMethod._nDispatchSlots * sizeof(DispatchSlot));
   if (classMethod._dispatchTable != NULL) {
      for (UInt i = 0; i < nSlots; i++)
         dispatchTable[i] = classMethod._dispatchTable[i];
      builtInMSA().free(classMethod._dispatchTable);
   }
   dispatchTable[nSlots]._type  = type;
   dispatchTable[nSlots]._label = typeMethod._label;

   classMethod._dispatchTable = dispatchTable;
}

Void Class::enterSuperclassMethods(Expr superClass, Type type) const {
   if (formOf(superClass) != CLASS) {
      ivoryError("Class::enterSuperclassMethods: not Class");
      return;
   }
   Class& class_ = toBody(superClass, Class);
   class_.enterMethods(type);
}

Bool Class::isSubclass(const Class* super) const {
   for (Expr x = _superclasses; x != Nil; x = tl(x)) {
      if (formOf(hd(x)) != CLASS) {
         ivoryError("Class::isSubclass: not class");
         return FALSE;
      }
      Class* class_ = &toBody(hd(x), Class);
      if (class_ == super || class_->isSubclass(super))
         return TRUE;
      }
   return FALSE;
}

Bool Class::method(Name name, Type type, Expr& res) const {
   for (Instance* instance = Instance::globalList;
        instance != NULL;
        instance = instance->_next)
      if ((instance->_class == this ||
          instance->_class->isSubclass(this)) &&
          instance->_type == type) {

//------ First look for instance method

         for (Expr defn = instance->_defns; defn != Nil; defn = tl(defn)) {
            assert(formOf(hd(defn)) == TYPE_METHOD, "Instance::method: expected TYPE_METHOD");
            TypeMethod& typeMethod = toBody(hd(defn), TypeMethod);
            if (typeMethod.name() == name) {
               res = typeMethod._val;
               return TRUE;
            }
         }

//------ If not found, return class or superclass method

         if (instance->_class->method(name, res))
            return TRUE;
      }
   return FALSE;
}

Void Class::initGlobalLists() {
   for (Class* class_ = globalList;
        class_ != NULL;
        class_ = class_->_next)
      class_->init();

   ClassMethod::initGlobalList();

   Instance::initGlobalList();

   ClassMethod::sortDispatchTables();

   ClassMethodRef::initGlobalList();
}

Class* Class::lookUp(Name name) {
   for (Class* class_ = globalList; class_ != NULL; class_ = class_->_next)
      if (class_->_name == name)
         return class_;
   return NULL;
}

// Look up named method for class
// T.B.D. continue with superclasses

Bool Class::method(Name name, Expr& res) const {
   for (Expr defn = _defns; defn != Nil; defn = tl(defn)) {
      assert(formOf(hd(defn)) == TYPE_METHOD, "Instance::method: expected TYPE_METHOD");
      TypeMethod& typeMethod = toBody(hd(defn), TypeMethod);
      if (typeMethod.name() == name) {
         res = typeMethod._val;
         return TRUE;
      }
   }

   for (Expr superclass = _superclasses; superclass != Nil; superclass = tl(superclass)) {
      assert(formOf(hd(superclass)) == CLASS, "Class::isSubclass: not class");
      if (toBody(hd(superclass), Class).method(name, res))
         return TRUE;
   }

   return FALSE;
}

// Function to make a class cell_

Expr mkClass(Expr (*mkSuperclasses)(Void), const char* name, TypeSig typeVar,
             Expr (*mkDecls)(Void), Expr (*mkDefns)(Void)) {
   Cell& cell_ = *new(sizeof(Class), builtInMSA()) Cell(CLASS);
   new(cell_.body()) Class(mkSuperclasses, name, typeVar, mkDecls, mkDefns);
   classNameSpace().add(NameExprBinding(builtInName$(name), fromCell(cell_)),
                        hashString(name), builtInMSA());

   return fromCell(cell_);
}

/*----------------------------------------------------------------------------*/

// Global built-in function list

ClassMethod* ClassMethod::globalList = NULL;

ClassMethod::ClassMethod(Name name, Label entryLabel, TypeSig (*mkTypeSig)(Void))
   : _class(NULL), _name(name),
     _mkTypeSig(mkTypeSig), _type(NULL_TYPE),
     _arity(0),
     _nDispatchSlots(0), _dispatchTable(NULL), _entryLabel(entryLabel) {
   // Link into global list

   _next    = globalList;
   globalList  = this;
}

// Initialisation 

Void ClassMethod::init() {
   _typeSig = _mkTypeSig();
   _type    = useBuiltInType(_typeSig);
   _arity = ::arity(_typeSig);

#ifdef DEBUG_INIT
   outStream << "ClassMethod::init: " << builtInNameTable().string(_name) << ' ' <<
                "arity " <<_arity << '\n';
   outStream.flush();
#endif
}

// Simple buble sort for disptach entries

Void ClassMethod::sortDispatchTable() {
   if (_nDispatchSlots == 0)
      return;

   Bool swapFlag;
   do {
      swapFlag = FALSE;
      for (UInt i = 0; i < _nDispatchSlots - 1; i++) {
         if (_dispatchTable[i]._type > _dispatchTable[i + 1]._type) {
            swapFlag = TRUE;
            DispatchSlot temp = _dispatchTable[i];
            _dispatchTable[i] = _dispatchTable[i + 1];
            _dispatchTable[i + 1] = temp;
          }
      }
   }
   while (swapFlag);
}

Void ClassMethod::initGlobalList(Void) {
   for (ClassMethod* classMethod = globalList;
        classMethod != NULL;
        classMethod = classMethod->_next)
      classMethod->init();
}

Void ClassMethod::sortDispatchTables(Void) {
   for (ClassMethod* classMethod = globalList;
        classMethod != NULL;
        classMethod = classMethod->_next)
      classMethod->sortDispatchTable();
}

// Dispatch method according to type

Label ClassMethod::dispatch(Type type) {

// Search dispatch table by binary section

   Int low = 0;
   Int high = _nDispatchSlots - 1;

   while (low <= high) {
      UInt mid = (low + high) / 2;
      Type midType = _dispatchTable[mid]._type;
      if      (midType == type)
         jump(_dispatchTable[mid]._label);
      else if (type < midType)
         high = mid - 1;
      else
         low  = mid + 1;
   }

   Env env(builtInNameTable(), NULL, NULL, builtInTypeTable());
   printName(_name, outStream, env.nameTable());
   outStream << ": no method for type - ";
   printType(type, outStream, env);
   outStream << "\n";
   outStream.flush();
   runTimeError("");
}

// Function to make a class method cell

Expr mkClassMethod(Name name, Label entryLabel, TypeSig (*mkTypeSig)(Void),
                   const CellInfo& cellInfo) {
   Cell& cell_ = *new(sizeof(ClassMethod), builtInMSA()) Cell(&cellInfo);
   new(cell_.body()) ClassMethod(name, entryLabel, mkTypeSig);

   builtInNameMap().add(NameExprBinding(name, fromCell(cell_)),
                               builtInNameTable().hashVal(name),
                               builtInMSA());

   return fromCell(cell_);
}

/*----------------------------------------------------------------------------*/

// Instance

Instance* Instance::globalList = NULL;

Instance::Instance(Expr (*mkClass)(Void), TypeSig (*mkTypeSig)(Void), Expr (*mkDefns)(Void))
   : _mkClass(mkClass),
     _mkTypeSig(mkTypeSig), _type(NULL_TYPE),
     _mkDefns(mkDefns),  _defns(ERROR) {

// Link into global list

  _next = globalList;
   globalList = this;
}

// Init Instance

Void Instance::init() {
   _type    = useBuiltInType(_mkTypeSig());
   _defns   = _mkDefns();
   _class   = &toBody(_mkClass(), Class);

   mapProc(enterMethod, _defns);
   _class->enterMethods(_type);
}

// Enter instance method

Void Instance::enterMethod(Expr method) const {
   _class->enterMethod(method, _type);
}

Void Instance::initGlobalList() {
   for (Instance* instance = globalList;
        instance != NULL;
        instance = instance->_next)
      instance->init();
}

// Function to make an instance cell

Expr mkInstance(Expr (*mkClass)(Void), TypeSig (*mkTypeSig)(Void), Expr (*mkDefns)(Void)) {
   Cell& cell_ = *new(sizeof(Instance), builtInMSA()) Cell(INSTANCE);
   new(cell_.body()) Instance(mkClass, mkTypeSig, mkDefns);

   return fromCell(cell_);
}

/*----------------------------------------------------------------------------*/

TypeMethod::TypeMethod(Name name, Label label, Expr val)
   : _name(name), _label(label), _val(val) {
}

// Function to make a type method cell

Expr mkTypeMethod(Name name, Label label, Expr val) {
   Cell& cell_ = *new(sizeof(TypeMethod), builtInMSA()) Cell(TYPE_METHOD);
   new(cell_.body()) TypeMethod(name, label, val);
   return fromCell(cell_);
}

/*----------------------------------------------------------------------------*/

// ClassMethodRef

ClassMethodRef* ClassMethodRef::globalList = NULL;

ClassMethodRef::ClassMethodRef(Expr& classMethod, const char* name)
   : _classMethod(classMethod), _name(builtInName$(name)) {
   _next = globalList;
   globalList = this;
}

Void ClassMethodRef::init() {
   if (!builtInNameMap().lookUp(_name,
                                builtInNameTable().hashVal(_name),
                                _classMethod)) {
      outStream << "ClassMethodRef::init: ";
      printName(_name, outStream, builtInNameTable());
      outStream << " not declared\n";
      outStream.flush();
   }
}

Void ClassMethodRef::initGlobalList() {
   for (ClassMethodRef* ref = globalList;
        ref != NULL;
        ref = ref->_next)
      ref->init();
}
