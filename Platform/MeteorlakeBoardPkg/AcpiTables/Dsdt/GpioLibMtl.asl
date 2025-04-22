/**@file
  GPIO ASL library

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Include ("GpioCommonV2.asl")

//
// GPIO Access Library
//
Scope (\_SB)
{
  Method (GDSC, 0, NotSerialized) {
    Return (GPCM)
  }

  //
  // Get GPIO maximal group
  // This is internal library function
  //
  Method (GMXG, 0x0, NotSerialized)
  {
    Return (GPIO_VER6_SOC_M_GPIO_GROUP_MAX)
  }

  Include ("GpioLibCommonV2.asl")
} // \_SB Scope

