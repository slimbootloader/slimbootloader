## @ GenReport.py
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import re
import sys
import uuid
from   ctypes  import *
from functools import reduce

sys.dont_write_bytecode = True
from BuildUtility import STITCH_OPS

def Bytes2Val(bytes):
    return reduce(lambda x, y: (x << 8) | y, bytes[::-1])


def Val2Bytes(value, blen):
    return [(value >> (i * 8) & 0xff) for i in range(blen)]


def AlignPtrUp(offset, alignment=8):
    return (offset + alignment - 1) & ~(alignment - 1)


def AlignPtrDown(offset, alignment=8):
    return offset & ~(alignment - 1)

def ExecAssignment (var, val):
    namespace = {}
    exec ('%s = %s' % (var, val), namespace)
    return namespace[var]

class c_uint24(Structure):
    _pack_ = 1
    _fields_ = [('Data', (c_uint8 * 3))]

    def __init__(self, val=0):
        self.set_value(val)

    def __str__(self, indent=0):
        return '0x%.6x' % self.value

    def __int__(self):
        return self.get_value()

    def set_value(self, val):
        self.Data[0:3] = Val2Bytes(val, 3)

    def get_value(self):
        return Bytes2Val(self.Data[0:3])

    value = property(get_value, set_value)

class EFI_FIRMWARE_VOLUME_HEADER(Structure):
    _fields_ = [
        ('ZeroVector', ARRAY(c_uint8, 16)),
        ('FileSystemGuid', ARRAY(c_uint8, 16)),
        ('FvLength', c_uint64),
        ('Signature', ARRAY(c_char, 4)),
        ('Attributes', c_uint32),
        ('HeaderLength', c_uint16),
        ('Checksum', c_uint16),
        ('ExtHeaderOffset', c_uint16),
        ('Reserved', c_uint8),
        ('Revision', c_uint8)
    ]


class EFI_FIRMWARE_VOLUME_EXT_HEADER(Structure):
    _fields_ = [
        ('FvName', ARRAY(c_uint8, 16)),
        ('ExtHeaderSize', c_uint32)
    ]


class EFI_FFS_INTEGRITY_CHECK(Structure):
    _fields_ = [
        ('Header', c_uint8),
        ('File', c_uint8)
    ]


class EFI_FFS_FILE_HEADER(Structure):
    _fields_ = [
        ('Name', ARRAY(c_uint8, 16)),
        ('IntegrityCheck', EFI_FFS_INTEGRITY_CHECK),
        ('Type', c_uint8),
        ('Attributes', c_uint8),
        ('Size', c_uint24),
        ('State', c_uint8)
    ]


class FSP_INFORMATION_HEADER(Structure):
    _fields_ = [
        ('Signature', ARRAY(c_char, 4)),
        ('HeaderLength', c_uint32),
        ('Reserved1', c_uint16),
        ('SpecVersion', c_uint8),
        ('HeaderRevision', c_uint8),
        ('ImageRevision', c_uint32),
        ('ImageId', ARRAY(c_char, 8)),
        ('ImageSize', c_uint32),
        ('ImageBase', c_uint32),
        ('ImageAttribute', c_uint16),
        ('ComponentAttribute', c_uint16),
        ('CfgRegionOffset', c_uint32),
        ('CfgRegionSize', c_uint32),
        ('Reserved2', c_uint32),
        ('TempRamInitEntryOffset', c_uint32),
        ('Reserved3', c_uint32),
        ('NotifyPhaseEntryOffset', c_uint32),
        ('FspMemoryInitEntryOffset', c_uint32),
        ('TempRamExitEntryOffset', c_uint32),
        ('FspSiliconInitEntryOffset', c_uint32)
    ]


