LIB = libpumpio-util.a

PWD = $(shell pwd)
BIN = bin
OBJ = $(BIN)/obj
SRC = src

SOURCES = usb.c
OBJECTS = $(SOURCES:.c=.o)

OBJECT_FILES=$(addprefix $(OBJ)/, $(OBJECTS))

CC = gcc
AR = ar
INCDIRS = -I .
CFLAGS = -g -Wall -Werror -O3 -fpic $(INCDIRS)
ARFLAGS = rcs
LDLIBS = -lusb-1.0

.PHONY: build # Build the static library
build: $(BIN)/$(LIB)

.PHONY: clean # Clean all build output files
clean:
	rm -rf $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) -c $(CFLAGS) $(OUTPUT_OPTION) $< 

$(BIN)/$(LIB): $(OBJECT_FILES)
	$(AR) $(ARFLAGS) $@ $^

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
	@echo pumpio utility library project makefile.
	@echo
	@echo "Targets:"
	@grep '^.PHONY: .* #' Makefile | gawk 'match($$0, /\.PHONY: (.*) # (.*)/, a) { printf("  \033[0;32m%-25s \033[0;0m%s\n", a[1], a[2]) }'