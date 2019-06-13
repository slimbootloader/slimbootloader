/**@file

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Register/PchRegsSerialIo.h"

#define SERIAL_IO_DISABLED 0
#define SERIAL_IO_PCI      1
#define SERIAL_IO_ACPI     2
#define SERIAL_IO_HIDDEN   3

//
// Include PchSsrialIo ACPI macros
//
// PCH_CNL - Default
Include("PchSerialIoAcpiDefinesCnl.h")

Scope(\_SB.PCI0) {
  //
  //  Common code for D3 entry for all SerialIo devices including PCI mode
  //
  //  @param[in]  Arg0      PCI calculated address or ACPI BAR1 address of device to be put in D3
  //  @param[in]  Arg1      Device mode
  //
  Method (LPD3, 2, Serialized) {
    OperationRegion(ICB1, SystemMemory, Arg0, Add (R_SERIAL_IO_CFG_PME_CTRL_STS, 4))
    Field(ICB1, AnyAcc, NoLock, Preserve) {
      Offset(R_SERIAL_IO_CFG_BAR0_LOW),
      BAR0, 64
    }
    Field(ICB1, ByteAcc, NoLock, Preserve) {
      Offset(R_SERIAL_IO_CFG_PME_CTRL_STS),
      PMEC, 8 //Note: SerialIO Does Not Support PME
    }
    Store(0x3, PMEC) // Set BIT[1:0] = 11b - Power State D3
    Store(PMEC,TEMP) // perform a read to avoid ordering and noncoherency problems
    If (LEqual (Arg1, SERIAL_IO_PCI)) {
      Store(0, BAR0)
    }
  }

  //
  //  Common code for D0 entry for all SerialIo devices including PCI mode
  //
  //  @param[in]  Arg0      PCI calculated address or ACPI BAR1 address of device to be put in D0
  //
  Method (LPD0, 1, Serialized) {
    OperationRegion(ICB1, SystemMemory, Add(Arg0,R_SERIAL_IO_CFG_PME_CTRL_STS), 4)
    Field(ICB1, DWordAcc, NoLock, Preserve) { PMEC, 32 } //Note: SerialIO Does Not Support PME
    And(PMEC, 0xFFFF7FFC, PMEC) // Set BIT[1:0] = 00b - Power State D0
    Store(PMEC,TEMP) // perform a read to avoid ordering and noncoherency problems
  }

  //
  //  Common code for retrieving hardware revision for all SerialIo devices
  //
  //  @param[in]  Arg0      BAR1 of device
  //
  //  @retval               Hardware Revison
  //
  Method (LHRV, 1, Serialized) {
    OperationRegion(ICB1, SystemMemory, Add(Arg0,0x8) , 4)
    Field(ICB1, DWordAcc, NoLock, Preserve) { HRV, 8 }
    Return(HRV)
  }

  //
  //  Get D state of SerialIo device - for use in _PSC method
  //
  //  @param[in]  Arg0      PCI calculated address or ACPI BAR1 of device
  //
  //  @retval               D State
  //
  Method (GETD, 1, Serialized) {
    OperationRegion(ICB1, SystemMemory, Add(Arg0,R_SERIAL_IO_CFG_PME_CTRL_STS) , 4)
    Field(ICB1, DWordAcc, NoLock, Preserve) { PMEC, 32 }
    Return (And(PMEC, 0x3))
  }

  //
  //  Common code for reporting memory and interrupt resources for all SerialIo devices
  //
  //  @param[in]  Arg0      Device mode
  //  @param[in]  Arg1      Memory address
  //  @param[in]  Arg2      Interrupt number
  //
  //  @retval               Resource buffer
  //
  Method (LCRS, 3, Serialized) {
    Name (RBUF, ResourceTemplate () {
      Memory32Fixed (ReadWrite, 0x00000000, 0x00001000, BAR0)
      Interrupt(ResourceConsumer, Level, ActiveLow, Shared, , , SIRQ) {20}
    })
    CreateDWordField(RBUF,BAR0._BAS,BVAL)
    CreateDWordField(RBUF,BAR0._LEN,BLEN)
    CreateDWordField(RBUF,SIRQ._INT,IRQN)
    Store (Arg1, BVAL)
    Store (Arg2, IRQN)
    If(LEqual(Arg0,SERIAL_IO_HIDDEN)) {Store(8, BLEN)}
    Return (RBUF)
  }

  //
  //  Common code for reporting status of all SerialIo devices
  //
  //  @param[in]  Arg0      Device's mode
  //
  //  @retval               0 (do not show device) or 0xF (device is present)
  //
  Method (LSTA, 1, Serialized) {
    If(LOr(LEqual(Arg0,SERIAL_IO_DISABLED),LEqual(Arg0,SERIAL_IO_HIDDEN)))  { Return(0) }
    If(CondRefOf(OSYS)) {
      If(LLess(OSYS,2012)) { Return(0) } // OS older than Win8 = not supported
    }
    Return (0xF)
  }
} // END Scope(\_SB.PCI0)



Scope(\_SB.PCI0) {
  //
  //  An invisible device that claims memory resources reserved for SerialIo devices
  //  SerialIo devices in ACPI mode can't claim their BAR1 resources because these are aliases for PciCfg space
  //
  Device(SIRC) {
    Name(_HID,EISAID("PNP0C02"))
    Name(_STA,0x3) // device present and decodes its resources, but not to be displayed in OSPM
    Name(_UID,5)

    //
    //  Designated only for SPI and I2C
    //  Returns resource buffer with memory ranges used but not explicitely claimed by the device:
    //  SERIAL_IO_DISABLED:    SMDn=0 Returns empty buffer.
    //  SERIAL_IO_PCI:         SMDn=1 Returns empty buffer.
    //  SERIAL_IO_ACPI:        SMDn=2 returns BAR1 (Pci Config Space).
    //- SERIAL_IO_HIDDEN:      SMDn=3 returns both BARs, except first 8 bytes from BAR0.
    //
    //  @param[in]  Arg0      SMDn SERIAL_IO_DEVICE_MODE
    //  @param[in]  Arg1      SB0n SERIAL_IO_DEVICE_BAR0
    //  @param[in]  Arg2      SB1n SERIAL_IO_DEVICE_BAR1
    //
    //  @retval               Resource buffer with memory ranges
    //                        if SMDn=0 (SERIAL_IO_DISABLED) or SMDn=1 (SERIAL_IO_PCI) returns an empty buffer
    //
    Method (ADDB, 3, Serialized) {

      Name(BUFF,ResourceTemplate() { Memory32Fixed (ReadWrite, 0, 0x1000, BUF) })
      CreateDWordField(BUFF, BUF._BAS, ADDR)
      CreateDWordField(BUFF, BUF._LEN, LENG)

      Store(ResourceTemplate() { }, Local0)  // an empty buffer

      If(LOr(LEqual(Arg0, SERIAL_IO_ACPI), LEqual(Arg0, SERIAL_IO_HIDDEN))) {
        Store(Arg2, ADDR)
        ConcatenateResTemplate(Local0, BUFF, Local1) // add BAR1
        Store(Local1, Local0)
      }

      If(LEqual(Arg0, SERIAL_IO_HIDDEN)) {
        Store(Add(8, Arg1), ADDR)
        Store(0xFF8, LENG)
        ConcatenateResTemplate(Local0, BUFF, Local1) // add BAR0 without 8 bytes
        Store(Local1, Local0)
      }
      Return (Local0)
    }

    Method (_CRS, 0x0, Serialized) {
      Store(ResourceTemplate() { }, Local0)  // placeholder for concatenated buffers
      //
      // SB1[X] if device is configured as ACPI represents BAR1
      //        in case of PCI it stands for memory mapped BDF
      //
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_I2C0_DEVICE_MODE,SERIAL_IO_I2C0_BAR0,SERIAL_IO_I2C0_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_I2C1_DEVICE_MODE,SERIAL_IO_I2C1_BAR0,SERIAL_IO_I2C1_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_I2C2_DEVICE_MODE,SERIAL_IO_I2C2_BAR0,SERIAL_IO_I2C2_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_I2C3_DEVICE_MODE,SERIAL_IO_I2C3_BAR0,SERIAL_IO_I2C3_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_I2C4_DEVICE_MODE,SERIAL_IO_I2C4_BAR0,SERIAL_IO_I2C4_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_I2C5_DEVICE_MODE,SERIAL_IO_I2C5_BAR0,SERIAL_IO_I2C5_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_SPI0_DEVICE_MODE,SERIAL_IO_SPI0_BAR0,SERIAL_IO_SPI0_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_SPI1_DEVICE_MODE,SERIAL_IO_SPI1_BAR0,SERIAL_IO_SPI1_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_SPI2_DEVICE_MODE,SERIAL_IO_SPI2_BAR0,SERIAL_IO_SPI2_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_UART0_DEVICE_MODE,SERIAL_IO_UART0_BAR0,SERIAL_IO_UART0_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_UART1_DEVICE_MODE,SERIAL_IO_UART1_BAR0,SERIAL_IO_UART1_BAR1), Local1)
      Store(Local1, Local0)
      ConcatenateResTemplate(Local0, ADDB(SERIAL_IO_UART2_DEVICE_MODE,SERIAL_IO_UART2_BAR0,SERIAL_IO_UART2_BAR1), Local1)
      Store(Local1, Local0)
      //
      //  GPIO device always exists in chipset. If it's not visible in ACPI
      //  and doesn't claim its resources, declare it here to prevent OS from reusing its mem addresses
      //
      If(LEqual(\_SB.PCI0.GPI0._STA(),0)) {
        ConcatenateResTemplate(Local0, \_SB.PCI0.GPI0._CRS(), Local1)
        Store(Local1, Local0)
      }
      Return (Local0)
    } // END _CRS
  } // END Device(SIRC)
} // END Scope(\_SB.PCI0)

//------------------------------------------
//  Serial IO I2C Controller 0 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (I2C0) {
    //
    // Applies for I2C0 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_I2C0_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_I2C0_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_I2C0_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_I2C0_DEVICE_MODE,SERIAL_IO_I2C0_BAR0,SERIAL_IO_I2C0_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_I2C0_DEVICE_MODE)) }
    }
    //
    // Applies for I2C0 PCI mode only
    //
    If(LEqual(SERIAL_IO_I2C0_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_I2C0_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // I2C0 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_I2C0_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_I2C0_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_I2C0_BAR1, SERIAL_IO_I2C0_DEVICE_MODE) }
  }
}
//------------------------------------------
//  Serial IO I2C Controller 1 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (I2C1) {
    //
    // Applies for I2C1 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_I2C1_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_I2C1_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_I2C1_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_I2C1_DEVICE_MODE,SERIAL_IO_I2C1_BAR0,SERIAL_IO_I2C1_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_I2C1_DEVICE_MODE)) }
    }
    //
    // Applies for I2C1 PCI mode only
    //
    If(LEqual(SERIAL_IO_I2C1_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_I2C1_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // I2C1 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_I2C1_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_I2C1_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_I2C1_BAR1, SERIAL_IO_I2C1_DEVICE_MODE) }
  }
}
//------------------------------------------
//  Serial IO I2C Controller 2 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (I2C2) {
    //
    // Applies for I2C2 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_I2C2_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_I2C2_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_I2C2_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_I2C2_DEVICE_MODE,SERIAL_IO_I2C2_BAR0,SERIAL_IO_I2C2_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_I2C2_DEVICE_MODE)) }
    }
    //
    // Applies for I2C2 PCI mode only
    //
    If(LEqual(SERIAL_IO_I2C2_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_I2C2_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // I2C2 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_I2C2_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_I2C2_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_I2C2_BAR1, SERIAL_IO_I2C2_DEVICE_MODE) }
  }
}
//------------------------------------------
//  Serial IO I2C Controller 3 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (I2C3) {
    //
    // Applies for I2C3 ACPI, Hidden or Disabled mode
    //
    if(LNotEqual(SERIAL_IO_I2C3_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_I2C3_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_I2C3_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_I2C3_DEVICE_MODE,SERIAL_IO_I2C3_BAR0,SERIAL_IO_I2C3_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_I2C3_DEVICE_MODE)) }
    }
    //
    // Applies for I2C3 PCI mode only
    //
    if(LEqual(SERIAL_IO_I2C3_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_I2C3_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // I2C3 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_I2C3_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_I2C3_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_I2C3_BAR1, SERIAL_IO_I2C3_DEVICE_MODE) }
  }
}
//------------------------------------------
//  Serial IO I2C Controller 4 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (I2C4) {
    //
    // Applies for I2C4 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_I2C4_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_I2C4_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_I2C4_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_I2C4_DEVICE_MODE,SERIAL_IO_I2C4_BAR0,SERIAL_IO_I2C4_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_I2C4_DEVICE_MODE)) }
    }
    //
    // Applies for I2C4 PCI mode only
    //
    If(LEqual(SERIAL_IO_I2C4_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_I2C4_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // I2C4 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_I2C4_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_I2C4_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_I2C4_BAR1, SERIAL_IO_I2C4_DEVICE_MODE) }
  }
}
//------------------------------------------
//  Serial IO I2C Controller 5 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (I2C5) {
    //
    // Applies for I2C5 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_I2C5_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_I2C5_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_I2C5_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_I2C5_DEVICE_MODE,SERIAL_IO_I2C5_BAR0,SERIAL_IO_I2C5_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_I2C5_DEVICE_MODE)) }
    }
    //
    // Applies for I2C5 PCI mode only
    //
    If(LEqual(SERIAL_IO_I2C5_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_I2C5_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // I2C5 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_I2C5_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_I2C5_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_I2C5_BAR1, SERIAL_IO_I2C5_DEVICE_MODE) }
  }
}
//-------------------------------------------
//  Serial IO SPI Controller 0 Configuration
//-------------------------------------------
Scope(\_SB.PCI0) {
  Device (SPI0) {
    //
    // Applies for SPI0 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_SPI0_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_SPI0_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_SPI0_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_SPI0_DEVICE_MODE,SERIAL_IO_SPI0_BAR0,SERIAL_IO_SPI0_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_SPI0_DEVICE_MODE)) }
    }
    //
    // Applies for SPI0 PCI mode only
    //
    If(LEqual(SERIAL_IO_SPI0_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_SPI0_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // SPI0 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_SPI0_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_SPI0_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_SPI0_BAR1, SERIAL_IO_SPI0_DEVICE_MODE) }
  }
}
//------------------------------------------
//  Serial IO SPI Controller 1 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (SPI1) {
    //
    // Applies for SPI1 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_SPI1_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_SPI1_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_SPI1_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_SPI1_DEVICE_MODE,SERIAL_IO_SPI1_BAR0,SERIAL_IO_SPI1_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_SPI1_DEVICE_MODE)) }
    }
    //
    // Applies for SPI1 PCI mode only
    //
    If(LEqual(SERIAL_IO_SPI1_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_SPI1_ADR)
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
    }
    //
    // SPI1 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_SPI1_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_SPI1_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_SPI1_BAR1, SERIAL_IO_SPI1_DEVICE_MODE) }
  }
}
//------------------------------------------
//  Serial IO SPI Controller 2 Configuration
//------------------------------------------
Scope(\_SB.PCI0) {
  Device (SPI2) {
    //
    // Applies for SPI2 ACPI, Hidden or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_SPI2_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_HID, SERIAL_IO_SPI2_HID)
      Method (_HRV) { Return (LHRV(SERIAL_IO_SPI2_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_SPI2_DEVICE_MODE,SERIAL_IO_SPI2_BAR0,SERIAL_IO_SPI2_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_SPI2_DEVICE_MODE)) }
    }
    //
    // Applies for SPI2 PCI mode only
    //
    If(LEqual(SERIAL_IO_SPI2_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_SPI2_ADR)
      Method (_DSM,4,Serialized){if(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer(){0})}
    }
    //
    // SPI2 PCI/ACPI PS0 PS3 and PSC
    //
    Method (_PSC) { Return (GETD(SERIAL_IO_SPI2_BAR1)) }
    Method (_PS0) { LPD0(SERIAL_IO_SPI2_BAR1) }
    Method (_PS3) { LPD3(SERIAL_IO_SPI2_BAR1, SERIAL_IO_SPI2_DEVICE_MODE) }
  }
}
//-------------------------------------------
//  Serial IO UART Controller 0 Configuration
//-------------------------------------------
Scope(\_SB.PCI0) {
  Device (UA00) {
    //
    // Applies for UART0 Hidden and Disabled mode
    //
    If(LNotEqual(SERIAL_IO_UART0_DEVICE_MODE,SERIAL_IO_PCI)) {
      Method(_HID) {
        If(LEqual(SERIAL_IO_UART0_DEVICE_MODE,SERIAL_IO_HIDDEN)) { Return (EISAID("PNP0C02")) }
        Else { Return (SERIAL_IO_UART0_HID) }
      }
      Name (_UID, "SerialIoUart0")
      Name (_DDN, "SerialIoUart0")
      Method (_HRV) { Return (LHRV(SERIAL_IO_UART0_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_UART0_DEVICE_MODE,SERIAL_IO_UART0_BAR0,SERIAL_IO_UART0_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_UART0_DEVICE_MODE)) }
    }
    //
    // Applies for UART0 PCI mode only
    //
    If(LEqual(SERIAL_IO_UART0_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_UART0_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // UART0 PCI/ACPI PS0 PS3 and PSC excluding Hidden mode
    //
    If(LNotEqual(SERIAL_IO_UART0_DEVICE_MODE,SERIAL_IO_HIDDEN)) {
      Method (_PSC) { Return (GETD(SERIAL_IO_UART0_BAR1)) }
      Method (_PS0) { LPD0(SERIAL_IO_UART0_BAR1) }
      Method (_PS3) { LPD3(SERIAL_IO_UART0_BAR1, SERIAL_IO_UART0_DEVICE_MODE) }
    }
  }
}
//-------------------------------------------
//  Serial IO UART Controller 1 Configuration
//-------------------------------------------
Scope(\_SB.PCI0) {
  Device (UA01) {
    //
    // Applies for UART1 Hidden and Disabled mode
    //
    If(LNotEqual(SERIAL_IO_UART1_DEVICE_MODE,SERIAL_IO_PCI)) {
      Method(_HID) {
        If(LEqual(SERIAL_IO_UART1_DEVICE_MODE,SERIAL_IO_HIDDEN)) { Return (EISAID("PNP0C02")) }
        Else { Return (SERIAL_IO_UART1_HID) }
      }
      Name (_UID, "SerialIoUart1")
      Name (_DDN, "SerialIoUart1")
      Method (_HRV) { Return (LHRV(SERIAL_IO_UART1_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_UART1_DEVICE_MODE,SERIAL_IO_UART1_BAR0,SERIAL_IO_UART1_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_UART1_DEVICE_MODE)) }
    }
    //
    // Applies for UART1 PCI mode only
    //
    If(LEqual(SERIAL_IO_UART1_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_UART1_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // UART1 PCI/ACPI PS0 PS3 and PSC excluding Hidden mode
    //
    If(LNotEqual(SERIAL_IO_UART1_DEVICE_MODE,SERIAL_IO_HIDDEN)) {
      Method (_PSC) { Return (GETD(SERIAL_IO_UART1_BAR1)) }
      Method (_PS0) { LPD0(SERIAL_IO_UART1_BAR1) }
      Method (_PS3) { LPD3(SERIAL_IO_UART1_BAR1, SERIAL_IO_UART1_DEVICE_MODE) }
    }
  }
}
//-------------------------------------------
//  Serial IO UART Controller 2 Configuration
//-------------------------------------------
Scope(\_SB.PCI0) {
  Device (UA02) {
    //
    // Applies for UART2 Hidden, ACPI or Disabled mode
    //
    If(LNotEqual(SERIAL_IO_UART2_DEVICE_MODE,SERIAL_IO_PCI)) {
      Method (_HID) {
        If(LEqual(SERIAL_IO_UART2_DEVICE_MODE,SERIAL_IO_HIDDEN)) { Return (EISAID("PNP0C02")) }
        Else { Return (SERIAL_IO_UART2_HID) }
      }
      Name (_UID, "SerialIoUart2")
      Name (_DDN, "SerialIoUart2")
      Method (_HRV) { Return (LHRV(SERIAL_IO_UART2_BAR1)) }
      Method (_CRS) { Return (LCRS(SERIAL_IO_UART2_DEVICE_MODE,SERIAL_IO_UART2_BAR0,SERIAL_IO_UART2_IRQ_NUMBER)) }
      Method (_STA) { Return (LSTA(SERIAL_IO_UART2_DEVICE_MODE)) }
    }
    //
    // Applies for UART2 PCI mode only
    //
    If(LEqual(SERIAL_IO_UART2_DEVICE_MODE,SERIAL_IO_PCI)) {
      Name (_ADR, SERIAL_IO_UART2_ADR)
      Method (_DSM,4,Serialized){If(PCIC(Arg0)) { Return(PCID(Arg0,Arg1,Arg2,Arg3)) }; Return(Buffer() {0})}
    }
    //
    // UART2 PCI/ACPI PS0 PS3 and PSC excluding Hidden mode
    //
    If(LNotEqual(SERIAL_IO_UART2_DEVICE_MODE,SERIAL_IO_HIDDEN)) {
      Method (_PSC) { Return (GETD(SERIAL_IO_UART2_BAR1)) }
      Method (_PS0) { LPD0(SERIAL_IO_UART2_BAR1) }
      Method (_PS3) { LPD3(SERIAL_IO_UART2_BAR1, SERIAL_IO_UART2_DEVICE_MODE) }
    }
  }
}
