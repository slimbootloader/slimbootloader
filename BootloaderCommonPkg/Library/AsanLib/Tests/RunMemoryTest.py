## @file
# Native direct-memory regressions using selected BaseMemoryLib code and NASM.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import ast
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest


class MemoryTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tests = Path(__file__).resolve().parent
        cls.root = cls.tests.parents[3]
        cls.compiler = shutil.which(os.environ.get("CC", "clang"))
        cls.nasm = shutil.which(os.environ.get("NASM", "nasm"))
        if cls.nasm is None and Path("C:/NASM/nasm.exe").is_file():
            cls.nasm = "C:/NASM/nasm.exe"
        if cls.compiler is None or cls.nasm is None:
            raise RuntimeError("Native x64 clang and NASM required (or set CC/NASM)")
        if os.name != "nt":
            raise RuntimeError("Real X64 backends use the Microsoft ABI; run on Windows")
        cls.temp = tempfile.TemporaryDirectory(prefix="sbl-asan-memory-")
        cls.addClassCleanup(cls.temp.cleanup)
        cls.includes = [
            "-I", str(cls.root / "MdePkg/Include"),
            "-I", str(cls.root / "MdePkg/Include/X64"),
            "-I", str(cls.root / "BootloaderCommonPkg/Include"),
        ]
        # Consume the exact DSC rename flags, rather than a test-only wiring.
        dsc = (cls.root / "BootloaderCorePkg/BootloaderCorePkg.dsc").read_text()
        cls.rename = {}
        for instance in ("Sse2", "RepStr"):
            pattern = (r"MdePkg/Library/BaseMemoryLib" + instance +
                       r"/BaseMemoryLib" + instance +
                       r"\.inf\s*\{\s*<BuildOptions>\s*"
                       r"\*_CLANGPDB_\*_CC_FLAGS\s*=\s*([^\n]+)")
            match = re.search(pattern, dsc)
            if match is None:
                raise RuntimeError("Missing ASAN backend flags for " + instance)
            cls.rename[instance] = match[1].split()

    def run_command(self, command):
        result = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0,
                         " ".join(map(str, command)) + "\n" +
                         result.stdout + result.stderr)
        return result.stdout

    def build_and_run(self, instance, mode, release=False, bypass=False):
        name = instance + "-" + mode + ("-release" if release else "-debug")
        if bypass:
            name += "-bypass"
        directory = Path(self.temp.name) / name
        directory.mkdir()
        library = self.root / ("MdePkg/Library/BaseMemoryLib" + instance)
        common = [self.compiler, "-std=c11", "-O2",
                  "-Werror=implicit-function-declaration",
                  "-Werror=incompatible-pointer-types"] + self.includes
        if release:
            common += ["-DMDEPKG_NDEBUG"]
        objects = []
        for source in ("CopyMemWrapper", "SetMemWrapper", "SetMem64Wrapper"):
            obj = directory / (source + ".obj")
            flags = self.rename[instance] if mode != "no-asan" else []
            self.run_command(common + flags + ["-c", str(library / (source + ".c")),
                                              "-o", str(obj)])
            objects.append(str(obj))
        for source in ("CopyMem", "SetMem", "SetMem64"):
            obj = directory / (source + "-asm.obj")
            self.run_command([self.nasm, "-f", "win64", "-DASM_PFX(x)=x",
                              str(library / "X64" / (source + ".nasm")),
                              "-o", str(obj)])
            objects.append(str(obj))
        defines = []
        if bypass:
            defines += ["-DASAN_TEST_BYPASS_DIRECT"]
        if mode != "runtime":
            defines += ["-DASAN_TEST_NULL"]
        if mode == "no-asan":
            defines += ["-DASAN_TEST_NO_ASAN"]
        sources = [str(self.tests / "MemoryTest.c")]
        if mode != "no-asan":
            sources += [str(self.root /
                            "BootloaderCommonPkg/Library/AsanStubLib/AsanStub.c")]
        exe = directory / "test.exe"
        self.run_command(common + defines + sources + objects + ["-o", str(exe)])
        if bypass:
            result = subprocess.run([str(exe)], capture_output=True, text=True)
            self.assertNotEqual(result.returncode, 0)
            self.assertIn(": FALSE", result.stderr)
            print("PASS negative control: unchecked direct call was rejected")
        else:
            print(name + ":\n" + self.run_command([str(exe)]), end="")

    def test_selected_backends(self):
        for instance in ("Sse2", "RepStr"):
            for mode in ("runtime", "no-runtime", "no-asan"):
                for release in (False, True):
                    with self.subTest(instance=instance, mode=mode, release=release):
                        self.build_and_run(instance, mode, release)

    def test_unchecked_direct_call_is_rejected(self):
        self.build_and_run("Sse2", "runtime", bypass=True)

    def test_freestanding_instrumented_links(self):
        linker = shutil.which("lld-link")
        nm = shutil.which("llvm-nm")
        self.assertIsNotNone(linker, "lld-link is required")
        self.assertIsNotNone(nm, "llvm-nm is required")
        tree = ast.parse((self.root / "BuildLoader.py").read_text())
        asan_flags = next(ast.literal_eval(node.value).split()
                          for node in tree.body if isinstance(node, ast.Assign)
                          and any(isinstance(target, ast.Name) and
                                  target.id == "ASAN_CC_FLAGS"
                                  for target in node.targets))
        for arch, target, machine, nasm_format, prefix in (
                ("IA32", "i686-pc-windows-msvc", "x86", "win32", "_%+x"),
                ("X64", "x86_64-pc-windows-msvc", "x64", "win64", "x")):
            for instance in ("Sse2", "RepStr"):
                with self.subTest(arch=arch, instance=instance):
                    directory = Path(self.temp.name) / (arch + instance)
                    directory.mkdir()
                    library = self.root / ("MdePkg/Library/BaseMemoryLib" + instance)
                    common = [self.compiler, "-target", target, "-O2",
                              "-ffreestanding", "-fno-stack-protector",
                              "-DMDEPKG_NDEBUG",
                              "-I", str(self.root / "MdePkg/Include"),
                              "-I", str(self.root / ("MdePkg/Include/" + arch)),
                              "-I", str(self.root / "BootloaderCommonPkg/Include")]
                    objects = []
                    typed = "SetMem32" if arch == "IA32" else "SetMem64"
                    for source in ("CopyMem", "SetMem", typed):
                        obj = directory / (source + ".obj")
                        self.run_command(common + self.rename[instance] +
                                         ["-c", str(library / (source + "Wrapper.c")),
                                          "-o", str(obj)])
                        objects.append(str(obj))
                        obj = directory / (source + "-asm.obj")
                        self.run_command([self.nasm, "-f", nasm_format,
                                          "-DASM_PFX(x)=" + prefix,
                                          str(library / arch / (source + ".nasm")),
                                          "-o", str(obj)])
                        objects.append(str(obj))
                    for name, source, flags in (
                            ("stub", self.root /
                             "BootloaderCommonPkg/Library/AsanStubLib/AsanStub.c", []),
                            ("caller", self.tests / "MemoryLinkTest.c", asan_flags)):
                        obj = directory / (name + ".obj")
                        self.run_command(common + flags + ["-c", str(source),
                                                           "-o", str(obj)])
                        symbols = self.run_command([nm, "--undefined-only", str(obj)])
                        if name == "caller":
                            self.assertIn("CopyMem", symbols)
                            self.assertIn("SetMem", symbols)
                            self.assertIn("__asan_", symbols)
                        else:
                            self.assertIn("AsanInternalCopyMem", symbols)
                            self.assertIn("AsanInternalSetMem", symbols)
                        objects.append(str(obj))
                    self.run_command([linker, "/nodefaultlib", "/entry:MemoryEntry",
                                      "/subsystem:efi_application", "/safeseh:no",
                                      "/machine:" + machine, "/MERGE:.ASAN=.data",
                                      "/out:" + str(directory / "test.efi")] + objects)
                    print("PASS freestanding instrumented no-runtime link " +
                          arch + " " + instance)


if __name__ == "__main__":
    unittest.main(verbosity=2)
