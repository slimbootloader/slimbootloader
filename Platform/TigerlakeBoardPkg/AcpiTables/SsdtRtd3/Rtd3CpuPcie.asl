/** @file
  ACPI RTD3 SSDT table for CPU PCIe(DEKEL PHY)

  Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
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
// External decalarations for optional objects.
// Defined by board specific code.
//
External(WAKG)
External(PWRG)
External(TMCS)
External(SCLK)
External(WAKP)
External(DGBA)
External(D2BA)
External(D2OF, MethodObj)
External(DGON, MethodObj)
External(DGOF, MethodObj)
External(CPRT)

// Variable holding CPU Port Mapped under VMD status
External(CPMV)

// WAKE enable on PCIe device.
Name(WKEN, 0)

// Last OFF Timestamp (WOFF): The time stamp of the last power resource _OFF method evaluation
Name(WOFF, 0)

Method(_S0W, 0) {
    Return(4)
}

Method(_DSW, 3)
{
/// This method is used to enable/disable wake from PCIe (WKEN)
  If(Arg1)
  { /// If entering Sx, need to disable WAKE# from generating runtime PME
    Store(0, WKEN)
  } Else {  /// If Staying in S0
    If(LAnd(Arg0, Arg2)) ///- Check if Exiting D0 and arming for wake
    { ///- Set PME
      Store(1, WKEN)
    } Else { ///- Disable runtime PME, either because staying in D0 or disabling wake
      Store(0, WKEN)
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
    If (CondRefOf(CPMV)) {
      If (LEqual (CPMV, 1)) {
        Return (0)
      }
    }
    Return(PSTA())
  }

  Method(_ON) {
    If (LEqual (CPRT,0)) {
      Return()
    }

    If (LAnd(CondRefOf(DGON), (LNotEqual(DGBA, 0)))) {
      DGON()
      Return()
    }
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return()
    }

    If (CondRefOf(CPMV)) {
      If (LEqual (CPMV, 1)) {
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

    //
    // Notify RTD3 Exit to EC
    //
    If(LNotEqual(D2BA, 0)) {
      If(\ECON) {
        \_SB.PC00.LPCB.H_EC.ECMD (0x3B)
      }
    }
  }

  Method(_OFF) {
    If (LEqual (CPRT,0)) {
      Return()
    }

    If (LAnd(CondRefOf(DGOF), (LNotEqual(DGBA, 0)))) {
      DGOF()
      Return()
    }
    If (LEqual (VDID, 0xFFFFFFFF)) {
      Return()
    }

    If (CondRefOf(CPMV)) {
      If (LEqual (CPMV, 1)) {
        Return ()
      }
    }

    //
    // Notify RTD3 Entry to EC
    //
    If(LNotEqual(D2BA, 0)) {
      If(\ECON) {
        \_SB.PC00.LPCB.H_EC.ECMD (0x3A)
      }
    }
    //
    // Trigger L2/L3 ready entry flow in rootport
    //
    DL23()

    //
    // Turn off slot power
    //
    If (LAnd(CondRefOf(D2OF), (LNotEqual(D2BA, 0)))) {
      D2OF()
    } Else {
      POFF()
    }
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
    If(CondRefOf(TMCS)) {
      SPCX(SCLK, 1, TMCS)
    } Else {
      SPCO(SCLK, 1)
    }
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

  //
  // On RTD3 entry, BIOS will instruct the PMC to disable source clocks.
  // This is done through sending a PMC IPC command.
  //
  If(CondRefOf(SCLK)) {
    If(CondRefOf(TMCS)) {
      SPCX(SCLK, 0, TMCS)
    } Else {
      SPCO(SCLK, 0)
    }
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
