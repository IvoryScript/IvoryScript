/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    enter.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to enter an expression.
 *
 * Call format:
 *
 *    ret = enter$(expr, env n_vm)
 *
 *       ret   (Label)     - Returned continuation label
 *       expr  (Expr)      - Expression to evaluate (enter)
 *       n_vm  (VM&)       - Virtual machine or absent.
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

#include "nameTable.h"
#include "ivory/char.h"
#include "ivory/int.h"
#include "ivory/name.h"
#include "ivory/nameExprMap.h"
#include "ivory/ref.h"

Label enter$(Expr expr, Env* env argN_VM) {
   assert(expr.tag() != PTR_TAG, "enter$: invalid tag");

repeat:
   if (isValue(expr)) {
      if (expr <= TUPLE_MAX)
         runTimeError("enter$: invalid unboxed value")
      else if (expr <= NAME_MAX) {
         Name name = toName(expr);
         if (name < builtInNameCount &&
             builtInNameMap().lookUp(name,
                                     builtInNameTable().hashVal(name),
                                     expr))
            if (!isPtr(expr))
               goto repeat;
            else {
               cellEnv = env;
               cell = expr;
               jump((static_cast<const CellInfo*>(cell->tag()))->entry_());
            }
         else
            returnName(name, env);
      }
      else if (expr <= TYPE_MAX)
         returnType(toType(expr), env)
      else if (expr <= CHAR_MAX)
         returnChar(toChar(expr))
      else
         returnInt((Int)(expr >> TAG_BITS) -
                   (Int)((UInt)INT_ZERO >> TAG_BITS))
   }
   jump(NULL);
}
