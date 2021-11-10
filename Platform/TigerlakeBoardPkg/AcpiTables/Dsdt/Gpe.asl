/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
  External(D1F0)
  External(D1F1)
  External(D1F2)
  External(D6F0)
  External(PG0E)
  External(PG1E)
  External(PG2E)
  External(PG3E)
  External(\_SB.PC00.PEG0, DeviceObj)
  External(\_SB.PC00.PEG1, DeviceObj)
  External(\_SB.PC00.PEG2, DeviceObj)
  External(\_SB.PC00.PEG3, DeviceObj)
  External(\_SB.PC00.PEG0.HPME, MethodObj)
  External(\_SB.PC00.PEG1.HPME, MethodObj)
  External(\_SB.PC00.PEG2.HPME, MethodObj)
  External(\_SB.PC00.PEG3.HPME, MethodObj)
  External(\_SB.PC00.PEG0.HPEV, MethodObj)
  External(\_SB.PC00.PEG1.HPEV, MethodObj)
  External(\_SB.PC00.PEG2.HPEV, MethodObj)
  External(\_SB.PC00.PEG3.HPEV, MethodObj)
  External(\_GPE.AL6F, MethodObj)
  External(\_GPE.P0L6, MethodObj)
  External(\_GPE.P1L6, MethodObj)
  External(\_GPE.P2L6, MethodObj)
  External(\_GPE.P3L6, MethodObj)
  External(CPRT)
  External(SGGP)
  External(P1GP)
  External(P2GP)
  External(P3GP)
  External(P0WK)
  External(P1WK)
  External(P2WK)
  External(P3WK)
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
  External(\_SB.DTSI, IntObj) // DTS Interrupt Status
  External(\_SB.PC00.TDM0)
  External(\_SB.PC00.TDM1)
  External(\_GPE.ITBT, MethodObj)
  External(\_SB.AWAC, DeviceObj)  // ACPI000E Wake Alarm Timer device
  External(\_SB.AWAC.WAST, IntObj)
  External(\_SB.PWRB, DeviceObj)
  //
  // General Purpose Events.  This Scope handles the Run-time and
  // Wake-time SCIs.  The specific method called will be determined by
  // the _Lxx value, where xx equals the bit location in the General
  // Purpose Event register(s).
  //

  // Note:
  // Originally, the two GPE methods below are automatically generated, but, for ASL code restructuring,
  // disabled the automatic generation and declare the ASL code here.
  //
