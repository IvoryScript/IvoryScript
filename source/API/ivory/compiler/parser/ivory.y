%glr-parser

%{
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

%}

%start script

%union {
   Char     _char;
   Int      _int;
   Bits     _bits;
   Double	_double;
   String	_string;
   ExprRepr _expr;
}

/* Ivory tokens */
%token L_MODULE 10001
%token L_DEF 10002
%token L_WITH 10003
%token L_WHERE 10004
%token L_UNDEF 10005
%token L_DO 10006
%token L_IF 10007
%token L_THEN 10008
%token L_ELSE 10009
%token L_CASE 10010
%token L_OF 10011
%token L_OTHERWISE 10012
%token L_LET 10013
%token L_IN 10014
%token L_RETURN 10015
%token L_THIS 10016
%token L_INLINE 10017
%token L_TAG 10018
%token L_SEMICOLON 10019
%token L_LSQB 10020
%token L_RSQB 10021
%token L_LPAREN 10022
%token L_RPAREN 10023
%token L_LCURLYB 10024
%token L_CMD_LCURLYB 10025
%token L_RCURLYB 10026
%token L_BACKSLASH 10027
%token L_COLON_EQ 10028
%token L_AT_EQ 10029
%token L_LT_HYPHEN 10030
%token L_EXCLMARK 10031
%token L_NUMSIGN_EXCLMARK 10032
%token L_COMMA 10033
%token L_UNDERSCORE 10034
%token L_COLON_COLON 10035
%token L_PREFIX_COLON_COLON 10036
%token L_NOTSIGN_COLON_COLON 10037
%token L_AMPERSAND 10038
%token L_VERTBAR 10039
%token L_HYPHEN_GT 10040
%token L_TYPESIG_HYPHEN_GT 10041
%token L_PREFIX_TYPESIG_HYPHEN_GT 10042
%token L_LT_LT 10043
%token L_GT_GT 10044
%token L_NOTSIGN 10045
%token L_NOT 10046
%token L_AND 10047
%token L_OR 10048
%token L_NUMSIGN_AMPERSAND 10049
%token L_NUMSIGN_VERTBAR 10050
%token L_NUMSIGN_CARET 10051
%token L_NUMSIGN_NOTSIGN 10052
%token L_LT 10053
%token L_LT_EQ 10054
%token L_EQ 10055
%token L_DEF_EQ 10056
%token L_NOTSIGN_EQ 10057
%token L_GT_EQ 10058
%token L_GT 10059
%token L_DOT 10060
%token L_PREFIX_DOT 10061
%token L_COLON 10062
%token L_COLON_PLUS 10063
%token L_PLUS_PLUS 10064
%token L_PLUS 10065
%token L_PREFIX_PLUS 10066
%token L_HYPHEN 10067
%token L_PREFIX_HYPHEN 10068
%token L_ASTERISK 10069
%token L_TYPESIG_ASTERISK 10070
%token L_SLASH 10071
%token L_DIV 10072
%token L_MOD 10073
%token L_CARET 10074
%token L_PREFIX_CARET 10075
%token L_LPAREN_CARET_RPAREN 10076
%token L_NOTSIGN_CARET 10077
%token L_NUMSIGN 10078
%token L_NUMSIGN_NUMSIGN 10079
%token L_NUMSIGN_COLON_COLON 10080
%token L_NUMSIGN_AT 10081
%token L_AT 10082
%token L_EQ_GT 10083
%token L_TILDE 10084
%token L_TILDE_TILDE 10085
%token L_TILDE_AT 10086
%token L_TYPE 10087
%token L_OBJECT 10088
%token L_CLASS 10089
%token L_PRIMITIVE 10090
%token L_INSTANCE 10091
%token L_SUBORDINATE 10092
%token L_CONSTANT 10093
%token L_VARIABLE 10094
%token L_INDISPENSABLE 10095
%token L_DYNAMIC 10096
%token L_TRANSIENT 10097
%token L_PERSIST 10098
%token L_PRIVATE 10099
%token L_PUBLIC 10100
%token L_MATCHING 10101
%token L_PTR 10102
%token <_string> L_UC_NAME 10103
%token <_string> L_LC_NAME 10104
%token <_int> L_INTEGER 10105
%token <_bits> L_HEX 10106
%token <_double> L_FIXED 10107
%token <_char> L_CHAR 10108
%token <_string> L_STRING 10109
%type <_expr> script
%type <_expr> module
%type <_expr> sequence
%type <_expr> modDeclOrDefnBody
%type <_expr> modDeclOrDefns
%type <_expr> modDeclOrDefnList
%type <_expr> modDeclOrDefn
%type <_expr> dynTypeDefn
%type <_expr> typeDefn
%type <_expr> objectDefn
%type <_expr> classDefn
%type <_expr> genClass
%type <_expr> classOrList
%type <_expr> bracketedClassList
%type <_expr> bracketedClassOrList
%type <_expr> bracketedClassListTail
%type <_expr> bracketedClass
%type <_expr> class
%type <_expr> classDeclOrDefnBody
%type <_expr> classDeclOrDefnList
%type <_expr> classDeclOrDefn
%type <_expr> instanceDefn
%type <_expr> instanceDefnTypes
%type <_expr> optInstanceDeclOrDefnBody
%type <_expr> instanceDeclOrDefnBody
%type <_expr> instanceDeclOrDefnList
%type <_expr> instanceDeclOrDefn
%type <_expr> dataConDecl
%type <_expr> modifiedDataConDefn
%type <_expr> dataConDefn
%type <_expr> selectors
%type <_expr> selectorBody
%type <_expr> selectorList
%type <_expr> selector
%type <_expr> declOrDefnBody
%type <_expr> declOrDefnList
%type <_expr> declOrDefn
%type <_expr> primDecl
%type <_expr> decl
%type <_expr> modifiedDefn
%type <_expr> defn
%type <_expr> defCmdBody
%type <_expr> defCmdList
%type <_expr> defCmd
%type <_expr> undefCmdBody
%type <_expr> undefCmdList
%type <_expr> undefCmd
%type <_expr> patterns
%type <_expr> patternList
%type <_expr> pattern
%type <_expr> binPattern
%type <_expr> plainPattern
%type <_expr> tuplePtrDeconPattern
%type <_expr> tuplePattern
%type <_expr> patternComps
%type <_expr> ptrPattern
%type <_expr> dataConPtrDeconPattern
%type <_expr> dataConPattern
%type <_expr> concretePattern
%type <_expr> constrainedPattern
%type <_expr> varPattern
%type <_expr> aPattern
%type <_expr> typeConstraint
%type <_expr> binTypeSig
%type <_expr> monTypeSig
%type <_expr> apTypeSig
%type <_expr> typeSig
%type <_expr> aTypeSig
%type <_expr> typeSigCompList
%type <_expr> typeSigCSList
%type <_expr> typeQuals
%type <_expr> typeQualOrList
%type <_expr> bTypeQualList
%type <_expr> bTypeQualOrList
%type <_expr> bTypeQualListTail
%type <_expr> typeQual
%type <_expr> dyadicTypeQual
%type <_expr> monadicTypeQual
%type <_expr> instanceTypeQual
%type <_expr> bTypeQual
%type <_expr> aTypeQual
%type <_expr> predRelOp
%type <_expr> predAndOp
%type <_expr> predAndAlts
%type <_expr> predOrOp
%type <_expr> predOrAlts
%type <_expr> predNotOp
%type <_expr> predNotAlts
%type <_expr> dataDecls
%type <_expr> dataDecl
%type <_expr> aTypeSigs
%type <_expr> aTypeSigList
%type <_expr> retExpr
%type <_expr> defExpr
%type <_expr> undefExpr
%type <_expr> seqExpr
%type <_expr> expr
%type <_expr> genExpr
%type <_expr> apExpr
%type <_expr> binExpr
%type <_expr> monExpr
%type <_expr> specialExpr
%type <_expr> sel
%type <_expr> selExpr
%type <_expr> selDataCon
%type <_expr> bExpr
%type <_expr> aExpr
%type <_expr> prefixOp
%type <_expr> selOp
%type <_expr> expOp
%type <_expr> mulDivOp
%type <_expr> concatOp
%type <_expr> addSubOp
%type <_expr> relOp
%type <_expr> logAndOp
%type <_expr> logAndAlts
%type <_expr> logOrOp
%type <_expr> logOrAlts
%type <_expr> shiftOp
%type <_expr> bitAndOp
%type <_expr> bitXorOp
%type <_expr> bitOrOp
%type <_expr> assignOp
%type <_expr> dyadicOp
%type <_expr> notOp
%type <_expr> notAlts
%type <_expr> bitwiseNotOp
%type <_expr> dePtrOp
%type <_expr> monadicOp
%type <_expr> exprs
%type <_expr> exprList
%type <_expr> compList
%type <_expr> caseAltList
%type <_expr> caseAlt
%type <_expr> casePatternList
%type <_expr> casePattern
%type <_expr> qual
%type <_expr> qualList
%type <_expr> typeVars
%type <_expr> typeVarList
%type <_expr> idOrPrefixOp
%type <_expr> bracketedSignedId
%type <_expr> signedId
%type <_expr> idName
%type <_expr> name
%type <_expr> wildcard
%type <_expr> modifier
%type <_expr> attribute
%type <_expr> modId
%type <_expr> typeCon
%type <_expr> tag
%type <_expr> dataCon
%type <_expr> ptr
%type <_expr> classId
%type <_expr> typeVar
%type <_expr> atom
%type <_expr> const
%type <_expr> semiSeq
%type <_expr> optSemi

