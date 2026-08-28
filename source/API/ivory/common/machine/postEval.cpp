/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    postEval.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 15 November 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to finalise the virtual machine prior after evaluation.
 *
 * Call format:
 *
 *    postEval(argVM)
 *       argVM  (Virtualmachine&) - Virtual Machine (conditionally)
 *
 * Method:
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
#include "ivory/trace.h"

Void postEval$(bool verbose argN_VM) {
//   if (updatePtr != NULL)
//      ivoryError("postEval: update pending");


#if (GLOBAL_VM == 1)
   if (--vmLevel == 0)
   {
#endif
      if (verbose)
         consMSA->info(outStream, 0);

 //     delete consMSA;
      consMSA = NULL;
#if (GLOBAL_VM == 1)
   }
   else
      if (vmLevel < 0)
         ivoryError("postEval: invalid nesting level");
#endif

   (void)pop(Cell*);
}
