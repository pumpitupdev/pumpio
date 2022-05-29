PWD = $(shell pwd)

default: help

.PHONY: all # Build all sub-projects
all: piuio

.PHONY: clean # Clean all build output of all sub-projects
clean:
	$(MAKE) -C $(PWD)/piuio clean

.PHONY: piuio # Build the piuio sub-project
piuio:
	$(MAKE) -C $(PWD)/piuio all

.PHONY: package # Create distribution packages (zip-files) of the sub-projects
package:
	$(MAKE) -C $(PWD)/piuio package

.PHONY: code-format # Run clang-format over the entire code base using the provided code style in .clang-format
code-format:
	@echo "Applying clang-format..."
	@find . -name '*.c' -o -name '*.h' | xargs clang-format -i -style=file

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