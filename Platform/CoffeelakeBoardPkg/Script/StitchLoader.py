## @ StitchLoader.py
#  This is a python stitching script for Slim Bootloader WHL/CFL build
#
# Copyright (c) 2019, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import argparse
from ctypes import *

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

class SPI_DESCRIPTOR(Structure):
    DESC_SIGNATURE = 0x0FF0A55A
    FLASH_REGIONS = {
        "descriptor"    : 0x0,
        "bios"          : 0x4
    }
    _pack_ = 1
    _fields_ = [
        ('Reserved',  ARRAY(c_char, 16)),
        ('FlValSig',  c_uint32),
        ('FlMap0',    c_uint32),
        ('Remaining', ARRAY(c_char, 0x1000 - 0x28)),
    ]

def FindSpiRegion (SpiDescriptor, RgnName):
    Frba = ((SpiDescriptor.FlMap0 >> 16) & 0xFF) << 4
    FlReg = SpiDescriptor.FLASH_REGIONS[RgnName] + Frba
    RgnOff = c_uint32.from_buffer(SpiDescriptor, FlReg)
    RgnBase = (RgnOff.value & 0x7FFF) << 12
    RgnLimit = ((RgnOff.value & 0x7FFF0000) >> 4) | 0xFFF
    if RgnLimit <= RgnBase:
        return (None, None)
    else:
        return (RgnBase, RgnLimit)

def ParseIfwiLayout (IfwiImgData):
    SpiDescriptor = SPI_DESCRIPTOR.from_buffer(IfwiImgData, 0)
    if SpiDescriptor.FlValSig != SpiDescriptor.DESC_SIGNATURE:
        return None

    RgnList = []
    Regions = [("descriptor", "DESC"), ("bios" , "BIOS")]
    for Rgn in Regions:
        Start, Limit = FindSpiRegion (SpiDescriptor, Rgn[0])
        if Start is None:
            continue
        RgnList.append((Rgn[1], Start, Limit - Start + 1))

    RgnList.sort (key = lambda Rgn : Rgn[1])
    return RgnList

def GetRegion (RgnList, RegionName):
    if RgnList == None:
        Fatal("Invalid Region List !")

    for Rgn in RgnList:
        if Rgn[0] == RegionName:
            return Rgn

    return None

def ReplaceRegion (IfwiData, Rgn, InputFile, TailPos):
    RgnPos = Rgn[1]
    RgnLen = Rgn[2]

    Fd = open(InputFile, "rb")
    InputData = bytearray(Fd.read())
    Fd.close()

    InputLen = len(InputData)
    if RgnLen < InputLen:
        Fatal("Input binary size(0x%08x) cannot exceed %s region size(0x%08x) !" % (InputLen, Rgn[0], RgnLen))

    RgnOff = RgnLen - InputLen
    if TailPos == 1:
        if RgnLen != InputLen:
            IfwiData[RgnPos:RgnPos+RgnOff] = '\xff' * RgnOff
        IfwiData[RgnPos+RgnOff:RgnPos+RgnLen] = InputData
    else:
        IfwiData[RgnPos:RgnPos+InputLen] = InputData
        if RgnLen != InputLen:
            IfwiData[RgnPos+InputLen:RgnPos+RgnLen] = '\xff' * RgnOff

def CreateIfwiImage (IfwiIn, IfwiOut, SblIn):
    Fd = open(IfwiIn, "rb")
    IfwiData = bytearray(Fd.read())
    Fd.close()

    RgnList = ParseIfwiLayout(IfwiData)

    # update bios region
    print "Update BIOS region ..."
    Rgn = GetRegion(RgnList, "BIOS")
    if Rgn == None:
        Fatal("Failed to find BIOS region!")
    ReplaceRegion(IfwiData, Rgn, SblIn, 1)
    print "Done"

    # create new ifwi
    print "Creating IFWI image ..."
    Fd = open (IfwiOut, 'wb')
    Fd.write(IfwiData)
    Fd.close()
    print ('Done!')

def PrintIfwiLayout (IfwiFile):
    Fd = open(IfwiFile, "rb")
    IfwiData = bytearray(Fd.read())
    Fd.close()

    RgnList = ParseIfwiLayout (IfwiData)
    for Rgn in RgnList:
        print "%04s: 0x%08x - 0x%08x" % (Rgn[0], Rgn[1], Rgn[1] + Rgn[2])

def main():
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

    if len(sys.argv) == 1:
        print ('%s' % ExtraUsageTxt)

    args = ap.parse_args()

    if args.ifwi_out == '' and args.sbl_in == '':
        PrintIfwiLayout (args.ifwi_in)
        return 0

    return CreateIfwiImage (args.ifwi_in, args.ifwi_out, args.sbl_in)

if __name__ == '__main__':
    sys.exit(main())
