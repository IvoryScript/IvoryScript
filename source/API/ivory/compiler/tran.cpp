/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    tran.cpp
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
 *    Implementation of 'Tran' class and associated translation functions
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

#ifndef min
   #define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#define MODULE_TYPE_CON_NAME_MAP_N_SLOTS     47
#define MODULE_CLASS_INSTANCES_MAP_N_SLOTS   47

#include "OptionsParser.h"
#include "ivory/any.h"
#include "ivory/bool.h"
#include "ivory/class.h"
#include "ivory/double.h"
#include "ivory/env.h"
#include "ivory/extant.h"
#include "ivory/float.h"
#include "ivory/int.h"
#include "ivory/list.h"
#include "ivory/ptr.h"
#include "ivory/string.h"
#include "ivory/trace.h"
#include "ivory/compiler/CopyContext.h"
#include "ivory/compiler/class.h"
#include "ivory/compiler/tran.h"

#define TRACE

#define TRACE_FAT_BAR_SEQ				// Remove comment to trace fat bar sequences
#define TRACE_COPY_EXPR				   // Remove comment to trace copying of expressions

#define PRINT_TYPE_SIGS             // If set, type signatures are printed
#define PRINT_NAME_OCC_SUBSTS       // If set, name occurrence substitutions are printed

declareTypeCon(Ptr);                // Ptr is a primtive type

//declareBuiltInDataCon(Ptr);         // Ptr (and Null) are primitve data constructors

extern Void insertEscape(char c, ostream& os);
extern Void insertEscapedString(ConstString s, ostream& os);

struct PrimFnMapping {
   const char*    _fnStr;
   UInt32         _repr;
   TypeSig*       _typeSig;
};

static Bool eqExpr(Expr expr1, Expr expr2);

static Bool eqConst(Expr expr1, Expr expr2);

static Bool constPattern(Expr pattern);

static Bool isConst(Expr expr1);

static Bool varPattern(Expr pattern);

static Bool wildCardPattern(Expr pattern);

static Bool dataConPattern(Expr pattern);

static Bool concretePattern(Expr pattern);

#ifdef TRACE
static Bool traceFlag = FALSE;
//#define toggle(flag) ((flag)=!(flag))
#define toggle(flag) (flag=TRUE)
static Int tranProbeSeqNo = -1;
static Int copyProbeSeqNo = -1;
static Int reduceProbeSeqNo = -1;
#endif

// newName: Supply a unique new name
// T.B.D. Consider using class to hide

Name nameSupply = NULL_NAME;

// resetNameSupply: Reset the supply of names

Void resetNameSupply(Void) {
   nameSupply = NULL_TYPE_VAR;
}

Name newName(Void) {

#ifdef TRACE

//	The following conditional can be useful when debugging to find out where
// a variable has been introduced.

   Name name = NULL_NAME;
//   Name name = -<n>;
	if (nameSupply == name + 1 && traceFlag) {
      outStream << "*** newName: Variable probe hit\n";
		traceFlag = TRUE;
	}
#endif

   return --nameSupply;
}

// mapVarName: Map a variable name between environments

Name mapVarName(Name name, const CopyContext& cc, MSA& msa) {
   return cc.varNameMap()->use(name,
                               cc.nameTable(),
                               newName,
                               msa);
}

// curriedFn: Return a curried function

static Expr curriedFn(Expr fun) {
   return isAp(fun) ? curriedFn(fun(fun))
                    : fun;
}

// Evaluated form of expression

static Expr evaluated(Expr expr) {
   while (isReduce(expr))
      expr = snd(expr);
   return expr;
}

// isNilaryDataCon: Nilary data constructor predicate

static Bool isNilaryDataCon(Expr dataCon) {
   return !isTupleCon(dataCon) && toBody(dataCon, DataCon).arity() == 0;
}

// dataConTag: A data constructor's tag

Tag dataConTag(Expr dataCon) {
   return isTupleCon(dataCon)
      ? 0
      : toBody(dataCon, DataCon).tag();
}

// dataConArity: A data constructor's arity

UInt dataConArity(Expr dataCon) {
   return isTupleCon(dataCon)
      ? tupleCard(dataCon)
      : toBody(dataCon, DataCon).arity();
}

Void newlineAndIndent(UInt level, ostream& os, Bool newline = TRUE) {
   if (newline)
      os << '\n';
   for (UInt i = 0; i < level; i++)
      os << "  ";
}

// printTranName: Print translated name

Void printTranName(Name name, ostream& os, const Env& env) {
   printName(name, os, env.nameTable());
}

// printTranList: Print translated list

Void printTranList(Expr list, Bool inner, ostream& os, const Env& env,
                   Bool pretty/* = FALSE*/, UInt indent/* = 0*/,
                   Bool itemPerLine/* = FALSE*/) {
   if (isNil(list)) {
      if (!inner)
         os << '[';
      os << ']';
   } else {
      os << (!inner ? '[' : ','); 
      if (pretty) {
         if (itemPerLine)
            newlineAndIndent(indent, os);
         else
            os << ' ';
      }
      printTranExpr(hd(list), FALSE, os, env, pretty, indent);
      printTranList(tl(list), TRUE, os, env, pretty, indent, itemPerLine);
   }
}

// printTranV: Print translated vector

Void printTranV(UInt n, Expr* v, ostream& os, const Env& env,
                Bool pretty/* = FALSE*/, UInt indent/* = 0*/) {
for (UInt i = 0; i < n; i++) {
      if (i > 0)
         os << ',';
      printTranExpr(v[i], FALSE, os, env, pretty, indent);
   }
}

// printTranExpr: Print translated expression

Void printTranExpr(Expr expr, Bool bFlag, ostream& os, const Env& env,
                   Bool pretty/* = FALSE*/, UInt indent/* = 0*/) {
   Bool needsB = FALSE;
   switch (formOf(expr)) {
      case CONST:
         if (isPtr(expr)) {
            toBody(expr, Const).print(os, env);
            return;
         }
         break;

     case NAME:
         printTranName(toName(expr), os, env);
         return;

      case VAR:
         printTranName(toBody(expr, Var)._name, os, env);
         return;

      case VAR_REF:
         printTranName(toBody(expr, VarRef).var().name(), os, env);
         return;

      case CONST_PATTERN:
         if (isPtr(expr)) {
            toBody(expr, ConstPattern).print(os, env);
            return;
         }
         break;

      case VAR_PATTERN:
         if (isPtr(expr)) {
            toBody(expr, VarPattern).print(os, env);
            return;
         }
         break;

      case WILDCARD_PATTERN:
         if (isPtr(expr)) {
            toBody(expr, WildCardPattern).print(os, env);
            return;
         }
         break;

		case DATA_CON_PATTERN:
         if (isPtr(expr)) {
            toBody(expr, DataConPattern).print(os, env);
            return;
         }
         break;

      case CONCRETE_PATTERN:
         if (isPtr(expr)) {
            toBody(expr, ConcretePattern).print(os, env);
            return;
         }
         break;

      case CONS:
         printTranList(expr, FALSE, os, env, pretty, indent == 0 ? 0 : indent + 1);
         return;

      case DATA: {
         Type type = toType(((const CellInfo*)toCell(expr).tag())->form());
         if (type == builtInType(Pair)) {
            Expr f = fst(expr);
            Expr s = snd(expr);
            if (pretty && f == Expr(REDUCE)) {
               UInt32 forms[] = { FN_AP, LAMBDA, LET, LET_REC };
               os << "#!(";
               if (formExistsIn(s, 4, forms))
                  newlineAndIndent(indent + 1, os);
               printTranExpr(s, FALSE, os, env, pretty, indent + 1);
               os << ')';
            } else if (f == Expr(LITERAL)) {
               os << '#';
               printTranExpr(s, FALSE, os, env, pretty, indent);
            }
            else {
               os << '(';
               printTranExpr(f, FALSE, os, env, pretty, indent);
               os << ',';
               printTranExpr(s, FALSE, os, env, pretty, indent);
               os << ')';
            }
         }
         else if (type == builtInType(Triple)) {
            Expr f = fst3(expr);
            Expr s = snd3(expr);
            Expr t = thd3(expr);
            os << '(';
            printTranExpr(f, FALSE, os, env, pretty, indent);
            os << ',';
				if (!(f == Expr(SELECT) ||
                  f == Expr(SEL_PTR)))
               printTranExpr(s, FALSE, os, env, pretty, indent);
				else
					printTypeSig(s, 0, FALSE, os, env);
            os << ',';
            if (f != Expr(CONSTRAIN))
               printTranExpr(t, FALSE, os, env, pretty, indent);
            else
               printTypeSig(t, 0, FALSE, os, env);
            os << ')';
         }
         else if (type == builtInType(String))
            insertEscapedString(toString(expr), os);
         else
            break;
         return;
      }

      case EQ_TAG_FN:
         os << "eqTAG";
         return;

      case EQ_NULL_FN:
         os << "eqNULL";
         return;         

      case RETURN_STATE:
         os << "RETURN_STATE";
         return;

      case SHARED:
         os << "SHARED";
         return;

      case DISPATCH:
         os << "DISPATCH";
         return;

      case CHAR:
         os << '\''; insertEscape(toChar(expr), os); os << '\'';
         return;

      case AP: {
         os << '$';
         if (pretty)
            newlineAndIndent(indent + 1, os);
         else
            os << ',';
         printTranExpr(fun(expr), TRUE, os, env, pretty, indent + 1);
         if (pretty)
            newlineAndIndent(indent + 1, os);
         else
            os << ',';
         printTranExpr(arg(expr), TRUE, os, env, pretty, indent + 1);
         return;
      }

		case NAME_TYPED_VAL_BINDING:
         toBody(expr, NameTypedValBinding).print(TRUE, os, env, pretty, indent);
         return; 

      case MODULE:
			if (isPtr(expr)) {
            toBody(expr, ModuleDefn).print(os, env, pretty);
				return;
			}
         break;

      case ORDER:
			if (isPtr(expr)) {
            toBody(expr, Order).print(os, env, pretty);
				return;
			}
         break;

      case SNIPPET:
         if (isPtr(expr)) {
            toBody(expr, Snippet).print(os, env, pretty);
            return;
         }
         break;


      case TYPE_DEFN:
			if (isPtr(expr)) {
            toBody(expr, TypeDefn).print(os, env, pretty);
				return;
			}
         break;

      case CLASS_DEFN:
			if (isPtr(expr)) {
            toBody(expr, ClassDefn).print(os, env, pretty);
				return;
			}
         break;

      case CLASS_OBJECT_DEFN:
         if (isPtr(expr)) {
            toBody(expr, ClassObjectDefn).print(os, env, pretty);
            return;
         }
         break;

      case INSTANCE_DEFN:
			if (isPtr(expr)) {
            toBody(expr, InstanceDefn).print(os, env, pretty);
				return;
			}
			break;

      case LAMBDA:
      case FN_AP:
		case APP_LAMBDA:
		case FAT_BAR_SEQ:
      case CASE:
      case LET:
      case SHARED_EXPR:
      case SHARED_PATTERN_AP:
        needsB = TRUE;
         // drop through

      case DATA_CON_DEFN:
      case CONCRETE_DATA_CON:
      case DECL:
      case DEFN:
      case CLASS_METHOD_DECL:
      case CLASS_METHOD_DEFN:
      case INSTANCE_METHOD_DECL:
      case INSTANCE_METHOD_DEFN:
      case INSTANCE_DATA_CON_DECL:
      case INSTANCE_DATA_CON_DEFN:
      case NAME_OCC:
      case COERCE:
         if (isPtr(expr)) {
            if (needsB && bFlag)
               os << '(';
            toBody(expr, AST_Node).print(os, env, pretty, indent);
            if (needsB && bFlag)
               os << ')';
				return;
         }
         break;

      default:
         break;
   }
   printExpr(expr, os, env);
   outStream.flush();
}

// isConst: Predicate for constant expression

Bool isConst(Expr expr) {
   if (isPair(expr))
      return fst(expr) == Expr(LITERAL)/* ||
             fst(expr) == Expr(REDUCE) && isConst(snd(expr))*/;
   else
      switch (formOf(expr)) {
         case LAMBDA:
         case TYPE_CON:
         case CHAR:
         case INT:
         case TAG:
         case DATA:
            return TRUE;
         default:
            break;
      }
   return FALSE;
}

// eqExpr:: equality function for expression

Bool eqExpr(Expr expr1, Expr expr2) {
   return expr1 == expr2;
}

// unconstrainedPattern: Return unconstrained pattern

Expr& unconstrainedPattern(Expr& pattern) {
   return isTriple(pattern) && fst3(pattern) == Expr(CONSTRAIN)
      ? unconstrainedPattern(snd3(pattern))
      : pattern;
}


// constPattern: predicate to test for a constant pattern

Bool constPattern(Expr pattern) {
   if (isPair(pattern))
      return fst(pattern) == Expr(LITERAL) ||
             fst(pattern) == Expr(TYPE_SIG);
   else switch (formOf(pattern)) {
      case TYPE_CON:
      case CHAR:
      case INT:
      case CONST_PATTERN:
         return TRUE;

      case DATA: {
         Type type_ toType(exprCellInfo(pattern).form());
         return type_ == builtInType(Int) ||
                type_ == builtInType(Float) ||
                type_ == builtInType(Double) ||
                type_ == builtInType(String);
      }

      default:    return FALSE;
   }
}

// varPattern: Predicate to test for a variable pattern

Bool varPattern(Expr pattern) {
   return isName(pattern) ||
          isPtr(pattern) && formOf(pattern) == VAR_PATTERN;
}

// wildCardPattern: Predicate to test for a wildcard pattern

Bool wildCardPattern(Expr pattern) {
   return pattern == Expr(WILDCARD_PATTERN) ||
          isPtr(pattern) && formOf(pattern) == WILDCARD_PATTERN;
}

// varOrWildCardPattern: Predicate to test for a variable or wildcard pattern

Bool varOrWildCardPattern(Expr pattern) {
   return varPattern(pattern) ||
          wildCardPattern(pattern);
}

// dataConPattern: Predicate to test for a data constructor pattern

Bool dataConPattern(Expr pattern) {
   return isPtr(pattern) && (formOf(pattern) == DATA_CON_PATTERN ||
                             formOf(pattern) == CONCRETE_PATTERN) ||
          isPair(pattern) && fst(pattern) == Expr(TUPLE_PATTERN) ||
          isTriple(pattern) && (fst3(pattern) == Expr(DATA_CON_PATTERN) ||
                                fst3(pattern) == Expr(CONCRETE_PATTERN));
   }

// varPatternName: Extract the name from a variable pattern

Name varPatternName(Expr pattern) {
   return toBody(pattern, VarPattern).name();
}

// patternTypeSig: The type signature of a pattern

static Bool hasNonMatchPatternTypeVar(TypeSig typeSig) {
   if (isTypeVar(typeSig))
      return TRUE;
   else if (isAp(typeSig))
      return hasNonMatchPatternTypeVar(fun(typeSig)) ||
             hasNonMatchPatternTypeVar(arg(typeSig));
   else
      return FALSE;
}

static Bool eqMatchTypeSig(TypeSig typeSig1, TypeSig typeSig2) {
   TypeSig ts1 = unwrapPlainTypeSig(typeSig1);
   TypeSig ts2 = unwrapPlainTypeSig(typeSig2);

   if (isAp(typeSig1) && isAp(typeSig2))
      return eqMatchTypeSig(fun(typeSig1), fun(typeSig2)) &&
             eqMatchTypeSig(arg(typeSig1), arg(typeSig2));

   return eqTypeSig(typeSig1, typeSig2);
}

TypeSig patternTypeSig(Expr& pattern, Tran& tran) {
   assert(tran.pass() > 1, "patternTypeSig: unexpected");
   if (isTriple(pattern) && fst3(pattern) == Expr(CONSTRAIN))
      pattern = snd3(pattern);
 
   TypeSig& typeSig = toBody(pattern, Pattern).typeSig();
   Subst::substTypeVars(typeSig, tran.substs());
   ExprRepr forms[] = { DATA_CON_PATTERN, CONCRETE_PATTERN };
   if (formExistsIn(pattern, 2, forms)) {
      DataConPattern& dataConPattern = toBody(pattern, DataConPattern);
      Subst::substSubsts(dataConPattern._substs, tran.substs());
      Subst::substTypeVars(dataConPattern._reprTypeSig, tran.substs());
      UInt arr = arity(typeSig);
      return dataConPattern.condTypeSig(arr == 0
         ? typeSig
         : resultTypeSig(typeSig, arr), tran.msa());
   }
   else
      return typeSig;
}

Expr concatPattern(Expr patternList, const Tran& tran) {
   return concat(thd3(hd(patternList)), tl(patternList), tran.msa());
}

// eqConst: Test two constants of the same type for equality

Bool eqConst(Expr const1, Expr const2) {
   if (isPair(const1))
      return snd(const1) == snd(const2); // compare names
   else switch (formOf(const1)) {
      case TYPE_CON:
         return eqTypeSig(const1, const2);
      case CHAR:
         return toChar(const1) == toChar(const2);
      case INT:
         return toInt(const1) == toInt(const2);

      case DATA: {
         Type type_ toType(exprCellInfo(const1).form());
         if (type_ == builtInType(Int))
            return toInt(const1) == toInt(const2);
         else if (type_ == builtInType(Float))
            return toFloat(const1) == toFloat(const2);
         else if (type_ == builtInType(Double))
            return toDouble(const1) == toDouble(const2);
         else if (type_ == builtInType(String))
            return strcmp(toString(const1), toString(const2)) == 0;
      }
      // drop through

      default:
         error("Tran::eqConst: invalid constant");
         return FALSE;
   }
}

// eqDataCon: Test two data constructor patterns for equality

Bool eqDataConPattern(Expr prefix1, Expr prefix2) {
   DataConPattern& dataConPattern1 = toBody(prefix1, DataConPattern);
   DataConPattern& dataConPattern2 = toBody(prefix2, DataConPattern);

   return formOf(prefix1) == formOf(prefix2) &&
          dataConPattern1.dataCon_() == dataConPattern2.dataCon_();
}

// idForm: Variable name and type from Name or (Name, TypeSig)

Void idForm(Expr signedId, Name& name, TypeSig& typeSig) {
   if (isName(signedId)) {
      name = toName(signedId);
      typeSig = UNKNOWN;
   }
   else if (signedId == Expr(WILDCARD_PATTERN)) {
      name = NULL_NAME;
      typeSig = UNKNOWN;
   } else {
		assert(isTriple(signedId), "idForm: expected triple");
		Expr s = snd3(signedId);
		Expr t = thd3(signedId);
		if (isName(s)) {
			name = toName(s);
			typeSig = t;
		} else
			assert(TRUE, "idForm: unexpected form");
   }
}

/*----------------------------------------------------------------------------*/

TypeSig typeDefnTypeSig(Expr typeVars, Expr typeSig, MSA& msa) {
   return typeVars != Nil
      ? typeDefnTypeSig(tl(typeVars), ap(typeSig, hd(typeVars), msa), msa)
      : typeSig;
}

// TypeDefn::TypeDefn: Constructor for a type definition

TypeDefn::TypeDefn(Name name, Expr typeVars, Expr dataConDecls,
                   Tran& tran, Bool objectFlag/* = FALSE */)
   : _objectFlag(FALSE), _dataConDecls(dataConDecls), _nilaryDataConDefns(Nil) {
   UInt hashVal = nameHashVal(name, tran.nameTable());
   if (!(name < builtInNameCount && 
         builtInTypeConNameMap().lookUp(name, hashVal, _typeCon))) {
      Cell& cell_ = *new(sizeof(TypeCon), tran.msa()) Cell(TYPE_CON);
      _typeCon = fromCell(cell_);

      new(cell_.body()) TypeCon(name, hashVal, tran.nameTable(), NULL, objectFlag);

//--- Add to module map from name to type constructor cell

      tran._moduleDefn->typeConNameMap()->add(NameExprBinding(name, fromCell(cell_)),
                                             hashVal,
                                             tran.msa());

      cellBody(cell_, TypeCon)._typeVars = typeVars;
   }
}

// For virtual derived classes

Void TypeDefn::postConstruct(Tran& tran) {
   Tag tag = 0;
   mapOver3(addDataCon,
            _dataConDecls,
            typeDefnTypeSig(toBody(_typeCon, TypeCon)._typeVars, _typeCon, tran.msa()), tag, tran);
}

TypeSig functionTypeSig(Expr typeSigs, TypeSig resTypeSig, MSA& msa) {
   return typeSigs != Nil
      ? TypeSig(ap2(ARROW, hd(typeSigs), functionTypeSig(tl(typeSigs), resTypeSig, msa), msa))
      : resTypeSig;
}

Expr TypeDefn::allocDataCon(Name name, Tag tag, TypeSig typeSig, MSA& msa) {
   Cell& cell_ = *new(sizeof(DataCon), msa) Cell(DATA_CON);

   new(cell_.body()) DataCon(name, tag, typeSig, UNKNOWN);
   return  fromCell(cell_);
}

Expr TypeDefn::addDataCon(Expr dataConDecl, TypeSig resTypeSig, Tag& tag, Tran& tran) {
   Name name = toName(fst(dataConDecl));
   Expr dataConExpr;

   TypeCon& typeCon = toBody(_typeCon, TypeCon);
   TypeSig typeSig;
   if (!(name < builtInNameCount &&
       builtInNameMap().lookUp(name, builtInNameTable().hashVal(name), dataConExpr))) {
      typeSig = functionTypeSig(snd(dataConDecl), resTypeSig, tran.msa());

      dataConExpr = allocDataCon(name, tag++, typeSig, tran.msa());

//--- Append to type constructor list

      typeCon.addDataCon(&toBody(dataConExpr, DataCon));

   } else
      typeSig = toBody(dataConExpr, BuiltInDataCon).typeSig(), tran.msa();


   //--- Nilary data constructors are not required to be defined, so
   //--- are defined implicitly at this point (except Null)
   //--- (although they may also be defined explicitly in rare cases)

   if (snd(dataConDecl) == Nil) {
      Cell& cell_ = *new(sizeof(DataConDefn), tran.msa()) Cell(DATA_CON_DEFN);
      if (_typeCon == typeCon$Ptr) {

//------ Null is built-in
         Cell& nullCell = *new(sizeof(Const_NULL_Ptr), tran.msa()) Cell(CONST);
         new(nullCell.body()) Const_NULL_Ptr(POINTER);
         new(cell_.body()) DataConDefn(mkPair(fromName(name),
                                             fromCell(nullCell),
                                             tran.msa()),
                                      Nil, TRUE, TRUE);
      } else
         new(cell_.body()) DataConDefn(mkPair(fromName(name),
                                             VOID,
                                             tran.msa()),
                                      Nil, TRUE, /*hasTypeVar(typeSig)*/TRUE);
      cellBody(cell_, DataConDefn).tran(tran);
      _nilaryDataConDefns = concat(_nilaryDataConDefns, cons(fromCell(cell_), Nil, tran.msa()), tran.msa());
   }
   typeSig = expTypeSig(typeSig, tran.msa());

   return tran.nameTypedValBinding(name, dataConExpr, typeSig);
}

Expr TypeDefn::tran(Expr expr, Tran& tran) {
   return expr;
}


Expr TypeDefn::tran(Expr expr, BindingSet& bindingEnv, Tran& tran) {
   mapProc2(tranNilaryDataConDefn, _nilaryDataConDefns, bindingEnv, tran);
   return expr;
}

Void TypeDefn::tranNilaryDataConDefn(Expr nilaryDataConDefn, BindingSet& bindingEnv, Tran& tran) {
   toBody(nilaryDataConDefn, DataConDefn).tran(bindingEnv, tran);
}

Void TypeDefn::addBindings(BindingSet& bindingEnv, Tran& tran) {
	tran.addBindings(_dataConDecls, bindingEnv);
   mapProc2(addNilaryDataConBindings, _nilaryDataConDefns, bindingEnv, tran);
}

Void TypeDefn::addNilaryDataConBindings(Expr nilaryDataConDefn, BindingSet& bindingEnv, Tran& tran) {
   toBody(nilaryDataConDefn, DataConDefn).addBindings(nilaryDataConDefn, bindingEnv, tran);
}

Void TypeDefn::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/) const {
	printTypeSig(_typeCon, 0, FALSE, outStream, env);
   os << (pretty ? " = " : ",");
   printTranList(_dataConDecls, FALSE, outStream, env, pretty, 1);
}

// BindingSet::BindingSet: Constructor for a set of bindings

BindingSet::BindingSet(BindingSet* next /* = NULL */, Lambda* lambda /* = NULL */)
   : _next(next), _lambda(lambda), _bindings(NULL)
{
   if (_lambda == NULL && _next != NULL)
      _lambda = _next->_lambda;
}

// BindingSet::add: Add a new binding
/*
Void BindingSet::add(NameTypedValBinding& binding) {

// Append to list, or before previous with matching name

   NameTypedValBinding** bindingPtr = &_bindings;
   while (*bindingPtr != NULL && binding.name() != (*bindingPtr)->name())
      bindingPtr = &(*bindingPtr)->_next;
   binding._next = *bindingPtr;
   *bindingPtr = &binding; 
}
*/
Void BindingSet::add(NameTypedValBinding& binding) {
   _bindings_.add(binding);
}

// BindingSet::lookUp: Find binding for name in list of binding sets (the environment)

TypedVal* BindingSet::lookUp(Name name, Bool dataCon, Bool nested/* = TRUE*/) const {
   NameTypedValBinding* binding = _bindings_.lookUp(name);
   if (dataCon) {
      ExprEnumVal forms[] = { BUILT_IN_DATA_CON, DATA_CON };
      while (binding != NULL && !formExistsIn(binding->val().val(), 2, forms))
         binding = binding->next();
   }
   return binding != NULL
      ? &binding->typedVal()
      : (nested && _next != NULL
           ? _next->lookUp(name, dataCon)
           : NULL);
}

Void BindingSet::print(ostream& os, const Env& env) const {
  NameTypedValBinding* binding = _bindings;
  UInt n = 0;
   while (binding != NULL) {
      if (n++ > 0)
         os << ' ';
      printName(binding->name(), os, env.nameTable());
      binding = binding->_next;
   }
   os << '\n';
   if (_next != NULL)
      _next->print(os, env);
}


/*----------------------------------------------------------------------------*/

NameBindingSet::NameBindingSet(Void)
   : BasicChainedHashTable_<NameTypedValBinding, Name,
                            N_NAME_BINDING_HASH_TABLE_SLOTS>() {
}

Void NameBindingSet::print(ostream& os, const Env& env) const {
   for (UInt i = 0; i < nSlots(); i++) {
      NameTypedValBinding* node = slot(i);
      while (node != NULL) {
         node->print(FALSE, os, env);
         os << endl;
         node = node->next();
      }
   }
}

/*----------------------------------------------------------------------------*/

TypedVal::TypedVal(Expr val, TypeSig typeSig)
   : _val(val), _typeSig(typeSig), _substs(NULL),
   _schematic(TRUE), _needsInstance(FALSE), _schemeConstraint(NULL),
   _typeEnv(NULL) {
}

TypedVal::TypedVal(const TypedVal& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran)
   : _val(tran.copyExpr(src._val, cc, typeInsts, substs)),
     _typeSig(tran.copyTypeSig(src._typeSig, cc, substs)),
     _substs(cc == NULL ? src._substs : Subst::copySubsts(src._substs, *cc, tran.msa())),
     _schematic(src._schematic), _needsInstance(FALSE), _schemeConstraint(src._schemeConstraint),
     _schematicTypeVars(TypeVarSet(src._schematicTypeVars, cc, tran.msa())),
     _typeEnv(NULL) {
}

// TypedVal::tran: Translate

Void TypedVal::tran(BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() == 1)
      tran.tranExpr(_val, bindingEnv);
   else {
      Subst::substTypeVars(_typeSig, tran.substs());
      tran.tranExpr(_val, bindingEnv);
   }
}

// TypedVal::print: Print

