# AddressSanitizer (ASAN) support in Slim Bootloader

ASAN finds memory errors at run time in Stage2 and in the OsLoader and FirmwareUpdate payloads. When it detects a bad access, it prints a report on the debug serial port and halts the system.

## Quick start

```
python BuildLoader.py build qemu -t clang --asan        # optimized (DEBUG) build
python BuildLoader.py build qemu -t clang --asan -no    # no optimization, clearest reports
```

- `--asan` requires the Clang toolchain (`-t clang`, `TOOL_CHAIN=CLANGPDB`). `BuildLoader.py` stops with an error for any other toolchain.
- Use a DEBUG or NOOPT build. The report is printed with `DEBUG ()`, so a release build (`-r`) may halt without printing anything.
- Builds without `--asan` don't change. They link `AsanLibNull`, and no code is instrumented.

## What is detected

| Error | Where | Report name |
|---|---|---|
| Heap overflow and underflow | Payload heap (`AllocatePool`) | `heap-buffer-overflow` |
| Overflow past the end of a Stage2 pool buffer | Stage2 pool (`AllocatePool`) | `heap-buffer-overflow` |
| Use after free | Payload heap; freed blocks are held back (quarantine) so they aren't reused right away | `heap-use-after-free` |
| Double free | Payload heap | `double-free` |
| Overflow of local arrays and variables | Stage2 and payload stacks | `stack-buffer-overflow` |
| Use of a variable outside its block | Stage2 and payload stacks | `stack-use-after-scope` |
| Overflow of global variables | Stage2, OsLoader and FirmwareUpdate globals | `global-buffer-overflow` |

**Not detected:**
- **Memory leaks.** There is no leak checker.
- **Use after return.** This is turned off (`-fsanitize-address-use-after-return=never`).
- **`alloca` buffers.** These aren't covered (`-asan-stack-dynamic-alloca=0`).
- **Accesses outside the tracked ranges.** Accesses outside the Stage2 pool, the Stage2 and payload stacks, the payload heap and the instrumented global ranges are ignored.
- **Stage2 pool underflow and use after free.** The Stage2 pool only grows down and never frees, so only the area just above each buffer is guarded, and buffers allocated before ASAN starts in Stage2 have no guard at all.
- **Code that isn't instrumented**, except its calls to the checked memory APIs (see Scope).
- **Accesses that skip over a guard area** (red zone) and land in another valid object.

## Scope

- **Instrumented modules:** Stage2, OsLoader and FirmwareUpdate. Each links the real `AsanLib`, and its own source files are instrumented.
  - **Stage2** hosts the runtime. Early in `SecStartup`, `InitializeAsan ()` (`Stage2Support.c`) tracks the Stage2 pool, the Stage2 stack and HOB range, and the Stage2 globals, and publishes the runtime with `RegisterService ()` as `ASAN_SERVICE`. Stage2 code that runs before `InitializeAsan ()` is not checked.
  - **Payloads** find that service in the service-list HOB at the start of `PayloadInit` and call `AsanUseService ()`. Their heap, stack and globals are then tracked by the Stage2 runtime, so one table covers both stages and a payload's bad access to Stage2-owned memory is also caught. If Stage2 doesn't publish the service, the payload uses its own copy of the runtime as before.
