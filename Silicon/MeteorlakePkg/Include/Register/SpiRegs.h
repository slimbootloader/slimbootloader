/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _SPI_REGS_H_
#define _SPI_REGS_H_


#define R_SPI_CFG_BAR0                      0x10


#define R_SPI_CFG_BC                        0xDC
#define B_SPI_CFG_BC_EISS                   BIT5            ///< Enable InSMM.STS
#define B_SPI_CFG_BC_LE                     BIT1            ///< Lock Enable
#define B_SPI_CFG_BC_WPD                    BIT0            ///< Write Protect Disable


#endif
