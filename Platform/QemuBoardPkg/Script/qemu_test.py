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
    tmp_dir = 'Outputs/qemu/temp'
    fwu_dir = 'Outputs/qemu/fwu'
    img_dir = 'Outputs/qemu/image'

    # check QEMU SlimBootloader.bin
    if not os.path.exists(sbl_img):
        print ('Could not find QEMU SlimBootloader.bin image !')
        return -1

    print ("SBL_KEY_DIR1 %s" % (os.environ.get('SBL_KEY_DIR')))

    # run test cases
    test_cases = [
      ('firmware_update.py',  [tst_img, fwu_dir]),
      ('linux_boot.py'     ,  [tst_img, img_dir])
    ]

    for test_file, test_args in test_cases:
        print ('######### Running run test %s' % test_file)
        # copy a test image so that the original image will not change
        shutil.copyfile (sbl_img, tst_img)

        # run QEMU test cases
        cmd = [ sys.executable, 'Platform/QemuBoardPkg/Script/TestCases/%s' % test_file] + test_args
        try:
            output = subprocess.run (cmd)
            output.check_returncode()
        except subprocess.CalledProcessError:
            print ('Failed to run test %s !' % test_file)
            return -3
        print ('######### Completed test %s\n\n' % test_file)

    print ('\nAll tests passed !\n')

    return 0

if __name__ == '__main__':
    sys.exit(main())
