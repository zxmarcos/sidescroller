flex -o lexer.cpp airLexer.l
bison -d -t -o parser.cpp airGrammar.y