- **Instrumented libraries:** the libraries listed below, except those deliberately left out. Some also link into Stage1A/1B.
  - **Generic libraries,** listed in `BaseBoard._ASAN_LIBS` in `BuildLoader.py`:
    - Payloads: shell, shell extension, setup UI, console input, USB, block I/O, file system, partition, boot option, media access, MMC, NVMe, AHCI, UFS, SPI block I/O, RPMB, seed list, A/B slot and SBL parameter libraries.
    - Payloads and Stage2: ELF, Linux boot, universal payload and IOMMU libraries.
    - Stage2: the Stage2 SoC and board init libraries (`$(SOC_INIT_STAGE2_LIB_INF_FILE)`, `$(BRD_INIT_STAGE2_LIB_INF_FILE)`), ACPI init, SMBIOS init, S3 save/restore, FSP-S API, PCI enumeration, FIT, FDT build and `BaseFdtLib`.
    - FirmwareUpdate: `$(SOC_FWU_LIB_INF_FILE)`.
  - **Board and silicon libraries:** each `BoardConfig*.py` adds the eligible library instances returned by `GetPlatformDsc()` to `self._ASAN_LIBS`. Library-class aliases are removed, board DSC macros are resolved to their concrete INF paths, and repeated calls do not add duplicates. The runtime, allocator, startup/debug, graphics, crypto and unsafe low-level exclusions below are not added. `BuildLoader.py` writes every library in `_ASAN_LIBS` (plus `$(SOC_FWU_LIB_INF_FILE)` when `ENABLE_FWU` is set), each with `ASAN_CC_FLAGS`, to the generated `BootloaderCorePkg/AsanComponents.dsc`, which the DSC includes. Check each board's `static_library_files.lst` under `Build` when changing the list.
  - **Linking anywhere:** EDK2 builds each library once and links that copy into every module that uses it, so instrumented code can also end up in modules without a runtime, such as PcdData or Stage1A/1B. Every module of an ASAN build links `AsanStubLib`, which provides the `__asan_*` callbacks. In a module whose runtime is not connected they only do the copy or fill they wrap, so instrumented code runs unchecked.
  - **Code on other CPUs:** the Stage2 SoC and board init libraries may run hooks on the APs. AP stacks come from the Stage2 pool, so their frames are tracked as part of the pool. Reporting is not multiprocessor-safe: a bad access caught on an AP (for example in `PlatformCpuInit`) will most likely just hang the system with no report or source location, because the report runs on the AP's small stack, its debug output is unsynchronized with the BSP, and the AP halts in `CpuDeadLoop ()` while the BSP keeps running. If boot stops during or soon after MP init with nothing in the log, suspect an ASAN error in AP code.
- **Libraries deliberately left uninstrumented:**
  - **Stage1 runtime/bootstrap/debug exclusions:** Stage1A/1B have no connected ASAN runtime, so their accesses are never checked even when an instrumented library is linked there. The shared container, PE/COFF, FV, decompression (generic, LZMA, LZ4), config data, secure boot, TPM, HOB, HOB build, CRC32, sort and variable libraries are instrumented by `--asan`; this also enables checking their code when called in Stage2 or payloads. Instrumentation makes Stage1 code and stack frames larger and can slow operations such as decompression; boards must reserve adequate Stage1 image and stack space.
  - **Deliberately excluded Stage1 libraries:** the crypto library (size and speed), graphics and console output libraries (debug output path), and stack switch, paging, exception, FSP call and core libraries (`StageLib`, `ModuleEntryLib`, `PagingLib`, `CpuExceptionLib`, `FspSupportLib`, `FsptApiLib`, `FspmApiLib`, `BootloaderCoreLib`, `BootloaderLib`, `PcdLib`).
  - **Libraries AsanLib and AsanStubLib call:** BaseLib, BaseMemoryLib, PrintLib, DebugLib, SerialPortLib and DebugLogBufferLib.
  - **`FullMemoryAllocationLib` and the core `MemoryAllocationLib`:** they read and write the heap and pool guard areas.
  - **`PayloadEntryLib`:** it runs before tracking starts and switches stacks.
  - **`PayloadLib`, `PlatformHookLib`, `PayloadSupportLib`:** they are on the payload startup and debug-output path.
  - **`MpInitLib`, `MpServiceLib`, `MtrrLib`, `BaseSynchronizationLib`:** they bring up or run on the other CPUs.
- **Direct memory APIs:** in ASAN builds made through `BootloaderCorePkg.dsc`,
  `CopyMem` checks the entire source for reads and destination for writes before
  copying; `SetMem` checks the entire destination for writes before filling.
  This applies to callers in both instrumented and uninstrumented code once this
  module's stub is connected. Compiler instrumentation alone did **not** check
  direct calls to these uninstrumented BaseMemoryLib APIs; `__asan_mem*` callbacks
  cover compiler intrinsics, not arbitrary calls named `CopyMem` or `SetMem`.
  Zero-length calls access nothing and issue no checks. `CopyMem` still supports
  overlap and returns the destination; nonzero self-copies are checked too.
  With no connected runtime (including Stage1A/1B), or after disconnection, the
  original memory operation runs unchecked. Non-ASAN builds keep the original
  BaseMemoryLib symbols and do not link these checked entry points.
  `ZeroMem`, `SetMem16/32/64/N`, `CompareMem`, scans, GUID helpers and
  `IsZeroBuffer` are **not** newly checked by this change; they have separate
  implementations. Arbitrary loads/stores in uninstrumented code remain unchecked.
