/** @file
  Register names for PCH LPC/eSPI device

  Copyright (c) 2021-2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_LPC_H_
#define _PCH_REGS_LPC_H_

#define B_LPC_CFG_DID         0xFFE0


#define R_LPC_CFG_IOD                             0x80
#define N_LPC_CFG_IOD_COMB                        4
#define V_LPC_CFG_IOD_COMB_2F8                    1
#define N_LPC_CFG_IOD_COMA                        0
#define V_LPC_CFG_IOD_COMA_3F8                    0
#define R_LPC_CFG_IOE                             0x82
#define B_LPC_CFG_IOE_ME1                         BIT11           ///< Microcontroller Enable #1, Enables decoding of I/O locations 62h and 66h to LPC.
#define B_LPC_CFG_IOE_CBE                         BIT1            ///< Com Port B Enable, Enables decoding of the COMB range to LPC. Range is selected LIOD.CB.
#define B_LPC_CFG_IOE_CAE                         BIT0            ///< Com Port A Enable, Enables decoding of the COMA range to LPC. Range is selected LIOD.CA.
#define B_LPC_CFG_IOE_ME2                         BIT13

#define R_LPC_CFG_BC                              0xDC            ///< Bios Control


//
// Processor interface registers
//
#define R_PCH_IO_NMI_EN                              0x70
#define B_PCH_IO_NMI_EN_NMI_EN                       BIT7


#endif
