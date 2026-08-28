/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinExpr.cpp
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
 *    Function to extract an expression from a byte input stream.
 *
 * Call format:
 *
 *    extractBinExpr(is, isEnv n_vm)
 *
 *       is       (InputStream_Byte&)  - Input stream
 *       isEnv    (Env&)               - Input stream environment
 *       n_vm                          - Virtual machine or absent
 *
 * Method:
 *
 *    Handles extraction of both unboxed and boxed expression values.
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

Expr extractBinExpr(InputStream_Byte& is, Env& isEnv argN_VM) {
   ExtractArchive ea(is._inAct, &isEnv);
   Byte kind;
   ea >> kind;
   switch (kind) {
      case EXPR_BOXED:
         return extractBinCell(is, isEnv n_vm);

      case EXPR_NAME: {
         Name name;
         ea >> name;
         Name builtInName = consEnv->nameTable().altName(name);
         return fromName(builtInName == NULL_NAME
                            ? name + builtInNameCount
                            : builtInName);
      }

      case EXPR_TYPE: {
         Type type;
         ea >> type;
         Type builtInType = consEnv->typeTable().altType(type);
         return fromType(builtInType == NULL_NAME
                            ? type + builtInTypeCount
                            : builtInType);
      }

      case EXPR_INT: {
         Int32 i;
         ea >> i;
         return fromInt((Int)i, *consMSA);
      }

      case EXPR_CHAR: {
         Char c;
         ea >> c;
         return fromChar(c);
      }

      case EXPR_TAG: {
         Tag tag;
         ea >> tag;
         return fromTag(tag);
      }

      default:
         ivoryError("extractBinExpr: invalid expression kind");
         return ERROR;
   }
}
