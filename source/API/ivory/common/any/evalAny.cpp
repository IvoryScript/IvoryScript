/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    evalAny.cpp
 *
 * Module:  Ivory common
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Built in function to check the denoted type of an 'Any' value
 *    before fully evaluating it using the assocaited type method.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/any.h"
#include "ivory/type.h"

// evalAny :: Any -> Type -> a

defineBuiltInFn_2_args(evalAny,
                       typeCon(Any),
                       typeCon(Type),
                       fromName(builtInName(a)),
                       any, Any,
                       expectedType, Type)

   Type type = any.type_();
   TypeDescr* typeDescr = type < builtInTypeCount
      ? builtInTypeTable().typeDescr(type)
      : anyEnv->typeTable().typeDescr(type - builtInTypeCount);

   if (typeDescr == NULL ||
       typeDescr->denotedType() == NULL_TYPE ||
       !eqType_(typeDescr->denotedType(), anyEnv->typeTable(),
                expectedType, expectedTypeEnv->typeTable()))
      throw(RunTimeError("evalAny: type mismatch"));

   if (typeDescr->_evalFn == NULL)
      throw(RunTimeError("evalAny: type has no eval method"));

   typeDescr->_evalFn(typeDescr, any.valPtr(), *anyEnv n_vm);

   drop_Stack(evalAny);
   jump(popLabel());

endBuiltInFn
