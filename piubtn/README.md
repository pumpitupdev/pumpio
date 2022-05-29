# PIUIO

The hardware was introduced with Pump It Up Pro as an upgrade/addon for FX and
GX cabinets and added additional menu buttons to the cabinet. Later cabinet
revisions had menu buttons built-in out of the factory.

* [lib](lib/README.md): User-space library to interface with the PIUBTN hardware.
  This includes a module utilizing libusb and another one using the kernel
  module
* [test](test/README.md): A small self-contained tool to test and debug setups
  using PIUBTN hardware.

## Building

Build all target: `make all`

Build output can be packaged for distribution and deployment using `make package`

The output is located under `bin/`

For further targets, see the help/usage output, run `make` or `make help`.
