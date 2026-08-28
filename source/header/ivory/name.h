/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    name.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Name native data type.
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

#ifndef IVORY_NAME_H_DEFINED
#define IVORY_NAME_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/env.h"
#include "ivory/type.h"

#define sizeOfName (sizeof(Name))

#define typeSpecName Name

declareTypeCon(Name);

#define stackFPSizeName (stackSlotSize(Name)+stackSlotSize(Env))

#define loadArgRegName(name)\
Name name=rName;\
Env* name##Env=rEnv

#define loadArgName(name,off)\
Env* name##Env=stack(off+stackSlotSize(Name),Env*);\
Name name=stack((off),Name)

#define storeArgName(name, off) stack(off, Name)=name

#define argRegFromName(s,c)\
rName=cellBody(*cell,s).c;\
rEnv=cellEnv

#define argRegFromCellName(s,c)\
rName=cellBody(*cell,s).c;\
rEnv=cellEnv

#define map_Name(name,srcEnv,dstEnv) mapName$(name,(srcEnv)->nameTable(),(dstEnv)->nameTable())

#define returnName(name,env){\
rName = name;\
rEnv = env;\
jump(popLabel());}

extern Bool eqName_(Name name1, const NameTable& nameTable1,
                    Name name2, const NameTable& nameTable2);

extern Cell* makeName(Name name, MSA& msa);

extern Name mapName$(Name name, const NameTable& srcNameTable,
                                      NameTable& dstNameTable);

extern Name mappedName$(Name name, const NameTable& srcNameTable,
                        const NameTable& dstNameTable);

declareTypeCon(Name);

declareType(Name);

declareBuiltInFn(eqName);

declareBuiltInFn(extractBinName);

declareBuiltInFn(hashName);

declareBuiltInFn(insertBinName);

declareBuiltInFn(mapName);

declareBuiltInFn(nameString);

declareBuiltInFn(showName);

declareBuiltInFn(insertTxtName);

#endif /* IVORY_NAME_H_DEFINED */
