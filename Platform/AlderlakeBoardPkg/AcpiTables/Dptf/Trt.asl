/** @file
  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope(\_SB.IETM)
{

   Name(TRT0, Package()    // TRT package when TFN1 is at 100% speed
   {
      // Source                           Target                    Influence  Period    Reserved
      Package(){\_SB.PC00.TCPU,           \_SB.PC00.LPCB.H_EC.SEN2, 40,        100,      0, 0, 0, 0},
      Package(){\_SB.PC00.LPCB.H_EC.CHRG, \_SB.PC00.LPCB.H_EC.SEN4, 20,        200,      0, 0, 0, 0}
    })

   Name(TRT1, Package()    // TRT package when TFN1 is at 100% speed
   {
      // Source                           Target                    Influence  Period    Reserved
      Package(){\_SB.PC00.TCPU,           \_SB.PC00.LPCB.ITE8.SEN2, 40,        100,      0, 0, 0, 0}
    })

    // _TRT (Thermal Relationship Table)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A variable-length Package containing a list of Thermal Relationship Packages as described below.
    //
    // Return Value Information
    //   Package {
    //   ThermalRelationship[0] // Package
    //    ...
    //   ThermalRelationship[n] // Package
    //   }
    //
    Method(_TRT,,,,PkgObj)
    {
      If (LEqual(ECON,1))
      {
        Return(TRT0)
      } Else {
        Return(TRT1)
      }
    } // End _TRT
}
