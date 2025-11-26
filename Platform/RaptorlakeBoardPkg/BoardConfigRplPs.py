## @file
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

sys.path.append (os.path.join('..', '..'))
from BuildLoader import *
sys.path.append(os.path.realpath(os.path.join(os.path.dirname (os.path.realpath(__file__)), '..', '..', '..', 'SblOpen', 'Platform', 'AlderlakeBoardPkg')))

import BoardConfig as AlderlakeBoardConfig

class Board(AlderlakeBoardConfig.Board):
    def __init__(self, *args, **kwargs):
        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_RPLPS'
        self.BOARD_NAME           = 'rplps'

        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 2
        self._CFGDATA_DEF_FILE    = 'CfgDataDefRplp.yaml'
        self._EXTRA_INC_PATH      = ['Silicon/RaptorlakePkg/Rplps/Fsp']
        self._FSP_PATH_NAME       = 'Silicon/RaptorlakePkg/Rplps/Fsp'
        self.FSP_INF_FILE         = 'Silicon/RaptorlakePkg/Rplps/Fsp/FspBinRplps.inf'
        self.BOARD_PKG_NAME_OVERRIDE = 'RaptorlakeBoardPkg'
        self.MICROCODE_INF_FILE   = 'Silicon/RaptorlakePkg/Rplps/Microcode/MicrocodeRplps.inf'
        self._generated_cfg_file_prefix = ''
        self._CFGDATA_EXT_FILE    = ['CfgDataInt_Rplps_Crb_Ddr5.dlt', 'CfgDataInt_Rplps_Rvp_Ddr5.dlt']
        self._MULTI_VBT_FILE      = {1:'VbtRplPsRvp.dat', 2:'VbtRplPsCrb.dat'}
        self._LP_SUPPORT          = True
        self.EPAYLOAD_SIZE        = 0x240000
        self.OS_LOADER_FD_SIZE    = 0x0005B000
        self.OS_LOADER_FD_NUMBLK  = self.OS_LOADER_FD_SIZE // self.FLASH_BLOCK_SIZE
        # 0 - PCH UART0, 1 - PCH UART1, 2 - PCH UART2, 0xFF - EC UART 0x3F8
        self.DEBUG_PORT_NUMBER = 0x0

        self.FSP_M_STACK_TOP      = 0xFEFDFF00
        self.PLD_HEAP_SIZE        = 0x0C000000
        self.LOADER_RSVD_MEM_SIZE = 0x580000

        # If ENABLE_SBL_RESILIENCY is 1, BiosRedAssistance FIT strap setting
        # needs to be manually changed to Enabled and TopSwapOverride
        # flash setting needs to be manually changed to 4MB in stitch config
        self.ENABLE_SBL_RESILIENCY = 0
        self.BUILD_IDENTICAL_TS    = 0

        if self.ENABLE_SBL_RESILIENCY:
            self.BUILD_IDENTICAL_TS    = 1

        if self.BUILD_IDENTICAL_TS:
            self.TOP_SWAP_SIZE        = 0x400000
            self.NON_REDUNDANT_SIZE   = 0x300000
            self.REDUNDANT_SIZE        = self.STAGE2_SIZE + self.FWUPDATE_SIZE + self.CFGDATA_SIZE + \
                                         self.KEYHASH_SIZE
        else:
            self.TOP_SWAP_SIZE        = 0x80000
            self.REDUNDANT_SIZE       = self.UCODE_SIZE + self.STAGE2_SIZE + self.STAGE1B_SIZE + \
                                        self.FWUPDATE_SIZE + self.CFGDATA_SIZE + self.KEYHASH_SIZE

        self.REDUNDANT_SIZE       = ((self.REDUNDANT_SIZE + 0xFFFF) & ~0xFFFF)

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
        ])

        non_redundant_comp = []
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
                    ('STAGE1A_A.fd'      , ''    , self.STAGE1A_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL),
                    ]
                ),
                ('TOP_SWAP_B.bin', [
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
