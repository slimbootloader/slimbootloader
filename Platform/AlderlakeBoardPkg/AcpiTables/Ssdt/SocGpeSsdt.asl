/**@file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <AcpiDebug.h>

DefinitionBlock (
  "SocGpe.aml",
  "SSDT",
  0x02,
  "SocGpe",
  "SocGpe ",
  0x3000
  )
{
  External(\_SB.PC00, DeviceObj)
  External(\_SB.PC01, DeviceObj)
  External(\_SB.PC00.MC, DeviceObj)
  External(\_SB.PC00.MC.D1F0, FieldUnitObj)
  External(\_SB.PC00.MC.D1F1, FieldUnitObj)
  External(\_SB.PC00.MC.D6F0, FieldUnitObj)
  External(PG0E, FieldUnitObj)
  External(PG1E, FieldUnitObj)
  External(PG2E, FieldUnitObj)
  External(\_SB.PC00.PEG0, DeviceObj)
  External(\_SB.PC00.PEG1, DeviceObj)
  External(\_SB.PC00.PEG2, DeviceObj)
  External(\_SB.PC00.PEG0.PEGP, DeviceObj)
  External(\_SB.PC00.PEG0.HPME, MethodObj)
  External(\_SB.PC00.PEG1.HPME, MethodObj)
  External(\_SB.PC00.PEG2.HPME, MethodObj)
  External(\_SB.PC00.PEG0.HPEV, MethodObj)
  External(\_SB.PC00.PEG1.HPEV, MethodObj)
  External(\_SB.PC00.PEG2.HPEV, MethodObj)
  External(\_GPE.P0L6, MethodObj)
  External(\_GPE.P1L6, MethodObj)
  External(\_GPE.P2L6, MethodObj)
  External(\_GPE.P3L6, MethodObj)
  External(CPRT, FieldUnitObj)
  External(SGGP, FieldUnitObj)
  External(P1GP, FieldUnitObj)
  External(P2GP, FieldUnitObj)
  External(\_SB.PC00.TRP0, DeviceObj)
  External(\_SB.PC00.TRP1, DeviceObj)
  External(\_SB.PC00.TRP2, DeviceObj)
  External(\_SB.PC00.TRP3, DeviceObj)
  External(\_SB.PC01.TRP0, DeviceObj)
  External(\_SB.PC01.TRP1, DeviceObj)
  External(\_SB.PC01.TRP2, DeviceObj)
  External(\_SB.PC01.TRP3, DeviceObj)
  External(\_SB.PC00.TRP0.HPME, MethodObj)
  External(\_SB.PC00.TRP1.HPME, MethodObj)
  External(\_SB.PC00.TRP2.HPME, MethodObj)
  External(\_SB.PC00.TRP3.HPME, MethodObj)
  External(\_SB.PC01.TRP0.HPME, MethodObj)
  External(\_SB.PC01.TRP1.HPME, MethodObj)
  External(\_SB.PC01.TRP2.HPME, MethodObj)
  External(\_SB.PC01.TRP3.HPME, MethodObj)
  External(\_SB.PC00.TRP0.HPEV, MethodObj)
  External(\_SB.PC00.TRP1.HPEV, MethodObj)
  External(\_SB.PC00.TRP2.HPEV, MethodObj)
  External(\_SB.PC00.TRP3.HPEV, MethodObj)
  External(\_SB.PC01.TRP0.HPEV, MethodObj)
  External(\_SB.PC01.TRP1.HPEV, MethodObj)
  External(\_SB.PC01.TRP2.HPEV, MethodObj)
  External(\_SB.PC01.TRP3.HPEV, MethodObj)
  External(\_SB.PC00.TDM0, DeviceObj)
  External(\_SB.PC00.TDM1, DeviceObj)
  External(\_GPE.ITBT, MethodObj)
  External(\_SB.PC00.RP01, DeviceObj)
  External(\_SB.PC00.RP02, DeviceObj)
  External(\_SB.PC00.RP03, DeviceObj)
  External(\_SB.PC00.RP04, DeviceObj)
  External(\_SB.PC00.RP05, DeviceObj)
  External(\_SB.PC00.RP06, DeviceObj)
  External(\_SB.PC00.RP07, DeviceObj)
  External(\_SB.PC00.RP08, DeviceObj)
  External(\_SB.PC00.RP09, DeviceObj)
  External(\_SB.PC00.RP10, DeviceObj)
  External(\_SB.PC00.RP11, DeviceObj)
  External(\_SB.PC00.RP12, DeviceObj)
  External(\_SB.PC00.RP13, DeviceObj)
  External(\_SB.PC00.RP14, DeviceObj)
  External(\_SB.PC00.RP15, DeviceObj)
  External(\_SB.PC00.RP16, DeviceObj)
  External(\_SB.PC00.RP17, DeviceObj)
  External(\_SB.PC00.RP18, DeviceObj)
  External(\_SB.PC00.RP19, DeviceObj)
  External(\_SB.PC00.RP20, DeviceObj)
  External(\_SB.PC00.RP21, DeviceObj)
  External(\_SB.PC00.RP22, DeviceObj)
  External(\_SB.PC00.RP23, DeviceObj)
  External(\_SB.PC00.RP24, DeviceObj)
  External(\_SB.PC00.RP25, DeviceObj)
  External(\_SB.PC00.RP26, DeviceObj)
  External(\_SB.PC00.RP27, DeviceObj)
  External(\_SB.PC00.RP28, DeviceObj)
  External(\_SB.PC00.RP01.HPME, MethodObj)
  External(\_SB.PC00.RP02.HPME, MethodObj)
  External(\_SB.PC00.RP03.HPME, MethodObj)
  External(\_SB.PC00.RP04.HPME, MethodObj)
  External(\_SB.PC00.RP05.HPME, MethodObj)
  External(\_SB.PC00.RP06.HPME, MethodObj)
  External(\_SB.PC00.RP07.HPME, MethodObj)
  External(\_SB.PC00.RP08.HPME, MethodObj)
  External(\_SB.PC00.RP09.HPME, MethodObj)
  External(\_SB.PC00.RP10.HPME, MethodObj)
  External(\_SB.PC00.RP11.HPME, MethodObj)
  External(\_SB.PC00.RP12.HPME, MethodObj)
  External(\_SB.PC00.RP13.HPME, MethodObj)
  External(\_SB.PC00.RP14.HPME, MethodObj)
  External(\_SB.PC00.RP15.HPME, MethodObj)
  External(\_SB.PC00.RP16.HPME, MethodObj)
  External(\_SB.PC00.RP17.HPME, MethodObj)
  External(\_SB.PC00.RP18.HPME, MethodObj)
  External(\_SB.PC00.RP19.HPME, MethodObj)
  External(\_SB.PC00.RP20.HPME, MethodObj)
  External(\_SB.PC00.RP21.HPME, MethodObj)
  External(\_SB.PC00.RP22.HPME, MethodObj)
  External(\_SB.PC00.RP23.HPME, MethodObj)
  External(\_SB.PC00.RP24.HPME, MethodObj)
  External(\_SB.PC00.RP25.HPME, MethodObj)
  External(\_SB.PC00.RP26.HPME, MethodObj)
  External(\_SB.PC00.RP27.HPME, MethodObj)
  External(\_SB.PC00.RP28.HPME, MethodObj)
  External(\_SB.PC00.TXHC, DeviceObj)
  External(\_SB.PC00.GFX0, DeviceObj)
  External(\_SB.PC00.GFX0.GSSE, FieldUnitObj)
  External(\_SB.PC00.GFX0.GSCI, MethodObj)
  External(\GSMI, FieldUnitObj)
  External(\PCHS, FieldUnitObj)
  External(\PCHX, IntObj)

  External(\_SB.PC00.XHCI.GPEH, MethodObj)
  External(\_SB.PC00.HDAS.GPEH, MethodObj)
  External(\_SB.PC00.GLAN.GPEH, MethodObj)
  External(\_SB.PC00.CNVW.GPEH, MethodObj)
  External(\_SB.PC00.XDCI.GPEH, MethodObj)

  External(\_SB.PC00.TXHC.PMES, FieldUnitObj)
  External(\_SB.PC00.TXHC._STA, MethodObj)
  External(\_SB.PC00.TXDC.PMES, FieldUnitObj)
  External(\_SB.PC00.TXDC._STA, MethodObj)
  External(\_SB.PC00.TDM0._STA, MethodObj)
  External(\_SB.PC00.TDM1._STA, MethodObj)
  External(\_SB.PC00.TDM0.STAT, IntObj)
  External(\_SB.PC00.TDM0.PMES, FieldUnitObj)
  External(\_SB.PC00.TDM1.STAT, IntObj)
  External(\_SB.PC00.TDM1.PMES, FieldUnitObj)
  External(\_SB.PC00.D3C, PowerResObj)
  External(\_SB.PC00.TBT0, PowerResObj)
  External(\_SB.PC00.TBT1, PowerResObj)
  External(\_SB.PC00.D3C._STA, MethodObj)
  External(\_SB.PC00.D3C._ON, MethodObj)
  External(\_SB.PC00.D3C._OFF, MethodObj)
  External(\_SB.PC00.TBT0._ON, MethodObj)
  External(\_SB.PC00.TBT0._OFF, MethodObj)
  External(\_SB.PC00.TBT1._ON, MethodObj)
  External(\_SB.PC00.TBT1._OFF, MethodObj)

  External(\_SB.PC00.TDM0.PMST, FieldUnitObj)
  External(\_SB.PC00.TDM1.PMST, FieldUnitObj)
  External(\_SB.PC00.TDM0.MEMS, FieldUnitObj)
  External(\_SB.PC00.TDM1.MEMS, FieldUnitObj)
  External(\_SB.PC00.TXHC.D0D3, FieldUnitObj)
  External(\_SB.PC00.TXDC.D0I3, FieldUnitObj)
  External(AL6D, FieldUnitObj)

  Scope (\_GPE) {

    //
    // _L61 - Hot Plug Event
    //
    Method (SL61) {

      If (LEqual (AL6D, 0x1)) {
        If (CondRefOf(\_SB.PC00.TXHC)) {
          If (CondRefOf(\_SB.PC01)) {
            \_SB.PC01.TRP0.HPEV()
            \_SB.PC01.TRP1.HPEV()
            \_SB.PC01.TRP2.HPEV()
            \_SB.PC01.TRP3.HPEV()
          } Else {
            \_SB.PC00.TRP0.HPEV()
            \_SB.PC00.TRP1.HPEV()
            \_SB.PC00.TRP2.HPEV()
            \_SB.PC00.TRP3.HPEV()
          }
        }
      }

      //PCI Express Base Specification Revision 5.0 Version 1.0
      //6.6.2 Function Level Reset (FLR)
      //Avoiding Data Corruption From Stale Completions
      //"..If software issues an FLR while there are outstanding Requests,
      //and then re-enables the Function for operation without waiting for potential stale Completions, any stale
      //Completions that arrive afterwards may cause data corruption..."
      //Step 4 - Wait 100ms
      Sleep(100)

      If (CondRefOf(\_SB.PC00.TXHC)) {
        //
        // The iTBT PCIe Hot-Plug event
        //
        If (CondRefOf(\_SB.PC01)) {
          \_SB.PC01.TRP0.HPEV()
          \_SB.PC01.TRP1.HPEV()
          \_SB.PC01.TRP2.HPEV()
          \_SB.PC01.TRP3.HPEV()
        } Else {
          \_SB.PC00.TRP0.HPEV()
          \_SB.PC00.TRP1.HPEV()
          \_SB.PC00.TRP2.HPEV()
          \_SB.PC00.TRP3.HPEV()
        }
      }

      // CPU PCIe Hot Plug Event
      if (LEqual(PG0E,1)) {
        \_SB.PC00.PEG0.HPEV()
      }
      if (LEqual(PG1E,1)) {
        \_SB.PC00.PEG1.HPEV()
      }
      if (LEqual(PG2E,1)) {
        \_SB.PC00.PEG2.HPEV()
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
      If(LEqual(PCHS, PCHX)) {
        #undef ROOT_PORT_OBJECT
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
        #define ROOT_PORT_OBJECT  RP25
        Include ("RootPortHotPlug.asl")
        #undef ROOT_PORT_OBJECT
        #define ROOT_PORT_OBJECT  RP26
        Include ("RootPortHotPlug.asl")
        #undef ROOT_PORT_OBJECT
        #define ROOT_PORT_OBJECT  RP27
        Include ("RootPortHotPlug.asl")
        #undef ROOT_PORT_OBJECT
        #define ROOT_PORT_OBJECT  RP28
        Include ("RootPortHotPlug.asl")
      }
      #undef ROOT_PORT_OBJECT
      #undef HOST_BUS

      If (LEqual (AL6D, 0x1)) {
        If (CondRefOf(\_SB.PC00.TXHC)) {
          If (CondRefOf(\_SB.PC01)) {
            \_SB.PC01.TRP0.HPEV()
            \_SB.PC01.TRP1.HPEV()
            \_SB.PC01.TRP2.HPEV()
            \_SB.PC01.TRP3.HPEV()
          } Else {
            \_SB.PC00.TRP0.HPEV()
            \_SB.PC00.TRP1.HPEV()
            \_SB.PC00.TRP2.HPEV()
            \_SB.PC00.TRP3.HPEV()
          }
        }
      }
    }

    //
    //  _L66 - GFX SCI
    //
    Method (SL66) {
      //
      //  Check for Integrated Graphics presence and its SCI
      //
      If (CondRefOf (\_SB.PC00.GFX0)) {
        If(LAnd(\_SB.PC00.GFX0.GSSE, LNot(GSMI))) // Graphics software SCI event?
        {
          \_SB.PC00.GFX0.GSCI() // Handle the SWSCI
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
      If (CondRefOf(\_SB.PC00.TXHC)) {
        //
        //  Check if PC01 exists and has PME SCI Status set
        //
        If (CondRefOf(\_SB.PC01)) {
          If (LEqual(\_SB.PC01.TRP0.HPME(), 1)) {
            Notify(\_SB.PC00.TDM0, 0x02)
            Notify(\_SB.PC01.TRP0, 0x02)
          }
          If (LEqual(\_SB.PC01.TRP1.HPME(), 1)) {
            Notify(\_SB.PC00.TDM0, 0x02)
            Notify(\_SB.PC01.TRP1, 0x02)
          }
          If (LEqual(\_SB.PC01.TRP2.HPME(), 1)) {
            Notify(\_SB.PC00.TDM1, 0x02)
            Notify(\_SB.PC01.TRP2, 0x02)
          }
          If (LEqual(\_SB.PC01.TRP3.HPME(), 1)) {
            Notify(\_SB.PC00.TDM1, 0x02)
            Notify(\_SB.PC01.TRP3, 0x02)
          }
        } Else {
          If (LEqual(\_SB.PC00.TRP0.HPME(), 1)) {
            Notify(\_SB.PC00.TDM0, 0x02)
            Notify(\_SB.PC00.TRP0, 0x02)
          }
          If (LEqual(\_SB.PC00.TRP1.HPME(), 1)) {
            Notify(\_SB.PC00.TDM0, 0x02)
            Notify(\_SB.PC00.TRP1, 0x02)
          }
          If (LEqual(\_SB.PC00.TRP2.HPME(), 1)) {
            Notify(\_SB.PC00.TDM1, 0x02)
            Notify(\_SB.PC00.TRP2, 0x02)
          }
          If (LEqual(\_SB.PC00.TRP3.HPME(), 1)) {
            Notify(\_SB.PC00.TDM1, 0x02)
            Notify(\_SB.PC00.TRP3, 0x02)
          }
        }
      }

      //
      // Lookup PME SCI Status on each Root Port
      //
      \_SB.PC00.RP01.HPME()
      \_SB.PC00.RP02.HPME()
      \_SB.PC00.RP03.HPME()
      \_SB.PC00.RP04.HPME()
      \_SB.PC00.RP05.HPME()
      \_SB.PC00.RP06.HPME()
      \_SB.PC00.RP07.HPME()
      \_SB.PC00.RP08.HPME()
      \_SB.PC00.RP09.HPME()
      \_SB.PC00.RP10.HPME()
      \_SB.PC00.RP11.HPME()
      \_SB.PC00.RP12.HPME()
      If(LEqual(PCHS, PCHX)) {
        \_SB.PC00.RP13.HPME()
        \_SB.PC00.RP14.HPME()
        \_SB.PC00.RP15.HPME()
        \_SB.PC00.RP16.HPME()
        \_SB.PC00.RP17.HPME()
        \_SB.PC00.RP18.HPME()
        \_SB.PC00.RP19.HPME()
        \_SB.PC00.RP20.HPME()
        \_SB.PC00.RP21.HPME()
        \_SB.PC00.RP22.HPME()
        \_SB.PC00.RP23.HPME()
        \_SB.PC00.RP24.HPME()
        \_SB.PC00.RP25.HPME()
        \_SB.PC00.RP26.HPME()
        \_SB.PC00.RP27.HPME()
        \_SB.PC00.RP28.HPME()
      }
      If(LEqual(\_SB.PC00.MC.D6F0,1))
      {
        \_SB.PC00.PEG0.HPME()
        Notify(\_SB.PC00.PEG0, 0x02)
        Notify(\_SB.PC00.PEG0.PEGP, 0x02)
      }
      If(LEqual(\_SB.PC00.MC.D1F0,1))
      {
        \_SB.PC00.PEG1.HPME()
        Notify(\_SB.PC00.PEG1, 0x02)
      }
      If(LEqual(\_SB.PC00.MC.D1F1,1))
      {
        \_SB.PC00.PEG2.HPME()
        Notify(\_SB.PC00.PEG2, 0x02)
      }
    }

    If (LEqual(AL6D, 0x1)) {
      //
      // PME# GPE event handler
      //
      Method (SL6D) {
         //The PME GPE is shared by multiple devices So BIOS must verify the same in the ASL handler by reading
        // offset for PMEENABLE and PMESTATUS bit.
        If (CondRefOf(\_SB.PC00.XHCI)) {
          \_SB.PC00.XHCI.GPEH ()
        }
        If (CondRefOf(\_SB.PC00.HDAS)) {
          \_SB.PC00.HDAS.GPEH ()
        }
        If (CondRefOf(\_SB.PC00.GLAN)) {
          \_SB.PC00.GLAN.GPEH ()
        }
        If (CondRefOf(\_SB.PC00.CNVW)) {
          \_SB.PC00.CNVW.GPEH ()
        }
        If (CondRefOf(\_SB.PC00.XDCI)) {
          \_SB.PC00.XDCI.GPEH ()
        }

        If (CondRefOf (\_SB.PC00.D3C)) { // Check if D3C power package exists
          If (LEqual(\_SB.PC00.D3C._STA(), 0x0)) { // Check if Power Packagae did run _OFF method
            \_SB.PC00.D3C._ON()
          }
        }
        If (CondRefOf(\_SB.PC00.TDM0)) {
          If (LEqual(\_SB.PC00.TDM0._STA(), 0xF)) { // Make sure device is enabled
            Store (0, Local0)
            If (LEqual(\_SB.PC00.TDM0.STAT, 0x0)) { // Check if it is in D3 Cold
              \_SB.PC00.TBT0._ON() // No need to check _STA()
              Store (1, Local0)
            }
            Store (\_SB.PC00.TDM0.PMES, Local1)
            If (LEqual(Local1, 0)) { // PME Status equal to 0, place back device in D3 Cold
              If (LEqual(Local0, 1)) {
                  If (LEqual(\_SB.PC00.TDM0.PMST, 0x3)) { // Make sure it is in D3 Hot
                     If (LEqual(\_SB.PC00.TDM0.MEMS, 0x0)) { // Make sure Memory Space Enable is cleared
                       If (LEqual(\_SB.PC00.TDM0.STAT, 0x1)) { // Check current state
                         \_SB.PC00.TBT0._OFF() // No need to check _STA()
                      }
                    }
                  }
              }
            } Else {
              Notify(\_SB.PC00.TDM0, 0x02)
            }
          }
        }
        If (CondRefOf(\_SB.PC00.TDM1)) {
          If (LEqual(\_SB.PC00.TDM1._STA(), 0xF)) { // Make sure device is enabled
            Store (0, Local0)
            If (LEqual(\_SB.PC00.TDM1.STAT, 0x0)) { // Check if it is in D3 Cold
              \_SB.PC00.TBT1._ON() // No need to check _STA()
              Store (1, Local0)
            }
            Store (\_SB.PC00.TDM1.PMES, Local1)
            If (LEqual(Local1, 0)) { // PME Status equal to 0, place back device in D3 Cold
               If (LEqual(Local0, 1)) {
                  If (LEqual(\_SB.PC00.TDM1.PMST, 0x3)) { // Make sure it is in D3 Hot
                     If (LEqual(\_SB.PC00.TDM1.MEMS, 0x0)) { // Make sure Memory Space Enable is cleared
                       If (LEqual(\_SB.PC00.TDM1.STAT, 0x1)) { // Check current state
                         \_SB.PC00.TBT1._OFF() // No need to check _STA()
                      }
                    }
                  }
               }
            } Else {
              Notify(\_SB.PC00.TDM1, 0x02)
            }
          }
        }
        If (CondRefOf(\_SB.PC00.TXDC)) {
          If (LEqual(\_SB.PC00.TXDC._STA(), 0xF)) { // Make sure device is enabled
            Store (\_SB.PC00.TXDC.PMES, Local1)
            If (LEqual(Local1, 1)) {
              Notify (\_SB.PC00.TXDC, 0x2)
            }
          }
        }
        If (CondRefOf(\_SB.PC00.TXHC)) {
          If (LEqual(\_SB.PC00.TXHC._STA(), 0xF)) { // Make sure device is enabled
            Notify (\_SB.PC00.TXHC, 0x2)
          }
        }
      }
    } //    End -> If (LEqual(AL6D, 0x1))

    //
    // _L6F - handle events from CPU PCIE when RTD3 is supported
    //
    Method (SL6F) {
      //
      //  Method call for PEG0/1/2/3 ports to handle 2-tier RTD3 GPE events
      //
        If(LEqual(SGGP,1))      // if PEG0 GPIO support is enabled
        {
          If (CondRefOf(\_GPE.P0L6))
          {
            \_GPE.P0L6()        // call handler from RTD3 table
          }
        }
        If(LEqual(P1GP,1)) // if PEG1 GPIO support is enabled
        {
          If (CondRefOf(\_GPE.P1L6))
          {
            \_GPE.P1L6()        // call handler from RTD3 table
          }
        }
        If(LEqual(P2GP,1)) // if PEG2 GPIO support is enabled
        {
          If (CondRefOf(\_GPE.P2L6))
          {
            \_GPE.P2L6()        // call handler from RTD3 table
          }
        }
    }
  } // End Scope (\_GPE)
} //End DefinitionBlock
