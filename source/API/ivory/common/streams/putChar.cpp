/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    putChar.cpp
 *
 * Module:  Ivory common (streams)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 6 July 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Low-level output operations for character streams.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/streams.h"

Void putChar(OutputStream_Char& os, const Env& osEnv, Char c argN_VM) {
   cell = os._outputFn;
   cellEnv = static_cast<Env*>((Void*)&osEnv);
   rChar = c;
   pushLabel(NULL);
   callSequence$((static_cast<const CellInfo*>(cell->tag()))->altEntry_() n_vm);
}

Void putString(OutputStream_Char& os, const Env& osEnv,
               const char* str argN_VM) {
   for (; *str != '\0'; ++str)
      putChar(os, osEnv, *str n_vm);
}
