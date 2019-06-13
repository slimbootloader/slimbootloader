/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define BIT7    0x00000080

Name(CTYP, 0) // Cooling Type Flag: 0 - Active cooling / 1 - Passive cooling
Name(ACTT,63) // Active Trip Point
Name(CRTT,125) // Critical Trip Point
Name(PSVT,111) // Passive Trip Point
Name(TC1V,1) // Passive Trip Point TC1 Value
Name(TSPV,10) // Passive Trip Point TSP Value
Name(TC2V,5) // Passive Trip Point TC2 Value

Scope(\_TZ)
{
  //
  // Memory window to MCHBAR+7000h.
  //
  OperationRegion (MBAR, SystemMemory, Add(ShiftLeft(\_SB.PCI0.VLVC.MHBR,15),0x7000), 0x100)
  Field (MBAR, ByteAcc, NoLock, Preserve)
  {
    Offset (0x1C), // P_CR_PKG_THERM_STATUS_0_0_0_MCHBAR (MCHBAR+0x701C)
    , 1,// THRESHOLD1_STATUS[0]
    , 1,// THRESHOLD1_LOG[1]
    , 1,// THRESHOLD2_STATUS[2]
    , 1,// THRESHOLD2_LOG[3]
    , 1,// CRITICAL_THERMAL_EVENT_STATUS[4]
    , 1,// CRITICAL_THERMAL_EVENT_LOG[5]
    , 10,// RESERVED[15:6]
    PKGT, 8,// TEMPERATURE[23:16]
  }

  // Notes:
  //  1) Temperature values are stored in tenths of Kelvin to eliminate the decimal place.
  //  2) Kelvin = Celsius + 273.2.
  //  3) All temperature must be >= 289K.

  // Flag to indicate Legacy thermal control.
  // 1 = Legacy thermal control
  // 0 = other thermal control, like DPTF
  Name(LEGA, 1) // for Active Policy
  Name(LEGP, 1) // for Passive Policy
  Name(LEGC, 1) // for Critical Policy

  // Fan 0 = Package Processor Fan.

  PowerResource(FN00,0,0)
  {

    // Return Virtual Fan 0 status.
    //
    Method(_STA,0,Serialized)
    {
      Return(0x0F)
    }

    Name(FANS,0) // Fan State: 1=ON, 0=OFF

    Method(_ON,0,Serialized)
    {
      // If EC access is enabled.
      If(\_TZ.LEGA)
      {
        Store(1,FANS) // save fan state as on
      }
    }

    Method(_OFF,0,Serialized)
    {
      // If EC access is enabled.
      If(\_TZ.LEGA)
      {
        Store(0,FANS) // save fan state as off
      }
    }
  }

  // Associate Virtual Fan 0 Power Resource with the FAN0 Device.
  //
  Device(FAN0)
  {
    Name(_HID, EISAID("PNP0C0B"))
    Name(_UID,0)
    Name(_PR0, Package(1){FN00})
  }

  // Thermal Zone 1 = DTS Thermal Zone.
  //
  ThermalZone(TZ01)
  {
    // Return the temperature at which the OS initiates Active Cooling.
    //
    Method(_AC0,0,Serialized)
    {
      If (LEqual(\_TZ.LEGA,0)){ Return(Add(2732,Multiply(210,10)))} // 210 degree C
      Return(Add(2732,Multiply(ACTT,10)))
    }

    // Returns the Device to turn on when _ACx is exceeded.
    //
    Name(_AL0, Package(1){FAN0})

    // Return the temperature at which the OS must perform a Critcal Shutdown.
    //
    Method(_CRT,0,Serialized)
    {
      If (LEqual(\_TZ.LEGC,0)){ Return(Add(2732,Multiply(210,10)))} // 210 degree C
      Return(Add(2732,Multiply(\CRTT,10)))
    }

    // Notifies ASL Code the current cooling mode.
    //  0 - Active cooling
    //  1 - Passive cooling
    //
    Method(_SCP,1,Serialized)
    {
      Store(Arg0,\CTYP)
    }

    // _TMP (Temperature)
    //
    // Return the highest of the CPU temperatures to the OS.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing the current temperature of the thermal zone (in tenths of degrees Kelvin)
    //
    Method(_TMP,0,Serialized)
    {

      Store (PKGT, Local0) //Read temperature from the Package Thermal Status register(0x701C)

      // Data format is signed, 2's complement with the LSB representing 1'C resolution
      If (And (Local0, BIT7)) //MSB is set: Negative number!
      {
        Add (And (Not (Local0), 0xFF), 1, Local0) //Invert and add 1 for 2's complement (AND with 0xFF to prevent extension)
        Return (Subtract (2732, Multiply (Local0, 10)))
      } Else {
        Return (Add (2732, Multiply (Local0, 10)))
      }
    } // End of _TMP

    // Return the Processor(s) used for Passive Cooling.
    //
    Method(_PSL,0,Serialized)
    {
      If(LEqual(MPEN, 4))
      {
        //  CMP - Throttling controls all four logical CPUs.
        Return(Package(){\_PR.CPU0,\_PR.CPU1,\_PR.CPU2,\_PR.CPU3})
      }
      If(MPEN)
      {
        //  CMP - Throttling controls both CPUs.
        Return(Package(){\_PR.CPU0,\_PR.CPU1})
      }
      Return(Package(){\_PR.CPU0})
    }

    // Returns the temperature at which the OS initiates CPU throttling.
    //
    Method(_PSV,0,Serialized)
    {
      If (LEqual(\_TZ.LEGP,0)){ Return(Add(2732,Multiply(110,10)))} // 110 degree C
      Return(Add(2732,Multiply(\PSVT,10)))
    }

    // Returns TC1 value used in the passive cooling formula.
    //
    Method(_TC1,0,Serialized)
    {
      Return(\TC1V)
    }

    // Returns TC2 value used in the passive cooling formula.
    //
    Method(_TC2,0,Serialized)
    {
      Return(\TC2V)
    }

    // Returns the sampling period used in the passive cooling formula.
    //
    Method(_TSP,0,Serialized)
    {
      Return(\TSPV)
    }

  }// end ThermalZone(TZ01)
} // end Scope(\_TZ)