- **Stages:** Stage1A and Stage1B have no connected runtime. Stage1A runs from flash before memory is available, so it has no RAM for shadow memory, and both have tight size limits. Instrumentation in shared libraries therefore runs unchecked in Stage1 but still incurs code, stack and performance costs.

## Shell test command

In ASAN builds the OsLoader shell has an `asan` command that makes a deliberate bad access. Each test halts the system, so reboot between tests.

```
Shell> asan overflow     # write 1 byte past a 13-byte pool buffer
Shell> asan underflow    # read 1 byte before a pool buffer
Shell> asan uaf          # read a pool buffer after FreePool
Shell> asan dfree        # free a pool buffer twice
Shell> asan stack        # write 1 byte past a 13-byte local array
Shell> asan global       # write 1 byte past a 13-byte global array
```

If ASAN misses the access, the command prints `ASAN did not detect the violation!`.

## Reading a report

Example (`asan stack`):

```
==ASAN== ERROR: stack-buffer-overflow on address 0x95FF95D
==ASAN== WRITE of size 1 at 0x95FF95D
==ASAN== image Stage2 at 0x678FE000
==ASAN== image OsLoader at 0x67709000
==ASAN==   #0 0xBC4E868 (OsLoader+0x67868)
==ASAN==   #1 0xBC454AE (OsLoader+0x5E4AE)
...
==ASAN== 0x95FF95D is at offset 29 in the stack frame of:
==ASAN==   0xBC4E710 (OsLoader+0x67710)
==ASAN== This frame has 1 object(s):
==ASAN==   [16, 29) 'Local:91' <== access overflows this variable
==ASAN== Shadow bytes around the buggy address (one byte per 8 bytes of memory):
...
```

The report has these parts:
- **Header:** the error type, read or write, the access size and address, and the base address of each image the runtime knows.
- **Backtrace:** found by following frame pointers on the current stack. Each frame shows its offset in the image that contains it (`Stage2+0x...`, `OsLoader+0x...`). The backtrace can stop early when a call passes through library code.
- **What was hit:**
  - **Stack:** the function, and every local variable in its frame with its offset range. Names include the declaration line, for example `Local:91`.
  - **Global:** the global's name, size and address, and the file it is defined in.
  - **Heap:** how far the address is from the nearest valid range, or where it falls inside the freed block.
- **Shadow dump:** the shadow bytes around the bad address, with the bad byte in `[ ]`, and a legend.

### Getting file and line

The firmware image doesn't contain line tables, so convert the `Name+0x...` offsets on the host with `llvm-symbolizer`:

```
python BootloaderCorePkg\Tools\AsanSymbolize.py serial.log
python BootloaderCorePkg\Tools\AsanSymbolize.py serial.log --image OsLoader=Build\...\OsLoader.dll --image Stage2=Build\...\Stage2.dll
```

For each image name in the log the script uses the newest `Name.dll` under `Build` unless `--image Name=PATH` is given, and prints which file it uses. Always use the images from the same build that produced the log, or the line numbers will be wrong. Older logs that print `image+0x...` are looked up in `OsLoader.dll`. Output:

```
==ASAN==   #0 0xBC4E868 (OsLoader+0x67868) in ShellCommandAsanFunc ...\PayloadPkg\OsLoader\AsanTest.c:102:0
==ASAN==   #1 0xBC454AE (OsLoader+0x5E4AE) in RunShellCommand ...\ShellLib\Shell.c:422:0
```

## How it works

### Compiler flags

The flags are set in the Stage2, OsLoader and FirmwareUpdate components of `BootloaderCorePkg.dsc` and, for the instrumented libraries, by `ASAN_CC_FLAGS` in `BuildLoader.py`, only when `ENABLE_ASAN` is set:

