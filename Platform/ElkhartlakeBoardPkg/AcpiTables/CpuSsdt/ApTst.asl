/** @file
  Intel Processor AP TST ACPI Code.

  Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

DefinitionBlock(
  "APTST.aml",
  "SSDT",
  0x02,
  "PmRef",
  "ApTst",
  0x3000
  )
{
  External(\PF00, IntObj)
  External(\TCNT, FieldUnitObj)
  External(\_SB.PR00, DeviceObj)
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
  External(\_SB.PR00._TPC, MethodObj)
  External(\_SB.PR00._PTC, MethodObj)
  External(\_SB.PR00._TSS, MethodObj)

  Scope(\_SB.PR01)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 1)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          1,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU1


  Scope(\_SB.PR02)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 2)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          2,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU2


  Scope(\_SB.PR03)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          3,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU3

  Scope(\_SB.PR04)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          4,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU4

  Scope(\_SB.PR05)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          5,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU5

  Scope(\_SB.PR06)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          6,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU6

  Scope(\_SB.PR07)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          7,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU7

  Scope(\_SB.PR08)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          8,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU8

  Scope(\_SB.PR09)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          9,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU9

  Scope(\_SB.PR10)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          10,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU10

  Scope(\_SB.PR11)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          11,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU11

  Scope(\_SB.PR12)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          12,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU12

  Scope(\_SB.PR13)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          13,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU13

  Scope(\_SB.PR14)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          14,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU14

  Scope(\_SB.PR15)
  {
    //
    // Max T-state available
    //
    Method(_TPC, 0)
    {
      Return(\_SB.PR00._TPC)
    }

    //
    // T-State Control/Status interface
    //
    Method(_PTC, 0)
    {
      Return(\_SB.PR00._PTC)
    }

    Method(_TSS, 0)
    {
      Return(\_SB.PR00._TSS)
    }

    //
    // T-State Dependency
    //
    Method(_TSD, 0)
    {
      //
      // IF four cores are supported/enabled && !(direct access to MSR)
      //    Report 4 processors and SW_ANY as the coordination
      // IF two cores are supported/enabled && !(direct access to MSR)
      //    Report 2 processors and SW_ANY as the coordination type
      //  ELSE
      //    Report 1 processor and SW_ALL as the coordination type (domain 3)
      //
      //  PDCx[2] = OSPM is capable of direct access to On
      //    Demand throttling MSR
      //

      If(LNot(And(PF00,4)))
      {
        Return(Package(){ // SW_ANY
          Package(){
            5,            // # entries.
            0,            // Revision.
            0,            // Domain #.
            0xFD,         // Coord Type- SW_ANY
            TCNT          // # processors.
          }
        })
      }
      Return(Package(){   // SW_ALL
        Package(){
          5,              // # entries.
          0,              // Revision.
          15,              // Domain #.
          0xFC,           // Coord Type- SW_ALL
          1               // # processors.
        }
      })
    }
  }  // End of CPU15
} // End of Definition Block

