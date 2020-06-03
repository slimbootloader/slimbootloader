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
from BuildLoader import BaseBoard, STITCH_OPS, HASH_USAGE

class Board(BaseBoard):

    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        EXECUTE_IN_PLACE          = 0
        FREE_TEMP_RAM_TOP         = 0x70000

        self.VERINFO_IMAGE_ID          = 'SB_QEMU '
        self.VERINFO_PROJ_MAJOR_VER    = 1
        self.VERINFO_PROJ_MINOR_VER    = 0
        self.VERINFO_SVN               = 1
        self.VERINFO_BUILD_DATE        = '09/25/2017'

        self.BOARD_NAME           = 'qemu'
        self.BOARD_PKG_NAME       = 'QemuBoardPkg'
        self.SILICON_PKG_NAME     = 'QemuSocPkg'

        #
        # By default,
        # _PCI_ENUM_DOWNGRADE_IO32  : 1
        # _PCI_ENUM_DOWNGRADE_MEM64 : 1
        # _PCI_ENUM_DOWNGRADE_PMEM64: 1
        # _PCI_ENUM_BUS_SCAN_TYPE   : 0 (0: list, 1: range)
        #
        #self._PCI_ENUM_DOWNGRADE_IO32   = 0
        #self._PCI_ENUM_DOWNGRADE_MEM64  = 0
        #self._PCI_ENUM_DOWNGRADE_PMEM64 = 0
        #self._PCI_ENUM_BUS_SCAN_TYPE    = 1
        self._PCI_ENUM_BUS_SCAN_ITEMS = '0,10,0x35,128,0xff'
        self.PCI_IO_BASE              = 0x00002000
        self.PCI_MEM32_BASE           = 0x80000000
        self.USB_KB_POLLING_TIMEOUT   = 10

        self.HAVE_VERIFIED_BOOT       = 1
        self.HAVE_VBT_BIN             = 1
        self.ENABLE_SPLASH            = 1
        self.ENABLE_FRAMEBUFFER_INIT  = 1
        self.ENABLE_FWU               = 1
        self.ENABLE_GRUB_CONFIG       = 1
        self.ENABLE_LINUX_PAYLOAD     = 1
        self.ENABLE_CRYPTO_SHA_OPT    = 0

        self.ENABLE_SMBIOS            = 1

        self.CPU_MAX_LOGICAL_PROCESSOR_NUMBER = 255

        # To enable source debug, set 1 to self.ENABLE_SOURCE_DEBUG
        # self.ENABLE_SOURCE_DEBUG  = 1



        # For test purpose
        # self.SKIP_STAGE1A_SOURCE_DEBUG = 1

        # BIT0:Serial  BIT1:USB KB
        # Support serial port input console by default
        self.CONSOLE_IN_DEVICE_MASK  = 0x00000001

        # BIT0:Serial  BIT1:GFX
        self.CONSOLE_OUT_DEVICE_MASK = 0x00000001

        # Let Stage1A verifies Stage1B
        self.VERIFIED_BOOT_STAGE_1B  = 0x1

        self.STAGE1B_XIP          = 0

        self.STAGE1A_SIZE         = 0x00010000
        self.STAGE1B_SIZE         = 0x00030000
        self.STAGE2_SIZE          = 0x00018000

        self.TEST_SIZE            = 0x00001000
        self.SIIPFW_SIZE          = 0x00010000
        self.EPAYLOAD_SIZE        = 0x0020D000
        self.PAYLOAD_SIZE         = 0x00020000
        self.CFGDATA_SIZE         = 0x00001000
        self.KEYHASH_SIZE         = 0x00001000
        self.VARIABLE_SIZE        = 0x00002000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x00018000 if self.ENABLE_FWU else 0

        self._REDUNDANT_LAYOUT    = 1
        if not self._REDUNDANT_LAYOUT:
            self.TOP_SWAP_SIZE      = 0x000000
            self.REDUNDANT_SIZE     = 0x000000
            self.NON_VOLATILE_SIZE  = 0x000000
            self.NON_REDUNDANT_SIZE = 0x400000
        else:
            self.TOP_SWAP_SIZE      = 0x010000
            self.REDUNDANT_SIZE     = 0x080000
            self.NON_VOLATILE_SIZE  = 0x001000
            self.NON_REDUNDANT_SIZE = 0x2DF000


        self.SLIMBOOTLOADER_SIZE = (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 + \
            self.NON_REDUNDANT_SIZE + self.NON_VOLATILE_SIZE

        self.STAGE1A_XIP          = EXECUTE_IN_PLACE
        if not self.STAGE1A_XIP:
            # For Stage1A, STAGE1_FD_BASE is always located at 4GB - STAGE1_FD_SIZE
            # And execution will start from there.  If STAGE1A_XIP is 0, Stage1A
            # will relocate itself to STAGE1A_LOAD_BASE to continue execution
            self.STAGE1A_LOAD_BASE  = FREE_TEMP_RAM_TOP

        self.STAGE1B_XIP          = EXECUTE_IN_PLACE
        if not self.STAGE1B_XIP:
            # For Stage1B, it can be compressed if STAGE1B_XIP is 0
            # If so, STAGE1B_FD_BASE/STAGE1B_FD_SIZE need to be defined
            self.STAGE1B_FD_SIZE    = 0x30000
            if self.NO_OPT_MODE:
                self.STAGE1B_FD_SIZE += 0x2000
            self.STAGE1B_FD_BASE    = FREE_TEMP_RAM_TOP - self.STAGE1B_FD_SIZE

        # For Stage2, it is always compressed.
        # if STAGE2_LOAD_HIGH is 1, STAGE2_FD_BASE will be ignored
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x00060000

        if self.NO_OPT_MODE:
            self.OS_LOADER_FD_SIZE   += 0x00010000
            self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE

        self.STAGE1_STACK_SIZE    = 0x00002000
        self.STAGE1_DATA_SIZE     = 0x0000E000

        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE

        # Add following to force to use a specific platform ID
        # By default it uses auto-detection.
        #self._PLATFORM_ID         = 1

        self._CFGDATA_INT_FILE    = []
        self._CFGDATA_EXT_FILE    = ['CfgDataExt_Brd1.dlt', 'CfgDataExt_Brd31.dlt']

        # If mulitple VBT table support is required, list them as:
        #   {VbtImageId1 : VbtFileName1, VbtImageId2 : VbtFileName2, ...}
        # VbtImageId is ID to identify a VBT image. It is a UINT32 number to match
        #   the ImageId field in the VBT container.
        # VbtFileName is the VBT file name. It needs to be located under platform
        #   VbtBin folder.
        self._MULTI_VBT_FILE      = {1:'Vbt800x600.dat', 2:'Vbt1024x768.dat'}

    def PlatformBuildHook (self, build, phase):
        if phase == 'post-build:before':
          # Create PTEST.bin
          bins = bytearray (b'12345678')
          file = build._fv_dir + '/PTEST.bin'
          with open(file, 'wb') as fd:
              fd.write(bins)

    def GetPlatformDsc (self):
        dsc = {}
        dsc['LibraryClasses.%s' % self.BUILD_ARCH] = [
            'LoaderLib|Platform/CommonBoardPkg/Library/LoaderLib/LoaderLib.inf',
            'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
            'GpioLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioLib/GpioLib.inf',
            'SpiFlashLib|Silicon/$(SILICON_PKG_NAME)/Library/SpiFlashLib/SpiFlashLib.inf',
        ]
        dsc['PcdsFeatureFlag.%s' % self.BUILD_ARCH] = [
            'gPlatformCommonLibTokenSpaceGuid.PcdMultiUsbBootDeviceEnabled | TRUE'
        ]
        return dsc

    def GetKeyHashList (self):
        # Define a set of new key used for different purposes
        # The key is either public key PEM format or private key PEM format
        pub_key_list = [
          (
            HASH_USAGE['PUBKEY_CFG_DATA'],
            'CFGDATA_KEY_ID'
          ),
           (
            # Use a single test key
            HASH_USAGE['PUBKEY_FWU'],
            'FIRMWAREUPDATE_KEY_ID'
          ),
         (
            # Use a single test key
            HASH_USAGE['PUBKEY_CONT_DEF'],
            'CONTAINER_KEY_ID'
          ),
        ]
        return pub_key_list

    def GetContainerList (self):
        container_list = []
        container_list.append ([
          # Name       | Image File |    CompressAlg  | AuthType             | Key File                      | Region Align | Region Size
          # ============================================================================================================================================
          ('IPFW',      'SIIPFW.bin',    '',           'RSA2048_PSS_SHA2_256',   'CONTAINER_KEY_ID',    0,             0     ),   # Container Header
          ('TST1',      '',              'Dummy',      '',                   '',                                 0,             0x2000),   # Component 1
          ('TST2',      '',              'Lz4',        '',                   '',                                 0,             0x3000),   # Component 2
          ('TST3',      '',              'Lz4',        'RSA2048_PSS_SHA2_256',   'CONTAINER_KEY_ID',    0,             0x3000),   # Component 3
          ('TST4',      '',              'Lzma',       'SHA2_256',           '',                                 0,             0x3000),   # Component 4
          ('TST5',      '',              'Dummy',      'RSA2048_PSS_SHA2_256',   'CONTAINER_KEY_ID',    0,             0x3000),   # Component 5
          ('TST6',      '',              '',           '',                   '',                                 0,             0x1000),   # Component 6
        ])
        return container_list

    def GetImageLayout (self):

        compress = '' if self.STAGE1B_XIP else 'Lz4'
        fwu_mode = STITCH_OPS.MODE_FILE_PAD if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR

        img_list = []

        # output files need to have unique base name (excluding file extension)
        # output files ends with 'rom' extension will be copied over for final stitching
        if self.REDUNDANT_SIZE == 0:
            img_list.extend ([
                ('SlimBootloader.bin', [
                    ('SBLRSVD.bin',    ''        , self.SBLRSVD_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('PAYLOAD.bin'  ,  'Lzma'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('EPAYLOAD.bin' ,  ''        , self.EPAYLOAD_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  ,  ''        , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B.fd'   ,  compress  , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A.fd'   ,  ''        , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ])
        else:
            img_list.extend ([
                ('NON_VOLATILE.bin', [
                    ('SBLRSVD.bin',    ''        , self.SBLRSVD_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('NON_REDUNDANT.bin', [
                    ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('PAYLOAD.bin'  ,  'Lzma'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('EPAYLOAD.bin' ,  ''        , self.EPAYLOAD_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('SIIPFW.bin'   ,  ''        , self.SIIPFW_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('PTEST.bin'    ,  ''        , self.TEST_SIZE,     STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('REDUNDANT_A.bin', [
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B_A.fd' ,  compress  , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, fwu_mode,                 STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  ,  ''        , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  ,  ''        , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('REDUNDANT_B.bin', [
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B_B.fd' ,  compress  , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, fwu_mode,                 STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  ,  ''        , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_A.bin', [
                    ('STAGE1A_A.fd'      , ''      , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_B.bin', [
                    ('STAGE1A_B.fd'      , ''      , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('SlimBootloader.bin', [
                    ('NON_VOLATILE.bin'  , '' , self.NON_VOLATILE_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('NON_REDUNDANT.bin' , '' , self.NON_REDUNDANT_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('REDUNDANT_B.bin'   , '' , self.REDUNDANT_SIZE,     STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('REDUNDANT_A.bin'   , '' , self.REDUNDANT_SIZE,     STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('TOP_SWAP_B.bin'    , '' , self.TOP_SWAP_SIZE,      STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ('TOP_SWAP_A.bin'    , '' , self.TOP_SWAP_SIZE,      STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                    ]
                ),
                ])

        return img_list
