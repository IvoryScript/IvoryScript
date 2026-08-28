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
 * Original date: 24 March 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory timer module
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

#ifndef IVORY_TIMER_H_DEFINED
#define IVORY_TIMER_H_DEFINED

#include "ivory/class.h"
#include "ivory/propertySet.h"
#include "ivory/timer_platform.h"

declareTypeCon(Timer);

declareType(Timer);

declareDataCon(Timer);

declareInstance(PropertySetBasedObject_Timer);

class TimerRepr : public PropertySet
{
public:
   TimerRepr(Void)
      : PropertySet(), _ins(NULL)
   {
   }
   TimerRepr(const TimerRepr& src, const Env& srcEnv, Env& env, MSA& msa);
   void TimerRepr$(argVM);
   Void destroy(Env& env, MSA& msa);

protected:
   TimerInstance*   _ins;

   friend Label selectTimer_l1(argVM);
   friend Void insTimer(ADSRef& adsr argN_VM);
   friend Void desTimer(ADSRef& adsr);
};

declareBuiltInFn(monitorTimer);

#endif /* IVORY_TIMER_H_DEFINED */
