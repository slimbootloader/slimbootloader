/**@file
  ACPI DSDT table for SCS UFS Controllers

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(UF0E)
External(UF1E)
External(ULTR, MethodObj)
External(PCRA, MethodObj)

#define PCI_UFS0_ADR                0x00120005
#define PCI_UFS1_ADR                0x00120007
#define R_SCS_CFG_PCS               0x84
#define R_SCS_CFG_PG_CONFIG         0xA2

#define PID_UFSX2                   0x50

#define R_SCS_PCR_1C20              0x1C20
#define R_SCS_PCR_4820              0x4820
#define R_SCS_PCR_4020              0x4020
#define R_SCS_PCR_5820              0x5820
#define R_SCS_PCR_5C20              0x5C20
#define R_SCS_PCR_1078              0x1078

Scope(\_SB.PC00) {
  Method (OCPD, 0, Serialized) {
    PCRA (PID_UFSX2, R_SCS_PCR_4020, 0x0)
    PCRA (PID_UFSX2, R_SCS_PCR_4820, 0x0)
    PCRA (PID_UFSX2, R_SCS_PCR_5C20, 0x0)
    PCRA (PID_UFSX2, R_SCS_PCR_5820, 0x0)
    PCRA (PID_UFSX2, R_SCS_PCR_1078, 0x0)
  }

  If (LEqual (UF0E, 1)) {
    //
    // SCS UFS0 (PCI Mode)
    //
    Device(PUF0) {
      Name(_ADR, PCI_UFS0_ADR)
      Name (_DDN, "Intel(R) UFS Controller 0")

      // Memory Region to access to the UFS PCI Configuration Space
      OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
      Field(SCSR, ByteAcc, NoLock, Preserve) {
        Offset(R_SCS_CFG_PCS),         // 0x84, PMCSR - Power Management Control and Status
        PSTA,32,
        Offset(R_SCS_CFG_PG_CONFIG),   // 0xA2, Device PG config
            , 2,
        PGEN, 1         // [BIT2] PGE - PG Enable
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
        //
        // Disable OCP Timer in SCS UFS IOSF Bridge
        //
        OCPD ()
      }

      Method(_PS3, 0, Serialized) {
        //
        // Add UFS LTR disqualification
        // Set IGN_UFS2x bit of PMC GCR space has register LTR_IGN
        //
        ULTR (1)
        Store(1, PGEN) // Enable PG
      }

      Device (CARD) {
        Name (_ADR, 0x00000008) // Slot 0, Function 8
        Method(_RMV, 0x0, NotSerialized) { Return (0) } // Device cannot be removed
      }
    }
  } //UF0E

  If (LEqual (\UF1E, 1)) {
    //
    // SCS UFS1 (PCI Mode)
    //
    Device(PUF1) {
      Name(_ADR, PCI_UFS1_ADR)
      Name (_DDN, "Intel(R) UFS Controller 1")

      // Memory Region to access to the UFS PCI Configuration Space
      OperationRegion(SCSR, PCI_Config, 0x00, 0x100)
      Field(SCSR, ByteAcc, NoLock, Preserve) {
        Offset(R_SCS_CFG_PCS),         // 0x84, PMCSR - Power Management Control and Status
        PSTA,32,
        Offset(R_SCS_CFG_PG_CONFIG),   // 0xA2, Device PG config
            , 2,
        PGEN, 1         // [BIT2] PGE - PG Enable
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
        //
        // Disable OCP Timer in SCS UFS IOSF Bridge
        //
        OCPD ()
      }

      Method(_PS3, 0, Serialized) {
        //
        // Add UFS LTR disqualification
        // Set IGN_UFS2x bit of PMC GCR space has register LTR_IGN
        //
        ULTR (1)

        Store(1, PGEN) // Enable PG
      }

      Device (CARD) {
        Name (_ADR, 0x00000008) // Slot 0, Function 8
        Method(_RMV, 0x0, NotSerialized) { Return (0) } // Device cannot be removed
      }
    }
  } //UF1E
}
