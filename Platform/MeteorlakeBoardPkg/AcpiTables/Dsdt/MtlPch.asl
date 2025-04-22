/**@file
  MTL PCH ASL description

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
Name (MNVB, 0xFFFF0000)
Name (MNVL, 0xAA55)

Scope (\_SB.PC02) {
  Include ("MtlPchNvs.asl")
  Include ("SataMtlPch.asl")
  Include ("MtlPchHda.asl")
}
