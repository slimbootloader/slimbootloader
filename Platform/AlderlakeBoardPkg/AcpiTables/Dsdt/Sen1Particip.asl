/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


Device(SEN1)
{
  Name (_HID, "INTC1046")  // Intel(R) Dynamic Tuning Technology Temperature Sensor Device
  Name (_UID, "SEN1")
  Name (_STR, Unicode ("Thermistor PCH VR"))

  Method (_STA, 0, Serialized)
  {
    // Only report resources to the OS if the SIO Device is present.

    //
    // Steps: 1) If board does not support IT8659E return 0x0000 (no device).
    //        2) If bit set (IT8659E present) aquire mutex and set the SIO
    //           to COM A.
    //        3) Read enable register to Local0.
    //        4) Release mutex and exit config mode.
    //        5) If Local0 set return 0x000f (device present and active).
    //        6) Else return 0x000d (device present, inactive).
    //
    If (LEqual (S1DE, 1)) {
      ENFG (0x04)
      Store (IT30, Local0)
      EXFG ()
      If (Local0) {
        Return (0x000F)
      }
      Return (0x000D)
    }

    Return (0x0000)
  }

  // ITRD (SIO Read Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (1)
  //    Arg0 - Object to Read
  //  Return Value:
  //    Read Value
  //
  Method(ITRD,1,Serialized, 0, IntObj, FieldUnitObj)
  {
    Store(Arg0, Local1) // Execute Read from EC
    Return(Local1)
  }

  // ICWT (SIO Write Method)
  //
  // Handle all commands sent to EC by BIOS
  //
  //  Arguments: (2)
  //    Arg0 - Value to Write
  //    Arg1 - Object to Write to
  //
  Method(ICWT,2,Serialized,,,{IntObj, FieldUnitObj})
  {
    Store(Arg0,Arg1) // Execute Write to SIO
  }

  // KTOC (Kelvin to Celsius)
  //
  // This control method converts from 10ths of degree Kelvin to Celsius.
  //
  // Arguments: (1)
  //   Arg0 - Temperature in 10ths of degree Kelvin
  // Return Value:
  //   Temperature in Celsius
  //
  Method(KTOC,1,Serialized)
  {
    If(LGreater(Arg0,2732)) // make sure we have a temperature above zero Celcius
    {
      Return(Divide(Subtract(Arg0, 2732), 10))
    } Else {
      Return(0) // negative temperatures Celcius are changed to 0 degrees Celcius
    }
  }

  // CTOK (Celsius to Kelvin)
  //
  // This control method converts from Celsius to 10ths of degree Kelvin.
  //
  // Arguments: (1)
  //   Arg0 - Temperature in Celsius
  // Return Value:
  //   Temperature in 10ths of degree Kelvin
  //
  Method(CTOK,1,Serialized)
  {
    Return(Add(Multiply(Arg0,10),2732))
  }

  // C10K (Celsius to Kelvin)
  //
  // This control method converts from 10ths of degree Celsius (16 bit signed) to 10ths of degree Kelvin.
  // This method works only for temperatures upto absolute 0 Kelvin.
  //
  // Arguments: (1)
  //   Arg0 - Temperature in 10ths of degree Celsius (16 bit signed value)
  // Return Value:
  //   Temperature in 10ths of degree Kelvin
  //
  Method(C10K,1,Serialized)
  {
    Name(TMP1, Buffer(16) {0})
    CreateByteField(TMP1, 0x00, TMPL)
    CreateByteField(TMP1, 0x01, TMPH)

    Add(Arg0, 2732, Local0)
    Store(And(Local0,0xFF),TMPL)
    Store(ShiftRight(And(Local0,0xFF00),0x8),TMPH)

    ToInteger(TMP1, Local1)
    Return(Local1)
  }

  // K10C (Kelvin to Celsius)
  //
  // This control method converts from 10ths of degree Kelvin to 10ths of degree Celsius (16 bit signed).
  // This method works only for temperatures upto 0 degree celsius. Temperatures below 0 deg celsius are changed to 0 deg C.
  //
  // Arguments: (1)
  //   Arg0 - Temperature in 10ths of degree Kelvin
  // Return Value:
  //   Temperature in 10ths of degree Celsius (16 bit signed value)
  //
  Method(K10C,1,Serialized)
  {
    If(LGreater(Arg0,2732)) // make sure we have a temperature above zero Celcius
    {
      Return(Subtract(Arg0, 2732))
    } Else {
      Return(0) // negative temperatures Celcius are changed to 0 degrees Celcius
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
    Store(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(\_SB.PC00.LPCB.ITE8.TFN1.R29R), Local1)
    Return(Local1)
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
  Method(PAT0,1,Serialized)
  {
    Store(\_SB.PC00.LPCB.ITE8.SEN1.K10C(Arg0),Local1)
    \_SB.PC00.LPCB.ITE8.SEN1.ICWT(Local1, \_SB.PC00.LPCB.ITE8.TFN1.R41R) // Set low limit trip point
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
  Method(PAT1,1,Serialized)
  {
    Store(\_SB.PC00.LPCB.ITE8.SEN1.K10C(Arg0),Local1)
    \_SB.PC00.LPCB.ITE8.SEN1.ICWT(Local1, \_SB.PC00.LPCB.ITE8.TFN1.R40R) // Set high limit trip point
  }

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
    Notify(\_SB.PC00.LPCB.ITE8.SEN1, 0x91) // notify the participant of a trip point change event
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
  Name (S1AC,60)  // Active0 trip point in celsius
  Name (S1A1,50)  // Active1 trip point in celsius
  Name (S1A2,40)  // Active2 trip point in celsius
  Name (S1PV,65)  // Passive trip point in celsius
  Name (S1CC,80)  // Critical trip point in celsius
  Name (S1C3,70)  // Critical s3 trip point in celsius
  Name (S1HP,75)  // Hot trip point in celsius
  Name (SSP1,0)   // Thermal Sampling Period

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
    Return(SSP1)
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
    Store(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(S1AC),Local1) // Active Cooling Policy
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
    Return(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(S1A1))
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
    Return(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(S1A2))
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
    Return(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(S1PV)) // Passive Cooling Policy
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
    Return(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(S1CC))
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
    Return(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(S1C3))
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
    Return(\_SB.PC00.LPCB.ITE8.SEN1.CTOK(S1HP))
  }

} // End SEN1 Device
