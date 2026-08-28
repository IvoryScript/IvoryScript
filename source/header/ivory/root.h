/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    root.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Ivory 'Root' data type.
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

#ifndef IVORY_ROOT_H_DEFINED
#define IVORY_ROOT_H_DEFINED

#include "ivory/ads.h"
#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/type.h"

extern Expr mkRoot(void);

declareTypeCon(Root);

declareType(Root);

declareBuiltInDataCon(Root);

#define Root dataCon(Root)

#define returnRoot(){\
rType = builtInType(Root);\
jump(popLabel());}

extern Void checkRoot$(argVM);
#define checkRoot() checkRoot$(vm)

extern CellInfo cellInfo$Root;

#define updateRoot()\
if(updatePtr!=NULL){updatePtr->tag()=&cellInfo$Root;updatePtr=NULL;}

extern ADS_Instance* rootADSI;

//declareBuiltInFn(addRegisterEntry_Root);

//declareBuiltInFn(hasRegisterEntry_Root);

declareBuiltInFn(createObjectRoot);

declareBuiltInFn(mapProcObjectsRoot);

//declareBuiltInFn(removeRegisterEntry_Root);

//declareBuiltInFn(selectRoot);

//declareBuiltInFn(showRoot);

#endif /* IVORY_ROOT_H_DEFINED */
