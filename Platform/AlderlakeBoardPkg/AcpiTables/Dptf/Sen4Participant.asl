/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.PC00.LPCB.H_EC)
{

  Device(SEN4)
  {
    Name(_UID, "SEN4")
    Method(_HID)
    {
      Return(\_SB.IETM.GHID(_UID))  // Intel(R) Dynamic Tuning Technology Temperature Sensor Device
    }
    Name(_STR, Unicode ("Thermistor Battery Charger")) // Near Battery Charger
    Name(PTYP, 0x03)
    Name(CTYP,0)  // Mode
    // PFLG
    //   0 - Default, participant is on the main board
    //   1 - Participant device is on the docking station
    //   2 - Participant device is on the detachable base
    //   3 - Participant device is an external device (such as a USB device, Intel(R) Dynamic Tuning Technology does not support this type of device today)
    Name(PFLG, 0)

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
    Method(_STA)
    {
      If (LEqual(\S4DE,1)) {
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // _TMP (Temperature)
    //
    // This control method returns the thermal zone's current operating temperature.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current temperature of the thermal zone (in tenths of degrees Kelvin)
    //
    Method(_TMP,0,Serialized)
    {
      If(\_SB.PC00.LPCB.H_EC.ECAV)
      {
        Return(\_SB.IETM.C10K(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.TSR4))))
      } Else {
        Return(3000)
      }
    }

    // Number of Aux Trips available
    Name(PATC, 2)

    // PATx (Participant Programmable Auxiliary Trip) - Sets Aux Trip Point
    //
    // The PATx objects shall take a single integer parameter, in tenths of degree Kelvin, which
    // represents the temperature at which the device should notify the participant driver of
    // an auxiliary trip event. A PATx control method returns no value.
    //
    //  Arguments: (1)
    //    Arg0 - temperature in tenths of degree Kelvin
    //  Return Value:
    //    None
    //
    Method(PAT0, 1, Serialized)
    {
      If (\_SB.PC00.LPCB.H_EC.ECAV)
      {
        Store (Acquire(\_SB.PC00.LPCB.H_EC.PATM, 100),Local0)  // Save Acquire result so we can check for Mutex acquired
        If (LEqual(Local0, Zero))  // Check for Mutex acquired
        {
          Store(\_SB.IETM.K10C(Arg0),Local1)
          \_SB.PC00.LPCB.H_EC.ECWT(0x3, RefOf(\_SB.PC00.LPCB.H_EC.TSI)) // Select Thermal Sensor
          \_SB.PC00.LPCB.H_EC.ECWT(0x2, RefOf(\_SB.PC00.LPCB.H_EC.HYST)) // Thermal Sensor Hysteresis, 2 degrees
          \_SB.PC00.LPCB.H_EC.ECWT(Local1, RefOf(\_SB.PC00.LPCB.H_EC.TSLT)) // Sensor Low Trip Point
          \_SB.PC00.LPCB.H_EC.ECMD(0x4A) //Set Trip point.
          Release(\_SB.PC00.LPCB.H_EC.PATM)
        }
      }
    }

    // PATx (Participant Programmable Auxiliary Trip) - Sets Aux Trip Point
    //
    // The PATx objects shall take a single integer parameter, in tenths of degree Kelvin, which
    // represents the temperature at which the device should notify the participant driver of
    // an auxiliary trip event. A PATx control method returns no value.
    //
    //  Arguments: (1)
    //    Arg0 - temperature in tenths of degree Kelvin
    //  Return Value:
    //    None
    //
    Method(PAT1, 1, Serialized)
    {
      If (\_SB.PC00.LPCB.H_EC.ECAV)
      {
        Store (Acquire(\_SB.PC00.LPCB.H_EC.PATM, 100),Local0)  // Save Acquire result so we can check for Mutex acquired
        If (LEqual(Local0, Zero))  // Check for Mutex acquired
        {
          Store(\_SB.IETM.K10C(Arg0),Local1)
          \_SB.PC00.LPCB.H_EC.ECWT(0x3, RefOf(\_SB.PC00.LPCB.H_EC.TSI)) // Select Thermal Sensor
          \_SB.PC00.LPCB.H_EC.ECWT(0x2, RefOf(\_SB.PC00.LPCB.H_EC.HYST)) // Thermal Sensor Hysteresis, 2 degrees
          \_SB.PC00.LPCB.H_EC.ECWT(Local1, RefOf(\_SB.PC00.LPCB.H_EC.TSHT)) // Sensor High Trip Point
          \_SB.PC00.LPCB.H_EC.ECMD(0x4A) // Set Trip point.
          Release(\_SB.PC00.LPCB.H_EC.PATM)
        }
      }
    }

    // Thermal Sensor Hysteresis, 2 degrees
    Name(GTSH, 20)

    Name(LSTM,0)  // Last temperature reported

    // _DTI (Device Temperature Indication)
    //
    // Conveys the temperature of a device's internal temperature sensor to the platform when a temperature trip point
    // is crossed or when a meaningful temperature change occurs.
    //
    // Arguments: (1)
    //   Arg0 - An Integer containing the current value of the temperature sensor (in tenths Kelvin)
    // Return Value:
    //   None
    //
    Method(_DTI, 1)
    {
      Store(Arg0,LSTM)
      Notify(\_SB.PC00.LPCB.H_EC.SEN4, 0x91) // notify the participant of a trip point change event
    }

    // _NTT (Notification Temperature Threshold)
    //
    // Returns the temperature change threshold for devices containing native temperature sensors to cause
    // evaluation of the _DTI object
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the temperature threshold in tenths of degrees Kelvin.
    //
    Method(_NTT, 0)
    {
      Return(2782)  // 5 degree Celcius, this could be a platform policy with setup item
    }

   // Default values for trip points for the validation purpose.
    Name (S4AC,60)  // Active0 trip point in celsius
    Name (S4A1,50)  // Active1 trip point in celsius
    Name (S4A2,40)  // Active2 trip point in celsius
    Name (S4PV,65)  // Passive trip point in celsius
    Name (S4CC,80)  // Critical trip point in celsius
    Name (S4C3,70)  // Critical s3 trip point in celsius
    Name (S4HP,75)  // Hot trip point in celsius
    Name (SSP4,0)   // Thermal Sampling Period

    // _TSP (Thermal Sampling Period)
    //
    // Sets the polling interval in 10ths of seconds. A value of 0 tells the driver to use interrupts.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the polling rate in tenths of seconds.
    //   A value of 0 will specify using interrupts through the ACPI notifications.
    //
    //   The granularity of the sampling period is 0.1 seconds. For example, if the sampling period is 30.0
    //   seconds, then _TSP needs to report 300; if the sampling period is 0.5 seconds, then it will report 5.
    //
    Method(_TSP,0,Serialized)
    {
      Return(SSP4)
    }

    // _ACx (Active Cooling)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant and then export the value to BIOS GDDV for storage.
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC0,0,Serialized)
    {
      Store(\_SB.IETM.CTOK(S4AC),Local1) // Active Cooling Policy
      If(LGreaterEqual(LSTM,Local1))
      {
        Return(Subtract(Local1,20)) // subtract 2 degrees which is the Hysteresis
      }
      Else
      {
        Return(Local1)
      }
    }

    // _ACx (Active Cooling)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant and then export the value to BIOS GDDV for storage.
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC1,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(S4A1))
    }

    // _ACx (Active Cooling)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant and then export the value to BIOS GDDV for storage.
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the active cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_AC2,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(S4A2))
    }

    // _PSV (Passive)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant and then export the value to BIOS GDDV for storage.
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the passive cooling temperature threshold in tenths of degrees Kelvin
    //
    Method(_PSV,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(S4PV)) // Passive Cooling Policy
    }

    // _CRT (Critical Temperature)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant and then export the value to BIOS GDDV for storage.
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CRT,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(S4CC))
    }

    // _CR3 (Critical Temperature for S3/CS)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant and then export the value to BIOS GDDV for storage.
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    An Integer containing the critical temperature threshold in tenths of degrees Kelvin
    //
    Method(_CR3,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(S4C3))
    }

    // _HOT (Hot Temperature)
    //
    // This ACPI method is optional.
    // OEM/ODM can directly set the value on Intel(R) Dynamic Tuning Technology UI for the participant and then export the value to BIOS GDDV for storage.
    // The value programmed here is the default value used on Intel RVP for the validation purpose.
    // OEM/ODM can set a different default value for a specific platform if necessary.
    //
    //  Arguments: (0)
    //    None
    //  Return Value:
    //    The return value is an integer that represents the critical sleep threshold tenths of degrees Kelvin.
    //
    Method(_HOT,0,Serialized)
    {
      Return(\_SB.IETM.CTOK(S4HP))
    }

  } // End SEN4 Device
}// end Scope(\_SB.PC00.LPCB.H_EC)
