/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    serialStorage.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory serial storage representation.
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

#ifndef SERIAL_STORAGE_H_DEFINED
#define SERIAL_STORAGE_H_DEFINED

//------------------------------------------------------------------------------
// Primary expression tags
//------------------------------------------------------------------------------
 
#define _TAG_BITS       1
#define _TAG_MASK       ((1<<_TAG_BITS)-1)

#define _VALUE_TAG      0
#define _OID_REF_TAG    1

// Tag constants

#define _TAG_INCR       (1<<_TAG_BITS)

// Special tags

#define _ERROR          _VALUE_TAG

#define _AP             (_ERROR+_TAG_INCR)
#define _MKAP           (_AP+_TAG_INCR)         // temporarily until code generator
#define _ARROW          (_MKAP+_TAG_INCR)
#define _TYPE_CON       (_ARROW+_TAG_INCR)
#define _PRIM           (_TYPE_CON+_TAG_INCR)

#define _TAG_20         (20*_TAG_INCR+_VALUE_TAG)

//------------------------------------------------------------------------------
// Common data constructors
//------------------------------------------------------------------------------

#define _FALSE          (_TAG_20)
#define _TRUE           (_FALSE+_TAG_INCR)
#define _NIL            (_TRUE+_TAG_INCR)
#define _ROOT           (_NIL+_TAG_INCR)

#define _TAG_40         (40*_TAG_INCR+_VALUE_TAG)

//------------------------------------------------------------------------------
// Tuple type and data constructors
//------------------------------------------------------------------------------

#define _TUPLE_CON      (_TAG_40)
#define _PAIR_CON       (_TUPLE_CON+_TAG_INCR)
#define _TRIPLE_CON     (_TRIPLE_CON+_TAG_INCR)

#define _TAG_50         (50*_TAG_INCR+_VALUE_TAG)

//------------------------------------------------------------------------------
// Name tag, name follows
//------------------------------------------------------------------------------

#define _NAME           (_TAG_50)

//------------------------------------------------------------------------------
// Character tag, character follows
//------------------------------------------------------------------------------

#define _CHAR           (_NAME+_TAG_INCR)

//------------------------------------------------------------------------------
// Integer tags
//------------------------------------------------------------------------------

#define _INT_ZERO       (_CHAR+_TAG_INCR)
#define _INT_8          (_INT_ZERO+_TAG_INCR)
#define _INT_16         (_INT_8_TAG+_TAG_INCR)
#define _INT_32         (_INT_16_TAG+_TAG_INCR)

//------------------------------------------------------------------------------
// Float tag, value follows
//------------------------------------------------------------------------------

#define _FLOAT          (_INT_32+_TAG_INCR)

//------------------------------------------------------------------------------
// Double tag, value follows
//------------------------------------------------------------------------------

#define _DOUBLE         (_FLOAT+_TAG_INCR)

//------------------------------------------------------------------------------
// String tag, characters follow
//------------------------------------------------------------------------------

#define _STRING         (_DOUBLE+_TAG_INCR)

//------------------------------------------------------------------------------
// ByteString tag, data follow
//------------------------------------------------------------------------------

#define _BYTE_STRING    (_STRING+_TAG_INCR)

//------------------------------------------------------------------------------
// Non-local reference, path from root follows <n, OID0, OID1, ... OIDn-1>
//------------------------------------------------------------------------------

#define _ADS_REF        (_BYTE_STRING+_TAG_INCR)

//------------------------------------------------------------------------------
// Common structure tags
//------------------------------------------------------------------------------

#define _CONS           (_ADS_REF+_TAG_INCR)
#define _PAIR           (_CONS+_TAG_INCR)
#define _TRIPLE         (_PAIR+_TAG_INCR)

//------------------------------------------------------------------------------
// Data tag, type index and value follow.
//------------------------------------------------------------------------------

#define _DATA           (_TRIPLE+_TAG_INCR)

//------------------------------------------------------------------------------
// Cell kinds
//------------------------------------------------------------------------------

#define CELL_KIND_ICELL        0
#define CELL_KIND_BUILT_IN_FN  1
#define CELL_KIND_BUILT_IN_PAP 2

typedef unsigned char _TAG;

#endif /* SERIAL_STORAGE_H_DEFINED */