class IMG_INFO:
    def __init__(self, Type, Name, Offset=0, Length=0, Base=0):
        if Type == 'IMG':
            self.Level = 0
        elif Type == 'FD':
            self.Level = 1
        elif Type == 'FV' or Type == 'FSP':
            self.Level = 2
        elif Type == 'FFS':
            self.Level = 3
        else:
            raise Exception('Invalid type "%s" !' % Type)
        self.Name = Name
        self.Type = Type
        self.Offset = Offset
        self.Length = Length
        self.Base = Base
        self.ChildList = []

    def __str__(self):
        Lines = []
        Indent = '  ' * self.Level
        Lines.append(Indent + 'Type     : %s' % self.Type)
        Lines.append(Indent + 'Name     : %s' % self.Name)
        Lines.append(Indent + 'Offset   : 0x%08X' % self.Offset)
        Lines.append(Indent + 'Length   : 0x%08X' % self.Length)
        Lines.append(Indent + 'Base     : 0x%08X' % self.Base)
        ChildLine = 'ChildList:'
        if not len(self.ChildList):
            ChildLine = ChildLine + ' []'
            Lines.append(Indent + ChildLine)
        else:
            Lines.append(Indent + ChildLine)
            for Child in self.ChildList:
                Lines.append(str(Child))
            Lines.append('\n')
        return '\n'.join(Lines)