/* IvoryScript precedence */

%precedence PREC_LET PREC_LAMBDA
%precedence PREC_IF_THEN
%precedence L_ELSE
%precedence PREC_IF_THEN_ELSE
%right L_COLON_EQ
%right L_AT_EQ

%right PREC_MODIFIER PREC_ATTRIBUTE

%right L_COLON_PLUS
%right L_COLON

%left L_VERTBAR L_OR PREC_LOG_OR
%left L_AMPERSAND L_AND PREC_LOG_AND
%left L_LT L_LT_EQ L_EQ L_NOTSIGN_EQ L_GT_EQ L_GT PREC_REL
%left L_NUMSIGN_VERTBAR PREC_BIT_OR
%left L_NUMSIGN_CARET PREC_BIT_XOR
%left L_NUMSIGN_AMPERSAND PREC_BIT_AND
%left L_LT_LT L_GT_GT PREC_SHIFT
%left L_PLUS L_HYPHEN PREC_ADD_SUB
%left L_PLUS_PLUS PREC_CONCAT
%left L_ASTERISK L_SLASH L_DIV L_MOD PREC_MUL_DIV
%left L_CARET PREC_EXP
%left L_DOT PREC_SEL
%right L_PLAIN PREC_PLAIN_PATTERN
%right L_AT_PATTERN PREC_AT_PATTERN
%right L_PTR L_PREFIX_CARET PREC_PTR PREC_PTR_PATTERN
%right L_NOTSIGN L_NOT PREC_LOG_NOT
%right L_NUMSIGN_NOTSIGN PREC_BITWISE_NOT
%right L_PREFIX_PLUS L_PREFIX_HYPHEN
%right L_LPAREN_CARET_RPAREN L_NOTSIGN_CARET
%right L_NOTSIGN_COLON_COLON L_PREFIX_COLON_COLON
%right L_AT PREC_DEPTR
%right L_EXCLMARK L_LPAREN_EXCLMARK_RPAREN
%right L_PREFIX_DOT

%left L_COLON_COLON
%right L_TYPESIG_HYPHEN_GT
%right L_PREFIX_TYPESIG_HYPHEN_GT
%right L_PREFIX_NUMSIGN_COLON_COLON

%left L_TILDE
%left L_TILDE_TILDE
%left L_TILDE_AT

%left L_NUMSIGN

%{
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
%}

%glr-parser 

%%

script:	error L_SEMICOLON					{				parserError();
													script = ERROR; }
	|	/* empty */						{ pp(0, lex_script);	script = VOID; }
	|	module						{ pp(1, lex_script);    clear();
												      script = l_pair(MODULE, $1 ); }
	|	sequence						{ pp(1, lex_script);    clear();
												      script = l_pair(ORDER, $1 ); }
	;

module:	L_MODULE modId L_WHERE modDeclOrDefnBody	{ pp(4, lex_module);	$$ = l_pair($2, $4); }
	;

sequence:	seqExpr semiSeq sequence			{ pp(3, lex_sequence);	$$ = l_cons($1, $3 ); }
	|	seqExpr optSemi					{ pp(2, lex_sequence);	$$ = l_cons($1, l_Nil); }
	;

modDeclOrDefnBody:
		modDeclOrDefn					{ pp(1, lex_modDeclOrDefnBody);
													$$ = l_cons($1, l_Nil); }
	|	L_LCURLYB modDeclOrDefns L_RCURLYB
									{ pp(3, lex_modDeclOrDefnBody);
													$$ = $2; }
	;

modDeclOrDefns:
		modDeclOrDefnList					{ pp(1, lex_modDeclOrDefns);
													$$ = $1; }
	|	/* empty */						{ pp(0, lex_modDeclOrDefns);
													$$ = l_Nil; }
	;

modDeclOrDefnList:
		modDeclOrDefn semiSeq modDeclOrDefnList
									{ pp(3, lex_modDeclOrDefnList);
													$$ = l_cons($1, $3 ); }
	|	modDeclOrDefn optSemi				{ pp(2, lex_modDeclOrDefnList);
													$$ = l_cons($1, l_Nil); }
	;

modDeclOrDefn:
	      dynTypeDefn				      	{ pp(1, lex_modDeclOrDefn);
	      											$$ = l_pair(TYPE_DEFN, $1); }
	|	objectDefn				      	{ pp(1, lex_modDeclOrDefn);
	      											$$ = $1; }

	|	classDefn				      	{ pp(1, lex_modDeclOrDefn);
	      											$$ = l_pair(CLASS_DEFN, $1); }
	|	instanceDefn					{ pp(1, lex_modDeclOrDefn);
													$$ = l_pair(INSTANCE_DEFN, $1); }
	|	L_SUBORDINATE instanceDefn			{ pp(1, lex_modDeclOrDefn);
													$$ = l_pair(INSTANCE_DEFN, l_pair(SUBORDINATE, $2)); }

      |     dataConDecl				            { pp(1, lex_modDeclOrDefn);
      												$$ = $1; }
	|	modifiedDataConDefn				{ pp(1, lex_modDeclOrDefn);
													$$ = $1; }
      |     primDecl						{ pp(1, lex_modDeclOrDefn);
      												$$ = $1; }
      |     modifiedDefn					{ pp(1, lex_modDeclOrDefn);
      												$$ = $1; }
	;


dynTypeDefn:	
		L_DYNAMIC typeDefn				{ pp(2, lex_dynTypeDefn);
													$$ = l_pair(DYNAMIC, $2); }
	|	typeDefn			            	{ pp(1, lex_dynTypeDefn);
													$$ = $1; }
	;

