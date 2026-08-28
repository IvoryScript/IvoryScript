/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    linkage.cpp
 *
 * Module:  Ivory common (linkage)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 13 February 2003
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Linkage information for the Ivory common module.
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

#ifndef LINKAGE
#define LINKAGE
#endif

#include "ivory/any.h"
#include "ivory/binding.h"
#include "ivory/bindingSet.h"
#include "ivory/bits.h"
#include "ivory/bool.h"
#include "ivory/byte.h"
#include "ivory/byteString.h"
#include "ivory/char.h"
#include "ivory/class.h"
#include "ivory/criteria.h"
#include "ivory/dir.h"
#include "ivory/double.h"
#include "ivory/env.h"
#include "ivory/event.h"
#include "ivory/extant.h"
#include "ivory/fileIO.h"
#include "ivory/float.h"
#include "ivory/int.h"
#include "ivory/linkage.h"
#include "ivory/name.h"
#include "ivory/object.h"
#include "ivory/ordering.h"
#include "ivory/plain.h"
#include "ivory/ptr.h"
#include "ivory/streams.h"
#include "ivory/string.h"
#include "ivory/sync.h"
#include "ivory/tag.h"
#include "ivory/thread.h"
#include "ivory/tuple.h"
#include "ivory/type.h"
#include "ivory/UTC.h"
#include "ivory/void.h"
#include "ivory/when.h"

static TypeSig* typeCons[] = {
   &typeCon(Any),
   &typeCon(Array),
   &typeCon(Binding),
   &typeCon(Bits),
   &typeCon(Bool),
   &typeCon(Byte),
   &typeCon(ByteString),
   &typeCon(Char),
   &typeCon(Dir),
   &typeCon(DirEntry),
   &typeCon(Double),
   &typeCon(Env),
   &typeCon(Expr),
   &typeCon(Extant),
   &typeCon(FileHandle),
   &typeCon(Float),
   &typeCon(InputStream),
   &typeCon(Int),
   &typeCon(IO_Mode),
   &typeCon(Name),
   &typeCon(Ordering),
   &typeCon(OutputStream),
   &typeCon(Plain),
   &typeCon(Ptr),
   &typeCon(SerialContext),
   &typeCon(String),
   &typeCon(Tag),
   &typeCon(Type),
   &typeCon(Void),
};

static Expr* dataCons[] = {
   &dataCon(Env),
   &False,
   &Nil,
   &True,
};


declareClass(Bitwise);
declareClass(Eq);
declareClass(Integral);
declareClass(Fractional);
declareClass(Num);
declareClass(Ord);
declareClass(Show);

static Expr* classes[] = {
   &class_(Bitwise),
   &class_(Eq),
   &class_(Fractional),
   &class_(Integral),
   &class_(Num),
   &class_(Ord),
   &class_(Show),
};

declareInstance(Bitwise_Bits);

declareInstance(Eq_Bool);
declareInstance(Eq_Char);
declareInstance(Eq_Double);
declareInstance(Eq_Float);
declareInstance(Eq_Int);
declareInstance(Eq_String);
declareInstance(Eq_Name);
declareInstance(Eq_Ptr);
declareInstance(Eq_UTC);

declareInstance(Fractional_Double);
declareInstance(Fractional_Float);

declareInstance(Integral_Int);

declareInstance(Num_Double);
declareInstance(Num_Float);

declareInstance(Ord_Char);
declareInstance(Ord_Double);
declareInstance(Ord_Float);
declareInstance(Ord_Int);
declareInstance(Ord_String);
declareInstance(Ord_UTC);

declareInstance(Show_Bool);
declareInstance(Show_ByteString);
declareInstance(Show_Char);
declareInstance(Show_Double);
declareInstance(Show_Float);
declareInstance(Show_Int);
declareInstance(Show_List);
declareInstance(Show_Name);
declareInstance(Show_Ordering);
declareInstance(Show_String);
declareInstance(Show_UTC);
declareInstance(Show_Type);

static Expr* instances[] = {
   &instance(Bitwise_Bits),

   &instance(Eq_Bool),
   &instance(Eq_Char),
   &instance(Eq_Double),
   &instance(Eq_Float),
   &instance(Eq_Int),
   &instance(Eq_String),
   &instance(Eq_Name),
   &instance(Eq_Ptr),
   &instance(Eq_UTC),

   &instance(Fractional_Double),
   &instance(Fractional_Float),

   &instance(Integral_Int),

   &instance(Num_Double),
   &instance(Num_Float),

   &instance(Ord_Char),
   &instance(Ord_Double),
   &instance(Ord_Float),
   &instance(Ord_Int),
   &instance(Ord_String),
   &instance(Ord_UTC),

   &instance(Show_Bool),
   &instance(Show_ByteString),
   &instance(Show_Char),
   &instance(Show_Double),
   &instance(Show_Float),
   &instance(Show_Int),
   &instance(Show_Name),
   &instance(Show_Ordering),
   &instance(Show_String),
   &instance(Show_UTC),
   &instance(Show_Type)
};

