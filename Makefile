SHELL=/bin/bash

CC=g++
CFLAGS=-std=c++20 -Wall -Wextra -pedantic -g -march=native
SRCS=$(wildcard *.cpp) $(shell ls {FinalCodeGenerator,IntermediateCodeGenerator,LexicalAnalysis,Preprocessor,SemanticAnalysis,SyntaxAnalysis}/*.cpp)
OBJS=$(SRCS:.cpp=.o)

debug: erosion
	bash -c "./erosion test.cor ; exit 0"

release: CFLAGS += -O2
release: erosion
	strip -s erosion

erosion: $(OBJS)
	$(CC) $(CFLAGS) -o erosion $(OBJS)

SyntaxAnalysis/ParseNode.o: SyntaxAnalysis/grammar.hpp

%.o: %.cpp Makefile
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) erosion
