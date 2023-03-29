CC=g++
CFLAGS=-std=c++11 -Wall -Wextra -pedantic -O0 -g
# SRCS=main.cpp FinalCodeGenerator/finalGen.cpp IntermediateCodeGenerator/intermediateGen.cpp LexicalAnalysis/flag.cpp LexicalAnalysis/lexicalAnalysis.cpp LexicalAnalysis/syntaxToken.cpp Preprocessor/preprocessor.cpp SemanticAnalysis/semanticAnalysis.cpp SyntaxAnalysis/syntaxAnalysis.cpp
SRCS=$(wildcard *.cpp) $(wildcard */*.cpp)
OBJS=$(SRCS:.cpp=.o)

release: all
	strip -s erosion

dev: all
	./erosion test.cor

all: erosion

erosion: $(OBJS)
	$(CC) $(CFLAGS) -o erosion $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) erosion
