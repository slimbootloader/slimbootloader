## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
  #
  # ================================= SPD_TMPL =========================================
  #
    =======================================================================================
  # !BSF DEFT:{SPD_TMPL:START}
    # !BSF NAME:{Spd Data CH$(1) DIMM$(2)} TYPE:{Combo}
    # !BSF OPTION:{0: NONE, 1: SPD0, 2: SPD1}
    # !BSF HELP:{Select which SPD data should be used for CH$(1) DIMM$(2).SPD0 and SPD1 are associated with configurable SPD data block from MEM SPD}
    gCfgData.SpdDataSel$(1)$(2)                     |      * | 0x01 | $(3)

  # !BSF DEFT:{SPD_TMPL:END}