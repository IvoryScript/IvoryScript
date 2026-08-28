/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    BasicRegister.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Register class for a basic data store.
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

#ifndef IVORY_BasicRegister_H_DEFINED
#define IVORY_BasicRegister_H_DEFINED

#include "ivory/nameAnyBinding.h"
#include "ivory/nameMap.h"
#include "ivory/register.h"

declareType(BasicRegisterRepr);

declareTypeCon(BasicRegisterEntryRepr);

declareType(BasicRegisterEntryRepr);


class BasicRegisterEntryRepr {
public:
   BasicRegisterEntryRepr(const Ref next, const NameAnyBinding& binding)
      : _next(next), _binding(binding) {
   }
   BasicRegisterEntryRepr(Ref next, const Env& next_env,
      const NameAnyBinding& binding, const Env& binding_env,
      Env&, MSA& msa);
   BasicRegisterEntryRepr(const BasicRegisterEntryRepr& src, const Env& srcEnv, Env& env, MSA& msa);
   Void destroy(Env& env, MSA& msa);
   inline
      void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
   inline
      void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   Void operator delete(Void* ptr, MSA& msa) {
   }
   Void operator delete(Void* ptr1, Void* ptr2)
#else
   Void operator delete(Void* ptr)
#endif
   {
   }

   inline const NameAnyBinding& binding() const {
      return _binding;
   }
   inline NameAnyBinding& binding() {
      return _binding;
   }

protected:
   Ref            _next;
   NameAnyBinding _binding;

   friend class BasicRegisterRepr;
};

class BasicRegisterRepr : public Name_T_Map<Ref, Any> {
public:
   BasicRegisterRepr(UInt nSlots);
   BasicRegisterRepr(const BasicRegisterRepr& src, const Env& srcEnv, Env& env, MSA& msa);
   Void destroy(Env& env, MSA& msa);

   inline
   void* operator new(size_t size, MSA& msa){
      return msa.alloc(size);
   }
   inline
   void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   Void operator delete(Void* ptr, MSA& msa)
   {
   }
   Void operator delete(Void* ptr1, Void* ptr2)
#else
   Void operator delete(Void* ptr)
#endif
   {
   }

public:
   BasicRegisterEntryRepr* lookUp(Name name, UInt hashVal, ObjStore& objStore) const;
   Ref remove(Name name, UInt hashVal, ObjStore& objStore);

protected:
   virtual Void* allocNode(size_t size, MSA& msa, Ref& next);
   virtual HashTableNode<Ref, NameBinding<Any>, Name>& linkToNode(Ref link) const {
      assert(FALSE, "BasicRegisterRepr::linkToNode: T.B.D");
      return *(HashTableNode<Ref, NameBinding<Any>, Name>*)NULL;
   }

   friend Label addBasicRegisterEntry$altEntry(argVM);
};

declareBuiltInDataCon(BasicRegister);

declareBuiltInDataCon(BasicRegisterEntry);


declareBuiltInFn(addBasicRegisterEntry);

declareBuiltInFn(hasBasicRegisterEntry);

declareBuiltInFn(removeBasicRegisterEntry);

declareBuiltInFn(selectBasicRegister);


#endif /* IVORY_BasicRegister_H_DEFINED */
