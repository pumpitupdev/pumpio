# PIUIO command line test and debug tool

A small self-contained tool to test and debug setups using PIUIO hardware.

## Building

Build all target: `make build`

Build output is located under `bin/`.

For further targets, see the help/usage output, run `make` or `make help`.

## Running

The tool provides different modes of operation with a few different parameters
to tweak depending on your setup.

Run `piuio-test -h` to print the usage/help screen explaining the available
parameters. When simply running the tool without any arguments, it defaults to
raw text output utilizing libusb.
