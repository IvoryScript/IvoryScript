%{
#include <stdlib.h>
#include "ivory/bits.h"
#include "ivory/compiler/parser.h"
#define YY_NO_UNISTD_H 1
#define input() yyinput()
%}
%option never-interactive
%option noyywrap
%option yylineno
%x incl
%x str
%x cmd
%{
#define MAX_INCLUDE_DEPTH 10
#define MAX_STR_CONST 65535
#define LEX_EOF 0
YY_BUFFER_STATE bufferStack[MAX_INCLUDE_DEPTH];
char* fileNameStack[MAX_INCLUDE_DEPTH];
FILE* fileStack[MAX_INCLUDE_DEPTH];
int lineNoStack[MAX_INCLUDE_DEPTH];
int stackPtr = 0;
char string_buf[MAX_STR_CONST];
char *string_buf_ptr;
char escapeSeq(char c){
switch (c){
case 'a': return '\a';
case 'b': return '\b';
case 'e': return '\e';
case 'f': return '\f';
case 'n': return '\n';      
case 'r': return '\r';
case 't': return '\t';
case 'v': return '\v';
case '\\':return '\\';
case '\'':return '\'';
case '"': return '"';
case '?': return '?';
default:return '\0';}}
%}

DIGIT       [0-9]
HEXDIG      [0-9a-fA-f]
UC_NAME     [A-Z][A-Za-z0-9'_]*
LC_NAME     [a-z][A-Za-z0-9'_]*
ESCAPE      [ntvbrfa\\?'"]

ANY         .
%%
"module" lex_push(L_MODULE); return(L_MODULE);
"def" lex_push(L_DEF); { BEGIN(cmd); return L_DEF; };
"with" lex_push(L_WITH); return(L_WITH);
"where" lex_push(L_WHERE); return(L_WHERE);
"undef" lex_push(L_UNDEF); { BEGIN(cmd); return L_UNDEF; };
"do" lex_push(L_DO); return(L_DO);
"if" lex_push(L_IF); return(L_IF);
"then" lex_push(L_THEN); return(L_THEN);
"else" lex_push(L_ELSE); return(L_ELSE);
"case" lex_push(L_CASE); return(L_CASE);
"of" lex_push(L_OF); { enterPendingArrowContext(); return L_OF; };
"otherwise" lex_push(L_OTHERWISE); return(L_OTHERWISE);
"let" lex_push(L_LET); return(L_LET);
"in" lex_push(L_IN); return(L_IN);
"return" lex_push(L_RETURN); return(L_RETURN);
"this" lex_push(L_THIS); return(L_THIS);
"inline" lex_push(L_INLINE); return(L_INLINE);
"TAG" lex_push(L_TAG); return(L_TAG);
";" lex_push(L_SEMICOLON); return(L_SEMICOLON);
"[" lex_push(L_LSQB); return(L_LSQB);
"]" lex_push(L_RSQB); return(L_RSQB);
"(" lex_push(L_LPAREN); return(L_LPAREN);
")" lex_push(L_RPAREN); return(L_RPAREN);
"{" lex_push(L_LCURLYB); return(L_LCURLYB);
"}" lex_push(L_RCURLYB); return(L_RCURLYB);
"\\" lex_push(L_BACKSLASH); { enterPendingArrowContext(); return L_BACKSLASH; };
":=" lex_push(L_COLON_EQ); return(L_COLON_EQ);
"@=" lex_push(L_AT_EQ); return(L_AT_EQ);
"<-" lex_push(L_LT_HYPHEN); return(L_LT_HYPHEN);
"!" lex_push(L_EXCLMARK); return(L_EXCLMARK);
"#!" lex_push(L_NUMSIGN_EXCLMARK); return(L_NUMSIGN_EXCLMARK);
"," lex_push(L_COMMA); return(L_COMMA);
"_" lex_push(L_UNDERSCORE); return(L_UNDERSCORE);
"::" lex_push(L_COLON_COLON); { enterTypeSigContext(); return L_COLON_COLON; };
"¬::" lex_push(L_NOTSIGN_COLON_COLON); return(L_NOTSIGN_COLON_COLON);
"&" lex_push(L_AMPERSAND); return(L_AMPERSAND);
"|" lex_push(L_VERTBAR); return(L_VERTBAR);
"->" lex_push(L_HYPHEN_GT); { if (pendingArrowContext()) { if (typeSigContext()) return L_TYPESIG_HYPHEN_GT; else { exitPendingArrowContext(); return L_HYPHEN_GT;} } else return L_TYPESIG_HYPHEN_GT; };
"<<" lex_push(L_LT_LT); return(L_LT_LT);
">>" lex_push(L_GT_GT); return(L_GT_GT);
"¬" lex_push(L_NOTSIGN); return(L_NOTSIGN);
"not" lex_push(L_NOT); return(L_NOT);
"and" lex_push(L_AND); return(L_AND);
"or" lex_push(L_OR); return(L_OR);
"#&" lex_push(L_NUMSIGN_AMPERSAND); return(L_NUMSIGN_AMPERSAND);
"#|" lex_push(L_NUMSIGN_VERTBAR); return(L_NUMSIGN_VERTBAR);
"#^" lex_push(L_NUMSIGN_CARET); return(L_NUMSIGN_CARET);
"#¬" lex_push(L_NUMSIGN_NOTSIGN); return(L_NUMSIGN_NOTSIGN);
"<" lex_push(L_LT); return(L_LT);
"<=" lex_push(L_LT_EQ); return(L_LT_EQ);
"=" lex_push(L_EQ); return(L_EQ);
"¬=" lex_push(L_NOTSIGN_EQ); return(L_NOTSIGN_EQ);
">=" lex_push(L_GT_EQ); return(L_GT_EQ);
">" lex_push(L_GT); return(L_GT);
"." lex_push(L_DOT); return(L_DOT);
":" lex_push(L_COLON); return(L_COLON);
":+" lex_push(L_COLON_PLUS); return(L_COLON_PLUS);
"++" lex_push(L_PLUS_PLUS); return(L_PLUS_PLUS);
"+" lex_push(L_PLUS); return(L_PLUS);
"-" lex_push(L_HYPHEN); return(L_HYPHEN);
"*" lex_push(L_ASTERISK); { return typeSigContext() ? L_TYPESIG_ASTERISK : L_ASTERISK; };
"/" lex_push(L_SLASH); return(L_SLASH);
"div" lex_push(L_DIV); return(L_DIV);
"mod" lex_push(L_MOD); return(L_MOD);
"^" lex_push(L_CARET); return(L_CARET);
"(^)" lex_push(L_LPAREN_CARET_RPAREN); return(L_LPAREN_CARET_RPAREN);
"¬^" lex_push(L_NOTSIGN_CARET); return(L_NOTSIGN_CARET);
"#" lex_push(L_NUMSIGN); return(L_NUMSIGN);
"##" lex_push(L_NUMSIGN_NUMSIGN); return(L_NUMSIGN_NUMSIGN);
"#::" lex_push(L_NUMSIGN_COLON_COLON); { enterTypeSigContext(); return L_NUMSIGN_COLON_COLON; };
"#@" lex_push(L_NUMSIGN_AT); return(L_NUMSIGN_AT);
"@" lex_push(L_AT); return(L_AT);
"=>" lex_push(L_EQ_GT); return(L_EQ_GT);
"~" lex_push(L_TILDE); return(L_TILDE);
"~~" lex_push(L_TILDE_TILDE); return(L_TILDE_TILDE);
"~@" lex_push(L_TILDE_AT); return(L_TILDE_AT);
"type" lex_push(L_TYPE); return(L_TYPE);
"object" lex_push(L_OBJECT); return(L_OBJECT);
"class" lex_push(L_CLASS); { enterTypeSigContext(); return L_CLASS; };
"primitive" lex_push(L_PRIMITIVE); return(L_PRIMITIVE);
"instance" lex_push(L_INSTANCE); { enterTypeSigContext(); return L_INSTANCE; };
"subordinate" lex_push(L_SUBORDINATE); return(L_SUBORDINATE);
"constant" lex_push(L_CONSTANT); return(L_CONSTANT);
"variable" lex_push(L_VARIABLE); return(L_VARIABLE);
"indispensable" lex_push(L_INDISPENSABLE); return(L_INDISPENSABLE);
"dynamic" lex_push(L_DYNAMIC); return(L_DYNAMIC);
"transient" lex_push(L_TRANSIENT); return(L_TRANSIENT);
"persistent" lex_push(L_PERSIST); return(L_PERSIST);
"private" lex_push(L_PRIVATE); return(L_PRIVATE);
"public" lex_push(L_PUBLIC); return(L_PUBLIC);
"matching" lex_push(L_MATCHING); return(L_MATCHING);
"Ptr" lex_push(L_PTR); return(L_PTR);
include		BEGIN(incl);

{UC_NAME}		{	yylval._string = (char*)_msa->alloc(yyleng + 1);
				strcpy(yylval._string, yytext);
				lex_push(L_UC_NAME);
				return L_UC_NAME;
			}

{LC_NAME}		{	yylval._string = (char*)_msa->alloc(yyleng + 1);
				strcpy(yylval._string, yytext);
				lex_push(L_LC_NAME);
				return L_LC_NAME;
			}

{DIGIT}+		{	yylval._int=atoi(yytext);
				lex_push(L_INTEGER);
				return L_INTEGER;
			}

0[xX]{HEXDIG}+	{	yytext[1]='x';
				yylval._bits=(Bits)strtoul(yytext,NULL,16);
				lex_push(L_HEX);
				return L_HEX;
			}

{DIGIT}+(("."{DIGIT}+)?)(([eE](["+"-]?){DIGIT}+)?) {
				yylval._double=atof(yytext);
				lex_push(L_FIXED); 
				return L_FIXED;
			}

\'[^']		{	input();
				yylval._char = yytext[1];
				lex_push(L_CHAR);
				return L_CHAR;
                  }

\'\\{ESCAPE}\'    {     yylval._char = escapeSeq(yytext[2]);
				lex_push(L_CHAR);
				return L_CHAR;
                  }

\'\\{DIGIT}+\'	{	yylval._char =(char)(atoi(&yytext[2]));
				lex_push(L_CHAR);
				return L_CHAR;
			}

