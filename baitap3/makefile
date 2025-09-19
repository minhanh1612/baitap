CC      = gcc
CFLAGS  = -Wall -Wextra -g

SRC_DIR = src
BIN_DIR = bin

TARGETS = process1 process2 zombie orphan
TARGETS_PATH = $(addprefix $(BIN_DIR)/,$(TARGETS))

all: $(TARGETS_PATH)

$(BIN_DIR)/process1: $(SRC_DIR)/process1.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/process2: $(SRC_DIR)/process2.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/zombie: $(SRC_DIR)/zombie.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

$(BIN_DIR)/orphan: $(SRC_DIR)/orphan.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<
clean:
	rm -f $(BIN_DIR)/*
