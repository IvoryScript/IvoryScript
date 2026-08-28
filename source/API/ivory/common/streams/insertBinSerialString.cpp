/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * Name:    insertBinSerialString.cpp
 *
 * Module:  Ivory common (Streams)
 *
 * Description:
 *
 *    Insert a string using the stream serial context name table.
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/byte.h"
#include "ivory/streams.h"
#include "ivory/string.h"
#include "ivory/void.h"

defineBuiltInFn_2_args(insertBinSerialString,
   builtInAp(typeCon(OutputStream), typeCon(Byte)), typeCon(String), typeCon(Void),
   os, OutputStream_Byte,
   s, String)

   InsertArchive ia(os->_outputFn, osEnv);
   insertBinSerialString(ia, *os->_serialContext, s);

   drop_Stack(insertBinSerialString);
   returnVoid();
endBuiltInFn
