## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2023 - 2025, Intel Corporation. All rights reserved.<BR>
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
sys.path.append(os.path.realpath(os.path.join(os.path.dirname (os.path.realpath(__file__)), '..', 'ArrowlakeBoardPkg')))

import BoardConfigArlh as ArrowlakeBoardConfig

class Board(ArrowlakeBoardConfig.Board):
    def __init__(self, *args, **kwargs):

        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID     = 'SB_ARLU'
        self.BOARD_NAME           = 'arlu'
        self.MICROCODE_INF_FILE   = 'Silicon/ArrowlakePkg/Arlh/Microcode/MicrocodeArlu.inf'
        self._ACM_CPU_FMS = [
                                    0xF3FFB065,
                                    0xF3FFA06A,
                                    0xF3FFA06B,
                                    0xF3FFA06C
        ]