typeDefn:   L_TYPE typeCon typeVars L_EQ dataDecls	{ pp(5, lex_typeDefn);	$$ = l_triple($2, $3, $5); } 
	;
																		
objectDefn:	L_OBJECT typeCon L_EQ dataDecl		{ pp(4, lex_objectDefn);	
													$$ = l_triple(OBJECT_DEFN, $2, $4); }
	;

classDefn:	L_CLASS genClass L_WHERE classDeclOrDefnBody
									{ pp(4, lex_classDefn);	$$ = l_pair($2, $4); }
	;

genClass:	class							{ pp(1, lex_genClass);	$$ = l_pair(l_Nil, $1); }
	|	classOrList L_EQ_GT class			{ pp(3, lex_genClass);	$$ = l_pair($1, $3); }
	;

classOrList:
		class							{ pp(1, lex_classOrList);
													$$ = l_cons($1, l_Nil); }
	|	bracketedClassList				{ pp(1, lex_classOrList);
													$$ = $1; }
	;

bracketedClassList:
		bracketedClass bracketedClassListTail	{ pp(2, lex_bracketedClassList);	
													$$ = l_cons($1, $2); }
	;

bracketedClassOrList:
		bracketedClass					{ pp(1, lex_bracketedClassOrList);
													$$ = l_cons($1, l_Nil); }
	|	bracketedClassList				{ pp(1, lex_bracketedClassOrList);
													$$ = $1; }
	;

bracketedClassListTail:
		L_COMMA bracketedClassOrList			{ pp(2, lex_bracketedClassListTail);
													$$ = $2; }
	;

bracketedClass:
		L_LPAREN class L_RPAREN    			{ pp(3, lex_bracketedClass);
													$$ = $2; }
	;

class:      classId typeSigCSList				{ pp(2, lex_class); exitTypeSigContext();
									$$ = l_pair($1, $2); }
	;

classDeclOrDefnBody:

	classDeclOrDefn  	      				{ pp(1, lex_classDeclOrDefnBody);
	      											$$ = l_cons($1, l_Nil); }
	|	L_LCURLYB classDeclOrDefnList L_RCURLYB
									{ pp(3, lex_classDeclOrDefnBody);
													$$ = $2; }
	;

classDeclOrDefnList:
		classDeclOrDefn semiSeq classDeclOrDefnList
									{ pp(3, lex_classDeclOrDefnList);
													$$ = l_cons($1, $3 ); }
	|	classDeclOrDefn optSemi				{ pp(2, lex_classDeclOrDefnList);
													$$ = l_cons($1, l_Nil); }
	;
            
classDeclOrDefn:
		modifiedDefn			            { pp(1, lex_classDeclOrDefn);
													$$ = $1; }
	|     objectDefn				            { pp(1, lex_classDeclOrDefn);
													$$ = $1; }
	|     decl				                  { pp(1, lex_classDeclOrDefn);
													$$ = $1; }
	;

instanceDefn:
	L_INSTANCE classId instanceDefnTypes optInstanceDeclOrDefnBody
									{ pp(4, lex_instanceDefn);	
													$$ = l_triple($2, $3, $4); }
	;

instanceDefnTypes:
	typeSigCSList typeQuals					{ pp(2, lex_instanceDefnTypes); exitTypeSigContext();
													$$ = l_pair($1, $2); }
	;

optInstanceDeclOrDefnBody:
      /* empty */						      { pp(0, lex_optInstanceDeclOrDefnBody);
													$$ = l_Nil; }
      |     L_WHERE instanceDeclOrDefnBody		{ pp(2, lex_optInstanceDeclOrDefnBody);  
													$$ = $2; }
	;

instanceDeclOrDefnBody:
	instanceDeclOrDefn  	      			{ pp(1, lex_instanceDeclOrDefnBody);
	      											$$ = l_cons($1, l_Nil); }
	|	L_LCURLYB instanceDeclOrDefnList L_RCURLYB
									{ pp(3, lex_instanceDeclOrDefnBody);
													$$ = $2; }
	;

instanceDeclOrDefnList:
		instanceDeclOrDefn semiSeq instanceDeclOrDefnList
									{ pp(3, lex_instanceDeclOrDefnList);
													$$ = l_cons($1, $3 ); }
	|	instanceDeclOrDefn optSemi			{ pp(2, lex_instanceDeclOrDefnList);
													$$ = l_cons($1, l_Nil); }
	;
            
instanceDeclOrDefn:
		decl			            		{ pp(1, lex_instanceDeclOrDefn);
													$$ = $1; }
	|     modifiedDefn					{ pp(1, lex_instanceDeclOrDefn);
													$$ = $1; }
	|     dataConDecl			                  { pp(1, lex_instanceDeclOrDefn);
													$$ = $1; }
	|     modifiedDataConDefn				{ pp(1, lex_instanceDeclOrDefn);
													$$ = $1; }
	;

dataConDecl:	
            dataCon typeConstraint			      { pp(2, lex_dataConDecl);
                                                                              $$ = l_triple(DATA_CON_DECL, $1, $2); }
	;

modifiedDataConDefn:
            L_INLINE dataConDefn				{ pp(2, lex_modifiedDataConDefn);
                                                                        	$$ = l_pair(INLINE, $2); }
	|	dataConDefn			                  { pp(1, lex_modifiedDataConDefn);
		                                                                  $$ = $1; }
	;

dataConDefn:
            dataCon patterns L_EQ expr selectors
									{ pp(4, lex_dataConDefn);
                                                                              $$ = l_triple(DATA_CON_DEFN, l_pair($1, defn($2, $4)), $5); }
      ;

selectors:  /* empty */						{ pp(0, lex_selectors); $$ = l_Nil; }
	|     L_MATCHING selectorBody		            { pp(2, lex_selectors); $$ = $2; }
	;

selectorBody:
	      L_LCURLYB selectorList L_RCURLYB          { pp(3, lex_selectorBody);
													$$ = $2; }
	;

selectorList:
		selector semiSeq selectorList
									{ pp(3, lex_selectorList);
													$$ = l_cons($1, $3 ); }
	|	selector optSemi			            { pp(2, lex_selectorList);
													$$ = l_cons($1, l_Nil); }
	;

selector:	expr							{ pp(1, lex_selector);	$$ = l_coerce($1); }
	|	L_INLINE expr			            { pp(1, lex_selector);	$$ = l_pair(INLINE, l_coerce($2)); }
	;

declOrDefnBody:
	declOrDefn  	      				{ pp(1, lex_declOrDefnBody);
	      											$$ = l_cons($1, l_Nil); }
	|	L_LCURLYB declOrDefnList L_RCURLYB
									{ pp(3, lex_declOrDefnBody);												$$ = $2; }
	;

declOrDefnList:
		declOrDefn semiSeq declOrDefnList
									{ pp(3, lex_declOrDefnList);
													$$ = l_cons($1, $3 ); }
	|	declOrDefn optSemi				{ pp(2, lex_declOrDefnList);
													$$ = l_cons($1, l_Nil); }
	;
            
declOrDefn:	decl			            		{ pp(1, lex_declOrDefn);
													$$ = $1; }
	|     modifiedDefn				      { pp(1, lex_declOrDefn);	
													$$ = $1; }
	;

primDecl:	L_PRIMITIVE decl					{ pp(2, lex_primDecl);	$$ = l_pair(PRIMITIVE, $2); }
	|	decl			            		{ pp(1, lex_primDecl);	$$ = $1; }
	;

