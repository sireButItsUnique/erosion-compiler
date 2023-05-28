SHELL=/bin/bash

CC=g++
CFLAGS=-std=c++20 -Wall -Wextra -pedantic -g -march=native
SRCS=$(wildcard *.cpp) $(shell ls */*.cpp)
HDRS=$(shell ls */*.hpp)
PCHS=$(HDRS:.hpp=.hpp.gch)
OBJS=$(SRCS:.cpp=.o)

debug: erosion
	bash -c "./erosion test.cor ; exit 0"

release: CFLAGS += -O2
release: erosion
	strip -s erosion

erosion: $(PCHS) $(OBJS)
	$(CC) $(CFLAGS) -o erosion $(OBJS)

%.hpp.gch: %.hpp
	g++ $(CFLAGS) $< -o $@

%.o: %.cpp $(PCHS) Makefile
	$(CC) $(CFLAGS) -H -Winvalid-pch -c $< -o $@

clean:
	rm -f $(OBJS) $(PCHS) erosion
