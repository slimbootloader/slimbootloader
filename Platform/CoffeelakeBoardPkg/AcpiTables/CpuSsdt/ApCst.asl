/** @file
  Intel Processor Power Management CST ACPI Code.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock (
  "APCST.aml",
  "SSDT",
  2,
  "PmRef",
  "ApCst",
  0x3000
  )
{
External(\_SB.PR01, DeviceObj)
External(\_SB.PR02, DeviceObj)
External(\_SB.PR03, DeviceObj)
External(\_SB.PR04, DeviceObj)
External(\_SB.PR05, DeviceObj)
External(\_SB.PR06, DeviceObj)
External(\_SB.PR07, DeviceObj)
External(\_SB.PR08, DeviceObj)
External(\_SB.PR09, DeviceObj)
External(\_SB.PR10, DeviceObj)
External(\_SB.PR11, DeviceObj)
External(\_SB.PR12, DeviceObj)
External(\_SB.PR13, DeviceObj)
External(\_SB.PR14, DeviceObj)
External(\_SB.PR15, DeviceObj)
External(\_SB.PR00._CST)

  Scope(\_SB.PR01)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR02)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR03)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR04)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR05)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR06)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR07)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR08)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR09)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR10)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR11)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR12)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR13)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR14)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

  Scope(\_SB.PR15)
  {
    Method(_CST,0)
    {
      //
      // Return P0's _CST object.
      //
      Return(\_SB.PR00._CST)
    }
  }

}  // End of Definition Block