class REPORTER:
    Cols = [4, 13, 15, 30]

    def __init__(self, ImgName, RptType='FD'):
        self.ImgName = ImgName
        self.RptType = RptType
        if RptType == 'FD':
            self.Header = ['', 'FD', 'FV', 'FFS']
        else:
            self.Header = ['', 'Name', 'File', 'Details']
        self.Layout = [[], [], [], []]
        self.WordWrap = ['STAGE1', 'STAGE1A', 'STAGE1B', 'STAGE2']
        self.DictDscDefines = {}
        self.DictGuidNameXref = {
            '1BA0062E-C779-4582-8566-336AE8F78F09': 'ResetVector',
            'E08CA6D5-8D02-43AE-ABB1-952CC787C933': 'VbtBin',
            '26FDAA3D-B7ED-4714-8509-EECF1593800D': 'AcmBin',
            '5E2D3BE9-AD72-4D1D-AAD5-6B08AF921590': 'Logo',
            '3473A022-C3C2-4964-B309-22B3DFB0B6CA': 'VerInfo',
            '18EDB1DF-1DBE-4EC5-8E26-C44808B546E1': 'HashStore',
            'EFAC3859-B680-4232-A159-F886F2AE0B83': 'PcdDatabase',
            'CD17FF5E-7731-4D16-8441-FC7A113C392F': 'FitTable',
            '3CEA8EF3-95FC-476F-ABA5-7EC5DFA1D77B': 'FlashMap',
            'FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF': 'BLANK'
        }
        self.ColsWidth = [4, 13, 15, 30]

    def GetFspType(self, Attribute):
        return "XTMSXXXXOXXXXXXX" [(Attribute >> 12) & 0x0F]

    def GetFfsSize(self, FvFile, FfsOffset):
        FdIn = open(FvFile, "rb")
        FdIn.seek(FfsOffset)
        Buffer = bytearray(FdIn.read(sizeof(EFI_FFS_FILE_HEADER)))
        FdIn.close()
        FfsHdr = EFI_FFS_FILE_HEADER.from_buffer(Buffer)
        return int(FfsHdr.Size)

    def GetFvFfsList(self, FvData):
        FfsList = []
        FvHdr = EFI_FIRMWARE_VOLUME_HEADER.from_buffer(FvData, 0)
        if FvHdr.ExtHeaderOffset > 0:
            FvExtHdr = EFI_FIRMWARE_VOLUME_EXT_HEADER.from_buffer(
                Buffer, FvHdr.ExtHeaderOffset)
            Offset = FvHdr.ExtHeaderOffset + FvExtHdr.ExtHeaderSize
        else:
            Offset = FvHdr.HeaderLength

        while Offset < FvHdr.FvLength:
            Offset = AlignPtrUp(Offset)
            FfsHdr = EFI_FFS_FILE_HEADER.from_buffer(FvData, Offset)
            FfsName = str(uuid.UUID(bytes_le=bytes(bytearray(FfsHdr.Name)))).upper()
            Ffs = IMG_INFO('FFS', FfsName, Offset, int(FfsHdr.Size))
            if bytearray(FfsHdr.Name) == b'\xff' * 16:
                if (int(FfsHdr.Size) == 0xFFFFFF):
                    Ffs.Length = FvHdr.FvLength - Offset
            Offset += Ffs.Length
            FfsList.append(Ffs)

        return FfsList

    def ParseFdFile(self, FdFile):
        FdSize = os.path.getsize(FdFile)
        if FdSize < sizeof(EFI_FIRMWARE_VOLUME_HEADER):
            return None
        Fd = IMG_INFO('FD', os.path.basename(FdFile), 0, FdSize)
        FvOffset = 0
        FdIn = open(FdFile, "rb")
        FdData = bytearray(FdIn.read())
        FdIn.close()
        while FvOffset < FdSize:
            FvHdr = EFI_FIRMWARE_VOLUME_HEADER.from_buffer(FdData, FvOffset)
            if b'_FVH' != FvHdr.Signature:
                raise Exception("ERROR: Invalid FV header in FD '%s' !" %
                                FdFile)
            FspHdr = FSP_INFORMATION_HEADER.from_buffer(FdData,
                                                        FvOffset + 0x94)
            OldOffset = FvOffset
            if b'FSPH' == FspHdr.Signature:
                FvOffset += FspHdr.ImageSize
                Fv = IMG_INFO(
                    'FSP', 'FSP-' + self.GetFspType(FspHdr.ComponentAttribute),
                    0, FspHdr.ImageSize, FspHdr.ImageBase)
                Ffs = IMG_INFO('FFS', Fv.Name, 0, Fv.Length)
                Fv.ChildList = [Ffs]
            else:
                FvOffset += FvHdr.FvLength
                Fv = IMG_INFO('FV', 'UNKNOWN', 0, FvHdr.FvLength)
                Fv.ChildList = self.GetFvFfsList(FdData[OldOffset:])
                FirstFfs = Fv.ChildList[0]
                FirstFfsName = self.GuidToModuleName(FirstFfs.Name)
                if FirstFfsName.upper() == 'ACMBIN':
                    # For ACM FV, it contains ACM, BPM, KM, etc
                    # so let ACM occupy the full FV and ignore all other FFS
                    while len(Fv.ChildList) > 1:
                        Fv.ChildList.pop()
                    Fv.ChildList[0].Offset = 0
                    Fv.ChildList[0].Length = Fv.Length

            if Fd.Name.startswith('STAGE2.'):
                if ('STAGE2_LOAD_HIGH' in self.DictDscDefines) and \
                   (int(self.DictDscDefines['STAGE2_LOAD_HIGH'], 16) == 1):
                    # Mark FV/FSP will be loaded dynamically
                    Fv.Base = 0xFFFFFFFF
            Fv.Offset = OldOffset
            Fd.ChildList.append(Fv)
        if FvOffset != FdSize:
            raise Exception('Invalid FD format !')
        return Fd

    def ParseFvTxtFile(self, FvTxtFile):
        FvName = os.path.basename(FvTxtFile)[0:-7].upper()
        Fv = IMG_INFO('FV', FvName + '.FV')

        FdIn = open(FvTxtFile, "r")
        Lines = FdIn.readlines()
        FdIn.close()
        for Line in Lines:
            Match = re.match("EFI_FV_TOTAL_SIZE\s+=\s+(0x[a-fA-F0-9]+)", Line)
            if Match is not None:
                Fv.Length = int(Match.group(1), 16)
                continue
            Match = re.match("(0x[a-fA-F0-9]+)\s([0-9a-fA-F\-]+)", Line)
            if Match is not None:
                Ffs = IMG_INFO('FFS', Match.group(2).upper())
                Ffs.Offset = int(Match.group(1), 16)
                Ffs.Length = self.GetFfsSize(FvTxtFile[:-4], Ffs.Offset)
                Fv.ChildList.append(Ffs)

        FdIn = open(FvTxtFile[0:-7] + '.inf', "r")
        Lines = FdIn.readlines()
        FdIn.close()
        for Line in Lines:
            Match = re.match("EFI_BASE_ADDRESS\s+=\s+(0x[a-fA-F0-9]+)", Line)
            if Match is not None:
                # Update base if it is not dynamic loading
                if Fv.Base != 0xFFFFFFFF:
                    Fv.Base = int(Match.group(1), 16)
                break

        return Fv

    def ParseGuidXrefFile(self, XrefFile):
        FdIn = open(XrefFile, "r")
        Lines = FdIn.readlines()
        FdIn.close()
        for Line in Lines:
            Match = re.match("([0-9a-fA-F\-]+)\s(.+)", Line)
            if Match:
                Guid = Match.group(1).upper()
                if Guid not in self.DictGuidNameXref:
                    Path = Match.group(2).strip()
                    Name = os.path.basename(Path)
                    self.DictGuidNameXref[Match.group(1).upper()] = Name

    def ParseDscParameter(self, DefineDscFile):
        FdIn = open(DefineDscFile, "r")
        Lines = FdIn.readlines()
        FdIn.close()

        IsDefSection = False
        for Line in Lines:
            Line = Line.strip()
            if Line.startswith('#'):
                continue
            if Line.startswith('['):
                if Line.upper().startswith('[DEFINES]'):
                    IsDefSection = True
                else:
                    IsDefSection = False
            if IsDefSection:
                Match = re.match("^DEFINE\s+([_\w]+)\s*=\s*([_\w]+)", Line)
                if Match is not None:
                    self.DictDscDefines[Match.group(1).upper()] = Match.group(
                        2)

    def GuidToModuleName(self, Guid):
        if Guid in self.DictGuidNameXref:
            return self.DictGuidNameXref[Guid]
        else:
            if len(Guid) < 0x24:
                return Guid
            else:
                return '????'

    def Report(self, FdList, CellHeight=3):
        Cols = self.ColsWidth
        LineHeight = CellHeight

        def Center(Idx, ValueIn, Pattern):
            Value = ValueIn.strip()
            if Value == '':
                Value = Pattern[2 * Idx + 1] * (REPORTER.Cols[Idx] - 1)
            Space = REPORTER.Cols[Idx] - 1
            Blank = Space - len(Value)
            if Blank < 0:
                Blank = 0
                Value = Value[:Space]
            Lead = Blank // 2
            Tail = Blank - Lead
            return ' ' * Lead + Value + ' ' * Tail + Pattern[2 * Idx + 2]

        def OutSplitter(Pattern='+-+-+-+-+', Value=['', '', '', '']):
            Lines = []
            Lines.append(Pattern[0])
            for i, j in enumerate(REPORTER.Cols):
                Lines.append(Center(i, Value[i], Pattern))
            return '     ' + ''.join(Lines)

        def FillTable(Table, Base, TotalRow, Col, InputLine):
            Lines = InputLine.split('!')
            Count = TotalRow - len(Lines)
            if Count < 0:
                Start = 0
            else:
                Start = Base + (Count >> 1)
            for Line in Lines:
                if Start + 1 >= len(Table[Col]):
                    break
                Table[Col][Start] = Line
                Start += 1

        def GetPattern(Layout, LineIdx):
            Pattern = [ord(i) for i in '| + + + |']
            for Col in range(len(Layout)):
                Cnt = 0
                Idx = Col * 2 + 1
                for Val in Layout[Col]:
                    Cnt += Val
                    if LineIdx == Cnt - 1:
                        Pattern[Idx] = ord('-')
                        break
            if LineIdx == sum(Layout[0]) - 1:
                # Last line
                Pattern[0]  = ord('+')
                Pattern[-1] = ord('+')

            for Idx, Char in enumerate(Pattern):
                if Char == ord('+') and Idx > 0 and Idx < len(Pattern) - 1:
                    # Check orphan in a row
                    if Pattern[Idx - 1] == ord(' ') and Pattern[Idx + 1] == ord(' '):
                        Pattern[Idx] = ord('|')
            Pattern = ''.join(chr(i) for i in Pattern)
            return Pattern

        def Normalize(Com):
            if Com.Type == 'FFS':
                Name = self.GuidToModuleName(Com.Name)
            else:
                Name = Com.Name
            if Com.Type in ['FV', 'FSP']:
                if Com.Base == 0xFFFFFFFF:
                    Display = 'DYNAMIC'
                else:
                    Display = '0x%X' % Com.Base
                Name = '%s!(0x%X)!%s' % (Name, Com.Length, Display)
            else:
                Name = '%s!(0x%X)' % (Name, Com.Length)
            Lines = Name.split('!')
            Output = []
            for Line in Lines:
                Part = Line.split('.')[0]
                Part = Part.replace(',', '.')
                if len(Part) + 1 >= REPORTER.Cols[Com.Level]:
                    Idx = 0
                    for Word in self.WordWrap:
                        if Part.startswith(Word):
                            Idx = len(Word)
                            break
                    if Idx == 0:
                        Idx = len(Part) // 2
                    Part = '%s!%s' % (Part[:Idx], Part[Idx:])
                Output.append(Part)
            return '!'.join(Output)

        def CalcRows(Image):
            Layout = [[], [], [], []]
            Idx1 = 0
            for Img, FdList in Image:
                Idx2 = 0
                for Fd, FvList in FdList:
                    for Fv, FfsList in FvList:
                        FfsNum = 0
                        for Ffs in FfsList:
                            if Ffs.startswith('FSP'):
                                Adjust = 4
                            else:
                                Adjust = LineHeight
                            FfsNum += Adjust
                            Layout[3].append(Adjust)
                        Layout[2].append(FfsNum)
                    Layout[1].append(sum(Layout[2][Idx2:]))
                    Idx2 = len(Layout[2])
                Layout[0].append(sum(Layout[1][Idx1:]))
                Idx1 = len(Layout[1])
            Count = sum(Layout[0])
            self.Layout = Layout
            Table = [[' ' for i in range(Count)] for i in range(len(Layout))]

            Index = 0
            FfsIdx = 0
            FvIdx = 0
            FdIdx = 0
            for Idx0, (Img, FdList) in enumerate(Image):
                FillTable(Table, Index, Layout[0][Idx0], 0, Img)
                for Idx1, (Fd, FvList) in enumerate(FdList):
                    FillTable(Table, Index, Layout[1][FdIdx], 1, Fd)
                    for Idx2, (Fv, FfsList) in enumerate(FvList):
                        FillTable(Table, Index, Layout[2][FvIdx], 2, Fv)
                        for Idx3, Ffs in enumerate(FfsList):
                            FillTable(Table, Index, Layout[3][FfsIdx], 3, Ffs)
                            Index += Layout[3][FfsIdx]
                            FfsIdx += 1
                        FvIdx += 1
                    FdIdx += 1

            return Table, Layout

        def GetComponentLineIdx(Level, ObjIdx):
            LineIdx = 0
            LineLen = 0
            for Idx, Each in enumerate(self.Layout[Level]):
                if Idx == ObjIdx:
                    LineLen = Each
                    break
                LineIdx += Each
            return LineIdx, LineLen

        def MarkFvInfo(Fv, FvIdx, Lines):
            LineIdx, LineNum = GetComponentLineIdx(2, FvIdx)
            Index = LineIdx + LineNum
            Lines[Index] = Lines[Index] + '  %08X' % Fv.Base

        Name = '!'.join(self.ImgName)
        Tree = [(Name, [])]
        FdParent = Tree[0][1]
        for Fd in FdList:
            FdParent.append((Normalize(Fd), []))
            FvParent = FdParent[-1][1]
            for Fv in Fd.ChildList:
                FvParent.append((Normalize(Fv), []))
                FfsParent = FvParent[-1][1]
                for Ffs in Fv.ChildList:
                    FfsParent.append(Normalize(Ffs))

        Table, Layout = CalcRows(Tree)
        Rows = len(Table[0])

        Lines = []
        Header = OutSplitter('         ', self.Header)
        Lines.append(OutSplitter())

        for Idx in range(Rows):
            Pattern = GetPattern(Layout, Idx)
            Line = OutSplitter(Pattern, [Table[i][Idx] for i in range(4)])
            Lines.append(Line)

        FfsIdx = 0
        ImgSize = 0
        for Fd in FdList:
            ImgSize += Fd.Length

        Offset = ImgSize
        TopOffset = Offset
        LineIdx, LineNum = GetComponentLineIdx(3, 0)
        Lines[LineIdx] += ' %08X' % (TopOffset)

        for Fd in FdList:
            for Fv in Fd.ChildList:
                for Ffs in Fv.ChildList:
                    LineIdx, LineNum = GetComponentLineIdx(3, FfsIdx)
                    Index = LineIdx + LineNum
                    Offset -= Ffs.Length
                    Offset = AlignPtrDown(Offset)
                    if Ffs == Fv.ChildList[-1]:
                        if Fv.Type == 'FV':
                            Offset -= Ffs.Offset
                    Lines[Index] = Lines[Index] + ' %08X' % (Offset)
                    FfsIdx += 1

        Lines.insert(0, Header)

        print('\n'.join(Lines))

