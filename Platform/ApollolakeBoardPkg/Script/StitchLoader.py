## @ StitchLoader.py
#  This is a python stitching script for Slim Bootloader APL build
#
# Copyright (c) 2018, Intel Corporation. All rights reserved. <BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

import os
import re
import sys
import struct
import argparse
import zipfile
import shutil
from ctypes import *
from subprocess  import check_output

ExtraUsageTxt = """
This script creates a new Apollo Lake Slim Bootloader IFWI image basing
on an existing IFWI image.  Please follow steps below:

  1.  Download an existing Apollo Lake UEFI IFWI image for the target platform
      such as MinnowBoard 3, LeafHill, etc.  The URL is below:
      https://firmware.intel.com/projects/minnowboard3

  2.  Build Slim Bootloader source tree and generate a stitching ZIP package.
      The generated ZIP package is located at:
      $(WORKSPACE)/Outputs/apl/Stitch_Components.zip

  3.  Stitch to create a new IFWI image.
      EX:
      python StitchLoader.py -i LEAFHILD.X64.0070.R01.1805070352.bin  -s
             Stitch_Components.zip -o SlimBoot.bin

  4.  Optionally, to view the flash layout for an given IFWI image,
      specify '-i' option only.
      EX:
      python StitchLoader.py -i LEAFHILD.X64.0070.R01.1805070352.bin

"""

FILE_ALIGN  = 0x1000

class BPDT_ENTRY_TYPE(Structure):
    Str2Val = {
        "BpdtOemSmip": 0,
        "BpdtCseRbe": 1,
        "BpdtCseBup": 2,
        "BpdtUcode": 3,
        "BpdtIbb": 4,
        "BpdtSbpdt": 5,
        "BpdtObb": 6,
        "BpdtCseMain": 7,
        "BpdtIsh": 8,
        "BpdtCseIdlm": 9,
        "BpdtIfpOverride": 10,
        "BpdtDebugTokens": 11,
        "BpdtUfsPhyConfig": 12,
        "BpdtUfsGppLunId": 13,
        "BpdtPmc": 14,
        "BpdtIunit": 15,
        "BpdtNvmConfig": 16,
        "BpdtUepType": 17,
        "BpdtUfsRateType": 18,
        "BpdtInvalidType": 19,
    }

    Val2Str = {v: k for k, v in Str2Val.items()}

    _fields_ = [('data', c_uint16)]

    def __init__(self, val=0):
        self.set_value(val)

    def __str__(self):
        if self.value < 0 or self.value >= self.Str2Val['BpdtInvalidType']:
            str = "BpdtInvalidType"
        else:
            str = self.Val2Str[self.value]
        return str

    def __int__(self):
        return self.get_value()

    def set_value(self, val):
        self.data = val

    def get_value(self):
        return self.data

    value = property(get_value, set_value)


class BPDT_INFO():
    def __init__(self, name, offset, bpdt_offset, primary):
        self.Name = name
        self.Primary = primary
        self.Offset = offset
        self.BpdtOffset = bpdt_offset


class BPDT_HEADER(Structure):
    _fields_ = [
        ('Signature', c_uint32),
        ('DescCnt', c_uint16),
        ('Version', c_uint16),
        ('XorSum', c_uint32),
        ('IfwiVer', c_uint32),
        ('FitVer', ARRAY(c_uint8, 8))
    ]


class BPDT_ENTRY(Structure):
    _fields_ = [
        ('Type', BPDT_ENTRY_TYPE),
        ('Flags', c_uint16),
        ('SubPartOffset', c_uint32),
        ('SubPartSize', c_uint32),
    ]


class SUBPART_DIR_HEADER(Structure):
    _fields_ = [
        ('HeaderMarker', ARRAY(c_char, 4)),
        ('NumOfEntries', c_uint32),
        ('HeaderVersion', c_uint8),
        ('EntryVersion', c_uint8),
        ('HeaderLength', c_uint8),
        ('Checksum', c_uint8),
        ('SubPartName', ARRAY(c_char, 4)),
    ]


class SUBPART_DIR_ENTRY(Structure):
    _pack_ = 1
    _fields_ = [
        ('EntryName', ARRAY(c_char, 12)),
        ('EntryOffset', c_uint32, 24),
        ('Reserved1', c_uint32, 8),
        ('EntrySize', c_uint32),
        ('Reserved2', c_uint32),
    ]


