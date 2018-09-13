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

  # !BSF PAGES:{GPU:SIL:"GPU"}
  # !BSF PAGES:{GPU_CFG:GPU:"GPU Config"}

  # !BSF PAGE:{GPU_CFG}

  # !BSF SUBT:{CFGHDR_TMPL:GPU_CFG_DATA:0:0}

  # !HDR EMBED:{GPU_CFG_DATA:TAG_308:START}

  # !BSF NAME:{ApertureSize}
  # !BSF TYPE:{Combo}
  # !BSF OPTION:{0x1:128 MB, 0x2:256 MB, 0x3:512 MB}
  # !BSF HELP:{Select the Aperture Size used by the Internal Graphics Device. 0x1:128 MB(Default), 0x2:256 MB, 0x3:512 MB.}
  gCfgData.ApertureSize                 |      * | 0x01 | 0x1

  # !BSF NAME:{Igd} TYPE:{Combo} OPTION:{$EN_DIS}
  # !BSF HELP:{When enabled, Integrated Graphics Device (IGD) is selected as the Primary Video}
  gCfgData.Igd                          |      * | 0x01 | 0x1

  # !BSF NAME:{IgdDvmt50PreAlloc} TYPE:{Combo}
  # !BSF OPTION:{ 0x02:64 MB, 0x03:96 MB, 0x04:128 MB, 0x05:160 MB, 0x06:192 MB, 0x07:224 MB, 0x08:256 MB, 0x09:288 MB,}
  # !BSF OPTION:{+0x0A:320 MB, 0x0B:352 MB, 0x0C:384 MB, 0x0D:416 MB, 0x0E:448 MB, 0x0F:480 MB, 0x10:512 MB}
  # !BSF HELP:{Select DVMT 5.0 Pre-Allocated (Fixed) Graphics Memory size used by the Internal Graphics Device}
  gCfgData.IgdDvmt50PreAlloc            |      * | 0x01 | 0x2

  # !BSF NAME:{GttSize} TYPE:{Combo}
  # !BSF OPTION:{0x1:2 MB, 0x2:4 MB, 0x3:8 MB}
  # !BSF HELP:{Select the GTT Size used by the Internal Graphics Device. 0x1:2 MB, 0x2:4 MB, 0x3:8 MB(Default).}
  gCfgData.GttSize                      |      * | 0x01 | 0x3

  # !BSF NAME:{PrimaryVideoAdaptor} TYPE:{Combo}
  # !BSF OPTION:{0x0:AUTO, 0x2:IGD, 0x3:PCI}
  # !BSF HELP:{Select which of IGD/PCI Graphics device should be Primary Display. 0x0:AUTO(Default), 0x2:IGD, 0x3:PCI}
  gCfgData.PrimaryVideoAdaptor          |      * | 0x01 | 0x0

  gCfgData.Dummy                  |      * | 0x03 | 0x0
  # !HDR EMBED:{GPU_CFG_DATA:TAG_308:END}