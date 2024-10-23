
# FIPS compliant IppCryptoLib
This version of IppCryptoLib utilizes a newer version of the ipp-crypto project that is certified to comply with the Federal Information Processing Standards. This version only supports build for the X64 architecture. A FIPS certified implementation is not available for the IA32 architecture in Slim Bootloader.

## Git Submodule
The ipp-crypto project is now included unmodified as a git submodule. This means that git submodules will need to be initialized as part of clone/fetch of the Slim Bootloader code.


## x86 Extended Instruction Set  Optimizations
The ipp-crypto project includes enhanced implementations of many crypto primitives using enhancements to the x86 ISA that are not present in every CPU. The optimization levels are represented internally by a letter/number combination. There are many more combinations than these supported by ipp-crypto, but these are the ones which have an effect on the primatives used by Slim Bootloader. The below table shows the optimized instruction sets used for each optimization level (row headings) for each algorithm (column headings). You should expect some performance increase from each lower row in the table.

|        | **SHA256** | **SHA384** | **SM3** | **RSA**   |
|--------|------------|------------|---------|-----------|
| **M7** | SSE3       | SSE3       | _None_  | SSE3      |
| **U8** | SSSE3      | SSE3       | SSSE3   | SSE3      |
| **Y8** | SSE4.2     | SSE3       | SSSE3   | SSE3      |
| **E9** | AVX        | AVX        | AVX     | SSE3      |
| **L9** | AVX2       | AVX2       | AVX2    | AVX2/SSE3 |

## ipp-crypto Project
The [Intel® Integrated Performance Primitives Cryptography Project](https://github.com/intel/cryptography-primitives) is a secure, fast and lightweight library of building blocks for cryptography, highly-optimized for various Intel® CPUs. ipp-crypto is validated for FIPS-140-3
You can find more information on the ipp-crypto github page.

### License
Intel® Cryptography Primitives Library is licensed under Apache License, Version 2.0.