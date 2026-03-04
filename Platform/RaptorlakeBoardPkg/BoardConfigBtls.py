## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
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
sys.path.append (os.path.dirname (os.path.realpath(__file__)))
from BuildLoader import *
import BoardConfigRpls as RaptorlakeBoardConfig

class Board(RaptorlakeBoardConfig.Board):
    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_BTLS'
        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 0
        self.VERINFO_SVN            = 1
        self.VERINFO_BUILD_DATE     = time.strftime("%m/%d/%Y")

        self.BOARD_NAME           = 'btls'
        self.FSP_IMAGE_ID         = '$RPLFSPE'
        self._EXTRA_INC_PATH      = ['Silicon/RaptorlakePkg/Btls/Fsp']
        self._FSP_PATH_NAME       = 'Silicon/RaptorlakePkg/Btls/Fsp'
        self.FSP_INF_FILE         = 'Silicon/RaptorlakePkg/Btls/Fsp/FspBinBtls.inf'
        self.MICROCODE_INF_FILE   = 'Silicon/RaptorlakePkg/Btls/Microcode/MicrocodeBtls.inf'

        self._CFGDATA_EXT_FILE = [self._generated_cfg_file_prefix + 'CfgDataInt_Rpls_Rvp_Ddr5.dlt', self._generated_cfg_file_prefix + 'CfgDataInt_Rpls_Rvp_SODdr5.dlt', self._generated_cfg_file_prefix + 'CfgDataInt_Adls_Crb_Ddr4.dlt', self._generated_cfg_file_prefix + 'CfgDataInt_Adls_Crb_Ddr5.dlt']

        self.ENABLE_SBL_SETUP       = 0
        self.SETUP_SIZE             = 0

        if self.ENABLE_SBL_SETUP:
            self.HAVE_VERIFIED_BOOT = 0
            self.HAVE_MEASURED_BOOT = 0
            self.SETUP_MPYM_SIZE    = 0x12000
            self.SETUP_STPY_SIZE    = 0x06000
            self.SETUP_CFGJ_SIZE    = 0x0B000
            self.SETUP_CFGD_SIZE    = 0x03000

            self.SETUP_SIZE           = 0x1000 + self.SETUP_MPYM_SIZE + self.SETUP_STPY_SIZE + self.SETUP_CFGJ_SIZE + self.SETUP_CFGD_SIZE

        self.UEFI_VARIABLE_SIZE = 0x1000
        if len(self._PAYLOAD_NAME.split(';')) > 1:
            self.UEFI_VARIABLE_SIZE = 0x00040000

        self.ENABLE_SOURCE_DEBUG   = 0
        self.PLD_HEAP_SIZE         = 0x0C000000

        # wait 600us for AP wake up. needed for 8P 16E CPUs
        self.CPU_AP_WAIT_TIME_US   = 600

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

        self.ENABLE_TCC = 0
        self.ENABLE_TSN = 0
        if self.ENABLE_TSN:
            self.TMAC_SIZE = 0x00001000
            self.SIIPFW_SIZE += self.TMAC_SIZE

        self.NON_REDUNDANT_SIZE   = 0x3BF000 + self.SIIPFW_SIZE
        self.SLIMBOOTLOADER_SIZE  = (self.TOP_SWAP_SIZE + self.REDUNDANT_SIZE) * 2 + \
                                    self.NON_REDUNDANT_SIZE + self.NON_VOLATILE_SIZE
        self.SLIMBOOTLOADER_SIZE  = ((self.SLIMBOOTLOADER_SIZE + 0xFFFFF) & ~0xFFFFF)

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

        if self.RELEASE_MODE and self.ENABLE_FAST_BOOT:
            self.STAGE1A_SIZE         = 0x00016000
            self.STAGE1B_SIZE         = 0x000E0000
            self.STAGE2_SIZE          = 0x000C0000
            self.STAGE2_FD_SIZE       = 0x000F0000
            self.PAYLOAD_SIZE         = 0x00024000

        if self.ENABLE_SOURCE_DEBUG:
            self.REDUNDANT_SIZE += 0x4000
            self.STAGE1B_SIZE += 0x4000
            if self.SKIP_STAGE1A_SOURCE_DEBUG == 0:
                self.STAGE1A_SIZE += 0x4000

        # adjust ACM_SIZE to meet 256KB alignment (to align 256KB ACM size)
        if self.ACM_SIZE > 0:
            acm_top = self.FLASH_LAYOUT_START - self.STAGE1A_SIZE
            acm_btm = acm_top - self.ACM_SIZE
            acm_btm = (acm_btm & 0xFFFC0000)
            self.ACM_SIZE     = acm_top - acm_btm

        self.FSP_M_STACK_TOP      = 0xFEFDFF00

        # 0: Disable  1: Enable  2: Auto (disable for UEFI payload, enable for others)
        # 3: Enable NOSMRR (for edk2-stable202411 and newer UEFI payload)  4: Auto NOSMRR
        self.ENABLE_SMM_REBASE    = 4

    def GetContainerList (self):
        container_list = super(Board, self).GetContainerList()
        bins = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'Binaries')
        container_list_auth_type = self._RSA_SIGN_TYPE + '_'+ self._SIGNING_SCHEME[4:] + '_' + self._SIGN_HASH

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

    def GetImageLayout (self):
        img_list = super(Board, self).GetImageLayout()
        setup_mode = STITCH_OPS.MODE_FILE_PAD if self.ENABLE_SBL_SETUP else STITCH_OPS.MODE_FILE_IGNOR
        for img_tuple in img_list:
            if img_tuple[0] == 'NON_REDUNDANT.bin':
                img_tuple[1].append(
                    ('SETP.bin'     ,  ''        , self.SETUP_SIZE,    setup_mode,               STITCH_OPS.MODE_POS_TAIL)
                )
        return img_list
