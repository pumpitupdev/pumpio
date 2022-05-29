# PIUIO

Also known as USB PIUIO or MK6IO.

* [kmod](kmod/README.md): A Linux kernel module that replicates the infamous
  "R16 kernel hack" interface.
* [lib](lib/README.md): User-space library to interface with the PIUIO hardware.
  This includes a module utilizing libusb and another one using the kernel
  module
* [test](test/README.md): A small self-contained tool to test and debug setups
  using PIUIO hardware.

## Building

Build all target: `make all`

Build output can be packaged for distribution and deployment using `make package`

The output is located under `bin/`

For further targets, see the help/usage output, run `make` or `make help`.

## Personal notes regarding IO refresh rate and issues with certain AMD hardware

Related to [measure before you judge](kmod/README.md), I have encountered a line
of mainboard/CPU combinations that are, apparently, badly engineered and
introduce significant delay over USB ports that are not connected directly
to the CPU but over an internal HUB.

This is apparently a [very wide-ranging issue](https://www.google.com/search?q=amd+ryzen+usb+issues&source=hp&ei=Q5STYru1E-aTxc8Pnsm_mAU&iflsig=AJiK0e8AAAAAYpOiU8XHVfuNivALl4bZfDONVLphje2u&ved=0ahUKEwj74sCIhoX4AhXmSfEDHZ7kD1MQ4dUDCAg&uact=5&oq=amd+ryzen+usb+issues&gs_lcp=Cgdnd3Mtd2l6EAMyBAgAEBMyCAgAEB4QFhATMggIABAeEBYQEzIICAAQHhAWEBMyCAgAEB4QFhATMggIABAeEBYQEzIICAAQHhAWEBM6EQguEIAEELEDEIMBEMcBENEDOhEILhCABBCxAxCDARDHARCjAjoOCC4QgAQQsQMQxwEQowI6CAgAELEDEIMBOgsIABCABBCxAxCDAToICC4QsQMQgwE6CAgAEIAEELEDOgsILhCABBDHARDRAzoQCC4QsQMQgwEQxwEQ0QMQQzoKCC4QsQMQgwEQQzoHCAAQsQMQQzoECC4QQzoECAAQQzoLCC4QgAQQsQMQgwE6CAguEIAEELEDOgUIABCABDoOCC4QgAQQsQMQxwEQ0QM6BggAEB4QFlAAWNwVYKQWaABwAHgAgAGcAYgB8A2SAQQxNC41mAEAoAEB&sclient=gws-wiz) with a large line of AMD Ryzen CPUs and mainboards. Further references:

* [AMD Suggests Possible Fixes for Ryzen USB Connectivity Issues](https://www.tomshardware.com/news/amd-suggest-possible-fixes-for-usb-connectivity-issues)
* [Do you guys still have USB issues with Ryzen 5000 CPUs with an up to date BIOS?](https://linustechtips.com/topic/1402063-do-you-guys-still-have-usb-issues-with-ryzen-5000-cpus-with-an-up-to-date-bios/)
* [Ryzen USB Connectivity Issues Questions](https://www.reddit.com/r/Amd/comments/rer86x/ryzen_usb_connectivity_issues_questions/)

The symptoms described differ from what you see polling PIUIO hardware on these
boards: Synchronous polling is highly delayed resulting in low refresh rates
for input states. This results in weird/wonky/inaccurate timing during gameplay.

At the time of writing this, any BIOS updates or other suggested
fixes/mitigations did not solve the issue.

The only solutions I found are:

* Test all USB ports and see if one or two are delivering expected performance.
  This apparently means they are not connected over a "bad HUB" that creates the
  IO bottleneck resulting in major latency. On my test-hardware, this resulted
  in an 8x increase of latency from ~3 ms to 24 ms for one input update cycle.
* Get different hardware that doesn't suck

# TODOs

* piuio-kmod
  * Have async implementation utilizing urb, switchable
* Support piuio mk3/5
* Add lxio
* Add piubtn
