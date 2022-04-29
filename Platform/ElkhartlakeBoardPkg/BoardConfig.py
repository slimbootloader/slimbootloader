## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
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
from BuildLoader import BaseBoard, STITCH_OPS, FLASH_REGION_TYPE, HASH_USAGE
from BuildLoader import IPP_CRYPTO_OPTIMIZATION_MASK, IPP_CRYPTO_ALG_MASK, HASH_TYPE_VALUE

class Board(BaseBoard):
    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_EHLK '
        # VERINFO_PROJ_MAJOR_VER: 1 PV Quality release
        # VERINFO_PROJ_MINOR_VER: 0: PV  1: MR1  2: MR2 etc.
        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 3
        self.VERINFO_SVN          = 1
        self.VERINFO_BUILD_DATE   = time.strftime("%m/%d/%Y")
        self.LOWEST_SUPPORTED_FW_VER = ((self.VERINFO_PROJ_MAJOR_VER << 8) + self.VERINFO_PROJ_MINOR_VER)

        self.BOARD_NAME           = 'ehl'
        self.BOARD_PKG_NAME       = 'ElkhartlakeBoardPkg'
        self.SILICON_PKG_NAME     = 'ElkhartlakePkg'

        self.PCI_EXPRESS_BASE     = 0xC0000000
        self.PCI_IO_BASE          = 0x00002000
        self.PCI_MEM32_BASE       = 0x80000000
        self.ACPI_PM_TIMER_BASE   = 0x1808
        self.ACPI_PROCESSOR_ID_BASE = 0

        self.FLASH_BASE_ADDRESS   = 0xFF000000
        self.FLASH_BASE_SIZE      = (self.FLASH_LAYOUT_START - self.FLASH_BASE_ADDRESS)

        self.HAVE_FIT_TABLE       = 1
        self.HAVE_FSP_BIN         = 1
        self.HAVE_VBT_BIN         = 1 if self.HAVE_FSP_BIN != 0 else 0
        self.HAVE_VERIFIED_BOOT   = 1
        self.HAVE_MEASURED_BOOT   = 1
        self.HAVE_ACPI_TABLE      = 1
        self.HAVE_PSD_TABLE       = 1
        self.ENABLE_VTD           = 1
        self.ENABLE_SPLASH        = 1
        # 0: Disable  1: Enable  2: Auto (disable for UEFI payload, enable for others)
        self.ENABLE_SMM_REBASE    = 2
        self.ENABLE_FRAMEBUFFER_INIT = 1

        self.SIIPFW_SIZE = 0x1000

        self.ENABLE_TCC         = 0
        # TSN manual configuration- If enabled, user will be able to have more refined control over TSN configuration via
        # PseTsnIpConfig, TsnConfig and TsnMacAddr binaries
        self.ENABLE_TSN         = 0

        if self.ENABLE_TCC:
            self.TCC_CCFG_SIZE   = 0x00001000
            self.TCC_CRL_SIZE    = 0x00008000
            self.TCC_STREAM_SIZE = 0x00005000
            self.SIIPFW_SIZE += self.TCC_CCFG_SIZE + self.TCC_CRL_SIZE + self.TCC_STREAM_SIZE

        self.ENABLE_PRE_OS_CHECKER = 1
        if self.ENABLE_PRE_OS_CHECKER:
            self.POSC_SIZE = 0x00028000
            self.SIIPFW_SIZE += self.POSC_SIZE

        bins = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Binaries')
        self.ENABLE_PSEFW_LOADING = 1

        if self.ENABLE_PSEFW_LOADING:
            self.PSEF_SIZE = 0x00030000
            self.SIIPFW_SIZE += self.PSEF_SIZE

        if self.ENABLE_TSN:
            self.TSNC_SIZE      = 0x00001000
            self.TMAC_SIZE      = 0x00001000
            self.PSE_TSIP_SIZE  = 0x00001000 if self.ENABLE_PSEFW_LOADING == 1 else 0
            self.SIIPFW_SIZE += self.TSNC_SIZE + self.TMAC_SIZE + self.PSE_TSIP_SIZE

        if self.HAVE_FIT_TABLE:
            self.FIT_ENTRY_MAX_NUM  = 10


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

        # 0 - PCH UART0, 1 - PCH UART1, 2 - PCH UART2, 0xFF - EC UART 0x3F8
        self.DEBUG_PORT_NUMBER = 0x2

        self.STAGE1A_SIZE         = 0x00028000
        self.STAGE1B_SIZE         = 0x00120000
        self.STAGE2_SIZE          = 0x00091000

        self.ENABLE_FWU           = 1
        self.ENABLE_CSME_UPDATE   = 1
        self.ENABLE_SMBIOS        = 1

        # CSME update library is required to enable this option and will be available as part of CSME kit
        self.BUILD_CSME_UPDATE_DRIVER   = 0

        self.STAGE1B_XIP          = 1

        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x00100000

        self.STAGE1_STACK_SIZE    = 0x00020000
        self.STAGE1_DATA_SIZE     = 0x00015000

        # if PAYLOAD_LOAD_HIGH is 1, PAYLOAD_EXE_BASE will be ignored
        self.PAYLOAD_LOAD_HIGH    = 1
        self.PAYLOAD_EXE_BASE     = 0x00B00000
        self.PAYLOAD_SIZE         = 0x00020000
        self.EPAYLOAD_SIZE        = 0x00162000
        self.UCODE_SIZE           = 0x00010000 if self.HAVE_FSP_BIN != 0 else 0
        self.MRCDATA_SIZE         = 0x00008000
        self.CFGDATA_SIZE         = 0x00004000
        self.KEYHASH_SIZE         = 0x00001000
        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE + 0x1000
        self.VARIABLE_SIZE        = 0x00002000
        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.UEFI_VARIABLE_SIZE = 0x00040000
        else:
            self.UEFI_VARIABLE_SIZE = 0x00001000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x0001B000 if self.ENABLE_FWU else 0

        self.TOP_SWAP_SIZE        = 0x080000
        self.REDUNDANT_SIZE       = 0x360000
        self.NON_VOLATILE_SIZE    = 0x001000
        self.SLIMBOOTLOADER_SIZE  = 0xD00000
        self.NON_REDUNDANT_SIZE   = self.SLIMBOOTLOADER_SIZE - \
                                    (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 - \
                                    self.NON_VOLATILE_SIZE

        # TBD: ACM/KM/BPM Size, as of Sep 2017
        #      ACM size is fixed 100KB, KM size is fixed 0x400, BPM size is fixed 0x600
        self.KM_SIZE              = 0x00000400
        self.BPM_SIZE             = 0x00000600
        self.ACM_SIZE             = 0x00020000 + self.KM_SIZE + self.BPM_SIZE
        # adjust ACM_SIZE to meet 128KB alignment (to align 100KB ACM size)
        if self.ACM_SIZE > 0:
            acm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE
            acm_btm = acm_top - self.ACM_SIZE
            acm_btm = (acm_btm & 0xFFFE0000)
            self.ACM_SIZE     = acm_top - acm_btm

        self.CFGDATA_REGION_TYPE  = FLASH_REGION_TYPE.BIOS
        self.SPI_IAS_REGION_TYPE  = FLASH_REGION_TYPE.BIOS

        self.SPI_IAS1_SIZE        = 0x0
        self.SPI_IAS2_SIZE        = 0x0

        self.PLD_HEAP_SIZE        = 0x04000000
        self.PLD_STACK_SIZE       = 0x00020000
        self.PLD_RSVD_MEM_SIZE    = 0x00500000
        self.LOADER_RSVD_MEM_SIZE = 0x500000

        # _CFGDATA_INT_FILE - Internal cfg data is generally used for internal boards like MRBs, RVPs etc.
        # _CFGDATA_EXT_FILE - External cfg data is for the customer boards to populate new data on top of the internal defaults.
        # Cfg data dlt files for internal boards could also put into external cfg data if want to update cfg data for these platforms
        # for test purpose. Based on the platform id, relevant data is populated for each platform.
        self._generated_cfg_file_prefix = 'Autogen_'
        self._CFGDATA_INT_FILE = []
        self._CFGDATA_EXT_FILE = [self._generated_cfg_file_prefix + 'CfgData_Int_IotgRvp1.dlt', self._generated_cfg_file_prefix + 'CfgData_Ext_IotgCrb.dlt', self._generated_cfg_file_prefix + 'CfgData_Ext_Up6000.dlt']

    def PlatformBuildHook (self, build, phase):
        if phase == 'pre-build:before':
            # create build folder if not exist
            if not os.path.exists(build._fv_dir):
                os.makedirs(build._fv_dir)

            # Generate the dlt files based on feature
            brd_cfg_src_dir = os.path.join(os.environ['PLT_SOURCE'], 'Platform', self.BOARD_PKG_NAME, 'CfgData')
            for dlt_file in self._CFGDATA_EXT_FILE:
                cfg_dlt_file  = os.path.join(brd_cfg_src_dir, dlt_file[len (self._generated_cfg_file_prefix):])
                lines         = open (cfg_dlt_file).read()

                # Enable TCC in dlt file
                if self.ENABLE_TCC:
                    lines += open (os.path.join(brd_cfg_src_dir, 'CfgData_Tcc_Feature.dlt')).read()

                # Write to generated final dlt file
                output_cfg_dlt_file = os.path.join(build._fv_dir, dlt_file)
                open(output_cfg_dlt_file, 'w').write(lines)

    def GetPlatformDsc (self):
        dsc = {}
        # These libraries will be added into the DSC files
        common_libs = [
            'LoaderLib|Platform/CommonBoardPkg/Library/LoaderLib/LoaderLib.inf',
            'PchInfoLib|Silicon/$(SILICON_PKG_NAME)/Library/PchInfoLib/PchInfoLib.inf',
            'PchSbiAccessLib|Silicon/CommonSocPkg/Library/PchSbiAccessLib/PchSbiAccessLib.inf',
            'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
            'TccLib|Silicon/CommonSocPkg/Library/TccLib/TccLib.inf',
            'GpioLib|Silicon/CommonSocPkg/Library/GpioLib/GpioLib.inf',
            'GpioSiLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioSiLib/GpioSiLib.inf',
            'PchSpiLib|Silicon/CommonSocPkg/Library/PchSpiLib/PchSpiLib.inf',
            'SpiFlashLib|Silicon/CommonSocPkg/Library/SpiFlashLib/SpiFlashLib.inf',
            'VtdLib|Silicon/$(SILICON_PKG_NAME)/Library/VTdLib/VTdLib.inf',
            'ShellExtensionLib|Platform/$(BOARD_PKG_NAME)/Library/ShellExtensionLib/ShellExtensionLib.inf',
            'IgdOpRegionLib|Silicon/CommonSocPkg/Library/IgdOpRegionLib/IgdOpRegionLib.inf',
            'BootGuardLib|Silicon/CommonSocPkg/Library/BootGuardLibCBnT/BootGuardLibCBnT.inf',
            'BdatLib|Silicon/CommonSocPkg/Library/BdatLib/BdatLib.inf',
            'PchSciLib|Silicon/$(SILICON_PKG_NAME)/Library/PchSciLib/PchSciLib.inf',
            'SmbusLib|Silicon/CommonSocPkg/Library/SmbusLib/SmbusLib.inf',
            'PsdLib|Silicon/$(SILICON_PKG_NAME)/Library/PsdLib/PsdLib.inf',
            'HeciInitLib|Silicon/$(SILICON_PKG_NAME)/Library/HeciInitLib/HeciInitLib.inf',
            'MeExtMeasurementLib|Silicon/$(SILICON_PKG_NAME)/Library/MeExtMeasurementLib/MeExtMeasurementLib.inf'
        ]

        if self.BUILD_CSME_UPDATE_DRIVER:
            common_libs.append ('MeFwUpdateLib|Silicon/$(SILICON_PKG_NAME)/Library/MeFwUpdateLib/MeFwUpdateLib.inf')
        dsc['LibraryClasses.%s' % self.BUILD_ARCH] = common_libs
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
          # Name | Image File         |    CompressAlg    | AuthType                            | Key File                  | Region Align   | Region Size    |  Svn Info
          # ===============================================================================================================================================================
          ('IPFW',      'SIIPFW.bin',          '',     container_list_auth_type,   'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,    0,           0        ,      0),   # Container Header
        )

        bins = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Binaries')

        CompFilePreOsChecker='PreOsChecker.bin' if os.path.exists(os.path.join(bins, 'PreOsChecker.bin')) else ''
        CompFilePseFw='PseFw.bin' if os.path.exists(os.path.join(bins, 'PseFw.bin')) else ''
        CompFilePseTsnIpConfig='PseTsnIpConfig.bin' if os.path.exists(os.path.join(bins, 'PseTsnIpConfig.bin')) else ''
        CompFileTsnConfig='TsnConfig.bin' if os.path.exists(os.path.join(bins, 'TsnConfig.bin')) else ''
        CompFileTsnMacAddr='TsnMacAddr.bin' if os.path.exists(os.path.join(bins, 'TsnMacAddr.bin')) else ''
        CompFileCrlFw='crl.bin' if os.path.exists(os.path.join(bins, 'crl.bin')) else ''

        if self.ENABLE_TCC:
            container_list.append (
              ('TCCC', '',     'Lz4',   container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE,    0,   self.TCC_CCFG_SIZE,  0),   # TCC Cache Config
            )
            container_list.append (
              ('TCCM', CompFileCrlFw,    'Lz4',   container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE,    0,   self.TCC_CRL_SIZE,   0),   # TCC CRL
            )
            container_list.append (
              ('TCCT',  '',   'Lz4',   container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE,    0,   self.TCC_STREAM_SIZE,0),   # TCC Stream Config
            )

        if self.ENABLE_PRE_OS_CHECKER:
            container_list.append (
              ('POSC',CompFilePreOsChecker,      '',     container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0,   self.POSC_SIZE,      0),   # Pre-OS Checker
            )

        if self.ENABLE_PSEFW_LOADING:
            container_list.append (
              ('PSEF',CompFilePseFw,          'Lz4',     container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0,   self.PSEF_SIZE,      0),   # OSE FW
            )

        if self.ENABLE_TSN:
            container_list.append (
              ('TSNC',CompFileTsnConfig,      'Lz4',     container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0,   self.TSNC_SIZE,      0),   # TSN Config
            )
            container_list.append (
              ('TMAC',CompFileTsnMacAddr,     'Lz4',     container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0,   self.TMAC_SIZE,      0),   # TSN MAC Address
            )
            if self.ENABLE_PSEFW_LOADING:
                container_list.append (
                  ('TSIP',CompFilePseTsnIpConfig, 'Lz4',     container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0,   self.PSE_TSIP_SIZE,      0),   # PSE TSN IP
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

        acm_flag = 0 if self.ACM_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        ias1_flag = 0 if self.SPI_IAS1_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        ias2_flag = 0 if self.SPI_IAS2_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        fwu_flag = 0 if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR
        if len(self._CFGDATA_EXT_FILE) > 0 and self.CFGDATA_REGION_TYPE == FLASH_REGION_TYPE.BIOS:
            cfg_flag = 0
        else:
            cfg_flag = STITCH_OPS.MODE_FILE_IGNOR

        img_list = []
        if len(self._CFGDATA_EXT_FILE) > 0 and self.CFGDATA_REGION_TYPE == FLASH_REGION_TYPE.PLATFORMDATA:
            img_list.extend ([
                ('CfgDataPdr.bin', [
                    ('CFGDATA.bin',   '',   self.CFGDATA_SIZE,     STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
            ])

        # output files need to have unique base name (excluding file extension)
        # output files ends with 'rom' extension will be copied over for final stitching
        ucode_flag = 0 if self.HAVE_FSP_BIN else STITCH_OPS.MODE_FILE_IGNOR
        img_list.extend ([
                ('NON_VOLATILE.bin', [
                    ('SBLRSVD.bin',    ''        , self.SBLRSVD_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('NON_REDUNDANT.bin', [
                    ('SIIPFW.bin'   ,  ''        , self.SIIPFW_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('SPI_IAS2.bin' ,  ''        , self.SPI_IAS2_SIZE, STITCH_OPS.MODE_FILE_PAD | ias2_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('SPI_IAS1.bin' ,  ''        , self.SPI_IAS1_SIZE, STITCH_OPS.MODE_FILE_PAD | ias1_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('MRCDATA.bin'  ,  ''        , self.MRCDATA_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('EPAYLOAD.bin' ,  ''        , self.EPAYLOAD_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('UEFIVARIABLE.bin', ''      , self.UEFI_VARIABLE_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ('PAYLOAD.bin'  ,  'Lzma'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('REDUNDANT_A.bin', [
                    ('UCODE.bin'    ,  ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD | ucode_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  ,  ''        , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  , ''         , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B_A.fd' ,  ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('REDUNDANT_B.bin', [
                    ('UCODE.bin'    ,  ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD | ucode_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE2.fd'    ,  'Lz4'     , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' ,  'Lzma'    , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  ,  ''        , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  , ''         , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1B_B.fd' ,  ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_A.bin', [
                    ('ACM.bin'      ,  ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A_A.fd' ,  ''        , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_B.bin', [
                    ('ACM.bin'      ,  ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A_B.fd' ,  ''        , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
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
