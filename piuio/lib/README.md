# PIUIO library

A static/dynamic library to interface with the USB PIUIO hardware. This provides
an abstraction layer on device level including data structures for mapping
inputs and outputs. Any higher level logic regarding evaluation of this data
and driving the hardware is not in the scope of this library.

* [piuio-kmod](src/piuio-kmod.h): Module to interface via the
  [kernel module](../kmod/README.md) with the device
* [piuio-usb](src/piuio-usb.h): Module to interface with the device using
  libusb

## Building

Build all target: `make build`

Build output is located under `bin/`.

For further targets, see the help/usage output, run `make` or `make help`.

## Usage

In your project, either include the static build output in your object file list
or dynamically link `libpiuio.so`. For API usage, refer to header files.

Extensive usage example given in [piuio-test project](../piuio-test/README.md).
