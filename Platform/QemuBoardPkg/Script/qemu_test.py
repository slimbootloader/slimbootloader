#!/usr/bin/env python
## @ qemu_fwu.py
#
# QEMU test script
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import os
import sys
import shutil
import subprocess

def main():

    sbl_img = 'Outputs/qemu/SlimBootloader.bin'
    tst_img = 'Outputs/qemu/SblFwuTest.bin'
    fwu_dir = 'Outputs/qemu/temp'

    # check QEMU SlimBootloader.bin
    if not os.path.exists(sbl_img):
        print ('Could not find QEMU SlimBootloader.bin image !')
        return -1

    # copy a test image so that the original image will not change
    shutil.copyfile (sbl_img, tst_img)

    # generate QEMU FWU capsule
    if not os.path.exists(fwu_dir):
        os.mkdir (fwu_dir)
    cmd = [ sys.executable,
            'BootloaderCorePkg/Tools/GenCapsuleFirmware.py',
            '-p',  'BIOS', tst_img,
            '-k',  'BootloaderCorePkg/Tools/Keys/TestSigningPrivateKey.pem',
            '-o',  '%s/FwuImage.bin' % fwu_dir
          ]
    try:
        output = subprocess.run (cmd)
        output.check_returncode()
    except subprocess.CalledProcessError:
        print ('Failed to generate QEMU SlimBootloader capsule image !')
        return -2

    # run QEMU FWU test
    cmd = [ sys.executable,
            'Platform/QemuBoardPkg/Script/qemu_fwu.py',
            tst_img,
            fwu_dir
          ]
    try:
        output = subprocess.run (cmd)
        output.check_returncode()
    except subprocess.CalledProcessError:
        print ('Failed to run QEMU firmware update test !')
        return -3

    print ('\nAll tests passed !\n')

    return 0

if __name__ == '__main__':
    sys.exit(main())
