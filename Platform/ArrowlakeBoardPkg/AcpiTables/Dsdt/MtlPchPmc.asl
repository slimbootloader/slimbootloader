/**@file
  Intel ACPI Reference Code for MTL PCH Power Management Control

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Device (PPMC) {
  Name (_HID,EISAID ("PNP0C02")) // motherboard resource
  Name (_UID,"MTPPMC")
  Name (_STA,0x8) // device present and decodes its resources, but not to be displayed in OSPM
  Include ("Pmc.asl")
}