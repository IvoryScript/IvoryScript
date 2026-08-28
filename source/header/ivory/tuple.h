/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    double.h
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
 *    Definitions for use with Pair data type
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

#ifndef IVORY_TUPLE_H_DEFINED
#define IVORY_TUPLE_H_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/type.h"

declareType(Pair);

declareType(Triple);

#define sizeOfPair sizeof(Ptr)

#define typeSpecPair Ptr

#define argRegPair rPtr

#define stackFPSizePair (stackSlotSize(Pair)+stackSlotSize(Env))

#define loadArgRegPair(name)\
Ptr name=rPtr;\
Env* name##Env=rEnv

#define loadArgPair(name,off)\
Env* name##Env=stack(off+stackSlotSize(Pair),Env*);\
Ptr name=stack((off),Ptr)

#define storeArgPair(name,off)\
stack(off+stackSlotSize(Pair),Env*)=name##Env;\
stack(off,Ptr)=name

#define mappedPair(name,srcEnv,dstEnv) name
// Tuple macros

#define isPair(expr) (isPtr(expr)&&\
isPtr(toCell(expr).tag())&&\
exprCellInfo(expr).form()==fromType(builtInType(Pair)))

#define isConstPair(expr) (isPtr(expr)&&\
isPtr(toConstCell(expr).tag())&&\
constExprCellInfo(expr).form()==fromType(builtInType(Pair)))


#define isTriple(expr) (isPtr(expr)&&\
isPtr(toCell(expr).tag())&&\
(static_cast<const CellInfo*>(toCell(expr).tag()))->form()==fromType(builtInType(Triple)))

#define isTupleTypeSig(typeSig) (isAp(typeSig)&&\
isAp(fun(typeSig))&&\
isTupleCon(fun(fun(typeSig))))

declareBuiltInDataCon(Pair);

#define returnPair(p, env){\
rPtr=(p);\
rEnv=(env);\
jump(popLabel());}

extern void* checkPair$(argVM);
#define checkPair() checkPair$(vm)

extern Void updatePair$(argVM);
#define updatePair()\
if(updatePtr!=NULL){updatePair$(vm);updatePtr=NULL;}

declareBuiltInFn(showPair);

declareBuiltInFn(insertTxtPair);

// Transfer functions

extern Cell* fromTuple(Type type, Ptr ptr, MSA& msa);

#endif /* IVORY_TUPLE_H_DEFINED */

