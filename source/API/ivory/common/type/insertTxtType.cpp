/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertTxtType.cpp
 *
 * Module:  Ivory common (type)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 July 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of insertTxtType function. Text insertion of a type.
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

#include <sstream>
#include "ivory/char.h"
#include "ivory/env.h"
#include "ivory/streams.h"
#include "ivory/type.h"
#include "ivory/void.h"

// insertTxtType :: OutputStream Char -> Type -> Void

defineBuiltInFn_2_args(insertTxtType,
   builtInAp(typeCon(OutputStream), typeCon(Char)), typeCon(Type), typeCon(Void),
   os, OutputStream_Char,
   type_, Type)

   std::ostringstream strm;
   printCanonicalType(type_, strm, *type_Env);
   const std::string str = strm.str();
   putString(*os, *osEnv, str.c_str() n_vm);

   drop_Stack(insertTxtType);
   returnVoid();
endBuiltInFn
