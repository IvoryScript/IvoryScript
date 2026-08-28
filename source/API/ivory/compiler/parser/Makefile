#-v -Wcex

ivory.tab.cpp: ivory.y lex.yy.c
	bison -l -v -Wcex -o ivory.tab.cpp ivory.y

ivory.l:	tables.txt lex_builder/head_lex lex_builder/awk_it lex_builder/tail_lex
	LANG=ISO-8591-1 awk -f lex_builder/awk_it tables.txt > lex_builder/temp
	cat lex_builder/head_lex lex_builder/temp lex_builder/tail_lex > ivory.l

lex.yy.c:	ivory.l
	flex -L ivory.l

ivory.y:	tables.txt yacc_builder/awk_it_1 yacc_builder/awk_it_2 yacc_builder/awk_it_3 yacc_builder/head_yacc yacc_builder/mid1_yacc yacc_builder/mid2_yacc yacc_builder/tail_yacc
	LANG=ISO-8591-1 awk -f yacc_builder/awk_it_1 tables.txt > yacc_builder/temp1
	LANG=ISO-8591-1 awk -f yacc_builder/awk_it_2 tables.txt > yacc_builder/temp2
	LANG=ISO-8591-1 awk -f yacc_builder/awk_it_3 tables.txt > yacc_builder/temp3
	cat yacc_builder/head_yacc yacc_builder/temp1 yacc_builder/mid1_yacc yacc_builder/temp2 yacc_builder/mid2_yacc yacc_builder/temp3 yacc_builder/tail_yacc > ivory.y
