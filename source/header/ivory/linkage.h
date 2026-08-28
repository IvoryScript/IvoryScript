/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    linkage.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory linkage information.
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

#ifndef IVORY_LINKAGE_H_DEFINED
#define IVORY_LINKAGE_H_DEFINED

#include "ivory/common.h"

struct Linkage
{
   UInt        _nTypeCons;
   TypeSig**   _typeCons;
   UInt        _nDataCons;
   Expr**      _dataCons;
   UInt        _nClasses;
   Expr**      _classes;
   UInt        _nInstances;
   Expr**      _instances;
   UInt        _nBuiltInFns;
   Cell**      _builtInFns;
};

#endif /* IVORY_LINKAGE_H_DEFINED */
