## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{PLT}

  # !HDR HEADER:{OFF}

  # !BSF SUBT:{CFGHDR_TMPL:PLATFORMID_CFG_DATA:0:0}
  # !HDR EMBED:{PLATFORMID_CFG_DATA:TAG_0F0:START}
  # !BSF NAME:{Platform ID} TYPE:{Combo}
  # !BSF OPTION:{  0:Board  0,  1:Board  1,  2:Board  2,  3:Board  3,  4:Board  4,  5:Board  5,  6:Board  6,  7:Board  7,}
  # !BSF OPTION:{+ 8:Board  8,  9:Board  9, 10:Board 10, 11:Board 11, 12:Board 12, 13:Board 13, 14:Board 14, 15:Board 15,}
  # !BSF OPTION:{+16:Board 16, 17:Board 17, 18:Board 18, 19:Board 19, 20:Board 20, 21:Board 21, 22:Board 22, 23:Board 23,}
  # !BSF OPTION:{+24:Board 24, 25:Board 25, 26:Board 26, 27:Board 27, 28:Board 28, 29:Board 29, 30:Board 30, 31:Board 31}
  # !BSF HELP:{Select platform ID}
  gCfgData.PlatformId             |      * | 0x02 | 0x0000

  gCfgData.PseRsvd                |      * | 0x02 | 0x0000
  # !HDR EMBED:{PLATFORMID_CFG_DATA:TAG_0F0:END}

  # !HDR HEADER:{ON}

  # ---------------------------------------------------------------------------------------
  # !BSF PAGE:{PLT}
  # !BSF SUBT:{CFGHDR_TMPL:PLAT_NAME_CFG_DATA:0:0}

  # !HDR EMBED:{PLAT_NAME_CFG_DATA:TAG_070:START}
  # !BSF NAME:{Platform Name}
  # !BSF TYPE:{EditText}
  # !BSF HELP:{Specify the platform name, a maximum of 8 characters}
  gCfgData.PlatformName           |      * | 0x8  | ' '
  # !HDR EMBED:{PLAT_NAME_CFG_DATA:TAG_070:END}
