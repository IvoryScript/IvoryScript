/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    vector.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory Vector type.
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
 */

#ifndef IVORY_VECTOR_H_DEFINED
#define IVORY_VECTOR_H_DEFINED

#define NOMINAL_VECTOR_LENGTH 65536

#include "ivory/array.h"
#include "ivory/int.h"
#include "ivory/list.h"

//-----------------------------------------------------------------------------
// Vector t = Vector<t> Expr
//
// parameterised type
//
// Expression should evaluate to a list whose elements when evaluated
// are all of type t.
//-----------------------------------------------------------------------------

template <class T>
class Vector
{
public:
   Vector(Void)
      : _nElem(0), _elems(NULL)
   {
   }

   void Vector$(argVM);           // Construct into hunk

   Vector(const Vector<T>& src, const Env& srcEnv, Env& env, MSA& msa)
      : _nElem(src._nElem),
      _elems(Array<T, NOMINAL_VECTOR_LENGTH>::new$(src._nElem, msa))
                               
   {
      if (_elems != NULL)
         _elems->Array$(src._nElem, *src._elems, srcEnv, env, msa);
   }
   Void destroy(Env& env, MSA& msa)
   {
      if (_elems != NULL)
      {
         _elems->destroy(_nElem, env, msa);
         msa.free(_elems);
      }
   }

   inline Void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
   inline Void* operator new(size_t size, Void* ptr)
   {
      return ptr;
   }

#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
   {
   }
   inline Void operator delete(Void* ptr1, Void* ptr2)
   {
   }
#endif

   inline UInt16 length(Void)
   {
      return _nElem;
   }

   inline
   Void add(const T& src, MSA& msa)
   {
      Array<T, NOMINAL_VECTOR_LENGTH>* elems =
         Array<T, NOMINAL_VECTOR_LENGTH>::new$(_nElem + 1, msa);
      memcpy(elems, _elems, Array<T, NOMINAL_VECTOR_LENGTH>::elemSize() * _nElem);
      (*elems)[_nElem++] = src;
      msa.free(_elems);
      _elems = elems;
   }

   inline
   Void add(const T& src, const Env& srcEnv, Env& env, MSA& msa)
   {
      Array<T, NOMINAL_VECTOR_LENGTH>* elems =
         Array<T, NOMINAL_VECTOR_LENGTH>::new$(_nElem + 1, msa);
      memcpy(elems, _elems, Array<T, NOMINAL_VECTOR_LENGTH>::elemSize() * _nElem);
      new(&(*elems)[_nElem++]) T(src, srcEnv, env, msa);
      msa.free(_elems);
      _elems = elems;
   }

   inline
   Void addAt(const T& src, UInt16 index, MSA& msa)
   {
      assert(index <= _nElem, "index out of range");
      Array<T, NOMINAL_VECTOR_LENGTH>* elems =
         Array<T, NOMINAL_VECTOR_LENGTH>::new$(_nElem + 1, msa);
      size_t elemSize = Array<T, NOMINAL_VECTOR_LENGTH>::elemSize();
      memcpy(elems, _elems, elemSize * index);
      (*elems)[index] = src;
      memcpy((unsigned char*)elems  + elemSize * (index + 1),
             (unsigned char*)_elems + elemSize * index,
             (_nElem - index) * elemSize);
      msa.free(_elems);
      _elems = elems;
      _nElem++;
   }

   inline
   Void addAt(const T& src, UInt16 index, const Env& srcEnv, Env& env, MSA& msa)
   {
      assert(index <= _nElem, "index out of range");
      Array<T, NOMINAL_VECTOR_LENGTH>* elems =
         Array<T, NOMINAL_VECTOR_LENGTH>::new$(_nElem + 1, msa);
      size_t elemSize = Array<T, NOMINAL_VECTOR_LENGTH>::elemSize();
      memcpy(elems, _elems, elemSize * index);
      memcpy((unsigned char*)elems  + elemSize * (index + 1),
             (unsigned char*)_elems + elemSize * index,
             (_nElem - index) * elemSize);
      msa.free(_elems);
      _elems = elems;
      _nElem++;
      new(&(*this)[index]) T(src, srcEnv, env, msa);
   }

   inline
   Void removeAt(UInt16 index, Env& env, MSA& msa)
   {  
      assert(index < _nElem, "index out of range");
      (*this)[index].destroy(env, msa);
      _nElem--;
      Array<T, NOMINAL_VECTOR_LENGTH>* elems =
         Array<T, NOMINAL_VECTOR_LENGTH>::new$(_nElem, msa);
      size_t elemSize = Array<T, NOMINAL_VECTOR_LENGTH>::elemSize();
      memcpy(elems, _elems, elemSize * index);
      memcpy((unsigned char*)elems  + elemSize *  index,
             (unsigned char*)_elems + elemSize * (index + 1),
             (_nElem - index) * elemSize);
      msa.free(_elems);
      _elems = elems;
   }

   inline
   T& operator [](UInt16 index)
   {
      return (*_elems)[index];
   }

   inline
   const T& operator [](UInt16 index) const
   {
      return (*_elems)[index];
   }

   inline
   Void print(ostream& os, const Env& env) const
   {
      _elems->print(_nElem, os, env);
   }

protected:
   UInt16                           _nElem;
   Array<T, NOMINAL_VECTOR_LENGTH>* _elems;

public:

   static Label construct(argVM);
   static Label construct$(argVM);
   static Label construct_l1(argVM);
   static Label construct_l2(argVM);
};

// Vector<t> elems = (!(length elems), !(Array<t> elems)<UInt, Array<t>>)

#define elems_env    sizeof(Env*)
#define elems        elems_env+sizeof(Expr)
#define stackDepth   elems

declareEntry(lengthList_);

template <class T>
inline void Vector<T>::Vector$(argVM)
{
   pushLabel(label(NULL));
//   push(local(elems), Expr);
   localCallSequence(entry(lengthList_));
   _nElem = (UInt16)rInt;
   pushLabel(NULL);
   push((Int)_nElem, Int);
   localCallSequence((Label)(Array<T,NOMINAL_VECTOR_LENGTH>::construct$));

// Need to consider update here

   if (updatePtr != NULL)
      updatePtr = NULL;

   _elems = (Array<T, NOMINAL_VECTOR_LENGTH>*)rPtr;
}

#undef stackDepth
#undef elems
#undef elems_env

// Heap cell constructor

template <class T>
inline Label Vector<T>::construct(argVM)
{
   checkArgs("Vector<T>::construct");
   jump(label(construct$));
}

template <class T>
inline Label Vector<T>::construct$(argVM)
{
   Vector<T>* vec = new (*consMSA) Vector<T>;
   vec->Vector$(vm);
   rPtr = vec;

// Currently, return type is always coerced

   jump(popLabel());
}

#endif /* IVORY_VECTOR_H_DEFINED */

