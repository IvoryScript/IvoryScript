/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showBool.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 7 November 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built in function to show a Bool value.
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

#include "ivory/bool.h"
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/void.h"

static const char* ident(Tag tag) {
   switch (tag) {
      case FALSE: return "False";
      case TRUE:  return "True";
      default:    return "???";
   }
}

// showBool :: Bool -> Void

defineBuiltInFn_1_arg(showBool,
   typeCon(Bool), typeCon(Void),
   q, Tag)
   *outStrm << ident(q);
   returnVoid();
endBuiltInFn

/*----------------------------------------------------------------------------*/

// instance Show Bool where
//    show b = showBool b

classMethodDefn_n(1, show, showBool) {
   push(rTag, Tag);
   jump(altEntry(showBool));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_Bool,
   Show,
   typeCon(Bool),
   builtInCons(methodDefn_1,
      Nil));
