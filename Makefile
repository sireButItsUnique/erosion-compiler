CC=g++
CFLAGS=-std=c++11 -Wall -Wextra -pedantic -O0 -g
SRCS=$(wildcard *.cpp) $(wildcard */*.cpp)
OBJS=$(SRCS:.cpp=.o)

debug: erosion
	./erosion test.cor

release: erosion
	strip -s erosion

erosion: $(OBJS)
	$(CC) $(CFLAGS) -o erosion $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) erosion
