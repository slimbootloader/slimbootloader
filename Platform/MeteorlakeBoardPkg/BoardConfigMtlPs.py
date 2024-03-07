## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2023 - 2024, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

##
# Import Modules
#
import os
import sys
tool_dir = os.path.realpath(os.path.join(os.path.dirname (os.path.realpath(__file__)), '..', 'MeteorlakeBoardPkg'))
sys.path.append (tool_dir)

import BoardConfigMtlp as MeteorlakeBoardConfig

class Board(MeteorlakeBoardConfig.Board):
    def __init__(self, *args, **kwargs):
        super(Board, self).__init__(*args, **kwargs)
        self.VERINFO_IMAGE_ID     = 'SB_MTLPS'
        self.BOARD_NAME           = 'mtlps'
        self._PS_SUPPORT          = True
        self.ACPI_TABLE_INF_FILE  = 'Platform/MeteorlakeBoardPkg/AcpiTables/AcpiTablesPs.inf'
