/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    enterReturn.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 June 2005
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to enter return state state for special expression.
 *
 * Call format:
 *
 *    enterReturn(expr, env n_vm)
 *
 *       expr  (Expr)      - Expression to evaluate (enter)
 *       env   (Env*)      - Environment (in case Name or Type)
 *       n_vm  (VM&)       - Virtual machine or absent
 *
 * Method:
 *
 *    Handles evaluation of unboxed values.
 *
 * Errors:
 *
 *    None
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

#include "ivory/char.h"
#include "ivory/int.h"
#include "ivory/name.h"
#include "ivory/ref.h"

Void enterReturn(Expr expr, Env* env argN_VM) {
   assert(expr.tag() != PTR_TAG, "enterReturn: invalid tag");
   if (expr.repr() <= NAME_MAX) {
      rName = toName(expr);
      rEnv = env;
   }
   else if (expr.repr() <= TYPE_MAX) {
      rType = toType(expr);
      rEnv = env;
   } else if (expr.repr() <= TAG_MAX)
      rTag = toTag(expr);
   else if (expr.repr() <= CHAR_MAX)
      rChar = toChar(expr);
   else
      rInt = (Int)(expr.repr() >> TAG_BITS) -
               (Int)((UInt)INT_ZERO >> TAG_BITS);
}
