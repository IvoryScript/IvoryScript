/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    when.cpp
 *
 * Module:  Ivory common (when)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 7 November 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of When type
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

#include "ivory/when.h"

defineTypeCon(When);

static Void init$When(TypeDescr* typeDescr);
defineType(When, typeCon(When), init$When);

Void insertTxtFn$When(TypeDescr* typeDescr, OutputStream_Char* os, const Env& osEnv, Ptr ptr, const Env& env argN_VM) {
   rPtr = os;
   rEnv = static_cast<Env*>((Void*)&osEnv);
   pushLabel(NULL);
   push(*static_cast<When*>(ptr), Tag);
   callSequence$(altEntry(insertTxtWhen) n_vm);
}

Void evalFn$When(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rTag = static_cast<Tag>(*static_cast<When*>(ptr));
}

Void init$When(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(When);
   typeDescr->_insertTxtFn = insertTxtFn$When;
   typeDescr->_evalFn = evalFn$When;
}

defineNullaryDataCon(Before, 0, When);
defineNullaryDataCon(After,  1, When);

CellInfo* cellInfoTable_When[] = {&cellInfo$Before, &cellInfo$After};

