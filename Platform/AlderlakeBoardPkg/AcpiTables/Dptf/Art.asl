/** @file
  Intel ACPI Reference Code for Intel Dynamic Power Performance Management

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/


Scope(\_SB.IETM)
{

    Name(ART1, Package()    // ART package when in Quiet Mode / Passive Cooling Mode
    {
      0, // Revision
      // Source                           Target                    Weight, AC0MaxLevel, AC1MaxLevel, AC21MaxLevel, AC31MaxLevel, AC41MaxLevel, AC51MaxLevel, AC61MaxLevel, AC71MaxLevel, AC81MaxLevel, AC91MaxLevel
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.TCPU,           100,             80,          60,           40,           30,           20,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN2, 100,             80,          60,           30,    0xFFFFFFFF,  0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN3, 100,     0xFFFFFFFF,  0xFFFFFFFF,   0xFFFFFFFF,           80,           60,           30,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN4, 100,             80,          60,           30,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN5, 100,             80,          60,           30,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
    })

    Name(ART0, Package()    // ART package when in Performance Mode / Active Cooling Mode
    {
      0,    // Revision
      // Source                           Target                    Weight, AC0MaxLevel, AC1MaxLevel, AC21MaxLevel, AC31MaxLevel, AC41MaxLevel, AC51MaxLevel, AC61MaxLevel, AC71MaxLevel, AC81MaxLevel, AC91MaxLevel
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.TCPU,           100,            100,          80,           50,           40,           30,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN2, 100,             80,          50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN3, 100,     0xFFFFFFFF,  0xFFFFFFFF,   0xFFFFFFFF,          100,           80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN4, 100,            100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.H_EC.TFN1, \_SB.PC00.LPCB.H_EC.SEN5, 100,            100,          80,           50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF}
    })

    Name(ART2, Package()    // ART package when in Performance Mode / Active Cooling Mode
    {
      0,    // Revision
      // Source                           Target                    Weight, AC0MaxLevel, AC1MaxLevel, AC21MaxLevel, AC31MaxLevel, AC41MaxLevel, AC51MaxLevel, AC61MaxLevel, AC71MaxLevel, AC81MaxLevel, AC91MaxLevel
      Package(){\_SB.PC00.LPCB.ITE8.TFN1, \_SB.PC00.TCPU,           100,            100,          80,           50,           40,           30,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF},
      Package(){\_SB.PC00.LPCB.ITE8.TFN1, \_SB.PC00.LPCB.ITE8.SEN1, 100,             80,          50,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF}
    })

    // _ART (Active Cooling Relationship Table)
    //
    // Arguments:
    //   None
    // Return Value:
    //   A variable-length Package containing a Revision ID and a list of Active Relationship Packages as described below:
    //
    // Return Value Information
    //   Package {
    //   Revision, // Integer - Current revision is: 0
    //   ActiveRelationship[0] // Package
    //   ...
    //   ActiveRelationship[n] // Package
    //   }
    //
    Method(_ART,,,,PkgObj)
    {
      If (LEqual(ECON,1))
      {
        If(\_SB.PC00.LPCB.H_EC.SEN3.CTYP)
        {
          Return(ART1)
        } Else {
          Return(ART0)
        }
      } Else {
        Return(ART2)
      }
    } // End _ART
}
