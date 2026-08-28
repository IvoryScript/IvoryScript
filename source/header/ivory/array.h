/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    array.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory Array types.
 *
 * Notes:
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

#ifndef IVORY_ARRAY_H_DEFINED
#define IVORY_ARRAY_H_DEFINED

#include "ivory/common.h"
#include "ivory/list.h"
#include "ivory/machine.h"
#include "ivory/repr.h"

// Array<t> n elems
//
// let copyFrom i arr n elems =
//    case elems of
//       Nil ->         {
//                         if (i ¬= n) then
//                            error "insufficient initialisers"
//                         rPtr = arr
//                         rType = (Array t)
//                      }
//
//       Cons x xs ->   if (i >= n) then
//                         error "too many initialisers"
//                      else
//                      {
//                         arr[i] = !(head list)
//                         copyFrom i + 1 arr n (tail list)
//                      }
// in
//    copyFrom 0 elems (allocate memory for (length t) * sizeOf t)
//
// (lambda lifted to avoid the need for a closure)

template <class T, Int length>
class Array {
private:

// Constructors declared but not defined
// due to issues with vector constructor iterator
//   Array(Void);
//   Array(const Array<T, length>& src);

public:
   inline
   Void Array$(UInt16 nElem) {
      for (UInt16 i = 0; i < nElem; i++)
         new(  &(*this)[i]  ) T();
   }
   inline
   Void Array$(UInt16 nElem, const Array<T, length>& src,
               const Env& srcEnv, Env& env, MSA& msa) {
      for (UInt16 i = 0; i < nElem; i++)
         new(&(*this)[i]) T(src[i], srcEnv, env, msa);
   }
   inline
   Void destroy(UInt16 nElem, Env& env, MSA& msa) {
      for (UInt16 i = 0; i < nElem; i++)
         (*this)[i].destroy(env, msa);
   }

   inline
   static size_t elemSize(Void) {
      return sizeof(Array<T, length>) / length;
   }
   inline
   static Array<T,length>* new$(UInt16 nElem, MSA& msa) {
      return (Array<T,length>*)msa.alloc(elemSize() * nElem);
   }

   inline T& operator [](UInt16 index) {
      return _elem[index];
   }
   inline const T& operator [](UInt16 index) const {
      return _elem[index];
   }

   inline
   Void print(UInt16 nElem, ostream& os, const Env& env) const {
      os << '[';
      for (UInt16 i = 0; i < nElem; i++) {
         if (i > 0 )
            os << ',';
         (*this)[i].print(os, env);
      }
      os << ']';
   }

   static Label construct(argVM);
   static Label construct$(argVM);
   static Label copyFrom(argVM);
   static Label copyFrom_l1(argVM);
   static Label copyFrom_l2(argVM);
protected:
   T  _elem[length];
};


#define elems_env    sizeof(Env*)
#define elems        elems_env+sizeof(Expr)
#define n            elems+sizeof(Int)
#define stackDepth   n

template <class T, Int length>
inline Label Array<T, length>::construct(argVM) {
   checkArgs("Array<T, length>::construct");
   jump(construct$);
}

// Entry - skipping argument checks

template <class T, Int length>
inline Label Array<T, length>::construct$(argVM) {
   Array<T, length>* ptr = new$(local(n, Int), *consMSA);
   ptr->Array$(local(n, Int));

// push pointer to array as Void* due to limitations of macro

   push(ptr, Void*);

#define arr          (n+sizeof(Void*))
#undef stackDepth
#define stackDepth  arr

   push(0, Int); // i
   jump(label(copyFrom));
}

#define i            (arr+sizeof(Int))
#undef stackDepth
#define stackDepth  i

template <class T, Int length>
inline Label Array<T, length>::copyFrom(argVM) {
   pushLabel(label(copyFrom_l1));
   emptyStack();
   enterLocal(elems, elems_env);
}

template <class T, Int length>
inline Label Array<T, length>::copyFrom_l1(argVM) {
   ConsExpr* list;
   if ((list = checkExprList()) != NULL) {
      pushLabel(label(copyFrom_l2));
      emptyStack();
      local(elems, Expr) = list->tail();    // Prepare for tail recursion
      enter(list->head(), rEnv); 
   } else {
      rPtr = local(arr, Void*);

      dropStack();

//    Currently, return type is always coerced

      jump(popLabel());
   }
}

template <class T, Int length>
inline Label Array<T, length>::copyFrom_l2(argVM) {

// Construct from return value and tail call copyFrom
/*
   returnCopy(&(*((Array<T, length>*)local(arr, Void*)))[local(i, Int)],
              *consEnv, *consMSA n_vm);
*/
   local(i, Int) += 1;
   jump(label(copyFrom));
}

#undef stackDepth
#undef elems
#undef n
#undef arr
#undef i

#endif /* IVORY_ARRAY_H_DEFINED */