class SPI_DESCRIPTOR(Structure):
    DESC_SIGNATURE = 0x0FF0A55A
    FLASH_REGIONS = {
        "descriptor"    : 0x0,
        "ifwi"          : 0x4,
        "txe"           : 0x8,
        "pdr"           : 0x10,
        "dev_expansion" : 0x14,
    }
    _pack_ = 1
    _fields_ = [
        ('Reserved', ARRAY(c_char, 16)),
        ('FlValSig', c_uint32),
        ('FlMap0', c_uint32),
        ('FlMap1', c_uint32),
        ('FlMap2', c_uint32),
        ('Remaining', ARRAY(c_char, 0x1000 - 0x20)),
    ]


class BIOS_ENTRY(Structure):
    _pack_ = 1
    _fields_ = [
        ('Name', ARRAY(c_char, 4)),
        ('Offset', c_uint32),
        ('Length', c_uint32),
        ('Reserved', c_uint32),
    ]


class COMPONENT:
    TYPE_IMG  = 0
    TYPE_RGN  = 1
    TYPE_BP   = 2
    TYPE_BPDT = 3
    TYPE_DIR  = 4
    TYPE_FILE = 5

    def __init__(self, Name, Type, Offset, Length):
        self.Name   = Name
        self.Type   = Type
        self.Offset = Offset
        self.Length = Length
        self.Child  = []
        self.Data   = None
        self.Parent = None

    def AddChild(self, Child, Index = -1):
        Child.Parent = self
        if Index == -1:
            self.Child.append (Child)
        else:
            self.Child.insert (Index, Child)

    def SetData(self, File):
        if File:
            Fd =open(File, 'rb')
            Data = bytearray(Fd.read())
            Fd.close()
        else:
            Data = bytearray('\xff' * self.Length)
        if self.Length > len(Data):
            self.Data = Data + '\xff' * (self.Length - len(Data))
        else:
            self.Data = Data[:self.Length]

    def GetData(self):
        return self.Data


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


def Bytes2Val(Bytes):
    return reduce(lambda x, y: (x << 8) | y, Bytes[::-1])


def PrintTree(Root, Level=0):
    print "%-24s [O:0x%06X  L:0x%06X]" % ('  ' * Level + Root.Name,
            Root.Offset, Root.Length)
    for Comp in Root.Child:
        Level += 1
        PrintTree(Comp, Level)
        Level -= 1


def BpdtParser(BinData, BpdtOffset, Offset):
    SubPartList = []
    Idx = BpdtOffset + Offset
    BpdtHdr = BPDT_HEADER.from_buffer(
        bytearray(BinData[Idx:Idx + sizeof(BPDT_HEADER)]), 0)
    Idx += sizeof(BpdtHdr)
    if BpdtHdr.Signature != 0x55AA:
        raise Exception ("Invalid BPDT header!")

    SBpdt = None
    for Desc in range(BpdtHdr.DescCnt):
        BpdtEntry = BPDT_ENTRY.from_buffer(
            bytearray(BinData[Idx:Idx + sizeof(BPDT_ENTRY)]), 0)
        Idx += sizeof(BpdtEntry)
        DirList = []
        if 'BpdtSbpdt' == str(BpdtEntry.Type):
            SBpdt = BpdtEntry

        if BpdtEntry.SubPartSize > sizeof(SUBPART_DIR_HEADER):
            PartIdx = BpdtOffset + BpdtEntry.SubPartOffset
            SubPartDirHdr = SUBPART_DIR_HEADER.from_buffer(
                bytearray(BinData[PartIdx:PartIdx + sizeof(
                    SUBPART_DIR_HEADER)]), 0)
            PartIdx += sizeof(SubPartDirHdr)
            if '$CPD' == SubPartDirHdr.HeaderMarker:
                for Dir in range(SubPartDirHdr.NumOfEntries):
                    PartDir = SUBPART_DIR_ENTRY.from_buffer(
                        bytearray(BinData[PartIdx:PartIdx + sizeof(
                            SUBPART_DIR_ENTRY)]), 0)
                    PartIdx += sizeof(PartDir)
                    DirList.append(PartDir)

        SubPartList.append((BpdtEntry, DirList))

    return SubPartList, SBpdt


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


