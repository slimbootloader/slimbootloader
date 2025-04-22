/** @file
  ACPI DSDT table

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  External(\_GPE.AL6F, MethodObj)
  External(P0WK)
  External(P1WK)
  External(P2WK)
  External(P3WK)
  External(\_SB.AWAC, DeviceObj)  // ACPI000E Wake Alarm Timer device
  External(\_SB.AWAC.WAST, IntObj)
  External(\_SB.PWRB, DeviceObj)

  External (\_GPE.SL69, MethodObj)
  External (\_GPE.SL61, MethodObj)
  External (\_GPE.SL62, MethodObj)
  External (\_GPE.SL66, MethodObj)
  External (\_GPE.SL6F, MethodObj)
  External (\_GPE.SL6D, MethodObj)

  External(SPNT, MethodObj)
  External(AL6D, FieldUnitObj)

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
#if FixedPcdGetBool(PcdAdlLpSupport) == 1
  Method(_L73, 0 , serialized) {
  }
#endif

  If (LEqual(AL6D, 0x1)) {
    Method(_L6D) {
      If (CondRefOf (\_GPE.SL6D)) {
        SL6D ();
      }
    }
  }

  Method(_L69, 0, serialized) {

    SL69 ()
  }

  // PCI Express Hot-Plug caused the wake event.

  Method(_L61)
  {
    Add(L01C,1,L01C)  // Increment L01 Entry Count.

    P8XH(0,0x01)      // Output information to Port 80h.
    P8XH(1,L01C)

    SL61 ()
  }

  //
  // Software GPE caused the event.
  //
  Method(_L62)
  {
    // Clear GPE status bit.
    Store(0,GPEC)
  }

  //
  // IGD OpRegion SCI event (see IGD OpRegion/Software SCI BIOS SPEC).
  //
  Method(_L66)
  {
    SL66 ()
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
#if FixedPcdGetBool(PcdAdlNSupport) == 0
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
          SPNT()
        }
      }

      // Make it an attended wake event, no matter what.
      Notify(\_SB.PWRB,0x02)
    }
#endif
    Return ()
  }

  Name(PRES, 1) // keep last PCH PwrBtn status, 0=press, 1=release
  Method(_L6F) // 2-tier GPE event handler
  {
    If (CondRefOf(\_GPE.AL6F))
    {
       \_GPE.AL6F()        // call handler from RTD3 table
    } Else {
    }
    SL6F ()
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