Void TypedVal::print(ostream& os, const Env& env,
                     Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   if (formOf(_val) != BUILT_IN_DATA_CON &&
       formOf(_val) != DATA_CON) //???
      printTranExpr(_val, FALSE, os, env, pretty, indent);
}

/*----------------------------------------------------------------------------*/

// NameTypedValBinding::NameTypedValBinding: Name/TypedVal binding constructor

NameTypedValBinding::NameTypedValBinding(Name name, Expr val, TypeSig typeSig,
                                         MSA& msa)
 : NameBinding(name, TypedVal(val, typeSig)),
   _next(NULL), _count(0) {
}

NameTypedValBinding::NameTypedValBinding(const NameTypedValBinding& src,
                                         CopyContext* cc, const TypeInsts* typeInsts,
                                         Subst* substs, Tran& tran)
 : NameBinding(cc == NULL
                    ? src.name()
                    : mapVarName(src.name(), *cc, tran.msa()),
                 TypedVal(src._val, cc, typeInsts, substs, tran)),
   _next(NULL), _count(0) {
}

// NameTypedValBinding::nameString: Return name as string

ConstString NameTypedValBinding::nameString(const Env& env) const {
	return ::nameString_(name(), env.nameTable());
}

// NameTypedValBinding::addBinding: Add name to environment

Void NameTypedValBinding::addBinding(BindingSet& bindingEnv) {
   bindingEnv.add(*this);
}

// NameTypedValBinding::tran: Translate a name/typed value binding

Void NameTypedValBinding::tran(BindingSet& bindingEnv, Tran& tran, Bool addBinding/* = TRUE*/) {
   if (addBinding)
      bindingEnv.add(*this);  // assume recursive

#ifdef TRACE
   if (traceFlag) {
      outStream << "NameTypedValBinding::tran ";
      printName(name(), outStream, tran.nameTable());
      outStream << '\n';
   }
#endif

   Name prevResolverBindingName = tran._resolverBindingName;
   TypeSig prevResolverBindingTypeSig = tran._resolverBindingTypeSig;
   tran._resolverBindingName = name();
   tran._resolverBindingTypeSig = typedVal().typeSig();
   typedVal().tran(bindingEnv, tran);
   tran._resolverBindingName = prevResolverBindingName;
   tran._resolverBindingTypeSig = prevResolverBindingTypeSig;
}

// NameTypedValBinding::copy: Copy a name/typed value binding

Expr NameTypedValBinding::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(NameTypedValBinding), tran.msa()) Cell(NAME_TYPED_VAL_BINDING);
   new(cell_.body()) NameTypedValBinding(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// NameTypedValBinding::tran: Print

Void NameTypedValBinding::print(Bool valFlag, ostream& os, const Env& env,
                                Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
//   if (pretty)
//      newlineAndIndent(indent, os);
   printTranName(name(), os, env);

#ifdef PRINT_TYPE_SIGS
   if (typedVal().typeSig() != TypeSig(UNKNOWN)) {
      os << "::";
      printTypeSig(typedVal().typeSig(), 0, FALSE, os, env);
      if (!typedVal()._schematicTypeVars.isEmpty()) {
         os << '{';
         typedVal()._schematicTypeVars.print(os, env);
         os << '}';
      }
//      if (typedVal().substs() != NULL) {
//         os << endl;
//         Subst::printSubsts(typedVal().substs(), outStream, env);
//      }
   }
#endif

//   os << '(' << (Void*)&val() << ')';   // Binding address

   if (valFlag)
      typedVal().print(os, env, pretty, indent);
}

TypedVal* NameTypedValBinding::lookUp(Name name, NameTypedValBinding* nameTypedValBinding) {
   while (nameTypedValBinding != NULL) {
      if (nameTypedValBinding->_id == name)
         return &nameTypedValBinding->_val;
      nameTypedValBinding = nameTypedValBinding->_next;
   }
   return NULL;
}

/*----------------------------------------------------------------------------*/

// ConstPattern::ConstPattern: Constant pattern constructor

ConstPattern::ConstPattern(Expr k, Expr eqOcc, TypeSig typeSig)
   : _k(k), _eqOcc(eqOcc), _typeSig(typeSig) {
}

ConstPattern::ConstPattern(const ConstPattern& src, CopyContext* cc,
                           const TypeInsts* typeInsts,
                           Subst* substs, Tran& tran)
   : _k(tran.copyExpr(src._k, cc, typeInsts, substs)), 
     _eqOcc(toBody((Expr)src._eqOcc, NameOcc).copy(cc, typeInsts, substs, tran)),
     _typeSig(tran.copyTypeSig(src._typeSig, cc, substs)) {
}

// ConstPattern::tran: Translate a constant pattern

Void ConstPattern::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   Subst::substTypeVars(_typeSig, tran.substs());
   tran.tranExpr(_k, bindingEnv);      // The pattern may include a type variable
   tran.tranExpr(_eqOcc, bindingEnv);
}

// ConstPattern::copy: Copy a constant pattern

Expr ConstPattern::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(ConstPattern), tran.msa()) Cell(CONST_PATTERN);
   new(cell_.body()) ConstPattern(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// ConstPattern::print: Print a constant pattern

Void ConstPattern::print(ostream& os, const Env& env,
                         Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "(CONST_PATTERN,";
   printTranExpr(_k, FALSE, os, env);
   os << ')';
}

/*----------------------------------------------------------------------------*/

// VarPattern::VarPattern: Variable pattern constructors

VarPattern::VarPattern(Expr v, TypeSig typeSig, MSA& msa)
   : _binding(toName(v), VAR_PATTERN, typeSig, msa) {
}

VarPattern::VarPattern(const VarPattern& src, CopyContext* cc,
                       const TypeInsts* typeInsts,
                       Subst* substs, Tran& tran)
   : _binding(src._binding, cc, typeInsts, substs, tran) {
}

// VarPattern::tran: Translate a variable pattern

Void VarPattern::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   _binding.tran(bindingEnv, tran);
}

// VarPattern::copy: Copy a variable pattern

Expr VarPattern::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                      Tran& tran) const {
   Cell& cell_ = *new(sizeof(VarPattern), tran.msa()) Cell(VAR_PATTERN);
   new(cell_.body()) VarPattern(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// VarPattern::print: Print variable pattern

Void VarPattern::print(ostream& os, const Env& env,
                       Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
    os << "VAR_PATTERN,";
   _binding.print(FALSE, os, env);
}

/*----------------------------------------------------------------------------*/

// WildCardPattern::WildCardPattern: Wildcard pattern constructors

WildCardPattern::WildCardPattern(TypeSig typeSig) {
   _typeSig = typeSig;
}

WildCardPattern::WildCardPattern(const WildCardPattern& src, CopyContext* cc,
                                 const TypeInsts* typeInsts,
                                 Subst* substs, Tran& tran)
   : _typeSig(tran.copyTypeSig(src._typeSig, cc, substs)) {
}

// WildCardPattern::tran: Translate a wildcard pattern

Void WildCardPattern::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() > 1)
      Subst::substTypeVars(_typeSig, tran.substs());
}

// WildCardPattern::copy: Copy a wildcard pattern

Expr WildCardPattern::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                           Tran& tran) const {
   Cell& cell_ = *new(sizeof(WildCardPattern), tran.msa()) Cell(WILDCARD_PATTERN);
   new(cell_.body()) WildCardPattern(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// WildCardPattern::print: Print a wildcard pattern

Void WildCardPattern::print(ostream& os, const Env& env,
                            Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "WILDCARD_PATTERN";
   if (_typeSig != TypeSig(UNKNOWN)) {
      outStream << "::";
	   printTypeSig(_typeSig, 0, FALSE, outStream, env);
   }
}

/*----------------------------------------------------------------------------*/

// DataConPattern::DataConPattern: Data constructor pattern constructors

DataConPattern::DataConPattern(Expr dataCon, Expr patterns,
                               Bool isPlainDecon, Bool isPtrDecon,
                               TypeSig typeSig, Tran& tran)
   : _dataCon(dataCon), _patterns(patterns),
     _isPlainDecon(isPlainDecon), _isPtrDecon(isPtrDecon),
     _typedVal(NULL), _moduleDefn(NULL),
     _typeSig(typeSig),
     _reprTypeSig(UNKNOWN),
     _substs(NULL) {
   mapOver(tran.doPat, _patterns);
}

DataConPattern::DataConPattern(const DataConPattern& src, CopyContext* cc,
                               const TypeInsts* typeInsts,
                               Subst* substs, Tran& tran)
   : _dataCon(src._dataCon),
   _patterns(tran.copyList(src._patterns, cc, typeInsts, substs)),
   _isPlainDecon(src._isPlainDecon), _isPtrDecon(src._isPtrDecon),
   _typedVal(src._typedVal),
   _moduleDefn(src._moduleDefn == NULL && cc != NULL
      ? cc->moduleDefn()
      : src._moduleDefn) {
   _typeSig = tran.copyTypeSig(src._typeSig, cc, substs);
      _reprTypeSig = src._reprTypeSig == TypeSig(UNKNOWN)
      ? TypeSig(UNKNOWN)
      : tran.copyTypeSig(src._reprTypeSig, cc, substs);
   Bool typeVarToMap = _moduleDefn != NULL && cc != NULL &&
         _moduleDefn != cc->moduleDefn();
   _substs = cc == NULL
      ? Subst::copySubsts(src._substs, tran.msa())
      : Subst::copySubsts(src._substs, *cc, tran.msa(), typeVarToMap);

    if (typeVarToMap) {
       Subst* subst = _substs;
      while (subst != NULL) {
         TypeVar typeVar = getBirNameMap(_moduleDefn->nameTable(),
            cc->moduleDefn()->typeVarMaps())->invLookUp(subst->typeVar());
         BidirNameMap* typeVarMap = getBirNameMap(_moduleDefn->nameTable(),
            tran._typeVarMaps, tran.msa());

         subst->typeVar() = typeVarMap->use(typeVar, tran.nameTable(),
                                            newTypeVar, tran.msa());
         subst = subst->next();
      }
   }

   Subst::substSubsts(_substs, substs);
}

// DataConPattern::tran: Translate a data constructor pattern

Void DataConPattern::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   tran.lookUpDataCon(_dataCon, bindingEnv, _typedVal, _moduleDefn);

   if (tran.pass() > 1) {
      Subst::substTypeVars(_typeSig, tran.substs());
      Subst::substTypeVars(_reprTypeSig, tran.substs());
      Subst::substSubsts(_substs, tran.substs());
   }
   mapProc1(tran.tranExpr, _patterns, bindingEnv);
}

TypeSig DataConPattern::condTypeSig(TypeSig ts, MSA& msa) {
   TypeSig res = ts;
   if (_isPtrDecon)
      res = ptrTypeSig(ts, msa);
   if (_isPlainDecon)
      res = plainTypeSig(ts, msa);
   return res;
}

// DataConPattern::deconstructP: Deconstruct argument for product type

Expr DataConPattern::deconstructP(Expr arg, TypeSig ts, Tran& tran) {
   return tran.reduce(tran.fnAp(!_isPtrDecon ? DECONSTRUCT : DECONSTRUCT_PTR,
                                arg,
                                ap2(ARROW,
                                    ts,
                                    condTypeSig(_reprTypeSig, tran.msa()), tran.msa())));
}

// DataConPattern::deconstructS: Deconstruct argument for sum type
// Includes tag to match for associated data constructor representation

Expr DataConPattern::deconstructS(Expr arg, Tag tag, TypeSig ts, Tran& tran) {
    return tran.reduce(tran.fnAp(mkPair(!_isPtrDecon ? DECONSTRUCT : DECONSTRUCT_PTR,
                                       fromTag(tag), tran.msa()),
                                 arg,
                                 ap2(ARROW,
                                     ts,
                                     condTypeSig(_reprTypeSig, tran.msa()), tran.msa())));
}

// DataConPattern::copy: Copy a data constructor pattern 

Expr DataConPattern::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                          Tran& tran) const {
   Cell& cell_ = *new(sizeof(DataConPattern), tran.msa()) Cell(DATA_CON_PATTERN);
   new(cell_.body()) DataConPattern(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// DataConPattern::print: Print a data constructor pattern

Void DataConPattern::print(ostream& os, const Env& env,
                           Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   const Env& _env = _typedVal == NULL || _moduleDefn == NULL ? env : _moduleDefn->env();
   if (_isPlainDecon)
      os << "PLAIN(";
   if (_isPtrDecon)
      os << "@";
   os << "DATA_CON_PATTERN";
   if (_isPlainDecon)
      os << ')';
   if (_typeSig != TypeSig(UNKNOWN)) {
      os << "::";
      printTypeSig(_typeSig, 0, FALSE, outStream, env);
      os << '(';
      printTypeSig(_reprTypeSig, 0, FALSE, outStream, env);
      os << ')';
   }

   os << ',';
   printTranExpr(_dataCon,  FALSE, os, _env, pretty, indent);
   os << ',';
   printTranList(_patterns, FALSE, os, env, pretty, indent);
}

/*----------------------------------------------------------------------------*/

// ConcretePattern::ConcretePattern: Concrete pattern constructors

ConcretePattern::ConcretePattern(Expr dataCon, Expr patterns,
                                 Bool isPlain, Bool isPtrDecon,
                                 TypeSig typeSig, Tran& tran)
   : DataConPattern(dataCon, patterns, isPlain, isPtrDecon, typeSig, tran) {
}

ConcretePattern::ConcretePattern(const ConcretePattern& src, CopyContext* cc,
                                 const TypeInsts* typeInsts,
                                 Subst* substs, Tran& tran)
   : DataConPattern(src, cc, typeInsts, substs, tran) {
}


// ConcretePattern::copy: Copy a concrete pattern 

Expr ConcretePattern::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                           Tran& tran) const {
   Cell& cell_ = *new(sizeof(ConcretePattern), tran.msa()) Cell(CONCRETE_PATTERN);
   new(cell_.body()) ConcretePattern(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// ConcretePattern::print: Print a concrete pattern

Void ConcretePattern::print(ostream& os, const Env& env,
   Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   const Env& _env = _typedVal == NULL || _moduleDefn == NULL ? env : _moduleDefn->env();
   if (_isPlainDecon)
      os << "PLAIN(";
   if (_isPtrDecon)
      os << "@";
   os << "CONCRETE_PATTERN";
   if (_isPlainDecon)
      os << ')';
   if (_typeSig != TypeSig(UNKNOWN)) {
      os << "::";
      printTypeSig(_typeSig, 0, FALSE, outStream, env);
      os << '(';
      printTypeSig(_reprTypeSig, 0, FALSE, outStream, env);
      os << ')';
   }

   os << ',';
   printTranExpr(_dataCon, FALSE, os, _env, pretty, indent);
   os << ',';
   printTranList(_patterns, FALSE, os, env, pretty, indent);
}

/*----------------------------------------------------------------------------*/

// Shared case alternative expression

SharedExpr::SharedExpr(Name name, Expr expr)
   : _id(name), _expr(expr), _patterns(Nil) {
}

// Merge unique names from shared pattern

Void SharedExpr::sharedPattern(Expr pattern, MSA& msa) {
   if (dataConPattern(pattern)) {
      mapProc1(sharedPattern, thd3(pattern), msa);
   } else
      if (varPattern(pattern) && formOf(pattern) != WILDCARD_PATTERN) {
         Expr patterns = _patterns;
         Name name = varPatternName(pattern);
         while (patterns != Nil) {
            if (varPatternName(hd(patterns)) == name) {

//------------ T.B.D. assert equal pattern

               return;
            }
            patterns = tl(patterns);
         }
         _patterns = cons(pattern, _patterns, msa);
      }
}

Void SharedExpr::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
	if (formOf(_expr) != LAMBDA) {
      _expr = tran.lambda(_patterns, _expr);
		tran.tranExpr(_expr, bindingEnv);

		if (formOf(_expr) == LAMBDA) {				// T.B.D. consider assertion instead
			Lambda& lambda = toBody(_expr, Lambda);
         if (tran.pass() == 1)
			   lambda.removeUnusedFormalParams();
			lambda._simple = TRUE;
		}

//      _expr = cellBody(cell_, Lambda)._nFormalParams > 0
//         ? expr
//         : cellBody(cell_, Lambda).body();
//   }
//   else
//      _expr = tran.tranExpr(_expr, bindingEnv);
	}

}

Void SharedExpr::print(ostream& os, const Env& env,
                       Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "SHARED_EXPR,";
   printTranExpr(_expr, FALSE, os, env);
}

/*----------------------------------------------------------------------------*/

SharedPatternAp::SharedPatternAp(SharedExpr& sharedExpr)
   : _sharedExpr(sharedExpr) {
}

Void SharedPatternAp::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if (formOf(_sharedExpr._expr) == LAMBDA) {
      Lambda* lambda = &toBody(_sharedExpr._expr, Lambda);
      expr = fromName(_sharedExpr._id);
      for (UInt i = 0; i < lambda->_nFormalParams; i++)
         expr = ap(expr, fromName(lambda->_formalParamV[i].name()), tran.msa());
   }
   else
      expr = fromName(_sharedExpr._id);
   tran.tranExpr(expr, bindingEnv);
}

Void SharedPatternAp::print(ostream& os, const Env& env,
                            Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "SHARED_PATTERN_AP";
}

/*----------------------------------------------------------------------------*/

CoercionInst::CoercionInst(CoercionInst* next,
                           Coercion* coercion)
   : _next(next),
     _coercion(coercion),
     _resolved(FALSE),
     _applyCast(FALSE),
     _typeSig(UNKNOWN) {
}

Coercion::Coercion(Expr castOcc,
                   Bool resolved/* = FALSE*/, Bool castFlag/* = FALSE*/,
                   CoercionInst* instances/* = NULL*/)
   : _castOcc(castOcc), _resolved(resolved), _castFlag(castFlag),
     _instances(instances) {
}


CoercionInst* Coercion::copyInst(CoercionInst* coercionInsts) const {
   CoercionInst* coercionInst = coercionInsts;
   while (coercionInst != NULL) {
      if (coercionInst->coercion() == this)
         return coercionInst;   
      coercionInst = coercionInst->next();
   }
   return NULL;
}


Coerced::Coerced(Expr expr, Expr castOcc)
   : Coercion(castOcc), _expr(expr) {
}

Coerced::Coerced(const Coerced& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                 Tran& tran)
   : Coercion(tran.copyExpr(src._castOcc, cc, typeInsts, substs)),
   _expr(tran.copyExpr(src._expr, cc, typeInsts, substs)) {
}

// Coerced::tran: Translate a coerced term

Void Coerced::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() == 1 || !_resolved) {
      tran.tranExpr(_expr, bindingEnv);
      tran.tranExpr(_castOcc, bindingEnv);
   } else {
      tran.tranExpr(_expr, bindingEnv);
      expr = _expr;
   }
}

// Coerced::copy: Copy a coerced term

Expr Coerced::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                   Tran& tran) const {

// Search for matching instance

   CoercionInst* coercionInst =
      typeInsts != NULL
      ? copyInst(typeInsts->_coercionInsts)
      : NULL;

   if (coercionInst == NULL && _resolved && !_castFlag ||
       coercionInst != NULL && coercionInst->resolved() && !coercionInst->applyCast())
      return tran.copyExpr(_expr, cc, typeInsts, substs);

   if (coercionInst != NULL && coercionInst->resolved() && coercionInst->applyCast()) {
      Cell& cell_ = *new(sizeof(FnAp), tran.msa()) Cell(FN_AP);
      TypeSig typeSig = tran.copyTypeSig(coercionInst->typeSig(), cc, substs);
      Expr castOcc = toBody((Expr)_castOcc, NameOcc).copy(cc, typeInsts, substs, tran);
      new(cell_.body()) FnAp(castOcc, tran.copyExpr(_expr, cc, typeInsts, substs), typeSig, tran.msa());
      return tran.reduce(fromCell(cell_));
   }

   Cell& cell_ = *new(sizeof(Coerced), tran.msa()) Cell(COERCE);
   new(cell_.body()) Coerced(*this, cc, typeInsts, substs, tran);

   if (coercionInst != NULL)
      coercionInst->coercion() = &cellBody(cell_, Coerced);

   return fromCell(cell_);
}

// Coerced::hasNameOcc: Predicate to test for the occurrence of a given name

Bool Coerced::hasNameOcc(Name name, const Tran& tran) const {
   return tran.hasNameOcc(name, _expr);
}

// Coerced::nameOccCount: Count for a given binding

UInt Coerced::nameOccCount(const TypedVal* typedVal, const Tran& tran) const {
   return tran.nameOccCount(_expr, typedVal);
}

// Coerced::alphaConv: Alpha convert

Void Coerced::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   tran.alphaConv(_expr, binding, toName);
}

// Coerced::substitute: Substitute a coerced term

Void Coerced::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
   tran.substituteExpr(_expr, val, binding);
}

Void Coerced::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   lambda.substituteExpr(_expr, argV, nArgs, tran);
}

Void Coerced::print(ostream& os, const Env& env,
                    Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "COERCE";
   if (pretty)
      os << ' ';
   os << '(';
   printTranExpr(_castOcc, FALSE, os, env, pretty, indent);
   os << ')';
   if (pretty)
      newlineAndIndent(indent + 1, os);
   printTranExpr(_expr, FALSE, os, env, pretty, indent + 1);
}

/*----------------------------------------------------------------------------*/

// Variable

Expr Var::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(Var), tran.msa()) Cell(VAR);
   new(cell_.body()) Var(cc == NULL
                            ? _name
                            : mapVarName(_name, *cc, tran.msa()),
                         tran.copyTypeSig(_typeSig, cc, substs), _kind);
   return fromCell(cell_);
}

// Name occurrence

NameOcc::NameOcc(Name name, TypedVal* typedVal/* = NULL*/, TypeSig typeSig /* = UNKNOWN*/)
 : _binding(name, typedVal), _moduleDefn(NULL),
   _mappedName(NULL_NAME),
   _noRebindOnCopy(FALSE),
   _typeSig(typeSig), _substs(NULL),
   _freeVarAssoc(NULL) {
}

NameOcc::NameOcc(const NameOcc& src)
 : _binding(src._binding), _moduleDefn(src._moduleDefn),
   _mappedName(src._mappedName),
   _noRebindOnCopy(src._noRebindOnCopy),
   _typeSig(src._typeSig), _substs(src._substs),
   _freeVarAssoc(NULL) {
}

NameOcc::NameOcc(const NameOcc& src, CopyContext* cc, const TypeInsts* typeInsts, TypedVal* typedVal,
                 const ModuleDefn* moduleDefn,
                 Bool noRebindOnCopy,
                 TypeSig typeSig,
                 Subst* substs, Tran& tran)
  : _binding(cc == NULL
      ? src._binding.name()
      : mapVarName(src._binding.name(), *cc, tran.msa()),
      typedVal),
   _moduleDefn(moduleDefn),
   _mappedName(cc == NULL ? src._mappedName : src._binding.name()),
   _noRebindOnCopy(noRebindOnCopy),
   _typeSig(typeSig), _substs(substs),
   _freeVarAssoc(NULL) {
   if (typeInsts != NULL)
      _typeInsts = *typeInsts;
}

Expr NameOcc::val(Void) const {
   return typedVal() != NULL
		? typedVal()->val()
      : (Expr)ERROR;
}

TypeSig NameOcc::typeSig(Void) const {
   return typedVal() != NULL
		? typedVal()->typeSig()
      : (TypeSig)ERROR;
}

Bool NameOcc::isSchematic(Void) const {
   return typedVal() != NULL
		? typedVal()->_schematic
      : FALSE;
}

Bool NameOcc::boundToGlobal(Void) const {
   Expr val = this->val();
   return (formOf(val) == VAR && toBody(val, Var).kind() == Var::GLOBAL_VAR);
}

Bool NameOcc::tranFinal(Expr& expr, BindingSet& bindingEnv, CopyContext* cc, Tran& tran) {
   if (_typeSig == TypeSig(UNKNOWN))
      _typeSig = tran.copyTypeSig(typedVal()->typeSig(), cc, NULL);

   if (hasTypeVar(_typeSig)) {
      Subst::substTypeVars(_typeSig, tran.substs());
      Subst::substSubsts(_substs, tran.substs());
   }

   UInt32 formOfVal = formOf(val());
   if (formOfVal == DEFN ||
      formOfVal == INSTANCE_METHOD_DEFN ||
      formOfVal == CLASS_METHOD_DEFN) {
      Defn& defn = toBody(val(), Defn);
      if (defn._isInline) {
            expr = !defn._exclusive
            ? tran.copyExpr(defn._expr, cc, &_typeInsts, _substs)
            : defn._expr;
         tran.tranExpr(expr, bindingEnv);
         return TRUE;
      }
   }
   return FALSE;
}

static Bool typeSigContainsNameAfter(TypeSig typeSig,
                                      Name firstName,
                                      Name secondName,
                                      Bool& seenFirstName) {
   if (isAp(typeSig)) {
      if (typeSigContainsNameAfter(fun(typeSig),
                                   firstName,
                                   secondName,
                                   seenFirstName))
         return TRUE;
      return typeSigContainsNameAfter(arg(typeSig),
                                      firstName,
                                      secondName,
                                      seenFirstName);
   }

   if (isTypeVar(typeSig)) {
      Name name = toTypeVar(typeSig);
      if (seenFirstName && name == secondName)
         return TRUE;
      if (name == firstName)
         seenFirstName = TRUE;
   }

   return FALSE;
}

Bool Tran::probableMissingDeclSemicolon(Name unresolvedName) const {
   if (pass() != 1 ||
       _resolverBindingName == NULL_NAME ||
       _resolverBindingTypeSig == TypeSig(UNKNOWN))
      return FALSE;

   Bool seenBindingName = FALSE;
   return typeSigContainsNameAfter(_resolverBindingTypeSig,
                                   _resolverBindingName,
                                   unresolvedName,
                                   seenBindingName);
}

Void Tran::printMissingDeclSemicolonHint(Name unresolvedName) const {
   outStream << "Hint: missing semicolon after \'";
   printName(_resolverBindingName, outStream, nameTable());
   outStream << "\' type declaration?" << endl;
}

Void NameOcc::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if ((typedVal() == NULL ||
      tran.pass() > 1 && formOf(typedVal()->val()) == VAR_PATTERN) &&
      (typedVal() = bindingEnv.lookUp(name(), FALSE)) == NULL &&
      (typedVal() = tran.lookUpExtBinding(name(), FALSE, _moduleDefn, _mappedName)) == NULL &&
      (typedVal() = tran.lookUpBuiltInBinding(name())) == NULL) {
      outStream << "Unresolved name: ";
      printName(name(), outStream, tran.nameTable());
      outStream << '\n';
      if (tran.probableMissingDeclSemicolon(name()))
         tran.printMissingDeclSemicolonHint(name());
      outStream.flush();
      tran._nErrors++;
      expr = (Expr)ERROR;
      return;
   }

   if (tran.pass() > 1) {
      if (_moduleDefn == NULL) {
         if (tranFinal(expr, bindingEnv, NULL, tran))
            return;
      }
      else {
         CopyContext cc(_moduleDefn, tran.nameTable(),
            tran._varNameMaps, tran._typeVarMaps,
            tran.msa());
         if (tranFinal(expr, bindingEnv, &cc, tran))
            return;
      }
   }
}

// NameOcc::copyInst: Search for instance of name occurrence

NameOccInst* NameOcc::copyInst(NameOccInst* nameOccInsts) const {
   NameOccInst* nameOccInst = nameOccInsts;
   while (nameOccInst != NULL) {
      if (nameOccInst->parent() != NULL) {
         if (nameOccInst->parent()->nameOcc() == this)
            return nameOccInst;
      }
      else {
         if (nameOccInst->nameOcc() == this)
            return nameOccInst;
      }
      nameOccInst = nameOccInst->next();
   }
   return NULL;
}

Void printInsts(NameOccInst* nameOccInsts) {
   NameOccInst* nameOccInst = nameOccInsts;
   UInt count = 0;
   while (nameOccInst != NULL) {
      if (count++ > 0)
         outStream << ',';
      outStream << (Void*)nameOccInst << ':' << nameOccInst->nameOcc();
      if (nameOccInst->parent() != NULL) {
         outStream << '(';
         printInsts(nameOccInst->parent());
         outStream << ')';
      }
      nameOccInst = nameOccInst->next();
   }
   outStream << '\n';
}

