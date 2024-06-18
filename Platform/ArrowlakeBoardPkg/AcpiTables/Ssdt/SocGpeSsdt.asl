/**@file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Include/AcpiDebug.h>

DefinitionBlock (
  "SocGpe.aml",
  "SSDT",
  0x02,
  "SocGpe",
  "SocGpe ",
  0x3000
  )
{
  ACPI_DEBUG_EXTERNAL_REFERENCE

  External (\_SB.PC00, DeviceObj)
  External (\_SB.PC01, DeviceObj)
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
  External (\_SB.PC00.TRP0.CRPM, MethodObj)
  External (\_SB.PC00.TRP1.CRPM, MethodObj)
  External (\_SB.PC00.TRP2.CRPM, MethodObj)
  External (\_SB.PC00.TRP3.CRPM, MethodObj)
  External (\_SB.PC00.TDM0, DeviceObj)
  External (\_SB.PC00.TDM1, DeviceObj)
  External (\_GPE.ITBT, MethodObj)
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
  External (\PCHX, IntObj)
  External (\GP1E, IntObj)

  Scope (\_GPE) {

    //
    // _L61 - Hot Plug Event
    //
    Method (SL61) {
      //PCI Express Base Specification Revision 5.0 Version 1.0
      //6.6.2 Function Level Reset (FLR)
      //Avoiding Data Corruption From Stale Completions
      //"..If software issues an FLR while there are outstanding Requests,
      //and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      //Completions that arrive afterwards may cause data corruption..."
      //Step 4 - Wait 100ms
      If (CondRefOf (GP1E)) {
        If (LEqual (GP1E, 0)) {
          If (CondRefOf (\_SB.PC00.TXHC)) {
            //
            // The iTBT PCIe Hot-Plug event
            //
            // Check power state of all iTBT RP
            // Determine all iTBT RP power state are in D0 state
            // If all are in D0 state, skip waiting 100 ms to prevent redundant delay
            Store (0, Local0)
            Store (\_SB.PC00.TRP0.CRPM (), Local1)
            If (LNotEqual(Local1, 0xFF)) {
              Or (Local0, Local1, Local0)
            }
            Store (\_SB.PC00.TRP1.CRPM (), Local1)
            If (LNotEqual(Local1, 0xFF)) {
              Or (Local0, Local1, Local0)
            }
            Store (\_SB.PC00.TRP2.CRPM (), Local1)
            If (LNotEqual(Local1, 0xFF)) {
              Or (Local0, Local1, Local0)
            }
            Store (\_SB.PC00.TRP3.CRPM (), Local1)
            If (LNotEqual(Local1, 0xFF)) {
              Or (Local0, Local1, Local0)
            }
            If (LNotEqual (Local0, 0)) {
              ADBG ("D3 100ms delay as one of iTBT RP is in D3 state")
              Sleep (100)
              \_SB.PC00.TRP0.HPEV ()
              \_SB.PC00.TRP1.HPEV ()
              \_SB.PC00.TRP2.HPEV ()
              \_SB.PC00.TRP3.HPEV ()
            }

            //
            // WA for Hot-Plug event
            // Toggle LTREN if devices is disconnected from RP
            //
            \_SB.PC00.TRP0.LTRC ()
            \_SB.PC00.TRP1.LTRC ()
            \_SB.PC00.TRP2.LTRC ()
            \_SB.PC00.TRP3.LTRC ()
          }

          #define HOST_BUS          \_SB.PC00
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
      }
    }

    //
    // _L69
    // PME event (PCH's GPE 69h) is received on one or more of the PCI Express* ports or
    // an assert PMEGPE message received via DMI
    //
    Method (SL69) {
      //
      //  Check if TCSS xHCi existis in the system
      //
      If (CondRefOf (\_SB.PC00.TXHC)) {
        //
        //  Check if PC01 exists and has PME SCI Status set
        //
        If (LEqual (\_SB.PC00.TRP0.HPME (), 1)) {
          ADBG ("Wake TBT RP0 and DMA")
          Notify (\_SB.PC00.TDM0, 0x02)
          Notify (\_SB.PC00.TRP0, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP1.HPME (), 1)) {
          ADBG ("Wake TBT RP1 and DMA")
          Notify (\_SB.PC00.TDM0, 0x02)
          Notify (\_SB.PC00.TRP1, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP2.HPME (), 1)) {
          ADBG ("Wake TBT RP2 and DMA")
          Notify (\_SB.PC00.TDM1, 0x02)
          Notify (\_SB.PC00.TRP2, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP3.HPME (), 1)) {
          ADBG ("Wake TBT RP3 and DMA")
          Notify (\_SB.PC00.TDM1, 0x02)
          Notify (\_SB.PC00.TRP3, 0x02)
        }
      }

      If (CondRefOf (GP1E)) {
        If (LEqual (GP1E, 0)) {
          //
          // Lookup PME SCI Status on each Root Port
          //
          \_SB.PC00.RP01.HPME ()
          \_SB.PC00.RP02.HPME ()
          \_SB.PC00.RP03.HPME ()
          \_SB.PC00.RP04.HPME ()
          \_SB.PC00.RP05.HPME ()
          \_SB.PC00.RP06.HPME ()
          \_SB.PC00.RP07.HPME ()
          \_SB.PC00.RP08.HPME ()
          \_SB.PC00.RP09.HPME ()
          \_SB.PC00.RP10.HPME ()
          \_SB.PC00.RP11.HPME ()
          \_SB.PC00.RP12.HPME ()
        }
      }
    }

    //
    // _L6F - handle events from CPU PCIE when RTD3 is supported
    //
    Method (SL6F) {
      //
      //  Method call for PEG0/1/2/3 ports to handle 2-tier RTD3 GPE events
      //
      If (LEqual (PGRT,1)) {} // if PCIE RTD3 enabled
    }

    //
    // _L71
    //
    Method (SL71) {
      If (CondRefOf (\_SB.PC00.IMNG)) {
        ADBG ("SL71 - Check if iGfx is in D3 and notify iGfx")
        \_SB.PC00.IMNG (0x71)
      }
    }

    //
    // _LA7 - IOE Hot Plug Event, GPE1
    // IOE_HOT_PLUG
    //
    Method (SLA7) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      If (CondRefOf (\_SB.PC00.TXHC)) {

        ADBG ("SLA7 - Check HPCX and notify OS first")
        // Check power state of all iTBT RP
        // Determine all iTBT RP power state are in D0 state
        // If all are in D0 state, skip waiting 100 ms to prevent redundant delay
        Store (0, Local0)
        Store (\_SB.PC00.TRP0.CRPM (), Local1)
        If (LNotEqual(Local1, 0xFF)) {
          Or (Local0, Local1, Local0)
        }
        Store (\_SB.PC00.TRP1.CRPM (), Local1)
        If (LNotEqual(Local1, 0xFF)) {
          Or (Local0, Local1, Local0)
        }
        Store (\_SB.PC00.TRP2.CRPM (), Local1)
        If (LNotEqual(Local1, 0xFF)) {
          Or (Local0, Local1, Local0)
        }
        Store (\_SB.PC00.TRP3.CRPM (), Local1)
        If (LNotEqual(Local1, 0xFF)) {
          Or (Local0, Local1, Local0)
        }
        If (LNotEqual (Local0, 0)) {
          ADBG ("D3 100ms delay as one of iTBT RP is in D3 state")
          Sleep (100)
          \_SB.PC00.TRP0.HPEV ()
          \_SB.PC00.TRP1.HPEV ()
          \_SB.PC00.TRP2.HPEV ()
          \_SB.PC00.TRP3.HPEV ()
        }

        //
        // WA for Hot-Plug event
        // Toggle LTREN if devices is disconnected from RP
        //
        \_SB.PC00.TRP0.LTRC ()
        \_SB.PC00.TRP1.LTRC ()
        \_SB.PC00.TRP2.LTRC ()
        \_SB.PC00.TRP3.LTRC ()
      }
    }

    //
    // _LC7
    // PME event (PCH's GPE1 47h) is received on one or more of the PCI Express ports or
    // an assert PMEGPE message received via DMI
    // GPE1_IOE_PCI_EXP
    //
    Method (SLC7) {
      //
      //  Check if TCSS xHCi exists in the system
      //
      If (CondRefOf (\_SB.PC00.TXHC)) {
        //
        //  Check if PC01 exists and has PME SCI Status set
        //
        If (LEqual (\_SB.PC00.TRP0.HPME (), 1)) {
          ADBG ("Wake TBT RP0 and DMA")
          Notify (\_SB.PC00.TDM0, 0x02)
          Notify (\_SB.PC00.TRP0, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP1.HPME (), 1)) {
          ADBG ("Wake TBT RP1 and DMA")
          Notify (\_SB.PC00.TDM0, 0x02)
          Notify (\_SB.PC00.TRP1, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP2.HPME (), 1)) {
          ADBG ("Wake TBT RP2 and DMA")
          Notify (\_SB.PC00.TDM1, 0x02)
          Notify (\_SB.PC00.TRP2, 0x02)
        }
        If (LEqual (\_SB.PC00.TRP3.HPME (), 1)) {
          ADBG ("Wake TBT RP3 and DMA")
          Notify (\_SB.PC00.TDM1, 0x02)
          Notify (\_SB.PC00.TRP3, 0x02)
        }
      }
      //
      // Lookup PME SCI Status on the Root Ports which related to GPE1_IOE_PCI_EXP
      //
      \_SB.PC00.RP10.HPME ()
      \_SB.PC00.RP11.HPME ()
      \_SB.PC00.RP12.HPME ()
    }
    //
    // _LA0 - Hot Plug Event for SPA
    //
    Method (SLA0) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
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

    //
    // _LA1 - Hot Plug Event for SPB
    //
    Method (SLA1) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
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

    //
    // _LA2 - Hot Plug Event for SPC
    //
    Method (SLA2) {
      // PCI Express Base Specification Revision 5.0 Version 1.0
      // 6.6.2 Function Level Reset (FLR)
      // Avoiding Data Corruption From Stale Completions
      // "..If software issues an FLR while there are outstanding Requests,
      // and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      // Completions that arrive afterwards may cause data corruption..."
      // Step 4 - Wait 100ms
      Sleep (100)

      #define HOST_BUS \_SB.PC00
      #define ROOT_PORT_OBJECT  RP09
      Include ("RootPortHotPlug.asl")
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS
    }

    //
    // _LC0 Pci Express status for SPA
    // PME event (PCH's GPE C0h) is received on one or more of the PCI Express* ports or
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

    //
    // _LC1 Pci Express status for SPB
    // PME event (PCH's GPE C0h) is received on one or more of the PCI Express* ports or
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

    //
    // _LC2 Pci Express status for SPC
    // PME event (PCH's GPE C0h) is received on one or more of the PCI Express* ports or
    // an assert PMEGPE message is received via DMI
    //
    Method (SLC2) {
      //
      // Lookup PME SCI Status on each Root Port
      //
      \_SB.PC00.RP09.HPME ()
    }
  } // End Scope (\_GPE)
} //End DefinitionBlock
