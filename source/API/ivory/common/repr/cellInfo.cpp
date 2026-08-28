/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    cellInfo.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Support for in-built type CellInfo
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
 * Notes:
 *
 */

#include "ivory/type.h"

static Void init_CellInfo(TypeDescr* typeDescr);
defineType(CellInfo, CELL_INFO, init_CellInfo);

Void init_CellInfo(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(CellInfo*);
}

CellInfo::CellInfo(Expr form, size_t size,
                   Label entry_,
                   CellCopyFn cellCopyFn/* = NULL*/,

#if (SERIALISATION==1)
                   Label cellExtractFn/* = NULL*/,
                   Label cellInsertFn/* = NULL*/,
#endif

#if (GARBAGE_COLLECTION==1)
                   Label cellGC_MarkFn/* = NULL*/,
#endif

                   CellDestroyFn destroyCellFn/* = NULL*/,
                   CellShowFn cellPrintFn/* = NULL*/)
 : _form(form), _size(size),
   _entry(entry_), _altEntry(entry_),
   _cellCopyFn(cellCopyFn),

#if (SERIALISATION==1)
   _cellExtractFn(cellExtractFn),
   _cellInsertFn(cellInsertFn),
#endif

#if (GARBAGE_COLLECTION==1)
   _cellGC_MarkFn(cellGC_MarkFn),
#endif

   _cellDestroyFn(destroyCellFn),
   _cellShowFn(cellPrintFn) {
}

CellInfo::CellInfo(Expr form, size_t size,
                   Label entry_, Label altEntry,
                   CellCopyFn cellCopyFn/* = NULL*/,

#if (SERIALISATION==1)
                   Label cellExtractFn/* = NULL*/,
                   Label cellInsertFn/* = NULL*/,
#endif

#if (GARBAGE_COLLECTION==1)
                   Label cellGC_MarkFn/* = NULL*/,
#endif

                   CellDestroyFn destroyCellFn/* = NULL*/,
                   CellShowFn cellPrintFn/* = NULL*/)
 : _form(form), _size(size),
   _entry(entry_), _altEntry(altEntry),
   _cellCopyFn(cellCopyFn),

#if (SERIALISATION==1)
   _cellExtractFn(cellExtractFn),
   _cellInsertFn(cellInsertFn),
#endif
   
#if (GARBAGE_COLLECTION==1)
   _cellGC_MarkFn(cellGC_MarkFn),
#endif

   _cellDestroyFn(destroyCellFn),
   _cellShowFn(cellPrintFn) {
}