decl:		signedId						{ pp(1, lex_decl);	$$ = l_pair(DECL, $1); }
	;

modifiedDefn: L_INLINE defn					{ pp(2, lex_modifiedDefn);
													$$ = l_pair(INLINE, $2); }
	|	defn			            		{ pp(1, lex_modifiedDefn);
													$$ = $1; }
	;

defn:		signedId L_EQ expr				{ pp(3, lex_defn);	$$ = l_triple(DEFN, $1, defn(Nil, $3)); }
	|	idOrPrefixOp patternList L_EQ expr		{ pp(4, lex_defn);	$$ = l_triple(DEFN, $1, defn($2, $4)); }
	|	bracketedSignedId patterns L_EQ expr	{ pp(4, lex_defn);	$$ = l_triple(DEFN, $1, defn($2, $4)); }
	;

defCmdBody:	defCmd					      { pp(1, lex_defCmdBody);
													$$ = $1; }
	|	L_CMD_LCURLYB defCmdList L_RCURLYB		{ pp(3, lex_defCmdBody);
													$$ = l_sequence($2); }
	;

defCmdList:	defCmd semiSeq defCmdList			{ pp(3, lex_defCmdList);
													$$ = l_cons($1, $3 ); }
	|	defCmd optSemi				      { pp(2, lex_defCmdList);
													$$ = l_cons($1, l_Nil); } 
	;

/* patterns temporarily omitted */
defCmd:	selExpr L_EQ expr					{ pp(3, lex_defCmd);	$$ = l_ap(l_reduce(l_ap(l_reduce(l_ap(l_useName("addBinding"), l_arg(l_snd(l_fun($1))))), l_arg($1))), l_pair(COERCE, l_eval($3))); }
	;

undefCmdBody:	
            undefCmd						{ pp(1, lex_undefCmdBody);
            											$$ = $1; }
	|	L_CMD_LCURLYB undefCmdList L_RCURLYB	{ pp(3, lex_undefCmdBody);
													$$ = l_sequence($2); }
	;

undefCmdList:
	      undefCmd semiSeq undefCmdList			{ pp(3, lex_undefCmdList);
	      											$$ = l_cons($1, $3); }
	|	undefCmd optSemi					{ pp(2, lex_undefCmdList);
													$$ = l_cons($1, l_Nil); }
	;

undefCmd:	selExpr						{ pp(1, lex_undefCmd);	$$ = l_ap(l_reduce(l_ap(l_useName("removeBinding"), l_arg(l_snd(l_fun($1))))), l_arg($1)); }
	;

patterns:	/* empty */						{ pp(0, lex_patterns);	$$ = l_Nil; }
	|	patternList						{ pp(1, lex_patterns);	$$ = $1; }
	;

patternList:		
		aPattern						{ pp(1, lex_patternList);
													$$ = l_cons($1, l_Nil); }
	|	aPattern patternList				{ pp(2, lex_patternList);
													$$ = l_cons($1, $2 ); }
	;	

pattern:	binPattern						{ pp(1, lex_pattern);	$$ = $1; }
	|	plainPattern					{ pp(1, lex_pattern);	$$ = $1; }
	|	tuplePattern					{ pp(1, lex_pattern);	$$ = $1; }
	|	tuplePtrDeconPattern				{ pp(1, lex_pattern);	$$ = $1; }
	|	ptrPattern						{ pp(1, lex_pattern);	$$ = $1; }
	|	dataConPtrDeconPattern				{ pp(1, lex_pattern);	$$ = $1; }
	|	dataConPattern					{ pp(1, lex_pattern);	$$ = $1; }
	|	concretePattern					{ pp(1, lex_pattern);	$$ = $1; }
	|	constrainedPattern				{ pp(1, lex_pattern);	$$ = $1; }
	|	aPattern						{ pp(1, lex_pattern);	$$ = $1; }
	;

binPattern:	idName L_COLON pattern				{ pp(3, lex_binPattern);
													$$ = l_triple(DATA_CON_PATTERN, l_Bind, l_cons($1, l_cons($3, l_Nil))); }
	|	pattern L_COLON_PLUS pattern	 		{ pp(3, lex_binPattern);
													$$ = l_triple(DATA_CON_PATTERN, l_Cons, l_cons($1, l_cons($3, l_Nil))); }
	;

plainPattern:
		L_TILDE tuplePattern				{ pp(2, lex_plainPattern);
													$$ = l_pair(PLAIN_PATTERN, $2); }
	|	L_TILDE_AT tuplePattern				{ pp(1, lex_plainPattern);
													$$ = l_pair(PLAIN_PATTERN, l_pair(PTR_DECON_PATTERN, $2)); }
	|	L_TILDE ptrPattern				{ pp(2, lex_plainPattern);
													$$ = l_pair(PLAIN_PATTERN, $2); }
	|	L_TILDE_AT dataCon pattern			{ pp(3, lex_plainPattern);
													$$ = l_pair(PLAIN_PATTERN, l_pair(PTR_DECON_PATTERN, l_triple(CONCRETE_PATTERN, $2, l_cons($3, l_Nil)))); }
	|	L_TILDE_TILDE dataCon pattern			{ pp(3, lex_plainPattern);
													$$ = l_pair(PLAIN_PATTERN, l_triple(CONCRETE_PATTERN, $2, l_cons($3, l_Nil))); }
	;

tuplePtrDeconPattern:
	L_AT tuplePattern	%prec PREC_AT_PATTERN		{ pp(2, lex_tuplePtrDeconPattern);
													$$ = l_pair(PTR_DECON_PATTERN, $2); } 
      ;

tuplePattern:
	L_LPAREN patternComps L_RPAREN			{ pp(3, lex_tuplePattern);
													$$ = l_pair(TUPLE_PATTERN, $2); }
      ;

patternComps:	
		patternComps L_COMMA pattern			{ pp(3, lex_patternComps);
													$$ = l_concat($1, l_cons($3, l_Nil)); }
	|	pattern L_COMMA pattern				{ pp(3, lex_patternComps);
													$$ = l_cons($1, l_cons($3, l_Nil)); }
	;

ptrPattern:	
		ptr pattern %prec PREC_PTR_PATTERN			{ pp(2, lex_ptrPattern);
													$$ = l_triple(DATA_CON_PATTERN, $1, l_cons($2, l_Nil)); } 
      ;

dataConPtrDeconPattern:	
		L_AT dataCon pattern				{ pp(2, lex_dataConPtrDeconPattern);
													$$ = l_pair(PTR_DECON_PATTERN, l_triple(CONCRETE_PATTERN, $2, l_cons($3, l_Nil))); } 
      ;

dataConPattern:	
		dataCon patternList				{ pp(2, lex_dataConPattern);
													$$ = l_triple(DATA_CON_PATTERN, $1, $2); } 
      ;


concretePattern:	
		L_TILDE dataCon pattern
									{ pp(3, lex_concretePattern);
													$$ = l_triple(CONCRETE_PATTERN, $2, l_cons($3, l_Nil)); } 
      ;

constrainedPattern:	
		aPattern typeConstraint           	      { pp(2, lex_pattern);	$$ = l_triple(CONSTRAIN, $1, $2); }
	;

varPattern:	idName						{ pp(1, lex_varPattern);
													$$ = $1; }
      |     wildcard                                  { pp(1, lex_varPattern);
													$$ = WILDCARD_PATTERN; }
      ;

