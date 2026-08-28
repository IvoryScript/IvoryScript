/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ref.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of showRef built-in function
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

#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/ref.h"
#include "ivory/trace.h"
#include "ivory/void.h"

// showRef :: showRef -> Void

defineBuiltInFn(showRef,
                builtInAp2(ARROW,
                           typeCon(Ref),
                           typeCon(Void)))
#define ref_env  sizeof(Env*)
#define ref      (ref_env+sizeof(Ref*))
#define stackDepth  ref
{
   checkArgs("showRef");
   jump(altEntry(showRef));
}

defineAltEntry(showRef)
{
   local(ref, Ref).print(*outStrm, *local(ref_env, Env*));
   dropStack();
   returnVoid();
}

/*----------------------------------------------------------------------------*/

// instance Show Ref where
//    show r = showRef r

classMethodDefn_n(1, show, showRef)
{
   push(rEnv, Env*);
   push(rRef, Ref);
   jump(altEntry(showRef));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_Ref,
               Show,
               typeCon(Ref),
               builtInCons(methodDefn_1,
                           Nil));