// NameOcc::copy: Copy a name occurrence

Expr NameOcc::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(NameOcc), tran.msa()) Cell(NAME_OCC);
   TypeSig typeSig = tran.copyTypeSig(_typeSig, cc, substs);

// Search for matching instance

   NameOccInst* nameOccInst =
      typeInsts != NULL
      ? copyInst(typeInsts->_nameOccInsts)
      : NULL;

   if (nameOccInst == NULL) {
      Subst* substs_ = cc == NULL
         ? Subst::copySubsts(_substs, tran.msa())
         : Subst::copySubsts(_substs, *cc, tran.msa());
      Subst* subst = _substs;
      while (subst != NULL) {
         if (cc != NULL &&
            _moduleDefn != NULL) {
            const BidirNameMap* typeVarMap = getBirNameMap(_moduleDefn->nameTable(),
                                             cc->moduleDefn()->typeVarMaps());
            TypeVar origTypeVar = typeVarMap->invLookUp(subst->typeVar());
            BidirNameMap* typeVarMap2 = getBirNameMap(cc->moduleDefn()->nameTable(),
                                                      tran._typeVarMaps, tran.msa());
                   
  
            subst->typeVar() = typeVarMap2->lookUp(origTypeVar);

         }

         subst = subst->next();
      }
      Subst::substSubsts(substs_, substs);

      new(cell_.body()) NameOcc(*this,
                                cc,
                                &_typeInsts,
                                !_noRebindOnCopy ? NULL : (TypedVal*)typedVal(),
                                !_noRebindOnCopy ? NULL : (FALSE && cc != NULL && boundToGlobal() ? cc->moduleDefn() : _moduleDefn), //???
                                _noRebindOnCopy,
                                typeSig, substs_,
                                tran);
   } else {
      Subst::substSubsts(nameOccInst->substs(), substs);

      new(cell_.body()) NameOcc(*this, cc,
                               nameOccInst->typeInsts(),
                               nameOccInst->binding(),
                               nameOccInst->bindingModuleDefn(),
                               nameOccInst->binding() != NULL,
                               typeSig,
                               nameOccInst->substs(), tran);

      nameOccInst->nameOcc() = &cellBody(cell_, NameOcc);
      nameOccInst->nameOccModuleDefn() = NULL;
   }
   return fromCell(cell_);
}

// Predicate to test for the occurrence of a given name

Bool NameOcc::hasNameOcc(Name name, const Tran& tran) const {
   return name == _binding.name();
}

// NameOcc::nameOccCount: Count for a given binding
// 1 on match, 0 otherwise

UInt NameOcc::nameOccCount(const TypedVal* typedVal,
                           const Tran& tran) const {
   return this->typedVal() == typedVal
      ? 1
      : 0;
}

// Alpha convert

Void NameOcc::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   if (_binding.val() == binding)
      _binding.name() = toName;
}

// NameOcc::substitute: Substitue a name occurrence

Void NameOcc::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
   if (typedVal() == binding) {
      expr = val;

      if (formOf(expr) == NAME_OCC) {
         Cell& cell_ = *new(sizeof(NameOcc), tran.msa()) Cell(NAME_OCC);
         new(cell_.body()) NameOcc(toBody(expr, NameOcc));
         expr = fromCell(cell_);
      }
   }
}

Void NameOcc::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   for (UInt i = 0; i < min(lambda._nFormalParams, nArgs); i++) {
      if (typedVal() == &lambda._formalParamV[i].typedVal()) {
         if (formOf(argV[i]) == NAME_TYPED_VAL_BINDING) {

//---------- Rebind if binding supplied as argument

            _binding.val() = &toBody(argV[i], NameTypedValBinding).typedVal();
         }
         else
            expr = argV[i];    
         break;
      }
   }
}

// NameOcc::print: Print contents to output stream

Void NameOcc::print(ostream& os, const Env& env,
                    Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   printTranName(name(), os, env);

//   os << '/' << this;

#ifdef PRINT_TYPE_SIGS
	if (_typeSig != TypeSig(UNKNOWN)) {
		os << "::";
		printTypeSig(_typeSig, 0, FALSE, os, env);
	}
#endif

#ifdef PRINT_NAME_OCC_SUBSTS
   if (pretty)
      Subst::printSubsts(_substs, os, env, TRUE, TRUE);
#endif

}

// Name occurrence instance

NameOccInst::NameOccInst(NameOccInst* next,
                         NameOccInst* parent,
                         NameOcc* nameOcc,
                         const ModuleDefn* nameOccModuleDefn,
                         Subst* substs/* = NULL*/)
   : _next(next),
     _parent(parent),
     _nameOcc(nameOcc),
     _nameOccModuleDefn(nameOccModuleDefn),
     _substs(substs),
     _binding(NULL),
     _bindingModuleDefn(NULL),
     _mappedName(NULL_NAME) {
}

Void NameOccInst::bind_(TypedVal* typedVal, const ModuleDefn* moduleDefn, Name mappedName) {
   _binding = typedVal;
   _bindingModuleDefn = moduleDefn;
   _mappedName = mappedName;
}

/*----------------------------------------------------------------------------*/

// Case

Case::Case(Kind kind, Expr arg, TypeSig argTypeSig,
           UInt n, Expr alts, TypeSig typeSig,
           Bool closed, MSA& msa)
   : _kind(kind), _arg(arg), _argTypeSig(argTypeSig),
     _n(0), _typeSig(typeSig), _closed(closed) {
   _altV = (Expr*)msa.alloc(n * sizeof(Expr));
   for (Expr x = alts; x != Nil; x = tl(x))
      _altV[_n++] = hd(x);
}

Case::Case(const Case& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
           Tran& tran)
   : _kind(src._kind), _argTypeSig(tran.copyTypeSig(src._argTypeSig, cc, substs)),
     _n(src._n), _typeSig(tran.copyTypeSig(src._typeSig, cc, substs)), _closed(src._closed) {
   _arg = tran.copyExpr(src._arg, cc, typeInsts, substs);
   _altV = (Expr*)tran.msa().alloc(_n * sizeof(Expr));
   for (UInt i = 0; i < _n; i++)
      _altV[i] = tran.copyExpr(src._altV[i], cc, typeInsts, substs);
}

Void Case::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   tran.tranExpr(_arg, bindingEnv);
   for (UInt i = 0; i < _n; i++)
      tran.tranExpr(_altV[i], bindingEnv);
}

// Case::copy: Copy a case expression

Expr Case::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(Case), tran.msa()) Cell(CASE);
   new(cell_.body()) Case(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// Case::hasNameOcc: Predicate to test for the occurrence of a given name

Bool Case::hasNameOcc(Name name, const Tran& tran) const {
   if (tran.hasNameOcc(name, _arg))
      return TRUE;
   for (UInt i = 0; i < _n; i++)
      if (tran.hasNameOcc(name, _altV[i]))
         return TRUE;
   return FALSE;
}

// Case::nameOccCount: Count for a given binding

UInt Case::nameOccCount(const TypedVal* typedVal,
                        const Tran& tran) const {
   UInt res = tran.nameOccCount(_arg, typedVal);
   for (UInt i = 0; i < _n; i++)
      res += tran.nameOccCount(_altV[i], typedVal);
   return res;
}

// Case::alphaConv: Alpha convert

Void Case::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   tran.alphaConv(_arg, binding, toName);
   for (UInt i = 0; i < _n; i++)
      tran.alphaConv(_altV[i], binding, toName);
}

Bool Case::innerReduce(Tran& tran) {
   for (UInt i = 0; i < _n; i++) {
      snd(_altV[i]) = tran.reduce(snd(_altV[i]));
      tran.tranReduce(snd(_altV[i]));
   }
   return TRUE;
}

// Case::substitute: Substitute occurrences in a case expression

Void Case::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
   tran.substituteExpr(_arg, val, binding);
   for (UInt i = 0; i < _n; i++)
      tran.substituteExpr(_altV[i], val, binding);
}

Void Case::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   lambda.substituteExpr(_arg, argV, nArgs, tran);
   for (UInt i = 0; i < _n; i++)
      lambda.substituteExpr(_altV[i], argV, nArgs, tran);
}

Void Case::print(ostream& os, const Env& env,
                 Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "CASE-";
   switch (_kind) {
      case CASE_TYPE: os << "TYPE";
         break;

      case CASE_TAG: os << "TAG";
         break;

      case CASE_CONST: os << "CONST";
         break;
   }
   os << ' ';
   printTranExpr(_arg, TRUE, os, env, pretty, indent);
   os << '{';
   printTranV(_n, _altV, os, env, pretty, indent);
   os << '}';
}

/*----------------------------------------------------------------------------*/

FnAp::FnAp(Expr fun, Expr arg, TypeSig typeSig, MSA& msa,
           Bool knownPartial/* = FALSE*/)
   : _fun(fun), _nArgs(1), _typeSig(typeSig),
   _lambda(NULL), _argVars(NULL), _fnLambda(NULL),
   _knownPartial(FALSE), _noCurry(FALSE) {
   while (isPair(_fun) && fst(_fun) == Expr(REDUCE) &&
          formOf(snd(_fun)) == AP) {
      _fun = fun(snd(_fun));
      _nArgs++;
   }

// Allocate space for arguments

   _argV = (Expr*)msa.alloc(_nArgs * sizeof(Expr));

   Expr f = fun;
   Expr a = arg;
   UInt i = _nArgs;
   do {
      _argV[--i] = a;
      if (i == 0)
         break;
      a = arg(snd(f));
      f = fun(snd(f));
   } while (TRUE);
}

// Function application

FnAp::FnAp(Expr fun, Expr* argV, UInt nArgs, TypeSig typeSig, MSA& msa,
           Bool knownPartial/* = FALSE*/)
   : _fun(fun), _nArgs(nArgs), _typeSig(typeSig),
     _lambda(NULL), _argVars(NULL), _fnLambda(NULL),
     _knownPartial(knownPartial), _noCurry(FALSE) {

// Allocate space for arguments

   _argV = (Expr*)msa.alloc(_nArgs * sizeof(Expr));

   memcpy(_argV, argV, _nArgs * sizeof(Expr));
}

// Function application

FnAp::FnAp(const FnAp& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran)
   : _nArgs(src._nArgs),
     _typeSig(tran.copyTypeSig(src._typeSig, cc, substs)),
     _lambda(NULL), _argVars(NULL), _fnLambda(NULL),
     _knownPartial(src._knownPartial), _noCurry(src._noCurry) {

   _fun = tran.copyExpr(src._fun, cc, typeInsts, substs);

// Allocate space and copy arguments

   _argV = (Expr*)tran.msa().alloc(_nArgs * sizeof(Expr));
   for (UInt i = 0; i < _nArgs; i++)
      _argV[i] = tran.copyExpr(src._argV[i], cc, typeInsts, substs);
}

// FnAp::copy: Copy a function application

Expr FnAp::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(FnAp), tran.msa()) Cell(FN_AP);
   new(cell_.body()) FnAp(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// FnAp::hasNameOcc: Predicate to test for the occurrence of a given name

Bool FnAp::hasNameOcc(Name name, const Tran& tran) const {
   if (tran.hasNameOcc(name, _fun))
      return TRUE;
   for (UInt i = 0; i < _nArgs; i++)
      if (tran.hasNameOcc(name, _argV[i]))
         return TRUE;
   return FALSE;
}

// FnAp::nameOccCount: Count for a given binding

UInt FnAp::nameOccCount(const TypedVal* typedVal,
                        const Tran& tran) const {
   UInt res = tran.nameOccCount(_fun, typedVal);
   for (UInt i = 0; i < _nArgs; i++)
      res += tran.nameOccCount(_argV[i], typedVal);
   return res;
}

// FnAp::alphaConv: Alpha convert

Void FnAp::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   tran.alphaConv(_fun, binding, toName);
   for (UInt i = 0; i < _nArgs; i++)
      tran.alphaConv(_argV[i], binding, toName);
}

// FnAp::substitute: Substitute occurrences in a function application

Void FnAp::substitute(Expr& expr, Expr val, TypedVal* binding,  Tran& tran) {
	tran.substituteExpr(_fun, val, binding);
	for (UInt i = 0; i < _nArgs; i++)
      tran.substituteExpr(_argV[i], val, binding);
}

Void FnAp::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda,  Tran& tran) {
	lambda.substituteExpr(_fun, argV, nArgs, tran);
	for (UInt i = 0; i < _nArgs; i++)
	   lambda.substituteExpr(_argV[i], argV, nArgs, tran);
}

// FnAp::tran: Translate function application

Void FnAp::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   _lambda = bindingEnv.lambda();
   tran.tranExpr(_fun, bindingEnv);

   if (tran.pass() > 1) {
      Subst::substTypeVars(_typeSig, tran.substs());
      if (_nArgs == 0) {
         expr = _fun;
         return;
      }
   }

   if (isTriple(curriedFn(_fun)) &&
       fst3(curriedFn(_fun)) == Expr(LAMBDA)) {
      // T.B.D. done, does this occur now?
/*
      Expr args = Nil;
      for (UInt i = _nArgs; i > 0;)
         args = cons(_argV[--i], args, tran.msa());
      expr = tran.tranDefault(tran.lambdaAp(snd3(_fun), thd3(_fun), args),
                              FALSE, (Expr)ERROR, bindingEnv);
*/
      return;
   }

   if (tran.pass() > 1 && !checkNArgs(tran)) {
      expr = (Expr)ERROR;
      return;
   }
	for (UInt i = 0; i < _nArgs; i++) {
         tran.tranExpr(_argV[i], bindingEnv);
         if (_argV[i] == (Expr)ERROR) {
            expr = (Expr)ERROR;
            return;
         }
	}

// (REDUCE, f a1 ... am) a(m+1) ... an => f a1 ... an

   if (!_noCurry)
      while (isPair(_fun) && fst(_fun) == Expr(REDUCE) &&
             formOf(snd(_fun)) == FN_AP) {
         FnAp& fnAp = toBody(snd(_fun), FnAp);
         Expr* argV = (Expr*)tran.msa().alloc((fnAp._nArgs + _nArgs) * sizeof(Expr));
         memcpy(argV, fnAp._argV, fnAp._nArgs * sizeof(Expr));
         memcpy(argV + fnAp._nArgs, _argV, _nArgs * sizeof(Expr));
         _fun = fnAp._fun;
         _nArgs += fnAp._nArgs;
         _argV = argV;
         _typeSig = fnAp._typeSig;
      }
}

// Default to no reduction during transformation

Bool FnAp::reduce(Expr& expr, Tran& tran) {
   if (isPair(_fun) && fst(_fun) == Expr(REDUCE) &&
      formOf(snd(_fun)) != LAMBDA)
      tran.tranReduce(_fun);

   if (isPair(_fun) && fst(_fun) == Expr(REDUCE) &&
       formOf(snd(_fun)) == LAMBDA)
      return reduceLambdaAp(expr, 0, tran);
   else if (_fun == Expr(SEQ )&& formOf(_argV[1]) == FN_AP) {
      FnAp& seqFnAp = toBody(_argV[1], FnAp);
      if (isPair(seqFnAp._fun) &&
         fst(seqFnAp._fun) == Expr(REDUCE) &&
         formOf(snd(seqFnAp._fun)) == LAMBDA) {
         _typeSig = ap(fun(_typeSig), ap2(ARROW, arg(arg(fun(arg(_typeSig)))),
            arg(arg(fun(arg(_typeSig)))), tran.msa()), tran.msa());
         return seqFnAp.reduceLambdaAp(_argV[1], 0, tran);
      }
   }
 
   return FALSE;
}

static Void normaliseFormalParamVars(Lambda& lambda) {
   for (UInt i = 0; i < lambda._nFormalParams; i++) {
      Expr val = lambda._formalParamV[i].typedVal().val();
      assert(formOf(val) == VAR, "normaliseFormalParamVars: expected VAR");
      Var& var = toBody(val, Var);
      var._kind = i == 0
         ? Var::FORMAL_PARAM_VAR
         : Var::EXTRA_FORMAL_PARAM_VAR;
      var._parentLambda = &lambda;
      var._index = i;
   }
}

// Reduce a lambda application (pass known > 1)

Bool FnAp::reduceLambdaAp(Expr& expr, UInt d, Tran& tran) {
   if (_nArgs > 1) {
      _nArgs--;
      Expr arg = _argV[_nArgs];
      _typeSig = arg(_typeSig);
      reduceLambdaAp(_fun, d + 1, tran);
      _argV[0] = arg;
   }

   if (!(isPair(_fun) &&
         fst(_fun) == (Expr)REDUCE && formOf(snd(_fun)) == LAMBDA)) {
      return FALSE;
   }
        
   Lambda& lambda = toBody(snd(_fun), Lambda);
   TypedVal* typedVal = &lambda._formalParamV[0].typedVal();
   Expr arg = _argV[0];

   if (formOf(arg) == NAME_OCC || isConst(arg) || lambda.isReduction()) {
      Subst::substTypeVars(lambda._typeSig, tran.substs());
      UInt arr = ::arity(lambda._typeSig);
      assert(arr >= 1, "FnAp::reduceLambdaAp: invalid application");
      lambda.substitute(&arg, 1, tran);

      if (lambda._nFormalParams > 1) {
         lambda._nFormalParams--;
//            for (UInt i = 0; i < lambda._nFormalParams; i++)
//               lambda._formalParamV[i] = lambda._formalParamV[i + 1];
         lambda._formalParamV++;
         lambda._typeSig = arg(lambda._typeSig);
         normaliseFormalParamVars(lambda);

         if (d == 0)
            expr = _fun;
      } else {
         if (d == 0)
            expr = lambda._body;
         else
            _fun = lambda._body;
      }
   }
   else {
      NameTypedValBinding& ntvb = lambda._formalParamV[0];
      Expr defn = tran.defn(mkTriple(CONSTRAIN,
                            fromName(ntvb.name()),
                            ntvb.typedVal().typeSig(), tran.msa()), arg);

//--- Rebind all name occurences

      lambda.substitute(&defn, 1, tran);

//--- Update name in case of alpha conversion

      toBody(defn, NameTypedValBinding).name() = ntvb.name();
      Expr body = tran.reduce(tran.let(cons(defn, Nil, tran.msa()), lambda._body));
      Let& let = toBody(snd(body), Let);
 
      if (lambda._nFormalParams > 1) {
         tran.tranReduce(body);
         lambda._nFormalParams--;
         lambda._formalParamV++;
         lambda._typeSig = arg(lambda._typeSig);
         lambda._body = body;
         normaliseFormalParamVars(lambda);

         if (d == 0)
            expr = _fun;
      }
      else {
         if (d == 0)  
            expr = body;
         else
            _fun = body;
      }
   }
   return TRUE;
}

// The transformation avoids building a lambda abstraction
// and then immediately reducing it

// SEQ e1 e2 => SEQ e1 (REDUCE, e2)
// COND e1 e2 e3 = COND e1 (REDUCE, e2) (REDUCE, e3)

Bool FnAp::innerReduce( Tran& tran) {
   if (_fun == Expr(SEQ)) {
      arg(fun(arg(_typeSig))) = arg(arg(fun(arg(_typeSig))));
      arg(arg(_typeSig)) = arg(arg(arg(_typeSig)));
      _argV[1] = tran.reduce(_argV[1]);
      tran.tranReduce(_argV[1]);
      return TRUE;
    } else if (_fun == Expr(COND)) {
      for (UInt i = 1; i < 3; i++) {
         _argV[i] = tran.reduce(_argV[i]);
         tran.tranReduce(_argV[i]);
      }
      arg(fun(arg(_typeSig))) = arg(arg(fun(arg(_typeSig))));
      arg(fun(arg(arg(_typeSig)))) = arg(arg(fun(arg(arg(_typeSig)))));
      arg(arg(arg(_typeSig))) = arg(arg(arg(arg(_typeSig))));
      return TRUE;
   }
   return FALSE;
}

//
// An argument of type * is assumed to be associated with a class method
// and the following transformation is applied.
// 
// f a1 ... ai::* ... an
// =>
// let !cm = f a1 ... a(i-1)           -- f if i = 1
// in
//    (DISPATCH cm ai) a(i+1) ... an   -- empty arg list if i = n
//
// Semantically, DISPATCH cm arg evaluates arg to a return state

Expr FnAp::tranDispatch(UInt i, BindingSet& bindingEnv, Tran& tran)
{
   Expr t = fromName(newName());
   Expr u;
   if (i > 0)
   {
      Cell& cell_ = *new(sizeof(FnAp), tran.msa()) Cell(FN_AP);
      new(cell_.body()) FnAp(_fun, _argV, i, _typeSig, tran.msa());
      u = fromCell(cell_);
   }
   else
      u = _fun;

   TypeSig typeSig = _typeSig;
   UInt j = i;
   while (j-- > 0)
      typeSig = arg(typeSig);

   Expr v;
   {
      Cell& cell_ = *new(sizeof(FnAp), tran.msa()) Cell(FN_AP);
      tran.tranExpr(_argV[i], bindingEnv);
      new(cell_.body()) FnAp(mkTriple(DISPATCH,
                                     t,
                                     _argV[i], tran.msa()),
                                     _argV + i + 1, _nArgs - (i + 1),
												 arg(typeSig),
												 tran.msa());
      v = fromCell(cell_);
   }

   Cell& cell_ = *new(sizeof(Let), tran.msa()) Cell(LET);
   new(cell_.body()) Let(tran.nameTypedValBinding(toName(t), u, typeSig), v, FALSE);
   Expr expr = fromCell(cell_);
   tran.tranExpr(expr, bindingEnv);
   return expr;
}


Void FnAp::print(ostream& os, const Env& env,
                 Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "$::";
   printTypeSig(_typeSig, 0, FALSE, outStream, env);
   if (pretty && _fun == Expr(SEQ) && _nArgs == 2) {
      newlineAndIndent(indent, os);
      printTranExpr(_argV[0], TRUE, os, env, pretty, indent + 1);
      os << ';';
      newlineAndIndent(indent, os);
      printTranExpr(_argV[1], TRUE, os, env, pretty, indent + 1);
   } else {
      if (pretty) {
         newlineAndIndent(indent, os);
         os << "fun: ";
      } else
         os << ','; 
      printTranExpr(_fun, TRUE, os, env, pretty, indent);
      for (UInt i = 0; i < _nArgs; i++) {
         Expr arg_ = _argV[i];
         if (pretty) {
            UInt32 forms[] = { FAT_BAR_SEQ, COERCE };
            newlineAndIndent(indent, os);
            os << "arg " << i + 1 << ": ";
            if (isReduce(arg_) &&
                formOf(snd(arg_)) == FN_AP || formExistsIn(arg_, 2, forms))
               newlineAndIndent(indent + 1, os);
         } else
            os << (i == 0 ? ' ' : ',');
         printTranExpr(arg_, TRUE, os, env, pretty, indent + 1);
      }
   }
}

Void FnAp::removeArg(UInt index) {
   memmove(&_argV[index],
           &_argV[index + 1], (_nArgs - index - 1) * sizeof(Expr));
   _nArgs--;
}

Bool FnAp::checkNArgs(Tran& tran){
   if ((_fun == Expr(FAT_BAR) || _fun == Expr(SEQ)) && _nArgs == 2)
      return TRUE;
   if (_fun == Expr(COND) && _nArgs == 3)
      return TRUE;

   if (_nArgs > arity(_typeSig)) {
      outStream << "f: ";

      printTranExpr(_fun, FALSE, outStream, tran.env());
      outStream << '\n';
      outStream.flush();

      for (UInt i = 0; i < _nArgs; i++) {
         outStream << i << ": ";
         printTranExpr(_argV[i], FALSE, outStream, tran.env());
         outStream << '\n';
         outStream.flush();
      }

      //print(outStream, tran.env());
      outStream << "\nToo many arguments (" << _nArgs << ") for: ";
      printTypeSig(_typeSig, 0, FALSE, outStream, tran.env());
      outStream << '\n';
      outStream.flush();
      tran._nErrors++;
      return FALSE;
   }
   else
      return TRUE;
}

/*----------------------------------------------------------------------------*/

// DataConDecl::DataConDecl: Constructor

DataConDecl::DataConDecl(Name name, TypeSig typeSig, Tran& tran)
   : _name(name), _dataConBinding(NULL) {
   ConstString nameStr = nameString_(name, tran.nameTable());
   String reprNameStr = (String)tran.msa().alloc(strlen(nameStr) + 5 + 1);
   sprintf(reprNameStr, "%s$repr", nameStr);
   _reprDecl = mkPair(DECL, mkTriple(CONSTRAIN,
                                     fromName(tran.useName(reprNameStr)),
                                     typeSig, tran.msa()), tran.msa());
}

// DataConDecl::tran: Basic translation

Void DataConDecl::tran(Tran& tran) {
   tran.tranDeclOrDefn(_reprDecl, TRUE, FALSE, FALSE);
}

// DataConDecl::addBinding: Add binding for representation declaration

Void DataConDecl::addBinding(BindingSet& bindingEnv, Tran& tran) {
   if (_dataConBinding == NULL) {

//--- Type definition must precede representation definition

      _dataConBinding = bindingEnv.lookUp(_name, FALSE);

      if (_dataConBinding == NULL) {
         outStream << "Undeclared data constructor: ";
         printName(_name, outStream, tran.nameTable());
         outStream << '\n';
         outStream.flush();
         tran._nErrors++;
         return;
      }
   }

   tran.addBinding(_reprDecl, bindingEnv);
}

// DataConDecl::tran: Translate

Void DataConDecl::tran(BindingSet& bindingEnv, Tran& tran) {
   tran.tranDeclOrDefn(_reprDecl, bindingEnv);
}

// DataConDecl::print: Print

Void DataConDecl::print(ostream& os, const Env& env,
                        Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
	os << "DATA_CON_DECL";
   os << (pretty ? '\n' : ',');
   printTranExpr(_reprDecl, TRUE, os, env, pretty, 0);
}

/*----------------------------------------------------------------------------*/

// DataConDefn::DataConDefn: Constructor

DataConDefn::DataConDefn(Expr reprDefn, Expr selectors,
                         Bool isImplicit, Bool isInline)
   : _reprDefn(reprDefn), _defn(VOID), _selectors(selectors),
     _name(NULL_NAME), _dataConBinding(NULL), _reprName(NULL_NAME),
     _isImplicit(isImplicit), _isInline(isInline) {
}

// DataConDefn::tran: Basic translation

Void DataConDefn::tran(Tran& tran) {
  _name = toName(fst(_reprDefn));
   ConstString nameStr = nameString_(_name, tran.nameTable());
   String reprNameStr = (String)tran.msa().alloc(strlen(nameStr) + 5 + 1);
   sprintf(reprNameStr, "%s$repr", nameStr);
   _reprName = tran.useName(reprNameStr);
   _reprDefn = tran.defn(fromName(_reprName), snd(_reprDefn), 
                         TRUE, _isImplicit, TRUE, TRUE); // Exclusive  
   UInt index = 0;
   mapProc3(tranSelector, _selectors, _name, index, tran);
}

// DataConDefn::tranSelector: Translate selector to new binding

Void DataConDefn::tranSelector(Expr& selector, Name name, UInt& index, Tran& tran) {
   Bool isInline = FALSE;
   if (isPair(selector) && fst(selector) == Expr(INLINE)) {
      selector = snd(selector);
      isInline = TRUE;
   }
   ConstString selNameStr = DataCon::selectorName(name, ++index, tran.nameTable(), tran.msa());

   selector = tran.defn(fromName(tran.useName(selNameStr)), selector, TRUE, FALSE, isInline);
}

Expr DataConDefn::vars(UInt n, MSA& msa) {

// Note - not using conditional to avoid argument evaluation order side effect

   if (n == 0)
      return Nil;
   else {
      Name name = newName();
      return cons(fromName(name), vars(n - 1, msa), msa);
   }
}

