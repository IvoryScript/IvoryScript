/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    compiler.cpp
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
 *    IvoryScript compiler.
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

#ifndef MINIMAL_IOSTREAM
   #include <sstream>
   #include <fstream>
   using namespace std;
#endif

#include "OptionsParser.h"
#include "ivory/byteCodeModule.h"
#include "ivory/exec.h"
#include "ivory/iFrameDescr.h"
#include "ivory/segment.h"
#include "ivory/trace.h"
#include "ivory/compiler/compiler.h"
#include "ivory/compiler/analyser.h"
#include "ivory/compiler/byteCode.h"
#include "ivory/compiler/parser.h"
#include "ivory/compiler/sourceCode.h"
#include "ivory/compiler/tran.h"
#include "ivory/compiler/type.h"

void* createParserBuffer(const char* s);

void destroyParserBuffer(void* buf);

#define DEFAULT_BLK_LEN             16384
#define DEFAULT_NAME_SLOTS          997
#define DEFAULT_NAMES_PER_SEG       256
#define DEFAULT_TYPE_SLOTS          997
#define DEFAULT_TYPES_PER_SEG       256
#define MODULE_EXPORT_MAP_N_SLOTS   997

Void termParser();

extern Name nameSupply;

Subst* globalSubsts = NULL;

static Void run(Bool initFlag, Bool isModule, Bool verbose,
                Int memLimit,
                IAddress codeSegment, Env& codeEnv,
                IAddress mainEntry,
                istream* iStream, ostream* oStream);

// compilerSequence: Execute the compiler sequence for a given target {"code", "source", "bytes"}
// If immFlag is set, s is a string otherwise it is the name of a file.

