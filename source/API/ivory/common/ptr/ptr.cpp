/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ptr.cpp
 *
 * Module:  Ivory common (ptr)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Ptr type
 *    Only included for type signature of constructor
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

#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/ptr.h"
#include "ivory/type.h"

static TypeSig mkTypeVars_Ptr(Void) {
   return builtInCons(fromName(builtInName(a)), Nil);
};

defineBuiltInParamTypeCon(Ptr, mkTypeVars_Ptr);

// Null is treated as a special case

static TypeSig mkTypeSig$Null(Void) {
   return builtInAp(typeCon(Ptr), fromName(builtInName(a)));
};

defineEntry(Null) {
   returnNull();
}

static Cell* cellCopy$Null(Cell& src, const Env& srcEnv, Env& env, MSA& msa argN_VM) {
   return dataCon(Null);
};

static Void cellShow$Null(const Cell& cell_, ostream& os, const Env& env argN_VM) {
   os << "Null";
};

CellInfo cellInfo$Null(BUILT_IN_DATA_CON, 0, entry(Null), cellCopy$Null,

#if (SERIALISATION==1)
                       NULL, NULL,
#endif

#if (GARBAGE_COLLECTION==1)
                       NULL,
#endif

                       destroyCell, cellShow$Null);

Expr dataCon$Null = mkBuiltInDataCon("Null", 0, mkTypeSig$Null, NULL, cellInfo$Null);

/*----------------------------------------------------------------------------*/

// Built-in ptr constructor

// Ptr :: a -> Ptr a

defineDataCon(Ptr, 1, 
              builtInAp2(ARROW,
                         fromName(builtInName(a)),
                         builtInAp(typeCon(Ptr), fromName(builtInName(a)))), POINTER) {
   jump(altEntry(Ptr));
}

defineAltEntry(Ptr) {
   assert(FALSE, "Invalid call to Ptr");
   returnNull();
}
