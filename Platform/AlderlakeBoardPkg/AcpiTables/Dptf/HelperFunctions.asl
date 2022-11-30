/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.IETM)
{
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
}
