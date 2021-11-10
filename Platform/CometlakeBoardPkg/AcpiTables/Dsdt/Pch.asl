/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/PchInfoLib.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsPcie.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsDmi.h>

Name (PNVB, 0xFFFF0000)  // PCH NVS Base address
Name (PNVL, 0xAA55)      // PCH NVS Length
Include ("PchNvs.asl")
Include ("PchClockControl.asl")

//
// Include GPIO macros and access library
//
Include("HsioDefine.asl")
Include("GpioAcpiDefinesCml.h")
Include("GpioLib.asl")
Include("HsioLib.asl")

//
// Trace Hub debug library
// Include it earlier so the debug function can be used as soon as possible
//
Include ("TraceHubDebug.asl")

External(\_SB.PCI0.GLAN.FGEN, MethodObj)
External(\_SB.PCI0.RP01.PPRW, MethodObj)
External(\_SB.PCI0.RP02.PPRW, MethodObj)
External(\_SB.PCI0.RP03.PPRW, MethodObj)
External(\_SB.PCI0.RP04.PPRW, MethodObj)
External(\_SB.PCI0.RP05.PPRW, MethodObj)
External(\_SB.PCI0.RP06.PPRW, MethodObj)
External(\_SB.PCI0.RP07.PPRW, MethodObj)
External(\_SB.PCI0.RP08.PPRW, MethodObj)
External(\_SB.PCI0.RP09.PPRW, MethodObj)
External(\_SB.PCI0.RP10.PPRW, MethodObj)
External(\_SB.PCI0.RP11.PPRW, MethodObj)
External(\_SB.PCI0.RP12.PPRW, MethodObj)
External(\_SB.PCI0.RP13.PPRW, MethodObj)
External(\_SB.PCI0.RP14.PPRW, MethodObj)
External(\_SB.PCI0.RP15.PPRW, MethodObj)
External(\_SB.PCI0.RP16.PPRW, MethodObj)
External(\_SB.PCI0.RP17.PPRW, MethodObj)
External(\_SB.PCI0.RP18.PPRW, MethodObj)
External(\_SB.PCI0.RP19.PPRW, MethodObj)
External(\_SB.PCI0.RP20.PPRW, MethodObj)
External(\_SB.PCI0.RP21.PPRW, MethodObj)
External(\_SB.PCI0.RP22.PPRW, MethodObj)
External(\_SB.PCI0.RP23.PPRW, MethodObj)
External(\_SB.PCI0.RP24.PPRW, MethodObj)

Name(PCHH,PCH_H) // PCH H
Name(PCHL,PCH_LP) // PCH LP

