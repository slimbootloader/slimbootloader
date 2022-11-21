## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
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

        self.VERINFO_IMAGE_ID     = 'SB_ADLP'
        self.BOARD_NAME           = 'adlp'
        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 2
        self._EXTRA_INC_PATH      = ['Silicon/AlderlakePkg/Adlp/Include']
        self._FSP_PATH_NAME       = 'Silicon/AlderlakePkg/Adlp/FspBin'
        self.MICROCODE_INF_FILE   = 'Silicon/AlderlakePkg/Microcode/Microcode.inf'
        self.ACPI_TABLE_INF_FILE  = 'Platform/AlderlakeBoardPkg/AcpiTables/AcpiTablesP.inf'
        self.ENABLE_TCC           = 0
        self._generated_cfg_file_prefix = ''
        self._CFGDATA_EXT_FILE    = ['CfgDataInt_Adlp_Crb_Ddr5.dlt', 'CfgDataInt_Adlp_Crb_Lpddr5.dlt', 'CfgDataInt_Adlp_Crb_Lpddr4.dlt']
        self._MULTI_VBT_FILE      = {2:'VbtAdlP.dat'}
        self._LP_SUPPORT          = True

        # 0 - PCH UART0, 1 - PCH UART1, 2 - PCH UART2, 0xFF - EC UART 0x3F8
        self.DEBUG_PORT_NUMBER = 0x0
