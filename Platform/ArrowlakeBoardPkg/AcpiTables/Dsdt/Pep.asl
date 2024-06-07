/** @file
  ACPI uPEP Support

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

/**
  Note: BKM to add any new device Root port constraint
        If you want to add new root port constraint make sure to follow
        below steps
        * DEVY package is the one which is shared to OS with list containing all
          the devices with constraints. DEVY package is declared with
          DEVY_INITIALIZER macro to accomodate size for all the device instances,
          So, if you wish to add new device entry into DEVY pkg make sure it has
          sufficent space to accomodate new entry.
        * New device can be added using either UPDATE_PEP_DEV_LIST or UPPR function.
          Make sure to add entries inside UPDV method.
            * UPDATE_PEP_DEV_LIST can be used for non RP devices like I2C, SPI, etc
              where the devices are fixed. Read Macro header for usage.
            * UPPR function can be used for PCIE RPs where the constraints are applied
              dynamically based on detected device. Read function header for usage.
        * If at all the DEVY package is not accomodating the newly added entry you
          will get runtime error message "PEP UPDE Error!!!! DEVY size is excedded"
**/

#include <Register/PmcRegs.h>
#include <Register/TcoRegs.h>
#include <Rtd3Defines.h>

#if (FixedPcdGet8(PcdMtlSSupport) == 1)
#define ROOT_BRIDGE_COMMON_STR "\\_SB.PC02"
#define ROOT_BRIDGE_COMMON PC02
#else
#define ROOT_BRIDGE_COMMON_STR "\\_SB.PC00"
#define ROOT_BRIDGE_COMMON PC00
#endif

#define ROOT_BRIDGE_SOC_STR "\\_SB.PC00"
#define ROOT_BRIDGE_SOC PC00

#define ROOT_BRIDGE_PCH_STR "\\_SB.PC02"
#define ROOT_BRIDGE_PCH PC02

External (\_SB.ROOT_BRIDGE_SOC)
External (\_SB.ROOT_BRIDGE_SOC.DPOF)
External (\_SB.ROOT_BRIDGE_SOC.IMNG, MethodObj)
External (\_SB.ROOT_BRIDGE_SOC.LPCB.NTIR, MethodObj)
External (\GMIO, MethodObj)
External (THCE) // TCSS XHCI Device Enable
External (TDCE) // TCSS XDCI Device Enable
External (TRE0) // TCSS ItbtPcie PCIE RP 0 Device Enable
External (TRE1) // TCSS ItbtPcie PCIE RP 1 Device Enable
External (TRE2) // TCSS ItbtPcie PCIE RP 2 Device Enable
External (TRE3) // TCSS ItbtPcie PCIE RP 3 Device Enable
External (DME0) // TCSS ITBT DMA0
External (DME1) // TCSS ITBT DMA1
External (GEDT, MethodObj)

#if (FixedPcdGet8(PcdMtlSSupport) == 0)
External (GBES)
#endif

External (\_SB.ROOT_BRIDGE_COMMON.GBES)

External (\PSON) // Indicates if PS_ON is enabled
External (\_SB.ROOT_BRIDGE_SOC.TCON, MethodObj)
External (\_SB.ROOT_BRIDGE_SOC.TDM0.SCME)
External (\_SB.ROOT_BRIDGE_SOC.TDM1.SCME)
External (\_SB.ROOT_BRIDGE_SOC.TDM0._STA)
External (\_SB.ROOT_BRIDGE_SOC.TDM1._STA)
External (\GPRV, MethodObj) // PS_ON control method
External (\_SB.ROOT_BRIDGE_SOC.LPCB.NRTS, MethodObj)
External (PPOE)


#if (FixedPcdGet8(PcdMtlSSupport) == 1)

