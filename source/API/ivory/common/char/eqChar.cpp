/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    eqChar.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Eq instance methods for type Char.
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

#include "ivory/bool.h"
#include "ivory/char.h"
#include "ivory/class.h"
#include "ivory/dyadicOp.h"
#include "ivory/list.h"

#define dyadicOp_Char_Bool_n(n,op,name,e)\
dyadicOp_n(n,op,name,Char,Bool,Tag,e)

// instance Eq Char where
//   (=)  x y = eqChar  x y
//   (¬=) x y = nEqChar x y

dyadicOp_Char_Bool_n(1, "(=)",  eq,  x == y);

dyadicOp_Char_Bool_n(2, "(¬=)", nEq, x != y);

/*----------------------------------------------------------------------------*/

declareClass(Eq);

defineInstance(Eq_Char,
   Eq,
   typeCon(Char),
   builtInCons(methodDefn_1,
      builtInCons(methodDefn_2,
         Nil)));
