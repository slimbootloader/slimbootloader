## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGE:{MEM}
  # !BSF SUBT:{CFGHDR_TMPL:MEMORY_CFG_DATA:0:0}

  # !HDR EMBED:{MEMORY_CFG_DATA:TAG_200:START}

  # !BSF PAGES:{MEM_SPD00:MEM:"SPD Data (Channel 0 Dimm 0)"}
  # !BSF PAGE:{MEM_SPD00}
  # !BSF NAME:{Memory SPD Pointer Channel 0 Dimm 0}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX, C:1:HEX, D:1:HEX, E:1:HEX, F:1:HEX}
  # !BSF HELP:{Pointer to SPD data in Memory}
  gCfgData.MemorySpdPtr00                   |      * | 0x200 | {FILE: spd00.bin}

  # !BSF PAGES:{MEM_SPD01:MEM:"SPD Data (Channel 0 Dimm 1)"}
  # !BSF PAGE:{MEM_SPD01}
  # !BSF NAME:{Memory SPD Pointer Channel 0 Dimm 1}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX, C:1:HEX, D:1:HEX, E:1:HEX, F:1:HEX}
  # !BSF HELP:{Pointer to SPD data in Memory}
  gCfgData.MemorySpdPtr01                   |      * | 0x200 | 0

  # !BSF PAGES:{MEM_SPD10:MEM:"SPD Data (Channel 1 Dimm 0)"}
  # !BSF PAGE:{MEM_SPD10}
  # !BSF NAME:{Memory SPD Pointer Channel 1 Dimm 0}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX, C:1:HEX, D:1:HEX, E:1:HEX, F:1:HEX}
  # !BSF HELP:{Pointer to SPD data in Memory}
  gCfgData.MemorySpdPtr10                   |      * | 0x200 | {FILE: spd00.bin}

  # !BSF PAGES:{MEM_SPD11:MEM:"SPD Data (Channel 1 Dimm 1)"}
  # !BSF PAGE:{MEM_SPD11}
  # !BSF NAME:{Memory SPD Pointer Channel 1 Dimm 1}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX, C:1:HEX, D:1:HEX, E:1:HEX, F:1:HEX}
  # !BSF HELP:{Pointer to SPD data in Memory}
  gCfgData.MemorySpdPtr11                   |      * | 0x200 | 0

  # !BSF PAGE:{MEM}
  # !BSF NAME:{Spd Address Tabl} TYPE:{EditNum, HEX, (0x00,0xFFFFFFFF)}
  # !BSF HELP:{Specify SPD Address table for CH0D0/CH0D1/CH1D0&CH1D1. MemorySpdPtr will be used if SPD Address is 00}
  gCfgData.SpdAddressTable                  |      * | 0x04 | { 0x0, 0x0, 0x0, 0x0 }

  # !BSF NAME:{Dq Byte Map CH0}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX}
  # !BSF HELP:{Dq byte mapping between CPU and DRAM, Channel 0: board-dependent}
  gCfgData.DqByteMapCh0                     |      * | 0xC | { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0, 0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 }

  # !BSF NAME:{Dq Byte Map CH1}
  # !BSF TYPE:{Table}
  # !BSF OPTION:{ 0:1:HEX, 1:1:HEX, 2:1:HEX, 3:1:HEX, 4:1:HEX, 5:1:HEX, 6:1:HEX, 7:1:HEX,}
  # !BSF OPTION:{+ 8:1:HEX, 9:1:HEX, A:1:HEX, B:1:HEX}
  # !BSF HELP:{Dq byte mapping between CPU and DRAM, Channel 0: board-dependent}
  gCfgData.DqByteMapCh1                     |      * | 0xC | { 0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC, 0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 }

  # !BSF NAME:{Dqs Map CPU to DRAM CH 0} TYPE:{EditNum, HEX, (0x00,0xFFFFFFFFFFFFFFFF)}
  # !BSF HELP:{Set Dqs mapping relationship between CPU and DRAM, Channel 0: board-dependent}
  gCfgData.DqsMapCpu2DramCh0                |     * | 0x08 | {0, 1, 3, 2, 4, 5, 6, 7}

  # !BSF NAME:{Dqs Map CPU to DRAM CH 1} TYPE:{EditNum, HEX, (0x00,0xFFFFFFFFFFFFFFFF)}
  # !BSF HELP:{Set Dqs mapping relationship between CPU and DRAM, Channel 1: board-dependent}
  gCfgData.DqsMapCpu2DramCh1                |     * | 0x08 | {1, 0, 4, 5, 2, 3, 6, 7}

  # !BSF NAME:{Dqs Pins Interleaved Setting} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Indicates DqPinsInterleaved setting: board-dependent}
  gCfgData.DqPinsInterleaved                |      * | 0x01 | 0x1

  # !BSF NAME:{MMIO Size}
  # !BSF TYPE:{EditNum, HEX, (0,0xC00)}
  # !BSF HELP:{Size of MMIO space reserved for devices. non-Zero=size in MB }
  gCfgData.MmioSize                         |      * | 0x02 | 0x0

  # !BSF NAME:{Rank Margin Tool} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/disable Rank Margin Tool.}
  gCfgData.RMT                              |      * | 0x01 | 0x01

  # !BSF NAME:{Generate BIOS Data ACPI Table} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable: Generate BDAT for MRC RMT or SA PCIe data. Disable (Default): Do not generate it}
  gCfgData.BdatEnable                       |      * | 0x01 | 0x01

  # !BSF NAME:{BdatTestType} TYPE:{Combo}
  # !BSF OPTION:{0:Rank Margin Tool, 1:Margin2D}
  # !BSF HELP:{Indicates the type of Memory Training data to populate into the BDAT ACPI table.}
  gCfgData.BdatTestType                     |      * | 0x1 | 0x00

  # !BSF NAME:{Processor Early Power On Configuration FCLK setting}
  # !BSF TYPE:{Combo} OPTION:{0:800 MHz, 1: 1 GHz, 2: 400 MHz, 3: Reserved }
  # !BSF HELP:{ <b>0: 800 MHz (ULT/ULX)</b>. <b>1: 1 GHz (DT/Halo)</b>. Not supported on ULT/ULX.- 2: 400 MHz. - 3: Reserved}
  gCfgData.FClkFrequency                    |      * | 0x01 | 0x00

  # !BSF NAME:{Board Type}
  # !BSF TYPE:{Combo} OPTION:{0:Mobile/Mobile Halo, 1:Desktop/DT Halo, 5:ULT/ULX/Mobile Halo, 7:UP Server}
  # !BSF HELP:{MrcBoardType, Options are 0=Mobile/Mobile Halo, 1=Desktop/DT Halo, 5=ULT/ULX/Mobile Halo, 7=UP Server}
  gCfgData.UserBd                           |      * | 0x01 | 0x05

  # !BSF NAME:{Dual Dimm Per Channel Board Type}
  # !BSF TYPE:{Combo} OPTION:{0:1DPC, 1:2DPC}
  # !BSF HELP:{Option to indicate if Board Layout includes One/Two DIMMs per channel. This is used to limit maximum frequency for some SKUs.}
  gCfgData.DualDpc                          |      * | 0x01 | 0x00

  # !BSF NAME:{PCH Trace Hub Mode} TYPE:{Combo}
  # !BSF OPTION:{0: Disable, 1: Target Debugger Mode, 2: Host Debugger Mode}
  # !BSF HELP:{Select 'Host Debugger' if Trace Hub is used with host debugger tool or 'Target Debugger' if Trace Hub is used by target debugger software or 'Disable' trace hub functionality.}
  gCfgData.PchTraceHubMode                  |     * | 0x01 | 0x00

  # !BSF NAME:{Platform Debug Consent} TYPE:{Combo}
  # !BSF OPTION:{0:Disabled, 1:Enabled (DCI OOB+[DbC]), 2:Enabled (DCI OOB), 3:Enabled (USB3 DbC), 4:Enabled (XDP/MIPI60), 5:Enabled (USB2 DbC)}
  # !BSF HELP:{To 'opt-in' for debug, please select 'Enabled' with the desired debug probe type. Enabling this BIOS option may alter the default value of other debug-related BIOS options. Note: DCI OOB (aka BSSB) uses CCA probe; [DCI OOB+DbC] and [USB2 DbC] have the same setting}
  gCfgData.PlatformDebugConsent             |     * | 0x01 | 0x00

  gCfgData.MemRsvd                          |     * | 0x01 | 0x00
  # !HDR EMBED:{MEMORY_CFG_DATA:TAG_200:END}