Scope (\_SB.ROOT_BRIDGE_SOC.RP01.PXSX)
{
  Name (PERN, "RP01") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP09.PXSX)
{
  Name (PERN, "RP09") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP13.PXSX)
{
  Name (PERN, "RP13") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP17.PXSX)
{
  Name (PERN, "RP17") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP21.PXSX)
{
  Name (PERN, "RP21") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP01.PXSX)
{
  Name (PERN, "RP01") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP02.PXSX)
{
  Name (PERN, "RP02") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP03.PXSX)
{
  Name (PERN, "RP03") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP04.PXSX)
{
  Name (PERN, "RP04") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP05.PXSX)
{
  Name (PERN, "RP05") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP06.PXSX)
{
  Name (PERN, "RP06") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP07.PXSX)
{
  Name (PERN, "RP07") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP08.PXSX)
{
  Name (PERN, "RP08") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP09.PXSX)
{
  Name (PERN, "RP09") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP10.PXSX)
{
  Name (PERN, "RP10") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP11.PXSX)
{
  Name (PERN, "RP11") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP12.PXSX)
{
  Name (PERN, "RP12") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP13.PXSX)
{
  Name (PERN, "RP13") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP14.PXSX)
{
  Name (PERN, "RP14") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP15.PXSX)
{
  Name (PERN, "RP15") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP16.PXSX)
{
  Name (PERN, "RP16") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP17.PXSX)
{
  Name (PERN, "RP17") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP18.PXSX)
{
  Name (PERN, "RP18") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP19.PXSX)
{
  Name (PERN, "RP19") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP20.PXSX)
{
  Name (PERN, "RP20") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP21.PXSX)
{
  Name (PERN, "RP21") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP22.PXSX)
{
  Name (PERN, "RP22") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP23.PXSX)
{
  Name (PERN, "RP23") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_PCH.RP24.PXSX)
{
  Name (PERN, "RP24") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

#else

Scope (\_SB.ROOT_BRIDGE_SOC.RP01.PXSX)
{
  Name (PERN, "RP01") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP02.PXSX)
{
  Name (PERN, "RP02") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP03.PXSX)
{
  Name (PERN, "RP03") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP04.PXSX)
{
  Name (PERN, "RP04") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP05.PXSX)
{
  Name (PERN, "RP05") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP06.PXSX)
{
  Name (PERN, "RP06") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP07.PXSX)
{
  Name (PERN, "RP07") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP08.PXSX)
{
  Name (PERN, "RP08") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP09.PXSX)
{
  Name (PERN, "RP09") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP10.PXSX)
{
  Name (PERN, "RP10") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP11.PXSX)
{
  Name (PERN, "RP11") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}

Scope (\_SB.ROOT_BRIDGE_SOC.RP12.PXSX)
{
  Name (PERN, "RP12") // PERN - PCIe  Root Port Name
  include ("PepPcieEndPoint.asl")
}
#endif

// DEVY initializer macro used in DEVY package to initialized or to allocate
// maximum space for device entries.
#define DEVY_INITIALIZER Package () {"Reserved.Reserved.Reserved.Reserved",   0x0, Package () {0, Package () {0xFF, 0, 0x81}}}

// Update Pep Device List
//
// Arg0: ACPI Device Descriptor Device parent path string
//       For example : "\\_SB_PC00"
// Arg1: ACPI Device Descriptor: Device name
//       For example : "I2C0"
// Arg2: Enabled/Disabled Field
//       0 = This device is disabled and applies no constraints
//       1+ = This device is enabled and applies constraints
// Arg3: D state constraint
//       0: Package () {0, Package () {0xFF, 0}}
//       1: Package () {0, Package () {0xFF, 0, 0x81}
//       2: Package () {0, Package () {0xFF, 2}
//       3: Package () {0, Package () {0xFF, 3}}
#define UPDATE_PEP_DEV_LIST(Arg0, Arg1, Arg2, Arg3) UPDE(Arg0, Arg1, Arg2, Arg3)

// PEP constraints values passed from AcpiPlatform.c file into PEPC object can be
// parsed using this macro by passing the index field.
// Note: Make sure to use it only in case of 1 BIT constraint. Enable/Disable constraints.
#define PEPC_PARSE(Pepc, BitIndex) (And (Pepc, BIT0 << BitIndex) >> BitIndex)

Scope (\_SB)
{
  Device (PEPD)
  {
    Name (DEVC, 0x0) // Device counter
    Name (DEVS, 0x0) // DEVY Size
    Name (IPDI, 0x0) // Is Pep Device List Initialized ?

    Name (_HID, "INT33A1")
    Name (_CID, EISAID ("PNP0D80"))
    Name (_UID, 0x1)

    Name (PCP0, Package () {0, Package () {0xFF, 0}})
    Name (PCP1, Package () {0, Package () {0xFF, 0, 0x81}})
    Name (PCP2, Package () {0, Package () {0xFF, 2}})
    Name (PCP3, Package () {0, Package () {0xFF, 3}})

      // 1: ACPI Device Descriptor: Fully Qualified name-string
      // 2: Enabled/Disabled Field
      //      0 = This device is disabled and applies no constraints
      //      1+ = This device is enabled and applies constraints
      // 3: Constraint Package: entry per LPI state in LPIT
      //     a. Associated LPI State UID
      //         ID == 0xFF: same constraints apply to all states in LPIT
      //     b: minimum Dx state as pre-condition
      //     c: (optional) OEM specific OEM may provide an additional encoding
      //         which further defines the D-state Constraint
      //            0x0-0x7F - Reserved
      //            0x80-0xFF - OEM defined
      //
    Name (DEVY, Package(){ // uPEP Device List
       DEVY_INITIALIZER, //GFX0
       DEVY_INITIALIZER, //HDAS
       DEVY_INITIALIZER, //UA00
       DEVY_INITIALIZER, //UA01
       DEVY_INITIALIZER, //UA02
       DEVY_INITIALIZER, //I2C0
       DEVY_INITIALIZER, //I2C1
       DEVY_INITIALIZER, //I2C2
       DEVY_INITIALIZER, //I2C3
       DEVY_INITIALIZER, //I2C4
       DEVY_INITIALIZER, //I2C5
       DEVY_INITIALIZER, //I2C6
       DEVY_INITIALIZER, //I2C7
       DEVY_INITIALIZER, //SPI0
       DEVY_INITIALIZER, //SPI1
       DEVY_INITIALIZER, //SPI2
       DEVY_INITIALIZER, //XHCI
       DEVY_INITIALIZER, //PEMC
       DEVY_INITIALIZER, //PSDC
       DEVY_INITIALIZER, //IPU0
       DEVY_INITIALIZER, //HECI
       DEVY_INITIALIZER, //GLAN
       DEVY_INITIALIZER, //THC0
       DEVY_INITIALIZER, //THC1
       DEVY_INITIALIZER, //GNA0
       DEVY_INITIALIZER, //VMD0
       DEVY_INITIALIZER, //HEC5
       DEVY_INITIALIZER, //PUF0
       DEVY_INITIALIZER, //PUF1
       DEVY_INITIALIZER, //VPU0
       DEVY_INITIALIZER, //TXHC
       DEVY_INITIALIZER, //TXDC
       DEVY_INITIALIZER, //TRP0
       DEVY_INITIALIZER, //TRP1
       DEVY_INITIALIZER, //TRP2
       DEVY_INITIALIZER, //TRP3
       DEVY_INITIALIZER, //TDM0
       DEVY_INITIALIZER, //TDM1
       DEVY_INITIALIZER, //TDM2
       DEVY_INITIALIZER, //TDM3
       DEVY_INITIALIZER, //RP01
       DEVY_INITIALIZER, //RP02
       DEVY_INITIALIZER, //RP03
       DEVY_INITIALIZER, //RP04
       DEVY_INITIALIZER, //RP05
       DEVY_INITIALIZER, //RP06
       DEVY_INITIALIZER, //RP07
       DEVY_INITIALIZER, //RP08
       DEVY_INITIALIZER, //RP09
       DEVY_INITIALIZER, //RP10
       DEVY_INITIALIZER, //RP11
       DEVY_INITIALIZER, //RP12
       DEVY_INITIALIZER, //RP13
       DEVY_INITIALIZER, //RP14
       DEVY_INITIALIZER, //RP15
       DEVY_INITIALIZER, //RP16
       DEVY_INITIALIZER, //RP17
       DEVY_INITIALIZER, //RP18
       DEVY_INITIALIZER, //RP19
       DEVY_INITIALIZER, //RP20
       DEVY_INITIALIZER, //PR01
       DEVY_INITIALIZER, //PR02
       DEVY_INITIALIZER, //PR03
       DEVY_INITIALIZER, //PR04
       DEVY_INITIALIZER, //PR05
       DEVY_INITIALIZER, //PR06
       DEVY_INITIALIZER, //PR07
       DEVY_INITIALIZER, //PR08
       DEVY_INITIALIZER, //PR09
       DEVY_INITIALIZER, //PR10
       DEVY_INITIALIZER, //PR11
       DEVY_INITIALIZER, //PR12
       DEVY_INITIALIZER, //PR13
       DEVY_INITIALIZER, //PR14
       DEVY_INITIALIZER, //PR15
       DEVY_INITIALIZER, //PR16
       DEVY_INITIALIZER, //PR17
       DEVY_INITIALIZER, //PR18
       DEVY_INITIALIZER, //PR19
       DEVY_INITIALIZER, //PR20
       DEVY_INITIALIZER, //PR21
       DEVY_INITIALIZER, //PR22
       DEVY_INITIALIZER, //PR23
       DEVY_INITIALIZER, //PR24
       DEVY_INITIALIZER, //PR25
       DEVY_INITIALIZER, //PR26
       DEVY_INITIALIZER, //PR27
       DEVY_INITIALIZER, //PR28
       DEVY_INITIALIZER, //PR29
       DEVY_INITIALIZER, //PR30
       DEVY_INITIALIZER, //PR31
       DEVY_INITIALIZER, //PR32
       DEVY_INITIALIZER, //PR33
       DEVY_INITIALIZER, //PR34
       DEVY_INITIALIZER, //PR35
       DEVY_INITIALIZER, //PR36
       DEVY_INITIALIZER, //PR37
       DEVY_INITIALIZER, //PR38
       DEVY_INITIALIZER, //PR39
       DEVY_INITIALIZER, //PR40
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
       DEVY_INITIALIZER,
    })

    // UPDE(Update Pep Device Entry)
    //
    // Arg0: ACPI Device Descriptor Device parent path string
    //       For example : "\\_SB_PC00"
    // Arg1: ACPI Device Descriptor: Device name
    //       For example : "I2C0"
    // Arg2: Enabled/Disabled Field
    //       0 = This device is disabled and applies no constraints
    //       1+ = This device is enabled and applies constraints
    // Arg3: D state constraint
    //       0: Package () {0, Package () {0xFF, 0}}
    //       1: Package () {0, Package () {0xFF, 0, 0x81}
    //       2: Package () {0, Package () {0xFF, 2}
    //       3: Package () {0, Package () {0xFF, 3}}
    Method (UPDE, 4, NotSerialized) {
       if(LLess ( DEVC, DEVS)) {

        if(LNotEqual(sizeof(Arg1), 0)) {
          Store(Concatenate(Concatenate(Arg0, "."), Arg1), Local0)
        } Else {
          Store(Arg0, Local0)
        }

        Store (Local0, Index (DeRefOf (Index (\_SB.PEPD.DEVY, DEVC)), 0)) // Update Device Descriptor

        // For reserved entry no need to update DEVY package with other information
        if(LEqual(Arg0, "Reserved")) {
          Increment(DEVC)
          Return ()
        }

        Store (Arg2, Index (DeRefOf (Index (\_SB.PEPD.DEVY, DEVC)), 1)) // Update Device Constraint

        If (LEqual (Arg3, 0)) {
          Store (PCP0, Index (DeRefOf(Index (\_SB.PEPD.DEVY, DEVC)), 2))
        } ElseIf (LEqual (Arg3, 1)) {
          Store (PCP1, Index (DeRefOf(Index (\_SB.PEPD.DEVY, DEVC)), 2))
        } ElseIf (LEqual (Arg3, 2)) {
          Store (PCP2, Index (DeRefOf(Index (\_SB.PEPD.DEVY, DEVC)), 2))
        } ElseIf (LEqual (Arg3, 3)) {
          Store (PCP3, Index (DeRefOf(Index (\_SB.PEPD.DEVY, DEVC)), 2))
        } Else {
          // If this case is evaluated, it means wrong D-state parameter is passed and it needs to fix.
         ADBG ("PEP Error !!! Unknown D- state constraint is passed..")
        }

        ADBG (Concatenate("DEVY entry index :", ToDecimalString(DEVC)))
        ADBG (Concatenate ("Device Descriptor                  :", DeRefOf(Index (DeRefOf (Index (\_SB.PEPD.DEVY, DEVC)), 0))))
        ADBG (Concatenate ("Constraint (Enable/Disable)        :", DeRefOf(Index (DeRefOf (Index (\_SB.PEPD.DEVY, DEVC)), 1))))
        ADBG (Concatenate ("Constraint applied (D0/D0F1/D2/D3) :", Arg3))
        ADBG ("------------ ")
        ADBG ("")

        Increment(DEVC)
       } Else {
         // If you are trying to add new device in DEVY package and the actual size of the DEVY pkg is smaller
         // then the number of entries you are trying to add then this error codition is evaluated.
         // In this case you will need to increase the size of DEVY package by adding one more entry of
         // DEVY_INITIALIZER, along with that make sure to update DEVS
         ADBG (Concatenate ("PEP UPDE Error!!!! DEVY size is excedded, DEVY size = ", ToDecimalString (DEVS)))
       }
    }

    // (UPPR) Update Pep Pcie RootPorts
    // Arguments: (4)
    // Arg0: Device object. For ex:  \_SB.PC00.RP01
    // Arg1: Device type (Call GRPT while using UPPR)
    // Arg2: Root bridge string. For Ex: "\_SB.PC00"
    // Arg3: Device string. For ex: "RP01"
    Method (UPPR, 4, Serialized) {
      If (CondRefOf(Arg0)) {
        Store(GRPC(Arg1), Local0)
        If(LNotEqual(Local0, 0)) {
          UPDATE_PEP_DEV_LIST(Arg2, Arg3, 0x01, Local0)
        } Else {
          UPDATE_PEP_DEV_LIST(Arg2, Arg3, 0x00, Local0)
        }
      }
    }

    //
    // Get User Configured PEP Constraint
    // Arguments: (2)
    //    Arg0 - low bit index in PEPC
    //    Arg1 - high bit index in PEPC
    // Returns: 0 ->no constraint; 1->D0 F1; 3->D3
    //
    Method (GUCP, 2, Serialized) {
      If (LEqual (And (PEPC, Arg0), Arg0)) {
        If (LEqual (And (PEPC, Or (Arg1, Arg0)), Or (Arg1, Arg0))) {
          ADBG ("D3 F0 constraint")
          Return (3)
        } Else {
          ADBG ("D0 F1 constraint")
          Return (1)
        }
      } Else {
        ADBG ("No constraint")
        Return (0)
      }
    }

    //
    // Get PEP Constraints of PCIe Root Port of specific EP type
    // Arguments: (1)
    //    Arg0 - PCIE EP Device type
    //         0->Other
    //         1->Storage
    //         2->LAN - PCH
    //         3->WLAN - PCH
    //         4->GFX - CPU, PCH
    //         5->DTBT - PCH
    //         6->WWAN - PCH
    //         99->Invalid (EP absent)
    // Returns: PeP constrant per EP type
    //         0 ->no constraint; 1->D0 F1; 3->D3
    //
    Method (GRPC, 1, Serialized) {
      If (LEqual (Arg0, 1)) {
        ADBG ("PCIE Storage PEP")
        Return (GUCP (BIT0 << PEP_PCIE_STORAGE_CONS_INDEX, BIT1 << PEP_PCIE_STORAGE_CONS_INDEX))
      }

      If (LEqual (Arg0, 2)) {
        ADBG ("PCIE LAN PEP")
        Return (GUCP (BIT0 << PEP_PCIE_LAN_CONS_INDEX, BIT1 << PEP_PCIE_LAN_CONS_INDEX))
      }

      If (LEqual (Arg0, 3)) {
        ADBG ("PCIE WLAN PEP")
        Return (GUCP (BIT0 << PEP_PCIE_WLAN_CONS_INDEX, BIT1 << PEP_PCIE_WLAN_CONS_INDEX))
      }

      If (LEqual (Arg0, 4)) {
        ADBG ("PCIE Graphics PEP")
        Return (GUCP (BIT0 << PEP_PCIE_GFX_CONS_INDEX, BIT1 << PEP_PCIE_GFX_CONS_INDEX))
      }

      If (LEqual (Arg0, 5)) {
        ADBG ("PCH PCIE DTBT PEP")
        Return (GUCP (BIT0 << PEP_TCSS_CONS_INDEX, BIT0 << PEP_TCSS_CONS_INDEX))
      }

      If (LEqual (Arg0, 6)) {
        ADBG ("PCIE WWAN PEP")
        If (LEqual (WRTO, 3)) {
          ADBG ("PEP Constraint = D3")
          Return (3)
        } ElseIf (LEqual (WRTO, 2)) {  // D0 F1 for D3/L1.2
          ADBG ("PEP Constraint = D0")
          Return (1)
        } Else {
          ADBG ("PEP Constraint = no constraint")
          Return (0) // if WRTO is not 3, then PCIR RP Constraint should be disable
        }
      }

      If(LEqual (Arg0, 0)) {
        ADBG("PCIE Others")
        Return(GUCP(BIT0 << PEP_PCIE_OTHER_CONS_INDEX, BIT1 << PEP_PCIE_OTHER_CONS_INDEX))
      }

      ADBG ("PCIE EP NOT present")
      Return (0)
    }

    Method (SPDE, 0, Serialized) {
      External (\ESPC) // Indicates if constraints should be enumerated
      //
      // Below methods check for drive presence in SATA port
      //
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT0.PRES, MethodObj)
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT1.PRES, MethodObj)
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT2.PRES, MethodObj)
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT3.PRES, MethodObj)
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT4.PRES, MethodObj)
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT5.PRES, MethodObj)
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT6.PRES, MethodObj)
      External (\_SB.ROOT_BRIDGE_COMMON.SAT0.PRT7.PRES, MethodObj)

      If (LNot (\ESPC)) {
        Return ()
      }

      If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.SAT0)) {
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT0", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT0.PRES (), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT1", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT1.PRES (), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT2", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT2.PRES (), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT3", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT3.PRES (), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT4", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT4.PRES (), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT5", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT5.PRES (), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT6", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT6.PRES (), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.PRT7", \_SB.ROOT_BRIDGE_COMMON.SAT0.PRT7.PRES (), 3)
      }
    }

    // (UPDV) Update Pep Devices
    // This function is called in _DSM to add device entries in DEVY package
    Method (UPDV, 0, Serialized) {

      Store (sizeof (DEVY), DEVS)
      ADBG (Concatenate ("PEP DEVY size = ", ToDecimalString (DEVS)))

      //
      // SATA port drive constraint enumeration
      //
      SPDE ()

      If (LEqual (And (PEPC, BIT0 << PEP_SATA_CONS_INDEX) >> PEP_SATA_CONS_INDEX, 1)) {// Storage (1:Adapter D0/F1 or 3:Adapter D3)

        // Constraint for SATA Controller (SAT0)
        If (LEqual (And (PEPC, Or (BIT1 << PEP_SATA_CONS_INDEX, BIT0 << PEP_SATA_CONS_INDEX)) >> PEP_SATA_CONS_INDEX, 1)) { // Storage (1:Adapter D0/F1)
          UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0", (And (PEPC, BIT0 << PEP_SATA_CONS_INDEX) >> PEP_SATA_CONS_INDEX), 1)
        } ElseIf (LEqual (And (PEPC, Or (BIT1 << PEP_SATA_CONS_INDEX, BIT0 << PEP_SATA_CONS_INDEX)) >> PEP_SATA_CONS_INDEX, 3)) { // Storage (3:Adapter D3)
            UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0", (And (PEPC, BIT0 << PEP_SATA_CONS_INDEX) >> PEP_SATA_CONS_INDEX), 3)
        }
      } ElseIf (LEqual (And (PEPC, Or (BIT1 << PEP_SATA_CONS_INDEX, BIT0 << PEP_SATA_CONS_INDEX)) >> PEP_SATA_CONS_INDEX, 2)) {
          // Storage (0:None, 1:Adapter D0/F1, 2:Raid, 3:Adapter D3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SAT0.VOL0", 1, 1)
      }

        // Apply PEP constraints for CPU,
        // Based on number of threads
        ADBG (Concatenate ("Pep TCNT : ", ToHexString (TCNT)))
         For (Local0 = Zero, Local0 < TCNT, Local0 += 1) {
          If ( Local0 < 10){
            Concatenate ("PR0", Local0, Local1)
          }
          Else {
            Concatenate ("PR", ToDecimalString(Local0), Local1)
          }

          If (LEqual (And (PEPC, BIT0 << PEP_CPU_CONS_INDEX), 0)) {
            UPDATE_PEP_DEV_LIST("\\_SB", Local1, 0x00, 0)
          } Else {
            UPDATE_PEP_DEV_LIST("\\_SB", Local1, 0X01, 0)
          }
         }

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "GFX0", (And (PEPC, BIT0 << PEP_GFX_CONS_INDEX) >> PEP_GFX_CONS_INDEX), 3)

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "HDAS", 0x1, GUCP(BIT0 << PEP_AUDIO_CONS_INDEX, BIT1 << PEP_AUDIO_CONS_INDEX))
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "UA00", (And (PEPC, BIT0 << PEP_UART0_CONS_INDEX) >> PEP_UART0_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "UA01", (And (PEPC, BIT0 << PEP_UART1_CONS_INDEX) >> PEP_UART1_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "UA02", (And (PEPC, BIT0 << PEP_UART2_CONS_INDEX) >> PEP_UART2_CONS_INDEX), 3)

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "I2C0", PEPC_PARSE(PEPC, PEP_I2C0_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "I2C1", PEPC_PARSE(PEPC, PEP_I2C1_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "I2C2", PEPC_PARSE(PEPC, PEP_I2C2_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "I2C3", PEPC_PARSE(PEPC, PEP_I2C3_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "I2C4", PEPC_PARSE(PEPC, PEP_I2C4_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "I2C5", PEPC_PARSE(PEPC, PEP_I2C5_CONS_INDEX), 3)

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "I2C6", PEPC_PARSE(PEPC, PEP_I2C6_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "I2C7", PEPC_PARSE(PEPC, PEP_I2C7_CONS_INDEX), 3)

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SPI0", PEPC_PARSE(PEPC, PEP_SPI0_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SPI1", PEPC_PARSE(PEPC, PEP_SPI1_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "SPI2", PEPC_PARSE(PEPC, PEP_SPI2_CONS_INDEX), 3)

#if (FixedPcdGet8(PcdMtlSSupport) == 0)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "XHCI", PEPC_PARSE(PEPC, PEP_XHCI_CONS_INDEX), 3)
#else
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_PCH_STR, "XHCI", PEPC_PARSE(PEPC, PEP_XHCI_CONS_INDEX), 3)
#endif

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "PEMC", PEPC_PARSE(PEPC, PEP_EMMC_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "PSDC", PEPC_PARSE(PEPC, PEP_SDXC_CONS_INDEX), 3)

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "IPU0", PEPC_PARSE(PEPC, PEP_IPU_CONS_INDEX), 3)

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "HECI", PEPC_PARSE(PEPC, PEP_CSME_CONS_INDEX), 1)

#if (FixedPcdGet8(PcdMtlSSupport) == 0)
      If (LOr (LEqual (PEPC_PARSE(PEPC, PEP_GBE_CONS_INDEX), 0), LEqual (GBES, 0))) { // En/Dis LAN (GBE)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "GLAN", 0, 3)
      } Else {
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "GLAN", 1, 3)
      }
#else
      If (LOr (LEqual (PEPC_PARSE(PEPC, PEP_GBE_CONS_INDEX), 0), LEqual (\_SB.ROOT_BRIDGE_COMMON.GBES, 0))) { // En/Dis LAN (GBE)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "GLAN", 0, 3)
      } Else {
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "GLAN", 1, 3)
      }
