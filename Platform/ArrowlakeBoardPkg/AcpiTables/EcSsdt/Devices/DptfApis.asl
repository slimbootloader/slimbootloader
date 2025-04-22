/** @file
  ACPI EC SSDT table for DPTF devices and APIs.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#if FixedPcdGetBool (PcdDptfSupported) == 1

External (\_SB.PLDT.FIDC, MethodObj)
External (\_SB.PC00.LPCB.H_EC.ECAV, IntObj)
External (\_SB.PC00.LPCB.H_EC.UVTH, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CMPP, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.VBNL, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.RBHF, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PBSS, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CTYP, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.BMAX, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.FCHG, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.B1FC, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.B1RC, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PWRT, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.ARTG, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PROP, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PBOK, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CHGA, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CHGV, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CHGR, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PPSL, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PINV, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PENV, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CFCS, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CFPM, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.FSTH, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.FSTL, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.FRTS, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.PSTP, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.CFSP, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.TSR1, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.TSR2, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.TSR3, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.TSR4, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.TSR5, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.TSI, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.HYST, FieldUnitObj)
External (\_SB.PC00.LPCB.H_EC.TSLT, FieldUnitObj)


// Methods needed for DPTF
Device (DPTF)
{
  //
  // Intel DPTF Container Device
  //
  Name (_HID, EISAID ("PNP0A05"))
  Name (_UID, 7)
  Name (_STR, Unicode ("DPTF Container")) // DPTF CONTAINER

  // _STA (Status)
  //
  // This object returns the current status of a device.
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   An Integer containing a device status bitmap:
  //    Bit 0 - Set if the device is present.
  //    Bit 1 - Set if the device is enabled and decoding its resources.
  //    Bit 2 - Set if the device should be shown in the UI.
  //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
  //    Bit 4 - Set if the battery is present.
  //    Bits 5-31 - Reserved (must be cleared).
  //
  Method (_STA)
  {
    Return (0x0F)
  }

  // UVTH (Under Voltage Threshold.)
  // UVTH is a command which BIOS sends to EC. [15:0]
  //
  // Arguments: (1)
  //    Arg0 should be sent by Intel (R) Dynamic Tuning Technology driver in response to power delivery capability changes. The unit is 1mV.
  // Return Value:
  //    0x0           - Success
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (UVTH, 1, Serialized)
  {
    If (^^ECAV) {
      ECWT (Arg0, RefOf (^^UVTH)) // power delivery capability changed
      ECMD (0x17)
      Return (0x0)
    } Else {
      Return (0xFFFFFFFF)
    }
  }

  // SCHG (Set Battery Charge Rate)
  //
  // Set Charge Rate in Amp
  //
  //  Arguments: (1)
  //    Arg0 -  Charge rate in Amperes
  //  Return Value:
  //    0x0           - Success
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (SCHG, 1, Serialized)
  {
    If (^^ECAV) {
      ECWT (Arg0, RefOf (^^CHGR))             // write amp value to EC
      ECMD (EC_C_CHARGE_RATE_CHANGE_NOTIFY)   // Set Charge Rate
      Return (0x0)
    } Else {
      Return (0xFFFFFFFF)
    }
  }

  /**
    UPPM          Update the Programable Power Supply Mode.

    @param        1 byte data represent the PPS mode.
    @retval       0x0           - Success.
    @retval       0xFFFFFFFF    - EC is not available.
  **/
  Method (UPPM, 1, Serialized)
  {
    If (^^ECAV) {
      ECWT (Arg0, RefOf (^^PPSM))
      ECMD (EC_C_SET_PROGRAMMABLE_POWER_SUPPLY_MODE)
      Return (0x0)
    } Else {
      Return (0xFFFFFFFF)
    }
  }

  // CMPP (Maximum peak current)
  //
  // The battery discharge current capability for at least 1ms, in 1mA unit
  //
  // Arguments: (0)
  //   None
  // Return Value:
  //   battery discharge current value in mA
  //
  Method (CMPP, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^CMPP)))
  }

  // VBNL (Battery No-Load Voltage)
  //
  // The battery instantaneous no-load voltage that can be obtained from battery fuel gauge. The unit is 1mV.
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    battery no-load voltage value in mV
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (VBNL, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^VBNL)))
  }

  // RBHF (Battery high frequency impedance Method)
  //
  // The RBHF The high frequency impedance value that can be obtained from battery fuel gauge. The unit is 1mOhm.
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    high frequency impedance value in mOhm
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (RBHF, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^RBHF)))
  }

  // PBSS (Power Battery Steady State Method)
  //
  // Returns max sustained power for battery.
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    power in milliwatts
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (PBSS, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^PBSS)))
  }

  // CTYP (Charger Type Method)
  //
  // The CTYP object provides charger type.
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    0x01 = Traditional
  //    0x02 = Hybrid
  //    0x03 = NVDC
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (CTYP, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^CTYP)))
  }

  // BMAX (Battery A Maximum Method)
  //
  // Returns Battery A Maximum value
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    Returns Battery A Maximum value
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (BMAX, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^BMAX)))
  }

  // FCHG (Check Fast Charging Enable Method)
  //
  // Checks if Fast Charging is enabled
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    Returns 1 if Fast Charging is enabled
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (FCHG, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^FCHG)))
  }

  // FNSL (Fan Set Speed Level Method)
  //
  // Set Fan Speed Level 0-100 by Fan Index
  //
  //  Arguments: (2)
  //    Arg0 - Fan Index (0-2) for fans 1-3
  //    Arg1 - New Speed Level 0-100
  //    Arg2 - Previous Speed Level. Limitation: EC doesn't remember the last Fan level in multi-fan setup
  //  Return Value:
  //    0x0           - Success
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (FNSL, 3, Serialized)
  {
    If (^^ECAV) {
      Store (\_SB.PLDT.FIDC (Arg0), Local0) // Convert Fan ID to Fan Port
      // EC implementation-specific PWM port control
      ECWT (Local0, RefOf (^^PPSL))
      ECWT (Arg2, RefOf (^^PINV))
      ECWT (Arg1, RefOf (^^PENV))
      ECWT (100, RefOf (^^PSTP))
      ECMD (0x1a)      // Command to turn the fan on
      Return (0x0)
    } Else {
      Return (0xFFFFFFFF)
    }
  }

  Name(FTHL, Package()
  {
    0xFFFFFFFF, // Fan RPM high threshold:Integer DWORD
    0xFFFFFFFF  // Fan RPM low threshold:Integer DWORD
  })

  // GFTE: Return the current fan speed threshold in RPM or percentage if fan does not report RPM information (EC Method)
  //
  // Arguments: (0)
  // None
  // Return Value:
  // Return the current fan speed threshold in RPM
  //
  Method (GFTE, 0, Serialized, , PkgObj)
  {
    If (^^ECAV) {
        Store( ECRD(RefOf(^^FSTH)), Index(FTHL, 0))
        Store( ECRD(RefOf(^^FSTL)), Index(FTHL, 1))
      }
      Return(FTHL)
  }

  // SFTE: Set the fan speed threshold in RPM.
  //
  // Arguments: (2)
  // Arg0 - Integer: conveys to the platform the fan speed High threshold to be set.
  // Arg1 - Integer: conveys to the platform the fan speed Low threshold to be set.
  // Return Value:
  // None
  //
  Method (SFTE, 2, Serialized)
  {
    If (^^ECAV) {
      ECWT (Arg0, RefOf(^^FSTH))
      ECWT (Arg1, RefOf(^^FSTL))
      ECMD (0x19) // Update Fan RPM Trip thresholds.
    }
  }

  // GFCE (Get Supported Fan Capability) (EC Method)
  //
  // Arguments: (0)
  // None
  // Return Value:
  // Return the current supported fan capability
  // Bit0 - Fan speed control supported, 0 - DTT cann't change fan speed directly, 1 - DTT can change the fan speed
  // Bit1 - Fan operating mode control supported, 0 - DTT cann't change fan operating mode, 1 - DTT can change the fan mode
  // Bit2 - Fan RPM report supported, 0 - Fan dosn't report RPM information(pin4 is not available), 1 - Fan can report RPM information to DTT
  // Bit3 - Fan maintainance mode supported, 0 - Fan doesn't support maintainance mode, 1 - Fan supports maintainance mode
  //
  Method (GFCE, 0, Serialized)
  {
    If (^^ECAV) {
      Return (ECRD (RefOf(^^CFCS)))
    } Else {
      Return (0)
    }
  }

  // GFME (Get Fan Policy Mode) (EC Method)
  //
  // Arguments: (0)
  // None
  // Return Value:
  // Return the current fan policy mode
  // 0 - Default - EC controls the fan operating mode
  // 1 - Performance - EC should allow high fan speed for best performance
  // 2 - Balanced - EC should allow median fan speed for better performance
  // 3 - Quiet - EC should allow low fan speed for quietness
  // 4-14 - Reserved
  // 15 - Maintainance - EC should perform any maintainance operation including dust cleaning by reverting the fan direction. OEM should decide the fan speed value in the maintainance mode
  //
  Method (GFME, 0, Serialized)
  {
    If (^^ECAV) {
      Return (ECRD (RefOf(^^CFPM)))
    } Else {
      Return (0)
    }
  }

  // SFME (Set Fan Policy Mode)
  //
  // Set the desired fan policy based on the current system usage scenarios
  //
  // Arguments: (1)
  // Arg0 - Integer: conveys to the platform the fan policy mode to be set.
  // Return Value:
  // None
  //
  Method (SFME, 1, Serialized)
  {
    If (^^ECAV) {
      Store(ECRD(RefOf(^^CFCS)), Local0)
      If(And(Local0, 0x2))
      { // Bit1 - Fan operating mode control supported, 0 - DTT cann't change fan operating mode, 1 - DTT can change the fan mode
        ECWT (Arg0, RefOf(^^CFPM))
        ECMD (0x1C) // Update fan operating mode.
      }
    }
  }


  // GFNS (Get Fan Speed Method)
  //
  // Read Current Fan Speed in RPM by Fan Index
  //
  //  Arguments: (1)
  //    Arg0 - Fan Index (0-2) for fans 1-3
  //  Return Value:
  //    Fan speed in RPM. report 0 RPM for invalid cases
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (GFNS, 1, Serialized, 0, IntObj)
  {
    Switch (ToInteger (Arg0))
    {
      Case (0){
        Return (ECRD (RefOf (^^CFSP)))
      }
      Case (1){
        Return (ECRD (RefOf (^^CFSP)))
      }
      Case (2){
        Return (ECRD (RefOf (^^CFSP)))
      }
      Default {
        Return (0) // Return 0 as speed if no registered Fan Present
      }
    }
  }

  // B1FC (Battery 1 Full Charge Method)
  //
  // Returns Battery 1 Full Charge Capacity
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    Battery 1 Full Charge Capacity
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (B1FC, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^B1FC)))
  }

  // B1RC (Battery 1 Remaining Charge Method)
  //
  // Returns Battery 1 Remaining Charge
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    Battery 1 Remaining Charge
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (B1RC, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^B1RC)))
  }

  // PWRT (Power Resource Type Method)
  //
  // This method is used to evaluated the current power resource type
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    bitwise encoded information about power resource connected
  //    0xFFFFFFFFF   - EC is not available
  // Notes: (Bitmap from EC)
  // PwrSrcType Bit[0] = 0 <DC>, Bit[0] = 1 <AC>, Bit[1] = 1 <USB-PD>, Bit[2] = 1 <Wireless Charging>
  // Bit[7:4]  = Power Delivery State Change Sequence Number
  //
  Method (PWRT, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^PWRT)))
  }

  // ARTG (Adapter Rating Method)
  //
  // This method returns the rating of the adapter connected in mW
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    Worst case platform power in Watts
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (ARTG, 0, Serialized, 0, IntObj)
  {
    If (^^ECAV) {
      Multiply (ECRD (RefOf (^^ARTG)), 10, Local0) // Becasue of EC-space constraint EC can only send 2bytes=15000mW but system uses 150000mW, hence bios is multiplying this value by 10.
    } Else {
      Store (0xFFFFFFFF, Local0)
    }
    Return (Local0)
  }

  // PROP (Rest of Platform Power Method)
  //
  // Worst case platform power in Watts
  //
  //  Arguments: (0)
  //    None
  //  Return Value:
  //    Worst case platform power in Watts
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (PROP, 0, Serialized, 0, IntObj)
  {
    Return (ECRD (RefOf (^^PROP)))
  }

  // PBOK (Power Boss OK Method)
  //
  // Used by DTT to inform EC about power delivery state change
  // EC will de-assert PROCHOT if the sequence number matches the current power change sequence number
  // Method used in conjuction with PSRC to provide a mechanism for AC removal protection.
  // to prevent system brown out
  //
  //  Arguments: (1)
  //    Arg0 - Power Delivery State change sequence Number
  //  Return Value:
  //    0x0           - Success
  //    0xFFFFFFFFF   - EC is not available
  //
  Method (PBOK, 1, Serialized)
  {
    // Check If EC opregion is available
    If (^^ECAV) {
      ECWT (Arg0, RefOf (^^PBOK)) // Store sequence number
      ECMD (0x15)
      Return (0x0)
    } Else {
      Return (0xFFFFFFFF)
    }
  }

  // SNRD (Sensor Read Method)
  //
  // Read a temperature value of EC Sensor by Sensor Index
  //
  //  Arguments: (1)
  //    Arg0 - Sensor Index (0-4) for sensors 1-5
  //  Return Value:
  //    Sensor temperature in tenths of deg celsius. report 300C for invalid cases
  //
  Method (SNRD, 1, Serialized, 0, IntObj)
  {
    If (^^ECAV) {
      Switch (ToInteger (Arg0))
      {
        Case (0){
          Return (ECRD (RefOf (^^TSR1)))
        }
        Case (1){
          Return (ECRD (RefOf (^^TSR2)))
        }
        Case (2){
          Return (ECRD (RefOf (^^TSR3)))
        }
        Case (3){
          Return (ECRD (RefOf (^^TSR4)))
        }
        Case (4){
          Return (ECRD (RefOf (^^TSR5)))
        }
        Default {
          Return (3000)
        }
      }
    }
    Else {
      Return (3000)
    }
  }
  //
  // Create a Mutex for PATx methods to prevent Sx resume race condition problems asscociated with EC commands.
  //
  Mutex (PATM, 0)

  // STRP (Set Trip Point Method)
  //
  // Set trip point for a specific EC sensor by Sensor Index
  //
  //  Arguments: (4)
  //    Arg0 - Sensor Index (0-4) for sensors 1-5
  //    Arg1 - Trip point temperature in tenths of degree celsius
  //    Arg2 - Sensor Hysteresis in deg celsius
  //    Arg3 - Input to indicate whether it is Low trip point or High trip point
  //           0 - Low trip point
  //           1 - High trip point
  //  Return Value:
  //    0x0           - Success
  //    0xFFFFFFFFF   - EC is not available or the Mutex acquire fail
  //
  Method (STRP, 4, Serialized, 0, IntObj)
  {
    If (^^ECAV) {
      Store (Acquire (PATM, 100), Local0)  // Save Acquire result so we can check for Mutex acquired
      If (LEqual (Local0, Zero))  // Check for Mutex acquired
      {
        ECWT (Arg0, RefOf (^^TSI)) // Select Thermal Sensor
        ECWT (Arg2, RefOf (^^HYST))  // Thermal Sensor Hysteresis, 2 degrees

        If (LEqual (Arg3, 0x00)) {
          ECWT (Arg1, RefOf (^^TSLT)) // Sensor Low Trip Point
        } Else {
          ECWT (Arg1, RefOf (^^TSHT)) // Sensor High Trip Point
        }
        ECMD (0x4A) // Set Trip point.
        Release (PATM)
        Return (0x0)
      } Else {
        Return (0xFFFFFFFFF)
      }
    } Else {
      Return (0xFFFFFFFFF)
    }
  }
}
#endif // FixedPcdGetBool (PcdDptfSupported) == 1
