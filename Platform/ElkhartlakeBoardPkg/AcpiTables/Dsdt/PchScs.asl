/**@file
  ACPI DSDT table for SCS Controllers

 Copyright (c) 2014 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Include ("AcpiResourcesOffsets.h")
#include <Register/ScsRegs.h>
#include <IndustryStandard/Pci.h>

External(ULTR, MethodObj)

Scope(\_SB.PC00) {

  //
  // SCS UFS0 (PCI Mode)
  //
  Device(PUF0) {
    Name(_ADR, 0x00120005)
    Name (_DDN, "Intel(R) UFS Controller 0")

    // Memory Region to access to the UFS PCI Configuration Space
    OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
    Field(SCSR, DWordAcc, NoLock, Preserve) {
      Offset(R_SCS_CFG_PCS),         // 0x84, PMCSR - Power Management Control and Status
      PSTA,32,
      Offset(R_SCS_CFG_PG_CONFIG),   // 0xA2, Device PG config
          , 2,
      PGEN, 1         // [BIT2] PGE - PG Enable
    }
    Method (_STA, 0, Serialized) {
      If (LEqual (\UF0E, 0)) {
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

    Method(_PS0, 0, Serialized) {
      Store(0, PGEN) // Disable PG
      And(PSTA, 0xFFFFFFFC, PSTA) // Set BIT[1:0] = 00b - Power State D0
      Store(PSTA, TEMP) // Read Back PMCSR
      //
      // Remove UFS LTR disqualification
      // Clear IGN_UFS2x bit of PMC GCR space has register LTR_IGN
      //
      ULTR (0)
      PCRA (PID_UFSX2, R_SCS_PCR_4020, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_4820, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_5C20, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_5820, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_1078, 0x0)
      //
      // Request UFS0 MODPHY PG Exit
      //
      If (LEqual (PCHS, PCHL)) {
        \_SB.CSD0 (MODPHY_SPD_GATING_UFS0)
      }
    }

    Method(_PS3, 0, Serialized) {
      //
      // Add UFS LTR disqualification
      // Set IGN_UFS2x bit of PMC GCR space has register LTR_IGN
      //
      ULTR (1)
      //
      // Request UFS0 MODPHY PG Entry
      //
      If (LEqual (PCHS, PCHL)) {
        \_SB.CSD3 (MODPHY_SPD_GATING_UFS0)
      }
      Store(1, PGEN) // Enable PG
    }

    Device (CARD) {
      Name (_ADR, 0x00000008) // Slot 0, Function 8
      Method(_RMV, 0x0, NotSerialized) { Return (0) } // Device cannot be removed
    }
  }

  //
  // SCS UFS1 (PCI Mode)
  //
  Device(PUF1) {
    Name(_ADR, 0x00120007)
    Name (_DDN, "Intel(R) UFS Controller 1")

    // Memory Region to access to the UFS PCI Configuration Space
    OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
    Field(SCSR, DWordAcc, NoLock, Preserve) {
      Offset(R_SCS_CFG_PCS),         // 0x84, PMCSR - Power Management Control and Status
      PSTA,32,
      Offset(R_SCS_CFG_PG_CONFIG),   // 0xA2, Device PG config
          , 2,
      PGEN, 1         // [BIT2] PGE - PG Enable
    }
    Method (_STA, 0, Serialized) {
      If (LEqual (\UF1E, 0)) {
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

    Method(_PS0, 0, Serialized) {
      Store(0, PGEN) // Disable PG
      And(PSTA, 0xFFFFFFFC, PSTA) // Set BIT[1:0] = 00b - Power State D0
      Store(PSTA, TEMP) // Read Back PMCSR
      //
      // Remove UFS LTR disqualification
      // Clear IGN_UFS2x bit of PMC GCR space has register LTR_IGN
      //
      ULTR (0)
      PCRA (PID_UFSX2, R_SCS_PCR_4020, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_4820, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_5C20, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_5820, 0x0)
      PCRA (PID_UFSX2, R_SCS_PCR_1078, 0x0)
      //
      // Request UFS1 MODPHY PG Exit
      //
      If (LEqual (PCHS, PCHL)) {
        \_SB.CSD0 (MODPHY_SPD_GATING_UFS1)
      }
    }

    Method(_PS3, 0, Serialized) {
      //
      // Add UFS LTR disqualification
      // Set IGN_UFS2x bit of PMC GCR space has register LTR_IGN
      //
      ULTR (1)
      //
      // Request UFS1 MODPHY PG Entry
      //
      If (LEqual (PCHS, PCHL)) {
        \_SB.CSD3 (MODPHY_SPD_GATING_UFS1)
      }
      Store(1, PGEN) // Enable PG
    }

    Device (CARD) {
      Name (_ADR, 0x00000008) // Slot 0, Function 8
      Method(_RMV, 0x0, NotSerialized) { Return (0) } // Device cannot be removed
    }
  }

  //
  // SCS eMMC (PCI Mode)
  //
  Device(PEMC) {
    Name(_ADR, 0x001A0000)

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
// @todo: revise for CNL PMC

      PCAO (PID_EMMC, R_SCS_PCR_1C20, 0xFFF8FFFF, 0x00000000)
      PCAO (PID_EMMC, R_SCS_PCR_4820, 0xFFFFF8FF, 0x00000000)
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

  //
  // SCS SD Card (PCI Mode)
  //
  Device(PSDC) {
    Name(_ADR, 0x001A0001)

    Method (_STA) {
      If (LEqual (\SDCE, 0)) {
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

    Method (_CRS, 0, Serialized) {
      //Wake from SD card events while host controller in D3
      Name (SBFI, ResourceTemplate () {
          GpioInt (Edge, ActiveBoth, SharedAndWake, PullNone, 10000, "\\_SB.GPI0", 0x00, ResourceConsumer, INTG) {0} // SD CARD DETECT GpioInt
          GpioIo (Shared, PullDefault, 0, 0, IoRestrictionInputOnly, "\\_SB.GPI0",,, GPIO) {0} // SD CARD DETECT GpioIo
      })
      If(LEqual(\NGPS,1)) {
        CreateWordField (SBFI, Add(INTG, ACPI_GPIO_CONN_DESC_OFFSET_NAME), INOF)
        CreateField (SBFI, Multiply (Add (INOF, INTG), 8), 72, IPCS)
        CreateWordField (SBFI, Add(GPIO, ACPI_GPIO_CONN_DESC_OFFSET_NAME), GPOF)
        CreateField (SBFI, Multiply (Add (GPOF, GPIO), 8), 72, GPCS)
      }
      CreateWordField (SBFI, INTG._PIN, SDIP)
      CreateWordField (SBFI, GPIO._PIN, SDGP)
        Store (GNUM(GPIO_VER3_VGPIO39), SDIP)
      If(LEqual(\NGPS,1)) {
        Concatenate ("\\_SB.GPI", ToDecimalString(GCOM(GPIO_VER3_VGPIO39)), IPCS)
      }
      Store (GNUM(GPIO_VER3_VGPIO39), SDGP)
      If(LEqual(\NGPS,1)) {
        Concatenate ("\\_SB.GPI", ToDecimalString(GCOM(GPIO_VER3_VGPIO39)), GPCS)
      }
      Return (SBFI)
    }

    // Memory Region to access to the SDHC PCI Configuration Space
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
// @todo: revise for CNL PMC

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

      // Check the UUID
      If(LEqual(Arg0, ToUUID("f6c13ea5-65cd-461f-ab7a-29f7e8d5bd61"))) {
        // Check the revision
        If(LGreaterEqual(Arg1, Zero)) {
          // Switch statement based on the function index.
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
            // For SD we have to support functions to
            // set 1.8V signalling and 3.3V signalling [BIT4, BIT3]
            //
            Case(0) {
              Return (Buffer() {0x19})
            }

            //
            // Function Index 3: Set 1.8v signalling.
            // We put a sleep of 100ms in this method to
            // work around a known issue with detecting
            // UHS SD card on PCH. This is to compensate
            // for the SD VR slowness.
            //
            Case(3) {
              Sleep (100) // Sleep 100ms
              Return(Buffer(){0x00})
            }

            //
            // Function Index 4: Set 3.3v signalling.
            // We put a sleep of 100ms in this method to
            // work around a known issue with detecting
            // UHS SD card on PCH. This is to compensate
            // for the SD VR slowness.
            //
            Case(4) {
              Sleep (100) // Sleep 100ms
              Return(Buffer(){0x00})
            }
          } // End - Switch(Arg2)
        }
      } // End - UUID check
      Return(Buffer() {0x0})
    } // End _DSM
  } // Device(PSDC)
}
