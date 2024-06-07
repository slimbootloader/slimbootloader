/**@file
 I2S Audio Codec ACPI

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "EverestCodec8316.h"

// HD Audio I2S codecs
#define HDAC_I2S_DISABLED 0
#define HDAC_I2S_ALC274   1
#define HDAC_I2S_ALC1308  2
#define HDAC_I2S_EVEREST8316  3
#define HDAC_I2S_EVEREST8326  4
#define HDAC_I2S_EVEREST8336  5

If (LAnd ( LNotEqual(I2SC, HDAC_I2S_EVEREST8326), LAnd ( LNotEqual(I2SC, HDAC_I2S_EVEREST8316), LNotEqual(I2SC, HDAC_I2S_EVEREST8336))))
{
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
      Return (0x0)  // I2S Codec Disaled (via BIOS Policy/Setup)
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
} Else {
  Device (ESSX)
  {
    Name (_HID, "ESSX8316")  // _HID: Hardware ID
    Name (_CID, "ESSX8316")  // _CID: Compatible ID
    Name (_DDN, "ESSX Codec Controller 8316 ")  // _DDN: DOS Device Name
    Name (_UID, One)  // _UID: Unique ID

    Method(_INI)
    {
      If (LEqual(I2SC, HDAC_I2S_EVEREST8316))
      {
        Store ("ESSX8316", _HID)
        Store ("ESSX8316", _CID)
        Store ("ESSX Codec Controller 8316", _DDN)
      } ElseIf (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
        Store ("ESSX8326", _HID)
        Store ("ESSX8326", _CID)
        Store ("ESSX Codec Controller 8326", _DDN)
      } ElseIf (LEqual(I2SC, HDAC_I2S_EVEREST8336)) {
        Store ("ESSX8336", _HID)
        Store ("ESSX8336", _CID)
        Store ("ESSX Codec Controller 8336", _DDN)
      }
    }

    Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
    {
      Name (PBUF, ResourceTemplate ()
      {
        GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){ GPIO_VER6_SOC_M_DRIVER_GPP_S4 }
      })
      Name (GBUF, ResourceTemplate ()
      {
        GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){ GPIO_VER6_SOC_M_DRIVER_GPP_S5 }
      })

      If ( LOr (LEqual(I2SC, HDAC_I2S_EVEREST8316), LEqual(I2SC, HDAC_I2S_EVEREST8336)))
      {
        Name (SBFB, ResourceTemplate ()
        {
          I2cSerialBusV2 (0x0011, ControllerInitiated, 0x00061A80,
              AddressingMode7Bit, "\\_SB.PC00.I2C3",
              0x00, ResourceConsumer, , Exclusive,
              )
        })

        Return (ConcatenateResTemplate (SBFB, ConcatenateResTemplate (PBUF, GBUF)))
      }

      ElseIf (LEqual(I2SC, HDAC_I2S_EVEREST8326))
      {
        If (LEqual(I2SB, 1))
        {
          Name (SBF1, ResourceTemplate () {
            I2cSerialBusV2 (0x00, ControllerInitiated, 0x00061A80, AddressingMode7Bit, "\\_SB.PC00.I2C1", 0x00, ResourceConsumer, ISB1, Exclusive,)
          })

          CreateWordField (SBF1, ISB1._ADR, SBS1)
          Store(EVSA, SBS1)

          Return (ConcatenateResTemplate (SBF1, ConcatenateResTemplate (PBUF, GBUF)))
        }
        ElseIf (LEqual(I2SB, 3))
        {
          Name (SBF3, ResourceTemplate () {
            I2cSerialBusV2 (0x00, ControllerInitiated, 0x00061A80, AddressingMode7Bit, "\\_SB.PC00.I2C3", 0x00, ResourceConsumer, ISB3, Exclusive,)
          })

          CreateWordField (SBF3, ISB3._ADR, SBS3)
          Store(EVSA, SBS3)

          Return (ConcatenateResTemplate (SBF3, ConcatenateResTemplate (PBUF, GBUF)))
        }
        Else
        {
          Return (Buffer(){0})
        }
      }

      Else
      {
        Return (Buffer(){0})
      }
    }

    Method (_PRS, 0, NotSerialized)  // _PRS: Possible Resource Settings
    {
      Return (_CRS)
    }

    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
      Return (0x0F)
    }

    Method (_PS3, 0, NotSerialized)  // _PS3: Power State 3
    {
    }

    Method (_PS0, 0, NotSerialized)  // _PS0: Power State 0
    {
    }

    Method (_SRS, 0x1, Serialized)  // _SRS: Set Resources Settings
    {
    }

    Method (_DIS, 0x0, NotSerialized)  // _DIS: Disable Device
    {
    }

    Method (_DSM, 4, NotSerialized)  // _DSM: Device-Specific Method
    {
      If ((Arg0 == ToUUID ("A9800C04-E016-343E-41F4-6BCCE70F4332")))
      {
        If ((Arg2 == ONBOARD_MICRPHONE_TYPE))
        {
          Return (ONBOARD_MICRPHONE_TYPE_MIC_2CH)
        }

        If ((Arg2 == HEADPHONE_MICROPHONE_LOCATION))
        {
          Return (HEADPHONE_MICROPHONE_LOCATION_LIN2RIN2_DFLT)
        }

        If ((Arg2 == SPEAKER_TYPE))
        {
          Return (SPEAKER_TYPE_STEREO_SPEAKER_DFLT)
        }

        If ((Arg2 == POLARITY_TYPE))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (POLARITY_TYPE_INVERTED_DFLT)
          } Else {
            Return (POLARITY_TYPE_NORMAL)
          }
        }

        If ((Arg2 == FORMAT_TYPE))
        {
          Return (FORMAT_TYPE_DSP_PCMA_DFLT)
        }

        If ((Arg2 == 0x06))
        {
          Return (Zero)
        }

        If ((Arg2 == EVREST_CODEC_TYPE))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (EVREST_CODEC_TYPE_8326)
          } Else {
            Return (EVREST_CODEC_TYPE_8316_8336_DFLT)
          }
        }

        If ((Arg2 == EVEREST_I2S_BUS_NUMBER))
        {
          //I2S Bus Number in CPU platform
          Return (EVEREST_MTL_I2S1_BUS_NUMBER)
        }

        If ((Arg2 == HEADSET_MPHONE_BOOST_GAIN))
        {
          Return (HEADSET_MPHONE_BOOST_GAIN_DFLT)
        }

        If ((Arg2 == ONBOARD_MPHONE_BOOST_GAIN))
        {
          Return (ONBOARD_MPHONE_BOOST_GAIN_DFLT)
        }

        If ((Arg2 == 0x2C))
        {
          Return (0x03)
        }

        If ((Arg2 == 0x2D))
        {
          Return (0x1B)
        }

        If ((Arg2 == HEADSET_MPHONE_15DB_GAIN))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (HEADSET_MPHONE_15DB_GAIN_8326)
          } Else {
            Return (HEADSET_MPHONE_15DB_GAIN_DFLT)
          }
        }

        If ((Arg2 == 0x21))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (One)
          } Else {
            Return (Zero)
          }
        }

        If ((Arg2 == 0x12))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x22))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x13))
        {
          Return (One)
        }

        If ((Arg2 == 0x23))
        {
          Return (One)
        }

        If ((Arg2 == 0x14))
        {
          Return (0x0A)
        }

        If ((Arg2 == 0x24))
        {
          Return (0x0A)
        }

        If ((Arg2 == 0x15))
        {
          Return (0x12)
        }

        If ((Arg2 == 0x25))
        {
          Return (0x12)
        }

        If ((Arg2 == 0x16))
        {
          Return (0x08)
        }

        If ((Arg2 == 0x26))
        {
          Return (0x08)
        }

        If ((Arg2 == 0x17))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x27))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x18))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x28))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x19))
        {
          Return (0x02)
        }

        If ((Arg2 == 0x29))
        {
          Return (0x02)
        }

        If ((Arg2 == 0x1A))
        {
          Return (0x03)
        }

        If ((Arg2 == 0x2A))
        {
          Return (0x03)
        }

        If ((Arg2 == 0x1B))
        {
          Return (One)
        }

        If ((Arg2 == 0x2B))
        {
          Return (One)
        }

        If ((Arg2 == 0x40))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x50))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x41))
        {
          Return (0xBB)
        }

        If ((Arg2 == 0x51))
        {
          Return (0xBB)
        }

        If ((Arg2 == 0x42))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x52))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x44))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0xBF)
          } Else {
            Return (Zero)
          }
        }

        If ((Arg2 == 0x54))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0xBF)
          } Else {
            Return (Zero)
          }
        }

        If ((Arg2 == 0x45))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0xBF)
          } Else {
            Return (Zero)
          }
        }

        If ((Arg2 == 0x55))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0xBF)
          } Else {
            Return (Zero)
          }
        }

        If ((Arg2 == 0x4A))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x5A))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x46))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x56))
        {
          Return (One)
        }

        If ((Arg2 == 0x5B))
        {
          Return (One)
        }

        If ((Arg2 == 0x85))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x10)
          } Else {
            Return (Zero)
          }
        }

        If ((Arg2 == 0x8E))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (One)
          } Else {
            Return (0)
          }
        }

        If ((Arg2 == 0x8F))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x22)
          } Else {
            Return (0x11)
          }
        }

        If ((Arg2 == 0x90))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x21)
          } Else {
            Return (0x22)
          }
        }

        If ((Arg2 == 0x91))
        {
          Return (One)
        }

        If ((Arg2 == 0x92))
        {
          Return (One)
        }

        If ((Arg2 == 0x93))
        {
          Return (One)
        }

        If ((Arg2 == 0x94))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x61)
          } Else {
            Return (0x62)
          }
        }

        If ((Arg2 == 0x95))
        {
          Return (0x41)
        }

        If ((Arg2 == 0x96))
        {
          Return (0x41)
        }

        If ((Arg2 == 0x97))
        {
          Return (0x41)
        }

        If ((Arg2 == 0x98))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x99))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x81)
          } Else {
            Return (0x80)
          }
        }

        If ((Arg2 == 0x9A))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x81)
          } Else {
            Return (0x80)
          }
        }

        If ((Arg2 == 0x9B))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x81)
          } Else {
            Return (0x80)
          }
        }

        If ((Arg2 == 0x9C))
        {
          Return (Zero)
        }

        If ((Arg2 == 0x9D))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x81)
          } Else {
            Return (0x80)
          }
        }

        If ((Arg2 == 0x9E))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x81)
          } Else {
            Return (0x80)
          }
        }

        If ((Arg2 == 0x9F))
        {
          If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
            Return (0x81)
          } Else {
            Return (0x80)
          }
        }

        If ((Arg2 == 0xA0))
        {
          Return (0x71)
        }

        If ((Arg2 == 0xA1))
        {
          Return (0x11)
        }

        If ((Arg2 == 0xA2))
        {
          Return (0x10)
        }

        If ((Arg2 == 0xA3))
        {
          Return (0x10)
        }

        If ((Arg2 == 0xA4))
        {
          Return (0x71)
        }

        If ((Arg2 == 0xA5))
        {
          Return (0x11)
        }

        If ((Arg2 == 0xA6))
        {
          Return (0x10)
        }

        If ((Arg2 == 0xA7))
        {
          Return (0x10)
        }

        If ((Arg2 == 0xA8))
        {
          Return (0x10)
        }

        If ((Arg2 == 0xA9))
        {
          Return (0x31)
        }

        If ((Arg2 == 0xAA))
        {
          Return (0x30)
        }

        If ((Arg2 == 0xAB))
        {
          Return (0x30)
        }

        If ((Arg2 == 0xAC))
        {
          Return (0x10)
        }

        If ((Arg2 == 0xAD))
        {
          Return (0x31)
        }

        If ((Arg2 == 0xAE))
        {
          Return (0x30)
        }

        If ((Arg2 == 0xAF))
        {
          Return (0x30)
        }

        If ((Arg2 == 0xB0))
        {
          Return (0x10)
        }

        If ((Arg2 == 0xB1))
        {
          Return (0x20)
        }

        If ((Arg2 == 0xB2))
        {
          Return (0x20)
        }

        If ((Arg2 == 0xB3))
        {
          Return (0x20)
        }

        If ((Arg2 == 0xB4))
        {
          Return (0xA0)
        }

        If ((Arg2 == 0xB5))
        {
          Return (0xB0)
        }

        If ((Arg2 == 0xB6))
        {
          Return (0xB0)
        }

        If ((Arg2 == 0xB7))
        {
          Return (0xB0)
        }

        If ((Arg2 == 0xB8))
        {
          Return (Zero)
        }

        If ((Arg2 == 0xB9))
        {
          Return (One)
        }

        If ((Arg2 == 0xBA))
        {
          Return (One)
        }

        If ((Arg2 == 0xBB))
        {
          Return (One)
        }

        If ((Arg2 == 0xBC))
        {
          Return (0x03)
        }

        If ((Arg2 == 0xBD))
        {
          Return (0x04)
        }

        If ((Arg2 == 0xBE))
        {
          Return (0x04)
        }

        If ((Arg2 == 0xBF))
        {
          Return (0x04)
        }

        If ((Arg2 == 0xC0))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xC1))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xC2))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xC3))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xC4))
        {
          Return (Zero)
        }

        If ((Arg2 == 0xC5))
        {
          Return (0xB3)
        }

        If ((Arg2 == 0xC6))
        {
          Return (0xC9)
        }

        If ((Arg2 == 0xC7))
        {
          Return (0x8C)
        }

        If ((Arg2 == 0xC8))
        {
          Return (Zero)
        }

        If ((Arg2 == 0xC9))
        {
          Return (0xB3)
        }

        If ((Arg2 == 0xCA))
        {
          Return (0xC9)
        }

        If ((Arg2 == 0xCB))
        {
          Return (0x8C)
        }

        If ((Arg2 == 0xCC))
        {
          Return (Zero)
        }

        If ((Arg2 == 0xCD))
        {
          Return (0xB3)
        }

        If ((Arg2 == 0xCE))
        {
          Return (0xC9)
        }

        If ((Arg2 == 0xCF))
        {
          Return (0x8C)
        }

        If ((Arg2 == 0xD0))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xD1))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xD2))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xD3))
        {
          Return (0xFF)
        }

        If ((Arg2 == 0xD4))
        {
          Return (Zero)
        }

        If ((Arg2 == 0xD5))
        {
          Return (0xC6)
        }

        If ((Arg2 == 0xD6))
        {
          Return (0x73)
        }

        If ((Arg2 == 0xD7))
        {
          Return (0x86)
        }

        If ((Arg2 == 0xD8))
        {
          Return (Zero)
        }

        If ((Arg2 == 0xD9))
        {
          Return (0xC6)
        }

        If ((Arg2 == 0xDA))
        {
          Return (0x73)
        }

        If ((Arg2 == 0xDB))
        {
          Return (0x86)
        }

        If ((Arg2 == 0xDC))
        {
          Return (Zero)
        }

        If ((Arg2 == 0xDD))
        {
          Return (0xC6)
        }

        If ((Arg2 == 0xDE))
        {
          Return (0x73)
        }

        If ((Arg2 == 0xDF))
        {
          Return (0x86)
        }
      }
      Return (0xFF)
    }
  }
}//End of Else
