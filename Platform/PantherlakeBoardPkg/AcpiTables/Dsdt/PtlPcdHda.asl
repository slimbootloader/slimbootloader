/**@file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#define HDAS_PATH                      \_SB.PC00.HDAS
//
// High Definition Audio Controller - Device 31, Function 3
//
Device (HDAS)
{
  Name (_ADR, 0x001F0003)

  // D0 Method for HD-A Controller
  Method (_PS0, 0, Serialized)
  {
    ADBG ("HD-A Ctrlr D0")

    \_SB.S023 (0, 1)
    //
    // Call platform HDAS PS0 method if present
    //
    If (CondRefOf (\_SB.PC00.HDAS.PS0X)) {
      \_SB.PC00.HDAS.PS0X ()
    }
  }

  // D3 Method for HD-A Controller
  Method (_PS3, 0, Serialized)
  {
    ADBG ("HD-A Ctrlr D3")

    \_SB.S023 (0, 0)
    //
    // Call platform HDAS PS3 method if present
    //
    If (CondRefOf (\_SB.PC00.HDAS.PS3X)) {
      \_SB.PC00.HDAS.PS3X ()
    }
  }

  Method (_PRW, 0) {
    If (CondRefOf (GP1E)){
      If (LNotEqual (GP1E, 0)) {
        Return (GPRW (GPE1_ACE_PME_B0, 4))
      }
    }
    Return (GPRW (GPE_PME_B0, 4)) // can wakeup from S4 state
  }

  include ("Hda.asl")
  include ("HdaIda.asl")
}

Scope (HDAS.IDA) {
  //
  // High Definition Audio - SoundWire Controller
  //
  include ("HdaSoundWireCtrl.asl")

  If (LNotEqual (UAOE,0)) {
    //
    // High Definition Audio - USB Audio Offload
    //
    include ("HdaUsbAudioOffload.asl")
  }

  Device (ISSW) {
    Name (_ADR, 0x70000000)  // _ADR: Address

    Name (_CID, Package () { // _CID: Compatible ID
      "PRP00001",
      "PNP0A05" /* Generic Container Device */
    })

    Method (_STA, 0, NotSerialized) { // _STA: Status
      Return (0x0F)
    }
  }

  Device (ISBL) {
    Name (_ADR, 0x70000010)  // _ADR: Address

    Name (_CID, Package () { // _CID: Compatible ID
      "PRP00001",
      "PNP0A05" /* Generic Container Device */
    })

    Method (_STA, 0, NotSerialized) { // _STA: Status
      Return (0x0F)
    }
  }
} // END Device (HDAS)

#undef HDAS_PATH
