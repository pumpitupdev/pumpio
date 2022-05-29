# PIUIO kernel module

This kernel module replicates the infamous "R16 kernel hack" that was introcued
on the R16 update on In The Groove 2. This patch improved input accuracy
significantly by reducing user-space call overhead. To complete a single update
cycle, only a single user to kernel call is required with the kernel module
instead of 8 when using the user-space usb library libusb.

## Achknowledgements

The code is based on version 0.1 of
[djpohly's piuio kernel module](https://github.com/djpohly/piuio). As their
development took a different direction with later versions scrapping the
concept of synchronous polling, I re-created this to support input handling of
older Stepmania forks like OpenITG.

Last commit before references to `/dev/piuio0` are removed:
`e71220344cef853ff3e0d309be85a048019b5d8b`

## Read: Measure before you judge

Over the years, many people encountered inaccurate/weird/wonky timing on 
OpenITG or other Stepmania forks which were caused by a low update frequency
in the available IO drivers.

However, the Linux kernel has evolved as well as other relevant parts to
interface with USB hardware. According to my measurements on different
hardware that came out in the last 5 years and with recent kernels (5.x line),
implementations depending on user-space abstractions such as libusb can be
on-par with the kernel space solution performance-wise.

Therefore, a general statement of "the kernel module is always better than
the non-kernel module solution" does not hold true in all cases. I highly
advice everyone to benchmark different solutions and base your decisions on
actual numbers from running on YOUR target hardware and configuration than
trusting outdated information passed around.

You can utilize this toolset to execute such measurements by using the
`benchmark` mode of [piuio-test](../test/README.md).

### Sample measurements

* B450 board with Ryzen 3200g: ~2.8 ms avg. per full update cycle poll
  (synchronous). Same measurement with libusb-based solution on that setup

## Building

Build all target: `make build`

Build output `piuio.ko` is located next to this file.

For further targets, see the help/usage output, run `make` or `make help`.

## Installing

Run `make install` to install it on the current system, may require root
permissions.

## Loading

Run `insmod piuio.ko` to load the module. Use `lsmod | grep piuio` to check
if loaded. Unload with `rmmod piuio`.

## Configuration

To configure the module use `modprobe piuio key=value`

Available parameters:

* `timeout_ms`: Timeout in ms for USB messages to complete before considered
  an error. Default: 10 ms

## Tools

* [piuio-test](../test/README.md) for testing and debugging
* [lib](../lib/README.md) provides a library abstracting the device to
  communicate with it via the kernel module