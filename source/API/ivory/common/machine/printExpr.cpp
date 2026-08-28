/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    printExpr.cpp
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
 *    Print expression.
 *
 * Call format:
 *
 *    printExpr(expr, os, env)
 *
 *       e           (Expr)         - Expression to print
 *       os          (ostream&)     - Output stream
 *       env         (Env&)         - Name and ADS environment
 *
 * Method:
 *
 *    Handles primitive types directly or invokes virtual function.
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

#include "ivory/binding.h"
#include "ivory/class.h"
#include "ivory/common.h"
#include "ivory/object.h"
#include "ivory/machine.h"
#include "ivory/trace.h"
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/string.h"
#include "ivory/tuple.h"

// Set PRINT_REFS to 0 or 1 as required

#define PRINT_REFS 1

Void printList(Expr list, ostream& os, const Env& env) {
   printExpr(hd(list), os, env);

   if (!isNil(tl(list))) {
      os << ',';
      printList(tl(list), os, env);
   }
}

Void printExpr(Expr expr, ostream& os, const Env& env) {
   if (expr == ERROR)
      os << "ERROR";
   else {
      UInt32 formOfExpr = formOf(expr);
      switch (formOfExpr) {
         case UNKNOWN:
            os << "UNKNOWN";
            break;

         case FAIL:
            os << "FAIL";
            break;

         case EXCEPTION:
            os << "EXCEPTION";
            break;

         case CONSTRAIN:
            os << "CONSTRAIN";
            break;

         case COERCE:
            os << "COERCE";
            break;

         case NO_COERCE:
            os << "NO_COERCE";
            break;

         case MAP_TO_ENV:
            os << "MAP_TO_ENV";
            break;

         case NO_MAP_TO_ENV:
            os << "NO_MAP_TO_ENV";
            break;

         case REDUCE:
            os << "REDUCE";
            break;

         case TYPE_SIG:
            os << "TYPE_SIG";
            break;

         case VOID:
            os << "VOID";
            break;

         case LITERAL:
         case CONCRETE_DATA_CON:
            os << "#";
            break;

         case SUBORDINATE:
            os << "subordinate";
            break;

         case CONSTANT:
            os << "CONSTANT";
            break;

         case VARIABLE:
            os << "VARIABLE";
            break;

         case INDISPENSABLE:
            os << "INDISPENSABLE";
            break;

         case TRANSIENT:
            os << "TRANSIENT";
            break;

         case PERSIST:
            os << "PERSIST";
            break;

         case PRIVATE:
            os << "PRIVATE";
            break;

         case PUBLIC:
            os << "PUBLIC";
            break;

         case THIS:
            os << "THIS";
            break;

         case CELL_INFO:
            os << "$CELL_INFO";
            break;

         case CELL_PTR:
            os << "$CELL_PTR";
            break;

         case ENV_PTR:
            os << "$ENV_PTR";
            break;

         case LABEL:
            os << "$LABEL";
            break;

         case VAR_PATTERN:
            os << "VAR_PATTERN";
            break;

         case TUPLE_PATTERN:
            os << "TUPLE_PATTERN";
            break;

         case DATA_CON_PATTERN:
            os << "DATA_CON_PATTERN";
            break;

         case CONCRETE_PATTERN:
            os << "CONCRETE_PATTERN";
            break;

         case PTR_DECON_PATTERN:
            os << "PTR_DECON_PATTERN";
            break;

         case PLAIN_PATTERN:
            os << "PLAIN_PATTERN";
            break;

         case AP:
            os << "$(";
            printExpr(fun(expr), os, env);
            os << ',';
            printExpr(arg(expr), os, env);
            os << ')';
            break;

         case INDIRECT:
            os << "IND(";
            //         printExpr(toBody(expr, EnvExpr).expr(), os, *toBody(expr, EnvExpr).env());
            os << ")";
            break;

         case ARROW:
            os << "->";
            break;

         case POINTER:
            os << "POINTER";
            break;

         case TYPE_CON:
            os << nameString_(toBody(expr, TypeCon).name(), toBody(expr, TypeCon).nameTable());
            break;

         case CLASS_DATA_CON:
         case DATA_CON: 
            os << nameString_(toBody(expr, DataCon).name(), env.nameTable());
            break;

         case BUILT_IN_DATA_CON:
            if (expr == Nil) {
               os << "[]";
               break;
            }
            else
               os << nameString_(toBody(expr, BuiltInDataCon).name(), env.nameTable());
            break;

         case BUILT_IN_FN:
            os << nameString_(toBody(expr, BuiltInFn)._name, env.nameTable());
            break;

         case CLASS_METHOD:
            os << nameString_(toBody(expr, ClassMethod)._name, env.nameTable());
            break;

         case MODULE:
            os << "MODULE";
            break;

         case ORDER:
            os << "ORDER";
            break;

         case SEQUENCE:
            os << "SEQUENCE";
            break;

         case PTR_CONSTRUCTOR:
            os << "PTR_CONSTRUCTOR";
            break;

         case CONSTRUCT:
            os << "CONSTRUCT";
            break;

         case DECONSTRUCT:
            os << "DECONSTRUCT";
            break;

         case DECONSTRUCT_PTR:
            os << "DECONSTRUCT_PTR";
            break;

         case SELECT:
            os << "SELECT";
            break;

         case SEL_PTR:
            os << "SEL_PTR";
            break;

         case SEQ:
            os << "SEQ";
            break;

         case DYNAMIC:
            os << "DYNAMIC";
            break;

         case TYPE_DEFN:
            os << "TYPE_DEFN";
            break;

         case OBJECT_DEFN:
            os << "OBJECT_DEFN";
            break;

         case INLINE:
            os << "INLINE";
            break;

         case PRIMITIVE:
            os << "PRIMITIVE";
            break;

         case DECL:
            os << "DECL";
            break;

         case DEFN:
            os << "DEFN";
            break;

         case DATA_CON_DECL:
            os << "DATA_CON_DECL";
            break;

         case DATA_CON_DEFN:
            os << "DATA_CON_DEFN";
            break;

         case CLASS:
            os << "CLASS";
            break;

         case INSTANCE:
            os << "INSTANCE";
            break;

         case CLASS_DEFN:
            os << "CLASS_DEFN";
            break;

         case INSTANCE_DEFN:
            os << "INSTANCE_DEFN";
            break;

         case EQUALITY:
            os << "EQUALITY";
            break;

         case INEQUALITY:
            os << "INEQUALITY";
            break;

         case CONJUNCTION:
            os << "CONJUNCTION";
            break;

         case DISJUNCTION:
            os << "DISJUNCTION";
            break;

         case NEGATION:
            os << "NEGATION";
            break;

         case CLASS_OBJECT_DEFN:
            os << "CLASS_OBJECT_DEFN";
            break;

         case LET_REC:
            os << "LET_REC";
            break;

         case LET:
            os << "LET";
            break;

         case LAMBDA:
            os << "LAMBDA";
            break;

         case CASE:
            os << "CASE";
            break;

         case WILDCARD_PATTERN:
            os << "WILDCARD_PATTERN";
            break;

         case OTHERWISE:
            os << "OTHERWISE";
            break;

         case IF:
            os << "IF";
            break;

         case COND:
            os << "COND";
            break;

         case RETURN:
            os << "RETURN";
            break;

         case LISTCOMP:
            os << "LISTCOMP";
            break;

         case GENERATOR:
            os << "GENERATOR";
            break;

         case FILTER:
            os << "FILTER";
            break;

         case STRICT:
            os << "STRICT";
            break;

         case TAG_OF:
            os << "TAG_OF";
            break;

         case FAT_BAR:
            os << "FAT_BAR";
            break;

         case FAT_BAR_SEQ:
            os << "FAT_BAR_SEQ";
            break;

         case NAME:
            printName(toName(expr), os, env.nameTable());
            break;

         case TYPE:
            if (toType(expr) >= 0)
               printType(toType(expr), os, env);
            else
               os << "NULL_TYPE";
            break;

         case INT:
            if (expr == Expr(INT))
               os << "Int";
            else
               os << toInt(expr);
            break;

         case TUPLE:
            if (expr == Expr(PAIR))
               os << "PAIR";
            else if (expr == Expr(TRIPLE))
               os << "TRIPLE";
            else {
               os << "TUPLE-" << (UInt)((toEnumVal(expr) - TUPLE_MIN) >> TAG_BITS) + 2;
            }
            break;

         case CHAR:
            if (expr == Expr(CHAR))
               os << "Char";
            else
               os << '\'' << toChar(expr) << '\'';
            break;

         case TAG:
            if (expr == Expr(TAG))
               os << "Tag";
            else
               os << toTag(expr);
            break;

         case CONST:
            os << "CONST";
            break;

         case CONS:
            if (!isPtr(expr))
               os << "CONS";
            else {
               os << '[';
               printList(expr, os, env);
               os << ']';
            }
            break;

         case CONS_ENV:
            os << "CONS_ENV";
            break;


         case DATA: {
            const CellInfo* cellInfo = static_cast<const CellInfo*>(toCell(expr).tag());
            const CellShowFn showFn = cellInfo->cellShowFn();
            if (showFn != NULL) {
               Bool isString = cellInfo->form() == fromType(builtInType(String));
               if (isString)
                  os << '"';

#if (GLOBAL_VM == 1)
#else
               VirtualMachine vm;
#endif

               (*showFn)(toCell(expr), os, env n_vm);
               if (isString)
                  os << '"';
            } else
               os << "???";
            break;
         }

         default:
            os << "(\?\?\?printExpr: unexpected form: " << hex << formOfExpr << dec << "\?\?\?)";;
            break;
      }
   }
}

void traceExpr(Expr expr, const Env& env) {
   printExpr(expr, outStream, env);
   outStream << "\n";
   outStream.flush();
}
