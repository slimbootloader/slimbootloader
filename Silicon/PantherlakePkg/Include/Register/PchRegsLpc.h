/** @file
  Register names for PCH LPC/eSPI device

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_LPC_H_
#define _PCH_REGS_LPC_H_

#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define B_ESPI_CFG_DID                    0xFFE0

#define R_LPC_CFG_IOD                             0x80
#define N_LPC_CFG_IOD_COMB                        4
#define V_LPC_CFG_IOD_COMB_2F8                    1
#define N_LPC_CFG_IOD_COMA                        0
#define V_LPC_CFG_IOD_COMA_3F8                    0
#define R_LPC_CFG_IOE                             0x82
#define B_LPC_CFG_IOE_ME1                         BIT11           ///< Microcontroller Enable #1, Enables decoding of I/O locations 62h and 66h to LPC.
#define B_LPC_CFG_IOE_CBE                         BIT1            ///< Com Port B Enable, Enables decoding of the COMB range to LPC. Range is selected LIOD.CB.
#define B_LPC_CFG_IOE_CAE                         BIT0            ///< Com Port A Enable, Enables decoding of the COMA range to LPC. Range is selected LIOD.CA.

#define R_ESPI_CFG_ESPI_IOD_IOE                   R_LPC_CFG_IOD
#define R_ESPI_CFG_ESPI_BC                        0xDC            ///< Bios Control

// Processor interface registers
//
#define R_PCH_IO_NMI_EN                              0x70
#define B_PCH_IO_NMI_EN_NMI_EN                       BIT7

#define R_RTC_IO_INDEX                           0x70
#define R_RTC_IO_TARGET                          0x71
#define R_RTC_IO_EXT_INDEX                       0x72
#define R_RTC_IO_EXT_TARGET                      0x73
#define R_RTC_IO_INDEX_ALT                       0x74
#define R_RTC_IO_TARGET_ALT                      0x75
#define R_RTC_IO_EXT_INDEX_ALT                   0x76
#define R_RTC_IO_EXT_TARGET_ALT                  0x77
#define R_RTC_PCR_BUC                         0x3414               ///< Backed Up Control
#define B_RTC_PCR_BUC_DSO                     BIT4                 ///< Daylight Savings Override
#define B_RTC_PCR_BUC_TS                      BIT0                 ///< Top Swap
#define R_RTC_PCR_RTCDCG                      0x3418               ///< RTC Dynamic Clock Gating Control
#define R_RTC_IO_REGC                         0x0C

#endif // _PCH_REGS_LPC_H_
