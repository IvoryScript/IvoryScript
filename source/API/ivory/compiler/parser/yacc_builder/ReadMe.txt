These files build the ivory.y file in a consistent fashion,
and in a similar way to lex_builder.

It is built with a make that should do something like:
ivory.y:	sed_it_1 sed_it_2 sed_it_3 head_yacc mid1_yacc mid2_yacc tail_yacc
	sed -f sed_it_1 ../tables.txt > temp1
	sed -f sed_it_2 ../tables.txt > temp2
	sed -f sed_it_3 ../tables.txt > temp3
	cat head_yacc temp1 mid1_yacc temp2 mid2_yacc temp3 tail_yacc > ivory.y
with appropriate directories.

It needs a tables.txt file, which has 4 fields per line:
token type, token number, token name, token string

The files are as follows:
head_yacc: Usual early bits
sed_it_1: builds the %token for terminals and %type for non-terminals
mid1_yacc: builds the operator precedence declarations
sed_it_2: builds the #defines for the non-terminals to allow the stack dump
mid2_yacc: glue
sed_it_3: the strings for the stack dump
tail_yacc: grammar, etc.