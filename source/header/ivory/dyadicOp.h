/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    dyadicOp.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *     Definitions for use with Dyadic operators
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

#ifndef IVORY_DYADIC_OP_H_DEFINED
#define IVORY_DYADIC_OP_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/class.h"
#include "ivory/machine.h"
#include "ivory/streams.h"

#define dyadicOp_n_BUILT_IN_PAP(n,opSpelling,name,t,tRes,rReg,expr)                           \
/* Built-in: name##tRes :: t -> t -> tRes */                                                  \
defineBuiltInFn_2_args_BUILT_IN_PAP(name##t,                                                  \
   typeCon(t), typeCon(t), typeCon(tRes),                                                     \
   x, t,                                                                                      \
   y, t)                                                                                      \
   drop_Stack(name##t);                                                                    \
   return##tRes(expr);                                                                        \
endBuiltInFn     \
   classOperatorDefn_n(n, opSpelling, name##t) {                                            \
      jump(popLabel());                                                                     \
   }                                                                                        \

#define dyadicOp_n_CLOSURE(n,opSpelling,name,t,tRes,rReg,expr)                                \
/* Built-in: name##tRes :: t -> t -> tRes */                                                  \
defineBuiltInFn_2_args(name##t,                                                              \
   typeCon(t), typeCon(t), typeCon(tRes),                                                     \
   x, t,                                                                                      \
   y, t)                                                                                      \
defineStackDepth(name##$PAP_enter, stackFPSize(t));   \
   drop_Stack(name##t);                                                                    \
   return##tRes(expr);                                                                        \
endBuiltInFn     \
   classOperatorDefn_n(n, opSpelling, name##t) {                                            \
      jump(popLabel());                                                                     \
   }                                                                                        \

#define dyadicOp_n_Double   dyadicOp_n_BUILT_IN_PAP
#define dyadicOp_n_Float    dyadicOp_n_BUILT_IN_PAP
#define dyadicOp_n_Int      dyadicOp_n_BUILT_IN_PAP
#define dyadicOp_n_Bits32   dyadicOp_n_BUILT_IN_PAP
#define dyadicOp_n_Char     dyadicOp_n_BUILT_IN_PAP
#define dyadicOp_n_Bool     dyadicOp_n_CLOSURE
#define dyadicOp_n_String   dyadicOp_n_CLOSURE
#define dyadicOp_n_UTC      dyadicOp_n_CLOSURE

#define dyadicOp_n(n,opSpelling,name,t,tRes,rReg,expr)                                        \
dyadicOp_n_##t(n,opSpelling,name,t,tRes,rReg,expr)

#define dyadicOp(opSpelling, name, t, tRes, rReg, expr) \
   dyadicOp_n(1, opSpelling, name, t, tRes, rReg, expr)

#define popPartialEnv(t,n)\
cellBody(*rCell,t)=map##t(stack(0,t),*stack(sizeof(t),Env*),*consEnv);\
drop(n+sizeof(Env*))

#define pushPartialEnv(t,n)\
stackAlloc(n+sizeof(Env*));\
stack(n,Env*)=cellEnv;\
stack(0,t)=cellBody(*cell,t)

#define dyadicOpEnv_n(n,op,name,t,tRes,rReg,e)\
declareEntry(name##t##_PAP);\
static CellInfo cellInfo##name##t##_PAP(CLOSURE,0,entry(name##t##_PAP));\
defineBuiltInFn(name##t,\
builtInAp2(ARROW,\
typeCon(t),\
builtInAp2(ARROW,\
typeCon(t),\
typeCon(tRes)))){\
if(depth()==stackDepth)\
jump(altEntry(name##t));\
else{\
rCell=new(sizeof(t),*consMSA)Cell(&cellInfo##name##t##_PAP);\
popPartialEnv(t,x_-y_);\
rEnv=consEnv;\
jump(popLabel());}}\
defineEntry(name##t##_PAP){\
pushPartialEnv(t,x_-y_);\
jump(altEntry(name##t));}\
declareLabel(name##t##_l1);\
static CellInfo cellInfo##n##$##t(CLOSURE,0,label(name##t##_l1));\
struct Closure##n{t _x;};\
classOperatorDefn_n(n,op,name##t){\
Cell* c= new(sizeof(struct Closure##n),*consMSA)Cell(&cellInfo##n##$##t);\
cellBody(*c,struct Closure##n)._x=(t)r##rReg;\
rPtr=c;\
jump(popLabel());}\
defineLabel(name##t##_l1){\
t y;\
t x=cellBody(*cell,struct Closure##n)._x;\
return##tRes(e,consEnv);}\
defineAltEntry(name##t){\
t x=local(x_,t);\
t y=local(y_,t);\
dropStack();\
return##tRes(e,consEnv);}

#define dyadicOpEnv(op,name,t,tX,rXReg,tY,rYReg,tRes,e)\
dyadicOpEnv_n(1,n,op,name,t,tX,rXReg,tY,rYReg,tRes,e)

#endif /* IVORY_DYADIC_OP_H_DEFINED */
