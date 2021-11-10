/**@file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "PlatformBase.h"
#include "ScRegs/RegsPmc.h"

#define NW_GPIO_116           0x00C40728    //GP_SSP_1_RXD

// Define the following External variables to prevent a WARNING when
// using ASL.EXE and an ERROR when using IASL.EXE.


Name(ECUP, 1)  // EC State indicator: 1- Normal Mode 0- Low Power Mode
Mutex(EHLD, 0) // EC Hold indicator: 0- No one accessing the EC Power State 1- Someone else is accessing the EC Power State

Name (ADW1, 0)

//
// Create a Global MUTEX.
//
Mutex(MUTX,0)

//
// Define SW SMI port as an ACPI Operating Region to use for generate SW SMI.
//
OperationRegion (SPRT, SystemIO, 0xB2, 2)
Field (SPRT, ByteAcc, Lock, Preserve) {
  SSMP, 8
}

//
// Operational region for ACPI Control (SMI_EN) access
//
OperationRegion (SMIN, SystemIO, (ACPI_BASE_ADDRESS + R_SMI_EN), 0x4)
Field (SMIN, AnyAcc, NoLock, Preserve)
{
  SMIE, 32
}

// The _PIC Control Method is optional for ACPI design.  It allows the
// OS to inform the ASL code which interrupt controller is being used,
// the 8259 or APIC.  The reference code in this document will address
// PCI IRQ Routing and resource allocation for both cases.
//
// The values passed into _PIC are:
//   0 = 8259
//   1 = IOAPIC

Method(\_PIC,1)
{
  Store(Arg0,PICM)
}

OperationRegion(SWC0, SystemIO, 0x610, 0x0F)
Field(SWC0, ByteAcc, NoLock, Preserve)
{
  G1S, 8,      //SWC GPE1_STS
  Offset(0x4),
  G1E, 8,
  Offset(0xA),
  G1S2, 8,     //SWC GPE1_STS_2
  G1S3, 8      //SWC GPE1_STS_3
}

OperationRegion (SWC1, SystemIO, \PMBS, 0x34)
Field(SWC1, DWordAcc, NoLock, Preserve)
{
  Offset(0x20),
  G0S, 32,      //GPE0_STS
  Offset(0x30),
  G0EN, 32      //GPE0_EN
}


OperationRegion (PMCM, SystemMemory, Add(DD1A,0x1000), 0x1000)
Field (PMCM, ByteAcc, NoLock, Preserve)
{
  Offset (0x90),
  FWSC, 32,
  Offset (0x94),
  DHPD, 32,       // DISPLAY_HPD_CTL
}

// Prepare to Sleep.  The hook is called when the OS is about to
// enter a sleep state.  The argument passed is the numeric value of
// the Sx state.

Method(_PTS,1)
{
  //clear the 3 SWC status bits
  Store(Ones, G1S3)
  Store(Ones, G1S2)
  Store(1, G1S)

  //set SWC GPE1_EN
  Store(1,G1E)

  //clear GPE0_STS
  Store(Ones, G0S)

  //
  // Call TPM PTS method
  //
  //\_SB.TPM.PTS (Arg0)

  //
  // Set GPIO_116 (GP_SSP_1_RXD) 20k pull-down for device I2S audio codec before enter S3/S4
  //
  Store (\_SB.GPC1 (NW_GPIO_116), ADW1)
  Or (ADW1, 0xFFFFC3FF, ADW1)
  And (ADW1, 0x00001000, ADW1)
  \_SB.SPC1 (NW_GPIO_116, ADW1)
}

Name (NEXP,1) // Native PCIE Setup value
Name (OSCC,0) // PCIE OSC Control

// Wake.  This hook is called when the OS is about to wake from a
// sleep state.  The argument passed is the numeric value of the
// sleep state the system is waking from.

Method(_WAK,1,Serialized)
{
  If(NEXP)
  {
    // Reinitialize the Native PCI Express after resume
    If(And(OSCC,0x02))
    {
      \_SB.PCI0.NHPG()
    }

    If(And(OSCC,0x04)) // PME control granted?
    {
      \_SB.PCI0.NPME()
    }
  }

  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4))) // If S3 or S4 Resume
  {
    // If CMP is enabled, we may need to restore the C-State and/or
    // P-State configuration, as it may have been saved before the
    // configuration was finalized based on OS/driver support.
    //
    //   CFGD[24]  = Two or more cores enabled
    //
    //If(And(CFGD,0x01000000))
    //{
      //
      // If CMP we will enable C1-SMI if
      // C-States are enabled.
      //
      //   CFGD[7:4] = C4, C3, C2, C1 Capable/Enabled
      //
      //
    //}

    //
    // Invoke SD card wake up method
    //
    \_SB.PCI0.SDC.WAK()
  }
  Return(Package(){0,0})
}

//
// System Bus
//
Scope(\_SB)
{
  Scope(PCI0)
  {
    Method(NHPG,0,Serialized)
    {
      Store(0,^RP01.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP02.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP03.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP04.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP05.HPEX) // clear the hot plug SCI enable bit
      Store(0,^RP06.HPEX) // clear the hot plug SCI enable bit
      Store(1,^RP01.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP02.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP03.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP04.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP05.HPSX) // clear the hot plug SCI status bit
      Store(1,^RP06.HPSX) // clear the hot plug SCI status bit
    }

    Method(NPME,0,Serialized)
    {
      Store(0,^RP01.PMEX) // clear the PME SCI enable bit
      Store(0,^RP02.PMEX) // clear the PME SCI enable bit
      Store(0,^RP03.PMEX) // clear the PME SCI enable bit
      Store(0,^RP04.PMEX) // clear the PME SCI enable bit
      Store(0,^RP05.PMEX) // clear the PME SCI enable bit
      Store(0,^RP06.PMEX) // clear the PME SCI enable bit
      Store(1,^RP01.PMSX) // clear the PME SCI status bit
      Store(1,^RP02.PMSX) // clear the PME SCI status bit
      Store(1,^RP03.PMSX) // clear the PME SCI status bit
      Store(1,^RP04.PMSX) // clear the PME SCI status bit
      Store(1,^RP05.PMSX) // clear the PME SCI status bit
      Store(1,^RP06.PMSX) // clear the PME SCI status bit
    }
  } // end Scope(PCI0)

  //--------------------
  //  GPIO
  //--------------------

  Device (GPO0) // North Community for DFx GPIO, SATA GPIO, PWM, LPSS/ISH UARTs, IUnit GPIO, JTAG, and SVID
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - North" )
    Name (_UID, 1)
    Name (LINK, "\\_SB.GPO0") // Support for Windows 7

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP0A, B0BA)
      Store(GP0L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      Return(0xf)
    }
    // Track status of GPIO OpRegion availability for this controller
    Name(AVBL, 0)
    Method(_REG,2) {
       If (Lequal(Arg0, 8)) {
         Store(Arg1, ^AVBL)
       }
    }

    OperationRegion(GPOP, SystemMemory, GP0A, GP0L)
    Field(\_SB.GPO0.GPOP, ByteAcc, NoLock, Preserve) {
      Offset(0x578), //PIN 15:15 * 8 + 0x500  // WiFi Reset
      CWLE, 1,
      Offset(0x5B0), //PIN 22:22 * 8 + 0x500  // SATA_ODD_PWRGT_R
      ODPW, 1
    }
  }   //  Device (GPO0)

  Device (GPO1) // Northwest Community for Display GPIO, PMC, Audio, and SPI
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - Northwest" )
    Name (_UID, 2)

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP1A, B0BA)
      Store(GP1L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      Return(0xf)
    }

  }   //  Device (GPO1)


  Device (GPO2) // West Community for LPSS/ISH I2C, ISH GPIO, iCLK, and PMU
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - West" )
    Name (_UID, 3)

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP2A, B0BA)
      Store(GP2L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      Return(0xf)
    }
  }   //  Device (GPO2)

  Device (GPO3) // Southwest Community for EMMC, SDIO, SDCARD, SMBUS, and LPC
  {
    Name (_ADR, 0)
    Name (_HID, "INT3452")
    Name (_CID, "INT3452")
    Name (_DDN, "General Purpose Input/Output (GPIO) Controller - Southwest" )
    Name (_UID, 4)

    Name (RBUF, ResourceTemplate ()
    {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00004000, BAR0)
      Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , ) {14}
    })

    Method (_CRS, 0x0, NotSerialized)
    {
      CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
      CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
      Store(GP3A, B0BA)
      Store(GP3L, B0LN)
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized)
    {
      Return(0xf)
    }

    // Track status of GPIO OpRegion availability for this controller
    Name(AVBL, 0)
    Method(_REG,2) {
      If (Lequal(Arg0, 8)) {
        Store(Arg1, ^AVBL)
      }
    }

    OperationRegion(GPOP, SystemMemory, GP3A, GP3L)
    Field(\_SB.GPO3.GPOP, ByteAcc, NoLock, Preserve) {
      Offset(0x5F0), //PIN 30: 30 * 8 + 0x500 // GPIO_183 SD_CARD_PWR_EN_N
      SDPC, 1
    }
  }   //  Device (GPO3)

  Scope (\_SB) {
    Device(IPC1)
    {
      Name (_ADR, 0)
      Name (_HID, "INT34D2")
      Name (_CID, "INT34D2")
      Name (_DDN, "Intel(R) IPCI controller ")
      Name (_UID, 1)

      Name (RBUF, ResourceTemplate ()
      {
        Memory32Fixed (ReadWrite, 0x00000000, 0x00002000, BAR0)    // IPC1 Bar, 8KB
        Memory32Fixed (ReadWrite, 0x00000000, 0x00000004, MDAT)    // PUnit mailbox Data
        Memory32Fixed (ReadWrite, 0x00000000, 0x00000004, MINF)    // PUnit mailbox Interface
        IO (Decode16, 0x400, 0x480, 0x4, 0x80)                     // ACPI IO Base address
        Memory32Fixed (ReadWrite, 0x00000000, 0x00002000, BAR1)    // SSRAM

        Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, , , ) {40}  // IPC1 IRQ
      })

      Method (_CRS, 0x0, NotSerialized)
      {
        CreateDwordField(^RBUF, ^BAR0._BAS, B0BA)
        CreateDwordField(^RBUF, ^BAR0._LEN, B0LN)
        Store(DD1A, B0BA) // D13A is the  BAR high address for B0/D13/F1
        Store(DD1L, B0LN) // D13L is the BAR length for B0/D13/F1

        CreateDwordField(^RBUF, ^MDAT._BAS, BM01)
        CreateDwordField(^RBUF, ^MDAT._LEN, BML1)
        CreateDwordField(^RBUF, ^MINF._BAS, BM02)
        CreateDwordField(^RBUF, ^MINF._LEN, BML2)
        Store(BMDA, BM01)                        // BMDA is the mail box data
        Store(4, BML1)                           // Length for BMDA is 4 bytes
        Store(BMIA, BM02)                        // BMDA is the mail box interface
        Store(4, BML2)                           // Length for BMIA is 4 bytes

        CreateDwordField(^RBUF, ^BAR1._BAS, B1BA)
        CreateDwordField(^RBUF, ^BAR1._LEN, B1LN)
        Store(DD3A, B1BA) // D13A is the  BAR high address for B0/D13/F3
        Store(DD3L, B1LN) // D13L is the BAR length for B0/D13/F3

        Return (RBUF)
      }

      Method (_STA, 0x0, NotSerialized)
      {
        If (LEqual (IPCE, 1)) {
          Return (0xF)
        }
        Return (0x00)
      }
    }
  }
}

Scope (\)
{
  //
  // Global Name, returns current Interrupt controller mode;
  // updated from _PIC control method
  //
  Name(PICM, 0)

  //
  // Procedure: GPRW
  //
  // Description: Generic Wake up Control Method ("Big brother")
  //              to detect the Max Sleep State available in ASL Name scope
  //              and Return the Package compatible with _PRW format.
  // Input: Arg0 =  bit offset within GPE register space device event will be triggered to.
  //        Arg1 =  Max Sleep state, device can resume the System from.
  //                If Arg1 = 0, Update Arg1 with Max _Sx state enabled in the System.
  // Output:  _PRW package
  //
  Name(PRWP, Package(){Zero, Zero})   // _PRW Package

  Method(GPRW, 2)
  {
    Store(Arg0, Index(PRWP, 0))             // copy GPE#
    //
    // SS1-SS4 - enabled in BIOS Setup Sleep states
    //
    Store(ShiftLeft(SS1,1),Local0)          // S1 ?
    Or(Local0,ShiftLeft(SS2,2),Local0)      // S2 ?
    Or(Local0,ShiftLeft(SS3,3),Local0)      // S3 ?
    Or(Local0,ShiftLeft(SS4,4),Local0)      // S4 ?
    //
    // Local0 has a bit mask of enabled Sx(1 based)
    // bit mask of enabled in BIOS Setup Sleep states(1 based)
    //
    If(And(ShiftLeft(1, Arg1), Local0))
    {
      //
      // Requested wake up value (Arg1) is present in Sx list of available Sleep states
      //
      Store(Arg1, Index(PRWP, 1))           // copy Sx#
    }
    Else
    {
      //
      // Not available -> match Wake up value to the higher Sx state
      //
      ShiftRight(Local0, 1, Local0)
      // If(LOr(LEqual(OSFL, 1), LEqual(OSFL, 2))) {  // ??? Win9x
      // FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Max Sx
      // } Else {           // ??? Win2k / XP
     FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Min Sx
      // }
    }

    Return(PRWP)
  }
}

Scope (\_SB)
{
  Name(OSCI, 0)  // \_SB._OSC DWORD2 input
  Name(OSCO, 0)  // \_SB._OSC DWORD2 output
  Name(OSCP, 0)  // \_SB._OSC CAPABILITIES
  // _OSC (Operating System Capabilities)
  //    _OSC under \_SB scope is used to convey platform wide OSPM capabilities.
  //    For a complete description of _OSC ACPI Control Method, refer to ACPI 5.0
  //    specification, section 6.2.10.
  // Arguments: (4)
  //    Arg0 - A Buffer containing the UUID "0811B06E-4A27-44F9-8D60-3CBBC22E7B48"
  //    Arg1 - An Integer containing the Revision ID of the buffer format
  //    Arg2 - An Integer containing a count of entries in Arg3
  //    Arg3 - A Buffer containing a list of DWORD capabilities
  // Return Value:
  //    A Buffer containing the list of capabilities
  //
  Method(_OSC,4,Serialized)
  {
    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg3, 4, CAP0)

    //
    // Check UUID
    //
    If(LEqual(Arg0,ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48")))
    {
      //
      // Check Revision
      //
      If(LEqual(Arg1,One))
      {
        Store(CAP0, OSCP)
        If(And(CAP0,0x04)) // Check _PR3 Support(BIT2)
        {
          Store(0x04, OSCO)
        }
      } Else{
        And(STS0,0xFFFFFF00,STS0)
        Or(STS0,0xA, STS0) // Unrecognised Revision and report OSC failure
      }
    } Else {
      And(STS0,0xFFFFFF00,STS0)
      Or (STS0,0x6, STS0) // Unrecognised UUID and report OSC failure
    }

    Return(Arg3)
  } // End _OSC
}

//
// Platform specific FWU trigger method
//
Method(FWUC, 2)
{
  If(LEqual(Arg0, Zero)) {
    // Read
    And(FWSC, 0x00FF0000, Local0)
    ShiftRight(Local0, 16, Local0)
  } Else {
    // Write
    And(ToInteger(Arg1), 0xFF, Local0)
    And(FWSC, 0xFF00FFFF, Local1)
    Or(ShiftLeft(Local0, 16), Local1, FWSC)
  }
  Return (Local0)
}
