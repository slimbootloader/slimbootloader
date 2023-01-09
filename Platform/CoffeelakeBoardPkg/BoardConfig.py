## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2017 - 2023, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import os
import sys
import shutil
import time

sys.dont_write_bytecode = True
sys.path.append (os.path.join('..', '..'))
from BuildLoader import BaseBoard, STITCH_OPS, FLASH_REGION_TYPE
from BuildLoader import HASH_USAGE

class Board(BaseBoard):
    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID          = 'SB_CFL'
        self.VERINFO_PROJ_MAJOR_VER    = 1
        self.VERINFO_PROJ_MINOR_VER    = 1
        self.VERINFO_SVN               = 1
        self.VERINFO_BUILD_DATE        = time.strftime("%m/%d/%Y")
        self.LOWEST_SUPPORTED_FW_VER   = ((self.VERINFO_PROJ_MAJOR_VER << 8) + self.VERINFO_PROJ_MINOR_VER)

        self.BOARD_NAME           = 'cfl'
        self.BOARD_PKG_NAME       = 'CoffeelakeBoardPkg'
        self.SILICON_PKG_NAME     = 'CoffeelakePkg'

        self.PCI_EXPRESS_BASE     = 0xE0000000
        self.PCI_IO_BASE          = 0x00003000
        self.PCI_MEM32_BASE       = 0x9F000000
        self.ACPI_PM_TIMER_BASE   = 0x1808

        self._PCI_ENUM_DOWNGRADE_PMEM64 = 1
        self._PCI_ENUM_DOWNGRADE_MEM64  = 1
        self._PCI_ENUM_DOWNGRADE_BUS0   = 1
        self.PCI_MEM64_BASE             = 0x4000000000

        self.FLASH_BASE_ADDRESS   = 0xFE000000
        self.FLASH_BASE_SIZE      = (self.FLASH_LAYOUT_START - self.FLASH_BASE_ADDRESS)

        self.HAVE_FIT_TABLE       = 1
        self.HAVE_VBT_BIN         = 1
        self.HAVE_VERIFIED_BOOT   = 1
        self.HAVE_MEASURED_BOOT   = 1
        self.HAVE_ACPI_TABLE      = 1
        self.ENABLE_SPLASH        = 1
        self.ENABLE_FRAMEBUFFER_INIT  = 1
        self.HAVE_PSD_TABLE       = 1
        self.ENABLE_VTD           = 1
        self.ENABLE_GRUB_CONFIG   = 1
        self.ENABLE_CSME_UPDATE   = 0
        self.ENABLE_DMA_PROTECTION    = 0
        self.ENABLE_SMM_REBASE        = 2
        self.DEBUG_PORT_NUMBER    = 0xFF

        # BIT0: Log buffer
        # BIT1: Serial port
        # BIT2: Platform debug port
        # BIT7: Platform console devices
        self.DEBUG_OUTPUT_DEVICE_MASK = 0x03

        # Input consoles
        #   BIT0: Serial port
        #   BIT1: USB Keyboard
        #   BIT2: Platform debug port
        self.CONSOLE_IN_DEVICE_MASK   = 0x00000001

        # Output consoles
        #   BIT0: Serial port
        #   BIT1: Framebuffer
        #   BIT2: Platform debug port
        self.CONSOLE_OUT_DEVICE_MASK  = 0x00000001

        # CSME update library is required to enable this option and will be available as part of CSME kit
        self.BUILD_CSME_UPDATE_DRIVER   = 0

        # To enable source debug, set 1 to self.ENABLE_SOURCE_DEBUG
        self.ENABLE_SOURCE_DEBUG  = 0
        # If ENABLE_SOURCE_DEBUG is disabled, SKIP_STAGE1A_SOURCE_DEBUG will be ignored
        self.SKIP_STAGE1A_SOURCE_DEBUG  = 0

        if self.HAVE_FIT_TABLE:
            self.FIT_ENTRY_MAX_NUM  = 12

        self.STAGE1A_SIZE         = 0x00010000
        if self.FSPDEBUG_MODE == 1:
            self.STAGE1B_SIZE     = 0x00100000
        else:
            self.STAGE1B_SIZE     = 0x000DB000
        self.STAGE2_SIZE          = 0x00080000
        if self.ENABLE_SOURCE_DEBUG:
            self.STAGE1B_SIZE += 0x4000

        self.ENABLE_FWU           = 1
        self.ENABLE_SMBIOS        = 1

        # Verify required minimum FSP version
        self.MIN_FSP_REVISION     = 0x07006550
        # Verify FSP image ID. Empty string means skipping verification
        self.FSP_IMAGE_ID         = '$CFLFSP$'

        self.STAGE1B_XIP          = 1

        # Stack settings to run FspMemoryInit
        self.FSP_M_STACK_TOP      = 0xFEF3FF00

        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x000E0000

        self.STAGE1_STACK_SIZE    = 0x00002000
        self.STAGE1_DATA_SIZE     = 0x00014000

        self.PAYLOAD_EXE_BASE     = 0x00B00000
        self.PAYLOAD_SIZE         = 0x00030000
        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.UEFI_VARIABLE_SIZE = 0x00040000
        else:
            self.UEFI_VARIABLE_SIZE = 0x1000
        self.EPAYLOAD_SIZE        = 0x00190000
        self.UCODE_SIZE           = 0x00080000
        self.MRCDATA_SIZE         = 0x00008000
        self.CFGDATA_SIZE         = 0x00004000
        self.KEYHASH_SIZE         = 0x00001000
        self.VARIABLE_SIZE        = 0x00002000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x00020000 if self.ENABLE_FWU else 0

        self.TOP_SWAP_SIZE        = 0x020000
        self.REDUNDANT_SIZE  = self.UCODE_SIZE + self.STAGE2_SIZE + self.STAGE1B_SIZE + \
                               self.FWUPDATE_SIZE + self.CFGDATA_SIZE + self.KEYHASH_SIZE

        self.NON_VOLATILE_SIZE    = 0x001000
        self.SLIMBOOTLOADER_SIZE  = 0x800000
        self.NON_REDUNDANT_SIZE   = self.SLIMBOOTLOADER_SIZE - \
                                    (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 - \
                                    self.NON_VOLATILE_SIZE

        self.PLD_HEAP_SIZE        = 0x04000000
        self.PLD_STACK_SIZE       = 0x00020000
        self.PLD_RSVD_MEM_SIZE    = 0x00500000

        # TBD: ACM/KM/BPM Size, as of Sep 2017
        #      ACM size is fixed 100KB, KM size is fixed 0x400, BPM size is fixed 0x600
        self.KM_SIZE              = 0x00000400
        self.BPM_SIZE             = 0x00000600
        self.ACM_SIZE             = 0x00008000 + self.KM_SIZE + self.BPM_SIZE
        # adjust ACM_SIZE to meet 128KB alignment (to align 100KB ACM size)
        if self.ACM_SIZE > 0:
            acm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE
            acm_btm = acm_top - self.ACM_SIZE
            acm_btm = (acm_btm & 0xFFFE0000)
            self.ACM_SIZE     = acm_top - acm_btm

        self.CFGDATA_REGION_TYPE  = FLASH_REGION_TYPE.BIOS
        self.SPI_IAS_REGION_TYPE  = FLASH_REGION_TYPE.BIOS

        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE + 0x4000
        self._CFGDATA_INT_FILE    = ['CfgDataInt_Cfls.dlt', 'CfgDataInt_Cflh.dlt', 'CfgDataInt_Whl.dlt']
        self._CFGDATA_EXT_FILE    = ['CfgDataExt_Upx.dlt']

        # If mulitple VBT table support is required, list them as:
        #   {VbtImageId1 : VbtFileName1, VbtImageId2 : VbtFileName2, ...}
        # VbtImageId is ID to identify a VBT image. It is a UINT32 number to match
        #   the ImageId field in the VBT container.
        # VbtFileName is the VBT file name. It needs to be located under platform
        #   VbtBin folder.
        self._MULTI_VBT_FILE      = {1:'Vbt.dat', 2:'VbtCflH.dat', 3:'VbtCflS.dat'}

    def GetPlatformDsc (self):
        debug_port_enable = False
        if (self.CONSOLE_OUT_DEVICE_MASK & 0x04) or (self.CONSOLE_IN_DEVICE_MASK & 0x04) or (self.DEBUG_OUTPUT_DEVICE_MASK & 0x04):
            debug_port_enable = True

        dsc = {}
        common_libs = [
            'LoaderLib|Platform/CommonBoardPkg/Library/LoaderLib/LoaderLib.inf',
            'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
            'PchSpiLib|Silicon/CommonSocPkg/Library/PchSpiLib/PchSpiLib.inf',
            'SpiFlashLib|Silicon/CommonSocPkg/Library/SpiFlashLib/SpiFlashLib.inf',
            'PchSbiAccessLib|Silicon/CommonSocPkg/Library/PchSbiAccessLib/PchSbiAccessLib.inf',
            'PchInfoLib|Silicon/$(SILICON_PKG_NAME)/Library/PchInfoLib/PchInfoLib.inf',
            'PchSerialIoLib|Silicon/$(SILICON_PKG_NAME)/Library/PchSerialIoLib/PchSerialIoLib.inf',
            'GpioLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioLib/GpioLib.inf',
            'IgdOpRegionLib|Silicon/$(SILICON_PKG_NAME)/Library/IgdOpRegionLib/IgdOpRegionLib.inf',
            'BdatLib|Silicon/CommonSocPkg/Library/BdatLib/BdatLib.inf',
            'BootMediaLib|Silicon/$(SILICON_PKG_NAME)/Library/BootMediaLib/BootMediaLib.inf',
            'StageCommonLib|Silicon/$(SILICON_PKG_NAME)/Library/StageCommonLib/StageCommonLib.inf',
            'BootGuardLib|Silicon/$(SILICON_PKG_NAME)/Library/BootGuardLib/BootGuardLib.inf',
            'SgxLib|Silicon/$(SILICON_PKG_NAME)/Library/SgxLib/SgxLib.inf',
            'PsdLib|Silicon/$(SILICON_PKG_NAME)/Library/PsdLib/PsdLib.inf',
            'HeciLib|Silicon/CommonSocPkg/Library/HeciLib/HeciLib.inf',
            'MeChipsetLib|Silicon/CommonSocPkg/Library/MeChipsetLib/MeChipsetLib.inf',
            'ShellExtensionLib|Platform/$(BOARD_PKG_NAME)/Library/ShellExtensionLib/ShellExtensionLib.inf',
            'VtdLib|Silicon/$(SILICON_PKG_NAME)/Library/VTdLib/VTdLib.inf',
            'VtdPmrLib|Silicon/CommonSocPkg/Library/VtdPmrLib/VtdPmrLib.inf',
            'TcoTimerLib|Silicon/CommonSocPkg/Library/TcoTimerLib/TcoTimerLib.inf',
            'TopSwapLib|Silicon/CommonSocPkg/Library/TopSwapLib/TopSwapLib.inf',
            'WatchDogTimerLib|Silicon/CommonSocPkg/Library/WatchDogTimerLib/WatchDogTimerLib.inf'
        ]
        if self.BUILD_CSME_UPDATE_DRIVER:
            common_libs.append ('MeFwUpdateLib|Silicon/$(SILICON_PKG_NAME)/Library/MeFwUpdateLib/MeFwUpdateLib.inf')
        if debug_port_enable:
            common_libs.append ('GpioDebugPortLib|Platform/CommonBoardPkg/Library/GpioDebugPortLib/GpioDebugPortLib.inf')
            common_libs.append ('MailboxDebugPortLib|Platform/CommonBoardPkg/Library/MailboxDebugPortLib/MailboxDebugPortLib.inf')
            common_libs.append ('DebugPortLib|Platform/$(BOARD_PKG_NAME)/Library/DebugPortLib/DebugPortLib.inf')
        dsc['LibraryClasses.%s' % self.BUILD_ARCH]   = common_libs

        fixed_pcds = []
        if debug_port_enable:
            # Use GPIO_CNL_LP_GPP_H12 pin for example
            # Refer to GpioPinsCnlLp.h file to look up its GPIO pad value.
            # The GPIO pad value is 0x0407000C.
            gpio_pad     = 0 # 0x0407000C for GPIO_CNL_LP_GPP_H12

            # IOSF SB access address for the GPIO DW0 register (DWORD)
            # Calculate the IOSF SB access address for GPIO DW0 register
            #   - Get GPIO DW0 offset from SOC EDS datasheet. It is 0x9D0 for H12.
            #   - Get GPIO community ID. It is community 1 for H12.
            #   - Look up PID_GPIOCOM1 in file PchRegsPcr.h for PortId. It is 0x6D.
            #    IOSF_MMIO = 0xFD000000 + (PortId<<16) + GpioOffset
            # If set to 0, gpio debug port will be disabled.
            iosf_mmio    = 0 # 0xFD6D09D0 for GPIO_CNL_LP_GPP_H12
            fixed_pcds = [
                'gCommonBoardTokenSpaceGuid.PcdGpioDebugPortPinPad      | 0x%08X' % gpio_pad,
                'gCommonBoardTokenSpaceGuid.PcdGpioDebugPortMmioBase    | 0x%08X' % iosf_mmio
            ]

            # Mailbox debug port, use GPI Interrupt Enable register as example
            # IOSF SB access address for the mailbox register (DWORD)
            # If set to 0, mailbox debug port will be disabled
            mailbox_mmio = 0 # 0xFD6E0120
            fixed_pcds.append (
                'gCommonBoardTokenSpaceGuid.PcdMailboxDebugPortMmioBase | 0x%08X' % mailbox_mmio,
            )

        if len(fixed_pcds) > 0:
            dsc['PcdsFixedAtBuild.%s' % self.BUILD_ARCH] = fixed_pcds

        return dsc

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
        img_list = []

        acm_flag = 0 if self.ACM_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        fwu_flag = 0 if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR
        cfg_flag = 0 if len(self._CFGDATA_EXT_FILE) > 0 and self.CFGDATA_REGION_TYPE == FLASH_REGION_TYPE.BIOS else STITCH_OPS.MODE_FILE_IGNOR

        # output files need to have unique base name (excluding file extension)
        # output files ends with 'rom' extension will be copied over for final stitching
        img_list.extend ([
                ('NON_VOLATILE.bin', [
                    ('SBLRSVD.bin',    ''        , self.SBLRSVD_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('NON_REDUNDANT.bin', [
                    ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('MRCDATA.bin'  ,  ''        , self.MRCDATA_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('EPAYLOAD.bin',   ''        , self.EPAYLOAD_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('UEFIVARIABLE.bin', ''      , self.UEFI_VARIABLE_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('PAYLOAD.bin'  ,  'Lz4'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('REDUNDANT_A.bin', [
                    ('UCODE.bin'    ,  ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  , ''         , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B_A.fd' ,  ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('REDUNDANT_B.bin', [
                    ('UCODE.bin'    ,  ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  , ''         , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B_B.fd' ,  ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_A.bin', [
                    ('ACM.bin'      ,  ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A_A.fd'      , ''    , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_B.bin', [
                    ('ACM.bin'      ,  ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A_B.fd'      , ''    , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
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
