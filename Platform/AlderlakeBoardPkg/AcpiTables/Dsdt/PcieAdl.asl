/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External(VMDE)
External(VMR1)
External(VMR2)
External(VMR3)
External(VMR4)
External(PINI, MethodObj) // Platform specific PCIe root port initialization.
External(\STD3, FieldUnitObj) // 1: D3Cold support for Storage 0: D3Hot support for Storage.

Scope(\_SB.PC00) {

  Name (SIPV, 0)

  //
  // Method returns SIP version
  //
  Method (GSIP, 0) {
    If (LEqual (PCHS, PCH_P)) {
      Return (14)
    } Else {
      Return (17)
    }
  }


  //
  // PCIE Root Port #01
  //
  Device (RP01) {
    Method (_ADR, 0) {
      If (LNotEqual (RPA1, 0)) {
        Return (RPA1)
      } Else {
        Return (0x001C0000)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 1)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR1, LTEN)
      Store (PML1, LMSL)
      Store (PNL1, LNSL)
      If (HBSL & 0x01) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x01), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP01
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA2, 0)) {
        Return (RPA2)
      } Else {
        Return (0x001C0001)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 2)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR2, LTEN)
      Store (PML2, LMSL)
      Store (PNL2, LNSL)
      If (HBSL & 0x01) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x02), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP02
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA3, 0)) {
        Return (RPA3)
      } Else {
        Return (0x001C0002)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 3)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR3, LTEN)
      Store (PML3, LMSL)
      Store (PNL3, LNSL)
      If (HBSL & 0x01) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x04), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP03
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA4, 0)) {
        Return (RPA4)
      } Else {
        Return (0x001C0003)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 4)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR4, LTEN)
      Store (PML4, LMSL)
      Store (PNL4, LNSL)
      If (HBSL & 0x01) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x08), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP04
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA5, 0)) {
        Return (RPA5)
      } Else {
        Return (0x001C0004)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 5)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR5, LTEN)
      Store (PML5, LMSL)
      Store (PNL5, LNSL)
      If (HBSL & 0x02) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x10), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP05
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA6, 0)) {
        Return (RPA6)
      } Else {
        Return (0x001C0005)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 6)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR6, LTEN)
      Store (PML6, LMSL)
      Store (PNL6, LNSL)
      If (HBSL & 0x02) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x20), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP06
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA7, 0)) {
        Return (RPA7)
      } Else {
        Return (0x001C0006)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 7)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR7, LTEN)
      Store (PML7, LMSL)
      Store (PNL7, LNSL)
      If (HBSL & 0x02) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x40), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP07
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA8, 0)) {
        Return (RPA8)
      } Else {
        Return (0x001C0007)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 8)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR8, LTEN)
      Store (PML8, LMSL)
      Store (PNL8, LNSL)
      If (HBSL & 0x02) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR1),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR1, 0x80), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP08
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPA9, 0)) {
        Return (RPA9)
      } Else {
        Return (0x001D0000)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 9)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTR9, LTEN)
      Store (PML9, LMSL)
      Store (PNL9, LNSL)
      If (HBSL & 0x04) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x01), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP09
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAA, 0)) {
        Return (RPAA)
      } Else {
        Return (0x001D0001)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 10)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRA, LTEN)
      Store (PMLA, LMSL)
      Store (PNLA, LNSL)
      If (HBSL & 0x04) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x02), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP10
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAB, 0)) {
        Return (RPAB)
      } Else {
        Return (0x001D0002)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 11)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRB, LTEN)
      Store (PMLB, LMSL)
      Store (PNLB, LNSL)
      If (HBSL & 0x04) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x04), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP11
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAC, 0)) {
        Return (RPAC)
      } Else {
        Return (0x001D0003)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 12)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRC, LTEN)
      Store (PMLC, LMSL)
      Store (PNLC, LNSL)
      If (HBSL & 0x04) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x08), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP12
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAD, 0)) {
        Return (RPAD)
      } Else {
        Return (0x001D0004)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 13)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRD, LTEN)
      Store (PMLD, LMSL)
      Store (PNLD, LNSL)
      If (HBSL & 0x08) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x10), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP13
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAE, 0)) {
        Return (RPAE)
      } Else {
        Return (0x001D0005)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 14)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRE, LTEN)
      Store (PMLE, LMSL)
      Store (PNLE, LNSL)
      If (HBSL & 0x08) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x20), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP14
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAF, 0)) {
        Return (RPAF)
      } Else {
        Return (0x001D0006)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 15)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRF, LTEN)
      Store (PMLF, LMSL)
      Store (PNLF, LNSL)
      If (HBSL & 0x08) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x40), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP15
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAG, 0)) {
        Return (RPAG)
      } Else {
        Return (0x001D0007)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 16)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRG, LTEN)
      Store (PMLG, LMSL)
      Store (PNLG, LNSL)
      If (HBSL & 0x08) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR2),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR2, 0x80), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP16
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAH, 0)) {
        Return (RPAH)
      } Else {
        Return (0x001B0000)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 17)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRH, LTEN)
      Store (PMLH, LMSL)
      Store (PNLH, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x01), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP17
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAI, 0)) {
        Return (RPAI)
      } Else {
        Return (0x001B0001)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 18)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRI, LTEN)
      Store (PMLI, LMSL)
      Store (PNLI, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x02), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP18
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAJ, 0)) {
        Return (RPAJ)
      } Else {
        Return (0x001B0002)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 19)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRJ, LTEN)
      Store (PMLJ, LMSL)
      Store (PNLJ, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x04), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP19
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAK, 0)) {
        Return (RPAK)
      } Else {
        Return (0x001B0003)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 20)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRK, LTEN)
      Store (PMLK, LMSL)
      Store (PNLK, LNSL)
      If (HBSL & 0x10) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x08), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP20
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAL, 0)) {
        Return (RPAL)
      } Else {
        Return (0x001B0004)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 21)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRL, LTEN)
      Store (PMLL, LMSL)
      Store (PNLL, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x10), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP21
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAM, 0)) {
        Return (RPAM)
      } Else {
        Return (0x001B0005)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 22)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRM, LTEN)
      Store (PMLM, LMSL)
      Store (PNLM, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x20), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP22
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAN, 0)) {
        Return (RPAN)
      } Else {
        Return (0x001B0006)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 23)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRN, LTEN)
      Store (PMLN, LMSL)
      Store (PNLN, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If(LAnd(CondRefOf(VMR3),CondRefOf(VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x40), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP23
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
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
    Method (_ADR, 0) {
      If (LNotEqual (RPAO, 0)) {
        Return (RPAO)
      } Else {
        Return (0x001B0007)
      }
    }
    //
    // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
    //
    Name (SLOT, 24)
    Name (LTEN, 0)
    Name (LMSL, 0)
    Name (LNSL, 0)
    Method (_INI)
    {
      Store (LTRO, LTEN)
      Store (PMLO, LMSL)
      Store (PNLO, LNSL)
      If (HBSL & 0x20) {
        Store (1, HBCS)
      }
      If (LAnd (CondRefOf (VMR3), CondRefOf (VMDE))) {
        If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR3, 0x80), 0))) {
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
      #define PCIE_ROOT_PORT  \_SB.PC00.RP24
      Include ("PcieStorage.asl")
      #undef PCIE_ROOT_PORT
    }
    Store (GSIP (), SIPV)
    Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
    Include ("PchPcieAtscCommon.asl")
#endif
    Method (_PRT, 0) {
      If (CondRefOf (PICM)) {
        If (PICM) {
          Return (AR07)
        }                                          // APIC mode
      }
      Return (PD07)                                // PIC Mode
    }                                              // end _PRT
  }                                                // end "PCIE Root Port#24"

  If (LEqual (PCHS, PCH_S)) {
    //
    // PCIE Root Port #25
    //
    Device (RP25) {
      Method (_ADR, 0) {
        If (LNotEqual (RPAP, 0)) {
          Return (RPAP)
        } Else {
          Return (0x001A0000)
        }
      }
      //
      // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
      //
      Name (SLOT, 25)
      Name (LTEN, 0)
      Name (LMSL, 0)
      Name (LNSL, 0)
      Method (_INI)
      {
        Store (LTRP, LTEN)
        Store (PMLP, LMSL)
        Store (PNLP, LNSL)
        If (HBSL & 0x40) {
          Store (1, HBCS)
        }
        If (LAnd (CondRefOf (VMR4), CondRefOf (VMDE))) {
          If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR4, 0x01), 0))) {
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
        #define PCIE_ROOT_PORT  \_SB.PC00.RP25
        Include ("PcieStorage.asl")
        #undef PCIE_ROOT_PORT
      }
      Store (GSIP (), SIPV)
      Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
      Include ("PchPcieAtscCommon.asl")
