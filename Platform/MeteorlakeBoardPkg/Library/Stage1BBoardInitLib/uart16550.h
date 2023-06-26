/** @file

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _uart16550_h_
#define _uart16550_h_

#define RECEIVER_BUFFER             (0x00)
#define TRANSMIT_HOLDING            (0x00)
#define DIVISOR_LATCH_LOW           (0x00)    // 0/1 - 16-bit

#define DIVISOR_LATCH_HIGH          (0x01)
#define INTERRUPT_ENABLE            (0x01)
#define DISABLE_INTERRUPTS          (0x00)

#define INTERRUPT_IDENTIFICATION    (0x02)
#define FIFO_CONTROL                (0x02)
#define FIFO_DISABLE_MASK           (0xFE)

#define LINE_CONTROL                (0x03)
#define DIVISOR_LATCH_ACCESS        (0x80)
#define STOP_BITS                   (0x04)
#define NO_PARITY                   (0x00)
#define BITS8                       (0x03) // 3 = 8

#define DLAB_BIT                    (0x01)
#define STOP_BIT                    (0x01)
#define DATA_BITS                   (0x03)

#define EOF                         (0xFF)

#define LINE_STATUS                 (0x05)
#define TRANS_HOLDING_REG_EMPTY     (0x20)
#define BREAK_INTERRUPT             (0x10)
#define DATA_READY                  (0x01)

#define INDEX_PORT0                 (0x04E)
#define DATA_PORT0                  (INDEX_PORT0 + 1)
//
// Global register in NSPC87427
//

#define ICH_LPC_EN                  (0x400F8080)     // D31:F0:R80h

#define ICH_LPC_IO_DEC              (0x80)
#define ICH_LPC_IO_ENABLE           (0x82)
#define ICH_LPC_GEN1_DEC            (0x84)
#define ICH_LPC_GEN2_DEC            (0x88)

#define BAUD_RATE_DIVISOR           (115200)

// Com port options
#define MRC_COM1_BASE               (0x3f8)
#define MRC_COM2_BASE               (0x2f8)
#define MRC_COM3_BASE               (0x3e8)
#define MRC_COM4_BASE               (0x2e8)

// Select the platform com port address
#define GLOBALCOMPORT               (MRC_COM1_BASE)     // 0x3F8-0x3FF
#define GLOBALCOMPORT2              (MRC_COM3_BASE)     // 0x3F8-0x3FF

#endif // _uart16550_h_
