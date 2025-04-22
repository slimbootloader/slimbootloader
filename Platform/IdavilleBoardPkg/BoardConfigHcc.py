## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2021 - 2023, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import IdavilleBoardConfig

class Board(IdavilleBoardConfig.Board):
    def __init__(self, *args, **kwargs):
        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_IDVH'
        self.VERINFO_PROJ_MAJOR_VER = 1
        self.VERINFO_PROJ_MINOR_VER = 2
        self.VERINFO_SVN          = 1

        self.BOARD_NAME           = 'idvh'
        self._EXTRA_INC_PATH      = ['Silicon/IdavillePkg/Hcc/Include']
        self._FSP_PATH_NAME       = 'Silicon/IdavillePkg/Hcc/FspBin'
        self.MICROCODE_INF_FILE   = 'Silicon/IdavillePkg/Microcode/Microcode.inf'

        self.STAGE1_STACK_BASE_OFFSET = 0x00140000  # PcdPeiTemporaryRamRcHeapSize
