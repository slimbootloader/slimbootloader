/** @file
  Register names for PCH PMC device

  Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PMC_H_
#define _PCH_REGS_PMC_H_


#define V_ACPI_IO_PM1_CNT_S0                     0
#define V_ACPI_IO_PM1_CNT_S3                     (BIT12 | BIT10)
#define V_ACPI_IO_PM1_CNT_S4                     (BIT12 | BIT11)
#define V_ACPI_IO_PM1_CNT_S5                     (BIT12 | BIT11 | BIT10)

#define R_ACPI_IO_PM1_STS                        0x00
#define B_ACPI_IO_PM1_STS_RTC_EN                 BIT26
#define B_ACPI_IO_PM1_EN_PWRBTN_EN               BIT24
#define B_ACPI_IO_PM1_STS_GBL                    BIT21
#define B_ACPI_IO_PM1_STS_WAK                    BIT15
#define B_ACPI_IO_PM1_STS_PRBTNOR                BIT11
#define B_ACPI_IO_PM1_STS_RTC                    BIT10
#define B_ACPI_IO_PM1_STS_PWRBTN                 BIT8
#define B_ACPI_IO_PM1_STS_TMROF                  BIT0

#define R_ACPI_IO_PM1_CNT                        0x04
#define B_ACPI_IO_PM1_CNT_SCI_EN                 BIT0
#define B_ACPI_IO_PM1_CNT_SLP_TYP                (BIT12 | BIT11 | BIT10)


#define R_ACPI_IO_SMI_STS                        0x34
#define B_ACPI_IO_SMI_STS_SMBUS                  BIT16
#define B_ACPI_IO_SMI_STS_PERIODIC               BIT14
#define B_ACPI_IO_SMI_STS_TCO                    BIT13
#define B_ACPI_IO_SMI_STS_MCSMI                  BIT11
#define B_ACPI_IO_SMI_STS_SWSMI_TMR              BIT6
#define B_ACPI_IO_SMI_STS_APM                    BIT5
#define B_ACPI_IO_SMI_STS_ON_SLP_EN              BIT4
#define B_ACPI_IO_SMI_STS_BIOS                   BIT2


#define R_ACPI_IO_SMI_EN                         0x30
#define B_ACPI_IO_SMI_EN_APMC                    BIT5
#define B_ACPI_IO_SMI_EN_EOS                     BIT1
#define B_ACPI_IO_SMI_EN_GBL_SMI                 BIT0

#define R_ACPI_IO_GPE_CNTL                       0x40

#define R_ACPI_IO_GPE0_STS_127_96                0x6C

#define R_ACPI_IO_GPE0_EN_127_96                 0x7C
#define B_ACPI_IO_GPE0_STS_127_96_PME_B0         BIT13

#define R_ACPI_IO_OC_WDT_CTL                     0x54

#define R_TCO_IO_TCO1_STS                        0x04
#define R_TCO_IO_TCO1_CNT                        0x08
#define B_TCO_IO_TCO1_CNT_LOCK                   BIT12
#define B_TCO_IO_TCO1_CNT_TMR_HLT                BIT11

#define R_PMC_PWRM_IPC_CMD                       0x00
#define V_PMC_PWRM_IPC_SRC_CLK_CMD               0xA8
#define R_PMC_PWRM_IPC_STS                       0x04
#define R_PMC_PWRM_IPC_WBUF0                     0x80
#define R_PMC_PWRM_IPC_WBUF1                     0x84
#define R_PMC_PWRM_IPC_WBUF2                     0x88
#define R_PMC_PWRM_IPC_WBUF3                     0x8C
#define R_PMC_PWRM_IPC_RBUF0                     0x90

#define R_PMC_PWRM_GEN_PMCON_A                   0x1020
#define B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS       BIT24
#define B_PMC_PWRM_GEN_PMCON_A_DISB              BIT23
#define B_PMC_PWRM_GEN_PMCON_A_MS4V              BIT18
#define B_PMC_PWRM_GEN_PMCON_A_SUS_PWR_FLR       BIT16
#define B_PMC_PWRM_GEN_PMCON_A_PWR_FLR           BIT14
#define B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS      BIT9

#define R_PMC_PWRM_GEN_PMCON_B                   0x1024
#define B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK          BIT4
#define B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS       BIT2

#define R_PMC_PWRM_MODPHY_PM_CFG5                0x10D0

#define R_PMC_PWRM_MODPHY_PM_CFG6                0x10D4

#define R_PMC_PWRM_WADT_AC                       0x1800

#define R_PMC_PWRM_CFG4                          0x18E8

#define R_PMC_PWRM_GPIO_CFG                      0x1920
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW2             (BIT11 | BIT10 | BIT9 | BIT8)
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW2             8
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW1             (BIT7 | BIT6 | BIT5 | BIT4)
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW1             4
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW0             (BIT3 | BIT2 | BIT1 | BIT0)
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW0             0

#define R_PMC_PWRM_1B1C                          0x1B1C

#define R_PMC_PWRM_ST_PG_FDIS_PMC_1              0x1E20

#define R_PMC_PWRM_NST_PG_FDIS_1                 0x1E28

#endif