//
// PME event (PCH's GPE 6Dh) is received when any PCH internal device with
// PCI Power Management capabilities on bus 0 asserts the equivalent of the PME# signal.
// Dedicated _L6D method is not needed if its only purpose is to notify the device.
// Instead to properly handle this GPE event below requirements need to be fulfilled
// - _PRW for GPE 6Dh in devices capable of generating PME (XHCI, XDCI, GbE, HDA)
// - _S0W(3) and _DSW is present within those devices
//
Scope (\_SB.PCI0) {
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

#ifdef CPU_CFL
      Name(BUF1,ResourceTemplate(){
        Memory32Fixed(ReadWrite, 0x00000000, 0x00400000, DFSM) // DSP FW Stolen Memory
      })

      CreateDWordField(BUF1,DFSM._BAS,HDBA)
      Store(DSPM, HDBA)

      If(LEqual(PCHS, PCHL) && LLess(PSTP, PCH_B0)) { // Apply until CNL-LP B0 stepping (B0 RevID = 0x10)
        Return(ConcatenateResTemplate(BUF0, BUF1))
      }
#endif

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
    Method(_CRS) {
      Store(ResourceTemplate() { }, Local0)
      Name(BUF0,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG0) })
      Name(BUF1,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG1) })
      Name(BUF2,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG2) })
      Name(BUF3,ResourceTemplate(){ Io(Decode16,0x0,0x0,0x1,0xFF,TAG3) })
      CreateWordField(BUF0,TAG0._MIN,AMI0);CreateWordField(BUF0,TAG0._MAX,AMA0)
      CreateWordField(BUF1,TAG1._MIN,AMI1);CreateWordField(BUF1,TAG1._MAX,AMA1)
      CreateWordField(BUF2,TAG2._MIN,AMI2);CreateWordField(BUF2,TAG2._MAX,AMA2)
      CreateWordField(BUF3,TAG3._MIN,AMI3);CreateWordField(BUF3,TAG3._MAX,AMA3)
      Store(ITA0,AMI0);Store(ITA0,AMA0)
      Store(ITA1,AMI1);Store(ITA1,AMA1)
      Store(ITA2,AMI2);Store(ITA2,AMA2)
      Store(ITA3,AMI3);Store(ITA3,AMA3)
      // Win7 can't store result of ConcatenateResTemplate directly into one of its input parameters
      if(LEqual(ITS0,1)) { ConcatenateResTemplate(Local0, BUF0, Local1); Store(Local1, Local0) }
      if(LEqual(ITS1,1)) { ConcatenateResTemplate(Local0, BUF1, Local1); Store(Local1, Local0) }
      if(LEqual(ITS2,1)) { ConcatenateResTemplate(Local0, BUF2, Local1); Store(Local1, Local0) }
      if(LEqual(ITS3,1)) { ConcatenateResTemplate(Local0, BUF3, Local1); Store(Local1, Local0) }
      return (Local0)
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
      // PCR[DMI] + 2778h[15:5]
      Store(PCRR (PID_DMI, R_PCH_DMI_PCR_TCOBASE),Local0)
      And(Local0,B_PCH_DMI_PCR_TCOBASE_TCOBA,TCBV)
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
    Offset(R_PMC_PWRM_WADT_AC),
    //
    // AdvancedFeaturesBegin
    //
    ACWA, 32,         // Wake Alarm Device Timer: AC
    DCWA, 32,         // Wake Alarm Device Timer: DC
    ACET, 32,         // Wake Alarm Device Expired Timer: AC
    DCET, 32,         // Wake Alarm Device Expired Timer: DC
    //
    // AdvancedFeaturesEnd
    //
    Offset(R_PMC_PWRM_CFG4),
    PMC4, 31,
    CECE, 1,          // BIT32 : PS_ON# de-assertion during SleepS0 enable
    Offset(R_PMC_PWRM_1B1C),
        , 22,
    XSQD,  1,         // 24MHz Crystal Shutdown Qualification Disable (XTALSDQDIS)
    Offset(R_PMC_PWRM_ST_PG_FDIS_PMC_1),
    GBED,  1,         // GBE_FDIS_PMC
    Offset(R_PMC_PWRM_NST_PG_FDIS_1),
        , 22,
    SCFD,  1,         // ST_FDIS_PMC (SATA)
  }

  //
  // PS_ON control. Enable/Disable PS_ON# de-assertion during SleepS0.
  // Arg0 - 0: Disable PS_ON# de-assertion, 1: Enable PS_ON# de-assertion
  //
  Method(PSOC, 1, Serialized) {
    If (PSON) {
      If(Arg0) {
        ADBG("PS_ON Enable")
        Store(1, CECE)
      } Else {
        ADBG("PS_ON Disable")
        Store(0, CECE)
      }
    }
  }

  //
  // PS_ON status.
  //
  // @retval 0 PS_ON de-assertion during SleepS0 disabled
  // @retval 1 PS_ON de-assertion during SleepS0 enabled
  //
  Method(PSOS) {
    If(CECE) {
      Return(1)
    } Else {
      Return(0)
    }
  }
}

If (LNotEqual(GBES,0)) {
  //
  // LAN Controller
  //
  Scope (\_SB.PCI0) {
    Device(GLAN) { // GbE Controller
      Name(_ADR, 0x001F0006)

      Name(_S0W, 3) // Device can wake itself from D3 in S0

      Method(_PRW, 0) {
        Return(GPRW(0x6D, 4)) // can wakeup from S4 state
      }

      Method(_DSW, 3) {}

    } // end "GbE Controller"
  } //scope
}

//
// xHCI Controller - Device 20, Function 0
//
Include("PchXhci.asl")

//
// xDCI (OTG) Controller - Device 20, Function 1
//
include("PchXdci.asl")

