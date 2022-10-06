/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/PchInfoLib.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsPcie.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsDmiRlink.h>

//
// Include Pch PCI BDF definitions
//
Include ("PchPciAcpiDefines.asi")

//
// Include GPIO macros and access library
//
Include("HsioDefine.asl")
Include("HsioLib.asl")
Include("PchGpio.asl")

#define PCH_GPIO_COM2 0x006c0000 // PID_GPIOCOM2 = 0x6C
#define PCH_GPIO_COM3 0x006b0000 // PID_GPIOCOM3 = 0x6B

//
// Trace Hub debug library
// Include it earlier so the debug function can be used as soon as possible
//
Include ("TraceHubDebug.asl")
Name(PCHH,PCH_H) // PCH H
Name(PCHN,PCH_N) // PCH N
Name(PCHL,PCH_LP) // PCH LP

//
// PME event (PCH's GPE 6Dh) is received when any PCH internal device with
// PCI Power Management capabilities on bus 0 asserts the equivalent of the PME# signal.
// Dedicated _L6D method is not needed if its only purpose is to notify the device.
// Instead to properly handle this GPE event below requirements need to be fulfilled
// - _PRW for GPE 6Dh in devices capable of generating PME (XHCI, XDCI, GbE, HDA)
// - _S0W(3) and _DSW is present within those devices
//

