/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Skeleton implementation for Bison GLR parsers in C

   Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C GLR parser skeleton written by Paul Hilfinger.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "glr.c"

/* Pure parsers.  */
#define YYPURE 0






/* First part of user prologue.  */

/* YACC grammar for the language Ivory
 *
 * This file is best viewed with tabs set to 6
 *
 * (c) Copyright (see Date) by Alasdair Scott and Hugh Glaser
 *
 * Date: 2000
 *
 * The commented line YYDEBUG 1 may be used to trace the parser state machine (stderr).
 */

#define YYERROR_VERBOSE
#define YYDEBUG 0
//#define YYDEBUG 1
#include <stdio.h>

#define report fprintf
extern int report(FILE* f, const char* format, ...);
#define YYFPRINTF report

extern char* yytext;
extern int yylex(void);
extern int yylineno;
extern char string_buf[];

#define yyerror(mess) outStream << "Near line " << yylineno << ' ' << mess << '\n';

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#ifndef MINIMAL_IOSTREAM
   #include <sstream>
   using namespace std;
#endif 
#include "ivory/common.h"
#include "ivory/repr.h"
#include "ivory/binding.h"
#include "ivory/bits.h"
#include "ivory/int.h"
#include "ivory/double.h"
#include "ivory/list.h"
#include "ivory/nameExprMap.h"
#include "ivory/ref.h"
#include "ivory/root.h"
#include "ivory/string.h"
#include "ivory/tuple.h"
#include "ivory/trace.h"
#include "ivory/compiler/parser.h"

declareTypeCon(Any);
declareTypeCon(Bits);
declareTypeCon(Int);
declareTypeCon(Double);

Expr result;
Name name;
Expr script;

static NameTable* _nameTable;
static MSA* _msa;
static int _errCount;

#define l_result (result.repr())
#define l_char(c) (fromChar(c).repr())
#define l_int(i) (fromInt(i,*_msa).repr())
#define l_bits(b) (fromBits(b,*_msa).repr())
#define l_double(d) (fromDouble(d,*_msa).repr())
#define l_string(s) (fromString(s,*_msa).repr())
#define l_ref(r) (fromRef(r).repr())
#define l_pair(fst,snd) (Expr(mkPair((fst),(snd),*_msa)).repr())
#define l_fst(pair) (fst(Expr(pair)).repr())
#define l_snd(pair) (snd(Expr(pair)).repr())
#define l_triple(fst3,snd3,thd3) (Expr(mkTriple((fst3),(snd3),(thd3),*_msa)).repr())
#define l_useName(name) (fromName(useName((name),*_nameTable)).repr())
#define l_coerce(expr) (coerce((expr),FALSE,FALSE).repr())
#define l_forceCoerce(expr) (coerce((expr),TRUE,FALSE).repr())
#define l_noCoerce(expr) (noCoerce((expr)).repr())
#define l_constrain(expr) (constrain(expr, 0).repr())
#define l_reduce(expr) (reduce(expr).repr())
#define l_eval(expr) (eval(expr).repr())
#define l_ap(f,arg) (Expr(ap((f),(arg),(*_msa))).repr())
#define l_ap2(f,arg1,arg2) (Expr(ap2((f),(arg1),(arg2),(*_msa))).repr())
#define l_ap3(f,arg1,arg2,arg3) (Expr(ap3((f),(arg1),(arg2),(arg3),(*_msa))).repr())
#define l_fun(ap) (fun(Expr(ap)).repr())
#define l_arg(ap) (arg(Expr(ap)).repr())
#define l_monAp(f,arg) (l_ap((f),l_coerce(arg)))
#define l_monApNoCoerce(f,arg) (l_ap((f),(arg)))
#define l_binAp(f,arg1,arg2) (l_ap(l_reduce(l_ap(f,l_coerce(arg1))),l_coerce(arg2)))
#define l_genAp(f,arg) (genAp((f),(arg)).repr())
#define l_mapToConsEnv(x) (mapToConsEnv((x),FALSE).repr())
#define l_mapToDstEnv(src,dst) (mapToDstEnv((src),(dst),FALSE).repr())
#define l_dataCon(dataCon) (genDataCon(dataCon).repr())
#define l_apTuple(comps,card,mapComps) (apTuple((comps),(card),(mapComps)).repr())
#define l_sequence(list) (sequence(list,TRUE))
#define l_Nil  (Nil.repr())
#define l_Ptr (l_useName("Ptr"))
#define l_Plain (l_useName("Plain"))
#define l_Any (l_useName("Any"))
#define l_Bind (l_useName("Bind"))
#define l_Cons (l_useName("Cons"))
#define l_cons(x,xs) (Expr(cons((x),(xs),*_msa)).repr())
#define l_concat(xs,ys) (concat((xs),(ys),*_msa).repr())
#define l_bind(n,v) (bind((n),(v),*_msa).repr())
#define l_root (l_useName("root"))

#define pp(off,on) lex_pop(off);lex_push(on)

static void parserError(void);

// Coerce an expression
// Consumes multiple coercions and is suppressed for explicit reduction

static Expr coerce(Expr x, Bool force, Bool suppress) {
   return isPair(x) && fst(x) == Expr(NO_COERCE)
      ? coerce(snd(x), force, TRUE)
      : (isPair(x) && fst(x) == Expr(COERCE)
         ? coerce(snd(x), TRUE, suppress)
         : (!force && (suppress ||
            isPair(x) && (fst(x) == Expr(REDUCE) ||
                          fst(x) == Expr(NO_MAP_TO_ENV) ||
                          fst(x) == Expr(LITERAL)))
            ? x
            : Expr(l_pair(COERCE, x))));
}


// Apply NO_COERCE to an expression

static Expr noCoerce(Expr x) {
   return Expr(l_pair(NO_COERCE, x));
}

// Applies constraint coercion unless explicit suppression

static Expr constrain(Expr x, UInt reduceCount) {
   return isPair(x) && fst(x) == Expr(NO_COERCE)
      ? constrain(snd(x), reduceCount + 1)
      : (reduceCount < 2
           ? Expr(l_pair(COERCE, x))
           : x);
}

// Reduce an expression
// Consumes any coercion suppression

static Expr reduce(Expr x) {
   return isPair(x) && fst(x) == Expr(NO_COERCE)
      ? reduce(snd(x))
      : Expr(l_pair(REDUCE, x));
}

// Evaluate an expression

static Expr eval(Expr x) {
   return isPair(x) && fst(x) == Expr(NO_COERCE)
      ? eval(snd(x))
      : Expr(reduce(l_ap((l_useName("eval")), x)));
}

// Similarly for mapping to constructor or destination environment
// Consumes multiple suppressions

static Expr mapToConsEnv(Expr x, Bool suppress) {
   return isPair(x) && fst(x) == Expr(NO_MAP_TO_ENV)
      ? mapToConsEnv(snd(x), TRUE)
      : (suppress
         ? x
         : Expr(l_pair(MAP_TO_ENV, x)));
}

static Expr mapToDstEnv(Expr src, Expr dst, Bool suppress) {
   return isPair(src) && fst(src) == Expr(NO_MAP_TO_ENV)
      ? mapToDstEnv(snd(src), dst, TRUE)
      : (suppress
         ? src
         : Expr(l_triple(MAP_TO_ENV, src, dst)));
}

static Expr defn(Expr patterns, Expr expr) {
   return patterns == Nil ? l_coerce(expr)
                          : l_pair(REDUCE, l_triple(LAMBDA, patterns, l_coerce(expr)));
}

static Expr genAp(Expr f, Expr arg) {
   return l_ap(isAp(f) ? l_reduce(f)
                       : l_coerce(f),
               l_coerce(arg));
}

static Expr genDataCon(String str) {
   return l_useName(str);
}

static Expr apTuple(Expr comps, UInt card, Bool mapAndReduce) {
   Expr comp = mapAndReduce ? Expr(l_mapToConsEnv(hd(comps))) : hd(comps);
   if (tl(comps) != Nil) {
      Expr f = apTuple(tl(comps), card + 1, mapAndReduce);
      return Expr(l_ap(mapAndReduce ? Expr(l_reduce(f)) : f, comp));
   } else
      return Expr(l_ap(TUPLE_MIN + ((card - 1) * TAG_INCR), comp));
}

static Expr sequence(Expr list, Bool first) {
   if (list == Nil)
      return VOID;
   else {
      Expr expr = hd(list);
      if (tl(list) != Nil) {
         expr = l_ap(l_reduce(l_ap(SEQ, l_coerce(expr))), sequence(tl(list), FALSE));
         return first 
            ? expr
            : Expr(l_reduce(expr));
      } else
         return first
            ? expr
            : Expr(l_coerce(expr));
   }
}



# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 10110,               /* "invalid token"  */
    L_MODULE = 10001,              /* L_MODULE  */
    L_DEF = 10002,                 /* L_DEF  */
    L_WITH = 10003,                /* L_WITH  */
    L_WHERE = 10004,               /* L_WHERE  */
    L_UNDEF = 10005,               /* L_UNDEF  */
    L_DO = 10006,                  /* L_DO  */
    L_IF = 10007,                  /* L_IF  */
    L_THEN = 10008,                /* L_THEN  */
    L_ELSE = 10009,                /* L_ELSE  */
    L_CASE = 10010,                /* L_CASE  */
    L_OF = 10011,                  /* L_OF  */
    L_OTHERWISE = 10012,           /* L_OTHERWISE  */
    L_LET = 10013,                 /* L_LET  */
    L_IN = 10014,                  /* L_IN  */
    L_RETURN = 10015,              /* L_RETURN  */
    L_THIS = 10016,                /* L_THIS  */
    L_INLINE = 10017,              /* L_INLINE  */
    L_TAG = 10018,                 /* L_TAG  */
    L_SEMICOLON = 10019,           /* L_SEMICOLON  */
    L_LSQB = 10020,                /* L_LSQB  */
    L_RSQB = 10021,                /* L_RSQB  */
    L_LPAREN = 10022,              /* L_LPAREN  */
    L_RPAREN = 10023,              /* L_RPAREN  */
    L_LCURLYB = 10024,             /* L_LCURLYB  */
    L_CMD_LCURLYB = 10025,         /* L_CMD_LCURLYB  */
    L_RCURLYB = 10026,             /* L_RCURLYB  */
    L_BACKSLASH = 10027,           /* L_BACKSLASH  */
    L_COLON_EQ = 10028,            /* L_COLON_EQ  */
    L_AT_EQ = 10029,               /* L_AT_EQ  */
    L_LT_HYPHEN = 10030,           /* L_LT_HYPHEN  */
    L_EXCLMARK = 10031,            /* L_EXCLMARK  */
    L_NUMSIGN_EXCLMARK = 10032,    /* L_NUMSIGN_EXCLMARK  */
    L_COMMA = 10033,               /* L_COMMA  */
    L_UNDERSCORE = 10034,          /* L_UNDERSCORE  */
    L_COLON_COLON = 10035,         /* L_COLON_COLON  */
    L_PREFIX_COLON_COLON = 10036,  /* L_PREFIX_COLON_COLON  */
    L_NOTSIGN_COLON_COLON = 10037, /* L_NOTSIGN_COLON_COLON  */
    L_AMPERSAND = 10038,           /* L_AMPERSAND  */
    L_VERTBAR = 10039,             /* L_VERTBAR  */
    L_HYPHEN_GT = 10040,           /* L_HYPHEN_GT  */
    L_TYPESIG_HYPHEN_GT = 10041,   /* L_TYPESIG_HYPHEN_GT  */
    L_PREFIX_TYPESIG_HYPHEN_GT = 10042, /* L_PREFIX_TYPESIG_HYPHEN_GT  */
    L_LT_LT = 10043,               /* L_LT_LT  */
    L_GT_GT = 10044,               /* L_GT_GT  */
    L_NOTSIGN = 10045,             /* L_NOTSIGN  */
    L_NOT = 10046,                 /* L_NOT  */
    L_AND = 10047,                 /* L_AND  */
    L_OR = 10048,                  /* L_OR  */
    L_NUMSIGN_AMPERSAND = 10049,   /* L_NUMSIGN_AMPERSAND  */
    L_NUMSIGN_VERTBAR = 10050,     /* L_NUMSIGN_VERTBAR  */
    L_NUMSIGN_CARET = 10051,       /* L_NUMSIGN_CARET  */
    L_NUMSIGN_NOTSIGN = 10052,     /* L_NUMSIGN_NOTSIGN  */
    L_LT = 10053,                  /* L_LT  */
    L_LT_EQ = 10054,               /* L_LT_EQ  */
    L_EQ = 10055,                  /* L_EQ  */
    L_DEF_EQ = 10056,              /* L_DEF_EQ  */
    L_NOTSIGN_EQ = 10057,          /* L_NOTSIGN_EQ  */
    L_GT_EQ = 10058,               /* L_GT_EQ  */
    L_GT = 10059,                  /* L_GT  */
    L_DOT = 10060,                 /* L_DOT  */
    L_PREFIX_DOT = 10061,          /* L_PREFIX_DOT  */
    L_COLON = 10062,               /* L_COLON  */
    L_COLON_PLUS = 10063,          /* L_COLON_PLUS  */
    L_PLUS_PLUS = 10064,           /* L_PLUS_PLUS  */
    L_PLUS = 10065,                /* L_PLUS  */
    L_PREFIX_PLUS = 10066,         /* L_PREFIX_PLUS  */
    L_HYPHEN = 10067,              /* L_HYPHEN  */
    L_PREFIX_HYPHEN = 10068,       /* L_PREFIX_HYPHEN  */
    L_ASTERISK = 10069,            /* L_ASTERISK  */
    L_TYPESIG_ASTERISK = 10070,    /* L_TYPESIG_ASTERISK  */
    L_SLASH = 10071,               /* L_SLASH  */
    L_DIV = 10072,                 /* L_DIV  */
    L_MOD = 10073,                 /* L_MOD  */
    L_CARET = 10074,               /* L_CARET  */
    L_PREFIX_CARET = 10075,        /* L_PREFIX_CARET  */
    L_LPAREN_CARET_RPAREN = 10076, /* L_LPAREN_CARET_RPAREN  */
    L_NOTSIGN_CARET = 10077,       /* L_NOTSIGN_CARET  */
    L_NUMSIGN = 10078,             /* L_NUMSIGN  */
    L_NUMSIGN_NUMSIGN = 10079,     /* L_NUMSIGN_NUMSIGN  */
    L_NUMSIGN_COLON_COLON = 10080, /* L_NUMSIGN_COLON_COLON  */
    L_NUMSIGN_AT = 10081,          /* L_NUMSIGN_AT  */
    L_AT = 10082,                  /* L_AT  */
    L_EQ_GT = 10083,               /* L_EQ_GT  */
    L_TILDE = 10084,               /* L_TILDE  */
    L_TILDE_TILDE = 10085,         /* L_TILDE_TILDE  */
    L_TILDE_AT = 10086,            /* L_TILDE_AT  */
    L_TYPE = 10087,                /* L_TYPE  */
    L_OBJECT = 10088,              /* L_OBJECT  */
    L_CLASS = 10089,               /* L_CLASS  */
    L_PRIMITIVE = 10090,           /* L_PRIMITIVE  */
    L_INSTANCE = 10091,            /* L_INSTANCE  */
    L_SUBORDINATE = 10092,         /* L_SUBORDINATE  */
    L_CONSTANT = 10093,            /* L_CONSTANT  */
    L_VARIABLE = 10094,            /* L_VARIABLE  */
    L_INDISPENSABLE = 10095,       /* L_INDISPENSABLE  */
    L_DYNAMIC = 10096,             /* L_DYNAMIC  */
    L_TRANSIENT = 10097,           /* L_TRANSIENT  */
    L_PERSIST = 10098,             /* L_PERSIST  */
    L_PRIVATE = 10099,             /* L_PRIVATE  */
    L_PUBLIC = 10100,              /* L_PUBLIC  */
    L_MATCHING = 10101,            /* L_MATCHING  */
    L_PTR = 10102,                 /* L_PTR  */
    L_UC_NAME = 10103,             /* L_UC_NAME  */
    L_LC_NAME = 10104,             /* L_LC_NAME  */
    L_INTEGER = 10105,             /* L_INTEGER  */
    L_HEX = 10106,                 /* L_HEX  */
    L_FIXED = 10107,               /* L_FIXED  */
    L_CHAR = 10108,                /* L_CHAR  */
    L_STRING = 10109,              /* L_STRING  */
    PREC_LET = 10111,              /* PREC_LET  */
    PREC_LAMBDA = 10112,           /* PREC_LAMBDA  */
    PREC_IF_THEN = 10113,          /* PREC_IF_THEN  */
    PREC_IF_THEN_ELSE = 10114,     /* PREC_IF_THEN_ELSE  */
    PREC_MODIFIER = 10115,         /* PREC_MODIFIER  */
    PREC_ATTRIBUTE = 10116,        /* PREC_ATTRIBUTE  */
    PREC_LOG_OR = 10117,           /* PREC_LOG_OR  */
    PREC_LOG_AND = 10118,          /* PREC_LOG_AND  */
    PREC_REL = 10119,              /* PREC_REL  */
    PREC_BIT_OR = 10120,           /* PREC_BIT_OR  */
    PREC_BIT_XOR = 10121,          /* PREC_BIT_XOR  */
    PREC_BIT_AND = 10122,          /* PREC_BIT_AND  */
    PREC_SHIFT = 10123,            /* PREC_SHIFT  */
    PREC_ADD_SUB = 10124,          /* PREC_ADD_SUB  */
    PREC_CONCAT = 10125,           /* PREC_CONCAT  */
    PREC_MUL_DIV = 10126,          /* PREC_MUL_DIV  */
    PREC_EXP = 10127,              /* PREC_EXP  */
    PREC_SEL = 10128,              /* PREC_SEL  */
    L_PLAIN = 10129,               /* L_PLAIN  */
    PREC_PLAIN_PATTERN = 10130,    /* PREC_PLAIN_PATTERN  */
    L_AT_PATTERN = 10131,          /* L_AT_PATTERN  */
    PREC_AT_PATTERN = 10132,       /* PREC_AT_PATTERN  */
    PREC_PTR = 10133,              /* PREC_PTR  */
    PREC_PTR_PATTERN = 10134,      /* PREC_PTR_PATTERN  */
    PREC_LOG_NOT = 10135,          /* PREC_LOG_NOT  */
    PREC_BITWISE_NOT = 10136,      /* PREC_BITWISE_NOT  */
    PREC_DEPTR = 10137,            /* PREC_DEPTR  */
    L_LPAREN_EXCLMARK_RPAREN = 10138, /* L_LPAREN_EXCLMARK_RPAREN  */
    L_PREFIX_NUMSIGN_COLON_COLON = 10139 /* L_PREFIX_NUMSIGN_COLON_COLON  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{

   Char     _char;
   Int      _int;
   Bits     _bits;
   Double	_double;
   String	_string;
   ExprRepr _expr;


};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);


/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_L_MODULE = 3,                   /* L_MODULE  */
  YYSYMBOL_L_DEF = 4,                      /* L_DEF  */
  YYSYMBOL_L_WITH = 5,                     /* L_WITH  */
  YYSYMBOL_L_WHERE = 6,                    /* L_WHERE  */
  YYSYMBOL_L_UNDEF = 7,                    /* L_UNDEF  */
  YYSYMBOL_L_DO = 8,                       /* L_DO  */
  YYSYMBOL_L_IF = 9,                       /* L_IF  */
  YYSYMBOL_L_THEN = 10,                    /* L_THEN  */
  YYSYMBOL_L_ELSE = 11,                    /* L_ELSE  */
  YYSYMBOL_L_CASE = 12,                    /* L_CASE  */
  YYSYMBOL_L_OF = 13,                      /* L_OF  */
  YYSYMBOL_L_OTHERWISE = 14,               /* L_OTHERWISE  */
  YYSYMBOL_L_LET = 15,                     /* L_LET  */
  YYSYMBOL_L_IN = 16,                      /* L_IN  */
  YYSYMBOL_L_RETURN = 17,                  /* L_RETURN  */
  YYSYMBOL_L_THIS = 18,                    /* L_THIS  */
  YYSYMBOL_L_INLINE = 19,                  /* L_INLINE  */
  YYSYMBOL_L_TAG = 20,                     /* L_TAG  */
  YYSYMBOL_L_SEMICOLON = 21,               /* L_SEMICOLON  */
  YYSYMBOL_L_LSQB = 22,                    /* L_LSQB  */
  YYSYMBOL_L_RSQB = 23,                    /* L_RSQB  */
  YYSYMBOL_L_LPAREN = 24,                  /* L_LPAREN  */
  YYSYMBOL_L_RPAREN = 25,                  /* L_RPAREN  */
  YYSYMBOL_L_LCURLYB = 26,                 /* L_LCURLYB  */
  YYSYMBOL_L_CMD_LCURLYB = 27,             /* L_CMD_LCURLYB  */
  YYSYMBOL_L_RCURLYB = 28,                 /* L_RCURLYB  */
  YYSYMBOL_L_BACKSLASH = 29,               /* L_BACKSLASH  */
  YYSYMBOL_L_COLON_EQ = 30,                /* L_COLON_EQ  */
  YYSYMBOL_L_AT_EQ = 31,                   /* L_AT_EQ  */
  YYSYMBOL_L_LT_HYPHEN = 32,               /* L_LT_HYPHEN  */
  YYSYMBOL_L_EXCLMARK = 33,                /* L_EXCLMARK  */
  YYSYMBOL_L_NUMSIGN_EXCLMARK = 34,        /* L_NUMSIGN_EXCLMARK  */
  YYSYMBOL_L_COMMA = 35,                   /* L_COMMA  */
  YYSYMBOL_L_UNDERSCORE = 36,              /* L_UNDERSCORE  */
  YYSYMBOL_L_COLON_COLON = 37,             /* L_COLON_COLON  */
  YYSYMBOL_L_PREFIX_COLON_COLON = 38,      /* L_PREFIX_COLON_COLON  */
  YYSYMBOL_L_NOTSIGN_COLON_COLON = 39,     /* L_NOTSIGN_COLON_COLON  */
  YYSYMBOL_L_AMPERSAND = 40,               /* L_AMPERSAND  */
  YYSYMBOL_L_VERTBAR = 41,                 /* L_VERTBAR  */
  YYSYMBOL_L_HYPHEN_GT = 42,               /* L_HYPHEN_GT  */
  YYSYMBOL_L_TYPESIG_HYPHEN_GT = 43,       /* L_TYPESIG_HYPHEN_GT  */
  YYSYMBOL_L_PREFIX_TYPESIG_HYPHEN_GT = 44, /* L_PREFIX_TYPESIG_HYPHEN_GT  */
  YYSYMBOL_L_LT_LT = 45,                   /* L_LT_LT  */
  YYSYMBOL_L_GT_GT = 46,                   /* L_GT_GT  */
  YYSYMBOL_L_NOTSIGN = 47,                 /* L_NOTSIGN  */
  YYSYMBOL_L_NOT = 48,                     /* L_NOT  */
  YYSYMBOL_L_AND = 49,                     /* L_AND  */
  YYSYMBOL_L_OR = 50,                      /* L_OR  */
  YYSYMBOL_L_NUMSIGN_AMPERSAND = 51,       /* L_NUMSIGN_AMPERSAND  */
  YYSYMBOL_L_NUMSIGN_VERTBAR = 52,         /* L_NUMSIGN_VERTBAR  */
  YYSYMBOL_L_NUMSIGN_CARET = 53,           /* L_NUMSIGN_CARET  */
  YYSYMBOL_L_NUMSIGN_NOTSIGN = 54,         /* L_NUMSIGN_NOTSIGN  */
  YYSYMBOL_L_LT = 55,                      /* L_LT  */
  YYSYMBOL_L_LT_EQ = 56,                   /* L_LT_EQ  */
  YYSYMBOL_L_EQ = 57,                      /* L_EQ  */
  YYSYMBOL_L_DEF_EQ = 58,                  /* L_DEF_EQ  */
  YYSYMBOL_L_NOTSIGN_EQ = 59,              /* L_NOTSIGN_EQ  */
  YYSYMBOL_L_GT_EQ = 60,                   /* L_GT_EQ  */
  YYSYMBOL_L_GT = 61,                      /* L_GT  */
  YYSYMBOL_L_DOT = 62,                     /* L_DOT  */
  YYSYMBOL_L_PREFIX_DOT = 63,              /* L_PREFIX_DOT  */
  YYSYMBOL_L_COLON = 64,                   /* L_COLON  */
  YYSYMBOL_L_COLON_PLUS = 65,              /* L_COLON_PLUS  */
  YYSYMBOL_L_PLUS_PLUS = 66,               /* L_PLUS_PLUS  */
  YYSYMBOL_L_PLUS = 67,                    /* L_PLUS  */
  YYSYMBOL_L_PREFIX_PLUS = 68,             /* L_PREFIX_PLUS  */
  YYSYMBOL_L_HYPHEN = 69,                  /* L_HYPHEN  */
  YYSYMBOL_L_PREFIX_HYPHEN = 70,           /* L_PREFIX_HYPHEN  */
  YYSYMBOL_L_ASTERISK = 71,                /* L_ASTERISK  */
  YYSYMBOL_L_TYPESIG_ASTERISK = 72,        /* L_TYPESIG_ASTERISK  */
  YYSYMBOL_L_SLASH = 73,                   /* L_SLASH  */
  YYSYMBOL_L_DIV = 74,                     /* L_DIV  */
  YYSYMBOL_L_MOD = 75,                     /* L_MOD  */
  YYSYMBOL_L_CARET = 76,                   /* L_CARET  */
  YYSYMBOL_L_PREFIX_CARET = 77,            /* L_PREFIX_CARET  */
  YYSYMBOL_L_LPAREN_CARET_RPAREN = 78,     /* L_LPAREN_CARET_RPAREN  */
  YYSYMBOL_L_NOTSIGN_CARET = 79,           /* L_NOTSIGN_CARET  */
  YYSYMBOL_L_NUMSIGN = 80,                 /* L_NUMSIGN  */
  YYSYMBOL_L_NUMSIGN_NUMSIGN = 81,         /* L_NUMSIGN_NUMSIGN  */
  YYSYMBOL_L_NUMSIGN_COLON_COLON = 82,     /* L_NUMSIGN_COLON_COLON  */
  YYSYMBOL_L_NUMSIGN_AT = 83,              /* L_NUMSIGN_AT  */
  YYSYMBOL_L_AT = 84,                      /* L_AT  */
  YYSYMBOL_L_EQ_GT = 85,                   /* L_EQ_GT  */
  YYSYMBOL_L_TILDE = 86,                   /* L_TILDE  */
  YYSYMBOL_L_TILDE_TILDE = 87,             /* L_TILDE_TILDE  */
  YYSYMBOL_L_TILDE_AT = 88,                /* L_TILDE_AT  */
  YYSYMBOL_L_TYPE = 89,                    /* L_TYPE  */
  YYSYMBOL_L_OBJECT = 90,                  /* L_OBJECT  */
  YYSYMBOL_L_CLASS = 91,                   /* L_CLASS  */
  YYSYMBOL_L_PRIMITIVE = 92,               /* L_PRIMITIVE  */
  YYSYMBOL_L_INSTANCE = 93,                /* L_INSTANCE  */
  YYSYMBOL_L_SUBORDINATE = 94,             /* L_SUBORDINATE  */
  YYSYMBOL_L_CONSTANT = 95,                /* L_CONSTANT  */
  YYSYMBOL_L_VARIABLE = 96,                /* L_VARIABLE  */
  YYSYMBOL_L_INDISPENSABLE = 97,           /* L_INDISPENSABLE  */
  YYSYMBOL_L_DYNAMIC = 98,                 /* L_DYNAMIC  */
  YYSYMBOL_L_TRANSIENT = 99,               /* L_TRANSIENT  */
  YYSYMBOL_L_PERSIST = 100,                /* L_PERSIST  */
  YYSYMBOL_L_PRIVATE = 101,                /* L_PRIVATE  */
  YYSYMBOL_L_PUBLIC = 102,                 /* L_PUBLIC  */
  YYSYMBOL_L_MATCHING = 103,               /* L_MATCHING  */
  YYSYMBOL_L_PTR = 104,                    /* L_PTR  */
  YYSYMBOL_L_UC_NAME = 105,                /* L_UC_NAME  */
  YYSYMBOL_L_LC_NAME = 106,                /* L_LC_NAME  */
  YYSYMBOL_L_INTEGER = 107,                /* L_INTEGER  */
  YYSYMBOL_L_HEX = 108,                    /* L_HEX  */
  YYSYMBOL_L_FIXED = 109,                  /* L_FIXED  */
  YYSYMBOL_L_CHAR = 110,                   /* L_CHAR  */
  YYSYMBOL_L_STRING = 111,                 /* L_STRING  */
  YYSYMBOL_PREC_LET = 112,                 /* PREC_LET  */
  YYSYMBOL_PREC_LAMBDA = 113,              /* PREC_LAMBDA  */
  YYSYMBOL_PREC_IF_THEN = 114,             /* PREC_IF_THEN  */
  YYSYMBOL_PREC_IF_THEN_ELSE = 115,        /* PREC_IF_THEN_ELSE  */
  YYSYMBOL_PREC_MODIFIER = 116,            /* PREC_MODIFIER  */
  YYSYMBOL_PREC_ATTRIBUTE = 117,           /* PREC_ATTRIBUTE  */
  YYSYMBOL_PREC_LOG_OR = 118,              /* PREC_LOG_OR  */
  YYSYMBOL_PREC_LOG_AND = 119,             /* PREC_LOG_AND  */
  YYSYMBOL_PREC_REL = 120,                 /* PREC_REL  */
  YYSYMBOL_PREC_BIT_OR = 121,              /* PREC_BIT_OR  */
  YYSYMBOL_PREC_BIT_XOR = 122,             /* PREC_BIT_XOR  */
  YYSYMBOL_PREC_BIT_AND = 123,             /* PREC_BIT_AND  */
  YYSYMBOL_PREC_SHIFT = 124,               /* PREC_SHIFT  */
  YYSYMBOL_PREC_ADD_SUB = 125,             /* PREC_ADD_SUB  */
  YYSYMBOL_PREC_CONCAT = 126,              /* PREC_CONCAT  */
  YYSYMBOL_PREC_MUL_DIV = 127,             /* PREC_MUL_DIV  */
  YYSYMBOL_PREC_EXP = 128,                 /* PREC_EXP  */
  YYSYMBOL_PREC_SEL = 129,                 /* PREC_SEL  */
  YYSYMBOL_L_PLAIN = 130,                  /* L_PLAIN  */
  YYSYMBOL_PREC_PLAIN_PATTERN = 131,       /* PREC_PLAIN_PATTERN  */
  YYSYMBOL_L_AT_PATTERN = 132,             /* L_AT_PATTERN  */
  YYSYMBOL_PREC_AT_PATTERN = 133,          /* PREC_AT_PATTERN  */
  YYSYMBOL_PREC_PTR = 134,                 /* PREC_PTR  */
  YYSYMBOL_PREC_PTR_PATTERN = 135,         /* PREC_PTR_PATTERN  */
  YYSYMBOL_PREC_LOG_NOT = 136,             /* PREC_LOG_NOT  */
  YYSYMBOL_PREC_BITWISE_NOT = 137,         /* PREC_BITWISE_NOT  */
  YYSYMBOL_PREC_DEPTR = 138,               /* PREC_DEPTR  */
  YYSYMBOL_L_LPAREN_EXCLMARK_RPAREN = 139, /* L_LPAREN_EXCLMARK_RPAREN  */
  YYSYMBOL_L_PREFIX_NUMSIGN_COLON_COLON = 140, /* L_PREFIX_NUMSIGN_COLON_COLON  */
  YYSYMBOL_YYACCEPT = 141,                 /* $accept  */
  YYSYMBOL_script = 142,                   /* script  */
  YYSYMBOL_module = 143,                   /* module  */
  YYSYMBOL_sequence = 144,                 /* sequence  */
  YYSYMBOL_modDeclOrDefnBody = 145,        /* modDeclOrDefnBody  */
  YYSYMBOL_modDeclOrDefns = 146,           /* modDeclOrDefns  */
  YYSYMBOL_modDeclOrDefnList = 147,        /* modDeclOrDefnList  */
  YYSYMBOL_modDeclOrDefn = 148,            /* modDeclOrDefn  */
  YYSYMBOL_dynTypeDefn = 149,              /* dynTypeDefn  */
  YYSYMBOL_typeDefn = 150,                 /* typeDefn  */
  YYSYMBOL_objectDefn = 151,               /* objectDefn  */
  YYSYMBOL_classDefn = 152,                /* classDefn  */
  YYSYMBOL_genClass = 153,                 /* genClass  */
  YYSYMBOL_classOrList = 154,              /* classOrList  */
  YYSYMBOL_bracketedClassList = 155,       /* bracketedClassList  */
  YYSYMBOL_bracketedClassOrList = 156,     /* bracketedClassOrList  */
  YYSYMBOL_bracketedClassListTail = 157,   /* bracketedClassListTail  */
  YYSYMBOL_bracketedClass = 158,           /* bracketedClass  */
  YYSYMBOL_class = 159,                    /* class  */
  YYSYMBOL_classDeclOrDefnBody = 160,      /* classDeclOrDefnBody  */
  YYSYMBOL_classDeclOrDefnList = 161,      /* classDeclOrDefnList  */
  YYSYMBOL_classDeclOrDefn = 162,          /* classDeclOrDefn  */
  YYSYMBOL_instanceDefn = 163,             /* instanceDefn  */
  YYSYMBOL_instanceDefnTypes = 164,        /* instanceDefnTypes  */
  YYSYMBOL_optInstanceDeclOrDefnBody = 165, /* optInstanceDeclOrDefnBody  */
  YYSYMBOL_instanceDeclOrDefnBody = 166,   /* instanceDeclOrDefnBody  */
  YYSYMBOL_instanceDeclOrDefnList = 167,   /* instanceDeclOrDefnList  */
  YYSYMBOL_instanceDeclOrDefn = 168,       /* instanceDeclOrDefn  */
  YYSYMBOL_dataConDecl = 169,              /* dataConDecl  */
  YYSYMBOL_modifiedDataConDefn = 170,      /* modifiedDataConDefn  */
  YYSYMBOL_dataConDefn = 171,              /* dataConDefn  */
  YYSYMBOL_selectors = 172,                /* selectors  */
  YYSYMBOL_selectorBody = 173,             /* selectorBody  */
  YYSYMBOL_selectorList = 174,             /* selectorList  */
  YYSYMBOL_selector = 175,                 /* selector  */
  YYSYMBOL_declOrDefnBody = 176,           /* declOrDefnBody  */
  YYSYMBOL_declOrDefnList = 177,           /* declOrDefnList  */
  YYSYMBOL_declOrDefn = 178,               /* declOrDefn  */
  YYSYMBOL_primDecl = 179,                 /* primDecl  */
  YYSYMBOL_decl = 180,                     /* decl  */
  YYSYMBOL_modifiedDefn = 181,             /* modifiedDefn  */
  YYSYMBOL_defn = 182,                     /* defn  */
  YYSYMBOL_defCmdBody = 183,               /* defCmdBody  */
  YYSYMBOL_defCmdList = 184,               /* defCmdList  */
  YYSYMBOL_defCmd = 185,                   /* defCmd  */
  YYSYMBOL_undefCmdBody = 186,             /* undefCmdBody  */
  YYSYMBOL_undefCmdList = 187,             /* undefCmdList  */
  YYSYMBOL_undefCmd = 188,                 /* undefCmd  */
  YYSYMBOL_patterns = 189,                 /* patterns  */
  YYSYMBOL_patternList = 190,              /* patternList  */
  YYSYMBOL_pattern = 191,                  /* pattern  */
  YYSYMBOL_binPattern = 192,               /* binPattern  */
  YYSYMBOL_plainPattern = 193,             /* plainPattern  */
  YYSYMBOL_tuplePtrDeconPattern = 194,     /* tuplePtrDeconPattern  */
  YYSYMBOL_tuplePattern = 195,             /* tuplePattern  */
  YYSYMBOL_patternComps = 196,             /* patternComps  */
  YYSYMBOL_ptrPattern = 197,               /* ptrPattern  */
  YYSYMBOL_dataConPtrDeconPattern = 198,   /* dataConPtrDeconPattern  */
  YYSYMBOL_dataConPattern = 199,           /* dataConPattern  */
  YYSYMBOL_concretePattern = 200,          /* concretePattern  */
  YYSYMBOL_constrainedPattern = 201,       /* constrainedPattern  */
  YYSYMBOL_varPattern = 202,               /* varPattern  */
  YYSYMBOL_aPattern = 203,                 /* aPattern  */
  YYSYMBOL_typeConstraint = 204,           /* typeConstraint  */
  YYSYMBOL_binTypeSig = 205,               /* binTypeSig  */
  YYSYMBOL_monTypeSig = 206,               /* monTypeSig  */
  YYSYMBOL_apTypeSig = 207,                /* apTypeSig  */
  YYSYMBOL_typeSig = 208,                  /* typeSig  */
  YYSYMBOL_aTypeSig = 209,                 /* aTypeSig  */
  YYSYMBOL_typeSigCompList = 210,          /* typeSigCompList  */
  YYSYMBOL_typeSigCSList = 211,            /* typeSigCSList  */
  YYSYMBOL_typeQuals = 212,                /* typeQuals  */
  YYSYMBOL_typeQualOrList = 213,           /* typeQualOrList  */
  YYSYMBOL_bTypeQualList = 214,            /* bTypeQualList  */
  YYSYMBOL_bTypeQualOrList = 215,          /* bTypeQualOrList  */
  YYSYMBOL_bTypeQualListTail = 216,        /* bTypeQualListTail  */
  YYSYMBOL_typeQual = 217,                 /* typeQual  */
  YYSYMBOL_dyadicTypeQual = 218,           /* dyadicTypeQual  */
  YYSYMBOL_monadicTypeQual = 219,          /* monadicTypeQual  */
  YYSYMBOL_instanceTypeQual = 220,         /* instanceTypeQual  */
  YYSYMBOL_bTypeQual = 221,                /* bTypeQual  */
  YYSYMBOL_aTypeQual = 222,                /* aTypeQual  */
  YYSYMBOL_predRelOp = 223,                /* predRelOp  */
  YYSYMBOL_predAndOp = 224,                /* predAndOp  */
  YYSYMBOL_predAndAlts = 225,              /* predAndAlts  */
  YYSYMBOL_predOrOp = 226,                 /* predOrOp  */
  YYSYMBOL_predOrAlts = 227,               /* predOrAlts  */
  YYSYMBOL_predNotOp = 228,                /* predNotOp  */
  YYSYMBOL_predNotAlts = 229,              /* predNotAlts  */
  YYSYMBOL_dataDecls = 230,                /* dataDecls  */
  YYSYMBOL_dataDecl = 231,                 /* dataDecl  */
  YYSYMBOL_aTypeSigs = 232,                /* aTypeSigs  */
  YYSYMBOL_aTypeSigList = 233,             /* aTypeSigList  */
  YYSYMBOL_retExpr = 234,                  /* retExpr  */
  YYSYMBOL_defExpr = 235,                  /* defExpr  */
  YYSYMBOL_undefExpr = 236,                /* undefExpr  */
  YYSYMBOL_seqExpr = 237,                  /* seqExpr  */
  YYSYMBOL_expr = 238,                     /* expr  */
  YYSYMBOL_genExpr = 239,                  /* genExpr  */
  YYSYMBOL_apExpr = 240,                   /* apExpr  */
  YYSYMBOL_binExpr = 241,                  /* binExpr  */
  YYSYMBOL_monExpr = 242,                  /* monExpr  */
  YYSYMBOL_specialExpr = 243,              /* specialExpr  */
  YYSYMBOL_sel = 244,                      /* sel  */
  YYSYMBOL_selExpr = 245,                  /* selExpr  */
  YYSYMBOL_selDataCon = 246,               /* selDataCon  */
  YYSYMBOL_bExpr = 247,                    /* bExpr  */
  YYSYMBOL_aExpr = 248,                    /* aExpr  */
  YYSYMBOL_prefixOp = 249,                 /* prefixOp  */
  YYSYMBOL_selOp = 250,                    /* selOp  */
  YYSYMBOL_expOp = 251,                    /* expOp  */
  YYSYMBOL_mulDivOp = 252,                 /* mulDivOp  */
  YYSYMBOL_concatOp = 253,                 /* concatOp  */
  YYSYMBOL_addSubOp = 254,                 /* addSubOp  */
  YYSYMBOL_relOp = 255,                    /* relOp  */
  YYSYMBOL_logAndOp = 256,                 /* logAndOp  */
  YYSYMBOL_logAndAlts = 257,               /* logAndAlts  */
  YYSYMBOL_logOrOp = 258,                  /* logOrOp  */
  YYSYMBOL_logOrAlts = 259,                /* logOrAlts  */
  YYSYMBOL_shiftOp = 260,                  /* shiftOp  */
  YYSYMBOL_bitAndOp = 261,                 /* bitAndOp  */
  YYSYMBOL_bitXorOp = 262,                 /* bitXorOp  */
  YYSYMBOL_bitOrOp = 263,                  /* bitOrOp  */
  YYSYMBOL_assignOp = 264,                 /* assignOp  */
  YYSYMBOL_dyadicOp = 265,                 /* dyadicOp  */
  YYSYMBOL_notOp = 266,                    /* notOp  */
  YYSYMBOL_notAlts = 267,                  /* notAlts  */
  YYSYMBOL_bitwiseNotOp = 268,             /* bitwiseNotOp  */
  YYSYMBOL_dePtrOp = 269,                  /* dePtrOp  */
  YYSYMBOL_monadicOp = 270,                /* monadicOp  */
  YYSYMBOL_exprs = 271,                    /* exprs  */
  YYSYMBOL_exprList = 272,                 /* exprList  */
  YYSYMBOL_compList = 273,                 /* compList  */
  YYSYMBOL_caseAltList = 274,              /* caseAltList  */
  YYSYMBOL_caseAlt = 275,                  /* caseAlt  */
  YYSYMBOL_casePatternList = 276,          /* casePatternList  */
  YYSYMBOL_casePattern = 277,              /* casePattern  */
  YYSYMBOL_qual = 278,                     /* qual  */
  YYSYMBOL_qualList = 279,                 /* qualList  */
  YYSYMBOL_typeVars = 280,                 /* typeVars  */
  YYSYMBOL_typeVarList = 281,              /* typeVarList  */
  YYSYMBOL_idOrPrefixOp = 282,             /* idOrPrefixOp  */
  YYSYMBOL_bracketedSignedId = 283,        /* bracketedSignedId  */
  YYSYMBOL_signedId = 284,                 /* signedId  */
  YYSYMBOL_idName = 285,                   /* idName  */
  YYSYMBOL_name = 286,                     /* name  */
  YYSYMBOL_wildcard = 287,                 /* wildcard  */
  YYSYMBOL_modifier = 288,                 /* modifier  */
  YYSYMBOL_attribute = 289,                /* attribute  */
  YYSYMBOL_modId = 290,                    /* modId  */
  YYSYMBOL_typeCon = 291,                  /* typeCon  */
  YYSYMBOL_tag = 292,                      /* tag  */
  YYSYMBOL_dataCon = 293,                  /* dataCon  */
  YYSYMBOL_ptr = 294,                      /* ptr  */
  YYSYMBOL_classId = 295,                  /* classId  */
  YYSYMBOL_typeVar = 296,                  /* typeVar  */
  YYSYMBOL_atom = 297,                     /* atom  */
  YYSYMBOL_const = 298,                    /* const  */
  YYSYMBOL_semiSeq = 299,                  /* semiSeq  */
  YYSYMBOL_optSemi = 300                   /* optSemi  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;


/* Default (constant) value used for initialization for null
   right-hand sides.  Unlike the standard yacc.c template, here we set
   the default value of $$ to a zeroed-out value.  Since the default
   value is undefined, this behavior is technically correct.  */
