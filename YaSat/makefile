TARGET     := yasat

SRC_DIR    := src/
INC_DIR    := include/
OBJ_DIR    := obj/
SCRIPT_DIR := scripts/

CC         := g++
CCFLAGS    := -I$(INC_DIR) -std=c++17 -Ofast -Wall -Wextra -Wshadow -Wold-style-cast
# CCFLAGS    := -I$(INC_DIR) -std=c++17 -Ofast -Wall -Wextra -Wshadow -Wold-style-cast -DDEBUG -g

SRC_FILES  := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJ_FILES  := $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRC_FILES))

all: $(shell mkdir -p $(OBJ_DIR)) $(TARGET)

checker: $(SCRIPT_DIR)check.cpp
	$(CC) $(CCFLAGS) -o $@ $^

$(TARGET): $(OBJ_FILES)
	$(CC) $(CCFLAGS) -o $@ $^

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp makefile
	$(shell mkdir -p $(dir $@))
	$(CC) $(CCFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET) checker
