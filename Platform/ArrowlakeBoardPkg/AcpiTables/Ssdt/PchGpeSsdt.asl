/**@file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Include/AcpiDebug.h>

DefinitionBlock (
  "PchGpe.aml",
  "SSDT",
  0x02,
  "PchGpe",
  "PchGpe ",
  0x3000
  )
{
  #include "Register/PmcRegs.h"

  Store ("[PchGpe PchGpe SSDT][AcpiTableEntry]", Debug)
  Store (Timer, Debug)
  ACPI_DEBUG_EXTERNAL_REFERENCE
  ADBG ("[PchGpe PchGpe SSDT][AcpiTableEntry]")

  External (\_SB.PC02, DeviceObj)
  //External (PGRT, FieldUnitObj)

  External (\_SB.PC02.RP01, DeviceObj)
  External (\_SB.PC02.RP02, DeviceObj)
  External (\_SB.PC02.RP03, DeviceObj)
  External (\_SB.PC02.RP04, DeviceObj)
  External (\_SB.PC02.RP05, DeviceObj)
  External (\_SB.PC02.RP06, DeviceObj)
  External (\_SB.PC02.RP07, DeviceObj)
  External (\_SB.PC02.RP08, DeviceObj)
  External (\_SB.PC02.RP09, DeviceObj)
  External (\_SB.PC02.RP10, DeviceObj)
  External (\_SB.PC02.RP11, DeviceObj)
  External (\_SB.PC02.RP12, DeviceObj)
  External (\_SB.PC02.RP13, DeviceObj)
  External (\_SB.PC02.RP14, DeviceObj)
  External (\_SB.PC02.RP15, DeviceObj)
  External (\_SB.PC02.RP16, DeviceObj)
  External (\_SB.PC02.RP17, DeviceObj)
  External (\_SB.PC02.RP18, DeviceObj)
  External (\_SB.PC02.RP19, DeviceObj)
  External (\_SB.PC02.RP20, DeviceObj)
  External (\_SB.PC02.RP21, DeviceObj)
  External (\_SB.PC02.RP22, DeviceObj)
  External (\_SB.PC02.RP23, DeviceObj)
  External (\_SB.PC02.RP24, DeviceObj)

  External (\_SB.PC02.RP01.HPME, MethodObj)
  External (\_SB.PC02.RP02.HPME, MethodObj)
  External (\_SB.PC02.RP03.HPME, MethodObj)
  External (\_SB.PC02.RP04.HPME, MethodObj)
  External (\_SB.PC02.RP05.HPME, MethodObj)
  External (\_SB.PC02.RP06.HPME, MethodObj)
  External (\_SB.PC02.RP07.HPME, MethodObj)
  External (\_SB.PC02.RP08.HPME, MethodObj)
  External (\_SB.PC02.RP09.HPME, MethodObj)
  External (\_SB.PC02.RP10.HPME, MethodObj)
  External (\_SB.PC02.RP11.HPME, MethodObj)
  External (\_SB.PC02.RP12.HPME, MethodObj)
  External (\_SB.PC02.RP13.HPME, MethodObj)
  External (\_SB.PC02.RP14.HPME, MethodObj)
  External (\_SB.PC02.RP15.HPME, MethodObj)
  External (\_SB.PC02.RP16.HPME, MethodObj)
  External (\_SB.PC02.RP17.HPME, MethodObj)
  External (\_SB.PC02.RP18.HPME, MethodObj)
  External (\_SB.PC02.RP19.HPME, MethodObj)
  External (\_SB.PC02.RP20.HPME, MethodObj)
  External (\_SB.PC02.RP21.HPME, MethodObj)
  External (\_SB.PC02.RP22.HPME, MethodObj)
  External (\_SB.PC02.RP23.HPME, MethodObj)
  External (\_SB.PC02.RP24.HPME, MethodObj)

  External (\_SB.PC02.ISHD, DeviceObj)
  External (\_SB.PC02.CNVW, DeviceObj)
  External (\_SB.PC02.GLAN, DeviceObj)
  External (\_SB.PC02.SAT0, DeviceObj)
  External (\_SB.PC02.XHCI, DeviceObj)
  External (\_SB.PC02.HDAS, DeviceObj)
  External (\_SB.PC02.XDCI, DeviceObj)
  External (\_SB.PC02.I3C0, DeviceObj)

  External (\_SB.PC02.ABAS) // PCH PMC ACPI base

  External (\_GPE.T1PL, MethodObj)

// Needed ?
  External (\PCHS, FieldUnitObj)
  External (\PCHX, IntObj)

  Scope (\_SB) {
    //
    // PCH reserved resources for MTL-S PCH
    //
    Device (PRR2) {
      Name (ABAS, 0)
      Store (\_SB.PC02.ABAS, ABAS)
      Name (_HID,EISAID ("PNP0C02")) // motherboard resource
      Name (_UID,"PCHRES2")
      Name (_STA,0x3) // device present and decodes its resources, but not to be displayed in OSPM

      Method (_CRS,0,Serialized)
      {
        Name (BUF0,ResourceTemplate () {
          // ACPI Base
          IO (Decode16,0,0,0x1,0xFF,ABA2)
        })
        // Update ACPI IO region
        CreateWordField (BUF0, ABA2._MIN,BMIN)
        CreateWordField (BUF0, ABA2._MAX,BMAX)
        Store (ABAS, BMIN)
        Store (ABAS, BMAX)
        return (BUF0)
      } // Method (_CRS)

      OperationRegion (PMC2, SystemIO, ABAS, 0xFF)
      Field (PMC2, DWordAcc,Lock,Preserve) {
        Offset (R_ACPI_IO_GPE1_STS_31_0),
        S210, 32,   // Tier 2 - GPE1_STS bits 0_31
        S211, 32,   // Tier 2 - GPE1_STS bits 32_63
        S212, 32,   // Tier 2 - GPE1_STS bits 64_95
        E210, 32,   // Tier 2 - GPE1_EN  bits 0_31
        E211, 32,   // Tier 2 - GPE1_EN  bits 32_63
        E212, 32,   // Tier 2 - GPE1_EN  bits 64_95
        Offset (R_ACPI_IO_GPE0_STS_31_0),
        S100, 32,   // Tier 1 - GPE0_STS bits 0_31
        S101, 32,   // Tier 1 - GPE0_STS bits 32_63
        S102, 32,   // Tier 1 - GPE0_STS bits 64_95
        S200, 32,   // Tier 2 - GPE0_STS bits 96_127
        E100, 32,   // Tier 1 - GPE0_EN  bits 0_31
        E101, 32,   // Tier 1 - GPE0_EN  bits 32_63
        E102, 32,   // Tier 1 - GPE0_EN  bits 64_95
        E200, 32,   // Tier 2 - GPE0_EN  bits 96_127
      }
    } // Device (PRR2)
  } // End  Scope (\_SB)

  Scope (\_GPE) {

    //
    // Handler for Tier 2 - GPE1_STS bits 0_31
    //
    Method (H210) {
      Store (\_SB.PRR2.S210, Local0)

      If (And (B_ACPI_IO_GPE1_STS_31_0_CNVI_PME_B0_STS, Local0)) {
        ADBG ("CNVI clear")
        If (CondRefOf (\_SB.PC02.CNVW)) {
          Notify (\_SB.PC02.CNVW, 0x02)
        }
        Store (B_ACPI_IO_GPE1_STS_31_0_CNVI_PME_B0_STS, \_SB.PRR2.S210)
      }

      If (And (B_ACPI_IO_GPE1_STS_31_0_GBE_PME_B0_STS, Local0)) {
        ADBG ("GBE clear")
        If (CondRefOf (\_SB.PC02.GLAN)) {
          Notify (\_SB.PC02.GLAN, 0x02)
        }
        Store (B_ACPI_IO_GPE1_STS_31_0_GBE_PME_B0_STS, \_SB.PRR2.S210)
      }

      If (And (B_ACPI_IO_GPE1_STS_31_0_SATA_PME_B0_STS, Local0)) {
        ADBG ("SATA clear")
        If (CondRefOf (\_SB.PC02.SAT0)) {
          Notify (\_SB.PC02.SAT0, 0x02)
        }
        Store (B_ACPI_IO_GPE1_STS_31_0_SATA_PME_B0_STS, \_SB.PRR2.S210)
      }

      If (And (B_ACPI_IO_GPE1_STS_31_0_XHCI_PME_B0_STS, Local0)) {
        ADBG ("XHCI clear")
        If (CondRefOf (\_SB.PC02.XHCI)) {
          Notify (\_SB.PC02.XHCI, 0x02)
        }
        Store (B_ACPI_IO_GPE1_STS_31_0_XHCI_PME_B0_STS, \_SB.PRR2.S210)
      }

      If (And (B_ACPI_IO_GPE1_STS_31_0_ACE_PME_B0_STS, Local0)) {
        ADBG ("ACE clear")
        If (CondRefOf (\_SB.PC02.HDAS)) {
          Notify (\_SB.PC02.HDAS, 0x02)
        }
        Store (B_ACPI_IO_GPE1_STS_31_0_ACE_PME_B0_STS, \_SB.PRR2.S210)
      }

      If (And (B_ACPI_IO_GPE1_STS_31_0_XDCI_PME_B0_STS, Local0)) {
        ADBG ("XDCI clear")
        If (CondRefOf (\_SB.PC02.XDCI)) {
          Notify (\_SB.PC02.XDCI, 0x02)
        }
        Store (B_ACPI_IO_GPE1_STS_31_0_XDCI_PME_B0_STS, \_SB.PRR2.S210)
      }

      If (And (B_ACPI_IO_GPE1_STS_31_0_LPSS_PME_B0_STS, Local0)) {
        ADBG ("LPSS clear")
        If (CondRefOf (\_SB.PC02.I3C0)) {
          Notify (\_SB.PC02.I3C0, 0x02)
        }
        Store (B_ACPI_IO_GPE1_STS_31_0_LPSS_PME_B0_STS, \_SB.PRR2.S210)
      }
    }

    //
    // Hot Plug Event for SPA
    //
    Method (SPAH) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC02
      #define ROOT_PORT_OBJECT  RP01
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP02
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP03
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP04
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    //
    // Hot Plug Event for SPB
    //
    Method (SPBH) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC02
      #define ROOT_PORT_OBJECT  RP05
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP06
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP07
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP08
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    //
    // Hot Plug Event for SPC
    //
    Method (SPCH) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC02
      #define ROOT_PORT_OBJECT  RP09
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP10
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP11
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP12
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    //
    // Hot Plug Event for SPD
    //
    Method (SPDH) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC02
      #define ROOT_PORT_OBJECT  RP13
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP14
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP15
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP16
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    //
    // Hot Plug Event for SPE
    //
    Method (SPEH) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC02
      #define ROOT_PORT_OBJECT  RP17
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP18
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP19
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP20
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    //
    // Hot Plug Event for SPF
    //
    Method (SPFH) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC02
      #define ROOT_PORT_OBJECT  RP21
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP22
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP23
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP24
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    //
    // Handler for Tier 2 - GPE1_STS bits 32_63
    //
    Method (H211) {
      Store (\_SB.PRR2.S211, Local0)

      If (And (B_ACPI_IO_GPE1_STS_63_32_SPA_HOT_PLUG_STS, Local0)) {
        ADBG ("SPA HOT_PLUG clear")
        SPAH ()
        Store (B_ACPI_IO_GPE1_STS_63_32_SPA_HOT_PLUG_STS, \_SB.PRR2.S211)
      }

      If (And (B_ACPI_IO_GPE1_STS_63_32_SPB_HOT_PLUG_STS, Local0)) {
        ADBG ("SPB HOT_PLUG clear")
        SPBH ()
        Store (B_ACPI_IO_GPE1_STS_63_32_SPB_HOT_PLUG_STS, \_SB.PRR2.S211)
      }

      If (And (B_ACPI_IO_GPE1_STS_63_32_SPC_HOT_PLUG_STS, Local0)) {
        Store ("SPC HOT_PLUG clear", Debug)
        SPCH ()
        Store (B_ACPI_IO_GPE1_STS_63_32_SPC_HOT_PLUG_STS, \_SB.PRR2.S211)
      }

      If (And (B_ACPI_IO_GPE1_STS_63_32_SPD_HOT_PLUG_STS, Local0)) {
        ADBG ("SPD HOT_PLUG clear")
        SPDH ()
        Store (B_ACPI_IO_GPE1_STS_63_32_SPD_HOT_PLUG_STS, \_SB.PRR2.S211)
      }

      If (And (B_ACPI_IO_GPE1_STS_63_32_SPE_HOT_PLUG_STS, Local0)) {
        ADBG ("SPE HOT_PLUG clear")
        SPEH ()
        Store (B_ACPI_IO_GPE1_STS_63_32_SPE_HOT_PLUG_STS, \_SB.PRR2.S211)
      }

      If (And (B_ACPI_IO_GPE1_STS_63_32_SPF_HOT_PLUG_STS, Local0)) {
        ADBG ("SPF HOT_PLUG clear")
        SPFH ()
        Store (B_ACPI_IO_GPE1_STS_63_32_SPF_HOT_PLUG_STS, \_SB.PRR2.S211)
      }
    }

    //
    // Handler for Tier 2 - GPE1_STS bits 64_95
    //
    Method (H212) {
      Store (\_SB.PRR2.S212, Local0)

      If (And (B_ACPI_IO_GPE1_STS_95_64_SPA_PCI_EXP_STS, Local0)) {
        ADBG ("SPA PCI_EXP clear")
        \_SB.PC02.RP01.HPME ()
        \_SB.PC02.RP02.HPME ()
        \_SB.PC02.RP03.HPME ()
        \_SB.PC02.RP04.HPME ()
        Store (B_ACPI_IO_GPE1_STS_95_64_SPA_PCI_EXP_STS, \_SB.PRR2.S212)
      }

      If (And (B_ACPI_IO_GPE1_STS_95_64_SPB_PCI_EXP_STS, Local0)) {
        ADBG ("SPB PCI_EXP clear")
        \_SB.PC02.RP05.HPME ()
        \_SB.PC02.RP06.HPME ()
        \_SB.PC02.RP07.HPME ()
        \_SB.PC02.RP08.HPME ()
        Store (B_ACPI_IO_GPE1_STS_95_64_SPB_PCI_EXP_STS, \_SB.PRR2.S212)
      }

      If (And (B_ACPI_IO_GPE1_STS_95_64_SPC_PCI_EXP_STS, Local0)) {
        ADBG ("SPC PCI_EXP clear")
        \_SB.PC02.RP09.HPME ()
        \_SB.PC02.RP10.HPME ()
        \_SB.PC02.RP11.HPME ()
        \_SB.PC02.RP12.HPME ()
        Store (B_ACPI_IO_GPE1_STS_95_64_SPC_PCI_EXP_STS, \_SB.PRR2.S212)
      }

      If (And (B_ACPI_IO_GPE1_STS_95_64_SPD_PCI_EXP_STS, Local0)) {
        ADBG ("SPD PCI_EXP clear")
        \_SB.PC02.RP13.HPME ()
        \_SB.PC02.RP14.HPME ()
        \_SB.PC02.RP15.HPME ()
        \_SB.PC02.RP16.HPME ()
        Store (B_ACPI_IO_GPE1_STS_95_64_SPD_PCI_EXP_STS, \_SB.PRR2.S212)
      }

      If (And (B_ACPI_IO_GPE1_STS_95_64_SPE_PCI_EXP_STS, Local0)) {
        ADBG ("SPE PCI_EXP clear")
        \_SB.PC02.RP17.HPME ()
        \_SB.PC02.RP18.HPME ()
        \_SB.PC02.RP19.HPME ()
        \_SB.PC02.RP20.HPME ()
        Store (B_ACPI_IO_GPE1_STS_95_64_SPE_PCI_EXP_STS, \_SB.PRR2.S212)
      }

      If (And (B_ACPI_IO_GPE1_STS_95_64_SPF_PCI_EXP_STS, Local0)) {
        ADBG ("SPF PCI_EXP clear")
        \_SB.PC02.RP21.HPME ()
        \_SB.PC02.RP22.HPME ()
        \_SB.PC02.RP23.HPME ()
        \_SB.PC02.RP24.HPME ()
        Store (B_ACPI_IO_GPE1_STS_95_64_SPF_PCI_EXP_STS, \_SB.PRR2.S212)
      }
    }

    Method (PL6B) {
      ADBG ("PCH L6B Start")

      If (LGreater (\_SB.PRR2.S210, 0)) {
        H210 ()
      }
      If (LGreater (\_SB.PRR2.S211, 0)) {
        H211 ()
      }
      If (LGreater (\_SB.PRR2.S212, 0)) {
        H212 ()
      }

      // Clear aggregated evenets at the end of handling
      Store (B_ACPI_IO_GPE0_STS_127_96_PME_B0_STS, \_SB.PRR2.S200)
      Store (B_ACPI_IO_GPE0_STS_127_96_PME_STS, \_SB.PRR2.S200)
      Store (B_ACPI_IO_GPE0_STS_127_96_WADT_STS, \_SB.PRR2.S200)
    }
    //
    // _L6E - SOC-S GPE Aggregated Handler
    //
    Method (PL6E) {
      ADBG ("PCH L6E Start")

      // Clear aggregated evenets at the end of handling
      Store (B_ACPI_IO_GPE0_STS_127_96_ESPI_SCI_STS, \_SB.PRR2.S200)
    }
  } // End Scope (\_GPE)

  ADBG ("[PchGpe PchGpe SSDT][AcpiTableExit]")
  Store ("[PchGpe PchGpe SSDT][AcpiTableExit]", Debug)
  Store (Timer, Debug)
} //End DefinitionBlock
