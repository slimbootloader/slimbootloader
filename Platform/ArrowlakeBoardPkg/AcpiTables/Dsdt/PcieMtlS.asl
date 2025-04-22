/**@file
  PCI Express ACPI definitions

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External (VMDE)
External (VMSP)
External (PINI, MethodObj) // Platform specific PCIe root port initialization.
External (\STD3, FieldUnitObj) // 0: D3 disable. 1: D3Hot support for Storages. 2: D3Cold support for Storages.

Scope (\_SB.PC00) {

  //
  //

  //
  // PCIE Root Port D6F0 #1
  //
  Device (RP01) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA1, 0)) {
        Return (RPA1)
      } Else {
        Return (0x00060000)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 1)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR1, LTEN)
      Store (PML1, LMSL)
      Store (PNL1, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x01), 0))) {
          ADBG ("RP01 (0/6/0) is mapped under VMD")
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
  // PCIE Root Port D6F3 #9
  //
  Device (RP09) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA2, 0)) {
        Return (RPA2)
      } Else {
        Return (0x00060003)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 2)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR2, LTEN)
      Store (PML2, LMSL)
      Store (PNL2, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x08), 0))) {
          ADBG ("RP09 (0/6/3) is mapped under VMD")
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
  }                                                // end "PCIE Root Port #09"

  //
  // PCIE Root Port D6F4 #13
  //
  Device (RP13) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA3, 0)) {
        Return (RPA3)
      } Else {
        Return (0x00060004)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 3)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR3, LTEN)
      Store (PML3, LMSL)
      Store (PNL3, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x10), 0))) {
          ADBG ("RP13 (0/6/4) is mapped under VMD")
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP13
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
  }                                                // end "PCIE Root Port#13"

  //
  // PCIE Root Port D6F1 #17
  //
  Device (RP17) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA4, 0)) {
        Return (RPA4)
      } Else {
        Return (0x00060001)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 4)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR4, LTEN)
      Store (PML4, LMSL)
      Store (PNL4, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x02), 0))) {
          ADBG ("RP17 (0/6/1) is mapped under VMD")
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP17
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    #define PCIE_HP_STS GPE1_IOE_PCI_EXP
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
  }                                                // end "PCIE Root Port#17"

  //
  // PCIE Root Port D1F0 #21
  //
  Device (RP21) {
    Name (PBNU, 0)
    Method (_ADR, 0) {
      If (LNotEqual (RPA5, 0)) {
        Return (RPA5)
      } Else {
        Return (0x00010000)
      }
    }
    //
    // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 5)
    Name (PRTP, PCIE_RP_TYPE_CPU) // PCIE RP TYPE
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR5, LTEN)
      Store (PML5, LMSL)
      Store (PNL5, LNSL)
      If (LAnd (CondRefOf (VMSP),CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMSP, 0x20), 0))) {
          ADBG ("RP21 (0/1/0) is mapped under VMD")
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP21
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
  }                                                // end "PCIE Root Port#21"

}