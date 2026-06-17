## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2026 Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import os
import sys
import time

sys.dont_write_bytecode = True
sys.path.append (os.path.join('..', '..'))
from BuildLoader import *

class Board(BaseBoard):
    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_KSV\0'
        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 4
        self.VERINFO_SVN          = 1
        self.VERINFO_BUILD_DATE   = time.strftime("%m/%d/%Y")

        self.BOARD_NAME           = 'ksv'
        self.BOARD_PKG_NAME       = 'KaseyvilleBoardPkg'
        self.SILICON_PKG_NAME     = 'KaseyvillePkg'
        self._EXTRA_INC_PATH      = ['Silicon/KaseyvillePkg/Ksv/Include']
        self._FSP_PATH_NAME       = 'Silicon/KaseyvillePkg/Ksv/FspBin'
        self.FSP_INF_FILE         = 'Silicon/KaseyvillePkg/Ksv/FspBin/FspBin.inf'
        self._SMBIOS_YAML_FILE    = os.path.join('Platform', self.BOARD_PKG_NAME, 'SmbiosStrings.yaml')
        self.MICROCODE_INF_FILE   = 'Silicon/KaseyvillePkg/Ksv/Microcode/Microcode.inf'
        self.ACTM_INF_FILE        = 'Silicon/KaseyvillePkg/Ksv/Actm/Actm.inf'

        self._PCI_ENUM_DOWNGRADE_MEM64  = 0
        self._PCI_ENUM_DOWNGRADE_PMEM64 = 0
        self._PCI_ENUM_DOWNGRADE_BUS0   = 0
        self.PCI_MEM64_BASE             = 0x200000000000

        self._PCI_ENUM_BUS_SCAN_TYPE    = 1 # range
        self._PCI_ENUM_BUS_SCAN_ITEMS   = '0,0xFA'
        self.PCI_EXPRESS_BASE     = 0x80000000
        self.PCI_IO_BASE          = 0x00001000
        self.PCI_MEM32_BASE       = 0x90000000
        self.SUPPORT_ARI          = 0
        self.SUPPORT_SR_IOV       = 0

        self.ACPI_PM_TIMER_BASE   = 0x508

        self.FLASH_LAYOUT_START   = 0x100000000 # 4GB Top
        self.FLASH_BASE_SIZE      = 0x08000000  # 128MB
        self.FLASH_BASE_ADDRESS   = (self.FLASH_LAYOUT_START - self.FLASH_BASE_SIZE)

        self.HAVE_FIT_TABLE       = 1
        self.HAVE_FIT4_ENTRY      = 1
        self.HAVE_VBT_BIN         = 0 # No internal gfx
        self.HAVE_VERIFIED_BOOT   = 1
        self.HAVE_MEASURED_BOOT   = 1
        self.HAVE_ACPI_TABLE      = 1
        self.HAVE_FLASH_MAP       = 1

        self.ENABLE_SPLASH        = 1
        self.ENABLE_FRAMEBUFFER_INIT  = 0 # No internal gfx
        self.ENABLE_GRUB_CONFIG   = 0
        self.ENABLE_CSME_UPDATE   = 0
        self.BUILD_CSME_UPDATE_DRIVER   = 0
        self.ENABLE_FWU           = 1
        self.ENABLE_SMBIOS        = 1
        self.ENABLE_VTD           = 0
        self.SUPPORT_X2APIC       = 1
        if self.SUPPORT_X2APIC:
            self.ENABLE_VTD       = 1
        # 0: Disable  1: Enable  2: Auto (disable for UEFI payload, enable for others)
        # 3: Enable and won't set SMRR 4: Auto and won't set SMRR
        self.ENABLE_SMM_REBASE    = 4

        self.SIIPFW_SIZE          = 0x1000

        if self._SMBIOS_YAML_FILE:
            self.SIIPFW_SIZE += 0x1000

        # For large ACPI table region
        self.LOADER_RSVD_MEM_SIZE             = 0x00A00000
        self.LOADER_ACPI_RECLAIM_MEM_SIZE     = 0x002B0000

        # HCC : Max 42 physical cores (2x with hyperthreading)
        # XCC : Max 72 physical cores (2x with hyperthreading)
        self.CPU_MAX_LOGICAL_PROCESSOR_NUMBER = 144
        self.CPU_AP_WAIT_TIME_US = 200000 # Time to wait for AP Wakeup in MicroSeconds

        if self.HAVE_FIT_TABLE:
            self.FIT_ENTRY_MAX_NUM  = 17 #

        if self.HAVE_FIT4_ENTRY:
            self.BOOT_POLICY_SIZE           = 0x00000000 # Should be set to 0 as per FIT spec
            self.BOOT_POLICY_ADDRESS        = 0xFF002000 # Base address set to Empty region (Next to PDA region)

        self.ENABLE_FAST_BOOT               = 0
        if self.ENABLE_FAST_BOOT:
            self.RELEASE_MODE               = 1
            self.ENABLE_SPLASH              = 0
            self.ENABLE_FRAMEBUFFER_INIT    = 0
            self.HAVE_VBT_BIN               = 0
            self.HAVE_VERIFIED_BOOT         = 0
            self.HAVE_MEASURED_BOOT         = 0
            self.VERIFIED_BOOT_HASH_MASK    = 0
            self.ENABLE_GRUB_CONFIG         = 0

        # RSA2048 or RSA3072
        self._RSA_SIGN_TYPE         = 'RSA3072'

        # 'SHA2_256' or 'SHA2_384'
        self._SIGN_HASH             = 'SHA2_384'

        # 0x01 for SHA2_256 or 0x02 for SHA2_384
        self.SIGN_HASH_TYPE         = HASH_TYPE_VALUE[self._SIGN_HASH]

        # 0x0010  for SM3_256 | 0x0008 for SHA2_512 | 0x0004 for SHA2_384 | 0x0002 for SHA2_256 | 0x0001 for SHA1
        self.IPP_HASH_LIB_SUPPORTED_MASK  = IPP_CRYPTO_ALG_MASK['SHA2_384'] | IPP_CRYPTO_ALG_MASK['SHA2_256']
        # G9 for 384 | W7 Opt for SHA384| Ni  Opt for SHA256| V8 Opt for SHA256
        self.ENABLE_CRYPTO_SHA_OPT  = IPP_CRYPTO_OPTIMIZATION_MASK['SHA256_NI'] | IPP_CRYPTO_OPTIMIZATION_MASK['SHA384_W7']

        # Key configuration
        self._MASTER_PRIVATE_KEY    = 'KEY_ID_MASTER' + '_' + self._RSA_SIGN_TYPE
        self._CFGDATA_PRIVATE_KEY   = 'KEY_ID_CFGDATA' + '_' + self._RSA_SIGN_TYPE
        self._CONTAINER_PRIVATE_KEY = 'KEY_ID_CONTAINER' + '_' + self._RSA_SIGN_TYPE

        # 2 - S3M UART, 0xFF - BMC UART
        self.DEBUG_PORT_NUMBER      = 0xFF

        # True - Enable simics environment, False - Disable simics
        self._SIMICS_ENV_ENABLE     = False

        self.STAGE1A_XIP          = 1
        self.STAGE1B_XIP          = 1
        self.STAGE2_XIP           = 0

        self.STAGE1A_SIZE         = 0x0001b000
        self.STAGE1_STACK_SIZE    = 0x00015000
        self.STAGE1_STACK_BASE_OFFSET = 0x00142000
        # STAGE1_DATA_SIZE increamented by 0x5C000 to allocate size for SGX meta data
        self.STAGE1_DATA_SIZE     = 0x00075000
        self.STAGE1B_SIZE         = 0x0037D000
        self.STAGE2_SIZE          = 0x000AE000
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x00400000

        self.PAYLOAD_SIZE         = 0x00040000
        self.EPAYLOAD_SIZE        = 0x00200000
        self.UEFI_VARIABLE_SIZE = 0x1000
        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.UEFI_VARIABLE_SIZE = 0x00090000

        self.UCODE_SIZE           = 0x193000
        self.MRCDATA_SIZE         = 0x00010000
        self.CFGDATA_SIZE         = 0x00004000
        self.KEYHASH_SIZE         = 0x00001000
        self.VARIABLE_SIZE        = 0x00002000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x00020000 if self.ENABLE_FWU else 0

        self.NON_REDUNDANT_SIZE   = 0x01000000

        self.OS_LOADER_FD_SIZE    = 0x00060000
        self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE

        self.SLIMBOOTLOADER_SIZE  = 0x01000000 # 16 MB
        self.PLD_HEAP_SIZE        = 0x09000000 # for UBUNTU

        self.KM_SIZE              = 0x00000400
        self.BPM_SIZE             = 0x00000600
        self.ACM_SIZE             = 0x00040000 + self.KM_SIZE + self.BPM_SIZE
        # adjust ACM_SIZE to meet 128KB alignment (to align 100KB ACM size)
        if self.ACM_SIZE > 0:
            acm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE - self.STAGE1B_SIZE
            acm_btm = acm_top - self.ACM_SIZE
            acm_btm = (acm_btm & 0xFFFC0000)
            self.ACM_SIZE     = acm_top - acm_btm

        self.ACTM_SIZE             = 0x00040000
        # adjust ACTM_SIZE to meet 256KB alignment
        if self.ACTM_SIZE > 0:
            actm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE - self.ACM_SIZE - self.UCODE_SIZE\
            - self.STAGE1B_SIZE - self.KEYHASH_SIZE - self.CFGDATA_SIZE - self.STAGE2_SIZE - self.PAYLOAD_SIZE\
            - self.UEFI_VARIABLE_SIZE - self.EPAYLOAD_SIZE - self.MRCDATA_SIZE - self.VARIABLE_SIZE -self.SBLRSVD_SIZE
            actm_btm = actm_top - self.ACTM_SIZE
            actm_btm = (actm_btm & 0xFFFC0000)
            self.ACTM_SIZE     = actm_top - actm_btm
            self.ACTM_BASE     = actm_btm

        self.TOP_SWAP_SIZE        = self.STAGE1A_SIZE + self.ACM_SIZE
        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE + 0x4000
        self.PLD_RSVD_MEM_SIZE    = 0x00500000

        self._CFGDATA_INT_FILE = []
        self._CFGDATA_EXT_FILE = ['CfgData_Int_Rp.dlt']

    def GetPlatformDsc (self):
        dsc = {}
        # These libraries will be added into the DSC files
        common_libs = [
            'LoaderLib|Platform/CommonBoardPkg/Library/LoaderLib/LoaderLib.inf',
            'SerialPortLib|BootloaderCommonPkg/Library/SerialPortLib/SerialPortLib.inf',
            'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
            'ResetSystemLib|Platform/$(BOARD_PKG_NAME)/Library/ResetSystemLib/ResetSystemLib.inf',
            'PchSpiLib|Silicon/CommonSocPkg/Library/PchSpiLib/PchSpiLib.inf',
            'SpiFlashLib|Silicon/CommonSocPkg/Library/SpiFlashLib/SpiFlashLib.inf',
            'GpioV2Lib|Silicon/CommonSocPkg/Library/GpioV2Lib/GpioV2Lib.inf',
            'P2sbLib|Silicon/CommonSocPkg/Library/P2sbLib/P2sbLib.inf',
            'GpioV2SiLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioV2SiLib/GpioV2SiLib.inf',
            'PchInfoLib|Silicon/$(SILICON_PKG_NAME)/Library/PchInfoLib/PchInfoLib.inf',
            'BootGuardLib|Silicon/CommonSocPkg/Library/BootGuardLibCBnT/BootGuardLibCBnT.inf',
            'SgxLib|Silicon/$(SILICON_PKG_NAME)/Library/SgxLib/SgxLib.inf',
            'PchCycleDecodingLib|Silicon/$(SILICON_PKG_NAME)/Library/PchCycleDecodingLib/PchCycleDecodingLib.inf',
            'KsvVtdLib|Silicon/$(SILICON_PKG_NAME)/Library/KsvVtdLib/KsvVtdLib.inf',
            'DmarLib|Silicon/CommonSocPkg/Library/DmarLib/DmarLib.inf',
            'MadtLib|Silicon/CommonSocPkg/Library/MadtLib/MadtLib.inf',
            'McfgLib|Silicon/CommonSocPkg/Library/McfgLib/McfgLib.inf',
            'ShellExtensionLib|Platform/$(BOARD_PKG_NAME)/Library/ShellExtensionLib/ShellExtensionLib.inf',
            'AspeedGfxLib|Platform/$(BOARD_PKG_NAME)/Library/AspeedGfxLib/AspeedGfxLib.inf',
            'BdatLib|Silicon/$(SILICON_PKG_NAME)/Library/BdatLib/BdatLib.inf',
            'WatchDogTimerLib|Silicon/CommonSocPkg/Library/WatchDogTimerLib/WatchDogTimerLib.inf',
            'TcoTimerLib|Silicon/CommonSocPkg/Library/TcoTimerLib/TcoTimerLib.inf',
            'TopSwapLib|Silicon/CommonSocPkg/Library/TopSwapLib/TopSwapLib.inf'
        ]
        if self.BUILD_CSME_UPDATE_DRIVER:
            common_libs.append ('MeFwUpdateLib|Silicon/$(SILICON_PKG_NAME)/Library/MeFwUpdateLib/MeFwUpdateLib.inf')

        dsc['LibraryClasses.%s' % self.BUILD_ARCH] = common_libs

        if self._SIMICS_ENV_ENABLE:
            dsc['PcdsFixedAtBuild'] = ['gPlatformKaseyvilleTokenSpaceGuid.PcdSimicsEnv | TRUE']
        else:
            dsc['PcdsFixedAtBuild'] = ['gPlatformKaseyvilleTokenSpaceGuid.PcdSimicsEnv | FALSE']

        return dsc

    def GetOutputImages (self):
        # define extra images that will be copied to output folder
        img_list = ['SlimBootloader.txt',
            'FlashMap.txt',
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
            # Use RSA2048 key for verifying OS image signed with RSA2048
            HASH_USAGE['PUBKEY_OS'],
            'KEY_ID_OS1_PUBLIC_RSA2048'
          ),
          (
            # Use RSA3072 key for verifying OS image signed with RSA3072
            HASH_USAGE['PUBKEY_OS'],
            'KEY_ID_OS1_PUBLIC_RSA3072'
          ),
        ]
        return pub_key_list

    def GetContainerList (self):
        container_list = []
        container_list_auth_type = self._RSA_SIGN_TYPE + '_'+ self._SIGNING_SCHEME[4:] + '_' + self._SIGN_HASH
        container_list.append (
          # Name     | Image File     | CompressAlg | AuthType                  | Key File                                  | Region Align | Region Size | Svn Info
          # ===============================================================================================================================================================
          [('IPFW',   'SIIPFW.bin',    '',           container_list_auth_type,   'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,  0,             0,            0),   # Container Header
           ('SMBS',   'smbios.bin',    'Dummy',      container_list_auth_type,   'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,  0,             0x1000,       0)],  # SMBIOS Component
        )

        return container_list

    def GetImageLayout (self):
        img_list = []

        acm_flag = 0 if self.ACM_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        fwu_flag = 0 if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR
        # Restore original condition after an EXT CFG FILE is available.
        # For now, if CNFG component update is required, CFGDATA.bin can't be ignored
        #    from FLASH_MAP.
        cfg_flag = 0 #if len(self._CFGDATA_EXT_FILE) > 0 and self.CFGDATA_REGION_TYPE == FLASH_REGION_TYPE.BIOS else STITCH_OPS.MODE_FILE_IGNOR

        img_list.extend ([
            ('NON_REDUNDANT.bin', [
                ('ACTM.bin'         , ''     , self.ACTM_SIZE,          STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('SBLRSVD.bin'      , ''     , self.SBLRSVD_SIZE,       STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('VARIABLE.bin'     , ''     , self.VARIABLE_SIZE,      STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('MRCDATA.bin'      , ''     , self.MRCDATA_SIZE,       STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('EPAYLOAD.bin'     , ''     , self.EPAYLOAD_SIZE,      STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('UEFIVARIABLE.bin' , ''     , self.UEFI_VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('PAYLOAD.bin'      , 'Lz4'  , self.PAYLOAD_SIZE,       STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('STAGE2.fd'        , 'Lzma' , self.STAGE2_SIZE,        STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('CFGDATA.bin'      , ''     , self.CFGDATA_SIZE,       STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                ('FWUPDATE.bin'     , 'Lzma' , self.FWUPDATE_SIZE,      STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                ('KEYHASH.bin'      , ''     , self.KEYHASH_SIZE,       STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('UCODE.bin'        , ''     , self.UCODE_SIZE,         STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('ACM.bin'          , ''     , self.ACM_SIZE,           STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                ('STAGE1B.fd'       , ''     , self.STAGE1B_SIZE,       STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('STAGE1A.fd'       , ''     , self.STAGE1A_SIZE,       STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ]
            ),
            ('SlimBootloader.bin', [
                ('NON_REDUNDANT.bin' , '' , self.NON_REDUNDANT_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD),
                ]
            ),
        ])

        return img_list