static YYSTYPE yyval_default;

/* Second part of user prologue.  */

#define lex_script 10110
#define lex_module 10111
#define lex_sequence 10112
#define lex_modDeclOrDefnBody 10113
#define lex_modDeclOrDefns 10114
#define lex_modDeclOrDefnList 10115
#define lex_modDeclOrDefn 10116
#define lex_dynTypeDefn 10117
#define lex_typeDefn 10118
#define lex_objectDefn 10119
#define lex_classDefn 10120
#define lex_genClass 10121
#define lex_classOrList 10122
#define lex_bracketedClassList 10123
#define lex_bracketedClassOrList 10124
#define lex_bracketedClassListTail 10125
#define lex_bracketedClass 10126
#define lex_class 10127
#define lex_classDeclOrDefnBody 10128
#define lex_classDeclOrDefnList 10129
#define lex_classDeclOrDefn 10130
#define lex_instanceDefn 10131
#define lex_instanceDefnTypes 10132
#define lex_optInstanceDeclOrDefnBody 10133
#define lex_instanceDeclOrDefnBody 10134
#define lex_instanceDeclOrDefnList 10135
#define lex_instanceDeclOrDefn 10136
#define lex_dataConDecl 10137
#define lex_modifiedDataConDefn 10138
#define lex_dataConDefn 10139
#define lex_selectors 10140
#define lex_selectorBody 10141
#define lex_selectorList 10142
#define lex_selector 10143
#define lex_declOrDefnBody 10144
#define lex_declOrDefnList 10145
#define lex_declOrDefn 10146
#define lex_primDecl 10147
#define lex_decl 10148
#define lex_modifiedDefn 10149
#define lex_defn 10150
#define lex_defCmdBody 10151
#define lex_defCmdList 10152
#define lex_defCmd 10153
#define lex_undefCmdBody 10154
#define lex_undefCmdList 10155
#define lex_undefCmd 10156
#define lex_patterns 10157
#define lex_patternList 10158
#define lex_pattern 10159
#define lex_binPattern 10160
#define lex_plainPattern 10161
#define lex_tuplePtrDeconPattern 10162
#define lex_tuplePattern 10163
#define lex_patternComps 10164
#define lex_ptrPattern 10165
#define lex_dataConPtrDeconPattern 10166
#define lex_dataConPattern 10167
#define lex_concretePattern 10168
#define lex_constrainedPattern 10169
#define lex_varPattern 10170
#define lex_aPattern 10171
#define lex_typeConstraint 10172
#define lex_binTypeSig 10173
#define lex_monTypeSig 10174
#define lex_apTypeSig 10175
#define lex_typeSig 10176
#define lex_aTypeSig 10177
#define lex_typeSigCompList 10178
#define lex_typeSigCSList 10179
#define lex_typeQuals 10180
#define lex_typeQualOrList 10181
#define lex_bTypeQualList 10182
#define lex_bTypeQualOrList 10183
#define lex_bTypeQualListTail 10184
#define lex_typeQual 10185
#define lex_dyadicTypeQual 10186
#define lex_monadicTypeQual 10187
#define lex_instanceTypeQual 10188
#define lex_bTypeQual 10189
#define lex_aTypeQual 10190
#define lex_predRelOp 10191
#define lex_predAndOp 10192
#define lex_predAndAlts 10193
#define lex_predOrOp 10194
#define lex_predOrAlts 10195
#define lex_predNotOp 10196
#define lex_predNotAlts 10197
#define lex_dataDecls 10198
#define lex_dataDecl 10199
#define lex_aTypeSigs 10200
#define lex_aTypeSigList 10201
#define lex_retExpr 10202
#define lex_defExpr 10203
#define lex_undefExpr 10204
#define lex_seqExpr 10205
#define lex_expr 10206
#define lex_genExpr 10207
#define lex_apExpr 10208
#define lex_binExpr 10209
#define lex_monExpr 10210
#define lex_specialExpr 10211
#define lex_sel 10212
#define lex_selExpr 10213
#define lex_selDataCon 10214
#define lex_bExpr 10215
#define lex_aExpr 10216
#define lex_prefixOp 10217
#define lex_selOp 10218
#define lex_expOp 10219
#define lex_mulDivOp 10220
#define lex_concatOp 10221
#define lex_addSubOp 10222
#define lex_relOp 10223
#define lex_logAndOp 10224
#define lex_logAndAlts 10225
#define lex_logOrOp 10226
#define lex_logOrAlts 10227
#define lex_shiftOp 10228
#define lex_bitAndOp 10229
#define lex_bitXorOp 10230
#define lex_bitOrOp 10231
#define lex_assignOp 10232
#define lex_dyadicOp 10233
#define lex_notOp 10234
#define lex_notAlts 10235
#define lex_bitwiseNotOp 10236
#define lex_dePtrOp 10237
#define lex_monadicOp 10238
#define lex_exprs 10239
#define lex_exprList 10240
#define lex_compList 10241
#define lex_caseAltList 10242
#define lex_caseAlt 10243
#define lex_casePatternList 10244
#define lex_casePattern 10245
#define lex_qual 10246
#define lex_qualList 10247
#define lex_typeVars 10248
#define lex_typeVarList 10249
#define lex_idOrPrefixOp 10250
#define lex_bracketedSignedId 10251
#define lex_signedId 10252
#define lex_idName 10253
#define lex_name 10254
#define lex_wildcard 10255
#define lex_modifier 10256
#define lex_attribute 10257
#define lex_modId 10258
#define lex_typeCon 10259
#define lex_tag 10260
#define lex_dataCon 10261
#define lex_ptr 10262
#define lex_classId 10263
#define lex_typeVar 10264
#define lex_atom 10265
#define lex_const 10266
#define lex_semiSeq 10267
#define lex_optSemi 10268
/* Symbols repesented by strings, at index+1000 */
const char * const lex_names[] = {
"module",
"def",
"with",
"where",
"undef",
"do",
"if",
"then",
"else",
"case",
"of",
"otherwise",
"let",
"in",
"return",
"this",
"inline",
"TAG",
";",
"[",
"]",
"(",
")",
"{",
"{",
"}",
"\\",
":=",
"@=",
"<-",
"!",
"#!",
",",
"_",
"::",
"::",
"¬::",
"&",
"|",
"->",
"->",
"->",
"<<",
">>",
"¬",
"not",
"and",
"or",
"#&",
"#|",
"#^",
"#¬",
"<",
"<=",
"=",
"=",
"¬=",
">=",
">",
".",
".",
":",
":+",
"++",
"+",
"+",
"-",
"-",
"*",
"*",
"/",
"div",
"mod",
"^",
"^",
"(^)",
"¬^",
"#",
"##",
"#::",
"#@",
"@",
"=>",
"~",
"~~",
"~@",
"type",
"object",
"class",
"primitive",
"instance",
"subordinate",
"constant",
"variable",
"indispensable",
"dynamic",
"transient",
"persistent",
"private",
"public",
"matching",
"Ptr",
"L_UC_NAME",
"L_LC_NAME",
"L_INTEGER",
"L_HEX",
"L_FIXED",
"L_CHAR",
"L_STRING",
"script",
"module",
"sequence",
"modDeclOrDefnBody",
"modDeclOrDefns",
"modDeclOrDefnList",
"modDeclOrDefn",
"dynTypeDefn",
"typeDefn",
"objectDefn",
"classDefn",
"genClass",
"classOrList",
"bracketedClassList",
"bracketedClassOrList",
"bracketedClassListTail",
"bracketedClass",
"class",
"classDeclOrDefnBody",
"classDeclOrDefnList",
"classDeclOrDefn",
"instanceDefn",
"instanceDefnTypes",
"optInstanceDeclOrDefnBody",
"instanceDeclOrDefnBody",
"instanceDeclOrDefnList",
"instanceDeclOrDefn",
"dataConDecl",
"modifiedDataConDefn",
"dataConDefn",
"selectors",
"selectorBody",
"selectorList",
"selector",
"declOrDefnBody",
"declOrDefnList",
"declOrDefn",
"primDecl",
"decl",
"modifiedDefn",
"defn",
"defCmdBody",
"defCmdList",
"defCmd",
"undefCmdBody",
"undefCmdList",
"undefCmd",
"patterns",
"patternList",
"pattern",
"binPattern",
"plainPattern",
"tuplePtrDeconPattern",
"tuplePattern",
"patternComps",
"ptrPattern",
"dataConPtrDeconPattern",
"dataConPattern",
"concretePattern",
"constrainedPattern",
"varPattern",
"aPattern",
"typeConstraint",
"binTypeSig",
"monTypeSig",
"apTypeSig",
"typeSig",
"aTypeSig",
"typeSigCompList",
"typeSigCSList",
"typeQuals",
"typeQualOrList",
"bTypeQualList",
"bTypeQualOrList",
"bTypeQualListTail",
"typeQual",
"dyadicTypeQual",
"monadicTypeQual",
"instanceTypeQual",
"bTypeQual",
"aTypeQual",
"predRelOp",
"predAndOp",
"predAndAlts",
"predOrOp",
"predOrAlts",
"predNotOp",
"predNotAlts",
"dataDecls",
"dataDecl",
"aTypeSigs",
"aTypeSigList",
"retExpr",
"defExpr",
"undefExpr",
"seqExpr",
"expr",
"genExpr",
"apExpr",
"binExpr",
"monExpr",
"specialExpr",
"sel",
"selExpr",
"selDataCon",
"bExpr",
"aExpr",
"prefixOp",
"selOp",
"expOp",
"mulDivOp",
"concatOp",
"addSubOp",
"relOp",
"logAndOp",
"logAndAlts",
"logOrOp",
"logOrAlts",
"shiftOp",
"bitAndOp",
"bitXorOp",
"bitOrOp",
"assignOp",
"dyadicOp",
"notOp",
"notAlts",
"bitwiseNotOp",
"dePtrOp",
"monadicOp",
"exprs",
"exprList",
"compList",
"caseAltList",
"caseAlt",
"casePatternList",
"casePattern",
"qual",
"qualList",
"typeVars",
"typeVarList",
"idOrPrefixOp",
"bracketedSignedId",
"signedId",
"idName",
"name",
"wildcard",
"modifier",
"attribute",
"modId",
"typeCon",
"tag",
"dataCon",
"ptr",
"classId",
"typeVar",
"atom",
"const",
"semiSeq",
"optSemi",
NULL};



#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif
#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YYFREE
# define YYFREE free
#endif
#ifndef YYMALLOC
# define YYMALLOC malloc
#endif
#ifndef YYREALLOC
# define YYREALLOC realloc
#endif

#ifdef __cplusplus
  typedef bool yybool;
# define yytrue true
# define yyfalse false
#else
  /* When we move to stdbool, get rid of the various casts to yybool.  */
  typedef signed char yybool;
# define yytrue 1
# define yyfalse 0
#endif

#ifndef YYSETJMP
# include <setjmp.h>
# define YYJMP_BUF jmp_buf
# define YYSETJMP(Env) setjmp (Env)
/* Pacify Clang and ICC.  */
# define YYLONGJMP(Env, Val)                    \
 do {                                           \
   longjmp (Env, Val);                          \
   YY_ASSERT (0);                               \
 } while (yyfalse)
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* The _Noreturn keyword of C11.  */
#ifndef _Noreturn
# if (defined __cplusplus \
      && ((201103 <= __cplusplus && !(__GNUC__ == 4 && __GNUC_MINOR__ == 7)) \
          || (defined _MSC_VER && 1900 <= _MSC_VER)))
#  define _Noreturn [[noreturn]]
# elif ((!defined __cplusplus || defined __clang__) \
        && (201112 <= (defined __STDC_VERSION__ ? __STDC_VERSION__ : 0) \
            || (!defined __STRICT_ANSI__ \
                && (4 < __GNUC__ + (7 <= __GNUC_MINOR__) \
                    || (defined __apple_build_version__ \
                        ? 6000000 <= __apple_build_version__ \
                        : 3 < __clang_major__ + (5 <= __clang_minor__))))))
   /* _Noreturn works as-is.  */
# elif (2 < __GNUC__ + (8 <= __GNUC_MINOR__) || defined __clang__ \
        || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= (defined _MSC_VER ? _MSC_VER : 0)
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  199
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2542

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  141
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  160
/* YYNRULES -- Number of rules.  */
#define YYNRULES  371
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  588
/* YYMAXRHS -- Maximum number of symbols on right-hand side of rule.  */
#define YYMAXRHS 6
/* YYMAXLEFT -- Maximum number of symbols to the left of a handle
   accessed by $0, $-1, etc., in any rule.  */
#define YYMAXLEFT 0

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   10139

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
       2,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140
};

#if YYDEBUG
/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   992,   992,   994,   995,   997,  1001,  1004,  1005,  1009,
    1011,  1017,  1019,  1024,  1027,  1032,  1034,  1037,  1039,  1041,
    1044,  1046,  1048,  1050,  1056,  1058,  1062,  1065,  1069,  1073,
    1074,  1078,  1080,  1085,  1090,  1092,  1097,  1102,  1106,  1112,
    1114,  1120,  1123,  1128,  1130,  1132,  1137,  1143,  1148,  1150,
    1155,  1157,  1163,  1166,  1171,  1173,  1175,  1177,  1182,  1187,
    1189,  1194,  1199,  1200,  1204,  1209,  1212,  1216,  1217,  1221,
    1223,  1228,  1231,  1235,  1237,  1241,  1242,  1245,  1248,  1250,
    1254,  1255,  1256,  1259,  1261,  1265,  1267,  1272,  1276,  1278,
    1283,  1285,  1289,  1292,  1293,  1297,  1299,  1303,  1304,  1305,
    1306,  1307,  1308,  1309,  1310,  1311,  1312,  1315,  1317,  1322,
    1324,  1326,  1328,  1330,  1335,  1340,  1345,  1347,  1352,  1357,
    1362,  1368,  1374,  1377,  1379,  1383,  1384,  1385,  1386,  1387,
    1388,  1392,  1398,  1400,  1405,  1410,  1411,  1414,  1415,  1416,
    1417,  1420,  1421,  1422,  1423,  1424,  1425,  1429,  1432,  1438,
    1440,  1446,  1447,  1451,  1453,  1458,  1463,  1465,  1470,  1474,
    1475,  1479,  1481,  1484,  1490,  1492,  1497,  1501,  1502,  1503,
    1506,  1509,  1510,  1513,  1518,  1520,  1524,  1527,  1529,  1533,
    1537,  1539,  1543,  1544,  1547,  1548,  1551,  1552,  1556,  1558,
    1562,  1563,  1566,  1569,  1572,  1573,  1576,  1577,  1578,  1579,
    1580,  1581,  1582,  1583,  1586,  1588,  1590,  1592,  1594,  1595,
    1598,  1599,  1602,  1603,  1605,  1607,  1609,  1610,  1612,  1614,
    1615,  1616,  1618,  1619,  1620,  1621,  1622,  1625,  1628,  1629,
    1630,  1631,  1632,  1633,  1634,  1635,  1636,  1639,  1641,  1643,
    1645,  1647,  1651,  1652,  1653,  1656,  1659,  1663,  1664,  1667,
    1668,  1669,  1670,  1671,  1672,  1673,  1674,  1675,  1676,  1677,
    1679,  1680,  1683,  1686,  1689,  1692,  1693,  1694,  1695,  1698,
    1701,  1702,  1705,  1706,  1707,  1708,  1709,  1710,  1713,  1716,
    1718,  1722,  1725,  1726,  1729,  1730,  1733,  1736,  1739,  1742,
    1745,  1746,  1747,  1748,  1749,  1750,  1751,  1752,  1753,  1754,
    1755,  1756,  1757,  1760,  1763,  1764,  1768,  1773,  1776,  1777,
    1778,  1781,  1782,  1785,  1786,  1789,  1790,  1794,  1796,  1800,
    1805,  1807,  1812,  1814,  1819,  1820,  1823,  1824,  1828,  1829,
    1833,  1835,  1840,  1842,  1844,  1849,  1853,  1854,  1857,  1860,
    1861,  1864,  1867,  1868,  1869,  1872,  1873,  1874,  1875,  1878,
    1881,  1882,  1883,  1886,  1889,  1892,  1893,  1896,  1899,  1902,
    1903,  1904,  1905,  1908,  1909,  1910,  1911,  1912,  1915,  1916,
    1919,  1920
};
#endif

