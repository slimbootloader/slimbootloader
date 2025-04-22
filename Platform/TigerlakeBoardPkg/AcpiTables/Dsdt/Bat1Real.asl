/** @file
  ACPI DSDT table

  Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


  // Define the Real Battery 1 Control Method.

  Device(BAT1) {
    Name(_HID,EISAID("PNP0C0A"))
    Method(_UID,0)
    {
      Return (RBY1)
    }

    Method (_STA,0) {
      If (And(BATP, BIT0)) {  // Battery is supported.
        If (And(\_SB.PC00.LPCB.H_EC.BNUM, BIT0)) { // Real Battery 1 present?
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

      If (ECAV) {
        If (LAnd(LAnd(ECRD(RefOf(B1DV)),ECRD(RefOf(B1FC))),ECRD(RefOf(B1DC)))) {
          // Convert mAh and mV to mWh
          Store(Divide(Multiply(ECRD(RefOf(B1DC)), ECRD(RefOf(B1DV))),1000), Index(BPK1,1))
          Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),1000), Index(BPK1,2))
          Store(B1DV, Index(BPK1,4))
          Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),10000), Index(BPK1,5))
          Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),25000), Index(BPK1,6))
          Store(0x100, Index(BPK1,7))
          Store(0x40,  Index(BPK1,8))
        }
      } // If(ECAV)
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
         0x00000001 // Battery Swapping Capability = Cold swappable
      })

      If (ECAV) {
        If (LAnd(LAnd(ECRD(RefOf(B1DV)),ECRD(RefOf(B1FC))),ECRD(RefOf(B1DC)))) {
          // Convert mAh and mV to mWh
          Store(Divide(Multiply(ECRD(RefOf(B1DC)), ECRD(RefOf(B1DV))),1000), Index(BPK1,2))
          Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),1000), Index(BPK1,3))
          Store(ECRD(RefOf(B1DV)), Index(BPK1,5))
          Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),10000), Index(BPK1,6))
          Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),25000), Index(BPK1,7))
          Store(0x40,   Index(BPK1,9))
          Store(0x320,  Index(BPK1,11))
          Store(0x251C, Index(BPK1,10))
          Store(ECRD(RefOf(BICC)), Index(BPK1,8)) // Battery cycle count
        }
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

      If (ECAV) {
        // Fix up the Battery Status.
        Store(And(ECRD(RefOf(B1ST)), 0x07),Index(PKG1,0))
        If(And(ECRD(RefOf(B1ST)), 0x01)) {
          // Calculate discharge rate
          // Return Rate in mW since we report _BIF data in mW
          Store(Multiply(ECRD(RefOf(B1DI)), ECRD(RefOf(B1FV))), Local0)
          Store(Divide(Local0, 1000), Local0)
          Store(Local0, Index(PKG1,1))
        } Else {
          // Calculate charge rate
          // Return Rate in mW since we report _BIF data in mW
          Store(Multiply(ECRD(RefOf(B1CI)), ECRD(RefOf(B1FV))), Local0)
          Store(Divide(Local0, 1000), Local0)
          Store(Local0, Index(PKG1,1))
        }
        // Calculate Remaining Capacity in mWh =
        // (Remaininng Capacity (mAh) * Design Voltage (mV))/1000
        // Use Remaininng Capacity in mAh multiply with a fixed Design Voltage
        // for Remaininng Capacity in mWh
        Store(Divide(Multiply(ECRD(RefOf(B1RC)), ECRD(RefOf(B1DV))),1000),Index(PKG1,2))
        // Report Battery Present Voltage (mV)
        Store(ECRD(RefOf(B1FV)), Index(PKG1,3))
      } // If(ECAV)
      Return(PKG1)
    }

    Method(_BLT,3) {
      If (ECAV) {
        // arg2 = Battery wake level in mWh, sent to EC as Threshold.
        // transfer input value from mWh to %
        If(LAnd(LNotEqual(ECRD(RefOf(B1FC)),0),LNotEqual(ECRD(RefOf(B1FV)),0))) {
          Store(Divide(Multiply(Arg2, 100), Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1FV))),1000)),Local0)
          // adjust offset between OS & EC
          Add(Local0,1,Local0)
          // store TP value in EC name space offset 219
          ECWT(Local0, RefOf(BTP1))
          // Send EC Battery Threshold Update Command - 0x34
          ECMD(0x34)
        }
      }
    }

//@todo Need to revisit this code once we have made sure EC FW has ported the changes from BDW base
/*
    Method(_BTP,1) {
      If (ECAV) {
        If(LAnd(LNotEqual(ECRD(RefOf(B1FC)),0),LNotEqual(ECRD(RefOf(B1FV)),0))) {
          // Arg0 - Trip point in mWh , convert it into mAh
          Store(Divide(Multiply(Arg0,1000),ECRD(RefOf(B1DV))), Local0) // Local0- trip point in mAh
          Store(Local0,Local1)
          // store converted trip point in mAh lower byte to EC offset 220 and higher byte to offset 221
          ECWT(And(Local0,0x00FF,Local0),RefOf(B1TL)) // store lower byte of BTP in offset 220
          And(Local1,0xFF00,Local1)
          ECWT(ShiftRight(Local1,8,Local1),RefOf(B1TH)) // store higher byte of BTP in offset 221
        }
      } // If(ECAV)
      Return()
    }
*/
    // Return that everything runs off Battery.

    Method(_PCL,0) {
      Return (
        Package() { _SB }
      )
    }
  }
