#!/usr/bin/env python
## @ qemu_test.py
#
# QEMU test script
#
# Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import os
import sys
import shutil
import subprocess
import fnmatch

def main():
    if len(sys.argv) > 1:
        if sys.argv[1].endswith('.py'):
            test_pat = sys.argv[1]
        else:
            test_pat = sys.argv[1] + '.py'
    else:
        test_pat = '*.py'

    sbl_img = 'Outputs/qemu/SlimBootloader.bin'
    tst_img = 'Outputs/qemu/SblFwuTest.bin'
    tmp_dir = 'Outputs/qemu/temp'
    fwu_dir = 'Outputs/qemu/fwu'
    img_dir = 'Outputs/qemu/image'

    # check QEMU SlimBootloader.bin
    if not os.path.exists(sbl_img):
        print ('Could not find QEMU SlimBootloader.bin image !')
        return -1

    # run test cases
    test_cases = [
      ('firmware_update.py',  [tst_img, fwu_dir]),
      ('linux_boot.py'     ,  [tst_img, img_dir]),
      ('uefi_upld_boot.py' ,  [tst_img, tmp_dir]),
      ('cfgdata_update.py' ,  [tst_img, tmp_dir]),
    ]

    for test_file, test_args in test_cases:
        filtered = fnmatch.filter([test_file], test_pat)
        if len(filtered) == 0:
            continue

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

    print ('\nAll test cases passed !\n')

    return 0

if __name__ == '__main__':
    sys.exit(main())
