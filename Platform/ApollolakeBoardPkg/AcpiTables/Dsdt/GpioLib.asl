/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

//include ("BxtPGpioDefine.asl")
//
// GPIO Access Library
//
Scope(\_SB)
{
  //
  // Get Pad Configuration DW0 register value
  //
  Method(GPC0, 0x1, Serialized)
  {
    //
    // Arg0 - (GpioCommunityAddress + Gpio MMIO_Offset)
    //

    // Local0 = (GpioCommunityAddress + Gpio MMIO_Offset)
    Store(Arg0, Local0)
    OperationRegion(PDW0, SystemMemory, Or (P2BA, Local0), 4)
    Field(PDW0, AnyAcc, NoLock, Preserve) {
      Offset(0x0),
        TEMP,32
    }
    Return(TEMP)
  }

  //
  // Set Pad Configuration DW0 register value
  //
  Method(SPC0, 0x2, Serialized)
  {
    //
    // Arg0 - (GpioCommunityAddress + Gpio MMIO_Offset)
    // Arg1 - Value for DW0 register
    //

    // Local0 = (GpioCommunityAddress + Gpio MMIO_Offset)
    Store(Arg0, Local0)
    OperationRegion(PDW0, SystemMemory, Or (P2BA, Local0), 4)
    Field(PDW0, AnyAcc, NoLock, Preserve) {
      Offset(0x0),
        TEMP,32
    }
    Store(Arg1,TEMP)
  }

  //
  // Get Pad Configuration DW1 register value
  //
  Method(GPC1, 0x1, Serialized)
  {
    //
    // Arg0 -Local0 = (GpioCommunityAddress + Gpio MMIO_Offset) + 0x4
    //

    // Local0 = (GpioCommunityAddress + Gpio MMIO_Offset) + 0x4
    Store( Add( Arg0, 0x4), Local0)
    OperationRegion(PDW1, SystemMemory, Or (P2BA, Local0), 4)
    Field(PDW1, AnyAcc, NoLock, Preserve) {
      Offset(0x0),
        TEMP,32
    }
    Return(TEMP)
  }

  //
  // Set Pad Configuration DW1 register value
  //
  Method(SPC1, 0x2, Serialized)
  {
    //
    // Arg0 - (GpioCommunityAddress + Gpio MMIO_Offset) + 0x4
    // Arg1 - Value for DW1 register
    //

    // Local0 = (GpioCommunityAddress + Gpio MMIO_Offset) + 0x4
    Store( Add( Arg0, 0x4), Local0)
    OperationRegion(PDW1, SystemMemory, Or (P2BA, Local0), 4)
    Field(PDW1, AnyAcc, NoLock, Preserve) {
      Offset(0x0),
        TEMP,32
    }
    Store(Arg1,TEMP)
  }

  //
  // Get GPI Input Value
  //
  Method(GGIV, 0x1, Serialized)
  {
    //
    // Arg0 - (GpioCommunityAddress + Gpio MMIO_Offset)
    //

    // Local0 = (GpioCommunityAddress + Gpio MMIO_Offset)
    Store( Arg0, Local0)
    OperationRegion(PDW0, SystemMemory, Or (P2BA, Local0), 4)
    Field(PDW0, AnyAcc, NoLock, Preserve) {
      Offset(0x0),
        ,    1,
        TEMP,1,
        ,    30
    }
    Return(TEMP)
  }

  //
  // Get GPO Output Value
  //
  Method(GGOV, 0x1, Serialized)
  {
    //
    // Arg0 - (GpioCommunityAddress + Gpio MMIO_Offset)
    //

    // Local0 = (GpioCommunityAddress + Gpio MMIO_Offset)
    Store( Arg0, Local0)
    OperationRegion(PDW0, SystemMemory, Or (P2BA, Local0), 4)
    Field(PDW0, AnyAcc, NoLock, Preserve) {
      Offset(0x0),
        TEMP,1,
        ,    31
    }
    Return(TEMP)
  }

  //
  // Set GPO Output Value
  //
  Method(SGOV, 0x2, Serialized)
  {
    //
    // Arg0 - (GpioCommunityAddress + Gpio MMIO_Offset)
    // Arg1 - Value of GPIO Tx State
    //

    // Local0 = (GpioCommunityAddress + Gpio MMIO_Offset)
    Store( Arg0, Local0)
    OperationRegion(PDW0, SystemMemory, Or (P2BA, Local0), 4)
    Field(PDW0, AnyAcc, NoLock, Preserve) {
      Offset(0x0),
        TEMP,1,
        ,    31
    }
    Store(Arg1,TEMP)
  }

} // \_SB Scope


