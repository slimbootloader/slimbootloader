/** @file
  ACPI DSDT table

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PlatformBoardId.h>
#include <Register/PmcRegs.h>

// Definitions for INTB (Interrupt descriptor buffer method)
#define INT_LEVEL_TRIG  0
#define INT_EDGE_TRIG   1
#define INT_ACTIVE_LOW  0
#define INT_ACTIVE_HIGH 1

DefinitionBlock (
  "DSDT.aml",
  "DSDT",
  0x02, // DSDT revision.
        // A Revision field value greater than or equal to 2 signifies that integers
        // declared within the Definition Block are to be evaluated as 64-bit values
  "INTEL",   // OEM ID (6 byte string)
  "SKL     ",// OEM table ID  (8 byte string)
  0x0 // OEM version of DSDT table (4 byte Integer)
)

// BEGIN OF ASL SCOPE
{
  //External (\_SB.PC00.GFX0.CLID)
  External(VMDE)
  External (\_SB.PC00.GFX0.CDCK)
  External (\_SB.PC00.GFX0.DD1F, DeviceObj)
  External (\_SB.PC00.GFX0.DD2F, DeviceObj)
  External (\_SB.PC00.SAT0.PRT6, DeviceObj)
  External (\_SB.PC00.SAT0.PRT7, DeviceObj)
  External (\_SB.PC00.GFX0.GDCK, MethodObj)
  External (\_SB.PC00.GFX0.GLID, MethodObj)
  External (\_SB.PC00.RP01, DeviceObj)
  External (\_SB.PC00.RP02, DeviceObj)
  External (\_SB.PC00.RP03, DeviceObj)
  External (\_SB.PC00.RP04, DeviceObj)
  External (\_SB.PC00.RP05, DeviceObj)
  External (\_SB.PC00.RP06, DeviceObj)
  External (\_SB.PC00.RP07, DeviceObj)
  External (\_SB.PC00.RP08, DeviceObj)
  External (\_SB.PC00.RP09, DeviceObj)
  External (\_SB.PC00.RP10, DeviceObj)
  External (\_SB.PC00.RP11, DeviceObj)
  External (\_SB.PC00.RP12, DeviceObj)
  External (\_SB.PC02.RP01, DeviceObj)
  External (\_SB.PC02.RP04, DeviceObj)
  External (\_SB.PC02.RP05, DeviceObj)
  External (\_SB.PC02.RP08, DeviceObj)
  External (\_SB.PC02.RP09, DeviceObj)
  External (\_SB.PC02.RP21, DeviceObj)
  External (\_SB.PR00._PSS, MethodObj)
  External (\_SB.PR00.LPSS, PkgObj)
  External (\_SB.PR00.TPSS, PkgObj)
  External (\_SB.PR00._PPC, MethodObj)
  External (\_SB.CPPC, IntObj)
  External (\_TZ.TZ00, DeviceObj)
  External (\_TZ.ETMD, IntObj)
  External (\_TZ.FN00._OFF, MethodObj)
  External (\_SB.PC00.TXHC, DeviceObj) // CPU XHCI object of TCSS, it is a must existed object for CPU TCSS
  External (\STD3, FieldUnitObj) // 0: D3 disable. 1: D3Hot support for Storages. 2: D3Cold support for Storages.
  // Miscellaneous services enabled in Project
  External(DGBA)
  External(\_SB.PC00.RP12.CLID)
  External(\_SB.PC00.RP12.CBLV)
  External(\_SB.PC00.RP12.DLID, MethodObj)
  //Name (VMNB, 0xFFFF0000)
  //Name (VMNL, 0xAA55)

  Name (BRMT, 0)
  Name (RDLY, 0)

  Include ("PcieRpObjCommonExternalInclude.asl")

  Include ("AmlUpd.asl")
  Include ("PlatformNvs.asl")
  Include ("PciTree.asl")
  //Include ("TimeAndAlarmDevice.asl")
  //Include ("PmaxDev.asl")
  //Include ("AcpiAssert.asl")

  if(LEqual(ECR1,1)){
    Scope(\_SB.PC00) {
      //
      // PCI-specific method's GUID
      //
      Name(PCIG, ToUUID("E5C937D0-3553-4d7a-9117-EA4D19C3434D"))
      //
      // PCI's _DSM - an attempt at modular _DSM implementation
      // When writing your own _DSM function that needs to include PCI-specific methods, do this:
      //
      // Method(_YOUR_DSM,4){
      //   if(Lequal(Arg0,PCIG)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }
      //   ...continue your _DSM by checking different GUIDs...
      //   else { return(0) }
      // }
      //
      Method(PCID, 4, Serialized) {
        If(LEqual(Arg0, PCIG)) {         // PCIE capabilities UUID
          If(LGreaterEqual(Arg1,3)) {                                              // revision at least 3
            If(LEqual(Arg2,0)) { Return (Buffer(2){0x01,0x03}) }                   // function 0: list of supported functions
            If(LEqual(Arg2,8)) { Return (1) }                                      // function 8: Avoiding Power-On Reset Delay Duplication on Sx Resume
            If(LEqual(Arg2,9)) { Return (Package(5){50000,Ones,Ones,50000,Ones}) } // function 9: Specifying Device Readiness Durations
          }
        }
        return (Buffer(1){0})
      }
    }//scope
    If (CondRefOf (\_SB.PC02)) {
      Scope (\_SB.PC02) {
        Method (PCID, 4, Serialized) {
          Return (\_SB.PC02.PCID (Arg0, Arg1, Arg2, Arg3))
        }
      }
    }
  }//if

  Scope(\_SB.PC00) {
    //PciCheck, Arg0=UUID, returns true if support for 'PCI delays optimization ECR' is enabled and the UUID is correct
    Method(PCIC,1,Serialized) {
      If(LEqual(ECR1,1)) {
        If(LEqual(Arg0, PCIG)) {
          return (1)
        }
      }
      return (0)
    }

    //
    // Create I2C Bus Resource descriptor for _CRS usage
    // Arg0 - I2C bus address of the connection (Target Address)
    // Arg1 - I2C controller number (Resource Source)
    // Returns buffer with 'I2cSerialBusV2' resource descriptor
    //
    Method (IICB, 2, Serialized) {
      Switch (ToInteger(Arg1)) {
        Case (0) {Name (IIC0, ResourceTemplate () {I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C0", 0x00, ResourceConsumer, DEV0, , ) })
          CreateWordField (IIC0, DEV0._ADR, DAD0)
          Store (Arg0, DAD0)
          Return (IIC0) }
        Case (1) {Name (IIC1, ResourceTemplate () {I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C1", 0x00, ResourceConsumer, DEV1, , ) })
          CreateWordField (IIC1, DEV1._ADR, DAD1)
          Store (Arg0, DAD1)
          Return (IIC1) }
        Case (2) {Name (IIC2, ResourceTemplate () {I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C2", 0x00, ResourceConsumer, DEV2, , ) })
          CreateWordField (IIC2, DEV2._ADR, DAD2)
          Store (Arg0, DAD2)
          Return (IIC2) }
        Case (3) {Name (IIC3, ResourceTemplate () {I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C3", 0x00, ResourceConsumer, DEV3, , ) })
          CreateWordField (IIC3, DEV3._ADR, DAD3)
          Store (Arg0, DAD3)
          Return (IIC3) }
        Case (4) {Name (IIC4, ResourceTemplate () {I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C4", 0x00, ResourceConsumer, DEV4, , ) })
          CreateWordField (IIC4, DEV4._ADR, DAD4)
          Store (Arg0, DAD4)
          Return (IIC4) }
        Case (5) {Name (IIC5, ResourceTemplate () {I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C5", 0x00, ResourceConsumer, DEV5, , ) })
          CreateWordField (IIC5, DEV5._ADR, DAD5)
          Store (Arg0, DAD5)
          Return (IIC5) }
        Case(6) { Name (IIC6 , ResourceTemplate () { I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C6", 0x00, ResourceConsumer, DEV6,) })
          CreateWordField (IIC6, DEV6._ADR, DAD6)
          Store (Arg0, DAD6)
          Return (IIC6) }
        Case(7) { Name (IIC7 , ResourceTemplate () { I2cSerialBusV2 (0, ControllerInitiated, 400000, AddressingMode7Bit,
          "\\_SB.PC00.I2C7", 0x00, ResourceConsumer, DEV7,) })
          CreateWordField (IIC7, DEV7._ADR, DAD7)
          Store (Arg0, DAD7)
          Return (IIC7) }
        Default {Return (0)}
      }
    }

    //
    // Create I2C Bus Resource descriptor for _CRS usage
    // Arg0 - I2C bus address of the connection (Peripheral Address)
    // Arg1 - I2C Device number
    // Returns buffer with 'I2cSerialBusV2' resource descriptor
    //
    Method (VIIC, 2, Serialized) {
      Switch (ToInteger (Arg1))
      {
        Case (Zero) // When virtual I2C device VIC0 is selected
        {
          Name (VI00, ResourceTemplate () {
              I2cSerialBusV2 (0, ControllerInitiated, 400000,
                  AddressingMode7Bit, "\\_SB.PC00.XHCI.RHUB.VIC0",
                  0x00, ResourceConsumer, VI0P,)
          })

          CreateWordField (VI00, VI0P._ADR, DAR0)
          Store (Arg0, DAR0)
          Return (VI00)
        }
        Case (One) // When virtual I2C device VIC1 is selected
        {
          Name (VI11, ResourceTemplate () {
              I2cSerialBusV2 (0, ControllerInitiated, 400000,
                  AddressingMode7Bit, "\\_SB.PC00.XHCI.RHUB.VIC1",
                  0x00, ResourceConsumer, VI1M,)
          })

          CreateWordField (VI11, VI1M._ADR, DAQ1)
          Store (Arg0, DAQ1)
          Return (VI11)
        }
        Default
        {
          Return (ResourceTemplate () {I2cSerialBusV2 (0, ControllerInitiated, 400000,AddressingMode7Bit, "\\_SB.PC00.XHCI.RHUB.VICX",0x00, ResourceConsumer, ,)}) // Dummy
        }
      }
    }

    //
    // Create Interrupt Resource descriptor for _CRS usage
    // Arg0 - GPIO Pad used as Interrupt source
    // Arg1 - Edge (1) or Level (0) interrupt triggering
    // Arg2 - Active Level: High (1) or Low (0)
    // Returns buffer with 'Interrupt' resource descriptor
    //
    Method (INTB, 3, Serialized) {
      Name (INTR, ResourceTemplate () { Interrupt (ResourceConsumer, Level, ActiveLow, ExclusiveAndWake,,,INTI) {0} })
      // Update APIC Interrupt descriptor
      CreateDWordField (INTR, INTI._INT, NUMI) // Interrupt Number
      Store (INUM(Arg0), NUMI)
      CreateBitField (INTR, INTI._HE, LEVI) // Level or Edge
      Store (Arg1, LEVI)
      CreateBitField (INTR, INTI._LL, ACTI) // Active High or Low
      Store (Arg2, ACTI)

      Return (INTR)
    }

  } // END Scope(\_SB.PC00)

  If (CondRefOf (\_SB.PC02)) {
    Scope (\_SB.PC02) {
      //PciCheck, Arg0=UUID, returns true if support for 'PCI delays optimization ECR' is enabled and the UUID is correct
      Method (PCIC,1,Serialized) {
        Return (\_SB.PC00.PCIC (Arg0))
      }
    }
  }

  Include ("MtlSoc.asl")

  If (LNotEqual (PCHA, 0)) {
    Include ("MtlPch.asl")
  }
  Include ("PcieMtl.asl")
  #define CNVI_ROOT_BRIDGE  PC00
  #define CNVI_UART_DEV_STR "\\_SB.PC00.UA00"

  //Include ("Pch.asl")  // Not in this package. Refer to the PCH Reference Code accordingly
  Include ("LpcB.asl")
  Include ("Platform.asl")
  Include ("Cpu.asl")
  Include ("PciDrc.asl")
  Include ("Video.asl")
  Include ("Gpe.asl")
  Include ("PchRpPxsxWrapper.asl")
  Include ("Pep.asl")
  Include ("SerialIoDevices.asl")
  Include ("HdaDspModules.asl")
  Include ("HidPlatformEventDev.asl")
  Include ("HidDynamicSarDsm.asl")
  Include ("PinDriverLib.asl")
  Include ("FwuWmi.asl")

  // If (LEqual (MCFS,1)) {
  //   Include ("Mcf.asl")
  // }

  If (LEqual (PCHA, 0)) {
    Scope (\_SB.PC00.SAT0) {
      //
      // Check for platform defined _DSM method support
      //
      // Arg0 - _DSM GUID
      //
      Method(SATC, 1, Serialized) {
        If(LEqual(Arg0, ToUUID("E4DB149B-FCFE-425b-A6D8-92357D78FC7F"))) {
          Return (1)
        } Else {
          Return (0)
        }
      }

      //
      // Execute platform defined _DSM method
      //
      // Arg0 - _DSM GUID
      // Arg1 - _DSM revision
      // Arg2 - Function index
      // Arg3 - Package with parameters
      //
      Method(SATD, 4, Serialized) {
        Switch (ToInteger(Arg2)) {
          Case (0) {
            ///Case 0: Standard query - A bitmask of functions supported
            Return(Buffer() {0x3})
          }
          Default {
            Return (0)
          }
        }
        Return(Buffer() {0x0})
      }

      //
      // ACPI Method to support PUIS capability for each Sata port
      //
      If (LAnd (LEqual (\S0ID,1), LEqual (\PUIS,1))) {
        Scope (PRT0) {
          Include ("SataPuis.asl")
        }

        Scope (PRT1) {
          Include ("SataPuis.asl")
        }

        Scope (PRT2) {
          Include ("SataPuis.asl")
        }

        Scope (PRT3) {
          Include ("SataPuis.asl")
        }

        Scope (PRT4) {
          Include ("SataPuis.asl")
        }

        Scope (PRT5) {
          Include ("SataPuis.asl")
        }

        If(CondRefOf(PRT6)) {
          Scope(PRT6) {
            Include ("SataPuis.asl")
          }
        }

        If(CondRefOf(PRT7)) {
          Scope(PRT7) {
            Include ("SataPuis.asl")
          }
        }
      }
    }
  }

  //
  // Add WWAN Firmware Flash Device Support
  //
  If (LEqual (WFFD, 1)) {
    Include ("WwanFlash.asl")
  }
  Name (\_S0, Package (4) {0x0,0x0,0,0}) // mandatory System state
  if (SS1) { Name (\_S1, Package (4) {0x1,0x0,0,0})}
  if (SS3) { Name (\_S3, Package (4) {0x5,0x0,0,0})}
  if (SS4) { Name (\_S4, Package (4) {0x6,0x0,0,0})}
  Name (\_S5, Package (4) {0x7,0x0,0,0}) // mandatory System state

  Method (PTS, 1) {        // METHOD CALLED FROM _PTS PRIOR TO ENTER ANY SLEEP STATE
    If (Arg0)              // entering any sleep state
      {
      }
    }
    Method (WAK, 1) {      // METHOD CALLED FROM _WAK RIGHT AFTER WAKE UP
  }

  //
  // PCH xDCI scope for platform specific functions
  //
  If (CondRefOf (\_SB.PC00.XDCI)) {
    Scope (\_SB.PC00.XDCI) {
      Method(XDAT, 0, Serialized) {
        If (CondRefOf (\_SB.PC00.LPCB.H_EC.XDAT)) {
          Return (\_SB.PC00.LPCB.H_EC.XDAT ())
        }
        Return (0)
      }
    }
  }
}// End of ASL File
