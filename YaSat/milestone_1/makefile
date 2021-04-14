CC = g++
FLAGS = -std=c++17 -Wall -Wextra -Wshadow -Wold-style-cast -pedantic -O3
DEBUG = -g -Ddebug

OBJS = parser.o sat.o
solver = yasat
checker = check

all: $(solver) $(checker)
	./$(solver) data.cnf && cat data.sat

$(checker): check.cpp
	$(CC) $(FLAGS) -o $(checker) check.cpp

$(solver): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(solver)

parser.o: parser.cpp parser.h
	$(CC) $(FLAGS) -c parser.cpp

sat.o: sat.cpp parser.h makefile
	$(CC) $(FLAGS) -c sat.cpp
# 	$(CC) $(FLAGS) $(DEBUG) -c sat.cpp

.PHONY: clean
clean:
	rm -rf $(OBJS) $(solver) $(checker) a.out
