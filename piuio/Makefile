PWD = $(shell pwd)
BIN = $(PWD)/bin

default: help

.PHONY: all # Build everything
all: lib test kmod

.PHONY: clean # Clean all build output from the project
clean:
	rm -rf bin/
	$(MAKE) -C $(PWD)/kmod clean
	$(MAKE) -C $(PWD)/lib clean
	$(MAKE) -C $(PWD)/test clean
	$(MAKE) -C $(PWD)/../util clean

.PHONY: kmod
kmod:
	$(MAKE) -C $(PWD)/kmod build

.PHONY: lib # Build the piuio static and dynamic libraries
lib:
	$(MAKE) -C $(PWD)/../util
	$(MAKE) -C $(PWD)/lib build

.PHONY: test # Build the piuio-test tool
test: lib
	$(MAKE) -C $(PWD)/test build

.PHONY: package # Create a distribution packages (zip-file) of all binary output files
package: all $(BIN) $(BIN)/piuio.zip

$(BIN):
	mkdir -p $(BIN)

$(BIN)/piuio.zip: \
    kmod/piuio.ko \
    lib/bin/libpiuio.a \
	lib/bin/libpiuio.so \
	test/bin/piuio-test \

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
	@echo piuio project makefile.
	@echo
	@echo "Targets:"
	@grep '^.PHONY: .* #' Makefile | gawk 'match($$0, /\.PHONY: (.*) # (.*)/, a) { printf("  \033[0;32m%-25s \033[0;0m%s\n", a[1], a[2]) }'