#endif
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
    // PCIE Root Port #26
    //
    Device (RP26) {
      Method (_ADR, 0) {
        If (LNotEqual (RPAQ, 0)) {
          Return (RPAQ)
        } Else {
          Return (0x001A0001)
        }
      }
      //
      // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
      //
      Name (SLOT, 26)
      Name (LTEN, 0)
      Name (LMSL, 0)
      Name (LNSL, 0)
      Method (_INI)
      {
        Store (LTRQ, LTEN)
        Store (PMLQ, LMSL)
        Store (PNLQ, LNSL)
        If (HBSL & 0x40) {
          Store (1, HBCS)
        }
        If (LAnd (CondRefOf (VMR4), CondRefOf (VMDE))) {
          If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR4, 0x02), 0))) {
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
        #define PCIE_ROOT_PORT  \_SB.PC00.RP26
        Include ("PcieStorage.asl")
        #undef PCIE_ROOT_PORT
      }
      Store (GSIP (), SIPV)
      Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
      Include ("PchPcieAtscCommon.asl")
#endif
      Method (_PRT, 0) {
        If (CondRefOf (PICM)) {
          If (PICM) {
            Return (AR05)
          }                                          // APIC mode
        }
        Return (PD05)                                // PIC Mode
      }                                              // end _PRT
    }                                                // end "PCIE Root Port#26"

    //
    // PCIE Root Port #27
    //
    Device (RP27) {
      Method (_ADR, 0) {
        If (LNotEqual (RPAR, 0)) {
          Return (RPAR)
        } Else {
          Return (0x001A0002)
        }
      }
      //
      // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
      //
      Name (SLOT, 27)
      Name (LTEN, 0)
      Name (LMSL, 0)
      Name (LNSL, 0)
      Method (_INI)
      {
        Store (LTRR, LTEN)
        Store (PMLR, LMSL)
        Store (PNLR, LNSL)
        If (HBSL & 0x40) {
          Store (1, HBCS)
        }
        If (LAnd (CondRefOf (VMR4), CondRefOf (VMDE))) {
          If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR4, 0x04), 0))) {
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
        #define PCIE_ROOT_PORT  \_SB.PC00.RP27
        Include ("PcieStorage.asl")
        #undef PCIE_ROOT_PORT
      }
      Store (GSIP (), SIPV)
      Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
      Include ("PchPcieAtscCommon.asl")
