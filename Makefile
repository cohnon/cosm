CC = gcc -std=c17
FLAGS = -g -Wall -Wextra -Werror -pedantic

BUILD_DIR = build
BIN = $(BUILD_DIR)/cosm

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

# commands
.Phony: all
all: $(BIN)

.Phony: clean
clean:
	rm -r $(BUILD_DIR)


$(BIN): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $(FLAGS) $^ -o $@