Scope (\_SB.PC00) {
  Name(TEMP,0)                          // scratch variable that does not cause compiler warnings as writes to Localx variables do.

  //
  // PCH reserved resource
  //
  Device(PRRE) {
    Name(_HID,EISAID("PNP0C02")) // motherboard resource
    Name(_UID,"PCHRESV")
    Name(_STA,0x3) // device present and decodes its resources, but not to be displayed in OSPM

    Method(_CRS,0,Serialized)
    {
      Name(BUF0,ResourceTemplate(){
        //
        // Reserve PCI MMCONFIG space
        //
        Memory32Fixed(ReadWrite,0x80000000,0x10000000)
        //
        // PCH RESERVED MMIO RANGE
        // Refer to PchReservedResources.h for ranges used by PCH
        //
        // 0xFC800000 to 0xFE7FFFFF
        // skip over address range that might be claimed by the GPIO, Intel Serial IO, and TraceHub
        // The GPIO COMMx and SerialIO ranges will be handled by SIRC device.
        //
        // skip 0xFC800000 - 0xFCFFFFFF for PCH Trace Hub : SW BAR space
        Memory32Fixed(ReadWrite,0xFD000000,0x006A0000) // 0xFD000000 - 0xFD69FFFF
        // PCH-LP: Skip range for GPIO_COMM4 (0xFD6A0000 - 0xFD6AFFFF)
        // PCH-H:  Skip range for GPIO_COMM3 and GPIO_COMM4 (0xFD6A0000 - 0xFD6BFFFF)
        Memory32Fixed(ReadWrite,0x0,0x0, GBR0)         // PCH-LP: GPIO_COMM2-3, PCH-H: GPIO_COMM2
        // Skip 0xFD6D0000 - 0xFD6EFFFFF for GPIO_COMM0 and GPIO_COMM1
        Memory32Fixed(ReadWrite,0xFD6F0000,0x00910000) // 0xFD6F0000 - 0xFDFFFFFF
        Memory32Fixed(ReadWrite,0xFE000000,0x00020000) // 0xFE000000 - 0xFE01FFFF
        // Skip 0xFE020000 - 0xFE035FFF for Serial IO
        // skip 0xFE100000 - 0xFE1FFFFF for PCH Trace Hub : MTB BAR space
        Memory32Fixed(ReadWrite,0xFE200000,0x00600000) // 0xFE200000 - 0xFE7FFFFF

        // BIOS decoding range. Assume it's always 16MB.
        Memory32Fixed(ReadOnly,0xFF000000,0x1000000)
        // ACPI Base
        IO(Decode16,0,0,0x1,0xFF,ABAR)
      })
      // Update ACPI IO region
      CreateWordField(BUF0,ABAR._MIN,AMIN)
      CreateWordField(BUF0,ABAR._MAX,AMAX)
      Store(PMBS, AMIN)
      Store(PMBS, AMAX)

      //Update region consumed by GPIO communities which are not reported as part of GPIO ACPI device
      CreateDWordField(BUF0,GBR0._BAS,GBAS)
      CreateDWordField(BUF0,GBR0._LEN,GLEN)

      if(LEqual(PCHS, PCHH)){
        // Cover GPIO_COM2 range
        Store( Add(SBRG,PCH_GPIO_COM2), GBAS)
        Store( 0x10000, GLEN)
      } else {
        // Cover GPIO_COM2 and GPIO_COM3 ranges
        Store( Add(SBRG,PCH_GPIO_COM3), GBAS)
        Store( 0x20000, GLEN)
      }

      Return(BUF0)
    }
  }
  Device(IOTR) {
    //
    // This device claims IO range reserved for IO traps
    // to prevent OS from reusing it for other purposes
    //
    Name(_HID,EISAID("PNP0C02"))
    Name(_UID,"IoTraps")
    Name(BUF0,ResourceTemplate(){
      Io(Decode16,0x0,0x0,0x1,0xFF,TAG0)
      Io(Decode16,0x0,0x0,0x1,0xFF,TAG1)
      Io(Decode16,0x0,0x0,0x1,0xFF,TAG2)
      Io(Decode16,0x0,0x0,0x1,0xFF,TAG3)
    })
    CreateWordField(BUF0,TAG0._MIN,AMI0)
    CreateWordField(BUF0,TAG0._MAX,AMA0)
    CreateByteField(BUF0,TAG0._LEN,LEN0)
    CreateWordField(BUF0,TAG1._MIN,AMI1)
    CreateWordField(BUF0,TAG1._MAX,AMA1)
    CreateByteField(BUF0,TAG1._LEN,LEN1)
    CreateWordField(BUF0,TAG2._MIN,AMI2)
    CreateWordField(BUF0,TAG2._MAX,AMA2)
    CreateByteField(BUF0,TAG2._LEN,LEN2)
    CreateWordField(BUF0,TAG3._MIN,AMI3)
    CreateWordField(BUF0,TAG3._MAX,AMA3)
    CreateByteField(BUF0,TAG3._LEN,LEN3)
    Method(_CRS) {
      Store(ITA0,AMI0);Store(ITA0,AMA0)
      Store(ITA1,AMI1);Store(ITA1,AMA1)
      Store(ITA2,AMI2);Store(ITA2,AMA2)
      Store(ITA3,AMI3);Store(ITA3,AMA3)
      if(LNotEqual(ITS0,1)) { Store(0, LEN0) }
      if(LNotEqual(ITS1,1)) { Store(0, LEN1) }
      if(LNotEqual(ITS2,1)) { Store(0, LEN2) }
      if(LNotEqual(ITS3,1)) { Store(0, LEN3) }
      return (BUF0)
    }
  }


  //
  // LPC Bridge - Device 31, Function 0, this is only for PCH register Memory Region declare,
  // it's better to be declared as early as possible since it's widely used in whole ACPI name space.
  // Please add any code which needs to reference any register of it after this
  //
  Device(LPCB) {
    Name(_ADR, 0x001F0000)

    Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}

    OperationRegion(LPC, PCI_Config, 0x00, 0x100)
    Field(LPC, AnyAcc, NoLock, Preserve)
    {
      Offset(0x02),
      CDID, 16,
      Offset(0x08),
      CRID,  8,
      Offset(R_LPC_CFG_IOD), // 0x80
      IOD0,  8,
      IOD1,  8,
      Offset(0xA0),
      , 9,
      PRBL,  1,
      Offset(R_LPC_CFG_BC),  // 0xDC
      ,      2,
      ESPI,  1,
    }
  }

  //
  // SMBus Controller - Device 31, Function 4
  //
  Device(SBUS) {
    Name(_ADR,0x001F0004)
    Method(_DSM,4,serialized){if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
  }
}

