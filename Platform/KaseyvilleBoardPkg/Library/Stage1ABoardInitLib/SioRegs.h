/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SIO_REG_H_
#define _SIO_REG_H_

#define REG_LOGICAL_DEVICE        0x07
#define ACTIVATE                  0x30

#define PRIMARY_INTERRUPT_SELECT  0x70
#define INTERRUPT_TYPE            0x71

//
//ASPEED AST2500 register
//
#define AST2500_CHIP_ID_REG        0x20
#define ASPEED2500_SIO_INDEX_PORT  0x2E
#define ASPEED2500_SIO_DATA_PORT   (ASPEED2500_SIO_INDEX_PORT+1)

//
//ASPEED AST2600 register
//
#define AST2600_CHIP_ID_REG        AST2500_CHIP_ID_REG
#define ASPEED2600_SIO_INDEX_PORT  ASPEED2500_SIO_INDEX_PORT
#define ASPEED2600_SIO_DATA_PORT   ASPEED2500_SIO_DATA_PORT
#define AST2600_SOFT_INT_REG       0x21

//logical device.
#define ASPEED2500_SIO_UART1       0x02
#define ASPEED2500_SIO_UART2       0x03

#define ASPEED2600_CHIP_ID         0x26

#define ASPEED2500_SIO_UNLOCK      0xA5
#define ASPEED2500_SIO_LOCK        0xAA

#define ASPEED2500_PRIMARY_INTERRUPT_SELECT  PRIMARY_INTERRUPT_SELECT
#define ASPEED2500_SIOR2_F0                  0xF0

#endif