def ReportFd(FvDir, Title, FdNames):
    # Parse all xref
    Reporter = REPORTER(Title)
    Reporter.ParseGuidXrefFile(os.path.join(FvDir, 'Guid.xref'))
    Reporter.ParseDscParameter(os.path.join(
        os.path.dirname(sys.argv[0]), '../Platform.dsc'))

    # Get FVs
    FvList = []
    for File in os.listdir(FvDir):
        if not File.lower().endswith('fv.txt'):
            continue
        Fv = Reporter.ParseFvTxtFile(os.path.join(FvDir, File))
        FvList.append(Fv)

    # Get FDs
    FdList = []
    for File in FdNames:
        Fd = Reporter.ParseFdFile(os.path.join(FvDir, File + '.fd'))
        if Fd is not None:
            FdList.append(Fd)

    # Reverse FD/FV/FFS
    if True:
        FdList.reverse()
        for Fd in FdList:
            Fd.ChildList.reverse()
            for Fv in Fd.ChildList:
                Fv.ChildList.reverse()

    # Match FV binary into its FV Name/Base
    for Fd in FdList:
        for FvIndex, Fv1 in enumerate(Fd.ChildList):
            if Fv1.Type == 'FSP':
                continue
            Found = False
            for Fv2 in FvList:
                if Fv2.Type != Fv1.Type or Fv2.Length != Fv1.Length:
                    continue
                Match = 0
                for FfsIndex, Child2 in enumerate(Fv2.ChildList):
                    for Child1 in Fv1.ChildList:
                        if Child1.Name == Child2.Name:
                            Match += 1
                            break
                if Match == FfsIndex + 1:
                    Found = True
                    break
            if Found:
                if Fv1.Base != 0xFFFFFFFF:
                    Fv1.Base = Fv2.Base
                Fv1.Name = Fv2.Name
            else:
                raise Exception("FV in FD could not be identified !")

    if len(FdList):
        Reporter.Report(FdList)
        print('\n')


