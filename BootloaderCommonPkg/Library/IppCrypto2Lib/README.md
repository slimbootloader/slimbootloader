
# FIPS compliant IppCryptoLib
This version of IppCryptoLib utilizes a newer version of the ipp-crypto project that is certified to comply with the Federal Information Processing Standards. FIPS mode (`IPPCP_FIPS_MODE`) is supported for both the X64 architecture (variants M7/U8/Y8/E9/L9) and the IA32 architecture (variants W7/V8/P8/G9). The ipp-crypto FIPS selftest sources contain no x64-specific code paths and compile cleanly on IA32; the selftest buffer-allocation pattern used by SBL (`AllocateTemporaryMemory`) applies equally to both architectures.

## Git Submodule
The ipp-crypto project is now included unmodified as a git submodule. This means that git submodules will need to be initialized as part of clone/fetch of the Slim Bootloader code.


## x86 Extended Instruction Set  Optimizations
The ipp-crypto project includes enhanced implementations of many crypto primitives using enhancements to the x86 ISA that are not present in every CPU. The optimization levels are represented internally by a letter/number combination. There are many more combinations than these supported by ipp-crypto, but these are the ones which have an effect on the primatives used by Slim Bootloader. The below table shows the optimized instruction sets used for each optimization level (row headings) for each algorithm (column headings). You should expect some performance increase from each lower row in the table.

|        | **SHA256** | **SHA384** | **SM3**  | **RSA**   |
|--------|------------|------------|----------|-----------|
| **M7** | SSE3       | SSE3       | _None_   | SSE3      |
| **U8** | SSSE3      | SSSE3      | SSSE3    | SSE3      |
| **Y8** | SSE4.2     | SSSE3      | SSSE3    | SSE3      |
| **E9** | AVX        | AVX        | AVX      | SSE3      |
| **L9** | AVX2       | AVX2 (RAM) / pure-C (XIP) | SM3-NI   | AVX2/SSE3 |

> **L9 SHA384 note — runtime XIP dispatch (changed from v1.0.1):**
> `UpdateSHA512` in `pcpsha512l9as.nasm` loads round-constant tables with
> `vmovdqa ymm`, which requires the table address to be 32-byte aligned.
> The NASM source declares `segment .data align=32`, but the MSVC linker
> merges all `.data` fragments from every object at link time — the absolute
> address of `SHA512_YMM_K` in the final flash image depends on link order
> and is not guaranteed to stay 32-byte aligned.
>
> In v1.0.1 `sha384.c` hardcoded `sha512_hashUpdate` (→ `UpdateSHA512`) in
> the `IppsHashMethod` struct with no runtime dispatch. If the flash layout
> of a given platform happened to place the constants at a 32-byte boundary,
> no fault occurred. On platforms where it did not, a `#GP(13)` would fire.
>
> In v2.0.0, new `*_tt.c` dispatch files call `IsFeatureEnabled()` at
> runtime, which reads `cpFeaturesMask` — a global that is ROM-mapped in XIP
> and always reads as 0. This causes dispatch to always select the AVX2 path,
> making the alignment fault **systematic** rather than layout-dependent.
>
> The fix in `sha384.c` uses runtime dispatch based on the executing address:
>
> - **XIP (Stage1A)**: function address falls within `[PcdFlashBaseAddress,
>   PcdFlashBaseAddress + PcdFlashSize)` → returns `method_c`, which uses
>   `sha512_hashUpdate_c` — the portable-C fallback from `pcphashsha512px.c`
>   included locally with its `_IPP32E_E9` guard bypassed. No alignment
>   requirement; XIP-safe.
> - **RAM (Stage1B+)**: function address is above flash → returns `method_avx2`,
>   which uses `sha512_hashUpdate` from `pcpsha512l9as.nasm`. AVX2 is safe
>   once the code and `.data` tables are copied to properly-aligned RAM.
>
> Both `method_c` and `method_avx2` are `static const` (`.rdata`), safe to
> reference in XIP. `pcpsha512l9as.nasm` is retained in the build for the
> AVX2 RAM path; the pure-C `UpdateSHA512_C` symbol is private to the
> translation unit and does not conflict with the NASM export.

