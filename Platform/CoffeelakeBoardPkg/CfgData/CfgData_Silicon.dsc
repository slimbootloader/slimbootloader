## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGE:{SIL}
  # !BSF SUBT:{CFGHDR_TMPL:SILICON_CFG_DATA:0:0}

  # !HDR EMBED:{SILICON_CFG_DATA:TAG_600:START}

  # !BSF NAME:{Enable EC Device}
  # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/disable EC}
  gCfgData.ECEnable               |      * | 0x01 | 0x01

  # !BSF NAME:{Enable or Disable processor debug features} TYPE:{Combo}
  # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable or Disable processor debug features; <b>0: Disable</b>; 1: Enable.}
  gCfgData.DebugInterfaceEnable   |      * | 0x01 | 0x00

  # !BSF NAME:{Enable or Disable legacy serial port expose to OS} TYPE:{Combo}
  # !BSF TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable or Disable legacy serial port expose to OS; <b>0: Disable</b>; 1: Enable.}
  gCfgData.EnableLegacySerial     |      * | 0x01 | 0x00

  # !BSF PAGES:{PSEL:SIL:"Payload Selection GPIO"}
  # !BSF PAGE:{PSEL}
  # !BSF NAME:{GPIO pin for switching payload}
  # !HDR STRUCT:{PAYLOAD_SEL_GPIO_PIN}
  # !BSF HELP:{specify GPIO PIN number to read and switch payloads}
  gCfgData.PayloadSelGpio         |      * | 0x02 | 0
    # !BSF NAME:{Pad Group}
    # !BSF TYPE:{Combo}
    # !BSF OPTION:{0x0: GROUP_A, 0x1: GROUP_B, 0x2: GROUP_C, 0x3: GROUP_D, 0x4: GROUP_E, 0x5: GROUP_F, 0x6: GROUP_G, 0x7: GROUP_H, 0x8: GROUP_I, 0x9 : GROUP_J, 0xA: GROUP_K}
    # !BSF HELP:{Specify Pad Group}
    # !BSF FIELD:{Group:4bW}

    # !BSF NAME:{Pin Number}
    # !BSF TYPE:{EditNum, INT, (0,24)}
    # !BSF HELP:{Specify Pin Number}
    # !BSF FIELD:{PinNum:5bW}

    # !BSF NAME:{Reserved} TYPE:{Reserved}
    # !BSF FIELD:{Rsvd1:6bW}

    # !BSF NAME:{Payload Selection Pin Enable} TYPE:{Combo} OPTION:{$EN_DIS}
    # !BSF HELP:{Enable/Disable this pin for payload selection.}
    # !BSF FIELD:{Enable:1bW}

  gCfgData.SiliconRsvd            |      * | 0x03 | 0x00

  # !HDR EMBED:{SILICON_CFG_DATA:TAG_600:END}
