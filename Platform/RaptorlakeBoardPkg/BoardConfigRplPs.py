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

sys.path.append(os.path.realpath(os.path.join(os.path.dirname (os.path.realpath(__file__)), '..', '..', '..', 'SblOpen', 'Platform', 'AlderlakeBoardPkg')))

import BoardConfig as AlderlakeBoardConfig

class Board(AlderlakeBoardConfig.Board):
    def __init__(self, *args, **kwargs):
        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_RPLPS'
        self.BOARD_NAME           = 'rplps'

        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 0
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
        self.EPAYLOAD_SIZE        = 0x230000

        # 0 - PCH UART0, 1 - PCH UART1, 2 - PCH UART2, 0xFF - EC UART 0x3F8
        self.DEBUG_PORT_NUMBER = 0x0

        self.FSP_M_STACK_TOP      = 0xFEFDFF00
        self.PLD_HEAP_SIZE        = 0x0C000000
        self.LOADER_RSVD_MEM_SIZE = 0x580000
