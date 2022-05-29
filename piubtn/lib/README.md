# PIUBTN library

A static/dynamic library to interface with the USB PIUBTN hardware. This provides
an abstraction layer on device level including data structures for mapping
inputs and outputs. Any higher level logic regarding evaluation of this data
and driving the hardware is not in the scope of this library.

* [piubtn-usb](src/piubtn-usb.h): Module to interface with the device using
  libusb

## Building

Build all target: `make build`

Build output is located under `bin/`.

For further targets, see the help/usage output, run `make` or `make help`.

## Usage

In your project, either include the static build output in your object file list
or dynamically link `libpiubtn.so`. For API usage, refer to header files.

Extensive usage example given in [piubtn-test project](../piubtn-test/README.md).
