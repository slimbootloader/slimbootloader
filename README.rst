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
Please visit [Supported Hardware](https://slimbootloader.github.io/supported-hardware/index.html) page for a list of platforms supported by Slim Bootloader.

Platform CI Build Status
------------------------

============================= ============= ============= ============= =============
 Toolchain                    DEBUGx86      RELEASEx86    DEBUGx64      RELEASEx64
============================= ============= ============= ============= =============
QEMU_Win_VS2019               |_unknown|    |_unknown|    |_unknown|    |_unknown|
QEMU_Lin_GCC5                 |qem_l32d|    |_unknown|    |_unknown|    |qem_l64r|
APL_Win_VS2019                |apl_w32d|    |_unknown|    |_unknown|    |apl_w64r|
APL_Lin_GCC5                  |_unknown|    |_unknown|    |apl_l64d|    |_unknown|
CFL_Win_VS2019                |_unknown|    |cfl_w32r|    |cfl_w64d|    |_unknown|
CFL_Lin_GCC5                  |_unknown|    |_unknown|    |_unknown|    |cfl_l64r|
CMLV_Win_VS2019               |_unknown|    |cmlv_w32r|   |cmlv_w64d|   |_unknown|
CMLV_Lin_GCC5                 |_unknown|    |_unknown|    |_unknown|    |cmlv_l64d|
CML_Win_VS2019                |_unknown|    |cml_w32r|    |cfl_w64d|    |_unknown|
CML_Lin_GCC5                  |cml_l32d|    |_unknown|    |_unknown|    |_unknown|
EHL_Win_VS2019                |_unknown|    |ehl_w32r|    |ehl_w64d|    |_unknown|
EHL_Lin_GCC5                  |ehl_l32d|    |_unknown|    |_unknown|    |_unknown|
TGL_Win_VS2019                |_unknown|    |tgl_w32r|    |tgl_w64d|    |_unknown|
TGL_Lin_GCC5                  |tgl_l32d|    |_unknown|    |_unknown|    |_unknown|
============================= ============= ============= ============= =============

Getting Started
---------------
Documentation for compiling, using, and extending Slim Bootloader can be found
[here](https://slimbootloader.github.io/).

Mailing List
--------------
You can subscribe to our mailing list for questions or share your learnings:
https://groups.io/g/slimbootloader

Topic Archive: https://groups.io/g/slimbootloader/topics

License
-------
Slim Bootloader is released under the terms of the BSD-2-Clause Plus Patent License.
Please see the `LICENSE` file for the full terms of the license.

Slim Bootloader may include other Open Source Software licensed pursuant to license agreement(s)
identified in the applicable source code file(s) and/or file header(s).

.. |_unknown| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20UNKNOWN
.. |qem_l32d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=QEMU
.. |qem_l64r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Linux&configuration=Linux%20QEMU_X64_RELEASE
.. |apl_w32d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20APL_X86_DEBUG
.. |apl_w64r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20APL_X64_RELEASE
.. |apl_l64d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Linux&configuration=Linux%20APL_X64_DEBUG
.. |cfl_w32r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20CFL_X86_RELEASE
.. |cfl_w64d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20CFL_X64_DEBUG
.. |cfl_l64r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Linux&configuration=Linux%20CFL_X64_RELEASE
.. |cmlv_w32r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20CMLV_X86_RELEASE
.. |cmlv_w64d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20CMLV_X64_DEBUG
.. |cmlv_l64d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Linux&configuration=Linux%20CMLV_X64_DEBUG
.. |cml_w32r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20CML_X86_RELEASE
.. |cml_w64d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20CML_X64_DEBUG
.. |cml_l32d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Linux&configuration=Linux%20CML_X86_DEBUG
.. |tgl_w32r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20TGL_X86_RELEASE
.. |tgl_w64d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20TGL_X64_DEBUG
.. |tgl_l32d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Linux&configuration=Linux%20TGL_X86_DEBUG
.. |ehl_w32r| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20EHL_X86_RELEASE
.. |ehl_w64d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Windows&configuration=Windows%20EHL_X64_DEBUG
.. |ehl_l32d| image:: https://dev.azure.com/slimbootloader/slimbootloader/_apis/build/status/slimbootloader.slimbootloader?branchName=master&jobName=Linux&configuration=Linux%20EHL_X86_DEBUG
