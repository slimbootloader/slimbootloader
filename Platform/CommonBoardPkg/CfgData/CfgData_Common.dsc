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

  #
  # This section is not used by the normal build process
  # However, FSP will use dedicated tool to handle it and generate a
  # VPD similar binary block (User Configuration Data). This block will
  # be accessed through a generated data structure directly rather than
  # PCD services. This is for size consideration.
  # Format:
  #   gCfgData.Updxxxxxxxxxxxxn      | OFFSET | LENGTH | VALUE
  # Only simple data type is supported
  #
  #
  # Comments with !BSF will be used to generate BSF file
  # Comments with !HDR will be used to generate H header file
  #
  # --------------------------------------------------------------------------------------
  # !BSF PAGE:{GEN}
  gCfgData.CfgHeader              |      * | 0x04 | {0x01:2b, ((_LENGTH_GEN_CFG_DATA_+8)/4):10b, 0:4b, 0:4b, _TAG_GEN_CFG_DATA_:12b}
  gCfgData.CondValue              |      * | 0x04 | 0x00000000

  # !HDR HEADER:{ON}
  # !HDR EMBED:{GEN_CFG_DATA:TAG_010:START}
  # !BSF NAME:{Debug Print Level}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{Specify debug print level}
  gCfgData.DebugPrintLevel        |      * | 0x04 | 0x8000004F

  # !BSF NAME:{Payload ID}
  # !BSF TYPE:{EditText}
  # !BSF HELP:{Specify payload ID string. Empty will boot default payload. Otherwise, boot specified payload ID in multi-payload binary.}
  gCfgData.PayloadId             |      * | 0x04 | ''

  # !BSF NAME:{OS Crash Memory Size}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{ 0:0MB, 1:4MB, 2:8MB, 3:16MB, 4:32MB, 5:64MB}
  # !BSF HELP:{Specify reserved memory size for OS to save OS crash data. The reserved memory address should be same across reset.}
  gCfgData.OsCrashMemorySize           |      * | 0x01 | 1

  # !BSF NAME:{Enable Crash Mode}
  # !BSF OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/Disable Crash Mode. Boot into Crash OS only when crash mode is enabled.}
  gCfgData.EnableCrashMode             |      * | 0x01 | 0

  # !BSF NAME:{RpmbKeyProvisioning} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable or disable Rpmb Key Provisioning}
  gCfgData.RpmbKeyProvisioning         |      * | 0x01 | 0

  gCfgData.Reserved                    |      * | 0x01 | 0
  # !HDR EMBED:{GEN_CFG_DATA:TAG_010:END}