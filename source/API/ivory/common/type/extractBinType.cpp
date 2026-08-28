/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    extractBinType.cpp
 *
 * Module:  Ivory common (type)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 November 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of extractBinType function. Binary serialisation of a
 *    type.
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

#include "ivory/byte.h"
#include "ivory/env.h"
#include "ivory/streams.h"
#include "ivory/type.h"
#include "ivory/typeTable.h"

defineBuiltInFn_1_arg(extractBinType,
   builtInAp(typeCon(InputStream), typeCon(Byte)), typeCon(Type),
   is, InputStream_Byte)

   ExtractArchive ea(is->_inAct, isEnv);
   Type type_;
   ea >> type_;
   Type builtInType = consEnv->typeTable().altType(type_);

   returnType(builtInType == NULL_NAME ? type_ + builtInTypeCount : builtInType, consEnv);

endBuiltInFn