aPattern:	atom							{ pp(1, lex_aPattern);	$$ = $1; }
	|	dataCon 						{ pp(1, lex_aPattern);	$$ = l_triple(DATA_CON_PATTERN, $1, l_Nil); }
	|	L_TILDE dataCon 					{ pp(2, lex_aPattern);	$$ = l_pair(PLAIN_PATTERN, l_triple(DATA_CON_PATTERN, $2, l_Nil)); }
      |     varPattern                                { pp(1, lex_aPattern);	$$ = $1; }
	|	L_LSQB L_RSQB	/* Nil */		      { pp(2, lex_aPattern);	$$ = l_triple(DATA_CON_PATTERN, l_useName("Nil"), l_Nil); }
	|	L_LPAREN pattern L_RPAREN			{ pp(3, lex_aPattern);	$$ = $2; }
	;
	
typeConstraint:
            L_COLON_COLON typeSig
									{ pp(2, lex_typeConstraint); exitTypeSigContext();
													$$ = $2; }	
      ;
  
binTypeSig:
		typeSig L_COLON typeSig				{ pp(3, lex_binTypeSig);
													$$ = l_ap2(l_useName("Binding"), $1, $3); }
	|	typeSig L_TYPESIG_HYPHEN_GT typeSig		{ pp(3, lex_binTypeSig);
													$$ = l_ap2(ARROW, $1, $3); }
	;

monTypeSig:	
		L_TYPESIG_HYPHEN_GT typeSig %prec L_PREFIX_TYPESIG_HYPHEN_GT
									{ pp(2, lex_monTypeSig); 
													$$ = l_ap(ARROW, $2); }
	;
   
apTypeSig:	aTypeSig aTypeSig					{ pp(2, lex_apTypeSig);	$$ = l_ap($1, $2); }
	|	apTypeSig aTypeSig				{ pp(2, lex_apTypeSig);	$$ = l_ap($1, $2); }
	;

typeSig:	apTypeSig						{ pp(1, lex_typeSig);	$$ = $1; }
	|	monTypeSig						{ pp(1, lex_typeSig);	$$ = $1; }
	|	binTypeSig						{ pp(1, lex_typeSig);	$$ = $1; }
	|	aTypeSig						{ pp(1, lex_typeSig);	$$ = $1; }
	;

aTypeSig:	typeCon						{ pp(1, lex_aTypeSig);	$$ = $1; }      
	|	typeVar						{ pp(1, lex_aTypeSig);	$$ = $1; }
      |     tag	                                    { pp(1, lex_aTypeSig);	$$ = $1; }
	|	L_LPAREN typeSigCompList L_RPAREN		{ pp(3, lex_aTypeSig);	$$ = l_apTuple($2, 0, FALSE); }
	|	L_LSQB typeSig L_RSQB				{ pp(3, lex_aTypeSig);	$$ = l_ap(l_useName("List"), $2); }
	|	L_LPAREN typeSig L_RPAREN 			{ pp(3, lex_aTypeSig);	$$ = $2; } 
	;

typeSigCompList:	
		typeSigCompList L_COMMA typeSig
									{ pp(3, lex_typeSigCompList);
													$$ = l_cons($3, $1); }
	|	typeSig L_COMMA typeSig
									{ pp(3, lex_typeSigCompList);
													$$ = l_cons($3, l_cons($1, l_Nil)); }
	;

typeSigCSList:
		typeSig						{ pp(1, lex_typeSigCSList);
													$$ = l_cons($1, Nil); }
	|	typeSig L_COMMA typeSigCSList			{ pp(3, lex_typeSigCSList);
													
													$$ = l_cons($1, $3); }
	;

typeQuals:
            /* empty */						{ pp(0, lex_typeQuals);	$$ = l_Nil; }
	|	L_VERTBAR typeQualOrList			{ pp(2, lex_typeQuals);	$$ = $2; }
	;

typeQualOrList:
		typeQual						{ pp(1, lex_typeQualOrList);
													$$ = l_cons($1, l_Nil); }
	|	bTypeQualList					{ pp(1, lex_typeQualOrList);
													$$ = $1; }
	;

bTypeQualList:
		bTypeQual bTypeQualListTail			{ pp(2, lex_bTypeQualList);	
													$$ = l_cons($1, $2); }
	;

bTypeQualOrList:
		bTypeQual						{ pp(1, lex_bTypeQualOrList);
													$$ = l_cons($1, l_Nil); }
	|	bTypeQualList					{ pp(1, lex_bTypeQualOrList);
													$$ = $1; }
	;

bTypeQualListTail:
		L_COMMA bTypeQualOrList				{ pp(2, lex_bTypeQualListTail);
													$$ = $2; }
	;

typeQual:	bTypeQual						{ pp(1, lex_typeQual);	$$ = $1; }
	|	instanceTypeQual					{ pp(1, lex_typeQual);	$$ = $1; }
 	;

dyadicTypeQual:
		typeSig predRelOp typeSig %prec PREC_REL  { pp(3, lex_dyadicTypeQual);
													$$ = l_triple($2, $1, $3); }
      |     bTypeQual predAndOp bTypeQual %prec PREC_LOG_AND
									{ pp(3, lex_dyadicTypeQual);
													$$ = l_triple($2, $1, $3); }
      |     bTypeQual predOrOp bTypeQual %prec PREC_LOG_OR      
									{ pp(3, lex_dyadicTypeQual);
													$$ = l_triple($2, $1, $3); }
	;

monadicTypeQual:
            L_EXCLMARK typeVar				{ pp(2, lex_monadicTypeQual);
													$$ = l_pair(STRICT, $2); };
	|	predNotOp bTypeQual %prec PREC_LOG_NOT	{ pp(3, lex_monadicTypeQual);
													$$ = l_pair($1, $2); }
	;

instanceTypeQual:
		L_INSTANCE classId typeSigCSList          { pp(3, lex_instanceTypeQual);  
													$$ = l_triple(INSTANCE, $2, $3); };
	;

bTypeQual:	dyadicTypeQual					{ pp(1, lex_typeQual);	$$ = $1; }
	|	monadicTypeQual					{ pp(1, lex_typeQual);	$$ = $1; }
	|	aTypeQual						{ pp(1, lex_typeQual);	$$ = $1; }
 	;
	
aTypeQual:	L_LPAREN typeQual L_RPAREN    		{ pp(3, lex_aTypeQual);	$$ = $2; }
 	;

predRelOp:	L_EQ							{ pp(1, lex_predRelOp);	$$ = EQUALITY; }
	|	L_NOTSIGN_EQ     					{ pp(1, lex_predRelOp);	$$ = INEQUALITY; }
	;

predAndOp:	predAndAlts 					{ pp(1, lex_predAndOp);
													$$ = CONJUNCTION; }
      ;

predAndAlts:	
		L_AND					            { pp(1, lex_predAndAlts);
													$$ = $$; }
	|	L_AMPERSAND 			            { pp(1, lex_predAndAlts);
													$$ = $$; }
	;

predOrOp:	predOrAlts 						{ pp(1, lex_predOrOp);	$$ = DISJUNCTION; }
      ;

predOrAlts:	L_OR 						      { pp(1, lex_predOrAlts);
													$$ = $$; }
	|	L_VERTBAR 					      { pp(1, lex_predOrAlts);
													$$ = $$; }
	;

predNotOp:	predNotAlts 					{ pp(1, lex_predNotOp);	$$ = NEGATION; }
      ;

predNotAlts:	
		L_NOT 						{ pp(1, lex_predNotAlts);
													$$ = $$; }
	|	L_NOTSIGN 					      { pp(1, lex_predNotAlts);
													$$ = $$; }
	;
														
