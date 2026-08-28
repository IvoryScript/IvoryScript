/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertBinExpr.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 9 February 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to insert an expression into a byte output stream.
 *
 * Call format:
 *
 *    insertBinExpr(os, osEnv, expr, exprEnv n_vm)
 *
 *       os       (OutputStream_Byte&) - Output stream
 *       osEnv    (Env&)               - Output stream environment
 *       expr     (Expr)               - Expression to insert
 *       exprEnv  (Env&)               - Expression environment
 *       n_vm                          - Virtual machine or absent
 *
 * Method:
 *
 *    Handles insertion of both unboxed and boxed expression values.
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

#include "ivory/machine.h"
#include "ivory/int.h"
#include "ivory/streams.h"
#include "ivory/typeTable.h"

#define EXPR_BOXED   0
#define EXPR_NAME    1
#define EXPR_TYPE    2
#define EXPR_INT     3
#define EXPR_CHAR    4
#define EXPR_TAG     5

static Void insertBinExprName(InsertArchive& ia, Name name, Env& nameEnv) {
   ia << (Name)(name >= builtInNameCount
                   ? name - builtInNameCount
                   : nameEnv.nameTable().lookUpByAlt(
                        name, builtInNameTable().hashVal(name))->_name);
}

static Void insertBinExprType(InsertArchive& ia, Type type, Env& typeEnv) {
   ia << (Type)(type >= builtInTypeCount
                   ? type - builtInTypeCount
                   : typeEnv.typeTable().lookUpByAlt(
                        type, builtInTypeTable().hashVal(type)));
}

Void insertBinExpr(OutputStream_Byte& os, Env& osEnv, Expr expr, Env& exprEnv argN_VM) {
   InsertArchive ia(os._outputFn, &osEnv);

   switch (expr.tag()) {
      case PTR_TAG:
         if (expr != ERROR) {
            ia << (Byte)EXPR_BOXED;
            insertBinCell(os, osEnv, *(Cell*)expr, exprEnv n_vm);
            return;
         }
         break;

      case ENUM_VAL_TAG: {
         ExprEnumVal enumVal = static_cast<ExprEnumVal>(expr);
         if (enumVal <= TUPLE_MAX)
            break;
         else if (enumVal <= NAME_MAX) {
            ia << (Byte)EXPR_NAME;
            insertBinExprName(ia, toName(expr), exprEnv);
            return;
         }
         else if (enumVal <= TYPE_MAX) {
            ia << (Byte)EXPR_TYPE;
            insertBinExprType(ia, toType(expr), exprEnv);
            return;
         }
         else if (enumVal <= TAG_MAX) {
            ia << (Byte)EXPR_TAG;
            ia << (Tag)toTag(expr);
            return;
         }
         else if (enumVal <= CHAR_MAX) {
            ia << (Byte)EXPR_CHAR;
            ia << toChar(expr);
            return;
         }
         else {
            ia << (Byte)EXPR_INT;
            ia << (Int32)toInt(expr);
            return;
         }
      }

      default:
         break;
   }

   ivoryError("insertBinExpr: invalid expression");
}
