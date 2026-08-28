/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    nameBinding.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definition of binding for types Binding Name a
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

#ifndef IVORY_NAME_BINDING_H_DEFINED
#define IVORY_NAME_BINDING_H_DEFINED

#include "nameTable.h"
#include "ivory/binding.h"

// NameBinding: A binding where the identifier is a name.

template <class VAL_TYPE>
class NameBinding  : public Binding<Name, VAL_TYPE> {
public:
   inline NameBinding(Void) {}
   inline NameBinding(Name name) : Binding<Name, VAL_TYPE>(name) {}
	inline NameBinding(Name name, const VAL_TYPE& val) : Binding<Name, VAL_TYPE>(name, val) {}
   inline
		NameBinding(const NameBinding<VAL_TYPE>& src, const Env& srcEnv, Env& env, MSA& msa) {
         this->_id = copyName(src._id, srcEnv.nameTable(), env.nameTable());
			VAL_TYPE t(src._val, srcEnv, env, msa);
         this->_val = t;
		}
   inline
	Void destroy(Env& env, MSA& msa) {
      dropName_(this->_id, env.nameTable());
      this->_val.destroy(env, msa);
   }
	inline void* operator new(size_t size, MSA& msa) { return msa.alloc(size); }
   inline void* operator new(size_t size, void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr, MSA& msa) {
   }
	void operator delete(void* ptr1, void* ptr2) {
#else
	void operator delete(void* ptr) {
#endif
   }

   inline Name name() const { return this->_id; }
   inline Name& name() { return this->_id; }
   Void print(ostream& os, const Env& env) const {
      printName(this->_id, os, env);
      os << ':';
      this->_val.print(os, env);
   }
};

#endif /* IVORY_NAME_BINDING_H_DEFINED */
