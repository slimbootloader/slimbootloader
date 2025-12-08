# @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import os
import sys
tool_dir = os.path.realpath(os.path.join(os.path.dirname (os.path.realpath(__file__)), '..', 'AlderlakeBoardPkg'))
sys.path.append (tool_dir)

import BoardConfig as AlderlakeBoardConfig

class Board(AlderlakeBoardConfig.Board):
    def __init__(self, *args, **kwargs):
        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_ADLN'
        self.BOARD_NAME           = 'adln50'
        self.FSP_IMAGE_ID         = '$TWLFSPE'
        self._EXTRA_INC_PATH      = ['Silicon/AlderlakePkg/Adln/Include']
        self._FSP_PATH_NAME       = 'Silicon/AlderlakePkg/Adln/FspBin'
        self.MICROCODE_INF_FILE   = 'Silicon/AlderlakePkg/Microcode/Microcode.inf'
        self.ACPI_TABLE_INF_FILE  = 'Platform/AlderlakeBoardPkg/AcpiTables/AcpiTablesN.inf'
        self._LP_SUPPORT          = True
        self._N_SUPPORT           = True

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
        self.DEBUG_PORT_NUMBER = 0x1

        self.BUILD_CSME_UPDATE_DRIVER   = 0

        self.STAGE1A_XIP          = 1
        self.STAGE1B_XIP          = 1
        self.STAGE2_XIP           = 0

        self.STAGE1A_SIZE         = 0x0001B000
        self.STAGE1_STACK_SIZE    = 0x00002000
        self.STAGE1_DATA_SIZE     = 0x00014000
        self.FSP_M_STACK_TOP      = 0xFEF7FF00
        self.STAGE1B_SIZE         = 0x00200000
        self.STAGE2_SIZE          = 0x000C2000
        self.STAGE2_FD_BASE       = 0x01000000
        self.STAGE2_FD_SIZE       = 0x001F0000

        self.PAYLOAD_SIZE         = 0x00030000
        self.EPAYLOAD_SIZE        = 0x00240000

        self.OS_LOADER_FD_SIZE    = 0x0005B000
        self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE

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

        self.PLD_HEAP_SIZE        = 0x09000000

        self.UEFI_VARIABLE_SIZE = 0x1000
        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.UEFI_VARIABLE_SIZE = 0x00040000
        self.UCODE_SIZE           = 0x000EC000
        self.MRCDATA_SIZE         = 0x00010000
        self.CFGDATA_SIZE         = 0x00004000
        self.KEYHASH_SIZE         = 0x00001000
        self.VARIABLE_SIZE        = 0x00002000
        self.SBLRSVD_SIZE         = 0x00001000
        self.FWUPDATE_SIZE        = 0x00020000 if self.ENABLE_FWU else 0

        self.SIIPFW_SIZE          = 0x1000

        # Declaring UCODE_SLOT_SIZE populates the uCode region with as many
        # uCode slots as possible, even if there aren't enough uCode patches
        # to fill each slot. This can have a slight performance impact on SBL,
        # as it tries to load dummy uCode patches.
        self.UCODE_SLOT_SIZE      = 0x3B000

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
            self.TOP_SWAP_SIZE         = 0x80000
            self.REDUNDANT_SIZE        = self.UCODE_SIZE + self.STAGE2_SIZE + self.STAGE1B_SIZE + \
                                         self.FWUPDATE_SIZE + self.CFGDATA_SIZE + self.KEYHASH_SIZE

        self.ENABLE_TCC = 0
        self.ENABLE_TSN = 0
        if self.ENABLE_TSN:
            self.TMAC_SIZE = 0x00001000
            self.SIIPFW_SIZE += self.TMAC_SIZE

        if self._SMBIOS_YAML_FILE:
            self.SIIPFW_SIZE += 0x1000

        self.NON_REDUNDANT_SIZE   = 0x3BF000 + self.SIIPFW_SIZE
        self.NON_VOLATILE_SIZE    = 0x001000
        self.SLIMBOOTLOADER_SIZE  = (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 + \
                                    self.NON_REDUNDANT_SIZE + self.NON_VOLATILE_SIZE
        self.SLIMBOOTLOADER_SIZE  = ((self.SLIMBOOTLOADER_SIZE + 0xFFFFF) & ~0xFFFFF)
        self.PLD_HEAP_SIZE        = 0x09000000
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

        self.LOADER_RSVD_MEM_SIZE = 0xC00000

        # If mulitple VBT table support is required, list them as:
        #   {VbtImageId1 : VbtFileName1, VbtImageId2 : VbtFileName2, ...}
        # VbtImageId is ID to identify a VBT image. It is a UINT32 number to match
        #   the ImageId field in the VBT container.
        # VbtFileName is the VBT file name. It needs to be located under platform
        #   VbtBin folder.
        self._MULTI_VBT_FILE      = {1:'VbtAdlNCrb.dat', 2:'VbtAdlnUp7000.dat'}

        self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE
        self._generated_cfg_file_prefix = 'Autogen_'

        self._CFGDATA_DEF_FILE = 'CfgDataDefAdln.yaml'
        self._CFGDATA_INT_FILE = []
        self._CFGDATA_EXT_FILE = [self._generated_cfg_file_prefix + 'CfgDataExt_Up7000eN50.dlt']
