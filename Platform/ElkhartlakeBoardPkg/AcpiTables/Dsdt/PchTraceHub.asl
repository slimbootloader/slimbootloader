/** @file
  PCH Trace Hub Device

  Copyright (c) 2018 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope (\_SB)
{
  Device(PCHT)
  {
    Name(_HID, "INTC1001")

    Method(_STA,0,Serialized)
    {
      Return(0x000F)
    }

    Name(_CRS, ResourceTemplate() {
        Memory32Fixed(ReadWrite,0xFE100000,0x100000) // 0xFE100000 - 0xFE1FFFFF // MTB BAR
        Memory32Fixed(ReadWrite,0xFC800000,0x800000) // 0xFC800000 - 0xFCFFFFFF // SW BAR
        })
  }
}
