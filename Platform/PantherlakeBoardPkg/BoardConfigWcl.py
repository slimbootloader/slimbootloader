## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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

        self.VERINFO_IMAGE_ID     = 'SBL_WCL'
        self.VERINFO_PROJ_MAJOR_VER = 0
        self.VERINFO_PROJ_MINOR_VER = 5
        self.VERINFO_SVN            = 1
        self.VERINFO_BUILD_DATE     = time.strftime("%m/%d/%Y")

        self.BUILD_ARCH           = 'X64'
        self.BOARD_NAME           = 'wcl'
        self.BOARD_PKG_NAME       = 'PantherlakeBoardPkg'
        self.SILICON_PKG_NAME     = 'PantherlakePkg'
        self.FSP_IMAGE_ID         = '$WCLFSPE'
        self.HAVE_FSP_AT_RESET    = False
        self.HAVE_SIGNED_FSP      = False
        self._EXTRA_INC_PATH      = ['Silicon/PantherlakePkg/Wcl/Fsp']
        self._FSP_PATH_NAME       = 'Silicon/PantherlakePkg/Wcl/Fsp'
        self._SMBIOS_YAML_FILE    = os.path.join('Platform', self.BOARD_PKG_NAME, 'SmbiosStrings.yaml')
        self.FSP_INF_FILE         = 'Silicon/PantherlakePkg/Wcl/Fsp/FspBin.inf'
        self.MICROCODE_INF_FILE   = 'Silicon/PantherlakePkg/Wcl/Microcode/Microcode.inf'
        self._LP_SUPPORT          = False
        self._N_SUPPORT           = False
        self.BOARD_ID             = 3

        self.PCI_EXPRESS_BASE     = 0xE0000000
        self.PCI_IO_BASE          = 0x00002000
        self.PCI_MEM32_BASE       = 0xA0000000
        self.PCI_MEM64_BASE       = 0x9000000000
        self._PCI_ENUM_DOWNGRADE_MEM64  = 1
        self._PCI_ENUM_DOWNGRADE_PMEM64 = 0
        self._PCI_ENUM_DOWNGRADE_BUS0   = 2
        self.ACPI_PM_TIMER_BASE   = 0x1808

        self.FLASH_LAYOUT_START   = 0x100000000 # 4GB Top
        self.FLASH_BASE_SIZE      = 0x02000000  # 32MB
        self.FLASH_BASE_ADDRESS   = (self.FLASH_LAYOUT_START - self.FLASH_BASE_SIZE)
        self.LOADER_ACPI_RECLAIM_MEM_SIZE = 0x000090000
        self.HAVE_FIT_TABLE       = 1
        self.HAVE_VBT_BIN         = 1
        self.HAVE_VERIFIED_BOOT   = 1
        self.HAVE_MEASURED_BOOT   = 1
        self.HAVE_FLASH_MAP       = 1
        self.HAVE_ACPI_TABLE      = 1
        self.HAVE_PSD_TABLE       = 1
        self.ENABLE_SPLASH            = 1
        self.ENABLE_FRAMEBUFFER_INIT  = 1
        self.ENABLE_VTD           = 1
        self.ENABLE_PCIE_PM       = 1
        # 0: Disable  1: Enable  2: Auto (disable for UEFI payload, enable for others)
        # 3: Enable and won't set SMRR 4: Auto and won't set SMRR
        self.ENABLE_SMM_REBASE    = 4
        # 0: Disable 1: Enable IPP Crypto performance measurement
        self.ENABLE_IPP_CRYPTO_PERF = 0
        self.ENABLE_FIPS_SELFTEST   = 0
        # 0: Disable 1: Enable TXT
        self.TXT_ENABLED            = 0
        self.PID_RTC_HOST_NUM       = 0x6C

        # 0 - PCH UART0, 1 - PCH UART1, 2 - PCH UART2
        self.DEBUG_PORT_NUMBER = 0

        self.ENABLE_MULTI_USB_BOOT_DEV = 1

        self.CPU_MAX_LOGICAL_PROCESSOR_NUMBER = 32

        self.SUPPORT_X2APIC = 1
        self.MADT_USE_PLATFORM_LAPIC = 1

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
        # Y8 for SSE4.2 instructions
        self.ENABLE_CRYPTO_SHA_OPT  = IPP_CRYPTO_OPTIMIZATION_MASK['X64_Y8']

        if self.ENABLE_IPP_CRYPTO_PERF:
            self.IPP_HASH_LIB_SUPPORTED_MASK |= IPP_CRYPTO_ALG_MASK['SM3_256']

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

        self.STAGE1A_SIZE         = 0x00040000
        self.STAGE1_STACK_SIZE    = 0x00002000
        self.STAGE1_DATA_SIZE     = 0x00014000
        self.FSP_M_STACK_TOP      = 0xFEF7FF00
        self.STAGE1B_SIZE         = 0x00200000
        self.STAGE2_SIZE          = 0x000C1000
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x001F0000
        self.PAYLOAD_SIZE         = 0x00034000
        self.OS_LOADER_FD_SIZE    = 0x00067000
        self.STAGE1_STACK_BASE_OFFSET = 0x00010000

        if self.NO_OPT_MODE:
            self.PAYLOAD_SIZE      = 0x0003C000
            self.STAGE1A_SIZE      = 0x00052000
            self.OS_LOADER_FD_SIZE = 0x000A1000
        self.EPAYLOAD_SIZE        = 0x00236000

        self.ENABLE_FAST_BOOT = 0
        if self.ENABLE_FAST_BOOT:
            self.ENABLE_SPLASH              = 0
            self.ENABLE_FRAMEBUFFER_INIT    = 0
            self.RELEASE_MODE               = 1
            self.HAVE_VERIFIED_BOOT         = 0
            self.HAVE_MEASURED_BOOT         = 0
            self.VERIFIED_BOOT_HASH_MASK    = 0

        if self.RELEASE_MODE and self.ENABLE_FAST_BOOT:
            self.STAGE1A_SIZE         = 0x00018000
            self.STAGE1B_SIZE         = 0x0012E000
            self.STAGE2_SIZE          = 0x000C0000
            self.STAGE2_FD_SIZE       = 0x000FE000
            self.PAYLOAD_SIZE         = 0x00027000

        self.ENABLE_UI_SETUP   = 0
        if self.ENABLE_UI_SETUP:
            self.PAYLOAD_SIZE            += 0x00058000
            self.OS_LOADER_FD_SIZE       += 0x00170000
            self.CONSOLE_OUT_DEVICE_MASK  = 0x00000003
            self.CONSOLE_IN_DEVICE_MASK   = 0x00000003
            self.ENABLE_USB_KB            = 1

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

        self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE

        # If ENABLE_SBL_RESILIENCY is 1, BiosRedAssistance FIT strap setting
        # needs to be manually changed to Enabled and TopSwapOverride
        # flash setting needs to be manually changed to 4MB in stitch config
        self.ENABLE_SBL_RESILIENCY = 0
        self.BUILD_IDENTICAL_TS    = 1

        if self.ENABLE_SBL_RESILIENCY:
            self.BUILD_IDENTICAL_TS    = 1

        if self.BUILD_IDENTICAL_TS:
            self.TOP_SWAP_SIZE         = 0x400000
            self.REDUNDANT_SIZE        = self.STAGE2_SIZE + self.FWUPDATE_SIZE + self.CFGDATA_SIZE + \
                                         self.KEYHASH_SIZE
        else:
            self.TOP_SWAP_SIZE        = 0x100000
            self.REDUNDANT_SIZE       = self.UCODE_SIZE + self.STAGE2_SIZE + self.STAGE1B_SIZE + \
                                        self.FWUPDATE_SIZE + self.CFGDATA_SIZE + self.KEYHASH_SIZE

        self.SIIPFW_SIZE = 0x1000

        if self._SMBIOS_YAML_FILE:
            self.SIIPFW_SIZE += 0x1000

        self.FUSA_SUPPORT         = 0
        self.FUSA_SIZE            = 0

        # TCC on WCL doesn't introduce any build changes except a small amount of init code.
        # It's fine to always leave this enabled. The TCC feature can be enabled in the SBL Config.
        # Disabling this will very slightly reduce the FspUpdUpdateLib size
        self.ENABLE_TCC = 1

        if self.FUSA_SUPPORT:
            self.ENABLE_PRE_OS_CHECKER = 0
            if self.ENABLE_PRE_OS_CHECKER:
                self.POSC_SIZE = 0x00080000
                self.SIIPFW_SIZE += self.POSC_SIZE
            self.FUSA_BIST_PATTERN_SUPPORT  = 0
            if self.FUSA_BIST_PATTERN_SUPPORT:
                self.FSBP_SIZE        = 0x1000000
                self.FUSA_SIZE        = 0x1000 + self.FSBP_SIZE

            # Allow TCC to be enabled by FuSa dlt file
            FusaConfig = ''

            brd_cfg_src_dir = os.path.join(os.environ['PLT_SOURCE'], 'Platform', self.BOARD_PKG_NAME, 'CfgData')
            brd_cfg2_src_dir = '.'
            if hasattr(self, 'BOARD_PKG_NAME_OVERRIDE'):
                brd_cfg2_src_dir = os.path.join(os.environ['PLT_SOURCE'], 'Platform', self.BOARD_PKG_NAME_OVERRIDE, 'CfgData')

            if os.path.exists(os.path.join(brd_cfg_src_dir, 'CfgData_Fusa_Feature.dlt')):
                FusaConfig = open (os.path.join(brd_cfg_src_dir, 'CfgData_Fusa_Feature.dlt')).readlines()
            else:
                if os.path.exists(os.path.join(brd_cfg2_src_dir, 'CfgData_Fusa_Feature.dlt')):
                    FusaConfig = open (os.path.join(brd_cfg2_src_dir, 'CfgData_Fusa_Feature.dlt')).readlines()

            for line in FusaConfig:
                if (re.search("TCC_CFG_DATA\.TccEnable\s+\|\s*1",line) != None or
                    re.search("TCC_CFG_DATA\.TccEnable\s+\|\s*0x0*1",line) != None):
                    # use setattr() to avoid matching release.py regex
                    setattr(self, 'ENABLE_TCC', 1)

        self.NON_REDUNDANT_SIZE   = 0x3BF000 + self.SIIPFW_SIZE + self.FUSA_SIZE
        self.NON_VOLATILE_SIZE    = 0x001000
        self.SLIMBOOTLOADER_SIZE  = (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 + \
                                    self.NON_REDUNDANT_SIZE + self.NON_VOLATILE_SIZE
        self.SLIMBOOTLOADER_SIZE  = ((self.SLIMBOOTLOADER_SIZE + 0xFFFFF) & ~0xFFFFF)
        self.PLD_HEAP_SIZE        = 0x09000000
        # self.PLD_HEAP_SIZE        = 0x04000000
        self.PLD_STACK_SIZE       = 0x00020000
        self.PLD_RSVD_MEM_SIZE    = 0x00500000

        if self.FUSA_SUPPORT:
            # FuSa validation requires prebuilt images that can boot OS Loader Ubuntu
            self.PLD_HEAP_SIZE        = 0x09000000
            self.DIAGNOSTICACM_SIZE   = 0x00008000
            self.DIAGNOSTICACM_BASE   = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE - self.DIAGNOSTICACM_SIZE

        self.KM_SIZE              = 0x00000400
        self.BPM_SIZE             = 0x00000600
        self.ACM_SIZE             = 0x00048000 + self.KM_SIZE + self.BPM_SIZE
        # adjust ACM_SIZE to meet 512KB alignment (to align 288KB ACM size)
        if self.ACM_SIZE > 0:
            acm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE - self.DIAGNOSTICACM_SIZE
            acm_btm = acm_top - self.ACM_SIZE
            acm_btm = (acm_btm & 0xFFF80000)
            self.ACM_SIZE     = acm_top - acm_btm

        # ACM FIT Type 2 version from LNC is 0x200
        self.ACM_FIT_VERISON      = 0x200
        self._ACM_CPU_FMS = [
                                    0xF3FFD065,
                                    0xF3FFC06D,
                                    0xF3FFC06C
        ]

        self.LOADER_RSVD_MEM_SIZE = 0x600000

        if self.ENABLE_IPP_CRYPTO_PERF:
            self.LOADER_RSVD_MEM_SIZE += 0x01000000

        # large BIST test patterns increase heap usage significantly. round to 1MB
        if (2*self.FUSA_SIZE & 0xFFFFF) != 0:
            self.LOADER_RSVD_MEM_SIZE += (2*self.FUSA_SIZE & ~0xFFFFF) + 0x100000
        else:
            self.LOADER_RSVD_MEM_SIZE += 2*self.FUSA_SIZE

        # If mulitple VBT table support is required, list them as:
        #   {VbtImageId1 : VbtFileName1, VbtImageId2 : VbtFileName2, ...}
        # VbtImageId is ID to identify a VBT image. It is a UINT32 number to match
        #   the ImageId field in the VBT container.
        # VbtFileName is the VBT file name. It needs to be located under platform
        #   VbtBin folder.
        self._MULTI_VBT_FILE      = {1:'VbtDdr5Crb.dat', 2:'VbtLp5Rvp.dat'}

        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE
        self._generated_cfg_file_prefix = 'Autogen_'
        self._CFGDATA_DEF_FILE = 'CfgDataDef_Wcl.yaml'
        self._CFGDATA_INT_FILE = []
        self._CFGDATA_EXT_FILE = [self._generated_cfg_file_prefix + 'CfgDataInt_Wcl_DDR5_Crb.dlt',
                                  self._generated_cfg_file_prefix + 'CfgDataInt_Wcl_LPDDR5_Rvp.dlt',
                                  self._generated_cfg_file_prefix + 'CfgDataInt_Wcl_DDR5_Rvp.dlt']

        self.ENABLE_SBL_SETUP       = 0
        self.SETUP_SIZE             = 0

        if self.ENABLE_SBL_SETUP:
            self.HAVE_VERIFIED_BOOT = 0
            self.HAVE_MEASURED_BOOT = 0
            self.SETUP_MPYM_SIZE    = 0x12000
            self.SETUP_STPY_SIZE    = 0x06000
            self.SETUP_CFGJ_SIZE    = 0x0B000
            self.SETUP_CFGD_SIZE    = 0x02000

            self.SETUP_SIZE           = 0x1000 + self.SETUP_MPYM_SIZE + self.SETUP_STPY_SIZE + self.SETUP_CFGJ_SIZE + self.SETUP_CFGD_SIZE

    def GetIppCryptoInf(self):
        ipp_crypto_opt_lvl = 0
        ipp_crypto_opt_name = ''
        ipp_crypto_inf = os.path.join('BootloaderCommonPkg', 'Library', 'IppCrypto2Lib', 'IppCrypto2Lib.inf')
        for k,v in IPP_CRYPTO_OPTIMIZATION_MASK.items():
            if self.ENABLE_CRYPTO_SHA_OPT & v:
                if v > ipp_crypto_opt_lvl:
                    ipp_crypto_opt_lvl = v
                    ipp_crypto_opt_name = k

        if ipp_crypto_opt_name[-2:]:
            ipp_crypto_inf = os.path.join('BootloaderCommonPkg', 'Library', 'IppCrypto2Lib', 'IppCrypto2Lib%s.inf' % ipp_crypto_opt_name[-2:])

        return ipp_crypto_inf

    def PlatformBuildHook (self, build, phase):
        global fv_dir
        fv_dir = build._fv_dir
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

                if self.FUSA_SUPPORT:
                    if os.path.exists(os.path.join(brd_cfg_src_dir, 'CfgData_Fusa_Feature.dlt')):
                        lines += open (os.path.join(brd_cfg_src_dir, 'CfgData_Fusa_Feature.dlt')).read()

                    if self.ENABLE_PRE_OS_CHECKER:
                        if os.path.exists(os.path.join(brd_cfg_src_dir, 'CfgData_Posc_Feature.dlt')):
                            lines += open (os.path.join(brd_cfg_src_dir, 'CfgData_Posc_Feature.dlt')).read()

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
            'GpioV2Lib|Silicon/CommonSocPkg/Library/GpioV2Lib/GpioV2Lib.inf',
            'P2sbLib|Silicon/CommonSocPkg/Library/P2sbLib/P2sbLib.inf',
            'GpioV2SiLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioV2SiLib/GpioV2SiLib.inf',
            'PchSbiAccessLib|Silicon/CommonSocPkg/Library/PchSbiAccessLib/PchSbiAccessLib.inf',
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
            'SerialIoI2cLib|Silicon/CommonSocPkg/Library/BaseSerialIoI2cLib/BaseSerialIoI2cLib.inf',
            'HeciLib|Silicon/CommonSocPkg/Library/HeciLib/HeciLib.inf',
            'MeChipsetLib|Silicon/CommonSocPkg/Library/MeChipsetLib/MeChipsetLib.inf',
            'VtdLib|Silicon/$(SILICON_PKG_NAME)/Library/VTdLib/VTdLib.inf',
            'DmarLib|Silicon/CommonSocPkg/Library/DmarLib/DmarLib.inf',
            'MadtLib|Silicon/CommonSocPkg/Library/MadtLib/MadtLib.inf',
            'McfgLib|Silicon/CommonSocPkg/Library/McfgLib/McfgLib.inf',
            'PsdLib|Silicon/CommonSocPkg/Library/PsdLib/PsdLib.inf',
            'HeciMeExtLib|Silicon/CommonSocPkg/Library/HeciMeExtLib/HeciMeExtLib.inf',
            'MeExtMeasurementLib|Silicon/$(SILICON_PKG_NAME)/Library/MeExtMeasurementLib/MeExtMeasurementLib.inf',
            'WatchDogTimerLib|Silicon/CommonSocPkg/Library/WatchDogTimerLib/WatchDogTimerLib.inf',
            'TcoTimerLib|Silicon/CommonSocPkg/Library/TcoTimerLib/TcoTimerLib.inf',
            'TopSwapLib|Silicon/CommonSocPkg/Library/TopSwapLib/TopSwapLib.inf',
            'AmtLib|Silicon/CommonSocPkg/Library/AmtLib/AmtLib.inf',
            'TxtLib|Silicon/$(SILICON_PKG_NAME)/Library/TxtLib/TxtLib.inf',
            'CryptoLib|%s' % self.GetIppCryptoInf()
        ]

        if self.ENABLE_PCIE_PM:
            lib = [
            'PciePm|Silicon/$(SILICON_PKG_NAME)/Library/PciePm/PciePm.inf',
            'PciExpressHelpersLib|Silicon/CommonSocPkg/Library/PciExpressHelpersLibrary/PciExpressHelpersLibrary.inf',
            'BasePcieHelperLib|Silicon/CommonSocPkg/Library/BasePcieHelperLib/BasePcieHelperLib.inf',
            'PcieRpLib|Silicon/CommonSocPkg/Library/PcieRpLib/PcieRpLib.inf',
            ]
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].extend (lib)
        else:
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('PciePm|Silicon/CommonSocPkg/Library/PciePmNull/PciePmNull.inf')

        if self.BUILD_CSME_UPDATE_DRIVER:
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('MeFwUpdateLib|Silicon/$(SILICON_PKG_NAME)/Library/MeFwUpdateLib/MeFwUpdateLib.inf')

        if self.FUSA_SUPPORT:
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('CrashLogLib|Silicon/PantherlakePkg/Library/FusaCrashLogLib/FusaCrashLogLib.inf')
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('FusaConfigLib|Silicon/PantherlakePkg/Library/FusaConfigLib/FusaConfigLib.inf')
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('PchGspiLib|Silicon/PantherlakePkg/Library/PchGspiLib/PchGspiLib.inf')

        dsc['PcdsFixedAtBuild'] = ['gPlatformModuleTokenSpaceGuid.PcdAcpiTablesMaxEntry | 40']

        if self.ENABLE_FIPS_SELFTEST:
            dsc['PcdsFixedAtBuild'].append ('gPlatformCommonLibTokenSpaceGuid.PcdFipsSupport | TRUE')

        dsc['PcdsFixedAtBuild.%s' % self.BUILD_ARCH] = [
            'gPlatformModuleTokenSpaceGuid.PcdFusaSupport | $(FUSA_SUPPORT)'
        ]

        dsc['PcdsFixedAtBuild'].append ('gPlatformCommonLibTokenSpaceGuid.PcdIppcrypto2Lib | TRUE')

        dsc['PcdsFixedAtBuild'].append ('gPlatformPantherLakeTokenSpaceGuid.PcdWclSupport | TRUE')

        # Update UFS ref clock frequency to 38.4 MHz
        dsc['PcdsFixedAtBuild'].append ('gPlatformCommonLibTokenSpaceGuid.PcdUfsRefClockFrequency | 0x2')
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
        bins = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Binaries')
        container_list.append ([
          # Name   | Image File | CompressAlg | AuthType                | Key File                                   | Region Align | Region Size |  Svn Info
          # ========================================================================================================================================================
          ('IPFW', 'SIIPFW.bin',  '',          container_list_auth_type,  'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,  0,             0,            0), # Container Header
          ('SMBS', 'smbios.bin',  'Dummy',     container_list_auth_type,  'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,  0,             0x1000,       0),  # SMBIOS Component
        ])

        if self.FUSA_SUPPORT:
            if self.ENABLE_PRE_OS_CHECKER:
                CompFilePreOsChecker= os.path.join(bins, 'PreOsChecker.bin') if os.path.exists(os.path.join(bins, 'PreOsChecker.bin')) else ''
                container_list[0].append (
                    ('POSC',CompFilePreOsChecker,  'Lz4',     container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0,   self.POSC_SIZE,      0),   # Pre-OS Checker
                )

            if self.FUSA_BIST_PATTERN_SUPPORT:
                CompFileStartupBist= os.path.join(bins, 'FusaStartupBist.fv') if os.path.exists(os.path.join(bins, 'FusaStartupBist.fv')) else ''
                container_list.append (
                # Name | Image File            | CompressAlg | AuthType                 |            Key File                               | Align | Region Size | Svn Info
                # ========================================================================================================================================================
                [('FUFW', 'FUFW.bin',               '',       container_list_auth_type,  'KEY_ID_CONTAINER'      + '_'+self._RSA_SIGN_TYPE,   0,         0,          0)], # Container Header
                )
                container_list[1].append (
                ('FSBP', CompFileStartupBist,     'Lz4',    container_list_auth_type,  'KEY_ID_CONTAINER_COMP' + '_'+self._RSA_SIGN_TYPE,   0,     self.FSBP_SIZE, 0),  # FuSa Startup BIST Pattern file
                )

        if self.ENABLE_SBL_SETUP:
            def_auth = container_list_auth_type
            cont_key = 'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE
            mpy_path  = os.path.join(bins, 'MicroPython.efi')
            if not os.path.isfile(mpy_path):
                raise Exception ("MicroPython.efi is required under 'Binaries', please build MicroPython payload module separately !")
            sbl_setup = os.path.join(os.environ['SBL_SOURCE'], 'BootloaderCorePkg', 'Tools', 'SblSetup.py')
            container_list.append ([
                # Name       | Image File |    CompressAlg  | AuthType    | Key File  | Region Align | Region Size          |  Svn Info
                # ==================================================================================================================================================================
                ('SETP',     'SETP.bin',       '',           def_auth,      cont_key,   0,             0,                      0),   # Container Header
                ('MPYM',      mpy_path,        'Lzma',       'SHA2_384',    '',         0,             self.SETUP_MPYM_SIZE,   0),   # Component 1
                ('STPY',      sbl_setup,       'Lz4',        'SHA2_384',    '',         0,             self.SETUP_STPY_SIZE,   0),   # Component 2
                ('CFGJ',     'CfgDataDef.json', 'Lzma',      'SHA2_384',    '',         0,             self.SETUP_CFGJ_SIZE,   0),   # Component 3
                ('CFGD',     '',               'Dummy',      '',            '',         0,             self.SETUP_CFGD_SIZE,   0),   # Component 4
            ])

        return container_list

    def GetOutputImages (self):
        # define extra images that will be copied to output folder
        img_list = ['SlimBootloader.txt',
            'FlashMap.txt',
            'CfgDataStitch.py',
            'CfgDataDef.yaml',
            'Bsis.bin']
        return img_list

    def GetImageLayout (self):
        img_list = []

        acm_flag = 0 if self.ACM_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
        fwu_flag = 0 if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR
        diagnosticacm_flag = 0 if self.DIAGNOSTICACM_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
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
        ])

        non_redundant_comp = []
        if self.FUSA_SUPPORT and self.FUSA_BIST_PATTERN_SUPPORT:
            non_redundant_comp = [('FUFW.bin'          , '' , self.FUSA_SIZE,          STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_HEAD)]

        non_redundant_comp.extend ([
                ('SIIPFW.bin'   ,  ''        , self.SIIPFW_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('VARIABLE.bin' ,  ''        , self.VARIABLE_SIZE, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('MRCDATA.bin'  ,  ''        , self.MRCDATA_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('EPAYLOAD.bin',   ''        , self.EPAYLOAD_SIZE, STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ('UEFIVARIABLE.bin', ''      , self.UEFI_VARIABLE_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                ('PAYLOAD.bin'  ,  'Lz4'    , self.PAYLOAD_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                ])

        img_list.extend ([
            ('NON_REDUNDANT.bin', non_redundant_comp)
        ])

        if self.BUILD_IDENTICAL_TS:
            img_list.extend ([
                ('REDUNDANT_A.bin', [
                    ('STAGE2.fd'    , 'Lz4'      , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' , 'Lzma'     , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  , ''        , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('REDUNDANT_B.bin', [
                    ('STAGE2.fd'    , 'Lz4'      , self.STAGE2_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('FWUPDATE.bin' , 'Lzma'     , self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag,  STITCH_OPS.MODE_POS_TAIL),
                    ('CFGDATA.bin'  , ''         , self.CFGDATA_SIZE,  STITCH_OPS.MODE_FILE_PAD | cfg_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('KEYHASH.bin'  , ''        , self.KEYHASH_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_A.bin', [
                    ('STAGE1B_A.fd' , ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('UCODE.bin'    , ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('ACM.bin'      , ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A_A.fd' , ''        , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_B.bin', [
                    ('STAGE1B_B.fd' , ''        , self.STAGE1B_SIZE,  STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('UCODE.bin'    , ''        , self.UCODE_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL),
                    ('ACM.bin'      , ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A_B.fd' , ''        , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
            ])
        else:
            img_list.extend ([
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
                    ('DIAGNOSTICACM.bin',  '',     self.DIAGNOSTICACM_SIZE,     STITCH_OPS.MODE_FILE_NOP | diagnosticacm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('STAGE1A_A.fd'      , ''    , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_B.bin', [
                    ('ACM.bin'      ,  ''        , self.ACM_SIZE,      STITCH_OPS.MODE_FILE_NOP | acm_flag, STITCH_OPS.MODE_POS_TAIL),
                    ('DIAGNOSTICACM.bin',  ''    , self.DIAGNOSTICACM_SIZE, STITCH_OPS.MODE_FILE_NOP | diagnosticacm_flag, STITCH_OPS.MODE_POS_TAIL),
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

        setup_mode = STITCH_OPS.MODE_FILE_PAD if self.ENABLE_SBL_SETUP else STITCH_OPS.MODE_FILE_IGNOR
        for img_tuple in img_list:
            if img_tuple[0] == 'NON_REDUNDANT.bin':
                img_tuple[1].append(
                    ('SETP.bin'     ,  ''        , self.SETUP_SIZE,    setup_mode,               STITCH_OPS.MODE_POS_TAIL)
                )

        return img_list
