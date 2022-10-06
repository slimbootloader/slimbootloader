/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Register/PchRegsScs.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsScsSd.h>

Include ("PchPciAcpiDefines.asi")
Scope(\_SB.PC00) {

  //
  // SCS eMMC (PCI Mode)
  //
  Device(PEMC) {
    Name(_ADR, SCS_EMMC_PCI_ADDR)

    Method (_STA) {
      If (LEqual (\EMCE, 0)) {
        //
        // This will prevent OSPM from running
        // methods like _PSx for this device.
        //
        Return (0)
      } Else {
        //
        // Report as fully functional and to be shown in UI.
        //
        Return (0xF)
      }
    }

    // Memory Region to access to the eMMC PCI Configuration Space
    OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
    Field(SCSR, WordAcc, NoLock, Preserve) {
      Offset(R_SCS_CFG_PCS),         // 0x84, PMCSR - Power Management Control and Status
      PSTA,32,
      Offset(R_SCS_CFG_PG_CONFIG),   // 0xA2, Device PG config
          , 2,
      PGEN, 1         // [BIT2] PGE - PG Enable
    }

    Method(_PS0, 0, Serialized) {
      Store(0, PGEN) // Disable PG

      //
      // On some platforms it has been observed that
      // transactions to SD and eMMC are failing due to
      // the OCP timeout being too low. This timeout doesn't
      // generate any interrupt so software can't react to this failure.
      // We disable the OCP timeout on D0 entry to avoid dropping transactions
      // to SD and eMMC. This write has to be done on D0 entry since power gating in D3
      // will reset it to default state.
      //
      PCRA (PID_EMMC, R_SCS_PCR_1C20, 0x0)
      PCRA (PID_EMMC, R_SCS_PCR_4820, 0x0)

      And(PSTA, 0xFFFFFFFC, PSTA) // Set BIT[1:0] = 00b - Power State D0
      Store(PSTA, TEMP) // Read Back PMCSR
    }

    Method(_PS3, 0, Serialized) {
      Store(1, PGEN) // Enable PG

      Or(PSTA, 0x3, PSTA) // Set BIT[1:0] = 11b - Power State D3
      Store(PSTA, TEMP) // Read Back PMCSR
    }

    // _DSM x86 Device Specific Method
    // Arg0: UUID Unique function identifier
    // Arg1: Integer Revision Level
    // Arg2: Integer Function Index (0 = Return Supported Functions)
    // Arg3: Package Parameters
    Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      If(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }
      ADBG(Concatenate("EMH4=",ToDecimalString(EMH4)))
      // Check the UUID
      If(LEqual(Arg0, ToUUID("f6c13ea5-65cd-461f-ab7a-29f7e8d5bd61"))) {
        // Check the revision
        If(LGreaterEqual(Arg1, Zero)) {
          //Switch statement based on the function index.
          Switch(ToInteger(Arg2)) {
            //
            // Function Index 0 the return value is a buffer containing
            // one bit for each function index, starting with zero.
            // Bit 0 - Indicates whether there is support for any functions other than function 0.
            // Bit 1 - Indicates support to clear power control register
            // Bit 2 - Indicates support to set power control register
            // Bit 3 - Indicates support to set 1.8V signalling
            // Bit 4 - Indicates support to set 3.3V signalling
            // Bit 5 - Indicates support for HS200 mode
            // Bit 6 - Indicates support for HS400 mode
            // Bit 9 - Indicates eMMC I/O Driver Strength
            //
            // For eMMC we have to support functions for
            // HS200 and HS400 and I/O Driver Strength [BIT9, BIT6, BIT5]
            //
            Case(0) {
              //
              // The return value 0x261 (BIT6 set) for HS400 enabled and
              // 0x221 (BIT6 not set) when HS400 is disabled in Setup menu.
              //
              If(LEqual(EMH4, 1)) {
                Return(Buffer() {0x61, 0x02}) // HS400 support enabled
              }
              Return(Buffer() {0x21, 0x02})   // HS400 support disabled
            }

            //
            // Function index 5 - corresponds to HS200 mode
            // Return value from this function is used to program
            // the UHS Mode Select bits in Host Control 2.
            // 011b - corresponds to SDR104 and according to the
            // SD Host Controller Spec and this value is overloaded
            // to program the controller to select HS200 mode for eMMC.
            //
            Case(5) {
              Return(Buffer() {0x3})
            }

            //
            // Function index 6 - corresponds to HS400 mode
            // Return value from this function is used to program
            // the UHS Mode Select bits in Host Control 2.
            // 101b is a reserved value according to the SD Host
            // Controller Spec and we use this value for HS400 mode
            // selection.
            //
            Case(6) {
              Return(Buffer() {0x5})
            }

            //
            // Function index 9 - corresponds to I/O Driver Strength
            // Return value from this function represents the values
            // of the Driver Strength selection
            // (eMMC 5.01 Specification JESD84-B50.1, Table 186)
            // that shall be programmed by the Host Driver (OS)
            // as part of the Initialization flows.
            //
            Case(9) {
              Switch(ToInteger(EMDS)) {
                Case(0x0) { Return(Buffer() {0x0}) } // 50 Ohm
                Case(0x1) { Return(Buffer() {0x1}) } // 33 Ohm
                Case(0x4) { Return(Buffer() {0x4}) } // 40 Ohm
              }
            }
          } // End - Switch(Arg2)
        }
      } // End - UUID check
      Return(Buffer() {0x0})
    } // End - _DSM

    Device (CARD) {
      Name (_ADR, 0x00000008)
      Method(_RMV, 0x0, NotSerialized) { Return (0) } // Device cannot be removed
    }
  } // Device(PEMC)
}

