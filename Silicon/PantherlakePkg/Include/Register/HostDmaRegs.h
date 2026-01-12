/** @file
  Register names for Host DMA Block
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _HOST_DMA_REGS_H_
#define _HOST_DMA_REGS_H_

//
// Device 13 (decimal) Function 2/3 Equates (Host DMAx , x=0/1)
//
#define HOST_DMA_BUS_NUM        0x00
#define HOST_DMA_DEV_NUM        0x0D

#define HOST_DMA0_FUN_NUM       0x02
#define HOST_DMA1_FUN_NUM       0x03

#define R_HOST_DMA_LBAR         0x10
#define R_HOST_DMA_UBAR         0x14

#define V_HOST_DMA_ITBT_SECURITY_LEVEL0           0x0
#define V_HOST_DMA_ITBT_SECURITY_LEVEL5           0x5
#define V_HOST_DMA_DMA_ACTIVE_DELAY_VALUE         0x22

#endif
