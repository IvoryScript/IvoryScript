/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertTxtFloat.cpp
 *
 * Module:  Ivory common (float)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 July 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of insertTxtFloat function. Text insertion of a float value.
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
#include "ivory/float.h"
#include "ivory/streams.h"
#include "ivory/void.h"

// insertTxtFloat :: OutputStream Char -> Float -> Void

defineBuiltInFn_2_args(insertTxtFloat,
   builtInAp(typeCon(OutputStream), typeCon(Char)), typeCon(Float), typeCon(Void),
   os, OutputStream_Char,
   f, Float)

   std::ostringstream strm;
   strm << f;
   const std::string str = strm.str();
   putString(*os, *osEnv, str.c_str() n_vm);

   drop_Stack(insertTxtFloat);
   returnVoid();
endBuiltInFn