/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    propertySet.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory PropertySet data type
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

#ifndef IVORY_PROPERTY_SET_H_DEFINED
#define IVORY_PROPERTY_SET_H_DEFINED

#include "ivory/dataCon.h"
#include "ivory/property.h"
#include "ivory/ref.h"
#include "ivory/vector.h"

declareTypeCon(PropertySet);

declareType(PropertySet);

class PropertySet : public Vector<Property> {
public:
   PropertySet(Void) : Vector<Property>() {}
   PropertySet(const PropertySet& src, const Env& srcEnv, Env& env, MSA& msa)
      : Vector<Property>(src, srcEnv, env, msa) {}
   inline Void PropertySet$(argVM) { Vector<Property>::Vector$(vm); }

   inline Void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }

#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {}
   inline Void operator delete(Void* ptr1, Void* ptr2) {}
#endif

   Void print(ostream& os, const Env& env) const;
};

extern PropertySet& checkPropertySet$(argVM);
#define checkPropertySet() checkPropertySet$(vm)

declareDataCon(PropertySet);

declareBuiltInFn(addPropertyPropertySetBasedObject);

declareBuiltInFn(checkBoolProperty);

declareBuiltInFn(hasPropertyPropertySet);

declareBuiltInFn(searchPropertySet);

declareBuiltInFn(selectPropertySet);

declareBuiltInFn(showPropertySet);

declareBuiltInFn(lengthPropertySet);

declareBuiltInFn(getAtPropertySet);

declareBuiltInFn(selectPropertySetBasedObject);

declareBuiltInFn(hasPropertyPropertySetBasedObject);

declareBuiltInFn(setPropertySetBasedObject);

declareBuiltInFn(addRefPropertySetBasedObject);

declareBuiltInFn(addRefAtPropertySetBasedObject);

declareBuiltInFn(removeRefPropertySetBasedObject);

declareBuiltInFn(removePropertyPropertySetBasedObject);

declareBuiltInFn(updatePropertyPropertySetBasedObject);

extern Tag enumProperty$(const PropertySet& ps, const Env& env, Name name, Type type argN_VM);

extern Tag enumProperty$(Ref ref, const ADS_Instance& adsi, Name name, Type type argN_VM);

extern Bool boolProperty$(const PropertySet& ps, const Env& env, Name name argN_VM);

extern Bool boolProperty$(Ref ref, const ADS_Instance& adsi, Name name argN_VM);

extern Void getProperty$(const PropertySet& ps, const Env& env, Name name argN_VM);

extern Void getProperty$(Ref ref, const ADS_Instance& adsi, Name name argN_VM);

extern Int intProperty$(const PropertySet& ps, const Env& env, Name name argN_VM);

extern Int intProperty$(const PropertySet& ps, const Env& env, Name name,
                        Int defVal argN_VM);

extern Int intProperty$(Ref ref, const ADS_Instance& adsi, Name name argN_VM);

extern Void refProperty$(const PropertySet& ps, const Env& env, Name name,
                         ADSRef& adsr argN_VM);

extern Void refProperty$(Ref ref, const ADS_Instance& adsi, Name name,
                         ADSRef& adsr argN_VM);

extern Void setBoolProperty$(const ADSRef& adsr, Name name, Bool val argN_VM);

extern Void setIntProperty$(const ADSRef& adsr, Name name, Int val argN_VM);

extern Void setStringProperty$(const ADSRef& adsr, Name name, const char* val argN_VM);

extern String stringProperty$(const PropertySet& ps, const Env& env, Name name argN_VM);

extern String stringProperty$(Ref ref, const ADS_Instance& adsi, Name name argN_VM);

extern Bool hasProperty$(const PropertySet& ps, Name name argN_VM);

extern Bool hasProperty$(const ADSRef& adsr, Name name argN_VM);

#define enumProperty(x,env,name,T) enumProperty$(x,env,name,builtInType(T) n_vm)

#define boolProperty(x,env,name) boolProperty$(x,env,name n_vm)

#define getProperty(x,env,name) getProperty$(x,env,name n_vm)

#define intProperty(x,env,name) intProperty$(x,env,name n_vm)

#define refProperty(x,env,name,adsr) refProperty$(x,env,name,adsr n_vm)

#define stringProperty(x,env,name) stringProperty$(x,env,name n_vm)

#define hasProperty(x,name) hasProperty$(x,name n_vm)

#define setBoolProperty(ref,name,val) setBoolProperty$(ref,name,val n_vm)

#define setIntProperty(ref,name,val) setIntProperty$(ref,name,val n_vm)

#define setStringProperty(ref,name,val) setStringProperty$(ref,name, val n_vm)

extern Void update_PropertySet(argVM);

#define updatePropertySet()\
if(updatePtr!=NULL){update_PropertySet(vm);updatePtr=NULL;}

#endif /* IVORY_PROPERTY_SET_H_DEFINED */
