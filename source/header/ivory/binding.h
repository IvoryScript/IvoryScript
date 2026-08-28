/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    binding.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions based on Binding class.
 *
 *    In this context, a binding is an polymorphic association between an identifer
 *    and a value.
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

#ifndef IVORY_BINDING_H_DEFINED
#define IVORY_BINDING_H_DEFINED

#include "ivory/type.h"

template <class ID_TYPE, class VAL_TYPE>
class Binding {
public:
   inline Binding(Void) {}
   inline Binding(ID_TYPE id) : _id(id) {}
   inline Binding(ID_TYPE id, const VAL_TYPE& val) : _id(id), _val(val) {}
	inline void* operator new(size_t size, MSA& msa) { return msa.alloc(size); }
   inline void* operator new(size_t size, void* ptr) { return ptr; }
#ifdef DELETE_NEEDS_NEW_ARG
   void operator delete(void* ptr, MSA& msa) {}
	void operator delete(void* ptr1, void* ptr2) {
#else
	void operator delete(void* ptr) {
#endif
   }

   inline const ID_TYPE& id(Void) const { return _id; }
   inline ID_TYPE& id(Void) { return _id; }
   inline const VAL_TYPE& val() const { return _val; }
   inline VAL_TYPE& val() { return _val; }

protected:
   ID_TYPE	_id;
   VAL_TYPE	_val;
};

declareTypeCon(Binding);

#endif /* IVORY_BINDING_H_DEFINED */
