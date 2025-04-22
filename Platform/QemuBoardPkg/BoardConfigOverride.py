#  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

# This import module name is generated from BuildLoader.py.
#   Directory name w/o 'BoardPkg' + parent 'BoardConfig.py' w/o '.py'
#   ex) QemuBoardPkg/BoardConfig.py -> QemuBoardConfig
#       QemuBoardPkg/BoardConfigParent.py -> QemuBoardConfigParent
import QemuBoardConfig

class Board(QemuBoardConfig.Board):
    def __init__(self, *args, **kwargs):
        super(Board, self).__init__(*args, **kwargs)

        self.VERINFO_IMAGE_ID         = 'QEMUOVRD'
        self.BOARD_NAME               = 'qemuovrd'
        self.PCI_MEM32_BASE           = 0xA0000000
