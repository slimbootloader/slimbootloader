/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// HD Audio I2S codecs
#define HDAC_I2S_DISABLED 0
#define HDAC_I2S_ALC274   1
#define HDAC_I2S_ALC1308  2

Device (HDAC)
{
  Name (_HID, "INT00000")
  Name (_CID, "INT00000")
  Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
  Name (_UID, 1)
  Name (CADR, 0) // Codec I2C address
  Name (CDIS, 0) // Codec Disabled state

  Method(_INI) {
    If (LEqual(I2SC, HDAC_I2S_ALC274)) {
      Store ("INT34C2", _HID)
      Store ("INT34C2", _CID)
      Store (0x1C, CADR)
    } ElseIf (LEqual(I2SC, HDAC_I2S_ALC1308)) {
      Store ("10EC1308", _HID)
      Store ("10EC1308", _CID)
      Store (0x0010, CADR)
    }
  }

  Method (_CRS, 0, NotSerialized) {

    // IICB: Method generates 'I2cSerialBus' descriptor buffer
    // CADR: Device variable with codec address (set in _INI based on codec model)
    // I2SB: NVS variable with I2C controller connection
    // INTB: Method generates 'Interrupt' descriptor buffer
    // I2SI: NVS variable with codec jack detection pin
    Return (ConcatenateResTemplate(IICB(CADR, I2SB), INTB(I2SI, INT_LEVEL_TRIG, INT_ACTIVE_LOW)))
  }

  Method (_STA, 0, NotSerialized) {
    If (LAnd(LNotEqual(I2SC, HDAC_I2S_DISABLED), LNotEqual(CDIS, 1))) {
      Return (0xF)  // I2S Codec Enabled
    }

    If (LEqual (CDIS, 1)) {
      Return (0xD)  // Disabled from _DIS 1101b - Present/Disabled (via Device Manager)
    }

    Return (0x0)  // I2S Codec Dialed (via BIOS Policy/Setup)
  }

  Method (_SRS, 0x1, Serialized) {
    Store (0, CDIS) // Clear Disabled bit
  }

  Method (_DIS, 0x0, NotSerialized) {
    Store (1, CDIS) // Set Disabled bit
  }
}  // Device (HDAC)