#define YYPACT_NINF (-396)
#define YYTABLE_NINF (-341)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     782,    58,    91,  1530,  1541,  1430,  1430,    35,  1430,  -396,
    1430,   890,  1214,  1740,   610,   610,   610,   610,  -396,  -396,
    -396,  -396,  1430,  1430,  -396,   610,   610,   765,   421,  -396,
     290,   -78,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
     -21,    17,  -396,  -396,  -396,  -396,  -396,    46,  -396,  -396,
    -396,  -396,  -396,   133,  2313,  -396,   667,  -396,  -396,  -396,
     202,    59,  -396,   667,   157,  -396,  -396,  1430,  -396,  1430,
    1430,  -396,  -396,   118,  1430,  1430,  -396,  1430,  -396,  -396,
    -396,  -396,  -396,  -396,   215,  1596,   -11,  -396,  -396,  -396,
    -396,   117,   195,   157,  1596,  -396,  -396,   157,  1956,  2044,
     -13,  1848,    44,   232,  -396,  -396,  -396,  -396,  1464,  1740,
     217,  2313,  2091,   249,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  1430,  1430,  -396,  -396,  -396,  -396,   271,  2180,
     274,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,   291,  1430,  1430,  1430,   297,
     124,   303,   313,  1699,  -396,   -78,   298,  -396,  -396,  1740,
    -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,   338,   338,
    -396,  -396,  -396,  -396,  -396,   321,   321,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  1430,  -396,  1430,  -396,  -396,
    -396,   998,  -396,  1430,   321,  1430,  -396,  -396,  -396,  -396,
    1430,  1430,  1430,  1430,  1430,  1430,  1430,  1430,  1430,  1430,
    1430,  -396,  -396,  -396,  -396,  1430,  -396,  -396,  -396,   338,
     338,   338,  1430,  2353,  2353,   338,  1765,   330,   133,  1430,
     348,   133,  1430,   351,  -396,   217,  2128,  -396,  -396,  -396,
    -396,   338,   357,   359,   133,  1430,   328,  -396,   334,  1430,
    1430,  1430,  -396,  -396,  1430,  -396,  -396,  -396,  1430,  -396,
    -396,  1699,    18,   333,   -78,    18,   -10,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  -396,  -396,   338,   340,  1740,  1699,
    -396,  1430,  -396,   321,  -396,  -396,   421,   140,   421,   376,
     152,  2226,   160,  -396,  -396,  -396,  2313,   -23,  2353,   338,
      53,   566,   541,  1096,  2466,  2433,   967,   772,  1312,  1204,
    2313,  2393,   134,   964,   171,   171,    25,     0,   287,   305,
     332,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,  -396,  1464,  -396,  1583,  -396,  1096,  -396,
    1583,  -396,  2003,  1663,  -396,  -396,   165,  -396,  2313,  1430,
    1430,  2313,  2313,   372,   409,   407,  2272,  -396,  2313,  2313,
     233,   197,  1699,  -396,  1699,  -396,  -396,  1699,  1699,  -396,
    1699,  -396,  1699,  -396,  1699,  -396,  -396,  2313,  -396,  -396,
    -396,   321,   321,  -396,  -396,   321,  -396,   321,  -396,  -396,
    1740,   424,  -396,   133,   117,   392,   287,  -396,   447,   369,
    -396,   425,   374,   321,  -396,  -396,   321,  -396,  -396,   412,
    -396,  -396,  -396,  1430,  -396,   405,   446,   133,   434,   442,
    -396,  2313,  2313,  1430,  -396,  1430,  1699,  -396,  1699,   166,
    -396,  -396,  -396,  -396,   405,  -396,   439,   -23,   -23,   -23,
    -396,  1775,  -396,   426,  -396,   117,   188,   460,   306,   287,
     462,  -396,   346,  -396,   484,   450,  1430,  2313,  -396,  1650,
    -396,  1430,  1663,  -396,  2313,   405,   405,  -396,   188,  -396,
    -396,   421,   421,  -396,    32,  -396,  -396,  -396,  -396,  -396,
    -396,  -396,  -396,   425,   321,   120,  -396,   605,  -396,  1896,
    -396,  2313,  -396,  -396,   453,   421,  -396,  -396,  -396,   469,
     133,  -396,    14,  -396,  -396,  -396,  -396,  -396,  -396,   605,
     117,  -396,  -396,   287,   408,  -396,  -396,  -396,  -396,  -396,
    -396,   536,  -396,   708,  -396,   473,  -396,   188,  -396,  -396,
     318,  -396,   474,   133,   292,   480,   373,  -396,   321,  -396,
    -396,   321,   708,  -396,  -396,  -396,  -396,  -396,   708,  -396,
     708,  -396,  -396,  1322,  -396,  -396,  -396,  -396,   155,  -396,
    -396,  -396,   -23,  -396,  -396,   536,  -396,   204,  1430,   479,
     133,  2313,  -396,  2313,  -396,  1106,  -396,  -396
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,   191,   261,
     311,     0,     0,    93,     0,     0,     0,     0,   305,   304,
     306,   263,     0,     0,   356,     0,     0,     0,     0,   307,
       0,     0,   342,   343,   344,   345,   346,   347,   348,   355,
     354,   338,   364,   365,   366,   363,   367,     0,     4,     5,
     195,   208,   209,   370,   194,   201,   196,   200,   199,   248,
       0,   197,   198,   202,   247,   333,   244,     0,   303,     0,
       0,   250,   332,     0,     0,     0,   251,     0,   249,   359,
       2,   340,   339,   349,     0,     0,     0,   354,   338,   192,
      83,     0,     0,     0,     0,   193,    88,    92,     0,     0,
       0,     0,     0,     0,    69,    73,    74,    79,   336,    93,
      77,   190,   314,     0,   312,   289,   280,   283,   284,   285,
     279,   282,   286,   288,   287,   272,   273,   274,   275,   276,
     277,   269,   270,   271,   265,   266,   267,   268,   356,     0,
     244,   291,   292,   293,   294,   295,   296,   278,   297,   281,
     298,   299,   300,   301,   302,     0,   308,   309,   310,     0,
       0,     0,     0,     0,   341,     0,     0,    94,   128,    95,
     123,   124,   126,   125,   238,   237,   227,   239,   233,   232,
     240,   241,   360,   362,   353,     0,     0,   352,   351,   350,
     361,   358,   141,   143,   142,     0,   252,     0,   253,     1,
     368,   371,     8,     0,     0,     0,   270,   271,   264,   203,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   211,   247,   245,   246,     0,   243,   210,   242,   234,
     235,   236,     0,   229,   228,   230,     0,     0,   370,     0,
       0,   370,     0,     0,    78,     0,     0,   290,   308,   309,
     310,   336,     0,     0,   370,     0,     0,   337,     0,     0,
       0,     0,   256,   260,     0,   262,   334,   254,     0,   258,
     129,     0,     0,     0,     0,     0,     0,    97,    98,   100,
      99,   101,   102,   103,   104,   105,   106,   123,   126,     0,
     127,     0,    96,     0,   139,   138,   137,     0,   140,     0,
       0,     0,     0,   231,   369,     7,   225,   131,   224,   212,
     213,   214,   215,   220,   221,   222,   216,   217,   218,   219,
     226,   223,     0,    12,     0,     0,     0,     0,     0,     0,
       0,     6,     9,    15,    25,    16,    17,    18,    20,    21,
      60,    22,    76,    23,    93,    84,   371,    86,    87,    89,
     371,    91,   207,     0,   335,    70,   371,    72,   205,     0,
       0,    80,   325,   327,     0,   332,   314,   313,   316,   315,
       0,     0,     0,   114,     0,   109,   111,   127,     0,   110,
       0,   130,     0,   122,     0,   120,   118,   204,   134,   136,
     145,     0,     0,   135,   146,     0,   144,     0,   255,    59,
      93,     0,    11,   370,   328,     0,     0,   357,     0,     0,
      32,     0,    29,     0,    75,    77,     0,    19,    24,     0,
      58,    85,    90,     0,   323,   322,     0,   370,     0,   321,
      71,    81,    82,     0,   257,     0,     0,   115,     0,     0,
     119,   121,   113,   112,   108,   107,   133,   132,   148,   147,
      10,   371,    14,     0,   329,   331,     0,     0,     0,     0,
       0,    33,   149,    38,    48,   151,     0,   206,   259,   371,
     318,     0,     0,   326,   324,   117,   116,    13,     0,   330,
      27,   186,   186,    37,     0,    44,    28,    39,    45,    43,
      30,    35,    36,    34,     0,     0,    46,     0,    47,    62,
     317,   319,   320,    26,   183,   189,   184,   187,   185,     0,
     370,   150,     0,    49,    50,    56,    57,    54,    55,     0,
       0,   181,   180,     0,     0,   152,   154,   153,   167,   168,
     160,   159,   169,     0,   179,     0,    61,     0,   188,    40,
     371,    42,     0,   370,     0,     0,   159,   164,     0,   171,
     172,     0,     0,   175,   178,   174,   177,   155,     0,   173,
       0,   176,   165,     0,    63,   182,    41,    51,   371,    53,
     170,   166,   161,   157,   158,   156,   162,   163,     0,     0,
     370,    67,    52,    68,    64,   371,    66,    65
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -396,  -396,  -396,    -2,  -396,  -396,    63,   275,  -396,   185,
    -324,  -396,  -396,  -396,    57,  -396,  -396,    61,  -349,  -396,
     -24,    62,   193,  -396,  -396,  -396,   -45,    34,  -377,  -271,
     209,  -396,  -396,   -52,  -396,  -396,   178,   532,  -396,  -222,
      29,   -92,  -396,   198,   542,  -396,   196,   544,     8,   -60,
     158,  -396,  -396,  -396,   161,  -396,   278,  -396,  -396,  -396,
    -396,  -396,     3,  -106,  -396,  -396,  -396,  -146,     7,  -396,
    -386,  -396,  -396,     2,  -396,  -396,    49,  -396,  -396,  -396,
    -102,  -396,  -396,  -396,  -396,  -396,  -396,  -396,  -396,    24,
     104,    81,    76,  -396,  -396,  -396,  -396,    -5,  -396,  -396,
    -396,  -396,  -396,    19,    22,  -396,   311,    70,  -396,    39,
       1,    23,    36,    41,    42,    56,  -396,    60,  -396,    64,
      65,    66,    67,  -396,  -396,    69,  -396,    77,    80,  -396,
    -396,   322,   389,   121,  -396,   115,  -396,  -396,   156,  -396,
     137,    -3,  -396,   -18,   175,   146,  -396,  -396,  -396,  -396,
      38,  -396,     6,   126,  -321,  -395,   387,   572,  -210,  -209
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    47,    48,    49,   331,   401,   402,   403,   333,   334,
     335,   336,   408,   409,   410,   492,   461,   411,   412,   486,
     509,   510,   337,   464,   496,   513,   542,   543,   338,   339,
     340,   536,   564,   579,   580,   103,   253,   254,   341,   105,
     106,   107,    89,   237,   238,    95,   240,   241,   419,   167,
     425,   277,   278,   279,   280,   371,   281,   282,   283,   284,
     285,   168,   286,   209,   294,   295,   296,   524,   298,   300,
     463,   498,   525,   526,   574,   557,   527,   528,   529,   530,
     531,   532,   551,   558,   559,   560,   561,   533,   534,   503,
     504,   506,   507,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
     210,   211,   212,   213,   214,   215,   147,   216,   149,   217,
     218,   219,   220,   154,   155,    67,    68,    69,    70,   159,
     113,   114,   160,   426,   427,   428,   429,   363,   364,   453,
     454,    71,   109,   110,    72,    73,   171,    74,    75,    84,
     192,   193,    76,    77,   413,   194,    78,    79,   201,   202
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      98,    99,   257,   111,   108,   112,   139,   416,   244,   455,
     161,   101,   141,   195,   342,   381,   169,   178,   179,   172,
     391,   166,    91,    91,   246,    92,    97,    87,   346,   347,
     465,   350,   351,   322,   142,   190,   196,   198,   101,   297,
     299,   392,   372,  -340,   356,   357,   199,   143,   256,   406,
     140,   100,   144,   145,   100,   382,   101,   457,   307,   101,
     455,   102,   229,   100,   230,   231,   224,   146,   101,   233,
     234,   148,   235,    93,    93,   150,   151,   152,   153,    80,
     156,  -339,   245,   252,   174,   175,   176,   177,   157,   225,
     204,   158,   196,    88,    87,   180,   181,   108,   251,   108,
     226,   342,   141,   228,    91,   414,    88,    92,   511,   292,
     490,   169,   169,    91,   172,   172,    97,   258,   515,    87,
      88,    21,   325,    87,   142,   547,   222,   178,   179,   208,
     407,   226,   228,   222,   485,   515,   226,   143,    88,   322,
     247,    88,   144,   145,   101,   257,   512,   388,    83,   267,
      88,   229,   230,   231,   200,    93,   197,   146,   101,   268,
     485,   148,   571,   390,    93,   150,   151,   152,   153,   288,
     248,   290,   169,   182,   322,   172,   304,   396,   249,   101,
     383,   250,   232,   391,   100,   398,   304,   397,   170,   101,
     301,   515,   303,   451,   452,   268,    81,    82,   306,   305,
     308,   436,   548,   191,   392,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   485,   469,   470,    21,
     320,   236,   437,    88,   516,    87,    88,   321,   385,   342,
     244,   382,   438,   108,   348,   223,   488,   352,   420,    87,
      88,   516,   344,   187,   553,   446,   447,   141,   255,   448,
     358,   449,   239,   555,   361,   362,   366,    21,   381,   368,
      87,    88,   488,   369,    24,   343,   223,   462,   436,   142,
     462,    88,   262,   517,   259,   188,   189,   288,   374,   377,
     378,   380,   143,   170,   170,   247,   387,   144,   145,   289,
     517,   169,    39,    87,   172,   288,  -264,   516,   382,  -290,
     540,   541,   146,   389,   245,   393,   148,    87,    88,   415,
     150,   151,   152,   153,   195,   248,   265,   394,   488,   108,
     108,   276,   266,   249,   251,   100,   250,   395,   400,   344,
     101,   269,   484,   568,   569,   391,   270,   100,   287,   304,
     291,   184,   101,   185,   170,   186,   517,   169,   462,   549,
     172,   550,   343,   108,   431,   432,   392,   372,   345,   288,
     191,   191,   404,   405,   293,    91,    24,   221,    92,    91,
     585,   586,    97,   544,   227,   204,   349,   353,   288,   191,
     288,   494,   354,   288,   288,   359,   288,   355,   288,   391,
     288,   360,   407,   187,    39,    87,   325,   289,   328,   289,
     173,   394,   462,   169,   384,   572,   172,   433,   325,    24,
     392,   395,    88,   553,   554,   289,    93,   546,   467,   391,
      93,   324,   555,   556,    88,   188,   189,    88,   362,   370,
     474,   562,   434,   373,   375,   365,   379,    39,    87,   435,
     392,   184,   288,   185,   288,   186,   287,   386,   108,   456,
     575,   391,   450,   458,   459,   108,   576,   344,   577,   -31,
     460,   499,   481,   170,   287,   549,   501,   550,   191,   466,
     382,   191,   392,   191,   468,   288,   471,   472,   288,   289,
     343,   108,   391,   478,   481,   483,   406,   489,   505,   505,
     495,   497,   108,   187,   537,   173,   173,   539,   289,   563,
     289,   344,   567,   289,   289,   570,   289,   584,   289,   108,
     289,   332,   505,   489,   477,   418,   566,   491,   344,   170,
     487,   493,   417,   582,   518,   188,   189,    88,   287,   514,
     439,   399,   440,   587,   430,   441,   442,   108,   443,   104,
     444,   518,   445,   481,   421,    90,   422,   287,    96,   287,
     173,   376,   287,   287,   573,   287,   173,   287,   581,   287,
     480,   565,   289,   508,   289,   108,   191,   191,   545,   489,
     191,   552,   191,   583,   344,   170,   553,   554,   204,   191,
     581,   538,   482,   367,   302,   555,   556,   502,   191,   473,
     500,   191,   479,     0,   475,   289,   476,   518,   289,   183,
       0,     0,     0,   204,   482,     0,     0,   131,   365,     0,
       0,   287,   134,   287,   135,   136,   137,   208,     0,     0,
       0,     0,     6,     0,     0,   184,     0,   185,     9,   519,
     191,     0,    10,     0,    11,     0,    12,   134,   520,   135,
     136,   137,   208,     0,   287,     0,     0,   287,   293,     0,
       0,     0,   521,   522,     0,     0,   191,   191,   173,     0,
       0,     0,     0,   482,     0,     0,     0,     0,     0,   191,
       0,     0,   191,     0,     0,   173,   173,   187,     0,     6,
     191,     0,     0,     0,     0,     9,     0,     0,     0,    10,
      27,    11,    28,    12,   191,   191,    86,    31,   523,     0,
      14,    15,     0,     0,     0,     0,    17,     0,   191,   188,
     189,    88,     0,     0,     0,    87,    88,    42,    43,    44,
      45,    46,     0,   191,     0,     0,   191,   191,   184,     0,
     185,   173,   519,   191,     0,   191,     0,     0,     0,     0,
     173,   520,     0,     0,     0,    25,    26,    27,     0,    28,
       0,   293,     0,    86,    31,   521,   522,     0,     0,   173,
       0,   173,     0,     0,   173,   173,     0,   173,     0,   173,
       0,   173,    87,    88,    42,    43,    44,    45,    46,     0,
     187,     0,    -3,     1,     0,     2,     3,   173,     0,     4,
       0,     5,     0,     0,     6,     0,     0,     7,     0,     8,
       9,     0,     0,     0,    10,     0,    11,     0,    12,   204,
       0,    13,   188,   189,    88,    14,    15,   118,   119,    16,
       0,    17,     0,   173,     0,   173,     0,     0,     0,    18,
      19,     0,     0,     0,     0,     0,    20,     0,   131,   206,
       0,   207,     0,   134,    21,   135,   136,   137,   208,    22,
       0,    23,     0,     0,     0,     0,   173,     0,    24,   173,
      25,    26,    27,     0,    28,     0,    29,     0,    30,    31,
      81,    82,    42,    43,    44,    45,    46,    32,    33,    34,
       0,    35,    36,    37,    38,     0,    39,    40,    41,    42,
      43,    44,    45,    46,     3,     0,     0,     4,     0,     5,
       0,     0,     6,     0,     0,     7,     0,     0,     9,     0,
       0,     0,    10,     0,    11,     0,    12,     0,     0,    13,
       0,   115,     0,    14,    15,     0,     0,    16,     0,    17,
     116,   117,     0,     0,     0,   118,   119,    18,    19,   120,
     121,   122,   123,   124,    20,   125,   126,   127,     0,   128,
     129,   130,    21,     0,     0,     0,   131,   132,     0,   133,
       0,   134,     0,   135,   136,   137,   138,     0,    25,    26,
      27,     0,    28,     0,    29,     0,    30,    31,     0,     0,
       0,     0,     0,   322,     0,    32,    33,    34,   101,    35,
      36,    37,    38,     0,    39,    40,    41,    42,    43,    44,
      45,    46,     3,     0,   204,     4,     0,     5,     0,     0,
       6,     0,     0,     7,     0,     8,     9,     0,     0,   304,
      10,     0,    11,     0,    12,     0,     0,    13,     0,     0,
       0,    14,    15,   131,   206,    16,   207,    17,   134,     0,
     135,   136,   137,   208,     0,    18,    19,     0,     0,     0,
       0,     0,    20,   324,   325,   326,   327,   328,   329,     0,
      21,     0,   330,     0,     0,    22,     0,    23,     0,    87,
      88,     0,     0,     0,    24,     0,    25,    26,    27,     0,
      28,     0,    29,     0,    30,    31,     0,     0,     0,     0,
       0,     0,     0,    32,    33,    34,     0,    35,    36,    37,
      38,     0,    39,    40,    41,    42,    43,    44,    45,    46,
       3,     0,     0,     4,     0,     5,     0,     0,     6,     0,
       0,     7,     0,     0,     9,   578,     0,   304,    10,     0,
      11,     0,    12,   204,     0,    13,     0,     0,     0,    14,
      15,   118,   119,    16,     0,    17,     0,   122,   123,   124,
       0,     0,     0,    18,    19,     0,     0,     0,     0,     0,
      20,     0,   131,   206,     0,   207,     0,   134,    21,   135,
     136,   137,   208,    22,     0,    23,     0,     0,     0,     0,
       0,     0,    24,     0,    25,    26,    27,     0,    28,     0,
      29,     0,    30,    31,     0,     0,     0,     0,     0,     0,
       0,    32,    33,    34,     0,    35,    36,    37,    38,     0,
      39,    40,    41,    42,    43,    44,    45,    46,     3,     0,
       0,     4,     0,     5,     0,     0,     6,     0,     0,     7,
       0,     8,     9,     0,     0,     0,    10,     0,    11,     0,
      12,   204,     0,    13,     0,     0,     0,    14,    15,   118,
     119,    16,     0,    17,     0,   122,     0,   124,     0,     0,
       0,    18,    19,     0,     0,     0,     0,     0,    20,     0,
     131,   206,     0,   207,     0,   134,    21,   135,   136,   137,
     208,    22,     0,    23,     0,     0,     0,     0,     0,     0,
      24,     0,    25,    26,    27,     0,    28,     0,    29,     0,
      30,    31,     0,     0,     0,     0,     0,     0,     0,    32,
      33,    34,     0,    35,    36,    37,    38,     0,    39,    40,
      41,    42,    43,    44,    45,    46,     3,     0,     0,     4,
       0,     5,     0,     0,     6,     0,     0,     7,     0,     0,
       9,   578,     0,     0,    10,     0,    11,     0,    12,   204,
       0,    13,     0,     0,     0,    14,    15,   118,   119,    16,
       0,    17,     0,   122,     0,     0,     0,     0,     0,    18,
      19,     0,     0,     0,     0,     0,    20,     0,   131,   206,
       0,   207,     0,   134,    21,   135,   136,   137,   208,    22,
       0,    23,     0,     0,     0,     0,     0,     0,    24,     0,
      25,    26,    27,     0,    28,     0,    29,     0,    30,    31,
       0,     0,     0,     0,     0,     0,     0,    32,    33,    34,
       0,    35,    36,    37,    38,     0,    39,    40,    41,    42,
      43,    44,    45,    46,     3,     0,     0,     4,     0,     5,
       0,     0,     6,     0,     0,     7,     0,     0,     9,     0,
       0,     0,    10,     0,    11,     0,    12,     0,     0,    13,
       0,     0,     0,    14,    15,     0,     0,    16,     0,    17,
       0,     0,     0,     0,     0,     0,     0,    18,    19,     0,
       0,     0,     0,     0,    20,     0,   162,     0,   163,     0,
       0,     0,    21,     0,     0,     0,     0,    22,     0,    23,
     164,   204,     0,     0,     0,     0,    24,     0,    25,    26,
      27,     0,    28,     0,    29,     0,    30,    31,     0,     0,
       0,     0,     0,     0,     0,    32,    33,    34,     0,    35,
      36,    37,    38,     0,    39,    40,    41,    42,    43,    44,
      45,    46,     6,     0,    27,     0,    28,     0,     9,     0,
     165,     0,    10,     6,    11,     0,    12,    85,     0,     9,
       0,     0,     0,    10,     0,    11,     0,    12,    94,    87,
      88,    42,    43,    44,    45,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    21,     0,     0,     6,     0,     0,     0,     0,
       0,     9,     0,    21,   304,    10,     0,    11,     6,    12,
      27,     0,    28,     0,     9,     0,    86,    31,    10,     0,
      11,    27,    12,    28,     0,     0,     0,    86,    31,     0,
       0,     0,     0,     0,     0,    87,    88,    42,    43,    44,
      45,    46,     0,     0,     0,    21,    87,    88,    42,    43,
      44,    45,    46,     0,     0,     0,     0,     0,    21,     0,
       0,     0,     0,    27,   424,    28,     0,     0,     0,    86,
      31,   304,   162,     0,   271,     0,    27,   424,    28,     0,
       0,     0,    86,    31,     0,   162,   164,   271,    87,    88,
      42,    43,    44,    45,    46,     0,     0,     0,     0,   164,
       0,    87,    88,    42,    43,    44,    45,    46,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   162,     0,   271,     0,     0,    24,     0,     0,     0,
      27,     0,    28,     0,   272,   164,   273,   274,   275,    24,
       0,     0,     0,    27,     0,    28,     0,   272,     0,   273,
     274,   275,     0,     0,    39,    87,    88,    42,    43,    44,
      45,    46,   162,     0,   163,     0,     0,    39,    87,    88,
      42,    43,    44,    45,    46,    24,   164,     0,     0,    27,
       0,    28,     0,   272,   322,   273,   274,   275,     0,   101,
       0,   323,     0,     0,   322,     0,   304,     0,     0,   101,
       0,     0,     0,    39,    87,    88,    42,    43,    44,    45,
      46,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,    28,     0,     0,     0,   165,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    87,    88,    42,    43,    44,
      45,    46,     0,     0,   324,   325,   326,   327,   328,   329,
       0,     0,     0,   330,   324,   325,   326,   327,   328,   329,
      87,    88,   246,   330,     0,     0,     0,     0,     0,   115,
      87,    88,     0,     0,     0,     0,     0,     0,   116,   117,
       0,     0,     0,   118,   119,    18,    19,   120,   121,   122,
     123,   124,    20,   125,   126,   127,     0,   128,   129,   130,
      21,     0,     0,     0,   131,   206,     0,   207,     0,   134,
       0,   135,   136,   137,   208,     0,     0,   203,     0,     0,
       0,     0,    29,   204,     0,     0,   116,   117,     0,     0,
       0,   118,   119,     0,     0,   120,   121,   122,   123,   124,
       0,   125,   126,   127,    88,   128,   129,   130,     0,     0,
       0,   205,   131,   206,     0,   207,   242,   134,     0,   135,
     136,   137,   208,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   203,     0,     0,
       0,     0,     0,   204,     0,     0,   116,   117,     0,   535,
       0,   118,   119,     0,     0,   120,   121,   122,   123,   124,
       0,   125,   126,   127,   423,   128,   129,   130,     0,     0,
       0,   205,   131,   206,     0,   207,     0,   134,     0,   135,
     136,   137,   208,     0,   203,     0,     0,     0,     0,     0,
     204,     0,     0,   116,   117,     0,     0,     0,   118,   119,
       0,     0,   120,   121,   122,   123,   124,   243,   125,   126,
     127,     0,   128,   129,   130,     0,     0,     0,   205,   131,
     206,     0,   207,     0,   134,   203,   135,   136,   137,   208,
       0,   204,     0,     0,   116,   117,     0,     0,     0,   118,
     119,     0,     0,   120,   121,   122,   123,   124,     0,   125,
     126,   127,     0,   128,   129,   130,     0,     0,     0,   205,
     131,   206,   260,   207,     0,   134,     0,   135,   136,   137,
     208,     0,   203,     0,     0,     0,   261,     0,   204,     0,
       0,   116,   117,     0,     0,     0,   118,   119,     0,     0,
     120,   121,   122,   123,   124,     0,   125,   126,   127,     0,
     128,   129,   130,     0,     0,     0,   205,   131,   206,   115,
     207,     0,   134,     0,   135,   136,   137,   208,   116,   117,
       0,     0,     0,   118,   119,    18,    19,   120,   121,   122,
     123,   124,    20,   125,   126,   127,     0,   128,   129,   130,
      21,     0,     0,     0,   131,   206,     0,   207,     0,   134,
       0,   135,   136,   137,   208,   263,     0,     0,     0,     0,
       0,   203,    29,     0,     0,   264,     0,   204,     0,     0,
     116,   117,     0,     0,     0,   118,   119,     0,     0,   120,
     121,   122,   123,   124,     0,   125,   126,   127,     0,   128,
     129,   130,     0,     0,     0,   205,   131,   206,     0,   207,
       0,   134,     0,   135,   136,   137,   208,   203,     0,     0,
       0,   264,     0,   204,     0,     0,   116,   117,     0,     0,
       0,   118,   119,     0,     0,   120,   121,   122,   123,   124,
       0,   125,   126,   127,     0,   128,   129,   130,     0,     0,
       0,   205,   131,   206,     0,   207,     0,   134,     0,   135,
     136,   137,   208,   203,     0,     0,     0,   261,     0,   204,
       0,     0,   116,   117,     0,     0,     0,   118,   119,     0,
       0,   120,   121,   122,   123,   124,     0,   125,   126,   127,
       0,   128,   129,   130,     0,     0,     0,   205,   131,   206,
       0,   207,     0,   134,   203,   135,   136,   137,   208,     0,
     204,     0,     0,   116,   117,     0,     0,     0,   118,   119,
       0,     0,   120,   121,   122,   123,   124,     0,   125,   126,
     127,     0,   128,   129,   130,     0,     0,     0,   205,   131,
     206,     0,   207,     0,   134,     0,   135,   136,   137,   208,
     204,     0,     0,   116,   117,     0,     0,     0,   118,   119,
       0,     0,   120,   121,   122,   123,   124,     0,   125,   126,
     127,     0,   128,   129,   130,     0,     0,     0,   205,   131,
     206,     0,   207,     0,   134,     0,   135,   136,   137,   208,
     204,     0,     0,   116,   117,     0,     0,     0,   118,   119,
       0,     0,   120,   121,   122,   123,   124,     0,   125,   126,
     127,     0,   128,   129,   130,     0,     0,     0,     0,   131,
     206,     0,   207,     0,   134,     0,   135,   136,   137,   208,
     204,     0,     0,   116,     0,     0,     0,     0,   118,   119,
       0,     0,   120,     0,   122,   123,   124,     0,   125,   126,
     127,     0,   128,   129,   130,     0,     0,     0,     0,   131,
     206,     0,   207,   204,   134,     0,   135,   136,   137,   208,
       0,   118,   119,     0,     0,     0,     0,   122,   123,   124,
       0,   125,   126,   127,     0,   128,   129,   130,     0,     0,
       0,     0,   131,   206,     0,   207,     0,   134,     0,   135,
     136,   137,   208
};