def ParseIfwiRegion (IfwiComp, ImgData):
    BiosStart, BiosLimit = IfwiComp.Offset, IfwiComp.Offset + IfwiComp.Length - 1
    BpOffset = [BiosStart, (BiosStart + BiosLimit + 1) / 2]
    for Idx, Offset in enumerate(BpOffset):
        BpComp = COMPONENT('BP%d' % Idx, COMPONENT.TYPE_BP, Offset,
                           (BiosLimit - BiosStart + 1) / 2)
        SubPartOffset = 0
        while True:
            Bpdt, SBpdtEntry = BpdtParser(ImgData, Offset, SubPartOffset)
            BpdtPrefix = '' if SubPartOffset == 0 else 'S'
            BpdtSize = SBpdtEntry.SubPartOffset if SBpdtEntry else BpdtComp.Child[-1].Length
            BpdtComp = COMPONENT('%sBPDT' % BpdtPrefix, COMPONENT.TYPE_BPDT,
                                 Offset + SubPartOffset, BpdtSize)
            SortedBpdt = sorted(Bpdt, key=lambda x: x[0].SubPartOffset)
            for Part, DirList in SortedBpdt:
                PartComp = COMPONENT(
                    str(Part.Type), COMPONENT.TYPE_DIR,
                    Offset + Part.SubPartOffset, Part.SubPartSize)
                SortedDir = sorted(DirList, key=lambda x: x.EntryOffset)
                for Dir in SortedDir:
                    FileComp = COMPONENT(Dir.EntryName, COMPONENT.TYPE_FILE,
                                         PartComp.Offset + Dir.EntryOffset,
                                         Dir.EntrySize)
                    PartComp.AddChild(FileComp)
                BpdtComp.AddChild(PartComp)
            BpComp.AddChild(BpdtComp)
            if SBpdtEntry:
                SubPartOffset = SBpdtEntry.SubPartOffset
            else:
                break
        IfwiComp.AddChild(BpComp)
    return IfwiComp


def LocateComponent(Root, Path):
    Nodes = Path.split('/')
    if len(Nodes) < 1 or Root.Name != Nodes[0]:
        return None
    if len(Nodes) == 1:
        return Root
    Comp = None
    for Comp in Root.Child:
        Comp = LocateComponent(Comp, '/'.join(Nodes[1:]))
        if Comp:
            break
    return Comp


