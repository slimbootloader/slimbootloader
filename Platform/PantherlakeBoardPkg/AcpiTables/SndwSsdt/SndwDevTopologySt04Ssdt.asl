/** @file
  SoundWire topology definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock ("SndwDevTopologySt04Ssdt.aml", "SSDT", 2, "INTEL", "St04Ssdt", 0x1000) {

  External(\_SB.PC00.HDAS.IDA.SNDW, DeviceObj)

  Scope (\_SB)
  {
    Device (AUDC)
    {
      Name (_HID, "ACPI0018")
      Include ("AudioComposition_722.asl")
    } //End AUDC
  }

  Scope (\_SB.PC00.HDAS.IDA.SNDW)
  {
    // SoundWire Device (SWD0) on Link #3 (Realtek ALC722 : UID_00)
    // _ADR: Link_3 Bit[51:48]=3 | ALC722_ID Bit[47:0]=30025d072201 | UID_00 Bit[43:40]=0
    Device (SWD0)
    {
      Name (_ADR, 0x000330025d072201)
      Include ("Peripheral_722.asl")
    }
  }
}
