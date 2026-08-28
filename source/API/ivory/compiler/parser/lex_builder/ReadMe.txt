These files build the ivory.l file in a consistent fashion,
and in a similar way to yacc_builder.

It is built with a make that should do something like:
ivory.l:	tables.txt lex_builder/head_lex lex_builder/sed_it lex_builder/tail_lex
	sed -f lex_builder/sed_it tables.txt > lex_builder/temp
	cat lex_builder/head_lex lex_builder/temp lex_builder/tail_lex > ivory.l
with appropriate directories.

It needs a tables.txt file, which has 4 fields per line:
token type, token number, token name, token string

