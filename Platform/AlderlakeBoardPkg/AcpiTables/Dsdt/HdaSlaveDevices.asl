/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//
// SoundWire Devices definition
//
// Address (_ADR) encoding:
// Bits [63:52] - Reserved (0)
// Bits [51:48]  - Link ID
// Zero-based SoundWire Link number. For single-link controllers, this value shall be 0x0
// Bits [47:0]  - Device ID
// DeviceID Encoding as described in the SoundWire 1.0 specification, Table 80
// For example:
// SNDW.SWD0 (ALC701 on Link#0): _ADR(0x000010025d070100) - LinkID[51:48] = 0, DevID[47:0]: 0x10025D070100
// SNDW.SWD6 (ALC700 on Link#2): _ADR(0x000210025d070000) - LinkID[51:48] = 2, DevID[47:0]: 0x10025D070000
//

#define N_HDA_SNDW_LINK_ID   48
#define V_HDA_SNDW_LINK_ID_0 (0x0000 << N_HDA_SNDW_LINK_ID)
#define V_HDA_SNDW_LINK_ID_1 (0x0001 << N_HDA_SNDW_LINK_ID)
#define V_HDA_SNDW_LINK_ID_2 (0x0002 << N_HDA_SNDW_LINK_ID)
#define V_HDA_SNDW_LINK_ID_3 (0x0003 << N_HDA_SNDW_LINK_ID)

#define V_HDA_SNDW_DID_REALTEK_ALC700 0x10025D070000
#define V_HDA_SNDW_DID_REALTEK_ALC701 0x10025D070100

// SoundWire Controller Scope
Scope(\_SB.PC00.HDAS.SNDW)
{
  // SoundWire Device 0..M

  // SoundWire Device #0 on Controller #0 (Realtek ALC701)
  // _ADR: 0x000010025d070100
  Device(SWD0)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_0 | V_HDA_SNDW_DID_REALTEK_ALC701))
    Include ("HdaSoundWireCodec.asl")
  }

  // SoundWire Device #1 on Controller #1 (Realtek ALC701)
  // _ADR: 0x000110025d070100
  Device(SWD1)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_1 | V_HDA_SNDW_DID_REALTEK_ALC701))
    Include ("HdaSoundWireCodec.asl")
  }

  // SoundWire Device #2 on Controller #2 (Realtek ALC701)
  // _ADR: 0x000210025d070100
  Device(SWD2)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_2 | V_HDA_SNDW_DID_REALTEK_ALC701))
    Include ("HdaSoundWireCodec.asl")
  }

  // SoundWire Device #3 on Controller #3 (Realtek ALC701)
  // _ADR: 0x000310025d070100
  Device(SWD3)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_3 | V_HDA_SNDW_DID_REALTEK_ALC701))
    Include ("HdaSoundWireCodec.asl")
  }

  // SoundWire Device #4 on Controller #0 (Realtek ALC700)
  // _ADR: 0x000010025d070000
  Device(SWD4)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_0 | V_HDA_SNDW_DID_REALTEK_ALC700))
    Include ("HdaSoundWireCodec.asl")
  }

  // SoundWire Device #5 on Controller #1 (Realtek ALC700)
  // _ADR: 0x000110025d070000
  Device(SWD5)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_1 | V_HDA_SNDW_DID_REALTEK_ALC700))
    Include ("HdaSoundWireCodec.asl")
  }

  // SoundWire Device #6 on Controller #2 (Realtek ALC700)
  // _ADR: 0x000210025d070000
  Device(SWD6)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_2 | V_HDA_SNDW_DID_REALTEK_ALC700))
    Include ("HdaSoundWireCodec.asl")
  }

  // SoundWire Device #7 on Controller #3 (Realtek ALC700)
  // _ADR: 0x000310025d070000
  Device(SWD7)
  {
    Name (_ADR, (V_HDA_SNDW_LINK_ID_3 | V_HDA_SNDW_DID_REALTEK_ALC700))
    Include ("HdaSoundWireCodec.asl")
  }
} // END SCOPE SNDW

//
// I2S Devices definition
//
If (I2SB == 0) {
  // I2C0 Controller Scope
  Scope(\_SB.PC00.I2C0) {
    Include ("HdaI2sCodec.asl")
  } // END SCOPE I2C0
}
ElseIf (I2SB == 1) {
  // I2C1 Controller Scope
  Scope(\_SB.PC00.I2C1) {
    Include ("HdaI2sCodec.asl")
  } // END SCOPE I2C1
}
ElseIf (I2SB == 2) {
  // I2C2 Controller Scope
  Scope(\_SB.PC00.I2C2) {
    Include ("HdaI2sCodec.asl")
  } // END SCOPE I2C2
}
ElseIf (I2SB == 3) {
  // I2C3 Controller Scope
  Scope(\_SB.PC00.I2C3) {
    Include ("HdaI2sCodec.asl")
  } // END SCOPE I2C3
}
ElseIf (I2SB == 4) {
  // I2C4 Controller Scope
  Scope(\_SB.PC00.I2C4) {
    Include ("HdaI2sCodec.asl")
  } // END SCOPE I2C4
}
ElseIf (I2SB == 5) {
  // I2C5 Controller Scope
  Scope(\_SB.PC00.I2C5) {
    Include ("HdaI2sCodec.asl")
  } // END SCOPE I2C5
}

//
// Realtek codec definition
//
Scope(\_SB.PC00.HDAS)
{
  //
  // Realtek codec
  //
  Device(RTKC) {
    Name (_ADR, 0x01)
    Method (_STA) {
      If (PXDC == 1) {
        Return (0x0)
      } else {
        Return (0xF)
      }
    }
  } // END Device(RTKC)
} // END Scope(HDAS)