\'\\[xX]{HEXDIG}+\' {
				yylval._char =(char)strtol(&yytext[3],NULL,16);
				lex_push(L_CHAR);
				return L_CHAR;
			}

\"	                  string_buf_ptr = string_buf; BEGIN(str);

<str>\"	      {
		            BEGIN(INITIAL);
		            *string_buf_ptr = '\0';
				lex_push(L_STRING);
				return L_STRING;
		      }

<str>\n	      {
		            error("unterminated string constant");
		      }

<str>\\[0-7]{1,3} {
                        int result;
                        (void) sscanf( yytext + 1, "%o", &result );
                        if ( result > 0xff )
	                        error("string constant out of range");

                        *string_buf_ptr++ = result;
                  }

<str>\\[0-9]+     {
                        error("invalid character escape");
		      }

<str>\\[xX]{HEXDIG}+ {
                        long result = strtol(yytext + 2, NULL, 16);
                        if (result > 0xff)
                                error("string hex escape out of range");
                        *string_buf_ptr++ = (char)result;
                  }

<str>\\{ESCAPE}   {
                        *string_buf_ptr++ = escapeSeq(yytext[1]);
                  }
<str>\\(.|\n)           *string_buf_ptr++ = yytext[1];

<str>[^\\\n\"]+   {
		            char *yptr = yytext;
		            while ( *yptr )
			            *string_buf_ptr++ = *yptr++;
		      }