#endif

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "THC0", PEPC_PARSE(PEPC, PEP_THC0_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "THC1", PEPC_PARSE(PEPC, PEP_THC1_CONS_INDEX), 3)

      If (LEqual (WRTO, 1)) {
        UPDATE_PEP_DEV_LIST("USB\\VID_8087&PID_0AC9&MI*", "", 1, WRTO)
        ADBG ("PEP Constraint for CDC MBIM = D0F1")
      } Else {
        UPDATE_PEP_DEV_LIST("USB\\VID_8087&PID_0AC9&MI*", "", 0, 2)
      }

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "GNA0", PEPC_PARSE(PEPC, PEP_GNA_CONS_INDEX), 3)

      If (CondRefOf(\_SB.ROOT_BRIDGE_SOC.HEC3)) {
        ADBG("HECI3 PEP")
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "HEC3", PEPC_PARSE(PEPC, PEP_HECI3_CONS_INDEX), 1)
      }

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "PUF0", PEPC_PARSE(PEPC, PEP_UFS0_CONS_INDEX), 3)
      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "PUF1", PEPC_PARSE(PEPC, PEP_UFS1_CONS_INDEX), 3)

      UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "VPU0", PEPC_PARSE(PEPC, PEP_VPU0_CONS_INDEX), 3)

      If (LNotEqual (And (PEPC, BIT0 << PEP_TCSS_CONS_INDEX), 0)) { // En/Dis TCSS IPs
        If (LEqual (THCE, 1)) {
          UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TXHC", 1, 3) // TCSS XHCI
        } Else {
          UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TXHC", 0, 3)
        }

        If(LEqual(TDCE, 1)) {
          UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TXDC", 1, 3) // TCSS XDCI
        } Else {
          UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TXDC", 0, 3) // TCSS XDCI
        }

        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP0", TRE0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP1", TRE1, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP2", TRE2, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP3", TRE3, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TDM0", DME0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TDM1", DME1, 3)

      } Else {
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TXHC", 0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TXDC", 0, 3) // TCSS XDCI
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP0", 0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP1", 0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP2", 0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TRP3", 0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TDM0", 0, 3)
        UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_SOC_STR, "TDM1", 0, 3)
      }

      UPDATE_PEP_DEV_LIST("\\_SB.PC01", "TRP0", 0, 0)
      UPDATE_PEP_DEV_LIST("\\_SB.PC01", "TRP1", 0, 0)
      UPDATE_PEP_DEV_LIST("\\_SB.PC01", "TRP2", 0, 0)
      UPDATE_PEP_DEV_LIST("\\_SB.PC01", "TRP3", 0, 0)