| Flag | Purpose |
|---|---|
| `-fsanitize=address` | Turns on instrumentation. |
| `-asan-instrumentation-with-call-threshold=0` | Every check becomes a call to `__asan_load*`/`__asan_store*`. The compiler never reads shadow memory itself. |
| `-asan-max-inline-poisoning-size=0` | Stack guard areas are set through `__asan_set_shadow_*` calls. The compiler never writes shadow memory itself. |
| `-asan-force-dynamic-shadow=1` | Uses a run-time shadow base (`__asan_shadow_memory_dynamic_address`) on IA32 as well as X64. The runtime keeps it at 0, so the callbacks receive `Address >> 3`. |
| `-asan-stack=1`, `-asan-stack-dynamic-alloca=0` | Guard areas around local variables (not `alloca`). |
| `-asan-globals=1`, `-asan-constructor-kind=none` | Guard areas around globals. There is no module constructor; the runtime finds the global metadata itself. |
| `-fsanitize-address-use-after-return=never` | No fake stack. |
| `-fno-sanitize-address-use-odr-indicator` | No ODR indicator symbols. |
| `-fno-omit-frame-pointer` | Keeps frame pointers for the report's backtrace. |
| `-fno-lto` | Works around an LLVM LTO crash on ASAN's COFF global metadata (`Associative COMDAT symbol '.str' is not a key for its COMDAT`). Libraries still use LTO. |
| `/MERGE:.ASAN=.data` (link) | Places the global metadata in `.data`. |

Because every check and every shadow write goes through a runtime call, the runtime is free to map shadow memory however it likes. Accesses to addresses it doesn't track are ignored.

### Callbacks (`AsanStubLib`)

`BootloaderCommonPkg/Library/AsanStubLib` is a `NULL` library linked into every module when `ENABLE_ASAN` is set. It defines `__asan_shadow_memory_dynamic_address` and every `__asan_*` callback the compiler emits. Each callback forwards to the `ASAN_SERVICE` (`Include/Service/AsanService.h`) connected with `AsanStubConnect ()`, passing the address in the instrumented code as the report's program counter. Until a runtime is connected, the callbacks do nothing except the copy or fill in `__asan_memcpy`, `__asan_memmove` and `__asan_memset`.

The stub also owns the checked `CopyMem` and `SetMem` symbols. ASAN-only
component build options rename the original wrappers in **both**
`BaseMemoryLibSse2` (default, including payloads) and `BaseMemoryLibRepStr`
(Stage1A/1B) to `AsanInternalCopyMem`/`AsanInternalSetMem`. Their existing
assertions and assembly primitives remain intact. Public APIs and compiler
intrinsics share the same check-and-execute helpers, without duplicate checks
or losing the caller's program counter.

Neither the stub, BaseMemoryLib, runtime nor its debug/print dependencies is
compiler-instrumented. Runtime shadow initialization, poisoning, unpoisoning,
compiler shadow writes and no-return cleanup explicitly use the raw fill
backend: shadow buffers may reside in tracked, poisoned memory themselves.
Runtime metadata inspection and report-time reads of poisoned stack headers
remain ordinary uninstrumented reads. Report formatting and log-buffer copies
use valid buffers; they do not need a blanket check-disabling guard.
There is no new recursion guard suppressing checks on unrelated accesses.

The standalone `PayloadPkg.dsc` and `IntelFsp2Pkg.dsc` do not enable this ASAN
build path and keep their original memory instances. A new ASAN memory-library
selection must provide the same raw wrapper renames; otherwise it will conflict
with the stub's public symbols at link time rather than silently bypass checking.

### Runtime (`AsanLib`)

`BootloaderCommonPkg/Library/AsanLib` is a small runtime written for SBL. It doesn't use compiler-rt. On first use it selects a runtime (the Stage2 service if `AsanUseService ()` accepted one, otherwise its own), names its image there with `gEfiCallerBaseName` and `__ImageBase`, and connects the module's `AsanStubLib` to it.

- **Shared runtime:** the Stage2 runtime code and data stay in loader-reserved memory and the payloads keep the Stage2 IDT, so the payloads can call it. Its reports are printed with the Stage2 `DEBUG` path: they go to the serial port and the Stage2 log buffer, not to the payload console.
- **Version check:** `AsanUseService ()` only accepts a service with the same signature and `ASAN_SERVICE_VERSION`. OsLoader and FirmwareUpdate are always built with the stages, so the pointer size matches.
- **Service slot:** ASAN builds raise `PcdServiceNumber` to 5 so the payload copy of the service list has room for it.

