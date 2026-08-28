/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    printType.cpp
 *
 * Module:  Ivory common
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Print type.
 *
 * Call format:
 *
 *    printType(name, os, env)
 *
 *       name  (type)         - Type
 *       os    (ostream&)     - Output stream
 *       env   (Env&)         - Type environment
 *
 * Method:
 *
 * Errors:
 *
 *    None.
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

#include "ivory/list.h"
#include "ivory/type.h"
#include "ivory/typeTable.h"

// typeSigHead: Return the head of a type signature

static TypeSig typeSigHead(TypeSig typeSig){
   return isAp(typeSig) ? typeSigHead(fun(typeSig))
                        : typeSig;
}

// printTypeSig: 'Pretty' print a type signature

static void printTypeSigBracket(UInt oPrec, UInt prec, Bool isRight,
										  Bool lAssoc, Bool openFlag, ostream& os) {
	if (prec < oPrec || prec == oPrec && isRight == lAssoc)
		os << (openFlag ? '(' : ')');
}

static Void printApTypeSig(TypeSig typeSig ,UInt prec, Bool isRight,
									ostream& os, const Env& env) {
	printTypeSigBracket(prec, 3, isRight, TRUE, TRUE, os);
	printTypeSig(fun(typeSig), 3, FALSE, os, env);
	os << ' ';
	printTypeSig(arg(typeSig), 3, TRUE, os, env);
	printTypeSigBracket(prec, 3, isRight, TRUE, FALSE, os);
}

static Void printDyadicTypeSigAp(TypeSig typeSig, UInt oPrec, UInt prec,
											Bool isRight, Bool lAssoc,
											ostream& os, const Env& env) {

	Expr& foo = arg(fun(typeSig));


	Cell* bar = fun(typeSig); //???

	printTypeSigBracket(oPrec, prec, isRight, lAssoc, TRUE, os);
	printTypeSig(arg(fun(typeSig)), prec, FALSE, os, env);
	printExpr(fun(fun(typeSig)), os, env);
	printTypeSig(arg(typeSig), prec, TRUE, os, env);
	printTypeSigBracket(oPrec, prec, isRight, lAssoc, FALSE, os);
}

static Void printCompsTypeSig(TypeSig typeSig, ostream& os, const Env& env) {
	if (isAp(fun(typeSig))) {
		printCompsTypeSig(fun(typeSig), os, env);
		os << ',';
	}
	TypeSig& foo = arg(typeSig);
	printTypeSig(arg(typeSig), 0, FALSE, os, env);
}

Void printTypeSig(TypeSig typeSig, UInt prec, Bool isRight,
						ostream& os, const Env& env) {
	if (isMonadicArrowTypeSig(typeSig))
		printApTypeSig(typeSig, 4, isRight, os, env);
	else if (isDyadicArrowTypeSig(typeSig))
		printDyadicTypeSigAp(typeSig, prec, 2, isRight, FALSE, os, env);
	else if (isAp(typeSig)) {
		if (isTupleCon(typeSigHead(typeSig))) {
			os << '(';
			printCompsTypeSig(typeSig, os, env);
			os << ')';
		} else if (fun(typeSig) == typeCon(List)) {
			os << '[';
         TypeSig* ts = &arg(typeSig);
			printTypeSig(arg(typeSig), 0, FALSE, os, env);
			os << ']';
		} else
			printApTypeSig(typeSig, prec, isRight, os, env);
   }
   else
      printExpr(typeSig, os, env);
}

// printCanonicalTypeSig: Print a type signature with canonical type variable names

Void printCanonicalTypeSig(TypeSig typeSig, UInt prec, Bool isRight,
                           ostream& os, Env& env) {
   printTypeSig(canonicalTypeSig(typeSig, env.nameTable(), env.msa()),
                prec, isRight, os, env);
}

// printTypeSigList: Print list of type signatures

Void printTypeSigList(Expr list, ostream& os, const Env& env, Bool inner/* = FALSE*/) {
	if (!inner)
		os << '[';
   printTypeSig(hd(list), 0, FALSE, os, env);
   if (!isNil(tl(list))) {
      os << ',';
      printTypeSigList(tl(list), os, env, TRUE);
   }
	if (!inner)
		os << ']';
}

// printType: Print a type

void printType(Type type, ostream& os, const Env& env) {
   printTypeSig(type < builtInTypeCount
               ? builtInTypeTable().typeSig(type)
               : env.typeTable().typeSig(type - builtInTypeCount), 0, FALSE, os , env);
}

// printCanonicalType: Print a type with canonical type variable names

void printCanonicalType(Type type, ostream& os, Env& env) {
   printCanonicalTypeSig(type < builtInTypeCount
                         ? builtInTypeTable().typeSig(type)
                         : env.typeTable().typeSig(type - builtInTypeCount),
                         0, FALSE, os, env);
}
