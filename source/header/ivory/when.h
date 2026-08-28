/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    when.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
*
*-----------------------------------------------------------------------------
*
* Description:
*
*    Definitions for use with 'When' data type/
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
*
*/

#ifndef IVORY_WHEN_H_DEFINED
#define IVORY_WHEN_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/common.h"
#include "ivory/ref.h"

/*----------------------------------------------------------------------------*/

typedef UInt When;

declareTypeCon(When);

declareType(When);

extern Tag checkWhen$(argVM);
#define checkWhen() checkWhen$(vm)

extern CellInfo* cellInfoTable_When[];

#define updateWhen()\
if(updatePtr!=NULL){updatePtr->tag()=cellInfoTable_When[rTag];updatePtr=NULL;}

#define BEFORE_TAG   0
#define AFTER_TAG    1

declareNullaryDataCon(Before);
declareNullaryDataCon(After);

#define returnWhen(w){\
rTag = w;\
jump(popLabel());}

/*----------------------------------------------------------------------------*/

declareBuiltInFn(showWhen);

declareBuiltInFn(insertTxtWhen);

#endif /* IVORY_WHEN_H_DEFINED */
