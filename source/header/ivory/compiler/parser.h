/*-----------------------------------------------------------------------------
 *
 * (c) Copyright (see Date) by Alasdair Scott
 *
 * Name:    parser.h
 *
 * Author:  A Scott
 *
 * Date:    24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory parser.
 *
 * Modification history:
 *
 *-----------------------------------------------------------------------------
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

#ifndef IVORY_COMPILER_PARSER_H_DEFINED
#define IVORY_COMPILER_PARSER_H_DEFINED

#include "ivory/common.h"

#define ATOMIC_DATA_CON_FLAG  1
#define DATA_CON_FLAG         2
#define PRIM_FN_FLAG          4  
#define CLASS_METHOD_FLAG     8


extern Void clearParserContext(Void);

extern Void enterPendingArrowContext(Void);

extern Void exitPendingArrowContext(Void);

extern Bool pendingArrowContext(Void);

extern Bool typeSigContext(Void);

extern Void enterTypeSigContext(Void);

extern Void exitTypeSigContext(Void);

extern Expr parser(NameTable& nameTable, MSA& msa);

extern int parserLineNumber(void);

extern void lex_push(int word);

extern void lex_pop(int depth);

extern void show_lex_token(int word);

extern void dump_lex_stack();

extern void outbit(const char* s);

extern void outnew();

extern void clear();

extern const char * const lex_names[];

#endif /* IVORY_COMPILER_PARSER_H_DEFINED */
