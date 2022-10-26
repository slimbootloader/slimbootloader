## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2017 - 2020, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import os
import sys

sys.dont_write_bytecode = True
sys.path.append (os.path.join('..', '..'))
from BuildLoader import FLASH_MAP, BaseBoard, STITCH_OPS
from BuildLoader import IPP_CRYPTO_OPTIMIZATION_MASK, IPP_CRYPTO_ALG_MASK, HASH_USAGE

#
#    Temporary Memory Layout for APL
#
#  FF000000 +--------------------------+
#           |        Stage1B           |
#           |     (Decompressed)       |
#  FEF80000 +--------------------------+
#           |     Stage1 Heap/Stack    |
#  FEF70000 +--------------------------+
#           |        Not Used          |
#           +-------------+------------+
#           |           Free           |
#           |             |------------|
#           +-------------+   MRC NVS  |
#           |             |            |
#  FEF40000 +-  Stage1B  -+------------+
#           |  Compressed |  FSP Mem   |
#  FEF16000 |             +------------+
#           |             |            |
#  FEF10000 --------------+------------+
#           |     N/A (Don't use)      |
#  FEF08000 +--------------------------+
#           |        Stage1A           |
#  FEF00000 +--------------------------+
#

class Board(BaseBoard):
    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID          = 'SB_APLI '
        self.VERINFO_PROJ_MAJOR_VER    = 1
        self.VERINFO_PROJ_MINOR_VER    = 0
        self.VERINFO_SVN               = 1
        self.VERINFO_BUILD_DATE        = '05/20/2018'

        self.BOARD_NAME           = 'apl'
        self.BOARD_PKG_NAME       = 'ApollolakeBoardPkg'
        self.SILICON_PKG_NAME     = 'ApollolakePkg'

        self._PCI_ENUM_DOWNGRADE_PMEM64 = 1
        self.PCI_IO_BASE          = 0x00001000
        self.PCI_MEM32_BASE       = 0x80000000
        self.PCI_MEM64_BASE       = 0x400000000

        self.FLASH_SIZE           = 0x800000
        self.FLASH_BASE           = self.FLASH_LAYOUT_START - self.FLASH_SIZE

        self.HAVE_VBT_BIN         = 1
        self.HAVE_VERIFIED_BOOT   = 1
        self.HAVE_MEASURED_BOOT   = 0
        self.HAVE_SEED_LIST       = 0
        self.HAVE_PSD_TABLE       = 1
        self.ENABLE_SMBIOS        = 1

        self.ENABLE_FSP_LOAD_IMAGE    = 0
        self.ENABLE_VTD               = 1
        self.ENABLE_FWU               = 1
        self.ENABLE_SPLASH            = 1
        self.ENABLE_FRAMEBUFFER_INIT  = 1
        self.ENABLE_GRUB_CONFIG       = 1
        self.ENABLE_DMA_PROTECTION    = 0
        self.ENABLE_SMM_REBASE        = 2

        # G9 for 384 | W7 Opt for SHA384| Ni  Opt for SHA256| V8 Opt for SHA256
        self.ENABLE_CRYPTO_SHA_OPT    = IPP_CRYPTO_OPTIMIZATION_MASK['SHA256_NI']

        # To enable source debug, set 1 to self.ENABLE_SOURCE_DEBUG
        # self.ENABLE_SOURCE_DEBUG   = 1

        # Temporary skip Stage1A due to 32KB(IBBL) size limitation
                # until library size optimization has done.
        # If ENABLE_SOURCE_DEBUG is disabled, SKIP_STAGE1A_SOURCE_DEBUG will be ignored
        self.SKIP_STAGE1A_SOURCE_DEBUG  = 1

        # BIT0:Serial  BIT1:USB KB
        # Support serial port input console by default
        self.CONSOLE_IN_DEVICE_MASK  = 0x00000001

        # BIT0:Serial  BIT1:GFX
        self.CONSOLE_OUT_DEVICE_MASK = 0x00000001

        # Mem | NVMe | Usb | Spi | Ufs | eMMC | SD | Sata
        self.BOOT_MEDIA_SUPPORT_MASK  = 0xBF

        # EXT | FAT
        self.FILE_SYSTEM_SUPPORT_MASK  = 3

        # Verify required minimum FSP version
        self.MIN_FSP_REVISION     = 0x01040301
        # Verify FSP image ID. Empty string means skipping verification
        self.FSP_IMAGE_ID         = '$APLFSP$'

        self.STAGE1A_SIZE         = 0x00008000
        self.STAGE1B_SIZE         = 0x00036000
        if self.ENABLE_SOURCE_DEBUG:
            self.STAGE1B_SIZE += 0x2000
        self.STAGE2_SIZE          = 0x00032000
        self.PAYLOAD_SIZE         = 0x00021000

        if len(self._PAYLOAD_NAME.split(';')) > 1:
            # EPAYLOAD is specified
            self.EPAYLOAD_SIZE      = 0x00130000
            self.UEFI_VARIABLE_SIZE = 0x00040000
        else:
            # EPAYLOAD does not exist, create a dummy one
            self.EPAYLOAD_SIZE      = 0x1000
            self.UEFI_VARIABLE_SIZE = 0x1000

        if self.FSPDEBUG_MODE == 1:
            self.STAGE1B_SIZE += 0x00009000
            self.STAGE2_SIZE  += 0x0000F000

        self.STAGE1A_XIP          = 0
        self.STAGE1A_LOAD_BASE    = 0xFEF00000
        self.STAGE1B_XIP          = 0
        self.STAGE1B_LOAD_BASE    = 0xFEF10000
        self.STAGE1B_FD_BASE      = 0xFEF80000
        self.STAGE1B_FD_SIZE      = 0x0006B000
        if self.ENABLE_SOURCE_DEBUG:
            self.STAGE1B_FD_SIZE += 0x00001000
        if self.RELEASE_MODE == 0:
            self.STAGE1B_FD_SIZE += 0x00002000
            self.PAYLOAD_SIZE    += 0x00007000
        # For Stage2, it is always compressed.
        # if STAGE2_LOAD_HIGH is 1, STAGE2_FD_BASE will be ignored
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x00080000
        self.STAGE2_LOAD_BASE     = 0x00100000

        self.STAGE1_STACK_SIZE    = 0x00002000
        self.STAGE1_DATA_SIZE     = 0x0000E000
        # Offset is relative to the temporary memory base 0xFEF00000
        self.STAGE1_STACK_BASE_OFFSET = 0x00080000 - (self.STAGE1_STACK_SIZE + self.STAGE1_DATA_SIZE)

        # To support large payload such as UEFI
        self.LOADER_RSVD_MEM_SIZE = 0x00B8C000

        self.PLD_RSVD_MEM_SIZE    = 0x00500000

        self.PLD_HEAP_SIZE        = 0x04000000

        self.FWUPDATE_SIZE        = 0x00020000
        self.CFGDATA_SIZE         = 0x00004000
        self.KEYHASH_SIZE         = 0x00001000
        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE
        self.MRCDATA_SIZE         = 0x00004000
        self.VARIABLE_SIZE        = 0x00002000
        self.S3_DEBUG             = 0
        self.SBLRSVD_SIZE         = 0x00001000

        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.SPI_IAS1_SIZE    = 0x00001000
        else:
            self.SPI_IAS1_SIZE    = 0x00150000

        self._CFGDATA_INT_FILE = ['CfgData_Int_LeafHill.dlt']
        self._CFGDATA_EXT_FILE = ['CfgData_Ext_Gpmrb.dlt', 'CfgData_Ext_Up2.dlt','CfgData_Ext_OxbHill.dlt','CfgData_Ext_MB3.dlt','CfgData_Ext_JuniperHill.dlt']

        # If mulitple VBT table support is required, list them as:
        #   {VbtImageId1 : VbtFileName1, VbtImageId2 : VbtFileName2, ...}
        # VbtImageId is ID to identify a VBT image. It is a UINT32 number to match
        #   the ImageId field in the VBT container.
        # VbtFileName is the VBT file name. It needs to be located under platform
        #   VbtBin folder.
        self._MULTI_VBT_FILE      = {1:'Vbt.dat', 2:'Vbt_Up2.dat'}

    def GetPlatformDsc (self):
        dsc = {}
        common_libs = [
            'LoaderLib|Platform/$(BOARD_PKG_NAME)/Library/LoaderLib/LoaderLib.inf',
            'SerialPortLib|Silicon/$(SILICON_PKG_NAME)/Library/SerialPortLib/SerialPortLib.inf',
            'SocInfoLib|Silicon/$(SILICON_PKG_NAME)/Library/SocInfoLib/SocInfoLib.inf',
            'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
            'ScSbiAccessLib|Silicon/$(SILICON_PKG_NAME)/Library/ScSbiAccessLib/ScSbiAccessLib.inf',
            'GpioLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioLib/GpioLib.inf',
            'PchSpiLib|Silicon/CommonSocPkg/Library/PchSpiLib/PchSpiLib.inf',
            'SpiFlashLib|Silicon/CommonSocPkg/Library/SpiFlashLib/SpiFlashLib.inf',
            'IgdOpRegionLib|Silicon/$(SILICON_PKG_NAME)/Library/IgdOpRegionLib/IgdOpRegionLib.inf',
            'IocIpcLib|Platform/$(BOARD_PKG_NAME)/Library/IocIpcLib/IocIpcLib.inf',
            'BootGuardLib|Silicon/$(SILICON_PKG_NAME)/Library/BootGuardLib20/BootGuardLib20.inf',
            'HeciLib|Silicon/CommonSocPkg/Library/HeciLib/HeciLib.inf',
            'MeChipsetLib|Silicon/ApollolakePkg/Library/MeChipsetLib/MeChipsetLib.inf',
            'PsdLib|Silicon/ApollolakePkg/Library/PsdLib/PsdLib.inf',
            'ShellExtensionLib|Platform/$(BOARD_PKG_NAME)/Library/ShellExtensionLib/ShellExtensionLib.inf',
            'BootMediaLib|Silicon/ApollolakePkg/Library/BootMediaLib/BootMediaLib.inf',
            'FlashDescriptorLib|Silicon/ApollolakePkg/Library/FlashDescriptorLib/FlashDescriptorLib.inf',
            'VtdLib|Silicon/$(SILICON_PKG_NAME)/Library/VtdLib/VtdLib.inf',
            'SmbusLib|Silicon/$(SILICON_PKG_NAME)/Library/SmbusLib/SmbusLib.inf',
            'HdaLib|Platform/$(BOARD_PKG_NAME)/Library/HdaLib/HdaLib.inf',
            'VtdPmrLib|Silicon/CommonSocPkg/Library/VtdPmrLib/VtdPmrLib.inf',
            'BaseIpcLib|Silicon/$(SILICON_PKG_NAME)/Library/BaseIpcLib/BaseIpcLib.inf',
            'TcoTimerLib|Silicon/CommonSocPkg/Library/TcoTimerLib/TcoTimerLib.inf',
            'TopSwapLib|Silicon/CommonSocPkg/Library/TopSwapLib/TopSwapLib.inf',
            'WatchDogTimerLib|Silicon/CommonSocPkg/Library/WatchDogTimerLib/WatchDogTimerLib.inf'
        ]
        dsc['LibraryClasses.%s' % self.BUILD_ARCH] = common_libs
        return dsc

    def GetFlashMapList (self):
        img_list  = self.GetImageLayout ()
        comp_list = []
        offset    = 0

        # Skip Stitch_IPAD and Stitch_OPAD for flash map
        for img in img_list[2:][::-1]:
                child = img[1][0]
                if child[3] & STITCH_OPS.MODE_FILE_IGNOR:
                    continue
                bname = os.path.splitext(child[0])[0]
                comp = {'name':child[0], 'bname':bname, 'offset':offset, 'size':child[2], 'flag': FLASH_MAP.FLASH_MAP_DESC_FLAGS['COMPRESSED'] if child[1] else 0}
                if bname in  ['STAGE1A', 'STAGE1B', 'STAGE2', 'FWUPDATE', 'CFGDATA', 'MRCDATA', 'PAYLOAD', 'VARIABLE']:
                    comp['flag'] |= FLASH_MAP.FLASH_MAP_DESC_FLAGS['REDUNDANT']
                else:
                    comp['flag'] |= FLASH_MAP.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT']
                comp_list.append (comp)
                offset += child[2]

        flag = FLASH_MAP.FLASH_MAP_DESC_FLAGS['REDUNDANT']
        comp_list.append ({'name':'SBLRSVD.bin','bname':'SBLRSVD','offset':0, 'size':self.SBLRSVD_SIZE, 'flag': FLASH_MAP.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT']})
        comp_list.append ({'name':'BPM.bin',    'bname':'BPM',    'offset':0, 'size':0, 'flag': flag})
        return comp_list[::-1]

    def GetOutputImages (self):
        # define extra images that will be copied to output folder
        img_list = ['SlimBootloader.txt',
                    'CfgDataStitch.py',
                    'CfgDataDef.yaml',
                    'CfgDataInt.bin'
                    ]
        return img_list

    def GetKeyHashList (self):
        # Define a set of new key used for different purposes
        # The key is either key id or public key PEM format or private key PEM format
        pub_key_list = [
          (
            # Key for verifying Config data blob
            HASH_USAGE['PUBKEY_CFG_DATA'],
            'KEY_ID_CFGDATA' + '_' + self._RSA_SIGN_TYPE
          ),
          (
            # Key for verifying firmware update
            HASH_USAGE['PUBKEY_FWU'],
            'KEY_ID_FIRMWAREUPDATE' + '_' + self._RSA_SIGN_TYPE
          ),
          (
            # Key for verifying container header
            HASH_USAGE['PUBKEY_CONT_DEF'],
            'KEY_ID_CONTAINER' + '_' + self._RSA_SIGN_TYPE
          ),
          (
            # key for veryfying OS image.
            HASH_USAGE['PUBKEY_OS'],
            'KEY_ID_OS1_PUBLIC' + '_' + self._RSA_SIGN_TYPE
          ),
        ]
        return pub_key_list

    def GetImageLayout (self):
        ias1_flag = 0 if self.SPI_IAS1_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        fwu_flag = 0 if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR
        img_list  = []
        img_list.extend ([
                    # Padding to ensure all other components in OBB partition will be aligned at 4KB boundary
                    # 0xB00 assumes (IBBP.man, BPM.met) + (IPAD, IBBL, IBBM,  OBB, FWUP, CFGD, PLD, VAR, MRCD) in BpdtIBB
                    # 0x180 assumes (OPAD,  PROV, EPLD) in BpdtOBB
                    # If more files are added, the offset needs to be adjusted accordingly
                    ('Stitch_IPAD.bin', [
                        ('PADDING.bin',  '',                0xB00,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_OPAD.bin', [
                        ('PADDING.bin',  '',                0x180,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_FWU.bin', [
                        ('FWUPDATE.bin' ,  'Lzma', self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_FB.bin', [
                        ('SPI_IAS1.bin',  '',    self.SPI_IAS1_SIZE,  STITCH_OPS.MODE_FILE_PAD | ias1_flag, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_PLD.bin', [
                        ('PAYLOAD.bin',  'Lz4', self.PAYLOAD_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_VAR.bin', [
                        ('VARIABLE.bin', '',     self.VARIABLE_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_MRCDATA.bin', [
                        ('MRCDATA.bin', '',      self.MRCDATA_SIZE,   STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_CFGDATA.bin', [
                        ('CFGDATA.bin',  '',     self.CFGDATA_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_KEYHASH.bin', [
                        ('KEYHASH.bin',  '',     self.KEYHASH_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_OBB.bin', [
                        ('STAGE2.fd',    'Lz4',  self.STAGE2_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_IBBM.bin', [
                        ('STAGE1B.fd',   'Lz4',  self.STAGE1B_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_IBBL.bin', [
                        ('STAGE1A.fd',   '',     self.STAGE1A_SIZE,   STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_EPLD.bin', [
                        ('EPAYLOAD.bin', '',    self.EPAYLOAD_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
                    ),
                    ('Stitch_UVAR.bin', [
                        ('UEFIVARIABLE.bin', '',  self.UEFI_VARIABLE_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL)],
                    ),
                    ])

        return img_list
