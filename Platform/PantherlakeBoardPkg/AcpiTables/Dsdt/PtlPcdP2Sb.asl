/**@file

  PTL SoC P2SB devices description.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PchReservedResources.h"

Scope (\_SB.PC00) {
  Device (SBSC) {
    Name (_HID,EISAID ("PNP0C02")) // motherboard resource
    Name (_UID,"SOCP2SB")
    Name (_STA,0x8) // device present and decodes its resources, but not to be displayed in OSPM
    Name (SBRG, 0)
    Store (\SBRG, SBRG)
    Include ("PcrLib.asl")
  }

  Device (SBIE) {
    Name (_HID,EISAID ("PNP0C02")) // motherboard resource
    Name (_UID,"SECP2SB")
    Name (_STA,0x8) // device present and decodes its resources, but not to be displayed in OSPM
    Name (SBRG, PCH_PCR_SECOND_BASE_ADDRESS)
    Include ("PcrLib.asl")
  }
}