Scope(\_SB_.PCI0) {

  //
  // High Definition Audio Controller - Device 31, Function 3
  //
  include("PchHda.asl")

  //
  // High Definition Audio - SoundWire Controller
  //
  include("PchHdaSoundWireCtrl.asl")

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
        Return (0x001C0000)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,1)
    Method(_INI) {
      If(PRES()) {
        Store (LTR1, LTRZ)
        Store (PML1, LMSL)
        Store (PNL1, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP01.PPRW)) {
        Return (\_SB.PCI0.RP01.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
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
        Return (0x001C0001)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,2)
    Method(_INI) {
      If(PRES()) {
        Store (LTR2, LTRZ)
        Store (PML2, LMSL)
        Store (PNL2, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP02.PPRW)) {
        Return (\_SB.PCI0.RP02.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
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
        Return (0x001C0002)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,3)
    Method(_INI) {
      If(PRES()) {
        Store (LTR3, LTRZ)
        Store (PML3, LMSL)
        Store (PNL3, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP03.PPRW)) {
        Return (\_SB.PCI0.RP03.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
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
        Return (0x001C0003)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,4)
    Method(_INI) {
      If(PRES()) {
        Store (LTR4, LTRZ)
        Store (PML4, LMSL)
        Store (PNL4, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP04.PPRW)) {
        Return (\_SB.PCI0.RP04.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
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
        Return (0x001C0004)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,5)
    Method(_INI) {
      If(PRES()) {
        Store (LTR5, LTRZ)
        Store (PML5, LMSL)
        Store (PNL5, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP05.PPRW)) {
        Return (\_SB.PCI0.RP05.PPRW())
      }
      Return (GPRW(0x69, 4))
    }

    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
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
        Return (0x001C0005)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,6)
    Method(_INI) {
      If(PRES()) {
        Store (LTR6, LTRZ)
        Store (PML6, LMSL)
        Store (PNL6, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP06.PPRW)) {
        Return (\_SB.PCI0.RP06.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
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
        Return (0x001C0006)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,7)
    Method(_INI) {
      If(PRES()) {
        Store (LTR7, LTRZ)
        Store (PML7, LMSL)
        Store (PNL7, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP07.PPRW)) {
        Return (\_SB.PCI0.RP07.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
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
        Return (0x001C0007)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,8)
    Method(_INI) {
      If(PRES()) {
        Store (LTR8, LTRZ)
        Store (PML8, LMSL)
        Store (PNL8, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP08.PPRW)) {
        Return (\_SB.PCI0.RP08.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
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
        Return (0x001D0000)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,9)
    Method(_INI) {
      If(PRES()) {
        Store (LTR9, LTRZ)
        Store (PML9, LMSL)
        Store (PNL9, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP09.PPRW)) {
        Return (\_SB.PCI0.RP09.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
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
        Return (0x001D0001)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,10)
    Method(_INI) {
      If(PRES()) {
        Store (LTRA, LTRZ)
        Store (PMLA, LMSL)
        Store (PNLA, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP10.PPRW)) {
        Return (\_SB.PCI0.RP10.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
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
        Return (0x001D0002)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,11)
    Method(_INI) {
      If(PRES()) {
        Store (LTRB, LTRZ)
        Store (PMLB, LMSL)
        Store (PNLB, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP11.PPRW)) {
        Return (\_SB.PCI0.RP11.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
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
        Return (0x001D0003)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,12)
    Method(_INI) {
      If(PRES()) {
        Store (LTRC, LTRZ)
        Store (PMLC, LMSL)
        Store (PNLC, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP12.PPRW)) {
        Return (\_SB.PCI0.RP12.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #12"

  //
  // PCIE Root Port #13
  //
  Device(RP13) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAD,0)) {
        Return (RPAD)
      } Else {
        Return (0x001D0004)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,13)
    Method(_INI) {
      If(PRES()) {
        Store (LTRD, LTRZ)
        Store (PMLD, LMSL)
        Store (PNLD, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP13.PPRW)) {
        Return (\_SB.PCI0.RP13.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #13"

  //
  // PCIE Root Port #14
  //
  Device(RP14) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAE,0)) {
        Return (RPAE)
      } Else {
        Return (0x001D0005)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,14)
    Method(_INI) {
      If(PRES()) {
        Store (LTRE, LTRZ)
        Store (PMLE, LMSL)
        Store (PNLE, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP14.PPRW)) {
        Return (\_SB.PCI0.RP14.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #14"

  //
  // PCIE Root Port #15
  //
  Device(RP15) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAF,0)) {
        Return (RPAF)
      } Else {
        Return (0x001D0006)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,15)
    Method(_INI) {
      If(PRES()) {
        Store (LTRF, LTRZ)
        Store (PMLF, LMSL)
        Store (PNLF, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP15.PPRW)) {
        Return (\_SB.PCI0.RP15.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #15"

  //
  // PCIE Root Port #16
  //
  Device(RP16) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAG,0)) {
        Return (RPAG)
      } Else {
        Return (0x001D0007)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,16)
    Method(_INI) {
      If(PRES()) {
        Store (LTRG, LTRZ)
        Store (PMLG, LMSL)
        Store (PNLG, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP16.PPRW)) {
        Return (\_SB.PCI0.RP16.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #16"

  //
  // PCIE Root Port #17
  //
  Device(RP17) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAH,0)) {
        Return (RPAH)
      } Else {
        Return (0x001B0000)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,17)
    Method(_INI) {
      If(PRES()) {
        Store (LTRH, LTRZ)
        Store (PMLH, LMSL)
        Store (PNLH, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP17.PPRW)) {
        Return (\_SB.PCI0.RP17.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #17"

  //
  // PCIE Root Port #18
  //
  Device(RP18) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAI,0)) {
        Return (RPAI)
      } Else {
        Return (0x001B0001)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,18)
    Method(_INI) {
      If(PRES()) {
        Store (LTRI, LTRZ)
        Store (PMLI, LMSL)
        Store (PNLI, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP18.PPRW)) {
        Return (\_SB.PCI0.RP18.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #18"

  //
  // PCIE Root Port #19
  //
  Device(RP19) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAJ,0)) {
        Return (RPAJ)
      } Else {
        Return (0x001B0002)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,19)
    Method(_INI) {
      If(PRES()) {
        Store (LTRJ, LTRZ)
        Store (PMLJ, LMSL)
        Store (PNLJ, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP19.PPRW)) {
        Return (\_SB.PCI0.RP19.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #19"

  //
  // PCIE Root Port #20
  //
  Device(RP20) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAK,0)) {
        Return (RPAK)
      } Else {
        Return (0x001B0003)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,20)
    Method(_INI) {
      If(PRES()) {
        Store (LTRK, LTRZ)
        Store (PMLK, LMSL)
        Store (PNLK, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP20.PPRW)) {
        Return (\_SB.PCI0.RP20.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #20"

  //
  // PCIE Root Port #21
  //
  Device(RP21) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAL,0)) {
        Return (RPAL)
      } Else {
        Return (0x001B0004)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,21)
    Method(_INI) {
      If(PRES()) {
        Store (LTRL, LTRZ)
        Store (PMLL, LMSL)
        Store (PNLL, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP21.PPRW)) {
        Return (\_SB.PCI0.RP21.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR04)
        }// APIC mode
      }
      Return (PD04) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #21"

  //
  // PCIE Root Port #22
  //
  Device(RP22) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAM,0)) {
        Return (RPAM)
      } Else {
        Return (0x001B0005)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,22)
    Method(_INI) {
      If(PRES()) {
        Store (LTRM, LTRZ)
        Store (PMLM, LMSL)
        Store (PNLM, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP22.PPRW)) {
        Return (\_SB.PCI0.RP22.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR05)
        }// APIC mode
      }
      Return (PD05) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #22"

  //
  // PCIE Root Port #23
  //
  Device(RP23) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAN,0)) {
        Return (RPAN)
      } Else {
        Return (0x001B0006)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,23)
    Method(_INI) {
      If(PRES()) {
        Store (LTRN, LTRZ)
        Store (PMLN, LMSL)
        Store (PNLN, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP23.PPRW)) {
        Return (\_SB.PCI0.RP23.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR06)
        }// APIC mode
      }
      Return (PD06) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #23"

  //
  // PCIE Root Port #24
  //
  Device(RP24) {
    Name(LTRZ, 0)
    Name(LMSL, 0)
    Name(LNSL, 0)
    Method (_ADR, 0) {
      If (LNotEqual(RPAO,0)) {
        Return (RPAO)
      } Else {
        Return (0x001B0007)
      }
    }
    //
    // Pass LTRx to LTRZ so PchPcie.asl can be reused for PCIes.
    //
    Name(SLOT,24)
    Method(_INI) {
      If(PRES()) {
        Store (LTRO, LTRZ)
        Store (PMLO, LMSL)
        Store (PNLO, LNSL)
        If(CondRefOf(PINI)) {PINI()}
      }
    }
    Include("PchPcie.asl")
    Method(_PRW, 0) {
      If(CondRefOf(\_SB.PCI0.RP24.PPRW)) {
        Return (\_SB.PCI0.RP24.PPRW())
      }
      Return(GPRW(0x69, 4)) // can wakeup from S4 state
    }
    Method(_PRT, 0) {
      If(CondRefOf(PICM)) {
        If(PICM) {
          Return(AR07)
        }// APIC mode
      }
      Return (PD07) // PIC Mode
    } // end _PRT
  } // end "PCIE Root Port #24"

  //
  // Serial ATA Host Controller - Device 31, Function 2
  //
  Include ("PchSata.asl")
}

//
//  GPIO ACPI device
//
Include ("Gpio.asl")

//
//  Serial IO Controllers definitions
//
Include ("PchSerialIo.asl")

//
// Storage and Communication Subsystems definitions
//
Include ("PchScs.asl")

//
// MEI 1 definition
//
Include ("PchHeci.asl")

//
// PCH Trace Hub, report ACPI deivce for host debugger mode.
//
If (LEqual(PTHM, 2)) {
  Include ("PchTraceHub.asl")
}
