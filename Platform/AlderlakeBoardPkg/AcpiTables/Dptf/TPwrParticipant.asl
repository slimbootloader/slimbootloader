/** @file
  Intel ACPI Reference Code for Intel(R) Dynamic Tuning Technology

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

External (\_SB.PC00.LPCB.H_EC.PBOK, FieldUnitObj)

Scope(\_SB)
{

  Device(TPWR)  // Power participant
  {
    Name(_UID, "TPWR")
    Method(_HID)
    {
      Return(\_SB.IETM.GHID(_UID))  //Intel(R) Dynamic Tuning Technology platform power device
    }
    Name(_STR, Unicode ("Platform Power"))
    Name(PTYP, 0x11)
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
      If(LEqual(\PWRE,1)) {
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // PSOC (Platform State of Charge)
    //
    // This object evaluates to the remaining battery state of charge in %.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   remaining battery charge in %
    //
    Method(PSOC)
    {
      If(LEqual(\_SB.PC00.LPCB.H_EC.ECAV,0)) // check EC opregion available
      {
        Return (0) // EC not available
      }
      If(LEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1FC)),0)) // prevent divide by zero
      {
        Return (0) // Full charge cannot be 0, indicate malfunction in battery
      }
      If(LGreater(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1RC)),\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1FC)))) // remaining charge > full charge
      {
        Return(0) // Remaining Charge cannot be greater than Full Charge, this illegal value indicates the battery is malfunctioning
      }
      If(LEqual(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1RC)),\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1FC)))) // remaining charge == full charge
      {
        Return(100) // indicate battery is fully charged
      }
      If(LLess(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1RC)),\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1FC)))) // (RC*100)/FC=percent
      {
        Multiply(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1RC)),100,Local0) // RC*100
        Divide(Local0,\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1FC)),Local2,Local1) // divide by FC
        Divide(Local2,100,,Local2) // account for the remainder in percent
        Divide(\_SB.PC00.LPCB.H_EC.ECRD(RefOf(\_SB.PC00.LPCB.H_EC.B1FC)),200,,Local3) // what number is 0.5%?
        If(LGreaterEqual(Local2,Local3)){ // round up if the remainder is equal or above .5
          Add(Local1,1,Local1)
        }
        Return(Local1) // current charge in percent
      } Else {
        Return (0) // avoid compiler warning
      }
    }

    // PSRC (Power SouRCe)
    //
    // The PSRC object provides power source type.
    //
    // Arguments: (0)
    //   None
    // Return Value:(enumeration which Intel(R) Dynamic Tuning Technology driver expects)
    //   Bit[3:0] = Power Resource Type
    //   0x00 = DC
    //   0x01 = AC
    //   0x02 = USB
    //   Bit[7:4] = Power Delivery State Change Sequence Number
    //
    // Notes: (Bitmap from EC)
    // PwrSrcType Bit[0] = 0 <DC>, Bit[0] = 1 <AC>, Bit[1] = 1 <USB-PD>, Bit[2] = 1 <Wireless Charging>
    // Bit[7:4]  = Power Delivery State Change Sequence Number

    Method (PSRC, 0, Serialized)
    {
     If (LEqual(\_SB.PC00.LPCB.H_EC.ECAV,0))
      {
        Return(0) // EC not available, default to battery
      } Else {
        Store (\_SB.PC00.LPCB.H_EC.ECRD (RefOf(\_SB.PC00.LPCB.H_EC.PWRT)),Local0)
        And (Local0, 0xF0, Local1) // Keep Bit[7:4], clear Bit[3:0] in Local1
      }

        Switch (ToInteger(And(ToInteger(Local0),0x07))) // Switch Power Source Type Bits [3:0]
        {
          Case (0x0) { // Power source is DC
            Or (Local1, 0x00, Local1)
          }
          Case (0x1) { // Power source is AC
            Or (Local1, 0x01, Local1)
          }
          Case (0x2) { // Power source is USB-PD
            Or (Local1, 0x02, Local1)
          }
          Default {
            Or (Local1, 0x00, Local1) //default to battery
          }
        } // end of switch case check
      Return(Local1)
    }

    // ARTG (Adapter RaTinG)
    //
    // The ARTG object provides AC adapter rating in mW.
    // ARTG should return 0 if PSRC is DC (0).
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   AC adapter rating in mW
    //
    Method(ARTG)
    {
      If(LEqual((And(PSRC(),0x07)),1)) // Check Power Source Type Bits [3:0]
      {
        If(LEqual(\_SB.PC00.LPCB.H_EC.ECAV,1)) // Check If EC opregion is available
        {
          Multiply(\_SB.PC00.LPCB.H_EC.ARTG,10,Local0) // Becasue of EC-space constraint EC can only send 2bytes=15000mW but system uses 150000mW, hence bios is multiplying this value by 10.
          Return(Local0)
        } Else {
          Return(90000) // 90 watts
        }
      } Else {
        Return(0)
      }
    }

    // PROP (Platform Rest Of worst case Power)
    //
    // This object provides maximum worst case platform rest of power.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in milliwatts
    //
    Method(PROP)
    {
      If(LEqual(\_SB.PC00.LPCB.H_EC.ECAV,1)) // Check If EC opregion is available
      {
        Multiply(\_SB.PC00.LPCB.H_EC.PROP,1000,Local0) // EC sending value in Watts, converting it to mW
        Return(Local0)
      } Else {
       Return(25000) // 25 watts
      }
    }

    // PBOK
    //
    // PBOK is a command which BIOS sends to EC.
    // It will have a parameter which is the sequence number.
    //
    // Arguments: (1)
    //   Arg0 should be sent by Intel(R) Dynamic Tuning Technology driver which will be corresponding to Power delivery state change sequence number.
    //   Bit[3:0] = Power Delivery State Change Sequence number
    // Return Value:
    //   None
    //
    Method(PBOK,1,Serialized)
    {
      If (LEqual(\_SB.PC00.LPCB.H_EC.ECAV,1)) // Check If EC opregion is available
      {
        Store (And(Arg0, 0x000F), Local0)
        \_SB.PC00.LPCB.H_EC.ECWT (Local0, RefOf(\_SB.PC00.LPCB.H_EC.PBOK)) // Store sequence number
        \_SB.PC00.LPCB.H_EC.ECMD (0x15)
      }
    }

  } // End TPWR Device
}// end Scope(\_SB)
