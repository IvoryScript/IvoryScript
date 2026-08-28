/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    analyser.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 1 December 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Declaration of 'Analyser' class and associated analysis functions
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

#ifndef IVORY_COMPILER_ANALYSER_H_DEFINED
#define IVORY_COMPILER_ANALYSER_H_DEFINED

#include "AST.h"

 // Analyse: AST analysis.

class Analyser {
public:
   Analyser(ConstString options, Env& env, MSA& msa, Lambda* lambda = NULL);

   inline ConstString options(Void) const { return _options; }
   inline const Env& env(Void) const { return _env; }
   inline MSA& msa(Void) const { return _msa; }
   inline NameTable& nameTable(Void) const { return _env.nameTable(); }
   inline Lambda* lambda(Void) const { return _lambda; }
   inline UInt lambdaDepth(Void) const { return _lambdaDepth; }
   inline Bool variableThunk(Void) const { return _variableThunk; }
   inline Bool closureValueBody(Void) const { return _closureValueBody; }
   inline Bool recInitAnalysis(Void) const { return _recInitDepth > 0; }
   inline Name recInitName(Void) const { return _recInitName; }
   inline TypedVal* recInitTypedVal(Void) const { return _recInitTypedVal; }
   inline UInt recInitLambdaDepth(Void) const { return _recInitLambdaDepth; }
   inline UInt recInitSelfValueLambdaDepth(Void) const {
      return _recInitSelfValueLambdaDepth;
   }
   inline UInt nErrors(Void) const { return _nErrors; }

   inline Void setLambda(Lambda* lambda) { _lambda = lambda; }
   inline Void setLambdaDepth(UInt lambdaDepth) { _lambdaDepth = lambdaDepth; }
   inline Void setVariableThunk(Bool q) { _variableThunk = q; }
   inline Void setClosureValueBody(Bool q) { _closureValueBody = q; }
   inline Void enterRecInitAnalysis(Void) { ++_recInitDepth; }
   inline Void leaveRecInitAnalysis(Void) { --_recInitDepth; }
   inline Void setRecInitBinding(Name name, TypedVal* typedVal,
                                 UInt lambdaDepth,
                                 UInt selfValueLambdaDepth) {
      _recInitName = name;
      _recInitTypedVal = typedVal;
      _recInitLambdaDepth = lambdaDepth;
      _recInitSelfValueLambdaDepth = selfValueLambdaDepth;
   }

   Void markRecNameOcc(Name name, TypedVal* typedVal, Bool needsClosureValue);
   Void error(ConstString s);

   Bool condGenThunk(Expr& expr, TypeSig typeSig, UInt nReduce);
   Void analyseValOf(Expr& expr, TypeSig typeSig, UInt nReduce);

   Void preAnalyseDeclOrDefn(Expr declOrDefn);
   Void preAnalyseDeclOrDefns(Expr declOrDefns);

   Void analyseDeclOrDefn(Expr declOrDefn);
   Void analyseDeclOrDefns(Expr declOrDefns);

   Void analyse(Expr expr);

protected:
   ConstString	_options;
   Env&        _env;
   MSA&        _msa;
   Lambda*     _lambda;
   UInt        _lambdaDepth;
   Bool        _variableThunk;
   Bool        _closureValueBody;
   UInt        _recInitDepth;
   Name        _recInitName;
   TypedVal*   _recInitTypedVal;
   UInt        _recInitLambdaDepth;
   UInt        _recInitSelfValueLambdaDepth;
   UInt        _nErrors;
};

#endif /* IVORY_COMPILER_ANALYSER_H_DEFINED */
