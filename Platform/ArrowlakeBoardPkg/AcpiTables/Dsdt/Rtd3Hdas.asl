/** @file
  ACPI RTD3 SSDT table for XDAS

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
    //Power Resource for Audio Codec
    Scope(\_SB.PC00)
    {
      PowerResource(PAUD, 0, 0) {
        /// Namespace variable used:
        Name(PSTA, One) /// PSTA: Physical Power Status of Codec 0 - OFF; 1-ON
        Name(ONTM, Zero) /// ONTM: 0 - Not in Speculative ON ; Non-Zero - elapsed time in Nanosecs after Physical ON

        Name(_STA, One) /// _STA: PowerResource Logical Status 0 - OFF; 1-ON

        Method(_ON, 0){     /// _ON method \n
          Store(One, _STA)        ///- Set Logocal power state
          PUAM()
        }

        Method(_OFF, 0){    /// _OFF method \n
          Store(Zero, _STA)    ///- Set the current power state
          PUAM()
        }

        Method(PUAM, 0, Serialized)
        {
          /// Turn power on
          If(LNotEqual(^PSTA, One)) {
            Store(One, ^PSTA)
            Store(Timer(), ^ONTM)
          }
        }
      }
    }

//
// Check HDAS (HD-Audio) controller present
//

    If(LNotEqual(\_SB.PC00.HDAS.VDID, 0xFFFFFFFF)) {
      Scope(\_SB.PC00.HDAS) {
        Method(PS0X,0,Serialized)     /// Platform D0 Method for HD-A Controller
        {
          If(LEqual(\_SB.PC00.PAUD.ONTM, Zero)){    ///- Check if ONTM=0
            Return()
          }

          Divide(Subtract(Timer(), \_SB.PC00.PAUD.ONTM), 10000, , Local0) ///- Store Elapsed time in ms, ignore remainder
          Add(AUDD, VRRD, Local1) ///- Incorporate VR Rampup Delay
          If(LLess(Local0, Local1)) { ///- Do not sleep if already past the delay requirement audio
            ///- Delay for device init
            Sleep(Subtract(Local1, Local0)) ///- Sleep (AUDD + VRRD - time elapsed)
          }
        }

        Name(_PR0, Package(){\_SB.PC00.PAUD})
      }
    }