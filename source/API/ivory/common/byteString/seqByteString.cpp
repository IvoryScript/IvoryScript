/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    seqByteString.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 23 April 2002
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of class Seq instance methods for type ByteString.
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

#include "ivory/class.h"
#include "ivory/byteString.h"
#include "ivory/dyadicOp.h"
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/name.h"
#include "ivory/name.h"

// lengthByteString :: ByteString -> Int

defineBuiltInFn(lengthByteString,
                builtInAp2(ARROW,
                           typeCon(ByteString),
                           typeCon(Int)))
#define bs            sizeof(ByteString*)
#define stackDepth  bs
{
   checkArgs("lengthByteString");
   jump(altEntry(lengthByteString));
}

defineAltEntry(lengthByteString)
{
   Int length = local(bs, ByteString)->_len;
   dropStack();
   returnInt(length);
}

#undef stackDepth
#undef bs

/*----------------------------------------------------------------------------*/

// getAtByteString :: ByteString -> Int -> Int

defineBuiltInFn(getAtByteString,
                builtInAp2(ARROW,
                           typeCon(ByteString),
                           builtInAp2(ARROW,
                                      typeCon(Int),
                                      typeCon(Int))))
#define i            sizeof(Int)
#define bs           (i+sizeof(ByteString))
#define stackDepth  bs
{
#ifdef CHECK_ARGS
   if (depth() < stackDepth)
      runTimeError("invalid application to getAtByteString");
#endif
   jump(altEntry(getAtByteString));
}

defineAltEntry(getAtByteString)
{
   if (local(i, Int) < 0 || local(i, Int) >= (Int)local(bs, ByteString)->_len)
      runTimeError("getAtByteString: index out of range");
   Int res = local(bs, ByteString)->_data[local(i, Int)];
   dropStack();
   returnInt(res);
}

/*----------------------------------------------------------------------------*/

// instance Seq ByteString where
//    length rl = lengthByteString (repr rl)
//    getAt  rl i    = getAt  (repr rl) i

classMethodDefn_n(1, length, lengthByteString)
{
   push((ByteString)rPtr, ByteString);
   jump(altEntry(lengthByteString));
}

classMethodDefn_n(2, getAt, getAtByteString)
{
   push((ByteString*)rPtr, ByteString*);
   jump(altEntry(getAtByteString));
}

static ByteString concatBS(ByteString bs1, ByteString bs2, MSA& msa) {
   ByteString res = (ByteString)msa.alloc(sizeof(ByteString_tag) +
      bs1->_len + bs2->_len - MAX_BYTE_STRING_LENGTH);

   memcpy(res->_data, bs1->_data, bs1->_len);
   memcpy(res->_data + bs1->_len, bs2->_data, bs2->_len);
   res->_len = bs1->_len + bs2->_len;
   return res;
}

#ifdef x_
#undef x_
#endif
#ifdef x_env
#undef x_env
#endif
#ifdef y_
#undef y_
#endif
#ifdef y_env
#undef y_env
#endif
#ifdef stackDepth
#undef stackDepth
#endif

#define y_env        sizeof(Env*)
#define y_           (y_env+sizeof(ByteString))
#define x_env        (y_+sizeof(Env*))
#define x_           (x_env+sizeof(ByteString))
#define stackDepth   x_

#define dyadicOp_ByteString_ByteString_n(n,op,name,e)\
dyadicOpEnv_n(n,op,name,ByteString,ByteString,Ptr,e)

dyadicOp_ByteString_ByteString_n(3, "(++)", concat, concatBS(x, y, *consMSA));

/*----------------------------------------------------------------------------*/

declareClass(Seq);

defineInstance(Seq_ByteString,
               Seq,
               typeCon(ByteString),
               builtInCons(methodDefn_1,
                  builtInCons(methodDefn_2,
                     builtInCons(methodDefn_3,
                        Nil))));
