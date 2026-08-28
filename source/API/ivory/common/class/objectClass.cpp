/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    object.cpp
 *
 * Module:  Ivory class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Object class
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

#include "ivory/basicADS.h"
#include "ivory/objectClass.h"
#include "ivory/object.h"
#include "ivory/void.h"

classMethodRef(cm1, "createObject");

declareEntry(createObject$$);

// class Object a where
//    createObject :: Expr -> Type -> Type -> a -> Ref
//    mapProcObjects :: Expr -> Type -> a -> Void

classMethodDecl_n(1, createObject, NULL,
                  builtInAp2(ARROW,
                             typeCon(Expr),
                             builtInAp2(ARROW,
                                        typeCon(Type),
                                        builtInAp2(ARROW,
                                                   typeCon(Type),
                                                   builtInAp2(ARROW,
                                                              fromName(builtInName(a)),
                                                              typeCon(Ref))))))
#define type_        sizeof(Int)
#define reprType     (type_+sizeof(Int))
#define x_env        reprType+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define e_env        x+sizeof(Env*)
#define e            e_env+sizeof(Expr)
#define stackDepth   e
{
   checkArgs("createObject");
   jump(altEntry(createObject));
}

defineAltEntry(createObject) {
   push((Cell*)classMethod(cm1), Cell*);
   pushLabel(entry(createObject$$));
   emptyStack();
   enterLocal(x, x_env);
}

defineEntry(createObject$$)
{
   local(x_env, Env*) = local(e_env, Env*);             
   local(x, Expr) = local(e, Expr);              // Rearrange stack
   dropStack();

   ClassMethod& classMethod = cellBody(*pop(Cell*), ClassMethod);
   Type type = rType;
   if (type == builtInType(Ref))
   { 
      Type t1 = (Type)pop(Int);    // Rearrange stack B
      Type t2 = (Type)pop(Int);
      push((ADS_Instance*)rEnv, ADS_Instance*);
      push(rRef, Ref);
      push((Int)t2, Int);
      push((Int)t1, Int);
      type = lockObject(rRef, *(BasicADS_Instance*)rEnv)->_type;
   }
   jump(classMethod.dispatch(type));
}
#undef x
#undef x_env
#undef e
#undef type_
#undef reprType
#undef stackDepth

classMethodRef(cm2, "mapProcObjects");

declareEntry(mapProcObjects$$);

classMethodDecl_n(2, mapProcObjects, NULL,
                  builtInAp2(ARROW,
                             typeCon(Expr),
                             builtInAp2(ARROW,
                                        typeCon(Type),
                                        builtInAp2(ARROW,
                                                   fromName(builtInName(a)),
                                                   typeCon(Void)))))


#define type_env     sizeof(Env*)
#define type_        (type_env+sizeof(Int))
#define proc_env     type_+sizeof(Env*)
#define proc         proc_env+sizeof(Expr)
#define x_env        proc+sizeof(Env*)
#define x            x_env+sizeof(Expr)
#define stackDepth   x
{
   checkArgs("mapProcObjects");
   jump(altEntry(mapProcObjects));
}

defineAltEntry(mapProcObjects)
{
   push((Cell*)classMethod(cm2), Cell*);
   pushLabel(entry(mapProcObjects$$));
   emptyStack();
   enterLocal(x, x_env);
}

defineEntry(mapProcObjects$$)
{
//   local(x_env, Env*) = local(e_env, Env*);
//   local(x, Expr) = local(e, Expr);              // Rearrange stack
   dropStack();

   ClassMethod& classMethod = cellBody(*pop(Cell*), ClassMethod);
   Type type = rType;
   if (type == builtInType(Ref))
   { 
      Type t = (Type)pop(Int);    // Rearrange stack B
      Env* t_env = pop(Env*);
      push((ADS_Instance*)rEnv, ADS_Instance*);
      push(rRef, Ref);
      push(t_env, Env*);
      push((Int)t, Int);
      type = lockObject(rRef, *(BasicADS_Instance*)rEnv)->_type;
   }
   jump(classMethod.dispatch(type));
}

/*----------------------------------------------------------------------------*/

defineClass(Nil,
            BasicObject,
            a,
            builtInCons(methodDecl_1,
               builtInCons(methodDecl_2,
                  Nil)),
            Nil);
