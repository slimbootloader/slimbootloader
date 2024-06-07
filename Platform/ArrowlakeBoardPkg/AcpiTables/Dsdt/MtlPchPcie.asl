/**@file
  PCI Express ACPI definitions for MTL PCH

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

External(PINI, MethodObj) // Platform specific PCIe root port initialization.

External (VMDE)
External (VMPP)
External (PBSB)

//
// PCIE Root Port #01
//
Device (RP01) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
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

    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x01), 0))) {
        ADBG ("PCH RP01 (129/28/0) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP01
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
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

    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x02), 0))) {
        ADBG ("PCH RP02 (129/28/1) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
        //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP02
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
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
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x04), 0))) {
        ADBG ("PCH RP03 (129/28/2) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
        //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP03
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
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
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x08), 0))) {
        ADBG ("PCH RP04 (129/28/3) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP04
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
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
    Store (PNL4, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x10), 0))) {
        ADBG ("PCH RP05 (129/28/4) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP05
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
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
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x20), 0))) {
        ADBG ("PCH RP06 (129/28/5) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP06
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
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
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x40), 0))) {
        ADBG ("PCH RP07 (129/28/6) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP07
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
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
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x80), 0))) {
        ADBG ("PCH RP08 (129/28/7) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP08
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPA9, 0)) {
      Return (RPA9)
    } Else {
      Return (0x001D0000)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 9)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTR9, LTEN)
    Store (PML9, LMSL)
    Store (PNL9, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x100), 0))) {
        ADBG ("PCH RP09 (129/29/0) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP09
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAA, 0)) {
      Return (RPAA)
    } Else {
      Return (0x001D0001)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 10)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRA, LTEN)
    Store (PMLA, LMSL)
    Store (PNLA, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x200), 0))) {
        ADBG ("PCH RP10 (129/29/1) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP10
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#10"

//
// PCIE Root Port #11
//
Device (RP11) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAB, 0)) {
      Return (RPAB)
    } Else {
      Return (0x001D0002)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 11)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRB, LTEN)
    Store (PMLB, LMSL)
    Store (PNLB, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x400), 0))) {
        ADBG ("PCH RP11 (129/29/2) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP11
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#11"

//
// PCIE Root Port #12
//
Device (RP12) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAC, 0)) {
      Return (RPAC)
    } Else {
      Return (0x001D0003)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 12)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRC, LTEN)
    Store (PMLC, LMSL)
    Store (PNLC, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x800), 0))) {
        ADBG ("PCH RP12 (129/29/3) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP12
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#12"

//
// PCIE Root Port #13
//
Device (RP13) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAD, 0)) {
      Return (RPAD)
    } Else {
      Return (0x001D0004)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 13)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRD, LTEN)
    Store (PMLD, LMSL)
    Store (PNLD, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x1000), 0))) {
        ADBG ("PCH RP13 (129/29/4) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP13
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#13"

//
// PCIE Root Port #14
//
Device (RP14) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAE, 0)) {
      Return (RPAE)
    } Else {
      Return (0x001D0005)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 14)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRE, LTEN)
    Store (PMLE, LMSL)
    Store (PNLE, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x2000), 0))) {
        ADBG ("PCH RP14 (129/29/5) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP14
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#14"

//
// PCIE Root Port #15
//
Device (RP15) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAF, 0)) {
      Return (RPAF)
    } Else {
      Return (0x001D0006)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 15)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRF, LTEN)
    Store (PMLF, LMSL)
    Store (PNLF, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x4000), 0))) {
        ADBG ("PCH RP15 (129/29/6) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP15
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#15"

//
// PCIE Root Port #16
//
Device (RP16) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAG, 0)) {
      Return (RPAG)
    } Else {
      Return (0x001D0007)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 16)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRG, LTEN)
    Store (PMLG, LMSL)
    Store (PNLG, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x8000), 0))) {
        ADBG ("PCH RP16 (129/29/7) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP16
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#16"

//
// PCIE Root Port #17
//
Device (RP17) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAH, 0)) {
      Return (RPAH)
    } Else {
      Return (0x001B0000)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 17)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRH, LTEN)
    Store (PMLH, LMSL)
    Store (PNLH, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x10000), 0))) {
        ADBG ("PCH RP17 (129/27/0) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP17
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#17"

//
// PCIE Root Port #18
//
Device (RP18) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAI, 0)) {
      Return (RPAI)
    } Else {
      Return (0x001B0001)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 18)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRI, LTEN)
    Store (PMLI, LMSL)
    Store (PNLI, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x20000), 0))) {
        ADBG ("PCH RP18 (129/27/1) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP18
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#18"

//
// PCIE Root Port #19
//
Device (RP19) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAJ, 0)) {
      Return (RPAJ)
    } Else {
      Return (0x001B0002)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 19)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRJ, LTEN)
    Store (PMLJ, LMSL)
    Store (PNLJ, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x40000), 0))) {
        ADBG ("PCH RP19 (129/27/2) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP19
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#19"

//
// PCIE Root Port #20
//
Device (RP20) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAK, 0)) {
      Return (RPAK)
    } Else {
      Return (0x001B0003)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 20)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRK, LTEN)
    Store (PMLK, LMSL)
    Store (PNLK, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x80000), 0))) {
        ADBG ("RP20 (129/29/3) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP20
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#20"

//
// PCIE Root Port #21
//
Device (RP21) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAL, 0)) {
      Return (RPAL)
    } Else {
      Return (0x001B0004)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 21)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRL, LTEN)
    Store (PMLL, LMSL)
    Store (PNLL, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x100000), 0))) {
        ADBG ("RP21 (129/27/4) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP21
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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

//
// PCIE Root Port #22
//
Device (RP22) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAM, 0)) {
      Return (RPAM)
    } Else {
      Return (0x001B0005)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 22)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRM, LTEN)
    Store (PMLM, LMSL)
    Store (PNLM, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x200000), 0))) {
        ADBG ("PCH RP22 (129/27/5) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP22
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#22"

//
// PCIE Root Port #23
//
Device (RP23) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAN, 0)) {
      Return (RPAN)
    } Else {
      Return (0x001B0006)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 23)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRN, LTEN)
    Store (PMLN, LMSL)
    Store (PNLN, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x400000), 0))) {
        ADBG ("PCH RP23 (129/27/6) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP23
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#23"

//
// PCIE Root Port #24
//
Device (RP24) {
  Name (PBNU, 0)
  Store (PBSB, PBNU)
  Method (_ADR, 0) {
    If (LNotEqual (RPAO, 0)) {
      Return (RPAO)
    } Else {
      Return (0x001B0007)
    }
  }
  //
  // Pass LTRx to LTEN so PcieCommon.asl can be reused for PCIes.
  //
  Name (SLOT, 24)
  Name (PRTP, PCIE_RP_TYPE_PCH) // PCIE RP TYPE
  Name (LTEN, 0)
  Name (LMSL, 0)
  Name (LNSL, 0)
  Method (_INI)
  {
    Store (LTRO, LTEN)
    Store (PMLO, LMSL)
    Store (PNLO, LNSL)
    If (LAnd (CondRefOf (VMPP),CondRefOf (VMDE))) {
      If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMPP, 0x800000), 0))) {
        ADBG ("RP24 (129/27/7) is mapped under VMD")
        Store (1, PRMV)
      }
    }
    If (PRES()) {
      If(CondRefOf(PINI)) {
        PINI()
      }
    }
    //
    // RP D3Cold/D3hot support status for Storage devices
    //
    #define PCIE_ROOT_PORT  \_SB.PC02.RP24
    Include ("PcieStorage.asl")
    #undef PCIE_ROOT_PORT
  }
  #define PCIE_HP_STS GPE_PME
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
}                                                // end "PCIE Root Port#24"
