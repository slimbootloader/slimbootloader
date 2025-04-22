## @ GenCapsuleFirmware.py
#
# Copyright (c) 2017-2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import argparse
import subprocess
import sys
import re
import struct
import uuid
import binascii
from ctypes import *

sys.dont_write_bytecode = True
from CommonUtility import *

class FmpCapsuleImageHeaderClass(object):
    # typedef struct {
    #   UINT32   Version;
    #
    #   ///
    #   /// Used to identify device firmware targeted by this update. This guid is matched by
    #   /// system firmware against ImageTypeId field within a EFI_FIRMWARE_IMAGE_DESCRIPTOR
    #   ///
    #   EFI_GUID UpdateImageTypeId;
    #
    #   ///
    #   /// Passed as ImageIndex in call to EFI_FIRMWARE_MANAGEMENT_PROTOCOL.SetImage ()
    #   ///
    #   UINT8    UpdateImageIndex;
    #   UINT8    reserved_bytes[3];
    #
    #   ///:
    #   /// Size of the binary update image which immediately follows this structure
    #   ///
    #   UINT32   UpdateImageSize;
    #
    #   ///
    #   /// Size of the VendorCode bytes which optionally immediately follow binary update image in the capsule
    #   ///
    #   UINT32   UpdateVendorCodeSize;
    #
    #   ///
    #   /// The HardwareInstance to target with this update. If value is zero it means match all
    #   /// HardwareInstances. This field allows update software to target only a single device in
    #   /// cases where there are more than one device with the same ImageTypeId GUID.
    #   /// This header is outside the signed data of the Authentication Info structure and
    #   /// therefore can be modified without changing the Auth data.
    #   ///
    #   UINT64   UpdateHardwareInstance;
    # } EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER;
    #
    #  #define EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION 0x00000002

    _StructFormat = '<I16sB3BIIQ'
    _StructSize = struct.calcsize(_StructFormat)

    EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION = 0x00000002

    def __init__(self):
        self._Valid = False
        self.Version = self.EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION
        self.UpdateImageTypeId = uuid.UUID(
            '00000000-0000-0000-0000-000000000000')
        self.UpdateImageIndex = 0
        self.UpdateImageSize = 0
        self.UpdateVendorCodeSize = 0
        self.UpdateHardwareInstance = 0x0000000000000000
        self.Payload = b''
        self.VendorCodeBytes = b''

    def Encode(self):
        self.UpdateImageSize = len(self.Payload)
        self.UpdateVendorCodeSize = len(self.VendorCodeBytes)
        FmpCapsuleImageHeader = struct.pack(
            self._StructFormat, self.Version, self.UpdateImageTypeId.bytes_le,
            self.UpdateImageIndex, 0, 0, 0, self.UpdateImageSize,
            self.UpdateVendorCodeSize, self.UpdateHardwareInstance)
        self._Valid = True
        return FmpCapsuleImageHeader + self.Payload + (self.VendorCodeBytes).encode()

    def Decode(self, Buffer):
        if len(Buffer) < self._StructSize:
            raise ValueError
        (Version, UpdateImageTypeId, UpdateImageIndex, r0, r1, r2,
         UpdateImageSize, UpdateVendorCodeSize,
         UpdateHardwareInstance) = struct.unpack(self._StructFormat,
                                                 Buffer[0:self._StructSize])

        if Version < self.EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION:
            raise ValueError
        if UpdateImageIndex < 1:
            raise ValueError
        if UpdateImageSize + UpdateVendorCodeSize != len(Buffer[
                self._StructSize:]):
            raise ValueError

        self.Version = Version
        self.UpdateImageTypeId = uuid.UUID(bytes_le=UpdateImageTypeId)
        self.UpdateImageIndex = UpdateImageIndex
        self.UpdateImageSize = UpdateImageSize
        self.UpdateVendorCodeSize = UpdateVendorCodeSize
        self.UpdateHardwareInstance = UpdateHardwareInstance
        self.Payload = Buffer[self._StructSize:self._StructSize +
                              UpdateImageSize]
        self.VendorCodeBytes = Buffer[self._StructSize + UpdateImageSize:]
        self._Valid = True
        return Buffer[self._StructSize:]

    def DumpInfo(self, padding=''):
        if not self._Valid:
            raise ValueError

        print(
            padding +
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER.Version                = {Version:08X}'.format(
                Version=self.Version))
        print(
            padding +
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER.UpdateImageTypeId      = {UpdateImageTypeId}'.format(
                UpdateImageTypeId=str(self.UpdateImageTypeId).upper()))
        print(
            padding +
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER.UpdateImageIndex       = {UpdateImageIndex:08X}'.format(
                UpdateImageIndex=self.UpdateImageIndex))
        print(
            padding +
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER.UpdateImageSize        = {UpdateImageSize:08X}'.format(
                UpdateImageSize=self.UpdateImageSize))
        print(
            padding +
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER.UpdateVendorCodeSize   = {UpdateVendorCodeSize:08X}'.format(
                UpdateVendorCodeSize=self.UpdateVendorCodeSize))
        print(
            padding +
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER.UpdateHardwareInstance = {UpdateHardwareInstance:016X}'.format(
                UpdateHardwareInstance=self.UpdateHardwareInstance))
        print(
            padding +
            'sizeof (Payload)                                                    = {Size:08X}'.format(
                Size=len(self.Payload)))
        print(
            padding +
            'sizeof (VendorCodeBytes)                                            = {Size:08X}'.format(
                Size=len(self.VendorCodeBytes)))


