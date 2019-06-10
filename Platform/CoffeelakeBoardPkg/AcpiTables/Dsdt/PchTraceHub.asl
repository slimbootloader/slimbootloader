/** @file
  PCH Trace Hub Device

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0)
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

