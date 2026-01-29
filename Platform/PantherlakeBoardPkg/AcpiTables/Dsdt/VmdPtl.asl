/**@file
  Intel ACPI Reference Code for VMD device support

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PcieRegs.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PchBdfAssignment.h>

#define VMD_STORAGE_REMAP_PORT_SATA         0
#define VMD_STORAGE_REMAP_PORT_PCIE_RP      1

#define ROOT_BRIDGE_COMMON PC00

//
// SSDT RTD3 imports
//
External (\_SB.ROOT_BRIDGE_COMMON.RP01.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP01.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP02.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP02.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP03.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP03.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP04.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP04.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP05.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP05.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP06.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP06.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP07.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP07.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP08.PON, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP08.POFF, MethodObj)
External (\_SB.ROOT_BRIDGE_COMMON.RP09.PON, MethodObj)      // P.E.G 0/6/0
External (\_SB.ROOT_BRIDGE_COMMON.RP09.POFF, MethodObj)     // P.E.G 0/6/0
External (\_SB.ROOT_BRIDGE_COMMON.RP10.PON, MethodObj)      // P.E.G 0/6/1
External (\_SB.ROOT_BRIDGE_COMMON.RP10.POFF, MethodObj)     // P.E.G 0/6/1
External (\_SB.ROOT_BRIDGE_COMMON.RP11.PON, MethodObj)      // P.E.G 0/6/2
External (\_SB.ROOT_BRIDGE_COMMON.RP11.POFF, MethodObj)     // P.E.G 0/6/2
External (\_SB.ROOT_BRIDGE_COMMON.RP12.PON, MethodObj)      // P.E.G 0/1/0
External (\_SB.ROOT_BRIDGE_COMMON.RP12.POFF, MethodObj)     // P.E.G 0/1/0
External (\VDSD, IntObj) // VMD _DSD Method - 1: Expose 0: Hide
External (\PBUS) // PCH ROOT PORT BUS Number

External (VPON, MethodObj)
External (VPOF, MethodObj)
External (VSON, MethodObj)
External (VSOF, MethodObj)
External (RLA, MethodObj)
External (RL23, MethodObj)

External (\XBAS)

Scope (\_SB.PC00) {
  Device (VMD0) {
    //
    // _ADR format for devices mapped under VMD is defined by RST OS driver team and provided this format
    //
    Name (_ADR, 0x000E0000)

    If (CondRefOf (\VDSD)) {
      If (LEqual (\VDSD, 1)) {
        //
        // Windows defined _DSD that informs the OS
        // that it should support D3 on this storage device.
        //
        Name (_DSD, Package () {
          ToUUID ("5025030F-842F-4AB4-A561-99A5189762D0"),
          Package () {
            Package (2) {"StorageD3Enable", 1}
          }
        })
      }
    }

    Method (_PS0, 0, Serialized) {
      ADBG ("VMD _PS0")
    }

    Method (_PS3, 0, Serialized) {
      ADBG ("VMD _PS3")
    }

    //
    // Returns PCI config base address
    // in VMD CFGBAR address space.
    // Arg0 - Remapped device bus number
    // Arg1 - Remapped device device number
    // Arg2 - Remapped device function number
    // Return - address of the remapped device PCI config space address
    //
    Method (CBAR, 3, Serialized) {

      Name (VCFB, 0) // VMD PCI config base address
      Store (\XBAS, VCFB)
      Or (VCFB, ShiftLeft (0, 20), VCFB)
      Or (VCFB, ShiftLeft (0xE, 15), VCFB)
      Or (VCFB, ShiftLeft (0, 12), VCFB)

      //
      // This is just a regular PCI access to VMD controller.
      // We have to use SystemMemory region since PCI region access
      // can be blocked by OSPM if PCI_Config is used
      //
      OperationRegion (VCFG, SystemMemory, VCFB, 0x100)
      Field (VCFG, DWordAcc, NoLock, Preserve) {
        Offset (R_PCI_BAR0_OFFSET),
        BAR0, 64,
      }

      Name (DCFB, 0) // VMD remapped device config base address
      Store (BAR0, DCFB)
      And (DCFB, Not (0xF), DCFB)
      Or (DCFB, ShiftLeft (Arg0, 20), DCFB)
      Or (DCFB, ShiftLeft (Arg1, 15), DCFB)
      Or (DCFB, ShiftLeft (Arg2, 12), DCFB)

      Return (DCFB)
    }


//
// ACPI Address Schema for VMD Remapped NVMe Devices is defined by
// RST OS driver team and provided this format
//
//----------------------------------------------------------------------------
// | Interface | Bus (low) | Dev     | Func      | Bus (high)     | Reserved |
//----------------------------------------------------------------------------
// | 31        | 30..24    | 23..19  | 18..16    | 15             | 14..0    |
// | (1 bit)   | (7bits)   | (5bits) | (3bits)   | (1bit)         | (15bits) |
//----------------------------------------------------------------------------
// | PCIe = 0  | VMD RP    | VMD RP  | VMD RP    |VMD RP          | 7FFF     |
// |           |PCI Bus Nr | DeviceNr| FunctionNr|PCI BusNr       |          |
// |           | (bits 6 0)|         |           |(inverted bit 7)|          |
//----------------------------------------------------------------------------

//
// ACPI Address Schema for VMD Remapped SATA Controller is defined by
// RST OS driver team and provided this format
//
//---------------------------------------------------------------------------------------
// | Interface | Bus (low) | Dev     | Func      | Bus (high)     | Reserved | ATA Port |
//---------------------------------------------------------------------------------------
// | 31        | 30..24    | 23..19  | 18..16    | 15             | 14..4    | 3..0     |
// | (1 bit)   | (7bits)   | (5bits) | (3bits)   | (1bit)         | (11bits) | (4bits)  |
//---------------------------------------------------------------------------------------
// | SATA = 0  | VMD       | VMD     | VMD       |VMD             | 7FF      | 0-7      |
// |           |PCI Bus Nr | DeviceNr| FunctionNr|PCI BusNr       |          |          |
// |           | (bits 6 0)|         |           |(inverted bit 7)|          |          |
//---------------------------------------------------------------------------------------
    //
    // Returns ACPI Address for VMD remapped NVMe/SATA devices
    // Arg0 - Interface
    // Arg1 - Remapped Device Bus number
    // Arg2 - Remapped Device Dev number
    // Arg3 - Remapped Device func number
    // Arg4 - Remapped ATA port number
    // Return - ACPI Address for remapped device NVMe /SATA devices
    //
    Method (VADR, 5, Serialized) {

      Name (AADR, 0) // ACPi address of the remapped device
      Or (AADR, ShiftLeft (Arg0, 31), AADR) // Interface 1: PCIe, 0: SATA
      Or (AADR, ShiftLeft (And (Arg1, 0x7F), 24), AADR) // Remapped Device Bus number (lower bits 6-0)
      Or (AADR, ShiftLeft (Arg2, 19), AADR) // Remapped device Dev number
      Or (AADR, ShiftLeft (Arg3, 16), AADR) // Remapped device Func number

      If (LEqual(ShiftRight(And (Arg1, 0x80), 7), 0)) {
        Or (AADR, ShiftLeft (1, 15), AADR) // Remapped Device Bus number (inverted bit 7)
      }

      If (LEqual (Arg0, 1)) {
        Or (AADR, 0x7FFF, AADR)
      } Else {
        Store (0x7FF, Local0)
        Or (ShiftLeft (Local0, 14), Arg4, Local0)
        Or (AADR, Local0, AADR)
      }

      Return (AADR)
    }


    //
    // PCIe root ports remapped under VMD controller
    // RPB -  remapped root port bus number
    // RPD -  remapped root port device number
    // RPF -  remapped root port function number
    // RSPT - remapped slot port type
    // RSPI - remapped slot port index
    //

    //
    // PCD PCIe root ports remapped under VMD controller
    //

    //
    // PCD PCIE Root Port B0D28F0 #01
    //
    Device (PP01) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 0)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 0)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIE Root Port B0D28F1 #02
    //
    Device (PP02) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 1)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 1)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIE Root Port B0D28F2 #03
    //
    Device (PP03) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 2)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 2)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIE Root Port B0D28F3 #04
    //
    Device (PP04) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 3)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 3)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIE Root Port B0D28F4 #05
    //
    Device (PP05) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 4)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 4)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIE Root Port B0D28F5 #06
    //
    Device (PP06) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 5)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 5)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIE Root Port B0D28F6 #07
    //
    Device (PP07) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 6)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 6)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIE Root Port B0D28F7 #08
    //
    Device (PP08) {
      Name (RPB, 0)
      Store (PBUS, RPB)
      Name (RPD, 28)
      Name (RPF, 7)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 7)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // PCD PCIe root ports remapped under VMD controller
    //

    // PCIE Root Port B0D6F0 #9
    Device (SP09) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 24)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D6F1 #10
    Device (SP10) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9)
      Name (RPF, 1)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 25)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D6F2 #11
    Device (SP11) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9)
      Name (RPF, 2)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 26)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D6F3 #12
    Device (SP12) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9)
      Name (RPF, 3)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 27)
      Method (_ADR, 0) {
        Return (VADR (RSPT, RPB, RPD, RPF, RSPI))
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // RTD3 support for logical volumes
    //
    Include ("VmdRaid.asl")

    //
    // Is D3Cold supported for given remapped storage port
    // Support for D3Cold is indicated by presence of POFF method.
    // Arg0  Storage remap port type
    // Arg1  Index of the storage remap port
    //
    Method (D3CV, 2, Serialized) {
      Switch (ToInteger (Arg0)) {
        //
        // For PCIe root ports the support for D3Cold transition
        // is indicated by the presence of the POFF method.
        // We support up to 24 root ports which is the current
        // limit on the desktop SKUs of the PCH.
        //
        Case (VMD_STORAGE_REMAP_PORT_PCIE_RP) {
          Switch (ToInteger (Arg1)) {
            // PCH PCIe
            Case (0) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP01.POFF)) {Return (1)}
            }
            Case (1) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP02.POFF)) {Return (1)}
            }
            Case (2) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP03.POFF)) {Return (1)}
            }
            Case (3) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP04.POFF)) {Return (1)}
            }
            Case (4) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP05.POFF)) {Return (1)}
            }
            Case (5) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP06.POFF)) {Return (1)}
            }
            Case (6) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP07.POFF)) {Return (1)}
            }
            Case (7) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP08.POFF)) {Return (1)}
            }

            // PCD PCIe
            Case (24) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP09.POFF)) {Return (1)}
            }
            Case (25) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP10.POFF)) {Return (1)}
            }
            Case (26) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP11.POFF)) {Return (1)}
            }
            Case (27) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP12.POFF)) {Return (1)}
            }

            Default {Return (0)}
          }
        }
        Default {Return (0)}
      }
      Return (0)
    }

    //
    // Turn on power to the remapped storage
    // Arg0  Storage remap port type
    // Arg1  Index of the storage remap port
    //
    Method (VDON, 2, Serialized) {
      Switch (ToInteger (Arg0)) {
        Case (VMD_STORAGE_REMAP_PORT_PCIE_RP) {
          Switch (ToInteger (Arg1)) {
            // PCD PCIe
            Case (0) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP01.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP01.PON ()}
            }
            Case (1) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP02.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP02.PON ()}
            }
            Case (2) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP03.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP03.PON ()}
            }
            Case (3) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP04.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP04.PON ()}
            }
            Case (4) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP05.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP05.PON ()}
            }
            Case (5) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP06.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP06.PON ()}
            }
            Case (6) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP07.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP07.PON ()}
            }
            Case (7) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP08.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP08.PON ()}
            }

            // PCD PCIe
            Case (24) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP09.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP09.PON ()}
            }
            Case (25) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP10.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP10.PON ()}
            }
            Case (26) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP11.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP11.PON ()}
            }
            Case (27) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP12.PON)) {\_SB.ROOT_BRIDGE_COMMON.RP12.PON ()}
            }
            Default {Return (0)}
          }
        }
        Default {Return (0)}
      }
      Return (0)
    }

    //
    // Turn off power to the remapped port PCIe slot
    // Arg0  Storage remap port type
    // Arg1  Index of the storage remap port
    //
    Method (VDOF, 2, Serialized) {
      Switch (ToInteger (Arg0)) {
        Case (VMD_STORAGE_REMAP_PORT_PCIE_RP) {
          Switch (ToInteger (Arg1)) {
            // PCD PCIe
            Case (0) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP01.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP01.POFF ()}
            }
            Case (1) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP02.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP02.POFF ()}
            }
            Case (2) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP03.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP03.POFF ()}
            }
            Case (3) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP04.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP04.POFF ()}
            }
            Case (4) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP05.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP05.POFF ()}
            }
            Case (5) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP06.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP06.POFF ()}
            }
            Case (6) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP07.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP07.POFF ()}
            }
            Case (7) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP08.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP08.POFF ()}
            }

            // PCD PCIe
            Case (24) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP09.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP09.POFF ()}
            }
            Case (25) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP10.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP10.POFF ()}
            }
            Case (26) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP11.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP11.POFF ()}
            }
            Case (27) {
              If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.RP12.POFF)) {\_SB.ROOT_BRIDGE_COMMON.RP12.POFF ()}
            }
            Default {Return (0)}
          }
        }
        Default {Return (0)}
      }
      Return (0)
    }

    // VLON  : Method to turn ON ports per port address
    // Arg0 : Package size
    // Arg1 :  List of Port Address
    Method (VLON,2,Serialized,,, {IntObj, PkgObj}) {
      Name (DRVA, 0)  /// Storage for ACPI Address of the port
      Store (0, Local0) // use this as the index value
      Store (Arg0, Local1)
      ADBG (Concatenate ("VLON: The size of the buffer:", ToInteger (Local1)))
      While (LGreater (Local1, Local0))
      {
        Store (ToInteger (DerefOf (Index (Arg1, Local0))), DRVA)
        ADBG (Concatenate ("VLON: ACPI Address of the port:", ToInteger (DRVA)))
        Switch (ToInteger (DRVA)) {
          // PCD PCIe Root Ports
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP01._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP01")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP01.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP02._ADR())) {
            ADBG ("VMD Logical volume - PCD PCIe RP02")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP02.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP03._ADR())) {
            ADBG ("VMD Logical volume - PCD PCIe RP03")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP03.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP04._ADR())) {
            ADBG ("VMD Logical volume - PCD PCIe RP04")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP04.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP05._ADR())) {
            ADBG ("VMD Logical volume - PCD PCIe RP05")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP05.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP06._ADR())) {
            ADBG ("VMD Logical volume - PCD PCIe RP06")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP06.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP07._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP07")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP07.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP08._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP08")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP08.VPON ()
          }

          // PCD PCIe ports6
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP09._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP09 0/6/0")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP09.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP10._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP09 0/6/1")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP10.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP11._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP11 0/6/2")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP11.VPON ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP12._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP12 0/6/3")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP12.VPON ()
          }
        } //Switch (DRVA)
        Increment (Local0)
      } // while
    }

    // VLOF  : Method to turn OFF ports per port addres
    // Arg0 : Package size
    // Arg1 :  List of Port Address
    Method (VLOF,2,Serialized,,, {IntObj, PkgObj}) {
      Name (DRVA, 0)  /// Storage for ACPI Address of the port
      Store (0, Local0) // use this as the index value
      Store (Arg0, Local1)
      ADBG (Concatenate ("VLOF: The size of the Pkg:", ToInteger (Local1)))
      While (LGreater (Local1, Local0))
      {
        Store (ToInteger (DerefOf (Index (Arg1, Local0))), DRVA)
        ADBG (Concatenate ("VLOF: ACPI Address of the port:", ToInteger (DRVA)))
        Switch (ToInteger (DRVA)) {
          // PCD PCIe Root Ports
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP01._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP01")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP01.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP02._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP02")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP02.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP03._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP03")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP03.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP04._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP04")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP04.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP05._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP05")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP05.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP06._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP06")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP06.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP07._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP07")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP07.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.PP08._ADR ())) {
            ADBG ("VMD Logical volume - PCD PCIe RP08")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.PP08.VPOF ()
          }

          // PCD PCIe ports
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP09._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP09 0/6/0")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP09.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP10._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP10 0/6/1")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP10.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP11._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP11 0/6/2")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP11.VPOF ()
          }
          Case (ToInteger (\_SB.ROOT_BRIDGE_COMMON.VMD0.SP12._ADR ())) {
            ADBG ("VMD Logical volume - PCD RP12 0/6/3")
            \_SB.ROOT_BRIDGE_COMMON.VMD0.SP12.VPOF ()
          }
        } //Switch (DRVA)
        Increment (Local0)
      } // while
    }

  }
}
