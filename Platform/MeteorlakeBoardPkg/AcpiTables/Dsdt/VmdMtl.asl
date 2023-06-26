/**@file
  Intel ACPI Reference Code for VMD device support

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PcieRegs.h>
#include <Register/PchPcieRpRegs.h>
#include <Register/PchBdfAssignment.h>

#define VMD_STORAGE_REMAP_PORT_SATA         0
#define VMD_STORAGE_REMAP_PORT_PCIE_RP      1

//
// SSDT RTD3 imports
//
External (\_SB.PC00.SAT0.PRT0.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT1.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT2.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT3.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT4.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT5.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT6.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT7.SPON, MethodObj)
External (\_SB.PC00.SAT0.PRT0.SPOF, MethodObj)
External (\_SB.PC00.SAT0.PRT1.SPOF, MethodObj)
External (\_SB.PC00.SAT0.PRT2.SPOF, MethodObj)
External (\_SB.PC00.SAT0.PRT3.SPOF, MethodObj)
External (\_SB.PC00.SAT0.PRT4.SPOF, MethodObj)
External (\_SB.PC00.SAT0.PRT5.SPOF, MethodObj)
External (\_SB.PC00.SAT0.PRT6.SPOF, MethodObj)
External (\_SB.PC00.SAT0.PRT7.SPOF, MethodObj)
External (\_SB.PC00.RP01.PON, MethodObj)
External (\_SB.PC00.RP01.POFF, MethodObj)
External (\_SB.PC00.RP02.PON, MethodObj)
External (\_SB.PC00.RP02.POFF, MethodObj)
External (\_SB.PC00.RP03.PON, MethodObj)
External (\_SB.PC00.RP03.POFF, MethodObj)
External (\_SB.PC00.RP04.PON, MethodObj)
External (\_SB.PC00.RP04.POFF, MethodObj)
External (\_SB.PC00.RP05.PON, MethodObj)
External (\_SB.PC00.RP05.POFF, MethodObj)
External (\_SB.PC00.RP06.PON, MethodObj)
External (\_SB.PC00.RP06.POFF, MethodObj)
External (\_SB.PC00.RP07.PON, MethodObj)
External (\_SB.PC00.RP07.POFF, MethodObj)
External (\_SB.PC00.RP08.PON, MethodObj)
External (\_SB.PC00.RP08.POFF, MethodObj)
External (\_SB.PC00.RP09.PON, MethodObj)      // P.E.G 0/6/0
External (\_SB.PC00.RP09.POFF, MethodObj)     // P.E.G 0/6/0
External (\_SB.PC00.RP10.PON, MethodObj)      // P.E.G 0/6/1
External (\_SB.PC00.RP10.POFF, MethodObj)     // P.E.G 0/6/1
External (\_SB.PC00.RP11.PON, MethodObj)      // P.E.G 0/6/2
External (\_SB.PC00.RP11.POFF, MethodObj)     // P.E.G 0/6/2
External (\_SB.PC00.RP12.PON, MethodObj)      // P.E.G 0/1/0
External (\_SB.PC00.RP12.POFF, MethodObj)     // P.E.G 0/1/0
External (\VDSD, IntObj) // VMD _DSD Method - 1: Expose 0: Hide
External (VMS0)
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
      If (CondRefOf (VMS0)) {
        If (LNotEqual (VMS0, 0)) { // If SATA Controller is mapped under VMD
          //
          // Clear LTR ignore bit for Sata on D0
          //
          Store (0, ISAT)
        }
      }
    }

    Method (_PS3, 0, Serialized) {
      If (CondRefOf (VMS0)) {
        If (LNotEqual (VMS0, 0)) { // If SATA Controller is mapped under VMD
          //
          // Set LTR ignore bit for Sata on D3
          //
          Store (1, ISAT)
        }
      }
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
    // PCIe root ports remapped under VMD controller
    // RPB -  remapped root port bus number
    // RPD -  remapped root port device number
    // RPF -  remapped root port function number
    // RSPT - remapped slot port type
    // RSPI - remapped slot port index
    //

    //
    // SOC and IOE PCIe root ports remapped under VMD controller
    //

    // PCIE Root Port B0D28F0 #1
    Device (SP01) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_1)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 0)
      Method (_ADR, 0) {
        Return (0x80E0FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D28F1 #2
    Device (SP02) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_2)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 1)
      Method (_ADR, 0) {
        Return (0x80E1FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D28F2 #3
    Device (SP03) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_3)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 2)
      Method (_ADR, 0) {
        Return (0x80E2FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D28F3 #4
    Device (SP04) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_4)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 3)
      Method (_ADR, 0) {
        Return (0x80E3FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D28F4 #5
    Device (SP05) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_5)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 4)
      Method (_ADR, 0) {
        Return (0x80E4FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D28F5 #6
    Device (SP06) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_6)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 5)
      Method (_ADR, 0) {
        Return (0x80E5FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D28F6 #7
    Device (SP07) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_7)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 6)
      Method (_ADR, 0) {
        Return (0x80E6FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D28F7 #8
    Device (SP08) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_8)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 7)
      Method (_ADR, 0) {
        Return (0x80E7FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D6F0 #9
    Device (SP09) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_9)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_9)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 8)
      Method (_ADR, 0) {
        Return (0x8030FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D6F1 #10
    Device (SP10) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_10)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_10)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 9)
      Method (_ADR, 0) {
        Return (0x8031FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D6F2 #11
    Device (SP11) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_11)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_11)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 10)
      Method (_ADR, 0) {
        Return (0x8032FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    // PCIE Root Port B0D1F0 #12
    Device (SP12) {
      Name (RPB, 0)
      Name (RPD, PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORT_12)
      Name (RPF, PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_12)
      Name (RSPT, VMD_STORAGE_REMAP_PORT_PCIE_RP)
      Name (RSPI, 11)
      Method (_ADR, 0) {
        Return (0x8008FFFF)
      }
      Include ("VmdPcieRp.asl")
    }

    //
    // Integrated SATA ports remapped under VMD controller
    // RSPT - remapped slot port type
    // RSPI - remapped slot port index
    //
    Device (PRT0) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 0)
      Method (_ADR, 0) {
        Return (0xB8FFF0)
      }
      Include ("VmdSata.asl")
    }

    Device (PRT1) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 1)
      Method (_ADR, 0) {
        Return (0xB8FFF1)
      }
      Include ("VmdSata.asl")
    }

    Device (PRT2) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 2)
      Method (_ADR, 0) {
        Return (0xB8FFF2)
      }
      Include ("VmdSata.asl")
    }

    Device (PRT3) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 3)
      Method (_ADR, 0) {
        Return (0xB8FFF3)
      }
      Include ("VmdSata.asl")
    }

    Device (PRT4) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 4)
      Method (_ADR, 0) {
        Return (0xB8FFF4)
      }
      Include ("VmdSata.asl")
    }

    Device (PRT5) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 5)
      Method (_ADR, 0) {
        Return (0xB8FFF5)
      }
      Include ("VmdSata.asl")
    }

    Device (PRT6) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 6)
      Method (_ADR, 0) {
        Return (0xB8FFF6)
      }
      Include ("VmdSata.asl")
    }

    Device (PRT7) {
      Name (RSPT, VMD_STORAGE_REMAP_PORT_SATA)
      Name (RSPI, 7)
      Method (_ADR, 0) {
        Return (0xB8FFF7)
      }
      Include ("VmdSata.asl")
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
        // For SATA the support for D3Cold transition
        // is indicated by the presence of the SPOF method.
        // We support up to 8 SATA ports as this is the current
        // limit in the integrated SATA controller.
        //
        Case (VMD_STORAGE_REMAP_PORT_SATA) {
          Switch (ToInteger (Arg1)) {
            Case (0) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT0.SPOF)) {Return (1)}
            }
            Case (1) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT1.SPOF)) {Return (1)}
            }
            Case (2) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT2.SPOF)) {Return (1)}
            }
            Case (3) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT3.SPOF)) {Return (1)}
            }
            Case (4) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT4.SPOF)) {Return (1)}
            }
            Case (5) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT5.SPOF)) {Return (1)}
            }
            Case (6) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT6.SPOF)) {Return (1)}
            }
            Case (7) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT7.SPOF)) {Return (1)}
            }
          }
        }
        //
        // For PCIe root ports the support for D3Cold transition
        // is indicated by the presence of the POFF method.
        // We support up to 24 root ports which is the current
        // limit on the desktop SKUs of the PCH.
        //
        Case (VMD_STORAGE_REMAP_PORT_PCIE_RP) {
          Switch (ToInteger (Arg1)) {
            Case (0) {
              If (CondRefOf (\_SB.PC00.RP01.POFF)) {Return (1)}
            }
            Case (1) {
              If (CondRefOf (\_SB.PC00.RP02.POFF)) {Return (1)}
            }
            Case (2) {
              If (CondRefOf (\_SB.PC00.RP03.POFF)) {Return (1)}
            }
            Case (3) {
              If (CondRefOf (\_SB.PC00.RP04.POFF)) {Return (1)}
            }
            Case (4) {
              If (CondRefOf (\_SB.PC00.RP05.POFF)) {Return (1)}
            }
            Case (5) {
              If (CondRefOf (\_SB.PC00.RP06.POFF)) {Return (1)}
            }
            Case (6) {
              If (CondRefOf (\_SB.PC00.RP07.POFF)) {Return (1)}
            }
            Case (7) {
              If (CondRefOf (\_SB.PC00.RP08.POFF)) {Return (1)}
            }
            Case (8) {
              If (CondRefOf (\_SB.PC00.RP09.POFF)) {Return (1)}
            }
            Case (9) {
              If (CondRefOf (\_SB.PC00.RP10.POFF)) {Return (1)}
            }
            Case (10) {
              If (CondRefOf (\_SB.PC00.RP11.POFF)) {Return (1)}
            }
            Case (13) {
              If (CondRefOf (\_SB.PC00.RP12.POFF)) {Return (1)}
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
        Case (VMD_STORAGE_REMAP_PORT_SATA) {
          Switch (ToInteger (Arg1)) {
            Case (0) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT0.SPON)) {\_SB.PC00.SAT0.PRT0.SPON ()}
            }
            Case (1) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT1.SPON)) {\_SB.PC00.SAT0.PRT1.SPON ()}
            }
            Case (2) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT2.SPON)) {\_SB.PC00.SAT0.PRT2.SPON ()}
            }
            Case (3) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT3.SPON)) {\_SB.PC00.SAT0.PRT3.SPON ()}
            }
            Case (4) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT4.SPON)) {\_SB.PC00.SAT0.PRT4.SPON ()}
            }
            Case (5) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT5.SPON)) {\_SB.PC00.SAT0.PRT5.SPON ()}
            }
            Case (6) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT6.SPON)) {\_SB.PC00.SAT0.PRT6.SPON ()}
            }
            Case (7) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT7.SPON)) {\_SB.PC00.SAT0.PRT7.SPON ()}
            }
          }
        }
        Case (VMD_STORAGE_REMAP_PORT_PCIE_RP) {
          Switch (ToInteger (Arg1)) {
            Case (0) {
              If (CondRefOf (\_SB.PC00.RP01.PON)) {\_SB.PC00.RP01.PON ()}
            }
            Case (1) {
              If (CondRefOf (\_SB.PC00.RP02.PON)) {\_SB.PC00.RP02.PON ()}
            }
            Case (2) {
              If (CondRefOf (\_SB.PC00.RP03.PON)) {\_SB.PC00.RP03.PON ()}
            }
            Case (3) {
              If (CondRefOf (\_SB.PC00.RP04.PON)) {\_SB.PC00.RP04.PON ()}
            }
            Case (4) {
              If (CondRefOf (\_SB.PC00.RP05.PON)) {\_SB.PC00.RP05.PON ()}
            }
            Case (5) {
              If (CondRefOf (\_SB.PC00.RP06.PON)) {\_SB.PC00.RP06.PON ()}
            }
            Case (6) {
              If (CondRefOf (\_SB.PC00.RP07.PON)) {\_SB.PC00.RP07.PON ()}
            }
            Case (7) {
              If (CondRefOf (\_SB.PC00.RP08.PON)) {\_SB.PC00.RP08.PON ()}
            }
            Case (8) {
              If (CondRefOf (\_SB.PC00.RP09.PON)) {\_SB.PC00.RP09.PON ()}
            }
            Case (9) {
              If (CondRefOf (\_SB.PC00.RP10.PON)) {\_SB.PC00.RP10.PON ()}
            }
            Case (10) {
              If (CondRefOf (\_SB.PC00.RP11.PON)) {\_SB.PC00.RP11.PON ()}
            }
            Case (11) {
              If (CondRefOf (\_SB.PC00.RP12.PON)) {\_SB.PC00.RP12.PON ()}
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
        Case (VMD_STORAGE_REMAP_PORT_SATA) {
          Switch (ToInteger (Arg1)) {
            Case (0) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT0.SPOF)) {\_SB.PC00.SAT0.PRT0.SPOF ()}
            }
            Case (1) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT1.SPOF)) {\_SB.PC00.SAT0.PRT1.SPOF ()}
            }
            Case (2) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT2.SPOF)) {\_SB.PC00.SAT0.PRT2.SPOF ()}
            }
            Case (3) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT3.SPOF)) {\_SB.PC00.SAT0.PRT3.SPOF ()}
            }
            Case (4) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT4.SPOF)) {\_SB.PC00.SAT0.PRT4.SPOF ()}
            }
            Case (5) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT5.SPOF)) {\_SB.PC00.SAT0.PRT5.SPOF ()}
            }
            Case (6) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT6.SPOF)) {\_SB.PC00.SAT0.PRT6.SPOF ()}
            }
            Case (7) {
              If (CondRefOf (\_SB.PC00.SAT0.PRT7.SPOF)) {\_SB.PC00.SAT0.PRT7.SPOF ()}
            }
          }
        }
        Case (VMD_STORAGE_REMAP_PORT_PCIE_RP) {
          Switch (ToInteger (Arg1)) {
            Case (0) {
              If (CondRefOf (\_SB.PC00.RP01.POFF)) {\_SB.PC00.RP01.POFF ()}
            }
            Case (1) {
              If (CondRefOf (\_SB.PC00.RP02.POFF)) {\_SB.PC00.RP02.POFF ()}
            }
            Case (2) {
              If (CondRefOf (\_SB.PC00.RP03.POFF)) {\_SB.PC00.RP03.POFF ()}
            }
            Case (3) {
              If (CondRefOf (\_SB.PC00.RP04.POFF)) {\_SB.PC00.RP04.POFF ()}
            }
            Case (4) {
              If (CondRefOf (\_SB.PC00.RP05.POFF)) {\_SB.PC00.RP05.POFF ()}
            }
            Case (5) {
              If (CondRefOf (\_SB.PC00.RP06.POFF)) {\_SB.PC00.RP06.POFF ()}
            }
            Case (6) {
              If (CondRefOf (\_SB.PC00.RP07.POFF)) {\_SB.PC00.RP07.POFF ()}
            }
            Case (7) {
              If (CondRefOf (\_SB.PC00.RP08.POFF)) {\_SB.PC00.RP08.POFF ()}
            }
            Case (8) {
              If (CondRefOf (\_SB.PC00.RP09.POFF)) {\_SB.PC00.RP09.POFF ()}
            }
            Case (9) {
              If (CondRefOf (\_SB.PC00.RP10.POFF)) {\_SB.PC00.RP10.POFF ()}
            }
            Case (10) {
              If (CondRefOf (\_SB.PC00.RP11.POFF)) {\_SB.PC00.RP11.POFF ()}
            }
            Case (11) {
              If (CondRefOf (\_SB.PC00.RP12.POFF)) {\_SB.PC00.RP12.POFF ()}
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
      Store (Arg0, Local1)
      Store (0, Local0) // use this as the index value
      While (LGreater (Local1, Local0))
      {
        Store (ToInteger (DerefOf (Index (Arg1, Local0))), DRVA)
        Switch (ToInteger (DRVA)) {
          Case (0x80E0FFFF) {
            \_SB.PC00.VMD0.SP01.VPON ()
          }
          Case (0x80E1FFFF) {
            \_SB.PC00.VMD0.SP02.VPON ()
          }
          Case (0x80E2FFFF) {
            \_SB.PC00.VMD0.SP03.VPON ()
          }
          Case (0x80E3FFFF) {
            \_SB.PC00.VMD0.SP04.VPON ()
          }
          Case (0x80E4FFFF) {
            \_SB.PC00.VMD0.SP05.VPON ()
          }
          Case (0x80E5FFFF) {
            \_SB.PC00.VMD0.SP06.VPON ()
          }
          Case (0x80E6FFFF) {
            \_SB.PC00.VMD0.SP07.VPON ()
          }
          Case (0x80E7FFFF) {
            \_SB.PC00.VMD0.SP08.VPON ()
          }

          // CPU PCIe ports
          Case (0x8030FFFF) {
            \_SB.PC00.VMD0.SP09.VPON ()
          }
          Case (0x8031FFFF) {
            \_SB.PC00.VMD0.SP10.VPON ()
          }
          Case (0x8032FFFF) {
            \_SB.PC00.VMD0.SP11.VPON ()
          }
          Case (0x8008FFFF) {
            \_SB.PC00.VMD0.SP12.VPON ()
          }

          // SATA Ports
          Case (0xB8FFF0) {
            \_SB.PC00.VMD0.PRT0.VSON ()
          }
          Case (0xB8FFF1) {
            \_SB.PC00.VMD0.PRT1.VSON ()
          }
          Case (0xB8FFF2) {
            \_SB.PC00.VMD0.PRT2.VSON ()
          }
          Case (0xB8FFF3) {
            \_SB.PC00.VMD0.PRT3.VSON ()
          }
          Case (0xB8FFF4) {
            \_SB.PC00.VMD0.PRT4.VSON ()
          }
          Case (0xB8FFF5) {
            \_SB.PC00.VMD0.PRT5.VSON ()
          }
          Case (0xB8FFF6) {
            \_SB.PC00.VMD0.PRT6.VSON ()
          }
          Case (0xB8FFF7) {
            \_SB.PC00.VMD0.PRT7.VSON ()
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
      Store (Arg0, Local1)
      Store (0, Local0) // use this as the index value
      While (LGreater (Local1, Local0))
      {
        Store (ToInteger (DerefOf (Index (Arg1, Local0))), DRVA)
        Switch (ToInteger (DRVA)) {
          Case (0x80E0FFFF) {
            \_SB.PC00.VMD0.SP01.VPOF ()
          }
          Case (0x80E1FFFF) {
            \_SB.PC00.VMD0.SP02.VPOF ()
          }
          Case (0x80E2FFFF) {
            \_SB.PC00.VMD0.SP03.VPOF ()
          }
          Case (0x80E3FFFF) {
            \_SB.PC00.VMD0.SP04.VPOF ()
          }
          Case (0x80E4FFFF) {
            \_SB.PC00.VMD0.SP05.VPOF ()
          }
          Case (0x80E5FFFF) {
            \_SB.PC00.VMD0.SP06.VPOF ()
          }
          Case (0x80E6FFFF) {
            \_SB.PC00.VMD0.SP07.VPOF ()
          }
          Case (0x80E7FFFF) {
            \_SB.PC00.VMD0.SP08.VPOF ()
          }

          // CPU PCIe ports
          Case (0x8030FFFF) {
            \_SB.PC00.VMD0.SP09.VPOF ()
          }
          Case (0x8031FFFF) {
            \_SB.PC00.VMD0.SP10.VPOF ()
          }
          Case (0x8032FFFF) {
            \_SB.PC00.VMD0.SP11.VPOF ()
          }
          Case (0x8008FFFF) {
            \_SB.PC00.VMD0.SP12.VPOF ()
          }
          // SATA Ports
          Case (0xB8FFF0) {
            \_SB.PC00.VMD0.PRT0.VSOF ()
          }
          Case (0xB8FFF1) {
            \_SB.PC00.VMD0.PRT1.VSOF ()
          }
          Case (0xB8FFF2) {
            \_SB.PC00.VMD0.PRT2.VSOF ()
          }
          Case (0xB8FFF3) {
            \_SB.PC00.VMD0.PRT3.VSOF ()
          }
          Case (0xB8FFF4) {
            \_SB.PC00.VMD0.PRT4.VSOF ()
          }
          Case (0xB8FFF5) {
            \_SB.PC00.VMD0.PRT5.VSOF ()
          }
          Case (0xB8FFF6) {
            \_SB.PC00.VMD0.PRT6.VSOF ()
          }
          Case (0xB8FFF7) {
            \_SB.PC00.VMD0.PRT7.VSOF ()
          }
        } //Switch (DRVA)
        Increment (Local0)
      } // while
    }

  }
}
