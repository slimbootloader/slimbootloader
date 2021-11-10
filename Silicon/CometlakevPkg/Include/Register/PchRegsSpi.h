/** @file
  Register names for PCH SPI device.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_SPI_H_
#define _PCH_REGS_SPI_H_

//
// SPI Registers (D31:F5)
//

#define PCI_DEVICE_NUMBER_PCH_SPI           31
#define PCI_FUNCTION_NUMBER_PCH_SPI         5

#define R_SPI_CFG_BAR0                      0x10
#define B_SPI_CFG_BAR0_MASK                 0x0FFF


#define R_SPI_CFG_BC                        0xDC
#define B_SPI_CFG_BC_BILD                   BIT7
#define B_SPI_CFG_BC_EISS                   BIT5            ///< Enable InSMM.STS
#define B_SPI_CFG_BC_LE                     BIT1            ///< Lock Enable

//
// BIOS Flash Program Registers (based on SPI_BAR0)
//
#define R_SPI_MEM_HSFSC                     0x04                          ///< Hardware Sequencing Flash Status and Control Register(32bits)
#define B_SPI_MEM_HSFSC_FLOCKDN             BIT15                         ///< Flash Configuration Lock-Down


//
// Descriptor Record 53
//
#endif
