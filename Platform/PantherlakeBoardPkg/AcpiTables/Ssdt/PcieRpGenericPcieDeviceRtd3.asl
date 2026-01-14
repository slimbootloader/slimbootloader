/**@file
  PCIe Root Port Generic PCIE Device Rtd3 file.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
/** @defgroup pcie_scope PCIe Root Port Scope **/

  //
  // AcpiPinDriverLib imports(from DSDT in platform)
  //
  External (\PIN.STA, MethodObj)
  External (\PIN.ON, MethodObj)
  External (\PIN.OFF, MethodObj)
  External (\PPIN.STA, MethodObj)
  External (\PPIN.ON, MethodObj)
  External (\PPIN.OFF, MethodObj)
  //
  // GpioLib imports(DSDT)
  //
  External (\_SB.SHPO, MethodObj)

  External (\_SB.PGPI.SHPO, MethodObj)

  External (D3HT, FieldUnitObj) // CPU PCIE RP Power State
  External(PGRT)

  // WAKE enable on PCIe device.
  Name (WKEN, 0)

  // Last OFF Timestamp (WOFF): The time stamp of the last power resource _OFF method evaluation
  Name (WOFF, 0)

  Name (LNRD, 0) // Delay before PERST# assertion in us
  Name (PWRR, 0) // PCIe core power removal

  // Device On/OFF delay
  External(PDOD)

  //
  // The deepest D-state supported by this device in the S0 system sleeping state where the device can wake itself,
  // "4" represents D3cold.
  //
  Method (_S0W, 0x0, NotSerialized)
  {
    If (CondRefOf (RD3C)) {
      If (LEqual (RD3C, 0x02)) {
        Return (0x4)
      }
    }
    Return (0x0)
  }

  //
  // Device Sleep Wake - sets the sleep and wake transition states for a device.
  // ACPI Specification Defined Method. Called by OSPM.
  //
  Method (_DSW, 3, NotSerialized)
  {
    /// This method is used to enable/disable wake from PCIe (WKEN)
    If (Arg1)
    { /// If entering Sx, enable Sx WAKE
      Store (1, WKEN)
    } Else {  /// If Staying in S0
      If (LAnd(Arg0, Arg2)) ///- Check if Exiting D0 and arming for wake
      { ///- Set PME
        Store (1, WKEN)
      } Else { ///- Disable runtime PME, either because staying in D0 or disabling wake
        Store (0, WKEN)
      }
    }
  /** @defgroup pcie_dsw PCIE _DSW **/
  } // End _DSW

  Method (PPS0, 0, Serialized) { // Platform specific PCIe root port _PS0 Hook Function.
  }

  Method (PPS3, 0, Serialized) { // Platform specific PCIe root port _PS3 Hook Function.
  }

  //
  // PCIe slot power resource definition
  //
  PowerResource (PXP, 0, 0) {

    Method (_STA, 0) {
      //
      // Check if PCIE RP is available or Not by Checking Vendor and Device ID.
      //
      If (LEqual (VDID, 0xFFFFFFFF)) {
        Return (0)
      }

      //
      // Check if PCIE RP Power Resource is supported or Not.
      //
      If (LEqual (GPRS (), 0)) {
        Return (0)
      }

      //
      // Return PCIE RP Power Resource Status.
      //
      Return (PSTA ())
    }

    Method (_ON) {
      //
      // Check if PCIE RP is available or Not by Checking Vendor and Device ID.
      //
      If (LEqual (VDID, 0xFFFFFFFF)) {
      }

      //
      // Check if PCIE RP Power Resource is supported or Not.
      //
      ElseIf (LEqual (GPRS (), 0)) {
      }

      Else {
      //
      // Turn on slot power
      //
      PON ()

      //
      // Trigger L2/L3 ready exit flow in rootport - transition link to Detect
      //
      L23D ()
    }
  }

    Method (_OFF) {
      //
      // Check if PCIE RP is available or Not by Checking Vendor and Device ID.
      //
      If (LEqual (VDID, 0xFFFFFFFF)) {
      }

      //
      // Check if PCIE RP Power Resource is supported or Not.
      //
      ElseIf (LEqual (GPRS (), 0)) {
      }

      Else {
      //
      // Trigger L2/L3 ready entry flow in rootport
      //
      DL23 ()

      //
      // Turn off slot power
      //
      POFF ()
      }
    }
  } // End PowerResource

  //
  // Get Permission for Power Removal.
  // Check whether or not to Disable Power Package GPIO During Device Power OFF(D3 Cold Transition).
  // Input: VOID
  //
  // @return 1 if it is Safe to Remove/Disable Power. 0 Not allow for Power Removal.
  //
  Method (GPPR, 0) {

    // If WAKP has not been defined we can safely disable power.
    // If WAKP is defined this slot does not supply device with auxilary power and we have to keep primary power
    // to allow for WAKE. If WAKP is not equal to 0 and WKEN has been enabled do not disable the power.
    If (CondRefOf (WAKP)) {
      If (LAnd (LNotEqual (WAKP, 0), LEqual (WKEN, 0))) {
        Return (0)
      }
    }

    //
    // If PWRR is 0, remove pcie slot power
    // If PWRR is 1, block pcie slot power
    //
    If (CondRefOf (PWRR)) {
      If (LNotEqual (PWRR, 0)) {
        Return (0)
      }
    }

    //
    // Now Safe To Remove/Disable Power.
    //
    Return (1)
  }

  //
  // Get PCIe RP Power Resource Support.
  // If D3 Cold is supported by PCIE RP or Not.
  // Input: VOID
  //
  // @return 1 PCIE RP Power Resource Supported. 0 PCIE RP Power Resource Not Supported.
  //
  Method (GPRS, 0) {
    If (CondRefOf (PGRT)) {
    If (LEqual (PGRT,0)) {
      Return(0)
        }
    }

    //
    // Check if D3 Cold is supported for PCIE RP.
    //
    If (CondRefOf (RD3C)) {
      If (LNotEqual (RD3C, 0x02)) {
        Return (0)
      }
    }

    //
    // Check if PCIE RP is Mapped under VMD. D3 Cold flow will be taken care by VMD and it's Child ACPI Devices.
    //
    If (CondRefOf (PRMV)) {
      If (LEqual (PRMV, 1)) {
        Return (0)
      }
    }

    //
    // D3 Cold is supported for PCIE RP
    //
    Return (1)
  }

  //
  // Returns the status of PCIe slot core power.
  //
  Method (PSTA, 0) {

    //
    // RESET# assertion is mandatory for PCIe RTD3
    // So if RESET# is asserted the whole slot is off
    //
    If (\PIN.STA (RSTG)) {
      Return (0)
    } Else {
      Return (1)
    }
  }

  // Turn on power to PCIe Slot
  // Since this method is also used by the remapped devices to turn on power to the slot
  // this method should not make any access to the PCie config space.
  Method (PON) {

    // Disable WAKE
    If (CondRefOf (WAKG)) {
      If (LNotEqual(WAKG, 0)) {
        \_SB.SHPO (WAKG, 1) // set gpio ownership to driver(0=ACPI mode, 1=GPIO mode)
        \_SB.CAGS (WAKG) // Clear GPIO Status if set.
      }
    }


    // Turn ON Power for PCIe Slot
    If (CondRefOf (PWRG)) {
      // Delay by PDOD ms if required using WOFF
      If (CondRefOf (WOFF)) {
        If (LNotEqual (WOFF, Zero)) {
          Divide (Subtract (Timer (), WOFF), 10000, , Local0) // Store Elapsed time in ms, ignore remainder
          If (LLess (Local0, PDOD)) {                           // If Elapsed time is less than PDOD
            Sleep (Subtract (PDOD, Local0))                     // Sleep for the remaining time
          }
          Store (0, WOFF)
        }
      }
      \PIN.ON (PWRG)
      Sleep (PEP0)
    }

    //
    // On RTD3 Exit, BIOS will instruct the PMC to Enable source clocks.
    // This is done through sending a PMC IPC command.
    //
    If (CondRefOf (SCLK)) {
        SPCO(SCLK, 1)
    }

    // De-Assert Reset Pin
    \PIN.OFF (RSTG)
  }

  // Turn off power to PCIe Slot
  // Since this method is also used by the remapped devices to turn off power to the slot
  // this method should not make any access to the PCIe config space.
  Method (POFF) {
    // Assert Reset Pin after the delay passed from the bus driver
    Divide (LNRD, 1000, , Local1)
    Sleep (Local1)

    // Reset pin is mandatory for correct PCIe RTD3 flow
    \PIN.ON (RSTG)


    //
    // On RTD3 entry, BIOS will instruct the PMC to disable source clocks.
    // This is done through sending a PMC IPC command.
    //
    If (CondRefOf (SCLK)) {
        SPCO(SCLK, 0)
    }

    // Power OFF for Slot
    If (CondRefOf (PWRG)) {
      If ( LEqual ( GPPR(), 1)) { // we can safely disable power.
        \PIN.OFF (PWRG)
      }
      // Store current timestamp in WOFF
      If (CondRefOf (WOFF)) {
        Store (Timer (), WOFF)
      }
    }

    // enable WAKE
    If (CondRefOf (WAKG)) {
      If (LAnd (LNotEqual (WAKG, 0), WKEN)) {
        \_SB.SHPO (WAKG, 0)
      }
    }
  }

  Method (PRST, 1, Serialized) {

    If (LEqual (Arg0, 1)) {
      // Assert Reset Pin after the delay passed from the bus driver
      \PIN.ON (RSTG)
    } Else {
      // De-Assert Reset Pin
      \PIN.OFF (RSTG)
    }
  }

  Method (_PR0) {
    Return (Package (){PXP})
  }

  Method (_PR3) {
    Return (Package (){PXP})
  }

  //
  // Update PERST# assertion delay.
  // This function will be called from reference code during PCIe _DSM function index 11 evaluation.
  // Arg0 - New delay value in microseconds. Max is 10ms
  //
  // @return Last sucessfully negotiated value in us. 0 if no such value exists.
  //
  Method (UPRD, 1, Serialized) {
    If (LLessEqual (Arg0, 10000)) {
      // If the value does not exceed the limit
      // Update last negotiated value and calculate the value in ms.
      Store (Arg0, LNRD)
    }
    Return (LNRD)
  }

  // Pcie slot core power turn on/off method.
  // Arg0 - DSM Func(A) Arg3 value to determine if
  //   power needs to be Blocked/Removed from pcie slot.
  //   0x80000000 - Block core power removal.
  //   0x0        - Allow core power removal.
  Method (PCPR, 1, Serialized) {
    If(LEqual (Arg0, 0x80000000)) {
      Store (1, PWRR) //Block slot Power Removal
      Return (2)
    } ElseIf (LEqual (Arg0, 0x0)) {
      store (0, PWRR) // Allow slot power Removal
      Return (1)
    } Else {
      Return (0)
    }
  }