static const yytype_int16 yycheck[] =
{
       5,     6,   108,     8,     7,    10,    11,   328,   100,   404,
      12,    24,    11,    24,   236,    25,    13,    22,    23,    13,
      43,    13,     3,     4,    24,     3,     4,   105,   238,   238,
     416,   241,   241,    19,    11,    28,    30,    31,    24,   185,
     186,    64,    24,    64,   254,   254,     0,    11,   108,    24,
      11,    19,    11,    11,    19,    65,    24,   406,   204,    24,
     455,    26,    67,    19,    69,    70,    60,    11,    24,    74,
      75,    11,    77,     3,     4,    11,    11,    11,    11,    21,
      11,    64,   100,   101,    14,    15,    16,    17,    11,    30,
      37,    11,    86,   106,   105,    25,    26,   100,   101,   102,
      61,   323,   101,    64,    85,   327,   106,    85,   494,   169,
     459,   108,   109,    94,   108,   109,    94,   109,   495,   105,
     106,    62,    90,   105,   101,   520,    56,   132,   133,    76,
     105,    92,    93,    63,   458,   512,    97,   101,   106,    19,
     101,   106,   101,   101,    24,   251,    26,   293,     2,    25,
     106,   156,   157,   158,    21,    85,    30,   101,    24,    35,
     484,   101,   548,    23,    94,   101,   101,   101,   101,   163,
     101,   165,   169,    27,    19,   169,    21,    25,   101,    24,
     286,   101,    64,    43,    19,    25,    21,    35,    13,    24,
     195,   568,   197,   403,   403,    35,   105,   106,   203,   201,
     205,    35,   523,    28,    64,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   540,   427,   427,    62,
     225,     6,    25,   106,   495,   105,   106,   232,   288,   451,
     322,    65,    35,   236,   239,    60,   458,   242,   344,   105,
     106,   512,   236,    72,    40,   391,   392,   246,    16,   395,
     255,   397,    57,    49,   259,   260,   261,    62,    25,   264,
     105,   106,   484,   268,    76,   236,    91,   413,    35,   246,
     416,   106,    23,   495,    57,   104,   105,   271,   272,   273,
     274,   275,   246,   108,   109,   246,   291,   246,   246,   163,
     512,   288,   104,   105,   288,   289,    25,   568,    65,    25,
     510,   510,   246,   296,   322,   298,   246,   105,   106,   327,
     246,   246,   246,   246,    24,   246,    25,    25,   540,   322,
     323,   163,    25,   246,   327,    19,   246,    35,   322,   323,
      24,    28,    26,   543,   543,    43,    23,    19,   163,    21,
      42,    20,    24,    22,   169,    24,   568,   344,   494,    57,
     344,    59,   323,   356,   359,   360,    64,    24,    28,   353,
     185,   186,   324,   325,    43,   346,    76,    56,   346,   350,
     580,   580,   350,   519,    63,    37,    28,    26,   372,   204,
     374,    35,    25,   377,   378,    57,   380,    28,   382,    43,
     384,    57,   105,    72,   104,   105,    90,   271,    93,   273,
      13,    25,   548,   400,    64,   551,   400,    35,    90,    76,
      64,    35,   106,    40,    41,   289,   346,   519,   423,    43,
     350,    89,    49,    50,   106,   104,   105,   106,   433,   271,
     435,   533,    23,   272,   273,   260,   275,   104,   105,    32,
      64,    20,   436,    22,   438,    24,   271,   289,   451,    57,
     552,    43,    28,     6,    85,   458,   558,   451,   560,    85,
      35,   466,   456,   288,   289,    57,   471,    59,   293,    57,
      65,   296,    64,   298,    28,   469,    42,    35,   472,   353,
     451,   484,    43,    57,   478,    25,    24,   458,   481,   482,
       6,    41,   495,    72,    41,   108,   109,    28,   372,    26,
     374,   495,    28,   377,   378,    25,   380,    28,   382,   512,
     384,   236,   505,   484,   451,   330,   540,   460,   512,   344,
     458,   460,   329,   568,   495,   104,   105,   106,   353,   495,
     372,   322,   374,   585,   356,   377,   378,   540,   380,     7,
     382,   512,   384,   537,   346,     3,   350,   372,     4,   374,
     163,   273,   377,   378,   552,   380,   169,   382,   563,   384,
     456,   537,   436,   482,   438,   568,   391,   392,   519,   540,
     395,    35,   397,   578,   568,   400,    40,    41,    37,   404,
     585,   505,   456,   261,   195,    49,    50,   472,   413,   433,
     469,   416,   455,    -1,   436,   469,   438,   568,   472,    27,
      -1,    -1,    -1,    37,   478,    -1,    -1,    66,   433,    -1,
      -1,   436,    71,   438,    73,    74,    75,    76,    -1,    -1,
      -1,    -1,    12,    -1,    -1,    20,    -1,    22,    18,    24,
     455,    -1,    22,    -1,    24,    -1,    26,    71,    33,    73,
      74,    75,    76,    -1,   469,    -1,    -1,   472,    43,    -1,
      -1,    -1,    47,    48,    -1,    -1,   481,   482,   271,    -1,
      -1,    -1,    -1,   537,    -1,    -1,    -1,    -1,    -1,   494,
      -1,    -1,   497,    -1,    -1,   288,   289,    72,    -1,    12,
     505,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    22,
      80,    24,    82,    26,   519,   520,    86,    87,    93,    -1,
      33,    34,    -1,    -1,    -1,    -1,    39,    -1,   533,   104,
     105,   106,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,    -1,   548,    -1,    -1,   551,   552,    20,    -1,
      22,   344,    24,   558,    -1,   560,    -1,    -1,    -1,    -1,
     353,    33,    -1,    -1,    -1,    78,    79,    80,    -1,    82,
      -1,    43,    -1,    86,    87,    47,    48,    -1,    -1,   372,
      -1,   374,    -1,    -1,   377,   378,    -1,   380,    -1,   382,
      -1,   384,   105,   106,   107,   108,   109,   110,   111,    -1,
      72,    -1,     0,     1,    -1,     3,     4,   400,    -1,     7,
      -1,     9,    -1,    -1,    12,    -1,    -1,    15,    -1,    17,
      18,    -1,    -1,    -1,    22,    -1,    24,    -1,    26,    37,
      -1,    29,   104,   105,   106,    33,    34,    45,    46,    37,
      -1,    39,    -1,   436,    -1,   438,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    -1,    -1,    54,    -1,    66,    67,
      -1,    69,    -1,    71,    62,    73,    74,    75,    76,    67,
      -1,    69,    -1,    -1,    -1,    -1,   469,    -1,    76,   472,
      78,    79,    80,    -1,    82,    -1,    84,    -1,    86,    87,
     105,   106,   107,   108,   109,   110,   111,    95,    96,    97,
      -1,    99,   100,   101,   102,    -1,   104,   105,   106,   107,
     108,   109,   110,   111,     4,    -1,    -1,     7,    -1,     9,
      -1,    -1,    12,    -1,    -1,    15,    -1,    -1,    18,    -1,
      -1,    -1,    22,    -1,    24,    -1,    26,    -1,    -1,    29,
      -1,    31,    -1,    33,    34,    -1,    -1,    37,    -1,    39,
      40,    41,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    61,    62,    -1,    -1,    -1,    66,    67,    -1,    69,
      -1,    71,    -1,    73,    74,    75,    76,    -1,    78,    79,
      80,    -1,    82,    -1,    84,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    19,    -1,    95,    96,    97,    24,    99,
     100,   101,   102,    -1,   104,   105,   106,   107,   108,   109,
     110,   111,     4,    -1,    37,     7,    -1,     9,    -1,    -1,
      12,    -1,    -1,    15,    -1,    17,    18,    -1,    -1,    21,
      22,    -1,    24,    -1,    26,    -1,    -1,    29,    -1,    -1,
      -1,    33,    34,    66,    67,    37,    69,    39,    71,    -1,
      73,    74,    75,    76,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    54,    89,    90,    91,    92,    93,    94,    -1,
      62,    -1,    98,    -1,    -1,    67,    -1,    69,    -1,   105,
     106,    -1,    -1,    -1,    76,    -1,    78,    79,    80,    -1,
      82,    -1,    84,    -1,    86,    87,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    95,    96,    97,    -1,    99,   100,   101,
     102,    -1,   104,   105,   106,   107,   108,   109,   110,   111,
       4,    -1,    -1,     7,    -1,     9,    -1,    -1,    12,    -1,
      -1,    15,    -1,    -1,    18,    19,    -1,    21,    22,    -1,
      24,    -1,    26,    37,    -1,    29,    -1,    -1,    -1,    33,
      34,    45,    46,    37,    -1,    39,    -1,    51,    52,    53,
      -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    66,    67,    -1,    69,    -1,    71,    62,    73,
      74,    75,    76,    67,    -1,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    -1,    78,    79,    80,    -1,    82,    -1,
      84,    -1,    86,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    95,    96,    97,    -1,    99,   100,   101,   102,    -1,
     104,   105,   106,   107,   108,   109,   110,   111,     4,    -1,
      -1,     7,    -1,     9,    -1,    -1,    12,    -1,    -1,    15,
      -1,    17,    18,    -1,    -1,    -1,    22,    -1,    24,    -1,
      26,    37,    -1,    29,    -1,    -1,    -1,    33,    34,    45,
      46,    37,    -1,    39,    -1,    51,    -1,    53,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    54,    -1,
      66,    67,    -1,    69,    -1,    71,    62,    73,    74,    75,
      76,    67,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    78,    79,    80,    -1,    82,    -1,    84,    -1,
      86,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,
      96,    97,    -1,    99,   100,   101,   102,    -1,   104,   105,
     106,   107,   108,   109,   110,   111,     4,    -1,    -1,     7,
      -1,     9,    -1,    -1,    12,    -1,    -1,    15,    -1,    -1,
      18,    19,    -1,    -1,    22,    -1,    24,    -1,    26,    37,
      -1,    29,    -1,    -1,    -1,    33,    34,    45,    46,    37,
      -1,    39,    -1,    51,    -1,    -1,    -1,    -1,    -1,    47,
      48,    -1,    -1,    -1,    -1,    -1,    54,    -1,    66,    67,
      -1,    69,    -1,    71,    62,    73,    74,    75,    76,    67,
      -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,
      78,    79,    80,    -1,    82,    -1,    84,    -1,    86,    87,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,
      -1,    99,   100,   101,   102,    -1,   104,   105,   106,   107,
     108,   109,   110,   111,     4,    -1,    -1,     7,    -1,     9,
      -1,    -1,    12,    -1,    -1,    15,    -1,    -1,    18,    -1,
      -1,    -1,    22,    -1,    24,    -1,    26,    -1,    -1,    29,
      -1,    -1,    -1,    33,    34,    -1,    -1,    37,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    22,    -1,    24,    -1,
      -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    -1,    69,
      36,    37,    -1,    -1,    -1,    -1,    76,    -1,    78,    79,
      80,    -1,    82,    -1,    84,    -1,    86,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    95,    96,    97,    -1,    99,
     100,   101,   102,    -1,   104,   105,   106,   107,   108,   109,
     110,   111,    12,    -1,    80,    -1,    82,    -1,    18,    -1,
      86,    -1,    22,    12,    24,    -1,    26,    27,    -1,    18,
      -1,    -1,    -1,    22,    -1,    24,    -1,    26,    27,   105,
     106,   107,   108,   109,   110,   111,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    62,    -1,    -1,    12,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    62,    21,    22,    -1,    24,    12,    26,
      80,    -1,    82,    -1,    18,    -1,    86,    87,    22,    -1,
      24,    80,    26,    82,    -1,    -1,    -1,    86,    87,    -1,
      -1,    -1,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,    -1,    -1,    -1,    62,   105,   106,   107,   108,
     109,   110,   111,    -1,    -1,    -1,    -1,    -1,    62,    -1,
      -1,    -1,    -1,    80,    14,    82,    -1,    -1,    -1,    86,
      87,    21,    22,    -1,    24,    -1,    80,    14,    82,    -1,
      -1,    -1,    86,    87,    -1,    22,    36,    24,   105,   106,
     107,   108,   109,   110,   111,    -1,    -1,    -1,    -1,    36,
      -1,   105,   106,   107,   108,   109,   110,   111,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    22,    -1,    24,    -1,    -1,    76,    -1,    -1,    -1,
      80,    -1,    82,    -1,    84,    36,    86,    87,    88,    76,
      -1,    -1,    -1,    80,    -1,    82,    -1,    84,    -1,    86,
      87,    88,    -1,    -1,   104,   105,   106,   107,   108,   109,
     110,   111,    22,    -1,    24,    -1,    -1,   104,   105,   106,
     107,   108,   109,   110,   111,    76,    36,    -1,    -1,    80,
      -1,    82,    -1,    84,    19,    86,    87,    88,    -1,    24,
      -1,    26,    -1,    -1,    19,    -1,    21,    -1,    -1,    24,
      -1,    -1,    -1,   104,   105,   106,   107,   108,   109,   110,
     111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      80,    -1,    82,    -1,    -1,    -1,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   105,   106,   107,   108,   109,
     110,   111,    -1,    -1,    89,    90,    91,    92,    93,    94,
      -1,    -1,    -1,    98,    89,    90,    91,    92,    93,    94,
     105,   106,    24,    98,    -1,    -1,    -1,    -1,    -1,    31,
     105,   106,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    61,
      62,    -1,    -1,    -1,    66,    67,    -1,    69,    -1,    71,
      -1,    73,    74,    75,    76,    -1,    -1,    31,    -1,    -1,
      -1,    -1,    84,    37,    -1,    -1,    40,    41,    -1,    -1,
      -1,    45,    46,    -1,    -1,    49,    50,    51,    52,    53,
      -1,    55,    56,    57,   106,    59,    60,    61,    -1,    -1,
      -1,    65,    66,    67,    -1,    69,    10,    71,    -1,    73,
      74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,
      -1,    -1,    -1,    37,    -1,    -1,    40,    41,    -1,   103,
      -1,    45,    46,    -1,    -1,    49,    50,    51,    52,    53,
      -1,    55,    56,    57,    11,    59,    60,    61,    -1,    -1,
      -1,    65,    66,    67,    -1,    69,    -1,    71,    -1,    73,
      74,    75,    76,    -1,    31,    -1,    -1,    -1,    -1,    -1,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    46,
      -1,    -1,    49,    50,    51,    52,    53,    13,    55,    56,
      57,    -1,    59,    60,    61,    -1,    -1,    -1,    65,    66,
      67,    -1,    69,    -1,    71,    31,    73,    74,    75,    76,
      -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,
      46,    -1,    -1,    49,    50,    51,    52,    53,    -1,    55,
      56,    57,    -1,    59,    60,    61,    -1,    -1,    -1,    65,
      66,    67,    21,    69,    -1,    71,    -1,    73,    74,    75,
      76,    -1,    31,    -1,    -1,    -1,    35,    -1,    37,    -1,
      -1,    40,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,
      49,    50,    51,    52,    53,    -1,    55,    56,    57,    -1,
      59,    60,    61,    -1,    -1,    -1,    65,    66,    67,    31,
      69,    -1,    71,    -1,    73,    74,    75,    76,    40,    41,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    61,
      62,    -1,    -1,    -1,    66,    67,    -1,    69,    -1,    71,
      -1,    73,    74,    75,    76,    25,    -1,    -1,    -1,    -1,
      -1,    31,    84,    -1,    -1,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    45,    46,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    55,    56,    57,    -1,    59,
      60,    61,    -1,    -1,    -1,    65,    66,    67,    -1,    69,
      -1,    71,    -1,    73,    74,    75,    76,    31,    -1,    -1,
      -1,    35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,
      -1,    45,    46,    -1,    -1,    49,    50,    51,    52,    53,
      -1,    55,    56,    57,    -1,    59,    60,    61,    -1,    -1,
      -1,    65,    66,    67,    -1,    69,    -1,    71,    -1,    73,
      74,    75,    76,    31,    -1,    -1,    -1,    35,    -1,    37,
      -1,    -1,    40,    41,    -1,    -1,    -1,    45,    46,    -1,
      -1,    49,    50,    51,    52,    53,    -1,    55,    56,    57,
      -1,    59,    60,    61,    -1,    -1,    -1,    65,    66,    67,
      -1,    69,    -1,    71,    31,    73,    74,    75,    76,    -1,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    46,
      -1,    -1,    49,    50,    51,    52,    53,    -1,    55,    56,
      57,    -1,    59,    60,    61,    -1,    -1,    -1,    65,    66,
      67,    -1,    69,    -1,    71,    -1,    73,    74,    75,    76,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    46,
      -1,    -1,    49,    50,    51,    52,    53,    -1,    55,    56,
      57,    -1,    59,    60,    61,    -1,    -1,    -1,    65,    66,
      67,    -1,    69,    -1,    71,    -1,    73,    74,    75,    76,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    46,
      -1,    -1,    49,    50,    51,    52,    53,    -1,    55,    56,
      57,    -1,    59,    60,    61,    -1,    -1,    -1,    -1,    66,
      67,    -1,    69,    -1,    71,    -1,    73,    74,    75,    76,
      37,    -1,    -1,    40,    -1,    -1,    -1,    -1,    45,    46,
      -1,    -1,    49,    -1,    51,    52,    53,    -1,    55,    56,
      57,    -1,    59,    60,    61,    -1,    -1,    -1,    -1,    66,
      67,    -1,    69,    37,    71,    -1,    73,    74,    75,    76,
      -1,    45,    46,    -1,    -1,    -1,    -1,    51,    52,    53,
      -1,    55,    56,    57,    -1,    59,    60,    61,    -1,    -1,
      -1,    -1,    66,    67,    -1,    69,    -1,    71,    -1,    73,
      74,    75,    76
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     1,     3,     4,     7,     9,    12,    15,    17,    18,
      22,    24,    26,    29,    33,    34,    37,    39,    47,    48,
      54,    62,    67,    69,    76,    78,    79,    80,    82,    84,
      86,    87,    95,    96,    97,    99,   100,   101,   102,   104,
     105,   106,   107,   108,   109,   110,   111,   142,   143,   144,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   248,   249,   250,   266,   267,   268,
     269,   282,   285,   286,   288,   289,   293,   294,   297,   298,
      21,   105,   106,   286,   290,    27,    86,   105,   106,   183,
     185,   244,   245,   248,    27,   186,   188,   245,   238,   238,
      19,    24,    26,   176,   178,   180,   181,   182,   282,   283,
     284,   238,   238,   271,   272,    31,    40,    41,    45,    46,
      49,    50,    51,    52,    53,    55,    56,    57,    59,    60,
      61,    66,    67,    69,    71,    73,    74,    75,    76,   238,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   268,   269,   270,
     273,   144,    22,    24,    36,    86,   189,   190,   202,   203,
     285,   287,   293,   297,   248,   248,   248,   248,   238,   238,
     248,   248,   286,   298,    20,    22,    24,    72,   104,   105,
     209,   285,   291,   292,   296,    24,   293,   294,   293,     0,
      21,   299,   300,    31,    37,    65,    67,    69,    76,   204,
     251,   252,   253,   254,   255,   256,   258,   260,   261,   262,
     263,   247,   248,   285,   293,    30,   250,   247,   250,   238,
     238,   238,    64,   238,   238,   238,     6,   184,   185,    57,
     187,   188,    10,    13,   182,   284,    24,   250,   266,   268,
     269,   282,   284,   177,   178,    16,   190,   204,   189,    57,
      21,    35,    23,    25,    35,    25,    25,    25,    35,    28,
      23,    24,    84,    86,    87,    88,   191,   192,   193,   194,
     195,   197,   198,   199,   200,   201,   203,   285,   293,   294,
     293,    42,   190,    43,   205,   206,   207,   208,   209,   208,
     210,   238,   273,   238,    21,   144,   238,   208,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,    19,    26,    89,    90,    91,    92,    93,    94,
      98,   145,   148,   149,   150,   151,   152,   163,   169,   170,
     171,   179,   180,   181,   293,    28,   299,   300,   238,    28,
     299,   300,   238,    26,    25,    28,   299,   300,   238,    57,
      57,   238,   238,   278,   279,   285,   238,   272,   238,   238,
     191,   196,    24,   195,   293,   195,   197,   293,   293,   195,
     293,    25,    65,   204,    64,   190,   191,   238,   208,   209,
      23,    43,    64,   209,    25,    35,    25,    35,    25,   171,
     293,   146,   147,   148,   291,   291,    24,   105,   153,   154,
     155,   158,   159,   295,   180,   284,   295,   163,   150,   189,
     204,   184,   187,    11,    14,   191,   274,   275,   276,   277,
     177,   238,   238,    35,    23,    32,    35,    25,    35,   191,
     191,   191,   191,   191,   191,   191,   208,   208,   208,   208,
      28,   299,   300,   280,   281,   296,    57,   159,     6,    85,
      35,   157,   208,   211,   164,   211,    57,   238,    28,   299,
     300,    42,    35,   279,   238,   191,   191,   147,    57,   281,
     231,   293,   294,    25,    26,   151,   160,   162,   180,   181,
     159,   155,   156,   158,    35,     6,   165,    41,   212,   238,
     274,   238,   276,   230,   231,   209,   232,   233,   232,   161,
     162,   211,    26,   166,   168,   169,   170,   180,   181,    24,
      33,    47,    48,    93,   208,   213,   214,   217,   218,   219,
     220,   221,   222,   228,   229,   103,   172,    41,   233,    28,
     299,   300,   167,   168,   208,   217,   221,   296,   295,    57,
      59,   223,    35,    40,    41,    49,    50,   216,   224,   225,
     226,   227,   221,    26,   173,   230,   161,    28,   299,   300,
      25,   211,   208,   214,   215,   221,   221,   221,    19,   174,
     175,   238,   167,   238,    28,   299,   300,   174
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   141,   142,   142,   142,   142,   143,   144,   144,   145,
     145,   146,   146,   147,   147,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   149,   149,   150,   151,   152,   153,
     153,   154,   154,   155,   156,   156,   157,   158,   159,   160,
     160,   161,   161,   162,   162,   162,   163,   164,   165,   165,
     166,   166,   167,   167,   168,   168,   168,   168,   169,   170,
     170,   171,   172,   172,   173,   174,   174,   175,   175,   176,
     176,   177,   177,   178,   178,   179,   179,   180,   181,   181,
     182,   182,   182,   183,   183,   184,   184,   185,   186,   186,
     187,   187,   188,   189,   189,   190,   190,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   192,   192,   193,
     193,   193,   193,   193,   194,   195,   196,   196,   197,   198,
     199,   200,   201,   202,   202,   203,   203,   203,   203,   203,
     203,   204,   205,   205,   206,   207,   207,   208,   208,   208,
     208,   209,   209,   209,   209,   209,   209,   210,   210,   211,
     211,   212,   212,   213,   213,   214,   215,   215,   216,   217,
     217,   218,   218,   218,   219,   219,   220,   221,   221,   221,
     222,   223,   223,   224,   225,   225,   226,   227,   227,   228,
     229,   229,   230,   230,   231,   231,   232,   232,   233,   233,
     234,   234,   235,   236,   237,   237,   238,   238,   238,   238,
     238,   238,   238,   238,   239,   239,   239,   239,   239,   239,
     240,   240,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   243,   243,   243,
     243,   243,   244,   244,   244,   245,   246,   247,   247,   248,
     248,   248,   248,   248,   248,   248,   248,   248,   248,   248,
     248,   248,   249,   250,   251,   252,   252,   252,   252,   253,
     254,   254,   255,   255,   255,   255,   255,   255,   256,   257,
     257,   258,   259,   259,   260,   260,   261,   262,   263,   264,
     265,   265,   265,   265,   265,   265,   265,   265,   265,   265,
     265,   265,   265,   266,   267,   267,   268,   269,   270,   270,
     270,   271,   271,   272,   272,   273,   273,   274,   274,   275,
     276,   276,   277,   277,   278,   278,   279,   279,   280,   280,
     281,   281,   282,   282,   282,   283,   284,   284,   285,   286,
     286,   287,   288,   288,   288,   289,   289,   289,   289,   290,
     291,   291,   291,   292,   293,   294,   294,   295,   296,   297,
     297,   297,   297,   298,   298,   298,   298,   298,   299,   299,
     300,   300
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     0,     1,     1,     4,     3,     2,     1,
       3,     1,     0,     3,     2,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     2,     1,     5,     4,     4,     1,
       3,     1,     1,     2,     1,     1,     2,     3,     2,     1,
       3,     3,     2,     1,     1,     1,     4,     2,     0,     2,
       1,     3,     3,     2,     1,     1,     1,     1,     2,     2,
       1,     5,     0,     2,     3,     3,     2,     1,     2,     1,
       3,     3,     2,     1,     1,     2,     1,     1,     2,     1,
       3,     4,     4,     1,     3,     3,     2,     3,     1,     3,
       3,     2,     1,     0,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     2,
       2,     2,     3,     3,     2,     3,     3,     3,     2,     3,
       2,     3,     2,     1,     1,     1,     1,     2,     1,     2,
       3,     2,     3,     3,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     1,
       3,     0,     2,     1,     1,     2,     1,     1,     2,     1,
       1,     3,     3,     3,     2,     2,     3,     1,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     2,     2,     0,     1,     2,     1,
       2,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     4,     4,     6,     4,     1,     1,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     3,     4,     3,     5,     3,     6,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     1,     3,     1,     3,     3,     3,     2,     3,
       3,     1,     1,     1,     3,     1,     3,     1,     0,     1,
       2,     1,     1,     1,     3,     3,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     2,
       0,     1
};