Scope(\_GPE)
{
  Method(_L73, 0 , serialized) {
  }

  //
  // This PME event (PCH's GPE 69h) is received on one or more of the PCI Express* ports or
  // an assert PMEGPE message received via DMI
  //
  Method(_L69, 0, serialized) {

    If (CondRefOf(\_SB.PC00.TXHC)) {
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

    If(LEqual(D6F0,1))
    {
      \_SB.PC00.PEG0.HPME()
      Notify(\_SB.PC00.PEG0, 0x02)
      Notify(\_SB.PC00.PEG0.PEGP, 0x02)
    }

    If(LEqual(D1F0,1))
    {
      \_SB.PC00.PEG1.HPME()
      Notify(\_SB.PC00.PEG1, 0x02)
    }

    If(LEqual(D1F1,1))
    {
      \_SB.PC00.PEG2.HPME()
      Notify(\_SB.PC00.PEG2, 0x02)
    }

    If(LEqual(D1F2,1))
    {
      If (CondRefOf(\_SB.PC00.PEG3)) {
        \_SB.PC00.PEG3.HPME()
        Notify(\_SB.PC00.PEG3, 0x02)
      }
    }
  }

  // PCI Express Hot-Plug caused the wake event.

  Method(_L61)
  {
    Add(L01C,1,L01C)  // Increment L01 Entry Count.

    P8XH(0,0x01)      // Output information to Port 80h.
    P8XH(1,L01C)

    // Delay for 100ms to meet the timing requirements
    // of the PCI Express Base Specification, Revision
    // 1.0A, Section 6.6 ("...software must wait at
    // least 100ms from the end of reset of one or more
    // device before it is permitted to issue
    // Configuration Requests to those devices").
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
    if (LEqual(PG3E,1)) {
      \_SB.PC00.PEG3.HPEV()
    }

    // Check Root Port 1 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP01.VDID,0xFFFFFFFF),\_SB.PC00.RP01.HPSX))
    {
      If(\_SB.PC00.RP01.PDCX)
      {
        // Clear all status bits first.

        Store(1,\_SB.PC00.RP01.PDCX)
        Store(1,\_SB.PC00.RP01.HPSX)

        //
        // PCH BIOS Spec Update Rev 1.03, Section 8.9 PCI Express* Hot-Plug BIOS Support
        // In addition, BIOS should intercept Presence Detect Changed interrupt, enable L0s on
        // hot plug and disable L0s on hot unplug. BIOS should also make sure the L0s is
        // disabled on empty slots prior booting to OS.
        //
        If(LNot(\_SB.PC00.RP01.PDSX)) {
          // The PCI Express slot is empty, so disable L0s on hot unplug
          //
          Store(0,\_SB.PC00.RP01.L0SE)

        }

        // Perform proper notification
        // to the OS.
        Notify(\_SB.PC00.RP01,0)
      }
      Else
      {
        // False event.  Clear Hot-Plug Status
        // then exit.

        Store(1,\_SB.PC00.RP01.HPSX)
      }
    }

    // Check Root Port 2 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP02.VDID,0xFFFFFFFF),\_SB.PC00.RP02.HPSX))
    {
      If(\_SB.PC00.RP02.PDCX)
      {
        Store(1,\_SB.PC00.RP02.PDCX)
        Store(1,\_SB.PC00.RP02.HPSX)

        If(LNot(\_SB.PC00.RP02.PDSX)) {
          Store(0,\_SB.PC00.RP02.L0SE)
        }

        Notify(\_SB.PC00.RP02,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP02.HPSX)
      }
    }

    // Check Root Port 3 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP03.VDID,0xFFFFFFFF),\_SB.PC00.RP03.HPSX))
    {
      If(\_SB.PC00.RP03.PDCX)
      {
        Store(1,\_SB.PC00.RP03.PDCX)
        Store(1,\_SB.PC00.RP03.HPSX)

        If(LNot(\_SB.PC00.RP03.PDSX)) {
          Store(0,\_SB.PC00.RP03.L0SE)
        }

        Notify(\_SB.PC00.RP03,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP03.HPSX)
      }
    }

    // Check Root Port 4 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP04.VDID,0xFFFFFFFF),\_SB.PC00.RP04.HPSX))
    {
      If(\_SB.PC00.RP04.PDCX)
      {
        Store(1,\_SB.PC00.RP04.PDCX)
        Store(1,\_SB.PC00.RP04.HPSX)

        If(LNot(\_SB.PC00.RP04.PDSX)) {
          Store(0,\_SB.PC00.RP04.L0SE)
        }

        Notify(\_SB.PC00.RP04,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP04.HPSX)
      }
    }

    // Check Root Port 5 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP05.VDID,0xFFFFFFFF),\_SB.PC00.RP05.HPSX))
    {
      If(\_SB.PC00.RP05.PDCX)
      {
        Store(1,\_SB.PC00.RP05.PDCX)
        Store(1,\_SB.PC00.RP05.HPSX)

        If(LNot(\_SB.PC00.RP05.PDSX)) {
          Store(0,\_SB.PC00.RP05.L0SE)
        }

        Notify(\_SB.PC00.RP05,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP05.HPSX)
      }
    }

    // Check Root Port 6 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP06.VDID,0xFFFFFFFF),\_SB.PC00.RP06.HPSX))
    {
      If(\_SB.PC00.RP06.PDCX)
      {
        Store(1,\_SB.PC00.RP06.PDCX)
        Store(1,\_SB.PC00.RP06.HPSX)

        If(LNot(\_SB.PC00.RP06.PDSX)) {
          Store(0,\_SB.PC00.RP06.L0SE)
        }

        Notify(\_SB.PC00.RP06,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP06.HPSX)
      }
    }

    // Check Root Port 7 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP07.VDID,0xFFFFFFFF),\_SB.PC00.RP07.HPSX))
    {
      If(\_SB.PC00.RP07.PDCX)
      {
        Store(1,\_SB.PC00.RP07.PDCX)
        Store(1,\_SB.PC00.RP07.HPSX)

        If(LNot(\_SB.PC00.RP07.PDSX)) {
          Store(0,\_SB.PC00.RP07.L0SE)
        }

        If(LEqual(PFLV,FlavorDesktop))
        {
          Notify(\_SB.PC00.RP07,0)
        }
        Else
        {
          If (\ECON)
          {
//@todo: Waiting for DOCK offect data
//            If(LEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.DOCK)), 0))
//            { // If not docked then it's hot plug
            Notify(\_SB.PC00.RP07,0)
//            }
          }
        }
      }
      Else
      {
        Store(1,\_SB.PC00.RP07.HPSX)
      }
    }

    // Check Root Port 8 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP08.VDID,0xFFFFFFFF),\_SB.PC00.RP08.HPSX))
    {
      If(\_SB.PC00.RP08.PDCX)
      {
        Store(1,\_SB.PC00.RP08.PDCX)
        Store(1,\_SB.PC00.RP08.HPSX)

        If(LNot(\_SB.PC00.RP08.PDSX)) {
          Store(0,\_SB.PC00.RP08.L0SE)
        }

        If(LEqual(PFLV,FlavorDesktop))
        {
          Notify(\_SB.PC00.RP08,0)
        }
        Else
        {
          If (\ECON)
          {
//@todo: Waiting for DOCK offect data
//            If(LEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.DOCK)), 0))
//            { // If not docked then it's hot plug
            Notify(\_SB.PC00.RP08,0)
//            }
          }
        }
      }
      Else
      {
        Store(1,\_SB.PC00.RP08.HPSX)
      }
    }

    // Check Root Port 9 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP09.VDID,0xFFFFFFFF),\_SB.PC00.RP09.HPSX))
    {
      If(\_SB.PC00.RP09.PDCX)
      {
        Store(1,\_SB.PC00.RP09.PDCX)
        Store(1,\_SB.PC00.RP09.HPSX)

        If(LNot(\_SB.PC00.RP09.PDSX)) {
          Store(0,\_SB.PC00.RP09.L0SE)
        }

        Notify(\_SB.PC00.RP09,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP09.HPSX)
      }
    }

    // Check Root Port 10 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP10.VDID,0xFFFFFFFF),\_SB.PC00.RP10.HPSX))
    {
      If(\_SB.PC00.RP10.PDCX)
      {
        Store(1,\_SB.PC00.RP10.PDCX)
        Store(1,\_SB.PC00.RP10.HPSX)

        If(LNot(\_SB.PC00.RP10.PDSX)) {
          Store(0,\_SB.PC00.RP10.L0SE)
        }

        Notify(\_SB.PC00.RP10,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP10.HPSX)
      }
    }

    // Check Root Port 11 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP11.VDID,0xFFFFFFFF),\_SB.PC00.RP11.HPSX))
    {
      If(\_SB.PC00.RP11.PDCX)
      {
        Store(1,\_SB.PC00.RP11.PDCX)
        Store(1,\_SB.PC00.RP11.HPSX)

        If(LNot(\_SB.PC00.RP11.PDSX)) {
          Store(0,\_SB.PC00.RP11.L0SE)
        }

        Notify(\_SB.PC00.RP11,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP11.HPSX)
      }
    }

    // Check Root Port 12 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP12.VDID,0xFFFFFFFF),\_SB.PC00.RP12.HPSX))
    {
      If(\_SB.PC00.RP12.PDCX)
      {
        Store(1,\_SB.PC00.RP12.PDCX)
        Store(1,\_SB.PC00.RP12.HPSX)

        If(LNot(\_SB.PC00.RP12.PDSX)) {
          Store(0,\_SB.PC00.RP12.L0SE)
        }

        Notify(\_SB.PC00.RP12,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP12.HPSX)
      }
    }

    // Check Root Port 13 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP13.VDID,0xFFFFFFFF),\_SB.PC00.RP13.HPSX))
    {
      If(\_SB.PC00.RP13.PDCX)
      {
        Store(1,\_SB.PC00.RP13.PDCX)
        Store(1,\_SB.PC00.RP13.HPSX)

        If(LNot(\_SB.PC00.RP13.PDSX)) {
          Store(0,\_SB.PC00.RP13.L0SE)
        }

        Notify(\_SB.PC00.RP13,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP13.HPSX)
      }
    }

    // Check Root Port 14 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP14.VDID,0xFFFFFFFF),\_SB.PC00.RP14.HPSX))
    {
      If(\_SB.PC00.RP14.PDCX)
      {
        Store(1,\_SB.PC00.RP14.PDCX)
        Store(1,\_SB.PC00.RP14.HPSX)

        If(LNot(\_SB.PC00.RP14.PDSX)) {
          Store(0,\_SB.PC00.RP14.L0SE)
        }

        Notify(\_SB.PC00.RP14,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP14.HPSX)
      }
    }

    // Check Root Port 15 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP15.VDID,0xFFFFFFFF),\_SB.PC00.RP15.HPSX))
    {
      If(\_SB.PC00.RP15.PDCX)
      {
        Store(1,\_SB.PC00.RP15.PDCX)
        Store(1,\_SB.PC00.RP15.HPSX)

        If(LNot(\_SB.PC00.RP15.PDSX)) {
          Store(0,\_SB.PC00.RP15.L0SE)
        }

        Notify(\_SB.PC00.RP15,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP15.HPSX)
      }
    }

    // Check Root Port 16 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP16.VDID,0xFFFFFFFF),\_SB.PC00.RP16.HPSX))
    {
      If(\_SB.PC00.RP16.PDCX)
      {
        Store(1,\_SB.PC00.RP16.PDCX)
        Store(1,\_SB.PC00.RP16.HPSX)

        If(LNot(\_SB.PC00.RP16.PDSX)) {
          Store(0,\_SB.PC00.RP16.L0SE)
        }

        Notify(\_SB.PC00.RP16,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP16.HPSX)
      }
    }

    // Check Root Port 17 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP17.VDID,0xFFFFFFFF),\_SB.PC00.RP17.HPSX))
    {
      If(\_SB.PC00.RP17.PDCX)
      {
        Store(1,\_SB.PC00.RP17.PDCX)
        Store(1,\_SB.PC00.RP17.HPSX)

        If(LNot(\_SB.PC00.RP17.PDSX)) {
          Store(0,\_SB.PC00.RP17.L0SE)
        }

        Notify(\_SB.PC00.RP17,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP17.HPSX)
      }
    }

    // Check Root Port 18 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP18.VDID,0xFFFFFFFF),\_SB.PC00.RP18.HPSX))
    {
      If(\_SB.PC00.RP18.PDCX)
      {
        Store(1,\_SB.PC00.RP18.PDCX)
        Store(1,\_SB.PC00.RP18.HPSX)

        If(LNot(\_SB.PC00.RP18.PDSX)) {
          Store(0,\_SB.PC00.RP18.L0SE)
        }

        Notify(\_SB.PC00.RP18,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP18.HPSX)
      }
    }

    // Check Root Port 19 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP19.VDID,0xFFFFFFFF),\_SB.PC00.RP19.HPSX))
    {
      If(\_SB.PC00.RP19.PDCX)
      {
        Store(1,\_SB.PC00.RP19.PDCX)
        Store(1,\_SB.PC00.RP19.HPSX)

        If(LNot(\_SB.PC00.RP19.PDSX)) {
          Store(0,\_SB.PC00.RP19.L0SE)
        }

        Notify(\_SB.PC00.RP19,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP19.HPSX)
      }
    }

    // Check Root Port 20 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP20.VDID,0xFFFFFFFF),\_SB.PC00.RP20.HPSX))
    {
      If(\_SB.PC00.RP20.PDCX)
      {
        Store(1,\_SB.PC00.RP20.PDCX)
        Store(1,\_SB.PC00.RP20.HPSX)

        If(LNot(\_SB.PC00.RP20.PDSX)) {
          Store(0,\_SB.PC00.RP20.L0SE)
        }

        Notify(\_SB.PC00.RP20,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP20.HPSX)
      }
    }
    // Check Root Port 21 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP21.VDID,0xFFFFFFFF),\_SB.PC00.RP21.HPSX))
    {
      If(\_SB.PC00.RP21.PDCX)
      {
        Store(1,\_SB.PC00.RP21.PDCX)
        Store(1,\_SB.PC00.RP21.HPSX)

        If(LNot(\_SB.PC00.RP21.PDSX)) {
          Store(0,\_SB.PC00.RP21.L0SE)
        }

        Notify(\_SB.PC00.RP21,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP21.HPSX)
      }
    }

    // Check Root Port 22 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP22.VDID,0xFFFFFFFF),\_SB.PC00.RP22.HPSX))
    {
      If(\_SB.PC00.RP22.PDCX)
      {
        Store(1,\_SB.PC00.RP22.PDCX)
        Store(1,\_SB.PC00.RP22.HPSX)

        If(LNot(\_SB.PC00.RP22.PDSX)) {
          Store(0,\_SB.PC00.RP22.L0SE)
        }

        Notify(\_SB.PC00.RP22,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP22.HPSX)
      }
    }

    // Check Root Port 23 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP23.VDID,0xFFFFFFFF),\_SB.PC00.RP23.HPSX))
    {
      If(\_SB.PC00.RP23.PDCX)
      {
        Store(1,\_SB.PC00.RP23.PDCX)
        Store(1,\_SB.PC00.RP23.HPSX)

        If(LNot(\_SB.PC00.RP23.PDSX)) {
          Store(0,\_SB.PC00.RP23.L0SE)
        }

        Notify(\_SB.PC00.RP23,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP23.HPSX)
      }
    }

    // Check Root Port 24 for a Hot Plug Event if the Port is
    // enabled.

    If(LAnd(LNotEqual(\_SB.PC00.RP24.VDID,0xFFFFFFFF),\_SB.PC00.RP24.HPSX))
    {
      If(\_SB.PC00.RP24.PDCX)
      {
        Store(1,\_SB.PC00.RP24.PDCX)
        Store(1,\_SB.PC00.RP24.HPSX)

        If(LNot(\_SB.PC00.RP24.PDSX)) {
          Store(0,\_SB.PC00.RP24.L0SE)
        }

        Notify(\_SB.PC00.RP24,0)
      }
      Else
      {
        Store(1,\_SB.PC00.RP24.HPSX)
      }
    }
  }

  //
  // Software GPE caused the event.
  //
  Method(_L62)
  {
    // Clear GPE status bit.
    Store(0,GPEC)

    //
    // Handle DTS Thermal SCI Event.
    //
    If(CondRefOf(\_SB.DTSE)){
      If(LGreaterEqual(\_SB.DTSE, 0x01)){
        If(LEqual(\_SB.DTSI, 1)){
          Notify(\_TZ.TZ00,0x80)
          ///
          /// Clear HWP interrupt status
          ///
          Store(0,\_SB.DTSI)
        }
      }
    }
  }

  // IGD OpRegion SCI event (see IGD OpRegion/Software SCI BIOS SPEC).

  Method(_L66)
  {
    If(LAnd(\_SB.PC00.GFX0.GSSE, LNot(GSMI))) // Graphics software SCI event?
    {
      \_SB.PC00.GFX0.GSCI() // Handle the SWSCI
    }
  }

  //
  // GPP_E_16 = EC WAKETIME SCI
  //
  Method(_L50)
  {
    // Do nothing if Desktop platform
    If (LEqual(\ECON,0))
    {
      Return ()
    }

    // If the wake event is not a Virtual Lid or Battery, then
    // it must be a Wake from either the Keyboard or Mouse. A
    // Power Button Event will be sent for both events such
    // that the video turns back on for these "attended wake"
    // events.

    // Check for Virtual Lid Event.

    If(LNotEqual(LIDS,\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.LSTE))))
    {
      Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.LSTE)),LIDS)

      If(IGDS)
      {
        // Upon waking a lid event may fail if driver is not ready.
        // If it does, set the high bit of the variable to indicate that
        // another notification is required during system callbacks.
        If(\_SB.PC00.GFX0.GLID(LIDS))
        {
          Or(0x80000000,\_SB.PC00.GFX0.CLID, \_SB.PC00.GFX0.CLID)
        }
      }
      Notify(\_SB.PC00.LPCB.H_EC.LID0,0x80)
    }
    Else
    {
      // Check for Virtual Power Event.

      If(LEqual(\_SB.PC00.LPCB.H_EC.BNUM,0))
      {
        If(LNotEqual(PWRS,\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.VPWR))))
        {
          // Update NVS Power State.
          Store(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.VPWR)),PWRS)
          // Perform needed ACPI Notifications.
          PNOT()
        }
      }

      // Make it an attended wake event, no matter what.
      Notify(\_SB.PWRB,0x02)
    }
    Return ()
  }

  Name(PRES, 1) // keep last PCH PwrBtn status, 0=press, 1=release
  Method(_L6F) // 2-tier GPE event handler
  {
    If (CondRefOf(\_GPE.AL6F))
    {
       \_GPE.AL6F()        // call handler from RTD3 table
    }

    //
    // handle events from CPU PCIE RTD3 support
    //
    If(LEqual(CPRT,1)) {  // if CPU PCIE RTD3 enabled

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

      If(LEqual(P3GP,1)) // if PEG3 GPIO support is enabled
      {
        If (CondRefOf(\_GPE.P3L6))
        {
          \_GPE.P3L6()        // call handler from RTD3 table
        }
      }
    }
  }

  //
  // AdvancedFeaturesBegin
  //
  //GPE 72h handler for Time And Alarm Device
  Method (_L72, 0, Serialized){
    If (CondRefOf(\_SB.AWAC)) {
      Store (1, \_SB.AWAC.WAST) // Indicates Wake Alarm expired.
      Notify (\_SB.AWAC, 0x02) // Notify the OSPM of Alarm Device Wake.
    }
  }
  //
  // AdvancedFeaturesEnd
  //

}