Scope(\)
{
  //
  // IPC Methods
  //
  // IPC Command Write (PMC IPC1 Normal Write)
  //   arg0: Command           Command to be issued to PMC IPC 1 interface
  //   arg1: CmdId             CMD_ID for provided Command
  //   arg2: CmdSize           Total size in byte to be sent via PMC IPC 1 interface
  //   arg3: WriteBuf0         1st DWORD Value to be issued to PMC IPC 1 interface
  //   arg4: WriteBuf1         2nd DWORD Value to be issued to PMC IPC 1 interface
  //   arg5: WriteBuf2         3rd DWORD Value to be issued to PMC IPC 1 interface
  //   arg6: WriteBuf3         4th DWORD Value to be issued to PMC IPC 1 interface
  //
  Method(IPCW, 7, Serialized){
    ///
    /// Program the Write Buffer 0 with the Data that needs to written to PMC
    ///
    Store(arg3,IBF0)
    Store(arg4,IBF1)
    Store(arg5,IBF2)
    Store(arg6,IBF3)

    ///
    /// Program the command register with command and size
    ///
    Store(arg0, Local0)                     // Set Command
    Add(Local0, ShiftLeft(arg1,12), Local0) // Set CMD_ID
    Add(Local0, ShiftLeft(arg2,16), Local0) // Set Command Size
    Store(Local0,CMDR)

    ///
    /// Read the IPC_STS to get BUSY or Error status
    /// Break on 10ms timeout or error
    ///
    Store(0, Local0)
    While(1) {
      If(LOr(LNot(IBSY),LEqual(IERR, 1))){
        Break
      }
      If(Lgreater(Local0, 10)) { // 10 ms timeout
        ADBG("IPC Busy TO")
        Break
      }
      Sleep(1)
      Increment(Local0)
    }
    If(LEqual(IERR,1)) {
     ADBG("IPC Error:")
     ADBG(IERC)
     ADBG(arg0)
     ADBG(arg1)
     ADBG(arg2)
     ADBG(arg3)
     ADBG(arg4)
     ADBG(arg5)
     ADBG(arg6)
    }
  }

  //
  // Message IPC to configure PCIe ClkReq Override
  //   arg0: Clock number
  //   arg1: Enable(1)/Disable(0) Clock
  //
  Method (SPCO, 2, Serialized){
    ShiftLeft(0x1, arg0, Local0)
    ShiftLeft(arg1, arg0, Local1)
    IPCW(V_PMC_PWRM_IPC_SRC_CLK_CMD, 0, 0x8, Local0, Local1, 0, 0)
  }

  //
  //  PCR Register Access Methods
  //
  // PCR Dword Read
  // arg0: PID
  // arg1: Offset
  //
  Method (PCRR, 2, Serialized) {
    Add (ShiftLeft (arg0, 16), arg1, Local0)
    Add (SBRG, Local0, Local0)
    OperationRegion (PCR0, SystemMemory, Local0, 0x4)
    Field(PCR0,DWordAcc,Lock,Preserve) {
      Offset(0x00),
      DAT0, 32
    } // End Field PCR0
    Return (DAT0)
  } // End Method PCRR

  //
  // PCR Dword Write
  // arg0: PID
  // arg1: Offset
  // arg2: write data
  //
  Method (PCRW, 3, Serialized) {
    Add (ShiftLeft (arg0, 16), arg1, Local0)
    Add (SBRG, Local0, Local0)
    OperationRegion (PCR0, SystemMemory, Local0, 0x4)
    Field(PCR0,DWordAcc,Lock,Preserve) {
      Offset(0x00),
      DAT0, 32
    } // End Field PCR0
    Store (arg2, DAT0)
  } // End Method PCRW

  //
  // PCR Dword Or
  // arg0: PID
  // arg1: Offset
  // arg2: Or data
  //
  Method (PCRO, 3, Serialized) {
    Store(PCRR(arg0,arg1),Local0)    // Store PCR Read data in Local0
    Store(Or(Local0,arg2),Local1)    // Or data
    PCRW(arg0,arg1,Local1)           // Write data back
  }

  //
  // PCR Dword And
  // arg0: PID
  // arg1: Offset
  // arg2: And data
  //
  Method (PCRA, 3, Serialized) {
    Store(PCRR(arg0,arg1),Local0)    // Store PCR Read data in Local0
    Store(And(Local0,arg2),Local1)   // And data
    PCRW(arg0,arg1,Local1)           // Write data back
  }

  //
  // PCR Dword AndThenOr
  // arg0: PID
  // arg1: Offset
  // arg2: And data
  // arg3: Or data
  //
  Method (PCAO, 4, Serialized) {
    Store(PCRR(arg0,arg1),Local0)           // Store PCR Read data in Local0
    Store(Or(And(Local0,arg2),arg3),Local1) // AndThenOr
    PCRW(arg0,arg1,Local1)                  // Write data back
  }

  Name (TCBV, 0)        // TCO I/O base address value
  Method (TCBS, 0) {
    If (LEqual(TCBV, 0)) {
      // PCR[DMI] + E78h[15:5]
      Store(PCRR (PID_DMI, R_PCH_RLINK_PCR_TCOBASE), Local0)
      And(Local0, B_PCH_RLINK_PCR_TCOBASE_TCOBA, TCBV)
    }
    Return (TCBV)
  }

  //
  // Define PCH ACPIBASE I/O as an ACPI operating region.  The base address
  // can be found in Device 31, Function 2, Offset 40h.
  //
  OperationRegion(PMIO, SystemIo, PMBS, 0x60)
  Field(PMIO, ByteAcc, NoLock, Preserve) {
          ,  8,
      PBSS,  1,       // Power Button Status
    Offset(R_ACPI_IO_GPE_CNTL),     // 0x40, General Purpose Event Control
          ,  17,
      GPEC,  1        // Software GPE Control
  }

  //
  // Define PCH TCOBASE I/O
  //
  OperationRegion(TCBA, SystemIo, TCBS, 0x10)
  Field(TCBA, ByteAcc, NoLock, Preserve) {
    Offset(R_TCO_IO_TCO1_STS), // 0x04
        , 9,
    CPSC, 1,        // CPUSCI_STS
  }

  //
  // PWRM register definitions
  //
  OperationRegion(PWMR, SystemMemory, PWRM, 0x1E30)
  Field(PWMR, DWordAcc, NoLock, Preserve) {
    //
    // IPC definitions
    //
    Offset(R_PMC_PWRM_IPC_CMD), // IPC Command (IPC_CMD)
    CMDR, 32,         // Command register. Includes: [7:0] Command, [8] MSI, [15:12] CMD_ID, [23:16] Size
    Offset(R_PMC_PWRM_IPC_STS), //  IPC Status (IPC_STS)
    IBSY, 1,          // [0]     Ready/Busy (BUSY)
    IERR, 1,          // [1]     Error (ERROR)
        , 1,          // [2]     IRQ (IRQ)
        , 1,          // [3]     Reserved
        , 4,          // [7:4]   Command ID (CMD_ID)
        , 8,          // [15:8]  Initiator ID (INIT_ID)
    IERC, 8,          // [23:16] Error Code (ERROR CODE)
        , 8,          // [31:24] Reserved
    Offset(R_PMC_PWRM_IPC_WBUF0), // IPC Write Buffer (IPC_WBUF0)
    IBF0, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_WBUF1), // IPC Write Buffer (IPC_WBUF1)
    IBF1, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_WBUF2), // IPC Write Buffer (IPC_WBUF2)
    IBF2, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_WBUF3), // IPC Write Buffer (IPC_WBUF3)
    IBF3, 32,         // [31:0] Write Buffer (WBUF)
    Offset(R_PMC_PWRM_IPC_RBUF0), // IPC Read Buffer (IPC_RBUF0)
    IRBF, 32,         // [31:0] Read Buffer (RBUF)
    Offset(R_PMC_PWRM_MODPHY_PM_CFG5),
    RAA0, 1,          // BIT0  : PCIE A, Function 0
    RAA1, 1,          // BIT1  : PCIE A, Function 1
    RAA2, 1,          // BIT2  : PCIE A, Function 2
    RAA3, 1,          // BIT3  : PCIE A, Function 3
    RPB0, 1,          // BIT4  : PCIE B, Function 0
    RPB1, 1,          // BIT5  : PCIE B, Function 1
    RPB2, 1,          // BIT6  : PCIE B, Function 2
    RPB3, 1,          // BIT7  : PCIE B, Function 3
    RPC0, 1,          // BIT8  : PCIE C, Function 0
    RPC1, 1,          // BIT9  : PCIE C, Function 1
    RPC2, 1,          // BIT10 : PCIE C, Function 2
    RPC3, 1,          // BIT11 : PCIE C, Function 3
    RSAT, 1,          // BIT12 : SATA
    RGBE, 1,          // BIT13 : GBE
    RXHC, 1,          // BIT14 : XHCI
    RXDC, 1,          // BIT15 : XDCI
    RUFS, 1,          // BIT16 : UFS
    RPD0, 1,          // BIT17 : PCIE D, Function 0
    RPD1, 1,          // BIT18 : PCIE D, Function 1
    RPD2, 1,          // BIT18 : PCIE D, Function 2
    RPD3, 1,          // BIT20 : PCIE D, Function 3
    RPE0, 1,          // BIT21 : PCIE E, Function 0
    RPE1, 1,          // BIT22 : PCIE E, Function 1
    RPE2, 1,          // BIT23 : PCIE E, Function 2
    RPE3, 1,          // BIT24 : PCIE E, Function 3
    RDMI, 1,          // BIT25 : DMI
    Offset(R_PMC_PWRM_MODPHY_PM_CFG6),
    APA0, 1,          // BIT0  : PCIE A, Function 0
    APA1, 1,          // BIT1  : PCIE A, Function 1
    APA2, 1,          // BIT2  : PCIE A, Function 2
    APA3, 1,          // BIT3  : PCIE A, Function 3
    APB0, 1,          // BIT4  : PCIE B, Function 0
    APB1, 1,          // BIT5  : PCIE B, Function 1
    APB2, 1,          // BIT6  : PCIE B, Function 2
    APB3, 1,          // BIT7  : PCIE B, Function 3
    APC0, 1,          // BIT8  : PCIE C, Function 0
    APC1, 1,          // BIT9  : PCIE C, Function 1
    APC2, 1,          // BIT10 : PCIE C, Function 2
    APC3, 1,          // BIT11 : PCIE C, Function 3
    ASAT, 1,          // BIT12 : SATA
    AGBE, 1,          // BIT13 : GBE
    AXHC, 1,          // BIT14 : XHCI
    AXDC, 1,          // BIT15 : XDCI
    AUFS, 1,          // BIT16 : UFS
    APD0, 1,          // BIT17 : PCIE D, Function 0
    APD1, 1,          // BIT18 : PCIE D, Function 1
    APD2, 1,          // BIT18 : PCIE D, Function 2
    APD3, 1,          // BIT20 : PCIE D, Function 3
    APE0, 1,          // BIT21 : PCIE E, Function 0
    APE1, 1,          // BIT22 : PCIE E, Function 1
    APE2, 1,          // BIT23 : PCIE E, Function 2
    APE3, 1,          // BIT24 : PCIE E, Function 3
    ADMI, 1,          // BIT25 : DMI
    Offset(R_PMC_PWRM_CFG4),
    PMC4, 32,
    Offset(R_PMC_PWRM_1B1C),
        , 22,
    XSQD,  1,         // 24MHz Crystal Shutdown Qualification Disable (XTALSDQDIS)
    Offset(R_PMC_PWRM_ST_PG_FDIS_PMC_1),
    GBED,  1,         // GBE_FDIS_PMC
    Offset(R_PMC_PWRM_NST_PG_FDIS_1),
        , 22,
    SCFD,  1,         // ST_FDIS_PMC (SATA)
  }
}

