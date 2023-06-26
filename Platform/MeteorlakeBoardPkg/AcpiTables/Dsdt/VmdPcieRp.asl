/**@file
  VMD remapped storage description for PCIe SSD remapped under VMD controller

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


Name (ID3C, 0) // Is device in D3Cold.

External (CBAR, MethodObj)
External (\_SB.PC00.VMD0.VDON, MethodObj)
External (\_SB.PC00.VMD0.VDOF, MethodObj)
External (D3CV, MethodObj)
External (RPB, IntObj)
External (RPD, IntObj)
External (RPF, IntObj)
External (RSPT, IntObj)
External (RSPI, IntObj)
External (_ADR, MethodObj)
//
// Method to find whether PCIe port is part of logical volume
//
Method (VR3D) {
  Return (VD3C (_ADR ()))
}

//
// Method to turn ON remapped PCIe RP.
//
Method (VPON, 0, Serialized) {
  Name(DCFB, 0)
  Store(CBAR(RPB, RPD, RPF), DCFB)
  OperationRegion(DCFG, SystemMemory, DCFB, 0xC80)
  Field(DCFG, DWordAcc, NoLock, Preserve) {
    Offset(0),
    VDID, 32,
  }

  // Check Vendor-Device ID for remapped devices under VMD
  If(CondRefOf(VDID)) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Notify (^, 0)
      Store(200, Local1)   // 2sec delay
      While (LGreater(Local1, 0)) {
        Store(Subtract(Local1, 1), Local1)
        If (LNotEqual(VDID, 0xFFFFFFFF)) {
          break
        }
        Sleep(10)
      }
    }
  }

  // If device was not set in D3 cold do not re-initiate.
  If (LEqual (ID3C ,0)) {
    Return ()
  }

  // Turn on power to the remapped PCIe slot under VMD
  \_SB.PC00.VMD0.VDON (RSPT, RSPI)

  // Transition into Link Active
  RLA ()

  //
  // Sleep for 100ms after transition to link active
  //
  Sleep (100)
  Store (0, ID3C)
}


//
// Method to turn OFF remapped PCIe RP.
//
Method (VPOF, 0, Serialized) {
  // Check if root port supports D3Cold
  // Return if there is no support
  // to avoid L2/L3 ready transition
  If (LNot (D3CV (RSPT, RSPI))) {
    Return ()
  }

  // Transition into L23 Ready
  RL23 ()

  // Turn off power to the remapped PCIe slot
  \_SB.PC00.VMD0.VDOF (RSPT, RSPI)

  Store (1, ID3C)
}

//
// RL23 method puts link to L2 or L3 state. Used for RTD3 flows, before endpoint is powered down.
//
Method (RL23, 0, Serialized) {
  Name (DCFB, 0)
  Store (CBAR (0, RPD, RPF), DCFB)
  OperationRegion (DCFG, SystemMemory, DCFB, 0xC80)
  Field (DCFG, DWordAcc, NoLock, Preserve) {
    Offset (0xE0),    // 0xE0, SPR - Scratch Pad Register
    , 0,
    SCB0, 1,                        // Non-Sticky Scratch Pad Bit (NSCB)[0]
    Offset (R_PCH_PCIE_CFG_RPPGEN), // 0xE2, RPPGEN - Root Port Power Gating Enable
    , 2,
    L23E, 1,      // 2,   L23_Rdy Entry Request (L23ER)
  }

  Store (1, L23E)
  Sleep (16)
  Store (0, Local0)
  While (L23E) {
    If (Lgreater (Local0, 4))
    {
      Break
    }
    Sleep (16)
    Increment (Local0)
  }
  Store (1,SCB0)
}

//
// RLA method recovers link from L2 or L3 state. Used for RTD3 flows, right after endpoint is powered up and exits reset.
//
Method (RLA, 0, Serialized) {
  Name (DCFB, 0)
  Store (CBAR (0, RPD, RPF), DCFB)
  OperationRegion (DCFG, SystemMemory, DCFB, 0xC80)
  Field (DCFG, DWordAcc, NoLock, Preserve) {
    Offset (0x52), // LSTS - Link Status Register
    , 13,
    LASX, 1,      // 0, Link Active Status
    Offset (0xE0),    // 0xE0, SPR - Scratch Pad Register
    , 0,
    SCB0, 1,                   // Non-Sticky Scratch Pad Bit (NSCB)[0]
    Offset (R_PCH_PCIE_CFG_RPPGEN), // 0xE2, RPPGEN - Root Port Power Gating Enable
    , 3,
    L23R, 1,       // 3,   L23_Rdy to Detect Transition (L23R2DT)
    Offset (R_PCH_PCIE_CFG_PCIEPMECTL), // 0x420, PCIEPMECTL (PCIe PM Extension Control)
    , 30,
    DPGE, 1,       // PCIEPMECTL[30]: Disabled, Detect and L23_Rdy State PHY Lane Power Gating Enable (DLSULPPGE):
  }

  If (LNotEqual (SCB0,0x1)) {
    Return ()
  }

  Store (1, L23R)
  Store (0, Local0)
  /// Wait for transition to Detect
  While (L23R) {
    If (Lgreater (Local0, 4))
    {
      Break
    }
    Sleep (16)
    Increment (Local0)
  }
  Store (0,SCB0)

  /// Once in Detect, wait up to 124 ms for Link Active (typically happens in under 70ms)
  /// Worst case per PCIe spec from Detect to Link Active is:
  /// 24ms in Detect (12+12), 72ms in Polling (24+48), 28ms in Config (24+2+2+2+2)
  Store (0, Local0)
  While (LEqual (LASX,0)) {
    If (Lgreater (Local0, 8))
    {
      Break
    }
    Sleep (16)
    Increment (Local0)
  }
}
