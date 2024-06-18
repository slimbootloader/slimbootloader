/** @file
  ACPI DSDT table

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  External (\_GPE.AL6B, MethodObj)
  External (\_GPE.AL6F, MethodObj)
  External (P0WK)
  External (P1WK)
  External (P2WK)
  External (P3WK)
  External (\_SB.PC00.TRP0.HPME, MethodObj)
  External (\_SB.PC00.TRP1.HPME, MethodObj)
  External (\_SB.PC00.TRP2.HPME, MethodObj)
  External (\_SB.PC00.TRP3.HPME, MethodObj)
  External (\_SB.PC00.TRP0.HPEV, MethodObj)
  External (\_SB.PC00.TRP1.HPEV, MethodObj)
  External (\_SB.PC00.TRP2.HPEV, MethodObj)
  External (\_SB.PC00.TRP3.HPEV, MethodObj)
  External (\_SB.PC00.TDM0)
  External (\_SB.PC00.TDM1)
  External (\_SB.PC00.IMNG, MethodObj)
  External (\_SB.PC00.AWAC, DeviceObj)  // ACPI000E Wake Alarm Timer device
  External (\_SB.PC00.AWAC.WAST, IntObj)
  External (\_SB.PWRB, DeviceObj)

  External (\_GPE.SL69, MethodObj)
  External (\_GPE.SL61, MethodObj)
  External (\_GPE.SL62, MethodObj)
  External (\_GPE.PL6B, MethodObj)
  External (\_GPE.SL6F, MethodObj)
  External (\_GPE.SL71, MethodObj)
  External (\_GPE.SLA7, MethodObj)
  External (\_GPE.SLC7, MethodObj)
  External (\_GPE.SLA0, MethodObj)
  External (\_GPE.SLA1, MethodObj)
  External (\_GPE.SLA2, MethodObj)
  External (\_GPE.SLC0, MethodObj)
  External (\_GPE.SLC1, MethodObj)
  External (\_GPE.SLC2, MethodObj)

  External (\_SB.PC00.LPCB.WKEC, MethodObj)

  #if (FixedPcdGet8(PcdMtlSSupport) == 0)
  #define PMC_ACPI_BASE_ADDRESS       0x1800 // ABAS

  External (\_SB.PWPR, MethodObj)
  External (\_SB.PWRR, MethodObj)
  #endif

  //
  // General Purpose Events.  This Scope handles the Run-time and
  // Wake-time SCIs.  The specific method called will be determined by
  // the _Lxx value, where xx equals the bit location in the General
  // Purpose Event register (s).
  //

  // Note:
  // Originally, the two GPE methods below are automatically generated, but, for ASL code restructuring,
  // disabled the automatic generation and declare the ASL code here.
  //
Scope (\_GPE)
{
  Method (_L73, 0 , serialized) {
    ADBG ("L73 Event")
  }

  Method (_L69, 0, serialized) {
    ADBG ("L69 Event")

    SL69 ()
  }

  // PCI Express Hot-Plug caused the wake event.
  Method (_L61)
  {
    ADBG ("L61 Event")
    Add (L01C,1,L01C)  // Increment L01 Entry Count.

    P8XH (0,0x01)      // Output information to Port 80h.
    P8XH (1,L01C)

    SL61 ()
  }

#if (FixedPcdGet8(PcdMtlSSupport) == 0)
  Device (PBOR) {
    Method (_ADR, 0) {
      Return (0x0)
    }
    Method (_STA) {
      Return (0x0)
    }
    OperationRegion (PMC3, SystemIO, PMC_ACPI_BASE_ADDRESS, 0xFF)
    Field (PMC3, WordAcc,Lock,Preserve) {
      Offset (R_ACPI_IO_PM1_EN),
      PWEN, 16,   // Power Enable bit
    }
  }

  Name(PBPR, 0) // 0: Button released 1: Button pressed
  Name(PBTM, 0) // Store timer

  // GPP_H_20, power button.
  Method(_E54)
  {
    ADBG ("E54 Event")

    // Have Power Enable bit set to zero.
    Store (And(~B_ACPI_IO_PM1_EN_PWRBTN, \_GPE.PBOR.PWEN), \_GPE.PBOR.PWEN)
    ADBG (Concatenate ("[1] PWEN = ", ToHexString (\_GPE.PBOR.PWEN)))

    If (LEqual (PBPR, 0)) {
      ADBG ("Power Button press")
      Store (1, PBPR)
      Store(Timer(), PBTM) // Start the timer
    } else {
      ADBG ("Power Button release")
      Store (0, PBPR)
      Divide(Subtract(Timer(), PBTM), 10000, , Local0) // Store Elapsed time in ms, ignore remainder

      If (LLessEqual (Local0, 4000)) { // If Elapsed time is less than or equal to 4 seconds, it is a short press.
        ADBG ("Power Button short press")
        \_SB.PWPR ()
        Sleep(500)                     // Sleep 500 ms
        \_SB.PWRR ()
      }
    }
  }
#endif

  //
  // Software GPE caused the event.
  //
  Method (_L62)
  {
    ADBG ("L62 Event")
    // Clear GPE status bit.
    Store (0,GPEC)
  }

  //
  // GPP_E_16 = EC WAKETIME SCI
  //
  Method (_L50)
  {
    ADBG ("L50 Event")

    \_SB.PC00.LPCB.WKEC ()

    Return ()
  }

  Method (_L6B) {
    ADBG ("_L6B Event")
    If (CondRefOf (\_GPE.AL6B))
    {
       \_GPE.AL6B ()        // call handler from RTD3 table
    }

    If (CondRefOf (\_GPE.PL6B)) {
      \_GPE.PL6B ()
    }
  }

  Name (PRES, 1) // keep last PCH PwrBtn status, 0=press, 1=release
  Method (_L6F) // 2-tier GPE event handler
  {
    ADBG ("L6F Event")
    If (CondRefOf (\_GPE.AL6F))
    {
       \_GPE.AL6F ()        // call handler from RTD3 table
    }
    SL6F ()
  }

  //
  // AdvancedFeaturesBegin
  //
  //GPE 72h handler for Time And Alarm Device
  Method (_L72, 0, Serialized) {
    If (CondRefOf (\_SB.PC00.AWAC)) {
      Store (1, \_SB.PC00.AWAC.WAST) // Indicates Wake Alarm expired.
      Notify (\_SB.PC00.AWAC, 0x02) // Notify the OSPM of Alarm Device Wake.
    }
  }

  Method (_L71, 0, Serialized) {
    If (CondRefOf (\_GPE.SL71)) {
      SL71()
    }
  }
  //
  // AdvancedFeaturesEnd
  //

//
//

  // GPE1_IOE_PCI_EXP
  // GPE1
  If (CondRefOf (GP1E)) {
    If (LNotEqual (GP1E, 0)) {
      Method (_LC7, 0, serialized) {
        ADBG ("LC7 Event")

        SLC7 ()
      }
    }
  }

  // IOE PCI Express Hot-Plug caused the wake event. GPE1
  If (CondRefOf (GP1E)) {
    If (LNotEqual (GP1E, 0)) {
      Method (_LA7)
      {
        ADBG ("LA7 Event")
        Add (L01C,1,L01C)  // Increment L01 Entry Count.

        P8XH (0,0xA7)      // Output information to Port 80h.
        P8XH (1,L01C)

        SLA7 ()
      }
    }
  }

  Method (_LA0, 0, serialized) {
    ADBG ("LA0 Event")
    SLA0 ()
  }

  Method (_LA1, 0, serialized) {
    ADBG ("LA1 Event")
    SLA1 ()
  }

  Method (_LA2, 0, serialized) {
    ADBG ("LA2 Event")
    SLA2 ()
  }

  Method (_LC0, 0, serialized) {
    ADBG ("LC0 Event")
    SLC0 ()
  }

  Method (_LC1, 0, serialized) {
    ADBG ("LC1 Event")
    SLC1 ()
  }

  Method (_LC2, 0, serialized) {
    ADBG ("LC2 Event")
    SLC2 ()
  }
}
