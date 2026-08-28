/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    NameExprBinding.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definition of a Name/Expr binding.
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

#ifndef IVORY_NAME_EXPR_BINDING_H_DEFINED
#define IVORY_NAME_EXPR_BINDING_H_DEFINED

#include "ivory/nameBinding.h"

typedef NameBinding<Expr> NameExprBinding;

extern NameExprBinding& checkBinding$(argVM);
#define checkBinding() checkBinding$(vm)

declareType(NameExprBinding);

#endif /* IVORY_NAME_EXPR_BINDING_H_DEFINED */
