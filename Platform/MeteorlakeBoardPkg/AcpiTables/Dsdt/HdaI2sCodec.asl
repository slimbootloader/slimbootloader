/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


// HD Audio I2S codecs
#define HDAC_I2S_DISABLED 0
#define HDAC_I2S_ALC274   1
#define HDAC_I2S_ALC1308  2

//
// HDAC 1308
//
Device (HDC1)
{
  Name (_HID, "10EC1308")
  Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
  Name (_UID, 1)
  Name (CDIS, 0) // Codec Disabled state

  Method (_CRS, 0, NotSerialized) {

    // IICB: Method generates 'I2cSerialBusV2' descriptor buffer
    // I2SB: NVS variable with I2C controller connection
    // INTB: Method generates 'Interrupt' descriptor buffer
    // I2SI: NVS variable with codec jack detection pin
    Return (ConcatenateResTemplate (IICB (0x10, I2SB), INTB (I2SI, INT_LEVEL_TRIG, INT_ACTIVE_LOW)))
    }

  Method (_PRS, 0, NotSerialized) {
    Return (_CRS)
  }

  Method (_STA, 0, NotSerialized) {
    If (LAnd (LEqual (I2SC, HDAC_I2S_ALC1308), LNotEqual (CDIS, 1))) {
      Return (0xF)  // I2S Codec Enabled
    }



    Return (0x0)  // I2S Codec Dialed (via BIOS Policy/Setup)
  }

  Method (_SRS, 0x1, Serialized) {
    Store (0, CDIS) // Clear Disabled bit
  }

  Method (_DIS, 0x0, NotSerialized) {
    Store (1, CDIS) // Set Disabled bit
  }
}  // Device (HDC1)
//
// HDAC 274
//
Device (HDC2)
{
  Name (_HID, "INT34C2")
  Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
  Name (_UID, 1)
  Name (CDIS, 0) // Codec Disabled state
  Method (_CRS, 0, NotSerialized) {
    // IICB: Method generates 'I2cSerialBusV2' descriptor buffer
    // I2SB: NVS variable with I2C controller connection
    // INTB: Method generates 'Interrupt' descriptor buffer
    // I2SI: NVS variable with codec jack detection pin
    Return (ConcatenateResTemplate (IICB (0x1C, I2SB), INTB (I2SI, INT_LEVEL_TRIG, INT_ACTIVE_LOW)))
  }
  Method (_PRS, 0, NotSerialized) {
    Return (_CRS)
  }
  Method (_STA, 0, NotSerialized) {
    If (LAnd (LEqual (I2SC, HDAC_I2S_ALC274), LNotEqual (CDIS, 1))) {
      Return (0xF)  // I2S Codec Enabled
    }
    Return (0x0)  // I2S Codec Disabled (via BIOS Policy/Setup)
  }
  Method (_SRS, 0x1, Serialized) {
    Store (0, CDIS) // Clear Disabled bit
  }
  Method (_DIS, 0x0, NotSerialized) {
    Store (1, CDIS) // Set Disabled bit
  }
}  // Device (HDC2)

