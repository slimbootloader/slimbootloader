## @file
# Native pool-free regression runner. No firmware build or external test packages.
#
# Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


class PoolFreeTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tests = Path(__file__).resolve().parent
        cls.root = cls.tests.parents[3]
        cls.compiler = shutil.which(os.environ.get("CC", "clang"))
        if cls.compiler is None:
            raise RuntimeError("A native x64 clang compiler is required (or set CC)")
        cls.temp = tempfile.TemporaryDirectory(prefix="sbl-pool-free-")
        cls.addClassCleanup(cls.temp.cleanup)

    def build_and_run(self, name, *defines):
        exe = Path(self.temp.name) / (name + (".exe" if os.name == "nt" else ""))
        command = [
            self.compiler, "-std=c11", "-O0",
            "-Werror=implicit-function-declaration",
            "-Werror=incompatible-pointer-types",
            "-I", str(self.root / "MdePkg/Include"),
            "-I", str(self.root / "MdePkg/Include/X64"),
            "-I", str(self.root / "BootloaderCommonPkg/Include"),
        ]
        command += ["-D" + define for define in defines]
        command += [
            str(self.tests / "PoolFreeTest.c"),
            str(self.root / "BootloaderCommonPkg/Library/AsanStubLib/AsanStub.c"),
            "-o", str(exe),
        ]
        built = subprocess.run(command, capture_output=True, text=True)
        self.assertEqual(built.returncode, 0, built.stdout + built.stderr)
        result = subprocess.run([str(exe)], capture_output=True, text=True)
        return result

    def test_real_and_null(self):
        for null in (False, True):
            for release in (False, True):
                name = ("null" if null else "real") + ("-release" if release else "-debug")
                with self.subTest(configuration=name):
                    defines = []
                    if null:
                        defines.append("ASAN_TEST_NULL")
                    if release:
                        defines.append("MDEPKG_NDEBUG")
                    result = self.build_and_run(name, *defines)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    print(name + ":\n" + result.stdout, end="")

    def test_old_order_is_rejected(self):
        for release in (False, True):
            with self.subTest(release=release):
                defines = ["ASAN_TEST_BEFORE_FIX"]
                if release:
                    defines.append("MDEPKG_NDEBUG")
                result = self.build_and_run("old-order-" + str(release), *defines)
                self.assertNotEqual(result.returncode, 0)
                if release:
                    self.assertIn("(ReportCount () == 1) && (Halts == 1)",
                                  result.stderr)
                else:
                    self.assertIn("Unexpected ASSERT", result.stderr)
                print("PASS old-order negative control rejected (" +
                      ("release" if release else "debug") + ")")


if __name__ == "__main__":
    unittest.main(verbosity=2)
