/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showDouble.cpp
 *
 * Module:  Ivory common (double)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of showDouble function
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
#include "ivory/double.h"
#include "ivory/list.h"
#include "ivory/void.h"

// showDouble :: Double -> Void

defineBuiltInFn_1_arg(showDouble,
   typeCon(Double), typeCon(Void),
   x, Double)
   *outStrm << x;
   returnVoid();
endBuiltInFn

/*----------------------------------------------------------------------------*/

// instance Show Double where
//    show d = showDouble d

classMethodDefn_n(1, show, showDouble)
{
   push(rDouble, Double);
   jump(altEntry(showDouble));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_Double,
               Show,
               typeCon(Double),
               builtInCons(methodDefn_1,
                           Nil));