def ReportImageLayout(FvDir, ImgPath, ImgList, ImgStart, TopDown):

    Name = os.path.splitext(os.path.basename(ImgPath))[0]
    Name = Name.upper().replace('_', ' ')
    Reporter = REPORTER(Name, 'IMG')

    FdList = []
    Offset = 0
    FfsNum = 0
    for Name, Algo, Val, Mode, Pos in ImgList:

        BaseName = os.path.splitext(Name)[0]
        Fd = IMG_INFO('FD', BaseName)

        if Algo:
            FileName = BaseName + '.lz'
            Compressed = True
        else:
            FileName = Name
            Compressed = False

        OrgName = FileName
        OrgSize = Val if Name == 'EMPTY' else os.path.getsize(os.path.join(FvDir, FileName))
        if OrgSize == 0:
            continue

        if Mode != STITCH_OPS.MODE_FILE_NOP:
            Padded = True
            FileName = BaseName + '.pad'
        else:
            Padded = False

        Fd.Offset = Offset
        Fd.Length = Val if Name == 'EMPTY' else os.path.getsize(os.path.join(FvDir, FileName))
        Offset += Fd.Length

        FvName = FileName.replace('.', ',')
        Fv = IMG_INFO('FV', FvName)
        Fv.Offset = 0
        Fv.Length = Fd.Length

        FileSize = Fd.Length
        OrgSize = AlignPtrUp(OrgSize, 8)
        FfsName = OrgName.replace('.', ',')
        NameList = [FfsName, 'BLANK']
        if Compressed:
            NameList[0] = 'Compressed!' + NameList[0]
        elif Padded:
            pass
        else:
            del NameList[1]

        if NameList is None:
            NameList = ['IMAGE', 'BLANK']

        if Pos == STITCH_OPS.MODE_POS_HEAD:
            if FileSize != OrgSize:
                NameList.reverse()
                OrgSize = FileSize - OrgSize

        for Idx, Name in enumerate(NameList):
            if Idx == 0:
                Len = OrgSize
                Ffs = IMG_INFO('FFS', Name, 0, Len)
            else:
                Len = FileSize - OrgSize
                Ffs = IMG_INFO('FFS', Name, OrgSize, Len)
            if Len:
                Fv.ChildList.append(Ffs)
                FfsNum += 1

        Fd.ChildList.append(Fv)
        FdList.append(Fd)

    FinalSize = os.path.getsize(ImgPath)
    if Offset < FinalSize:
        Length = FinalSize - Offset
        Fd = IMG_INFO('FD', 'BLANK', 0, Length)
        Fv = IMG_INFO('FV', 'BLANK', 0, Length)
        Ffs = IMG_INFO('FFS', 'BLANK', 0, Length)
        Fv.ChildList.append(Ffs)
        Fd.ChildList.append(Fv)
        if TopDown:
          FdList.insert(0, Fd)
        else:
          FdList.append(Fd)
        FfsNum += 1

    Base = None

    for Fd in FdList:
        for Fv in Fd.ChildList:
            if Base is None:
                if TopDown:
                    Base = ImgStart - FinalSize
                else:
                    Base = ImgStart
            Fv.Base = Base
            Base += Fv.Length

    # Reverse FD/FV/FFS
    FdList.reverse()
    for Fd in FdList:
        Fd.ChildList.reverse()
        for Fv in Fd.ChildList:
            Fv.ChildList.reverse()

    FfsHeight = 6
    MinHeight = len(Reporter.ImgName) + 2
    if FfsNum * FfsHeight <= MinHeight:
        FfsHeight = (MinHeight + FfsNum - 1) // FfsNum
    Reporter.Report(FdList, FfsHeight)