<cmd>"{"		{ BEGIN(INITIAL); lex_push(L_CMD_LCURLYB); return L_CMD_LCURLYB; }
<cmd>[ \t\n]+	{}
<cmd>.		{ yyless(0); BEGIN(INITIAL); }

[ \t\r\n]		;

"--".*		;

"{-"			{	char c;
				unsigned int level = 1;
				for ( ; ; ) {

               while ((c = input()) != '-' &&
                        c != '{' &&
                        c != 0)
                     ;

               if (c == 0) {
                     error1("Mismatched nested comment at line %d", yylineno);
                     break;
               }

               if (c == '-') {
                     while ((c = input()) == '-') ;
                     if (c == 0) {
                        error1("Mismatched nested comment at line %d", yylineno);
                        break;
                     }
                     if (c == '}' && --level == 0)
                        break;
               } else if (c == '{') {
                     while ((c = input()) == '{') ;
                     if (c == 0) {
                        error1("Mismatched nested comment at line %d", yylineno);
                        break;
                     }
                     if (c == '-')
                        level++;
					}
				}
			}

<incl>[ \t]*	;

<incl>\"[^"]*	{	if (input() == '"')
					yytext[yyleng]='\0';
				else {
					error("expected \"");
					return 0;
				}

				if (stackPtr >= MAX_INCLUDE_DEPTH) {
					error("Too many nested includes");
					yyterminate();
				} else {
					char* fileName = (char*)_msa->alloc(yyleng - 1 + 1);
					strcpy(fileName, yytext + 1);

					if ((yyin = fopen(fileName, "r")) == NULL) {
						error1("Can\'t open include file %s", fileName);
						yyterminate();
					}
					fileNameStack[stackPtr] = fileName;
                              fileStack[stackPtr] = yyin;
					bufferStack[stackPtr] = YY_CURRENT_BUFFER;
					lineNoStack[stackPtr] = yylineno;
					yylineno = 1;
					yy_switch_to_buffer(
						yy_create_buffer(yyin, YY_BUF_SIZE));
                              stackPtr++;
					BEGIN(INITIAL);
				}
			}
			
<<EOF>>		{
				if (--stackPtr < 0) {
					stackPtr = 0;
					yyterminate();
				} else {
					yylineno = lineNoStack[stackPtr];
                              fclose(fileStack[stackPtr]);
					yy_delete_buffer(YY_CURRENT_BUFFER);
					yy_switch_to_buffer(bufferStack[stackPtr]);
				}
			}
	
{ANY}		{}		
%%

extern FILE* openFile(void);
extern void clearParserState(void);
int initParser(const char* filename) {
   if ((yyin = fopen(filename, "r")) == NULL) {
      error1("Can\'t open file \"%s\"\n", filename);
      return 0;
   }
   clearParserState();
   return 1;
}

void termParser() {
   if (yyin != NULL) {
      fclose(yyin);
      yyin = NULL;
   }
}

void* createParserBuffer(const char* s) {
   return (void*)yy_scan_string(s);
}

void destroyParserBuffer(void* buf) {
   yy_flush_buffer((YY_BUFFER_STATE)buf);
   yy_delete_buffer((YY_BUFFER_STATE)buf);
}
