/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    list.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with list data type
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

#ifndef IVORY_LIST_H_DEFINED
#define IVORY_LIST_H_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/machine.h"
#include "ivory/type.h"

declareBuiltInTypeCon(List);

declareBuiltInType(ExprList);

declareBuiltInNullaryDataCon(Nil);
declareBuiltInNullaryDataCon(Cons);

#define Nil  builtInDataCon(Nil)
#define Cons builtInDataCon(Cons)

#define NIL_TAG   0
#define CONS_TAG  1

template <class T>
class Cons_ {
public:
   Cons_(T head, Expr tail)
      : _head(head), _tail(tail) {
   }
   inline Void* operator new(size_t size, MSA& msa) {
      Void *ptr = msa.alloc(size);
      return ptr;
   }
   inline Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {
      msa.free(ptr);
   }
   inline Void operator delete(Void* ptr1, Void* ptr2) {
   }
#else
   inline Void operator delete(Void* ptr) {
   }
#endif
   inline T head(Void) const { return _head; };
   inline Expr tail(Void) const { return _tail; }

protected:
   T     _head;
   Expr  _tail;
};
/*
template <class T>
UInt lengthList(Void* l) {
   UInt res = 0;
   Void* elems = l;
   while (l != NULL) {

   }
}
*/

typedef Cons_<Expr> ConsExpr;

#define returnNil(){\
rPtr=NULL;\
jump(popLabel());}

#define returnCons(cons,env){\
rPtr=cons;\
rEnv=(env);\
jump(popLabel());}

// Commented out rTag assignment. T.B.D. check if required now
/*
#define returnCons(cons,env){\
rPtr=cons;\
rTag=CONS_TAG;\
rEnv=(env);\
rType=type(ExprList);\
jump(popLabel());}
*/

extern Void updateExprList$(argVM);
#define updateExprList()\
if(updatePtr!=NULL){updateExprList$(vm);updatePtr=NULL;}

#define checkExprList() checkExprList$(vm)

declareBuiltInFn(condCons);

declareBuiltInFn(noDups);

declareBuiltInFn(hasElement);

declareBuiltInFn(head);

declareBuiltInFn(lengthExprList);

declareBuiltInFn(tail);

declareBuiltInFn(unique);

#define isListTypeSig(typeSig) (isAp(typeSig)&&(fun(typeSig)==typeCon(List)))

#define cons(x,xs,msa) Expr(mkCons((x),(xs),(msa)))
#define builtInCons(x,xs) cons((x),(xs),builtInMSA())

#define hd(xs) fst(xs)
#define tl(xs) snd(xs)

extern CellInfo cellInfo$NilExprList;
#define isNil(expr) (isPtr(expr)&&(static_cast<const CellInfo*>(toCell(expr).tag()))==&cellInfo$NilExprList)
#define isCons(expr) (isPtr(expr)&&(static_cast<const CellInfo*>(toCell(expr).tag()))->form().repr()==CONS)
#define isList(expr) (isNil(expr)||(isCons(expr))

#define mapBasic(xs,step) {for (Expr x =(xs);x != Nil;x = tl(x))(step);}

#define mapProc(f,xs) mapBasic((xs),(f)(hd(x)))
#define mapProcVM(f,xs) mapBasic((xs),(f)(hd(x) n_vm))
#define mapProc1(f,xs,a) mapBasic((xs),(f)(hd(x),(a)))
#define mapProc1VM(f,xs,a) mapBasic((xs),(f)(hd(x),(a) n_vm))
#define mapProc2(f,xs,a,b) mapBasic((xs),(f)(hd(x),(a),(b)))
#define mapProc3(f,xs,a,b,c) mapBasic((xs),(f)(hd(x),(a),(b),(c)))
#define mapProc4(f,xs,a,b,c,d) mapBasic((xs),(f)(hd(x),(a),(b),(c),(d)))
#define mapProc5(f,xs,a,b,c,d, e) mapBasic((xs),(f)(hd(x),(a),(b),(c),(d),(e)))

#define mapModify(xs,step)	mapBasic(xs,hd(x)=step)

#define mapOver(f,xs) mapModify((xs),(f)(hd(x)))
#define mapOver1(f,xs,a) mapModify((xs),(f)(hd(x),(a)))
#define mapOver2(f,xs,a,b) mapModify((xs),(f)(hd(x),(a),(b)))
#define mapOver3(f,xs,a,b,c) mapModify((xs),(f)(hd(x),(a),(b),(c)))
#define mapOver4(f,xs,a,b,c,d) mapModify((xs),(f)(hd(x),(a),(b),(c),(d)))
#define mapOver5(f,xs,a,b,c,d,e) mapModify((xs),(f)(hd(x),(a),(b),(c),(d),(e)))
#define mapOver1VM(f,xs,a) mapModify((xs),(f)(hd(x),(a) n_vm))
#define mapOver2VM(f,xs,a,b) mapModify((xs),(f)(hd(x),(a),(b) n_vm))

extern Expr concat(Expr xs1, Expr xs2, MSA& msa);

extern Expr reverse(Expr xs, MSA& msa);

extern Cell* mkCons(Expr x, Expr xs, MSA& msa);

extern UInt length(Expr xs);

extern ConsExpr* checkExprList$(argVM);

declareBuiltInFn(map);

declareBuiltInFn(mapProc);

declareBuiltInFn(showList);

#endif /* IVORY_LIST_H_DEFINED */
