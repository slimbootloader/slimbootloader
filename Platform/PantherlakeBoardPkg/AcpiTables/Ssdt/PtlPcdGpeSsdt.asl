/**@file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#include "Register/PmcRegs.h"

DefinitionBlock (
  "SocGpe.aml",
  "SSDT",
  0x02,
  "SocGpe",
  "SocGpe ",
  0x3000
  )
{

  External (\_SB.PC00, DeviceObj)
  External (\_SB.PC00.MC, DeviceObj)
  External (\_SB.PC00.MC.D1F0, FieldUnitObj)
  External (\_SB.PC00.MC.D1F1, FieldUnitObj)
  External (\_SB.PC00.MC.D6F0, FieldUnitObj)
  External (\_GPE.P0L6, MethodObj)
  External (\_GPE.P1L6, MethodObj)
  External (\_GPE.P2L6, MethodObj)
  External (\_GPE.P3L6, MethodObj)
  External (PGRT, FieldUnitObj)
  External (\_SB.PC00.TRP0, DeviceObj)
  External (\_SB.PC00.TRP1, DeviceObj)
  External (\_SB.PC00.TRP2, DeviceObj)
  External (\_SB.PC00.TRP3, DeviceObj)
  External (\_SB.PC00.TRP0.HPME, MethodObj)
  External (\_SB.PC00.TRP1.HPME, MethodObj)
  External (\_SB.PC00.TRP2.HPME, MethodObj)
  External (\_SB.PC00.TRP3.HPME, MethodObj)
  External (\_SB.PC00.TRP0.HPEV, MethodObj)
  External (\_SB.PC00.TRP1.HPEV, MethodObj)
  External (\_SB.PC00.TRP2.HPEV, MethodObj)
  External (\_SB.PC00.TRP3.HPEV, MethodObj)
  External (\_SB.PC00.TRP0.LTRC, MethodObj)
  External (\_SB.PC00.TRP1.LTRC, MethodObj)
  External (\_SB.PC00.TRP2.LTRC, MethodObj)
  External (\_SB.PC00.TRP3.LTRC, MethodObj)
  External (\_SB.PC00.TRP0.PID3, MethodObj)
  External (\_SB.PC00.TRP1.PID3, MethodObj)
  External (\_SB.PC00.TRP2.PID3, MethodObj)
  External (\_SB.PC00.TRP3.PID3, MethodObj)
  External (\_SB.PC00.RP01, DeviceObj)
  External (\_SB.PC00.RP02, DeviceObj)
  External (\_SB.PC00.RP03, DeviceObj)
  External (\_SB.PC00.RP04, DeviceObj)
  External (\_SB.PC00.RP05, DeviceObj)
  External (\_SB.PC00.RP06, DeviceObj)
  External (\_SB.PC00.RP07, DeviceObj)
  External (\_SB.PC00.RP08, DeviceObj)
  External (\_SB.PC00.RP09, DeviceObj)
  External (\_SB.PC00.RP10, DeviceObj)
  External (\_SB.PC00.RP11, DeviceObj)
  External (\_SB.PC00.RP12, DeviceObj)
  External (\_SB.PC00.RP01.HPME, MethodObj)
  External (\_SB.PC00.RP02.HPME, MethodObj)
  External (\_SB.PC00.RP03.HPME, MethodObj)
  External (\_SB.PC00.RP04.HPME, MethodObj)
  External (\_SB.PC00.RP05.HPME, MethodObj)
  External (\_SB.PC00.RP06.HPME, MethodObj)
  External (\_SB.PC00.RP07.HPME, MethodObj)
  External (\_SB.PC00.RP08.HPME, MethodObj)
  External (\_SB.PC00.RP09.HPME, MethodObj)
  External (\_SB.PC00.RP10.HPME, MethodObj)
  External (\_SB.PC00.RP11.HPME, MethodObj)
  External (\_SB.PC00.RP12.HPME, MethodObj)
  External (\_SB.PC00.TXHC, DeviceObj)
  External (\_SB.PC00.GFX0, DeviceObj)
  External (\_SB.PC00.IMNG, MethodObj)
  External (\PCHS, FieldUnitObj)
  External (\PCHH, IntObj)
  External (S210, FieldUnitObj)
  External (\_SB.PC00.I3C0.VDID, FieldUnitObj)
  External (\_SB.PC00.I3C0.PSTE, FieldUnitObj)
  External (\_SB.PC00.I3C0.PMEN, FieldUnitObj)
  External (\_SB.PC00.I3C0.PSTS, FieldUnitObj)
  External (\_SB.PC00.I3C2.VDID, FieldUnitObj)
  External (\_SB.PC00.I3C2.PSTE, FieldUnitObj)
  External (\_SB.PC00.I3C2.PMEN, FieldUnitObj)
  External (\_SB.PC00.I3C2.PSTS, FieldUnitObj)
  External (\GP1E, IntObj)

  Scope (\_GPE) {

    //
    // SL61 - Hot Plug Event
    //
    Method (SL61) {
    }

    //
    // SL69
    // PME event (PCH's GPE 69h) is received on one or more of the PCI Express* ports or
    // an assert PMEGPE message received via DMI
    //
    Method (SL69) {
    }

    //
    // SL71 - Check if iGfx is in D3 and notify iGfx
    //
    Method (SL71) {
      If (CondRefOf (\_SB.PC00.IMNG)) {
        \_SB.PC00.IMNG (0x71)
      }
    }

    //
    // SL76
    // PME event (TCSS PCI EXP GPE 76h) is received on one or more of the TCSS PCI Express* ports
    //
    Method (SL76) {
      //
      //  Check if TCSS xHCi existis in the system
      //
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (LEqual (\_SB.PC00.TRP0.HPME (), 1)) {
          Notify (\_SB.PC00.TRP0, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP1.HPME (), 1)) {
          Notify (\_SB.PC00.TRP1, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP2.HPME (), 1)) {
          Notify (\_SB.PC00.TRP2, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP3.HPME (), 1)) {
          Notify (\_SB.PC00.TRP3, 0x02)
        }
      }
    }

    //
    // TPR3 - Is any TCSS PCIe RP in D3?
    //
    Method (TPR3) {
      If (CondRefOf (\_SB.PC00.TRP0.PID3)) {
        If (LEqual (\_SB.PC00.TRP0.PID3 (), 1)) {
          Return (1)
        }
      }
      If (CondRefOf (\_SB.PC00.TRP1.PID3)) {
        If (LEqual (\_SB.PC00.TRP1.PID3 (), 1)) {
          Return (1)
        }
      }
      If (CondRefOf (\_SB.PC00.TRP2.PID3)) {
        If (LEqual (\_SB.PC00.TRP2.PID3 (), 1)) {
          Return (1)
        }
      }
      If (CondRefOf (\_SB.PC00.TRP3.PID3)) {
        If (LEqual (\_SB.PC00.TRP3.PID3 (), 1)) {
          Return (1)
        }
      }
      Return (0)
    }

    //
    // SL77 - TCSS Hot Plug Event
    //
    Method (SL77) {
      //PCI Express Base Specification Revision 5.0 Version 1.0
      //6.6.2 Function Level Reset (FLR)
      //Avoiding Data Corruption From Stale Completions
      //"..If software issues an FLR while there are outstanding Requests,
      //and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      //Completions that arrive afterwards may cause data corruption..."
      If (CondRefOf (\_SB.PC00.TXHC)) {
        //
        // The iTBT PCIe Hot-Plug event
        //
        // Check power state of all iTBT RP
        // Determine all iTBT RP power state are in D0 state
        // If all are in D0 state, skip waiting 100 ms to prevent redundant delay
        If (LEqual (TPR3 (), 1)) {
          Sleep (100)
          \_SB.PC00.TRP0.HPEV ()
          \_SB.PC00.TRP1.HPEV ()
          \_SB.PC00.TRP2.HPEV ()
          \_SB.PC00.TRP3.HPEV ()
        }

        //
        // WA for Hot-Plug event
        // Toggle LTREN if device is disconnected from RP
        //
        \_SB.PC00.TRP0.LTRC ()
        \_SB.PC00.TRP1.LTRC ()
        \_SB.PC00.TRP2.LTRC ()
        \_SB.PC00.TRP3.LTRC ()
      }
    }

    //
    // SL6F - handle events from PCIE when RTD3 is supported
    //
    Method (SL6F) {
      //
      //  Method call for PEG0/1/2/3 ports to handle 2-tier RTD3 GPE events
      //
      If (LEqual (PGRT,1)) {} // if PCIE RTD3 enabled
    }

    //
    // SL8E - I3C GPE1 Event
    //
    Method (SL8E) {
      //
      //  Method call to handle I3C GPE events
      //
      If (LNotEqual (\_SB.PC00.I3C0.VDID, 0xFFFFFFFF)) {
        If (And (\_SB.PC00.I3C0.PSTS, 1)) {
          If (CondRefOf (\_SB.PC00.I3C0)) {
            Notify (\_SB.PC00.I3C0, 0x02)
            Sleep (100)
          }
        }
      }

      If (LNotEqual (\_SB.PC00.I3C2.VDID, 0xFFFFFFFF)) {
        If (And (\_SB.PC00.I3C2.PSTS, 1)) {
          If (CondRefOf (\_SB.PC00.I3C2)) {
            Notify (\_SB.PC00.I3C2, 0x02)
            Sleep (100)
          }
        }
      }

      If (And (B_ACPI_IO_GPE1_STS_31_0_LPSS_PME_B0_STS, S210)) {
        Store (B_ACPI_IO_GPE1_STS_31_0_LPSS_PME_B0_STS, S210)
      }

    }

    //
    // SLA8 - TCSS PCIE0 Hot Plug Event, GPE1
    // TC_PCIE0_HOT_PLUG
    //
    Method (SLA8) {
      //PCI Express Base Specification Revision 5.0 Version 1.0
      //6.6.2 Function Level Reset (FLR)
      //Avoiding Data Corruption From Stale Completions
      //"..If software issues an FLR while there are outstanding Requests,
      //and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      //Completions that arrive afterwards may cause data corruption..."
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (CondRefOf (\_SB.PC00.TRP0.PID3)) {
          If (LEqual (\_SB.PC00.TRP0.PID3 (), 1)) {
            Sleep (100)
            \_SB.PC00.TRP0.HPEV ()
          }
        }
        \_SB.PC00.TRP0.LTRC ()
      }
    }

    //
    // SLA9 - TCSS PCIE1 Hot Plug Event, GPE1
    // TC_PCIE1_HOT_PLUG
    //
    Method (SLA9) {
      //PCI Express Base Specification Revision 5.0 Version 1.0
      //6.6.2 Function Level Reset (FLR)
      //Avoiding Data Corruption From Stale Completions
      //"..If software issues an FLR while there are outstanding Requests,
      //and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      //Completions that arrive afterwards may cause data corruption..."
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (CondRefOf (\_SB.PC00.TRP1.PID3)) {
          If (LEqual (\_SB.PC00.TRP1.PID3 (), 1)) {
            Sleep (100)
            \_SB.PC00.TRP1.HPEV ()
          }
        }
        \_SB.PC00.TRP1.LTRC ()
      }
    }

    //
    // SLAA - TCSS PCIE2 Hot Plug Event, GPE1
    // TC_PCIE2_HOT_PLUG
    //
    Method (SLAA) {
      //PCI Express Base Specification Revision 5.0 Version 1.0
      //6.6.2 Function Level Reset (FLR)
      //Avoiding Data Corruption From Stale Completions
      //"..If software issues an FLR while there are outstanding Requests,
      //and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      //Completions that arrive afterwards may cause data corruption..."
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (CondRefOf (\_SB.PC00.TRP2.PID3)) {
          If (LEqual (\_SB.PC00.TRP2.PID3 (), 1)) {
            Sleep (100)
            \_SB.PC00.TRP2.HPEV ()
          }
        }
        \_SB.PC00.TRP2.LTRC ()
      }
    }

    //
    // SLAB - TCSS PCIE3 Hot Plug Event, GPE1
    // TC_PCIE3_HOT_PLUG
    //
    Method (SLAB) {
      //PCI Express Base Specification Revision 5.0 Version 1.0
      //6.6.2 Function Level Reset (FLR)
      //Avoiding Data Corruption From Stale Completions
      //"..If software issues an FLR while there are outstanding Requests,
      //and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      //Completions that arrive afterwards may cause data corruption..."
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (CondRefOf (\_SB.PC00.TRP3.PID3)) {
          If (LEqual (\_SB.PC00.TRP3.PID3 (), 1)) {
            Sleep (100)
            \_SB.PC00.TRP3.HPEV ()
          }
        }
        \_SB.PC00.TRP3.LTRC ()
      }
    }

    //
    // SLC8
    // PME event (TCSS GPE1 48h) is received on one or more of the PCI Express ports or
    // an assert PMEGPE message received via DMI
    // GPE1_TC_PCI0_PCI_EXP
    //
    Method (SLC8) {
      //
      //  Check if TCSS xHCi exists in the system
      //
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (LEqual (\_SB.PC00.TRP0.HPME (), 1)) {
          Notify (\_SB.PC00.TRP0, 0x02)
        }
      }
    }

    //
    // SLC9
    // PME event (TCSS GPE1 49h) is received on one or more of the PCI Express ports or
    // an assert PMEGPE message received via DMI
    // GPE1_TC_PCI1_PCI_EXP
    //
    Method (SLC9) {
      //
      //  Check if TCSS xHCi exists in the system
      //
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (LEqual (\_SB.PC00.TRP1.HPME (), 1)) {
          Notify (\_SB.PC00.TRP1, 0x02)
        }
      }
    }

    //
    // SLCA
    // PME event (TCSS GPE1 4Ah) is received on one or more of the PCI Express ports or
    // an assert PMEGPE message received via DMI
    // GPE1_TC_PCI2_PCI_EXP
    //
    Method (SLCA) {
      //
      //  Check if TCSS xHCi exists in the system
      //
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (LEqual (\_SB.PC00.TRP2.HPME (), 1)) {
          Notify (\_SB.PC00.TRP2, 0x02)
        }
      }
    }

    //
    // SLCB
    // PME event (TCSS GPE1 48h) is received on one or more of the PCI Express ports or
    // an assert PMEGPE message received via DMI
    // GPE1_TC_PCI3_PCI_EXP
    //
    Method (SLCB) {
      //
      //  Check if TCSS xHCi exists in the system
      //
      If (CondRefOf (\_SB.PC00.TXHC)) {
        If (LEqual (\_SB.PC00.TRP3.HPME (), 1)) {
          Notify (\_SB.PC00.TRP3, 0x02)
        }
      }
    }

    //
    // SLA0 - Hot Plug Event for SPA
    //
    Method (SLA0) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC00
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

#if FixedPcdGetBool (PcdWclSupport) == 0
    //
    // SLA1 - Hot Plug Event for SPB
    //
    Method (SLA1) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC00
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
#endif

#if FixedPcdGetBool (PcdWclSupport) == 1
    //
    // SLA2 - Hot Plug Event for SPC
    //
    Method (SLA2) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC00
      #define ROOT_PORT_OBJECT  RP05
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP06
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT

      #undef HOST_BUS
    }
#endif

    //
    // _LBC - Hot Plug Event for PXPC
    //
    Method (SLBC) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC00
      #define ROOT_PORT_OBJECT  RP09
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #define ROOT_PORT_OBJECT  RP10
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    If (LEqual (PCHS, PCHH)) {
      //
      // _LB8 - Hot Plug Event for PXPD
      //
      Method (SLB8) {
        // PCI Express Base Specification Revision 5.0 Version 1.0
        // 6.6.2 Function Level Reset (FLR)
        // Avoiding Data Corruption From Stale Completions
        // "..If software issues an FLR while there are outstanding Requests,
        // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
        // Completions that arrive afterwards may cause data corruption..."
        // Step 4 - Wait 100ms
        Sleep (100)

        #define HOST_BUS \_SB.PC00
        #define ROOT_PORT_OBJECT  RP11
        Include ("RootPortHotPlug.asl")
        #undef ROOT_PORT_OBJECT
        #undef HOST_BUS
      }

      //
      // _LB9 - Hot Plug Event for PXPE
      //
      Method (SLB9) {
        // PCI Express Base Specification Revision 5.0 Version 1.0
        // 6.6.2 Function Level Reset (FLR)
        // Avoiding Data Corruption From Stale Completions
        // "..If software issues an FLR while there are outstanding Requests,
        // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
        // Completions that arrive afterwards may cause data corruption..."
        // Step 4 - Wait 100ms
        Sleep (100)

        #define HOST_BUS \_SB.PC00
        #define ROOT_PORT_OBJECT  RP12
        Include ("RootPortHotPlug.asl")
        #undef ROOT_PORT_OBJECT
        #undef HOST_BUS
      }
    }

    //
    // _LC0 Pci Express status for PXPA
    // PME event (GPE C0h) is received on one or more of the PCI Express* ports or
    // an assert PMEGPE message is received via DMI
    //
    Method (SLC0) {

      //
      // Lookup PME SCI Status on each Root Port
      //
      \_SB.PC00.RP01.HPME ()
      \_SB.PC00.RP02.HPME ()
      \_SB.PC00.RP03.HPME ()
      \_SB.PC00.RP04.HPME ()
    }

  #if FixedPcdGetBool (PcdWclSupport) == 0
    //
    // SLC1 Pci Express status for SPB
    // PME event (GPE C1h) is received on one or more of the PCI Express* ports or
    // an assert PMEGPE message is received via DMI
    //
    Method (SLC1) {
      //
      // Lookup PME SCI Status on each Root Port
      //
      \_SB.PC00.RP05.HPME ()
      \_SB.PC00.RP06.HPME ()
      \_SB.PC00.RP07.HPME ()
      \_SB.PC00.RP08.HPME ()
    }
  #endif

  #if FixedPcdGetBool (PcdWclSupport) == 1
    //
    // SLC2 Pci Express status for SPC
    // PME event (GPE C2h) is received on one or more of the PCI Express* ports or
    // an assert PMEGPE message is received via DMI
    //
    Method (SLC2) {
      //
      // Lookup PME SCI Status on each Root Port
      //
      \_SB.PC00.RP05.HPME ()
      \_SB.PC00.RP06.HPME ()
    }
  #endif

    //
    // _LDC Pci Express status for PXPC
    // PME event (GPE DCh) is received on one or more of the PCI Express* ports or
    // an assert PMEGPE message is received via DMI
    //
    Method (SLDC) {
      //
      // Lookup PME SCI Status on each Root Port
      //
      \_SB.PC00.RP09.HPME ()
      \_SB.PC00.RP10.HPME ()
    }

    If (LEqual (PCHS, PCHH)) {
      //
      // _LD8 Pci Express status for PXPD
      // PME event (GPE DCh) is received on one or more of the PCI Express* ports or
      // an assert PMEGPE message is received via DMI
      //
      Method (SLD8) {
        //
        // Lookup PME SCI Status on each Root Port
        //
        \_SB.PC00.RP11.HPME ()
      }

      //
      // _LD9 Pci Express status for PXPE
      // PME event (GPE D9h) is received on one or more of the PCI Express* ports or
      // an assert PMEGPE message is received via DMI
      //
      Method (SLD9) {
        //
        // Lookup PME SCI Status on each Root Port
        //
        \_SB.PC00.RP12.HPME ()
      }
    }
  } // End Scope (\_GPE)
} //End DefinitionBlock
