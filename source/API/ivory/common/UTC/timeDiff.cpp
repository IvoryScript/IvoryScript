/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    timeDiff.cpp
 *
 * Module:  Ivory common (UTC)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 03 August 2005
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Primitive function to return the difference between two UTC values.
 *
 * Method:
 *
 * Notes:
 *    The result type is currently Int.  At a later stage, a time difference
 *    type could be introduced, say UTC_Diff.
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

#include "ivory/int.h"
#include "ivory/UTC.h"

declareBuiltInFn(timeDiff);

// timeDiff :: UTC -> UTC -> Int

defineBuiltInFn_2_args(timeDiff,
   typeCon(UTC), typeCon(UTC), typeCon(Int),
   y, UTC,
   x, UTC)
   drop_Stack(timeDiff);
   returnInt((Int)(x - y));
endBuiltInFn
