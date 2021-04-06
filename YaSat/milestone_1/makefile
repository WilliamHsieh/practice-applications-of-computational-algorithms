CC = g++
FLAGS = -std=c++17 -Wall -Wextra -Wshadow -Wold-style-cast -pedantic -O3

OBJS = parser.o sat.o
EXENAME = yasat

all: $(OBJS)
	./$(EXENAME) data.cnf

EXENAME: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(EXENAME)

parser.o: parser.cpp parser.h
	$(CC) $(FLAGS) -c parser.cpp

sat.o: sat.cpp parser.h makefile
	$(CC) $(FLAGS) -c sat.cpp

.PHONY: clean
clean:
	rm -rf $(OBJS) $(EXENAME) a.out
