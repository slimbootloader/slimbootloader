## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
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

        self.VERINFO_IMAGE_ID     = 'SB_MTLP'
        self.VERINFO_PROJ_MAJOR_VER = 0
        self.VERINFO_PROJ_MINOR_VER = 0
        self.VERINFO_SVN            = 1
        self.VERINFO_BUILD_DATE     = time.strftime("%m/%d/%Y")

        self.BOARD_NAME           = 'mtl'
        self.BOARD_PKG_NAME       = 'MeteorlakeBoardPkg'
        self.SILICON_PKG_NAME     = 'MeteorlakePkg'
        self.FSP_IMAGE_ID         = '$MTLFSP$'
        self._EXTRA_INC_PATH      = ['Silicon/MeteorlakePkg/FspBin']
        self._FSP_PATH_NAME       = 'Silicon/MeteorlakePkg/FspBin'
        self.FSP_INF_FILE         = 'Silicon/MeteorlakePkg/FspBin/FspBin.inf'
        self.MICROCODE_INF_FILE   = 'Silicon/MeteorlakePkg/Microcode/Microcode.inf'

        self._LP_SUPPORT          = False
        self._N_SUPPORT           = False
        self._PS_SUPPORT          = False

        self.PCI_EXPRESS_BASE     = 0xC0000000
        self.PCI_IO_BASE          = 0x00002000
        self.PCI_MEM32_BASE       = 0x80000000
        self.ACPI_PM_TIMER_BASE   = 0x1808

        self.FLASH_LAYOUT_START   = 0x100000000 # 4GB Top
        self.FLASH_BASE_SIZE      = 0x02000000  # 32MB
        self.FLASH_BASE_ADDRESS   = (self.FLASH_LAYOUT_START - self.FLASH_BASE_SIZE)
        self.LOADER_ACPI_RECLAIM_MEM_SIZE = 0x000090000
        self.HAVE_FIT_TABLE       = 1
        self.HAVE_VBT_BIN         = 1 # TBD
        self.HAVE_VERIFIED_BOOT   = 1
        self.HAVE_MEASURED_BOOT   = 1
        self.HAVE_FLASH_MAP       = 1
        self.HAVE_ACPI_TABLE      = 1
        self.HAVE_PSD_TABLE       = 1
        self.ENABLE_SPLASH            = 1
        self.ENABLE_FRAMEBUFFER_INIT  = 1
        self.ENABLE_VTD           = 1
        # To enable source debug, set 1 to self.ENABLE_SOURCE_DEBUG
        self.ENABLE_SOURCE_DEBUG  = 0
        # If ENABLE_SOURCE_DEBUG is disabled, SKIP_STAGE1A_SOURCE_DEBUG will be ignored
        self.SKIP_STAGE1A_SOURCE_DEBUG = 1
        # 0: Disable  1: Enable  2: Auto (disable for UEFI payload, enable for others)
        self.ENABLE_SMM_REBASE    = 2

        # 0 - PCH UART0, 1 - PCH UART1, 2 - PCH UART2, 0xFF - EC UART 0x3F8
        self.DEBUG_PORT_NUMBER = 0x0

        self.ENABLE_MULTI_USB_BOOT_DEV = 1

        self.CPU_MAX_LOGICAL_PROCESSOR_NUMBER = 32

        self.SUPPORT_X2APIC = 0

        self.ENABLE_GRUB_CONFIG       = 1

        if self.HAVE_FIT_TABLE:
            self.FIT_ENTRY_MAX_NUM  = 17

        # RSA2048 or RSA3072
        self._RSA_SIGN_TYPE          = 'RSA3072'

        # 'SHA2_256' or 'SHA2_384'
        self._SIGN_HASH              = 'SHA2_384'

        # 0x01 for SHA2_256 or 0x02 for SHA2_384
        self.SIGN_HASH_TYPE          = HASH_TYPE_VALUE[self._SIGN_HASH]

        # 0x0010  for SM3_256 | 0x0008 for SHA2_512 | 0x0004 for SHA2_384 | 0x0002 for SHA2_256 | 0x0001 for SHA1
        self.IPP_HASH_LIB_SUPPORTED_MASK   = IPP_CRYPTO_ALG_MASK['SHA2_384'] | IPP_CRYPTO_ALG_MASK['SHA2_256']
        # G9 for 384 | W7 Opt for SHA384| Ni  Opt for SHA256| V8 Opt for SHA256
        self.ENABLE_CRYPTO_SHA_OPT  = IPP_CRYPTO_OPTIMIZATION_MASK['SHA256_NI'] | IPP_CRYPTO_OPTIMIZATION_MASK['SHA384_W7']

        # Key configuration
        self._MASTER_PRIVATE_KEY    = 'KEY_ID_MASTER' + '_' + self._RSA_SIGN_TYPE
        self._CFGDATA_PRIVATE_KEY   = 'KEY_ID_CFGDATA' + '_' + self._RSA_SIGN_TYPE
        self._CONTAINER_PRIVATE_KEY = 'KEY_ID_CONTAINER' + '_' + self._RSA_SIGN_TYPE
        self.ENABLE_SMP_INIT      = 1 # SMP is N/A on SIMICS
        self.ENABLE_FWU           = 1
        self.ENABLE_SMBIOS        = 1

        self.ENABLE_CSME_UPDATE   = 1

        # CSME update library is required to enable this option and will be available as part of CSME kit
        self.BUILD_CSME_UPDATE_DRIVER   = 0

        self.STAGE1A_XIP          = 1
        self.STAGE1B_XIP          = 1
        self.STAGE2_XIP           = 0

        self.STAGE1A_SIZE         = 0x00020000
        self.STAGE1_STACK_SIZE    = 0x00002000
        self.STAGE1_DATA_SIZE     = 0x00014000
        self.FSP_M_STACK_TOP      = 0xFEF7FF00
        self.STAGE1B_SIZE         = 0x00200000
        self.STAGE2_SIZE          = 0x000C1000
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x001F0000

        self.PAYLOAD_SIZE         = 0x0002D000
        self.EPAYLOAD_SIZE        = 0x00230000

        self.ENABLE_FAST_BOOT = 0
        if self.ENABLE_FAST_BOOT:
            self.ENABLE_SPLASH              = 0
            self.ENABLE_FRAMEBUFFER_INIT    = 0
            self.RELEASE_MODE               = 1
            self.HAVE_VERIFIED_BOOT         = 0
            self.HAVE_MEASURED_BOOT         = 0
            self.VERIFIED_BOOT_HASH_MASK    = 0

        if self.RELEASE_MODE and self.ENABLE_FAST_BOOT:
            self.STAGE1A_SIZE         = 0x00016000
            self.STAGE1B_SIZE         = 0x000E0000
            self.STAGE2_SIZE          = 0x000C0000
            self.STAGE2_FD_SIZE       = 0x000F0000
            self.PAYLOAD_SIZE         = 0x00024000

        if self.ENABLE_SOURCE_DEBUG:
            self.STAGE1B_SIZE += 0x4000
            if self.SKIP_STAGE1A_SOURCE_DEBUG == 0:
                self.STAGE1A_SIZE += 0x4000


        self.UEFI_VARIABLE_SIZE = 0x1000
        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.UEFI_VARIABLE_SIZE = 0x00040000
        self.UCODE_SIZE           = 0x000B7000
        self.UCODE_SLOT_SIZE      = 0x00037000
        self.MRCDATA_SIZE         = 0x00010000
        self.CFGDATA_SIZE         = 0x00004000
        self.KEYHASH_SIZE         = 0x00001000
        self.VARIABLE_SIZE        = 0x00002000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x00020000 if self.ENABLE_FWU else 0
        # Need a little bit more for full paging table
        self.OS_LOADER_FD_SIZE    = 0x00058000
        self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE

        # If BUILD_IDENTICAL_TS is 0, the flash map sizings and layout
        # will need to be adjusted so that uCode and Stage 1B are in
        # redudant partition
        self.BUILD_IDENTICAL_TS    = 1

        # If ENABLE_SBL_RESILIENCY is 1, BiosRedAssistance FIT strap setting
        # needs to be manually changed to Enabled
        self.ENABLE_SBL_RESILIENCY = 0

        self.TOP_SWAP_SIZE        = 0x00400000
        self.REDUNDANT_SIZE       = self.STAGE2_SIZE + self.FWUPDATE_SIZE + \
                                    self.CFGDATA_SIZE + self.KEYHASH_SIZE

        self.SIIPFW_SIZE = 0x1000

        self.ENABLE_TCC = 0
        if self.ENABLE_TCC:
            self.TCC_CCFG_SIZE   = 0x00001000
            self.TCC_CRL_SIZE    = 0x00008000
            self.TCC_STREAM_SIZE = 0x00005000
            self.SIIPFW_SIZE    += self.TCC_CCFG_SIZE + self.TCC_CRL_SIZE + self.TCC_STREAM_SIZE

        self.ENABLE_TSN = 0
        if self.ENABLE_TSN:
            self.TMAC_SIZE = 0x00001000
            self.SIIPFW_SIZE += self.TMAC_SIZE

        self.NON_REDUNDANT_SIZE   = 0x3BF000 + self.SIIPFW_SIZE
        self.NON_VOLATILE_SIZE    = 0x001000
        self.SLIMBOOTLOADER_SIZE  = (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 + \
                                    self.NON_REDUNDANT_SIZE + self.NON_VOLATILE_SIZE
        self.SLIMBOOTLOADER_SIZE  = ((self.SLIMBOOTLOADER_SIZE + 0xFFFFF) & ~0xFFFFF)
        self.PLD_HEAP_SIZE        = 0x09000000
        # self.PLD_HEAP_SIZE        = 0x04000000
        self.PLD_STACK_SIZE       = 0x00020000
        self.PLD_RSVD_MEM_SIZE    = 0x00500000
        self.KM_SIZE              = 0x00000400
        self.BPM_SIZE             = 0x00000600
        self.ACM_SIZE             = 0x00040000 + self.KM_SIZE + self.BPM_SIZE
        # adjust ACM_SIZE to meet 256KB alignment (to align 256KB ACM size)
        if self.ACM_SIZE > 0:
            acm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE
            acm_btm = acm_top - self.ACM_SIZE
            acm_btm = (acm_btm & 0xFFFC0000)
            self.ACM_SIZE     = acm_top - acm_btm

        self.LOADER_RSVD_MEM_SIZE = 0x500000

        # If mulitple VBT table support is required, list them as:
        #   {VbtImageId1 : VbtFileName1, VbtImageId2 : VbtFileName2, ...}
        # VbtImageId is ID to identify a VBT image. It is a UINT32 number to match
        #   the ImageId field in the VBT container.
        # VbtFileName is the VBT file name. It needs to be located under platform
        #   VbtBin folder.
        self._MULTI_VBT_FILE      = {1:'VbtMtlpCrb.dat', 2:'VbtMtlpLp5.dat', 3:'VbtMtlpRvp.dat', 4:'VbtMtlpsCrb.dat', 5:'VbtMtlpsRvp.dat'}

        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE
        self._generated_cfg_file_prefix = 'Autogen_'

        self._CFGDATA_INT_FILE = []
        self._CFGDATA_EXT_FILE = [self._generated_cfg_file_prefix + 'CfgData_Int_Mtlp_DDR5_Rvp.dlt', self._generated_cfg_file_prefix + 'CfgData_Int_Mtlps_DDR5_RVP.dlt', self._generated_cfg_file_prefix + 'CfgData_Int_Mtlps_DDR5_CRB.dlt', self._generated_cfg_file_prefix + 'CfgData_Int_Mtlp_LPDDR5_Rvp.dlt',  self._generated_cfg_file_prefix + 'CfgData_Int_Mtlp_DDR5_CRB.dlt']

    def PlatformBuildHook (self, build, phase):
        if phase == 'pre-build:before':
            # create build folder if not exist
            if not os.path.exists(build._fv_dir):
                os.makedirs(build._fv_dir)

            # Generate the dlt files based on feature
            brd_cfg_src_dir = os.path.join(os.environ['PLT_SOURCE'], 'Platform', self.BOARD_PKG_NAME, 'CfgData')
            brd_cfg2_src_dir = '.'
            if hasattr(self, 'BOARD_PKG_NAME_OVERRIDE'):
                brd_cfg2_src_dir = os.path.join(os.environ['PLT_SOURCE'], 'Platform', self.BOARD_PKG_NAME_OVERRIDE, 'CfgData')
            for dlt_file in self._CFGDATA_EXT_FILE:
                cfg_dlt_file  = os.path.join(brd_cfg_src_dir, dlt_file[len (self._generated_cfg_file_prefix):])
                if not os.path.exists(cfg_dlt_file):
                    cfg_dlt_file = os.path.join(brd_cfg2_src_dir, dlt_file[len (self._generated_cfg_file_prefix):])
                lines         = open (cfg_dlt_file).read()

                # Enable TCC in dlt file
                if self.ENABLE_TCC:
                    if os.path.exists(os.path.join(brd_cfg_src_dir, 'CfgData_Tcc_Feature.dlt')):
                        lines += open (os.path.join(brd_cfg_src_dir, 'CfgData_Tcc_Feature.dlt')).read()
                    else:
                        lines += open (os.path.join(brd_cfg2_src_dir, 'CfgData_Tcc_Feature.dlt')).read()

                # Enable TSN in dlt file
                if self.ENABLE_TSN:
                    if os.path.exists(os.path.join(brd_cfg_src_dir, 'CfgData_Tsn_Feature.dlt')):
                        lines += open (os.path.join(brd_cfg_src_dir, 'CfgData_Tsn_Feature.dlt')).read()
                    else:
                        lines += open (os.path.join(brd_cfg2_src_dir, 'CfgData_Tsn_Feature.dlt')).read()

                # Write to generated final dlt file
                output_cfg_dlt_file = os.path.join(build._fv_dir, dlt_file)
                open(output_cfg_dlt_file, 'w').write(lines)

    def GetPlatformDsc (self, BuildPkgName = "BootLoaderCorePkg"):
        dsc = {}
        # These libraries will be added into the DSC files
        dsc['LibraryClasses.%s' % self.BUILD_ARCH] = [
            'LoaderLib|Platform/CommonBoardPkg/Library/LoaderLib/LoaderLib.inf',
            'SerialPortLib|BootloaderCommonPkg/Library/SerialPortLib/SerialPortLib.inf',
            'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
            'ResetSystemLib|Platform/$(BOARD_PKG_NAME)/Library/ResetSystemLib/ResetSystemLib.inf',
            'GpioLib|Silicon/CommonSocPkg/Library/GpioLib/GpioLib.inf',
            'GpioV2Lib|Silicon/$(SILICON_PKG_NAME)/Library/GpioV2Lib/GpioV2Lib.inf',
            'PchP2sbLib|Silicon/$(SILICON_PKG_NAME)/Library/PchP2sbLib/PchP2sbLib.inf',
            'GpioSiLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioSiLib/GpioSiLib.inf',
            'PchSbiAccessLib|Silicon/CommonSocPkg/Library/PchSbiAccessLib/PchSbiAccessLib.inf',
            'TccLib|Silicon/CommonSocPkg/Library/TccLib/TccLib.inf',
            'ShellExtensionLib|Platform/$(BOARD_PKG_NAME)/Library/ShellExtensionLib/ShellExtensionLib.inf',
            'FspmUpdUpdateLib|Platform/$(BOARD_PKG_NAME)/Library/FspmUpdUpdateLib/FspmUpdUpdateLib.inf',
            'FspsUpdUpdateLib|Platform/$(BOARD_PKG_NAME)/Library/FspsUpdUpdateLib/FspsUpdUpdateLib.inf',
            'PchInfoLib|Silicon/$(SILICON_PKG_NAME)/Library/PchInfoLib/PchInfoLib.inf',
            'PchPciBdfLib|Silicon/$(SILICON_PKG_NAME)/Library/BasePchPciBdfLib/BasePchPciBdfLib.inf',
            'PchSpiLib|Silicon/CommonSocPkg/Library/PchSpiLib/PchSpiLib.inf',
            'SpiFlashLib|Silicon/CommonSocPkg/Library/SpiFlashLib/SpiFlashLib.inf',
            'BootGuardLib|Silicon/$(SILICON_PKG_NAME)/Library/BootGuardLibCBnT/BootGuardLibCBnT.inf',
            'BdatLib|Silicon/CommonSocPkg/Library/BdatLib/BdatLib.inf',
            'SmbusLib|Silicon/CommonSocPkg/Library/SmbusLib/SmbusLib.inf',
            'IgdOpRegionLib30|Silicon/CommonSocPkg/Library/IgdOpRegionLib/IgdOpRegionLib30/IgdOpRegionLib30.inf',
            'HeciLib|Silicon/CommonSocPkg/Library/HeciLib/HeciLib.inf',
            'MeChipsetLib|Silicon/CommonSocPkg/Library/MeChipsetLib/MeChipsetLib.inf',
            'VtdLib|Silicon/$(SILICON_PKG_NAME)/Library/VTdLib/VTdLib.inf',
            'PsdLib|Silicon/$(SILICON_PKG_NAME)/Library/PsdLib/PsdLib.inf',
            'HeciMeExtLib|Silicon/$(SILICON_PKG_NAME)/Library/HeciMeExtLib/HeciMeExtLib.inf',
            'MeExtMeasurementLib|Silicon/$(SILICON_PKG_NAME)/Library/MeExtMeasurementLib/MeExtMeasurementLib.inf',
            'CpuPcieHsPhyInitLib|Silicon/$(SILICON_PKG_NAME)/Library/CpuPcieHsPhyInitLib/CpuPcieHsPhyInitLib.inf',
            'WatchDogTimerLib|Silicon/CommonSocPkg/Library/WatchDogTimerLib/WatchDogTimerLib.inf',
            'TcoTimerLib|Silicon/CommonSocPkg/Library/TcoTimerLib/TcoTimerLib.inf',
            'TopSwapLib|Silicon/$(SILICON_PKG_NAME)/Library/TopSwapLib/TopSwapLib.inf'
        ]

        if self.BUILD_CSME_UPDATE_DRIVER:
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('MeFwUpdateLib|Silicon/$(SILICON_PKG_NAME)/Library/MeFwUpdateLib/MeFwUpdateLib.inf')

        dsc['PcdsFixedAtBuild'] = ['gPlatformModuleTokenSpaceGuid.PcdAcpiTablesMaxEntry | 40']

        if self._PS_SUPPORT:
            dsc['PcdsFixedAtBuild'].append('gPlatformMeteorLakeTokenSpaceGuid.PcdMtlPSSupport | TRUE')

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
          # Name | Image File             |    CompressAlg  | AuthType                        | Key File                        | Region Align   | Region Size |  Svn Info
          # ========================================================================================================================================================
          ('IPFW',      'SIIPFW.bin',          '',     container_list_auth_type,   'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,        0,          0     ,        0),   # Container Header
        )

        bins = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Binaries')

        if self.ENABLE_TCC:
            container_list.append (
              ('TCCC', 'TccCacheCfg.bin' if os.path.exists(os.path.join(bins,   'TccCacheCfg.bin')) else '',   'Lz4', container_list_auth_type, 'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0, self.TCC_CCFG_SIZE, 0),   # TCC Cache Config
            )
            container_list.append (
              ('TCCM', 'TccCrlBinary.bin' if os.path.exists(os.path.join(bins, 'TccCrlBinary.bin')) else '', 'Lz4', container_list_auth_type, 'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0, self.TCC_CRL_SIZE, 0),   # TCC Crl
            )
            container_list.append (
              ('TCCT', 'TccStreamCfg.bin' if os.path.exists(os.path.join(bins,  'TccStreamCfg.bin')) else '',  'Lz4', container_list_auth_type, 'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0, self.TCC_STREAM_SIZE, 0),   # TCC Stream Config
            )

        if self.ENABLE_TSN:
            container_list.append (
              ('TMAC','TsnSubRegion.bin' if os.path.exists(os.path.join(bins,  'TsnSubRegion.bin')) else '',   'Lz4', container_list_auth_type, 'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0, self.TMAC_SIZE, 0),   # TSN MAC Address
            )

        return [container_list]

    def GetOutputImages (self):
        # define extra images that will be copied to output folder
        img_list = ['SlimBootloader.txt',
            'FlashMap.txt',
            'CfgDataStitch.py',
            'CfgDataDef.yaml']
        return img_list

    def GetImageLayout (self):
        img_list = []

        acm_flag = 0 if self.ACM_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        fwu_flag = 0 if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR
        cfg_flag = 0 if len(self._CFGDATA_EXT_FILE) > 0 and self.CFGDATA_REGION_TYPE == FLASH_REGION_TYPE.BIOS else STITCH_OPS.MODE_FILE_IGNOR

        if len(self._CFGDATA_EXT_FILE) > 0 and self.CFGDATA_REGION_TYPE == FLASH_REGION_TYPE.PLATFORMDATA:
            img_list.extend ([
                ('CFGDATA_PDR.bin', [
                    ('CFGDATA.bin',   '',   self.CFGDATA_SIZE,     STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
            ])

        img_list.extend ([
            ('NON_VOLATILE.bin', [
                ('SBLRSVD.bin',    ''        , self.SBLRSVD_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ]
            ),
            ('NON_REDUNDANT.bin', [
                ('SIIPFW.bin'   ,  ''        , self.SIIPFW_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('MRCDATA.bin'  ,  ''        , self.MRCDATA_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('EPAYLOAD.bin',   ''        , self.EPAYLOAD_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('UEFIVARIABLE.bin', ''      , self.UEFI_VARIABLE_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('PAYLOAD.bin'  ,  'Lz4'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ]
            ),
            ('REDUNDANT_A.bin', [
                ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                ('KEYHASH.bin'  , ''         , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ]
            ),
            ('REDUNDANT_B.bin', [
                ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                ('KEYHASH.bin'  , ''         , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ]
            ),
            ('TOP_SWAP_A.bin', [
                ('STAGE1B_A.fd' ,  ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('UCODE.bin'    ,  ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('ACM.bin'      ,  ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                ('STAGE1A_A.fd'      , ''    , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ]
            ),
            ('TOP_SWAP_B.bin', [
                ('STAGE1B_B.fd' ,  ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('UCODE.bin'    ,  ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('ACM.bin'      ,  ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                ('STAGE1A_B.fd'      , ''    , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ]
            ),
        ])


        img_list.extend ([
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