#if (FixedPcdGet8(PcdMtlSSupport) == 0)
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP01, \_SB.ROOT_BRIDGE_SOC.RP01.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP01")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP02, \_SB.ROOT_BRIDGE_SOC.RP02.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP02")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP03, \_SB.ROOT_BRIDGE_SOC.RP03.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP03")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP04, \_SB.ROOT_BRIDGE_SOC.RP04.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP04")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP05, \_SB.ROOT_BRIDGE_SOC.RP05.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP05")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP06, \_SB.ROOT_BRIDGE_SOC.RP06.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP06")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP07, \_SB.ROOT_BRIDGE_SOC.RP07.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP07")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP08, \_SB.ROOT_BRIDGE_SOC.RP08.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP08")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP09, \_SB.ROOT_BRIDGE_SOC.RP09.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP09")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP10, \_SB.ROOT_BRIDGE_SOC.RP10.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP10")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP11, \_SB.ROOT_BRIDGE_SOC.RP11.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP11")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP12, \_SB.ROOT_BRIDGE_SOC.RP12.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP12")
#else
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP01, \_SB.ROOT_BRIDGE_SOC.RP01.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP01")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP09, \_SB.ROOT_BRIDGE_SOC.RP09.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP09")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP13, \_SB.ROOT_BRIDGE_SOC.RP13.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP13")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP17, \_SB.ROOT_BRIDGE_SOC.RP17.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP17")
      UPPR(\_SB.ROOT_BRIDGE_SOC.RP21, \_SB.ROOT_BRIDGE_SOC.RP21.PXSX.GRPT (), ROOT_BRIDGE_SOC_STR, "RP21")

      UPPR(\_SB.ROOT_BRIDGE_PCH.RP01, \_SB.ROOT_BRIDGE_PCH.RP01.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP01")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP02, \_SB.ROOT_BRIDGE_PCH.RP02.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP02")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP03, \_SB.ROOT_BRIDGE_PCH.RP03.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP03")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP04, \_SB.ROOT_BRIDGE_PCH.RP04.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP04")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP05, \_SB.ROOT_BRIDGE_PCH.RP05.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP05")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP06, \_SB.ROOT_BRIDGE_PCH.RP06.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP06")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP07, \_SB.ROOT_BRIDGE_PCH.RP07.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP07")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP08, \_SB.ROOT_BRIDGE_PCH.RP08.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP08")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP09, \_SB.ROOT_BRIDGE_PCH.RP09.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP09")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP10, \_SB.ROOT_BRIDGE_PCH.RP10.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP10")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP11, \_SB.ROOT_BRIDGE_PCH.RP11.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP11")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP12, \_SB.ROOT_BRIDGE_PCH.RP12.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP12")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP13, \_SB.ROOT_BRIDGE_PCH.RP13.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP13")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP14, \_SB.ROOT_BRIDGE_PCH.RP14.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP14")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP15, \_SB.ROOT_BRIDGE_PCH.RP15.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP15")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP16, \_SB.ROOT_BRIDGE_PCH.RP16.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP16")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP17, \_SB.ROOT_BRIDGE_PCH.RP17.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP17")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP18, \_SB.ROOT_BRIDGE_PCH.RP18.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP18")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP19, \_SB.ROOT_BRIDGE_PCH.RP19.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP19")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP20, \_SB.ROOT_BRIDGE_PCH.RP20.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP20")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP21, \_SB.ROOT_BRIDGE_PCH.RP21.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP21")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP22, \_SB.ROOT_BRIDGE_PCH.RP22.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP22")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP23, \_SB.ROOT_BRIDGE_PCH.RP23.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP23")
      UPPR(\_SB.ROOT_BRIDGE_PCH.RP24, \_SB.ROOT_BRIDGE_PCH.RP24.PXSX.GRPT (), ROOT_BRIDGE_PCH_STR, "RP24")
