/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    insertTxtOrdering.cpp
 *
 * Module:  Ivory common (ord)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 July 2026
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of insertTxtOrdering function. Text insertion of an Ordering value.
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

#include "ivory/char.h"
#include "ivory/ordering.h"
#include "ivory/streams.h"
#include "ivory/tag.h"
#include "ivory/void.h"

static const char* ident(Tag tag)
{
   switch (tag)
   {
      case LT_TAG:   return "LT";
      case EQ_TAG:   return "EQ";
      case GT_TAG:   return "GT";
      default: return "???";
   }
}

// insertTxtOrdering :: OutputStream Char -> Ordering -> Void

defineBuiltInFn_2_args(insertTxtOrdering,
   builtInAp(typeCon(OutputStream), typeCon(Char)), typeCon(Ordering), typeCon(Void),
   os, OutputStream_Char,
   ord, Tag)

   putString(*os, *osEnv, ident(ord) n_vm);

   drop_Stack(insertTxtOrdering);
   returnVoid();
endBuiltInFn
