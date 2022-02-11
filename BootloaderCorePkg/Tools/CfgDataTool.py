## @ CfgDataTool.py
#
# Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import sys
import collections

sys.dont_write_bytecode = True
from   IfwiUtility import *
from   CommonUtility import *

CFGDATA_INT_GUID = b'\xD0\x6C\x6E\x01\x34\x48\x7E\x4C\xBC\xFE\x41\xDF\xB8\x8A\x6A\x6D'

class CCfgData:

    DEBUG_FLAG_PARSE  = (1 << 0)

    DUMP_FLAG_INPUT   = (1 << 0)
    DUMP_FLAG_OUTPUT  = (1 << 1)
    DUMP_FLAG_VERBOSE = (1 << 7)

    class CDATA_BLOB_HEADER(Structure):
        ATTR_SIGNED  = 1 << 0
        ATTR_MERGED  = 1 << 7
        _pack_ = 1
        _fields_ = [
            ('Signature', ARRAY(c_char, 4)),
            ('HeaderLength', c_uint8),
            ('Attribute', c_uint8),
            ('Svn', c_uint8),
            ('Reserved', ARRAY(c_char, 1)),
            ('UsedLength', c_uint32),
            ('TotalLength', c_uint32),
        ]

    class CDATA_COND(Structure):
        _pack_ = 1
        _fields_ = [('Value', c_uint32)]

    class CDATA_HEADER(Structure):
        FLAG_ITEM_TYPE_NORMAL =  0
        FLAG_ITEM_TYPE_ARRAY  =  1
        FLAG_ITEM_TYPE_REFER  =  2
        FLAG_ITEM_TYPE_MASK   =  3
        _pack_ = 1
        _fields_ = [
            ('ConditionNum', c_uint32, 2),
            ('Length', c_uint32, 10),
            ('Flags', c_uint32, 4),
            ('Version', c_uint32, 4),
            ('Tag', c_uint32, 12),
        ]

    class CDATA_PLATFORM_ID(Structure):
        TAG = 0x0F0
        _pack_ = 1
        _fields_ = [('PlatformId', c_uint16), ('Reserved', c_uint16)]

    class CDATA_REFERENCE(Structure):
        _pack_ = 1
        _fields_ = [('PlatformId', c_uint16), ('Tag', c_uint16, 12), ('IsInternal', c_uint16, 1),  ('Reserved', c_uint16, 3)]

    class CDATA_ITEM_ARRAY(Structure):
        _pack_ = 1
        _fields_ = [
            ('HeaderSize', c_uint8),
            ('BasePlatformId', c_uint8),
            ('ItemSize', c_uint16),
            ('ItemCount', c_uint16),
            ('ItemIdBitOff', c_uint8),
            ('ItemIdBitLen', c_uint8),
            ('ItemValidBitOff', c_uint8),
            ('ItemUnused', c_uint8),
        ]

    def __init__ (self):
        self._Debug = 0 & CCfgData.DEBUG_FLAG_PARSE
        self.PlatformId   = None
        self.CfgDataBase  = collections.OrderedDict()
        self.CfgDataPid   = {}
        self.CfgDataItems = []
        self.CfgDataDataArrayDict = {}
        self.CfgDataArrayPidDict  = {}

    def NormalizePid (self, PlatformId):
        if (PlatformId & ~0x1F):
            raise Exception(
                "Invalid platform ID 0x%04X, should be in range from 0 to 31!\n"
                % PlatformId)
        return PlatformId

    def DumpTags (self, Flag, CfgItemList):
        Offset = sizeof(CCfgData.CDATA_BLOB_HEADER)
        for CfgItem in CfgItemList:
            CfgData   = CfgItem[0]
            CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(CfgData[0])
            DataCond  = CCfgData.CDATA_COND.from_buffer(CfgData[1])
            PrintData = True
            IsArray   = False
            ExtraInfo = []
            if CfgItem[3]:
                ExtraInfo.append ('Built-In')
            if (CfgTagHdr.Flags & CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_MASK) == \
                CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_ARRAY:
                ArrayInfo = CCfgData.CDATA_ITEM_ARRAY.from_buffer(CfgData[2])
                if ArrayInfo.BasePlatformId < 0x80:
                  ExtraInfo.append ('BasePid:0x%02X' %  ArrayInfo.BasePlatformId)
                ExtraInfo.append ('Array:%d*%d' % (ArrayInfo.ItemSize, ArrayInfo.ItemCount))
                IsArray = True
            if (CfgTagHdr.Flags & CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_MASK) == \
                CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_REFER:
                Reference = CCfgData.CDATA_REFERENCE.from_buffer(CfgData[2])
                if Reference.IsInternal:
                  Internal = '[Int]'
                else:
                  Internal = ''
                ExtraInfo.append ('ReferPid:0x%02X%s' %  (Reference.PlatformId, Internal))
                if Reference.Tag != CfgTagHdr.Tag:
                  ExtraInfo.append ('ReferTag:0x%03X' %  Reference.Tag)
                PrintData = False
            Extra = ', '.join (ExtraInfo)
            if Extra:
                Extra = '(%s)' % Extra
            print ("  TAG %03X: MSK=%08X LEN=%04X OFF=%04X %s" %
                  (CfgTagHdr.Tag, CfgItem[1], CfgTagHdr.Length * 4, Offset, Extra))
            if Flag & CCfgData.DUMP_FLAG_VERBOSE:
                if PrintData:
                    if not IsArray:
                        print_bytes (CfgData[2], 5)
                    else:
                        Offset    = 0
                        DataOffset = sizeof(CCfgData.CDATA_ITEM_ARRAY)
                        BitMaskLen = ArrayInfo.HeaderSize - DataOffset
                        print("    ARRAY HEADER:")
                        print_bytes (CfgData[2][:DataOffset], 5,  Offset)
                        Offset  +=  DataOffset
                        print("    ARRAY MASK:")
                        print_bytes (CfgData[2][DataOffset:DataOffset+BitMaskLen], 5, Offset)
                        Offset  +=  BitMaskLen
                        if ArrayInfo.ItemCount > 0:
                            print("    ARRAY DATA:")
                            ArrayData  = CfgData[2][ArrayInfo.HeaderSize:]
                            DataOffset = 0
                            for Idx in range (ArrayInfo.ItemCount):
                                print_bytes (ArrayData[DataOffset:DataOffset + ArrayInfo.ItemSize], 5, Offset)
                                DataOffset += ArrayInfo.ItemSize
                                Offset     += ArrayInfo.ItemSize

            Offset += CfgTagHdr.Length * 4

    def Dump (self, Flag, Input = True):
        if Flag & CCfgData.DUMP_FLAG_INPUT:
            print("%sPUT:" % ("IN" if Input else "OUT"))
            for CfgFile, (CfgItemList, CfgBlobHeader, IsBuiltIn) in list(self.CfgDataBase.items()):
                if CfgFile in self.CfgDataPid:
                    Pid = self.CfgDataPid[CfgFile]
                else:
                    Pid = 0
                BuiltIn = '*' if IsBuiltIn else ''
                print("PID=%04X LEN=%04X (%s%s)" % (Pid, CfgBlobHeader.UsedLength, CfgFile, BuiltIn))
                self.DumpTags (Flag, CfgItemList)

        if Flag & CCfgData.DUMP_FLAG_OUTPUT:
            print("MERGED:")
            self.DumpTags (Flag, self.CfgDataItems)
        print('')


    def ProcessCfgArray (self, Header, Data, PidMask, CfgBinFile):
        ArrayInfo = CCfgData.CDATA_ITEM_ARRAY.from_buffer(Data)
        ActualLen = ArrayInfo.ItemCount * ArrayInfo.ItemSize + ArrayInfo.HeaderSize + \
                    sizeof(Header) + sizeof(CCfgData.CDATA_COND) * Header.ConditionNum

        if ActualLen % 4 > 0:
            raise Exception(
                "The full array config size must be DWORD aligned in TAG '%03X'!" % Header.Tag)

        if ArrayInfo.ItemSize % 4 > 0:
            raise Exception(
                "Each config item size must be DWORD aligned in TAG '%03X'!" %
                Header.Tag)

        if Header.Length != ActualLen // 4:
            raise Exception(
                "Invalid array item count/size field in TAG '0x%03X'!" %
                Header.Tag)

        BitMaskLen = ArrayInfo.HeaderSize - sizeof (ArrayInfo)
        ByteWidth  = (ArrayInfo.ItemCount + 7) // 8
        if ByteWidth < 2:
            ByteWidth = 2
        if BitMaskLen < ByteWidth:
            raise Exception(
                "Bit mask array is too small in TAG '0x%03X', at least %d bytes required!"
                % (Header.Tag, ByteWidth))

        BitMaskDat = bytearray('1' * ArrayInfo.ItemCount + '0' *
                     (BitMaskLen * 8 - ArrayInfo.ItemCount), 'utf-8')

        ItemValidByteOffset = ArrayInfo.ItemValidBitOff // 8
        ItemValidByteMask   = 1 << (ArrayInfo.ItemValidBitOff & (8 - 1))

        DataOff     = ArrayInfo.HeaderSize
        ArrayTagKey = '%03X' % Header.Tag
        if ArrayInfo.BasePlatformId == 0x80:
            # The bit mask has been processed for base table
            if ArrayTagKey in self.CfgDataDataArrayDict:
                raise Exception(
                    "Base configuration already exists for TAG '0x%s'!" % ArrayTagKey)
            Pid = (PidMask&-PidMask).bit_length() - 1
            if Pid < 0:
               raise Exception("Invalid condition value '%08X'!" % PidMask)
            self.CfgDataDataArrayDict[ArrayTagKey] = []
            self.CfgDataArrayPidDict[ArrayTagKey]  = Pid

            while DataOff < len(Data):
                self.CfgDataDataArrayDict[ArrayTagKey].append(Data[DataOff:DataOff +
                                                          ArrayInfo.ItemSize])
                DataOff += ArrayInfo.ItemSize

        elif ArrayInfo.BasePlatformId == 0xFF:

            # The bit mask has not been processed yet for base table
            if ItemValidByteOffset >= ArrayInfo.ItemSize:
                raise Exception(
                    "Item valid byte offset (%d) must be less than the item size (%d) in TAG '0x%03X'!"
                    % (ItemValidByteOffset, ArrayInfo.ItemSize, Header.Tag))

            if ArrayTagKey in self.CfgDataDataArrayDict:
                if self.CfgDataPid[CfgBinFile] == self.CfgDataArrayPidDict[ArrayTagKey]:
                    ArrayInfo.BasePlatformId = 0x80
                else:
                    ArrayInfo.BasePlatformId = self.CfgDataArrayPidDict[ArrayTagKey]
            else:
                # Mark it as a base config item
                ArrayInfo.BasePlatformId = 0x80
                self.CfgDataDataArrayDict[ArrayTagKey] = []
                self.CfgDataArrayPidDict[ArrayTagKey]  = self.CfgDataPid[CfgBinFile]

            # Check the invliad flag and remove those items
            ItemDict = {}
            RemovedItem = 0
            Index = 0
            DataLen = len(Data)

            while DataOff < DataLen:
                Remove = False
                if ArrayInfo.BasePlatformId == 0x80:
                    # Check ItemID to make sure it is unique
                    ItemId = get_bits_from_bytes (Data[DataOff:DataOff + ArrayInfo.ItemSize], ArrayInfo.ItemIdBitOff, ArrayInfo.ItemIdBitLen)
                    if ItemId not in ItemDict.keys():
                        ItemDict[ItemId] = 1
                    else:
                        raise Exception("ItemId '0x%X' is not unique indicated by ItemIdBitOff/ItemIdBitLen in array header !" % ItemId)

                    # It is a base table, remove marker and assemble mask
                    if Data[DataOff + ItemValidByteOffset] & ItemValidByteMask:
                        Data[DataOff + ItemValidByteOffset] = Data[
                            DataOff + ItemValidByteOffset] ^ ItemValidByteMask
                        BitMaskDat[Index] = ord('0')
                    self.CfgDataDataArrayDict[ArrayTagKey].append(Data[DataOff:DataOff +
                                                          ArrayInfo.ItemSize])
                else:
                    if ArrayTagKey in self.CfgDataDataArrayDict:
                        if Data[DataOff:DataOff + ArrayInfo.ItemSize] != self.CfgDataDataArrayDict[ArrayTagKey][Index]:
                            BitMaskDat[Index] = ord('0')
                        else:
                            Remove = True
                    if Data[DataOff + ItemValidByteOffset] & ItemValidByteMask:
                        Remove = True
                if Remove:
                    Data[DataOff:] = Data[DataOff + ArrayInfo.ItemSize:] + b'\x00' * ArrayInfo.ItemSize
                    DataLen -= ArrayInfo.ItemSize
                    RemovedItem += 1
                else:
                    DataOff += ArrayInfo.ItemSize
                Index += 1

            ArrayInfo.ItemCount -= RemovedItem
            Header.Length -= (RemovedItem * ArrayInfo.ItemSize) // 4

            # Update mask
            BitWidth = BitMaskLen * 8
            MaskHexStr = '{0:0{w}x}'.format(int(BitMaskDat.decode()[::-1], 2), w=BitWidth // 4)
            BinData = bytearray.fromhex(MaskHexStr)[::-1]
            Offset  = sizeof (CCfgData.CDATA_ITEM_ARRAY)
            Data[Offset:Offset + BitMaskLen] = BinData

            return DataLen

    def Parse (self, CfgBinFile):

        if CfgBinFile.endswith('*'):
            IsBuiltIn  = True
            CfgBinFile = CfgBinFile[:-1]
        else:
            IsBuiltIn  = False

        if self._Debug & CCfgData.DEBUG_FLAG_PARSE:
            MiscStr = ' built-in' if IsBuiltIn else ''
            print ("Parsing%s config binary '%s'" % (MiscStr, CfgBinFile))

        with open(CfgBinFile, "rb") as Fin:
            FileData = bytearray(Fin.read())

        CfgBlobHeader = CCfgData.CDATA_BLOB_HEADER.from_buffer(FileData)
        if CfgBlobHeader.Signature != b'CFGD':
            raise Exception("Invalid config binary file '%s' !" % CfgBinFile)

        IsMergedCfg = True if CfgBlobHeader.Attribute & CCfgData.CDATA_BLOB_HEADER.ATTR_MERGED else False

        CfgItemList = []
        Length = min(len(FileData), CfgBlobHeader.UsedLength)

        # Find Platform ID first
        if not IsMergedCfg:
            Offset = sizeof(CCfgData.CDATA_BLOB_HEADER)
            while Offset < Length:
                CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(FileData, Offset)
                CfgDlen   = CfgTagHdr.Length * 4
                CfgHdrLen = sizeof(CCfgData.CDATA_HEADER) + CfgTagHdr.ConditionNum * sizeof(CCfgData.CDATA_COND)
                if CfgTagHdr.Tag == CCfgData.CDATA_PLATFORM_ID.TAG:
                    NextOff   = Offset + CfgHdrLen
                    DataBin   = FileData[NextOff:Offset + CfgDlen]
                    Pid       = CCfgData.CDATA_PLATFORM_ID.from_buffer(DataBin)
                    self.CfgDataPid[CfgBinFile] = self.NormalizePid(Pid.PlatformId)
                    break

                if (CfgTagHdr.Flags & CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_MASK) == \
                    CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_ARRAY:
                    NextOff   = Offset + CfgHdrLen
                    ArrayInfo = CCfgData.CDATA_ITEM_ARRAY.from_buffer(FileData[NextOff:])
                    ActualLen = ArrayInfo.ItemCount * ArrayInfo.ItemSize + ArrayInfo.HeaderSize + \
                                sizeof(CfgTagHdr) + sizeof(CCfgData.CDATA_COND) * CfgTagHdr.ConditionNum
                    if ActualLen % 4 > 0:
                        raise Exception(
                            "The full array config size must be DWORD aligned in TAG '%03X'!" % CfgTagHdr.Tag)
                    if ActualLen != CfgDlen:
                        raise Exception(
                            "Actual config data length does not match the length indicated "
                            "by the config header in TAG '%03X'!" % CfgTagHdr.Tag)
                Offset   += CfgDlen
            if CfgBinFile not in self.CfgDataPid:
                raise Exception("TAG PlatformId cannot be found in file '%s'!" % CfgBinFile)

        # Add config tags
        Offset = sizeof(CCfgData.CDATA_BLOB_HEADER)
        while Offset < Length:
            CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(FileData, Offset)
            NextOff = Offset + sizeof(CCfgData.CDATA_HEADER)
            CondBin = bytearray()
            for Idx in range(0, CfgTagHdr.ConditionNum):
                CondBin.extend(FileData[NextOff:NextOff + sizeof(
                    CCfgData.CDATA_COND)])
                NextOff += sizeof(CCfgData.CDATA_COND)
            CfgDlen  = CfgTagHdr.Length * 4
            DataBin  = FileData[NextOff:Offset + CfgDlen]
            DataCond = CCfgData.CDATA_COND.from_buffer(CondBin)
            if IsMergedCfg:
                PidMask  = DataCond.Value
            else:
                PidMask  = 1 << self.CfgDataPid[CfgBinFile]
            DataCond.Value = 0x00000000

            if self._Debug & CCfgData.DEBUG_FLAG_PARSE:
                print ("  TAG %03X: OFF=%04X PIDMSK=%08X LEN=%04X" %
                      (CfgTagHdr.Tag, Offset, PidMask, CfgDlen))
            if (CfgTagHdr.Flags & CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_MASK) == \
                CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_ARRAY:
                DataLen = self.ProcessCfgArray(CfgTagHdr, DataBin, PidMask, CfgBinFile)
            else:
                DataLen = len(DataBin)

            CfgItemList.append([(bytearray(CfgTagHdr), CondBin, DataBin[:DataLen]), PidMask, PidMask, IsBuiltIn])
            Offset += CfgDlen

        if (Offset != Length) or (Length % 4 != 0):
            raise Exception("Invalid CFGDATA binary blob format for file '%s' !" % CfgBinFile)

        self.CfgDataBase[CfgBinFile] = (CfgItemList, CfgBlobHeader, IsBuiltIn)


    def Merge(self, CfgItem, PidMask):
        CfgData = CfgItem[0]
        # Try to find a match on TAG and DATA
        Idx  = next((i for i, v in enumerate(self.CfgDataItems) if v[0] == CfgData), -1)
        if Idx >= 0:
            # Found one.  Change the MASK to reuse the existing data
            self.CfgDataItems[Idx][1] |= PidMask
            return

        Append = True
        CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(CfgData[0])
        if CfgTagHdr.Tag == CCfgData.CDATA_PLATFORM_ID.TAG:
            if not self.PlatformId:
                self.PlatformId = CCfgData.CDATA_PLATFORM_ID.TAG
                DataCond = CCfgData.CDATA_COND.from_buffer(CfgData[1])
                DataCond.Value = 0x00000000
                Pid = CCfgData.CDATA_PLATFORM_ID.from_buffer(CfgData[2])
                Pid.PlatformId = 0
            else:
                Append = False

        if Append:
            OrgPidMask = CfgItem[1]
            IsBuiltIn  = CfgItem[3]
            CfgTag     = 0
            NewPidMask = PidMask | OrgPidMask
            if len(CfgData[2]) > 4:
                # Try to find a match on DATA only
                Idx  = next((i for i, v in enumerate(self.CfgDataItems) if v[0][2] == CfgData[2]), -1)
                if Idx >= 0:
                    RefCfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(self.CfgDataItems[Idx][0][0])
                    CfgTag       = RefCfgTagHdr.Tag
                    NewPidMask   = self.CfgDataItems[Idx][2]
                    CfgData      = (CfgData[0], CfgData[1], '\x00')
                    IsBuiltIn    = IsBuiltIn or self.CfgDataItems[Idx][3]
            self.CfgDataItems.append ([CfgData, NewPidMask, OrgPidMask, IsBuiltIn, CfgTag])

    def Create(self, CfgOutFile, PlatformIdStr):
        self.CfgDataItems   = []
        self.PlatformId    = None

        if PlatformIdStr:
            if PlatformIdStr.startswith('0x'):
                PlatformId = int(PlatformIdStr, 16)
            else:
                PlatformId = int(PlatformIdStr)
        else:
            PlatformId = -1

        if PlatformId >= 32 and PlatformId != 0xFF:
            raise Exception("Invalid platfrom ID '%d' is specified !" % PlatformId)

        for CfgFile, (CfgItemList, CfgBlobHeader, IsBuiltIn) in list(self.CfgDataBase.items()):
            for CfgItem in CfgItemList:
                if CfgBlobHeader.Attribute & CCfgData.CDATA_BLOB_HEADER.ATTR_MERGED:
                    PidMask = 0
                else:
                    PidMask = (1 << self.CfgDataPid[CfgFile]) & 0xFFFFFFFF
                self.Merge(CfgItem, PidMask)

        # CfgGrp: (HEADER, CONDITION, DATA)
        PidMaskUpdate = []
        BinDat = bytearray()
        for CfgItem in self.CfgDataItems:
            IsBuiltIn = CfgItem[3]
            ReferTag  = CfgItem[4]
            TagHdr, CondBin, DataBin = CfgItem[0]

            CfgDataHdr  = CCfgData.CDATA_HEADER.from_buffer(TagHdr)
            if CfgDataHdr.Tag == CCfgData.CDATA_PLATFORM_ID.TAG:
                if PlatformId >=0:
                    print("Set platform ID to %d" % PlatformId)
                    DataCond = CCfgData.CDATA_COND.from_buffer(CondBin)
                    DataCond.Value = 0xFFFFFFFF
                    Pid = CCfgData.CDATA_PLATFORM_ID.from_buffer(DataBin)
                    Pid.PlatformId = PlatformId
                BinDat.extend (TagHdr + CondBin + DataBin)
                continue

            CfgDataCond = CCfgData.CDATA_COND.from_buffer(CondBin)
            CfgDataCond.Value = CfgItem[1]

            if IsBuiltIn or ReferTag > 0:
                NewPidMask = CfgItem[1]
                OldPidMask = CfgItem[2]
                if ((NewPidMask != OldPidMask) and (OldPidMask != 0)) or (ReferTag > 0):

                    # A built-in pidmask cannot be modified directly.
                    # Build a speical refer tag to reuse this item
                    if ReferTag > 0:
                        PidMask = OldPidMask
                        BasePid = NewPidMask.bit_length() - 1
                    else:
                        PidMask = NewPidMask ^ OldPidMask
                        BasePid = OldPidMask.bit_length() - 1

                    CfgTagHdr    = CCfgData.CDATA_HEADER()
                    CfgDataCond  = CCfgData.CDATA_COND()
                    CfgDataRefer = CCfgData.CDATA_REFERENCE()
                    CfgTagHdr.ConditionNum = 1
                    CfgTagHdr.Tag    = CfgDataHdr.Tag
                    CfgTagHdr.Flags  = CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_REFER
                    CfgTagHdr.Length = (sizeof(CfgDataRefer) + sizeof(CfgTagHdr) + \
                                       sizeof(CfgDataCond) * CfgTagHdr.ConditionNum + 3) // 4
                    CfgDataCond.Value = PidMask
                    CfgDataRefer.PlatformId = BasePid
                    CfgDataRefer.IsInternal = 1 if IsBuiltIn else 0
                    CfgTag           = CfgDataHdr.Tag if ReferTag == 0 else ReferTag
                    CfgDataRefer.Tag = CfgTag
                    BinDat.extend (bytearray(CfgTagHdr) + bytearray(CfgDataCond) + bytearray(CfgDataRefer))
            else:
                BinDat.extend (TagHdr + CondBin + DataBin)

        CfgdHdr = CCfgData.CDATA_BLOB_HEADER()
        CfgdHdr.Signature    = b'CFGD'
        CfgdHdr.Attribute    = CCfgData.CDATA_BLOB_HEADER.ATTR_MERGED
        CfgdHdr.HeaderLength = sizeof(CfgdHdr)
        CfgdHdr.UsedLength   = len(BinDat) + CfgdHdr.HeaderLength
        CfgdHdr.TotalLength  = CfgdHdr.UsedLength

        with open(CfgOutFile, "wb") as Fout:
            Fout.write (CfgdHdr)
            Fout.write (BinDat)


def GetCfgDataByTag (CfgData, Pid, Tag, IsInternal = False):
    Idx = 0 if IsInternal else 1
    CfgFile, (CfgItemList, CfgBlobHdr, IsBuiltIn) = list(CfgData[Idx].CfgDataBase.items())[0]

    for CfgItem in CfgItemList:
        TagHdr, CondBin, DataBin = CfgItem[0]
        CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(TagHdr)
        if CfgTagHdr.Tag != Tag:
            continue
        if (CfgTagHdr.Tag != CCfgData.CDATA_PLATFORM_ID.TAG) and (CfgItem[1] & (1 << Pid) == 0):
            continue

        if (CfgTagHdr.Flags & CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_MASK) == CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_ARRAY:
            ArrayInfo = CCfgData.CDATA_ITEM_ARRAY.from_buffer(DataBin)
            Offset     = ArrayInfo.HeaderSize
            MaskOff    = sizeof(ArrayInfo)
            MaskLen    = Offset - MaskOff
            if ArrayInfo.BasePlatformId < 0x80:
                RefPid = ArrayInfo.BasePlatformId
                TagHdr, CondBin, BaseDataBin = GetCfgDataByTag (CfgData, RefPid, Tag, True)

                CurrArrayInfo = CCfgData.CDATA_ITEM_ARRAY.from_buffer(DataBin)
                BaseArrayInfo = CCfgData.CDATA_ITEM_ARRAY.from_buffer(BaseDataBin)
                NewDataBin = bytearray (BaseDataBin)

                # Copy entries from base table
                ItemDict = {}
                ItemLen  = BaseArrayInfo.ItemSize
                for Idx1 in range (BaseArrayInfo.ItemCount):
                    Off1     = Offset + Idx1 * ItemLen
                    BaseItem = BaseDataBin[Off1 : Off1 + ItemLen]
                    ItemId   = get_bits_from_bytes (BaseItem, BaseArrayInfo.ItemIdBitOff, BaseArrayInfo.ItemIdBitLen)
                    NewItem  = NewDataBin[Off1 : Off1 + ItemLen]
                    if DataBin[MaskOff + (Idx1 >> 3)] & (1 << (Idx1 & 7)):
                        set_bits_to_bytes (NewItem, BaseArrayInfo.ItemValidBitOff, 1, 0)
                    else:
                        ItemDict[ItemId] = Idx1
                        set_bits_to_bytes (NewItem, BaseArrayInfo.ItemValidBitOff, 1, 1)
                    NewDataBin[Off1 : Off1 + ItemLen] = NewItem

                for Idx2 in range (CurrArrayInfo.ItemCount):
                    Off2 = Offset + Idx2 * ItemLen
                    CurrItem = DataBin[Off2 : Off2 + ItemLen]
                    ItemId   = get_bits_from_bytes (CurrItem, BaseArrayInfo.ItemIdBitOff, BaseArrayInfo.ItemIdBitLen)
                    Idx1 = ItemDict[ItemId]
                    Off1 = Offset + Idx1 * ItemLen
                    NewDataBin[Off1 : Off1 + ItemLen] = CurrItem
            elif ArrayInfo.BasePlatformId == 0x80:
                NewDataBin = bytearray (DataBin)

            # Zero masks and base pid
            NewDataBin[MaskOff : MaskOff + MaskLen] = b'\x00' * MaskLen
            NewArrayInfo = CCfgData.CDATA_ITEM_ARRAY.from_buffer(NewDataBin)
            NewArrayInfo.BasePlatformId = 0xFF
            DataBin = NewDataBin

        elif (CfgTagHdr.Flags & CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_MASK) == CCfgData.CDATA_HEADER.FLAG_ITEM_TYPE_REFER:
            Refer = CCfgData.CDATA_REFERENCE.from_buffer(DataBin)
            TagHdrInt, CondBinInt, DataBin = GetCfgDataByTag (CfgData, Refer.PlatformId, Refer.Tag, True)

        return  TagHdr, CondBin, DataBin

    if Idx == 1:
        # Try to find it in internal database
        return GetCfgDataByTag (CfgData, Pid, Tag, True)
    else:
        raise Exception ('Could not find TAG:0x%03X for PID:0x%02X in internal or external CFGDATA !' % Tag)


def CmdExport(Args):
    BrdNameDict = {}
    if Args.board_name_list:
      Parts = Args.board_name_list.split(',')
      for Part in Parts:
        Info = Part.split(':')
        if len(Info) == 2:
          BrdNameDict[int(Info[0],0)] = Info[1].strip()

    OutputDir     = Args.output_dir
    if not os.path.exists(OutputDir):
        os.mkdir (OutputDir)

    # Locate CFGDATA in BIOS region
    IfwiBin = bytearray (get_file_data(Args.ifwi_file))
    IfwiParser = IFWI_PARSER ()
    Ifwi = IfwiParser.parse_ifwi_binary (IfwiBin)
    Cfgs = IfwiParser.find_components(Ifwi, 'CNFG')
    if not Cfgs:
        IsBpdt  = True
        Cfgs = IfwiParser.find_components(Ifwi, 'CFGD')
    else:
        IsBpdt  = False
        PartFmt = '/RD%%d/'

    if len(Cfgs) == 0:
        print ("ERROR: Conld not find external CFGDATA !")
        return -1

    # Adjust path to point to proper boot partition
    Bp = int(Args.boot_part)
    CfgdPath = ''
    for Cfgd in Cfgs:
        CfgdPath = IfwiParser.get_component_path (Cfgd)
        if IsBpdt:
            PartStr = '/BP%d/' % Bp
        else:
            PartStr = '/RD%d/' % Bp
        if PartStr in CfgdPath:
            break

    # For non-redundant layout, just use the 1st CFGD found
    if CfgdPath == '':
        print ('INFO: No redundant boot partition found !')
        CfgdPath = Cfgs[0]

    # Locate Stage1B image
    Stage1bName = 'IBB' if IsBpdt else 'SG1B'
    Stage1bPath = '/'.join(CfgdPath.split('/')[:-1]) + '/%s' % Stage1bName
    Stage1bComp = IfwiParser.locate_component (Ifwi, Stage1bPath)
    if not Stage1bComp:
        print ('ERROR: Failed to extract external STAGE1B !')
        return -2

    # Decompress Stage1B image if required
    Stage1bBin  = IfwiBin[Stage1bComp.offset : Stage1bComp.offset + Stage1bComp.length]
    if Stage1bBin[0:2] == b'LZ':
        if Args.tool_dir == '':
            print ("ERROR: '-t' is required to specify compress tool directory !")
            return -3

        Stage1bLz = OutputDir + '/Stage1b.lz'
        Stage1bFd = OutputDir + '/Stage1b.fd'
        gen_file_from_object (Stage1bLz, Stage1bBin)
        decompress (Stage1bLz, Stage1bFd, tool_dir = Args.tool_dir)
        Stage1bBin = bytearray (get_file_data (Stage1bFd))

    # Locate and generate internal CFGDATA
    Offset = Stage1bBin.find (CFGDATA_INT_GUID)
    if Offset < 0:
        print ('ERROR: Failed to locate internal CFGDATA !')
        return -4

    Offset += 0x1C
    CfgBlobHeader = CCfgData.CDATA_BLOB_HEADER.from_buffer(Stage1bBin, Offset)
    if CfgBlobHeader.Signature != b'CFGD':
        print ('ERROR: Invalid internal CFGDATA format !')
        return -5

    CfgDataInt =  Stage1bBin[Offset : Offset + CfgBlobHeader.TotalLength]
    CfgBinIntFile = OutputDir + '/CfgDataInt.bin'
    gen_file_from_object (CfgBinIntFile, CfgDataInt)

    # Generate external CFGDATA
    CfgBinExtFile = OutputDir + '/CfgDataExt.bin'
    gen_file_from_object (CfgBinExtFile, IfwiBin[Cfgd.offset : Cfgd.offset + Cfgd.length])

    # Parse CFGDATA blobs
    CfgDataInt = CCfgData()
    CfgDataInt.Parse(CfgBinIntFile)
    CfgDataExt = CCfgData()
    CfgDataExt.Parse(CfgBinExtFile)

    # Generate CfgDataDef blob
    CfgFile, (CfgIntItemList, CfgIntBlobHdr, IsBuiltIn) = list(CfgDataInt.CfgDataBase.items())[0]
    CfgDef  = bytearray(CfgIntBlobHdr)
    TagDict = collections.OrderedDict()
    for Idx, CfgIntItem in enumerate(CfgIntItemList):
        TagHdr, CondBin, DataBin = CfgIntItem[0]
        CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(TagHdr)
        if CfgTagHdr.Tag in TagDict.keys():
            break
        else:
            TagDict[CfgTagHdr.Tag] = Idx
            CfgDef.extend(TagHdr + CondBin + DataBin)

    CfgDefLen = len(CfgDef)
    CfgDefBlobHdr = CCfgData.CDATA_BLOB_HEADER.from_buffer(bytearray(CfgIntBlobHdr))
    CfgDefBlobHdr.UsedLength   = CfgDefLen
    CfgDefBlobHdr.TotalLength  = CfgDefLen
    CfgDefBlobHdr.Attribute    = 0

    # Collect available platform ID
    PidMask = 0
    CfgFile, (CfgExtItemList, CfgExtBlobHdr, IsBuiltIn) = list(CfgDataExt.CfgDataBase.items())[0]
    for CfgItem in CfgExtItemList:
        PidMask |= CfgItem[1]

    # Export board specific external CFGDATA
    for Pid in range(32):
        if (1 << Pid) & PidMask == 0:
            continue

        print ('Exporting external CFGDATA for PlatformID = 0x%02X' % Pid)
        CfgDataBrd = bytearray (CfgDefBlobHdr)
        CfgData    = [CfgDataInt, CfgDataExt]
        for Tag in TagDict.keys():
            TagHdr, CondBin, DataBin = GetCfgDataByTag (CfgData, Pid, Tag)
            CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(TagHdr)
            CondBin = b'\x00' * sizeof(CCfgData.CDATA_COND)
            TagHdr  = bytearray (CfgIntItemList[TagDict[Tag]][0][0])
            NewData = bytearray (DataBin)
            if  CfgTagHdr.Tag == CCfgData.CDATA_PLATFORM_ID.TAG:
                PidCfg = CCfgData.CDATA_PLATFORM_ID.from_buffer(NewData)
                PidCfg.PlatformId = Pid
            CfgDataBrd.extend (TagHdr + CondBin + NewData)

        if Pid in BrdNameDict.keys():
            Ext = BrdNameDict[Pid]
        else:
            Ext = 'CfgDataExt_%02X' % Pid
        gen_file_from_object (OutputDir + '/%s.bin' % Ext, CfgDataBrd)

def CmdView(Args):
    CfgData = CCfgData()
    for CfgBinFile in Args.cfg_in_file:
        CfgData.Parse(CfgBinFile)
    Flag = CCfgData.DUMP_FLAG_INPUT
    if Args.dbg_lvl > 2:
        Flag |= CCfgData.DUMP_FLAG_VERBOSE
    CfgData.Dump(Flag)


def CmdMerge(Args):
    CfgData = CCfgData()
    for CfgBinFile in Args.cfg_in_file:
        CfgData.Parse(CfgBinFile)

    if Args.dbg_lvl > 0:
        Flag = CCfgData.DUMP_FLAG_INPUT
        if Args.dbg_lvl > 2:
            Flag |= CCfgData.DUMP_FLAG_VERBOSE
        CfgData.Dump (Flag)

    CfgData.Create(Args.cfg_out_file, Args.platform_id)
    if Args.dbg_lvl > 1:
        Flag = CCfgData.DUMP_FLAG_OUTPUT
        if Args.dbg_lvl > 2:
            Flag |= CCfgData.DUMP_FLAG_VERBOSE
        CfgData.Dump (Flag)

    if Args.dbg_lvl > 0:
        CfgData = CCfgData()
        CfgData.Parse(Args.cfg_out_file)
        Flag = CCfgData.DUMP_FLAG_INPUT
        if Args.dbg_lvl > 2:
            Flag |= CCfgData.DUMP_FLAG_VERBOSE
        CfgData.Dump(Flag, False)

    print ("%d config binary files were merged successfully!" % len(Args.cfg_in_file))

def CmdSign(Args):
    Fd       = open (Args.cfg_in_file, 'rb')
    FileData = bytearray (Fd.read ())
    Fd.close ()

    CfgBlobHeader = CCfgData.CDATA_BLOB_HEADER.from_buffer(FileData)
    if CfgBlobHeader.Signature != b'CFGD':
        raise Exception("Invalid config binary file '%s' !" % CfgDataFile)
    CfgBlobHeader.Attribute |= CCfgData.CDATA_BLOB_HEADER.ATTR_SIGNED

    CfgBlobHeader.Svn = Args.svn

    TmpFile = Args.cfg_in_file + '.tmp'
    Fd      = open (TmpFile, 'wb')
    Fd.write (FileData)
    Fd.close ()

    if Args.hash_alg == 'AUTO':
        Args.hash_alg = adjust_hash_type(Args.cfg_pri_key)

    rsa_sign_file (Args.cfg_pri_key, None, Args.hash_alg, Args.sign_scheme, TmpFile, Args.cfg_out_file, True, True)
    if os.path.exists(TmpFile):
      os.remove(TmpFile)

    print ("Config file was signed successfully!")

def CmdExtract(Args):
    Found = False
    TagNo = int(Args.cfg_tag, 0)

    CfgData = CCfgData()
    for CfgBinFile in Args.cfg_in_file:
        CfgData.Parse(CfgBinFile)

    for CfgFile, (CfgItemList, CfgBlobHeader, IsBuiltIn) in list(CfgData.CfgDataBase.items()):
        for CfgItem in CfgItemList:
            TagHdr, CondBin, DataBin = CfgItem[0]
            CfgTagHdr = CCfgData.CDATA_HEADER.from_buffer(TagHdr)
            if CfgTagHdr.Tag == TagNo:
                Found = True
                break
        if Found:
            break

    if Found:
        BinDat = bytearray()
        BinDat.extend (TagHdr + CondBin + DataBin)
        print_bytes (BinDat)
        if Args.cfg_out_file != None:
            with open(Args.cfg_out_file, "wb") as Fout:
                Fout.write (BinDat)
            print ("Config data (Tag=0x%X) was saved to a file - %s" % (TagNo, Args.cfg_out_file))
    else:
        print ("Config data (Tag=0x%X) was not found!" % TagNo)

def CmdReplace(Args):
    IfwiParser = IFWI_PARSER ()

    CfgFile = Args.cfg_in_file
    if not os.path.exists(CfgFile):
        raise Exception("Cannot find CFGDATA binary file '%s'" % CfgFile)

    IfwiImgIn = Args.ifwi_in_file
    if not os.path.exists(IfwiImgIn):
        raise Exception("Cannot find IFWI image file '%s'" % IfwiImgIn)

    # Get cfg binary
    Fh = open(CfgFile, 'rb')
    CfgBins = bytearray(Fh.read())
    Fh.close()

    CfgHdr = CCfgData.CDATA_BLOB_HEADER.from_buffer(CfgBins)
    if CfgHdr.Signature != b'CFGD':
        raise Exception("Invalid CFGDATA image file '%s'" % CfgFile)

    if not CfgHdr.Attribute & CCfgData.CDATA_BLOB_HEADER.ATTR_MERGED:
        raise Exception("CFGDATA image file '%s' is not merged yet!" % CfgFile)

    # Get flash image
    Fh = open(IfwiImgIn, 'rb')
    BiosBins = bytearray(Fh.read())
    Fh.close()

    CfgLen = len(CfgBins)

    # Check to see if the IFWI is
    if Args.pdr:
        # CFGDATA in PDR region
        RegionName = 'pdr'
    else:
        # Assume CFGDATA in BIOS region
        RegionName = 'bios'

    CompList = []
    StartOff = 0
    EndOff   = 0
    if IfwiParser.is_ifwi_image(BiosBins):
        #Check if it has BPDT
        SpiDesc = SPI_DESCRIPTOR.from_buffer(BiosBins, 0)
        Comp = IfwiParser.find_ifwi_region(SpiDesc, RegionName)
        if len(Comp) < 1:
            raise Exception("Cannot not find CFGDATA in SPI flash region '%s' !" % RegionName)

    if not CompList:
        if RegionName == 'bios':
            Ifwi = IfwiParser.parse_ifwi_binary (BiosBins)
            cfgs = IfwiParser.find_components(Ifwi, 'CNFG')
            if not cfgs:
                cfgs = IfwiParser.find_components(Ifwi, 'CFGD')
            for cfgd in cfgs:
                print (IfwiParser.get_component_path (cfgd))
                CompList.append((cfgd.offset, cfgd.length))
        else:
            # For PDR region, always assume CFGDATA starts from offset 0
            CfgBlobHeader = CCfgData.CDATA_BLOB_HEADER.from_buffer(BiosBins[StartOff:])
            if CfgBlobHeader.Signature != b'CFGD':
                raise Exception("Cannot not find CFGDATA in SPI flash PDR region!")
            if CfgBlobHeader.TotalLength > EndOff - StartOff:
                raise Exception("Invalid CFGDATA length in PDR region ")
            CompList = [(StartOff, EndOff - StartOff)]

    for Offset, Size in CompList:
        if Offset < 0 or Offset >= len(BiosBins):
            raise Exception("Invalid CFGDATA region offset 0x%X!" % Offset)

        if CfgLen > Size:
            raise Exception("CfgData file size 0x%X shall not be greater than CFGDATA region size 0x%X !" % (CfgLen, Size))

        print("Patching CFGDATA region at image offset 0x%X (len: 0x%X)!" % (Offset, Size))
        BiosBins[Offset:Offset + CfgLen] = CfgBins

    IfwiImgOut = Args.ifwi_out_file
    if not IfwiImgOut:
        IfwiImgOut = IfwiImgIn

    Fh = open(IfwiImgOut, 'wb')
    Fh.write(BiosBins)
    Fh.close()

    if len(CompList):
        print("%d CFGDATA region has been patched successfully !" % len(
            CompList))
    else:
        print("No CFGDATA region has been patched!")

    return

def Main():
    #
    # Parse the options and args
    #
    ArgParser = argparse.ArgumentParser()
    SubParser = ArgParser.add_subparsers(help='command')

    ViewParser = SubParser.add_parser('view', help='display config data')
    ViewParser.add_argument('cfg_in_file',
                            type=str,
                            nargs='+',
                            help='Configuration input binary file')
    ViewParser.add_argument('-v',   dest='dbg_lvl', type=int, help='Display verbose info:: 0,1,2.Default=0', default = 0)
    ViewParser.set_defaults(func=CmdView)

    MergeParser = SubParser.add_parser('merge', help='merge config data')
    MergeParser.add_argument('cfg_in_file',
                            type=str,
                            nargs='+',
                            help='Configuration input binary file(s) - Input files can be: xxx.rom generated from BCT, xxx.bin generated from SBL source. xxx.bin* - Star represents internal cfg data bin generated from source to be added to merged cfg_out_file')
    MergeParser.add_argument('-o', dest='cfg_out_file', type=str, help='Specify Configuration output binary file name to be generated', required=True)
    MergeParser.add_argument('-p', dest='platform_id', type=str, help='Force a given platform ID to be used', default = '')
    MergeParser.add_argument('-v',   dest='dbg_lvl', type=int, help='Display verbose info: 0,1,2.Default=0',  default = 0)
    MergeParser.set_defaults(func=CmdMerge)

    SignParser = SubParser.add_parser('sign', help='sign external config data')
    SignParser.add_argument('cfg_in_file',
                            type=str,
                            help='Configuration binary file')
    SignParser.add_argument('-o', dest='cfg_out_file', type=str, help='Signed configuration output binary file name to be generated', required=True)
    SignParser.add_argument('-k', dest='cfg_pri_key', type=str, help='Key Id or Private key file (PEM format) used to sign configuration data', required=True)
    SignParser.add_argument('-a', dest='hash_alg', type=str, choices=['SHA2_256', 'SHA2_384', 'AUTO'], help='Hash Type for signing. For AUTO hash type will be choosen based on key length',  default = 'AUTO')
    SignParser.add_argument('-s', dest='sign_scheme', type=str, choices=['RSA_PKCS1', 'RSA_PSS'], help='Signing Scheme',   default = 'RSA_PSS')
    SignParser.add_argument('-svn', dest='svn', type=int, help='Security version number for Config Data', default = 0)
    SignParser.set_defaults(func=CmdSign)

    ExtractParser = SubParser.add_parser('extract', help='extract a single config data to a file')
    ExtractParser.add_argument('cfg_in_file',
                            type=str,
                            nargs='+',
                            help='Configuration input binary file')
    ExtractParser.add_argument('-t', dest='cfg_tag', type=str, help='Specify tag value to be extracted', required=True)
    ExtractParser.add_argument('-o', dest='cfg_out_file', type=str, help='Specify Configuration output binary file name to be generated')
    ExtractParser.set_defaults(func=CmdExtract)

    ReplaceParser = SubParser.add_parser('replace', help='Replace config data blob within a IFWI')
    ReplaceParser.add_argument('cfg_in_file',
                            type=str,
                            help='Configuration input binary file')

    ReplaceParser.add_argument('-i', dest='ifwi_in_file',  type=str,  help='Specify IFWI input binary file', required=True)
    ReplaceParser.add_argument('-o', dest='ifwi_out_file', type=str,  help='Specify IFWI output binary file', default='')
    ReplaceParser.add_argument('-p', dest='pdr', action='store_true', help='Replace CFGDATA in PDR region', default=False)
    ReplaceParser.set_defaults(func=CmdReplace)

    ExportParser = SubParser.add_parser('export', help='Export board external CFGDATA from BIOS or IFWI file')
    ExportParser.add_argument('-i', dest='ifwi_file',  type=str,  help='Specify BIOS or IFWI input binary file', required=True)
    ExportParser.add_argument('-b', dest='boot_part', choices=['0', '1'], help='Specify which boot partition to export CFGDATA from', default = '0')
    ExportParser.add_argument('-o', dest='output_dir', type=str,  help='Specify output directory', default='.')
    ExportParser.add_argument('-t', dest='tool_dir', type=str,  help='Specify compress tool directory', default='')
    ExportParser.add_argument('-n', dest='board_name_list', type=str,  help='Specify board name to id map list', default='')
    ExportParser.set_defaults(func=CmdExport)

    Args = ArgParser.parse_args()
    return Args.func(Args)

if __name__ == '__main__':
    sys.exit(Main())
