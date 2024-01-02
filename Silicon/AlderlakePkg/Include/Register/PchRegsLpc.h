/** @file
  Register names for PCH LPC/eSPI device

  Copyright (c) 2021-2023, Intel Corporation. All rights reserved.<BR>
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

#define R_ESPI_CFG_ESPI_LGIR1                     0x84                      ///< LPC Generic IO Range 1
#define B_ESPI_CFG_ESPI_LGIR1_LDE                 BIT0

#define R_PCH_DMI_PCR_LPCLGIR1                    0x2730                       ///< LPC Generic I/O Range 1
#define R_PCH_DMI_PCR_LPCLGIR2                    0x2734                       ///< LPC Generic I/O Range 2
#define R_PCH_DMI_PCR_LPCLGIR3                    0x2738                       ///< LPC Generic I/O Range 3
#define R_PCH_DMI_PCR_LPCLGIR4                    0x273C                       ///< LPC Generic I/O Range 4

#define R_LPC_CFG_BC                              0xDC            ///< Bios Control


//
// Processor interface registers
//
#define R_PCH_IO_NMI_EN                              0x70
#define B_PCH_IO_NMI_EN_NMI_EN                       BIT7


#endif
