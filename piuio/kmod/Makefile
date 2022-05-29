PWD = $(shell pwd)
KERNEL = $(shell uname -r)
KERNEL_DIR = /lib/modules/$(KERNEL)/build

obj-m := piuio.o

default: help

.PHONY: build # Build the kernel module piuio.ko
build:
	make -C $(KERNEL_DIR) M=$(PWD) modules

.PHONY: install # Install the kernel module on the current system
install:
	make -C $(KERNEL_DIR) M=$(PWD) modules_install

.PHONY: clean # Clean all build output
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

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
	@echo piuio kernel module project makefile.
	@echo
	@echo "Targets:"
	@grep '^.PHONY: .* #' Makefile | gawk 'match($$0, /\.PHONY: (.*) # (.*)/, a) { printf("  \033[0;32m%-25s \033[0;0m%s\n", a[1], a[2]) }'