/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    timer_platform.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 July 2001
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Common platform related definitions for use with timers.
 *
 *    TimerInstance is an abstract base class for platform specific timers.
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

#ifndef TIMER_PLATFORM_H_DEFINED
#define TIMER_PLATFORM_H_DEFINED

#include "MSA.h"

class TimerInstance
{
public:
   virtual ~TimerInstance(void);

#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
   {
   }
#endif
   inline void operator delete(void* ptr)
   {
   }

   void signal(void);

public:
   void*    _adsi;
   UInt32   _ref;
   bool     _destroy;
};

extern TimerInstance* makeTimerInstance(UInt delay, bool periodic, MSA& msa);

#endif /* TIMER_PLATFORM_H_DEFINED */
