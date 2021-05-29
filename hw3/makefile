SRC_DIR := src/
SOURCE  := main.cpp
TARGET  := viterbi

CC      := g++
CCFLAGS := -std=c++17 -Ofast -Wall -Wextra -Wshadow

.PHONY: test clean

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC_DIR)$(SOURCE)
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	rm -rf $(TARGET)
