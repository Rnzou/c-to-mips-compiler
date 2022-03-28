CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include_ast
 
all : src/parser.tab.cpp src/parser.tab.hpp src/lexer.yy.cpp bin/c_compiler compiler 

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y
	@echo "Parser.y compile "
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.flex src/parser.tab.hpp
	@echo "lexer.flex compile"
	flex -o src/lexer.yy.cpp  src/lexer.flex
bin/c_compiler : src/compiler.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o
	@echo "compiler.cpp compile"
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^

compiler : bin/c_compiler

clean :
	rm src/*.o
	rm bin/*
	rm src/*.tab.cpp
	rm src/*.yy.cpp
	rm src/*.output 
	