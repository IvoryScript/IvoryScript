/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ordering.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the 'Ordering' data type
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

#ifndef IVORY_ORDERING_H_DEFINED
#define IVORY_ORDERING_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/builtInDataCon.h"
#include "ivory/type.h"

#define LT_TAG 0
#define EQ_TAG 1
#define GT_TAG 2

#define sizeOfOrdering sizeof(Tag)

typedef Tag Ordering;

declareTypeCon(Ordering);

declareType(Ordering);

declareNullaryDataCon(LT);
declareNullaryDataCon(EQ);
declareNullaryDataCon(GT);

#define returnLT(){\
rTag = LT_TAG;\
jump(popLabel());}

#define returnEQ(){\
rTag = EQ_TAG;\
jump(popLabel());}

#define returnGT(){\
rTag = GT_TAG;\
jump(popLabel());}


#define loadArgRegOrdering(name) Tag name=rTag

#define loadArgOrdering(name, off) Tag name = stack(off,Tag)

#define returnOrdering(tag){\
rTag = tag;\
jump(popLabel());}

extern Tag checkOrdering$(argVM);
#define checkOrdering() checkOrdering$(vm)

extern CellInfo* cellInfoTable_Ordering[];

#define updateOrdering()\
if(updatePtr!=NULL){updatePtr->tag()=cellInfoTable_Ordering[rTag];updatePtr=NULL;}

declareBuiltInFn(showOrdering);

declareBuiltInFn(insertTxtOrdering);

#endif /* IVORY_ORDERING_H_DEFINED */