Expr DataConDefn::fnAp(Expr vars, Expr res, MSA & msa) {
   if (vars == Nil)
      return res;
   else
      return fnAp(tl(vars), mkPair(REDUCE, ap(res, hd(vars), msa), msa), msa);
}

// Note that Nilary data constructors may have oimplicit definition

Void DataConDefn::addBindings(Expr dataConDefn, BindingSet& bindingEnv, Tran& tran) {
   if (_dataConBinding == NULL) {

//--- Type definition must precede representation definition

      _dataConBinding = bindingEnv.lookUp(_name, FALSE);
      if (_dataConBinding != NULL && formOf(_dataConBinding->val()) == DEFN &&
         toBody(_dataConBinding->val(), Defn)._redefinable) {
         Defn& defn = toBody(_dataConBinding->val(), Defn);
         defn.setOverridden();
         _dataConBinding = defn._prevDeclOrDefn;
      }

      if (_dataConBinding == NULL) {
         outStream << "Undeclared data constructor: ";
         printName(_name, outStream, tran.nameTable());
         outStream << '\n';
         outStream.flush();
         tran._nErrors++;
         return;
      } else {
         DataCon& dataCon = toBody(_dataConBinding->val(), DataCon);
         Expr constructFn = !dataCon.sumType()
            ? (Expr)CONSTRUCT
            : (Expr)mkPair(CONSTRUCT, fromTag(dataCon.tag()), tran.msa());

         if (dataCon.arity() > 0) {
            Expr patterns = vars(dataCon.arity(), tran.msa());
            _defn = tran.defn(fromName(_name),
                              tran.lambda(patterns,
                                          tran.reduce(tran.fnAp(constructFn,
                                                                fnAp(patterns,
                                                                     fromName(_reprName), tran.msa())))),
                              TRUE, FALSE, _isInline, FALSE);
        } else
            _defn = tran.defn(fromName(_name), 
                              tran.fnAp(constructFn,
                                         fromName(_reprName), UNKNOWN, FALSE),
                              TRUE, _isImplicit, _isInline, FALSE);
      }
   }

   if (tran.pass() > 1) {
      Expr& reprExpr =
         toBody(toBody(_reprDefn, NameTypedValBinding).typedVal().val(), Defn).expr();
      if (isPair(reprExpr) && fst(reprExpr) == Expr(VARIABLE)) {
         reprExpr = snd(reprExpr);
         Expr& defnExpr =
            toBody(toBody(_defn, NameTypedValBinding).typedVal().val(), Defn).expr();
         defnExpr = mkPair(VARIABLE, defnExpr, tran.msa());
      }
   }

   tran.addBinding(_reprDefn, bindingEnv);
   tran.addBinding(_defn, bindingEnv);
}

// DataConDefn::tran: Translate

Void DataConDefn::tran(BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() == 1)
      tran.tranDeclOrDefn(_reprDefn, bindingEnv);

   if (tran.pass() > 1) {
      DataCon& dataCon = toBody(_dataConBinding->val(), DataCon);
      TypeSig typeSig = dataCon.reprTypeSig();
      if (hasTypeVar(typeSig)) {
         Subst::substTypeVars(typeSig, tran.substs());
         typeSig = mutableTypeSig(typeSig, tran.msa());  // May not be unique

 //----- Inverse substitute to match type parameters

         Subst* substs = this->_dataConBinding->substs();
         Subst::substSubsts(substs,  tran.substs());
         substs = Subst::inverseSubsts(substs, tran.msa());

         Subst::substTypeVars(typeSig, substs);
         dataCon.reprTypeSig() = typeSig;
      }
   }

	tran.tranDeclOrDefn(_defn, bindingEnv);
   tran.addBindings(_selectors, bindingEnv);
   tran.tranDeclOrDefns(_selectors, bindingEnv);
}

// DataConDefn::print: Print

Void DataConDefn::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/,
                        UInt indent/* = 0*/) const {
	printTranExpr(_reprDefn, FALSE, os, env, pretty, 0);
   os << (pretty ? '\n' : ',');
	printTranExpr(_defn, FALSE, os, env, pretty, 0);
   if (_selectors != Nil) {
      os << (pretty ? '\n' : ',');
      printTranList(_selectors, FALSE, os, env, pretty, 0, TRUE);
   }
}

/*----------------------------------------------------------------------------*/

// ConcreteDataCon::ConcreteDataCon: Constructor

ConcreteDataCon::ConcreteDataCon(Expr dataCon)
   : _dataCon(dataCon), _typedVal(NULL), _moduleDefn(NULL), _substs(NULL) {
}

Void ConcreteDataCon::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() == 1)
      tran.lookUpDataCon(_dataCon, bindingEnv, _typedVal, _moduleDefn);
   else {
      DataCon& dataCon = toBody(_dataCon, DataCon);
      expr = !dataCon.sumType()
         ? (Expr)CONSTRUCT
         : (Expr)mkPair(CONSTRUCT, fromTag(dataCon.tag()), tran.msa());
   }
}

Void ConcreteDataCon::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/,
   UInt indent/* = 0*/) const {
   os << '#';
   printTranExpr(_dataCon, FALSE, os,
                 _moduleDefn != NULL ? _moduleDefn->env() : env,
                 pretty, 0);
}

/*----------------------------------------------------------------------------*/

// Decl::Decl: Constructor

Decl::Decl(Bool outermost)
   : _outermost(outermost),
     _prevDeclOrDefn(NULL), _moduleDefn(NULL), _unifyWithPrevDeclOrDefn(TRUE),
     _var(NULL) {
}

// Decl::tran: Translate

Void Decl::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
}

// Decl:copy Copy a declaration

Expr Decl::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(Decl), tran.msa()) Cell(DECL);
   new(cell_.body()) Decl(_outermost);
   return fromCell(cell_);
}

// Decl::print: Print

Void Decl::print(ostream& os, const Env& env,
                 Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
}

/*----------------------------------------------------------------------------*/

// Defn::Defn: Constructor

Defn::Defn(Expr expr, Bool outermost, Bool redefinable,
   Bool isInline, Bool exclusive)
 : _expr(expr), _boundVal((Expr)ERROR),
   _outermost(outermost), _outer(FALSE),
   _isInline(isInline), _exclusive(exclusive),
   _prevDeclOrDefn(NULL), _moduleDefn(NULL), _unifyWithPrevDeclOrDefn(TRUE), 
   _redefinable(redefinable), _overridden(FALSE),
   _translated(FALSE),
   _minConstraintSeqNo(0), _maxConstraintSeqNo(0),
   _parentLambda(NULL), _recInitStrategy(REC_INIT_DEFAULT),
   _recInitState(REC_STATE_READY) {
}

// Defn::tran: Translate

Void Defn::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if (!_overridden) {
      if (tran.pass() > 1)
         _translated = TRUE;
      tran.tranExpr(_expr, bindingEnv);
   }
}

// Defn::copy: Copy a definition

Expr Defn::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(Defn), tran.msa()) Cell(DEFN);
   Defn& defn = *new(cell_.body()) Defn(tran.copyExpr(_expr, cc, typeInsts, substs),
							                  _outermost,
                                      _redefinable,
	                                   _isInline,
                                      _exclusive);
   defn._recInitStrategy = _recInitStrategy;
   return fromCell(cell_);
}

// Defn::hasNameOcc: Predicate to test for the occurrence of a given name

Bool Defn::hasNameOcc(Name name, const Tran& tran) const {
   return tran.hasNameOcc(name, _expr);
}

// Defn::nameOccCount: Count for a given binding

UInt Defn::nameOccCount(const TypedVal* typedVal,
                        const Tran& tran) const {
   return tran.nameOccCount(_expr, typedVal);
}

// Defn::alphaConv: Alpha convert

Void Defn::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   tran.alphaConv(_expr, binding, toName);
}

// Defn::substitute: Substitute occurrences in a definition

Void Defn::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
	tran.substituteExpr(_expr, val, binding);
}

Void Defn::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   lambda.substituteExpr(_expr, argV, nArgs, tran);
}

// Defn::print: Print

Void Defn::print(ostream& os, const Env& env,
                 Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << '=';
	printTranExpr(_expr, FALSE, os, env, pretty, indent);
 }

/*----------------------------------------------------------------------------*/

Lambda::Lambda(UInt nFormalParams, NameTypedValBinding* formalParamV,
               Expr body, TypeSig typeSig/* = UNKNOWN*/,
               Bool updatable/* = TRUE*/)
   : _parent(NULL),
     _nFormalParams(nFormalParams), _formalParamV(formalParamV), _body(body),
	  _isInline(FALSE), _typeSig(typeSig),
     _freeVarAssocs(NULL),
     _nNonGlobalFree(0),
     _entryLabel(NULL), _altEntryLabel(NULL),
     _closureCell(NULL), _closure(NULL),
     _refCount(0),
     _needsClosure(FALSE), _noPartialAps(TRUE),
     _updatable(nFormalParams == 0 && updatable),
     _simple(FALSE), _isOrder(FALSE),
     _segment(NULL) {
   for (UInt i = 0; i < nFormalParams; i++)
      toBody(_formalParamV[i].typedVal().val(), Var).setParentLambda(this);
}

Lambda::Lambda(const Lambda& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
   Tran& tran)
 : _parent(NULL),
   _nFormalParams(src._nFormalParams),
   _isInline(FALSE),   // By definition, if copied           
   _typeSig(tran.copyTypeSig(src._typeSig, cc, substs)),
   _freeVarAssocs(NULL),
   _nNonGlobalFree(0),
   _entryLabel(NULL), _altEntryLabel(NULL),
   _closureCell(NULL), _closure(NULL),
   _refCount(0),
   _needsClosure(FALSE), _noPartialAps(TRUE),
   _updatable(src._updatable),
   _simple(FALSE), _isOrder(FALSE),
   _segment(NULL) {
   _body = tran.copyExpr(src._body, cc, typeInsts, substs);
   _formalParamV = (NameTypedValBinding*)tran.msa().alloc(
      sizeof(NameTypedValBinding) * _nFormalParams);
   for (UInt i = 0; i < _nFormalParams; i++) {
      _formalParamV[i].name() = cc == NULL
         ? src._formalParamV[i].name()
         : mapVarName(src._formalParamV[i].name(), *cc, tran.msa());
      _formalParamV[i]._val = 
         TypedVal(src._formalParamV[i].typedVal(), cc, typeInsts, substs, tran);
      toBody(_formalParamV[i].typedVal().val(), Var)._parentLambda = this;
   }
}


Void Lambda::adjRefCount(Int diff) {
   _refCount += diff;
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

Bool Lambda::isReduction(Void) const {
   if (!(isReduce(_body) && _nFormalParams == 1))
      return FALSE;
   Expr expr = _body;

   do {
      expr = snd(expr);
   } while (isReduce(expr));

   return formOf(expr) == NAME_OCC &&
          toBody(expr, NameOcc).typedVal() == &_formalParamV[0]._val;
}

#undef localConst
#define localConst

// Lambda::tran: Translate a lambda function

Void Lambda::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   _parent = bindingEnv._lambda;

   BindingSet localBindingEnv(&bindingEnv, this);

   for (UInt i = 0; i < _nFormalParams; i++) {
      _formalParamV[i].tran(localBindingEnv, tran);
      if (tran.pass() > 1)
          toBody(_formalParamV[i].typedVal().val(), Var)._typeSig =
            _formalParamV[i].typedVal().typeSig();
   }
   if (tran.pass() > 1)
      Subst::substTypeVars(_typeSig, tran.substs());

   tran.tranExpr(_body, localBindingEnv);
}

// Lambda::copy: Copy a lambda cell

Expr Lambda::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(Lambda), tran.msa()) Cell(LAMBDA);
   new(cell_.body()) Lambda(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// Lambda::hasNameOcc: Predicate to test for the occurrence of a given name
// Note: No comparison with formal parameters
//       (if no occurrence of name in body, alpha conversion not needed)

Bool Lambda::hasNameOcc(Name name, const Tran& tran) const {
   return tran.hasNameOcc(name, _body);
}

// Lambda::nameOccCount: Count for a given binding

UInt Lambda::nameOccCount(const TypedVal* typedVal,
                          const Tran& tran) const {
   return tran.nameOccCount(_body, typedVal);
}

// Lambda::alphaConv: Alpha convert

Void Lambda::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
    tran.alphaConv(_body, binding, toName);
}

// Lambda::substitute: Substitute occurrences in a lambda function

Void Lambda::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
   tran.substituteExpr(_body, val, binding);
}

Void Lambda::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   lambda.substituteExpr(_body, argV, nArgs, tran);
}

Void Lambda::substituteExpr(Expr& expr, Expr* argV, UInt nArgs, Tran& tran) {
  	if (isPair(expr))
		substitutePair(expr, argV, nArgs, tran);
	else
      switch (formOf(expr)) {
			case NAME_TYPED_VAL_BINDING:
			   substituteExpr(toBody(expr, NameTypedValBinding).typedVal().val(), argV, nArgs, tran);
            break;

	      case DEFN:
         case COERCE:
	      case LAMBDA:
         case LET:
         case NAME_OCC:
         case FN_AP:
         case CASE:
         case APP_LAMBDA:
         case FAT_BAR_SEQ:
            assert(isPtr(expr), "Lambda::substituteExpr: Unexpected form");
            toBody(expr, AST_Node).substitute(expr, argV, nArgs, *this, tran);
            break;

			default:
/*
            printTranExpr(expr, FALSE, outStream, tran.env(), FALSE);
            outStream << '\n';
            outStream.flush();
*/
				break;
		}
/*
   outStream << "Substituted Expression:\n";
   printTranExpr(expr, FALSE, outStream, tran.env());
   outStream << '\n';
*/
}

Void Lambda::substitutePair(Expr& pair, Expr* argV, UInt nArgs, Tran& tran) {
   if (fst(pair) == Expr(REDUCE)) {
      substituteExpr(snd(pair), argV, nArgs, tran);
      tran.tranReduce(pair);
   } else {
      substituteExpr(fst(pair), argV, nArgs, tran);
      substituteExpr(snd(pair), argV, nArgs, tran);
   }
}

Void Lambda::substituteList(Expr list, Expr* argV, UInt nArgs, Tran& tran) {
   mapProc3(substituteExpr, list, argV, nArgs, tran);
}

//  Lambda::substitute: Substitute parameters with arguments

Void Lambda::substitute(Expr* argV, UInt nArgs, Tran& tran) {

#ifdef TRACE
	static UInt probe = 0;
   if (++probe == 0) {
		outStream << "***** Lambda::substitute: probe hit\n";
      traceFlag = TRUE;
   }

   if (traceFlag) {
      outStream << "Substituting (" << probe << "):\n";
      for (UInt i = 0; i < nArgs; i++) {
         outStream << "Arg " << i << ": ";
         printTranExpr(argV[i], FALSE, outStream, tran.env(), FALSE);
         outStream << '\n';
      }

      outStream << "Lambda ";
      print(outStream, tran.env(), TRUE);
      outStream << endl;
   }
#endif

   for (UInt i = 0; i < _nFormalParams; i++)
      for (UInt j = 0; j < nArgs; j++)
         if (tran.hasNameOcc(_formalParamV[i].name(), argV[j])) {

//--------- Free variable captured by formal parameter, apply alpha conversion

            Name fromName = _formalParamV[i].name();
            Name toName = newName();

#ifdef TRACE
            if (traceFlag) {
               outStream << "Lambda::substitute: Formal parameter ";
               printName(fromName, outStream, tran.nameTable());
               outStream << " capture of free variable substitued with ";
               printName(toName, outStream, tran.nameTable());
               outStream << '\n';
               outStream.flush();
            }
#endif

            tran.alphaConv(_body, &_formalParamV[i].val(), toName);

//--------- Substitute formal parameter and variable name
//--------- T.B.D. Remove duplication by creating variable during code generation

            _formalParamV[i].name() = toName;
            toBody(_formalParamV[i].typedVal().val(), Var).name() = toName;
         }
   substituteExpr(_body, argV, nArgs, tran);
}


// Lambda::removeUnusedArgs: Remove unused arguments from a lambda function
// Designed for shared case alternatives.

// Note that this is performed before the tree of name occurrences has been built
// so a simple count of bound occurrences is used.

Void Lambda::removeUnusedFormalParams(Void) {
   for (UInt i = 0; i < _nFormalParams; i++)
      if (_formalParamV[i]._count == 0) {
         memcpy(&_formalParamV[i], &_formalParamV[i + 1], sizeof(NameTypedValBinding*) * (_nFormalParams - i - 1));
         _nFormalParams--;
      }
}

Expr Lambda::instantiatePair(Expr pair, Expr* argV, UInt nArgs, const Subst* substs, Tran& tran) const {
	return mkPair(instantiateExpr(fst(pair), argV, nArgs, substs, tran),
                 instantiateExpr(snd(pair), argV, nArgs, substs, tran), tran.msa());
}

Expr Lambda::instantiateList(Expr list, Expr* argV, UInt nArgs, const Subst* substs, Tran& tran) const {
	if (list == Nil)
		return Nil;
	else
	   return cons(instantiateExpr(hd(list), argV, nArgs, substs, tran),
	               instantiateList(tl(list), argV, nArgs, substs, tran),
						tran.msa());
}

Expr Lambda::instantiateExpr(Expr expr, Expr* argV, UInt nArgs, const Subst* substs, Tran& tran) const {

#ifdef TRACE
   if (traceFlag) {
      outStream << '\n';
      outStream << "Lambda::instantiateExpr: ";
      printTranExpr(expr, FALSE, outStream, tran.env());
      outStream << '\n';
      outStream.flush();
   }
#endif

	if (isPair(expr))
		return instantiatePair(expr, argV, nArgs, substs, tran);
	else
		switch (formOf(expr)) {
			case NAME_TYPED_VAL_BINDING: {
				NameTypedValBinding& binding = toBody(expr, NameTypedValBinding);
				Cell& cell_ = *new(sizeof(NameTypedValBinding), tran.msa()) Cell(NAME_TYPED_VAL_BINDING);
				new(cell_.body()) NameTypedValBinding(binding.name(),
																 instantiateExpr(binding.typedVal().val(), argV, nArgs, substs, tran),
                                                 binding.typedVal().typeSig(),
                                                 tran.msa());
            Subst::substTypeVars(cellBody(cell_, NameTypedValBinding).typedVal().typeSig(), substs);
				return fromCell(cell_);
			}
			case DEFN: {
				Defn& defn = toBody(expr, Defn);
				Cell& cell_ = *new(sizeof(Defn), tran.msa()) Cell(DEFN);
				new(cell_.body()) Defn(instantiateExpr(defn._expr, argV, nArgs, substs, tran),
											 defn._outermost,
                                  defn._redefinable,
					                   defn._isInline,
                                  defn._exclusive);
				return fromCell(cell_);
			}

			case NAME_OCC: {
				NameOcc& nameOcc = toBody(expr, NameOcc);
				if (formOf(nameOcc.val()) == VAR) {
					Var& var = toBody(nameOcc.val(), Var);
					for (UInt i = 0; i < nArgs; i++)
						if (&toBody(_formalParamV[i].typedVal().val(), Var) == &var)
							return argV[i];
				}
				Cell& cell_ = *new(sizeof(NameOcc), tran.msa()) Cell(NAME_OCC);
            new(cell_.body()) NameOcc(nameOcc.name());
				NameOcc& newNameOcc = cellBody(cell_, NameOcc);
				newNameOcc._typeSig = nameOcc._typeSig;
            Subst::substTypeVars(newNameOcc._typeSig, substs);
				return fromCell(cell_);
			}
	
			case LET: {
				Let& let = toBody(expr, Let);
				Cell& cell_ = *new(sizeof(Let), tran.msa()) Cell(LET);
				new(cell_.body()) Let(instantiateList(let._declOrDefns, argV, nArgs, substs, tran),
											instantiateExpr(let._expr, argV, nArgs, substs, tran),
											let._recursive,
											let._substitutable);
				return fromCell(cell_);
			}

         case LAMBDA: {
            Lambda& lambda = toBody(expr, Lambda);
            NameTypedValBinding* lambdaFormalParamV =
               (NameTypedValBinding*)tran.msa().alloc(sizeof(NameTypedValBinding) * lambda._nFormalParams);
            for (UInt i = 0; i < lambda._nFormalParams; i++)
               lambdaFormalParamV[i] = lambda._formalParamV[i];
            Cell& cell_ = *new(sizeof(Lambda), tran.msa()) Cell(LAMBDA);
            Expr body = instantiateExpr(lambda._body, argV, nArgs, substs, tran);
            new(cell_.body()) Lambda(lambda._nFormalParams, lambdaFormalParamV,
               body,
               lambda._typeSig, lambda._updatable);

            Subst::substTypeVars(cellBody(cell_, Lambda)._typeSig, substs);
            return fromCell(cell_);
         }

			case FN_AP: {
				FnAp& fnAp = toBody(expr, FnAp);

				Expr* args = (Expr*)tran.msa().alloc(fnAp._nArgs * sizeof(Expr));
				for (UInt i = 0; i < fnAp._nArgs; i++)
					args[i] = instantiateExpr(fnAp._argV[i], argV, nArgs, substs, tran);

				Cell& cell_ = *new(sizeof(FnAp), tran.msa()) Cell(FN_AP);
				new(cell_.body()) FnAp(instantiateExpr(fnAp._fun, argV, nArgs, substs, tran),
											 args, fnAp._nArgs, fnAp._typeSig, tran.msa());

            Subst::substTypeVars(cellBody(cell_, FnAp)._typeSig, substs);   

				tran.msa().free(args);

				return fromCell(cell_);
			}

			default:
				break;
		}
   return expr;
}

Expr Lambda::instantiate(Expr* argV, UInt nArgs, TypeSig typeSig, Tran& tran) const {
   Subst* substs = NULL;

#ifdef TRACE
   if (traceFlag) {
      outStream << "Lambda::instantiate: ";
      print(outStream, tran.env());
      outStream << '\n';
      outStream.flush();
      for (UInt i = 0; i < nArgs; i++) {
         outStream << "Arg " << i << ": ";
         printTranExpr(argV[i], FALSE, outStream, tran.env());
         outStream << '\n';
         outStream.flush();
      }
   }
#endif

   return instantiateExpr(_body, argV, nArgs, substs, tran);
}

// Lambda::reduce: No arguments

Bool Lambda::reduce(Expr& expr, Tran& tran) {
   return FALSE;
}

Void Lambda::print(ostream& os, const Env& env,
                   Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << '\\';
//   os << _nNonGlobalFree << ", " << _needsClosure << ", " << _noPartialAps << ", " << _updatable;

#ifdef PRINT_TYPE_SIGS
	os << "(::";
	printTypeSig(_typeSig, 0, FALSE, os, env);
	os << ") ";
#endif

   for (UInt i = 0; i < _nFormalParams; i++) {
      if (i > 0)
         os << (pretty ? ' ' : ',');
      _formalParamV[i].print(FALSE, os, env, FALSE);
   }
   os << "->";
   if (pretty)
      newlineAndIndent(indent + 1, os);

   printTranExpr(_body, FALSE, os, env, pretty, indent + 1);
}

Void CurriedLambda::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
}

// Let::Let: Constructor

static Void setRecInitState(Expr declOrDefns, Defn::RecInitState recInitState) {
   for (; declOrDefns != Nil; declOrDefns = tl(declOrDefns)) {
      NameTypedValBinding& binding = toBody(hd(declOrDefns), NameTypedValBinding);
      Expr val = binding.typedVal().val();
      ExprEnumVal form = formOf(val);
      if (form == DEFN ||
          form == CLASS_METHOD_DEFN ||
          form == INSTANCE_METHOD_DEFN)
         toBody(val, Defn).setRecInitState(recInitState);
   }
}

Let::Let(Expr declOrDefns, Expr expr, Bool recursive/* = FALSE*/, Bool substitutable/* = FALSE*/)
   : _declOrDefns(declOrDefns), _expr(expr),
     _recursive(recursive), _needsReservation(FALSE),
     _substitutable(substitutable),
     _lambda(NULL) {
   if (_recursive)
      setRecInitState(_declOrDefns, Defn::REC_STATE_PENDING);
}

Let::Let(const Let& src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran)
   : _recursive(src._recursive), _needsReservation(FALSE),
     _substitutable(src._substitutable),
   _lambda(NULL)  {
   _declOrDefns = tran.copyList(src._declOrDefns, cc, typeInsts, substs);
   _expr = tran.copyExpr(src._expr, cc, typeInsts, substs);
   if (_recursive)
      setRecInitState(_declOrDefns, Defn::REC_STATE_PENDING);
}

// T.B.D. Consider eta substitution: (let x = y in x) => y

Void Let::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
	tran.tranDeclOrDefns(_declOrDefns, FALSE);
   BindingSet localBindingEnv(&bindingEnv, _lambda);
   tran.addBindings(_declOrDefns, localBindingEnv);
   tran.tranDeclOrDefns(_declOrDefns, localBindingEnv);
   tran.tranExpr(_expr, localBindingEnv);
}

//Let::copy: Copy a let expression

Expr Let::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(Let), tran.msa()) Cell(LET);
   new(cell_.body()) Let(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// Let::hasNameOcc: Predicate to test for the occurrence of a given name

Bool Let::hasNameOcc(Name name, const Tran& tran) const {
   if (tran.hasNameOccInList(name, _declOrDefns))
      return TRUE;
   return tran.hasNameOcc(name, _expr);
}

// Let::nameOccCount: Count for a given binding

UInt Let::nameOccCount(const TypedVal* typedVal,
                       const Tran& tran) const {
   return tran.nameOccCountInList(_declOrDefns, typedVal) +
          tran.nameOccCount(_expr, typedVal);
}

// Let::alphaConv: Alpha convert

Void Let::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   tran.alphaConvList(_declOrDefns, binding, toName);
   tran.alphaConv(_expr, binding, toName);
}

// Let::substitute: Substitute occurrences in a let

Void Let::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
   for (Expr declOrDefns = _declOrDefns; declOrDefns != Nil; declOrDefns = tl(declOrDefns)) {
      NameTypedValBinding& ntvb = toBody(hd(declOrDefns), NameTypedValBinding);
      if (tran.hasNameOcc(ntvb.name(), val)) {

//------ Free variable captured by the substitution, apply alpha conversion

         Name fromName = ntvb.name();
         Name toName = newName();

#ifdef TRACE
         if (traceFlag) {
            outStream << "Let::substitute:  ";
            printName(fromName, outStream, tran.nameTable());
            outStream << " capture of free variable substitued with ";
            printName(toName, outStream, tran.nameTable());
            outStream.flush();
         }
#endif

//------ Change the name of all declarations or definitions with the same name

         for (Expr declOrDefns = _declOrDefns; declOrDefns != Nil; declOrDefns = tl(declOrDefns)) {
            NameTypedValBinding& ntvb = toBody(hd(declOrDefns), NameTypedValBinding);
            if (ntvb.name() == fromName) {
               ntvb.name() = toName;
               if (formOf(ntvb.typedVal().val()) == DEFN)
                  alphaConv(&ntvb.val(), toName, tran);
            }
         }
      }
      tran.substituteExpr(ntvb.typedVal().val(), val, binding);
   }
   tran.substituteExpr(_expr, val, binding);
}

Void Let::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   lambda.substituteList(_declOrDefns, argV, nArgs, tran);
   lambda.substituteExpr(_expr, argV, nArgs, tran);
}


// Let::isSubstitutable: Special case to for constant lambda
//                       or to avoid expression construction
//                       followed by immediate reduction

Bool Let::isSubstitutable(Name name) {
   if (isReduce(_expr)) {
      Expr form = formOf(snd(_expr));
      if (form == (Expr)LAMBDA)
         return TRUE;
      if (form == (Expr)LET) {
         Let& innerLet = toBody(snd(_expr), Let);
         if (length(innerLet._declOrDefns) == 1) {
            NameTypedValBinding& binding = toBody(hd(innerLet._declOrDefns), NameTypedValBinding);
            Expr val = binding.typedVal().val();
            Defn& defn = toBody(val, Defn);
            if (isReduce(defn.expr()) &&
               formOf(snd(defn.expr())) == NAME_OCC &&
               toBody(snd(defn.expr()), NameOcc).name() == name)
               return TRUE;
         }
      }
   }
   return FALSE;
}

