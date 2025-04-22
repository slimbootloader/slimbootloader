/**@file

 Copyright (c) 1999 - 2021, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Register/PchRegsSerialIo.h>

#define SERIAL_IO_BAR_RESET 1
#define SERIAL_IO_D3        1

//
// Include PchSerialIo ACPI macros
//
#include <PchSerialIoAcpiDefinesEhl.h>

#define I2C0_MODE         IM00
#define I2C1_MODE         IM01
#define I2C2_MODE         IM02
#define I2C3_MODE         IM03
#define I2C4_MODE         IM04
#define I2C5_MODE         IM05
#define I2C6_MODE         IM06
#define I2C7_MODE         IM07

#define I2C0_PCIE_BASE    IC00
#define I2C1_PCIE_BASE    IC01
#define I2C2_PCIE_BASE    IC02
#define I2C3_PCIE_BASE    IC03
#define I2C4_PCIE_BASE    IC04
#define I2C5_PCIE_BASE    IC05
#define I2C6_PCIE_BASE    IC06
#define I2C7_PCIE_BASE    IC07

#define SPI0_MODE         SM00
#define SPI1_MODE         SM01
#define SPI2_MODE         SM02
#define SPI3_MODE         SM03
#define SPI4_MODE         SM04
#define SPI5_MODE         SM05
#define SPI6_MODE         SM06

#define SPI0_PCIE_BASE    SC00
#define SPI1_PCIE_BASE    SC01
#define SPI2_PCIE_BASE    SC02
#define SPI3_PCIE_BASE    SC03
#define SPI4_PCIE_BASE    SC04
#define SPI5_PCIE_BASE    SC05
#define SPI6_PCIE_BASE    SC06

#define UART0_MODE        UM00
#define UART1_MODE        UM01
#define UART2_MODE        UM02
#define UART3_MODE        UM03
#define UART4_MODE        UM04
#define UART5_MODE        UM05
#define UART6_MODE        UM06

#define UART0_PCIE_BASE   UC00
#define UART1_PCIE_BASE   UC01
#define UART2_PCIE_BASE   UC02
#define UART3_PCIE_BASE   UC03
#define UART4_PCIE_BASE   UC04
#define UART5_PCIE_BASE   UC05
#define UART6_PCIE_BASE   UC06

#define UART0_DMA         UD00
#define UART1_DMA         UD01
#define UART2_DMA         UD02
#define UART3_DMA         UD03
#define UART4_DMA         UD04
#define UART5_DMA         UD05
#define UART6_DMA         UD06

#define UART0_IRQ         UI00
#define UART1_IRQ         UI01
#define UART2_IRQ         UI02
#define UART3_IRQ         UI03
#define UART4_IRQ         UI04
#define UART5_IRQ         UI05
#define UART6_IRQ         UI06

#define UART0_PG          UP00
#define UART1_PG          UP01
#define UART2_PG          UP02
#define UART3_PG          UP03
#define UART4_PG          UP04
#define UART5_PG          UP05
#define UART6_PG          UP06

Scope(\_SB.PC00) {
  //
  //  Serial IO D3
  //  Common code for D3 entry for all SerialIo devices including PCI mode
  //
  //  @param[in]  Arg0      PCI CFG address
  //  @param[in]  Arg1      Boolean - Place Device in D3 (SERIAL_IO_D3)
  //  @param[in]  Arg2      Boolean - Reset BAR0 (SERIAL_IO_BAR_RESET) or leave BAR0 unchanged (0)
  //                                  depends on Arg1 equal to SERIAL_IO_D3 - device placed in D3
  //
  Method (SOD3, 3, Serialized) {
    OperationRegion(ICB1, SystemMemory, Add(\_SB.PC00.GPCB(), Arg0), Add (R_SERIAL_IO_CFG_PME_CTRL_STS, 4))
    If (Arg1) { // SERIAL_IO_D3
      Field(ICB1, ByteAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_PME_CTRL_STS),
        PMEC, 8 //Note: SerialIO Does Not Support PME
      }
      Store(0x3, PMEC) // Set BIT[1:0] = 11b - Power State D3
      Store(PMEC,Local3) // perform a read to avoid ordering and noncoherency problems
      Store(Local3,Local3)
    }
    If (LAnd(Arg1, Arg2)) { //SERIAL_IO_BAR_RESET only if device is placed in D3
      Field(ICB1, AnyAcc, NoLock, Preserve) {
        Offset(R_SERIAL_IO_CFG_BAR0_LOW),
        BAR0, 64
      }
      Store(0, BAR0)
    }
  }
} //End Scope(\_SB.PC00)

Include ("PchSerialIoSpi.asl")
Include ("PchSerialIoI2c.asl")
Include ("PchSerialIoUart.asl")