class FmpCapsuleHeaderClass(object):
    # typedef struct {
    #   UINT32 Version;
    #
    #   ///
    #   /// The number of drivers included in the capsule and the number of corresponding
    #   /// offsets stored in ItemOffsetList array.
    #   ///
    #   UINT16 EmbeddedDriverCount;
    #
    #   ///
    #   /// The number of payload items included in the capsule and the number of
    #   /// corresponding offsets stored in the ItemOffsetList array.
    #   ///
    #   UINT16 PayloadItemCount;
    #
    #   ///
    #   /// Variable length array of dimension [EmbeddedDriverCount + PayloadItemCount]
    #   /// containing offsets of each of the drivers and payload items contained within the capsule
    #   ///
    #   // UINT64 ItemOffsetList[];
    # } EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER;
    #
    #  #define EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION       0x00000001
    _StructFormat = '<IHH'
    _StructSize = struct.calcsize(_StructFormat)

    _ItemOffsetFormat = '<Q'
    _ItemOffsetSize = struct.calcsize(_ItemOffsetFormat)

    EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION = 0x00000001

    def __init__(self):
        self._Valid = False
        self.Version = self.EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION
        self.EmbeddedDriverCount = 0
        self.PayloadItemCount = 0
        self._ItemOffsetList = []
        self._EmbeddedDriverList = []
        self._PayloadList = []
        self._FmpCapsuleImageHeaderList = []

    def AddEmbeddedDriver(self, EmbeddedDriver):
        self._EmbeddedDriverList.append(EmbeddedDriver)

    def GetEmbeddedDriver(self, Index):
        if Index > len(self._EmbeddedDriverList):
            raise ValueError
        return self._EmbeddedDriverList[Index]

    def GetEmbeddedDriverCount(self):
        return len(self._EmbeddedDriverList)

    def GetPayload(self, Index):
        return self._PayloadList[Index]

    def GetPayloadCount(self):
        return len(self._PayloadList)

    def AddPayload(self,
                   UpdateImageTypeId,
                   Payload=b'',
                   VendorCodeBytes=b'',
                   HardwareInstance=0):
        self._PayloadList.append((UpdateImageTypeId, Payload, VendorCodeBytes,
                                  HardwareInstance))

    def GetFmpCapsuleImageHeader(self, Index):
        if Index >= len(self._FmpCapsuleImageHeaderList):
            raise ValueError
        return self._FmpCapsuleImageHeaderList[Index]

    def Encode(self):
        self.EmbeddedDriverCount = len(self._EmbeddedDriverList)
        self.PayloadItemCount = len(self._PayloadList)

        FmpCapsuleHeader = struct.pack(self._StructFormat, self.Version,
                                       self.EmbeddedDriverCount,
                                       self.PayloadItemCount)

        FmpCapsuleData = b''
        Offset = self._StructSize + (
            self.EmbeddedDriverCount +
            self.PayloadItemCount) * self._ItemOffsetSize
        for EmbeddedDriver in self._EmbeddedDriverList:
            FmpCapsuleData = FmpCapsuleData + EmbeddedDriver
            self._ItemOffsetList.append(Offset)
            Offset = Offset + len(EmbeddedDriver)
        Index = 1
        for (UpdateImageTypeId, Payload, VendorCodeBytes,
             HardwareInstance) in self._PayloadList:
            FmpCapsuleImageHeader = FmpCapsuleImageHeaderClass()
            FmpCapsuleImageHeader.UpdateImageTypeId = UpdateImageTypeId
            FmpCapsuleImageHeader.UpdateImageIndex = Index
            FmpCapsuleImageHeader.Payload = Payload
            FmpCapsuleImageHeader.VendorCodeBytes = VendorCodeBytes
            FmpCapsuleImageHeader.UpdateHardwareInstance = HardwareInstance
            FmpCapsuleImage = FmpCapsuleImageHeader.Encode()

            # for BIOS payload, adjust the starting offset to be 8-byte aligned with 0xFF paddings
            if str(UpdateImageTypeId) == UpdateGuidDict['BIOS'].lower():
                if Offset & 0x7:
                    Padding8Num = 8 - Offset & 0x7
                    print('Adjusted the starting offset of BIOS payload with %s paddings' %Padding8Num)
                    Padding = bytearray(b'\xFF') * Padding8Num
                    FmpCapsuleData += Padding
                    Offset += Padding8Num

            FmpCapsuleData = FmpCapsuleData + FmpCapsuleImage

            self._ItemOffsetList.append(Offset)
            self._FmpCapsuleImageHeaderList.append(FmpCapsuleImageHeader)

            Offset = Offset + len(FmpCapsuleImage)
            Index = Index + 1

        for Offset in self._ItemOffsetList:
            FmpCapsuleHeader = FmpCapsuleHeader + struct.pack(
                self._ItemOffsetFormat, Offset)

        self._Valid = True
        return FmpCapsuleHeader + FmpCapsuleData

    def Decode(self, Buffer):
        if len(Buffer) < self._StructSize:
            raise ValueError
        (Version, EmbeddedDriverCount, PayloadItemCount) = struct.unpack(
            self._StructFormat, Buffer[0:self._StructSize])
        if Version < self.EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION:
            raise ValueError

        self.Version = Version
        self.EmbeddedDriverCount = EmbeddedDriverCount
        self.PayloadItemCount = PayloadItemCount
        self._ItemOffsetList = []
        self._EmbeddedDriverList = []
        self._PayloadList = []
        self._FmpCapsuleImageHeaderList = []

        #
        # Parse the ItemOffsetList values
        #
        Offset = self._StructSize
        for Index in range(0, EmbeddedDriverCount + PayloadItemCount):
            ItemOffset = struct.unpack(
                self._ItemOffsetFormat,
                Buffer[Offset:Offset + self._ItemOffsetSize])[0]
            if ItemOffset >= len(Buffer):
                raise ValueError
            self._ItemOffsetList.append(ItemOffset)
            Offset = Offset + self._ItemOffsetSize
        Result = Buffer[Offset:]

        #
        # Parse the EmbeddedDrivers
        #
        for Index in range(0, EmbeddedDriverCount):
            Offset = self._ItemOffsetList[Index]
            if Index < (len(self._ItemOffsetList) - 1):
                Length = self._ItemOffsetList[Index + 1] - Offset
            else:
                Length = len(Buffer) - Offset
            self.AddEmbeddedDriver(Buffer[Offset:Offset + Length])

        #
        # Parse the Payloads that are FMP Capsule Images
        #
        for Index in range(EmbeddedDriverCount,
                           EmbeddedDriverCount + PayloadItemCount):
            Offset = self._ItemOffsetList[Index]
            if Index < (len(self._ItemOffsetList) - 1):
                Length = self._ItemOffsetList[Index + 1] - Offset
            else:
                Length = len(Buffer) - Offset
            FmpCapsuleImageHeader = FmpCapsuleImageHeaderClass()
            FmpCapsuleImageHeader.Decode(Buffer[Offset:Offset + Length])
            self.AddPayload(FmpCapsuleImageHeader.UpdateImageTypeId,
                            FmpCapsuleImageHeader.Payload,
                            FmpCapsuleImageHeader.VendorCodeBytes)
            self._FmpCapsuleImageHeaderList.append(FmpCapsuleImageHeader)

        self._Valid = True
        return Result

    def DumpInfo(self):
        if not self._Valid:
            raise ValueError
        print(
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER.Version             = {Version:08X}'.format(
                Version=self.Version))
        print(
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER.EmbeddedDriverCount = {EmbeddedDriverCount:08X}'.format(
                EmbeddedDriverCount=self.EmbeddedDriverCount))
        print(
            'EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER.PayloadItemCount    = {PayloadItemCount:08X}'.format(
                PayloadItemCount=self.PayloadItemCount))
        print('EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER.ItemOffsetList      = ')
        for Offset in self._ItemOffsetList:
            print('  {Offset:016X}'.format(Offset=Offset))
        print('')
        for FmpCapsuleImageHeader in self._FmpCapsuleImageHeaderList:
            FmpCapsuleImageHeader.DumpInfo('  ')
            print('')

