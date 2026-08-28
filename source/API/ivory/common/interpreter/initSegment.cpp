/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    initSegment.cpp
 *
 * Module:  Ivory common (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2019
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to run a segment's initialisation code
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

#include "ivory/exec.h"
#include "ivory/segment.h"
#include "ivory/iSegmentTable.h"

// Define size of stack for initialisation code
// Determined by maximum nesting level

#define INIT_MAX_NESTING_LEVEL   32
#define INIT_STACK_SIZE          (INIT_MAX_NESTING_LEVEL * (sizeof(Label) + sizeof(IAddress)))

Void initSegment(const IAddress segment, Env& env) {

//--- Execute initialisation code with a minimal VM

#if (GLOBAL_VM == 0)
      VirtualMachine vm;
#else
      StackPtr prev_sp = sp;
      Env* prev_consEnv = consEnv;
      MSA* prev_consMSA = consMSA;
#endif

      Byte initStack[INIT_STACK_SIZE];

      sp = initStack + INIT_STACK_SIZE;
      consEnv = &env;
      consMSA = &env.msa();

      pushLabel(NULL);

      callIContinuation(initCodeAddr(segment) n_vm);

#if (GLOBAL_VM == 1)
      sp = prev_sp;
      consEnv = prev_consEnv;
      consMSA = prev_consMSA;
#endif
     
}
