/**@file

  MTL SoC P2SB devices description.

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include "PchReservedResources.h"

Scope (\_SB.PC00) {
  // MTL SoC sideband device
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
    Name (_UID,"IOEP2SB")
    Name (_STA,0x8) // device present and decodes its resources, but not to be displayed in OSPM
    Name (SBRG, 0x3FFF0000000)
    Include ("PcrLib.asl")
  }
}
