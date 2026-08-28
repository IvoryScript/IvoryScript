/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    dir.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 November 2020
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of Dir and DirEntry types
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

#include "ivory/dir.h"
#include "ivory/machine.h"

defineTypeCon(Dir);

defineTypeCon(DirEntry);

static Void init$Dir(TypeDescr* typeDescr);
defineType_n(1, Dir, typeCon(Dir), init$Dir);

static Void init$DirEntry(TypeDescr* typeDescr);
defineType_n(2, DirEntry, typeCon(DirEntry), init$DirEntry);

// Dir type methods.

Void evalFn$Dir(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rPtr = *static_cast<DIR**>(ptr);
}

Void init$Dir(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(DIR*);
   typeDescr->_evalFn = evalFn$Dir;
}

// DirEntry type methods.

Void evalFn$DirEntry(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rPtr = *static_cast<struct dirent**>(ptr);
}

Void init$DirEntry(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(struct dirent*);
   typeDescr->_evalFn = evalFn$DirEntry;
}
