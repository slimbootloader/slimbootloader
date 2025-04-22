/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// the 'SATA_CTRL' a macro to instatiate actual server SATA controller name
// the 'SATA_PCI_ADDR' a macro to instatiate actual server SATA controller PCI addr
Device (SATA_CTRL) {
  Method (_ADR, 0) {
    return (SATA_PCI_ADDR)
  }
  Method(_PS0,0,Serialized) {
    ADBG("SATA0 Ctrlr D0")
    //
    // Call CSD0 only if ST_FDIS_PMC == 0
    //
    If(LEqual(PCHS, PCHL)) {
      If(LNot(SCFD)){
        \_SB.CSD0(MODPHY_SPD_GATING_SATA)
      }
    }
  }

  Method(_PS3,0,Serialized) {
    ADBG("SATA0 Ctrlr D3")
    If(LEqual(PCHS, PCHL)) {
      \_SB.CSD3(MODPHY_SPD_GATING_SATA)
    }
  }

  Method(_S0W) {
    Return(3)
  }

  Device(PRT0)
  {
    Name(_ADR,0x0000FFFF)
  }

  Device(PRT1)
  {
    Name(_ADR,0x0001FFFF)
  }

  Device(PRT2)
  {
    Name(_ADR,0x0002FFFF)
  }

  Device(PRT3)
  {
    Name(_ADR,0x0003FFFF)
  }

  Device(PRT4)
  {
    Name(_ADR,0x0004FFFF)
  }

  Device(PRT5)
  {
    Name(_ADR,0x0005FFFF)
  }
}
