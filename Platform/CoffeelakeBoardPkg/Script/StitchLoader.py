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


class BIOS_ENTRY(Structure):
    _pack_ = 1
    _fields_ = [
        ('Name', ARRAY(c_char, 4)),
        ('Offset', c_uint32),
        ('Length', c_uint32),
        ('Reserved', c_uint32),
    ]


class COMPONENT:
    TYPE_BIOS = 0
    TYPE_IFWI = 1
    TYPE_BP   = 2
    TYPE_BPDT = 3
    TYPE_PART = 4
    TYPE_FILE = 5

    def __init__(self, Name, Type, Offset, Length):
        self.Name = Name
        self.Type = Type
        self.Offset = Offset
        self.Length = Length
        self.Child = []

    def AddChild(self, Child):
        self.Child.append(Child)
        self.Child.sort (key=lambda x: x.Offset)


class FlashMapDesc(Structure):
    _pack_ = 1
    _fields_ = [
        ('Sig',     ARRAY(c_char, 4)),
        ('Flags',   c_uint32),
        ('Offset',  c_uint32),
        ('Size',    c_uint32),
        ]

class FlashMap(Structure):
    FLASH_MAP_DESC_SIGNATURE = 'FLMP'

    FLASH_MAP_ATTRIBUTES = {
        "PRIMARY_REGION"  : 0x00000000,
        "BACKUP_REGION"   : 0x00000001,
    }

    FLASH_MAP_DESC_FLAGS = {
        "TOP_SWAP"      : 0x00000001,
        "REDUNDANT"     : 0x00000002,
        "NON_REDUNDANT" : 0x00000004,
        "NON_VOLATILE"  : 0x00000008,
        "COMPRESSED"    : 0x00000010,
        "BACKUP"        : 0x00000040,
    }

    _pack_ = 1
    _fields_ = [
        ('Sig',              ARRAY(c_char, 4)),
        ('Version',          c_uint16),
        ('Length',           c_uint16),
        ('Attributes',       c_uint8),
        ('Reserved',         ARRAY(c_char, 3)),
        ('Romsize',          c_uint32),
        ]


def Bytes2Val (bytes):
    return reduce(lambda x,y: (x<<8)|y,  bytes[::-1] )

def Val2Bytes (value, length):
    return [(value>>(i*8) & 0xff) for i in range(length)]

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


def ParseFlashMap (ImgData, BaseOff = 0):

    PartDict = {
        0x01:  "TS0",
        0x41:  "TS1",
        0x02:  "RD0",
        0x42:  "RD1",
        0x04:  "NRD",
        0x08:  "NVS",
    }

    #
    # No SPI descriptor, try to check the flash map
    #
    Offset = Bytes2Val(ImgData[-8:-4]) - (0x100000000 - len(ImgData))
    if Offset <0 or Offset >= len(ImgData) - 0x10:
        return None

    FlaMapOff = Offset
    if Bytes2Val(ImgData[FlaMapOff:FlaMapOff+4]) != 0x504d4c46:
        return None

    IfwiComp  = COMPONENT('IFWI', COMPONENT.TYPE_IFWI, BaseOff, len(ImgData))
    BiosComp  = COMPONENT('BIOS', COMPONENT.TYPE_BIOS, BaseOff, len(ImgData))
    CurrPart  = -1
    FlaMapStr = FlashMap.from_buffer (ImgData, FlaMapOff)
    EntryNum  = (FlaMapStr.Length - sizeof(FlashMap)) // sizeof(FlashMapDesc)
    for Idx in range (EntryNum):
        Idx   = EntryNum - 1 - Idx
        Desc  = FlashMapDesc.from_buffer (ImgData, FlaMapOff + sizeof(FlashMap) + Idx * sizeof(FlashMapDesc))
        FileComp = COMPONENT(Desc.Sig, COMPONENT.TYPE_FILE, Desc.Offset + BaseOff, Desc.Size)
        if CurrPart != Desc.Flags & 0x4F:
            CurrPart = Desc.Flags & 0x4F
            PartComp = COMPONENT('%s' % (PartDict[CurrPart]), COMPONENT.TYPE_BIOS, Desc.Offset + BaseOff, Desc.Size)
            BiosComp.AddChild (PartComp)
        else:
            PartComp.Length += Desc.Size

        PartComp.AddChild(FileComp)
    IfwiComp.AddChild(BiosComp)
    return IfwiComp


