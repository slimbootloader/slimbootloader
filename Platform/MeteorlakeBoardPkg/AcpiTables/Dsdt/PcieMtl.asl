/**@file
  PCI Express ACPI definitions

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (VMDE)
External (VMSP) // Mask for SOC + IOE RPs mapped under VMD
External (PINI, MethodObj) // Platform specific PCIe root port initialization.
External (\STD3, FieldUnitObj) // 0: D3 disable. 1: D3Hot support for Storages. 2: D3Cold support for Storages.
External (DGBA)

Scope (\_SB.PC00) {
  //
  // PCIE Root Port #01
  //
  Device (RP01) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA1, 0)) {
        Return (RPA1)
      } Else {
        Return (0x001C0000)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 1)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR1, LTEN)
      Store (PML1, LMSL)
      Store (PNL1, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x100), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }
      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP01
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPA
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR04)
        }                                          // APIC mode
      }
      Return (PD04)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#01"

  //
  // PCIE Root Port #02
  //
  Device (RP02) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA2, 0)) {
        Return (RPA2)
      } Else {
        Return (0x001C0001)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 2)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR2, LTEN)
      Store (PML2, LMSL)
      Store (PNL2, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x200), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP02
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPA
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR05)
        }                                          // APIC mode
      }
      Return (PD05)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port #02"

  //
  // PCIE Root Port #03
  //
  Device (RP03) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA3, 0)) {
        Return (RPA3)
      } Else {
        Return (0x001C0002)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 3)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR3, LTEN)
      Store (PML3, LMSL)
      Store (PNL3, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x400), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP03
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPA
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR06)
        }                                          // APIC mode
      }
      Return (PD06)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#03"

  //
  // PCIE Root Port #04
  //
  Device (RP04) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA4, 0)) {
        Return (RPA4)
      } Else {
        Return (0x001C0003)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 4)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR4, LTEN)
      Store (PML4, LMSL)
      Store (PNL4, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x800), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP04
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPA
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR07)
        }                                          // APIC mode
      }
      Return (PD07)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#04"

  //
  // PCIE Root Port #05
  //
  Device (RP05) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA5, 0)) {
        Return (RPA5)
      } Else {
        Return (0x001C0004)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 5)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR5, LTEN)
      Store (PML5, LMSL)
      Store (PNL5, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x1000), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP05
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPB
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR04)
        }                                          // APIC mode
      }
      Return (PD04)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#05"

  //
  // PCIE Root Port #06
  //
  Device (RP06) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA6, 0)) {
        Return (RPA6)
      } Else {
        Return (0x001C0005)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 6)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR6, LTEN)
      Store (PML6, LMSL)
      Store (PNL6, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x2000), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP06
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPB
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR05)
        }                                          // APIC mode
      }
      Return (PD05)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#06"

  //
  // PCIE Root Port #07
  //
  Device (RP07) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA7, 0)) {
        Return (RPA7)
      } Else {
        Return (0x001C0006)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 7)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR7, LTEN)
      Store (PML7, LMSL)
      Store (PNL7, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x4000), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP07
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPB
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR06)
        }                                          // APIC mode
      }
      Return (PD06)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#07"

  //
  // PCIE Root Port #08
  //
  Device (RP08) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA8, 0)) {
        Return (RPA8)
      } Else {
        Return (0x001C0007)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 8)
    Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR8, LTEN)
      Store (PML8, LMSL)
      Store (PNL8, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x8000), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP08
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPB
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR07)
        }                                          // APIC mode
      }
      Return (PD07)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#08"

  //
  // PCIE Root Port #09
  //
  Device (RP09) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA9, 0)) {
        Return (RPA9)
      } Else {
        Return (0x00060000)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 9)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR9, LTEN)
      Store (PML9, LMSL)
      Store (PNL9, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x1), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP09
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_PCI_EXP_SPC
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR04)
        }                                          // APIC mode
      }
      Return (PD04)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#09"

  //
  // PCIE Root Port #10
  //
  Device (RP10) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPAA, 0)) {
        Return (RPAA)
      } Else {
        Return (0x00060001)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 10)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRA, LTEN)
      Store (PMLA, LMSL)
      Store (PNLA, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x2), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP10
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_IOE_PCI_EXP
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR04)
        }                                          // APIC mode
      }
      Return (PD04)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#10"

  //
  // PCIE Root Port #11
  //
  Device (RP11) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPAB, 0)) {
        Return (RPAB)
      } Else {
        Return (0x00060002)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 11)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRB, LTEN)
      Store (PMLB, LMSL)
      Store (PNLB, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x4), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP11
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_IOE_PCI_EXP
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR04)
        }                                          // APIC mode
      }
      Return (PD04)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#11"

  //
  // PCIE Root Port #12
  //
  Device (RP12) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPAC, 0)) {
        Return (RPAC)
      } Else {
        Return (0x00010000)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 12)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRC, LTEN)
      Store (PMLC, LMSL)
      Store (PNLC, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x20), 0))) {
          Store (1, PRMV)
        }
      }
      If (PRES ()) {
        If (CondRefOf (PINI)) {
          PINI ()
        }
      }

      //
      // RP D3Cold/D3hot support status for Storage devices
      //
      #define PCIE_ROOT_PORT  \_SB.PC00.RP12
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_IOE_PCI_EXP
    Include ("PcieCommon.asl")
    #undef PCIE_HP_STS
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR04)
        }                                          // APIC mode
      }
      Return (PD04)                                // PIC Mode
    }                                              // end _PRT
  }                                             // end "PCIE Root Port#12"
}
