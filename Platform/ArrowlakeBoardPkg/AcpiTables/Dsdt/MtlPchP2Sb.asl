/**@file

  MTL PCH P2SB devices description.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (SBPC) {
  Name (_HID,EISAID ("PNP0C02")) // motherboard resource
  Name (_UID,"PCHP2SB")
  Name (_STA,0x8) // device present and decodes its resources, but not to be displayed in OSPM
  Name (SBRG, 0)
  Store (^SBRG, SBRG)
  Include ("PcrLib.asl")
}