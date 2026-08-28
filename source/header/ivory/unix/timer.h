/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    timer.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 19 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    unix specific definitions for use with timers
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

#ifndef IVORY_UNIX_TIMER_H_DEFINED
#define IVORY_UNIX_TIMER_H_DEFINED

#include "ivory/timer_platform.h"

class TimerInstance_unix : public TimerInstance
{
public:
   TimerInstance_unix(unsigned int delay, bool periodic);
   ~TimerInstance_unix();

   inline void* operator new(size_t size, MSA& msa)
   {
      void *ptr = msa.alloc(size);
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr, MSA& msa)
   {
      msa.free(ptr);
   }
#endif
   inline void operator delete(void* ptr)
   {
   }

   void toList(void);
   void fromList(void);

protected:
   TimerInstance_unix*  _next;
   TimerInstance_unix*  _prev;

   unsigned int         _delay;
   unsigned int         _periodic;   
   unsigned int         _ticks;

   friend bool timerActive(TimerInstance_unix* instance1);
   friend void checkTimers_unix(unsigned int timeDiff);
   friend bool mainTimeout_unix(unsigned int& delay);
};

extern void checkTimers_unix(unsigned int timeDiff);

extern bool mainTimeout_unix(unsigned int& delay);

#endif /* IVORY_UNIX_TIMER_H_DEFINED */
