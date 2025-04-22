/** @file
  Register names for PCH GPIO

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_GPIO_H_
#define _PCH_REGS_GPIO_H_

#define R_GPIO_PCR_REV_ID               0x00
#define R_GPIO_PCR_CAP_LIST             0x04
#define R_GPIO_PCR_FAMBAR               0x08
#define R_GPIO_PCR_PADBAR               0x0C
#define B_GPIO_PCR_PADBAR               0x0000FFFF
#define R_GPIO_PCR_MISCCFG              0x10
#define B_GPIO_PCR_MISCCFG_GPE0_DW2     (BIT19 | BIT18 | BIT17 | BIT16)
#define N_GPIO_PCR_MISCCFG_GPE0_DW2     16
#define B_GPIO_PCR_MISCCFG_GPE0_DW1     (BIT15 | BIT14 | BIT13 | BIT12)
#define N_GPIO_PCR_MISCCFG_GPE0_DW1     12
#define B_GPIO_PCR_MISCCFG_GPE0_DW0     (BIT11 | BIT10 | BIT9 | BIT8)
#define N_GPIO_PCR_MISCCFG_GPE0_DW0     8
#define B_GPIO_PCR_MISCCFG_GPSIDEDPCGEN    BIT5
#define B_GPIO_PCR_MISCCFG_GPRCOMPCDLCGEN  BIT4
#define B_GPIO_PCR_MISCCFG_GPRTCDLCGEN     BIT3
#define B_GPIO_PCR_MISCCFG_GPDPCGEN     BIT1
#define B_GPIO_PCR_MISCCFG_GPDLCGEN     BIT0
#define R_GPIO_PCR_MISCSECCFG           0x14

//
#define R_GPIO_PCR_CAP_LIST_1_PWM         0x0200
#define R_GPIO_PCR_PWMC                   0x0204
#define R_GPIO_PCR_CAP_LIST_2_SER_BLINK   0x0208
#define R_GPIO_PCR_GP_SER_BLINK           0x020C
#define B_GPIO_PCR_GP_SER_BLINK           0x1F
#define R_GPIO_PCR_GP_SER_CMDSTS          0x0210
#define B_GPIO_PCR_GP_SER_CMDSTS_DLS      (BIT23 | BIT22)
#define N_GPIO_PCR_GP_SER_CMDSTS_DLS      22
#define B_GPIO_PCR_GP_SER_CMDSTS_DRS      0x003F0000
#define N_GPIO_PCR_GP_SER_CMDSTS_DRS      16
#define B_GPIO_PCR_GP_SER_CMDSTS_BUSY     BIT8
#define B_GPIO_PCR_GP_SER_CMDSTS_GO       BIT0
#define R_GPIO_PCR_GP_SER_DATA            0x0210
#define V_GPIO_PCR_HOSTSW_OWN_ACPI      0x00
#define V_GPIO_PCR_HOSTSW_OWN_GPIO      0x01
#define V_GPIO_PCR_PAD_OWN_HOST         0x00
#define V_GPIO_PCR_PAD_OWN_CSME         0x01
#define V_GPIO_PCR_PAD_OWN_ISH          0x02
// PADCFG register is split into multiple DW registers
// S_GPIO_PCR_PADCFG refers to number of bytes used by all those registers for one pad
//
#define S_GPIO_PCR_PADCFG               0x10


#define B_GPIO_PCR_RST_CONF             (BIT31 | BIT30)
#define N_GPIO_PCR_RST_CONF             30
#define V_GPIO_PCR_RST_CONF_POW_GOOD    0x00
#define V_GPIO_PCR_RST_CONF_DEEP_RST    0x01
#define V_GPIO_PCR_RST_CONF_GPIO_RST    0x02
#define V_GPIO_PCR_RST_CONF_RESUME_RST  0x03  // Only for GPD Group
#define B_GPIO_PCR_RX_PAD_STATE         BIT29
#define N_GPIO_PCR_RX_PAD_STATE         29
#define V_GPIO_PCR_RX_PAD_STATE_RAW     0x00
#define V_GPIO_PCR_RX_PAD_STATE_INT     0x01
#define B_GPIO_PCR_RX_RAW1              BIT28
#define N_GPIO_PCR_RX_RAW1              28
#define V_GPIO_PCR_RX_RAW1_DIS          0x00
#define V_GPIO_PCR_RX_RAW1_EN           0x01
#ifdef PCH_ICL
#define B_PCH_GPIO_NAF_VWE              BIT27
#define N_PCH_GPIO_NAF_VWE              27
#define V_PCH_GPIO_NAF_VWE_DIS          0x00
#define V_PCH_GPIO_NAF_VWE_EN           0x01
#endif
#define B_GPIO_PCR_RX_LVL_EDG           (BIT26 | BIT25)
#define N_GPIO_PCR_RX_LVL_EDG           25
#define V_GPIO_PCR_RX_LVL_EDG_LVL       0x00
#define V_GPIO_PCR_RX_LVL_EDG_EDG       0x01
#define V_GPIO_PCR_RX_LVL_EDG_0         0x02
#define V_GPIO_PCR_RX_LVL_EDG_RIS_FAL   0x03
#define B_GPIO_PCR_RXINV                BIT23
#define N_GPIO_PCR_RXINV                23
#define V_GPIO_PCR_RXINV_NO             0x00
#define V_GPIO_PCR_RXINV_YES            0x01
#define B_GPIO_PCR_RX_APIC_ROUTE        BIT20
#define N_GPIO_PCR_RX_APIC_ROUTE        20
#define V_GPIO_PCR_RX_APIC_ROUTE_DIS    0x00
#define V_GPIO_PCR_RX_APIC_ROUTE_EN     0x01
#define B_GPIO_PCR_RX_SCI_ROUTE         BIT19
#define N_GPIO_PCR_RX_SCI_ROUTE         19
#define V_GPIO_PCR_RX_SCI_ROUTE_DIS     0x00
#define V_GPIO_PCR_RX_SCI_ROUTE_EN      0x01
#define B_GPIO_PCR_RX_SMI_ROUTE         BIT18
#define N_GPIO_PCR_RX_SMI_ROUTE         18
#define V_GPIO_PCR_RX_SMI_ROUTE_DIS     0x00
#define V_GPIO_PCR_RX_SMI_ROUTE_EN      0x01
#define B_GPIO_PCR_RX_NMI_ROUTE         BIT17
#define N_GPIO_PCR_RX_NMI_ROUTE         17
#define V_GPIO_PCR_RX_NMI_ROUTE_DIS     0x00
#define V_GPIO_PCR_RX_NMI_ROUTE_EN      0x01
#define B_GPIO_PCR_PAD_MODE             (BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_PAD_MODE             10
#define V_GPIO_PCR_PAD_MODE_GPIO        0
#define V_GPIO_PCR_PAD_MODE_NAT_1       1
#define V_GPIO_PCR_PAD_MODE_NAT_2       2
#define V_GPIO_PCR_PAD_MODE_NAT_3       3
#define V_GPIO_PCR_PAD_MODE_NAT_4       4 // SPT-H only
#define B_GPIO_PCR_RXDIS                BIT9
#define N_GPIO_PCR_RXDIS                9
#define V_GPIO_PCR_RXDIS_EN             0x00
#define V_GPIO_PCR_RXDIS_DIS            0x01
#define B_GPIO_PCR_TXDIS                BIT8
#define N_GPIO_PCR_TXDIS                8
#define V_GPIO_PCR_TXDIS_EN             0x00
#define V_GPIO_PCR_TXDIS_DIS            0x01
#define B_GPIO_PCR_RX_STATE             BIT1
#define N_GPIO_PCR_RX_STATE             1
#define V_GPIO_PCR_RX_STATE_LOW         0x00
#define V_GPIO_PCR_RX_STATE_HIGH        0x01
#define B_GPIO_PCR_TX_STATE             BIT0
#define N_GPIO_PCR_TX_STATE             0
#define V_GPIO_PCR_TX_STATE_LOW         0x00
#define V_GPIO_PCR_TX_STATE_HIGH        0x01
#define B_GPIO_PCR_PADTOL               BIT25
#define N_GPIO_PCR_PADTOL               25
#define V_GPIO_PCR_PADTOL_NONE          0x00
#define V_GPIO_PCR_PADTOL_CLEAR         0x00
#define V_GPIO_PCR_PADTOL_SET           0x01
#define B_GPIO_PCR_TERM                (BIT13 | BIT12 | BIT11 | BIT10)
#define N_GPIO_PCR_TERM                 10
#define V_GPIO_PCR_TERM_WPD_NONE        0x00
#define V_GPIO_PCR_TERM_WPD_5K          0x02
#define V_GPIO_PCR_TERM_WPD_20K         0x04
#define V_GPIO_PCR_TERM_WPU_NONE        0x08
#define V_GPIO_PCR_TERM_WPU_1K          0x09
#define V_GPIO_PCR_TERM_WPU_2K          0x0B
#define V_GPIO_PCR_TERM_WPU_5K          0x0A
#define V_GPIO_PCR_TERM_WPU_20K         0x0C
#define V_GPIO_PCR_TERM_WPU_1K_2K       0x0D
#define V_GPIO_PCR_TERM_NATIVE          0x0F
//Interrupt number
#define B_GPIO_PCR_INTSEL               0x7F
#define N_GPIO_PCR_INTSEL               0


#define B_GPIO_PCR_VCCIOSEL              BIT8
#define N_GPIO_PCR_VCCIOSEL              8
#define B_GPIO_PCR_DEBOUNCE             (BIT4 | BIT3 | BIT2 | BIT1)
#define N_GPIO_PCR_DEBOUNCE              1
#define B_GPIO_PCR_DEBEN                 BIT0
#define N_GPIO_PCR_DEBEN                 0
//
#define V_GPIO_PCR_OWN_GPIO             0x01
#define V_GPIO_PCR_OWN_ACPI             0x00
#define V_GPIO_PCR_GPE_EN               0x01
#define V_GPIO_PCR_GPE_DIS              0x00
#define V_GPIO_PCR_SMI_EN               0x01
#define V_GPIO_PCR_SMI_DIS              0x00
#define V_GPIO_PCR_NMI_EN               0x01
#define V_GPIO_PCR_NMI_DIS              0x00
// GPIO native features pins data
//
#define PCH_GPIO_HDA_LINK_NUMBER_OF_PINS     6
#define PCH_GPIO_HDA_DMIC_NUMBER_OF_PINS     2
#define PCH_GPIO_HDA_SSP_NUMBER_OF_PINS      4
#define PCH_GPIO_HDA_SNDW_NUMBER_OF_PINS     2
#define PCH_GPIO_SMBUS_NUMBER_OF_PINS        2
#define PCH_GPIO_CPU_GP_NUMBER_OF_PINS       4
#define PCH_GPIO_EDP_NUMBER_OF_PINS          4
#define PCH_GPIO_DDSP_HPD_NUMBER_OF_PINS     4
#define PCH_GPIO_DDP_NUMBER_OF_INTERFACES    4
#define PCH_GPIO_DDP_NUMBER_OF_PINS          2
#define PCH_GPIO_CNVI_UART_NUMBER_OF_PINS    4
#define PCH_GPIO_CNVI_SSP_NUMBER_OF_PINS     4
#define PCH_GPIO_CNVI_BRI_RGI_NUMBER_OF_PINS 4


#define PCH_GPIO_NUM_SUPPORTED_GPIS       512
#define S_GPIO_PCR_GP_SMI_EN                4
#define S_GPIO_PCR_GP_SMI_STS               4
#define PCH_GPIO_2_TIER_MASTER_GPE_NUMBER  0x6F
#endif // _GPIO_REGS_H_
