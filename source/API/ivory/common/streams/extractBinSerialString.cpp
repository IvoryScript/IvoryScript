/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * Name:    extractBinSerialString.cpp
 *
 * Module:  Ivory common (streams)
 *
 * Description:
 *
 *    Extract a string using the stream serial context name table.
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/byte.h"
#include "ivory/streams.h"
#include "ivory/string.h"

defineBuiltInFn_1_arg(extractBinSerialString,
   builtInAp(typeCon(InputStream), typeCon(Byte)), typeCon(String),
   is, InputStream_Byte)

   ExtractArchive ea(is->_inAct, isEnv);
   Bool isFirst = FALSE;
   returnString((String)extractBinSerialString(ea, *is->_serialContext, isFirst), NULL);

endBuiltInFn