Scope(\_SB_.PC00) {

  //
  // PCIE Root Port #01
  //
  Device(RP01) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA1,0)) {
        Return (RPA1)
      } Else {
        Return (RP01_PCI_ADDR)
      }
    }

    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,1)
    Method(_INI)
    {
      Store (LTR1, LTRZ)
      Store (PML1, LMSL)
      Store (PNL1, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR12) }// APIC mode
      Return (PD12) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #01"

  //
  // PCIE Root Port #02
  //
  Device(RP02) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA2,0)) {
        Return (RPA2)
      } Else {
        Return (RP02_PCI_ADDR)
      }
    }

    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,2)
    Method(_INI)
    {
      Store (LTR2, LTRZ)
      Store (PML2, LMSL)
      Store (PNL2, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR13) }// APIC mode
      Return (PD13) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #02"

  //
  // PCIE Root Port #03
  //
  Device(RP03) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA3,0)) {
        Return (RPA3)
      } Else {
        Return (RP03_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,3)
    Method(_INI)
    {
      Store (LTR3, LTRZ)
      Store (PML3, LMSL)
      Store (PNL3, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR14) }// APIC mode
      Return (PD14) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #03"

  //
  // PCIE Root Port #04
  //
  Device(RP04) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA4,0)) {
        Return (RPA4)
      } Else {
        Return (RP04_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,4)
    Method(_INI)
    {
      Store (LTR4, LTRZ)
      Store (PML4, LMSL)
      Store (PNL4, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR15) }// APIC mode
      Return (PD15) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #04"

  //
  // PCIE Root Port #05
  //
  Device(RP05) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA5,0)) {
        Return (RPA5)
      } Else {
        Return (RP05_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,5)
    Method(_INI)
    {
      Store (LTR5, LTRZ)
      Store (PML5, LMSL)
      Store (PNL5, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR16) }// APIC mode
      Return (PD16) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #05"

  //
  // PCIE Root Port #06
  //
  Device(RP06) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA6,0)) {
        Return (RPA6)
      } Else {
        Return (RP06_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,6)
    Method(_INI)
    {
      Store (LTR6, LTRZ)
      Store (PML6, LMSL)
      Store (PNL6, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR17) }// APIC mode
      Return (PD17) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #06"

  //
  // PCIE Root Port #07
  //
  Device(RP07) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA7,0)) {
        Return (RPA7)
      } Else {
        Return (RP07_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,7)
    Method(_INI)
    {
      Store (LTR7, LTRZ)
      Store (PML7, LMSL)
      Store (PNL7, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR18) }// APIC mode
      Return (PD18) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #07"

  //
  // PCIE Root Port #08
  //
  Device(RP08) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA8,0)) {
        Return (RPA8)
      } Else {
        Return (RP01_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,8)
    Method(_INI)
    {
      Store (LTR8, LTRZ)
      Store (PML8, LMSL)
      Store (PNL8, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR19) }// APIC mode
      Return (PD19) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #08"

  //
  // PCIE Root Port #09
  //
  Device(RP09) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPA9,0)) {
        Return (RPA9)
      } Else {
        Return (RP09_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,9)
    Method(_INI)
    {
      Store (LTR9, LTRZ)
      Store (PML9, LMSL)
      Store (PNL9, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR1A) }// APIC mode
      Return (PD1A) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #09"

  //
  // PCIE Root Port #10
  //
  Device(RP10) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAA,0)) {
        Return (RPAA)
      } Else {
        Return (RP10_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,10)
    Method(_INI)
    {
      Store (LTRA, LTRZ)
      Store (PMLA, LMSL)
      Store (PNLA, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR1B) }// APIC mode
      Return (PD1B) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #10"

  //
  // PCIE Root Port #11
  //
  Device(RP11) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAB,0)) {
        Return (RPAB)
      } Else {
        Return (RP11_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,11)
    Method(_INI)
    {
      Store (LTRB, LTRZ)
      Store (PMLB, LMSL)
      Store (PNLB, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR1C) }// APIC mode
      Return (PD1C) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #11"

  //
  // PCIE Root Port #12
  //
  Device(RP12) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAC,0)) {
        Return (RPAC)
      } Else {
        Return (RP12_PCI_ADDR)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,12)
    Method(_INI)
    {
      Store (LTRC, LTRZ)
      Store (PMLC, LMSL)
      Store (PNLC, LNSL)
    }

    Include("PchPcie.asl")

    Method(_PRW, 0) { Return(GPRW(0x09, 4)) }  // can wakeup from S4 state
    Method(_PRT, 0) {
      If(PICM) { Return(AR1D) }// APIC mode
      Return (PD1D) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #12"


  //
  // Serial ATA Host Controller - Device 31, Function 2
  //
#define SATA_CTRL SAT1
#define SATA_PCI_ADDR SATA1_PCI_ADDR
Include ("PchSata.asl")
#undef SATA_CTRL
#undef SATA_PCI_ADDR

#define SATA_CTRL SAT2
#define SATA_PCI_ADDR SATA2_PCI_ADDR
Include ("PchSata.asl")
#undef SATA_CTRL
#undef SATA_PCI_ADDR

#define SATA_CTRL SAT3
#define SATA_PCI_ADDR SATA3_PCI_ADDR
Include ("PchSata.asl")
#undef SATA_CTRL
#undef SATA_PCI_ADDR
}
//
// xHCI Controller
//
Include ("PchXhci.asl")

//
// Serial IO
//
Include ("PchSerialIoUart.asl")

//
// Storage and Communication Subsystems definitions
//
Include ("PchScs.asl")

Include ("PchTraceHub.asl")