- **Tracked ranges:** it keeps a table of up to 8 address ranges, each with its own shadow buffer: the Stage2 pool, stack and globals, and each payload's heap, stack and globals. Each shadow byte describes 8 bytes of memory. A globals range keeps a pointer to the metadata of the image it belongs to.
- **Stage2 pool:** the shadow for the whole pool, the stack and the globals is taken from the pool itself, but only if it fits in half of the free space; otherwise that range isn't tracked and a warning is printed. The pool starts addressable. From then on the core `AllocatePool` (`BootloaderCorePkg/Library/MemoryAllocationLib`) adds a 32-byte guard above each buffer, because the pool grows down and the previous buffer is above it.
- **Payload heap:** `FullMemoryAllocationLib` takes the heap's shadow from the top of the heap (`AsanInitialize`). The pool allocator marks red zones around each allocation and marks freed blocks as freed. Freed blocks of 64 KB or less go into a 64-entry quarantine so they aren't reused right away.
  `AsanCheckFree ()` checks freed heap shadow before the allocator reads or validates
  the pool header, including the signature check in `CR`. This catches a second
  free after quarantine eviction has overwritten the header with free-list metadata,
  until the block is reused. Only after header and tail validation does
  `AsanQuarantine ()` insert a block; it retains its own double-free check.
  The shadow-only check reports an access size of one because the block size cannot
  yet be trusted. It returns success without checking when the runtime is not
  connected or ASAN is disabled. Service version 2 adds the `CheckFree` callback;
  version 1 services are rejected by the existing exact-version check.
- **Payload stack and globals:** `PayloadEntryLib` allocates shadow for them from the heap and registers them (`AsanTrackStack`, `AsanTrackGlobals`) before it switches to the payload stack.
- **Global metadata:** Clang writes one metadata entry per instrumented global into the COFF section `.ASAN$GL`. AsanLib defines marker entries in `.ASAN$GA` and `.ASAN$GZ`. The linker sorts sections by the text after `$`, so the markers end up before and after the metadata.
- **No-return calls:** `__asan_handle_no_return` clears the stack's shadow before calls that never return, because the frames they skip never clean up after themselves.
- **Linking:** `/MERGE:.ASAN=.data` is a global link flag in ASAN builds, because any module may contain instrumented globals.

`AsanLibNull` provides empty versions of the same functions for normal builds.

### Native pool-free regression test

From the SBL source root, run:

```
python BootloaderCommonPkg/Library/AsanLib/Tests/RunPoolFreeTest.py
```

This requires a native x64 Clang toolchain (`clang` on PATH, or `CC` set to its
executable). It builds the actual `Pool.c`, real/null runtime and compiler stubs
against the production headers, with host support for pages, locks, memory and
debug output. Executables are built in a temporary directory and removed.
Both assertion-enabled and `MDEPKG_NDEBUG` configurations cover metadata
validation before quarantine, eviction with overwritten metadata, reuse,
service dispatch/version rejection, inactive runtimes and disconnected compiler
callbacks. Negative controls disable only the allocator's early check and must
fail in both configurations. The host halt stub returns deliberately to verify
that a reported double free cannot proceed to metadata validation even if the
halt returns. This does not replace firmware link or on-board testing.

### Native direct-memory regression tests

On Windows, from the SBL source root:

```
python BootloaderCommonPkg/Library/AsanLib/Tests/RunMemoryTest.py
```

Requires native x64 Clang, NASM, `lld-link` and `llvm-nm` (`CC`/`NASM` can select the
compilers). Uses the actual SSE2 and REP C wrappers and assembly backends,
with the rename options read from the core DSC, plus the real runtime/stub.
Temporary executables cover both debug and release, connected runtime, no
runtime and no-ASAN linkage. Cases include oversized source reads and destination
writes, fills, legal accesses, zero length, overlap in both directions,
self-copy, intrinsic dispatch without double checks, service-connected payload
dispatch, disconnection, and shadow storage inside poisoned heap memory.
An unchecked-direct-call negative control must fail, reproducing the old gap.
Host debug/print/page/lock support is reused from `PoolFreeTest.c`.

