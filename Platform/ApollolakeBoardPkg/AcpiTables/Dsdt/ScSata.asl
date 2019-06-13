/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope(\_SB.PCI0)
{
  //
  // Serial ATA Host Controller - Device 18, Function 0
  //
  Device(SATA) {
    Name(_ADR,0x00120000)

    Device(PRT0)
    {
      Name(_ADR,0x0000FFFF)  // Port 0
    }
    Device(PRT1)
    {
      Name(_ADR,0x0001FFFF)  // Port 1
    }
    //
    // SATA Methods pulled in via SSDT.
    //
    OperationRegion(SATR, PCI_Config, 0x74,0x4)
    Field(SATR,WordAcc,NoLock,Preserve) {
      Offset(0x00), // 0x74, PMCR
          ,   8,
      PMEE,   1,    //PME_EN
          ,   6,
      PMES,   1     //PME_STS
    }

    Method (_STA, 0x0, NotSerialized)
    {
      //Enable SATA controller PME_EN bit
      Store (1, \_SB.PCI0.SATA.PMEE)
      Return(0xf)
    }

    Method(_DSW, 3)
    {
      If(Arg1)
      { // Entering Sx, need to disable WAKE# from generating runtime PME
          Store(0, \_SB.PCI0.SATA.PMEE)
      } Else {  // Staying in S0
        If(LAnd(Arg0, Arg2)) // Exiting D0 and arming for wake
        { // Set PME
          Store(1, \_SB.PCI0.SATA.PMEE)
        } Else { // Disable runtime PME, either because staying in D0 or disabling wake
          Store(0, \_SB.PCI0.SATA.PMEE)
        }
      }
    } // End _DSW
  }
}


