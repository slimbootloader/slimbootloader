## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGES:{MEM_SPD:MEM:"MEM SPD"}
  # !BSF PAGE:{MEM_SPD}

  # !BSF SUBT:{CFGHDR_TMPL:MEM_SPD0_CFG_DATA:0:0}
  # !HDR EMBED:{MEM_SPD0_CFG_DATA:TAG_8F0:START}
  # !BSF NAME:{Memory SPD Data 0}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX, C:1:HEX, D:1:HEX, E:1:HEX, F:1:HEX}
  # !BSF HELP:{Pointer to SPD data in Memory}
  gCfgData.MemorySpdPtr0                   |      * | 0x200 | {FILE: Spd_Upx8G.bin}
  # !HDR EMBED:{MEM_SPD0_CFG_DATA:TAG_8F0:END}

  # !BSF SUBT:{CFGHDR_TMPL:MEM_SPD1_CFG_DATA:0:0}
  # !HDR EMBED:{MEM_SPD1_CFG_DATA:TAG_8F1:START}
  # !BSF NAME:{Memory SPD Data 1}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX, C:1:HEX, D:1:HEX, E:1:HEX, F:1:HEX}
  # !BSF HELP:{Pointer to SPD data in Memory}
  gCfgData.MemorySpdPtr1                   |      * | 0x200 | {FILE: Spd_Upx4G.bin}
  # !HDR EMBED:{MEM_SPD1_CFG_DATA:TAG_8F1:END}
