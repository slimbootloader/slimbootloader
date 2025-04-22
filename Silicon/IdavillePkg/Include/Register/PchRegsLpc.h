/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __PCH_REGS_LPC_H__
#define __PCH_REGS_LPC_H__

//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define R_PCH_IO_NMI_EN                           0x70

#define R_LPC_CFG_IOD                             0x80
#define N_LPC_CFG_IOD_LPT                         8
#define V_LPC_CFG_IOD_LPT_378                     0
#define R_LPC_CFG_IOE                             0x82
#define B_LPC_CFG_IOE_ME2                         BIT13           ///< Microcontroller Enable #2, Enables decoding of I/O locations 4Eh and 4Fh to LPC
#define B_LPC_CFG_IOE_SE                          BIT12           ///< Super I/O Enable, Enables decoding of I/O locations 2Eh and 2Fh to LPC.
#define B_LPC_CFG_IOE_ME1                         BIT11           ///< Microcontroller Enable #1, Enables decoding of I/O locations 62h and 66h to LPC.
#define B_LPC_CFG_IOE_KE                          BIT10           ///< Keyboard Enable, Enables decoding of the keyboard I/O locations 60h and 64h to LPC.
#define B_LPC_CFG_IOE_HGE                         BIT9            ///< High Gameport Enable, Enables decoding of the I/O locations 208h to 20Fh to LPC.
#define B_LPC_CFG_IOE_LGE                         BIT8            ///< Low Gameport Enable, Enables decoding of the I/O locations 200h to 207h to LPC.
#define B_LPC_CFG_IOE_FDE                         BIT3            ///< Floppy Drive Enable, Enables decoding of the FDD range to LPC. Range is selected by LIOD.FDE
#define B_LPC_CFG_IOE_PPE                         BIT2            ///< Parallel Port Enable, Enables decoding of the LPT range to LPC. Range is selected by LIOD.LPT.
#define R_LPC_CFG_GEN1_DEC                        0x84
#define B_LPC_CFG_GENX_DEC_IODRA                  0x00FC0000
#define B_LPC_CFG_GENX_DEC_IOBAR                  0x0000FFFC
#define B_LPC_CFG_GENX_DEC_EN                     0x00000001
#define R_LPC_CFG_LGMR                            0x98
#define B_LPC_CFG_LGMR_MA                         0xFFFF0000
#define B_LPC_CFG_LGMR_LMRD_EN                    BIT0
#define R_ESPI_CFG_CS1IORE                        0xA0
#define R_ESPI_CFG_CS1GMR1                        0xA8

#define R_LPC_CFG_BDE                             0xD8            ///< BIOS decode enable
#define R_ESPI_CFG_CS1GIR1                        0xA4
#define R_LPC_CFG_BC                              0xDC            ///< Bios Control

#define B_PCH_IO_NMI_EN_NMI_EN                    BIT7

//
// RTC PCRs (PID:RTC)
//
#define R_RTC_PCR_BUC                         0x3414               ///< Backed Up Control
#define B_RTC_PCR_BUC_TS                      BIT0                 ///< Top Swap

#endif /* __PCH_REGS_LPC_H__ */