def PatchFlashMap (ImageData, PlatformData = 0xffffffff):
    CompBpdtDict = {
      'RSVD' : 'ROOT/IFWI/BP1/SBPDT/BpdtObb/RSVD',
      'IAS1' : 'ROOT/IFWI/BP1/SBPDT/BpdtObb/PROV',
      'EPLD' : 'ROOT/IFWI/BP1/SBPDT/BpdtObb/EPLD',
      'PYLD' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/PLD',
      'VARS' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/VAR',
      'MRCD' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/MRCD',
      'CNFG' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/CFGD',
      'FWUP' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/FWUP',
      'SG02' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/OBB',
      'SG1B' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/IBB',
      'SG1A' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/IBBL',
      '_BPM' : 'ROOT/IFWI/BP0/BPDT/BpdtIbb/BPM.met',
      'OEMK' : 'ROOT/IFWI/BP0/BPDT/BpdtCseBup/oem.key',
    }

    print ("Patching Slim Bootloader Flash Map table ...")

    OutputImageData  =  ImageData
    Ifwi = ParseIfwiLayout (OutputImageData)
    if not Ifwi:
        return -1

    Pld  = LocateComponent (Ifwi, CompBpdtDict['PYLD'])
    if not Pld:
        CompBpdtDict['PYLD'] = 'ROOT/IFWI/BP1/SBPDT/BpdtObb/PLD'

    Bp0  = LocateComponent (Ifwi, 'ROOT/IFWI/BP0')
    Bp1  = LocateComponent (Ifwi, 'ROOT/IFWI/BP1')
    if not Bp0 or not Bp1:
        return -2

    # Locate FlashMap offset
    for Part in range(2):
        Path = CompBpdtDict['SG1A'].replace("BP0", "BP%d" % Part)
        Comp = LocateComponent (Ifwi, Path)
        if not Comp:
            if Part == 0:
                raise Exception("Cannot locate %s !" % Path)
            else:
                continue
        Stage1AOffset = Comp.Offset
        Stage1ALength = Comp.Length
        Temp = Stage1AOffset + Stage1ALength - 8

        c_uint32.from_buffer (OutputImageData, Temp - 4).value = PlatformData

        FlaMapOff = (Bytes2Val(OutputImageData[Temp:Temp+4]) + Stage1ALength) & 0xFFFFFFFF
        FlaMapStr = FlashMap.from_buffer (OutputImageData, Stage1AOffset + FlaMapOff)
        EntryNum  = (FlaMapStr.Length - sizeof(FlashMap)) // sizeof(FlashMapDesc)
        FlaMapStr.Romsize = Bp0.Length + Bp1.Length

        if Part == 1:
            FlaMapStr.Attributes |= FlashMap.FLASH_MAP_ATTRIBUTES['BACKUP_REGION']

        for Idx in range (EntryNum):
            Desc  = FlashMapDesc.from_buffer (OutputImageData, Stage1AOffset + FlaMapOff + sizeof(FlashMap) + Idx * sizeof(FlashMapDesc))
            Path = CompBpdtDict[Desc.Sig]
            if Part == 1 or (Desc.Flags & FlashMap.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT']):
                Path = Path.replace("BP0", "BP1")
            if Part == 1 and (Desc.Flags & FlashMap.FLASH_MAP_DESC_FLAGS['REDUNDANT']):
                Desc.Flags |= FlashMap.FLASH_MAP_DESC_FLAGS['BACKUP']
            if Desc.Sig == 'RSVD':
                Desc.Offset = Bp1.Offset + Bp1.Length - Desc.Size - Bp0.Offset
                continue

            Comp  = LocateComponent (Ifwi, Path)
            if not Comp:
                raise Exception("Cannot locate component '%s' in BPDT !" % Path)
            if (Desc.Size == 0) and (Desc.Offset == 0):
                Desc.Size = Comp.Length
                Desc.Offset = Comp.Offset - Bp0.Offset
                continue
            if Desc.Size != Comp.Length and Comp.Name != 'PROV':
                raise Exception("Mismatch component '%s' length in FlashMap and BPDT !" % CompBpdtDict[Desc.Sig])
            if Desc.Sig not in ['_BPM', 'OEMK'] and (Comp.Offset & 0xFFF > 0):
                raise Exception("Component '%s' %x is not aligned at 4KB boundary, " \
                                "please adjust padding size for IPAD/OPAD in BoardConfig.py and rebuild !" % (CompBpdtDict[Desc.Sig], Comp.Offset))
            Desc.Offset = Comp.Offset - Bp0.Offset

            # Last 4k in bios region is reserved for bootloader, throw exception if any component falls in that range
            if (Bp1.Offset + Bp1.Length - 0x1000) <= (Desc.Offset + Desc.Size) <= (Bp1.Offset + Bp1.Length):
                raise Exception("Component '%s' offset is in bootloader reserved region, please try to reduce compoent size !" % CompBpdtDict[Desc.Sig])

    print ("Flash map was patched successfully!")

    return 0


def AddComponent (Root, Path, Before = '$', FilePath = ''):
    Nodes      = Path.split('/')
    ParentPath = '/'.join(Nodes[:-1])
    DirComp    = LocateComponent (Root, ParentPath)
    if not DirComp:
        print ('Cannot find DIR %s !' % '/'.join(Nodes[:-1]))
        return -1
    if DirComp.Type != COMPONENT.TYPE_DIR:
        print ('Can only add FILE type !')
        return -2
    Index   = None
    if Before == '$':
        # Add to end
        Index = len(DirComp.Child)
    elif Before == '^':
        # Add to top
        Index = 0
    else:
        for Idx, File in enumerate(DirComp.Child):
            if Before == File.Name:
                Index = Idx
    if Index is None:
        print ('Cannot find FILE %s !' % Before)
        return -3
    else:
        Length = os.path.getsize(FilePath) if FilePath else 0x1000
        Comp = COMPONENT (Nodes[-1], COMPONENT.TYPE_FILE, 0, Length)
        Comp.SetData (FilePath)
        DirComp.AddChild (Comp, Index)
        return 0


def RemoveComponent (Root, Path):
    Nodes      = Path.split('/')
    ParentPath = '/'.join(Nodes[:-1])
    DirComp = LocateComponent (Root, ParentPath)

    if not DirComp:
        print ('Cannot find DIR %s !' % '/'.join(Nodes[:-1]))
        return -1
    if DirComp.Type != COMPONENT.TYPE_DIR:
        print ('Can only replace FILE type !')
        return -2
    Index = None
    for Idx, File in enumerate(DirComp.Child):
        if File.Name == Nodes[-1]:
            Index = Idx
            break
    if Index is None:
        print ('Cannot find FILE %s !' % Path)
        return -3
    else:
        del DirComp.Child[Index]

    return 0


def ReplaceComponent (Root, Path, FilePath):
    Comp = LocateComponent (Root, Path)
    if not Comp:
        print ('Cannot find FILE %s !' % Path)
        return -1
    if Comp.Type != COMPONENT.TYPE_FILE:
        print ('Can only replace FILE type !' % Path)
        return -2
    Comp.Length = os.path.getsize(FilePath) if FilePath else 0x1000
    if FilePath:
        Comp.SetData (FilePath)
    return 0


def CopyComponent (Root, Path, IfwiData):

    print ("COPY BP0 BPDT to BP1 BPDT ...")
    # Backup BP0 BPDT and BP1 SBPDT
    Bp1      = LocateComponent (Root, 'ROOT/IFWI/BP1')
    Bp0Bpdt  = LocateComponent (Root, 'ROOT/IFWI/BP0/BPDT')
    Bp1Bpdt  = LocateComponent (Root, 'ROOT/IFWI/BP1/BPDT')
    Bp1SBpdt = LocateComponent (Root, 'ROOT/IFWI/BP1/SBPDT')
    Bp0BpdtData  = bytearray(IfwiData[Bp0Bpdt.Offset :Bp0Bpdt.Offset  + Bp0Bpdt.Length])
    Bp1SBpdtData = bytearray(IfwiData[Bp1SBpdt.Offset:Bp1SBpdt.Offset + Bp1SBpdt.Length])

    # Copy to BP0 BPDT to BP1 BPDT
    Bp1SBpdtOffset = Bp1Bpdt.Offset + Bp0Bpdt.Length
    IfwiData[Bp1Bpdt.Offset:Bp1SBpdtOffset] = Bp0BpdtData

    # Append original BP1 SBPDT
    Bp1SBpdtEndOffset = Bp1SBpdtOffset + Bp1SBpdt.Length
    IfwiData[Bp1SBpdtOffset:Bp1SBpdtEndOffset] = Bp1SBpdtData
    Padding = Bp1.Offset + Bp1.Length - Bp1SBpdtEndOffset
    if Padding < 0:
        print ('Insufficiant space in BP1 partition !')
        return -1

    IfwiData[Bp1SBpdtEndOffset:Bp1SBpdtEndOffset+Padding] = '\xff' * Padding

    # Fix Sbpdt length in BP1 BPDT
    Offset  = Bp1Bpdt.Offset
    BpdtHdr = BPDT_HEADER.from_buffer(IfwiData, Offset)
    Offset += sizeof(BPDT_HEADER)
    for Idx in range(BpdtHdr.DescCnt):
        BpdtEntry = BPDT_ENTRY.from_buffer(IfwiData, Offset)
        if 'BpdtSbpdt' == str(BpdtEntry.Type):
            BpdtEntry.SubPartSize = Bp1SBpdt.Length
        Offset   += sizeof(BPDT_ENTRY)

    # Fix Sbpdt headers
    Offset  = Bp1SBpdtOffset
    BpdtHdr = BPDT_HEADER.from_buffer(IfwiData, Offset)
    Offset += sizeof(BPDT_HEADER)
    for Idx in range(BpdtHdr.DescCnt):
        BpdtEntry = BPDT_ENTRY.from_buffer(IfwiData, Offset)
        BpdtEntry.SubPartOffset += (Bp0Bpdt.Length - Bp1Bpdt.Length)
        Offset   += sizeof(BPDT_ENTRY)

    print ("Done!")

    return 0

def CreateDirData (Dir, IfwiData):
    # Claculate new DIR length and creaet new DIR data
    SupportList = ['BpdtIbb', 'BpdtObb']
    if Dir.Name not in SupportList:
        raise Exception ('Only %s are supported !' % ' '.join(SupportList))

    Adjust    = True
    Offset    = len(Dir.Child) * sizeof(SUBPART_DIR_ENTRY) + sizeof(SUBPART_DIR_HEADER)
    SubDirHdr = SUBPART_DIR_HEADER.from_buffer(IfwiData, Dir.Offset)
    DirData   = bytearray(SubDirHdr) + '\xff' * (Offset - sizeof(SUBPART_DIR_HEADER))
    SubDirHdr = SUBPART_DIR_HEADER.from_buffer(DirData)
    SubDirHdr.NumOfEntries = len(Dir.Child)
    for Idx, Comp in enumerate(Dir.Child):
        Delta = 0
        Parts = os.path.splitext(Comp.Name)
        if len(Parts) > 1 and Parts[1] in ['.man', '.met']:
            Align = 1
        elif Comp.Name in ['IPAD', 'OPAD']:
            Align = 0x40
        else:
            Align = FILE_ALIGN
            Delta = Dir.Offset & (FILE_ALIGN - 1)
        NextOffset  = ((Offset + Delta + Align - 1) & ~(Align - 1))
        Count = NextOffset - Offset
        if Adjust:
            Adjust = False
            Count -= Delta
        DirData.extend('\xff' * Count)
        CompData = Comp.GetData()
        if CompData:
            DirData.extend(CompData)
        else:
            DirData.extend(IfwiData[Comp.Offset : Comp.Offset + Comp.Length])
        EntryOffset = Idx * sizeof(SUBPART_DIR_ENTRY) + sizeof(SUBPART_DIR_HEADER)
        DirData[EntryOffset:EntryOffset+sizeof(SUBPART_DIR_ENTRY)] = '\x00' * sizeof(SUBPART_DIR_ENTRY)
        SubDir = SUBPART_DIR_ENTRY.from_buffer(DirData, EntryOffset)
        SubDir.EntryName   = Comp.Name
        SubDir.EntryOffset = NextOffset - Delta
        SubDir.EntrySize   = Comp.Length
        SubDir.Reserved1   = 0
        SubDir.Reserved2   = 0
        NextOffset += Comp.Length
        Offset = NextOffset

    Align       = FILE_ALIGN
    NextOffset  = ((Offset + Align - 1) & ~(Align - 1))
    DirData.extend('\xff' * (NextOffset - Offset))

    # Update checksum
    SubDirHdr = SUBPART_DIR_HEADER.from_buffer(DirData)
    SubDirHdr.Checksum = 0
    Length    = SubDirHdr.NumOfEntries * sizeof(SUBPART_DIR_ENTRY) + sizeof(SUBPART_DIR_HEADER)
    SumBuf    = (c_uint8 * Length).from_buffer(DirData)
    SubDirHdr.Checksum = (~sum(SumBuf) + 1) & 0xFF

    Remaining = (Dir.Offset + len(DirData))  & (FILE_ALIGN - 1)
    if Remaining:
        # Not page aligned, add padding
        DirData.extend('\xff' * (FILE_ALIGN - Remaining))

    return DirData


def RefreshIfwiForDir (Dir, IfwiData):
    # Claculate new DIR length and creaet new DIR data
    DirData = CreateDirData (Dir, IfwiData)
    Length  = len (DirData)
    AdjustLength = Length - Dir.Length
    if (Dir.Offset + Length)  & (FILE_ALIGN - 1):
        print hex(Dir.Offset), hex(Length)
        print  ('Dir total size needs to be 4KB aligned !')

    # Remember original SBPDT offset
    OrgBpdtOffset  = Dir.Parent.Parent.Child[0].Offset
    OrgSbpdtOffset = Dir.Parent.Parent.Child[1].Offset

    # Adjust offset and size for peer and up level in tree
    OldDir = Dir
    while Dir.Type != COMPONENT.TYPE_BP:
        for Each in Dir.Parent.Child:
            if Each.Offset > Dir.Offset:
                Each.Offset += AdjustLength
        Dir.Length += AdjustLength
        Dir = Dir.Parent
    Dir = OldDir

    # Update parent BPDT header info in IFWI data
    Parent  = Dir.Parent
    BpdtHdr = BPDT_HEADER.from_buffer(IfwiData, Parent.Offset)
    Base    = Parent.Offset + sizeof(BPDT_HEADER)
    Found   = False
    for Idx in range(BpdtHdr.DescCnt):
        BpdtEntry = BPDT_ENTRY.from_buffer(IfwiData, Base + Idx * sizeof(BPDT_ENTRY))
        Comps = [x for x in Parent.Child if x.Name == str(BpdtEntry.Type)]
        if len(Comps) > 1:
            raise Exception ('Found duplicated DIR %s !', BpdtEntry.Type)
        BpdtEntry.SubPartOffset = Comps[0].Offset - Parent.Parent.Offset
        if Dir.Name == str(BpdtEntry.Type):
            BpdtEntry.SubPartSize = Length
            Found = True
    if not Found:
        raise Exception ('Could not find DIR %s !', Dir.Name)

    # Update SBPDT DIR header in IFWI data
    BpComp = Parent.Parent
    if Parent.Name == 'BPDT':
        BpdtHdr    = BPDT_HEADER.from_buffer (IfwiData, OrgSbpdtOffset)
        BpdtHdr.XorSum = 0
        BaseOffset = OrgSbpdtOffset + sizeof(BPDT_HEADER)
        for Idx in range(BpdtHdr.DescCnt):
            BpdtEntry = BPDT_ENTRY.from_buffer(IfwiData, BaseOffset + Idx * sizeof(BPDT_ENTRY))
            BpdtEntry.SubPartOffset += AdjustLength
            if  (BpdtEntry.SubPartOffset + BpdtEntry.SubPartSize) > BpComp.Length:
                raise Exception ('Insufficiant space in layout !')
    else:
        # 'SBPDT', update length in BPDT
        BpdtHdr    = BPDT_HEADER.from_buffer (IfwiData, OrgBpdtOffset)
        BpdtHdr.XorSum = 0
        BaseOffset = OrgBpdtOffset + sizeof(BPDT_HEADER)
        for Idx in range(BpdtHdr.DescCnt):
            BpdtEntry = BPDT_ENTRY.from_buffer(IfwiData, BaseOffset + Idx * sizeof(BPDT_ENTRY))
            if str(BpdtEntry.Type) == 'BpdtSbpdt':
                BpdtEntry.SubPartSize += AdjustLength
            if  (BpdtEntry.SubPartOffset + BpdtEntry.SubPartSize) > BpComp.Length:
                raise Exception ('Insufficiant space in layout !')

    # Generate actual final IFWI Data
    if AdjustLength > 0:
        IfwiData[:] = IfwiData[:OldDir.Offset] + DirData + \
                      IfwiData[OldDir.Offset + OldDir.Length - AdjustLength : BpComp.Offset + BpComp.Length - AdjustLength] + \
                      IfwiData[BpComp.Offset + BpComp.Length:]
    else:
        AdjustLength = -AdjustLength
        IfwiData[:] = IfwiData[:OldDir.Offset] + DirData + \
                      IfwiData[OldDir.Offset + OldDir.Length + AdjustLength: BpComp.Offset + BpComp.Length] + \
                      '\xff' * AdjustLength + IfwiData[BpComp.Offset + BpComp.Length:]
    return 0


def ParseIfwiLayout (IfwiImgData):

    SpiDescriptor = SPI_DESCRIPTOR.from_buffer(IfwiImgData, 0)
    if SpiDescriptor.FlValSig != SpiDescriptor.DESC_SIGNATURE:
        return None

    RgnList = []
    Regions = [("descriptor", "DESC"), ("ifwi" , "IFWI"), ("pdr", "PDR"), ("dev_expansion", "DEVE")]
    for Rgn in Regions:
        Start, Limit = FindSpiRegion (SpiDescriptor, Rgn[0])
        if Start is None:
            continue
        RgnList.append((Rgn[1], Start, Limit - Start + 1))

    RgnList.sort (key = lambda Rgn : Rgn[1])

    Root = COMPONENT ('ROOT', COMPONENT.TYPE_IMG, 0, len(IfwiImgData))
    for Idx, Rgn in enumerate(RgnList):
        Comp = COMPONENT (Rgn[0], COMPONENT.TYPE_RGN, Rgn[1], Rgn[2])
        if Rgn[0] == 'IFWI':
            ParseIfwiRegion (Comp, IfwiImgData)
        Root.AddChild (Comp)

    return Root


def ManipulateIfwi (Action, Path, IfwiData, FileName = '', Before = '$'):
    print ('%s %s ...' % (Action, Path))
    Root = ParseIfwiLayout (IfwiData)
    if Action == "REMOVE":
        Ret  = RemoveComponent (Root, Path)
    elif Action == "ADD":
        Ret  = AddComponent (Root, Path, Before, FileName)
    elif Action == "REPLACE":
        Ret  = ReplaceComponent (Root, Path, FileName)
    elif Action == "COPY":
        Ret  = CopyComponent (Root, 'ROOT/IFWI/BP0/BPDT', IfwiData)
    else:
        Ret  = -100

    if Ret == 0 and Path:
        DirPath = '/'.join(Path.split('/')[:-1])
        Dir = LocateComponent (Root, DirPath)
        RefreshIfwiForDir (Dir, IfwiData)
        print ('Done!')

    return Ret


def CreateIfwiImage (IfwiIn, IfwiOut, BiosOut, PlatformData, NonRedundant, StitchDir):

    print ('Creating %sredundant image ...' % ('Non-' if NonRedundant else ''))

    RedundantPayload = True
    Fd = open(IfwiIn, "rb")
    IfwiData = bytearray(Fd.read())
    Fd.close()

    Root = ParseIfwiLayout (IfwiData)

    # Remove all in IBB/OBB
    RemoveList = [
      'ROOT/IFWI/BP0/BPDT/BpdtIbb',
      'ROOT/IFWI/BP1/BPDT/BpdtIbb',
      'ROOT/IFWI/BP1/SBPDT/BpdtObb'
    ]
    for DirPath in RemoveList:
        Comp = LocateComponent (Root, DirPath)
        if not Comp:
            continue
        for Each in Comp.Child:
            if Each.Name.endswith('.man') or Each.Name.endswith('.met'):
                continue
            Ret = ManipulateIfwi  ('REMOVE', DirPath + '/' + Each.Name,  IfwiData)
            if Ret != 0:
                raise Exception ('REMOVE failed (error code %d) !' % (Ret))

    # Copy BP0 BPDT into BP1 BPDT
    if not NonRedundant:
        Ret = ManipulateIfwi  ('COPY', '', IfwiData)
        if Ret != 0:
            raise Exception ('COPY failed (error code %d) !' % (Ret))

    if StitchDir:
        IbbList = [
          ('IBBL' , 'IBBL'),
          ('IBB'  , 'IBBM'),
          ('OBB'  , 'OBB'),
          ('FWUP' , 'FWU'),
          ('CFGD' , 'CFGDATA'),
          ('VAR'  , 'VAR'),
          ('MRCD' , 'MRCDATA'),
          ('PLD'  , 'PLD'),
        ]

        ObbList = [
          ('PROV' , 'FB'),
          ('EPLD' , 'EPLD'),
          ('PLD'  , 'PLD'),

        ]

        if RedundantPayload:
            del ObbList[-1]
        else:
            del IbbList[-1]

        Bp1Sbpdt = 'ROOT/IFWI/BP1/SBPDT/BpdtObb/'

        Loop = 1 if NonRedundant else 2
        for Bp in range(Loop):
            Dir = 'ROOT/IFWI/BP%d/BPDT/BpdtIbb/' % Bp
            for CompName, FileName in IbbList:
                FilePath = os.path.join(StitchDir, 'Stitch_%s.bin' % FileName)
                Ret = ManipulateIfwi  ('ADD', Dir + CompName,  IfwiData, FilePath)
                if Ret != 0:
                    raise Exception ('ADD failed (error code %d) !' % (Ret))

        for CompName, FileName in ObbList:
            if FileName == '':
                FilePath = ''
            else:
                FilePath = os.path.join(StitchDir, 'Stitch_%s.bin' % FileName)
            if CompName == 'EPLD' and not os.path.exists(FilePath):
                Ret = 0
            else:
                Ret = ManipulateIfwi  ('ADD', Bp1Sbpdt + CompName,  IfwiData, FilePath)
            if Ret != 0:
                raise Exception ('ADD failed (error code %d) !' % (Ret))

        PatchFlashMap (IfwiData, PlatformData)

    if BiosOut:
      print ('Creating BIOS image ...')
      Bios = LocateComponent (Root, 'ROOT/IFWI')
      Fd = open (BiosOut, 'wb')
      Fd.write(IfwiData[Bios.Offset:Bios.Offset+Bios.Length])
      Fd.close()

    print ('Creating IFWI image ...')
    Fd = open (IfwiOut, 'wb')
    Fd.write(IfwiData)
    Fd.close()


    print ('Done!')


def PrintIfwiLayout (IfwiFile):
    Fd = open(IfwiFile, "rb")
    IfwiData = bytearray(Fd.read())
    Fd.close()

    Root = ParseIfwiLayout (IfwiData)
    PrintTree (Root)

    Ifwi = LocateComponent (Root, 'ROOT/IFWI')

    print ("\nFree Space:")
    for Idx in range(2):
        Bp    = Ifwi.Child[Idx]
        Sbpdt = Bp.Child[1]
        print ("  BP%d Free Space: 0x%05X" % (Idx,  Bp.Length - ((Sbpdt.Offset + Sbpdt.Length) - Bp.Offset)))

    return 0


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

    ap.add_argument('-b',
                    '--output-bios-region',
                    dest='bios_out',
                    type=str,
                    default='',
                    help='Specify generated output BIOS region image file path')

    ap.add_argument('-s',
                    '--sitch-zip-file',
                    dest='stitch_in',
                    type=str,
                    default='',
                    help='Specify input sitching zip package file path')

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=0xFFFFFFFF,
                    help='Specify a platform specific data (HEX, DWORD) for customization')

    ap.add_argument('-n',
                    '--non-redundant',
                    dest='non_redundant',
                    action="store_true",
                    help='Specify if the flash layout will be full redundant or not')

    if len(sys.argv) == 1:
        print ('%s' % ExtraUsageTxt)

    args = ap.parse_args()

    if args.ifwi_out == '' and args.stitch_in == '':
        PrintIfwiLayout (args.ifwi_in)
        return 0
    else:
        if args.ifwi_out and args.stitch_in == '':
            Ret = CreateIfwiImage (args.ifwi_in, args.ifwi_out, args.bios_out, args.plat_data, args.non_redundant, None)
            return Ret

    # Unpack files from zip
    print ("Unpacking sitching ZIP package ...")
    OutputDir = os.path.dirname(args.ifwi_out)
    StitchDir = os.path.join(OutputDir, 'StitchComp')
    if os.path.exists(StitchDir):
        shutil.rmtree(StitchDir)
    zf = zipfile.ZipFile(args.stitch_in, 'r', zipfile.ZIP_DEFLATED)
    zf.extractall(StitchDir)
    zf.close()

    # Create new IFWI
    Ret = CreateIfwiImage (args.ifwi_in, args.ifwi_out, args.bios_out, args.plat_data, args.non_redundant, StitchDir)

    # Remove extracted files
    if os.path.exists(StitchDir):
        shutil.rmtree(StitchDir)

    return Ret

if __name__ == '__main__':
    sys.exit(main())

