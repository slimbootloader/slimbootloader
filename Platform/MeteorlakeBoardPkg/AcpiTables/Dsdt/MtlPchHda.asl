/**@file
  Integrated HDA MTL PCH controller ACPI methods

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

//
// High Definition Audio Controller - Device 31, Function 3
//

External (\_SB.PC02.HDAS.PS0X, MethodObj)
External (\_SB.PC02.HDAS.PS3X, MethodObj)
External (\_SB.PC02.HDAS.PPMS, MethodObj)

//
// High Definition Audio Controller - Device 31, Function 3
//
Device (HDAS)
{
  Name (_ADR, 0x001F0003)

  // D0 Method for HD-A Controller
  Method (_PS0, 0, Serialized)
  {
    \_SB.S023 (0, 1)
    //
    // Call platform HDAS PS0 method if present
    //
    If (CondRefOf (\_SB.PC02.HDAS.PS0X)) {
      \_SB.PC02.HDAS.PS0X ()
    }
  }

  // D3 Method for HD-A Controller
  Method (_PS3, 0, Serialized)
  {
    \_SB.S023 (0, 0)
    //
    // Call platform HDAS PS3 method if present
    //
    If (CondRefOf (\_SB.PC02.HDAS.PS3X)) {
      \_SB.PC02.HDAS.PS3X ()
    }
  }

  include ("PchHda.asl")
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
} // END Device (HDAS)