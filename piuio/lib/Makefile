LIB_STATIC = libpiuio.a
LIB_DYNAMIC = libpiuio.so

GITREV = $(shell git rev-parse HEAD)
VERSION = "0.0.1"

PWD = $(shell pwd)
BIN = bin
OBJ = $(BIN)/obj
SRC = src

SOURCES = piuio-kmod.c piuio-usb.c version.c
OBJECTS = $(SOURCES:.c=.o)

OBJECT_FILES=$(addprefix $(OBJ)/, $(OBJECTS)) ../../util/bin/libpumpio-util.a

CC = gcc
AR = ar
INCDIRS = -I ../../util/src -I .
DEFINES= -D PIUIO_GITREV="$(GITREV)" -D PIUIO_VERSION="$(VERSION)"
CFLAGS = -g -Wall -Werror -O3 -fpic $(INCDIRS)
ARFLAGS = rcsT
LDLIBS = -lusb-1.0

default: help

.PHONY: build # Build the static and dynamic libraries
build: $(BIN)/$(LIB_STATIC) $(BIN)/$(LIB_DYNAMIC)

.PHONY: clean # Clean all build output files
clean:
	rm -rf $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) -c $(CFLAGS) $(OUTPUT_OPTION) $< 

$(BIN)/$(LIB_STATIC): $(OBJECT_FILES)
	$(AR) $(ARFLAGS) $@ $^

$(BIN)/$(LIB_DYNAMIC): $(OBJECT_FILES)
	$(CC) -shared -o $@ $^

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
	@echo piuio library project makefile.
	@echo
	@echo "Targets:"
	@grep '^.PHONY: .* #' Makefile | gawk 'match($$0, /\.PHONY: (.*) # (.*)/, a) { printf("  \033[0;32m%-25s \033[0;0m%s\n", a[1], a[2]) }'