dataDecls:  dataDecl L_VERTBAR dataDecls	            { pp(3, lex_dataDecls);  $$ = l_cons($1, $3); }
	|	dataDecl						{ pp(1, lex_dataDecls);  $$ = l_cons($1, l_Nil); }
	;
	
dataDecl:   dataCon aTypeSigs                          { pp(2, lex_dataDecl);	$$ = l_pair($1, $2); }
	|     ptr aTypeSigs                              { pp(2, lex_dataDecl);	$$ = l_pair($1, $2); }
	;

aTypeSigs:	/* empty */						{ pp(0, lex_aTypeSigs);	$$ = l_Nil; }
	|	aTypeSigList					{ pp(1, lex_aTypeSigs);	$$ = $1; }
	;
	
aTypeSigList:	
		aTypeSig aTypeSigList				{ pp(2, lex_aTypeSigList);
													$$ = l_cons($1, $2 ); }
	|	aTypeSig						{ pp(1, lex_aTypeSigList);
													$$ = l_cons($1, l_Nil); }
	;	

retExpr:	L_RETURN expr					{ pp(2, lex_retExpr);	$$ = l_pair(RETURN, l_coerce($2)); }
	|     L_RETURN					      { pp(1, lex_retExpr);	$$ = RETURN; }
 	;

defExpr:	L_DEF defCmdBody					{ pp(2, lex_defExpr);	$$ = $2; }
 	;

undefExpr:	L_UNDEF undefCmdBody				{ pp(2, lex_undefExpr);	$$ = $2; }
 	;

seqExpr:	expr							{ pp(1, lex_seqExpr);	$$ = $1; }
	|	retExpr						{ pp(1, lex_seqExpr);	$$ = $1; }
	;
      
expr:		apExpr						{ pp(1, lex_expr);	$$ = $1; }
	|	selExpr						{ pp(1, lex_expr);	$$ = $1; }
	|	selDataCon						{ pp(1, lex_expr);	$$ = $1; }
	|	monExpr						{ pp(1, lex_expr);	$$ = $1; }
	|	binExpr						{ pp(1, lex_expr);	$$ = $1; }
	|	genExpr						{ pp(1, lex_expr);	$$ = $1; }
	|	bExpr							{ pp(1, lex_expr);	$$ = $1; }
	|	expr typeConstraint				{ pp(2, lex_expr);	$$ = l_noCoerce(l_triple(CONSTRAIN, l_constrain($1), $2)); }
	;

genExpr:	L_BACKSLASH patterns L_HYPHEN_GT expr %prec PREC_LAMBDA
									{ pp(4, lex_genExpr);	$$ = l_triple(LAMBDA, $2, l_coerce($4)); }
	|	L_LET declOrDefnBody L_IN expr %prec PREC_LET
									{ pp(4, lex_genExpr);	$$ = l_triple(LET_REC, $2, l_coerce($4)); }
	|	L_IF expr L_THEN expr L_ELSE expr %prec PREC_IF_THEN_ELSE
									{ pp(6, lex_genExpr);	$$ = l_pair(IF, l_triple(l_coerce($2), l_coerce($4), l_coerce($6))); } 
	|	L_IF expr L_THEN expr %prec PREC_IF_THEN
									{ pp(4, lex_genExpr);	$$ = l_pair(IF, l_pair(l_coerce($2), l_coerce($4))); }
	|	defExpr						{ pp(1, lex_genExpr);	$$ = $1; }
	|	undefExpr						{ pp(1, lex_genExpr);	$$ = $1; }
 	;

apExpr:	bExpr bExpr						{ pp(1, lex_apExpr);	$$ = l_genAp($1, $2); }
	|	apExpr bExpr					{ pp(2, lex_apExpr);	$$ = l_genAp($1, $2); }
	;

