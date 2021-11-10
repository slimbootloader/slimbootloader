/** @file
  Register names for PCH LPC/eSPI device

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_LPC_H_
#define _PCH_REGS_LPC_H_

#define B_LPC_CFG_DID                             0xFFE0

#define PCI_DEVICE_NUMBER_PCH_LPC                 31
#define PCI_FUNCTION_NUMBER_PCH_LPC               0

#define V_LPC_CFG_VENDOR_ID                       V_PCH_INTEL_VENDOR_ID

#define R_PCH_PCR_DMI_LPCIOD                      0x2770
#define R_PCH_PCR_DMI_LPCIOE                      0x2774


#define PCH_PCR_BASE_ADDRESS                      0xFD000000
#define PCH_PCR_ADDRESS(Pid, Offset)              (PCH_PCR_BASE_ADDRESS | ((UINT8)(Pid) << 16) | (UINT16)(Offset))

#define B_LPC_CFG_IOE_ME1                         BIT11
#define B_LPC_CFG_IOE_CBE                         BIT1
#define B_LPC_CFG_IOE_CAE                         BIT0

#define R_PCH_LPC_IOD                             0x80
#define N_PCH_LPC_IOD_COMB                        4
#define V_PCH_LPC_IOD_COMB_3E8                    7
#define N_PCH_LPC_IOD_COMA                        0
#define V_PCH_LPC_IOD_COMA_3F8                    0

#define R_LPC_CFG_IOD                             0x80
#define R_LPC_CFG_IOE                             0x82
#define R_LPC_CFG_BC                              0xDC

#define R_RTC_PCR_BUC                             0x3414

#endif
