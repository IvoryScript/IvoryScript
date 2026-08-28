/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    indirection.cpp
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
 *    Indirection cell implemenatation.
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

#include "ivory/common.h"
#include "ivory/machine.h"

#if (GARBAGE_COLLECTION==1)
#include "MSA_GC.h"
#endif

defineLabel(indirectionEntry) {
   cell = toBody(cell, Indirection)._cell;
   if (cell == NULL)
      runTimeError("uninitialised recursive binding");
   jump(static_cast<const CellInfo*>(cell->tag())->entry_());
}

defineLabel(indirectionAltEntry) {
   cell = toBody(cell, Indirection)._cell;
   if (cell == NULL)
      runTimeError("uninitialised recursive binding");
   jump(static_cast<const CellInfo*>(cell->tag())->altEntry_());
}

static Cell* copyIndirection(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   if (cellBody(src, Indirection)._cell == NULL)
      return allocIndirection(&src, msa);
   Cell& _cell = *cellBody(src, Indirection)._cell;
   return (*((const CellInfo*)_cell.tag())->cellCopyFn())(_cell, srcEnv, env, msa n_vm);
}

#if (GARBAGE_COLLECTION==1)
defineLabel(indirectionMark_GC) {
   Cell* cell_ = toBody(cell, Indirection)._cell;
   if (((MSA_GC*)consMSA)->mark(cell_) == 1) {
      Label label = static_cast<const CellInfo*>(cell_->tag())->cellGC_MarkFn();
      if (label != NULL) {
         cell = cell_;
         jump(label);
      }
   }
   jump(popLabel());
}
#endif

Cell* allocIndirection(Cell* cell, MSA& msa) {
   Cell& cell_ = *new(sizeof(Indirection), msa) Cell(&indirectionCellInfo);
   cellBody(cell_, Indirection)._cell = cell;
   return &cell_;
}

CellInfo indirectionCellInfo(
   INDIRECT,0,
   label(indirectionEntry),
   label(indirectionAltEntry),
   copyIndirection,

#if (SERIALISATION==1)
   NULL,
   NULL,
#endif

#if (GARBAGE_COLLECTION==1)
   label(indirectionMark_GC),
#endif

   NULL,
   NULL);

/*----------------------------------------------------------------------------*/
