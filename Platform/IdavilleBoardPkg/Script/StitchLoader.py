## @ StitchLoader.py
#  This is a python stitching script for Slim Bootloader
#
# Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import os
import sys
import argparse
from   ctypes import *
from   functools import reduce

sys.dont_write_bytecode = True
sblopen_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '../../../')
if not os.path.exists (sblopen_dir):
    sblopen_dir = os.getenv('SBL_SOURCE', '')
sys.path.append (os.path.join(sblopen_dir, "BootloaderCorePkg" , "Tools"))

try:
    from   IfwiUtility import *
except ImportError:
    err_msg  = "Cannot find IfwiUtility module!\n"
    err_msg += "Please make sure 'SBL_SOURCE' environment variable is set to open source SBL root folder."
    raise  ImportError(err_msg)

extra_usage_txt = \
"""This script creates a new Idaville ICX-D Slim Bootloader IFWI image basing
on an existing IFWI base image.  Please note, this stitching method will work
only if Boot Guard in the base image is not enabled, and the silicon is not
fused with Boot Guard enabled.
Please follow steps below:
  1.  Get an existing Idaville ICX-D UEFI IFWI image associated with the target platform.
      Alternatively, the original IFWI image from the onboard SPI flash can be
      read out as the base image too.
  2.  Build Slim Bootloader binary, the generated ZIP package is located at:
      $(WORKSPACE)/Outputs/idv/SlimBootloader.bin
  3.  Stitch to create a new IFWI image.
      EX:
      python StitchLoader.py -i IDV_UEFI_IFWI.bin -s SlimBootloader.bin -o IDV_SBL_IFWI.bin
  4.  Optionally, to view the flash layout for an given IFWI image,
      specify '-i' option only.
      EX:
      python StitchLoader.py -i IFWI.bin
"""
def add_platform_data (bios_data, platform_data = None):
    if platform_data:
        # Parse BIOS image
        ifwi_parser = IFWI_PARSER ()
        bios = ifwi_parser.parse_ifwi_binary (bios_data)
        if not bios:
            return None

        for part in range(2):
          path = 'IFWI/BIOS/TS%d/SG1A' % part
          stage1A = ifwi_parser.locate_component (bios, path)
          if stage1A:
              plat_data_offset = stage1A.offset + stage1A.length - 12
              c_uint32.from_buffer (bios_data, plat_data_offset).value = platform_data
          print("Platform data was patched for %s" % path)

    return bios_data


def create_ifwi_image (ifwi_in, ifwi_out, sbl_in, platform_data):
    ifwi_data   = bytearray (get_file_data (ifwi_in))
    ifwi_parser = IFWI_PARSER ()
    ifwi = ifwi_parser.parse_ifwi_binary (ifwi_data)
    if not ifwi:
        print ("Invalid IFWI input image!")
        return -1

    # update bios region
    bios = ifwi_parser.locate_component (ifwi, 'IFWI/BIOS')
    if not bios:
        print ("Cound not find BIOS region!")
        return -2

    bios_data = bytearray (get_file_data (sbl_in))
    bios_data = add_platform_data (bios_data, platform_data)
    if not bios_data:
        print ("Failed to parse BIOS image!")
        return -3

    padding  = bios.length - len(bios_data)
    if padding < 0:
        print ("BIOS image is too big to fit into BIOS region!")
        return -4

    bios_data = b'\xff' * padding + bios_data
    ret = ifwi_parser.replace_component (ifwi_data, bios_data, 'IFWI/BIOS')
    if ret != 0:
        print ("Failed to replace BIOS region!")
        return -5

    # create new ifwi
    print("Creating IFWI image ...")
    if ifwi_out == '':
        ifwi_out = ifwi_in
    gen_file_from_object (ifwi_out, ifwi_data)

    print ('done!')


def print_ifwi_layout (ifwi_file):
    ifwi_data   = bytearray (get_file_data (ifwi_file))
    ifwi_parser = IFWI_PARSER ()
    ifwi = ifwi_parser.parse_ifwi_binary (ifwi_data)
    if ifwi:
        ifwi_parser.print_tree (ifwi)


if __name__ == '__main__':
    hexstr = lambda x: int(x, 16)

    ap = argparse.ArgumentParser()
    ap.add_argument('-i',
                    '--input-ifwi-file',
                    dest='ifwi_in',
                    type=str,
                    required=True,
                    help='Specify input template IFWI image file path')

    ap.add_argument('-o',
                    '--output-ifwi-file',
                    dest='ifwi_out',
                    type=str,
                    default='',
                    help='Specify generated output IFWI image file path')

    ap.add_argument('-s',
                    '--sbl-input',
                    dest='sbl_in',
                    type=str,
                    default='',
                    help='Specify input sbl binary file path')

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=None,
                    help='Specify a platform specific data (HEX, DWORD) for customization')


    if len(sys.argv) == 1:
        print ('%s' % extra_usage_txt)

    args = ap.parse_args()

    if args.ifwi_out == '' and args.sbl_in == '':
        print_ifwi_layout (args.ifwi_in)
        ret = 0
    else:
        ret = create_ifwi_image (args.ifwi_in, args.ifwi_out, args.sbl_in, args.plat_data)


    sys.exit(ret)

