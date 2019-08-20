## @ StitchLoader.py
#  This is a python stitching script for Slim Bootloader WHL/CFL build
#
# Copyright (c) 2019, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import os
import sys
import argparse
from   ctypes import *
from   functools import reduce

sys.dont_write_bytecode = True
sys.path.append (os.path.join(os.getenv('SBL_SOURCE', ''), 'BootloaderCorePkg' , 'Tools'))
try:
    from   IfwiUtility import *
except ImportError:
    ErrMsg  = "Cannot find IfwiUtility module!\n"
    ErrMsg += "Please make sure 'SBL_SOURCE' environment variable is set to open source SBL root folder."
    raise  ImportError(ErrMsg)

ExtraUsageTxt = """
This script creates a new Whiskeylake/Coffeelake Slim Bootloader IFWI image basing
on an existing IFWI base image.  Please note, this stitching method will work
only if Boot Guard in the base image is not enabled, and the silicon is not
fused with Boot Guard enabled.
Please follow steps below:
  1.  Download an existing Whiskeylake/Coffeelake UEFI IFWI image associated
      with the target platform.
      Alternatively, the original IFWI image from the onboard SPI flash can be
      read out as the base image too.
  2.  Build Slim Bootloader source tree.
      The generated Slim Bootloader binary is located at:
      $(WORKSPACE)/Outputs/cfl/SlimBootloader.bin
  3.  Stitch to create a new IFWI image.
      EX:
      python StitchLoader.py -i WHL_UEFI_IFWI.bin -s SlimBootloader.bin -o WHL_SBL_IFWI.bin
  4.  Optionally, to view the flash layout for an given IFWI image,
      specify '-i' option only.
      EX:
      python StitchLoader.py -i IFWI.bin
"""

def AddPlatformData (BiosData, PlatformData = None):
    if PlatformData:
        # Parse BIOS image
        IfwiParser = IFWI_PARSER ()
        Bios = IfwiParser.parse_ifwi_binary (BiosData)
        if not Bios:
            return None

        for Part in range(2):
          Path = 'IFWI/BIOS/TS%d/SG1A' % Part
          Stage1A = IfwiParser.locate_component (Bios, Path)
          if Stage1A:
              PlatDataOffset = Stage1A.offset + Stage1A.length - 12
              c_uint32.from_buffer (BiosData, PlatDataOffset).value = PlatformData
          print("Platform data was patched for %s" % Path)

    return BiosData


def CreateIfwiImage (IfwiIn, IfwiOut, SblIn, PlatformData):
    IfwiData   = bytearray (get_file_data (IfwiIn))
    IfwiParser = IFWI_PARSER ()
    Ifwi = IfwiParser.parse_ifwi_binary (IfwiData)
    if not Ifwi:
        print ("Invalid IFWI input image!")
        return -1

    # update bios region
    Bios = IfwiParser.locate_component (Ifwi, 'IFWI/BIOS')
    if not Bios:
        print ("Cound not find BIOS region!")
        return -2

    BiosData = bytearray (get_file_data (SblIn))
    BiosData = AddPlatformData (BiosData, PlatformData)
    if not BiosData:
        print ("Failed to parse BIOS image!")
        return -3

    Padding  = Bios.length - len(BiosData)
    if Padding < 0:
        print ("BIOS image is too big to fit into BIOS region!")
        return -4

    BiosData = b'\xff' * Padding + BiosData
    Ret = IfwiParser.replace_component (IfwiData, BiosData, 'IFWI/BIOS')
    if Ret != 0:
        print ("Failed to replace BIOS region!")
        return -5

    # create new ifwi
    print("Creating IFWI image ...")
    if IfwiOut == '':
        IfwiOut = IfwiIn
    gen_file_from_object (IfwiOut, IfwiData)

    print ('Done!')


def PrintIfwiLayout (IfwiFile):
    IfwiData   = bytearray (get_file_data (IfwiFile))
    IfwiParser = IFWI_PARSER ()
    Ifwi = IfwiParser.parse_ifwi_binary (IfwiData)
    if Ifwi:
        IfwiParser.print_tree (Ifwi)


def main():
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
        print ('%s' % ExtraUsageTxt)

    args = ap.parse_args()

    if args.ifwi_out == '' and args.sbl_in == '':
        PrintIfwiLayout (args.ifwi_in)
        return 0

    return CreateIfwiImage (args.ifwi_in, args.ifwi_out, args.sbl_in, args.plat_data)

if __name__ == '__main__':
    sys.exit(main())