int compilerSequence(ConstString target, ConstString s, ConstString options,
                     istream* iStream, ostream* oStream) {
   MSA* msa_ = new MSA(DEFAULT_BLK_LEN);
   MSA& msa = *msa_;
   Env* env_ = new(msa) Env(msa ,
                            *new(msa) NameTable(DEFAULT_NAME_SLOTS,
                                                DEFAULT_NAMES_PER_SEG,
                                                msa),
                            NULL, NULL,
                            *new(msa) TypeTable(DEFAULT_TYPE_SLOTS,
                                                DEFAULT_TYPES_PER_SEG,
                                                msa));
   Env& env = *env_;

   resetNameSupply();
   resetTypeVarSupply();

   OptionsParser compilerOptions(options);
   Bool verbose = compilerOptions.getString("verbose") != NULL;
   Bool immFlag = compilerOptions.getString("script") != NULL;
   Bool listFlag = compilerOptions.getString("list") != NULL;

// Parse, translate and evaluate script

   Void* parserBuffer = NULL;

   if (immFlag)
      parserBuffer = createParserBuffer(s);
   else
      if (!initParser(s)) {
         ivoryError("Can\'t initialise parser");
         return FALSE;
      }

   if (verbose)
      outStream << "*** Compiling: " << s << endl;

   Expr tree = parser(env.nameTable(), msa);

   OptionsParser parserOptions(compilerOptions.getString("parser"));
   if (parserOptions.hasKey("printAST")) {
      printExpr(tree, outStream, env);
      outStream << "\n--------------------------------------------\n";
      outStream.flush();
   }

   if (immFlag)
      destroyParserBuffer(parserBuffer);
   else
      termParser();

   if (tree == (Expr)VOID)
      return TRUE;   // Empty script

   else if (tree == (Expr)ERROR) {
      compilerError("could not parse expression");
      return FALSE;
   }

   if (immFlag) {
		if (!(isPair(tree) && fst(tree) == Expr(ORDER))) {
			compilerError("expected an ORDER script");
			return FALSE;
		}
      tree = mkPair(ORDER, snd(tree), msa);
   }

   if (verbose) {
      outStream << ">>>>>> building AST\n";
      outStream.flush();
   }

   if (!Tran(1, compilerOptions.getString("tran"), env).tran(tree))
      return FALSE;

   if (verbose) {
      outStream << ">>>>>> resolving types\n";
      outStream.flush();
   }

   globalSubsts = NULL;

   if (!TypeCheck(compilerOptions.getString("typeCheck"),env).typeCheck(tree))
      return FALSE;

   if (verbose) {
      outStream << ">>>>>> processing AST\n";
      outStream.flush();
   }

   if (!Tran(2, compilerOptions.getString("tran"), env).tran(tree))
      return FALSE;

   Code* code;
   OptionsParser codeOptionsParser(options);
   const char* codeOptions = codeOptionsParser.getString("target");
   if (strcmp(target, "code") == 0) {
      listFlag = TRUE;
      code = new Code(codeOptions, env, msa);
   } 
   else if (strcmp(target, "bytes") == 0)
      code = new ByteCode(codeOptions, env, msa);
   else if (strcmp(target, "source") == 0)
      code = new SourceCode(codeOptions, env, msa);
   else {
      error1("Invalid target %s", target);
      exit(0);
   }

   if (verbose) {
      outStream << ">>>>>> analysing AST\n";
      outStream.flush();
   }

   {
      OptionsParser parser(options);
      Analyser analyser(parser.getString("analyse"), env, msa);
      analyser.analyse(tree);
      if (analyser.nErrors() > 0)
         return FALSE;
   }

   if (verbose) {
      outStream << ">>>>>> generating intermediate code\n";
      outStream.flush();
   }

   if (formOf(tree) == MODULE) {
      ModuleDefn& moduleDefn = toBody(tree, ModuleDefn);
       if (!moduleDefn.gen(*code))
         return FALSE;
   } else {
      if (!toBody(tree, Order).gen(*code))
         return FALSE;
   }

   if (verbose) {
      outStream << ">>>>>> generating target code\n";
      outStream.flush();
   }

   if (listFlag) {
      code->genTarget(outStream);
      outStream.flush();
   } else {
      std::ofstream fs("/dev/null", ios::out);
      code->genTarget((ostream&)fs);
      fs.flush();
   }

   if (verbose) {
      outStream << ">>>>>> done\n";
      outStream.flush();
   }

   if (verbose)
      msa.info(outStream, 0);

   if (strcmp(target, "bytes") == 0) {
      OptionsParser runOptions(compilerOptions.getString("run"));

      if (runOptions.hasKey("trace")) {
         verbose = TRUE;
         traceInterpreter(TRUE);
      }

      if (formOf(tree) == MODULE) {
         ModuleDefn& moduleDefn = toBody(tree, ModuleDefn);
         ByteCodeModule* module = new(env.msa()) ByteCodeModule(moduleDefn.name(),
            &moduleDefn.env(),
            moduleDefn.typeConNameMap(),
            moduleDefn.exportMap(),
            ((ByteCode*)code)->codeSegment(),
            ((ByteCode*)code)->rackOrigin());
      }

      try {
         static Int memLimit = 0;

         if (runOptions.getNum("memoryLimit", memLimit))
            msa.setLimit(memLimit);

         run(!immFlag, formOf(tree) == MODULE, verbose,
            memLimit,
            ((ByteCode*)code)->codeSegment(),
            code->env(),
            ((ByteCode*)code)->mainEntry(),
            iStream, oStream);
      }
      catch (RunTimeError err) {
         if (strlen(err.reason()) > 0) {
            outStream << "\n*** Run time error: " << err.reason() << endl;
            outStream.flush();
         }
      }

      if (formOf(tree) == ORDER) {
         MSAFrameDescrMapAssoc::remove(msa_);
         delete msa_;
      }
 
      return TRUE;
   }
   else {
      delete msa_;
      return FALSE;
   }
}

Void run(Bool initFlag, Bool isModule, Bool verbose,
         Int memLimit,
         IAddress codeSegment, Env& codeEnv,
         IAddress mainEntry,
         istream* iStream, ostream* oStream) {
   preEval(iStream, oStream, memLimit);

// Assign initial segment imports (dynamic linking)

   assignSegmentImports(codeSegment, codeEnv);

   hashSegment(codeSegment, FALSE);

   pushLabel(NULL);
   emptyStack();
   callIContinuation(initCodeAddr(codeSegment) n_vm);

//   pushLabel(NULL);
   emptyStack();
   callIContinuation(mainEntry n_vm);
   outStrm->flush();

   postEval(verbose);
}

Void compilerError(ConstString msg) {
   error(msg);
}
