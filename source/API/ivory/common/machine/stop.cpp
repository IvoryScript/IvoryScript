/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    stop.cpp
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
 *    Invoke stop for process if present.
 *
 * Call format:
 *
 *    retCode = stop(void)
 *
 * Method:
 *
 *    Creates a new environment and attempts to evaluate .stop.
 *
 *    Returns False is .stop evalutes to False, otherwise True.
 *
 * Notes:
 *
 *    A script may postpone termination by returning False from '.stop'.
 *    In this case, the primitive 'exit' should be invoked at an
 *    an appropriate later point.
 *
 * Errors:
 *
 *    Errors are signalled by a return of -1.
 *
 * Notes:
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/bool.h"
#include "ivory/class.h"
#include "ivory/root.h"

#define redAp(fun,arg,msa) (mkReducibleAp((fun),(arg),(msa)))
#define redAp2(fun,arg1,arg2,msa) (redAp(redAp((fun),(arg1),(msa)),(arg2),(msa)))

extern Cell* mkReducibleAp(Expr fun, Expr arg, MSA& msa);

Name stop_name            = builtInName(stop);

classMethodRef(hasVar, "hasVar");
classMethodRef(sel, "(.)");

Bool stop(void) {
   preEval(NULL, NULL, 0);

   eval(redAp2(classMethod(hasVar),
               fromName(stop_name),
               Root, *consMSA), *consEnv n_vm);
   if (rType == builtInType(Bool) && rTag == TRUE)
   {
      trace("Invoking stop");

      eval(redAp2(classMethod(sel),
         Root,
         fromName(stop_name), *consMSA),
         *consEnv n_vm);

      postEval(FALSE);

      return rType != builtInType(Bool) || rTag == TRUE; 
   }
   else
   {
      return TRUE;

      postEval(FALSE);
   }
}