def PrintTree(Root, Level=0):
    print "%-24s [O:0x%06X  L:0x%06X]" % ('  ' * Level + Root.Name,
                                          Root.Offset, Root.Length)
    for Comp in Root.Child:
        Level += 1
        PrintTree(Comp, Level)
        Level -= 1

def LocateComponents(Root, Path):
    Result = []
    Nodes  = Path.split('/')
    if len(Nodes) < 1 or Root.Name != Nodes[0]:
        return []
    if len(Nodes) == 1:
        return [Root]
    for Comp in Root.Child:
        Return = LocateComponents(Comp, '/'.join(Nodes[1:]))
        if len(Return) > 0:
            Result.extend(Return)
    return Result

def LocateComponent(Root, Path):
    result = LocateComponents(Root, Path)
    if len(result) > 0:
        return result[0]
    else:
        return None

def AddPlatformData (InputData, RgnLen, PlatformData = None):
    InputLen = len(InputData)
    if RgnLen is not 0:
        if RgnLen < InputLen:
            Fatal("Input binary size(0x%08x) cannot exceed %s region size(0x%08x) !" % (InputLen, Rgn[0], RgnLen))
            return None

    if PlatformData:
      # Parse Flash Map
      Ifwi = ParseFlashMap (InputData)
      for Part in range(2):
        Path = 'IFWI/BIOS/TS%d/SG1A' %Part
        Stage1A = LocateComponent (Ifwi, Path)
        if Stage1A:
          PlatDataOffset = Stage1A.Offset + Stage1A.Length - 12
          c_uint32.from_buffer (InputData, PlatDataOffset).value = PlatformData
        print "Platform data was patched for %s" % Path
      return InputData

def ReplaceRegion (IfwiData, Rgn, InputFile, TailPos, PlatformData = None):
    RgnPos = Rgn[1]
    RgnLen = Rgn[2]

    Fd = open(InputFile, "rb")
    InputData = bytearray(Fd.read())
    Fd.close()

    if PlatformData:
        InputData = AddPlatformData(InputData, RgnLen, PlatformData)
        if InputData is None:
            return

    InputLen = len(InputData)
    RgnOff = RgnLen - InputLen
    if TailPos == 1:
        if RgnLen != InputLen:
            IfwiData[RgnPos:RgnPos+RgnOff] = '\xff' * RgnOff
        IfwiData[RgnPos+RgnOff:RgnPos+RgnLen] = InputData
    else:
        IfwiData[RgnPos:RgnPos+InputLen] = InputData
        if RgnLen != InputLen:
            IfwiData[RgnPos+InputLen:RgnPos+RgnLen] = '\xff' * RgnOff

def CreateIfwiImage (IfwiIn, IfwiOut, SblIn, PlatformData):
    Fd = open(IfwiIn, "rb")
    IfwiData = bytearray(Fd.read())
    Fd.close()

    RgnList = ParseIfwiLayout(IfwiData)

    # update bios region
    print "Update BIOS region ..."
    Rgn = GetRegion(RgnList, "BIOS")
    if Rgn == None:
        Fatal("Failed to find BIOS region!")

    ReplaceRegion(IfwiData, Rgn, SblIn, 1, PlatformData)
    print "Done"

    # create new ifwi
    print "Creating IFWI image ..."
    if IfwiOut == '':
        IfwiOut = IfwiIn
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
