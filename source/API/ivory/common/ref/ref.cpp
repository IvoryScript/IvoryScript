/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ref.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Ivory Ref type
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
#include "ivory/object.h"
#include "ivory/ref.h"
#include "ivory/type.h"

defineTypeCon(Ref);

static Void init_Ref(TypeDescr* typeDescr);
defineType(Ref, typeCon(Ref), init_Ref);


Void evalFn$Ref(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rRef = *static_cast<Ref*>(ptr);
   rEnv = &env;
}

Void init_Ref(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(Ref);
   typeDescr->setRepr(Repr::REPR_REF);
   typeDescr->_evalFn = evalFn$Ref;
}

//*---------------------------------------------------------------------------*/


Void Ref::print(ostream& os, const Env& env) const {
   if (*this != NULL_REF)
      os << "Ref#" << hex << ((OID)*this).repr() << dec;
   else
      os << "NULL_REF";
}


defineNullaryDataCon(NullRef, 0, Ref);

/*----------------------------------------------------------------------------*/
