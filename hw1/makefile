CC = g++
ARGS = -std=c++20 -O2 -Wall -Wextra -Wshadow -Wpedantic

SRC = main.cpp
TAR = solver
SIZE = 16

run: solver
	./solver $(SIZE) answer.txt MiniSat_v1.14_linux
	cat answer.txt
ifeq ($(SIZE), 16)
	diff answer.txt 16.ans
endif

solver: makefile $(SRC)
	$(CC) $(ARGS) $(SRC) -o $(TAR)

clean:
	rm -f a.out answer.txt sat.in sat.out $(TAR)