def UnicodeStrToHexString(Str):
    return (binascii.hexlify(Str.encode('utf-8'))).decode()

def _SIGNATURE_32(A, B, C, D):
    return struct.unpack('=I', bytearray(A + B + C + D, 'ascii'))[0]


def _SIGNATURE_32_TO_STRING(Signature):
    return struct.pack("<I", Signature).decode()


class FmpPayloadHeaderClass(object):
    def __init__(self):
        self._Valid = False
        self.Payload = b''

    def Encode(self):
        self._Valid = True
        return self.Payload

    def Decode(self, Buffer):
        self.Payload = Buffer[0:]
        self._Valid = True
        return self.Payload

    def DumpInfo(self):
        if not self._Valid:
            raise ValueError
        print(
            'FMP_PAYLOAD_HEADER.Signature              = {Signature:08X} ({SignatureString})'.format(
                Signature=self.Signature,
                SignatureString=_SIGNATURE_32_TO_STRING(self.Signature)))
        print(
            'FMP_PAYLOAD_HEADER.HeaderSize             = {HeaderSize:08X}'.format(
                HeaderSize=self.HeaderSize))
        print(
            'FMP_PAYLOAD_HEADER.FwVersion              = {FwVersion:08X}'.format(
                FwVersion=self.FwVersion))
        print(
            'FMP_PAYLOAD_HEADER.LowestSupportedVersion = {LowestSupportedVersion:08X}'.format(
                LowestSupportedVersion=self.LowestSupportedVersion))
        print('sizeof (Payload)                          = {Size:08X}'.format(
            Size=len(self.Payload)))