binExpr:	expr expOp expr %prec PREC_EXP		{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr mulDivOp expr %prec PREC_MUL_DIV
									{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr concatOp expr %prec PREC_CONCAT
									{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr addSubOp expr %prec PREC_ADD_SUB
									{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr shiftOp expr %prec PREC_SHIFT		{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr bitAndOp expr %prec PREC_BIT_AND
									{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr bitXorOp expr %prec PREC_BIT_XOR
									{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr bitOrOp expr %prec PREC_BIT_OR		{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr relOp expr %prec PREC_REL		{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr logAndOp expr %prec PREC_LOG_AND
									{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     expr logOrOp expr %prec PREC_LOG_OR		{ pp(3, lex_binExpr);	$$ = l_binAp($2, $1, $3); }
      |     name L_COLON expr					{ pp(3, lex_binExpr);	$$ = l_ap(l_reduce(l_ap(l_reduce(l_Bind), l_pair(LITERAL, $1))), $3); } 
      |     expr L_COLON_PLUS expr				{ pp(3, lex_binExpr);	$$ = l_binAp(l_reduce(l_Cons), $1, $3); }
      |     expr L_AT_EQ expr					{ pp(3, lex_binExpr);	$$ = l_binAp(l_useName("(@=)"), $3, $1); }
      |     selExpr L_COLON_EQ expr				{ pp(3, lex_binExpr);	$$ = l_ap3(l_useName("set"), l_arg(l_fun($1)), l_arg($1), l_coerce($3)); }
	;

monExpr:	L_COLON_COLON aExpr %prec L_PREFIX_COLON_COLON
									{ pp(2, lex_monExpr);	exitTypeSigContext();
													$$ = l_pair(COERCE, $2); }
	|	attribute expr %prec PREC_ATTRIBUTE       { pp(2, lex_monExpr);	$$ = l_pair($1, $2); }
	|	modifier expr %prec PREC_MODIFIER		{ pp(2, lex_monExpr);	$$ = l_noCoerce(l_pair($1, l_coerce($2))); }
	|	ptr expr %prec PREC_PTR      			{ pp(2, lex_monExpr);	$$ = l_monApNoCoerce(l_reduce($1), l_mapToConsEnv($2)); }
	|	L_TILDE ptr expr      				{ pp(2, lex_monExpr);	$$ = l_monApNoCoerce(l_pair(PLAIN_DATA_CON, $2), $3); }
	|	L_HYPHEN expr %prec L_PREFIX_HYPHEN		{ pp(2, lex_monExpr);	$$ = l_monAp(l_useName("negate"), $2); }
	|	L_PLUS expr %prec L_PREFIX_PLUS		{ pp(2, lex_monExpr);	$$ = l_monAp(l_useName("plus"), $2); }
     	|	notOp expr %prec PREC_LOG_NOT			{ pp(2, lex_monExpr);	$$ = l_monAp($1, $2); }
     	|	bitwiseNotOp expr %prec PREC_BITWISE_NOT	{ pp(2, lex_monExpr);	$$ = l_monAp($1, $2); }
     	|	dePtrOp expr %prec PREC_DEPTR 		{ pp(2, lex_monExpr);	$$ = l_monAp($1, $2); }

specialExpr:
		L_NUMSIGN_EXCLMARK aExpr			{ pp(2, lex_specialExpr);
													$$ = l_reduce($2); }
	|	L_EXCLMARK aExpr					{ pp(2, lex_specialExpr);
													$$ = l_eval($2); }
	|	L_NOTSIGN_COLON_COLON aExpr		      { pp(2, lex_specialExpr);
													$$ = l_noCoerce($2); }
	|	L_LPAREN_CARET_RPAREN aExpr			{ pp(2, lex_specialExpr);
													$$ = l_mapToConsEnv($2); }
	|	L_NOTSIGN_CARET aExpr		            { pp(2, lex_specialExpr);
													$$ = l_pair(NO_MAP_TO_ENV, $2); }
	;

sel:		aExpr selOp						{ pp(2, lex_sel);		$$ = l_ap($2, l_coerce($1)); }
	|	selExpr selOp					{ pp(2, lex_sel);		$$ = l_ap($2, l_reduce($1)); }
	|	selOp	%prec	L_PREFIX_DOT			{ pp(1, lex_sel);		$$ = l_ap($1, l_root); }
	;

selExpr:	sel idName						{ pp(2, lex_selExpr);	$$ = l_ap(l_reduce($1), l_pair(LITERAL, $2)); }
	;

selDataCon:	sel dataCon						{ pp(2, lex_selDataCon);
													$$ = l_ap($1, $2); }
	;

bExpr:	aExpr							{ pp(1, lex_bExpr);	$$ = $1; }
	|	specialExpr						{ pp(1, lex_bExpr);	$$ = $1; }
	;

aExpr:  	atom							{ pp(1, lex_aExpr);	$$ = $1; }
	|	idOrPrefixOp					{ pp(1, lex_aExpr);	$$ = l_noCoerce($1); }
	|	dataCon						{ pp(1, lex_aExpr);	$$ = $1; }
	|	L_TILDE dataCon       				{ pp(1, lex_aExpr);	$$ = l_noCoerce(l_pair(CONCRETE_DATA_CON, $2)); }
	|	L_TILDE_TILDE dataCon       			{ pp(1, lex_aExpr);	$$ = l_noCoerce(l_pair(CONCRETE_DATA_CON, $2)); }
	|	L_LPAREN compList L_RPAREN			{ pp(3, lex_aExpr);	$$ = l_apTuple($2, 0, TRUE); }
	|	L_TILDE L_LPAREN compList L_RPAREN		{ pp(3, lex_aExpr);	$$ = l_apTuple($3, 0, TRUE); }
	|	L_LSQB exprs L_RSQB	/* list */		{ pp(3, lex_aExpr);	$$ = $2; }
	|	L_LSQB expr L_SEMICOLON qualList L_RSQB	{ pp(5, lex_aExpr);	$$ = l_triple(LISTCOMP, l_coerce($2), $4); }
      |     L_LCURLYB sequence L_RCURLYB			{ pp(3, lex_aExpr);	$$ = l_sequence($2); }
	|	L_CASE expr L_OF L_LCURLYB caseAltList L_RCURLYB
									{ pp(6, lex_aExpr);	$$ =  l_triple(CASE, l_coerce($2), $5); }
	|	L_LPAREN expr L_RPAREN				{ pp(3, lex_aExpr);	$$ = $2; }
	|	L_THIS						{ pp(1, lex_aExpr);	$$ = THIS; }
	;

prefixOp:	L_LPAREN dyadicOp L_RPAREN			{ pp(3, lex_prefixOp);	$$ = $2; }
	;

selOp:      L_DOT							{ pp(1, lex_selOp);	$$ = l_useName("(.)"); }
      ;

expOp:      L_CARET						{ pp(1, lex_expOp);	$$ = l_useName("pow"); }
      ;
	
mulDivOp:	L_ASTERISK 						{ pp(1, lex_mulDivOp);	$$ = l_useName("(*)"); }
	|	L_SLASH						{ pp(1, lex_mulDivOp);	$$ = l_useName("(/)"); }
	|	L_DIV							{ pp(1, lex_mulDivOp);	$$ = l_useName("(div)"); }
	|	L_MOD							{ pp(1, lex_mulDivOp);	$$ = l_useName("(mod)"); }
	;

concatOp:	L_PLUS_PLUS						{ pp(1, lex_concatOp);	$$ = l_useName("(++)"); }
	;

addSubOp:	L_PLUS						{ pp(1, lex_addSubOp);	$$ = l_useName("(+)"); }
	|	L_HYPHEN						{ pp(1, lex_addSubOp);	$$ = l_useName("(-)"); }
	;

relOp:	L_LT							{ pp(1, lex_relOp);	$$ = l_useName("(<)"); }
	|	L_LT_EQ			 			{ pp(1, lex_relOp);	$$ = l_useName("(<=)"); }
	|	L_EQ							{ pp(1, lex_relOp);	$$ = l_useName("(=)"); }
	|	L_NOTSIGN_EQ   					{ pp(1, lex_relOp);	$$ = l_useName("(¬=)"); }
	|	L_GT_EQ						{ pp(1, lex_relOp);	$$ = l_useName("(>=)"); }
	|	L_GT							{ pp(1, lex_relOp);	$$ = l_useName("(>)"); }
	;

logAndOp:	logAndAlts 						{ pp(1, lex_logAndOp);	$$ = l_useName("(&)"); }
      ;

logAndAlts:	L_AND					            { pp(1, lex_logAndAlts);
													$$ = $$; }
	|	L_AMPERSAND 			            { pp(1, lex_logAndAlts);
													$$ = $$; }
	;

logOrOp:	logOrAlts 						{ pp(1, lex_logOrOp);	$$ = l_useName("(|)"); }
      ;

logOrAlts:	L_OR 						      { pp(1, lex_logOrAlts);	$$ = $$; }
	|	L_VERTBAR 					      { pp(1, lex_logOrAlts);	$$ = $$; }
	;

shiftOp:	L_LT_LT						{ pp(1, lex_shiftOp);	$$ = l_useName("(<<)"); }
	|	L_GT_GT						{ pp(1, lex_shiftOp);	$$ = l_useName("(>>)"); }
	;

bitAndOp:   L_NUMSIGN_AMPERSAND				{ pp(1, lex_bitAndOp);	$$ = l_useName("(#&)"); }
	;

bitXorOp:   L_NUMSIGN_CARET					{ pp(1, lex_bitXorOp);	$$ = l_useName("(#^)"); }
	;

bitOrOp:   L_NUMSIGN_VERTBAR					{ pp(1, lex_bitOrOp);	$$ = l_useName("(#|)"); }
	;

assignOp:  L_AT_EQ						{ pp(1, lex_assignOp);	$$ = l_useName("(@=)"); }
	;

dyadicOp:	selOp 						{ pp(1, lex_dyadicOp);	$$ = $$; }
	|	expOp							{ pp(1, lex_dyadicOp);	$$ = $$; }
	|	mulDivOp						{ pp(1, lex_dyadicOp);	$$ = $$; }
	|	concatOp                                  { pp(1, lex_dyadicOp);	$$ = $$; }
	|	addSubOp                                  { pp(1, lex_dyadicOp);	$$ = $$; }
	|	relOp							{ pp(1, lex_dyadicOp);	$$ = $$; }
	|	logAndOp                                  { pp(1, lex_dyadicOp);	$$ = $$; }
	|	logOrOp                                   { pp(1, lex_dyadicOp);	$$ = $$; }
      |	shiftOp                                   { pp(1, lex_dyadicOp);	$$ = $$; }
      |	bitAndOp                                  { pp(1, lex_dyadicOp);	$$ = $$; }
      |	bitXorOp                                  { pp(1, lex_dyadicOp);	$$ = $$; }
      |	bitOrOp                                   { pp(1, lex_dyadicOp);	$$ = $$; }
      |	assignOp                                  { pp(1, lex_dyadicOp);	$$ = $$; }
	;

notOp:	notAlts 						{ pp(1, lex_notOp);	$$ = l_useName("(not)"); }
      ;

notAlts:	L_NOT 						{ pp(1, lex_notAlts);	$$ = $$; }
	|	L_NOTSIGN 					      { pp(1, lex_notAlts);	$$ = $$; }
	;

bitwiseNotOp:
	L_NUMSIGN_NOTSIGN						{ pp(1, lex_bitwiseNotOp);	
													$$ = l_useName("(#¬)"); }
	;

dePtrOp:
	L_AT								{ pp(1, lex_dePtrOp);	$$ = l_useName("dePtr"); }
	;

monadicOp:	notOp 						{ pp(1, lex_monadicOp);	$$ = $1; }
	|	bitwiseNotOp					{ pp(1, lex_monadicOp);	$$ = $1; }
	|	dePtrOp						{ pp(1, lex_monadicOp);	$$ = $1; }
	;
      
exprs:	/* empty */						{ pp(0, lex_exprs);	$$ = l_useName("Nil"); }
	|	exprList						{ pp(1, lex_exprs);	$$ = $1; }
	;

exprList:	expr L_COMMA exprList				{ pp(3, lex_exprList);	$$ = l_ap(l_reduce(l_ap(l_reduce(l_Cons), l_coerce($1))), $3); }
	|	expr							{ pp(1, lex_exprList);	$$ = l_ap(l_reduce(l_ap(l_reduce(l_Cons), l_coerce($1))), l_useName("Nil")); }
	;

compList:	compList L_COMMA expr				{ pp(3, lex_compList);	$$ = l_cons($3, $1); }
	|	expr L_COMMA expr					{ pp(3, lex_compList);	$$ = l_cons($3, l_cons($1, l_Nil)); }
	;

caseAltList:
		caseAlt semiSeq caseAltList 			{ pp(3, lex_caseAltList);
													$$ = l_cons($1, $3 ); }
	|	caseAlt optSemi					{ pp(2, lex_caseAltList);
													$$ = l_cons($1, l_Nil); }
	;

caseAlt:	casePatternList L_HYPHEN_GT expr		{ pp(3, lex_caseAlt);	enterPendingArrowContext();									
													$$ = l_pair($1, l_coerce($3)); }
	;

casePatternList:	
		casePattern L_COMMA casePatternList		{ pp(3, lex_casePatternList);
													$$ = l_cons($1, $3 ); }
	|	casePattern						{ pp(1, lex_casePatternList);
													$$ = l_cons($1, l_Nil); }
	;

casePattern:
		pattern						{ pp(1, lex_casePattern);
													$$ = $1; }
	|	L_OTHERWISE						{ pp(1, lex_casePattern);
													$$ = OTHERWISE; }
	;


qual:		idName L_LT_HYPHEN expr				{ pp(3, lex_qual);	$$ = l_triple(GENERATOR, $1, l_coerce($3)); }
	|	expr							{ pp(1, lex_qual);	$$ = l_pair(FILTER, l_coerce($1)); }
	;

qualList:	qual L_COMMA qualList				{ pp(3, lex_qualList);	$$ = l_cons($1, $3); }
	|	qual							{ pp(1, lex_qualList);	$$ = l_cons($1, l_Nil); }
	;


typeVars:	/* empty */						{ pp(0, lex_typeVars);	$$ = l_Nil; }
	|	typeVarList						{ pp(1, lex_typeVars);	$$ = $1; }
	;
	
typeVarList:	
		typeVar typeVarList				{ pp(2, lex_typeVarList);
													$$ = l_cons($1, $2 ); }
	|	typeVar						{ pp(1, lex_typeVarList);
													$$ = l_cons($1, l_Nil); }
	;	

idOrPrefixOp:
		idName						{ pp(1, lex_idOrPrefixOp);
													$$ = $1; }
	|	prefixOp						{ pp(1, lex_idOrPrefixOp);
													$$ = $1; }
      |     L_LPAREN monadicOp L_RPAREN			{ pp(3, lex_idOrPrefixOp);
													$$ = $2; }
	;

bracketedSignedId:
	L_LPAREN signedId L_RPAREN				{ pp(3, lex_bracketedSignedId);
													$$ = $2; }
	;

signedId:	idOrPrefixOp					{ pp(1, lex_signedId);	$$ = $1; }
	|     idOrPrefixOp typeConstraint			{ pp(2, lex_signedId);	$$ = l_triple(CONSTRAIN, $1, $2); } 
	;	

idName:	L_LC_NAME					      { pp(1, lex_idName);	$$ = l_useName($1); }
	;

name:		L_LC_NAME					      { pp(1, lex_name);	$$ = l_useName($1); }
	|	L_UC_NAME					      { pp(1, lex_name);	$$ = l_useName($1); } 
	;

wildcard:	L_UNDERSCORE					{ pp(1, lex_wildcard);	$$ = WILDCARD_PATTERN; }
	;

modifier:	L_CONSTANT						{ pp(1, lex_modifier);	$$ = CONSTANT; }
	|	L_VARIABLE						{ pp(1, lex_modifier);	$$ = VARIABLE; }
	|	L_INDISPENSABLE					{ pp(1, lex_modifier);	$$ = INDISPENSABLE; }
	;

attribute:	L_TRANSIENT						{ pp(1, lex_attribute);	$$ = TRANSIENT; }
	|	L_PERSIST						{ pp(1, lex_attribute);	$$ = PERSIST; }
	|	L_PRIVATE						{ pp(1, lex_attribute);	$$ = PRIVATE; }
	|	L_PUBLIC						{ pp(1, lex_attribute);	$$ = PUBLIC; }
	;

modId:	name						      { pp(1, lex_modId);	$$ = $1; }
	;

typeCon:	L_UC_NAME						{ pp(1, lex_typeCon);	$$ = l_useName($1); }
	|	L_PTR       				      { pp(1, lex_typeCon);	$$ = l_Ptr; }
	|	L_TYPESIG_ASTERISK			      { pp(1,lex_typeCon);	$$ = l_Any; }
	;

tag:		L_TAG							{ pp(1, lex_tag);		$$ = TAG; }
	;

dataCon:	L_UC_NAME						{ pp(1, lex_dataCon);	$$ = l_dataCon($1); }
	;

ptr:        L_PTR						      { pp(1, lex_ptr);	      $$ = l_Ptr; }
	|	L_CARET						{ pp(1, lex_ptr);	      $$ = l_Ptr; }
	;

classId:	L_UC_NAME						{ pp(1, lex_classId);	$$ = l_useName($1); }
	;

typeVar:	idName						{ pp(1, lex_typeVar);	$$ = $1; }
	;

atom:		const							{ pp(1, lex_atom);	$$ = $1; }                       
	|	L_NUMSIGN name					{ pp(2, lex_atom);	$$ = l_pair(LITERAL, $2); }
	|	L_NUMSIGN_COLON_COLON aTypeSig		{ pp(2, lex_atom);	exitTypeSigContext(); $$ = l_pair(TYPE_SIG, $2); }
	|	L_NUMSIGN const					{ pp(2, lex_atom);	$$ = l_pair(LITERAL, $2); }
	;

const:	L_CHAR						{ pp(1, lex_const);	$$ = l_char($1); }                       
	|	L_INTEGER						{ pp(1, lex_const);	$$ = l_int($1); }
	|	L_HEX						{ pp(1, lex_const);	$$ = l_bits($1); }
	|	L_FIXED						{ pp(1, lex_const);	$$ = l_double($1); }
	|	L_STRING						{ pp(1, lex_const);	$$ = l_string(string_buf); } 
	;

semiSeq:	L_SEMICOLON						{ pp(1, lex_semiSeq); }
	|	semiSeq L_SEMICOLON				{ pp(2, lex_semiSeq); }
	;

optSemi:	/* empty */						{ pp(0, lex_optSemi); }
	|	semiSeq						{ pp(1, lex_optSemi); }
	;

%%

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
