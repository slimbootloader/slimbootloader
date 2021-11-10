#!/usr/bin/env python
## @ cfgdate_update.py
#
# Test CFGDATA update for SBL image post build.
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
              "GPIO GPP_A07 DATA: 0xC0000700 0x00000017",
              "GPIO GPP_A04 DATA: 0x80000101 0x00050014",
              "Jump to payload",
            ]
    return lines

def usage():
    print("usage:\n  python %s bios_image out_dir\n" % sys.argv[0])
    print("   bios_image :  QEMU Slim Bootloader firmware image.")
    print("                 This image can be generated through the normal Slim Bootloader build process.")
    print("      out_dir :  Output directory.")
    print("")


def main():
    if sys.version_info.major < 3:
        print ("This script needs Python3 !")
        return -1

    if len(sys.argv) != 3:
        usage()
        return -2

    bios_img = sys.argv[1]
    bios_dir = os.path.dirname (bios_img)
    tmp_dir  = os.path.join(os.path.dirname (sys.argv[2]), 'temp')
    sbl_dir  = os.getcwd()
    key_dir  = get_key_dir (sbl_dir)

    print("CFGDATA update test for Slim BootLoader")
    if os.path.exists(tmp_dir):
        shutil.rmtree (tmp_dir)
    create_dirs  ([tmp_dir])

    # copy files to temp dir
    src_files = ['CfgDataStitch.py', 'CfgDataDef.yaml']
    for file in src_files:
        shutil.copyfile (os.path.join(bios_dir, file), os.path.join(tmp_dir, file))

    # run cfg stitch tool to generate delta files and bin files
    cfg_stitch = os.path.join(bios_dir, src_files[0])
    cmds = [sys.executable, cfg_stitch, '-i', bios_img,
            '-k', key_dir + '/ConfigTestKey_Priv_RSA3072.pem',
            '-t', get_tool_dir (sbl_dir),
            '-s', sbl_dir + '/BootloaderCorePkg/Tools',
            '-c', tmp_dir,
            '-o', tmp_dir + '/SlimBootloader.bin']
    res = run_command (cmds)
    if res:
        return res

    # Modify delta files to change GPIO config
    fd   = open (tmp_dir + '/CfgDataExt_Brd1.dlt', 'r')
    dlt_lines = fd.readlines ()
    fd.close ()
    dlt_lines.append ('GPIO_CFG_DATA.GpioConfPad0_GPP_A4 | 0x80000101\n')
    dlt_lines.append ('GPIO_CFG_DATA.GpioConfPad1_GPP_A4.GPIOElectricalCfg | 5\n')
    dlt_lines.append ('GPIO_CFG_DATA.GpioConfPad1_GPP_A6.GPIOSkip | 1\n')
    fd   = open (tmp_dir + '/CfgDataExt_Brd1.dlt', 'w')
    fd.write (''.join(dlt_lines))
    fd.close ()

    # run cfg stitch tool again to generate new slimbootloader.bin
    res = run_command (cmds)
    if res:
        return res

    # run QEMU boot with timeout
    bios_img = os.path.join (tmp_dir, 'SlimBootloader.bin')
    output = []
    lines = run_qemu(bios_img, bios_dir, boot_order = '', timeout = 2)
    output.extend(lines)

    # check test result
    ret = check_result (output, get_check_lines())
    print ('\nCFGDATA update test %s !\n' % ('PASSED' if ret == 0 else 'FAILED'))

    return ret

if __name__ == '__main__':
    sys.exit(main())