#endif
      Method (_PRT, 0) {
        If (CondRefOf (PICM)) {
          If (PICM) {
            Return (AR06)
          }                                          // APIC mode
        }
        Return (PD06)                                // PIC Mode
      }                                              // end _PRT
    }                                                // end "PCIE Root Port#27"

    //
    // PCIE Root Port #28
    //
    Device (RP28) {
      Method (_ADR, 0) {
        If (LNotEqual (RPAS, 0)) {
          Return (RPAS)
        } Else {
          Return (0x001A0003)
        }
      }
      //
      // Pass LTRx to LTEN so PchPcieCommon.asl can be reused for PCIes.
      //
      Name (SLOT, 28)
      Name (LTEN, 0)
      Name (LMSL, 0)
      Name (LNSL, 0)
      Method (_INI)
      {
        Store (LTRS, LTEN)
        Store (PMLS, LMSL)
        Store (PNLS, LNSL)
        If (HBSL & 0x40) {
          Store (1, HBCS)
        }
        If (LAnd (CondRefOf (VMR4), CondRefOf (VMDE))) {
          If (LAnd (LEqual (VMDE, 1), LNotEqual (And (VMR4, 0x08), 0))) {
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
        #define PCIE_ROOT_PORT  \_SB.PC00.RP28
        Include ("PcieStorage.asl")
        #undef PCIE_ROOT_PORT
      }
      Store (GSIP (), SIPV)
      Include ("PchPcieCommon.asl")
#if FeaturePcdGet (PcdTccEnabled) == 1
      Include ("PchPcieAtscCommon.asl")
#endif
      Method (_PRT, 0) {
        If (CondRefOf (PICM)) {
          If (PICM) {
            Return (AR07)
          }                                          // APIC mode
        }
        Return (PD07)                                // PIC Mode
      }                                              // end _PRT
    }                                                // end "PCIE Root Port#28"
  }
}