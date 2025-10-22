## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2021 - 2025, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import os
import sys
import time
import re

sys.dont_write_bytecode = True
sys.path.append (os.path.join('..', '..'))
from BuildLoader import *

fv_dir = ''

class Board(BaseBoard):
    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_RPLP'
        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 4
        self.VERINFO_SVN            = 1
        self.VERINFO_BUILD_DATE     = time.strftime("%m/%d/%Y")

        self.BOARD_NAME           = 'rplp'
        self.BOARD_PKG_NAME       = 'AlderlakeBoardPkg'
        self.SILICON_PKG_NAME     = 'AlderlakePkg'
        self.BOARD_PKG_NAME_OVERRIDE = 'RaptorlakeBoardPkg'
        self.FSP_IMAGE_ID         = 'ADLI-FSP'
        self._CFGDATA_DEF_FILE    = 'CfgDataDefRplp.yaml'
        self._EXTRA_INC_PATH      = ['Silicon/RaptorlakePkg/Rplp/Fsp']
        self._FSP_PATH_NAME       = 'Silicon/RaptorlakePkg/Rplp/Fsp'
        self.FSP_INF_FILE         = 'Silicon/RaptorlakePkg/Rplp/Fsp/FspBinRplp.inf'
        self.MICROCODE_INF_FILE   = 'Silicon/RaptorlakePkg/Rplp/Microcode/MicrocodeRplp.inf'
        self._SMBIOS_YAML_FILE    = os.path.join('Platform', self.BOARD_PKG_NAME_OVERRIDE, 'SmbiosStrings.yaml')
        self._LP_SUPPORT          = False
        self._N_SUPPORT           = False

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
        self.ENABLE_PCIE_PM       = 1
        # 0: Disable  1: Enable  2: Auto (disable for UEFI payload, enable for others)
        # 3: Enable NOSMRR (for edk2-stable202411 and newer UEFI payload)  4: Auto NOSMRR
        self.ENABLE_SMM_REBASE    = 4

        # 0 - PCH UART0, 1 - PCH UART1, 2 - PCH UART2, 0xFF - EC UART 0x3F8
        self.DEBUG_PORT_NUMBER =0

        self.ENABLE_MULTI_USB_BOOT_DEV = 1

        self.CPU_MAX_LOGICAL_PROCESSOR_NUMBER = 32

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

        self.STAGE1A_SIZE         = 0x0001B000
        self.STAGE1_STACK_SIZE    = 0x00002000
        self.STAGE1_DATA_SIZE     = 0x00014000
        self.FSP_M_STACK_TOP      = 0xFEFDFF00
        self.STAGE1B_SIZE         = 0x00200000
        self.STAGE2_SIZE          = 0x000CE000
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x001F0000

        if self.FSPDEBUG_MODE:
            self.STAGE2_SIZE += 0x4000

        self.PAYLOAD_SIZE         = 0x00030000
        self.EPAYLOAD_SIZE        = 0x00240000

        self.OS_LOADER_FD_SIZE    = 0x0005C000
        self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE

        self.ENABLE_FAST_BOOT = 0
        try:
            if os.environ['SBL_ENABLE_FAST_BOOT'] == '1':
                self.ENABLE_FAST_BOOT = 1
        except Exception:
            pass
        if self.ENABLE_FAST_BOOT:
            self.ENABLE_SPLASH              = 0
            self.ENABLE_FRAMEBUFFER_INIT    = 0
            self.RELEASE_MODE               = 1
            self.HAVE_VERIFIED_BOOT         = 0
            self.HAVE_MEASURED_BOOT         = 0
            self.VERIFIED_BOOT_HASH_MASK    = 0
            self.HAVE_PSD_TABLE             = 0
            self.FSPDEBUG_MODE              = 0
            self.ENABLE_SOURCE_DEBUG        = 0
            self.ENABLE_PCIE_PM             = 0

        if self.RELEASE_MODE and self.ENABLE_FAST_BOOT:
            self.STAGE1A_SIZE         = 0x00016000
            self.STAGE1B_SIZE         = 0x000E0000
            self.STAGE2_SIZE          = 0x000C0000
            self.STAGE2_FD_SIZE       = 0x000F0000
            self.PAYLOAD_SIZE         = 0x00024000

        if self.ENABLE_SOURCE_DEBUG:
            self.STAGE1B_SIZE += 0x4000


        self.UEFI_VARIABLE_SIZE = 0x1000
        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.UEFI_VARIABLE_SIZE = 0x00040000
        self.UCODE_SIZE           = 0x000B7000
        self.MRCDATA_SIZE         = 0x00010000
        self.CFGDATA_SIZE         = 0x00005000
        self.KEYHASH_SIZE         = 0x00001000
        self.VARIABLE_SIZE        = 0x00002000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x00020000 if self.ENABLE_FWU else 0

        self.SIIPFW_SIZE = 0x1000

        # Declaring UCODE_SLOT_SIZE populates the uCode region with as many
        # uCode slots as possible, even if there aren't enough uCode patches
        # to fill each slot. This can have a slight performance impact on SBL,
        # as it tries to load dummy uCode patches.
        self.UCODE_SLOT_SIZE      = 0x4C000

        # If ENABLE_SBL_RESILIENCY is 1, BiosRedAssistance FIT strap setting
        # needs to be manually changed to Enabled and TopSwapOverride
        # flash setting needs to be manually changed to 4MB in stitch config
        self.ENABLE_SBL_RESILIENCY = 0
        self.BUILD_IDENTICAL_TS    = 0

        if self.ENABLE_SBL_RESILIENCY:
            self.BUILD_IDENTICAL_TS    = 1

        if self.BUILD_IDENTICAL_TS:
            self.TOP_SWAP_SIZE         = 0x400000
            self.REDUNDANT_SIZE        = self.STAGE2_SIZE + self.FWUPDATE_SIZE + self.CFGDATA_SIZE + \
                                         self.KEYHASH_SIZE
        else:
            self.TOP_SWAP_SIZE        = 0x80000
            self.REDUNDANT_SIZE       = self.UCODE_SIZE + self.STAGE2_SIZE + self.STAGE1B_SIZE + \
                                        self.FWUPDATE_SIZE + self.CFGDATA_SIZE + self.KEYHASH_SIZE

        self.FUSA_SUPPORT         = 0
        self.FUSA_SIZE            = 0

        # These can be overridden by FUSA dlt file if FUSA_SUPPORT is set
        self.ENABLE_TCC           = 0
        self.ENABLE_TSN           = 0

        if self.FUSA_SUPPORT:
            self.ENABLE_PRE_OS_CHECKER = 0
            if self.ENABLE_PRE_OS_CHECKER:
                self.POSC_SIZE = 0x00080000
                self.SIIPFW_SIZE += self.POSC_SIZE
            self.FUSA_BIST_PATTERN_SUPPORT  = 0
            if self.FUSA_BIST_PATTERN_SUPPORT:
                self.FUSA_PATTERN_IS_FFS        = 0
                self.FUSA_STARTUP_BIST_FILE     = 'FusaStartupBist.bin'
                self.FUSA_PERIODIC_BIST_FILE    = 'FusaPeriodicBist.bin'
                self.FSBP_SIZE        = 0x1000000
                self.FPBP_SIZE        = 0x1000000
                self.FUSA_SIZE        = 0x1000 + self.FPBP_SIZE + self.FSBP_SIZE

            # Allow TCC and TSN to be enabled by FuSa dlt file
            FusaConfig = ''

            brd_cfg_src_dir = os.path.join(os.environ['SBL_SOURCE'], 'Platform', self.BOARD_PKG_NAME, 'CfgData')
            brd_cfg2_src_dir = '.'
            if hasattr(self, 'BOARD_PKG_NAME_OVERRIDE'):
                brd_cfg2_src_dir = os.path.join(os.environ['SBL_SOURCE'], 'Platform', self.BOARD_PKG_NAME_OVERRIDE, 'CfgData')

            if os.path.exists(os.path.join(brd_cfg_src_dir, 'CfgData_Fusa_Feature.dlt')):
                FusaConfig = open (os.path.join(brd_cfg_src_dir, 'CfgData_Fusa_Feature.dlt')).readlines()
            else:
                if os.path.exists(os.path.join(brd_cfg2_src_dir, 'CfgData_Fusa_Feature.dlt')):
                    FusaConfig = open (os.path.join(brd_cfg2_src_dir, 'CfgData_Fusa_Feature.dlt')).readlines()

            for line in FusaConfig:
                if (re.search(r"TCC_CFG_DATA\.TccEnable\s+\|\s*1",line) != None or
                    re.search(r"TCC_CFG_DATA\.TccEnable\s+\|\s*0x0*1",line) != None):
                    # use setattr() to avoid matching release.py regex
                    setattr(self, 'ENABLE_TCC', 1)
                elif (re.search(r"SILICON_CFG_DATA\.PchTsnEnable\s+\|\s*1",line) != None or
                    re.search(r"SILICON_CFG_DATA\.PchTsnEnable\s+\|\s*0x0*1",line) != None):
                    # use setattr() to avoid matching release.py regex
                    setattr(self, 'ENABLE_TSN', 1)

        if self.ENABLE_TSN:
            self.TMAC_SIZE = 0x00001000
            self.SIIPFW_SIZE += self.TMAC_SIZE

        if self._SMBIOS_YAML_FILE:
            self.SIIPFW_SIZE += 0x1000

        self.NON_REDUNDANT_SIZE   = 0x3BF000 + self.SIIPFW_SIZE + self.FUSA_SIZE
        self.NON_VOLATILE_SIZE    = 0x001000
        self.SLIMBOOTLOADER_SIZE  = (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 + \
                                    self.NON_REDUNDANT_SIZE + self.NON_VOLATILE_SIZE
        self.SLIMBOOTLOADER_SIZE  = ((self.SLIMBOOTLOADER_SIZE + 0xFFFFF) & ~0xFFFFF)
        self.PLD_HEAP_SIZE        = 0x09000000
        self.PLD_STACK_SIZE       = 0x00020000
        self.PLD_RSVD_MEM_SIZE    = 0x00500000

        if self.FUSA_SUPPORT:
            # FuSa validation requires prebuilt images that can boot OS Loader Ubuntu
            self.PLD_HEAP_SIZE    = 0x09000000
            self.DIAGNOSTICACM_SIZE   = 0x00001000

        self.KM_SIZE              = 0x00000400
        self.BPM_SIZE             = 0x00000600
        self.ACM_SIZE             = 0x00040000 + self.KM_SIZE + self.BPM_SIZE
        # adjust ACM_SIZE to meet 256KB alignment (to align 256KB ACM size)
        if self.ACM_SIZE > 0:
            acm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE - self.DIAGNOSTICACM_SIZE
            acm_btm = acm_top - self.ACM_SIZE
            acm_btm = (acm_btm & 0xFFFC0000)
            self.ACM_SIZE     = acm_top - acm_btm

        self.LOADER_RSVD_MEM_SIZE = 0xC00000
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
        self._MULTI_VBT_FILE      = {1:'Vbt_rplp.dat', 2:'Vbt_rplp_crb.dat', 3:'Vbt_rplp_rki.dat'}

        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE
        self._generated_cfg_file_prefix = 'Autogen_'

        self._CFGDATA_INT_FILE = []
        self._CFGDATA_EXT_FILE = [self._generated_cfg_file_prefix + 'CfgDataInt_Rplp_Rvp_Ddr5.dlt',
                                  self._generated_cfg_file_prefix + 'CfgDataInt_Rplp_Rvp_Lpddr5.dlt',
                                  self._generated_cfg_file_prefix + 'CfgDataInt_Rplp_Crb_Ddr5.dlt',
                                  self._generated_cfg_file_prefix + 'CfgDataExt_Rplp_Upx12.dlt',
                                  self._generated_cfg_file_prefix + 'CfgDataExt_Rplp_Rki.dlt']

    def PlatformBuildHook (self, build, phase):
        global fv_dir
        fv_dir = build._fv_dir
        if phase == 'pre-build:before':
            # create build folder if not exist
            if not os.path.exists(build._fv_dir):
                os.makedirs(build._fv_dir)

            # Generate the dlt files based on feature
            brd_cfg_src_dir = os.path.join(os.environ['SBL_SOURCE'], 'Platform', self.BOARD_PKG_NAME, 'CfgData')
            brd_cfg2_src_dir = '.'
            if hasattr(self, 'BOARD_PKG_NAME_OVERRIDE'):
                brd_cfg2_src_dir = os.path.join(os.environ['SBL_SOURCE'], 'Platform', self.BOARD_PKG_NAME_OVERRIDE, 'CfgData')
            for dlt_file in self._CFGDATA_EXT_FILE:
                cfg_dlt_file  = os.path.join(brd_cfg_src_dir, dlt_file[len (self._generated_cfg_file_prefix):])
                if not os.path.exists(cfg_dlt_file):
                    cfg_dlt_file = os.path.join(brd_cfg2_src_dir, dlt_file[len (self._generated_cfg_file_prefix):])
                lines         = open (cfg_dlt_file).read()

               # Enable TSN in dlt file
                if self.ENABLE_TSN:
                    if os.path.exists(os.path.join(brd_cfg2_src_dir, 'CfgData_Tsn_Feature.dlt')):
                        lines += open (os.path.join(brd_cfg2_src_dir, 'CfgData_Tsn_Feature.dlt')).read()
                    else:
                        lines += open (os.path.join(brd_cfg_src_dir, 'CfgData_Tsn_Feature.dlt')).read()

                if self.FUSA_SUPPORT:
                    if os.path.exists(os.path.join(brd_cfg2_src_dir, 'CfgData_Fusa_Feature.dlt')):
                        lines += open (os.path.join(brd_cfg2_src_dir, 'CfgData_Fusa_Feature.dlt')).read()
                    else:
                        lines += open (os.path.join(brd_cfg_src_dir, 'CfgData_Fusa_Feature.dlt')).read()
                    # remove the TCC_CFG_DATA.TccEnable line to avoid its presence in final dlt files
                    lines = re.sub(r"TCC_CFG_DATA\.TccEnable\s+\|[^\n]+","",lines)
                    if self.ENABLE_PRE_OS_CHECKER:
                        if os.path.exists(os.path.join(brd_cfg2_src_dir, 'CfgData_Posc_Feature.dlt')):
                            lines += open (os.path.join(brd_cfg2_src_dir, 'CfgData_Posc_Feature.dlt')).read()
                        else:
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
            'GpioSiLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioSiLib/GpioSiLib.inf',
            'PchSbiAccessLib|Silicon/CommonSocPkg/Library/PchSbiAccessLib/PchSbiAccessLib.inf',
            'TccLib|Silicon/CommonSocPkg/Library/TccLib/TccLib.inf',
            'ShellExtensionLib|Platform/$(BOARD_PKG_NAME)/Library/ShellExtensionLib/ShellExtensionLib.inf',
            'FspmUpdUpdateLib|Platform/$(BOARD_PKG_NAME)/Library/FspmUpdUpdateLib/FspmUpdUpdateLib.inf',
            'FspsUpdUpdateLib|Platform/$(BOARD_PKG_NAME_OVERRIDE)/Library/FspsUpdUpdateLib/FspsUpdUpdateLib.inf',
            'PchInfoLib|Silicon/$(SILICON_PKG_NAME)/Library/PchInfoLib/PchInfoLib.inf',
            'PchPciBdfLib|Silicon/$(SILICON_PKG_NAME)/Library/BasePchPciBdfLib/BasePchPciBdfLib.inf',
            'PchPcrLib|Silicon/CommonSocPkg/Library/PchPcrLib/PchPcrLib.inf',
            'PchSpiLib|Silicon/CommonSocPkg/Library/PchSpiLib/PchSpiLib.inf',
            'SpiFlashLib|Silicon/CommonSocPkg/Library/SpiFlashLib/SpiFlashLib.inf',
            'BootGuardLib|Silicon/CommonSocPkg/Library/BootGuardLibCBnT/BootGuardLibCBnT.inf',
            'BdatLib|Silicon/CommonSocPkg/Library/BdatLib/BdatLib.inf',
            'SmbusLib|Silicon/CommonSocPkg/Library/SmbusLib/SmbusLib.inf',
            'IgdOpRegionLib|Silicon/CommonSocPkg/Library/IgdOpRegionLib/IgdOpRegionLib.inf',
            'SerialIoI2cLib|Silicon/$(SILICON_PKG_NAME)/Library/BaseSerialIoI2cLib/BaseSerialIoI2cLib.inf',
            'HeciLib|Silicon/CommonSocPkg/Library/HeciLib/HeciLib.inf',
            'MeChipsetLib|Silicon/$(SILICON_PKG_NAME)/Library/MeChipsetLib/MeChipsetLib.inf',
            'VtdLib|Silicon/$(SILICON_PKG_NAME)/Library/VTdLib/VTdLib.inf',
            'DmarLib|Silicon/CommonSocPkg/Library/DmarLib/DmarLib.inf',
            'PsdLib|Silicon/$(SILICON_PKG_NAME)/Library/PsdLib/PsdLib.inf',
            'HeciMeExtLib|Silicon/$(SILICON_PKG_NAME)/Library/HeciMeExtLib/HeciMeExtLib.inf',
            'MeExtMeasurementLib|Silicon/$(SILICON_PKG_NAME)/Library/MeExtMeasurementLib/MeExtMeasurementLib.inf',
            'CpuPcieHsPhyInitLib|Silicon/$(SILICON_PKG_NAME)/Library/CpuPcieHsPhyInitLib/CpuPcieHsPhyInitLib.inf',
            'WatchDogTimerLib|Silicon/CommonSocPkg/Library/WatchDogTimerLib/WatchDogTimerLib.inf',
            'TcoTimerLib|Silicon/CommonSocPkg/Library/TcoTimerLib/TcoTimerLib.inf',
            'TopSwapLib|Silicon/CommonSocPkg/Library/TopSwapLib/TopSwapLib.inf'
        ]

        if self.BUILD_CSME_UPDATE_DRIVER:
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('MeFwUpdateLib|Silicon/$(SILICON_PKG_NAME)/Library/MeFwUpdateLib/MeFwUpdateLib.inf')

        if self.FUSA_SUPPORT:
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('CrashLogLib|Silicon/RaptorlakePkg/Library/FusaCrashLogLib/FusaCrashLogLib.inf')
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('FusaConfigLib|Silicon/RaptorlakePkg/Library/FusaConfigLib/FusaConfigLib.inf')
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('PchGspiLib|Silicon/RaptorlakePkg/Library/PchGspiLib/PchGspiLib.inf')

        if self.ENABLE_PCIE_PM:
            lib = [
                'PciePm|Silicon/$(SILICON_PKG_NAME)/Library/PciePm/PciePm.inf',
                'PciExpressHelpersLib|Silicon/$(SILICON_PKG_NAME)/Library/PciExpressHelpersLibrary/PciExpressHelpersLibrary.inf',
                'BasePcieHelperLib|Silicon/$(SILICON_PKG_NAME)/Library/BasePcieHelperLib/BasePcieHelperLib.inf',
                'PcieClientRpLib|Silicon/$(SILICON_PKG_NAME)/Library/PcieClientRpLib/PcieClientRpLib.inf',
                'BasePchPciBdfLib|Silicon/$(SILICON_PKG_NAME)/Library/BasePchPciBdfLib/BasePchPciBdfLib.inf',
            ]
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].extend (lib)
        else:
            dsc['LibraryClasses.%s' % self.BUILD_ARCH].append ('PciePm|Silicon/CommonSocPkg/Library/PciePmNull/PciePmNull.inf')

        dsc['PcdsFixedAtBuild'] = ['gPlatformModuleTokenSpaceGuid.PcdAcpiTablesMaxEntry | 40']

        if self._LP_SUPPORT:
            dsc['PcdsFixedAtBuild'].append ('gPlatformAlderLakeTokenSpaceGuid.PcdAdlLpSupport | TRUE')

        if self._N_SUPPORT:
            dsc['PcdsFixedAtBuild'].append ('gPlatformAlderLakeTokenSpaceGuid.PcdAdlNSupport | TRUE')

        dsc['PcdsFixedAtBuild.%s' % self.BUILD_ARCH] = [
            'gPlatformModuleTokenSpaceGuid.PcdFusaSupport | $(FUSA_SUPPORT)'
        ]
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
          [('IPFW',      'SIIPFW.bin',          '',     container_list_auth_type,   'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,        0,          0     ,        0)],   # Container Header
        )
        container_list[0].append (
          ('SMBS',      'smbios.bin',    'Dummy',        container_list_auth_type,   'KEY_ID_CONTAINER'+'_'+self._RSA_SIGN_TYPE,            0,              0x1000,    0),   # SMBIOS Component
        )

        bins = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Binaries')

        if self.ENABLE_TSN:
            TsnSubRegion = os.path.join(bins,  'TsnSubRegion.bin')if os.path.exists(os.path.join(bins,  'TsnSubRegion.bin')) else ''
            container_list[0].append (
              ('TMAC',TsnSubRegion,   'Lz4', container_list_auth_type, 'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0, self.TMAC_SIZE, 0),       # TSN MAC Address
            )

        if self.FUSA_SUPPORT:
            if self.ENABLE_PRE_OS_CHECKER:
                CompFilePreOsChecker= os.path.join(bins, 'PreOsChecker.bin') if os.path.exists(os.path.join(bins, 'PreOsChecker.bin')) else ''
                container_list[0].append (
                    ('POSC',CompFilePreOsChecker,  'Lz4',     container_list_auth_type,   'KEY_ID_CONTAINER_COMP'+'_'+self._RSA_SIGN_TYPE, 0,   self.POSC_SIZE,      0),   # Pre-OS Checker
                )

            if self.FUSA_BIST_PATTERN_SUPPORT:
                global fv_dir
                StartupBistPattRegion = os.path.join(bins,  self.FUSA_STARTUP_BIST_FILE)if os.path.exists(os.path.join(bins,  self.FUSA_STARTUP_BIST_FILE)) else ''
                PeriodicBistPattRegion = os.path.join(bins,  self.FUSA_PERIODIC_BIST_FILE)if os.path.exists(os.path.join(bins,  self.FUSA_PERIODIC_BIST_FILE)) else ''
                if self.FUSA_PATTERN_IS_FFS:
                    FUSA_STARTUP_PATTERN_BIST_GUID = '2A81105C-2AB6-4C7A-AC9C-E8AEFAB8B229'
                    FUSA_PERIODIC_PATTERN_BIST_GUID = 'E48CAE50-6EC5-4F3B-8CB4-1ECD12B00CAF'
                    StartupBistPattRegionSection = os.path.join(fv_dir,'FusaStartup.sec')
                    StartupBistPattRegionFfs = os.path.join(fv_dir,'FusaStartup.ffs')
                    PeriodicBistPattRegionSection = os.path.join(fv_dir,'FusaPeriodic.sec')
                    PeriodicBistPattRegionFfs = os.path.join(fv_dir,'FusaPeriodic.ffs')

                    sblopen_dir = os.getenv('SBL_SOURCE', '')

                    if not os.path.exists (sblopen_dir):
                        raise  Exception("Please set env 'SBL_SOURCE' to SBL open source root folder")

                    if os.name == 'nt':
                        tool_bin_dir  = os.path.join(sblopen_dir, "BaseTools", "Bin", "Win32")
                        gensec_bin = os.path.join(tool_bin_dir, "GenSec.exe")
                        genffs_bin = os.path.join(tool_bin_dir, "GenFfs.exe")
                    else:
                        tool_bin_dir  = os.path.join(sblopen_dir, "BaseTools", "BinWrappers", "PosixLike")
                        gensec_bin = os.path.join(tool_bin_dir, "GenSec")
                        genffs_bin = os.path.join(tool_bin_dir, "GenFfs")

                    #Build Startup pattern Section
                    cmd_args = [
                        gensec_bin,
                        "-s",
                        "EFI_SECTION_RAW",
                        "-o",
                        StartupBistPattRegionSection,
                        StartupBistPattRegion
                    ]
                    run_process (cmd_args)

                    #Build Startup pattern FFS
                    cmd_args = [
                        genffs_bin,
                        "-t",
                        "EFI_FV_FILETYPE_FREEFORM",
                        "-g",
                        FUSA_STARTUP_PATTERN_BIST_GUID,
                        "-o",
                        StartupBistPattRegionFfs,
                        "-i",
                        StartupBistPattRegionSection
                    ]
                    run_process (cmd_args)

                    StartupBistPattRegion = StartupBistPattRegionFfs

                    #Build Periodic pattern Section
                    cmd_args = [
                        gensec_bin,
                        "-s",
                        "EFI_SECTION_RAW",
                        "-o",
                        PeriodicBistPattRegionSection,
                        PeriodicBistPattRegion
                    ]
                    run_process (cmd_args)

                    #Build Periodic pattern FFS
                    cmd_args = [
                        genffs_bin,
                        "-t",
                        "EFI_FV_FILETYPE_FREEFORM",
                        "-g",
                        FUSA_PERIODIC_PATTERN_BIST_GUID,
                        "-o",
                        PeriodicBistPattRegionFfs,
                        "-i",
                        PeriodicBistPattRegionSection
                    ]
                    run_process (cmd_args)

                    PeriodicBistPattRegion = PeriodicBistPattRegionFfs

                container_list.append (
                # Name | Image File            | CompressAlg | AuthType                 |            Key File                               | Align | Region Size | Svn Info
                # ========================================================================================================================================================
                [('FUFW', 'FUFW.bin',               '',       container_list_auth_type,  'KEY_ID_CONTAINER'      + '_'+self._RSA_SIGN_TYPE,   0,         0,          0)], # Container Header
                )
                container_list[1].append (
                ('FSBP', StartupBistPattRegion,     'Lz4',    container_list_auth_type,  'KEY_ID_CONTAINER_COMP' + '_'+self._RSA_SIGN_TYPE,   0,     self.FSBP_SIZE, 0),  # FuSa Startup BIST Pattern file
                )
                container_list[1].append (
                ('FPBP', PeriodicBistPattRegion,    'Lz4',    container_list_auth_type,  'KEY_ID_CONTAINER_COMP' + '_'+self._RSA_SIGN_TYPE,   0,     self.FPBP_SIZE, 0),  # FuSa Periodic BIST Pattern file
                )

        return container_list

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

        return img_list
