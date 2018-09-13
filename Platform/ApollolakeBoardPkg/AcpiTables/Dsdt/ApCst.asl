/** @file
  Intel Processor Power Management CST ACPI Code.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
