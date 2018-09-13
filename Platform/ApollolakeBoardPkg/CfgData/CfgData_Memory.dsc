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

  # !BSF PAGE:{MEM}
  # !BSF SUBT:{CFGHDR_TMPL:MEMORY_CFG_DATA:0:0}

  # !HDR EMBED:{MEMORY_CFG_DATA:TAG_200:START}

  # !BSF NAME:{MrcFastBoot} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/Disable MRC fast boot support}
  gCfgData.MrcFastBoot                  |      * | 0x01 | 0x1

  # !BSF NAME:{Package} TYPE:{Combo}
  # !BSF OPTION:{0x0:SODIMM, 0x1:BGA, 0x2:BGA mirrored (LPDDR3 only), 0x3:SODIMM/UDIMM with Rank 1 Mirrored (DDR3L)}
  # !BSF HELP:{Specifies CA Mapping for all technologies}
  gCfgData.Package                      |      * | 0x01 | 0x1

  # !BSF NAME:{Profile} TYPE:{EditNum, HEX, (0x00,0xFF)}
  # !BSF HELP:{Select Profile}
  gCfgData.Profile                      |      * | 0x01 | 0xB

  # !BSF NAME:{MemoryDown} TYPE:{Combo}
  # !BSF OPTION:{0x0:No, 0x1:Yes, 0x2:1MD+SODIMM (for DDR3L only) ACRD, 0x3:1x32 LPDDR4}
  # !BSF HELP:{Memory Down}
  gCfgData.MemoryDown                   |      * | 0x01 | 0x1

  # !BSF NAME:{DDR3LPageSize} TYPE:{Combo}
  # !BSF OPTION:{0x0:AUTO, 0x1:1KB, 0x2:2KB}
  # !BSF HELP:{DDR3L Page Size}
  gCfgData.DDR3LPageSize                |      * | 0x01 | 0x0

  # !BSF NAME:{DDR3LASR} TYPE:{Combo}
  # !BSF OPTION:{0x0:Not Supported, 0x1:Supported}
  # !BSF HELP:{DDR3L ASR}
  gCfgData.DDR3LASR                     |      * | 0x01 | 0x0

  # !BSF NAME:{ScramblerSupport} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable or disable the memory scrambler}
  gCfgData.ScramblerSupport             |      * | 0x01 | 0x1

  # !BSF NAME:{ChannelHashMask} TYPE:{EditNum, HEX, (0x00,0xFFFF)}
  # !BSF HELP:{Specify channel hash mask which is an input param to the channel hashing algorithm}
  gCfgData.ChannelHashMask              |      * | 0x02 | 0x0

  # !BSF NAME:{SliceHashMask} TYPE:{EditNum, HEX, (0x00,0xFFFF)}
  # !BSF HELP:{Specify slice hash mask which is an input param to the channel hashing algorithm}
  gCfgData.SliceHashMask                |      * | 0x02 | 0x0

  # !BSF NAME:{InterleavedMode} TYPE:{Combo}
  # !BSF OPTION:{0x0:Disable, 0x2:Enable}
  # !BSF HELP:{Interleaved Mode}
  gCfgData.InterleavedMode              |      * | 0x01 | 0x0

  # !BSF NAME:{ChannelsSlicesEnable} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Channels Slices Enable}
  gCfgData.ChannelsSlicesEnable         |      * | 0x01 | 0x0

  # !BSF NAME:{MinRefRate2xEnable} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{When enabled, it provides defence against Row-Hammer attacks}
  gCfgData.MinRefRate2xEnable           |      * | 0x01 | 0x0

  # !BSF NAME:{DualRankSupportEnable} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Dual Rank Support Enable}
  gCfgData.DualRankSupportEnable        |      * | 0x01 | 0x1

  # !BSF NAME:{RmtMode} TYPE:{Combo}
  # !BSF OPTION:{0x0:Disabled, 0x1:Enabled, 0x3:Enabled(Enhanced)}
  # !BSF HELP:{Rank Margin Tool Mode}
  gCfgData.RmtMode                      |      * | 0x01 | 0x1

  # !BSF NAME:{MemorySizeLimit} TYPE:{EditNum, HEX, (0x00,0xFFFF)}
  # !BSF HELP:{This value(in MB) is used to restrict the total amount of memory and the calculations based on it}
  gCfgData.MemorySizeLimit              |      * | 0x02 | 0x1800

  # !BSF NAME:{LowMemoryMaxValue} TYPE:{EditNum, HEX, (0x00,0xFFFF)}
  # !BSF HELP:{This value(in MB) is used to restrict the amount of memory below 4GB and the calculations based on it}
  gCfgData.LowMemoryMaxValue            |      * | 0x02 | 0x0

  # !BSF NAME:{HighMemoryMaxValue} TYPE:{EditNum, HEX, (0x00,0xFFFF)}
  # !BSF HELP:{This value(in MB) is used to restrict the amount of memory above 4GB and the calculations based on it}
  gCfgData.HighMemoryMaxValue           |      * | 0x02 | 0x0

  # !BSF NAME:{DisableFastBoot} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{If enabled, use saved training data (if valid) after first boot}
  gCfgData.DisableFastBoot              |      * | 0x01 | 0x0

  # !BSF NAME:{DIMM0SPDAddress} TYPE:{EditNum, HEX, (0x00,0xFF)}
  # !BSF HELP:{DIMM0 SPD Address}
  gCfgData.DIMM0SPDAddress              |      * | 0x01 | 0x0

  # !BSF NAME:{DIMM1SPDAddress} TYPE:{EditNum, HEX, (0x00,0xFF)}
  # !BSF HELP:{DIMM1 SPD Address }
  gCfgData.DIMM1SPDAddress              |      * | 0x01 | 0x0

  # MEMCH_TMPL structure
  # Channel
  # UINT8   RankEnable;
  # UINT8   DeviceWidth;
  # UINT8   DramDensity;
  # UINT8   Option;
  # UINT8   OdtConfig;
  # UINT8   TristateClk1;
  # UINT8   Mode2N;
  # UINT8   OdtLevels;

  # !BSF SUBT:{MEMCH_TMPL: Ch0 : 0x3 : 0x1 : 0x1 : 0x3 : 0x0 : 0x0 : 0x0 : 0x0}
  # !BSF SUBT:{MEMCH_TMPL: Ch1 : 0x3 : 0x1 : 0x1 : 0x3 : 0x0 : 0x0 : 0x0 : 0x0}
  # !BSF SUBT:{MEMCH_TMPL: Ch2 : 0x3 : 0x1 : 0x1 : 0x3 : 0x0 : 0x0 : 0x0 : 0x0}
  # !BSF SUBT:{MEMCH_TMPL: Ch3 : 0x3 : 0x1 : 0x1 : 0x3 : 0x0 : 0x0 : 0x0 : 0x0}

  # !BSF NAME:{RmtCheckRun} TYPE:{EditNum, HEX, (0x00,0xFF)}
  # !BSF HELP:{Parameter used to determine whether to run the margin check}
  gCfgData.RmtCheckRun                  |      * | 0x01 | 0x0

  # !BSF NAME:{RmtMarginCheckScaleHighThreshold} TYPE:{EditNum, HEX, (0x00,0xFFFF)}
  # !BSF HELP:{Percentage used to determine the margin tolerances over the failing margin}
  gCfgData.RmtMarginCheckScaleHighThreshold |      * | 0x02 | 0xC8

  # !BSF NAME:{Ch0_Bit_swizzling}
  # !BSF TYPE:{EditNum, HEX, (0x00,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)}
  # !BSF HELP:{Channel 0 PHY to DUnit DQ mapping}
  gCfgData.Ch0_Bit_swizzling                |      * | 0x20 | { 0x09, 0x0e, 0x0c, 0x0d, 0x0a, 0x0b, 0x08, 0x0f, 0x05, 0x06, 0x01, 0x00, 0x02, 0x07, 0x04, 0x03, 0x1a, 0x1f, 0x1c, 0x1b, 0x1d, 0x19, 0x18, 0x1e, 0x14, 0x16, 0x17, 0x11, 0x12, 0x13, 0x10, 0x15 }
  # !BSF NAME:{Ch1_Bit_swizzling}
  # !BSF TYPE:{EditNum, HEX, (0x00,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)}
  # !BSF HELP:{Channel 1 PHY to DUnit DQ mapping}
  gCfgData.Ch1_Bit_swizzling                |      * | 0x20 | { 0x06, 0x07, 0x05, 0x04, 0x03, 0x01, 0x00, 0x02, 0x0c, 0x0a, 0x0b, 0x0d, 0x0e, 0x08, 0x09, 0x0f, 0x14, 0x10, 0x16, 0x15, 0x12, 0x11, 0x13, 0x17, 0x1e, 0x1c, 0x1d, 0x19, 0x18, 0x1a, 0x1b, 0x1f }
  # !BSF NAME:{Ch2_Bit_swizzling}
  # !BSF TYPE:{EditNum, HEX, (0x00,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)}
  # !BSF HELP:{Channel 2 PHY to DUnit DQ mapping}
  gCfgData.Ch2_Bit_swizzling                |      * | 0x20 | { 0x0f, 0x09, 0x08, 0x0b, 0x0c, 0x0d, 0x0e, 0x0a, 0x05, 0x02, 0x00, 0x03, 0x06, 0x07, 0x01, 0x04, 0x19, 0x1c, 0x1e, 0x1f, 0x1a, 0x1b, 0x18, 0x1d, 0x14, 0x17, 0x16, 0x15, 0x12, 0x13, 0x10, 0x11 }
  # !BSF NAME:{Ch3_Bit_swizzling}
  # !BSF TYPE:{EditNum, HEX, (0x00,0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)}
  # !BSF HELP:{Channel 2 PHY to DUnit DQ mapping}
  gCfgData.Ch3_Bit_swizzling                |      * | 0x20 | { 0x03, 0x04, 0x06, 0x05, 0x00, 0x01, 0x02, 0x07, 0x0b, 0x0a, 0x08, 0x09, 0x0e, 0x0c, 0x0f, 0x0d, 0x11, 0x17, 0x13, 0x10, 0x15, 0x16, 0x14, 0x12, 0x1c, 0x1d, 0x1a, 0x19, 0x1e, 0x1b, 0x18, 0x1f }

  # !BSF NAME:{MsgLevelMask} TYPE:{EditNum, HEX, (0x00,0xFFFFFFFF)}
  # !BSF HELP:{Specify 32 bits that used to mask out debug messages}
  gCfgData.MsgLevelMask                     |      * | 0x04 | 0x0

  # !BSF NAME:{EnhancePort8xhDecoding} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/Disable Enhance the port 8xh decoding}
  gCfgData.EnhancePort8xhDecoding           |      * | 0x01 | 0x0

  # !BSF NAME:{MrcDataSaving} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/Disable MRC training data saving in FSP}
  gCfgData.MrcDataSaving                    |      * | 0x01 | 0x1

  # !BSF NAME:{SpdWriteEnable} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/Disable SPD data write on the SMBUS}
  gCfgData.SpdWriteEnable                   |      * | 0x01 | 0x0

  # !BSF NAME:{NpkEn}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0:Disable, 1:Enable, 2:Debugger, 3:Auto}
  # !BSF HELP:{Enable/Disable Npk. 0:Disable, 1:Enable, 2:Debugger, 3:Auto(Default)}
  gCfgData.NpkEn                            |      * | 0x01 | 0x0

  gCfgData.MemRsvd                          |      * | 0x00 | 0x0
  # !HDR EMBED:{MEMORY_CFG_DATA:TAG_200:END}