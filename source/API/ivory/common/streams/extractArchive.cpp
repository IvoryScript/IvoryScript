/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractArchive.cpp
 *
 * Module:  Ivory common (streams)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 29 May 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of an archive for serialization from a stream
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
#include "ivory/machine.h"
#include "ivory/streams.h"

ExtractArchive::ExtractArchive(Expr inAct, Env* inActEnv)
   : _inAct(inAct), _inActEnv(inActEnv) {
}

Byte ExtractArchive::get(void) {

#if (GLOBAL_VM==0)
   VirtualMachine& vm = _vm;
#endif

   pushLabel(NULL);
   cellEnv = _inActEnv;
   cell = (Cell*)_inAct;

#if (GLOBAL_VM==0)
   callSequence$(enter$(_inAct, _inActEnv, _vm), _vm);
#else
   callSequence$((static_cast<const CellInfo*>(cell->tag()))->altEntry_());
#endif

   return rByte;
}

void ExtractArchive::put(Byte b) {
   error("ExtractArchive::put: not supported");

}

void ExtractArchive::get(Byte& b) {
   b = get();
}
