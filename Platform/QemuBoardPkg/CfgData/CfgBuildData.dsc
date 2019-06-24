## @file
#
#  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{BLD}
  # !HDR HEADER:{OFF}

  # !BSF SUBT:{CFGHDR_TMPL:BUILD_CFG_DATA:0:0}
  # !HDR EMBED:{BUILD_CFG_DATA:TAG_500:START}
  # !BSF NAME:{STAGE2_FD_BASE}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{FD base address to stage 2}
  gCfgData.STAGE2_FD_BASE         |      * | 0x04 | 0x01000000

  # !BSF NAME:{TEST1}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{Testing with reserved data if more to add}
  gCfgData.Reserved               |      * | 0x04 | 0xDEADBEEF
  # !HDR EMBED:{BUILD_CFG_DATA:TAG_500:END}

  # !HDR HEADER:{ON}