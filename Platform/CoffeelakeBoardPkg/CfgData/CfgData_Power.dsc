## @file
#
#  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
  # !BSF PAGES:{PWR:SIL:"POWER"}
  # !BSF PAGE:{PWR}
  # !BSF SUBT:{CFGHDR_TMPL:POWER_CFG_DATA:0:0}
  # !HDR EMBED:{POWER_CFG_DATA:TAG_610:START}

  # !BSF NAME:{1-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{1-Core Ratio Limit: LFM to Fused, For overclocking part: LFM to 255. This 1-Core Ratio Limit Must be greater than or equal to 2-Core Ratio Limit, 3-Core Ratio Limit, 4-Core Ratio Limit, 5-Core Ratio Limit, 6-Core Ratio Limit, 7-Core Ratio Limit, 8-Core Ratio Limit. Range is 0 to 255}
  gCfgData.OneCoreRatioLimit                 |      * | 0x01 | 0x2C

  # !BSF NAME:{2-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{LFM to Fused, For overclocking part: LFM to 255. This 2-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.Range is 0 to 255}
  gCfgData.TwoCoreRatioLimit                 |      * | 0x01 | 0x2C

  # !BSF NAME:{3-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{LFM to Fused, For overclocking part: LFM to 255. This 3-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.Range is 0 to 255}
  gCfgData.ThreeCoreRatioLimit               |      * | 0x01 | 0x2A

  # !BSF NAME:{4-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{LFM to Fused, For overclocking part: LFM to 255. This 4-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.Range is 0 to 255}
  gCfgData.FourCoreRatioLimit                |      * | 0x01 | 0x2A

  # !BSF NAME:{5-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{LFM to Fused, For overclocking part: LFM to 255. This 5-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.Range is 0 to 255 0x0:0xFF}
  gCfgData.FiveCoreRatioLimit                |      * | 0x01 | 0

  # !BSF NAME:{6-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{LFM to Fused, For overclocking part: LFM to 255. This 6-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.Range is 0 to 255 0x0:0xFF}
  gCfgData.SixCoreRatioLimit                 |      * | 0x01 | 0

  # !BSF NAME:{7-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{LFM to Fused, For overclocking part: LFM to 255. This 7-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.Range is 0 to 255 0x0:0xFF}
  gCfgData.SevenCoreRatioLimit               |      * | 0x01 | 0

  # !BSF NAME:{8-Core Ratio Limit}
  # !BSF TYPE:{EditNum, HEX, (0,0xFF)}
  # !BSF HELP:{LFM to Fused, For overclocking part: LFM to 255. This 8-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.Range is 0 to 255 0x0:0xFF}
  gCfgData.EightCoreRatioLimit               |      * | 0x01 | 0

  # !BSF NAME:{Thermal Sensor 0 Target Width}
  # !BSF TYPE:{Combo} OPTION:{0:x1, 1:x2, 2:x4, 3:x8, 4:x16}
  # !BSF HELP:{DMT thermal sensor suggested representative values.}
  gCfgData.DmiTS0TW                         |      * | 0x01 | 0x3

  # !BSF NAME:{Thermal Sensor 1 Target Width}
  # !BSF TYPE:{Combo} OPTION:{0:x1, 1:x2, 2:x4, 3:x8, 4:x16}
  # !BSF HELP:{DMT thermal sensor suggested representative values.}
  gCfgData.DmiTS1TW                         |      * | 0x01 | 0x2

  # !BSF NAME:{Thermal Sensor 2 Target Width}
  # !BSF TYPE:{Combo} OPTION:{0:x1, 1:x2, 2:x4, 3:x8, 4:x16}
  # !BSF HELP:{DMT thermal sensor suggested representative values.}
  gCfgData.DmiTS2TW                         |      * | 0x01 | 0x1

  # !BSF NAME:{Memory Thermal Management} TYPE:{Combo}
  # !BSF OPTION:{0:Disabled, 1:Enabled}
  # !BSF HELP:{Enable Memory Thermal Management}
  gCfgData.MemoryThermalMgmt              |      * | 0x01 | 0x1

  # !HDR EMBED:{POWER_CFG_DATA:TAG_610:END}

