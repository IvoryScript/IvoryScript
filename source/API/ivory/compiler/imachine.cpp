/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    imachine.cpp
 *
 * Module:  IvoryScript compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Implementation of regCodeToString
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

#include "ivory/iMachine.h"

const char* regCodeToString(UInt regCode) {
   switch (regCode) {
      case REG_consMSA:    return "consMSA";
      case REG_consEnv:    return "consEnv";
      case REG_sb:         return "sb";
      case REG_sp:         return "sp";

#if (GARBAGE_COLLECTION==1)
      case REG_fp:         return "fp";
#endif

      case REG_rack:       return "rack";
      case REG_cell:       return "cell";
      case REG_cellEnv:    return "cellEnv";
      case REG_rEnv:       return "rEnv";
      case REG_rName:      return "rName";
      case REG_rType:      return "rType";
      case REG_rCell:      return "rCell";
      case REG_rExpr:      return "rExpr";
      case REG_rByte:      return "rByte";
      case REG_rBits:      return "rBits";
      case REG_rInt:       return "rInt";
      case REG_rFloat:     return "rFloat";
      case REG_rDouble:    return "rDouble";
      case REG_rTag:       return "rTag";
      case REG_rChar:      return "rChar";
      case REG_rStruct:    return "rStruct";
      case REG_rStructSize:return "rStructSize";
      case REG_rPtr:       return "rPtr";
      case REG_rRef:       return "rRef";
      case REG_rUTC:       return "rUTC";
 
      default:             return "unknown register";
   }
}
