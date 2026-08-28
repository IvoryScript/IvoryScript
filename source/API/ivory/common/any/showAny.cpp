/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showAny.cpp
 *
 * Module:  Ivory common (Any)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of showAny built in function
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

#include "ivory/any.h"
#include "ivory/char.h"
#include "ivory/env.h"
#include "ivory/streams.h"
#include "ivory/void.h"

// showAny :: Any -> Void

defineBuiltInFn_1_arg(showAny,
   typeCon(Any), typeCon(Void),
   any, Any)
   Type type = any.type_();
   TypeDescr* typeDescr = type < builtInTypeCount
      ? builtInTypeTable().typeDescr(type)
      : rEnv->typeTable().typeDescr(type - builtInTypeCount);
   OutputStream_Char stdoutTxtStream = { builtInFn(showChar), builtInDataCon(Void), NULL };
   typeDescr->_insertTxtFn(typeDescr, &stdoutTxtStream, builtInEnv(), any.valPtr(), *rEnv n_vm);
   returnVoid();
endBuiltInFn