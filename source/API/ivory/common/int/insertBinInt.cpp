/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertBinInt.cpp
 *
 * Module:  Ivory common (int)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 November 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of insertBinInt function. Binary serialisation of an
 *    integer.
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

#include "ivory/byte.h"
#include "ivory/int.h"
#include "ivory/streams.h"
#include "ivory/void.h"

defineBuiltInFn_2_args(insertBinInt,
   builtInAp(typeCon(OutputStream), typeCon(Byte)), typeCon(Int), typeCon(Void),
   os, OutputStream_Byte,
   i, Int)

   InsertArchive ia(os->_outputFn, osEnv);
   ia << (Int32)i;

   drop_Stack(insertBinInt);
   returnVoid();
endBuiltInFn
