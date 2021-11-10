/** @file
  Intel Processor Power Management CST ACPI Code.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  Scope(\_PR.CPU1)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_PR.CPU0._CST)
    }
  }

  Scope(\_PR.CPU2)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_PR.CPU0._CST)
    }
  }

  Scope(\_PR.CPU3)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_PR.CPU0._CST)
    }
  }
