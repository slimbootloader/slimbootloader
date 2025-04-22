#!/usr/bin/env python
## @ linux_boot.py
#
# Test boot linux on QEMU
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import struct
from   ctypes import Structure, c_char, c_uint32, c_uint8, c_uint64, c_uint16, sizeof, ARRAY
from   test_base import *

def get_check_lines ():
    lines = [
              "===== Intel Slim Bootloader STAGE1A =====",
              "===== Intel Slim Bootloader STAGE1B =====",
              "===== Intel Slim Bootloader STAGE2 ======",
              "Jump to payload",
              "Starting Kernel ...",
              "Linux version",
              "Freeing unused kernel image",
              'Welcome to "Minimal Linux"',
            ]
    return lines

def usage():
    print("usage:\n  python %s bios_image os_image_dir\n" % sys.argv[0])
    print("  bios_image  :  QEMU Slim Bootloader firmware image.")
    print("                 This image can be generated through the normal Slim Bootloader build process.")
    print("  os_image_dir:  Directory containing bootable OS image.")
    print("                 This image can be generated using GenContainer.py tool.")
    print("")


def main():
    if sys.version_info.major < 3:
        print ("This script needs Python3 !")
        return -1

    if len(sys.argv) != 3:
        usage()
        return -2

    bios_img = sys.argv[1]
    os_dir   = sys.argv[2]

    print("Linux boot test for Slim BootLoader")

    # download and unzip OS image
    tmp_dir = os.path.dirname(os_dir) + '/temp'
    create_dirs ([tmp_dir, os_dir])
    local_file = tmp_dir + '/QemuLinux.zip'
    download_url (
        'https://github.com/slimbootloader/slimbootloader/files/4463548/QemuLinux.zip',
        local_file
    )
    unzip_file (local_file, os_dir)

    # run QEMU boot with timeout
    output = []
    lines = run_qemu(bios_img, os_dir, timeout = 10)
    output.extend(lines)

    # check test result
    ret = check_result (output, get_check_lines())

    print ('\nLinux Boot test %s !\n' % ('PASSED' if ret == 0 else 'FAILED'))

    return ret

if __name__ == '__main__':
    sys.exit(main())
