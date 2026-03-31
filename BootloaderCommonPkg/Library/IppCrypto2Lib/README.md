
# FIPS-compliant IppCryptoLib

Uses ipp-crypto (unmodified git submodule) with FIPS mode (`IPPCP_FIPS_MODE`) enabled for X64 variants M7/U8/Y8/E9/L9.

Initialise submodules after clone: `git submodule update --init --recursive`


## x86 Optimization Variants

Each `.inf` builds a different ISA tier. Only tiers marked "Yes" for SHA-NI and AVX2 should use L9.

|        | **SHA256** | **SHA384** | **SM3**  | **RSA**   |
|--------|------------|------------|----------|-----------|
| **M7** | SSE3       | SSE3       | _None_   | SSE3      |
| **U8** | SSSE3      | SSSE3      | SSSE3    | SSE3      |
| **Y8** | SSE4.2     | SSSE3      | SSSE3    | SSE3      |
| **E9** | AVX        | AVX        | AVX      | SSE3      |
| **L9** | SHA-NI     | AVX2       | AVX (E9) | AVX2/SSE3 |

**L9 SHA384:** `pcpsha512l9as.nasm` requires 32-byte aligned `.data` constants (`vmovdqa ymm`),
which is not guaranteed in XIP. `sha384.c` dispatches at runtime: pure-C (`UpdateSHA512_C`)
when executing from flash (Stage1A), AVX2 (`sha512_hashUpdate`) when in RAM (Stage1B+).

## ipp-crypto Project

[Intel® Cryptography Primitives Library](https://github.com/intel/cryptography-primitives) —
FIPS-140-3 validated, licensed under Apache 2.0.

---

## ipp-crypto v2.0.0 L9 FIPS Selftest Fix

v2.0.0 added runtime-dispatch SHA method files (`*_tt.c`) that call `IsFeatureEnabled()`,
which reads `cpFeaturesMask` — a global that is ROM-mapped in XIP and always reads 0.
This causes AVX2 paths to be selected in Stage1A, leading to `#GP(13)` from unaligned
`vmovdqa ymm` on flash-mapped constants.

**Fix:** `IppCrypto2LibL9.inf` defines `IPPCP_CUSTOM_BUILD` and
`IPP_CUSTOM_ENABLED_FEATURES=0x80000` (ippCPUID_SHA). This replaces the runtime
`IsFeatureEnabled()` call with a compile-time constant, unconditionally selecting
the SHA-NI path for SHA256 — no alignment requirement, XIP-safe.

**L9 is only valid on platforms with SHA-NI (CNL and later).** Do not use L9 on
Haswell/Broadwell/Skylake/Kaby Lake (AVX2 but no SHA-NI) — use E9 instead.

### L9 XIP Hash Backend Summary

| Algorithm | Stage1A (XIP) | Stage1B+ (RAM) |
|---|---|---|
| SHA256 | SHA-NI (`sha256_ni_hashUpdate`) | SHA-NI |
| SHA384 | pure-C `UpdateSHA512_C` (`pcphashsha512px.c`) | AVX2 `sha512_hashUpdate` (`pcpsha512l9as.nasm`) |
| SM3 | AVX1 `UpdateSM3` (`pcpsm3e9as.nasm`) | AVX1 |

SM3-NI (`vsm3*`) is not available on current supported platforms (RPL/MTL/ARL/PTL);
`pcpsm3e9as.nasm` is used as an E9 fallback. When a future platform confirms SM3-NI:
add `ippCPUID_SM3` to `IPP_CUSTOM_ENABLED_FEATURES`, restore `pcpsm3l9_ni_as.nasm`,
and remove the `IPPCP_CUSTOM_BUILD` SM3 override in `sm3.c`.

---

## Stage2 TE Image 32-Byte Alignment Fix

`pcpsha512l9as.nasm` (`vmovdqa ymm`) requires the `.data` constant table `SHA512_YMM_K`
to be 32-byte aligned at runtime. The TE image inside the decompressed Stage2 FD lands at
a fixed 0x70-byte structural offset from the FD start. Whether the `.data` symbols end up
32-byte aligned depends on `StrippedSize` (derived from the PE header layout), which
changes whenever Stage2's PE section count changes. This makes alignment fragile across
normal code evolution.

**Fix:** `fixup_stage2_avx2_alignment()` (implemented in `BootloaderCorePkg/Tools/BuildUtility.py`,
invoked from `BuildLoader.py` post-build for L9 boards) reads the TE header, computes the
misalignment `(te_fd_offset - Adjust) mod 32`, and inserts 1–31 pad bytes at the TE image
boundary inside STAGE2.fd. It also:
- Updates TE `StrippedSize`, TE section header size, and FFS file header size.
- Recomputes the FFS `IntegrityCheck.Checksum.Header`.
- Walks the `.reloc` table to fix all absolute pointer fixups.
- Patches `PcdAcpiTablesAddress`, `PcdGraphicsVbtAddress`, and `PcdSplashLogoAddress`
  PCDs (BinaryPatch FFS at offset +0x1C) shifted by the inserted pad.

This adapts dynamically to any `StrippedSize` without modifying ipp-crypto or BoardConfig.

---

## FIPS Selftest GCC Limitation

The upstream RSA verify selftest sources (`selftest_rsa_sign_verify_pss.c` and
`selftest_rsa_sign_verify_pkcs_v15.c`) read a 3-byte public-exponent array via a
`Ipp32u*` cast, accessing a 4th byte that lies outside the array. With MSVC the
trailing byte is 0x00 (correct); with GCC it is non-zero (layout-dependent), causing
`ippsRSA_SetPublicKey` to return `ippStsSizeErr` and the selftest to fail.

The defect is in the upstream ipp-crypto source. Until it is fixed there,
`IppCrypto2LibFipsSelftestRsaVerifyPSS()` and `IppCrypto2LibFipsSelftestRsaVerifyPkcsv15()`
skip the test under GCC and return `RETURN_UNSUPPORTED` (a `DEBUG_WARN` is emitted).
MSVC and ICX builds run the selftests normally.

