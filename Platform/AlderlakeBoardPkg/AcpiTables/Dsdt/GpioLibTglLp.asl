/**@file
 GPIO ASL library

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Include ("GpioCommon.asl")

//
// GPIO Access Library
//
Scope(\_SB)
{
  //
  // Get GPIO group information
  // This is internal library function
  //
  Method(GINF, 0x2, NotSerialized)
  {
    //
    // Arg0 - GPIO Group index
    // Arg1 - GPIO field type
    //
    Return (PCH_GPIO_GET_FIELD(Arg0,Arg1,GPCL))
  }

  //
  // Get GPIO maximal group
  // This is internal library function
  //
  Method(GMXG, 0x0, NotSerialized)
  {
    Return (GPIO_VER2_PCH_LP_GPIO_GROUP_MAX)
  }
} // \_SB Scope

Include ("GpioLibCommon.asl")
