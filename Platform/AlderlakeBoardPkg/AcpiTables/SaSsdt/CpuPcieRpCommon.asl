/** @file
  This file contains the PCIe Root Port Common configuration

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  External(ECR1)
  External(GPRW, MethodObj)
  External(PICM)
  External(\_SB.PC00.PC2M, MethodObj)
  External(PPBA, MethodObj) // PCIe power budget allocation
  External(UPRD, MethodObj) // PCIe update PERST# assertion delay
  External(ATSC)
  External(PPS0, MethodObj) // Platform specific PCIe root port _PS0 Hook Function.
  External(PPS3, MethodObj) // Platform specific PCIe root port _PS3 Hook Function.

  Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE

  OperationRegion(PXCS,SystemMemory,\_SB.PC00.PC2M(_ADR ()),0x480)
  Field(PXCS,AnyAcc, NoLock, Preserve)
  {
    Offset(0),
    VDID, 32,
    Offset(0x50),                      // LCTL - Link Control Register
    L0SE, 1,                           // 0, L0s Entry Enabled
    , 3,
    LDIS, 1,
    , 3,
    Offset(0x52),                      // LSTS - Link Status Register
    , 13,
    LASX, 1,                           // 0, Link Active Status
    Offset(0x5A),                      // SLSTS[7:0] - Slot Status Register
    ABPX, 1,                           // 0, Attention Button Pressed
    , 2,
    PDCX, 1,                           // 3, Presence Detect Changed
    , 2,
    PDSX, 1,                           // 6, Presence Detect State
    , 1,
    Offset(0x60),                      // RSTS - Root Status Register
    , 16,
    PSPX, 1,                           // 16,  PME Status
    Offset(0xA4),                      // PMCSR
    D3HT, 2,                           // PowerState
    Offset(0xD8), // 0xD8, MPC - Miscellaneous Port Configuration Register
    , 30,
    HPEX, 1,                           // 30,  Hot Plug SCI Enable
    PMEX, 1,                           // 31,  Power Management SCI Enable
    Offset(0xE0), // 0xE0, SPR - Scratch Pad Register
    SCB0, 1,                           // Scratchpad register SPR[0] (SCB)
    , 6,
    NCB7, 1,                           // Non-Sticky Scratch Pad Bit (NSCB)[7]
    Offset(0xE2), // 0xE2, RPPGEN - Root Port Power Gating Enable
    , 2,
    L23E, 1,                           // 2,   L23_Rdy Entry Request (L23ER)
    L23R, 1,                           // 3,   L23_Rdy to Detect Transition (L23R2DT)
    Offset(0x324), // 0x324 - PCIEDBG
    , 3,
    LEDM, 1,       // PCIEDBG.DMIL1EDM
    Offset(0x328),                     // 0x328, PCI Express Status 1
    , 24,
    LTSM, 8,
  }
  Field(PXCS,AnyAcc, NoLock, WriteAsZeros)
  {
    Offset(0xDC), // 0xDC, SMSCS - SMI/SCI Status Register
    , 30,
    HPSX, 1,                           // 30,  Hot Plug SCI Status
    PMSX, 1                            // 31,  Power Management SCI Status
  }

  //
  // L23D method recovers link from L2 or L3 state. Used for RTD3 flows, right after endpoint is powered up and exits reset.
  // This flow is implemented in ASL because rootport registers used for L2/L3 entry/exit
  // are proprietary and OS drivers don't know about them.
  //
  Method (L23D, 0, Serialized) {
    If(LNotEqual(SCB0,0x1)) {
      Return()
    }

    /// Set L23_Rdy to Detect Transition  (L23R2DT)
    Store(1, L23R)
    Store(0, Local0)
    /// Wait for transition to Detect
    While(L23R) {
      If(Lgreater(Local0, 4))
      {
        Break
      }
      Sleep(16)
      Increment(Local0)
    }
    Store(0,SCB0)

    /// Once in Detect, wait up to 124 ms for Link Active (typically happens in under 70ms)
    /// Worst case per PCIe spec from Detect to Link Active is:
    /// 24ms in Detect (12+12), 72ms in Polling (24+48), 28ms in Config (24+2+2+2+2)
    Store(0, Local0)
    While(LEqual(LASX,0)) {
      If(Lgreater(Local0, 8))
      {
        Break
      }
      Sleep(16)
      Increment(Local0)
    }
  }

  //
  // DL23 method puts link to L2 or L3 state. Used for RTD3 flows, before endpoint is powered down.
  // This flow is implemented in ASL because rootport registers used for L2/L3 entry/exit
  // are proprietary and OS drivers don't know about them.
  //
  Method (DL23, 0, Serialized) {
    Store(1, L23E)
    Sleep(16)
    Store(0, Local0)
    While(L23E) {
      If(Lgreater(Local0, 4))
      {
        Break
      }
      Sleep(16)
      Increment(Local0)
    }
    Store(1,SCB0)
  }

  Name(LTRV, Package(){0,0,0,0})
  Name(PRMV, 0) // PCIE Rp Mapped under VMD
  Name(RD3C, 1) // 1: D3Cold support for Storage connected to PCIE port, 0: D3Hot support for Storage connected to PCIE port. By default this is set as 1 for other EndPoints

  //
  // Check if root port is present.
  // @return 0: root port is disabled, 1: root port is enabled
  //
  Method(PRES) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return(0)
    } Else {
      Return(1)
    }
  }

  //
  // _DSM Device Specific Method
  //
  // Arg0: UUID Unique function identifier
  // Arg1: Integer Revision Level
  // Arg2: Integer Function Index (0 = Return Supported Functions)
  // Arg3: Package Parameters
  Method(_DSM, 4, Serialized) {
    //
    // Switch based on which unique function identifier was passed in
    //
    If (LEqual(Arg0, ToUUID ("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))) {
      //
      // _DSM Definitions for Latency Tolerance Reporting
      //
      // Arguments:
      // Arg0: UUID: E5C937D0-3553-4d7a-9117-EA4D19C3434D
      // Arg1: Revision ID: 3
      // Arg2: Function Index: 0, 6, 8, 9
      // Arg3: Empty Package
      //
      // Switch by function index
      //
      Switch(ToInteger(Arg2)) {
        //
        // Function Index:0
        // Standard query - A bitmask of functions supported
        //
        Case (0) {
          Name(OPTS,Buffer(2){0,0})
          CreateBitField(OPTS,0,FUN0)
          CreateBitField(OPTS,6,FUN6)
          CreateBitField(OPTS,8,FUN8)
          CreateBitField(OPTS,9,FUN9)
          CreateBitField(OPTS,10,FUNA)
          CreateBitField(OPTS,11,FUNB)

          if (LGreaterEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
            Store(1,FUN0)
            if (LNotEqual (LTEN, 0)) {
              Store(1,FUN6)
            }

            If(CondRefOf(ECR1)) {
              if(LEqual(ECR1,1)){
                if (LGreaterEqual(Arg1, 3)){ // test Arg1 for Revision ID: 3
                  Store(1,FUN8)
                  Store(1,FUN9)
                }
              }
            }
          }

          If (LGreaterEqual(Arg1, 4)) { //test Arg1 for Revision ID: 4
            If(CondRefOf(PPBA)) {
              Store(1,FUNA)
            }
            If(CondRefOf(UPRD)) {
              Store(1,FUNB)
            }
          }
          Return (OPTS)
        }

        //
        // Function Index: 6
        // LTR Extended Capability Structure
        //
        Case(6) {
          if (LGreaterEqual(Arg1, 2)){ // test Arg1 for Revision ID: 2
            Store(And(ShiftRight(LMSL,10),7), Index(LTRV, 0))
            Store(And(LMSL,0x3FF), Index(LTRV, 1))
            Store(And(ShiftRight(LNSL,10),7), Index(LTRV, 2))
            Store(And(LNSL,0x3FF), Index(LTRV, 3))
            Return (LTRV)
          }
        }
        Case(8) { //ECR ACPI additions for FW latency optimizations, DSM for Avoiding Power-On Reset Delay Duplication on Sx Resume
          If(CondRefOf(ECR1)) {
            if(LEqual(ECR1,1)){
              if (LGreaterEqual(Arg1, 3)) { // test Arg1 for Revision ID: 3
                return (1)
              }
            }
          }
        }
        Case(9) { //ECR ACPI additions for FW latency optimizations, DSM for Specifying Device Readiness Durations
          If(CondRefOf(ECR1)) {
            if(LEqual(ECR1,1)){
              if (LGreaterEqual(Arg1, 3)) { // test Arg1 for Revision ID: 3
                return(Package(5){50000,Ones,Ones,50000,Ones})
              }
            }
          }
        }
        //
        //  Function index 10 - negotiate device auxilary power consumption.
        //
        Case(10) {
          If(CondRefOf(PPBA)) {
            Return(PPBA(Arg3))
          }
        }
        //
        // Function index 11 update delay between PME_TO_Ack and PERST# assertion
        //
        Case(11) {
          If(CondRefOf(UPRD)) {
            Return(UPRD(Arg3))
          }
        }
      } // End of switch(Arg2)
    } // End of if
    return (Buffer() {0x00})
  } // End of _DSM

  Method(_PRW, 0) {
    Return(GPRW(0x69, 4)) // can wakeup from S4 state
  }

  Method (_PS0, 0, Serialized)
  {
    If (LEqual (HPEX, 1)) {
      Store (0, HPEX) // Disable Hot Plug SCI
      Store (1, HPSX) // Clear Hot Plug SCI status
    }
    If (LEqual (PMEX, 1)) {
      Store (0, PMEX) // Disable Power Management SCI
      Store (1, PMSX) // Clear Power Management SCI status
    }
    If (CondRefOf (PPS0)) {
      PPS0 ()
    }
  }
  Method (_PS3, 0, Serialized)
  {
    If (CondRefOf (PPS3)) {
      PPS3 ()
    }
    If (LEqual (HPEX, 0)) {
      Store(1, HPEX) // Enable Hot Plug SCI
      Store(1, HPSX) // Clear Hot Plug SCI status
    }
    If (LEqual(PMEX, 0)) {
      Store(1, PMEX) // Enable Power Management SCI
      Store(1, PMSX) // Clear Power Management SCI status
    }
  }

  Method (_DSD, 0) {
    If(CondRefOf(PINI)) {
      Return (
        Package () {
          ToUUID ("6211E2C0-58A3-4AF3-90E1-927A4E0C55A4"),
          Package () {
            Package (2) {"HotPlugSupportInD3", 1},
          },
          ToUUID("FDF06FAD-F744-4451-BB64-ECD792215B10"),
          Package () {
            Package (2) {"FundamentalDeviceResetTriggeredOnD3ToD0", 1},
          },
          // This _DSD object informs Windows PCIe bus driver that
          // this root ports supports auxilary power budgeting.
          ToUUID("6B4AD420-8FD3-4364-ACF8-EB94876FD9EB"),
          Package () {
          },
          // This informs driver that PTM ART is traceble to TSC
          ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
          Package () {
          }
        }
      ) //End of Return ()
    } else {
      Return (
        Package () {
          ToUUID ("6211E2C0-58A3-4AF3-90E1-927A4E0C55A4"),
          Package () {
            Package (2) {"HotPlugSupportInD3", 1},
          },
          ToUUID("FDF06FAD-F744-4451-BB64-ECD792215B10"),
          Package () {
            Package (2) {"FundamentalDeviceResetTriggeredOnD3ToD0", 1},
          },
          // This informs driver that PTM ART is traceble to TSC
          ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
          Package () {
          }
        }
      ) // End of Return ()
    }
  }

  //
  // PCI_EXP_STS Handler for PCIE Root Port
  //
  Method(HPME,0,Serialized) {
    If(LAnd(LNotEqual(VDID,0xFFFFFFFF), LEqual(PMSX,1))) { //if port exists and has PME SCI Status set...
      Store(1,PMSX) // clear rootport's PME SCI status
      Store(1,PSPX) // consume one pending PME status to prevent it from blocking the queue
      Return(0x01)
    }
    Return(0x00)
  }

  //
  // Sub-Method of _L61 Hot-Plug event
  // _L61 event handler should invoke this method to support HotPlug wake event from PEG RP
  //
  Method(HPEV,0,Serialized) {
    If(LAnd(LNotEqual(VDID,0xFFFFFFFF), HPSX)) {
      // Clear HotPlug SCI event status
      Store(1, HPSX)

      If(LEqual(PDCX, 1)) {
        // Clear Presence Detect Changed
        Store(1,PDCX)

        If(LEqual(PDSX, 0)) {
          // The PCI Express slot is empty, so disable L0s on hot unplug
          //
          Store(0,L0SE)
        }
        // Perform proper notification
        // to the OS.
        Notify(^,0)
      }
    }
  }
