/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showInt.cpp
 *
 * Module:  Ivory common (int)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of showInt function
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
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/trace.h"
#include "ivory/void.h"

// showInt :: Int -> Void

defineBuiltInFn_1_arg(showInt,
   typeCon(Int), typeCon(Void),
   i, Int)
   *outStrm << i;
returnVoid();
endBuiltInFn

/*----------------------------------------------------------------------------*/

// instance Show Int where
//    show i = showInt i

classMethodDefn_n(1, show, showInt) {
   push(rInt, Int);
   jump(altEntry(showInt));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_Int,
               Show,
               typeCon(Int),
               builtInCons(methodDefn_1,
                           Nil));

