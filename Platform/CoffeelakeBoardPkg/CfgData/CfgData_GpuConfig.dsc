## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

  # !BSF PAGES:{GPU:SIL:"GPU"}
  # !BSF PAGES:{GPU_CFG:GPU:"GPU Config"}

  # !BSF PAGE:{GPU_CFG}

  # !BSF SUBT:{CFGHDR_TMPL:GPU_CFG_DATA:0:0}

  # !HDR EMBED:{GPU_CFG_DATA:TAG_308:START}

 # !BSF NAME:{Internal Graphics Pre-allocated Memory} TYPE:{Combo}
  # !BSF OPTION:{0x00:0MB, 0x01:32MB, 0x02:64MB, 0x03:96MB, 0x04:128MB, 0x05:160MB, 0xF0:4MB, 0xF1:8MB, 0xF2:12MB, 0xF3:16MB, 0xF4:20MB,}
  # !BSF OPTION:{+ 0xF5:24MB, 0xF6:28MB, 0xF7:32MB, 0xF8:36MB, 0xF9:40MB, 0xFA:44MB, 0xFB:48MB, 0xFC:52MB, 0xFD:56MB, 0xFE:60MB}
  # !BSF HELP:{Size of memory preallocated for internal graphics.}
  gCfgData.IgdDvmt50PreAlloc           |    * | 0x01 | 0x01

  # !BSF NAME:{Aperture Size} TYPE:{Combo}
  # !BSF OPTION:{0:128 MB, 1:256 MB, 2:512 MB}
  # !BSF HELP:{Select the Aperture Size.}
  gCfgData.ApertureSize                |    * | 0x01 | 0x01

  # !BSF NAME:{Internal Graphics} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{Enable/disable internal graphics.}
  gCfgData.InternalGfx                 |    * | 0x01 | 0x02

  # !BSF NAME:{Selection of the primary display device} TYPE:{Combo}
  # !BSF OPTION:{0:iGFX, 1:PEG, 2:PCIe Graphics on PCH, 3:AUTO, 4:Switchable Graphics}
  # !BSF HELP:{0=iGFX, 1=PEG, 2=PCIe Graphics on PCH, 3(Default)=AUTO, 4=Switchable Graphics}
  gCfgData.PrimaryDisplay              |    * | 0x01 | 0x3

  # !BSF NAME:{Selection of iGFX GTT Memory size} TYPE:{Combo}
  # !BSF OPTION:{1:2MB, 2:4MB, 3:8MB}
  # !BSF HELP:{1=2MB, 2=4MB, 3=8MB}
  gCfgData.GttSize                     |    * | 0x02 | 0x3

  # !BSF NAME:{eDP device} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortEdp                  |    * | 0x01 | 0x1

  # !BSF NAME:{HPD of DDI port B} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortBHpd                 |    * | 0x01 | 0x1

  # !BSF NAME:{HPD of DDI port C} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortCHpd                 |    * | 0x01 | 0x1

  # !BSF NAME:{HPD of DDI port D} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortDHpd                 |    * | 0x01 | 0x1

  # !BSF NAME:{HPD of DDI port F} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortFHpd                 |    * | 0x01 | 0x1

  # !BSF NAME:{DDC of DDI port B} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortBDdc                 |    * | 0x01 | 0x1

  # !BSF NAME:{DDC of DDI port C} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortCDdc                 |    * | 0x01 | 0x1

  # !BSF NAME:{DDC of DDI port D} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortDDdc                 |    * | 0x01 | 0x1

  # !BSF NAME:{DDC of DDI port F} TYPE:{Combo}
  # !BSF OPTION:{1:Enable, 0:Disable}
  # !BSF HELP:{Enable or disable}
  gCfgData.DdiPortFDdc                 |    * | 0x01 | 0x0

  gCfgData.Dummy                       |    * | 0x01 | 0x0

  # !HDR EMBED:{GPU_CFG_DATA:TAG_308:END}