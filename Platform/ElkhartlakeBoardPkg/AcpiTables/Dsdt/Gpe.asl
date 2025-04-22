/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/PchRegsPcr.h>

  External(D1F0)
  External(D1F1)
  External(D1F2)
  External(D6F0)
  External(\_GPE.AL6F, MethodObj)
  External(\_GPE.P0L6, MethodObj)
  External(\_GPE.P1L6, MethodObj)
  External(\_GPE.P2L6, MethodObj)
  External(\_GPE.P3L6, MethodObj)
  External(SGGP)
  External(P1GP)
  External(P2GP)
  External(P3GP)
  External(P0WK)
  External(P1WK)
  External(P2WK)
  External(P3WK)
  External(\_SB.HWPI, IntObj) // HWP Interrupt Status
  External(\_SB.DTSI, IntObj) // DTS Interrupt Status
  External(\_SB.ITBI, IntObj) // Intel Turbo Boost Max Technology 3.0 (ITBM) Interrupt Status
  External(\_GPE.HLVT, MethodObj) // Interrupt handler for HWP
  External(\_GPE.ITBH, MethodObj) // Interrupt handler for Intel Turbo Boost Max Technology 3.0
  External(\_SB.PC00.TDM0)
  External(\_SB.PC00.TDM1)
  External(\_SB.PC00.PSED)
  External(\_GPE.ITBT, MethodObj)
  External(\_SB.AWAC, DeviceObj)  // ACPI000E Wake Alarm Timer device
  External(\_SB.AWAC.WAST, IntObj)
  External(\_SB.PC00.GFX0.IUEH, MethodObj)
  External(\_SB.PC00.GFX0.IUER, IntObj)

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
  //
  // Home Button
  //
  Method(_L29, 0, Serialized) {
    \_SB.HIDD.HHBE ()
  }
  // Volume Up Button
  Method(_L2C, 0, Serialized) {
    \_SB.HIDD.HVUE ()
  }
  //
  // Volume Down Button
  //
  Method(_L48, 0, Serialized) {
    \_SB.HIDD.HVDE ()
  }
  Method(_L73, 0 , serialized) {
  }
  Method(_L68, 0 , serialized) {
    if (LEqual(\_SB.PC00.PSED.IPME(), 1)) {
      Notify(\_SB.PC00.PSED, 0x02)
    }

    //
    // Notify to PSE GBE if WoL is enabled
    //
    If(LEqual(\PWOL, 1)) {
      // Notify Only PME status bit set on PMCSR for PSE TSN0 device
      if (LEqual(\_SB.PC00.OTN0.IPME(), 1)) {
        Notify(\_SB.PC00.OTN0, 0x02)
      }

      // Notify Only PME status bit set on PMCSR for PSE TSN1 device
      if (LEqual(\_SB.PC00.OTN1.IPME(), 1)) {
        Notify(\_SB.PC00.OTN1, 0x02)
      }
    }
  }
  Method(_L6B, 0 , serialized) {
  }
  //
  // This PME event (PCH's GPE 69h) is received on one or more of the PCI Express* ports or
  // an assert PMEGPE message received via DMI
  //
  Method(_L69, 0, serialized) {
    \_SB.PC00.RP01.HPME()
    \_SB.PC00.RP02.HPME()
    \_SB.PC00.RP03.HPME()
    \_SB.PC00.RP04.HPME()
    \_SB.PC00.RP05.HPME()
    \_SB.PC00.RP06.HPME()
    \_SB.PC00.RP07.HPME()
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
              //If(LOr(LNotEqual(TBTS, 0x01),LOr(LNotEqual(RPS0,0x7),LNotEqual(RPS1,0x7)))) {
              //Notify(\_SB.PC00.RP07,0)
            //}
//            }
          }
        }
      }
      Else
      {
        Store(1,\_SB.PC00.RP07.HPSX)
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
    ///
    /// Handle HWP SCI event
    ///
    If (LEqual(\_SB.HWPI, 1)) {
      If (CondRefOf(\_GPE.HLVT)) {
        \_GPE.HLVT()
      }
      ///
      /// Clear HWP interrupt status
      ///
      Store(0,\_SB.HWPI)
    }
    ///
    /// Handle Intel Turbo Boost Max Technology 3.0 SCI event
    ///
    If (LEqual(\_SB.ITBI, 1)) {
      If (CondRefOf(\_GPE.ITBH)) {
        \_GPE.ITBH()
      }
      ///
      /// Clear interrupt status
      ///
      Store(0,\_SB.ITBI)
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
    Return ()
  }

  Method(_L42, 0, Serialized) // ACPI GPE GPPC_C_2
  {
    If(LNotEqual(VBSG,0)) {
      Store (PCRR (PID_GPIOCOM4, 0x140), Local1)
      And (Local1, 0x4, Local2)
      If (LEqual (Local2, 0x4)) {
        If (CondRefOf (\_SB.ADCS)) {
          \_SB.ADCS(1)
        }
        PCRO (PID_GPIOCOM4, 0x140, 0x4)
      }
    }
  }
  Method(_L6F, 0, Serialized) // 2-tier GPE event handler
  {
    //
    // handle events from RTD3 support
    //
    If(LEqual(RTD3,1)) // if RTD3 enabled
    {
      If (CondRefOf(\_GPE.AL6F))
      {
        \_GPE.AL6F()        // call handler from RTD3 table
      }
    }
    Store (PCRR (PID_GPIOCOM2, 0x140), Local1)
    And (Local1, 0x4, Local2)
    If (LEqual (Local2, 0x4)) {
      \_SB.HIDD.HPBE()
      PCRO (PID_GPIOCOM2, 0x140, 0x4)
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