#
# GUID for firmware update image file
#
FIRMWARE_UPDATE_IMAGE_FILE_GUID = uuid.UUID('{1A3EAE58-B580-4fef-ACA3-A16D9E00DF5F}')
#0x1a3eae58, 0xb580, 0x4fef, 0xac, 0xa3, 0xa1, 0x6d, 0x9e, 0x0, 0xdf, 0x5f);

class FirmwareUpdateHeader(Structure):

  CAPSULE_FLAG_FORCE_BIOS_UPDATE = 0x80000000

  _fields_ = [
    ('FileGuid',           ARRAY(c_uint8, 16)),
    ('HeaderSize',         c_uint32),
    ('FirmwreVersion',     c_uint32),
    ('CapsuleFlags',       c_uint32),
    ('PubKeyOffset',       c_uint32),
    ('PubKeySize',         c_uint32),
    ('ImageOffset',        c_uint32),
    ('ImageSize',          c_uint32),
    ('SignatureOffset',    c_uint32),
    ('SignatureSize',      c_uint32),
    ('Reserved',           ARRAY(c_uint32, 3))
  ]


def SignImage(RawData, OutFile, HashType, SignScheme, PrivKey, ForceBiosUpdate):

    #
    # Generate the new image layout
    # 1. write firmware update header
    #
    unsigned_image = bytearray(sizeof(FirmwareUpdateHeader))
    header = FirmwareUpdateHeader.from_buffer(unsigned_image, 0)

    file_size = len(RawData)

    key_type = get_key_type (PrivKey)
    if key_type ==  'RSA2048':
        key_size = 256
    elif key_type ==  'RSA3072':
        key_size = 384

    if HashType == 'AUTO':
        HashType = adjust_hash_type(PrivKey)

    header.FileGuid         = (c_ubyte *16).from_buffer_copy(FIRMWARE_UPDATE_IMAGE_FILE_GUID.bytes_le)
    header.HeaderSize       = sizeof(FirmwareUpdateHeader)
    header.FirmwreVersion   = 1
    header.CapsuleFlags     = 0
    if ForceBiosUpdate:
        header.CapsuleFlags  |= FirmwareUpdateHeader.CAPSULE_FLAG_FORCE_BIOS_UPDATE
    header.ImageOffset      = header.HeaderSize
    header.ImageSize        = file_size
    header.SignatureOffset  = header.ImageOffset + header.ImageSize
    header.SignatureSize    = sizeof(SIGNATURE_HDR) + key_size
    header.PubKeyOffset     = header.SignatureOffset + header.SignatureSize

    pubkey_file = 'fwu_public_key.bin'

    gen_pub_key(PrivKey, pubkey_file)

    header.PubKeySize = os.path.getsize(pubkey_file)

    fwupdate_bin_file = 'fwupdate_unsigned.bin'
    open(fwupdate_bin_file, 'wb').write(unsigned_image + RawData)

    rsa_sign_file(PrivKey, pubkey_file, HashType, SignScheme, fwupdate_bin_file, OutFile, True, True )

    os.remove(pubkey_file)
    os.remove(fwupdate_bin_file)