/* YYDPREC[RULE-NUM] -- Dynamic precedence of rule #RULE-NUM (0 if none).  */
static const yytype_int8 yydprec[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYMERGER[RULE-NUM] -- Index of merging function for rule #RULE-NUM.  */
static const yytype_int8 yymerger[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYIMMEDIATE[RULE-NUM] -- True iff rule #RULE-NUM is not to be deferred, as
   in the case of predicates.  */
static const yybool yyimmediate[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYCONFLP[YYPACT[STATE-NUM]] -- Pointer into YYCONFL of start of
   list of conflicting reductions corresponding to action entry for
   state STATE-NUM in yytable.  0 means no conflicts.  The list in
   yyconfl is terminated by a rule number of 0.  */
static const yytype_int8 yyconflp[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0
};

/* YYCONFL[I] -- lists of conflicting rule numbers, each terminated by
   0, pointed into by YYCONFLP.  */
static const short yyconfl[] =
{
       0
};



YYSTYPE yylval;

int yynerrs;
int yychar;

enum { YYENOMEM = -2 };

typedef enum { yyok, yyaccept, yyabort, yyerr, yynomem } YYRESULTTAG;

#define YYCHK(YYE)                              \
  do {                                          \
    YYRESULTTAG yychk_flag = YYE;               \
    if (yychk_flag != yyok)                     \
      return yychk_flag;                        \
  } while (0)

/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYMAXDEPTH * sizeof (GLRStackItem)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Minimum number of free items on the stack allowed after an
   allocation.  This is to allow allocation and initialization
   to be completed by functions that call yyexpandGLRStack before the
   stack is expanded, thus insuring that all necessary pointers get
   properly redirected to new data.  */
#define YYHEADROOM 2

#ifndef YYSTACKEXPANDABLE
#  define YYSTACKEXPANDABLE 1
#endif

#if YYSTACKEXPANDABLE
# define YY_RESERVE_GLRSTACK(Yystack)                   \
  do {                                                  \
    if (Yystack->yyspaceLeft < YYHEADROOM)              \
      yyexpandGLRStack (Yystack);                       \
  } while (0)
#else
# define YY_RESERVE_GLRSTACK(Yystack)                   \
  do {                                                  \
    if (Yystack->yyspaceLeft < YYHEADROOM)              \
      yyMemoryExhausted (Yystack);                      \
  } while (0)
#endif

/** State numbers. */
typedef int yy_state_t;

/** Rule numbers. */
typedef int yyRuleNum;

/** Item references. */
typedef short yyItemNum;

typedef struct yyGLRState yyGLRState;
typedef struct yyGLRStateSet yyGLRStateSet;
typedef struct yySemanticOption yySemanticOption;
typedef union yyGLRStackItem yyGLRStackItem;
typedef struct yyGLRStack yyGLRStack;

struct yyGLRState
{
  /** Type tag: always true.  */
  yybool yyisState;
  /** Type tag for yysemantics.  If true, yyval applies, otherwise
   *  yyfirstVal applies.  */
  yybool yyresolved;
  /** Number of corresponding LALR(1) machine state.  */
  yy_state_t yylrState;
  /** Preceding state in this stack */
  yyGLRState* yypred;
  /** Source position of the last token produced by my symbol */
  YYPTRDIFF_T yyposn;
  union {
    /** First in a chain of alternative reductions producing the
     *  nonterminal corresponding to this state, threaded through
     *  yynext.  */
    yySemanticOption* yyfirstVal;
    /** Semantic value for this state.  */
    YYSTYPE yyval;
  } yysemantics;
};

struct yyGLRStateSet
{
  yyGLRState** yystates;
  /** During nondeterministic operation, yylookaheadNeeds tracks which
   *  stacks have actually needed the current lookahead.  During deterministic
   *  operation, yylookaheadNeeds[0] is not maintained since it would merely
   *  duplicate yychar != YYEMPTY.  */
  yybool* yylookaheadNeeds;
  YYPTRDIFF_T yysize;
  YYPTRDIFF_T yycapacity;
};

struct yySemanticOption
{
  /** Type tag: always false.  */
  yybool yyisState;
  /** Rule number for this reduction */
  yyRuleNum yyrule;
  /** The last RHS state in the list of states to be reduced.  */
  yyGLRState* yystate;
  /** The lookahead for this reduction.  */
  int yyrawchar;
  YYSTYPE yyval;
  /** Next sibling in chain of options.  To facilitate merging,
   *  options are chained in decreasing order by address.  */
  yySemanticOption* yynext;
};

/** Type of the items in the GLR stack.  The yyisState field
 *  indicates which item of the union is valid.  */
union yyGLRStackItem {
  yyGLRState yystate;
  yySemanticOption yyoption;
};

struct yyGLRStack {
  int yyerrState;


  YYJMP_BUF yyexception_buffer;
  yyGLRStackItem* yyitems;
  yyGLRStackItem* yynextFree;
  YYPTRDIFF_T yyspaceLeft;
  yyGLRState* yysplitPoint;
  yyGLRState* yylastDeleted;
  yyGLRStateSet yytops;
};

#if YYSTACKEXPANDABLE
static void yyexpandGLRStack (yyGLRStack* yystackp);
#endif

_Noreturn static void
yyFail (yyGLRStack* yystackp, const char* yymsg)
{
  if (yymsg != YY_NULLPTR)
    yyerror (yymsg);
  YYLONGJMP (yystackp->yyexception_buffer, 1);
}

_Noreturn static void
yyMemoryExhausted (yyGLRStack* yystackp)
{
  YYLONGJMP (yystackp->yyexception_buffer, 2);
}

/** Accessing symbol of state YYSTATE.  */
static inline yysymbol_kind_t
yy_accessing_symbol (yy_state_t yystate)
{
  return YY_CAST (yysymbol_kind_t, yystos[yystate]);
}

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "L_MODULE", "L_DEF",
  "L_WITH", "L_WHERE", "L_UNDEF", "L_DO", "L_IF", "L_THEN", "L_ELSE",
  "L_CASE", "L_OF", "L_OTHERWISE", "L_LET", "L_IN", "L_RETURN", "L_THIS",
  "L_INLINE", "L_TAG", "L_SEMICOLON", "L_LSQB", "L_RSQB", "L_LPAREN",
  "L_RPAREN", "L_LCURLYB", "L_CMD_LCURLYB", "L_RCURLYB", "L_BACKSLASH",
  "L_COLON_EQ", "L_AT_EQ", "L_LT_HYPHEN", "L_EXCLMARK",
  "L_NUMSIGN_EXCLMARK", "L_COMMA", "L_UNDERSCORE", "L_COLON_COLON",
  "L_PREFIX_COLON_COLON", "L_NOTSIGN_COLON_COLON", "L_AMPERSAND",
  "L_VERTBAR", "L_HYPHEN_GT", "L_TYPESIG_HYPHEN_GT",
  "L_PREFIX_TYPESIG_HYPHEN_GT", "L_LT_LT", "L_GT_GT", "L_NOTSIGN", "L_NOT",
  "L_AND", "L_OR", "L_NUMSIGN_AMPERSAND", "L_NUMSIGN_VERTBAR",
  "L_NUMSIGN_CARET", "L_NUMSIGN_NOTSIGN", "L_LT", "L_LT_EQ", "L_EQ",
  "L_DEF_EQ", "L_NOTSIGN_EQ", "L_GT_EQ", "L_GT", "L_DOT", "L_PREFIX_DOT",
  "L_COLON", "L_COLON_PLUS", "L_PLUS_PLUS", "L_PLUS", "L_PREFIX_PLUS",
  "L_HYPHEN", "L_PREFIX_HYPHEN", "L_ASTERISK", "L_TYPESIG_ASTERISK",
  "L_SLASH", "L_DIV", "L_MOD", "L_CARET", "L_PREFIX_CARET",
  "L_LPAREN_CARET_RPAREN", "L_NOTSIGN_CARET", "L_NUMSIGN",
  "L_NUMSIGN_NUMSIGN", "L_NUMSIGN_COLON_COLON", "L_NUMSIGN_AT", "L_AT",
  "L_EQ_GT", "L_TILDE", "L_TILDE_TILDE", "L_TILDE_AT", "L_TYPE",
  "L_OBJECT", "L_CLASS", "L_PRIMITIVE", "L_INSTANCE", "L_SUBORDINATE",
  "L_CONSTANT", "L_VARIABLE", "L_INDISPENSABLE", "L_DYNAMIC",
  "L_TRANSIENT", "L_PERSIST", "L_PRIVATE", "L_PUBLIC", "L_MATCHING",
  "L_PTR", "L_UC_NAME", "L_LC_NAME", "L_INTEGER", "L_HEX", "L_FIXED",
  "L_CHAR", "L_STRING", "PREC_LET", "PREC_LAMBDA", "PREC_IF_THEN",
  "PREC_IF_THEN_ELSE", "PREC_MODIFIER", "PREC_ATTRIBUTE", "PREC_LOG_OR",
  "PREC_LOG_AND", "PREC_REL", "PREC_BIT_OR", "PREC_BIT_XOR",
  "PREC_BIT_AND", "PREC_SHIFT", "PREC_ADD_SUB", "PREC_CONCAT",
  "PREC_MUL_DIV", "PREC_EXP", "PREC_SEL", "L_PLAIN", "PREC_PLAIN_PATTERN",
  "L_AT_PATTERN", "PREC_AT_PATTERN", "PREC_PTR", "PREC_PTR_PATTERN",
  "PREC_LOG_NOT", "PREC_BITWISE_NOT", "PREC_DEPTR",
  "L_LPAREN_EXCLMARK_RPAREN", "L_PREFIX_NUMSIGN_COLON_COLON", "$accept",
  "script", "module", "sequence", "modDeclOrDefnBody", "modDeclOrDefns",
  "modDeclOrDefnList", "modDeclOrDefn", "dynTypeDefn", "typeDefn",
  "objectDefn", "classDefn", "genClass", "classOrList",
  "bracketedClassList", "bracketedClassOrList", "bracketedClassListTail",
  "bracketedClass", "class", "classDeclOrDefnBody", "classDeclOrDefnList",
  "classDeclOrDefn", "instanceDefn", "instanceDefnTypes",
  "optInstanceDeclOrDefnBody", "instanceDeclOrDefnBody",
  "instanceDeclOrDefnList", "instanceDeclOrDefn", "dataConDecl",
  "modifiedDataConDefn", "dataConDefn", "selectors", "selectorBody",
  "selectorList", "selector", "declOrDefnBody", "declOrDefnList",
  "declOrDefn", "primDecl", "decl", "modifiedDefn", "defn", "defCmdBody",
  "defCmdList", "defCmd", "undefCmdBody", "undefCmdList", "undefCmd",
  "patterns", "patternList", "pattern", "binPattern", "plainPattern",
  "tuplePtrDeconPattern", "tuplePattern", "patternComps", "ptrPattern",
  "dataConPtrDeconPattern", "dataConPattern", "concretePattern",
  "constrainedPattern", "varPattern", "aPattern", "typeConstraint",
  "binTypeSig", "monTypeSig", "apTypeSig", "typeSig", "aTypeSig",
  "typeSigCompList", "typeSigCSList", "typeQuals", "typeQualOrList",
  "bTypeQualList", "bTypeQualOrList", "bTypeQualListTail", "typeQual",
  "dyadicTypeQual", "monadicTypeQual", "instanceTypeQual", "bTypeQual",
  "aTypeQual", "predRelOp", "predAndOp", "predAndAlts", "predOrOp",
  "predOrAlts", "predNotOp", "predNotAlts", "dataDecls", "dataDecl",
  "aTypeSigs", "aTypeSigList", "retExpr", "defExpr", "undefExpr",
  "seqExpr", "expr", "genExpr", "apExpr", "binExpr", "monExpr",
  "specialExpr", "sel", "selExpr", "selDataCon", "bExpr", "aExpr",
  "prefixOp", "selOp", "expOp", "mulDivOp", "concatOp", "addSubOp",
  "relOp", "logAndOp", "logAndAlts", "logOrOp", "logOrAlts", "shiftOp",
  "bitAndOp", "bitXorOp", "bitOrOp", "assignOp", "dyadicOp", "notOp",
  "notAlts", "bitwiseNotOp", "dePtrOp", "monadicOp", "exprs", "exprList",
  "compList", "caseAltList", "caseAlt", "casePatternList", "casePattern",
  "qual", "qualList", "typeVars", "typeVarList", "idOrPrefixOp",
  "bracketedSignedId", "signedId", "idName", "name", "wildcard",
  "modifier", "attribute", "modId", "typeCon", "tag", "dataCon", "ptr",
  "classId", "typeVar", "atom", "const", "semiSeq", "optSemi", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

/** Left-hand-side symbol for rule #YYRULE.  */
static inline yysymbol_kind_t
yylhsNonterm (yyRuleNum yyrule)
{
  return YY_CAST (yysymbol_kind_t, yyr1[yyrule]);
}

#if YYDEBUG

# ifndef YYFPRINTF
#  define YYFPRINTF fprintf
# endif

# define YY_FPRINTF                             \
  YY_IGNORE_USELESS_CAST_BEGIN YY_FPRINTF_

# define YY_FPRINTF_(Args)                      \
  do {                                          \
    YYFPRINTF Args;                             \
    YY_IGNORE_USELESS_CAST_END                  \
  } while (0)

# define YY_DPRINTF                             \
  YY_IGNORE_USELESS_CAST_BEGIN YY_DPRINTF_

# define YY_DPRINTF_(Args)                      \
  do {                                          \
    if (yydebug)                                \
      YYFPRINTF Args;                           \
    YY_IGNORE_USELESS_CAST_END                  \
  } while (0)





/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                  \
  do {                                                                  \
    if (yydebug)                                                        \
      {                                                                 \
        YY_FPRINTF ((stderr, "%s ", Title));                            \
        yy_symbol_print (stderr, Kind, Value);        \
        YY_FPRINTF ((stderr, "\n"));                                    \
      }                                                                 \
  } while (0)

static inline void
yy_reduce_print (yybool yynormal, yyGLRStackItem* yyvsp, YYPTRDIFF_T yyk,
                 yyRuleNum yyrule);

# define YY_REDUCE_PRINT(Args)          \
  do {                                  \
    if (yydebug)                        \
      yy_reduce_print Args;             \
  } while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;

static void yypstack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
  YY_ATTRIBUTE_UNUSED;
static void yypdumpstack (yyGLRStack* yystackp)
  YY_ATTRIBUTE_UNUSED;

#else /* !YYDEBUG */

# define YY_DPRINTF(Args) do {} while (yyfalse)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_REDUCE_PRINT(Args)

#endif /* !YYDEBUG */



/** Fill in YYVSP[YYLOW1 .. YYLOW0-1] from the chain of states starting
 *  at YYVSP[YYLOW0].yystate.yypred.  Leaves YYVSP[YYLOW1].yystate.yypred
 *  containing the pointer to the next state in the chain.  */
static void yyfillin (yyGLRStackItem *, int, int) YY_ATTRIBUTE_UNUSED;
static void
yyfillin (yyGLRStackItem *yyvsp, int yylow0, int yylow1)
{
  int i;
  yyGLRState *s = yyvsp[yylow0].yystate.yypred;
  for (i = yylow0-1; i >= yylow1; i -= 1)
    {
#if YYDEBUG
      yyvsp[i].yystate.yylrState = s->yylrState;
#endif
      yyvsp[i].yystate.yyresolved = s->yyresolved;
      if (s->yyresolved)
        yyvsp[i].yystate.yysemantics.yyval = s->yysemantics.yyval;
      else
        /* The effect of using yyval or yyloc (in an immediate rule) is
         * undefined.  */
        yyvsp[i].yystate.yysemantics.yyfirstVal = YY_NULLPTR;
      s = yyvsp[i].yystate.yypred = s->yypred;
    }
}


/** If yychar is empty, fetch the next token.  */
static inline yysymbol_kind_t
yygetToken (int *yycharp)
{
  yysymbol_kind_t yytoken;
  if (*yycharp == YYEMPTY)
    {
      YY_DPRINTF ((stderr, "Reading a token\n"));
      *yycharp = yylex ();
    }
  if (*yycharp <= YYEOF)
    {
      *yycharp = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YY_DPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (*yycharp);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }
  return yytoken;
}

/* Do nothing if YYNORMAL or if *YYLOW <= YYLOW1.  Otherwise, fill in
 * YYVSP[YYLOW1 .. *YYLOW-1] as in yyfillin and set *YYLOW = YYLOW1.
 * For convenience, always return YYLOW1.  */
static inline int yyfill (yyGLRStackItem *, int *, int, yybool)
     YY_ATTRIBUTE_UNUSED;
static inline int
yyfill (yyGLRStackItem *yyvsp, int *yylow, int yylow1, yybool yynormal)
{
  if (!yynormal && yylow1 < *yylow)
    {
      yyfillin (yyvsp, *yylow, yylow1);
      *yylow = yylow1;
    }
  return yylow1;
}

/** Perform user action for rule number YYN, with RHS length YYRHSLEN,
 *  and top stack item YYVSP.  YYLVALP points to place to put semantic
 *  value ($$), and yylocp points to place for location information
 *  (@$).  Returns yyok for normal return, yyaccept for YYACCEPT,
 *  yyerr for YYERROR, yyabort for YYABORT, yynomem for YYNOMEM.  */
static YYRESULTTAG
yyuserAction (yyRuleNum yyrule, int yyrhslen, yyGLRStackItem* yyvsp,
              yyGLRStack* yystackp, YYPTRDIFF_T yyk,
              YYSTYPE* yyvalp)
{
  const yybool yynormal YY_ATTRIBUTE_UNUSED = yystackp->yysplitPoint == YY_NULLPTR;
  int yylow = 1;
  YY_USE (yyvalp);
  YY_USE (yyk);
  YY_USE (yyrhslen);
# undef yyerrok
# define yyerrok (yystackp->yyerrState = 0)
# undef YYACCEPT
# define YYACCEPT return yyaccept
# undef YYABORT
# define YYABORT return yyabort
# undef YYNOMEM
# define YYNOMEM return yynomem
# undef YYERROR
# define YYERROR return yyerrok, yyerr
# undef YYRECOVERING
# define YYRECOVERING() (yystackp->yyerrState != 0)
# undef yyclearin
# define yyclearin (yychar = YYEMPTY)
# undef YYFILL
# define YYFILL(N) yyfill (yyvsp, &yylow, (N), yynormal)
# undef YYBACKUP
# define YYBACKUP(Token, Value)                                              \
  return yyerror (YY_("syntax error: cannot back up")),     \
         yyerrok, yyerr

  if (yyrhslen == 0)
    *yyvalp = yyval_default;
  else
    *yyvalp = yyvsp[YYFILL (1-yyrhslen)].yystate.yysemantics.yyval;
  /* If yyk == -1, we are running a deferred action on a temporary
     stack.  In that case, YY_REDUCE_PRINT must not play with YYFILL,
     so pretend the stack is "normal". */
  YY_REDUCE_PRINT ((yynormal || yyk == -1, yyvsp, yyk, yyrule));
  switch (yyrule)
    {
  case 2: /* script: error L_SEMICOLON  */
                                                                {				parserError();
													script = ERROR; }
    break;

  case 3: /* script: %empty  */
                                                                        { pp(0, lex_script);	script = VOID; }
    break;

  case 4: /* script: module  */
                                                                { pp(1, lex_script);    clear();
												      script = l_pair(MODULE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 5: /* script: sequence  */
                                                                        { pp(1, lex_script);    clear();
												      script = l_pair(ORDER, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 6: /* module: L_MODULE modId L_WHERE modDeclOrDefnBody  */
                                                        { pp(4, lex_module);	((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 7: /* sequence: seqExpr semiSeq sequence  */
                                                                { pp(3, lex_sequence);	((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 8: /* sequence: seqExpr optSemi  */
                                                                { pp(2, lex_sequence);	((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 9: /* modDeclOrDefnBody: modDeclOrDefn  */
                                                                { pp(1, lex_modDeclOrDefnBody);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 10: /* modDeclOrDefnBody: L_LCURLYB modDeclOrDefns L_RCURLYB  */
                                                                        { pp(3, lex_modDeclOrDefnBody);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 11: /* modDeclOrDefns: modDeclOrDefnList  */
                                                                        { pp(1, lex_modDeclOrDefns);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 12: /* modDeclOrDefns: %empty  */
                                                                        { pp(0, lex_modDeclOrDefns);
													((*yyvalp)._expr) = l_Nil; }
    break;

  case 13: /* modDeclOrDefnList: modDeclOrDefn semiSeq modDeclOrDefnList  */
                                                                        { pp(3, lex_modDeclOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 14: /* modDeclOrDefnList: modDeclOrDefn optSemi  */
                                                                { pp(2, lex_modDeclOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 15: /* modDeclOrDefn: dynTypeDefn  */
                                                                { pp(1, lex_modDeclOrDefn);
	      											((*yyvalp)._expr) = l_pair(TYPE_DEFN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 16: /* modDeclOrDefn: objectDefn  */
                                                                { pp(1, lex_modDeclOrDefn);
	      											((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 17: /* modDeclOrDefn: classDefn  */
                                                                { pp(1, lex_modDeclOrDefn);
	      											((*yyvalp)._expr) = l_pair(CLASS_DEFN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 18: /* modDeclOrDefn: instanceDefn  */
                                                                { pp(1, lex_modDeclOrDefn);
													((*yyvalp)._expr) = l_pair(INSTANCE_DEFN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 19: /* modDeclOrDefn: L_SUBORDINATE instanceDefn  */
                                                                { pp(1, lex_modDeclOrDefn);
													((*yyvalp)._expr) = l_pair(INSTANCE_DEFN, l_pair(SUBORDINATE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 20: /* modDeclOrDefn: dataConDecl  */
                                                            { pp(1, lex_modDeclOrDefn);
      												((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 21: /* modDeclOrDefn: modifiedDataConDefn  */
                                                                { pp(1, lex_modDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 22: /* modDeclOrDefn: primDecl  */
                                                                { pp(1, lex_modDeclOrDefn);
      												((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 23: /* modDeclOrDefn: modifiedDefn  */
                                                                { pp(1, lex_modDeclOrDefn);
      												((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 24: /* dynTypeDefn: L_DYNAMIC typeDefn  */
                                                                { pp(2, lex_dynTypeDefn);
													((*yyvalp)._expr) = l_pair(DYNAMIC, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 25: /* dynTypeDefn: typeDefn  */
                                                                { pp(1, lex_dynTypeDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 26: /* typeDefn: L_TYPE typeCon typeVars L_EQ dataDecls  */
                                                        { pp(5, lex_typeDefn);	((*yyvalp)._expr) = l_triple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 27: /* objectDefn: L_OBJECT typeCon L_EQ dataDecl  */
                                                        { pp(4, lex_objectDefn);	
													((*yyvalp)._expr) = l_triple(OBJECT_DEFN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 28: /* classDefn: L_CLASS genClass L_WHERE classDeclOrDefnBody  */
                                                                        { pp(4, lex_classDefn);	((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 29: /* genClass: class  */
                                                                        { pp(1, lex_genClass);	((*yyvalp)._expr) = l_pair(l_Nil, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 30: /* genClass: classOrList L_EQ_GT class  */
                                                                { pp(3, lex_genClass);	((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 31: /* classOrList: class  */
                                                                        { pp(1, lex_classOrList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 32: /* classOrList: bracketedClassList  */
                                                                { pp(1, lex_classOrList);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 33: /* bracketedClassList: bracketedClass bracketedClassListTail  */
                                                        { pp(2, lex_bracketedClassList);	
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 34: /* bracketedClassOrList: bracketedClass  */
                                                                { pp(1, lex_bracketedClassOrList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 35: /* bracketedClassOrList: bracketedClassList  */
                                                                { pp(1, lex_bracketedClassOrList);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 36: /* bracketedClassListTail: L_COMMA bracketedClassOrList  */
                                                                { pp(2, lex_bracketedClassListTail);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 37: /* bracketedClass: L_LPAREN class L_RPAREN  */
                                                                { pp(3, lex_bracketedClass);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 38: /* class: classId typeSigCSList  */
                                                                { pp(2, lex_class); exitTypeSigContext();
									((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 39: /* classDeclOrDefnBody: classDeclOrDefn  */
                                                                { pp(1, lex_classDeclOrDefnBody);
	      											((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 40: /* classDeclOrDefnBody: L_LCURLYB classDeclOrDefnList L_RCURLYB  */
                                                                        { pp(3, lex_classDeclOrDefnBody);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 41: /* classDeclOrDefnList: classDeclOrDefn semiSeq classDeclOrDefnList  */
                                                                        { pp(3, lex_classDeclOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 42: /* classDeclOrDefnList: classDeclOrDefn optSemi  */
                                                                { pp(2, lex_classDeclOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 43: /* classDeclOrDefn: modifiedDefn  */
                                                            { pp(1, lex_classDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 44: /* classDeclOrDefn: objectDefn  */
                                                                    { pp(1, lex_classDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 45: /* classDeclOrDefn: decl  */
                                                                  { pp(1, lex_classDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 46: /* instanceDefn: L_INSTANCE classId instanceDefnTypes optInstanceDeclOrDefnBody  */
                                                                        { pp(4, lex_instanceDefn);	
													((*yyvalp)._expr) = l_triple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 47: /* instanceDefnTypes: typeSigCSList typeQuals  */
                                                                { pp(2, lex_instanceDefnTypes); exitTypeSigContext();
													((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 48: /* optInstanceDeclOrDefnBody: %empty  */
                                                                      { pp(0, lex_optInstanceDeclOrDefnBody);
													((*yyvalp)._expr) = l_Nil; }
    break;

  case 49: /* optInstanceDeclOrDefnBody: L_WHERE instanceDeclOrDefnBody  */
                                                        { pp(2, lex_optInstanceDeclOrDefnBody);  
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 50: /* instanceDeclOrDefnBody: instanceDeclOrDefn  */
                                                        { pp(1, lex_instanceDeclOrDefnBody);
	      											((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 51: /* instanceDeclOrDefnBody: L_LCURLYB instanceDeclOrDefnList L_RCURLYB  */
                                                                        { pp(3, lex_instanceDeclOrDefnBody);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 52: /* instanceDeclOrDefnList: instanceDeclOrDefn semiSeq instanceDeclOrDefnList  */
                                                                        { pp(3, lex_instanceDeclOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 53: /* instanceDeclOrDefnList: instanceDeclOrDefn optSemi  */
                                                                { pp(2, lex_instanceDeclOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 54: /* instanceDeclOrDefn: decl  */
                                                                { pp(1, lex_instanceDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 55: /* instanceDeclOrDefn: modifiedDefn  */
                                                                { pp(1, lex_instanceDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 56: /* instanceDeclOrDefn: dataConDecl  */
                                                                  { pp(1, lex_instanceDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 57: /* instanceDeclOrDefn: modifiedDataConDefn  */
                                                                { pp(1, lex_instanceDeclOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 58: /* dataConDecl: dataCon typeConstraint  */
                                                              { pp(2, lex_dataConDecl);
                                                                              ((*yyvalp)._expr) = l_triple(DATA_CON_DECL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 59: /* modifiedDataConDefn: L_INLINE dataConDefn  */
                                                                { pp(2, lex_modifiedDataConDefn);
                                                                        	((*yyvalp)._expr) = l_pair(INLINE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 60: /* modifiedDataConDefn: dataConDefn  */
                                                                  { pp(1, lex_modifiedDataConDefn);
		                                                                  ((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 61: /* dataConDefn: dataCon patterns L_EQ expr selectors  */
                                                                        { pp(4, lex_dataConDefn);
                                                                              ((*yyvalp)._expr) = l_triple(DATA_CON_DEFN, l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval._expr), defn((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr))), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 62: /* selectors: %empty  */
                                                                { pp(0, lex_selectors); ((*yyvalp)._expr) = l_Nil; }
    break;

  case 63: /* selectors: L_MATCHING selectorBody  */
                                                            { pp(2, lex_selectors); ((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 64: /* selectorBody: L_LCURLYB selectorList L_RCURLYB  */
                                                        { pp(3, lex_selectorBody);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 65: /* selectorList: selector semiSeq selectorList  */
                                                                        { pp(3, lex_selectorList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 66: /* selectorList: selector optSemi  */
                                                                    { pp(2, lex_selectorList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 67: /* selector: expr  */
                                                                        { pp(1, lex_selector);	((*yyvalp)._expr) = l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 68: /* selector: L_INLINE expr  */
                                                            { pp(1, lex_selector);	((*yyvalp)._expr) = l_pair(INLINE, l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 69: /* declOrDefnBody: declOrDefn  */
                                                        { pp(1, lex_declOrDefnBody);
	      											((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 70: /* declOrDefnBody: L_LCURLYB declOrDefnList L_RCURLYB  */
                                                                        { pp(3, lex_declOrDefnBody);												((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 71: /* declOrDefnList: declOrDefn semiSeq declOrDefnList  */
                                                                        { pp(3, lex_declOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 72: /* declOrDefnList: declOrDefn optSemi  */
                                                                { pp(2, lex_declOrDefnList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 73: /* declOrDefn: decl  */
                                                                { pp(1, lex_declOrDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 74: /* declOrDefn: modifiedDefn  */
                                                              { pp(1, lex_declOrDefn);	
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 75: /* primDecl: L_PRIMITIVE decl  */
                                                                        { pp(2, lex_primDecl);	((*yyvalp)._expr) = l_pair(PRIMITIVE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 76: /* primDecl: decl  */
                                                                { pp(1, lex_primDecl);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 77: /* decl: signedId  */
                                                                        { pp(1, lex_decl);	((*yyvalp)._expr) = l_pair(DECL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 78: /* modifiedDefn: L_INLINE defn  */
                                                                { pp(2, lex_modifiedDefn);
													((*yyvalp)._expr) = l_pair(INLINE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 79: /* modifiedDefn: defn  */
                                                                { pp(1, lex_modifiedDefn);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 80: /* defn: signedId L_EQ expr  */
                                                                { pp(3, lex_defn);	((*yyvalp)._expr) = l_triple(DEFN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), defn(Nil, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 81: /* defn: idOrPrefixOp patternList L_EQ expr  */
                                                                { pp(4, lex_defn);	((*yyvalp)._expr) = l_triple(DEFN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval._expr), defn((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 82: /* defn: bracketedSignedId patterns L_EQ expr  */
                                                        { pp(4, lex_defn);	((*yyvalp)._expr) = l_triple(DEFN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval._expr), defn((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 83: /* defCmdBody: defCmd  */
                                                              { pp(1, lex_defCmdBody);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 84: /* defCmdBody: L_CMD_LCURLYB defCmdList L_RCURLYB  */
                                                                { pp(3, lex_defCmdBody);
													((*yyvalp)._expr) = l_sequence((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)); }
    break;

  case 85: /* defCmdList: defCmd semiSeq defCmdList  */
                                                                { pp(3, lex_defCmdList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 86: /* defCmdList: defCmd optSemi  */
                                                              { pp(2, lex_defCmdList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 87: /* defCmd: selExpr L_EQ expr  */
                                                                { pp(3, lex_defCmd);	((*yyvalp)._expr) = l_ap(l_reduce(l_ap(l_reduce(l_ap(l_useName("addBinding"), l_arg(l_snd(l_fun((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)))))), l_arg((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)))), l_pair(COERCE, l_eval((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)))); }
    break;

  case 88: /* undefCmdBody: undefCmd  */
                                                                { pp(1, lex_undefCmdBody);
            											((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 89: /* undefCmdBody: L_CMD_LCURLYB undefCmdList L_RCURLYB  */
                                                        { pp(3, lex_undefCmdBody);
													((*yyvalp)._expr) = l_sequence((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)); }
    break;

  case 90: /* undefCmdList: undefCmd semiSeq undefCmdList  */
                                                                { pp(3, lex_undefCmdList);
	      											((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 91: /* undefCmdList: undefCmd optSemi  */
                                                                        { pp(2, lex_undefCmdList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 92: /* undefCmd: selExpr  */
                                                                { pp(1, lex_undefCmd);	((*yyvalp)._expr) = l_ap(l_reduce(l_ap(l_useName("removeBinding"), l_arg(l_snd(l_fun((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)))))), l_arg((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 93: /* patterns: %empty  */
                                                                        { pp(0, lex_patterns);	((*yyvalp)._expr) = l_Nil; }
    break;

  case 94: /* patterns: patternList  */
                                                                        { pp(1, lex_patterns);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 95: /* patternList: aPattern  */
                                                                        { pp(1, lex_patternList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 96: /* patternList: aPattern patternList  */
                                                                { pp(2, lex_patternList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 97: /* pattern: binPattern  */
                                                                        { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 98: /* pattern: plainPattern  */
                                                                { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 99: /* pattern: tuplePattern  */
                                                                { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 100: /* pattern: tuplePtrDeconPattern  */
                                                                { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 101: /* pattern: ptrPattern  */
                                                                        { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 102: /* pattern: dataConPtrDeconPattern  */
                                                                { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 103: /* pattern: dataConPattern  */
                                                                { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 104: /* pattern: concretePattern  */
                                                                { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 105: /* pattern: constrainedPattern  */
                                                                { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 106: /* pattern: aPattern  */
                                                                        { pp(1, lex_pattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 107: /* binPattern: idName L_COLON pattern  */
                                                                { pp(3, lex_binPattern);
													((*yyvalp)._expr) = l_triple(DATA_CON_PATTERN, l_Bind, l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil))); }
    break;

  case 108: /* binPattern: pattern L_COLON_PLUS pattern  */
                                                                { pp(3, lex_binPattern);
													((*yyvalp)._expr) = l_triple(DATA_CON_PATTERN, l_Cons, l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil))); }
    break;

  case 109: /* plainPattern: L_TILDE tuplePattern  */
                                                                { pp(2, lex_plainPattern);
													((*yyvalp)._expr) = l_pair(PLAIN_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 110: /* plainPattern: L_TILDE_AT tuplePattern  */
                                                                { pp(1, lex_plainPattern);
													((*yyvalp)._expr) = l_pair(PLAIN_PATTERN, l_pair(PTR_DECON_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 111: /* plainPattern: L_TILDE ptrPattern  */
                                                                { pp(2, lex_plainPattern);
													((*yyvalp)._expr) = l_pair(PLAIN_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 112: /* plainPattern: L_TILDE_AT dataCon pattern  */
                                                                { pp(3, lex_plainPattern);
													((*yyvalp)._expr) = l_pair(PLAIN_PATTERN, l_pair(PTR_DECON_PATTERN, l_triple(CONCRETE_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil)))); }
    break;

  case 113: /* plainPattern: L_TILDE_TILDE dataCon pattern  */
                                                                { pp(3, lex_plainPattern);
													((*yyvalp)._expr) = l_pair(PLAIN_PATTERN, l_triple(CONCRETE_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil))); }
    break;

  case 114: /* tuplePtrDeconPattern: L_AT tuplePattern  */
                                                                { pp(2, lex_tuplePtrDeconPattern);
													((*yyvalp)._expr) = l_pair(PTR_DECON_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 115: /* tuplePattern: L_LPAREN patternComps L_RPAREN  */
                                                        { pp(3, lex_tuplePattern);
													((*yyvalp)._expr) = l_pair(TUPLE_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)); }
    break;

  case 116: /* patternComps: patternComps L_COMMA pattern  */
                                                                { pp(3, lex_patternComps);
													((*yyvalp)._expr) = l_concat((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil)); }
    break;

  case 117: /* patternComps: pattern L_COMMA pattern  */
                                                                { pp(3, lex_patternComps);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil)); }
    break;

  case 118: /* ptrPattern: ptr pattern  */
                                                                        { pp(2, lex_ptrPattern);
													((*yyvalp)._expr) = l_triple(DATA_CON_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil)); }
    break;

  case 119: /* dataConPtrDeconPattern: L_AT dataCon pattern  */
                                                                { pp(2, lex_dataConPtrDeconPattern);
													((*yyvalp)._expr) = l_pair(PTR_DECON_PATTERN, l_triple(CONCRETE_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil))); }
    break;

  case 120: /* dataConPattern: dataCon patternList  */
                                                                { pp(2, lex_dataConPattern);
													((*yyvalp)._expr) = l_triple(DATA_CON_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 121: /* concretePattern: L_TILDE dataCon pattern  */
                                                                        { pp(3, lex_concretePattern);
													((*yyvalp)._expr) = l_triple(CONCRETE_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil)); }
    break;

  case 122: /* constrainedPattern: aPattern typeConstraint  */
                                                              { pp(2, lex_pattern);	((*yyvalp)._expr) = l_triple(CONSTRAIN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 123: /* varPattern: idName  */
                                                                { pp(1, lex_varPattern);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 124: /* varPattern: wildcard  */
                                                      { pp(1, lex_varPattern);
													((*yyvalp)._expr) = WILDCARD_PATTERN; }
    break;

  case 125: /* aPattern: atom  */
                                                                        { pp(1, lex_aPattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 126: /* aPattern: dataCon  */
                                                                        { pp(1, lex_aPattern);	((*yyvalp)._expr) = l_triple(DATA_CON_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 127: /* aPattern: L_TILDE dataCon  */
                                                                        { pp(2, lex_aPattern);	((*yyvalp)._expr) = l_pair(PLAIN_PATTERN, l_triple(DATA_CON_PATTERN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil)); }
    break;

  case 128: /* aPattern: varPattern  */
                                                      { pp(1, lex_aPattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 129: /* aPattern: L_LSQB L_RSQB  */
                                                              { pp(2, lex_aPattern);	((*yyvalp)._expr) = l_triple(DATA_CON_PATTERN, l_useName("Nil"), l_Nil); }
    break;

  case 130: /* aPattern: L_LPAREN pattern L_RPAREN  */
                                                                { pp(3, lex_aPattern);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 131: /* typeConstraint: L_COLON_COLON typeSig  */
                                                                        { pp(2, lex_typeConstraint); exitTypeSigContext();
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 132: /* binTypeSig: typeSig L_COLON typeSig  */
                                                                { pp(3, lex_binTypeSig);
													((*yyvalp)._expr) = l_ap2(l_useName("Binding"), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 133: /* binTypeSig: typeSig L_TYPESIG_HYPHEN_GT typeSig  */
                                                                { pp(3, lex_binTypeSig);
													((*yyvalp)._expr) = l_ap2(ARROW, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 134: /* monTypeSig: L_TYPESIG_HYPHEN_GT typeSig  */
                                                                        { pp(2, lex_monTypeSig); 
													((*yyvalp)._expr) = l_ap(ARROW, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 135: /* apTypeSig: aTypeSig aTypeSig  */
                                                                        { pp(2, lex_apTypeSig);	((*yyvalp)._expr) = l_ap((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 136: /* apTypeSig: apTypeSig aTypeSig  */
                                                                { pp(2, lex_apTypeSig);	((*yyvalp)._expr) = l_ap((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 137: /* typeSig: apTypeSig  */
                                                                        { pp(1, lex_typeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 138: /* typeSig: monTypeSig  */
                                                                        { pp(1, lex_typeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 139: /* typeSig: binTypeSig  */
                                                                        { pp(1, lex_typeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 140: /* typeSig: aTypeSig  */
                                                                        { pp(1, lex_typeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 141: /* aTypeSig: typeCon  */
                                                                { pp(1, lex_aTypeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 142: /* aTypeSig: typeVar  */
                                                                { pp(1, lex_aTypeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 143: /* aTypeSig: tag  */
                                                    { pp(1, lex_aTypeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 144: /* aTypeSig: L_LPAREN typeSigCompList L_RPAREN  */
                                                                { pp(3, lex_aTypeSig);	((*yyvalp)._expr) = l_apTuple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), 0, FALSE); }
    break;

  case 145: /* aTypeSig: L_LSQB typeSig L_RSQB  */
                                                                { pp(3, lex_aTypeSig);	((*yyvalp)._expr) = l_ap(l_useName("List"), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)); }
    break;

  case 146: /* aTypeSig: L_LPAREN typeSig L_RPAREN  */
                                                                { pp(3, lex_aTypeSig);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 147: /* typeSigCompList: typeSigCompList L_COMMA typeSig  */
                                                                        { pp(3, lex_typeSigCompList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)); }
    break;

  case 148: /* typeSigCompList: typeSig L_COMMA typeSig  */
                                                                        { pp(3, lex_typeSigCompList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_Nil)); }
    break;

  case 149: /* typeSigCSList: typeSig  */
                                                                { pp(1, lex_typeSigCSList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), Nil); }
    break;

  case 150: /* typeSigCSList: typeSig L_COMMA typeSigCSList  */
                                                                { pp(3, lex_typeSigCSList);
													
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 151: /* typeQuals: %empty  */
                                                                { pp(0, lex_typeQuals);	((*yyvalp)._expr) = l_Nil; }
    break;

  case 152: /* typeQuals: L_VERTBAR typeQualOrList  */
                                                                { pp(2, lex_typeQuals);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 153: /* typeQualOrList: typeQual  */
                                                                        { pp(1, lex_typeQualOrList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 154: /* typeQualOrList: bTypeQualList  */
                                                                { pp(1, lex_typeQualOrList);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 155: /* bTypeQualList: bTypeQual bTypeQualListTail  */
                                                                { pp(2, lex_bTypeQualList);	
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 156: /* bTypeQualOrList: bTypeQual  */
                                                                        { pp(1, lex_bTypeQualOrList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 157: /* bTypeQualOrList: bTypeQualList  */
                                                                { pp(1, lex_bTypeQualOrList);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 158: /* bTypeQualListTail: L_COMMA bTypeQualOrList  */
                                                                { pp(2, lex_bTypeQualListTail);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 159: /* typeQual: bTypeQual  */
                                                                        { pp(1, lex_typeQual);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 160: /* typeQual: instanceTypeQual  */
                                                                        { pp(1, lex_typeQual);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 161: /* dyadicTypeQual: typeSig predRelOp typeSig  */
                                                          { pp(3, lex_dyadicTypeQual);
													((*yyvalp)._expr) = l_triple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 162: /* dyadicTypeQual: bTypeQual predAndOp bTypeQual  */
                                                                        { pp(3, lex_dyadicTypeQual);
													((*yyvalp)._expr) = l_triple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 163: /* dyadicTypeQual: bTypeQual predOrOp bTypeQual  */
                                                                        { pp(3, lex_dyadicTypeQual);
													((*yyvalp)._expr) = l_triple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 164: /* monadicTypeQual: L_EXCLMARK typeVar  */
                                                        { pp(2, lex_monadicTypeQual);
													((*yyvalp)._expr) = l_pair(STRICT, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 165: /* monadicTypeQual: predNotOp bTypeQual  */
                                                        { pp(3, lex_monadicTypeQual);
													((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 166: /* instanceTypeQual: L_INSTANCE classId typeSigCSList  */
                                                          { pp(3, lex_instanceTypeQual);  
													((*yyvalp)._expr) = l_triple(INSTANCE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 167: /* bTypeQual: dyadicTypeQual  */
                                                                { pp(1, lex_typeQual);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 168: /* bTypeQual: monadicTypeQual  */
                                                                { pp(1, lex_typeQual);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 169: /* bTypeQual: aTypeQual  */
                                                                        { pp(1, lex_typeQual);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 170: /* aTypeQual: L_LPAREN typeQual L_RPAREN  */
                                                        { pp(3, lex_aTypeQual);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 171: /* predRelOp: L_EQ  */
                                                                        { pp(1, lex_predRelOp);	((*yyvalp)._expr) = EQUALITY; }
    break;

  case 172: /* predRelOp: L_NOTSIGN_EQ  */
                                                                        { pp(1, lex_predRelOp);	((*yyvalp)._expr) = INEQUALITY; }
    break;

  case 173: /* predAndOp: predAndAlts  */
                                                                { pp(1, lex_predAndOp);
													((*yyvalp)._expr) = CONJUNCTION; }
    break;

  case 174: /* predAndAlts: L_AND  */
                                                                    { pp(1, lex_predAndAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 175: /* predAndAlts: L_AMPERSAND  */
                                                            { pp(1, lex_predAndAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 176: /* predOrOp: predOrAlts  */
                                                                        { pp(1, lex_predOrOp);	((*yyvalp)._expr) = DISJUNCTION; }
    break;

  case 177: /* predOrAlts: L_OR  */
                                                                      { pp(1, lex_predOrAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 178: /* predOrAlts: L_VERTBAR  */
                                                                      { pp(1, lex_predOrAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 179: /* predNotOp: predNotAlts  */
                                                                { pp(1, lex_predNotOp);	((*yyvalp)._expr) = NEGATION; }
    break;

  case 180: /* predNotAlts: L_NOT  */
                                                                { pp(1, lex_predNotAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 181: /* predNotAlts: L_NOTSIGN  */
                                                                      { pp(1, lex_predNotAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 182: /* dataDecls: dataDecl L_VERTBAR dataDecls  */
                                                            { pp(3, lex_dataDecls);  ((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 183: /* dataDecls: dataDecl  */
                                                                        { pp(1, lex_dataDecls);  ((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 184: /* dataDecl: dataCon aTypeSigs  */
                                                       { pp(2, lex_dataDecl);	((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 185: /* dataDecl: ptr aTypeSigs  */
                                                         { pp(2, lex_dataDecl);	((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 186: /* aTypeSigs: %empty  */
                                                                        { pp(0, lex_aTypeSigs);	((*yyvalp)._expr) = l_Nil; }
    break;

  case 187: /* aTypeSigs: aTypeSigList  */
                                                                { pp(1, lex_aTypeSigs);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 188: /* aTypeSigList: aTypeSig aTypeSigList  */
                                                                { pp(2, lex_aTypeSigList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 189: /* aTypeSigList: aTypeSig  */
                                                                        { pp(1, lex_aTypeSigList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 190: /* retExpr: L_RETURN expr  */
                                                                { pp(2, lex_retExpr);	((*yyvalp)._expr) = l_pair(RETURN, l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 191: /* retExpr: L_RETURN  */
                                                              { pp(1, lex_retExpr);	((*yyvalp)._expr) = RETURN; }
    break;

  case 192: /* defExpr: L_DEF defCmdBody  */
                                                                        { pp(2, lex_defExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 193: /* undefExpr: L_UNDEF undefCmdBody  */
                                                                { pp(2, lex_undefExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 194: /* seqExpr: expr  */
                                                                        { pp(1, lex_seqExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 195: /* seqExpr: retExpr  */
                                                                { pp(1, lex_seqExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 196: /* expr: apExpr  */
                                                                { pp(1, lex_expr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 197: /* expr: selExpr  */
                                                                { pp(1, lex_expr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 198: /* expr: selDataCon  */
                                                                        { pp(1, lex_expr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 199: /* expr: monExpr  */
                                                                { pp(1, lex_expr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 200: /* expr: binExpr  */
                                                                { pp(1, lex_expr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 201: /* expr: genExpr  */
                                                                { pp(1, lex_expr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 202: /* expr: bExpr  */
                                                                        { pp(1, lex_expr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 203: /* expr: expr typeConstraint  */
                                                                { pp(2, lex_expr);	((*yyvalp)._expr) = l_noCoerce(l_triple(CONSTRAIN, l_constrain((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 204: /* genExpr: L_BACKSLASH patterns L_HYPHEN_GT expr  */
                                                                        { pp(4, lex_genExpr);	((*yyvalp)._expr) = l_triple(LAMBDA, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 205: /* genExpr: L_LET declOrDefnBody L_IN expr  */
                                                                        { pp(4, lex_genExpr);	((*yyvalp)._expr) = l_triple(LET_REC, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 206: /* genExpr: L_IF expr L_THEN expr L_ELSE expr  */
                                                                        { pp(6, lex_genExpr);	((*yyvalp)._expr) = l_pair(IF, l_triple(l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval._expr)), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)))); }
    break;

  case 207: /* genExpr: L_IF expr L_THEN expr  */
                                                                        { pp(4, lex_genExpr);	((*yyvalp)._expr) = l_pair(IF, l_pair(l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)))); }
    break;

  case 208: /* genExpr: defExpr  */
                                                                { pp(1, lex_genExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 209: /* genExpr: undefExpr  */
                                                                        { pp(1, lex_genExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 210: /* apExpr: bExpr bExpr  */
                                                                { pp(1, lex_apExpr);	((*yyvalp)._expr) = l_genAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 211: /* apExpr: apExpr bExpr  */
                                                                { pp(2, lex_apExpr);	((*yyvalp)._expr) = l_genAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 212: /* binExpr: expr expOp expr  */
                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 213: /* binExpr: expr mulDivOp expr  */
                                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 214: /* binExpr: expr concatOp expr  */
                                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 215: /* binExpr: expr addSubOp expr  */
                                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 216: /* binExpr: expr shiftOp expr  */
                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 217: /* binExpr: expr bitAndOp expr  */
                                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 218: /* binExpr: expr bitXorOp expr  */
                                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 219: /* binExpr: expr bitOrOp expr  */
                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 220: /* binExpr: expr relOp expr  */
                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 221: /* binExpr: expr logAndOp expr  */
                                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 222: /* binExpr: expr logOrOp expr  */
                                                        { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 223: /* binExpr: name L_COLON expr  */
                                                                { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_ap(l_reduce(l_ap(l_reduce(l_Bind), l_pair(LITERAL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)))), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 224: /* binExpr: expr L_COLON_PLUS expr  */
                                                                { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp(l_reduce(l_Cons), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 225: /* binExpr: expr L_AT_EQ expr  */
                                                                { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_binAp(l_useName("(@=)"), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)); }
    break;

  case 226: /* binExpr: selExpr L_COLON_EQ expr  */
                                                                { pp(3, lex_binExpr);	((*yyvalp)._expr) = l_ap3(l_useName("set"), l_arg(l_fun((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr))), l_arg((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 227: /* monExpr: L_COLON_COLON aExpr  */
                                                                        { pp(2, lex_monExpr);	exitTypeSigContext();
													((*yyvalp)._expr) = l_pair(COERCE, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 228: /* monExpr: attribute expr  */
                                                          { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 229: /* monExpr: modifier expr  */
                                                                { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_noCoerce(l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)))); }
    break;

  case 230: /* monExpr: ptr expr  */
                                                                { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_monApNoCoerce(l_reduce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)), l_mapToConsEnv((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 231: /* monExpr: L_TILDE ptr expr  */
                                                                { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_monApNoCoerce(l_pair(PLAIN_DATA_CON, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 232: /* monExpr: L_HYPHEN expr  */
                                                                { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_monAp(l_useName("negate"), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 233: /* monExpr: L_PLUS expr  */
                                                        { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_monAp(l_useName("plus"), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 234: /* monExpr: notOp expr  */
                                                                { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_monAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 235: /* monExpr: bitwiseNotOp expr  */
                                                                { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_monAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 236: /* monExpr: dePtrOp expr  */
                                                        { pp(2, lex_monExpr);	((*yyvalp)._expr) = l_monAp((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 237: /* specialExpr: L_NUMSIGN_EXCLMARK aExpr  */
                                                                { pp(2, lex_specialExpr);
													((*yyvalp)._expr) = l_reduce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 238: /* specialExpr: L_EXCLMARK aExpr  */
                                                                        { pp(2, lex_specialExpr);
													((*yyvalp)._expr) = l_eval((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 239: /* specialExpr: L_NOTSIGN_COLON_COLON aExpr  */
                                                              { pp(2, lex_specialExpr);
													((*yyvalp)._expr) = l_noCoerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 240: /* specialExpr: L_LPAREN_CARET_RPAREN aExpr  */
                                                                { pp(2, lex_specialExpr);
													((*yyvalp)._expr) = l_mapToConsEnv((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 241: /* specialExpr: L_NOTSIGN_CARET aExpr  */
                                                            { pp(2, lex_specialExpr);
													((*yyvalp)._expr) = l_pair(NO_MAP_TO_ENV, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 242: /* sel: aExpr selOp  */
                                                                        { pp(2, lex_sel);		((*yyvalp)._expr) = l_ap((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr))); }
    break;

  case 243: /* sel: selExpr selOp  */
                                                                { pp(2, lex_sel);		((*yyvalp)._expr) = l_ap((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_reduce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr))); }
    break;

  case 244: /* sel: selOp  */
                                                                { pp(1, lex_sel);		((*yyvalp)._expr) = l_ap((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_root); }
    break;

  case 245: /* selExpr: sel idName  */
                                                                        { pp(2, lex_selExpr);	((*yyvalp)._expr) = l_ap(l_reduce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)), l_pair(LITERAL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 246: /* selDataCon: sel dataCon  */
                                                                        { pp(2, lex_selDataCon);
													((*yyvalp)._expr) = l_ap((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 247: /* bExpr: aExpr  */
                                                                { pp(1, lex_bExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 248: /* bExpr: specialExpr  */
                                                                        { pp(1, lex_bExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 249: /* aExpr: atom  */
                                                                        { pp(1, lex_aExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 250: /* aExpr: idOrPrefixOp  */
                                                                { pp(1, lex_aExpr);	((*yyvalp)._expr) = l_noCoerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 251: /* aExpr: dataCon  */
                                                                { pp(1, lex_aExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 252: /* aExpr: L_TILDE dataCon  */
                                                                { pp(1, lex_aExpr);	((*yyvalp)._expr) = l_noCoerce(l_pair(CONCRETE_DATA_CON, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 253: /* aExpr: L_TILDE_TILDE dataCon  */
                                                                { pp(1, lex_aExpr);	((*yyvalp)._expr) = l_noCoerce(l_pair(CONCRETE_DATA_CON, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 254: /* aExpr: L_LPAREN compList L_RPAREN  */
                                                                { pp(3, lex_aExpr);	((*yyvalp)._expr) = l_apTuple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), 0, TRUE); }
    break;

  case 255: /* aExpr: L_TILDE L_LPAREN compList L_RPAREN  */
                                                                { pp(3, lex_aExpr);	((*yyvalp)._expr) = l_apTuple((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), 0, TRUE); }
    break;

  case 256: /* aExpr: L_LSQB exprs L_RSQB  */
                                                                { pp(3, lex_aExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 257: /* aExpr: L_LSQB expr L_SEMICOLON qualList L_RSQB  */
                                                        { pp(5, lex_aExpr);	((*yyvalp)._expr) = l_triple(LISTCOMP, l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-3)].yystate.yysemantics.yyval._expr)), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)); }
    break;

  case 258: /* aExpr: L_LCURLYB sequence L_RCURLYB  */
                                                                { pp(3, lex_aExpr);	((*yyvalp)._expr) = l_sequence((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)); }
    break;

  case 259: /* aExpr: L_CASE expr L_OF L_LCURLYB caseAltList L_RCURLYB  */
                                                                        { pp(6, lex_aExpr);	((*yyvalp)._expr) =  l_triple(CASE, l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-4)].yystate.yysemantics.yyval._expr)), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr)); }
    break;

  case 260: /* aExpr: L_LPAREN expr L_RPAREN  */
                                                                { pp(3, lex_aExpr);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 261: /* aExpr: L_THIS  */
                                                                { pp(1, lex_aExpr);	((*yyvalp)._expr) = THIS; }
    break;

  case 262: /* prefixOp: L_LPAREN dyadicOp L_RPAREN  */
                                                                { pp(3, lex_prefixOp);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 263: /* selOp: L_DOT  */
                                                                        { pp(1, lex_selOp);	((*yyvalp)._expr) = l_useName("(.)"); }
    break;

  case 264: /* expOp: L_CARET  */
                                                                { pp(1, lex_expOp);	((*yyvalp)._expr) = l_useName("pow"); }
    break;

  case 265: /* mulDivOp: L_ASTERISK  */
                                                                        { pp(1, lex_mulDivOp);	((*yyvalp)._expr) = l_useName("(*)"); }
    break;

  case 266: /* mulDivOp: L_SLASH  */
                                                                { pp(1, lex_mulDivOp);	((*yyvalp)._expr) = l_useName("(/)"); }
    break;

  case 267: /* mulDivOp: L_DIV  */
                                                                        { pp(1, lex_mulDivOp);	((*yyvalp)._expr) = l_useName("(div)"); }
    break;

  case 268: /* mulDivOp: L_MOD  */
                                                                        { pp(1, lex_mulDivOp);	((*yyvalp)._expr) = l_useName("(mod)"); }
    break;

  case 269: /* concatOp: L_PLUS_PLUS  */
                                                                        { pp(1, lex_concatOp);	((*yyvalp)._expr) = l_useName("(++)"); }
    break;

  case 270: /* addSubOp: L_PLUS  */
                                                                { pp(1, lex_addSubOp);	((*yyvalp)._expr) = l_useName("(+)"); }
    break;

  case 271: /* addSubOp: L_HYPHEN  */
                                                                        { pp(1, lex_addSubOp);	((*yyvalp)._expr) = l_useName("(-)"); }
    break;

  case 272: /* relOp: L_LT  */
                                                                { pp(1, lex_relOp);	((*yyvalp)._expr) = l_useName("(<)"); }
    break;

  case 273: /* relOp: L_LT_EQ  */
                                                                { pp(1, lex_relOp);	((*yyvalp)._expr) = l_useName("(<=)"); }
    break;

  case 274: /* relOp: L_EQ  */
                                                                        { pp(1, lex_relOp);	((*yyvalp)._expr) = l_useName("(=)"); }
    break;

  case 275: /* relOp: L_NOTSIGN_EQ  */
                                                                { pp(1, lex_relOp);	((*yyvalp)._expr) = l_useName("(¬=)"); }
    break;

  case 276: /* relOp: L_GT_EQ  */
                                                                { pp(1, lex_relOp);	((*yyvalp)._expr) = l_useName("(>=)"); }
    break;

  case 277: /* relOp: L_GT  */
                                                                        { pp(1, lex_relOp);	((*yyvalp)._expr) = l_useName("(>)"); }
    break;

  case 278: /* logAndOp: logAndAlts  */
                                                                        { pp(1, lex_logAndOp);	((*yyvalp)._expr) = l_useName("(&)"); }
    break;

  case 279: /* logAndAlts: L_AND  */
                                                                    { pp(1, lex_logAndAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 280: /* logAndAlts: L_AMPERSAND  */
                                                            { pp(1, lex_logAndAlts);
													((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 281: /* logOrOp: logOrAlts  */
                                                                        { pp(1, lex_logOrOp);	((*yyvalp)._expr) = l_useName("(|)"); }
    break;

  case 282: /* logOrAlts: L_OR  */
                                                                      { pp(1, lex_logOrAlts);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 283: /* logOrAlts: L_VERTBAR  */
                                                                      { pp(1, lex_logOrAlts);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 284: /* shiftOp: L_LT_LT  */
                                                                { pp(1, lex_shiftOp);	((*yyvalp)._expr) = l_useName("(<<)"); }
    break;

  case 285: /* shiftOp: L_GT_GT  */
                                                                { pp(1, lex_shiftOp);	((*yyvalp)._expr) = l_useName("(>>)"); }
    break;

  case 286: /* bitAndOp: L_NUMSIGN_AMPERSAND  */
                                                        { pp(1, lex_bitAndOp);	((*yyvalp)._expr) = l_useName("(#&)"); }
    break;

  case 287: /* bitXorOp: L_NUMSIGN_CARET  */
                                                                { pp(1, lex_bitXorOp);	((*yyvalp)._expr) = l_useName("(#^)"); }
    break;

  case 288: /* bitOrOp: L_NUMSIGN_VERTBAR  */
                                                                { pp(1, lex_bitOrOp);	((*yyvalp)._expr) = l_useName("(#|)"); }
    break;

  case 289: /* assignOp: L_AT_EQ  */
                                                                { pp(1, lex_assignOp);	((*yyvalp)._expr) = l_useName("(@=)"); }
    break;

  case 290: /* dyadicOp: selOp  */
                                                                { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 291: /* dyadicOp: expOp  */
                                                                        { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 292: /* dyadicOp: mulDivOp  */
                                                                        { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 293: /* dyadicOp: concatOp  */
                                                          { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 294: /* dyadicOp: addSubOp  */
                                                          { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 295: /* dyadicOp: relOp  */
                                                                        { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 296: /* dyadicOp: logAndOp  */
                                                          { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 297: /* dyadicOp: logOrOp  */
                                                          { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 298: /* dyadicOp: shiftOp  */
                                                  { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 299: /* dyadicOp: bitAndOp  */
                                                  { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 300: /* dyadicOp: bitXorOp  */
                                                  { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 301: /* dyadicOp: bitOrOp  */
                                                  { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 302: /* dyadicOp: assignOp  */
                                                  { pp(1, lex_dyadicOp);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 303: /* notOp: notAlts  */
                                                                { pp(1, lex_notOp);	((*yyvalp)._expr) = l_useName("(not)"); }
    break;

  case 304: /* notAlts: L_NOT  */
                                                                { pp(1, lex_notAlts);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 305: /* notAlts: L_NOTSIGN  */
                                                                      { pp(1, lex_notAlts);	((*yyvalp)._expr) = ((*yyvalp)._expr); }
    break;

  case 306: /* bitwiseNotOp: L_NUMSIGN_NOTSIGN  */
                                                                        { pp(1, lex_bitwiseNotOp);	
													((*yyvalp)._expr) = l_useName("(#¬)"); }
    break;

  case 307: /* dePtrOp: L_AT  */
                                                                        { pp(1, lex_dePtrOp);	((*yyvalp)._expr) = l_useName("dePtr"); }
    break;

  case 308: /* monadicOp: notOp  */
                                                                { pp(1, lex_monadicOp);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 309: /* monadicOp: bitwiseNotOp  */
                                                                { pp(1, lex_monadicOp);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 310: /* monadicOp: dePtrOp  */
                                                                { pp(1, lex_monadicOp);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 311: /* exprs: %empty  */
                                                                { pp(0, lex_exprs);	((*yyvalp)._expr) = l_useName("Nil"); }
    break;

  case 312: /* exprs: exprList  */
                                                                        { pp(1, lex_exprs);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 313: /* exprList: expr L_COMMA exprList  */
                                                                { pp(3, lex_exprList);	((*yyvalp)._expr) = l_ap(l_reduce(l_ap(l_reduce(l_Cons), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)))), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 314: /* exprList: expr  */
                                                                        { pp(1, lex_exprList);	((*yyvalp)._expr) = l_ap(l_reduce(l_ap(l_reduce(l_Cons), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)))), l_useName("Nil")); }
    break;

  case 315: /* compList: compList L_COMMA expr  */
                                                                { pp(3, lex_compList);	((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr)); }
    break;

  case 316: /* compList: expr L_COMMA expr  */
                                                                        { pp(3, lex_compList);	((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_Nil)); }
    break;

  case 317: /* caseAltList: caseAlt semiSeq caseAltList  */
                                                                { pp(3, lex_caseAltList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 318: /* caseAltList: caseAlt optSemi  */
                                                                { pp(2, lex_caseAltList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 319: /* caseAlt: casePatternList L_HYPHEN_GT expr  */
                                                                { pp(3, lex_caseAlt);	enterPendingArrowContext();									
													((*yyvalp)._expr) = l_pair((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 320: /* casePatternList: casePattern L_COMMA casePatternList  */
                                                                { pp(3, lex_casePatternList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 321: /* casePatternList: casePattern  */
                                                                        { pp(1, lex_casePatternList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 322: /* casePattern: pattern  */
                                                                { pp(1, lex_casePattern);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 323: /* casePattern: L_OTHERWISE  */
                                                                        { pp(1, lex_casePattern);
													((*yyvalp)._expr) = OTHERWISE; }
    break;

  case 324: /* qual: idName L_LT_HYPHEN expr  */
                                                                { pp(3, lex_qual);	((*yyvalp)._expr) = l_triple(GENERATOR, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 325: /* qual: expr  */
                                                                        { pp(1, lex_qual);	((*yyvalp)._expr) = l_pair(FILTER, l_coerce((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr))); }
    break;

  case 326: /* qualList: qual L_COMMA qualList  */
                                                                { pp(3, lex_qualList);	((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-2)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 327: /* qualList: qual  */
                                                                        { pp(1, lex_qualList);	((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 328: /* typeVars: %empty  */
                                                                        { pp(0, lex_typeVars);	((*yyvalp)._expr) = l_Nil; }
    break;

  case 329: /* typeVars: typeVarList  */
                                                                        { pp(1, lex_typeVars);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 330: /* typeVarList: typeVar typeVarList  */
                                                                { pp(2, lex_typeVarList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr) ); }
    break;

  case 331: /* typeVarList: typeVar  */
                                                                { pp(1, lex_typeVarList);
													((*yyvalp)._expr) = l_cons((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr), l_Nil); }
    break;

  case 332: /* idOrPrefixOp: idName  */
                                                                { pp(1, lex_idOrPrefixOp);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 333: /* idOrPrefixOp: prefixOp  */
                                                                        { pp(1, lex_idOrPrefixOp);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 334: /* idOrPrefixOp: L_LPAREN monadicOp L_RPAREN  */
                                                        { pp(3, lex_idOrPrefixOp);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 335: /* bracketedSignedId: L_LPAREN signedId L_RPAREN  */
                                                                { pp(3, lex_bracketedSignedId);
													((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr); }
    break;

  case 336: /* signedId: idOrPrefixOp  */
                                                                { pp(1, lex_signedId);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 337: /* signedId: idOrPrefixOp typeConstraint  */
                                                                { pp(2, lex_signedId);	((*yyvalp)._expr) = l_triple(CONSTRAIN, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (-1)].yystate.yysemantics.yyval._expr), (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 338: /* idName: L_LC_NAME  */
                                                              { pp(1, lex_idName);	((*yyvalp)._expr) = l_useName((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._string)); }
    break;

  case 339: /* name: L_LC_NAME  */
                                                                      { pp(1, lex_name);	((*yyvalp)._expr) = l_useName((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._string)); }
    break;

  case 340: /* name: L_UC_NAME  */
                                                                      { pp(1, lex_name);	((*yyvalp)._expr) = l_useName((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._string)); }
    break;

  case 341: /* wildcard: L_UNDERSCORE  */
                                                                { pp(1, lex_wildcard);	((*yyvalp)._expr) = WILDCARD_PATTERN; }
    break;

  case 342: /* modifier: L_CONSTANT  */
                                                                        { pp(1, lex_modifier);	((*yyvalp)._expr) = CONSTANT; }
    break;

  case 343: /* modifier: L_VARIABLE  */
                                                                        { pp(1, lex_modifier);	((*yyvalp)._expr) = VARIABLE; }
    break;

  case 344: /* modifier: L_INDISPENSABLE  */
                                                                { pp(1, lex_modifier);	((*yyvalp)._expr) = INDISPENSABLE; }
    break;

  case 345: /* attribute: L_TRANSIENT  */
                                                                        { pp(1, lex_attribute);	((*yyvalp)._expr) = TRANSIENT; }
    break;

  case 346: /* attribute: L_PERSIST  */
                                                                        { pp(1, lex_attribute);	((*yyvalp)._expr) = PERSIST; }
    break;

  case 347: /* attribute: L_PRIVATE  */
                                                                        { pp(1, lex_attribute);	((*yyvalp)._expr) = PRIVATE; }
    break;

  case 348: /* attribute: L_PUBLIC  */
                                                                        { pp(1, lex_attribute);	((*yyvalp)._expr) = PUBLIC; }
    break;

  case 349: /* modId: name  */
                                                              { pp(1, lex_modId);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 350: /* typeCon: L_UC_NAME  */
                                                                        { pp(1, lex_typeCon);	((*yyvalp)._expr) = l_useName((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._string)); }
    break;

  case 351: /* typeCon: L_PTR  */
                                                              { pp(1, lex_typeCon);	((*yyvalp)._expr) = l_Ptr; }
    break;

  case 352: /* typeCon: L_TYPESIG_ASTERISK  */
                                                              { pp(1,lex_typeCon);	((*yyvalp)._expr) = l_Any; }
    break;

  case 353: /* tag: L_TAG  */
                                                                        { pp(1, lex_tag);		((*yyvalp)._expr) = TAG; }
    break;

  case 354: /* dataCon: L_UC_NAME  */
                                                                        { pp(1, lex_dataCon);	((*yyvalp)._expr) = l_dataCon((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._string)); }
    break;

  case 355: /* ptr: L_PTR  */
                                                                      { pp(1, lex_ptr);	      ((*yyvalp)._expr) = l_Ptr; }
    break;

  case 356: /* ptr: L_CARET  */
                                                                { pp(1, lex_ptr);	      ((*yyvalp)._expr) = l_Ptr; }
    break;

  case 357: /* classId: L_UC_NAME  */
                                                                        { pp(1, lex_classId);	((*yyvalp)._expr) = l_useName((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._string)); }
    break;

  case 358: /* typeVar: idName  */
                                                                { pp(1, lex_typeVar);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 359: /* atom: const  */
                                                                        { pp(1, lex_atom);	((*yyvalp)._expr) = (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr); }
    break;

  case 360: /* atom: L_NUMSIGN name  */
                                                                { pp(2, lex_atom);	((*yyvalp)._expr) = l_pair(LITERAL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 361: /* atom: L_NUMSIGN_COLON_COLON aTypeSig  */
                                                        { pp(2, lex_atom);	exitTypeSigContext(); ((*yyvalp)._expr) = l_pair(TYPE_SIG, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 362: /* atom: L_NUMSIGN const  */
                                                                { pp(2, lex_atom);	((*yyvalp)._expr) = l_pair(LITERAL, (YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._expr)); }
    break;

  case 363: /* const: L_CHAR  */
                                                        { pp(1, lex_const);	((*yyvalp)._expr) = l_char((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._char)); }
    break;

  case 364: /* const: L_INTEGER  */
                                                                        { pp(1, lex_const);	((*yyvalp)._expr) = l_int((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._int)); }
    break;

  case 365: /* const: L_HEX  */
                                                                { pp(1, lex_const);	((*yyvalp)._expr) = l_bits((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._bits)); }
    break;

  case 366: /* const: L_FIXED  */
                                                                { pp(1, lex_const);	((*yyvalp)._expr) = l_double((YY_CAST (yyGLRStackItem const *, yyvsp)[YYFILL (0)].yystate.yysemantics.yyval._double)); }
    break;

  case 367: /* const: L_STRING  */
                                                                        { pp(1, lex_const);	((*yyvalp)._expr) = l_string(string_buf); }
    break;

  case 368: /* semiSeq: L_SEMICOLON  */
                                                                        { pp(1, lex_semiSeq); }
    break;

  case 369: /* semiSeq: semiSeq L_SEMICOLON  */
                                                                { pp(2, lex_semiSeq); }
    break;

  case 370: /* optSemi: %empty  */
                                                                        { pp(0, lex_optSemi); }
    break;

  case 371: /* optSemi: semiSeq  */
                                                                { pp(1, lex_optSemi); }
    break;



      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yylhsNonterm (yyrule), yyvalp, yylocp);

  return yyok;
# undef yyerrok
# undef YYABORT
# undef YYACCEPT
# undef YYNOMEM
# undef YYERROR
# undef YYBACKUP
# undef yyclearin
# undef YYRECOVERING
}


static void
yyuserMerge (int yyn, YYSTYPE* yy0, YYSTYPE* yy1)
{
  YY_USE (yy0);
  YY_USE (yy1);

  switch (yyn)
    {

      default: break;
    }
}

                              /* Bison grammar-table manipulation.  */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}

/** Number of symbols composing the right hand side of rule #RULE.  */
static inline int
yyrhsLength (yyRuleNum yyrule)
{
  return yyr2[yyrule];
}

static void
yydestroyGLRState (char const *yymsg, yyGLRState *yys)
{
  if (yys->yyresolved)
    yydestruct (yymsg, yy_accessing_symbol (yys->yylrState),
                &yys->yysemantics.yyval);
  else
    {
#if YYDEBUG
      if (yydebug)
        {
          if (yys->yysemantics.yyfirstVal)
            YY_FPRINTF ((stderr, "%s unresolved", yymsg));
          else
            YY_FPRINTF ((stderr, "%s incomplete", yymsg));
          YY_SYMBOL_PRINT ("", yy_accessing_symbol (yys->yylrState), YY_NULLPTR, &yys->yyloc);
        }
#endif

      if (yys->yysemantics.yyfirstVal)
        {
          yySemanticOption *yyoption = yys->yysemantics.yyfirstVal;
          yyGLRState *yyrh;
          int yyn;
          for (yyrh = yyoption->yystate, yyn = yyrhsLength (yyoption->yyrule);
               yyn > 0;
               yyrh = yyrh->yypred, yyn -= 1)
            yydestroyGLRState (yymsg, yyrh);
        }
    }
}

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

/** True iff LR state YYSTATE has only a default reduction (regardless
 *  of token).  */
static inline yybool
yyisDefaultedState (yy_state_t yystate)
{
  return yypact_value_is_default (yypact[yystate]);
}

/** The default reduction for YYSTATE, assuming it has one.  */
static inline yyRuleNum
yydefaultAction (yy_state_t yystate)
{
  return yydefact[yystate];
}

#define yytable_value_is_error(Yyn) \
  0

/** The action to take in YYSTATE on seeing YYTOKEN.
 *  Result R means
 *    R < 0:  Reduce on rule -R.
 *    R = 0:  Error.
 *    R > 0:  Shift to state R.
 *  Set *YYCONFLICTS to a pointer into yyconfl to a 0-terminated list
 *  of conflicting reductions.
 */
static inline int
yygetLRActions (yy_state_t yystate, yysymbol_kind_t yytoken, const short** yyconflicts)
{
  int yyindex = yypact[yystate] + yytoken;
  if (yytoken == YYSYMBOL_YYerror)
    {
      // This is the error token.
      *yyconflicts = yyconfl;
      return 0;
    }
  else if (yyisDefaultedState (yystate)
           || yyindex < 0 || YYLAST < yyindex || yycheck[yyindex] != yytoken)
    {
      *yyconflicts = yyconfl;
      return -yydefact[yystate];
    }
  else if (! yytable_value_is_error (yytable[yyindex]))
    {
      *yyconflicts = yyconfl + yyconflp[yyindex];
      return yytable[yyindex];
    }
  else
    {
      *yyconflicts = yyconfl + yyconflp[yyindex];
      return 0;
    }
}

/** Compute post-reduction state.
 * \param yystate   the current state
 * \param yysym     the nonterminal to push on the stack
 */
static inline yy_state_t
yyLRgotoState (yy_state_t yystate, yysymbol_kind_t yysym)
{
  int yyr = yypgoto[yysym - YYNTOKENS] + yystate;
  if (0 <= yyr && yyr <= YYLAST && yycheck[yyr] == yystate)
    return yytable[yyr];
  else
    return yydefgoto[yysym - YYNTOKENS];
}

static inline yybool
yyisShiftAction (int yyaction)
{
  return 0 < yyaction;
}

static inline yybool
yyisErrorAction (int yyaction)
{
  return yyaction == 0;
}

                                /* GLRStates */

/** Return a fresh GLRStackItem in YYSTACKP.  The item is an LR state
 *  if YYISSTATE, and otherwise a semantic option.  Callers should call
 *  YY_RESERVE_GLRSTACK afterwards to make sure there is sufficient
 *  headroom.  */

static inline yyGLRStackItem*
yynewGLRStackItem (yyGLRStack* yystackp, yybool yyisState)
{
  yyGLRStackItem* yynewItem = yystackp->yynextFree;
  yystackp->yyspaceLeft -= 1;
  yystackp->yynextFree += 1;
  yynewItem->yystate.yyisState = yyisState;
  return yynewItem;
}

/** Add a new semantic action that will execute the action for rule
 *  YYRULE on the semantic values in YYRHS to the list of
 *  alternative actions for YYSTATE.  Assumes that YYRHS comes from
 *  stack #YYK of *YYSTACKP. */
static void
yyaddDeferredAction (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyGLRState* yystate,
                     yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yySemanticOption* yynewOption =
    &yynewGLRStackItem (yystackp, yyfalse)->yyoption;
  YY_ASSERT (!yynewOption->yyisState);
  yynewOption->yystate = yyrhs;
  yynewOption->yyrule = yyrule;
  if (yystackp->yytops.yylookaheadNeeds[yyk])
    {
      yynewOption->yyrawchar = yychar;
      yynewOption->yyval = yylval;
    }
  else
    yynewOption->yyrawchar = YYEMPTY;
  yynewOption->yynext = yystate->yysemantics.yyfirstVal;
  yystate->yysemantics.yyfirstVal = yynewOption;

  YY_RESERVE_GLRSTACK (yystackp);
}

                                /* GLRStacks */

/** Initialize YYSET to a singleton set containing an empty stack.  */
static yybool
yyinitStateSet (yyGLRStateSet* yyset)
{
  yyset->yysize = 1;
  yyset->yycapacity = 16;
  yyset->yystates
    = YY_CAST (yyGLRState**,
               YYMALLOC (YY_CAST (YYSIZE_T, yyset->yycapacity)
                         * sizeof yyset->yystates[0]));
  if (! yyset->yystates)
    return yyfalse;
  yyset->yystates[0] = YY_NULLPTR;
  yyset->yylookaheadNeeds
    = YY_CAST (yybool*,
               YYMALLOC (YY_CAST (YYSIZE_T, yyset->yycapacity)
                         * sizeof yyset->yylookaheadNeeds[0]));
  if (! yyset->yylookaheadNeeds)
    {
      YYFREE (yyset->yystates);
      return yyfalse;
    }
  memset (yyset->yylookaheadNeeds,
          0,
          YY_CAST (YYSIZE_T, yyset->yycapacity) * sizeof yyset->yylookaheadNeeds[0]);
  return yytrue;
}

static void yyfreeStateSet (yyGLRStateSet* yyset)
{
  YYFREE (yyset->yystates);
  YYFREE (yyset->yylookaheadNeeds);
}

/** Initialize *YYSTACKP to a single empty stack, with total maximum
 *  capacity for all stacks of YYSIZE.  */
static yybool
yyinitGLRStack (yyGLRStack* yystackp, YYPTRDIFF_T yysize)
{
  yystackp->yyerrState = 0;
  yynerrs = 0;
  yystackp->yyspaceLeft = yysize;
  yystackp->yyitems
    = YY_CAST (yyGLRStackItem*,
               YYMALLOC (YY_CAST (YYSIZE_T, yysize)
                         * sizeof yystackp->yynextFree[0]));
  if (!yystackp->yyitems)
    return yyfalse;
  yystackp->yynextFree = yystackp->yyitems;
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;
  return yyinitStateSet (&yystackp->yytops);
}


#if YYSTACKEXPANDABLE
# define YYRELOC(YYFROMITEMS, YYTOITEMS, YYX, YYTYPE)                   \
  &((YYTOITEMS)                                                         \
    - ((YYFROMITEMS) - YY_REINTERPRET_CAST (yyGLRStackItem*, (YYX))))->YYTYPE

/** If *YYSTACKP is expandable, extend it.  WARNING: Pointers into the
    stack from outside should be considered invalid after this call.
    We always expand when there are 1 or fewer items left AFTER an
    allocation, so that we can avoid having external pointers exist
    across an allocation.  */
static void
yyexpandGLRStack (yyGLRStack* yystackp)
{
  yyGLRStackItem* yynewItems;
  yyGLRStackItem* yyp0, *yyp1;
  YYPTRDIFF_T yynewSize;
  YYPTRDIFF_T yyn;
  YYPTRDIFF_T yysize = yystackp->yynextFree - yystackp->yyitems;
  if (YYMAXDEPTH - YYHEADROOM < yysize)
    yyMemoryExhausted (yystackp);
  yynewSize = 2*yysize;
  if (YYMAXDEPTH < yynewSize)
    yynewSize = YYMAXDEPTH;
  yynewItems
    = YY_CAST (yyGLRStackItem*,
               YYMALLOC (YY_CAST (YYSIZE_T, yynewSize)
                         * sizeof yynewItems[0]));
  if (! yynewItems)
    yyMemoryExhausted (yystackp);
  for (yyp0 = yystackp->yyitems, yyp1 = yynewItems, yyn = yysize;
       0 < yyn;
       yyn -= 1, yyp0 += 1, yyp1 += 1)
    {
      *yyp1 = *yyp0;
      if (*YY_REINTERPRET_CAST (yybool *, yyp0))
        {
          yyGLRState* yys0 = &yyp0->yystate;
          yyGLRState* yys1 = &yyp1->yystate;
          if (yys0->yypred != YY_NULLPTR)
            yys1->yypred =
              YYRELOC (yyp0, yyp1, yys0->yypred, yystate);
          if (! yys0->yyresolved && yys0->yysemantics.yyfirstVal != YY_NULLPTR)
            yys1->yysemantics.yyfirstVal =
              YYRELOC (yyp0, yyp1, yys0->yysemantics.yyfirstVal, yyoption);
        }
      else
        {
          yySemanticOption* yyv0 = &yyp0->yyoption;
          yySemanticOption* yyv1 = &yyp1->yyoption;
          if (yyv0->yystate != YY_NULLPTR)
            yyv1->yystate = YYRELOC (yyp0, yyp1, yyv0->yystate, yystate);
          if (yyv0->yynext != YY_NULLPTR)
            yyv1->yynext = YYRELOC (yyp0, yyp1, yyv0->yynext, yyoption);
        }
    }
  if (yystackp->yysplitPoint != YY_NULLPTR)
    yystackp->yysplitPoint = YYRELOC (yystackp->yyitems, yynewItems,
                                      yystackp->yysplitPoint, yystate);

  for (yyn = 0; yyn < yystackp->yytops.yysize; yyn += 1)
    if (yystackp->yytops.yystates[yyn] != YY_NULLPTR)
      yystackp->yytops.yystates[yyn] =
        YYRELOC (yystackp->yyitems, yynewItems,
                 yystackp->yytops.yystates[yyn], yystate);
  YYFREE (yystackp->yyitems);
  yystackp->yyitems = yynewItems;
  yystackp->yynextFree = yynewItems + yysize;
  yystackp->yyspaceLeft = yynewSize - yysize;
}
#endif

static void
yyfreeGLRStack (yyGLRStack* yystackp)
{
  YYFREE (yystackp->yyitems);
  yyfreeStateSet (&yystackp->yytops);
}

/** Assuming that YYS is a GLRState somewhere on *YYSTACKP, update the
 *  splitpoint of *YYSTACKP, if needed, so that it is at least as deep as
 *  YYS.  */
static inline void
yyupdateSplit (yyGLRStack* yystackp, yyGLRState* yys)
{
  if (yystackp->yysplitPoint != YY_NULLPTR && yystackp->yysplitPoint > yys)
    yystackp->yysplitPoint = yys;
}

/** Invalidate stack #YYK in *YYSTACKP.  */
static inline void
yymarkStackDeleted (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
    yystackp->yylastDeleted = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yystates[yyk] = YY_NULLPTR;
}

/** Undelete the last stack in *YYSTACKP that was marked as deleted.  Can
    only be done once after a deletion, and only when all other stacks have
    been deleted.  */
static void
yyundeleteLastStack (yyGLRStack* yystackp)
{
  if (yystackp->yylastDeleted == YY_NULLPTR || yystackp->yytops.yysize != 0)
    return;
  yystackp->yytops.yystates[0] = yystackp->yylastDeleted;
  yystackp->yytops.yysize = 1;
  YY_DPRINTF ((stderr, "Restoring last deleted stack as stack #0.\n"));
  yystackp->yylastDeleted = YY_NULLPTR;
}

static inline void
yyremoveDeletes (yyGLRStack* yystackp)
{
  YYPTRDIFF_T yyi, yyj;
  yyi = yyj = 0;
  while (yyj < yystackp->yytops.yysize)
    {
      if (yystackp->yytops.yystates[yyi] == YY_NULLPTR)
        {
          if (yyi == yyj)
            YY_DPRINTF ((stderr, "Removing dead stacks.\n"));
          yystackp->yytops.yysize -= 1;
        }
      else
        {
          yystackp->yytops.yystates[yyj] = yystackp->yytops.yystates[yyi];
          /* In the current implementation, it's unnecessary to copy
             yystackp->yytops.yylookaheadNeeds[yyi] since, after
             yyremoveDeletes returns, the parser immediately either enters
             deterministic operation or shifts a token.  However, it doesn't
             hurt, and the code might evolve to need it.  */
          yystackp->yytops.yylookaheadNeeds[yyj] =
            yystackp->yytops.yylookaheadNeeds[yyi];
          if (yyj != yyi)
            YY_DPRINTF ((stderr, "Rename stack %ld -> %ld.\n",
                        YY_CAST (long, yyi), YY_CAST (long, yyj)));
          yyj += 1;
        }
      yyi += 1;
    }
}

/** Shift to a new state on stack #YYK of *YYSTACKP, corresponding to LR
 * state YYLRSTATE, at input position YYPOSN, with (resolved) semantic
 * value *YYVALP and source location *YYLOCP.  */
static inline void
yyglrShift (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yy_state_t yylrState,
            YYPTRDIFF_T yyposn,
            YYSTYPE* yyvalp)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yytrue;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyval = *yyvalp;
  yystackp->yytops.yystates[yyk] = yynewState;

  YY_RESERVE_GLRSTACK (yystackp);
}

/** Shift stack #YYK of *YYSTACKP, to a new state corresponding to LR
 *  state YYLRSTATE, at input position YYPOSN, with the (unresolved)
 *  semantic value of YYRHS under the action for YYRULE.  */
static inline void
yyglrShiftDefer (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yy_state_t yylrState,
                 YYPTRDIFF_T yyposn, yyGLRState* yyrhs, yyRuleNum yyrule)
{
  yyGLRState* yynewState = &yynewGLRStackItem (yystackp, yytrue)->yystate;
  YY_ASSERT (yynewState->yyisState);

  yynewState->yylrState = yylrState;
  yynewState->yyposn = yyposn;
  yynewState->yyresolved = yyfalse;
  yynewState->yypred = yystackp->yytops.yystates[yyk];
  yynewState->yysemantics.yyfirstVal = YY_NULLPTR;
  yystackp->yytops.yystates[yyk] = yynewState;

  /* Invokes YY_RESERVE_GLRSTACK.  */
  yyaddDeferredAction (yystackp, yyk, yynewState, yyrhs, yyrule);
}

#if YYDEBUG

/*----------------------------------------------------------------------.
| Report that stack #YYK of *YYSTACKP is going to be reduced by YYRULE. |
`----------------------------------------------------------------------*/

static inline void
yy_reduce_print (yybool yynormal, yyGLRStackItem* yyvsp, YYPTRDIFF_T yyk,
                 yyRuleNum yyrule)
{
  int yynrhs = yyrhsLength (yyrule);
  int yyi;
  YY_FPRINTF ((stderr, "Reducing stack %ld by rule %d (line %d):\n",
               YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule]));
  if (! yynormal)
    yyfillin (yyvsp, 1, -yynrhs);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YY_FPRINTF ((stderr, "   $%d = ", yyi + 1));
      yy_symbol_print (stderr,
                       yy_accessing_symbol (yyvsp[yyi - yynrhs + 1].yystate.yylrState),
                       &yyvsp[yyi - yynrhs + 1].yystate.yysemantics.yyval                       );
      if (!yyvsp[yyi - yynrhs + 1].yystate.yyresolved)
        YY_FPRINTF ((stderr, " (unresolved)"));
      YY_FPRINTF ((stderr, "\n"));
    }
}
#endif

/** Pop the symbols consumed by reduction #YYRULE from the top of stack
 *  #YYK of *YYSTACKP, and perform the appropriate semantic action on their
 *  semantic values.  Assumes that all ambiguities in semantic values
 *  have been previously resolved.  Set *YYVALP to the resulting value,
 *  and *YYLOCP to the computed location (if any).  Return value is as
 *  for userAction.  */
static inline YYRESULTTAG
yydoAction (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyRuleNum yyrule,
            YYSTYPE* yyvalp)
{
  int yynrhs = yyrhsLength (yyrule);

  if (yystackp->yysplitPoint == YY_NULLPTR)
    {
      /* Standard special case: single stack.  */
      yyGLRStackItem* yyrhs
        = YY_REINTERPRET_CAST (yyGLRStackItem*, yystackp->yytops.yystates[yyk]);
      YY_ASSERT (yyk == 0);
      yystackp->yynextFree -= yynrhs;
      yystackp->yyspaceLeft += yynrhs;
      yystackp->yytops.yystates[0] = & yystackp->yynextFree[-1].yystate;
      return yyuserAction (yyrule, yynrhs, yyrhs, yystackp, yyk,
                           yyvalp);
    }
  else
    {
      yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
      yyGLRState* yys = yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred
        = yystackp->yytops.yystates[yyk];
      int yyi;
      for (yyi = 0; yyi < yynrhs; yyi += 1)
        {
          yys = yys->yypred;
          YY_ASSERT (yys);
        }
      yyupdateSplit (yystackp, yys);
      yystackp->yytops.yystates[yyk] = yys;
      return yyuserAction (yyrule, yynrhs, yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                           yystackp, yyk, yyvalp);
    }
}

/** Pop items off stack #YYK of *YYSTACKP according to grammar rule YYRULE,
 *  and push back on the resulting nonterminal symbol.  Perform the
 *  semantic action associated with YYRULE and store its value with the
 *  newly pushed state, if YYFORCEEVAL or if *YYSTACKP is currently
 *  unambiguous.  Otherwise, store the deferred semantic action with
 *  the new state.  If the new state would have an identical input
 *  position, LR state, and predecessor to an existing state on the stack,
 *  it is identified with that existing state, eliminating stack #YYK from
 *  *YYSTACKP.  In this case, the semantic value is
 *  added to the options for the existing state's semantic value.
 */
static inline YYRESULTTAG
yyglrReduce (yyGLRStack* yystackp, YYPTRDIFF_T yyk, yyRuleNum yyrule,
             yybool yyforceEval)
{
  YYPTRDIFF_T yyposn = yystackp->yytops.yystates[yyk]->yyposn;

  if (yyforceEval || yystackp->yysplitPoint == YY_NULLPTR)
    {
      YYSTYPE yyval;

      YYRESULTTAG yyflag = yydoAction (yystackp, yyk, yyrule, &yyval);
      if (yyflag == yyerr && yystackp->yysplitPoint != YY_NULLPTR)
        YY_DPRINTF ((stderr,
                     "Parse on stack %ld rejected by rule %d (line %d).\n",
                     YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule]));
      if (yyflag != yyok)
        return yyflag;
      yyglrShift (yystackp, yyk,
                  yyLRgotoState (yystackp->yytops.yystates[yyk]->yylrState,
                                 yylhsNonterm (yyrule)),
                  yyposn, &yyval);
    }
  else
    {
      YYPTRDIFF_T yyi;
      int yyn;
      yyGLRState* yys, *yys0 = yystackp->yytops.yystates[yyk];
      yy_state_t yynewLRState;

      for (yys = yystackp->yytops.yystates[yyk], yyn = yyrhsLength (yyrule);
           0 < yyn; yyn -= 1)
        {
          yys = yys->yypred;
          YY_ASSERT (yys);
        }
      yyupdateSplit (yystackp, yys);
      yynewLRState = yyLRgotoState (yys->yylrState, yylhsNonterm (yyrule));
      YY_DPRINTF ((stderr,
                   "Reduced stack %ld by rule %d (line %d); action deferred.  "
                   "Now in state %d.\n",
                   YY_CAST (long, yyk), yyrule - 1, yyrline[yyrule],
                   yynewLRState));
      for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
        if (yyi != yyk && yystackp->yytops.yystates[yyi] != YY_NULLPTR)
          {
            yyGLRState *yysplit = yystackp->yysplitPoint;
            yyGLRState *yyp = yystackp->yytops.yystates[yyi];
            while (yyp != yys && yyp != yysplit && yyp->yyposn >= yyposn)
              {
                if (yyp->yylrState == yynewLRState && yyp->yypred == yys)
                  {
                    yyaddDeferredAction (yystackp, yyk, yyp, yys0, yyrule);
                    yymarkStackDeleted (yystackp, yyk);
                    YY_DPRINTF ((stderr, "Merging stack %ld into stack %ld.\n",
                                 YY_CAST (long, yyk), YY_CAST (long, yyi)));
                    return yyok;
                  }
                yyp = yyp->yypred;
              }
          }
      yystackp->yytops.yystates[yyk] = yys;
      yyglrShiftDefer (yystackp, yyk, yynewLRState, yyposn, yys0, yyrule);
    }
  return yyok;
}

static YYPTRDIFF_T
yysplitStack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  if (yystackp->yysplitPoint == YY_NULLPTR)
    {
      YY_ASSERT (yyk == 0);
      yystackp->yysplitPoint = yystackp->yytops.yystates[yyk];
    }
  if (yystackp->yytops.yycapacity <= yystackp->yytops.yysize)
    {
      YYPTRDIFF_T state_size = YYSIZEOF (yystackp->yytops.yystates[0]);
      YYPTRDIFF_T half_max_capacity = YYSIZE_MAXIMUM / 2 / state_size;
      if (half_max_capacity < yystackp->yytops.yycapacity)
        yyMemoryExhausted (yystackp);
      yystackp->yytops.yycapacity *= 2;

      {
        yyGLRState** yynewStates
          = YY_CAST (yyGLRState**,
                     YYREALLOC (yystackp->yytops.yystates,
                                (YY_CAST (YYSIZE_T, yystackp->yytops.yycapacity)
                                 * sizeof yynewStates[0])));
        if (yynewStates == YY_NULLPTR)
          yyMemoryExhausted (yystackp);
        yystackp->yytops.yystates = yynewStates;
      }

      {
        yybool* yynewLookaheadNeeds
          = YY_CAST (yybool*,
                     YYREALLOC (yystackp->yytops.yylookaheadNeeds,
                                (YY_CAST (YYSIZE_T, yystackp->yytops.yycapacity)
                                 * sizeof yynewLookaheadNeeds[0])));
        if (yynewLookaheadNeeds == YY_NULLPTR)
          yyMemoryExhausted (yystackp);
        yystackp->yytops.yylookaheadNeeds = yynewLookaheadNeeds;
      }
    }
  yystackp->yytops.yystates[yystackp->yytops.yysize]
    = yystackp->yytops.yystates[yyk];
  yystackp->yytops.yylookaheadNeeds[yystackp->yytops.yysize]
    = yystackp->yytops.yylookaheadNeeds[yyk];
  yystackp->yytops.yysize += 1;
  return yystackp->yytops.yysize - 1;
}

/** True iff YYY0 and YYY1 represent identical options at the top level.
 *  That is, they represent the same rule applied to RHS symbols
 *  that produce the same terminal symbols.  */
static yybool
yyidenticalOptions (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  if (yyy0->yyrule == yyy1->yyrule)
    {
      yyGLRState *yys0, *yys1;
      int yyn;
      for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
           yyn = yyrhsLength (yyy0->yyrule);
           yyn > 0;
           yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
        if (yys0->yyposn != yys1->yyposn)
          return yyfalse;
      return yytrue;
    }
  else
    return yyfalse;
}

/** Assuming identicalOptions (YYY0,YYY1), destructively merge the
 *  alternative semantic values for the RHS-symbols of YYY1 and YYY0.  */
static void
yymergeOptionSets (yySemanticOption* yyy0, yySemanticOption* yyy1)
{
  yyGLRState *yys0, *yys1;
  int yyn;
  for (yys0 = yyy0->yystate, yys1 = yyy1->yystate,
       yyn = yyrhsLength (yyy0->yyrule);
       0 < yyn;
       yys0 = yys0->yypred, yys1 = yys1->yypred, yyn -= 1)
    {
      if (yys0 == yys1)
        break;
      else if (yys0->yyresolved)
        {
          yys1->yyresolved = yytrue;
          yys1->yysemantics.yyval = yys0->yysemantics.yyval;
        }
      else if (yys1->yyresolved)
        {
          yys0->yyresolved = yytrue;
          yys0->yysemantics.yyval = yys1->yysemantics.yyval;
        }
      else
        {
          yySemanticOption** yyz0p = &yys0->yysemantics.yyfirstVal;
          yySemanticOption* yyz1 = yys1->yysemantics.yyfirstVal;
          while (yytrue)
            {
              if (yyz1 == *yyz0p || yyz1 == YY_NULLPTR)
                break;
              else if (*yyz0p == YY_NULLPTR)
                {
                  *yyz0p = yyz1;
                  break;
                }
              else if (*yyz0p < yyz1)
                {
                  yySemanticOption* yyz = *yyz0p;
                  *yyz0p = yyz1;
                  yyz1 = yyz1->yynext;
                  (*yyz0p)->yynext = yyz;
                }
              yyz0p = &(*yyz0p)->yynext;
            }
          yys1->yysemantics.yyfirstVal = yys0->yysemantics.yyfirstVal;
        }
    }
}

/** Y0 and Y1 represent two possible actions to take in a given
 *  parsing state; return 0 if no combination is possible,
 *  1 if user-mergeable, 2 if Y0 is preferred, 3 if Y1 is preferred.  */
static int
yypreference (yySemanticOption* y0, yySemanticOption* y1)
{
  yyRuleNum r0 = y0->yyrule, r1 = y1->yyrule;
  int p0 = yydprec[r0], p1 = yydprec[r1];

  if (p0 == p1)
    {
      if (yymerger[r0] == 0 || yymerger[r0] != yymerger[r1])
        return 0;
      else
        return 1;
    }
  if (p0 == 0 || p1 == 0)
    return 0;
  if (p0 < p1)
    return 3;
  if (p1 < p0)
    return 2;
  return 0;
}

static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp);


/** Resolve the previous YYN states starting at and including state YYS
 *  on *YYSTACKP. If result != yyok, some states may have been left
 *  unresolved possibly with empty semantic option chains.  Regardless
 *  of whether result = yyok, each state has been left with consistent
 *  data so that yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveStates (yyGLRState* yys, int yyn,
                 yyGLRStack* yystackp)
{
  if (0 < yyn)
    {
      YY_ASSERT (yys->yypred);
      YYCHK (yyresolveStates (yys->yypred, yyn-1, yystackp));
      if (! yys->yyresolved)
        YYCHK (yyresolveValue (yys, yystackp));
    }
  return yyok;
}

/** Resolve the states for the RHS of YYOPT on *YYSTACKP, perform its
 *  user action, and return the semantic value and location in *YYVALP
 *  and *YYLOCP.  Regardless of whether result = yyok, all RHS states
 *  have been destroyed (assuming the user action destroys all RHS
 *  semantic values if invoked).  */
static YYRESULTTAG
yyresolveAction (yySemanticOption* yyopt, yyGLRStack* yystackp,
                 YYSTYPE* yyvalp)
{
  yyGLRStackItem yyrhsVals[YYMAXRHS + YYMAXLEFT + 1];
  int yynrhs = yyrhsLength (yyopt->yyrule);
  YYRESULTTAG yyflag =
    yyresolveStates (yyopt->yystate, yynrhs, yystackp);
  if (yyflag != yyok)
    {
      yyGLRState *yys;
      for (yys = yyopt->yystate; yynrhs > 0; yys = yys->yypred, yynrhs -= 1)
        yydestroyGLRState ("Cleanup: popping", yys);
      return yyflag;
    }

  yyrhsVals[YYMAXRHS + YYMAXLEFT].yystate.yypred = yyopt->yystate;
  {
    int yychar_current = yychar;
    YYSTYPE yylval_current = yylval;
    yychar = yyopt->yyrawchar;
    yylval = yyopt->yyval;
    yyflag = yyuserAction (yyopt->yyrule, yynrhs,
                           yyrhsVals + YYMAXRHS + YYMAXLEFT - 1,
                           yystackp, -1, yyvalp);
    yychar = yychar_current;
    yylval = yylval_current;
  }
  return yyflag;
}

#if YYDEBUG
static void
yyreportTree (yySemanticOption* yyx, int yyindent)
{
  int yynrhs = yyrhsLength (yyx->yyrule);
  int yyi;
  yyGLRState* yys;
  yyGLRState* yystates[1 + YYMAXRHS];
  yyGLRState yyleftmost_state;

  for (yyi = yynrhs, yys = yyx->yystate; 0 < yyi; yyi -= 1, yys = yys->yypred)
    yystates[yyi] = yys;
  if (yys == YY_NULLPTR)
    {
      yyleftmost_state.yyposn = 0;
      yystates[0] = &yyleftmost_state;
    }
  else
    yystates[0] = yys;

  if (yyx->yystate->yyposn < yys->yyposn + 1)
    YY_FPRINTF ((stderr, "%*s%s -> <Rule %d, empty>\n",
                 yyindent, "", yysymbol_name (yylhsNonterm (yyx->yyrule)),
                 yyx->yyrule - 1));
  else
    YY_FPRINTF ((stderr, "%*s%s -> <Rule %d, tokens %ld .. %ld>\n",
                 yyindent, "", yysymbol_name (yylhsNonterm (yyx->yyrule)),
                 yyx->yyrule - 1, YY_CAST (long, yys->yyposn + 1),
                 YY_CAST (long, yyx->yystate->yyposn)));
  for (yyi = 1; yyi <= yynrhs; yyi += 1)
    {
      if (yystates[yyi]->yyresolved)
        {
          if (yystates[yyi-1]->yyposn+1 > yystates[yyi]->yyposn)
            YY_FPRINTF ((stderr, "%*s%s <empty>\n", yyindent+2, "",
                         yysymbol_name (yy_accessing_symbol (yystates[yyi]->yylrState))));
          else
            YY_FPRINTF ((stderr, "%*s%s <tokens %ld .. %ld>\n", yyindent+2, "",
                         yysymbol_name (yy_accessing_symbol (yystates[yyi]->yylrState)),
                         YY_CAST (long, yystates[yyi-1]->yyposn + 1),
                         YY_CAST (long, yystates[yyi]->yyposn)));
        }
      else
        yyreportTree (yystates[yyi]->yysemantics.yyfirstVal, yyindent+2);
    }
}
#endif

static YYRESULTTAG
yyreportAmbiguity (yySemanticOption* yyx0,
                   yySemanticOption* yyx1)
{
  YY_USE (yyx0);
  YY_USE (yyx1);

#if YYDEBUG
  YY_FPRINTF ((stderr, "Ambiguity detected.\n"));
  YY_FPRINTF ((stderr, "Option 1,\n"));
  yyreportTree (yyx0, 2);
  YY_FPRINTF ((stderr, "\nOption 2,\n"));
  yyreportTree (yyx1, 2);
  YY_FPRINTF ((stderr, "\n"));
#endif

  yyerror (YY_("syntax is ambiguous"));
  return yyabort;
}

/** Resolve the ambiguity represented in state YYS in *YYSTACKP,
 *  perform the indicated actions, and set the semantic value of YYS.
 *  If result != yyok, the chain of semantic options in YYS has been
 *  cleared instead or it has been left unmodified except that
 *  redundant options may have been removed.  Regardless of whether
 *  result = yyok, YYS has been left with consistent data so that
 *  yydestroyGLRState can be invoked if necessary.  */
static YYRESULTTAG
yyresolveValue (yyGLRState* yys, yyGLRStack* yystackp)
{
  yySemanticOption* yyoptionList = yys->yysemantics.yyfirstVal;
  yySemanticOption* yybest = yyoptionList;
  yySemanticOption** yypp;
  yybool yymerge = yyfalse;
  YYSTYPE yyval;
  YYRESULTTAG yyflag;

  for (yypp = &yyoptionList->yynext; *yypp != YY_NULLPTR; )
    {
      yySemanticOption* yyp = *yypp;

      if (yyidenticalOptions (yybest, yyp))
        {
          yymergeOptionSets (yybest, yyp);
          *yypp = yyp->yynext;
        }
      else
        {
          switch (yypreference (yybest, yyp))
            {
            case 0:
              return yyreportAmbiguity (yybest, yyp);
              break;
            case 1:
              yymerge = yytrue;
              break;
            case 2:
              break;
            case 3:
              yybest = yyp;
              yymerge = yyfalse;
              break;
            default:
              /* This cannot happen so it is not worth a YY_ASSERT (yyfalse),
                 but some compilers complain if the default case is
                 omitted.  */
              break;
            }
          yypp = &yyp->yynext;
        }
    }

  if (yymerge)
    {
      yySemanticOption* yyp;
      int yyprec = yydprec[yybest->yyrule];
      yyflag = yyresolveAction (yybest, yystackp, &yyval);
      if (yyflag == yyok)
        for (yyp = yybest->yynext; yyp != YY_NULLPTR; yyp = yyp->yynext)
          {
            if (yyprec == yydprec[yyp->yyrule])
              {
                YYSTYPE yyval_other;
                yyflag = yyresolveAction (yyp, yystackp, &yyval_other);
                if (yyflag != yyok)
                  {
                    yydestruct ("Cleanup: discarding incompletely merged value for",
                                yy_accessing_symbol (yys->yylrState),
                                &yyval);
                    break;
                  }
                yyuserMerge (yymerger[yyp->yyrule], &yyval, &yyval_other);
              }
          }
    }
  else
    yyflag = yyresolveAction (yybest, yystackp, &yyval);

  if (yyflag == yyok)
    {
      yys->yyresolved = yytrue;
      yys->yysemantics.yyval = yyval;
    }
  else
    yys->yysemantics.yyfirstVal = YY_NULLPTR;
  return yyflag;
}

static YYRESULTTAG
yyresolveStack (yyGLRStack* yystackp)
{
  if (yystackp->yysplitPoint != YY_NULLPTR)
    {
      yyGLRState* yys;
      int yyn;

      for (yyn = 0, yys = yystackp->yytops.yystates[0];
           yys != yystackp->yysplitPoint;
           yys = yys->yypred, yyn += 1)
        continue;
      YYCHK (yyresolveStates (yystackp->yytops.yystates[0], yyn, yystackp
                             ));
    }
  return yyok;
}

/** Called when returning to deterministic operation to clean up the extra
 * stacks. */
static void
yycompressStack (yyGLRStack* yystackp)
{
  /* yyr is the state after the split point.  */
  yyGLRState *yyr;

  if (yystackp->yytops.yysize != 1 || yystackp->yysplitPoint == YY_NULLPTR)
    return;

  {
    yyGLRState *yyp, *yyq;
    for (yyp = yystackp->yytops.yystates[0], yyq = yyp->yypred, yyr = YY_NULLPTR;
         yyp != yystackp->yysplitPoint;
         yyr = yyp, yyp = yyq, yyq = yyp->yypred)
      yyp->yypred = yyr;
  }

  yystackp->yyspaceLeft += yystackp->yynextFree - yystackp->yyitems;
  yystackp->yynextFree = YY_REINTERPRET_CAST (yyGLRStackItem*, yystackp->yysplitPoint) + 1;
  yystackp->yyspaceLeft -= yystackp->yynextFree - yystackp->yyitems;
  yystackp->yysplitPoint = YY_NULLPTR;
  yystackp->yylastDeleted = YY_NULLPTR;

  while (yyr != YY_NULLPTR)
    {
      yystackp->yynextFree->yystate = *yyr;
      yyr = yyr->yypred;
      yystackp->yynextFree->yystate.yypred = &yystackp->yynextFree[-1].yystate;
      yystackp->yytops.yystates[0] = &yystackp->yynextFree->yystate;
      yystackp->yynextFree += 1;
      yystackp->yyspaceLeft -= 1;
    }
}

static YYRESULTTAG
yyprocessOneStack (yyGLRStack* yystackp, YYPTRDIFF_T yyk,
                   YYPTRDIFF_T yyposn)
{
  while (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
    {
      yy_state_t yystate = yystackp->yytops.yystates[yyk]->yylrState;
      YY_DPRINTF ((stderr, "Stack %ld Entering state %d\n",
                   YY_CAST (long, yyk), yystate));

      YY_ASSERT (yystate != YYFINAL);

      if (yyisDefaultedState (yystate))
        {
          YYRESULTTAG yyflag;
          yyRuleNum yyrule = yydefaultAction (yystate);
          if (yyrule == 0)
            {
              YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              return yyok;
            }
          yyflag = yyglrReduce (yystackp, yyk, yyrule, yyimmediate[yyrule]);
          if (yyflag == yyerr)
            {
              YY_DPRINTF ((stderr,
                           "Stack %ld dies "
                           "(predicate failure or explicit user error).\n",
                           YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              return yyok;
            }
          if (yyflag != yyok)
            return yyflag;
        }
      else
        {
          yysymbol_kind_t yytoken = yygetToken (&yychar);
          const short* yyconflicts;
          const int yyaction = yygetLRActions (yystate, yytoken, &yyconflicts);
          yystackp->yytops.yylookaheadNeeds[yyk] = yytrue;

          for (/* nothing */; *yyconflicts; yyconflicts += 1)
            {
              YYRESULTTAG yyflag;
              YYPTRDIFF_T yynewStack = yysplitStack (yystackp, yyk);
              YY_DPRINTF ((stderr, "Splitting off stack %ld from %ld.\n",
                           YY_CAST (long, yynewStack), YY_CAST (long, yyk)));
              yyflag = yyglrReduce (yystackp, yynewStack,
                                    *yyconflicts,
                                    yyimmediate[*yyconflicts]);
              if (yyflag == yyok)
                YYCHK (yyprocessOneStack (yystackp, yynewStack,
                                          yyposn));
              else if (yyflag == yyerr)
                {
                  YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yynewStack)));
                  yymarkStackDeleted (yystackp, yynewStack);
                }
              else
                return yyflag;
            }

          if (yyisShiftAction (yyaction))
            break;
          else if (yyisErrorAction (yyaction))
            {
              YY_DPRINTF ((stderr, "Stack %ld dies.\n", YY_CAST (long, yyk)));
              yymarkStackDeleted (yystackp, yyk);
              break;
            }
          else
            {
              YYRESULTTAG yyflag = yyglrReduce (yystackp, yyk, -yyaction,
                                                yyimmediate[-yyaction]);
              if (yyflag == yyerr)
                {
                  YY_DPRINTF ((stderr,
                               "Stack %ld dies "
                               "(predicate failure or explicit user error).\n",
                               YY_CAST (long, yyk)));
                  yymarkStackDeleted (yystackp, yyk);
                  break;
                }
              else if (yyflag != yyok)
                return yyflag;
            }
        }
    }
  return yyok;
}






static void
yyreportSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState != 0)
    return;
  yyerror (YY_("syntax error"));
  yynerrs += 1;
}

/* Recover from a syntax error on *YYSTACKP, assuming that *YYSTACKP->YYTOKENP,
   yylval, and yylloc are the syntactic category, semantic value, and location
   of the lookahead.  */
static void
yyrecoverSyntaxError (yyGLRStack* yystackp)
{
  if (yystackp->yyerrState == 3)
    /* We just shifted the error token and (perhaps) took some
       reductions.  Skip tokens until we can proceed.  */
    while (yytrue)
      {
        yysymbol_kind_t yytoken;
        int yyj;
        if (yychar == YYEOF)
          yyFail (yystackp, YY_NULLPTR);
        if (yychar != YYEMPTY)
          {
            yytoken = YYTRANSLATE (yychar);
            yydestruct ("Error: discarding",
                        yytoken, &yylval);
            yychar = YYEMPTY;
          }
        yytoken = yygetToken (&yychar);
        yyj = yypact[yystackp->yytops.yystates[0]->yylrState];
        if (yypact_value_is_default (yyj))
          return;
        yyj += yytoken;
        if (yyj < 0 || YYLAST < yyj || yycheck[yyj] != yytoken)
          {
            if (yydefact[yystackp->yytops.yystates[0]->yylrState] != 0)
              return;
          }
        else if (! yytable_value_is_error (yytable[yyj]))
          return;
      }

  /* Reduce to one stack.  */
  {
    YYPTRDIFF_T yyk;
    for (yyk = 0; yyk < yystackp->yytops.yysize; yyk += 1)
      if (yystackp->yytops.yystates[yyk] != YY_NULLPTR)
        break;
    if (yyk >= yystackp->yytops.yysize)
      yyFail (yystackp, YY_NULLPTR);
    for (yyk += 1; yyk < yystackp->yytops.yysize; yyk += 1)
      yymarkStackDeleted (yystackp, yyk);
    yyremoveDeletes (yystackp);
    yycompressStack (yystackp);
  }

  /* Pop stack until we find a state that shifts the error token.  */
  yystackp->yyerrState = 3;
  while (yystackp->yytops.yystates[0] != YY_NULLPTR)
    {
      yyGLRState *yys = yystackp->yytops.yystates[0];
      int yyj = yypact[yys->yylrState];
      if (! yypact_value_is_default (yyj))
        {
          yyj += YYSYMBOL_YYerror;
          if (0 <= yyj && yyj <= YYLAST && yycheck[yyj] == YYSYMBOL_YYerror
              && yyisShiftAction (yytable[yyj]))
            {
              /* Shift the error token.  */
              int yyaction = yytable[yyj];
              YY_SYMBOL_PRINT ("Shifting", yy_accessing_symbol (yyaction),
                               &yylval, &yyerrloc);
              yyglrShift (yystackp, 0, yyaction,
                          yys->yyposn, &yylval);
              yys = yystackp->yytops.yystates[0];
              break;
            }
        }
      if (yys->yypred != YY_NULLPTR)
        yydestroyGLRState ("Error: popping", yys);
      yystackp->yytops.yystates[0] = yys->yypred;
      yystackp->yynextFree -= 1;
      yystackp->yyspaceLeft += 1;
    }
  if (yystackp->yytops.yystates[0] == YY_NULLPTR)
    yyFail (yystackp, YY_NULLPTR);
}

#define YYCHK1(YYE)                             \
  do {                                          \
    switch (YYE) {                              \
    case yyok:     break;                       \
    case yyabort:  goto yyabortlab;             \
    case yyaccept: goto yyacceptlab;            \
    case yyerr:    goto yyuser_error;           \
    case yynomem:  goto yyexhaustedlab;         \
    default:       goto yybuglab;               \
    }                                           \
  } while (0)

/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
  int yyresult;
  yyGLRStack yystack;
  yyGLRStack* const yystackp = &yystack;
  YYPTRDIFF_T yyposn;

  YY_DPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY;
  yylval = yyval_default;

  if (! yyinitGLRStack (yystackp, YYINITDEPTH))
    goto yyexhaustedlab;
  switch (YYSETJMP (yystack.yyexception_buffer))
    {
    case 0: break;
    case 1: goto yyabortlab;
    case 2: goto yyexhaustedlab;
    default: goto yybuglab;
    }
  yyglrShift (&yystack, 0, 0, 0, &yylval);
  yyposn = 0;

  while (yytrue)
    {
      /* For efficiency, we have two loops, the first of which is
         specialized to deterministic operation (single stack, no
         potential ambiguity).  */
      /* Standard mode. */
      while (yytrue)
        {
          yy_state_t yystate = yystack.yytops.yystates[0]->yylrState;
          YY_DPRINTF ((stderr, "Entering state %d\n", yystate));
          if (yystate == YYFINAL)
            goto yyacceptlab;
          if (yyisDefaultedState (yystate))
            {
              yyRuleNum yyrule = yydefaultAction (yystate);
              if (yyrule == 0)
                {
                  yyreportSyntaxError (&yystack);
                  goto yyuser_error;
                }
              YYCHK1 (yyglrReduce (&yystack, 0, yyrule, yytrue));
            }
          else
            {
              yysymbol_kind_t yytoken = yygetToken (&yychar);
              const short* yyconflicts;
              int yyaction = yygetLRActions (yystate, yytoken, &yyconflicts);
              if (*yyconflicts)
                /* Enter nondeterministic mode.  */
                break;
              if (yyisShiftAction (yyaction))
                {
                  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
                  yychar = YYEMPTY;
                  yyposn += 1;
                  yyglrShift (&yystack, 0, yyaction, yyposn, &yylval);
                  if (0 < yystack.yyerrState)
                    yystack.yyerrState -= 1;
                }
              else if (yyisErrorAction (yyaction))
                {
                  /* Issue an error message unless the scanner already
                     did. */
                  if (yychar != YYerror)
                    yyreportSyntaxError (&yystack);
                  goto yyuser_error;
                }
              else
                YYCHK1 (yyglrReduce (&yystack, 0, -yyaction, yytrue));
            }
        }

      /* Nondeterministic mode. */
      while (yytrue)
        {
          yysymbol_kind_t yytoken_to_shift;
          YYPTRDIFF_T yys;

          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            yystackp->yytops.yylookaheadNeeds[yys] = yychar != YYEMPTY;

          /* yyprocessOneStack returns one of three things:

              - An error flag.  If the caller is yyprocessOneStack, it
                immediately returns as well.  When the caller is finally
                yyparse, it jumps to an error label via YYCHK1.

              - yyok, but yyprocessOneStack has invoked yymarkStackDeleted
                (&yystack, yys), which sets the top state of yys to NULL.  Thus,
                yyparse's following invocation of yyremoveDeletes will remove
                the stack.

              - yyok, when ready to shift a token.

             Except in the first case, yyparse will invoke yyremoveDeletes and
             then shift the next token onto all remaining stacks.  This
             synchronization of the shift (that is, after all preceding
             reductions on all stacks) helps prevent double destructor calls
             on yylval in the event of memory exhaustion.  */

          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            YYCHK1 (yyprocessOneStack (&yystack, yys, yyposn));
          yyremoveDeletes (&yystack);
          if (yystack.yytops.yysize == 0)
            {
              yyundeleteLastStack (&yystack);
              if (yystack.yytops.yysize == 0)
                yyFail (&yystack, YY_("syntax error"));
              YYCHK1 (yyresolveStack (&yystack));
              YY_DPRINTF ((stderr, "Returning to deterministic operation.\n"));
              yyreportSyntaxError (&yystack);
              goto yyuser_error;
            }

          /* If any yyglrShift call fails, it will fail after shifting.  Thus,
             a copy of yylval will already be on stack 0 in the event of a
             failure in the following loop.  Thus, yychar is set to YYEMPTY
             before the loop to make sure the user destructor for yylval isn't
             called twice.  */
          yytoken_to_shift = YYTRANSLATE (yychar);
          yychar = YYEMPTY;
          yyposn += 1;
          for (yys = 0; yys < yystack.yytops.yysize; yys += 1)
            {
              yy_state_t yystate = yystack.yytops.yystates[yys]->yylrState;
              const short* yyconflicts;
              int yyaction = yygetLRActions (yystate, yytoken_to_shift,
                              &yyconflicts);
              /* Note that yyconflicts were handled by yyprocessOneStack.  */
              YY_DPRINTF ((stderr, "On stack %ld, ", YY_CAST (long, yys)));
              YY_SYMBOL_PRINT ("shifting", yytoken_to_shift, &yylval, &yylloc);
              yyglrShift (&yystack, yys, yyaction, yyposn,
                          &yylval);
              YY_DPRINTF ((stderr, "Stack %ld now in state %d\n",
                           YY_CAST (long, yys),
                           yystack.yytops.yystates[yys]->yylrState));
            }

          if (yystack.yytops.yysize == 1)
            {
              YYCHK1 (yyresolveStack (&yystack));
              YY_DPRINTF ((stderr, "Returning to deterministic operation.\n"));
              yycompressStack (&yystack);
              break;
            }
        }
      continue;
    yyuser_error:
      yyrecoverSyntaxError (&yystack);
      yyposn = yystack.yytops.yystates[0]->yyposn;
    }

 yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;

 yybuglab:
  YY_ASSERT (yyfalse);
  goto yyabortlab;

 yyabortlab:
  yyresult = 1;
  goto yyreturnlab;

 yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;

 yyreturnlab:
  if (yychar != YYEMPTY)
    yydestruct ("Cleanup: discarding lookahead",
                YYTRANSLATE (yychar), &yylval);

  /* If the stack is well-formed, pop the stack until it is empty,
     destroying its entries as we go.  But free the stack regardless
     of whether it is well-formed.  */
  if (yystack.yyitems)
    {
      yyGLRState** yystates = yystack.yytops.yystates;
      if (yystates)
        {
          YYPTRDIFF_T yysize = yystack.yytops.yysize;
          YYPTRDIFF_T yyk;
          for (yyk = 0; yyk < yysize; yyk += 1)
            if (yystates[yyk])
              {
                while (yystates[yyk])
                  {
                    yyGLRState *yys = yystates[yyk];
                    if (yys->yypred != YY_NULLPTR)
                      yydestroyGLRState ("Cleanup: popping", yys);
                    yystates[yyk] = yys->yypred;
                    yystack.yynextFree -= 1;
                    yystack.yyspaceLeft += 1;
                  }
                break;
              }
        }
      yyfreeGLRStack (&yystack);
    }

  return yyresult;
}

/* DEBUGGING ONLY */
#if YYDEBUG
/* Print *YYS and its predecessors. */
static void
yy_yypstack (yyGLRState* yys)
{
  if (yys->yypred)
    {
      yy_yypstack (yys->yypred);
      YY_FPRINTF ((stderr, " -> "));
    }
  YY_FPRINTF ((stderr, "%d@%ld", yys->yylrState, YY_CAST (long, yys->yyposn)));
}

/* Print YYS (possibly NULL) and its predecessors. */
static void
yypstates (yyGLRState* yys)
{
  if (yys == YY_NULLPTR)
    YY_FPRINTF ((stderr, "<null>"));
  else
    yy_yypstack (yys);
  YY_FPRINTF ((stderr, "\n"));
}

/* Print the stack #YYK.  */
static void
yypstack (yyGLRStack* yystackp, YYPTRDIFF_T yyk)
{
  yypstates (yystackp->yytops.yystates[yyk]);
}

/* Print all the stacks.  */
static void
yypdumpstack (yyGLRStack* yystackp)
{
#define YYINDEX(YYX)                                                    \
  YY_CAST (long,                                                        \
           ((YYX)                                                       \
            ? YY_REINTERPRET_CAST (yyGLRStackItem*, (YYX)) - yystackp->yyitems \
            : -1))

  yyGLRStackItem* yyp;
  for (yyp = yystackp->yyitems; yyp < yystackp->yynextFree; yyp += 1)
    {
      YY_FPRINTF ((stderr, "%3ld. ",
                   YY_CAST (long, yyp - yystackp->yyitems)));
      if (*YY_REINTERPRET_CAST (yybool *, yyp))
        {
          YY_ASSERT (yyp->yystate.yyisState);
          YY_ASSERT (yyp->yyoption.yyisState);
          YY_FPRINTF ((stderr, "Res: %d, LR State: %d, posn: %ld, pred: %ld",
                       yyp->yystate.yyresolved, yyp->yystate.yylrState,
                       YY_CAST (long, yyp->yystate.yyposn),
                       YYINDEX (yyp->yystate.yypred)));
          if (! yyp->yystate.yyresolved)
            YY_FPRINTF ((stderr, ", firstVal: %ld",
                         YYINDEX (yyp->yystate.yysemantics.yyfirstVal)));
        }
      else
        {
          YY_ASSERT (!yyp->yystate.yyisState);
          YY_ASSERT (!yyp->yyoption.yyisState);
          YY_FPRINTF ((stderr, "Option. rule: %d, state: %ld, next: %ld",
                       yyp->yyoption.yyrule - 1,
                       YYINDEX (yyp->yyoption.yystate),
                       YYINDEX (yyp->yyoption.yynext)));
        }
      YY_FPRINTF ((stderr, "\n"));
    }

  YY_FPRINTF ((stderr, "Tops:"));
  {
    YYPTRDIFF_T yyi;
    for (yyi = 0; yyi < yystackp->yytops.yysize; yyi += 1)
      YY_FPRINTF ((stderr, "%ld: %ld; ", YY_CAST (long, yyi),
                   YYINDEX (yystackp->yytops.yystates[yyi])));
    YY_FPRINTF ((stderr, "\n"));
  }
#undef YYINDEX
}
#endif

#undef yylval
#undef yychar
#undef yynerrs






#include "lex.yy.c"

#define MAX_ERROR_COUNT 5

Expr parser(NameTable& nameTable, MSA& msa) {
   _nameTable = &nameTable;
   _msa = &msa;
   _errCount = 0;

#if (YYDEBUG==1)
	yydebug = 1;
#endif

	if (!yyparse() && _errCount == 0) {
//		printf("Parse complete\n");
//		printf("--------------\n");
		return script;
	}
	else
        return ERROR;
}

extern void clearParserState() {
   _errCount = 0;
   yylineno = 1;
   clearParserContext();
}

int parserLineNumber(void) {
   return yylineno;
}

void parserError(void) {
   if (_errCount++ < MAX_ERROR_COUNT) {
      char s[256];
      sprintf(s, "Parse error at line %d%s%s%s Current token: \"%s\"",
         yylineno,
         stackPtr == 0
         ? ""
         : " in include file \"",
         stackPtr == 0
         ? ""
         : fileNameStack[stackPtr - 1],
         stackPtr == 0
         ? ""
         : "\"",
         yytext);
      outbit(s);
      outnew();
      dump_lex_stack();
      clear();
   }
   else {
      outbit("*** Too many parser errors: stopping\n");
      termParser();
      exit(1);
   }
}
