/*------------------------------------------------------------------------------
 *
 * (c) Copyright (see Date)
 *
 * Name:    yystack.cpp
 *
 * Module:  Ivory yacc stack utilities
 *
 * Author:  H Glaser
 *
 * Date:    17 April 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Functions to maintain a readable version of the yacc stack.
 *    Allows a report of the "parse so far"
 *
 *
 * Errors:
 *
 * Notes:
 *
 * Modification history:
 *
 *-----------------------------------------------------------------------------
 */

#include "general.h"
#include "ivory/compiler/parser.h"

//#define MONITOR_STACK_OPS	

#define STACKSIZE 	         8000
#define BOTTOM		            0
#define MAX_ELEMENTS_TO_PRINT 20

int lex_stack[ STACKSIZE ];
int stackPointer = BOTTOM;

/* put a token onto lex_stack */
void lex_push( int word )
{
	extern int stackPointer;

	lex_stack [ ++stackPointer]= word; /* add item */
	if (stackPointer == STACKSIZE)
	{ /* filled entire lex_stack */
			error("lex stack overflow");
	};

#ifdef MONITOR_STACK_OPS
   char buf[256];
   sprintf(buf, "%d: <-- ", parserLineNumber());
   outbit(buf);
   show_lex_token(word);
   outbit("\n");
#endif

//   dump_lex_stack();
/*	if (show_parsing)
	{
*/
// 	dump_lex_stack();
/*	};
*/}

/* pop 'depth' items off the lex_stack */
void lex_pop( int depth )
{
	extern int stackPointer;

#ifdef MONITOR_STACK_OPS
   char buf[256];
   sprintf(buf, "%d: --> ", parserLineNumber());
   outbit(buf);
   for (int i = 0; i < depth; i++) {
      if (i > 0)
         outbit(" ");
      show_lex_token(lex_stack[stackPointer - depth]);
}
   outbit("\n");
#endif

	if (stackPointer - depth < BOTTOM )
	{ /* popped more than pushed */
			error("lex stack error");
	};

   stackPointer -= depth;
}

void show_lex_token(int token) {
   const char* name = lex_names[token - 10001];
   if (token > 0 && name != NULL)
      outbit(name);
}

void dump_lex_stack() {
	extern int stackPointer;
	
   outbit("Stack dump:");

   for (int i = stackPointer; i >= stackPointer - MAX_ELEMENTS_TO_PRINT &&
                              i >= BOTTOM; --i) {
		outbit(" ");
      show_lex_token(lex_stack[i]);
	} 
	outnew();
}

/* empty the entire lex_stack */
void clear ()
{
	extern int stackPointer;
	
   stackPointer = BOTTOM;
}