// Let::reduce: Single definition and rhs not reduction

Bool Let::reduce(Expr& expr, Tran& tran) {
   if (!_recursive && length(_declOrDefns) == 1) {
      NameTypedValBinding& binding = toBody(hd(_declOrDefns), NameTypedValBinding);
      Expr val = binding.typedVal().val();
      assert(formOf(val) == DEFN || formOf(val) == CLASS_METHOD_DEFN,
         "Let::reduce: Expected definition");
      Defn& defn = toBody(val, Defn);
      if (tran.nameOccCount(_expr, &binding.typedVal()) < 2 &&
         (_substitutable ||
            formOf(defn._expr) == NAME_OCC ||
            isConst(defn._expr) ||
            isSubstitutable(binding.name()) ||
            formOf(_expr) == NAME_OCC ||
            isReduce(_expr) && formOf(snd(_expr)) == FN_AP &&
            toBody(snd(_expr), FnAp)._fun == Expr(COND))) {
         substitute(expr, defn._expr, &binding.typedVal(), tran);
         expr = _expr;
         return TRUE;
      }
   }
   return FALSE;
}

Bool Let::innerReduce(Tran& tran) {
   return formOf(_expr) == FN_AP &&
      toBody(_expr, FnAp).reduce(_expr, tran);
}

// Let::print: Print a 'let' expression

Void Let::print(ostream& os, const Env& env,
                Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   if (!pretty) {
	   os << (_recursive ? "LET_REC," : "LET,");
      printTranList(_declOrDefns, FALSE, os, env, pretty, indent + 1);
      os << ",";
      printTranExpr(_expr, FALSE, os, env);
   } else {
      os << "let ";
      printTranList(_declOrDefns, FALSE, os, env, pretty, indent + 1, TRUE);
      newlineAndIndent(indent, os);
      os << "in";
      newlineAndIndent(indent + 1, os);
      printTranExpr(_expr, FALSE, os, env, pretty, indent + 1);
   }
}

/*----------------------------------------------------------------------------*/

// AppLambda: Constructors

AppLambda::AppLambda(Expr patterns, Expr body, Expr castOcc, Bool castFlag)
   : Coercion(castOcc, !castFlag, castFlag), _patterns(patterns), _body(body),
     _patTypeSig(UNKNOWN),
     _typeSig((TypeSig)UNKNOWN) {
}

AppLambda::AppLambda(const AppLambda& src, CopyContext* cc,
                     const TypeInsts* typeInsts,
                     Subst* substs, Tran& tran)
   : Coercion(tran.copyExpr(src._castOcc, cc, typeInsts, substs), src._resolved,
              src._castFlag, src._instances),
     _patterns(tran.copyList(src._patterns, cc, typeInsts, substs)),
     _body(tran.copyExpr(src._body, cc, typeInsts, substs)),
     _patTypeSig(tran.copyTypeSig(src._patTypeSig, cc, substs)),
     _typeSig(tran.copyTypeSig(src._typeSig, cc, substs)) {
}

// AppLambda::tran: Translate a fully applied lambda application term

Void AppLambda::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() > 1)
      Subst::substTypeVars(_typeSig, tran.substs());

// Bind pattern variables

   BindingSet localBindingEnv = BindingSet(bindingEnv);
   Expr pats = _patterns;
   while (pats != Nil) {
      Expr& pattern = unconstrainedPattern(hd(pats));
      toBody(pattern, Pattern).tran(pattern, localBindingEnv, tran);
      pats = tl(pats);
   }
   tran.tranExpr(_castOcc, localBindingEnv);
   tran.tranExpr(_body, localBindingEnv);   
}

// AppLambda::copy: Copy a coerced term

Expr AppLambda::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs, Tran& tran) const {
   Cell& cell_ = *new(sizeof(AppLambda), tran.msa()) Cell(APP_LAMBDA);
   new(cell_.body()) AppLambda(*this, cc, typeInsts, substs, tran);

   CoercionInst* coercionInst =
      typeInsts != NULL
      ? copyInst(typeInsts->_coercionInsts)
      : NULL;

   if (coercionInst != NULL) {
      AppLambda& appLambda = cellBody(cell_, AppLambda);
      appLambda._resolved = TRUE;
      if (!coercionInst->applyCast())
         appLambda._castFlag = FALSE;
      else {
         Subst::substTypeVars(coercionInst->typeSig(), substs);
         Expr castOcc = toBody((Expr)_castOcc, NameOcc).copy(cc, typeInsts, substs, tran);
         appLambda._castOcc = castOcc;
         appLambda._castFlag = TRUE;
      }
   }

   return fromCell(cell_);
}

// AppLambda::hasNameOcc: Predicate to test for the occurrence of a given name

Bool AppLambda::hasNameOcc(Name name, const Tran& tran) const {
   return tran.hasNameOcc(name, _body);
}

// AppLambda::nameOccCount: Count for a given binding

UInt AppLambda::nameOccCount(const TypedVal* typedVal,
                             const Tran& tran) const {
   return tran.nameOccCount(_body, typedVal);
}

// AppLambda::Alpha convert

Void AppLambda::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   tran.alphaConv(_body, binding, toName);
}

// AppLambda::substitute: Substitute occurrences in an applied lambda

Void AppLambda::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
   tran.substituteExpr(_body, val, binding);
}

Void AppLambda::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   lambda.substituteExpr(_body, argV, nArgs, tran);
}

Void AppLambda::print(ostream& os, const Env& env,
                      Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
	os << "APP_LAMBDA::";
   printTypeSig(_typeSig, 0, FALSE, outStream, env);
   if (!_resolved || _castFlag) {
      os << ',';
      printTranExpr(_castOcc, FALSE, outStream, env, pretty, indent);
   }
   os << ',';
   printTranList(_patterns, FALSE, outStream, env, FALSE);
   if (pretty)
      newlineAndIndent(indent + 1, outStream);
   else
      os << ',';
   printTranExpr(_body, FALSE, outStream, env, pretty, indent + 1);
}

AppLambda& AppLambda::fromExpr(Expr alt) {
	assert(formOf(alt) == APP_LAMBDA, "AppLambda::fromExpr: expected lambda alternative");
	return toBody(alt, AppLambda);
}

/*----------------------------------------------------------------------------*/

// FatBarSeq: Constructors

FatBarSeq::FatBarSeq(Expr alts, Expr args, TypeSig patTypeSig)
   : _alts(alts), _args(args), _patTypeSig(patTypeSig) {
}

FatBarSeq::FatBarSeq(const FatBarSeq& src, CopyContext* cc,
                     const TypeInsts* typeInsts,
                     Subst* substs, Tran& tran) {
   _alts = tran.copyList(src._alts, cc, typeInsts, substs);
   _args = tran.copyList(src._args, cc, typeInsts, substs);
   _patTypeSig = tran.copyTypeSig(src._patTypeSig, cc, substs);
}

// FatBarSeq::hasNameOcc: Predicate to test for the occurrence of a given name

Bool FatBarSeq::hasNameOcc(Name name, const Tran& tran) const {
   return tran.hasNameOccInList(name, _args) ||
          tran.hasNameOccInList(name, _alts);
}

// FatBarSeq::nameOccCount: Count for a given binding

UInt FatBarSeq::nameOccCount(const TypedVal* typedVal,
                             const Tran& tran) const {
   return tran.nameOccCountInList(_args, typedVal) +
          tran.nameOccCountInList(_alts, typedVal);
}

// FatBarSeq::alphaConv: Alpha convert

Void FatBarSeq::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   tran.alphaConvList(_args, binding, toName);
   tran.alphaConvList(_alts, binding, toName);
}

// FatBarSeq::substitute: Substitute occurrences in a fat bar sequence

Void FatBarSeq::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
   tran.substituteList(_args, val, binding);
   tran.substituteList(_alts, val, binding);
}

Void FatBarSeq::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
   lambda.substituteList(_args, argV, nArgs, tran);
   lambda.substituteList(_alts, argV, nArgs, tran);
}

// FatBarSeq::altsPrefix:

Void FatBarSeq::altsPrefix(Expr alts, ExprPred pred, Expr& alts1, Expr& alts2) {
   alts1 = alts;
   Expr* altPtr = &alts1;
   while (*altPtr != Nil) {
		if (!(*pred)(hd(AppLambda::fromExpr(hd(*altPtr)).patterns()))) {
         alts2 = *altPtr;
         *altPtr = Nil;
         return;
      }
      altPtr = &tl(*altPtr);
   }
   alts2 = Nil;
}

// FatBarSeq::extKeyAltsList
// Extend key/alts list either by adding to list for key or inserting a new (key, [alt])

Expr FatBarSeq::extKeyAltsList(Expr alt, Expr key, Expr keyAltsList, ExprEqFn eqFn, MSA& msa) {
   Expr tailAlt = cons(alt, Nil, msa);
   Expr list = keyAltsList;
   while (list != Nil) {
      if ((*eqFn)(key, fst(hd(list)))) {
         snd(hd(list)) = concat(snd(hd(list)), tailAlt, msa);
         return keyAltsList;
      }
      list = tl(list);
   }
   return concat(keyAltsList, cons(mkPair(key, tailAlt, msa), Nil, msa), msa);
}


// FatBarSeq::splitAlts: Split alternatives either by the type of the head pattern
// or by data constructor, or by constant.

Expr FatBarSeq::splitAlts(Expr alts, Expr keyAltsList, FatBarSeq::SplitAltsKind kind, Tran& tran) {
   if (alts == Nil)
      return keyAltsList;
   else {
      AppLambda& appLambda = AppLambda::fromExpr(hd(alts));
      Expr pattern = hd(appLambda.patterns());
      Expr key;
      ExprEqFn eqFn = NULL;
      switch (kind) {
         case SplitByType:
            key = patternTypeSig(pattern, tran);
            eqFn = eqMatchTypeSig;
            break;

         case SplitByConst: {
            appLambda.patterns() = tl(appLambda.patterns());
            key = toBody(pattern, ConstPattern).k();
            eqFn = eqConst;
            break;
         }

         case SplitByDataCon: {
            DataConPattern& dataConPattern = toBody(pattern, DataConPattern);
            appLambda.patterns() = concat(dataConPattern.patterns(),
                                          tl(appLambda.patterns()),
                                          tran.msa());
            dataConPattern._patterns = Nil;
            key = pattern;
            eqFn = eqDataConPattern;
            break;
         }
      }
      return splitAlts(tl(alts), extKeyAltsList(hd(alts), key, keyAltsList, eqFn, tran.msa()), kind, tran);
   }
}

// FatBarSeq::varAlt: Head pattern of alternative is a variable

static TypeSig fatBarFnTypeSig(TypeSig typeSig, Tran& tran) {
   return arrowTypeSig(typeSig,
                       arrowTypeSig(typeSig, typeSig, tran.msa()),
                       tran.msa());
}

