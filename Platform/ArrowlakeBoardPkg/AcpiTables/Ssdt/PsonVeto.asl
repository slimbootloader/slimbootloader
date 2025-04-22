/** @file
  Global power removal veto tracker

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// Global mask to track PS_ON consent from IPs.
// Bit:  0 - SATA, 1 - PCIe Aux Power, 2 - TCSS PD, 3 - Windows Standby, 4 - PCIe CEM D3cold
Name(GPOM, 0x8) // Stanby bit value -by default veto

//
// Global power removal veto tracker. Gathers information from all IPs about power
// removal consent.
// Arg0 - Interface index, 0 - SATA, 1 - PCIe Aux Power, 2 - TCSS PD, 3 - Windows Standby, 4 - PCIe CEM D3cold
// Arg1 - 0: Allow power removal, 1: veto power removal
//
Method(GPRV, 2, Serialized) {
  ShiftLeft(0x1, Arg0, Local0)
  If(Arg1) {
    Or(\GPOM, Local0, \GPOM)
  } Else {
    And(\GPOM, Not(Local0), \GPOM)
  }
  ADBG(Concatenate("GPOM=0x", ToHexString(\GPOM)))
  If(LEqual(\GPOM, 0)) {
    \PSOC(1)
  } Else {
    \PSOC(0)
  }
}

Name(PCPB, 0x0) // PCIe core power veto bitmask, default - allow for core power removal

//
// CEM PCIE slots core power removal permission
// aggregation.
// Arg0 - PCIe index in the aggregation bitmask
//           Indexing:
//           0 -  PCH slot1
//           1 -  PCH slot2
//           2 -  PCH slot3
//           3 -  PEG slot1
//           4 -  PEG slot2
// Arg1 - 0: agree to core power down, 1: veto core power down
Method(PCPA, 2, Serialized) {
  ShiftLeft(0x1, Arg0, Local0)
  If(Arg1) {
    Or(\PCPB, Local0, \PCPB)
  } Else {
    And(\PCPB, Not(Local0), \PCPB)
  }
  ADBG(Concatenate("PWRD_CONS=", ToHexString(PCPB)))
  If(LEqual(PCPB, 0)) {
    \GPRV(1, 0)
  } Else {
    \GPRV(1, 1)
  }
}

Name(RPSM, 0x0) // PCIe CEM Slot D3cold aggregation status bitmask, default - All enter D3cold

//
// CEM PCIE slots D3cold permission for PS_ON
// Arg0 - CEM PCIe slot index in the D3cold aggregation status bitmask
//           Indexing:
//           0 -  PCH slot1
//           1 -  PCH slot2
//           2 -  PCH slot3
//           3 -  PEG slot1
//           4 -  PEG slot2
// Arg1 - 0: enter D3cold, 1: leave D3cold
Method(RPSC, 2, Serialized) {
  ADBG (Concatenate("RPSM=0x", ToHexString(\RPSM)))
  ShiftLeft(0x1, Arg0, Local0)
  If(Arg1) {
    Or(\RPSM, Local0, \RPSM)
  } Else {
    And(\RPSM, Not(Local0), \RPSM)
  }
  ADBG (Concatenate("RPSM=0x", ToHexString(\RPSM)))
  If(LEqual(\RPSM, 0)) {
    ADBG ("PSON CEM D3cold OK")
    \GPRV(4, 0)
  } Else {
     ADBG ("PSON CEM D3cold Block")
    \GPRV(4, 1)
  }
}

Name(SATM, 0) // SATA ports disable bitmask. Each bit represents SATA port.

//
// Update SATA bitmask that allow for PS_ON entry
// Arg0 - SATA port index
// Arg1 - 0: SATA disable, 1: SATA enable
//
Method(STMC, 2, Serialized) {
  //
  // This is just to clear the junk values on SATM variables since it might be modified
  // unexpectedly by RST driver via _DSM method.
  //
  ADBG(Concatenate("STMC: SPPR=", ToHexString(SPPR)))
  ADBG(Concatenate("STMC: Arg0=", ToHexString(Arg0)))
  ADBG(Concatenate("STMC: Arg1=", ToHexString(Arg1)))
  And(\SATM, SPPR, Local0)
  Store (Local0, SATM)
  ShiftLeft(0x1, Arg0, Local0)
  If(Arg1) {
    Or(\SATM, Local0, \SATM)
  } Else {
    And(\SATM, Not(Local0), \SATM)
  }
  ADBG(Concatenate("SATA_MASK=", ToHexString(\SATM)))
  If(LEqual(\SATM, 0)) {
    \GPRV(0,0)
  } Else {
    \GPRV(0,1)
  }
}

//
// Get the SATA port status in the mask
// Arg0 - SATA port index
// @return 0: SATA disabled, 1: SATA enabled
//
Method(STMS, 1, Serialized) {
  ShiftLeft(0x1, Arg0, Local0)
  And(Local0, \SATM, Local1)
  If(Local1) {
    Return(1)
  } Else {
    Return(0)
  }
}

//
// PCIe power budgeting support.
//
External(\AUPL)        // Auxilary power limit
Name(AURB, 0xFFFFFFFF) // Auxilary power remaining budget. Initialized on demand to \AUPL value by budgeting method.

//
// Reserve auxilary power budget.
// Arg0 - 0: release, 1: reserve
// Arg1 - power in mW
//
// @retval 0 - Request denied, remaining power not updated
// @retval 1 - Request accepted, remaining power updated
//
Method(RAPC, 2, Serialized) {
  If(LEqual(\AURB, 0xFFFFFFFF)) {
    Store(\AUPL, \AURB)
  }
  ADBG(Concatenate("BUD=", ToDecimalString(\AURB)))
  ADBG(Concatenate("REQ=", ToDecimalString(Arg1)))
  If(Arg0) {
    If(LGreaterEqual(\AURB, Arg1)) {
      Subtract(\AURB, Arg1, \AURB)
      ADBG("Reserve accepted")
      Return(1)
    } Else {
      ADBG("Reserve denied")
      Return(0)
    }
  } Else {
    Add(Arg1, \AURB, \AURB)
    ADBG("Release accepted")
    Return(1)
  }
} //End of RAPC
