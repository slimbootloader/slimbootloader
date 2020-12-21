/**@file
  RST remapping support in ACPI

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/SataRegs.h>

//
// Include this file in SATA device scope to enable remapping support
//

//
// NOTE: For platform implementers.
// To enable D3Cold support for remapped drives implement PON/POFF methods in remap capable root port scope.
// PON - should not make any access to PCIe config space and should only restore power and clock
// and de-assert reset signal.
// POFF - should not make any access to PCIe config space and should only assert reset and disable power(optional) and clock(optional).
//

#define ROOTPORT_READ           0
#define ROOTPORT_WRITE          1
#define ENDPOINT_READ           2
#define ENDPOINT_WRITE          3

//
// SSDT RTD3 imports
//
External(\_SB.PC00.RP05.PON, MethodObj)
External(\_SB.PC00.RP05.POFF, MethodObj)
External(\_SB.PC00.RP07.PON, MethodObj)
External(\_SB.PC00.RP07.POFF, MethodObj)
External(\_SB.PC00.RP09.PON, MethodObj)
External(\_SB.PC00.RP09.POFF, MethodObj)
External(\_SB.PC00.RP11.PON, MethodObj)
External(\_SB.PC00.RP11.POFF, MethodObj)
External(\_SB.PC00.RP13.PON, MethodObj)
External(\_SB.PC00.RP13.POFF, MethodObj)
External(\_SB.PC00.RP15.PON, MethodObj)
External(\_SB.PC00.RP15.POFF, MethodObj)
External(\_SB.PC00.RP17.PON, MethodObj)
External(\_SB.PC00.RP17.POFF, MethodObj)
External(\_SB.PC00.RP19.PON, MethodObj)
External(\_SB.PC00.RP19.POFF, MethodObj)
External(\_SB.PC00.RP21.PON, MethodObj)
External(\_SB.PC00.RP21.POFF, MethodObj)
External(\_SB.PC00.RP23.PON, MethodObj)
External(\_SB.PC00.RP23.POFF, MethodObj)

Device(NVM1) {
  Name(_ADR,0x00C1FFFF)

  Method(_INI) {
    Store (NIT1, NITV)
    Store (NPM1, NPMV)
    Store (NPC1, NPCV)
    Store (NL11, NL1V)
    Store (ND21, ND2V)
    Store (ND11, ND1V)
    Store (NLR1, NLRV)
    Store (NLD1, NLDV)
    Store (NEA1, NEAV)
    Store (NEB1, NEBV)
    Store (NEC1, NECV)
    Store (NRA1, NRAV)
    Store (NMB1, NMBV)
    Store (NMV1, NMVV)
    Store (NPB1, NPBV)
    Store (NPV1, NPVV)
    Store(NRP1, NRPN)
    Store (0, NCRN)
  }

  Include("PchRstPcieStorage.asl")
}

Device(NVM2) {
  Name(_ADR,0x00C2FFFF)

  Method(_INI) {
    Store (NIT2, NITV)
    Store (NPM2, NPMV)
    Store (NPC2, NPCV)
    Store (NL12, NL1V)
    Store (ND22, ND2V)
    Store (ND12, ND1V)
    Store (NLR2, NLRV)
    Store (NLD2, NLDV)
    Store (NEA2, NEAV)
    Store (NEB2, NEBV)
    Store (NEC2, NECV)
    Store (NRA2, NRAV)
    Store (NMB2, NMBV)
    Store (NMV2, NMVV)
    Store (NPB2, NPBV)
    Store (NPV2, NPVV)
    Store(NRP2, NRPN)
    Store (1, NCRN)
  }

  Include("PchRstPcieStorage.asl")
}

Device(NVM3) {
  Name(_ADR,0x00C3FFFF)
  Method(_INI) {
    Store (NIT3, NITV)
    Store (NPM3, NPMV)
    Store (NPC3, NPCV)
    Store (NL13, NL1V)
    Store (ND23, ND2V)
    Store (ND13, ND1V)
    Store (NLR3, NLRV)
    Store (NLD3, NLDV)
    Store (NEA3, NEAV)
    Store (NEB3, NEBV)
    Store (NEC3, NECV)
    Store (NRA3, NRAV)
    Store (NMB3, NMBV)
    Store (NMV3, NMVV)
    Store (NPB3, NPBV)
    Store (NPV3, NPVV)
    Store(NRP3, NRPN)
    Store (2, NCRN)
  }
   Include("PchRstPcieStorage.asl")
}

//
// Method to perform RST PCIe Storage Remapping read or write access to the remapped device / hidden root port configuration space
// This method takes 5 parameters as below:
// Arg0 - RST PCIe Storage Cycle Router#
// Arg1 - PCI Offset
// Arg2 - WriteData (AndMask)
// Arg3 - WriteData (OrMask)
// Arg4 - Access Methods: ROOTPORT_READ, ROOTPORT_WRITE, ENDPOINT_READ, ENDPOINT_WRITE
//
Method(RDCA, 5, Serialized) {
  //
  // Operation Region for Sata Extended Config Space for Hidden Root Port Access (0x100)
  //
  OperationRegion(RPAL, SystemMemory, Add(\_SB.PC00.GPCB(), Add(Add(0xB8000, V_SATA_MEM_RST_PCIE_STORAGE_REMAP_RP_OFFSET),Arg1)), 0x4)
  Field(RPAL,DWordAcc,Lock,Preserve)
  {
    RPCD, 32,
  }

  //
  // Operation Region for Endpoint Index-Data Pair for Remapped Device Access (0x308)
  //
  OperationRegion(EPAC, SystemMemory, Add(\_SB.PC00.GPCB(), Add(0xB8000, R_SATA_CFG_RST_PCIE_STORAGE_CAIR)), 0x8)
  Field(EPAC,DWordAcc,Lock,Preserve)
  {
    CAIR, 32,
    CADR, 32,
  }

  //
  // Operation Region for RST PCIe Storage Cycle Router Global configuration registers (0xFC0)
  //
  OperationRegion(NCRG, SystemMemory, Add(\_SB.PC00.GPCB(), Add(0xB8000, R_SATA_CFG_RST_PCIE_STORAGE_CRGC)), 0x4)
  Field(NCRG,DWordAcc,Lock,Preserve)
  {
    CRGC, 32,
  }

  If (LGreater(Arg0, 2))     // Return if RST PCIe Storage Cycle Router# is invalid
  {
    Return(0)
  }
  Else                       // Set RST PCIe Storage Cycle Router Accessibility based on Arg0 - RST PCIe Storage Cycle Router#
  {
    Store(Arg0,CRGC)
  }

  Switch(ToInteger(Arg4))
  {
    Case(ROOTPORT_READ)     // Read access to the Hidden Root Port
    {
      Return(RPCD)
    }
    Case(ENDPOINT_READ)     // Read access to the Remapped Device
    {
      Store(Arg1,CAIR)
      Return(CADR)
    }
    Case(ROOTPORT_WRITE)     // Write access to the Hidden Root Port
    {
      And(Arg2,RPCD,Local0)
      Or(Arg3,Local0,Local0)
      Store(Local0,RPCD)
    }
    Case(ENDPOINT_WRITE)     // Write access to the Remapped Device
    {
      Store(Arg1,CAIR)
      And(Arg2,CADR,Local0)
      Or(Arg3,Local0,Local0)
      Store(Local0,CADR)
    }
    Default
    {
      Return(0)
    }
  }
  Return(0)
}

//
// Is D3Cold supported for given root port.
// Support for D3Cold is indicated by presence of POFF method.
// Arg0  Root port number
//
Method(D3CS, 1, Serialized) {
  Switch(ToInteger(Arg0)) {
    Case(4) {
      If(CondRefOf(\_SB.PC00.RP05.POFF)) {Return(1)}
    }
    Case(6) {
      If(CondRefOf(\_SB.PC00.RP07.POFF)) {Return(1)}
    }
    Case(8) {
      If(CondRefOf(\_SB.PC00.RP09.POFF)) {Return(1)}
    }
    Case(10) {
      If(CondRefOf(\_SB.PC00.RP11.POFF)) {Return(1)}
    }
    Case(12) {
      If(CondRefOf(\_SB.PC00.RP13.POFF)) {Return(1)}
    }
    Case(14) {
      If(CondRefOf(\_SB.PC00.RP15.POFF)) {Return(1)}
    }
    Case(16) {
      If(CondRefOf(\_SB.PC00.RP17.POFF)) {Return(1)}
    }
    Case(18) {
      If(CondRefOf(\_SB.PC00.RP19.POFF)) {Return(1)}
    }
    Case(20) {
      If(CondRefOf(\_SB.PC00.RP21.POFF)) {Return(1)}
    }
    Case(22) {
      If(CondRefOf(\_SB.PC00.RP23.POFF)) {Return(1)}
    }
  }
  Return(0)
}

//
// Turn on power to the remapped port PCIe slot
// Arg0  Root port number
//
Method(RSON, 1, Serialized) {
  Switch(ToInteger(Arg0)) {
    Case(4) {
      If(CondRefOf(\_SB.PC00.RP05.PON)) {\_SB.PC00.RP05.PON()}
    }
    Case(6) {
      If(CondRefOf(\_SB.PC00.RP07.PON)) {\_SB.PC00.RP07.PON()}
    }
    Case(8) {
      If(CondRefOf(\_SB.PC00.RP09.PON)) {\_SB.PC00.RP09.PON()}
    }
    Case(10) {
      If(CondRefOf(\_SB.PC00.RP11.PON)) {\_SB.PC00.RP11.PON()}
    }
    Case(12) {
      If(CondRefOf(\_SB.PC00.RP13.PON)) {\_SB.PC00.RP13.PON()}
    }
    Case(14) {
      If(CondRefOf(\_SB.PC00.RP15.PON)) {\_SB.PC00.RP15.PON()}
    }
    Case(16) {
      If(CondRefOf(\_SB.PC00.RP17.PON)) {\_SB.PC00.RP17.PON()}
    }
    Case(18) {
      If(CondRefOf(\_SB.PC00.RP19.PON)) {\_SB.PC00.RP19.PON()}
    }
    Case(20) {
      If(CondRefOf(\_SB.PC00.RP21.PON)) {\_SB.PC00.RP21.PON()}
    }
    Case(22) {
      If(CondRefOf(\_SB.PC00.RP23.PON)) {\_SB.PC00.RP23.PON()}
    }
  }
}

//
// Turn off power to the remapped port PCIe slot
// Arg0  Root port number
//
Method(RSOF, 1, Serialized) {
  Switch(ToInteger(Arg0)) {
    Case(4) {
      If(CondRefOf(\_SB.PC00.RP05.POFF)) {\_SB.PC00.RP05.POFF()}
    }
    Case(6) {
      If(CondRefOf(\_SB.PC00.RP07.POFF)) {\_SB.PC00.RP07.POFF()}
    }
    Case(8) {
      If(CondRefOf(\_SB.PC00.RP09.POFF)) {\_SB.PC00.RP09.POFF()}
    }
    Case(10) {
      If(CondRefOf(\_SB.PC00.RP11.POFF)) {\_SB.PC00.RP11.POFF()}
    }
    Case(12) {
      If(CondRefOf(\_SB.PC00.RP13.POFF)) {\_SB.PC00.RP13.POFF()}
    }
    Case(14) {
      If(CondRefOf(\_SB.PC00.RP15.POFF)) {\_SB.PC00.RP15.POFF()}
    }
    Case(16) {
      If(CondRefOf(\_SB.PC00.RP17.POFF)) {\_SB.PC00.RP17.POFF()}
    }
    Case(18) {
      If(CondRefOf(\_SB.PC00.RP19.POFF)) {\_SB.PC00.RP19.POFF()}
    }
    Case(20) {
      If(CondRefOf(\_SB.PC00.RP21.POFF)) {\_SB.PC00.RP21.POFF()}
    }
    Case(22) {
      If(CondRefOf(\_SB.PC00.RP23.POFF)) {\_SB.PC00.RP23.POFF()}
    }
  }
}

