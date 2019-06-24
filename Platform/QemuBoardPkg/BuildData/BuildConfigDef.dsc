## @file
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

[PcdsDynamicVpd.Upd]
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

  # Global definitions in BSF
  # !BSF PAGES:{BLD::"Build Settings"}
  # !BSF BLOCK:{NAME:"QEMU Platform", VER:"0.1"}

  # !BSF PAGE:{BLD}

  # !BSF NAME:{PCI_IO_BASE}
  # !BSF TYPE:{EditNum, HEX, (0x0000,0xFFFF)}
  # !BSF HELP:{Specify PCI IO16 base address}
  gCfgData.PCI_IO_BASE               |  0x0000  | 0x04 | 0x2000

  # !BSF NAME:{PCI_MEM32_BASE}
  # !BSF TYPE:{EditNum, HEX, (0x00000000,0xFFFFFFFF)}
  # !BSF HELP:{Specify PCI MMIO32 base address}
  gCfgData.PCI_MEM32_BASE            |      *  | 0x04 | 0x80000000
