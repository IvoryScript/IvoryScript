/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    showOrdering.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 7 November 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of showOrdering function
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
#include "ivory/ordering.h"
#include "ivory/trace.h"
#include "ivory/void.h"

#define stackDepth  0

// showOrdering :: Ordering -> Void

static const char* ident(Tag tag) {
   switch (tag) {
      case LT_TAG:   return "LT";
      case EQ_TAG:   return "EQ";
      case GT_TAG:   return "GT";
      default: return "???";
   }
}

defineBuiltInFn_1_arg(showOrdering,
   typeCon(Ordering), typeCon(Void),
   o, Tag)
   *outStrm << ident(o);
   returnVoid();
endBuiltInFn

/*----------------------------------------------------------------------------*/

// instance Show Ordering where
//    show o = showOrdering o

classMethodDefn_n(1, show, showOrdering)
{
   push(rTag, Tag);
   jump(altEntry(showOrdering));
}

/*----------------------------------------------------------------------------*/

declareClass(Show);

defineInstance(Show_Ordering,
               Show,
               typeCon(Ordering),
               builtInCons(methodDefn_1,
                           Nil));
