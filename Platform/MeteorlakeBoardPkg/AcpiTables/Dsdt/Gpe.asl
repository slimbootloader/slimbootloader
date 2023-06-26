/** @file
  ACPI DSDT table

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
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
  External (\_SB.PC00.AWAC, DeviceObj)  // ACPI000E Wake Alarm Timer device
  External (\_SB.PC00.AWAC.WAST, IntObj)
  External (\_SB.PWRB, DeviceObj)

  External (\_GPE.SL69, MethodObj)
  External (\_GPE.SL61, MethodObj)
  External (\_GPE.SL62, MethodObj)
  External (\_GPE.PL6B, MethodObj)
  External (\_GPE.SL6F, MethodObj)
  External (\_GPE.SLA7, MethodObj)
  External (\_GPE.SLC7, MethodObj)
  External (\_GPE.SLA0, MethodObj)
  External (\_GPE.SLA1, MethodObj)
  External (\_GPE.SLA2, MethodObj)
  External (\_GPE.SLC0, MethodObj)
  External (\_GPE.SLC1, MethodObj)
  External (\_GPE.SLC2, MethodObj)

  External (\_SB.PC00.LPCB.WKEC, MethodObj)

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
  }

  Method (_L69, 0, serialized) {

    SL69 ()
  }

  // PCI Express Hot-Plug caused the wake event.
  Method (_L61)
  {
    Add (L01C,1,L01C)  // Increment L01 Entry Count.

    P8XH (0,0x01)      // Output information to Port 80h.
    P8XH (1,L01C)

    SL61 ()
  }

  //
  // Software GPE caused the event.
  //
  Method (_L62)
  {
    // Clear GPE status bit.
    Store (0,GPEC)
  }

  //
  // GPP_E_16 = EC WAKETIME SCI
  //
  Method (_L50)
  {

    \_SB.PC00.LPCB.WKEC ()

    Return ()
  }

  Method (_L6B) {
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
  //
  // AdvancedFeaturesEnd
  //

  // GPE1_IOE_PCI_EXP
  // GPE1
  If (CondRefOf (GP1E)) {
    If (LNotEqual (GP1E, 0)) {
      Method (_LC7, 0, serialized) {

        SLC7 ()
      }
    }
  }

  // IOE PCI Express Hot-Plug caused the wake event. GPE1
  If (CondRefOf (GP1E)) {
    If (LNotEqual (GP1E, 0)) {
      Method (_LA7)
      {
        Add (L01C,1,L01C)  // Increment L01 Entry Count.

        P8XH (0,0xA7)      // Output information to Port 80h.
        P8XH (1,L01C)

        SLA7 ()
      }
    }
  }

  Method (_LA0, 0, serialized) {
    SLA0 ()
  }

  Method (_LA1, 0, serialized) {
    SLA1 ()
  }

  Method (_LA2, 0, serialized) {
    SLA2 ()
  }

  Method (_LC0, 0, serialized) {
    SLC0 ()
  }

  Method (_LC1, 0, serialized) {
    SLC1 ()
  }

  Method (_LC2, 0, serialized) {
    SLC2 ()
  }
}
