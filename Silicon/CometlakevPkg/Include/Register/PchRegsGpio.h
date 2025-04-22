/** @file
  Register names for PCH GPIO

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_GPIO_H_
#define _PCH_REGS_GPIO_H_

#define S_GPIO_PCR_PADCFG               0x08

#define B_GPIO_PCR_RST_CONF             (BIT31 | BIT30)
#define N_GPIO_PCR_RST_CONF             30
#define V_GPIO_PCR_RST_CONF_POW_GOOD    0x00
#define V_GPIO_PCR_RST_CONF_DEEP_RST    0x01
#define V_GPIO_PCR_RST_CONF_GPIO_RST    0x02
#define V_GPIO_PCR_RST_CONF_RESUME_RST  0x03
#define B_GPIO_PCR_RX_RAW1              BIT28
#define N_GPIO_PCR_RX_RAW1              28
#define B_GPIO_PCR_RX_LVL_EDG           (BIT26 | BIT25)
#define N_GPIO_PCR_RX_LVL_EDG           25
#define B_GPIO_PCR_RXINV                BIT23
#define N_GPIO_PCR_RXINV                23
#define B_GPIO_PCR_RX_APIC_ROUTE        BIT20
#define B_GPIO_PCR_RX_SCI_ROUTE         BIT19
#define B_GPIO_PCR_RX_SMI_ROUTE         BIT18
#define B_GPIO_PCR_RX_NMI_ROUTE         BIT17
#define N_GPIO_PCR_RX_NMI_ROUTE         17
#define B_GPIO_PCR_TERM                 (BIT13 | BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_TERM                 10
#define B_GPIO_PCR_PAD_MODE             (BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_PAD_MODE             10
#define B_GPIO_PCR_RXDIS                BIT9
#define B_GPIO_PCR_TXDIS                BIT8
#define N_GPIO_PCR_TXDIS                8
#define B_GPIO_PCR_INTSEL               0x7F
#define B_GPIO_PCR_RX_STATE             BIT1
#define N_GPIO_PCR_RX_STATE             1
#define B_GPIO_PCR_TX_STATE             BIT0
#define N_GPIO_PCR_TX_STATE             0

#endif
