/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    trampoline.cpp
 *
 * Module:  Ivory common (machine)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to execute continuation trampoline
 *
 * Call format:
 *
 *    callSequence$(label n_vm);
 *
 *       label          (Label)              - Initial label
 *       n_vm           (VirtualMachine&)    - Virtual machine (absent if global VM)
 *
 * Method:
 *
 *    Repeatedly invokes continuations until a null label is encountered.
 *
 * Errors:
 *
 *    None
 *
 * Notes:
 *
 *    1. Exceptions will be caught unless NO_INTERP_EXCEPTIONS is defined
 * 
 *    2. T.B.D. Consider using 'setjmp' and 'longjump' to exit the sequence.
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

#ifdef WIN32
#include <excpt.h>
#define FILTER_EXCEPTIONS
#endif
#include "ivory/machine.h"

RunTimeError::RunTimeError(const char* reason)
 : _reason(reason) {
}

#ifdef FILTER_EXCEPTIONS
int filter(Void) {
	return EXCEPTION_CONTINUE_SEARCH;
}
#endif

#define TRACE_JUMPS 0

#define N_JMP_TRACE_ENTRIES 100

#define traceOffset(offset)\
((offset)<jmpTraceN?jmpTrace[(jmpTraceN-((offset)+1))%N_JMP_TRACE_ENTRIES]:NULL)

#if (TRACE_JUMPS==1)
   static Label jmpTrace[N_JMP_TRACE_ENTRIES];
   static Int jmpTraceN = 0;

 Label jmpM50;
 Label jmpM49;
 Label jmpM48;
 Label jmpM47;
 Label jmpM46;
 Label jmpM45;
 Label jmpM44;
 Label jmpM43;
 Label jmpM42;
 Label jmpM41;
 Label jmpM40;
 Label jmpM39;
 Label jmpM38;
 Label jmpM37;
 Label jmpM36;
 Label jmpM35;
 Label jmpM34;
 Label jmpM33;
 Label jmpM32;
 Label jmpM31;
 Label jmpM30;
 Label jmpM29;
 Label jmpM28;
 Label jmpM27;
 Label jmpM26;
 Label jmpM25;
 Label jmpM24;
 Label jmpM23;
 Label jmpM22;
 Label jmpM21;
 Label jmpM20;
 Label jmpM19;
 Label jmpM18;
 Label jmpM17;
 Label jmpM16;
 Label jmpM15;
 Label jmpM14;
 Label jmpM13;
 Label jmpM12;
 Label jmpM11;
 Label jmpM10;
 Label jmpM9;
 Label jmpM8;
 Label jmpM7;
 Label jmpM6;
 Label jmpM5;
 Label jmpM4;
 Label jmpM3;
 Label jmpM2;
 Label jmpM1;
 Label jmpM0;

#endif

Void localCallSequence$(Label label argN_VM) {

#if (TRACE_JUMPS==1)
   if (jmpTraceN == 0)
      memset(jmpTrace, 0, sizeof(jmpTrace));
#endif

   do {
#if (TRACE_JUMPS==1)
      jmpM50 = traceOffset(50);
      jmpM49 = traceOffset(49);
      jmpM48 = traceOffset(48);
      jmpM47 = traceOffset(47);
      jmpM46 = traceOffset(46);
      jmpM45 = traceOffset(45);
      jmpM44 = traceOffset(44);
      jmpM43 = traceOffset(43);
      jmpM42 = traceOffset(42);
      jmpM42 = traceOffset(42);
      jmpM41 = traceOffset(41);
      jmpM40 = traceOffset(40);
      jmpM39 = traceOffset(39);
      jmpM38 = traceOffset(38);
      jmpM37 = traceOffset(37);
      jmpM36 = traceOffset(36);
      jmpM35 = traceOffset(35);
      jmpM34 = traceOffset(34);
      jmpM33 = traceOffset(33);
      jmpM32 = traceOffset(32);
      jmpM32 = traceOffset(32);
      jmpM31 = traceOffset(31);
      jmpM30 = traceOffset(30);
      jmpM29 = traceOffset(29);
      jmpM28 = traceOffset(28);
      jmpM27 = traceOffset(27);
      jmpM26 = traceOffset(26);
      jmpM25 = traceOffset(25);
      jmpM24 = traceOffset(24);
      jmpM23 = traceOffset(23);
      jmpM22 = traceOffset(22);
      jmpM22 = traceOffset(22);
      jmpM21 = traceOffset(21);
      jmpM20 = traceOffset(20);
      jmpM19 = traceOffset(19);
      jmpM18 = traceOffset(18);
      jmpM17 = traceOffset(17);
      jmpM16 = traceOffset(16);
      jmpM15 = traceOffset(15);
      jmpM14 = traceOffset(14);
      jmpM13 = traceOffset(13);
      jmpM12 = traceOffset(12);
      jmpM11 = traceOffset(11);
      jmpM10 = traceOffset(10);
      jmpM9  = traceOffset(9);
      jmpM8  = traceOffset(8);
      jmpM7  = traceOffset(7);
      jmpM6  = traceOffset(6);
      jmpM5  = traceOffset(5);
      jmpM4  = traceOffset(4);
      jmpM3  = traceOffset(3);
      jmpM2  = traceOffset(2);
      jmpM1  = traceOffset(1);
      jmpM0  = traceOffset(0);

      jmpTrace[jmpTraceN++ % N_JMP_TRACE_ENTRIES] = label;
#endif

      label = (Label)(*label)(vm);
   } while (label != NULL);
}

Void callSequence_$(Label label argN_VM) {
#ifndef NO_INTERP_EXCEPTIONS
   try
#endif
   {
      localCallSequence$(label n_vm);
   }
#ifndef NO_INTERP_EXCEPTIONS
   catch (RunTimeError err) {
      if (strlen(err.reason()) > 0) {
         outStream << "\n*** Run time error: " << err.reason() << '\n';
         outStream.flush();
      }
   }
#endif
}

Void callSequence$(Label label argN_VM) {
#ifdef FILTER_EXCEPTIONS
#ifndef NO_INTERP_EXCEPTIONS
   __try
#endif
#endif
   {
      callSequence_$(label n_vm);
   }
#ifdef FILTER_EXCEPTIONS
#ifndef NO_INTERP_EXCEPTIONS
   __except(filter())
   {
   }
#endif
#endif
}

defineEntry(runTimeError$) {
   throw(RunTimeError("No continuation label"));
   return NULL;
}
