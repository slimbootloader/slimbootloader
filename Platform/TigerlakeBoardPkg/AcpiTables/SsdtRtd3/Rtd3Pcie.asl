/** @file
  ACPI RTD3 SSDT table for PCIe

  Copyright (c) 2011 - 2017, Intel Corporation. All rights reserved.<BR>
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
External(WKOT)

// Variable holding PCH Rp Mapped under VMD status
External(PRMV)

// Variable holding hybrid drive connection status
External(HBCS)

// WAKE enable on PCIe device.
Name(WKEN, 0)

// Last OFF Timestamp (WOFF): The time stamp of the last power resource _OFF method evaluation
Name(WOFF, 0)

Name(_S0W, 4)

Method(_DSW, 3)
{
/// This method is used to enable/disable wake from PCIe (WKEN)
  If(Arg1)
  { /// If entering Sx, need to disable WAKE# from generating runtime PME
    Store(0, WKEN)
    //
    // Arm for wake on a device that will remain in D0 and has wake signal disabled in runtime
    //
    If(CondRefOf(WKOT)) {
      If (LEqual(WKOT, 1)) {
        If(LEqual(\_SB.GHPO(WAKG), 1)) {
          //Sx Entry - arm for wake
          \_SB.SHPO(WAKG, 0)
          Store (0, WKOT)
        }
      }
    }
  } Else {  /// If Staying in S0
    If(LAnd(Arg0, Arg2)) ///- Check if Exiting D0 and arming for wake
    { ///- Set PME
      Store(1, WKEN)
    } Else { ///- Disable runtime PME, either because staying in D0 or disabling wake
      Store(0, WKEN)
      //
      // Other Wake related configuration for devices that
      // do not fully support RTD3
      //
      If(CondRefOf(WKOT)) {
        If(LEqual(WKOT, 2)) {
          //First Boot - make sure wake is disarmed
          \_SB.SHPO(WAKG, 1)
          Store (1, WKOT)
        } ElseIf (LEqual(WKOT, 0)) {
          If(LEqual(\_SB.GHPO(WAKG), 1)) {
            //Resume from Sx
          } Else {
            \_SB.SHPO(WAKG, 1)
            //Wake did not come from this device - disarimng
          }
          Store (1, WKOT)
        }
      }
    }
  }

/** @defgroup pcie_dsw PCIE _DSW **/
} // End _DSW

//
// PCIe slot power resource definition
//
PowerResource(PXP, 0, 0) {
  Method(_STA, 0) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return(0)
    }
    If (CondRefOf(HBCS)) {
      If (LEqual (HBCS, 1)) {
        Return (0)
      }
    }
    If (CondRefOf(PRMV)) {
      If (LEqual (PRMV, 1)) {
        Return (0)
      }
    }
    Return(PSTA())
  }

  Method(_ON) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return()
    }

    If (CondRefOf(HBCS)) {
      If (LEqual (HBCS, 1)) {
        Return ()
      }
    }

    If (CondRefOf(PRMV)) {
      If (LEqual (PRMV, 1)) {
        Return ()
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
  }

  Method(_OFF) {
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return()
    }

    If (CondRefOf(HBCS)) {
      If (LEqual (HBCS, 1)) {
        Return ()
      }
    }

    If (CondRefOf(PRMV)) {
      If (LEqual (PRMV, 1)) {
        Return ()
      }
    }

    //
    // Trigger L2/L3 ready entry flow in rootport
    //
    DL23()

    //
    // Turn off slot power
    //
    POFF()
  }
}

// Returns the status of PCIe slot core power
Method(PSTA, 0) {
  //
  // RESET# assertion is mandatory for PCIe RTD3
  // So if RESET# is asserted the whole slot is off
  //
  If(\PIN.STA(RSTG)) {
    Return(0)
  } Else {
    Return (1)
  }
}

// Turn on power to PCIe Slot
// Since this method is also used by the remapped devices to turn on power to the slot
// this method should not make any access to the PCie config space.
Method(PON) {
  // Restore power to the modPHY
  \_SB.PSD0(SLOT)

  // Turn ON Power for PCIe Slot
  If(CondRefOf(PWRG)) {
    // Delay by 10 ms if required using WOFF
    If(CondRefOf(WOFF)) {
      If(LNotEqual(WOFF, Zero)) {
        Divide(Subtract(Timer(), WOFF), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
        If(LLess(Local0, 200)) {                           // If Elapsed time is less than 20ms
          Sleep(Subtract(200, Local0))                     // Sleep for the remaining time
        }
        Store(0, WOFF)
      }
    }
    \PIN.ON(PWRG)
    Sleep(PEP0)
  }

  If(CondRefOf(SCLK)) {
    SPCO(SCLK, 1)
  }

  // De-Assert Reset Pin
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
  }

  // Power OFF for Slot
  If(CondRefOf(PWRG)) {
    // If WAKP has not been defined we can safely disable power.
    // If WAKP is defined this slot does not supply device with auxilary power and we have to keep primary power
    // to allow for WAKE. If WAKP is not equal to 0 and WKEN has been enabled do not disable the power.
    If(CondRefOf (WAKP)) {
      If(LOr(LEqual(WAKP, 0), LNot(WKEN))) {
        \PIN.OFF(PWRG)
      }
    } Else {
      \PIN.OFF(PWRG)
    }
    // Store current timestamp in WOFF
    If(CondRefOf(WOFF)) {
      Store(Timer(), WOFF)
    }
  }

  // enable WAKE
  If(CondRefOf (WAKG)) {
    If(LAnd(LNotEqual(WAKG, 0), WKEN)) {
      \_SB.SHPO(WAKG, 0)
    }
  }
}

Method(_PR0) {
  Return(Package(){PXP})
}

Method(_PR3) {
  Return(Package(){PXP})
}