The runner also compiles an instrumented freestanding caller with the production
`ASAN_CC_FLAGS` and links it with only the stub and real memory backends for IA32
and X64, both SSE2 and REP. These no-runtime EFI files are link checks, not boot
tests. Neither runner builds a complete firmware image or validates hardware.

### Library dependencies

| Library | Uses AsanLib for |
|---|---|
| `FullMemoryAllocationLib` | Heap shadow, red zones, freed blocks, quarantine |
| `MemoryAllocationLib` (core) | Guard above each Stage2 pool buffer |
| `Stage2` | Tracking the Stage2 pool, stack and globals; publishing the service |
| `PayloadEntryLib` | Using the Stage2 service; registering the stack and global ranges |
| `OsLoader` | The `asan` shell command (`AsanTest.c`) |

`AsanLib` is mapped to `AsanLibNull` for all modules in both `BootloaderCorePkg.dsc` and `PayloadPkg.dsc`, and to the real `AsanLib` only for Stage2, OsLoader and FirmwareUpdate when `ENABLE_ASAN` is set.

## Related build changes

- **`MemIntrinsicsLib`:** for IA32 NOOPT Clang builds, the compiler turns struct copies into calls to `memcpy`/`memset`, and nothing else provides them on IA32. This library implements them with `CopyMem`/`SetMem`. X64 already gets them from `IppCryptoSupportX64.c`.
- **`ShellAddCommand ()`:** a new ShellLib function that lets a module add commands to the shell. OsLoader uses it to add `asan` only when ASAN is enabled.
- **QEMU FSP patch:** the patch that builds the QEMU FSP adds Windows SDK `10.0.26100.0` and fixes a VS2022 C4319 warning in `MdeModulePkg/Core/Pei/Image/Image.c`.

## Costs

- **Image size:** Stage2, OsLoader and FirmwareUpdate grow because of the checks and the larger stack frames. On a real board, check that they still fit the flash layout.
- **Memory:** shadow memory is 1/8 of each tracked range. The payload's comes from the payload heap. Stage2's comes from the Stage2 pool, about 1/8 of the pool plus 1/8 of the Stage2 stack; raise `LOADER_RSVD_MEM_SIZE` if the warning about a too-small pool appears.
- **Stack:** frames are larger because of the guard areas. Increase `PLD_STACK_SIZE` if deep call paths overflow the stack.
- **Speed:** every load and store in instrumented code becomes a function call.

## Files

| File | Role |
|---|---|
| `BootloaderCommonPkg/Include/Library/AsanLib.h` | Runtime interface |
| `BootloaderCommonPkg/Include/Library/AsanStubLib.h` | `AsanStubConnect ()` |
| `BootloaderCommonPkg/Include/Service/AsanService.h` | Interface between the callbacks and the runtime |
| `BootloaderCommonPkg/Library/AsanLib/` | Runtime |
| `BootloaderCommonPkg/Library/AsanStubLib/` | `__asan_*` callbacks linked into every module |
| `BootloaderCommonPkg/Library/AsanLibNull/` | Empty runtime for normal builds |
| `BootloaderCommonPkg/Library/FullMemoryAllocationLib/` | Payload heap hooks |
| `BootloaderCorePkg/Library/MemoryAllocationLib/` | Stage2 pool guards |
| `BootloaderCorePkg/Stage2/Stage2Support.c` | `InitializeAsan ()` |
| `BootloaderCommonPkg/Library/MemIntrinsicsLib/` | `memcpy`/`memset` for IA32 NOOPT Clang builds |
| `BootloaderCommonPkg/Library/ShellLib/` | `ShellAddCommand ()` |
| `PayloadPkg/Library/PayloadEntryLib/` | Using the Stage2 service; stack and global registration |
| `PayloadPkg/OsLoader/AsanTest.c` | `asan` shell command |
| `BootloaderCorePkg/BootloaderCorePkg.dsc` | Library mapping and compiler flags |
| `BootloaderCorePkg/Tools/AsanSymbolize.py` | Host tool that adds file:line |
| `BuildLoader.py` | `--asan` option, toolchain check, `ASAN_CC_FLAGS`, `_ASAN_LIBS` and the generated `AsanComponents.dsc` |
