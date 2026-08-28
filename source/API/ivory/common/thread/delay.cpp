/*------------------------------------------------------------------------------
 *
 * (c) Copyright (see Date) by Alasdair Scott
 *
 * Name:    delay.cpp
 *
 * Module:  Ivory common (thread -unix specific)
 *
 * Author:  A Scott
 *
 * Date:    18 July 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Delay the current thread by the specified number of seconds
 *
 * Modification history:
 *                      
 *------------------------------------------------------------------------------
 */

#ifdef unix
#include <unistd.h>
#endif

#include "ivory/int.h"
#include "ivory/thread.h"
#include "ivory/void.h"

 // delay :: Int -> Void

defineBuiltInFn_1_arg(delay,
   typeCon(Int), typeCon(Void),
   i, Int)

#ifdef unix
   sleep(rInt);
#endif

   returnVoid();
endBuiltInFn
