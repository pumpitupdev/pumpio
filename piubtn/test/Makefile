EXEC = piubtn-test

GITREV = $(shell git rev-parse HEAD)

PWD = $(shell pwd)
BIN = bin
OBJ = $(BIN)/obj
SRC = src

SOURCES = main.c options.c
OBJECTS = $(SOURCES:.c=.o)

OBJECT_FILES=$(addprefix $(OBJ)/, $(OBJECTS)) ../lib/bin/libpiubtn.a

CC = gcc
INCDIRS = -I ../../util/src -I ../lib/src -I .
DEFINES= -D GITREV="$(GITREV)"
CFLAGS = -g -Wall -O3 -fpic $(INCDIRS)
LDLIBS = -lusb-1.0

default: help

.PHONY: build # Build the executable
build: $(BIN)/$(EXEC)

.PHONY: clean # Clean all build output files
clean:
	rm -rf $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) -c $(CFLAGS) $(OUTPUT_OPTION) $< 

$(BIN)/$(EXEC): $(OBJECT_FILES)
	$(CC) -o $@ $^ $(LDLIBS)

# -----------------------------------------------------------------------------
# Utility, combo and alias targets
# -----------------------------------------------------------------------------

# Help screen note:
# Variables that need to be displayed in the help screen need to strictly
# follow the pattern "^[A-Z_]+ \?= .* # .*".
# Targets that need to be displayed in the help screen need to add a separate
# phony definition strictly following the pattern "^\.PHONY\: .* # .*".

.PHONY: help # Default target, print help screen
help:
	@echo piubtn-test application project makefile.
	@echo
	@echo "Targets:"
	@grep '^.PHONY: .* #' Makefile | gawk 'match($$0, /\.PHONY: (.*) # (.*)/, a) { printf("  \033[0;32m%-25s \033[0;0m%s\n", a[1], a[2]) }'