def Usage():
    print("Usage: \n\tGenReport FvBuildDir [StitchInput]")

def Main():

    if len(sys.argv) < 2:
        Usage()
        return 1

    if len(sys.argv) < 3:
        StitchInput = 'ImgStitch.txt'
    else:
        StitchInput = sys.argv[2]

    FvDir = sys.argv[1]
    ImgLayoutPath = os.path.join(FvDir, StitchInput)
    if not os.path.exists(ImgLayoutPath):
        raise Exception("No layout file '%s' found !" % StitchInput)
        return 0

    FdIn  = open(ImgLayoutPath, 'r')
    Lines = FdIn.readlines()
    FdIn.close()

    FdReported = False
    FdNames    = ['STAGE2', 'STAGE1B', 'STAGE1A']
    for Line in Lines:
        Line    = Line.strip()
        Parts   = Line.split('=')
        if len(Parts) == 2:
            if Parts[0].strip() == 'BOARD_INFO':
                BrdInfo = ExecAssignment ('BrdInfo', Parts[1])
                Title = 'Flash Layout for Board %s' % BrdInfo[0]
                Padding = (sum(REPORTER.Cols) - len(Title)) // 2
                if Padding < 0:
                  Padding = 0
                print('')
                print('    %s%s' % (' ' * Padding, Title))
                print('    %s%s' % (' ' * Padding, '=' * len(Title)))
            if Parts[0].strip() == 'IMAGE_INFO':
                ImgInfo = ExecAssignment ('ImgInfo', Parts[1])
                ImgPath = os.path.join(FvDir, ImgInfo[0])
                Start   = ImgInfo[1]
                TopDown = ImgInfo[2]
                if not FdReported:
                    for Name in FdNames:
                        ReportFd(FvDir, '', [Name])
                    FdReported = True
            elif Parts[0].strip() == 'IMAGE_LIST':
                ImgList = ExecAssignment ('ImgList', Parts[1])
                ReportImageLayout(FvDir, ImgPath, ImgList, Start, TopDown)
        print('\n')

    return 0


if __name__ == '__main__':
    sys.exit(Main())