declareBuiltInFn(eqType);
declareBuiltInFn(error);
declareBuiltInFn(receiveScript);
declareBuiltInFn(trace);

declareBuiltInFn(nestedTuple);

static Cell* builtInFns[] = {
   builtInFn(acos),
   builtInFn(asin),
   builtInFn(atan),
   builtInFn(atan2),

   builtInFn(bytesToString),

   builtInFn(closeDir),
   builtInFn(closeFile),
   builtInFn(cos),
   builtInFn(cosh),

   builtInFn(deleteFile),
   builtInFn(destroySerialContext),
   builtInFn(degrees),
   builtInFn(delay),
   builtInFn(dirEntryName),

   builtInFn(eqName),
   builtInFn(eqPlainPtr),
   builtInFn(eqPtr),
   builtInFn(eqType),
   builtInFn(evalAny),
   builtInFn(exit),
   builtInFn(exp),
   builtInFn(error),
   builtInFn(extractBinAnyPtr),
   builtInFn(extractBinChar),
   builtInFn(extractBinDouble),
   builtInFn(extractBinEnv),
   builtInFn(extractBinFloat),
   builtInFn(extractBinInt),
   builtInFn(extractBinName),
   builtInFn(extractBinSerialString),
   builtInFn(extractBinString),
   builtInFn(extractBinTag),
   builtInFn(extractBinType),

   builtInFn(fetchInt),
   builtInFn(fetchDouble),
   builtInFn(formatChar),
   builtInFn(formatDouble),
   builtInFn(formatInt),
   builtInFn(formatFloat),
   builtInFn(formatString),
   builtInFn(formatUTC),
   builtInFn(fromByteBits),
   builtInFn(fromBitsByte),
   builtInFn(fromBitsInt),
   builtInFn(fromIntBits),
   builtInFn(fromIntByte),
   builtInFn(fromIntFloat),
   builtInFn(fromStringFloat),
   builtInFn(fromFloatDouble),
   builtInFn(fromIntDouble),
   builtInFn(fromStringDouble),

   builtInFn(getSerialContext),

   builtInFn(hashName),
   builtInFn(hashString),

   builtInFn(insertBinAnyPtr),
   builtInFn(insertBinChar),
   builtInFn(insertBinDouble),
   builtInFn(insertBinEnv),
   builtInFn(insertBinFloat),
   builtInFn(insertBinInt),
   builtInFn(insertBinName),
   builtInFn(insertBinSerialString),
   builtInFn(insertBinString),
   builtInFn(insertBinTag),
   builtInFn(insertBinType),
   builtInFn(insertTxtAny),
   builtInFn(insertTxtBits),
   builtInFn(insertTxtBool),
   builtInFn(insertTxtByteString),
   builtInFn(insertTxtChar),
   builtInFn(insertTxtDouble),
   builtInFn(insertTxtFloat),
   builtInFn(insertTxtInt),
   builtInFn(insertTxtName),
   builtInFn(insertTxtOrdering),
   builtInFn(insertTxtPair),
   builtInFn(insertTxtString),
   builtInFn(insertTxtTag),
   builtInFn(insertTxtType),
   builtInFn(insertTxtUTC),
   builtInFn(insertTxtWhen),
   builtInFn(isSubstring),

   builtInFn(log),
   builtInFn(log10),
   builtInFn(lowercaseString),

   builtInFn(mapAnyPtr),
   builtInFn(mapByteString),
   builtInFn(mapName),
   builtInFn(mapString),
   builtInFn(markAny_GC),

   builtInFn(mutableString),

   builtInFn(makeDir),

   builtInFn(nameString),
   builtInFn(nextDirEntry),

   builtInFn(openDir),
   builtInFn(openFile),

   builtInFn(pow),
   builtInFn(putAtString),

   builtInFn(radians),
   builtInFn(randomInt),
   builtInFn(readFileByte),
   builtInFn(removeDir),

   builtInFn(seedRandomInt),
   builtInFn(setEnv),
   builtInFn(sin),
   builtInFn(sinh),
   builtInFn(showAny),
   builtInFn(showBits),
   builtInFn(showWhen),
   builtInFn(sqrt),
   builtInFn(substring),

   builtInFn(tan),
   builtInFn(tanh),
   builtInFn(time),
   builtInFn(trace),
   builtInFn(typeString),
   builtInFn(timeDiff),

   builtInFn(uppercaseString),

   builtInFn(writeFileByte)
};

struct Linkage linkage_common = {
   sizeof(typeCons)   / sizeof(TypeSig*), typeCons,
   sizeof(dataCons)   / sizeof(Expr*),    dataCons,
   sizeof(classes)    / sizeof(Expr*),    classes,
   sizeof(instances)  / sizeof(Expr*),    instances,
   sizeof(builtInFns) / sizeof(Cell*),    builtInFns,
};
