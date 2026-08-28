/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    iMachine.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Register definitions for use with IvoryScript byte code interpreter.
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

#ifndef IVORY_I_MACHINE_H
#define IVORY_I_MACHINE_H

#include "machine.h"

#define REG_consMSA     0
#define REG_consEnv     1
#define REG_sb          2
#define REG_sp          3

#if (GARBAGE_COLLECTION==1)
#define REG_fp          4
#endif

#define REG_rack        5

#define REG_cell        6
#define REG_cellEnv     7

#define REG_rEnv        8
#define REG_rName       9
#define REG_rType       10
#define REG_rCell       11
#define REG_rExpr       12
#define REG_rByte       13
#define REG_rBits       14
#define REG_rInt        15
#define REG_rFloat      16
#define REG_rDouble     17
#define REG_rTag        18
#define REG_rChar       19
#define REG_rStruct     20
#define REG_rStructSize 21
#define REG_rPtr        22
#define REG_rRef        23
#define REG_rUTC        24

#define N_REGS        (REG_rUTC+1)

extern const char* opCodeToString(UInt opCode);
extern const char* regCodeToString(UInt regCode);

#endif /* IVORY_I_MACHINE_H_DEFINED */
