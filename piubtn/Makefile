PWD = $(shell pwd)
BIN = $(PWD)/bin

default: help

.PHONY: all # Build everything
all: lib test

.PHONY: clean # Clean all build output from the project
clean:
	rm -rf bin/
	$(MAKE) -C $(PWD)/lib clean
	$(MAKE) -C $(PWD)/test clean
	$(MAKE) -C $(PWD)/../util clean

.PHONY: lib # Build the piubtn static and dynamic libraries
lib:
	$(MAKE) -C $(PWD)/../util
	$(MAKE) -C $(PWD)/lib build

.PHONY: test # Build the piubtn-test tool
test: lib
	$(MAKE) -C $(PWD)/test build

.PHONY: package # Create a distribution packages (zip-file) of all binary output files
package: all $(BIN) $(BIN)/piubtn.zip

$(BIN):
	mkdir -p $(BIN)

$(BIN)/piubtn.zip: \
    lib/bin/libpiubtn.a \
	lib/bin/libpiubtn.so \
	test/bin/piubtn-test \

	$(V)echo ... $@
	$(V)zip -j $@ $^

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
	@echo piubtn project makefile.
	@echo
	@echo "Targets:"
	@grep '^.PHONY: .* #' Makefile | gawk 'match($$0, /\.PHONY: (.*) # (.*)/, a) { printf("  \033[0;32m%-25s \033[0;0m%s\n", a[1], a[2]) }'
