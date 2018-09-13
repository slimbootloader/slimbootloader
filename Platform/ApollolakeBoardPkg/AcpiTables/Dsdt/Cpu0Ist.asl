/** @file
  Intel Processor Power Management ACPI Code.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


  Scope(\_PR.CPU0)
  {
    //
    // Report supported P-States.
    //
    Name(_PPC, 0)

    //
    // NOTE:  For CMP systems; this table is not loaded unless
    //        the required driver support is present.
    //        So, we do not check for those cases here.
    //
    // CFGD[0]  = EIST Capable/Enabled
    // PDCx[0]  = OS Capable of Hardware P-State control
    //
    Method(_PCT,0)
    {
      // Update the _PPC value
      //
      Store (0x00, \_PR.CPU0._PPC)

      If(LAnd(And(\_PR.CFGD,0x0001), And(PDC0,0x0001)))
      {
        Return(Package()  // Native Mode
        {
          ResourceTemplate(){Register(FfixedHW, 0, 0, 0)},
          ResourceTemplate(){Register(FfixedHW, 0, 0, 0)}
        })
      }
      // For all other cases, report control through the
      // SMI interface.  (The port used for SMM control is fixed up
      // by the initialization code.)
      //
      Return(Package()        // SMM Mode
      {                
        ResourceTemplate(){Register(FfixedHW, 0, 0, 0)},
        ResourceTemplate(){Register(FfixedHW, 0, 0, 0)}
      })
    }

    Method(_PSS,0)
    {
      Return(SPSS)    // PSS entry
    }

    Name(SPSS,Package()
    {
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000},
      Package(){0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000}
    })

    Method(_PSD,0)
    {
      Return(Package(){    // HW_ALL
              Package(){
                         5,              // # entries.
                         0,              // Revision.
                         0,              // Domain #.
                         0xFE,           // Coord Type- HW_ALL.
                         NLPC            // # processors.
              }
            })
    }

  }
