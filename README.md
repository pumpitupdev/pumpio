# Pump It Up I/O tools

This repository provides tooling to interface with various types of I/O
hardware used on the Pump It Up game series.

* [PIUIO](piuio/README.md): USB PIUIO/MK6 I/O introduced with MK6 hardware and Exceed 2
* [PIUBTN](piubtn/README.md): Additional menu buttons introduced with Pump It Up Pro

## Building

Build all target for all sub-projects: `make all`

Build output can be packaged for distribution and deployment using `make package`

The output is located under `bin/` in the respective sub-projects

For further targets, see the help/usage output, run `make` or `make help`.