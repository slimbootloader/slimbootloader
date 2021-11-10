/** @file
  ACPI RTD3 SSDT table for PCIe WWAN

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

/// @details
/// Code in this file uses following variables:
/// SCLK: ICC Clock number - optional
/// WAKG: WAKE GPIO pad - optional
/// Below objects should be defined according to the format described in PinDriverLib.asl
/// RSTG: reset pin definition - mandatory
/// PWRG: power GPIO pad - optional
/// WAKP: Flag to indicate that power gating must not be performed if WAKE is enabled - optional
/// @defgroup pcie_scope PCIe Root Port Scope **/

//
// AcpiPinDriverLib imports(from DSDT in platform)
//
External(\PIN.STA, MethodObj)
External(\PIN.ON, MethodObj)
External(\PIN.OFF, MethodObj)
//
// GpioLib imports(DSDT)
//
External(\_SB.SHPO, MethodObj)

//
// HSIO lib imports
//
External(\_SB.PSD0, MethodObj)
External(\_SB.PSD3, MethodObj)
//
// External decalarations for optional objects.
// Defined by board specific code.
//
External(WAKG)
External(PWRG)
External(SCLK)
External(WAKP)

// WAKE enable on PCIe device.
Name(WKEN, 0)
Name(MOL3, 1)
Name(OFEN, 1)
Name(ONEN, 0)
Name(FLGN, 0)
Name(FLGF, 0)
Name(FLGW, 0)

Name(_S0W, 3)

Method(_DSW, 3) {
  // This method is used to enable/disable wake from PCIe (WKEN)
  If(Arg1) {                 // If entering Sx, need to disable WAKE# from generating runtime PME
    Store(0, WKEN)
  } Else {                   // If Staying in S0
    If(LAnd(Arg0, Arg2))     // Check if Exiting D0 and arming for wake
    {                        // Set PME
      Store(1, WKEN)
    } Else {                 // Disable runtime PME, either because staying in D0 or disabling wake
      Store(0, WKEN)
    }
  }

  If(LEqual(Arg0,0)) {
    Store(1, MOL3) // Modem will go to L3 state
  } Else {
    Store(0, MOL3) // Modem will go to L2 state
  }
  /** @defgroup pcie_dsw PCIE _DSW **/
} // End _DSW

//
// PCIe slot power resource definition
//
PowerResource(PXP, 0, 0) {
  Name(WOFF, 0)  // Last OFF Timestamp (WOFF): The time stamp of the last power resource _OFF method evaluation

  Method(_STA, 0) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return(0)
    }
    Return(PSTA())
  }

  Method(_ON) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return()
    }

    If (LEqual(ONEN, 0)) {
      Return()
    }

    \_SB.SHPO(WAKG, 1)

    If(CondRefOf(PWRG)) {
      If(\PIN.STA(PWRG)) {
        Store(0, FLGN)
      } Else {
        Store(1, FLGN)
      }
    }

    If(CondRefOf(^WOFF)) {
      If(LNotEqual(^WOFF, Zero)) {
        Divide(Subtract(Timer(), ^WOFF), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
        If(LLess(Local0, 500)) {                          // If Elapsed time is less than 500ms
          Sleep(Subtract(500, Local0))                    // Sleep for the remaining time
        }
      }
    }

    //
    // Turn on slot power
    //
    PON()

    //
    // Trigger L2/L3 ready exit flow in rootport - transition link to Detect
    //
    L23D()

    If(FLGN) {
      // This time includes PCIe Detect Timer and PCIe Link Establishment Timer for modem. Time delay should
      // kick in L3 Path only
      If(CondRefOf(^WOFF)) {
        If(LNotEqual(^WOFF, Zero)) {
          Store(0, ^WOFF)
          Sleep(500)
        }
      }
    }

    Store(1, OFEN)
    Store(0, ONEN)
  }

  Method(_OFF) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return()
    }

    If (LEqual(OFEN, 0)) {
      Return()
    }

    // Store the current MOL3/WKEN to prevent race problem that a new _DSW MOL3/WKEN setting is called before _OFF finish
    Store(MOL3, FLGF)
    Store(WKEN, FLGW)

    //
    // Trigger L2/L3 ready entry flow in rootport
    //
    DL23()

    //
    // Turn off slot power
    //
    POFF()

    If(FLGF) {
      // Drive BB RESET Pin low
      \PIN.ON(BRST)
      Sleep(2) // 2ms
      \PIN.OFF(PWRG)          // Set power pin to low
      If(CondRefOf(^WOFF)) {
        Store(Timer(), ^WOFF) // Start OFF timer here
      }
    }

    // Enable WAKE
    If(CondRefOf (WAKG)) {
      If(FLGW) {
        \_SB.SHPO(WAKG, 0)
      } Else {
        \_SB.SHPO(WAKG, 1)
      }
    }

    Store(0, OFEN)
    Store(1, ONEN)
  }
}

// Returns the status of PCIe slot core power
Method(PSTA, 0) {
  //
  // Detect power pin status
  //
  If(CondRefOf(PWRG)) {
    If(LNot(\PIN.STA(PWRG))) {
      Return(0)
    }
  }

  //
  // RESET# assertion is mandatory for PCIe RTD3
  // So if RESET# is asserted the whole slot is off
  //
  If(\PIN.STA(RSTG)) {
    Return(0)
  } Else {
    Return(1)
  }
}

// Turn on power to PCIe Slot
// Since this method is also used by the remapped devices to turn on power to the slot
// this method should not make any access to the PCie config space.
Method(PON) {
  // Restore power to the modPHY
  \_SB.PSD0(SLOT)

  If(FLGN) {
    // Turn ON Power for PCIe Slot
    If(CondRefOf(PWRG)) {
      \PIN.ON(PWRG)
      Sleep(PEP0)
    }

    // Drive BB Reset Pin high
    // Delay of 20ms to make it ensure modem is come out from Reset modem
    \PIN.OFF(BRST)
    Sleep(20)
  } Else {
    // As per PCIe M.2 Spec , clock Should be applied in 20 microsecond
    Stall(20)
  }

  If(CondRefOf(SCLK)) {
    SPCO(SCLK, 1)
  }

  // PERST De-assertion should be greater than 100 ms
  Sleep(100)

  // De-assert Reset Pin
  \PIN.OFF(RSTG)
}

// Turn off power to PCIe Slot
// Since this method is also used by the remapped devices to turn off power to the slot
// this method should not make any access to the PCIe config space.
Method(POFF) {

  // Assert Reset Pin
  // Reset pin is mandatory for correct PCIe RTD3 flow
  \PIN.ON(RSTG)

  // Enable modPHY power gating
  // This must be done after the device has been put in reset
  \_SB.PSD3(SLOT)

  //
  // On RTD3 entry, BIOS will instruct the PMC to disable source clocks.
  // This is done through sending a PMC IPC command.
  //
  If(CondRefOf(SCLK)) {
    SPCO(SCLK, 0)
    Sleep(16)
  }
}

Method(_PR0) {
  Return(Package(){PXP})
}
