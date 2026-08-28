/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    mapClosure.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to map a closure from one environment to another.
 *
 * Call format:
 *
 *    res = mapClosure(closure, srcEnv, dstEnv)
 *                     
 *       res      (Closure)         - Returned closure mapped to destination
 *       closure  (Closure)         - Closure to map
 *       srcEnv   (Env&)            - Source environment
 *       dstEnv   (Env&)            - Destination environment
 *
 * Method:
 *
 *    Simply returns the given closure if the closure the environments match.
 *    Otherwise copies the closure to the destination environment
 *    and returns the result.
 *
 * Errors:
 *
 *    Errors are signalled by a ERROR return value
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

Cell* mapClosure(Cell* closure, const Env& srcEnv, Env& dstEnv argN_VM) {
   if (&srcEnv == &dstEnv)
      return closure;
   CellCopyFn copyCellFn = ((CellInfo*)closure->tag())->cellCopyFn();
   if (copyCellFn != NULL)
      return (*copyCellFn)(*closure, srcEnv, dstEnv, dstEnv.msa() n_vm);
   else {
      throw(RunTimeError("mapClosure$: Cell can\'t be copied"));
      return NULL;
   }
}
