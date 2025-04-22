/** @file
  Graphics device table

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

Scope (\_SB.PCI0)
{
  Device (GFX0) {
    Name (_ADR, 0x00020000)
    include ("IgfxOpRn.asl")
  }
}
