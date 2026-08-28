/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    eqPtr.cpp
 *
 * Module:  Ivory common (ptr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Primitive function to compare two pointers
 *
 * Method:
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

#include <string.h>
#include "ivory/bool.h"
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/ptr.h"

declareTypeCon(Plain);

// eqPtr :: Ptr a -> Ptr a -> Bool

defineBuiltInFn(eqPtr,
                builtInAp2(ARROW,
                           builtInAp(typeCon(Ptr),
                                     fromName(builtInName(a))),
                           builtInAp2(ARROW,
                                      builtInAp(typeCon(Ptr),
                                                fromName(builtInName(a))),
                                      typeCon(Bool))))
#define ptr2_env     sizeof(Env*)
#define ptr2         (ptr2_env+sizeof(Ptr))
#define stackDepth   ptr2
{
   checkArgs("eqPtr");
   jump(altEntry(eqPtr));
}

defineAltEntry(eqPtr) {
   Ptr ptr2_ = (Ptr)local(ptr2, Ptr);
   dropStack();
   returnBool(rPtr == ptr2_);
}


// eqPlainPtr :: Plain (Ptr a) -> Plain (Ptr a) -> Bool

defineBuiltInFn(eqPlainPtr,
   builtInAp2(ARROW,
      builtInAp(typeCon(Plain), builtInAp(typeCon(Ptr),
         fromName(builtInName(a)))),
      builtInAp2(ARROW,
         builtInAp(typeCon(Plain), builtInAp(typeCon(Ptr),
            fromName(builtInName(a)))),
         typeCon(Bool))))
#define ptr2_env     sizeof(Env*)
#define ptr2         (ptr2_env+sizeof(Ptr))
#define stackDepth   ptr2
{
   checkArgs("eqPlainPtr");
   jump(altEntry(eqPlainPtr));
}

defineAltEntry(eqPlainPtr) {
   Ptr ptr2_ = (Ptr)local(ptr2, Ptr);
   dropStack();
   returnBool(rPtr == ptr2_);
}

// instance Eq Ptr where
//    (=) x y = eqPtr x y

classOperatorDefn_n(1, "(=)", eqPtr)
{
   push(rPtr, Ptr);
   jump(altEntry(eqPtr));
}

/*----------------------------------------------------------------------------*/

declareClass(Eq);

defineInstance(Eq_Ptr,
               Eq,
               builtInAp(typeCon(Ptr),
                  fromName(builtInName(a))),
               builtInCons(methodDefn_1,
                           Nil));