Expr FatBarSeq::varAlt(Expr alt, Expr args, TypeSig typeSig, Tran& tran) {

#ifdef TRACE
   if (traceFlag) {
      outStream << "FatBarSeq::varAlt: ";
      printTypeSig(typeSig, 0, FALSE, outStream, tran.env());
      outStream << "\nAlt\n";
      printTranExpr(alt, FALSE, outStream, tran.env(), TRUE);
      outStream << "\nArgs\n";
      printTranList(args, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

	AppLambda& appLambda = AppLambda::fromExpr(alt);
   Expr pattern = hd(appLambda.patterns());
   appLambda.patterns() = tl(appLambda.patterns());

   Expr expr = tran.fatBarSeq(cons(alt, Nil, tran.msa()), tl(args));
	if (formOf(pattern) == VAR_PATTERN) 
		return tran.reduce(tran.simpleLet(mkTriple(CONSTRAIN,
                                                  fromName(toBody(pattern, VarPattern).name()),
                                                  typeSig, tran.msa()),
                                        hd(args),
                                        expr));
	else {
		assert (formOf(pattern) == WILDCARD_PATTERN, "FatBarSeq::varAlts: Unexpected pattern");
		return expr;
	}
}

// FatBarSeq::varAlts: Head pattern of alternatives is variable or wildcard

Expr FatBarSeq::varAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran) {
   TypeSig altTypeSig = AppLambda::fromExpr(hd(alts)).typeSig();
   Expr expr = varAlt(hd(alts), args,  typeSig, tran);
	return tl(alts) != Nil
		? tran.reduce(tran.fnAp2(FAT_BAR,
                               expr,
                               varAlts(tl(alts), tl(args), typeSig, tran),
                               fatBarFnTypeSig(altTypeSig, tran)))
	   : expr;
}


// FatBarSeq::commonConst: Head pattern is common constant

Expr FatBarSeq::commonConst(Expr constPair, Expr args, TypeSig typeSig, Tran& tran) {

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::commonConst\n";
      printTranExpr(constPair, FALSE, outStream, tran.env(), TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   TypeSig patTypeSig = TypeSig(UNKNOWN);
   if (snd(constPair) != Nil) {
      AppLambda& appLambda = AppLambda::fromExpr(hd(snd(constPair)));
      if (appLambda.patterns() != Nil)
         patTypeSig = patternTypeSig(hd(appLambda.patterns()), tran);
   }

   snd(constPair) = tran.fatBarSeq(snd(constPair), tl(args), patTypeSig);

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::commonConst\n";
      printTranExpr(constPair, FALSE, outStream, tran.env(), TRUE), 
      outStream << '\n';
      outStream.flush();
   }
#endif

   return constPair;
}

// FatBarSeq::constAlts: Head pattern of alternatives is a constant

Expr FatBarSeq::constAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran) {
   AppLambda& appLambda = AppLambda::fromExpr(hd(alts));
   ConstPattern& constPattern = toBody(hd(appLambda.patterns()), ConstPattern);

// Split into a list of (const, alts) pairs

   alts = splitAlts(alts, Nil, SplitByConst, tran);
   mapOver3(commonConst, alts, args, typeSig, tran);

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::constAlts ";
      printTranList(alts, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   Expr eqOcc = constPattern._eqOcc;
	UInt n;
   Expr arg = hd(args);
   Expr k = fst(hd(alts));
   Bool typeSigConst = isPair(k) && fst(k) == Expr(TYPE_SIG);
   if (typeSigConst) {
      Name eqTypeName = tran.useName("eqType");
      Cell& cell_ = *new(sizeof(NameOcc), tran.msa()) Cell(NAME_OCC);
      TypedVal* typedVal = tran.lookUpBuiltInBinding(eqTypeName);
      new(cell_.body()) NameOcc(eqTypeName,
                                typedVal,
                                typedVal != NULL
                                   ? typedVal->typeSig()
                                   : TypeSig(UNKNOWN));
      eqOcc = fromCell(cell_);
   }

   n = length(alts);
   if (typeSigConst && n > 1) {
      return tran.reduce(tran.caseExpr(Case::CASE_TYPE,
                                       arg,
                                       typeCon(Type),
                                       n,
                                       alts,
                                       appLambda.typeSig(),
                                       FALSE));
   }

   return n > 1
      ? tran.reduce(tran.caseExpr(Case::CASE_CONST,
                                  arg,
                                  constPattern.typeSig(),
                                  n,
                                  alts,
                                  appLambda.typeSig(),
                                  FALSE))
      : tran.condEq(arg, eqOcc,
                    typeSigConst
                       ? k
                       : Expr(mkPair(LITERAL, k, tran.msa())),
                    snd(hd(alts)), appLambda.typeSig());
}

// FatBarSeq::commonDataCon: Head pattern is common data constructor

static Bool isDeptrAp(Expr expr) {
   if (formOf(expr) == FN_AP) {
      Expr fn = toBody(expr, FnAp).fun_();
      return isTriple(fn) &&
             fst3(fn) == (Expr)SELECT &&
             isPair(thd3(fn)) &&
             fst(thd3(fn)) == dataCon(Ptr);
   } else
      return FALSE;
}

static Bool isDeconDeptrAp(Expr expr) {
   if (isReduce(expr) && formOf(snd(expr)) == FN_AP) {
      FnAp fnAp = toBody(snd(expr), FnAp);
      return fnAp.fun_() == (Expr)DECONSTRUCT &&
             isReduce(fnAp.arg_()) &&
             isDeptrAp(snd(fnAp.arg_()));
   }
   else
      return FALSE;
}

Expr FatBarSeq::commonDataCon(Expr dataConPair, Expr args, TypeSig typeSig, Tran& tran) { 

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::commonDataCon: ";
      printTypeSig(typeSig, 0, FALSE, outStream, tran.env());
      outStream << '\n';
      printTranExpr(dataConPair, FALSE, outStream, tran.env(), TRUE);
      outStream << '\n';
      printTranList(args, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
   }
#endif

   DataConPattern& dataConPattern = toBody(fst(dataConPair), DataConPattern);
   Expr dataCon = dataConPattern.dataCon_();
   Bool isPrimitive = !isDataCon(dataCon) || dataCon == dataCon(Ptr);
   Bool isConcrete = formOf(fst(dataConPair)) == CONCRETE_PATTERN;
   Subst* substs = dataConPattern._substs;

   fst(dataConPair) = fromTag(dataConTag(dataCon));

   if (isNilaryDataCon(dataCon) && !isConcrete) {
      assert(tl(args) == Nil, "FatBarSeq::commonDataCon: expected Nil");
      snd(dataConPair) = tran.fatBarSeq(snd(dataConPair), Nil);
   }
   else {
      Expr arg = isPrimitive
         ? hd(args)
         : (!toBody(dataCon, DataCon).sumType()
            ? dataConPattern.deconstructP(hd(args), typeSig, tran)
            : dataConPattern.deconstructS(hd(args), toBody(dataCon, DataCon).tag(), typeSig, tran));

      if (!isConcrete) {
         UInt arity = dataConArity(dataCon);
         if (arity > 1 && !isPrimitive) {
  
            TypeSig ts = isPrimitive
               ? typeSig
               : dataConPattern.condTypeSig(dataConPattern._reprTypeSig, tran.msa());

            if (!isDeconDeptrAp(arg)) {
               Expr name = fromName(newName());

               snd(dataConPair) = tran.reduce(
                  tran.simpleLet(
                     mkTriple(CONSTRAIN,
                        name,
                        ts, tran.msa()),
                     arg,
                     tran.fatBarSeq(snd(dataConPair),
                        concat(tran.unpackDataCon(dataCon, arity,
                           dataConPattern._isPlainDecon, dataConPattern._isPtrDecon, FALSE,
                           dataConPattern._typeSig,
                           ts,
                           substs,
                           dataConPattern._moduleDefn,
                           name), tl(args), tran.msa()))));
            }
            else
               snd(dataConPair) =
                  tran.fatBarSeq(snd(dataConPair),
                     concat(tran.unpackDataCon(dataCon, arity,
                        dataConPattern._isPlainDecon, dataConPattern._isPtrDecon, TRUE,
                        dataConPattern._typeSig,
                        ts,
                        substs,
                        dataConPattern._moduleDefn,
                        tran.copyExpr(arg, NULL, NULL, NULL)), tl(args), tran.msa()));
         }
         else
            snd(dataConPair) = tran.fatBarSeq(snd(dataConPair),
               concat(tran.unpackDataCon(dataCon, arity,
                  dataConPattern._isPlainDecon, dataConPattern._isPtrDecon, FALSE,
                  dataConPattern._typeSig,
                  isPrimitive ? typeSig : dataConPattern._reprTypeSig,
                  substs,
                  dataConPattern._moduleDefn,
                  arg), tl(args), tran.msa()));
      }
      else
      {
         snd(dataConPair) =
            tran.fatBarSeq(snd(dataConPair),
               concat(cons(arg, Nil, tran.msa()), tl(args), tran.msa()));
      }
   }
   return dataConPair;
}

// FatBarSeq::dataConAlts: Head pattern of alternatives is a data constructor of a type

// [\ C p1 p2 ... -> e, \ C p1 p2 ... -> e, ...]

// T.B.D. handle nilary data constructors more simply

static Expr fatBarPairAlts(Expr alts, TypeSig altTypeSig, Tran& tran) {
   Expr alt = snd(hd(alts));
   return tl(alts) == Nil
      ? alt
      : tran.reduce(tran.fnAp2(FAT_BAR,
                               alt,
                               fatBarPairAlts(tl(alts), altTypeSig, tran),
                               fatBarFnTypeSig(altTypeSig, tran)));
}

static Expr fatBarTypeAlts(Expr alts, TypeSig altTypeSig, Tran& tran);

Expr FatBarSeq::splitSameDataConAltsByHeadType(Expr alts, Tran& tran) {
   Expr typePairs = Nil;
   Expr nextAlts = alts;
   Expr dataCon = ERROR;

   while (nextAlts != Nil) {
      AppLambda& appLambda = AppLambda::fromExpr(hd(nextAlts));
      Expr pattern = hd(appLambda.patterns());
      DataConPattern& dataConPattern = toBody(pattern, DataConPattern);

      if (dataCon == (Expr)ERROR)
         dataCon = dataConPattern.dataCon_();
      else if (dataConPattern.dataCon_() != dataCon)
         return Nil;

      if (dataConPattern.patterns() == Nil)
         return Nil;

      TypeSig typeSig = patternTypeSig(hd(dataConPattern.patterns()), tran);
      if (hasNonMatchPatternTypeVar(typeSig))
         return Nil;

      typePairs = extKeyAltsList(hd(nextAlts),
                                 typeSig,
                                 typePairs,
                                 eqMatchTypeSig,
                                 tran.msa());
      nextAlts = tl(nextAlts);
   }

   return length(typePairs) > 1
      ? typePairs
      : Nil;
}

Expr FatBarSeq::commonDataConAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran) {

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::commonDataConAlts: ";
      printTypeSig(typeSig, 0, FALSE, outStream, tran.env());
      outStream << "\nAlts: ";
      printTranList(alts, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << "\nArgs: ";
      printTranList(args, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   TypeSig hf = headForm(typeSig);
   Int nDataCons = isTupleCon(hf)
      ? 1
      : toBody(hf, TypeCon).n(); 
   UInt nAlts = length(alts);

// All alternatives are of the same type
// So use the first to provide it

   AppLambda& hdAppLambda = AppLambda::fromExpr(hd(snd(hd(alts))));
   TypeSig altTypeSig = hdAppLambda.typeSig();

   mapOver3(commonDataCon, alts, args, typeSig, tran);

   Expr arg = hd(args);

   if (nDataCons == 1 && nAlts > 1)
      return fatBarPairAlts(alts, altTypeSig, tran);

   if (nAlts > 1) {

//--- Test for special cases

      Bool isBool = typeSig == typeCon(Bool);
      Bool isPtr  = isPtrType(unwrapPlainTypeSig(typeSig));
      Bool isList = isListTypeSig(unwrapPlainTypeSig(typeSig));

      if (isBool || isPtr || isList) {
         Expr hdTag = fst(hd(alts));
         Expr alt1 = snd(hd(alts));
         Expr alt2 = snd(hd(tl(alts)));
         if (typeSig == typeCon(Bool))
            return tran.cond(arg, alt1, alt2, hdTag == fromTag(FALSE),
                             hdAppLambda.typeSig());
         return tran.condNULL(arg, alt1, alt2, hdTag != fromTag(NULL_TAG),
                              typeSig, hdAppLambda.typeSig());
      } else
         return tran.reduce(tran.caseExpr(Case::CASE_TAG,
                                          tran.reduce(tran.fnAp(TAG_OF,
                                                      arg,
                                                      ap2(ARROW, typeSig, TAG, tran.msa()))),
                                          TAG,
                                          nAlts,
                                          alts,
                                          typeSig,
                                          nDataCons == nAlts));
   }
	else {

// Currently a test for Null is suppressed for a single Ptr match
// Because of some key data constructors like Cons

      Expr hdTag = fst(hd(alts));
      Expr alt1 = snd(hd(alts));
      return isListTypeSig(unwrapPlainTypeSig(typeSig))
         ? tran.condNULL(arg, alt1, Expr(FAIL), hdTag != fromTag(NULL_TAG),
                         typeSig, hdAppLambda.typeSig())
         : (nDataCons == 1 || isPtrType(unwrapPlainTypeSig(typeSig))
               ? alt1
               : tran.condTag(arg, hdTag, alt1, typeSig, hdAppLambda.typeSig()));
	}
}

Expr FatBarSeq::dataConAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran) {

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::dataConAlts: ";
      printTypeSig(typeSig, 0, FALSE, outStream, tran.env());
      outStream << "\nAlts\n";
      printTranList(alts, FALSE, outStream, tran.env(), TRUE);
      outStream << "\nArgs\n";
      printTranList(args, FALSE, outStream, tran.env(), TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   Expr typePairs = splitSameDataConAltsByHeadType(alts, tran);
   if (typePairs != Nil) {
      TypeSig altTypeSig = AppLambda::fromExpr(hd(snd(hd(typePairs)))).typeSig();
      Expr resPairs = Nil;
      Expr pairs = typePairs;
      while (pairs != Nil) {
         resPairs = concat(resPairs,
                           cons(mkPair(fst(hd(pairs)),
                                       dataConAlts(snd(hd(pairs)), args, typeSig, tran),
                                       tran.msa()),
                                Nil,
                                tran.msa()),
                           tran.msa());
         pairs = tl(pairs);
      }
      return fatBarTypeAlts(resPairs, altTypeSig, tran);
   }

// Split into a list of (data constructor, alts) pairs

// [ (C1, [\p1 p2 ... -> e, \p1 p2 ... -> e ...]), (C2, [\p1 p2 ... -> e, \p1 p2 ... -> e ...]), ...] 

   alts = splitAlts(alts, Nil, SplitByDataCon, tran);

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      printTranList(alts, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   Expr arg = hd(args);
   if ((isReduce(arg) &&
       (length(_alts) > 1 ||
        dataConArity(toBody(fst(hd(alts)), DataConPattern)._dataCon) > 0))) {
      if (!isDeptrAp(snd(arg))) {
         Expr name = fromName(newName());
         hd(args) = name;
         return tran.reduce(tran.simpleLet(mkTriple(CONSTRAIN, name, typeSig, tran.msa()),
            arg,
            commonDataConAlts(alts, args, typeSig, tran)));
      }
      else {
         hd(args) = tran.copyExpr(arg, NULL, NULL, NULL);
         return commonDataConAlts(alts, args, typeSig, tran);
      }
   } else
	   return commonDataConAlts(alts, args, typeSig, tran);
}

// FatBarSeq::equalTypeAlts: Match alternatives of the same type

Expr FatBarSeq::equalTypeAlts(Expr alts, Expr args, TypeSig typeSig, Tran& tran) {

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::equalTypeAlts: ";
      printTypeSig(typeSig, 0, FALSE, outStream, tran.env());
      outStream << "\nAlts: ";
      printTranList(alts, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << "\nArgs: ";
      printTranList(args, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   Expr allAlts = alts;
   Expr allArgs = args;
   Expr alts1 = alts;
   Expr alts2 = Nil;

// 1. Generate list of alternatives where the head pattern is a variable or wildcard

   altsPrefix(alts, varOrWildCardPattern, alts1, alts2);

   if (alts1 != Nil)
      alts = varAlts(alts1, args, typeSig, tran);
   else {

//--- 2. Generate list of alternatives where the head pattern is a constant

      altsPrefix(alts, constPattern, alts1, alts2);
      if (alts1 != Nil)
         alts = constAlts(alts1, args, typeSig, tran);
      else {

//------ 3. Generate list of alternatives where the head pattern is a data constructor

         altsPrefix(alts, dataConPattern, alts1, alts2);
         assert(alts1 != Nil && alts2 == Nil, "Tran::typedFatBarSeq: unexpected pattern");
         return dataConAlts(alts1, args, typeSig, tran);
      }
   }

   if (alts2 == Nil)
      return alts;
   else
      return defaultAlts(allAlts, allArgs, tran);
}

// FatBarSeq::typeAlt: Head pattern  is common distinct type

Expr FatBarSeq::commonType(Expr typePair, Expr args, Tran& tran) {

   tran.copyExpr(hd(args), NULL, NULL, NULL);

   Expr arg = hd(args);
   if (!isReduce(arg))
      snd(typePair) = tran.fatBarSeq(snd(typePair), args);
   else {
      Expr name = fromName(newName());
      snd(typePair) = tran.reduce(tran.simpleLet(mkTriple(CONSTRAIN, name, fst(typePair), tran.msa()),
         hd(args),
         tran.fatBarSeq(snd(typePair), cons(name, tl(args), tran.msa()))));
   }
   return typePair;
}

static Expr fatBarTypeAlts(Expr alts, TypeSig altTypeSig, Tran& tran) {
   Expr alt = snd(hd(alts));
   return tl(alts) == Nil
      ? alt
      : tran.reduce(tran.fnAp2(FAT_BAR,
                               alt,
                               fatBarTypeAlts(tl(alts), altTypeSig, tran),
                               fatBarFnTypeSig(altTypeSig, tran)));
}

// FatBarSeq::typeAlts: head patterns of alternatives are different types

Expr FatBarSeq::typeAlts(Expr alts, Expr args, UInt, Tran& tran) {

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::typeAlts\n";
      printTranList(alts, FALSE, outStream, tran.env(), TRUE);
      outStream << "\nArgs\n";
      printTranList(args, FALSE, outStream, tran.env(), TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   TypeSig altTypeSig = AppLambda::fromExpr(hd(snd(hd(alts)))).typeSig();

   mapOver2(commonType, alts, args, tran);

   return fatBarTypeAlts(alts, altTypeSig, tran);
}

Expr FatBarSeq::defaultLambdaAp(Expr patterns, Expr body, Expr args, Tran& tran) {
   if (patterns == Nil) {
      assert(args == Nil, "defaultLambdaAp: arg list not empty");
      return body;
   }

   Expr pattern = hd(patterns);
   Expr expr = defaultLambdaAp(tl(patterns), body, tl(args), tran);
   if (formOf(pattern) == VAR_PATTERN)
      return tran.reduce(tran.simpleLet(mkTriple(CONSTRAIN,
                                                 fromName(toBody(pattern, VarPattern).name()),
                                                 toBody(pattern, VarPattern).typeSig(),
                                                 tran.msa()),
                                        hd(args),
                                        expr));
   else if (formOf(pattern) == WILDCARD_PATTERN)
      return expr;
   else if (formOf(pattern) == CONST_PATTERN)
      return tran.condEq(hd(args),
                         toBody(pattern, ConstPattern)._eqOcc,
                         pattern,
                         expr,
                         UNKNOWN);

   assert(FALSE, "defaultLambdaAp: unexpected pattern");
   return (Expr)ERROR;
}

Expr FatBarSeq::defaultAlts(Expr alts, Expr args, Tran& tran) {
   AppLambda& appLambda = AppLambda::fromExpr(hd(alts));
   TypeSig altTypeSig = appLambda._typeSig;
   Expr expr = defaultLambdaAp(appLambda.patterns(), appLambda.body(), args, tran);
   if (tl(alts) == Nil)
      return expr;

   Expr otherwise = defaultAlts(tl(alts), args, tran);
   return otherwise != (Expr)ERROR
      ? tran.reduce(tran.fnAp2(FAT_BAR,
                               expr,
                               otherwise,
                               fatBarFnTypeSig(altTypeSig, tran)))
      : (Expr)ERROR;
}

// FatBarSeq::matchPatterns: Match patterns according to common criteria
// Returns true if pattern types are fixed, false otherwise

static Bool reducedFatBarSeq(Expr expr) {
   return isReduce(expr) && formOf(snd(expr)) == FAT_BAR_SEQ;
}

Bool FatBarSeq::mergeReducedArg(Tran& tran, Expr& res) {
   if (length(_args) != 1 || !reducedFatBarSeq(hd(_args)))
      return FALSE;

   FatBarSeq& inner = toBody(snd(hd(_args)), FatBarSeq);
   TypeSig resTypeSig = _alts != Nil
      ? AppLambda::fromExpr(hd(_alts)).typeSig()
      : TypeSig(UNKNOWN);
   Expr innerAlts = inner._alts;

   while (innerAlts != Nil) {
      AppLambda& innerAlt = AppLambda::fromExpr(hd(innerAlts));
      Expr outerAlts = tran.copyList(_alts, NULL, NULL, NULL);
      Expr outerArg = innerAlt.body();
      innerAlt.body() =
         tran.reduce(tran.fatBarSeq(outerAlts,
                                    cons(outerArg, Nil, tran.msa()),
                                    _patTypeSig));
      if (resTypeSig != TypeSig(UNKNOWN))
         innerAlt._typeSig = resTypeSig;
      innerAlts = tl(innerAlts);
   }

   res = hd(_args);
   return TRUE;
}

Bool FatBarSeq::matchPatterns(Tran& tran, Expr& res) {

#ifdef TRACE_FAT_BAR_SEQ
	static UInt probe = 0;
   if (++probe == 0) {
		outStream << "***** FatBarSeq::matchPatterns: probe hit\n";
      traceFlag = TRUE;
   }

   if (traceFlag) {
      outStream << "FatBarSeq::matchPatterns(" << probe << "): ";
      outStream << "Alts: ";
      printTranList(_alts, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << "\nArgs: ";
      printTranList(_args, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
   }
#endif

	assert(_alts != Nil, "FatBarSeq::matchPatterns: unexpected empty list");

   if (mergeReducedArg(tran, res))
      return TRUE;

	AppLambda& hdAlt = AppLambda::fromExpr(hd(_alts));
   Subst::substTypeVars(hdAlt._typeSig, tran.substs());

// Empty pattern, simple return

	if (hdAlt.patterns() == Nil) {
      if (length(_alts) != 1)
         error("FatBarSeq::matchPatterns: case not distinct");
      res = hdAlt.body();
      return TRUE;
   }
   Expr alts = _alts;

// Split into list of (type, alts) pairs

   alts = splitAlts(_alts, Nil, SplitByType, tran);

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag) {
      outStream << "FatBarSeq::matchPatterns (type, alts) pairs\n";
      printTranList(alts, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
      printTranList(_args, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

// Check polymorphic

   Expr tAlts = alts;
   while (tAlts != Nil) {
      TypeSig typeSig = fst(hd(tAlts));
      if (hasNonMatchPatternTypeVar(typeSig))
         return false;
      tAlts = tl(tAlts);
   }

   UInt n;
   if ((n = length(alts)) > 1) {
      res = typeAlts(alts, _args, n, tran);
   }
   else {
      TypeSig typeSig = fst(hd(alts));
      if (_patTypeSig != TypeSig(UNKNOWN)) {
         Subst::substTypeVars(_patTypeSig, tran.substs());
         typeSig = _patTypeSig;
      }

//--- The head patterns of all the alternatives are the same type
//--- Arbitrarily select the first cast name occurence to apply to the argument

      hdAlt._castFlag = FALSE;  // T.B.D. use cast for *

      if (hdAlt._castFlag) {
         hd(_args) = tran.reduce(tran.fnAp(hdAlt._castOcc,
            hd(_args),
            toBody(hdAlt._castOcc, NameOcc)._typeSig));
         hdAlt._castFlag = FALSE;   // Sub patterns are not cast
      }
      Expr arg = hd(_args);

      if (!isReduce(arg) || length(_alts) < 2)
         res = equalTypeAlts(_alts, _args, typeSig, tran);
      else {
         Expr name = fromName(newName());

         hd(_args) = name;
         res = tran.reduce(tran.simpleLet(mkTriple(CONSTRAIN, name, typeSig, tran.msa()),
                                                       arg,
                                                       equalTypeAlts(_alts,
                                                                     _args,
                                                                     typeSig, tran)));
      }
   }
   return TRUE;
}

// FatBarSeq::tran: Translate a fat bar sequence term

// [args] [AppLambda]
// where AppLambda = [<pattern>, <body>, <cast occ.>]
// Cast occ. provides a template for casting the intial argument

Void FatBarSeq::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() == 1 || !matchPatterns(tran, expr)) {
      BindingSet localBindingEnv(&bindingEnv, bindingEnv._lambda);
      mapProc1(tran.tranExpr, _args, localBindingEnv);
      mapProc1(tran.tranExpr, _alts, localBindingEnv);
   }
   else {
      if (traceFlag) {
         outStream << "FatBarSeq::tran: matchPatterns result\n";
         printTranExpr(expr, FALSE, outStream, tran.env(), TRUE);
         outStream << '\n';
         outStream.flush();
      }
      tran.tranExpr(expr, bindingEnv);
   }
}

// FatBarSeq::innerReduce

Void FatBarSeq::innerReduceAlt(Expr alt, Tran& tran) {
   AppLambda& alt_ = AppLambda::fromExpr(alt);
   alt_._body = tran.reduce(alt_._body);
   tran.tranReduce(alt_._body);
   alt_._typeSig = arg(alt_._typeSig);
}

Bool FatBarSeq::innerReduce(Tran& tran) {
   mapProc1(innerReduceAlt, _alts, tran);
   return TRUE;
}

// FatBarSeq::copy: Copy a fat bar sequence term

Expr FatBarSeq::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                     Tran& tran) const {
   Cell& cell_ = *new(sizeof(FatBarSeq), tran.msa()) Cell(FAT_BAR_SEQ);
   new(cell_.body()) FatBarSeq(*this, cc, typeInsts, substs, tran);
   return fromCell(cell_);
}

// FatBarSeq::print: Print a fat bar sequence term

Void FatBarSeq::print(ostream& os, const Env& env,
                      Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   os << "FAT_BAR_SEQ,";
   printTranList(_args, FALSE, outStream, env, FALSE);
   if (!pretty)
      os << ',';
   printTranList(_alts, FALSE, outStream, env, pretty, indent + 1, TRUE);
   os << endl;
}

/*----------------------------------------------------------------------------*/

// Global module list

ModuleDefn* ModuleDefn::workingSet = NULL;

// ModuleDefn: Constructor

ModuleDefn::ModuleDefn(Name name, Expr declOrDefns, Env* env)
   : _env(env), _name(name), _declOrDefns(declOrDefns),
     _typeConNameMap(NULL),
     _classDefns(NULL), _classInstancesMap(MODULE_CLASS_INSTANCES_MAP_N_SLOTS),
     _substs(NULL), _constraints(NULL),
     _varNameMaps(NULL), _typeVarMaps(NULL),
     _next(NULL) {
   _typeConNameMap = new(env->msa()) NameExprMap(47);
   _exportMap = new(env->msa()) ExportMap(47);
}

// ModuleDefn::tran: Translate a module definition

Void ModuleDefn::tran(BindingSet& bindingEnv, Tran& tran) {
   tran._moduleDefn = this;
	if (tran.pass() == 1) {
		mapProc1(tranDeclOrDefn, _declOrDefns, tran);
	} else {
	   ((Tran*)((void*)&tran))->_substs = _substs;
      ((Tran*)((void*)&tran))->_typeVarMaps = _typeVarMaps;
   }

   mapProc2(addBinding, _declOrDefns, bindingEnv, tran);
   mapProc2(tranDeclOrDefn, _declOrDefns, bindingEnv, tran);
 
   if (tran.pass() == 2) {
      _bindings._bindings_ = bindingEnv._bindings_;


#ifdef TRACE
      if (traceFlag) {
         outStream << "Bindings" << endl;
         outStream << "========" << endl;
         bindingEnv._bindings_.print(outStream, *_env);
      }
#endif

//--- Name mappings may be added during transformation

      _varNameMaps = tran._varNameMaps;
      _typeVarMaps = tran._typeVarMaps;

       ModuleDefn** moduleDefnPtr = &workingSet;
      while (*moduleDefnPtr != NULL)
         moduleDefnPtr = &(*moduleDefnPtr)->_next;

      // Append to working set
      *moduleDefnPtr = this;
   }
 
#ifdef TRACE
   OptionsParser optionsParser(tran._options);
   if (optionsParser.hasKey("printAST")) {
      Int optionsPass;
      if (!optionsParser.getNum("pass", optionsPass) ||
         tran.pass() == optionsPass) {
         printTranList(_declOrDefns, FALSE, outStream, tran.env(), TRUE, 0, TRUE);
         outStream << "\n--------------------------------------------\n";
         outStream.flush();
      }
   }
#endif

}

// ModuleDefn::tranDeclOrDef: Basic translation of outermost definitions

Void ModuleDefn::tranDeclOrDefn(Expr& declOrDefn, Tran& tran) {
   Bool isInline = FALSE;
 
   Bool knownPair = FALSE;
   if (isPair(declOrDefn)) {
      if (fst(declOrDefn) == Expr(INLINE)) {
         isInline = TRUE;
         declOrDefn = snd(declOrDefn);
      } else
         knownPair = TRUE;
   }
   if (knownPair || isPair(declOrDefn)) {
      Expr s = snd(declOrDefn);
      switch (formOf(fst(declOrDefn))) {
         case TYPE_DEFN: {
            Bool isDynamic = FALSE;
            if (isPair(s)) {
               isDynamic = TRUE;
               s = snd(s);
            }
            Cell& cell_ = *new(sizeof(TypeDefn), tran.msa()) Cell(TYPE_DEFN);
            new(cell_.body()) TypeDefn(toName(fst3(s)),
												   snd3(s),
												   thd3(s),
                                       tran);
            cellBody(cell_, TypeDefn).postConstruct(tran);
            declOrDefn = fromCell(cell_);
				cellBody(cell_, TypeDefn).tran(declOrDefn, tran);
            break;
         }

         case CLASS_DEFN: {
            Cell& cell_ = *new(sizeof(ClassDefn), tran.msa()) Cell(CLASS_DEFN);
            new(cell_.body()) ClassDefn(fst(fst(s)),
                                        toName(fst(snd(fst(s)))),
													snd(snd(fst(s))),
													snd(s));
            addClassDefn(cellBody(cell_, ClassDefn));
            declOrDefn = fromCell(cell_);
				cellBody(cell_, ClassDefn).tran(tran);
            break;
         }

         case INSTANCE_DEFN: {
            Bool subordinate = isPair(s) &&
                               fst(s) == Expr(SUBORDINATE);
            if (subordinate)
               s = snd(s);
            Cell& cell_ = *new(sizeof(InstanceDefn), tran.msa()) Cell(INSTANCE_DEFN);
            new(cell_.body()) InstanceDefn(toName(fst3(s)),
                                          subordinate,
														fst(snd3(s)),
														snd(snd3(s)),
														thd3(s));
            addInstanceDefn(cellBody(cell_, InstanceDefn));
            declOrDefn = fromCell(cell_);
            cellBody(cell_, InstanceDefn).tran(tran);
            break;
         }

         default:
            break;
      }
   } else if (isTriple(declOrDefn))
      switch (formOf(fst3(declOrDefn))) {
         case DATA_CON_DECL: {
            Cell& cell_ = *new(sizeof(DataConDecl), tran.msa()) Cell(DATA_CON_DECL);
            new(cell_.body()) DataConDecl(toName(snd3(declOrDefn)), thd3(declOrDefn), tran);
            declOrDefn = fromCell(cell_);
				cellBody(cell_, DataConDecl).tran(tran);
            return;
         }

         case DATA_CON_DEFN: {
            Cell& cell_ = *new(sizeof(DataConDefn), tran.msa()) Cell(DATA_CON_DEFN);
            new(cell_.body()) DataConDefn(snd3(declOrDefn), thd3(declOrDefn),
                                         FALSE, isInline);
            declOrDefn = fromCell(cell_);
				cellBody(cell_, DataConDefn).tran(tran);
            return;
         }

         default:
            break;
      }
	tran.tranDeclOrDefn(declOrDefn, TRUE, isInline, FALSE);
}

// ModuleDefn::addBinding: Add binding for declaration or definition

Void ModuleDefn::addBinding(Expr declOrDefn, BindingSet& bindingEnv, Tran& tran) const {
    switch (formOf(declOrDefn)) {
      case TYPE_DEFN:
         toBody(declOrDefn, TypeDefn).addBindings(bindingEnv, tran);
			break;

      case DATA_CON_DECL:
         toBody(declOrDefn, DataConDecl).addBinding(bindingEnv, tran);
			break;

      case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).addBindings(declOrDefn, bindingEnv, tran);
			break;

      case CLASS_DEFN:
         toBody(declOrDefn, ClassDefn).addBindings(bindingEnv, tran);
			break;

      case INSTANCE_DEFN:

//------ Instances are bound by type

         break;

      default:
         tran.addBinding(declOrDefn, bindingEnv);
			break;
   }
}

// ModuleDefn::tranDeclOrDef: Translate outermost definitions

Void ModuleDefn::tranDeclOrDefn(Expr& declOrDefn, BindingSet& bindingEnv, Tran& tran) {
   switch (formOf(declOrDefn)) {
      case TYPE_DEFN:
         toBody(declOrDefn, TypeDefn).tran(declOrDefn, bindingEnv, tran);
         break;

      case DATA_CON_DECL:
         toBody(declOrDefn, DataConDecl).tran(bindingEnv, tran);
         break;

      case DATA_CON_DEFN:
         toBody(declOrDefn, DataConDefn).tran(bindingEnv, tran);
         break;

      case CLASS_DEFN:
         toBody(declOrDefn, ClassDefn).tran(bindingEnv, tran);
         break;

      case INSTANCE_DEFN:
         toBody(declOrDefn, InstanceDefn).tran(bindingEnv, tran);
         break;

      default:
         tran.tranDeclOrDefn(declOrDefn, bindingEnv);
         break;
   }
}

TypedVal* ModuleDefn::lookUp(Name name, Bool dataCon, const Env& env,
                             Name& mappedNameRes) const {
   if (!(isBuiltInName(name) || &env == _env)) {
      const char* str = nameString_(name, env.nameTable());
      if (str == NULL || (name = _env->nameTable().lookUp(str)) == NULL_NAME)
         return NULL;
      name += builtInNameCount;
   }
   TypedVal* res = _bindings.lookUp(name, dataCon, FALSE);
   if (res != NULL)
      mappedNameRes = name;
   return res;
}

// ModuleDefn::addGlobal: Add global name position

Void ModuleDefn::addGlobal(Name name, UInt pos) {

// If built-in name, map to module definition name table
/*
   if (name < builtInNameCount) {
      const char* str = nameString(name, builtInNameTable());
      name = _env->nameTable().lookUp(str);  
   }
   else
      name -= builtInNameCount;
*/

   ExportMapEntry* entry = _exportMap->lookUp(name, name);
   if (entry == NULL) {
      ExportMapEntry entry = ExportMapEntry(name, pos);
      _exportMap->add(entry, name, _env->msa());
   }
}

// ModuleDefn::print: Print module definitions

Void ModuleDefn::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/) const {
   os << "MODULE";
   os << (pretty ? ": " : ",");
   printTranName(_name, os, env);
   os << (pretty ? '\n' : ',');
   printTranList(_declOrDefns, FALSE, os, env, pretty, 0, pretty);
}

/*----------------------------------------------------------------------------*/

Order::Order(Expr sequence, Tran& tran)
   : _sequence(sequence), _substs(NULL),
     _varNameMaps(NULL), _typeVarMaps(NULL)
{

// Reset supply of type variables

   resetTypeVarSupply();

    mapProc1(tranSequenceExpr, _sequence, tran);

   _sequence = tran.lambda(Nil, tran.tranSequenceList(_sequence, TRUE), UNKNOWN, FALSE);
}

Void Order::tran(BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() > 1) {
      ((Tran*)((void*)&tran))->_substs = _substs;
      ((Tran*)((void*)&tran))->_varNameMaps = _varNameMaps;
      ((Tran*)((void*)&tran))->_typeVarMaps = _typeVarMaps;
   }

   tran.tranExpr(_sequence, bindingEnv);

#ifdef TRACE
   OptionsParser optionsParser(tran._options);
   if (optionsParser.hasKey("printAST")) {
      Int optionsPass;
      if (optionsParser.getNum("pass", optionsPass) &&
          optionsPass == tran.pass()) {
         printTranExpr(_sequence, FALSE, outStream, tran.env(), TRUE);
         outStream << "\n--------------------------------------------\n";
         outStream.flush();
      }
      if (_substs != NULL)
      _substs->print(outStream, tran.env());
   }
#endif

}

Void Order::tranSequenceExpr(Expr& expr, Tran& tran) const {
    expr = tran.reduce(ap(fromName(tran.useName("showWithNewline")),
                         tran.coerce(expr, !(isPair(expr) && fst(expr) == (Expr)NO_COERCE &&
                                             isPair(snd(expr)) && fst(snd(expr)) == (Expr)NO_COERCE)),
                         tran.msa()));
}

Void Order::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/) const {
   os << "ORDER,";
   if (!pretty)
      os << ',';
   newlineAndIndent(1, os);
   printTranExpr(_sequence, FALSE, os, env);
}

/*----------------------------------------------------------------------------*/

Snippet::Snippet(Expr expr)
   : _expr(expr), _substs(NULL),
   _varNameMaps(NULL), _typeVarMaps(NULL) {
   resetTypeVarSupply();
}

Void Snippet::tran(BindingSet& bindingEnv, Tran& tran) {
   if (tran.pass() > 1) {
      ((Tran*)((void*)&tran))->_substs =        _substs;
      ((Tran*)((void*)&tran))->_typeVarMaps =   _typeVarMaps;
   }
   tran.tranExpr(_expr, bindingEnv);

#ifdef TRACE
   OptionsParser optionsParser(tran._options);
   if (optionsParser.hasKey("printAST")) {
      Int optionsPass;
      if (!optionsParser.getNum("pass", optionsPass) ||
         tran.pass() == optionsPass) {
         printTranExpr(_expr, FALSE, outStream, tran.env(), TRUE);
         outStream << '\n';
         outStream.flush();
      }
   }
#endif

}

Void Snippet::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/) const {
   os << "SNIPPET,";
   if (!pretty)
      os << ',';
   newlineAndIndent(1, os);
   printTranExpr(_expr, FALSE, os, env, pretty, 1);
}

/*----------------------------------------------------------------------------*/

// Catch non-overloaded syntax forms

Void AST_Node::tran(Expr& expr, BindingSet& bindingEnv, Tran& tran) {
   notOverloaded("tran", tran.env());
}

Expr AST_Node::copy(CopyContext* cc, const TypeInsts* typeInsts, Subst* substs,
                      Tran& tran) const {
   notOverloaded("copy", cc == NULL ? tran.env() : cc->env());
   return (Expr)ERROR;
}

Bool AST_Node::hasNameOcc(Name name, const Tran& tran) const {
   notOverloaded("hasNameOcc", tran.env());
   return FALSE;
}

UInt AST_Node::nameOccCount(const TypedVal* typedVal,
   const Tran& tran) const {
   notOverloaded("nameOccCount", tran.env());
   return 0;
}

Void AST_Node::alphaConv(const TypedVal* binding, Name toName, const Tran& tran) {
   notOverloaded("alphaConv", tran.env());
}

Void AST_Node::substitute(Expr& expr, Expr val, TypedVal* binding, Tran& tran) {
    notOverloaded("substitute", tran.env());
}

Void AST_Node::substitute(Expr& expr, Expr* argV, UInt nArgs, Lambda& lambda, Tran& tran) {
    notOverloaded("substitute", tran.env());
}


Bool AST_Node::reduce(Expr& expr, Tran& tran) {
   notOverloaded("reduce", tran.env());
   return FALSE;
}

Bool AST_Node::innerReduce(Tran& tran) {
   notOverloaded("innerReduce", tran.env());
   return FALSE;
}

Void AST_Node::print(ostream& os, const Env& env, Bool pretty/* = FALSE*/, UInt indent/* = 0*/) const {
   error("SyntaxForm::print: not overloaded");
}

Void AST_Node::notOverloaded(ConstString methodStr, const Env& env) const {
   print(outStream, env);
   outStream << '\n';
   static Char buf[256];
   sprintf(buf, "SyntaxForm::%s: not overloaded", methodStr);
   assert(FALSE, buf);
}

/*----------------------------------------------------------------------------*/

// Tran::Tran: Constructor for the translator object

Tran::Tran(UInt pass, ConstString options, Env& env)
   : _nErrors(0), _nWarnings(0),
     _pass(pass),
     _options(options),
     _env(&env),
	  _substs(NULL),
     _varNameMaps(NULL), _typeVarMaps(NULL),
     _moduleDefn(NULL),
     _resolverBindingName(NULL_NAME),
     _resolverBindingTypeSig(UNKNOWN) {

#ifdef TRACE
   OptionsParser optionsParser(_options);
   optionsParser.getNum("tranProbe", tranProbeSeqNo);
   optionsParser.getNum("copyProbe", copyProbeSeqNo);
   optionsParser.getNum("reduceProbe", reduceProbeSeqNo);
   if (optionsParser.hasKey("trace")) {
      Int optionsPass;
      if (optionsParser.getNum("pass", optionsPass) &&
         optionsPass == pass) {
         traceFlag = TRUE;
         if (traceFlag)
            traceClassCompilation(TRUE);
      }
      else
         traceFlag = FALSE;
   }
   else
      traceFlag = FALSE;
#endif
}

// Tran::Tran: Look up an external Binding

TypedVal* Tran::lookUpExtBinding(Name name, Bool dataCon,
                                 const ModuleDefn*& moduleDefnRes, Name& mappedNameRes) {
   TypedVal* typedVal;
   ModuleDefn* moduleDefn = ModuleDefn::workingSet;
   while (moduleDefn != NULL) {
      typedVal = moduleDefn->lookUp(name, dataCon, *_env, mappedNameRes);
      if (typedVal != NULL) {
         moduleDefnRes = moduleDefn;
         return typedVal;
      }
      moduleDefn = moduleDefn->next();
   }
 
   return NULL;
}

// Tran::Tran: Look up a built-in Binding

TypedVal* Tran::lookUpBuiltInBinding(Name name) {
   TypedVal* typedVal;
   if ((typedVal = _builtInBindingSet.lookUp(name, FALSE)) == NULL) {
      const char* str = nameString_(name, _env->nameTable());
      if (str == NULL)
         return NULL;

      Expr res;
      if (builtInNameMap().lookUp(builtInNameTable().lookUp(str),
                                          hashString(str),
                                          res)) {
         TypeSig typeSig;
         switch (formOf(res)) {
            case BUILT_IN_DATA_CON:
               typeSig = expTypeSig(toBody(res, BuiltInDataCon).typeSig(), msa());
               break;

            case BUILT_IN_FN:
               typeSig = toBody(res, BuiltInFn).typeSig();
               break;

            case CLASS_METHOD:
               typeSig = toBody(res, ClassMethod).typeSig();
               break;

            default:
               outStream << "Unrecognised name: ";
               printName(name, outStream, nameTable());
               outStream << '\n';
               outStream.flush();
               _nErrors++;
               return NULL;
         }
         TypeVarSet typeVarSet;
         if (hasTypeVar(typeSig)) {
            typeSig = mutableTypeSig(typeSig, msa());
            TypeEnv typeEnv;
            insTypeSig(typeSig, &typeEnv, &typeVarSet, msa());
         }

			NameTypedValBinding* binding = new (msa())
				NameTypedValBinding(name, res, typeSig, msa());
         binding->typedVal().schematicTypeVars() = typeVarSet;

         ((BindingSet*)(void*)(&_builtInBindingSet))->add(*binding);
			return &binding->typedVal();
      }
   }

   return typedVal;
}

Void Tran::lookUpDataCon(Expr& dataCon, BindingSet& bindingEnv, TypedVal*& typedVal, const ModuleDefn*& moduleDefnRes) {
   if (formOf(dataCon) == NAME) {
      Name name = toName(dataCon);
      Name mappedName;
      if ((typedVal = bindingEnv.lookUp(name, TRUE)) == NULL &&
         (typedVal = lookUpExtBinding(name, TRUE, moduleDefnRes, mappedName)) == NULL &&
         (typedVal = lookUpBuiltInBinding(name)) == NULL) {
         outStream << "Unresolved data constructor: ";
         printName(name, outStream, nameTable());
         outStream << '\n';
         outStream.flush();
         _nErrors++;
         dataCon = (Expr)ERROR;
      }

      while (typedVal != NULL) {
         if (isDataCon(typedVal->val()))
            break;
         if (formOf(typedVal->val()) == DEFN)
            typedVal = toBody(typedVal->val(), Defn)._prevDeclOrDefn;
         else
            break;
      }
      assert(typedVal != NULL, "Tran::lookUpDataCon: Missing data constructor");
      dataCon = typedVal->val();
   }
}

// Tran::useName: useName in the context of the translation name table

Name Tran::useName(ConstString nameString) const {
	return ::useName(nameString, nameTable());
}

Expr Tran::fnAp(Expr f, Expr arg, TypeSig typeSig/* = UNKNOWN*/,
                Bool updatable/* = TRUE*/) const {
   Cell& cell_ = *new(sizeof(FnAp), msa()) Cell(FN_AP);
	new(cell_.body()) FnAp(f, arg, typeSig, msa(), updatable);
   return fromCell(cell_);
}

Expr Tran::fnAp2(Expr f, Expr arg1, Expr arg2, TypeSig typeSig/* = UNKNOWN*/,
                 Bool updatable/* = TRUE*/) const {
   Cell& cell_ = *new(sizeof(FnAp), msa()) Cell(FN_AP);
   Expr args[2] = { arg1, arg2 };
   new(cell_.body()) FnAp(f, args, 2, typeSig, msa(), updatable);
   return fromCell(cell_);
}

Expr Tran::fnAp(Expr f, Expr* argV, UInt nArgs, TypeSig typeSig/* = UNKNOWN*/) const {
   Cell& cell_ = *new(sizeof(FnAp), msa()) Cell(FN_AP);
	new(cell_.body()) FnAp(f, argV, nArgs, typeSig, msa());
   return fromCell(cell_);
}

Bool Tran::coerceToApply(Expr& expr, Bool noCast) const {
   if (isPair(expr) && fst(expr) == Expr(NO_COERCE)) {
      expr = snd(expr);
      return coerceToApply(expr, TRUE);
   }
   else
      return !(noCast ||
         isTriple(expr) && fst3(expr) == Expr(CONSTRAIN) ||
         isPair(expr) && fst(expr) == Expr(NO_MAP_TO_ENV) ||
         isPair(expr) && fst(expr) == Expr(REDUCE) ||
 //        isPair(expr) && fst(expr) == Expr(EVAL) ||
         isPair(expr) && fst(expr) == Expr(LITERAL));
}

Expr Tran::coerce(Expr expr, Bool force/*= FALSE */) const {
   if (force || coerceToApply(expr, FALSE)) {
      Cell& cell_ = *new(sizeof(Coerced), msa()) Cell(COERCE);
      new(cell_.body()) Coerced(expr, fromName(useName("cast")));
      return fromCell(cell_);
   }
   else
      return expr;
}

Expr Tran::mapToEnv(Expr src, Expr dstEnv) const {
   return reduce(fnAp(reduce(fnAp(fromName(useName("condMapToEnv")),
                                  coerce(src))),
                      dstEnv));
}

Expr Tran::reduce(Expr expr) const {
   return mkPair(REDUCE, expr, msa());
}

Expr Tran::condReduce(Expr expr, Bool flag) const {
   return flag ? reduce(expr)
               : expr;
}

Bool Tran::hasNameOcc(Name name, Expr expr) const {
   if (isPair(expr))
      return hasNameOcc(name, fst(expr)) || hasNameOcc(name, snd(expr));
   else
      switch (formOf(expr)) {
         case NAME_TYPED_VAL_BINDING:
            return hasNameOcc(name, toBody(expr, NameTypedValBinding).typedVal().val());
            break;

         case DEFN:
         case COERCE:
         case LAMBDA:
         case LET:
         case NAME_OCC:
         case FN_AP:
         case FAT_BAR_SEQ:
         case APP_LAMBDA:
         case CASE:
            assert(isPtr(expr), "Tran::hasNameOcc: Unexpected form");
            return toBody(expr, AST_Node).hasNameOcc(name, *this);
         default:
            return FALSE;
      }
}

Bool Tran::hasNameOccInList(Name name, Expr list) const {
   for (Expr x = list; x != Nil; x = tl(x))
      if (hasNameOcc(name, hd(x)))
         return TRUE;
   return FALSE;
}

UInt Tran::nameOccCount(Expr expr, const TypedVal* typedVal) const {
   if (isPair(expr))
      return nameOccCount(fst(expr), typedVal) +
      nameOccCount(snd(expr), typedVal);
   else
      switch (formOf(expr)) {
         case NAME_TYPED_VAL_BINDING:
            return nameOccCount(toBody(expr, NameTypedValBinding).typedVal().val(),
                                typedVal);
            break;

         case DEFN:
         case COERCE:
         case LAMBDA:
         case LET:
         case NAME_OCC:
         case FN_AP:
         case FAT_BAR_SEQ:
         case APP_LAMBDA:
         case CASE:
            assert(isPtr(expr), "Tran::nameOccCount: Unexpected form");
            return toBody(expr, AST_Node).nameOccCount(typedVal, *this);
         default:
            return 0;
      }
}

UInt Tran::nameOccCountInList(Expr list, const TypedVal* typedVal) const {
   UInt res = 0;
   for (Expr x = list; x != Nil; x = tl(x))
      res += nameOccCount(hd(x), typedVal);
   return res;
}

// Tran::alphaConv
// Replace the name of all occurences with a matching binding

Void Tran::alphaConv(Expr expr, const TypedVal* binding, Name toName) const {
   if (isPair(expr)) {
      alphaConv(fst(expr), binding, toName);
      alphaConv(snd(expr), binding, toName);
   } else
      switch (formOf(expr)) {
         case NAME_TYPED_VAL_BINDING:
            alphaConv(toBody(expr, NameTypedValBinding).typedVal().val(), binding, toName);
            break;

         case DEFN:
         case COERCE:
         case LAMBDA:
         case LET:
         case NAME_OCC:
         case FN_AP:
         case FAT_BAR_SEQ:
         case APP_LAMBDA:
         case CASE:
            assert(isPtr(expr), "Tran::substituteExpr: Unexpected form");
            toBody(expr, AST_Node).alphaConv(binding, toName, *this);
            break;
      }
}

Void Tran::alphaConvList(Expr list, const TypedVal* binding, Name toName) const {
   mapProc2(alphaConv, list, binding, toName);
}

Void Tran::substitutePair(Expr pair, Expr val, TypedVal* binding) {
   Expr& f = fst(pair);
   Expr& s = snd(pair);
   if (f == Expr(REDUCE)) {
      substituteExpr(s, val, binding);
      Bool repeat;
      do {
         repeat = FALSE;
         switch (formOf(s)) {
            case FN_AP:
            case LET:
               if (toBody(s, AST_Node).reduce(s, *this) )
                  repeat = TRUE;
               break;

            default:
               break;
         }

         if (repeat) {
            pair = s;
            if (isPair(pair) && fst(pair) == Expr(REDUCE))
               s = snd(pair);
            else
               repeat = FALSE;
         }
      } while (repeat);
   } else {
      substituteExpr(f, val, binding);
      substituteExpr(s, val, binding);
   }
}

Void Tran::substituteList(Expr list, Expr val, TypedVal* binding) {
   mapProc2(substituteExpr, list, val, binding);
}

Void Tran::substituteExpr(Expr& expr, Expr val, TypedVal* binding) {
   if (isPair(expr))
      substitutePair(expr, val, binding);
   else
      switch (formOf(expr)) {
			case NAME_TYPED_VAL_BINDING:
			   substituteExpr(toBody(expr, NameTypedValBinding).typedVal().val(), val, binding);
            break;

	      case DEFN:
         case COERCE:
	      case LAMBDA:
         case LET:
         case NAME_OCC:
         case FN_AP:
         case FAT_BAR_SEQ:
         case APP_LAMBDA:
         case CASE:
            assert(isPtr(expr), "Tran::substituteExpr: Unexpected form");
            toBody(expr, AST_Node).substitute(expr, val, binding, *this);
            break;
      }
}

#ifdef localConst
#undef localConst
#endif
#define localConst const

// Tran::copyTypeSig: Return a unique type signature

// Supports mutual recursion by first applying overall substitution

TypeSig Tran::copyTypeSig(TypeSig typeSig, CopyContext* cc, const Subst* substs) {
   if (hasTypeVar(typeSig)) {
      if (cc == NULL)
         Subst::substTypeVars(typeSig, this->substs());
      if (hasTypeVar(typeSig)) {
         typeSig = cc == NULL
            ? mutableTypeSig(typeSig, msa())
            : mutableTypeSig(typeSig, *cc, msa());
         Subst::substTypeVars(typeSig, substs);
      }
   }
   return typeSig;
}

Expr Tran::copyPair(const Expr src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs) {
   Expr f = fst(src);
   Expr s = snd(src);

   if (f == Expr(TYPE_SIG))
      s = copyTypeSig(s, cc, substs);
   else if (f == Expr(LITERAL)) {
      ExprEnumVal form = formOf(s);
      if (form == NAME && cc != NULL && cc->moduleDefn() != NULL)
         s = fromName(mapName$(toName(s), cc->moduleDefn()->nameTable(), nameTable()));
      else if (form == TYPE && cc != NULL && cc->moduleDefn() != NULL)
         s = fromType(mapType(toType(s), cc->moduleDefn()->env(), env()));
   } else
      s = copyExpr(s, cc, typeInsts, substs);

   return mkPair(f, s, msa());
}

Expr Tran::copyTriple(const Expr src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs) {
   Expr f = fst3(src);
   Expr s = snd3(src);

   if (f == Expr(SELECT) || f == Expr(SEL_PTR))
      TypeSig typeSig = copyTypeSig(s, cc, substs);
   else 
      s = copyExpr(s, cc, typeInsts, substs);
   
   Expr t = copyExpr(thd3(src), cc, typeInsts, substs);
   return mkTriple(f, s, t, msa());
}

Expr Tran::copyList(const Expr list, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs) {
   if (list == Nil)
      return Nil;
   return cons(copyExpr(hd(list), cc, typeInsts, substs),
               copyList(tl(list), cc, typeInsts, substs), msa());
}


Expr Tran::copyExpr(const Expr src, CopyContext* cc, const TypeInsts* typeInsts, Subst* substs) {

#ifdef TRACE_COPY_EXPR
   static UInt probe = 0;
   if (++probe == copyProbeSeqNo) {
		outStream << "***** Tran::copyExpr: probe hit\n";
      traceFlag = TRUE;
   }
   UInt localProbe = probe;
   if (traceFlag) {
      outStream << "Tran::copyExpr(" << probe << "): ";
      printTranExpr(src, FALSE, outStream, cc == NULL
         ? env()
         : cc->env(), TRUE);
      outStream << '\n';

      Subst::printSubsts(substs, outStream, env());
      outStream.flush();
   }
#endif

   switch (formOf(src)) {
      case RETURN:
         return VOID;

      case NAME: {   // Assume name is type variable in this context
         const Subst* subst = Subst::lookUp(toTypeVar(src), substs);
         return subst == NULL ? src
                              : subst->typeSig();

      }

      case AP: return ap(copyExpr(fun(src), cc, typeInsts, substs),
                         copyExpr(arg(src), cc, typeInsts, substs), msa());

      case  VAR:
         return toBody(src, Var).copy(cc, typeInsts, substs, *this);

      case LAMBDA:
      case FN_AP:
		case APP_LAMBDA:
		case FAT_BAR_SEQ:
      case CASE:
      case LET:
		case CONST_PATTERN:
		case VAR_PATTERN:
		case WILDCARD_PATTERN:
		case DATA_CON_PATTERN:
      case CONCRETE_PATTERN:
      case SHARED_EXPR:
      case SHARED_PATTERN_AP:
      case DECL:
      case DEFN:
      case CLASS_METHOD_DECL:
      case CLASS_METHOD_DEFN:
      case INSTANCE_METHOD_DECL:
      case INSTANCE_METHOD_DEFN:
      case NAME_OCC:
      case COERCE:
         if (isPtr(src))
            return toBody(src, AST_Node).copy(cc, typeInsts, substs, *this);
         break;

      case NAME_TYPED_VAL_BINDING:
         return toBody(src, NameTypedValBinding).copy(cc, typeInsts, substs, *this);

      case DATA: {
         Type type = toType(((const CellInfo*)toCell(src).tag())->form());
         if (type == builtInType(Pair))
            return copyPair(src, cc, typeInsts, substs);
         else if (type == builtInType(Triple))
            return copyTriple(src, cc, typeInsts, substs);
         break;
      }

      default:
         break;
   }

   return Expr(src);
}

#undef localConst
#define localConst

// Tran::simpleLet: let signedId = expr in body

Expr Tran::simpleLet(Expr signedId, Expr expr, Expr body, Bool substitutable/* = FALSE*/) {
   return let(cons(defn(signedId, expr, FALSE), Nil, msa()), body, FALSE, substitutable);
}

Expr Tran::defn(Expr lhs, Expr rhs,
                Bool outermost/* = FALSE*/,  Bool redefinable/* = FALSE*/,
                Bool isInline/* = FALSE*/, Bool exclusive/* = FALSE*/) {
   Cell& cell_ = *new(sizeof(Defn), msa()) Cell(DEFN);
   new(cell_.body()) Defn(rhs, outermost, redefinable, isInline, exclusive);
	return nameTypedValBinding(lhs, fromCell(cell_));
}

Expr Tran::let(Expr declOrDefns, Expr body, Bool recursive/* = FALSE*/, Bool substitutable/* = FALSE*/) {
   Cell& cell_ = *new(sizeof(Let), msa()) Cell(LET);
   new(cell_.body()) Let(declOrDefns, body, recursive, substitutable);
	return fromCell(cell_);
}

Expr Tran::makeDataConAlt(Expr dataCon, Expr expr) const {
   return mkPair(cons(mkTriple(DATA_CON_PATTERN, dataCon, Nil, msa()), Nil, msa()),
                 expr, msa());
}

Void Tran::tranCons(Expr& cons, BindingSet& bindingEnv) {
   tranExpr(hd(cons), bindingEnv);
   tranExpr(tl(cons), bindingEnv);
}

// Unpacks components of arg for data constructor
// returns list of patterns and list of components

Expr Tran::unpackDataCon(Expr dataCon, UInt arity,
                         Bool isPlainDecon, Bool isPtrDecon, Bool copyArg,
                         TypeSig typeSig, TypeSig reprTypeSig,
                         Subst* substs, const ModuleDefn* moduleDefn, Expr arg) {

#ifdef TRACE
   if (traceFlag) {
      const Env& env_ = moduleDefn == NULL ? env() : moduleDefn->env();
      outStream << "Tran::unpackDataCon\n";
      printTranExpr(dataCon, FALSE, outStream, env_);
      outStream << '\n';
      printTranExpr(arg, FALSE, outStream, env());
      outStream << '\n';
      printTypeSig(typeSig, 0, FALSE, outStream, env_);
      outStream << '\n';
      Subst::printSubsts(substs, outStream, env());
      outStream.flush();
   }
#endif
   if (isNilaryDataCon(dataCon))
      return Nil;
   else {
      TypeSig resTypeSig = resultTypeSig(typeSig, arity);
      if (isPtrDecon)
         reprTypeSig = ptrTypeSig(reprTypeSig, msa());
  //    if (isPlainDecon)
  //       reprTypeSig = plainTypeSig(reprTypeSig, msa());
      Expr args = Nil;
      Expr* argPtr = &args;
      for (UInt i = 0; i++ < arity;) {
         Expr selector;
         TypeSig argTS = argTypeSig(typeSig, i);
         if (isPtrDecon)
            argTS = ptrTypeSig(argTS, msa());
         if (isPlainDecon)
            argTS = plainTypeSig(argTS, msa());
     
         TypeSig selTypeSig = ap2(ARROW,
            reprTypeSig,
            argTS, msa());
          if (!isDataCon(dataCon) || dataCon == dataCon$Ptr)

 //--------- Tuple and Ptr are primitive

            selector = mkTriple(!isPtrDecon ? SELECT : SEL_PTR,
                                selTypeSig,
                                mkPair(dataCon, fromInt(i, msa()), msa()), msa());
         else {
            ConstString selNameStr = DataCon::selectorName(toBody(dataCon, DataCon).name(), i,
               moduleDefn == NULL ? nameTable() : moduleDefn->nameTable(),
               msa());
            Cell& cell_ = *new(sizeof(NameOcc), msa()) Cell(NAME_OCC);
            new(cell_.body()) NameOcc(useName(selNameStr));

            cellBody(cell_, NameOcc)._typeSig = selTypeSig;
            cellBody(cell_, NameOcc)._substs = substs;

            selector = fromCell(cell_);
         }

         *argPtr = cons(reduce(fnAp(selector,
            !copyArg ? arg : copyExpr(arg, NULL, NULL, NULL),
            selTypeSig)), *argPtr, msa());
         argPtr = &tl((*argPtr));
      }
      return args;
   }
}

// Tran::cond: Return a conditional expression

Expr Tran::cond(Expr arg, Expr eTrue, Expr eFalse, Bool negate, TypeSig typeSig) const {

#ifdef TRACE
   if (traceFlag) {
      outStream << "Tran::cond ";
      if (negate)
         outStream << "(negate)";
      outStream << '\n';

      printTranExpr(arg, FALSE, outStream, env());
      outStream << '\n';
      printTranExpr(eTrue, FALSE, outStream, env());
      outStream << '\n';
      printTranExpr(eFalse, FALSE, outStream, env());
      outStream << '\n';
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << '\n';
      outStream.flush();
   }
#endif

   if (negate)
      swap(eTrue, eFalse);
	Expr argV[] = {arg, eTrue, eFalse};
	return reduce(fnAp(COND, argV, sizeof(argV) / sizeof(Expr), condTypeSig(typeSig)));
}

// Tran::condTypeSig: Return a conditional expression type signature

Expr Tran::condTypeSig(TypeSig typeSig) const {
   return ap2(ARROW,
              typeCon(Bool),
              ap2(ARROW,
                  typeSig,
                  ap2(ARROW,
                      typeSig,
                      typeSig, msa()), msa()), msa());
}

// Tran::condTestTypeSig: Return a conditional expression test type signature

Expr Tran::condTestTypeSig(TypeSig typeSig) const {
   return ap2(ARROW, typeSig, typeCon(Bool), msa());
}

// Tran::condEq: if <arg> = <val> then <expr> else FAIL

Expr Tran::condEq(Expr arg, Expr eqOcc, Expr val, Expr expr, TypeSig typeSig) const {
   NameOcc& nameOcc = toBody(eqOcc, NameOcc);
   Expr argV[2] = {arg, val};
   return cond(reduce(fnAp(eqOcc, argV, 2, toBody(eqOcc, NameOcc)._typeSig)),
               expr,
               FAIL,
               FALSE, typeSig);
}

// Tran::condTag: if <arg> == <tag> then <expr> else <FAIL> 
// Uses: (TAG_OF t)::(t -> TAG)
// where TAG is an hidden primitive type 

Expr Tran::condTag(Expr arg, Expr tag, Expr expr, TypeSig argTypeSig, TypeSig typeSig) const {
   return cond(reduce(fnAp2(EQ_TAG_FN,
                            reduce(fnAp(TAG_OF,
                                        arg,
                                        ap2(ARROW, argTypeSig, TAG, msa()))),
                            reduce(tag),
                            ap2(ARROW, TAG, ap2(ARROW, TAG, typeCon(Bool), msa()), msa()))),
               expr, FAIL, FALSE, typeSig);
}

// Tran::cond: if <arg> == NULL then <e1> else <e2> (swap e1 & e2 if negate)
// Primarily used for lists.

Expr Tran::condNULL(Expr arg, Expr e1, Expr e2, Bool negate, 
                    TypeSig argTypeSig, TypeSig typeSig) const {


#ifdef TRACE
   if (traceFlag) {
      outStream << "Tran::condNULL\n";
      printTranExpr(arg, FALSE, outStream, env());
      outStream << '\n';
      printTranExpr(e1, FALSE, outStream, env());
      outStream << '\n';
      printTranExpr(e2, FALSE, outStream, env());
      outStream << '\n';
      printTypeSig(argTypeSig, 0, FALSE, outStream, env());
      outStream << '\n';
      outStream.flush();
      printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << '\n';
      outStream.flush();
   }
#endif

   return cond(reduce(fnAp(EQ_NULL_FN, arg, condTestTypeSig(argTypeSig))),
               e1, e2, negate, typeSig);
}

// Generate lambda form


Void Tran::tranPattern(Expr pattern, NameTypedValBinding* bindings, UInt& i) {
   Name name;
   TypeSig typeSig;
   idForm(pattern, name, typeSig);
   Cell& cell_ = *new(sizeof(Var), msa()) Cell(VAR);
   new(cell_.body()) Var(name, typeSig, i == 0 ? Var::FORMAL_PARAM_VAR : Var::EXTRA_FORMAL_PARAM_VAR);
   bindings[i++] = NameTypedValBinding(name, fromCell(cell_), typeSig, msa());
}

NameTypedValBinding* Tran::tranPatterns(UInt nPatterns, Expr patterns) {
   NameTypedValBinding* bindings = (NameTypedValBinding*)msa().alloc(sizeof(NameTypedValBinding) * nPatterns);
   UInt i = 0;
   mapProc2(tranPattern, patterns, bindings, i);
   return bindings;
}

Expr Tran::lambda(Expr patterns, Expr body, TypeSig typeSig/* = UNKNOWN*/, Bool updatable/* = TRUE*/) {
   if (isTriple(body) && fst3(body) == Expr(LAMBDA))

//--- uncurry the body

      return lambda(concat(patterns, snd3(body), msa()), thd3(body));

   if (patterns != Nil && varOrWildCardPattern(unconstrainedPattern(hd(patterns)))) {
      Bool nonVar = FALSE;
      Expr pats = tl(patterns);
      while (pats != Nil) {
         if (!varOrWildCardPattern(unconstrainedPattern(hd(patterns)))) {
            nonVar = TRUE;
            break;
         }
         pats = tl(pats);
      }
      if (nonVar)
         return mkTriple(LAMBDA,
                         cons(hd(patterns), Nil, msa()),
                         lambda(tl(patterns), body), msa());
   }
   else {
      Expr vars = Nil;
      Expr pats = patterns;
      while (pats != Nil) {
          vars = cons(fromName(newName()), vars, msa());
         pats = tl(pats);
      }
      reverse(vars, msa());
      body = lambdaAp(patterns, body, vars);
      patterns = vars;
   }

   Cell& cell_ = *new(sizeof(Lambda), msa()) Cell(LAMBDA);
   UInt nPatterns = length(patterns);
   new(cell_.body()) Lambda(nPatterns, tranPatterns(nPatterns, patterns),
                            body, typeSig, updatable);
   return fromCell(cell_);
}

Expr Tran::lambdaAp(Expr patterns, Expr body, Expr args) {
   Bool dummy = FALSE;

   /* outStream << "lambdaAp:\n";
      printTranExpr(patterns, FALSE, outStream, env());
      outStream << '\n';

      printTranExpr(body, FALSE, outStream, env());
      outStream << '\n';

      printTranExpr(args, FALSE, outStream, env());
      outStream << '\n';

      outStream.flush(); */

   if (length(patterns) == 0) {
      assert(length(args) == 0, "Tran::lambdaAp: arg list not empty");
      return body;
   }

   Expr pattern = hd(patterns);
   Expr arg = hd(args);

    if (varPattern(pattern)) {

      // let pattern = arg in ... body

      return mkPair(LET,
         mkTriple(pattern,
            arg,
            lambdaAp(tl(patterns), body, tl(args)), msa()),
         msa());
   } else if (wildCardPattern(pattern)) {
      return lambdaAp(tl(patterns), body, tl(args));
   } else if (constPattern(pattern)) {

      // if arg = pattern then ... body else FAIL

//      Expr Tran::condEq(Expr arg, Expr eqOcc, Expr val, Expr expr, TypeSig typeSig) const {

     return condEq(arg, toBody(pattern, ConstPattern)._eqOcc,
                   pattern,
                   lambdaAp(tl(patterns), body, tl(args)),
                   UNKNOWN);
   }

// Handle dataCon pattern

// (DATA_CON_PATTERN, DATA_CON, [pattern])

   else if (dataConPattern(pattern)) {

// Handle sum or product case

      Expr dataCon;
      Expr conPats;

      if (isPair(pattern)) {
         dataCon = fromTupleCon(length(snd(pattern)) - 2);
         conPats = snd(pattern);
      }
      else {
         dataCon = snd3(pattern);
         conPats = thd3(pattern);
      }
//      TypeSig typeSig = patternTypeSig(dataCon);

// T.B.D. Investigate use
//        If needed, then unpackDataCon needs module definition argument

      assert(FALSE, "Tran::lambdaAp: unexpected");

      TypeSig typeSig = ERROR;
      args = concat(unpackDataCon(dataCon, dataConArity(dataCon), FALSE, FALSE, FALSE, typeSig, typeSig, NULL, NULL, arg), tl(args), msa());
      return lambdaAp(concat(conPats, tl(patterns), msa()), body, args);
//      return condTag(dataCon, arg,
//                     lambdaAp(concat(thd3(pattern), tl(patterns), msa()), body,
//                              args),
//                     typeSig);
   }
   error("invalid pattern");

   printTranExpr(pattern, FALSE, outStream, env());
   outStream << '\n';

   return (Expr)ERROR;
}

// FAT_BAR sequence

// \(p11 p12 ... p1n.e1) a1 a2 ... an []
// \(p21 p22 ... p2n.e2) a1 a2 ... an []
// ...
// \(pm1 pm2 ... pmn.em) a1 a2 ... an

Expr Tran::fatBarSeq(Expr alts, Expr args, TypeSig patTypeSig) {

#ifdef TRACE_FAT_BAR_SEQ
   if (traceFlag && pass() > 1) {
      outStream << "Alts: ";
      printTranList(alts, FALSE, outStream, env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream << "Args: ";
      printTranList(args, FALSE, outStream, env(), TRUE, 0, TRUE);
      outStream << '\n';
      outStream.flush();
   }
#endif

   Cell& cell_ = *new(sizeof(FatBarSeq), msa()) Cell(FAT_BAR_SEQ);
   new(cell_.body()) FatBarSeq(alts, args, patTypeSig);
	return fromCell(cell_);
}

Expr Tran::caseExpr(Case::Kind kind, Expr arg, TypeSig argTypeSig,
                    UInt n, Expr alts, TypeSig typeSig, Bool closed) {
   Cell& cell_ = *new(sizeof(Case), msa()) Cell(CASE);
   new(cell_.body()) Case(kind, arg, argTypeSig, n, alts, typeSig,
                         closed, msa());
   return fromCell(cell_);
}

// Conditionally transform a default case for a pattern

Expr Tran::tranDefault(Expr expr, Bool hasDefault, Expr otherwise) {
   return hasDefault
      ? reduce(fnAp2(FAT_BAR, expr, otherwise))
      : expr;
}

// [([patterns], expr)]
// ->
// [(pattern, SHARED expr)]
//
// OTHERWISE is extracted if present

Expr Tran::caseAlts(Expr alts, Expr& sharedExprs, Bool& hasDefault, Expr& otherwise) {
   if (alts == Nil)
      return Nil;
   else {
      Expr alt = hd(alts);
      Bool shared = FALSE;
      if (length(fst(alt)) > 1) {
         Name name = newName();
         shared = TRUE;
         Cell& cell_ = *new(sizeof(SharedExpr), msa()) Cell(SHARED_EXPR);
         new(cell_.body()) SharedExpr(name, snd(alt));
         snd(alt) = fromCell(cell_);
         sharedExprs = cons(fromCell(cell_), sharedExprs, msa());
      }
      for (Expr* patternPtr = &fst(alt); *patternPtr != Nil; ) {
         Expr pattern = hd(*patternPtr);

         if (pattern != Expr(OTHERWISE)) {
            Expr expr;
            if (shared) {
               SharedExpr& sharedExpr = toBody(snd(alt), SharedExpr);
               sharedExpr.sharedPattern(pattern, msa());
               Cell& cell_ = *new(sizeof(SharedPatternAp), msa()) Cell(SHARED_PATTERN_AP);
               new(cell_.body()) SharedPatternAp(sharedExpr);
               expr = fromCell(cell_);
            }
            else
               expr = snd(alt);
            hd(*patternPtr) = mkPair(cons(pattern, Nil, msa()), expr, msa());
            patternPtr = &tl(*patternPtr);
         }
         else {
            if (hasDefault)
               error("Tran::caseAlts: duplicate otherwise");
            otherwise = snd(alt);
            hasDefault = true;
            *patternPtr = tl(*patternPtr);
         }
      }
      return concat(fst(alt), caseAlts(tl(alts), sharedExprs, hasDefault, otherwise), msa());
   }
}

// Generate pattern forms

Expr Tran::doPat(Expr pattern) {
   TypeSig typeSig = UNKNOWN;

   Bool isPlainDecon = FALSE;
   Bool isPtrDecon = FALSE;

   if (isPair(pattern)) {
      if (fst(pattern) == Expr(PLAIN_PATTERN)) {
         pattern = snd(pattern);
         isPlainDecon = TRUE;
      }
      if (fst(pattern) == Expr(PTR_DECON_PATTERN)) {
         pattern = snd(pattern);
         isPtrDecon = TRUE;
      }
   }

   if (isTriple(pattern) && fst3(pattern) == Expr(CONSTRAIN)) {
      typeSig = thd3(pattern);
      pattern = snd3(pattern);
   }
 
   if (constPattern(pattern)) {
      Cell& cell_ = *new(sizeof(ConstPattern), msa()) Cell(CONST_PATTERN);
      new(cell_.body()) ConstPattern(pattern, fromName(useName("(=)")), typeSig);
      return fromCell(cell_);
   }
   else if (varPattern(pattern)) {
      Cell& cell_ = *new(sizeof(VarPattern), msa()) Cell(VAR_PATTERN);
      new(cell_.body()) VarPattern(pattern, typeSig, msa());
      return fromCell(cell_);
   }
   else if (wildCardPattern(pattern)) {
      Cell& cell_ = *new(sizeof(WildCardPattern), msa()) Cell(WILDCARD_PATTERN);
      new(cell_.body()) WildCardPattern(typeSig);
      return fromCell(cell_);
   }
   else if (dataConPattern(pattern)) {
      if (isPair(pattern) ||
          isTriple(pattern) && fst3(pattern) == Expr(DATA_CON_PATTERN)) {
         Cell& cell_ = *new(sizeof(DataConPattern), msa()) Cell(DATA_CON_PATTERN);
         if (isPair(pattern)) {
            UInt card = length(snd(pattern));
            new(cell_.body()) DataConPattern(TUPLE_MIN + (card - 2) * ENUM_VAL_INCR, snd(pattern),
                                             isPlainDecon, isPtrDecon,
                                             typeSig, *this);
         } else
            new(cell_.body()) DataConPattern(snd3(pattern), thd3(pattern),
                                             isPlainDecon, isPtrDecon,
                                             typeSig, *this);
         return fromCell(cell_);
      } else {
         Cell& cell_ = *new(sizeof(ConcretePattern), msa()) Cell(CONCRETE_PATTERN);
         new(cell_.body()) ConcretePattern(snd3(pattern), thd3(pattern),
                                           isPlainDecon, isPtrDecon,
                                           typeSig, *this);
         return fromCell(cell_);
      }
   }

   return (Expr)ERROR;
}

static Void transformAnyAlt(AppLambda& appLambda, Tran& tran) {
   Expr patterns = appLambda.patterns();
   if (patterns == Nil || !isTriple(hd(patterns)) ||
       fst3(hd(patterns)) != Expr(DATA_CON_PATTERN))
      return;

   Expr anyPattern = hd(patterns);
   if (snd3(anyPattern) != fromName(tran.useName("Any")) ||
       length(thd3(anyPattern)) != 1)
      return;

   Expr pat = hd(thd3(anyPattern));
   TypeSig patTypeSig = fromName(newName());
   Expr ptrX = fromName(newName());
   Expr anyReprPattern =
      mkPair(TUPLE_PATTERN,
             cons(mkPair(TYPE_SIG, patTypeSig, tran.msa()),
                  cons(ptrX, Nil, tran.msa()),
                  tran.msa()),
             tran.msa());

   hd(patterns) =
      mkTriple(CONCRETE_PATTERN,
               snd3(anyPattern),
               cons(anyReprPattern, Nil, tran.msa()),
               tran.msa());

   TypeSig castPtrTypeSig =
      ap2(ARROW,
          ap(fromName(tran.useName("Ptr")),
             fromName(tran.useName("Extant")),
             tran.msa()),
          ap(fromName(tran.useName("Ptr")),
             patTypeSig,
             tran.msa()),
          tran.msa());
   Expr castPtr =
      mkTriple(CONSTRAIN,
               fromName(tran.useName("castPtr")),
               castPtrTypeSig,
               tran.msa());
   Expr castPtrX =
      tran.reduce(ap(castPtr, ptrX, tran.msa()));
   Expr ptrPat =
      mkTriple(DATA_CON_PATTERN,
               fromName(tran.useName("Ptr")),
               cons(pat, Nil, tran.msa()),
               tran.msa());
   Expr ptrAlt =
      mkPair(cons(ptrPat, Nil, tran.msa()),
              appLambda.body(),
              tran.msa());

   appLambda.body() =
      tran.reduce(mkTriple(CASE,
                           castPtrX,
                           cons(ptrAlt, Nil, tran.msa()),
                           tran.msa()));
}

// Transform case alternative patterns

Expr Tran::doCaseAlt(Expr alt, Bool castFlag) {
	Cell& cell_ = *new(sizeof(AppLambda), msa()) Cell(APP_LAMBDA);
	new(cell_.body()) AppLambda(fst(alt), snd(alt), fromName(useName("cast")), castFlag);
   transformAnyAlt(cellBody(cell_, AppLambda), *this);
   mapOver(doPat, cellBody(cell_, AppLambda).patterns());
   return fromCell(cell_);
}

// Transform patterns in case alternatives

Expr Tran::doCaseAlts(Expr alts, Bool castFlag) {
   mapOver1(doCaseAlt, alts, castFlag);
   return alts;
}

// case e of  { p1 -> e1 ... pn -> en } =>
// (\ [p1] -> e1) [s(e)] [|] ... (\ [pn] -> en) [s(e)]

Expr Tran::tranCase(Expr expr, Expr alts) {
   Bool hasDefault = FALSE;
   Expr otherwise;
   Expr sharedExprs = Nil;

   Bool castFlag = coerceToApply(expr, FALSE);

	alts = doCaseAlts(caseAlts(alts, sharedExprs, hasDefault, otherwise), castFlag);

// Transform shared alternatives to outer level 'let'

   while (sharedExprs != Nil) {
      SharedExpr& sharedExpr = toBody(hd(sharedExprs), SharedExpr);
      expr = mkPair(LET,
                    mkTriple(mkPair(STRICT, fromName(sharedExpr._id), msa()),
                             hd(sharedExprs),
                             expr, msa()),
                    msa());
      sharedExprs = tl(sharedExprs);
   }  

// The use of a 'let' expression preserves non-strict semantics for case
// Later reduction may eliminate it
   Expr idName = fromName(newName());
   return simpleLet(idName,
                    expr,
                    tranDefault(fatBarSeq(alts,
                                          cons(idName, Nil, msa())),
                                hasDefault,
                                otherwise));
}

// Tran::typedValBinding: return a NameTypedValBinding cell

Expr Tran::nameTypedValBinding(Name name, Expr val, TypeSig typeSig) {
	Cell& cell_ = *new(sizeof(NameTypedValBinding), msa()) Cell(NAME_TYPED_VAL_BINDING);
	new(cell_.body()) NameTypedValBinding(name, val, typeSig, msa());
	return fromCell(cell_);
}

// Tran::typedValBinding: return a NameTypedValBinding cell

Expr Tran::nameTypedValBinding(Expr signedId, Expr val) {
   Name name;
   TypeSig typeSig;
   idForm(signedId, name, typeSig);
	Cell& cell_ = *new(sizeof(NameTypedValBinding), msa()) Cell(NAME_TYPED_VAL_BINDING);
	new(cell_.body()) NameTypedValBinding(name, val, typeSig, msa());
	return fromCell(cell_);
}

// tranDeclOrDefn: Translate basic declaration and definition forms

Void Tran::tranDeclOrDefn(Expr& declOrDefn, Bool outermost, Bool isInline, Bool exclusive) {
   Bool isPrimitive = FALSE;
   if (isPair(declOrDefn)) {
      if (fst(declOrDefn) == Expr(INLINE)) {
         isInline = TRUE;
         declOrDefn = snd(declOrDefn);
      }
      else if (fst(declOrDefn) == Expr(PRIMITIVE)) {
         isPrimitive = TRUE;
         declOrDefn = snd(declOrDefn);
      }
	}
	if (isPair(declOrDefn)) {
		assert(formOf(fst(declOrDefn)) == DECL, "Tran::tranDeclOrDefn: expected 'DECL'");
      Cell& cell_ = *new(sizeof(Decl), msa()) Cell(DECL);
      new(cell_.body()) Decl(outermost);
		declOrDefn = nameTypedValBinding(snd(declOrDefn), fromCell(cell_));
	} else if (isTriple(declOrDefn)) {
		assert(formOf(fst3(declOrDefn)) == DEFN, "Tran::tranDeclOrDefn: expected 'DEFN'");
		declOrDefn = defn(snd3(declOrDefn), thd3(declOrDefn), outermost, FALSE, isInline, exclusive);
	}
}

// Tran::tranDeclOrDefns: basic translation of list of declarations

Void Tran::tranDeclOrDefns(Expr declOrDefns, Bool outermost) {
   mapProc3(tranDeclOrDefn, declOrDefns, outermost, FALSE, FALSE);
}

Void Tran::addBinding(Expr declOrDefn, BindingSet& bindingEnv) {
   assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "Tran::addBinding: expected binding");
   NameTypedValBinding& binding = toBody(declOrDefn, NameTypedValBinding);
   TypedVal* typedVal = bindingEnv.lookUp(binding.name(), FALSE, FALSE);

#ifdef TRACE
   if (traceFlag) {
      outStream << "Tran::addBinding: ";
      binding.print(FALSE, outStream, env());
      outStream << '\n';
      outStream.flush();
   }
#endif

   if (typedVal != NULL) {
      Bool inconsistent = FALSE;
      switch (formOf(binding.val().val())) {
         case DECL:
            switch (formOf(typedVal->val())) {
                case DECL:
                case DEFN:
                  toBody(binding.val().val(), Decl)._prevDeclOrDefn= typedVal;
                  break;
               default:
                  inconsistent = TRUE;
                  break;
            }
            break;

         case DEFN:
            switch (formOf(typedVal->val())) {
               case BUILT_IN_DATA_CON:
               case DATA_CON:
               case DECL:
                  toBody(binding.val().val(), Defn)._prevDeclOrDefn = typedVal;
                  break;
               case DEFN: {
                  if (!toBody(typedVal->val(), Defn)._overridden) {
                     Char buf[256];
                     sprintf(buf, "Tran::addBinding: Duplicate definition of %s",
                        binding.nameString(env()));
                     error(buf);
                     return;
                  }
                  toBody(binding.val().val(), Defn)._prevDeclOrDefn =
                     toBody(typedVal->val(), Defn)._prevDeclOrDefn;
                  break;

               }
               default:
                  inconsistent = TRUE;
                  break;
            }
            break;

         default:
            inconsistent = TRUE;
            break;
      }

      if (inconsistent) {
         Char buf[256];
         sprintf(buf, "Tran::addBinding: Inconsistent with previous declaration or definition of %s",
            binding.nameString(env()));
         printTranExpr(binding.val().val(), FALSE, outStream, env());
         printTranExpr(typedVal->val(), FALSE, outStream, env());
         error(buf);
         return;
      }
   }
   bindingEnv.add(binding);
}


// Tran::addBindings: Add bindings to environment

Void Tran::addBindings(Expr declOrDefns, BindingSet& bindingEnv) {
   mapProc1(addBinding, declOrDefns, bindingEnv);
}

// Tran::tranDeclOrDefns: Translate a declaration or definition binding

Void Tran::tranDeclOrDefn(Expr declOrDefn, BindingSet& bindingEnv) {
	assert(formOf(declOrDefn) == NAME_TYPED_VAL_BINDING, "Tran::tranDeclOrDefn: expected binding");
	toBody(declOrDefn, NameTypedValBinding).tran(bindingEnv, *this, FALSE);
}

// Tran::tranDeclOrDefns: Translate a list of declaration or definition bindings

Void Tran::tranDeclOrDefns(Expr declOrDefns, BindingSet& bindingEnv) {
   mapProc1(tranDeclOrDefn, declOrDefns, bindingEnv);
}

// Tran::tranDefnCmd: Translate a 'def' command
/*
Expr Tran::tranDefnCmd(Expr defn) const {
    return ap2(fromName(useName("addBinding")),
               ap2(fromName(useName("Bind")), arg(fst3(defn)), thd3(defn), msa()),
               arg(fun(fst3(defn))), msa());
}
*/
Expr Tran::tranDefnCmd(Expr def) const {
   return reduce(ap(reduce(ap(reduce(ap(fromName(useName("addBinding")),
                                        fun(fst3(def)), msa())),
                              arg(fst3(def)), msa())),
                           reduce(ap(fromName(useName("Any")),
                                  thd3(def), msa())), msa()));
}

// Tran::tranUndefnCmd: Translate an 'undef' command

Expr Tran::tranUndefnCmd(Expr undefn) const {
   return ap2(fromName(builtInName(removeVar)), arg(undefn),
              arg(fun(undefn)), msa());
}

Void Tran::tranReduce(Expr& pair) {
   Expr s = snd(pair);
   Bool repeat;

   Bool firstTime = TRUE;
   do {

#ifdef TRACE
      static UInt probe = 0;
      UInt localProbe = probe;
      if (++probe == reduceProbeSeqNo) {
         outStream << "***** Tran::tranReduce: probe hit\n";
         traceFlag = TRUE;
      }

      if (traceFlag && firstTime) {
         outStream << "Tran::tranReduce (" << probe << "):\n";
         outStream << "================\n";
         printTranExpr(s, FALSE, outStream, env(), TRUE);
         outStream << endl;
      }
#endif

      repeat = FALSE;
      if (isPair(s)) {
         if (fst(s) == Expr(REDUCE)) {
            Expr sndSnd = snd(s);
            switch (formOf(sndSnd)) {
               case LET: {
                  Let& let = toBody(sndSnd, Let);
                  if (let.reduce(sndSnd, *this)) {
                     s = sndSnd;
                     tranReduce(s);
                     return;
                  }

                  repeat = let.innerReduce(*this);
                  if (!repeat)
                     return;

                  break;
               }
               case FN_AP:  
               case FAT_BAR_SEQ:
               case CASE:
                  repeat = toBody(sndSnd, AST_Node).innerReduce(*this);
                  break;

               default:
                  break;
            }
         }
      } else
         switch (formOf(s)) {
            case FN_AP:
            case LAMBDA:
            case LET:
               repeat = toBody(s, AST_Node).reduce(s, *this);
               break;

            case NAME_OCC:
               if (toBody(s, NameOcc).val() == dataCon$Ptr) // Ptr is built-in
                  pair = PTR_CONSTRUCTOR;
               break;

            default:
               break;
         }

      if (repeat) {

#ifdef TRACE
         if (traceFlag) {
            outStream << "Tran::tranReduce result\n";
            outStream << "=======================\n";
            printTranExpr(s, FALSE, outStream, env(), TRUE);
            outStream << '\n';
            outStream.flush();
         }
#endif

         pair = s;
         if (isPair(pair) && fst(pair) == Expr(REDUCE))
            s = snd(pair);
         else
            repeat = FALSE;
         firstTime = FALSE;
      }
   } while (repeat);
}

// Tran::tranClosure: Construct a closure for an expression value
// to be transferred.
//
// e => #!(fromThunk #!(\ -> #!e))

Void Tran::tranClosure(Expr& expr, TypeSig typeSig, Bool updatable, Bool force/* = FALSE*/) {
	if (!force && isPair(expr) && fst(expr) == Expr(REDUCE))
		return;

	switch (formOf(expr)) {
		case DEFN:
			tranClosure(toBody(expr, Defn)._expr, typeSig, updatable, force);
         return;

		case CLASS_METHOD_DEFN:
			tranClosure(toBody(expr, ClassMethodDefn).expr(), typeSig, updatable, force);
         return;

      case INSTANCE_METHOD_DEFN:
         tranClosure(toBody(expr, InstanceMethodDefn).expr(), typeSig, updatable, force);
         return;

		case FN_AP:
      case LAMBDA:
		case LET:
		case CASE:
			break;

		default:
			break;
	}

#ifdef TRACE
   if (traceFlag) {
      outStream << "Tran::tranClosure: ";
      printTranExpr(expr, FALSE, outStream, env(), TRUE);
      outStream << " :: ";
	   printTypeSig(typeSig, 0, FALSE, outStream, env());
      outStream << endl;
   }
#endif

   TypeSig tTypeSig = thunkTypeSig(denotedTypeSig(typeSig), msa());
   Cell& cell_ = *new(sizeof(Lambda), msa()) Cell(LAMBDA);
   new(cell_.body()) Lambda(0, NULL, reduce(expr), tTypeSig, updatable);
   Lambda& lambda = cellBody(cell_, Lambda);
   Expr fn = fromName(useName("fromThunk"));
   Tran(1, "", env()).tran(fn);
   Expr faExp = fnAp(fn,
                     reduce(fromCell(cell_)),
                     arrowTypeSig(tTypeSig, typeSig, msa()));
   FnAp& fa = toBody(faExp, FnAp);
   expr = reduce(faExp);
}

// Tran::tranPair: Translate a pair form

Void Tran::tranPair(Expr& pair, BindingSet& bindingEnv) {
   Expr& f = fst(pair);
   Expr& s = snd(pair);
//   Bool suppressEval = FALSE;
   switch (formOf(f)) {
      case MODULE: {
         Cell& cell_ = *new(sizeof(ModuleDefn), msa()) Cell(MODULE);
         new(cell_.body()) ModuleDefn(toName(fst(s)), snd(s), _env);
         cellBody(cell_, ModuleDefn).tran(bindingEnv, *this);
         pair = fromCell(cell_);
         return;
      }

      case ORDER: {
         Cell& cell_ = *new(sizeof(Order), msa()) Cell(ORDER);
         new(cell_.body()) Order(s, *this);
         cellBody(cell_, Order).tran(bindingEnv, *this);
         pair = fromCell(cell_);
         return;
      }

      case NO_COERCE:
      case NO_MAP_TO_ENV:
         pair = s;      // strip out any remaining suppresion directives
         break;

      case MAP_TO_ENV: {
         pair = mapToEnv(s, fromName(useName("consEnv")));
         break;
      }

      case TYPE_SIG:
         if (pass() > 1)
            Subst::substTypeVars(s, substs());
         return;

      case REDUCE:
         switch (formOf(s)) {
            case INT:  // Drop through
            case CHAR:
               f = LITERAL;
               break;

            default:
               tranExpr(s, bindingEnv);
               if (pass() > 1)
                  tranReduce(pair);
               break;
         }
         return;

      case SHARED:
         pair = s;
         break;

      case COERCE:
         pair = coerce(s, TRUE);
         break;

      case LITERAL:
         if (isPair(s) && fst(s) == Expr(TYPE_SIG))
            tranPair(s, bindingEnv);
         return;

      case CONCRETE_DATA_CON: {
         Cell& cell_ = *new(sizeof(ConcreteDataCon), msa()) Cell(CONCRETE_DATA_CON);
         new(cell_.body()) ConcreteDataCon(s);
         pair = fromCell(cell_);
         cellBody(cell_, ConcreteDataCon).tran(pair, bindingEnv, *this);
         return;
      }

      case SEQUENCE:
         pair = tranSequenceList(s, FALSE);
         break;

      case IF:

//------ if ec then e1 else e2 => case ec of { True -> e1; False -> e2 }
//------ if ec then e =>          case ec of { True -> e;  False -> Void}

         pair = isTriple(s)
            ? tranCase(fst3(s),
                       cons(makeDataConAlt(fromName(useName("True")), snd3(s)),
                            cons(makeDataConAlt(fromName(useName("False")), thd3(s)), Nil, msa()), msa()))
			   : tranCase(fst(s),
                       cons(makeDataConAlt(fromName(useName("True")), snd(s)),
                            cons(makeDataConAlt(fromName(useName("False")), VOID), Nil, msa()), msa()));
         break;

      default:
         tranExpr(f, bindingEnv);
         tranExpr(s, bindingEnv);
         return;
   }
   tranExpr(pair, bindingEnv);
}

// Tran::tranTriple: Translate a triple form

Void Tran::tranTriple(Expr& triple, BindingSet& bindingEnv) {
   Expr& f = fst3(triple);
   Expr& s = snd3(triple);
   Expr& t = thd3(triple);

	ExprRepr formOfFst3 = formOf(f);
   switch (formOfFst3) {
      case MAP_TO_ENV:
         triple = mapToEnv(s, reduce(fnAp(fromName(useName("envOf")), t)));
         break;

      case LAMBDA:
         triple = lambda(s, t);
         break;
 
		case LET:
         triple = let(s, t);
         break;

		case LET_REC:
         triple = let(s, t, TRUE);
         break;

      case CASE:
         triple = tranCase(s, t);
         break;

      case CONSTRAIN:
      case DISPATCH:
			tranExpr(s, bindingEnv);
         if (pass() > 1)
            triple = s;
			return;

      case SELECT:
      case SEL_PTR:
         return;

      case FAT_BAR_SEQ:
         triple = fatBarSeq(t, s);
         break;

      default:
         tranExpr(f, bindingEnv);
         tranExpr(s, bindingEnv);
         tranExpr(t, bindingEnv);
         return;
   }
   tranExpr(triple, bindingEnv);
}

// Tran::tranSequenceList: Translate a list of sequential expressions

Expr Tran::tranSequenceList(Expr list, Bool first) {
   if (list == Nil)
      return VOID;
   else {
      Expr expr = hd(list);
      if (tl(list) != Nil)
         return reduce(fnAp2(SEQ, expr, tranSequenceList(tl(list), FALSE)));
      else {
         return !first
            ? coerce(expr)
            : expr;
      }
   }
}

// ((.) x C) expr) => #!(C expr) x)


// Tran::tranAp: Translate a function application

static Bool isConstructorName(Expr expr, const Tran& tran) {
   return isName(expr) &&
          isupper(nameString_(toName(expr), tran.nameTable())[0]);
}

Void Tran::tranAp(Expr& expr, Expr f, Expr a, BindingSet& bindingEnv) {
   if (isPair(f) &&
      fst(f) == Expr(REDUCE) &&
      isAp(snd(f)) &&
      isAp(fun(snd(f))) &&
      fun(fun(snd(f))) == fromName(builtInName$("(.)")) &&
      isConstructorName(arg(snd(f)), *this)) {
      expr = ap(reduce(ap(reduce(arg(snd(f))),
                          a, msa())),
                arg(fun(snd(f))), msa());
      tranExpr(expr, bindingEnv);
   } else {
      expr = fnAp(f, a);
      toBody(expr, FnAp).tran(expr, bindingEnv, *this);
   }
}

// Tran::tranExpr: Translate an expression

Void Tran::tranExpr(Expr& expr, BindingSet& bindingEnv) {

#ifdef TRACE
	static UInt probe = 0;
   UInt localProbe = probe;

   ExprRepr forms[] = {DECL, DEFN, CLASS_METHOD_DECL, CLASS_METHOD_DEFN, INSTANCE_METHOD_DECL, INSTANCE_METHOD_DEFN };
	if (!formExistsIn(expr, sizeof(forms) / sizeof(ExprRepr), forms)) {
      if (++probe == tranProbeSeqNo + 1) {
	      outStream << "***** Tran::tranExpr: probe hit\n";
         traceFlag = TRUE;
      }

      if (traceFlag) {
         outStream << "Tran::tranExpr(" << localProbe << "): ";
	      printTranExpr(expr, FALSE, outStream, env(), TRUE);
         outStream << endl;
      }
   }
#endif

   if (expr == (Expr)ERROR)
      _nErrors++;

   if (isPair(expr)) {
      tranPair(expr, bindingEnv);
   } else if (isTriple(expr))
      tranTriple(expr, bindingEnv);
   else
      switch (formOf(expr)) {
         case MODULE:
            toBody(expr, ModuleDefn).tran(bindingEnv, *this);
            break;

         case ORDER:
            toBody(expr, Order).tran(bindingEnv, *this);
            break;

         case SNIPPET:
            toBody(expr, Snippet).tran(bindingEnv, *this);
            break;

         case AP:
            tranAp(expr, fun(expr), arg(expr), bindingEnv);
            break;
 
         case NAME: {
            Cell& cell_ = *new(sizeof(NameOcc), msa()) Cell(NAME_OCC);
            new(cell_.body()) NameOcc(toName(expr));
            expr = fromCell(cell_);
            cellBody(cell_, NameOcc).tran(expr, bindingEnv, *this);
            break;
         }
  
         case CONS:
            tranCons(expr, bindingEnv);
            break;

			case CONST_PATTERN:
			case VAR_PATTERN:
			case WILDCARD_PATTERN:
			case DATA_CON_PATTERN:
         case CONCRETE_PATTERN:
         case DATA_CON_DEFN:
         case CONCRETE_DATA_CON:
			case DECL:
			case DEFN:
			case CLASS_METHOD_DECL:
			case CLASS_METHOD_DEFN:
         case INSTANCE_METHOD_DECL:
         case INSTANCE_METHOD_DEFN:
			case LAMBDA:
         case LET:
         case NAME_OCC:
         case FN_AP:
         case CASE:
         case APP_LAMBDA:
         case FAT_BAR_SEQ:
         case SHARED_EXPR:
         case SHARED_PATTERN_AP:
         case COERCE:
            if (isPtr(expr))
               toBody(expr, AST_Node).tran(expr, bindingEnv, *this);
            break;

         default:
            break;
      }

#ifdef TRACE
   if (traceFlag) {
      ExprRepr forms[] = {DECL, DEFN, CLASS_METHOD_DECL, CLASS_METHOD_DEFN, INSTANCE_METHOD_DECL, INSTANCE_METHOD_DEFN };
		if (!formExistsIn(expr, sizeof(forms) / sizeof(ExprRepr), forms)) {
         outStream << "Tran::tranExpr(" << localProbe << ") result: ";
         printTranExpr(expr, FALSE, outStream, env(), TRUE);
         outStream << endl;
      }
   }
#endif

}

// Tran::tran: Translate an expression with the given set of bindings

Bool Tran::tran(Expr& expr, BindingSet* bindingEnv /* = NULL*/) {
   BindingSet localBindingEnv = BindingSet(bindingEnv);

   NameTypedValBinding* consEnvBinding = new (msa())
      NameTypedValBinding(useName("consEnv"), CONS_ENV, typeCon(Env), msa());
   ((BindingSet*)(void*)(&_builtInBindingSet))->add(*consEnvBinding);
   tranExpr(expr, localBindingEnv);
   return _nErrors == 0;
}

// Tran::error: Increment the error count and report an error

Void Tran::error(const char* s) {
   _nErrors++;
   ivoryError(s);
}

// Tran::warn: Increment the warning count and report a warning

Void Tran::warn(const char* s) {
   _nWarnings++;
   ivoryTrace(s);
}

/*
Useful debugging code snippets

   printTranExpr(e, FALSE, outStream, env());
   outStream << '\n';
   outStream.flush();

   printTranExpr(e, FALSE, outStream, tran.env());
   outStream << '\n';
   outStream.flush();

	printTypeSig(_typeSig, 0, FALSE, outStream, tran.env());
   outStream << '\n';
   outStream.flush();
*/
