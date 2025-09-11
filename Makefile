CC = gcc -std=c17
FLAGS = -g -Wall -Wextra -Werror -pedantic
INCLUDE = -Ishared

BUILD_DIR = build
BIN = $(BUILD_DIR)/cosm

FE_SRC = $(wildcard frontend/src/*.c)
SH_SRC = $(wildcard shared/*.c)

SRCS = $(FE_SRC) $(SH_SRC)
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
	$(CC) -c $(FLAGS) $(INCLUDE) $^ -o $@
