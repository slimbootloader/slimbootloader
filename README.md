Slim Bootloader
================

**Welcome to the Slim Bootloader project!**

Slim Bootloader is an open-source boot firmware solution, built from the
ground up to be secure, lightweight, and highly optimized while leveraging
robust tools and libraries from the [EDK II](https://www.tianocore.org/edk2/)
framework.

What is a Bootloader?
---------------------
Just like a typical [BIOS](https://en.wikipedia.org/wiki/BIOS), Slim Bootloader
is the piece of software responsible for initializing the core hardware
components of the system when it is powered on, followed by the loading and
launching of the desired operating system.

Slim Bootloader offers the flexibility for OEMs, device manufacturers, and hobbyists
to isolate, configure, and optimize for the the specific requirements of the system 
thereby achieving unparalleled boot performance and minimizing firmware footprint.

Supported Platforms
-------------------
Slim Bootloader currently has support for [Apollo Lake](https://www.intel.com/content/www/us/en/embedded/products/apollo-lake/overview.html) based designs including 
* [UP Squared](http://www.up-board.org/upsquared/) maker board
* Intel Leaf Hill customer reference design
* MinnowBoard 3 Module

Additionally, Slim Bootloader supports booting a virtual machine via [QEMU](https://www.qemu.org/).

Getting Started
---------------
Documentation for compiling, using, and extending Slim Bootloader can be found
[here](https://slimbootloader.github.io/).

Mailing List
--------------
You can subscribe to our mailing list for questions or share your learnings: 
https://lists.01.org/mailman/listinfo/sbl-devel

Archives: https://lists.01.org/pipermail/sbl-devel/

License
-------
Slim Bootloader is released under the terms of the BSD license. 
Please see the `LICENSE` file for the full terms of the license.
