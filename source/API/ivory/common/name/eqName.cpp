/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    eqName.cpp
 *
 * Module:  Ivory common (name)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Primitive function to compare two names
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

#include <string.h>
#include "ivory/bool.h"
#include "ivory/class.h"
#include "ivory/list.h"
#include "ivory/name.h"

// eqName :: Name -> Name -> Bool

defineBuiltInFn_2_args(eqName,
                       typeCon(Name), typeCon(Name), typeCon(Bool),
                       x, Name,
                       y, Name) 
    Bool res = eqName_(x, xEnv->nameTable(),
                      y, yEnv->nameTable());
   drop_Stack(eqName);  
   returnBool(res);
endBuiltInFn

// instance Eq Name where
//    (=) x y = eqName x y

classOperatorDefn_n(1, "(=)", eqName)
{
   push(rName, Int);
   jump(altEntry(eqName));
}

/*----------------------------------------------------------------------------*/

declareClass(Eq);

defineInstance(Eq_Name,
               Eq,
               typeCon(Name),
               builtInCons(methodDefn_1,
                           Nil));
