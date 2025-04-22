/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


Scope (\_SB)
{
  // Define the Real Battery 1 Control Method.
  Device(BAT1) {
    Name(_HID,EISAID("PNP0C0A"))
    Name (_DEP, Package() {\_SB.ECLT})
    Method(_UID,0)
    {
      Return (RBY1)
    }

    Method (_STA,0) {
      If (And(BATP, BIT0))
      {  // Battery is supported.
        If (And(\_SB.ECLT.BNUM, BIT0)) // Real Battery 1 present?
        {
          Return (0x001F)  // Yes.  Show it.
        } Else {
          Return(0x000B)    // No.  Hide it.
        }
      }
      Return (0)
    }

    Method(_BIF,0, Serialized) {
      Name(BPK1, Package() {
        0x00000000, // Power Unit = mWh.
        0xFFFFFFFF, // Unknown Design Capacity.
        0xFFFFFFFF, // Unknown Last Full Charge.
        0x00000001, // Secondary Battery Technology.
        0xFFFFFFFF, // Unknown Design Voltage.
        0x00000000, // 10% Warning Level.
        0x00000000, // 4% Low Level.
        0x00000100, // 1% Granularity Level 1.
        0x00000040, // 1% Granularity Level 2.
        "BASE-BAT",
        "123456789",
        "LiP",
        "Simplo"
      })
      Store(0, Local0)
      Store(0, Local1)
      Store(0, Local2)

      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1DC_OFFSET, ECLITE_BYTES_COUNT_6)
      Store(  \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1DC) ), Local0 )
      Store(  \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1FC) ), Local1 )
      Store(  \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1DV) ), Local2 )

      If (LAnd(LAnd(Local2,Local1),Local0)) {
        // Convert mAh and mV to mWh
        Store(Divide(Multiply(Local0, Local2),1000), Index(BPK1,1))
        Store(Divide(Multiply(Local1, Local2),1000), Index(BPK1,2))
        Store(Local2, Index(BPK1,4))
        Store(Divide(Multiply(Local1, Local2),10000), Index(BPK1,5))
        Store(Divide(Multiply(Local1, Local2),25000), Index(BPK1,6))
        Store(0x100, Index(BPK1,7))
        Store(0x40,  Index(BPK1,8))
      }
      Return (BPK1)
    }

    Method(_BIX,0, Serialized) {
      Name(BPK1, Package() {
        0x01,       // Integer Revision
        0x00000000, // Power Unit = mWh. 0x0 indicates units are mW/mWh
        0xFFFFFFFF, // Unknown Design Capacity.
        0xFFFFFFFF, // Unknown Last Full Charge.
        0x00000001, // Secondary Battery Technology.
        0xFFFFFFFF, // Unknown Design Voltage.
        0x00000000, // 10% Warning Level.
        0x00000000, // 4% Low Level.
               100, // Cycle Count, MUST not be 0xFFFFFFFF
             95000, // Measurement Accuracy 95%
        0x00000000, // Max sampling time, MSFT No specific requirement.
        0x00000000, // Min sampling time, MSFT No specific requirement.
        0x00000000, // Max averaging interval, MSFT No specific requirement.
        0x00000000, // Min averaging interval, MSFT No specific requirement.
        0x00000100, // Battery capacity granularity 1
        0x00000040, // Battery capacity granularity 2
        "BASE-BAT", // Model number
        "123456789",//Serial Number
        "LiP",      // Battery type
        "Simplo",   // OEM information
         0x00000000 // Non swappable battery (sealed internal battery not accessible to user)
      })
      Store(0, Local0)
      Store(0, Local1)
      Store(0, Local2)
      Store(0, Local3)

      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1DC_OFFSET, ECLITE_BYTES_COUNT_6)
      Store(  \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1DC) ), Local0 )
      Store(  \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1FC) ), Local1 )
      Store(  \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1DV) ), Local2 )
      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1CC_OFFSET, ECLITE_BYTES_COUNT_2)
      Store(  \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1CC) ), Local3 )

      If (LAnd(LAnd(Local2,Local1),Local0)) {
        // Convert mAh and mV to mWh
        Store(Divide(Multiply(Local0, Local2),1000), Index(BPK1,2))
        Store(Divide(Multiply(Local1, Local2),1000), Index(BPK1,3))
        Store(Local2, Index(BPK1,5))
        Store(Divide(Multiply(Local1, Local2),10000), Index(BPK1,6))
        Store(Divide(Multiply(Local1, Local2),25000), Index(BPK1,7))
        Store(0x40,   Index(BPK1,9))
        Store(0x320,  Index(BPK1,11))
        Store(0x251C, Index(BPK1,10))
        Store(Local3, Index(BPK1,8)) // Battery cycle count
      }
      Return (BPK1)
    }

    Method(_BST,0, Serialized) {
      Name(PKG1,Package() {
        0xFFFFFFFF, // Battery State.
        0xFFFFFFFF, // Battery Present Rate. (in mWh)
        0xFFFFFFFF, // Battery Remaining Capacity. (in mWh)
        0xFFFFFFFF  // Battery Present Voltage. (in mV)
      })

      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1ST_OFFSET, ECLITE_BYTES_COUNT_8)
      // Battery State.
      Store(And(\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1ST)),0x07), Index(PKG1,0))
      // Battery Present Rate. (in mWh)
      Store(Divide(Multiply(\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1PR)), \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1PV))), 1000), Index(PKG1,1))
      // Remaining Capacity in mWh
      Store(Divide(Multiply(\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1RC)), \_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1DV))), 1000), Index(PKG1,2))
      // Report Battery Present Voltage (mV)
      Store(\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1PV)), Index(PKG1,3))
      Return(PKG1)
    }

    Method(_BLT,3) {
      //
      // Arg0 BatteryWarningLevel and Arg1 BatteryLowLevel in ISH FW with OS Provided Values
      //
      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1PV_OFFSET, ECLITE_BYTES_COUNT_2)
      If(LNotEqual(\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1PV) ), 0 ))
      {
        Store(Divide(Multiply(Arg0, 1000),\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1PV ))),Local0)
        \_SB.ECLT.ECLW(Local0, RefOf(\_SB.ECLT.B1WL ))
        \_SB.ECLT.ECLC(ECLITE_WRITE_COMMAND, ECLITE_WARNING_BATTERY_LEVEL_UPDATE, ECLITE_B1WL_OFFSET, ECLITE_BYTES_COUNT_2)

        Store(Divide(Multiply(Arg1, 1000),\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1PV ))),Local0)
        \_SB.ECLT.ECLW(Local0, RefOf(\_SB.ECLT.B1LL ))
        \_SB.ECLT.ECLC(ECLITE_WRITE_COMMAND, ECLITE_LOW_BATTERY_LEVEL_UPDATE, ECLITE_B1LL_OFFSET, ECLITE_BYTES_COUNT_2)
     }
    }

    Method(_BTP,1) {
      //
      // Program Battery Trip Point
      //
      \_SB.ECLT.ECLC(ECLITE_READ_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1DV_OFFSET, ECLITE_BYTES_COUNT_2)
      If(LNotEqual(\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1DV) ), 0 ))
      {
        Store(Divide(Multiply(Arg0, 1000),\_SB.ECLT.ECLR( RefOf (\_SB.ECLT.B1DV ))),Local0)
        Store(Local0,Local1)
        \_SB.ECLT.ECLW(And(Local0,0xFFFF,Local0), RefOf(\_SB.ECLT.B1T0 ))
        \_SB.ECLT.ECLC(ECLITE_WRITE_COMMAND, ECLITE_DEFAULT_UPDATE, ECLITE_B1T0_OFFSET, ECLITE_BYTES_COUNT_2)
        \_SB.ECLT.ECLW(ShiftRight(Local1,16,Local1), RefOf(\_SB.ECLT.B1T1 ))
        \_SB.ECLT.ECLC(ECLITE_WRITE_COMMAND, ECLITE_BATTERY_TRIP_POINT_UPDATE, ECLITE_B1T1_OFFSET, ECLITE_BYTES_COUNT_2)
      }
      Return()
    }
    // Return that everything runs off Battery.

    Method(_PCL,0) {
      Return (
        Package() { _SB }
      )
    }
  }
} // End of Scope (\_SB)