def main():

    # The types of updates and their associated GUIDs
    global UpdateGuidDict
    UpdateGuidDict = {
        'BIOS': '605C6813-C2C7-4242-9C27-50A4C363DBA4',  # SBL/BIOS Region
        'MISC': '66030B7A-47D1-4958-B73D-00B44B9DD4B6',  # SBL Component
        'CSME': '43AEF186-0CA5-4230-B1BD-193FB4627201',  # ME/CSME Region
        'CSMD': '4A467997-A909-4678-910C-E0FE1C9056EA',  # CSME Update Driver
        'CMDI': '9034cab1-4d19-4856-a3cd-f074263c4a4d',  # CSME Command Request
    }

    def BuildGuid(Argument):
        try:
            Value = uuid.UUID(Argument)
        except:
            Message = '{Argument} is not a valid registry format GUID value.'.format(
                Argument=Argument)
            raise argparse.ArgumentTypeError(Message)
        return Value

    #
    # Create command line argument parser object
    #

    parser = argparse.ArgumentParser()

    parser.add_argument('-p',  '--payload', nargs=2, action='append', type=str, required=True, help='Specify payload information including GUID, FileName')
    parser.add_argument('-k',  '--priv_key', dest='PrivKey', type=str, required=True, help='Key Id or Private RSA 2048/RSA3072 key in PEM format to sign image')
    parser.add_argument('-a',  '--alg_hash', dest='HashType', type=str, choices=['SHA2_256', 'SHA2_384', 'AUTO'], default='AUTO', help='Hash type for signing. For AUTO hash type will be choosen based on key length')
    parser.add_argument('-s',  '--sign_scheme', dest='SignScheme', type=str, choices=['RSA_PKCS1', 'RSA_PSS'], default='RSA_PSS', help='Signing Scheme types')
    parser.add_argument('-o',  '--output', dest='NewImage', type=str, required=True, help='Output file for signed image')
    parser.add_argument("-v",  "--verbose", dest='Verbose', action="store_true", help= "Turn on verbose output with informational messages printed, including capsule headers and warning messages.")
    parser.add_argument("-f",  "--force_bios_update", dest='ForceBiosUpdate', action="store_true", help= "Force update whole BIOS region in a single shot.")

    #
    # Parse command line arguments
    #
    args = parser.parse_args()

    #
    # Error out if components overlap
    #
    PldSigs = {Pld[0] for Pld in args.payload}
    NonSblCompSigs = {Sig for Sig in UpdateGuidDict.keys()}
    SblCompInPldSigs = PldSigs - NonSblCompSigs
    if 'BIOS' in PldSigs and SblCompInPldSigs:
        raise Exception ('SBL/BIOS region given with SBL/BIOS region component')

    FmpCapsuleHeader = FmpCapsuleHeaderClass()
    for PldSig, PldFile in args.payload:

        FmpPayloadHeader = FmpPayloadHeaderClass()
        Buffer = open(PldFile, 'rb').read()
        Result = Buffer
        FmpPayloadHeader.Payload = Result
        Result = FmpPayloadHeader.Encode()

        if PldSig in UpdateGuidDict:
            HardwareInstance = '00000000' + UnicodeStrToHexString(PldSig[::-1])
            Guid = UpdateGuidDict[PldSig]
        else:
            CompName = PldSig.upper()

            if ":" in CompName:
                items = CompName.split(":")
                item_cnt = len(items)
                if item_cnt > 2:
                    raise Exception ('more than 2 strings in component descriptor')

                if len(items[0]) != 4 or len(items[1]) != 4:
                    raise Exception ('%s is not a  4 character string' % CompName)

                ContainerCompName = UnicodeStrToHexString(items[0][::-1])
                SblComponentName = UnicodeStrToHexString(items[1][::-1])
            else:
                if len(CompName) != 4:
                    raise Exception ('%s is not a  4 character string' % CompName)
                ContainerCompName = '00000000'
                SblComponentName = UnicodeStrToHexString(CompName[::-1])
            HardwareInstance = ContainerCompName + SblComponentName
            Guid = UpdateGuidDict['MISC']
        HardwareInstance = int (HardwareInstance, 16)
        Guid = BuildGuid(Guid)
        FmpCapsuleHeader.AddPayload(Guid, Result, '', HardwareInstance)

    if args.ForceBiosUpdate:
        if FmpCapsuleHeader.GetPayloadCount() != 1  or  FmpCapsuleHeader.GetEmbeddedDriverCount() != 0:
            raise Exception ("When '-f' flag is enabled, only one component is supported in capsule !")
        Payload = FmpCapsuleHeader.GetPayload(0)
        if str(Payload[0]) != UpdateGuidDict['BIOS'].lower():
            raise Exception ("When '-f' flag is enabled, only BIOS component is supported in capsule !")
        if len(Payload[1]) & 0xFFF:
            raise Exception ("BIOS component size is not 4KB aligned !")

    Result = FmpCapsuleHeader.Encode()

    #
    # Write binary output file
    #
    if args.Verbose:
        FmpCapsuleHeader.DumpInfo()

    #
    # Create final capsule
    #
    SignImage(Result, args.NewImage, args.HashType, args.SignScheme, args.PrivKey, args.ForceBiosUpdate)
    print('Success')

    #
    # Warn the user for "-f" option
    #
    if args.ForceBiosUpdate:
       print ("\nWAINING: ")
       print (  "  It might be risky to force BIOS region update since any firmware update failure might\n"
                "  not be recoverable and cause system in a non-bootable state. If it occurs, external \n"
                "  flash programmer has to be used to reprogram the full IFWI image. This feature is only\n"
                "  for development purpose, please use it with extreme caution !!!\n")



if __name__ == '__main__':
    sys.exit(main())