## ipp-crypto Project
The [Intel® Integrated Performance Primitives Cryptography Project](https://github.com/intel/cryptography-primitives) is a secure, fast and lightweight library of building blocks for cryptography, highly-optimized for various Intel® CPUs. ipp-crypto is validated for FIPS-140-3
You can find more information on the ipp-crypto github page.

### License
Intel® Cryptography Primitives Library is licensed under Apache License, Version 2.0.

---

## IA32 Fix: 64-bit Arithmetic Without MSVC Runtime Helpers

### Problem

On IA32 builds, the MSVC compiler emits calls to runtime helper functions when
performing 64-bit division or multiplication on 32-bit targets:

- `__aulldiv` — unsigned 64-bit divide
- `__allmul` — 64-bit multiply

These helpers are part of the MSVC C runtime library (`msvcrt.lib`). The SBL
Stage1A and Stage1B images are bare-metal EDK2 modules with no C runtime linked
in, so these symbols are not present. Executing a `CALL __aulldiv` instruction
with no handler mapped results in a **triple fault → immediate CPU reset** — seen
as a boot loop with the serial log stopping at:

```
calling cpMod_BNU pX=... pMod=...
```

The root cause is in three ipp-crypto source files that use native C 64-bit
operators on `Ipp64u` temporaries:

| Source file | Problematic expression |
|---|---|
| `pcpbnu32_arith_div.c` | `Ipp64u q = tmp / yHi;` |
| `pcpbnu32_arith_muldigit.c` | `Ipp64u t = (Ipp64u)val * (Ipp64u)pA[i] + carry;` |
| `pcpbnu32_arith_submuldigit.c` | `Ipp64u r = (Ipp64u)(*pA++) * val - carry;` |

The old `IppCryptoLib` avoided this with `#ifdef _SLIMBOOT_OPT` guards that
replaced those expressions with EDK2's `DivU64x32()` and `MultU64x32()` helpers,
which expand directly to the CPU's 32-bit `DIV`/`MUL` instructions.

### Symptom vs. Root Cause Confirmation

The struct size of `gsModEngine` increased by 4 bytes in `IppCrypto2Lib` due to
an added `const void* method_alt` field. This shifted the logged `MODULUS` and
`MNT_R` pointers by +4 bytes compared to the old library, which was the first
observable difference in the debug logs. However, the actual crash was the
`__aulldiv` triple fault during `cpMod_BNU` → `cpDiv_BNU32`.

### Fix

To keep the `ipp-crypto` submodule **completely unmodified**, the three affected
functions are re-implemented in **`IppCryptoSupportIA32.c`** — the IA32-only
build-support file that lives outside the submodule.

`IppCryptoSupportIA32.c`:
- Includes `owncp.h`, `pcpbnu32arith.h` etc. from ipp-crypto (via the existing
  include path) so that `OWNAPI`/`IPP_OWN_DEFN` macros apply the correct
  per-variant symbol prefix (`w7_`, `v8_`, `p8_`, `g9_`, etc.)
- Uses `DivU64x32()` and `MultU64x32()` from EDK2 `<Library/BaseLib.h>` instead
  of native C `/` and `*` operators
- Is listed directly in `[Sources]` of each IA32 variant `.inf` (not
  `[Sources.IA32]`) since those INFs are already IA32-only

The three original ipp-crypto arithmetic files are already removed from `[Sources]` in
each IA32-capable variant `.inf`:

| .inf | Scope |
|---|---|
| `IppCrypto2LibW7.inf` | IA32 W7 (SSE2) — default for RPLP |
| `IppCrypto2LibV8.inf` | IA32 V8 (SSSE3) |
| `IppCrypto2LibP8.inf` | IA32 P8 (SSE4.1) |
| `IppCrypto2LibG9.inf` | IA32 G9 (AVX) |
| `IppCrypto2LibIA32.inf` | Portable IA32 (no SIMD) |

The `ipp-crypto` submodule at `IppCrypto2Lib/ipp-crypto/` is left byte-for-byte
identical to upstream.

---

## Upgrade to ipp-crypto v2.0.0 — X64 L9 FIPS Selftest Fix

### What changed in v2.0.0

ipp-crypto v2.0.0 introduced new SHA dispatch files (`pcphashmethodset_sha256_tt.c`,
`pcphashmethodset_sha512_tt.c`, and several others) that call `IsFeatureEnabled()`
at runtime to select between AVX2 and SHA-NI code paths. These replace the static
compile-time dispatch used in v1.0.1.

New sources added to `IppCrypto2LibL9.inf` vs v1.0.1:

| File | Purpose |
|---|---|
| `hash/sha224/pcphashmethodset_sha224_tt.c` | SHA-224 runtime dispatch |
| `hash/sha384/pcphashmethodset_sha384_tt.c` | SHA-384 runtime dispatch |
| `hash/sha512/pcphashmethodset_sha512_tt.c` | SHA-512 runtime dispatch |
| `hash/sha512/pcphashmethodset_sha512_224_tt.c` | SHA-512/224 runtime dispatch |
| `hash/sha512/pcphashmethodset_sha512_256_tt.c` | SHA-512/256 runtime dispatch |
| `asm_intel64/pcpsha512l9ni.nasm` | SHA-512 NI assembly (L9) |

The MSFT NASM flags line was also changed from `=` to `==` (EDK2 override
instead of append) and gains explicit `-Ox -f win64` flags.

### Problem

On X64 L9 builds with `PcdFipsSupport=TRUE`, Stage1A crashes with a **#GP
(exception 13)** during the FIPS selftest for SHA-256 (and SHA-384).

The failure chain:
1. `RunFipsSelftests()` calls `ippsHashMethodSet_SHA256_TT()` (new in v2.0.0).
2. That function calls `IsFeatureEnabled(ippCPUID_SHA)`, which reads the static
   global `cpFeaturesMask` in `cpinit.c`.
3. Stage1A runs execute-in-place (XIP) from flash. The `.data` section containing
   `cpFeaturesMask` is ROM-mapped — writes to it are silently discarded.
4. `cpFeaturesMask` is always 0. `IsFeatureEnabled()` always returns 0.
5. Dispatch falls back to the L9 AVX2 path: `sha256_hashUpdate` → `UpdateSHA256`.
6. `UpdateSHA256` loads round constants with `vmovdqa ymm` (requires 32-byte
   alignment). The ROM-mapped constant table is not guaranteed 32-byte aligned.
7. **`#GP(13)` fault.**

Calling `ippcpInit()` before the selftests does not help — it also writes to
`cpFeaturesMask`, which is ROM-mapped and discards the write.

### Fix in `IppCrypto2LibL9.inf`

`cpinit.c` contains a compile-time bypass path:

```c
int cpGetFeature(Ipp64u Feature) {
#if (!defined(_MERGED_BLD) && defined(IPPCP_CUSTOM_BUILD))
    return ((IPP_CUSTOM_ENABLED_FEATURES & Feature) == Feature);
#else
    // reads cpFeaturesMask global ...
```

When `IPPCP_CUSTOM_BUILD` is defined, `cpGetFeature()` (and therefore
`IsFeatureEnabled()`) becomes a pure compile-time constant expression — no globals,
no ROM writes. `IPP_CUSTOM_ENABLED_FEATURES=0x80000` sets `ippCPUID_SHA`, so
`IsFeatureEnabled(ippCPUID_SHA)` always returns 1 and `sha256_ni_hashUpdate` is
always selected. SHA-NI instructions have no alignment requirement.

No changes to the `ipp-crypto/` submodule are required.

### Platform compatibility for `IppCrypto2LibL9.inf`

`IPPCP_CUSTOM_BUILD` with `IPP_CUSTOM_ENABLED_FEATURES=ippCPUID_SHA` forces SHA-NI
instructions at compile time. This is safe **only on platforms that have SHA-NI**.

| Platform (codename) | microarch | L9 (AVX2)? | SHA-NI? | Use `IppCrypto2LibL9.inf`? |
|---|---|---|---|---|
| Haswell | HSW | Yes | **No** | **No — use E9** |
| Broadwell | BDW | Yes | **No** | **No — use E9** |
| Skylake / Kaby Lake / Coffee Lake | SKL/KBL/CFL | Yes | **No** | **No — use E9** |
| Cannon Lake | CNL | Yes | Yes | Yes |
| Ice Lake (client) | ICL | Yes | Yes | Yes |
| Tiger Lake | TGL | Yes | Yes | Yes |
| Alder Lake | ADL | Yes | Yes | Yes |
| Raptor Lake / Raptor Lake-P | RPL/RPLP | Yes | Yes | Yes |
| Meteor Lake | MTL | Yes | Yes | Yes |
| Arrow Lake / Panther Lake | ARL/PTL | Yes | Yes | Yes |
| Goldmont / Goldmont Plus / Tremont (Atom) | GLM/GLP/TNT | No (SSE4.2) | Yes | No — use Y8/E9 |

Platforms marked **No — use E9** must use `IppCrypto2LibE9.inf` (AVX1,
`vmovdqu`, unaligned-safe). On those platforms, `IppCrypto2LibL9.inf` will
cause an **`#UD` (invalid opcode) fault** when `SHA256RNDS2` or `SHA256MSG*`
instructions execute.

---

## X64 L9 XIP Hash Fixes

Raptor Lake-P Stage1A executes in place from flash. Some L9 hash assembly paths
in ipp-crypto either assume aligned ROM constants or use ISA extensions that are
not available on the target CPU. The following fixes are applied in the SBL
wrapper layer without modifying the `ipp-crypto` submodule.

### SHA256

`sha256.c` uses the SHA-NI update path for `IPPCP_CUSTOM_BUILD` on L9:

- `sha256_ni_hashUpdate` is selected instead of `sha256_hashUpdate`
- This avoids the AVX2 `UpdateSHA256` path that faults on XIP constant alignment

This fix requires SHA-NI support on the platform, which is present on RPL/RPLP.

### SHA384 / SHA512 Update

There is no usable lower-tier NASM fallback for `UpdateSHA512` in an L9 build
for the XIP context:

- `pcpsha512l9as.nasm` faults in XIP due to `vmovdqa ymm` on unaligned ROM-mapped
  `.data` constants (`SHA512_YMM_K`), but is safe once loaded into RAM
- `pcpsha512m7as.nasm` is guarded out when `_IPP32E = _IPP32E_L9`
- `pcpsha512e9as.nasm` is also guarded out for L9

`sha384.c` implements runtime dispatch to handle both cases without modifying
the `ipp-crypto` submodule:

- A second, renamed copy of `UpdateSHA512` (`UpdateSHA512_C`) is compiled by
  including `pcphashsha512px.c` with its `_IPP32E_E9` guard bypassed. This
  is the pure-C, XIP-safe path.
- Two `IppsHashMethod` tables (`method_c` and `method_avx2`) are held as
  `static const`.
- At call time, `ippsHashMethod_SHA384Sbl()` checks whether its own code
  address falls in the PCD-defined flash region. If yes (XIP), it returns
  `method_c`; otherwise (RAM), it returns `method_avx2`.

`pcpsha512l9as.nasm` is **retained** in `[Sources.X64]` to provide
`sha512_hashUpdate` for the RAM path.

### SM3

The L9 NI path uses SM3-specific instructions that are not available on RPL/RPLP:

- `sm3_hashUpdate_ni` calls `UpdateSM3ni` from `pcpsm3l9_ni_as.nasm`
- This executes `vsm3*` instructions and faults with `#UD`

The fix is:

- `sm3.c` dispatches to `sm3_hashUpdate` for `IPPCP_CUSTOM_BUILD`
- `IppCrypto2LibL9.inf` includes `pcpsm3e9as.nasm`, which provides `UpdateSM3`

`pcpsm3e9as.nasm` assembles for L9, does not use SM3-NI instructions, and is
safe for the RPLP XIP environment.

### Final L9 XIP behavior

With these changes, the effective hash-update backends in `IppCrypto2LibL9.inf`
on any XIP-capable L9 platform are:

| Algorithm | Backend used in SBL | Reason |
|---|---|---|
| SHA256 | SHA-NI (`sha256_ni_hashUpdate`) | avoids AVX2 aligned ROM loads |
| SHA384 (XIP / Stage1A) | pure-C `UpdateSHA512_C` from `pcphashsha512px.c` | `vmovdqa ymm` faults on unaligned ROM-mapped constants |
| SHA384 (RAM / Stage1B+) | AVX2 `sha512_hashUpdate` from `pcpsha512l9as.nasm` | safe after copy to aligned RAM |
| SM3 | AVX1 `UpdateSM3` from `pcpsm3e9as.nasm` | avoids unsupported SM3-NI opcodes |

---

## Platform Capability Matrix and Porting to Future Platforms

The SBL wrapper choices in `IppCrypto2LibL9.inf` — `IPPCP_CUSTOM_BUILD`,
`IPP_CUSTOM_ENABLED_FEATURES`, the pure-C SHA384 fallback, and the SM3 E9
fallback — are driven by which ISA extensions the target platform supports in
the XIP execution context. The table below summarises the relevant capability
bits and the corresponding build choices for current and anticipated platforms.

| Platform family | SM3-NI | SHA-NI | AVX2 | SHA384 backend | SM3 backend |
|---|---|---|---|---|---|
| Raptor Lake (RPL/RPLP) | No | Yes | Yes | pure-C (XIP) / AVX2 (RAM) | E9 AVX1 (`pcpsm3e9as`) |
| Meteor Lake (MTL) | No | Yes | Yes | pure-C (XIP) / AVX2 (RAM) | E9 AVX1 (`pcpsm3e9as`) |
| Arrow Lake (ARL-H/ARL-S) | No | Yes | Yes | pure-C (XIP) / AVX2 (RAM) | E9 AVX1 (`pcpsm3e9as`) |
| Panther Lake (PTL) | TBD | Yes | Yes | pure-C (XIP) / AVX2 (RAM) | E9 AVX1 (until SM3-NI confirmed) |
| Nova Lake (NVL) | TBD | Yes | Yes | pure-C (XIP) / AVX2 (RAM) | E9 AVX1 (until SM3-NI confirmed) |

### Enabling SM3-NI on future platforms

If a future platform (e.g. PTL or NVL) confirms SM3-NI support:

1. Add `ippCPUID_SM3` to `IPP_CUSTOM_ENABLED_FEATURES` in `IppCrypto2LibL9.inf`
   (combine with `ippCPUID_SHA`: `0x80000 | <SM3_bit>`).
2. Remove `pcpsm3e9as.nasm` from `[Sources.X64]` and restore
   `pcpsm3l9_ni_as.nasm`.
3. Remove the `IPPCP_CUSTOM_BUILD` SM3 dispatch override from `sm3.c`
   (the `#ifdef IPPCP_CUSTOM_BUILD` block that forces `sm3_hashUpdate`).
4. Verify that `pcpsm3l9_ni_as.nasm` does not use `vmovdqa` on unaligned
   `.data` constants — the XIP alignment constraint still applies.

### SHA384 AVX2 RAM path

AVX2 SHA384 is **already active** for Stage1B and later (post-memory) execution.
`pcpsha512l9as.nasm` is included in the build and `sha384.c` returns
`method_avx2` whenever `ippsHashMethod_SHA384Sbl` executes outside the flash
region. No further changes are needed for current platforms.

If a future platform also needs AVX2 (or SHA-NI) SHA384 **in XIP**, the
conditions are:

- **AVX2 in XIP**: only safe if the linker guarantees `SHA512_YMM_K` is
  32-byte aligned in the flash image. Verify with the platform linker map;
  if confirmed, remove the `FnAddr` XIP check in `sha384.c` and always
  return `method_avx2`.
- **SHA-NI SHA512**: `pcpsha512l9ni.nasm` is already in `[Sources.X64]`.
  It provides `UpdateSHA512ni` (no alignment requirement). Add a third
  `method_ni` table in `sha384.c` pointing to `sha512_ni_hashUpdate` and
  select it when `IsFeatureEnabled(ippCPUID_SHA)` returns true at runtime.