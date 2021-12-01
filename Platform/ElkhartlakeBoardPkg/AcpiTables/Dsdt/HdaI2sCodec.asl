/**@file
 I2S Audio Codec ACPI

  Copyright (c) 2017 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

// HD Audio I2S codecs
#define HDAC_I2S_DISABLED 0
#define HDAC_I2S_ALC274   1
#define HDAC_I2S_ALC5660  2

///
/// I2S HW Audio Codec ACPI definition body
///
//-----------------------------------
//  HD Audio I2S Codec device
//  Disabled               (I2SC = 0)
//  Realtek ALC274         (I2SC = 1)
//  Realtek ALC5660I       (I2SC = 2)
//-----------------------------------
Device (HDAC)
{
  Name (_HID, "INT00000")
  Name (_CID, "INT00000")
  Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")
  Name (_UID, 1)
  Name (CADR, 0) // Codec I2C address
  Name (CDIS, 0) // Codec Disabled state

  Method(_INI) {
    If (LEqual(I2SC, HDAC_I2S_ALC274)) { // ALC274
      Store ("INT34C2", _HID)
      Store ("INT34C2", _CID)
      Store (0x1C, CADR)
      Return
    }
    If (LEqual(I2SC, HDAC_I2S_ALC5660)) { // ALC5660
      Store ("INTC1027", _HID)
      Store ("INTC1027", _CID)
      Store (0x1C, CADR)
      Return
    }
  }

  Method (_CRS, 0, NotSerialized) {

    Name (GBUF, ResourceTemplate () {
      GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullDefault, 0x0000, "\\_SB.GPI0", 0x00, ResourceConsumer, GINT ) { 0 }
    })
    CreateWordField(GBUF,GINT._PIN,GPIN)
    CreateField(GBUF,GINT._POL,2,GLVL)
    CreateBitField(GBUF,GINT._MOD,GTRG)

    // IICB: Method generates 'I2cSerialBus' descriptor buffer
    // CADR: Device variable with codec address (set in _INI based on codec model)
    // I2SB: NVS variable with I2C controller connection
    // INTB: Method generates 'Interrupt' descriptor buffer
    // I2SI: NVS variable with codec jack detection pin
    If (LEqual(I2SC, HDAC_I2S_ALC5660)) {
      Store (GNUM(I2SI),GPIN)
      Store (0,GLVL) // gpioint active high
      Store (1,GTRG) // gpioint edge triggered
      Return (ConcatenateResTemplate(IICB(CADR, I2SB), GBUF))
    } Else {
      Store (GNUM(I2SI),GPIN)
      Store (1,GLVL) // gpioint active low
      Store (0,GTRG) // gpioint level triggered
      Return (ConcatenateResTemplate(IICB(CADR, I2SB), GBUF))
    }
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
  Name(_PRS, ResourceTemplate(){
    StartDependentFn(0, 0) {
    IRQNoFlags(){12}
    }
    EndDependentFn()
  })
}  // Device (HDAC)
