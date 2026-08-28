/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    expr.cpp
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
 *    Construct a copy of an expression in the context of a given source
 *    environment mapped to the context of a given ADS.
 *
 * Call format:
 *
 *    res = copyExpr(src, env, adsi, msa)
 *
 *       res      (Expr)         - Returned expression copy
 *       env      (Env&)         - Source environment
 *       adsi     (ADSInstance&) - Destination ADS instance
 *       msa      (MSA&)         - Memory storage allocator
 *
 * Method:
 *
 *    Assign primitive types or invoke virtual function.
 *
 * Errors:
 *
 *    Errors are signalled by a return of ERROR.
 *
 * Notes:
 *
 *    Function application (AP) is currently mapped to (MKAP)
 *    This is to ensure that CAF expressions are re-evaluated.
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
#include "ivory/machine.h"
#include "ivory/object.h"

defineTypeCon(Expr);

defineTypeCon(Exp);

static Void init_Expr(TypeDescr* typeDescr);
defineType(Expr, typeCon(Expr), init_Expr);

Void init_Expr(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Expr);
}

Void Expr::destroy(Env& env, MSA& msa argN_VM) {
   switch (tag()) {
      case PTR_TAG: {
         Expr tag = toCell(*this).tag();
         if (isPtr(tag)) {
            if (((const CellInfo*)tag)->cellDestroyFn() != NULL)
               (*((const CellInfo*)tag)->cellDestroyFn())(toCell(*this), env, msa n_vm);
            return;
         }
         break;
      }

      case ENUM_VAL_TAG:
         if (_repr._enumVal >= NAME_MIN && _repr._enumVal <= NAME_MAX)
            dropName_(toName(*this), env.nameTable());
         return;

      default:
         break;
   }
   throw(RunTimeError("Expr::destroy: unknown form"));
}

// Modify macros for constant objects

Expr::Expr(Expr src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   switch (src.tag()) {
      case PTR_TAG: {
         Expr tag = toCell(src).tag();
         if (isPtr(tag)) {
            *this = mapClosure(src, srcEnv, env n_vm);
            return;
         }
			if (tag == Expr(AP)) {
				*this = mkAp(Expr(fun(src), srcEnv, env, msa n_vm),
                         Expr(arg(src), srcEnv, env, msa n_vm), msa);
				return;
			}
			if (tag == Expr(TYPE_CON)) {
				*this = src;
				return;
			}
         break;
      }

      case ENUM_VAL_TAG:
         if (src._repr._enumVal < NAME_MIN || src._repr._enumVal > NAME_MAX)
            *this = src;
         else
            *this = fromName(copyName(toName(src),
                             srcEnv.nameTable(), env.nameTable()));
         return;

      default:
         break;
   }
   ivoryError("Expr::Expr unknown form");
   traceExpr(src, env);
   throw(RunTimeError(""));
   *this = ERROR;
}


#ifdef localConst
#undef localConst
#endif
#define localConst const

Void Expr::assign(const Expr& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   destroy(env, msa n_vm);
   *this = Expr(src, srcEnv, env, msa n_vm);
}

Void Expr::print(ostream& os, const Env& env) const {
   printExpr(*this, os, env);
}
