/** @file

  Copyright (c) 2020 - 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SERIAL_IO_I2C_REGS_H_
#define _SERIAL_IO_I2C_REGS_H_

//
// I2C Controller
// Registers accessed through BAR0 + offset
//
#define R_I2C_MEM_CON                        0x00  // I2c Control
#define B_I2C_MEM_MASTER_MODE                BIT0
#define B_I2C_MEM_RESTART_EN                 BIT5
#define B_I2C_MEM_SLAVE_DISABLE              BIT6
#define V_I2C_MEM_SPEED_STANDARD             0x02
#define V_I2C_MEM_SPEED_FAST                 0x04
#define V_I2C_MEM_SPEED_HIGH                 0x06

#define R_I2C_MEM_TAR                        0x04  // I2c Target Address
#define B_I2C_MEM_TAR_10BITADDR_MASTER       BIT12

#define R_I2C_MEM_DATA_CMD                   0x10  // I2c Rx/Tx Data Buffer and Command
#define B_I2C_MEM_CMD_READ                   BIT8  // 1 = read, 0 = write
#define B_I2C_MEM_CMD_STOP                   BIT9  // 1 = STOP
#define B_I2C_MEM_CMD_RESTART                BIT10 // 1 = I2C_RESTART_EN
#define V_I2C_MEM_WRITE_CMD_MASK             0xFF

#define R_I2C_MEM_SS_SCL_HCNT                0x14  // Standard Speed I2c Clock SCL High Count
#define R_I2C_MEM_SS_SCL_LCNT                0x18  // Standard Speed I2c Clock SCL Low Count
#define R_I2C_MEM_FS_SCL_HCNT                0x1C  // Full Speed I2c Clock SCL High Count
#define R_I2C_MEM_FS_SCL_LCNT                0x20  // Full Speed I2c Clock SCL Low Count
#define R_I2C_MEM_HS_SCL_HCNT                0x24  // High Speed I2c Clock SCL High Count
#define R_I2C_MEM_HS_SCL_LCNT                0x28  // High Speed I2c Clock SCL Low Count
#define R_I2C_MEM_INTR_STAT                  0x2C  // I2c Inetrrupt Status
#define R_I2C_MEM_INTR_MASK                  0x30  // I2c Interrupt Mask
#define B_I2C_MEM_INTR_GEN_CALL              BIT11 // General call received
#define B_I2C_MEM_INTR_START_DET             BIT10
#define B_I2C_MEM_INTR_STOP_DET              BIT9
#define B_I2C_MEM_INTR_ACTIVITY              BIT8
#define B_I2C_MEM_INTR_TX_ABRT               BIT6  // Set on NACK
#define B_I2C_MEM_INTR_TX_EMPTY              BIT4
#define B_I2C_MEM_INTR_TX_OVER               BIT3
#define B_I2C_MEM_INTR_RX_FULL               BIT2  // Data bytes in RX FIFO over threshold
#define B_I2C_MEM_INTR_RX_OVER               BIT1
#define B_I2C_MEM_INTR_RX_UNDER              BIT0
#define R_I2C_MEM_RAW_INTR_STAT              0x34  // I2c Raw Interrupt Status
#define R_I2C_MEM_RX_TL                      0x38  // I2c Receive FIFO Threshold
#define R_I2C_MEM_TX_TL                      0x3C  // I2c Transmit FIFO Threshold
#define R_I2C_MEM_CLR_INTR                   0x40  // Clear Combined and Individual Interrupts
#define R_I2C_MEM_CLR_RX_UNDER               0x44  // Clear RX_UNDER Interrupt
#define R_I2C_MEM_CLR_RX_OVER                0x48  // Clear RX_OVERinterrupt
#define R_I2C_MEM_CLR_TX_OVER                0x4C  // Clear TX_OVER interrupt
#define R_I2C_MEM_CLR_RD_REQ                 0x50  // Clear RD_REQ interrupt
#define R_I2C_MEM_CLR_TX_ABRT                0x54  // Clear TX_ABRT interrupt
#define R_I2C_MEM_CLR_RX_DONE                0x58  // Clear RX_DONE interrupt
#define R_I2C_MEM_CLR_ACTIVITY               0x5C  // Clear ACTIVITY interrupt
#define R_I2C_MEM_CLR_STOP_DET               0x60  // Clear STOP_DET interrupt
#define R_I2C_MEM_CLR_START_DET              0x64  // Clear START_DET interrupt
#define R_I2C_MEM_CLR_GEN_CALL               0x68  // Clear GEN_CALL interrupt
#define R_I2C_MEM_ENABLE                     0x6C  // I2c Enable

#define R_I2C_MEM_STATUS                     0x70  // I2c Status
#define B_I2C_MEM_STATUS_RFF                 BIT4  // RX FIFO is completely full
#define B_I2C_MEM_STATUS_RFNE                BIT3  // RX FIFO is not empty
#define B_I2C_MEM_STATUS_TFE                 BIT2  // TX FIFO is completely empty
#define B_I2C_MEM_STATUS_TFNF                BIT1  // TX FIFO is not full
#define B_I2C_MEM_STATUS_ACTIVITY            BIT0  // Controller Activity Status.

#define R_I2C_MEM_TXFLR                      0x74  // Transmit FIFO Level Register
#define R_I2C_MEM_RXFLR                      0x78  // Receive FIFO Level Register
#define R_SERIAL_IO_I2C_MEM_SDA_HOLD         0x7C
#define V_SERIAL_IO_I2C_MEM_SDA_HOLD_VALUE   0x002C002C
#define R_I2C_MEM_TX_ABRT_SOURCE             0x80  // I2c Transmit Abort Status Register
#define B_I2C_MEM_TX_ABRT_7B_ADDR_NACK       BIT0  // NACK on 7-bit address

#define R_I2C_MEM_SDA_SETUP                  0x94  // I2c SDA Setup Register
#define R_I2C_MEM_ACK_GENERAL_CALL           0x98  // I2c ACK General Call Register
#define R_I2C_MEM_ENABLE_STATUS              0x9C  // I2c Enable Status Register
#define B_I2C_MEM_EN                         BIT0  // I2c enable status

#define R_I2C_MEM_CLK_GATE                   0xC0
#define R_I2C_MEM_COMP_PARAM                 0xF4  // Component Parameter Register
#define R_I2C_MEM_COMP_VERSION               0xF8  // Component Version ID
#define R_I2C_MEM_COMP_TYPE                  0xFC  // Component Type

#define R_I2C_MEM_GENERAL                    0x208  // I2C Convergence Layer General
#define B_I2C_MEM_GENERAL_SDA_LINE_STATE     BIT26  // I2C SDA Line State: 0 - Low, 1 - High
#define B_I2C_MEM_GENERAL_SCL_LINE_STATE     BIT24  // I2C SCL Line State: 0 - Low, 1 - High

#define B_SERIAL_IO_CFG_PME_CTRL_STS_PWR_ST                 (BIT1| BIT0)

#endif // _SERIAL_IO_I2C_REGS_H_