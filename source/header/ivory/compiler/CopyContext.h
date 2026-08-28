/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:   CopyContext.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2019
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definition of class to provide a context for copying expressions.
 *
 * Notes:
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

#ifndef IVORY_COMPILER_COPY_CONTEXT_H_DEFINED
#define IVORY_COMPILER_COPY_CONTEXT_H_DEFINED

#include "NameMap.h"
#include "tran.h"

class CopyContext {
public:
   CopyContext(const ModuleDefn* moduleDefn, NameTable& nameTable,
               BidirNameMap*& varNameMaps, BidirNameMap*& typeVarMaps,
               MSA& msa);

   inline const ModuleDefn* moduleDefn(Void) const { return _moduleDefn; }
   inline BidirNameMap* varNameMap(Void) const { return _varNameMap; }
   inline BidirNameMap* typeVarMap(Void) const { return _typeVarMap; }
   inline NameTable& nameTable(Void) const { return _nameTable; }
   inline const Env& env(Void) const { return _moduleDefn->env(); }

protected:
   const ModuleDefn* _moduleDefn;
   NameTable&        _nameTable;
   BidirNameMap*     _varNameMap;
   BidirNameMap*     _typeVarMap;
};

#endif /* IVORY_COMPILER_COPY_CONTEXT_H_DEFINED */
