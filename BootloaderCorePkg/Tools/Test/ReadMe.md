# Slim Bootloader Python tool tests

Host-executed unit tests for the Python build tooling in
`BootloaderCorePkg/Tools`, plus a static checker for DSC library-class
resolution.

Nothing here compiles firmware, launches QEMU or touches hardware. The whole
suite runs in about a second on a clean checkout with no toolchain installed.

## Running

```sh
python -m pip install pytest
python -m pytest BootloaderCorePkg/Tools/Test
```

The static DSC checker can also be run on its own:

```sh
python BootloaderCorePkg/Tools/CheckDscLibraryClasses.py --all-boards
```

## What is covered

| File | Unit under test | Why it matters |
| --- | --- | --- |
| `test_dsc_library_classes.py` | `CheckDscLibraryClasses.py` | Every library class reachable from a DSC must be mapped, for every board |
| `test_common_utility.py` | `CommonUtility.py` bit/byte/alignment helpers | Underpin container packing, config data packing and IFWI layout arithmetic |
| `test_container_format.py` | `GenContainer.py` structures and auth sizing | The container header is an on-disk contract consumed by Stage2 and firmware update |
| `test_gen_cfg_data.py` | `GenCfgData.py` helpers, YAML round-trip, `.dlt` overrides | A silently wrong config blob changes board behaviour without failing the build |
| `test_board_config.py` | Every `Platform/*/BoardConfig*.py` | A board config that cannot load breaks the build for all boards |

## The defect this exists to prevent

Commit `de041423` split `CsmePerfIdToStrLib` out of `LoaderPerformanceLib` and
added the mapping to `BootloaderCorePkg.dsc`, but not to `PayloadPkg.dsc`.
That left `PayloadPkg.dsc` unbuildable:

```
error 4000: Instance of library class [CsmePerfIdToStrLib] is not found
```

It went unnoticed for roughly seven months because no CI job built that DSC
standalone, and was only found by someone following the public "add a new
payload" guide, which fails at its first build command. It was fixed in
slimbootloader/slimbootloader#2816.

`test_detects_missing_library_class_mapping` reproduces exactly that defect by
removing the mapping from a scratch copy of `PayloadPkg.dsc` and asserting the
checker reports it, naming the library that pulled the class in.

## How the DSC checker works

It resolves library classes the way the EDK II build does, without compiling:

1. Start from each module in `[Components]`.
2. Walk `[LibraryClasses]` of each module and of every library instance it
   resolves to, transitively.
3. Report any class with no instance mapped.

Two details matter:

- **Component override blocks.** A `Module.inf { <LibraryClasses> ... }` block
  applies to that module only, and must not leak into the DSC-wide map.
- **Conditionals are evaluated, not unioned.** `!if` / `!ifdef` / `!elseif` /
  `!else` are resolved against the `DEFINE` macros in scope. An undefined
  macro expands to an empty string, so `!if $(UNSET) == 1` is false. A
  condition that cannot be parsed is treated as true, so its body is still
  checked rather than silently skipped.

`BootloaderCorePkg.dsc` does `!include Platform.dsc`, which `BuildLoader.py`
generates from the selected board's `BoardConfig.py`. The checker generates
that file itself by reusing `BuildLoader.Build.create_dsc_inc_file`, so no
build-tool code is duplicated and every board can be checked. Without a board
the core DSC is skipped rather than reported as passing.

## Adding tests

Prefer units with no I/O and no external tool dependency. Anything that needs
a compiler, `nasm`, `iasl`, OpenSSL, built `BaseTools` binaries or a QEMU boot
belongs in the existing build and `qemu_test.py` stages instead.
