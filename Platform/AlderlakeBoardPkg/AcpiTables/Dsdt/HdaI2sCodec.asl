/** @file

  I2S Audio Codec ACPI

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
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
#define HDAC_I2S_ALC5682I_VD  6
#define HDAC_I2S_ALC5682I_VS  7
#define HDAC_I2S_PCM3168A     8

If (LAnd ( LNotEqual(I2SC, HDAC_I2S_EVEREST8326)     , LAnd ( LNotEqual(I2SC, HDAC_I2S_EVEREST8316) ,LNotEqual(I2SC, HDAC_I2S_EVEREST8336)) ) )
{
        If ( LOr(LEqual(I2SC, HDAC_I2S_ALC274), LEqual(I2SC, HDAC_I2S_ALC1308)) )
 {

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

      //Added for HSD 16015447895
       If ( Lor (LOr (LEqual(I2SC, HDAC_I2S_ALC274), LEqual(I2SC, HDAC_I2S_ALC1308)), LEqual(I2SC, HDAC_I2S_DISABLED) )) {
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

        //
        // [ACPI] platform:InternalOnlyBegin
        //
        // _DIS and _SRS methods are used to disable/enable device from Windows Device Manager.
        // Currently I2S HW Codec Driver does not support this feature and without these ACPI methods 'Disable' option is inactive/grayed-out.
        //
        // [ACPI] platform:InternalOnlyEnd
        //
        Method (_SRS, 0x1, Serialized) {
          Store (0, CDIS) // Clear Disabled bit
        }

        Method (_DIS, 0x0, NotSerialized) {
          Store (1, CDIS) // Set Disabled bit
        }
      } // I2SC == HDAC_I2S_ALC274 or HDAC_I2S_ALC1308
    }  // Device (HDAC)

} ElseIf ( LOr(LEqual(I2SC, HDAC_I2S_ALC5682I_VD), LEqual(I2SC, HDAC_I2S_ALC5682I_VS)) ) { // I2SC is not HDAC_I2S_ALC274 or HDAC_I2S_ALC1308
    Device(RT58)
    {
      Name (_HID, "INT00000")  // _HID: Hardware ID
      Name (_CID, "INT00000")  // _CID: Compatible ID
      Name (_UID, Zero)  // _UID: Unique ID
      Name (_DDN, "Headset Codec")  // _DDN: DOS Device Name

      Method(_INI) {
        If (LEqual(I2SC, HDAC_I2S_ALC5682I_VD)) {
          Store ("10EC5682", _HID)
          Store ("10EC5682", _CID)
        } ElseIf (LEqual(I2SC, HDAC_I2S_ALC5682I_VS)) {
          Store ("RTL5682", _HID)
          Store ("RTL5682", _CID)
        }
     }

      Method(_STA, 0, NotSerialized)  // _STA: Status
      {
        Return (0x0F)
      }

      Name (_CRS, ResourceTemplate ()
      {
        I2cSerialBusV2 (0x001A, ControllerInitiated, 0x00061A80,
            AddressingMode7Bit, "\\_SB.PC00.I2C0",
            0x00, ResourceConsumer, ,Exclusive,
        )
        GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
            "\\_SB.GPI0", 0x00, ResourceConsumer, ,)
                {
                //GPIO_VER2_LP_DRIVER_GPP_R6
                358
                }

        GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,
            "\\_SB.GPI0", 0x00, ResourceConsumer, ,)
                {
                //GPIO_VER2_LP_DRIVER_GPP_R2
                354
                }
      })

      Name (_DSD, Package (0x02)  //_DSD: Device-Specific Data
      {
        ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),/*Device Properties for _DSD */
        Package (0x02)
        {
           Package (0x02)
           {
              "irq-gpios",
              Package (0x04)
              {
                \_SB.PC00.I2C0.RT58,
                Zero,
                Zero,
                Zero
              }
            },

            Package (0x02)
            {
              "realtek,jd-src",
              One
            }
          }
        })
      } // Device (RT58)

}ElseIf (LEqual(I2SC, HDAC_I2S_PCM3168A)) { // I2SC is not HDAC_I2S_ALC5682I_VD or HDAC_I2S_ALC5682I_VS
    Device(TI36)
    {
      Name (_HID, "INT00000")       // _HID: Hardware ID
      Name (_CID, "INT00000")       // _CID: Compatible ID
      Name (_UID, Zero)             // _UID: Unique ID
      Name (_DDN, "PCM3168A codec")   // _DDN: DOS Device Name

      Method(_INI)
      {
        Store ("pcm3168a", _HID)
        Store ("pcm3168a", _CID)
      }

      Method(_STA, 0, NotSerialized)  // _STA: Status
      {
        Return (0x0F)
      }

      Name (_CRS, ResourceTemplate ()
      {
        I2cSerialBusV2 (0x44, ControllerInitiated, 0x00061A80,
            AddressingMode7Bit, "\\_SB.PC00.I2C0",
            0x00, ResourceConsumer, ,Exclusive,
        )
        //GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,
           // "\\_SB.GPI0", 0x00, ResourceConsumer, ,)
           //     {
                //GPIO_VER2_LP_DRIVER_GPP_S4
              //  100
              //  }

       // GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,
          //  "\\_SB.GPI0", 0x00, ResourceConsumer, ,)
             //   {
                //GPIO_VER2_LP_DRIVER_GPP_S5
             //   101
             //   }
      })

      Name (_DSD, Package (0x02)  //_DSD: Device-Specific Data
      {
        ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),/*Device Properties for _DSD */
        Package (0x02)
        {
           Package (0x02)
           {
              "irq-gpios",
              Package (0x04)
              {
                \_SB.PC00.I2C0.TI36,
                Zero,
                Zero,
                Zero
              }
            },

            Package (0x02)
            {
              "realtek,jd-src",
              One
            }
          }
        })
    } // Device (TI36)
 }

}Else{
    Device (ESSX)
    {
            //Name (_ADR, Zero)  // _ADR: Address
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
            }ElseIf (LEqual(I2SC, HDAC_I2S_EVEREST8336)) {
              Store ("ESSX8336", _HID)
              Store ("ESSX8336", _CID)
              Store ("ESSX Codec Controller 8336", _DDN)
            }
        }

        Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings
        {
            Name (SBFB, ResourceTemplate ()
            {
                    I2cSerialBusV2 (0x0011, ControllerInitiated, 0x00061A80,AddressingMode7Bit, "\\_SB.PC00.I2C0",0x00, ResourceConsumer, , Exclusive,)
            })
            Name (SBF2, ResourceTemplate () {
                    I2cSerialBusV2 (0x0018, ControllerInitiated, 0x00061A80, AddressingMode7Bit, "\\_SB.PC00.I2C0", 0x00, ResourceConsumer, , Exclusive,)
                    I2cSerialBusV2 (0x0009, ControllerInitiated, 0x00061A80, AddressingMode7Bit, "\\_SB.PC00.I2C0", 0x00, ResourceConsumer, , Exclusive,)
            })
            If ( LOr (LEqual(I2SC, HDAC_I2S_EVEREST8316),LEqual(I2SC, HDAC_I2S_EVEREST8336) )  )
            {

                If (ToInteger(PLID) == 0x39)//BoardIdRplSAdpSDdr5UDimm1DCrb
                {
                    Name (GBUF, ResourceTemplate ()
                    {
                        GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){34}//GPIO_VER4_S_DRIVER_GPP_R2
                    })

                    Return (ConcatenateResTemplate (SBFB, GBUF))

                 }Else{
                    Name (PBF1, ResourceTemplate ()
                    {
                      GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){358 }//GPIO_VER2_LP_DRIVER_GPP_R6
                    })
                    Name (GBF1, ResourceTemplate ()
                    {
                        GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){354}//GPIO_VER2_LP_DRIVER_GPP_R2
                    })
                    Return (ConcatenateResTemplate (SBFB, ConcatenateResTemplate (PBF1, GBF1)))
                 }//End of If (ToInteger(PLID) == BoardIdRplSAdpSDdr5UDimm1DCrb)

            }Else
            {

                If (ToInteger(PLID) == 0x39)//BoardIdRplSAdpSDdr5UDimm1DCrb
                {
                    Name (GBF2, ResourceTemplate ()
                    {
                        GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){34}//GPIO_VER4_S_DRIVER_GPP_R2
                    })

                    Return (ConcatenateResTemplate (SBF2, GBF2))

                 }Else{
                    Name (PBF3, ResourceTemplate ()
                    {
                      GpioIo (Exclusive, PullDefault, 0x0000, 0x0000, IoRestrictionOutputOnly,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){358 }//GPIO_VER2_LP_DRIVER_GPP_R6
                    })
                    Name (GBF3, ResourceTemplate ()
                    {
                        GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullUp, 0x0000,"\\_SB.GPI0", 0x00, ResourceConsumer, ,){354}//GPIO_VER2_LP_DRIVER_GPP_R2
                    })

                    Return (ConcatenateResTemplate (SBF2, ConcatenateResTemplate (PBF3, GBF3)))
                }//End of  If (ToInteger(PLID) == BoardIdRplSAdpSDdr5UDimm1DCrb)
            }//End of If ( LOr (LEqual(I2SC, HDAC_I2S_EVEREST8316),LEqual(I2SC, HDAC_I2S_EVEREST8336) )  )



        }//End of  Method (_CRS, 0, NotSerialized)  // _CRS: Current Resource Settings

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

        Method (_DIS, 0, NotSerialized)  // _DIS: Disable Device
        {
        }


        Method (_DSM, 4, NotSerialized)  // _DSM: Device-Specific Method
        {

            If ((Arg0 == ToUUID ("A9800C04-E016-343E-41F4-6BCCE70F4332")))
            {
                    If ((Arg2 == ONBOARD_MICRPHONE_TYPE)) {
                        Return (ONBOARD_MICRPHONE_TYPE_MIC_2CH)
                    }
                    If ((Arg2 == HEADPHONE_MICROPHONE_LOCATION)) {
                        Return (HEADPHONE_MICROPHONE_LOCATION_LIN2RIN2_DFLT)
                    }
                    If ((Arg2 == SPEAKER_TYPE)) {
                        Return (SPEAKER_TYPE_STEREO_SPEAKER_DFLT)
                    }
                    If ((Arg2 == POLARITY_TYPE)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (POLARITY_TYPE_INVERTED_DFLT)
                        }Else {
                            Return (POLARITY_TYPE_NORMAL)
                        }
                    }
                    If ((Arg2 == 0x06)) {
                        Return (Zero)
                    }
                    If ((Arg2 == EVREST_CODEC_TYPE)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (EVREST_CODEC_TYPE_8326)
                        }Else {
                            Return (EVREST_CODEC_TYPE_8316_8336_DFLT)
                        }
                    }
                    If ((Arg2 == EVEREST_I2S_BUS_NUMBER)) {
                        //I2S Bus Number in CPU platform
                        Return (EVEREST_ADLP_I2S_BUS_NUMBER)
                    }
                    If ((Arg2 == HEADSET_MPHONE_BOOST_GAIN)) {
                        Return (HEADSET_MPHONE_BOOST_GAIN_DFLT)
                    }
                    If ((Arg2 == ONBOARD_MPHONE_BOOST_GAIN)) {
                        Return (ONBOARD_MPHONE_BOOST_GAIN_DFLT)
                    }
                    If ((Arg2 == HEADSET_MPHONE_15DB_GAIN)) {
                       If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (HEADSET_MPHONE_15DB_GAIN_8326)
                        }Else {
                            Return (HEADSET_MPHONE_15DB_GAIN_DFLT)
                        }
                    }
                    If ((Arg2 == 0x2C)) {
                        Return (0x03)
                    }
                    If ((Arg2 == 0x2D)) {
                        Return (0x1B)
                    }
                    If ((Arg2 == 0x21)) {
                         If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (One)
                        }Else {
                             Return (Zero)
                        }
                    }
                    If ((Arg2 == 0x12)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x22)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x13)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x23)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x14)) {
                        Return (0x0A)
                    }
                    If ((Arg2 == 0x24)) {
                        Return (0x0A)
                    }
                    If ((Arg2 == 0x15)) {
                        Return (0x12)
                    }
                    If ((Arg2 == 0x25)) {
                        Return (0x12)
                    }
                    If ((Arg2 == 0x16)) {
                        Return (0x08)
                    }
                    If ((Arg2 == 0x26)) {
                        Return (0x08)
                    }
                    If ((Arg2 == 0x17)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x27)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x18)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x28)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x19)) {
                        Return (0x02)
                    }
                    If ((Arg2 == 0x29)) {
                        Return (0x02)
                    }
                    If ((Arg2 == 0x1A)) {
                        Return (0x03)
                    }
                    If ((Arg2 == 0x2A)) {
                        Return (0x03)
                    }
                    If ((Arg2 == 0x1B)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x2B)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x40)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x50)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x41)) {
                        Return (0xBB)
                    }
                    If ((Arg2 == 0x51)) {
                        Return (0xBB)
                    }
                    If ((Arg2 == 0x42)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x52)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x44))                     {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0xBF)
                        }Else {
                             Return (Zero)
                        }
                    }
                    If ((Arg2 == 0x54)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0xBF)
                        }Else{
                             Return (Zero)
                        }
                    }
                    If ((Arg2 == 0x45)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0xBF)
                        }Else{
                             Return (Zero)
                        }
                    }
                    If ((Arg2 == 0x55)) {
                       If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0xBF)
                        }Else {
                             Return (Zero)
                        }
                    }
                    If ((Arg2 == 0x4A)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x5A)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x46)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x56)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x5B)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x85)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (0x10)
                        }Else {
                            Return (Zero)
                        }
                    }
                    If ((Arg2 == 0x8E)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (One)
                        }Else {
                            Return (Zero)
                        }
                    }
                    If ((Arg2 == 0x8F)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (0x22)
                        }Else {
                            Return (0x11)
                        }
                    }
                    If ((Arg2 == 0x90)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (0x21)
                        }Else {
                            Return (0x22)
                        }
                    }
                    If ((Arg2 == 0x91)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x92)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x93)) {
                        Return (One)
                    }
                    If ((Arg2 == 0x94)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (0x61)
                        }Else {
                            Return (0x62)
                        }
                    }
                    If ((Arg2 == 0x95)) {
                        Return (0x41)
                    }
                    If ((Arg2 == 0x96)) {
                        Return (0x41)
                    }
                    If ((Arg2 == 0x97)) {
                        Return (0x41)
                    }
                    If ((Arg2 == 0x98)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x99)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (0x81)
                        }Else {
                            Return (0x80)
                        }
                    }
                    If ((Arg2 == 0x9A)) {
                       If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0x81)
                        }Else{
                             Return (0x80)
                        }
                    }
                    If ((Arg2 == 0x9B)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0x81)
                        }Else{
                             Return (0x80)
                        }
                    }
                    If ((Arg2 == 0x9C)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0x9D)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                            Return (0x81)
                        }Else {
                            Return (0x80)
                        }
                    }
                    If ((Arg2 == 0x9E)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0x81)
                        }Else {
                             Return (0x80)
                        }
                    }
                    If ((Arg2 == 0x9F)) {
                        If (LEqual(I2SC, HDAC_I2S_EVEREST8326)) {
                             Return (0x81)
                        }Else {
                             Return (0x80)
                        }
                    }
                    If ((Arg2 == 0xA0)) {
                        Return (0x71)
                    }
                    If ((Arg2 == 0xA1)) {
                        Return (0x11)
                    }
                    If ((Arg2 == 0xA2)) {
                        Return (0x10)
                    }
                    If ((Arg2 == 0xA3)) {
                        Return (0x10)
                    }
                    If ((Arg2 == 0xA4)) {
                        Return (0x71)
                    }
                    If ((Arg2 == 0xA5)) {
                        Return (0x11)
                    }
                    If ((Arg2 == 0xA6)) {
                        Return (0x10)
                    }
                    If ((Arg2 == 0xA7)) {
                        Return (0x10)
                    }
                    If ((Arg2 == 0xA8)) {
                        Return (0x10)
                    }
                    If ((Arg2 == 0xA9)) {
                        Return (0x31)
                    }
                    If ((Arg2 == 0xAA)) {
                        Return (0x30)
                    }
                    If ((Arg2 == 0xAB)) {
                        Return (0x30)
                    }
                    If ((Arg2 == 0xAC)) {
                        Return (0x10)
                    }
                    If ((Arg2 == 0xAD)) {
                        Return (0x31)
                    }
                    If ((Arg2 == 0xAE)) {
                        Return (0x30)
                    }
                    If ((Arg2 == 0xAF)) {
                        Return (0x30)
                    }
                    If ((Arg2 == 0xB0)) {
                        Return (0x10)
                    }
                    If ((Arg2 == 0xB1)) {
                        Return (0x20)
                    }
                    If ((Arg2 == 0xB2)) {
                        Return (0x20)
                    }
                    If ((Arg2 == 0xB3)) {
                        Return (0x20)
                    }
                    If ((Arg2 == 0xB4)) {
                        Return (0xA0)
                    }
                    If ((Arg2 == 0xB5)) {
                        Return (0xB0)
                    }
                    If ((Arg2 == 0xB6)) {
                        Return (0xB0)
                    }
                    If ((Arg2 == 0xB7)) {
                        Return (0xB0)
                    }
                    If ((Arg2 == 0xB8)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0xB9)) {
                        Return (One)
                    }
                    If ((Arg2 == 0xBA)) {
                        Return (One)
                    }
                    If ((Arg2 == 0xBB)) {
                        Return (One)
                    }
                    If ((Arg2 == 0xBC)) {
                        Return (0x03)
                    }
                    If ((Arg2 == 0xBD)) {
                        Return (0x04)
                    }
                    If ((Arg2 == 0xBE)) {
                        Return (0x04)
                    }
                    If ((Arg2 == 0xBF)) {
                        Return (0x04)
                    }
                    If ((Arg2 == 0xC0)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xC1)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xC2)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xC3)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xC4)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0xC5)) {
                        Return (0xB3)
                    }
                    If ((Arg2 == 0xC6)) {
                        Return (0xC9)
                    }
                    If ((Arg2 == 0xC7)) {
                        Return (0x8C)
                    }
                    If ((Arg2 == 0xC8)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0xC9)) {
                        Return (0xB3)
                    }
                    If ((Arg2 == 0xCA)) {
                        Return (0xC9)
                    }
                    If ((Arg2 == 0xCB)) {
                        Return (0x8C)
                    }
                    If ((Arg2 == 0xCC)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0xCD)) {
                        Return (0xB3)
                    }
                    If ((Arg2 == 0xCE)) {
                        Return (0xC9)
                    }
                    If ((Arg2 == 0xCF)) {
                        Return (0x8C)
                    }
                    If ((Arg2 == 0xD0)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xD1)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xD2)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xD3)) {
                        Return (0xFF)
                    }
                    If ((Arg2 == 0xD4)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0xD5)) {
                        Return (0xC6)
                    }
                    If ((Arg2 == 0xD6)) {
                        Return (0x73)
                    }
                    If ((Arg2 == 0xD7)) {
                        Return (0x86)
                    }
                    If ((Arg2 == 0xD8)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0xD9)) {
                        Return (0xC6)
                    }
                    If ((Arg2 == 0xDA)) {
                        Return (0x73)
                    }
                    If ((Arg2 == 0xDB)) {
                        Return (0x86)
                    }
                    If ((Arg2 == 0xDC)) {
                        Return (Zero)
                    }
                    If ((Arg2 == 0xDD)) {
                        Return (0xC6)
                    }
                    If ((Arg2 == 0xDE)) {
                        Return (0x73)
                    }
                    If ((Arg2 == 0xDF)) {
                        Return (0x86)
                    }
            }
            Return (0xFF)
        }
    }//End of Device
}//End of Else
