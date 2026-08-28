/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    formOf.cpp
 *
 * Module:  Ivory common (repr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to return the form of an expression.
 *
 * Call format:
 *
 *    res = formOf(e)
 *
 *       res   (Int)    - Returned form
 *       e     (Expr)   - Expression
 *
 * Method:
 *
 *    Returns the form either from unboxed values or a cell tag.
 *
 * Errors:
 *
 *    Errors are signalled by a return of ERROR.
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

#include "ivory/repr.h"

ExprEnumVal formOf(Expr expr) {
   switch (expr.tag()) {
      case PTR_TAG: {
         if (expr == ERROR)
            return ERROR;
         Expr tag = toCell(expr).tag();
         return !isPtr(tag)
            ? tag
            : (isType((static_cast<const CellInfo*>(tag))->form())
                 ? (Expr)DATA
                 : (static_cast<const CellInfo*>(tag))->form());
      }

      case ENUM_VAL_TAG:
         if (static_cast<ExprEnumVal>(expr) < TUPLE_MIN)
            return static_cast<ExprEnumVal>(expr);
         else if (static_cast<ExprEnumVal>(expr) <= TUPLE_MAX)
            return TUPLE;
         else if (static_cast<ExprEnumVal>(expr) <= NAME_MAX)
            return NAME;
         else if (static_cast<ExprEnumVal>(expr) <= TYPE_MAX)
            return TYPE;
         else if (static_cast<ExprEnumVal>(expr) <= TAG_MAX)
            return TAG;
         else if (static_cast<ExprEnumVal>(expr) <= CHAR_MAX)
            return CHAR;
         else
            return INT;


      default:
         return ERROR;
   }
}

// Predicate to check that the form of an expression is one contained in a specified array.

Bool formExistsIn(Expr expr, UInt n, UInt32* forms) {
	UInt32 form = formOf(expr);
	for (UInt i = 0; i < n; i++)
		if (form == forms[i])
			return TRUE;
	return FALSE;
}
