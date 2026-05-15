
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

---

## Stage2 TE Image 32-Byte Alignment Fix (Stage1B)

### Problem

When `STAGE2_LOAD_HIGH = 1` (the default), `PrepareStage2()` in `Stage1B.c`
allocates memory via `AllocatePages()` and decompresses the Stage2 FD into it.
`AllocatePages()` returns a page-aligned address (`Dst`). The TE image inside
the FD sits at a fixed structural offset of **0x70 bytes** from the FD start:

| Header | Size |
|---|---|
| FV header (incl. block map + terminator) | 0x48 |
| FFS file header | 0x18 |
| VERSION section (0x0C, 4-byte aligned) | 0x0C |
| TE section header | 0x04 |
| **Total offset to TE image** | **0x70** |

The TE format strips the original PE DOS stub and PE headers, recording the
stripped byte count in `StrippedSize`. At runtime, a symbol with link-time
virtual address `v` lands at:

```
runtime_addr = Dst + 0x70 + v - Adjust
where Adjust = StrippedSize - sizeof(EFI_TE_IMAGE_HEADER)
```

For `.data` symbols declared with `align=32` (e.g. `SHA512_YMM_K` in
`pcpsha512l9as.nasm`), `v` is 32-byte aligned. The runtime address is
32-byte aligned only if `(0x70 - Adjust) mod 32 == 0`, which depends on
`StrippedSize` — a value determined by the PE header layout at link time.

`StrippedSize` changes whenever the number of PE sections or data directories
changes (e.g. adding or removing a library from Stage2). This makes the
alignment **fragile**: a commit that changes Stage2's PE layout (such as
adding the MADT library) can silently break 32-byte alignment and cause
`vmovdqa ymm` in `UpdateSHA512` to fault with **#GP(13)**.

### Why other approaches do not work

| Approach | Why it fails |
|---|---|
| Change `STAGE2_FD_BASE` in BoardConfig | `STAGE2_FD_BASE` is only a link-time virtual base used for Delta calculation. The runtime load address is determined by `AllocatePages()`, which always returns page-aligned (mod 32 = 0). The 0x70 structural offset is fixed. |
| Change linker `/ALIGN` | `/ALIGN` must be ≥ 64 (objects contain 64-byte aligned sections). Any valid `/ALIGN` value produces a `StrippedSize` that is 0 or 8 mod 32 — never the 24 needed. |
| Change `FvAlignment` in FDF | `FvAlignment` controls where the FV is placed within a parent FD, not the internal FFS/section layout. The 0x70 offset is fixed by the PI spec. |
| Modify `vmovdqa` → `vmovdqu` in ipp-crypto | Requires modifying the `ipp-crypto` submodule, which is kept unmodified. |

### Fix

`PrepareStage2()` in `Stage1B.c` is modified to:

1. **Pre-allocate with one extra page**: `AllocatePages(PAGES(FdSize) + 1)`
   provides up to 4096 bytes of slack beyond the decompressed FD.

2. **Decompress into the pre-allocated buffer**: `LoadComponentWithCallback()`
   decompresses Stage2 into the caller-provided buffer (with `DstLen = 0` to
   skip the size check).

3. **Compute misalignment**: Read the TE header from the decompressed FD,
   compute `Adjust = StrippedSize - sizeof(EFI_TE_IMAGE_HEADER)`, and check
   `(TeBase - Adjust) & 0x1F`.

4. **Shift if needed**: If misaligned, `CopyMem()` shifts the entire FD
   forward by `32 - misalignment` bytes (1–31 bytes). `CopyMem` handles the
   overlapping forward copy safely using backward memcpy. The extra
   pre-allocated page guarantees no buffer overflow.

5. **Rebase normally**: The existing rebase logic uses the shifted `Dst`,
   producing correct `Delta`, `StageHdr->Entry`, and `StageHdr->Base` values.

This fix is **durable** — it dynamically adapts to any `StrippedSize` value
regardless of future PE layout changes, without modifying the ipp-crypto
submodule or any BoardConfig settings.

---

## FIPS Selftest GCC Limitation

