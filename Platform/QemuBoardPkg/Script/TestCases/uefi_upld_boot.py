#!/usr/bin/env python
## @ uefi_upld_boot.py
#
# Test UEFI universal payload on QEMU
#
# Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import shutil
from   test_base import *

def get_check_lines ():
    lines = [
              "===== Intel Slim Bootloader STAGE1A =====",
              "===== Intel Slim Bootloader STAGE1B =====",
              "===== Intel Slim Bootloader STAGE2 ======",
              "Universal Payload UEFI",
              "Jump to payload",
              "[Bds]Booting UEFI Shell",
              "any other key to continue.",
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

    print("Universal UEFI payload boot test for Slim BootLoader")

    tmp_dir = os.path.dirname(os_dir) + '/temp'
    create_dirs ([tmp_dir, os_dir])

    # download and unzip UEFI payload image
    local_file = tmp_dir + '/UniversalPayload.zip'
    download_url (
        'https://github.com/slimbootloader/slimbootloader/files/10221853/UniversalPayload.zip',
        local_file
    )
    unzip_file (local_file, tmp_dir)

    # Create new EPAYLOAD and replace it in SlimBootloader.bin
    layout = ',\n'.join ([
                 "( 'EPLD', 'EPAYLOAD.bin'          , 'NORMAL'  , 'RSA3072_PSS_SHA2_384'  , 'KEY_ID_CONTAINER_RSA3072'      , 0x10      , 0         , 0x0       )",
                 "( 'UEFI', 'UniversalPayload.elf'  , 'Lzma'    , 'SHA2_384'              , ''                              , 0x10      , 0         , 0x0       )"
               ])
    gen_file_from_object (tmp_dir + '/epld.txt', layout, '')
    sbl_dir = os.getcwd()
    os.environ['SBL_KEY_DIR'] = os.path.join (sbl_dir, '..', 'SblKeys')
    os.chdir (tmp_dir)
    cmds = [sys.executable, sbl_dir + '/BootloaderCorePkg/Tools/GenContainer.py', 'create',  '-l', 'epld.txt', '-td', get_tool_dir(sbl_dir)]
    run_process (cmds)
    cmds = [sys.executable, sbl_dir + '/BootloaderCorePkg/Tools/IfwiUtility.py', 'replace',  '-i', os.path.join(sbl_dir, bios_img), '-f', 'EPAYLOAD.bin', '-p', 'IFWI/BIOS/NRD/EPLD']
    run_process (cmds)
    os.chdir (sbl_dir)

    # run QEMU boot with timeout
    output = []
    lines = run_qemu(bios_img, os_dir, boot_order = 'ba', timeout = 8)
    output.extend(lines)

    # check test result
    ret = check_result (output, get_check_lines())

    print ('UEFI Universal Payload boot test %s !\n' % ('PASSED' if ret == 0 else 'FAILED'))

    return ret

if __name__ == '__main__':
    sys.exit(main())
