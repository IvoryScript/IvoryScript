/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    start.cpp
 *
 * Module:  Ivory machine
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Invoke start for process if present.
 *
 * Call format:
 *
 *    retCode = start(void)
 *
 * Method:
 *
 *    Creates a new environment and attempts to evaluate .start.
 *
 *    If .start is an action the return code is 0, otherwise the result of
 *    .start is returned if of type Int, otherwise -1.
 * Errors:
 *
 *    Errors are signalled by a return of -1.
 *
 * Notes:
 *
 *    02/10/03 -  AJS   Added .interpreter boolean flag
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/bool.h"
#include "ivory/class.h"
#include "ivory/int.h"
#include "ivory/root.h"
#include "ivory/string.h"
#include "ivory/void.h"

#define redAp(fun,arg,msa) (mkReducibleAp((fun),(arg),(msa)))
#define redAp2(fun,arg1,arg2,msa) (redAp(redAp((fun),(arg1),(msa)),(arg2),(msa)))

extern Expr mkReducibleAp(Expr fun, Expr arg, MSA& msa);

extern bool enableInterpreter;

Name errorLogFileName_name = builtInName(errorLogFileName);
Name interpreter_name      = builtInName(interpreter);
Name start_name            = builtInName(start);
Name traceLogFileName_name = builtInName(traceLogFileName);

classMethodRef(hasVar, "hasVar");
classMethodRef(sel, "(.)");

Int start(void)
{
   preEval(NULL, NULL, 0);

// Enable error logging if .errorLogFileName present

   eval(redAp2(classMethod(hasVar),
               fromName(errorLogFileName_name),
               Root, *consMSA), *consEnv n_vm);
   if (rTag == TRUE)
   {
      eval(redAp2(classMethod(sel),
                  Root,
                  fromName(errorLogFileName_name), *consMSA), *consEnv n_vm);
      if (rType != builtInType(String))
         ivoryError("start: errorLogFileName not string");
      else
      {
         errorLogFlag      = true;
         errorLogFileName  = (const char*)rPtr;
      }
   }

// Enable trace logging if .traceLogFileName present

   eval(redAp2(classMethod(hasVar),
               fromName(traceLogFileName_name),
               Root, *consMSA), *consEnv n_vm);
   if (rTag == TRUE)
   {
      eval(redAp2(classMethod(sel),
                  Root,
                  fromName(traceLogFileName_name), *consMSA), *consEnv n_vm);
      if (rType != builtInType(String))
         ivoryError("start: traceLogFileName not string");
      else
      {
         traceLogFlag      = true;
         traceLogFileName  = (const char*)rPtr;
      }
   }

// Enable interpreter if .interpreter present

   enableInterpreter = false;
   eval(redAp2(classMethod(hasVar),
               fromName(interpreter_name),
               Root, *consMSA), *consEnv n_vm);
   if (rTag == TRUE)
   {
      eval(redAp2(classMethod(sel), Root, fromName(interpreter_name), *consMSA),
           *consEnv n_vm);
      if (rType != builtInType(Bool))
         ivoryError("start: interpreter not bool");
      else
         enableInterpreter = rTag == TRUE;
   }

// Evaluate start if present

   eval(redAp2(classMethod(hasVar),
               fromName(start_name),
               Root, *consMSA), *consEnv n_vm);
   if (rTag == TRUE)
   {
      trace("Invoking start");

      eval(redAp2(classMethod(sel),
                  Root,
                  fromName(start_name), *consMSA), *consEnv n_vm);

      postEval(FALSE);

      return rType == builtInType(Void) ? 0 : (rType == builtInType(Int) ? rInt : -1);
   }
   else
   {
      postEval(FALSE);
      return -1;
   }
}