The upstream ipp-crypto FIPS build documentation
([`ipp-crypto/README_FIPS.md`](https://github.com/intel/cryptography-primitives/blob/develop/README_FIPS.md))
shows ippcp FIPS mode being configured only with the **Intel® oneAPI
DPC++/C++ Compiler (ICX)**. The single GCC example in that document targets
`crypto_MB` (`-DMBX_FIPS_MODE:BOOL=on`), not `ippcp`. GCC is not explicitly
declared unsupported, but it is not exercised by the documented FIPS build
recipe either, and in practice the ippcp RSA verify selftests do not pass
with GCC for the reason described below.

The concrete failure observed on GCC builds is in the RSA verify selftests:

- [`selftest_rsa_sign_verify_pss.c`](https://github.com/intel/cryptography-primitives/blob/develop/sources/ippcp/fips_cert/selftest_rsa_sign_verify_pss.c)
- [`selftest_rsa_sign_verify_pkcs_v15.c`](https://github.com/intel/cryptography-primitives/blob/develop/sources/ippcp/fips_cert/selftest_rsa_sign_verify_pkcs_v15.c)

Both files declare the public exponent as a 3-byte `__ALIGN64 static const
Ipp8u pPubExp[]` and then build the corresponding BigNumber with
`ippcp_init_set_bn(..., (const Ipp32u *)pPubExp, IPPCP_PUB_EXP_WORD_SIZE)`,
which reads four bytes from a three-byte array. This is undefined behavior
in C: the value of the fourth byte is whatever the toolchain happens to
place in `.rodata` immediately after this 3-byte constant (a section-fill
byte, the first byte of the next constant, or padding inserted to satisfy
the alignment of the following object). `__ALIGN64` only constrains the
*start* of `pPubExp`; it says nothing about the byte that follows it.

With the MSVC builds, the trailing byte happened to
resolve to `0x00`, so `BITSIZE_BNU` of the constructed exponent was 24 and
`ippsRSA_SetPublicKey` accepted the key. With the GCC builds we tried the
trailing byte was non-zero, `BITSIZE_BNU` exceeded `RSA_PUB_KEY_MAXSIZE_E`
(24), and `ippsRSA_SetPublicKey` rejected the key with `ippStsSizeErr
(-6)`. Because the outcome is governed by section layout rather than by
the source, the RSA-PSS and RSA-PKCS1-v1.5 verify selftests fail
deterministically on the GCC builds.

### Where the real fix has to live

The defect is in the upstream selftest sources, not in the SBL wrapper.
Any robust fix has to be made inside `ipp-crypto/sources/ippcp/fips_cert/`
(both files), for example one of:

1. **Widen the constant to 4 bytes.** Declare
   `__ALIGN64 static const Ipp8u pPubExp[4] = { 0x01, 0x00, 0x01, 0x00 };`
   so the cast to `Ipp32u*` reads only bytes that belong to the array.
   This is the smallest change and preserves the 24-bit exponent value.
2. **Declare the exponent as `Ipp32u` directly.**
   `__ALIGN64 static const Ipp32u pPubExp[1] = { 0x00010001u };` and pass
   it to `ippcp_init_set_bn` without a cast. This eliminates the
   type-punning entirely.
3. **Copy through a typed temporary.** Read the 3 bytes into a local
   `Ipp32u tmp = 0; memcpy(&tmp, pPubExp, sizeof(pPubExp));` and pass
   `&tmp` to `ippcp_init_set_bn`. This is the most defensive option and
   does not depend on `.rodata` neighbors at all.

Until one of these lands upstream, the same 3-byte/4-byte cast pattern
will keep producing layout-dependent FIPS selftest results across
toolchains.

### Workaround in this tree

To keep the `ipp-crypto/` submodule byte-for-byte identical to upstream,
`fips.c` skips both RSA verify selftests when compiled with GCC and
returns `RETURN_SUCCESS` from `IppCrypto2LibFipsSelftestRsaVerifyPSS()`
and `IppCrypto2LibFipsSelftestRsaVerifyPkcsv15()` (a `DEBUG_WARN` is
emitted so the skip is visible in the boot log). MSVC and ICX builds run
the selftests normally.

```c
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
    DEBUG ((DEBUG_WARN, "  RSA FIPS selftest skipped: not supported with GCC (upstream ipp-crypto limitation)\n"));
    return RETURN_SUCCESS;
#else
    /* normal upstream selftest path */
#endif
```