#endif

      If (CondRefOf (\_SB.ROOT_BRIDGE_COMMON.CNVW.VDID)) {
        If (LNotEqual (\_SB.ROOT_BRIDGE_COMMON.CNVW.VDID, 0xFFFFFFFF)) {
          ADBG ("CNVi- WiFI PEP")
          UPDATE_PEP_DEV_LIST(ROOT_BRIDGE_COMMON_STR, "CNVW", 1, 3)
        }
      }

      // Below for loop is to fill empty DEVY entries with Unique names to avoid boot BSOD.
      // OS pep driver does not accept mutliple entries with same device descriptor, which results in BSOD
      //
      // Note: Always make sure to add new device entries before this code.
      //
      For (Local0 = DEVC, Local0 < DEVS, Local0 += 1) {
        UPDATE_PEP_DEV_LIST("Reserved", ToDecimalString (Local0), 0, 0)
      }
      Store(0,DEVC)
      ADBG ("[PEP] All Pep Constraints are updated ")
    }

    Name (BCCD, Package () // Bugcheck Critical Device (s)
    {  })

    //
    // DSDV to notify OSPM about CEC PS_ON support.
    //
    Name (DSDV, Package (0x02) { // Using Name object to be compatible for different OSPM
      ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package (0x01) {
        Package (0x02) {
          "intel-cec-pson-switching-enabled-in-s0",
          0 // to be updated in PSOP
        }
      }
    })

    Method (_DSD) {
      //
      // Loop through DSDV object to find index for UUID of interest.
      // Stride length is 2 because list of properties comes right after the corresponding UUID.
      //
      For (Local0 = Zero, Local0 < SizeOf (DSDV), Local0 += 2) {
        If (LNotEqual (DerefOf (DSDV[Local0]), ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"))) {
          // skip if not the uuid of interest
          Continue
        }
        // get the package associated with this UUID
        Local1 = DerefOf (DSDV[Add (Local0, 1)])
        For (Local2 = Zero, Local2 < SizeOf (Local1), Increment (Local2)) {
          // get the key/value pair at the index
          Local3 = DerefOf (Local1[Local2])
          If (LAnd (LEqual ("intel-cec-pson-switching-enabled-in-s0", DerefOf (Local3[Zero])), PSON)) {
            ADBG ("PEPD: Storing 1 in DSDV Object")
            DerefOf (DerefOf (DSDV[Local0 + 1])[Local2])[One] = One
          }
        }
      }
      Return (DSDV)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      If (LEqual (S0ID, 1))
      {
        Return (0xf)
      }
      Return (0)
    }

    Method (_DSM, 0x4, Serialized)
    {
      ADBG (Concatenate ("PEPC = ", ToHexString (PEPC)))

      If (LEqual (Arg0,ToUUID ("11E00D56-CE64-47ce-837B-1F898F9AA461")))
      {
        // Number of Functions (including this one)
        If (LEqual (Arg2, Zero))
        {
          Return (Buffer () {0x81, 0x01})
        }

        If (LEqual (Arg2, 0x7))
        {
          If (LEqual (PFLV,FlavorDesktop)) {
            If (CondRefOf (ECON)) {
              If (LEqual (ECON, 1)) {
                If (LEqual (PPOE, 1)) {
                  If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.LPCB.NRTS)) {
                    \_SB.ROOT_BRIDGE_SOC.LPCB.NRTS (0)
                  }
                }
              }
            }
          }
        }

        If (LEqual (Arg2, 0x8))
        {
          If (LEqual (PFLV,FlavorDesktop)) {
            If (CondRefOf (ECON)) {
              If (LEqual (ECON, 1)) {
                If (LEqual (PPOE, 1)) {
                  If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.LPCB.NRTS)) {
                    \_SB.ROOT_BRIDGE_SOC.LPCB.NRTS (1)
                  }
                }
              }
            }
          }
        }
      }
      If (LEqual (Arg0,ToUUID ("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
      {
        // Number of Functions (including this one)
        If (LEqual (Arg2, Zero))
        {
          Return (Buffer (One) {0x7F})
        }
        // Device Constraints Enumeration
        If (LEqual (Arg2, One))
        {
          If (LEqual (S0ID, 0)) {
            Return (Package () {})
          }

          If (LEqual (IPDI, 0)) { // Check if DEVY is already initialzed or not.
             UPDV()
             Store(1, IPDI)
          } Else {
            ADBG ("PEP DEVY is already initialized !!! ")
          }
          Return (DEVY)
        }

        // BCCD
        If (LEqual (Arg2, 2))
        {
          Return (BCCD)
        }

        // Screen off notification
        If (LEqual (Arg2, 0x3))
        {
          // no action for screen off
          If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.IMNG)) {
            \_SB.ROOT_BRIDGE_SOC.IMNG (Arg2)
          }
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            D8XH (0, 0xC5)
            D8XH (1, 0)
          }
        }

        // Screen on notification
        If (LEqual (Arg2, 0x4))
        {
          // no action for screen on
          If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.IMNG)) {
            \_SB.ROOT_BRIDGE_SOC.IMNG (Arg2)
          }
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            D8XH (0, 0xC5)
            D8XH (1, 0xAB)
          }
        }

        // resiliency phase entry (deep standby entry)
        Store (0, Local0) // Display All Monitor off flag
        If (LEqual (Arg2, 0x5))
        {
          ADBG ("[PEP] deep standby entry")
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \GUAM (1) // 0x01 - Power State Standby (CS Resiliency Entry)
          }

          If (LEqual(\ECON,0)) // EC not available.
          {
            If(LEqual(ITSI,1)) // SIO present.
            {
              \_SB.ROOT_BRIDGE_SOC.LPCB.ITE8.SETS(0x18)
              \_SB.ROOT_BRIDGE_SOC.LPCB.ITE8.SETP(0)
            }
          }

          // Call method to notify EC of Idle Resiliency entry
          \_SB.ROOT_BRIDGE_SOC.LPCB.NTIR (1)

          If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.TXHC)) {
            // Local0 = 1 - Give indication to IOM that all the display is OFF
            // Local0 = 0
            // If system is running in FW CM mode, will need to raise DPOF bit
            If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.TDM0)) {  // check DMA0
              If (LEqual(\_SB.ROOT_BRIDGE_SOC.TDM0._STA, 0x0F)) {  // check DMA0
                If (LEqual (\_SB.ROOT_BRIDGE_SOC.TDM0.SCME, 0)) {
                  Store (1, Local0)
                  ADBG (Concatenate ("TDM0 set DPOF to ", ToHexString (Local0)))
                }
              }
            }
            If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.TDM1)) {  // check DMA1
              If (LEqual(\_SB.ROOT_BRIDGE_SOC.TDM1._STA, 0x0F)) {  // check DMA1
                If (LEqual (\_SB.ROOT_BRIDGE_SOC.TDM1.SCME, 0)) {
                  Store (1, Local0)
                  ADBG (Concatenate ("TDM1 set DPOF to ", ToHexString (Local0)))
                }
              }
            }
            If (LEqual (Local0, 1)) {
              Store (1, \_SB.ROOT_BRIDGE_SOC.DPOF)
            }
            ADBG (Concatenate ("All Monitor off flag: ", ToHexString (\_SB.ROOT_BRIDGE_SOC.DPOF)))

            If (CondRefOf (\GPRV)) {
              ADBG ("[PEP] allow PS_ON")
              \GPRV(3, 0)
            }
          }
        }

        // resiliency phase exit (deep standby exit)
        If (LEqual (Arg2, 0x6))
        {
          ADBG ("[PEP] deep standby exit")
          If (LEqual (S0ID, 1)) { //S0ID: >=1: CS 0: non-CS
            // call method specific to CS platforms when the system is in a
            // standby state with very limited SW activities
            \GUAM (0) // 0x00 - Power State On (CS Resiliency Exit)
          }
          If (LEqual(\ECON,0)) // EC not available.
          {
            If(LEqual(ITSI,1)) // SIO present.
            {
              \_SB.ROOT_BRIDGE_SOC.LPCB.ITE8.SETP(0xE0)
              \_SB.ROOT_BRIDGE_SOC.LPCB.ITE8.SETS(0x19)
            }
          }
          // Call method to notify EC of Idle Resiliency exit
          \_SB.ROOT_BRIDGE_SOC.LPCB.NTIR (0)
          If (CondRefOf (\_SB.ROOT_BRIDGE_SOC.TXHC)) {
            Store (0, \_SB.ROOT_BRIDGE_SOC.DPOF) //Clear bit17 to indicate IOM that all the display is not OFF.
          }
          If (CondRefOf (\GPRV)) {
            ADBG ("[PEP] veto PS_ON")
            \GPRV(3, 1)
          }
        }
      }// If (LEqual (Arg0,ToUUID ("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))

      Return (Buffer () {0})
    } // Method (_DSM)
  } //device (PEPD)
} // End Scope (\_SB)
