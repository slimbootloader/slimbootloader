/** @file
  Register names for PCH PMC device

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_[generation_name]_" in register/bit names.
  - Registers / bits that are specific to PCH-H denoted by "_H_" in register/bit names.
    Registers / bits that are specific to PCH-LP denoted by "_LP_" in register/bit names.
    e.g., "_PCH_H_", "_PCH_LP_"
    Registers / bits names without _H_ or _LP_ apply for both H and LP.
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without [generation_name] inserted.

  Copyright (c) 1999 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_PMC_H_
#define _PCH_REGS_PMC_H_

//
// PMC Registers (D31:F2)
//
#define PCI_DEVICE_NUMBER_PCH_PMC                           31
#define PCI_FUNCTION_NUMBER_PCH_PMC                         2

#define B_PCH_PMC_PM_DATA_BAR                               0xFFFFC000
#define B_PCH_PMC_GEN_PMCON_A_DISB                          BIT23

//
// ACPI and legacy I/O register offsets from ACPIBASE
//
#define R_ACPI_IO_PM1_TMR                        0x08
#define V_ACPI_IO_PM1_TMR_FREQUENCY              3579545
#define B_ACPI_IO_PM1_TMR_VAL                    0xFFFFFF
#define V_ACPI_IO_PM1_TMR_MAX_VAL                0x1000000       ///< The timer is 24 bit overflow

#define R_ACPI_IO_SMI_EN                              0x30
#define S_ACPI_IO_SMI_EN                              4
#define B_ACPI_IO_SMI_EN_LEGACY_USB3                  BIT31
#define B_ACPI_IO_SMI_EN_GPIO_UNLOCK_SMI              BIT27
#define B_ACPI_IO_SMI_EN_LEGACY_USB2                  BIT17
#define B_ACPI_IO_SMI_EN_PERIODIC                     BIT14
#define B_ACPI_IO_SMI_EN_TCO                          BIT13
#define B_ACPI_IO_SMI_EN_MCSMI                        BIT11
#define B_ACPI_IO_SMI_EN_BIOS_RLS                     BIT7
#define B_ACPI_IO_SMI_EN_SWSMI_TMR                    BIT6
#define B_ACPI_IO_SMI_EN_APMC                         BIT5
#define B_ACPI_IO_SMI_EN_ON_SLP_EN                    BIT4
#define B_ACPI_IO_SMI_EN_LEGACY_USB                   BIT3
#define B_ACPI_IO_SMI_EN_BIOS                         BIT2
#define B_ACPI_IO_SMI_EN_EOS                          BIT1
#define B_ACPI_IO_SMI_EN_GBL_SMI                      BIT0
#define N_ACPI_IO_SMI_EN_LEGACY_USB3                  31
#define N_ACPI_IO_SMI_EN_ESPI                         28
#define N_ACPI_IO_SMI_EN_GPIO_UNLOCK                  27
#define N_ACPI_IO_SMI_EN_INTEL_USB2                   18
#define N_ACPI_IO_SMI_EN_LEGACY_USB2                  17
#define N_ACPI_IO_SMI_EN_PERIODIC                     14
#define N_ACPI_IO_SMI_EN_TCO                          13
#define N_ACPI_IO_SMI_EN_MCSMI                        11
#define N_ACPI_IO_SMI_EN_BIOS_RLS                     7
#define N_ACPI_IO_SMI_EN_SWSMI_TMR                    6
#define N_ACPI_IO_SMI_EN_APMC                         5
#define N_ACPI_IO_SMI_EN_ON_SLP_EN                    4
#define N_ACPI_IO_SMI_EN_LEGACY_USB                   3
#define N_ACPI_IO_SMI_EN_BIOS                         2
#define N_ACPI_IO_SMI_EN_EOS                          1
#define N_ACPI_IO_SMI_EN_GBL_SMI                      0

#define B_ACPI_IO_PM1_EN_PWRBTN_EN                BIT24

#define B_PCH_SMI_STS_APM                         BIT5

//
// PWRM Registers
//
#define B_PCH_PWRM_GPIO_CFG_GPE0_DW2                        (BIT11 | BIT10 | BIT9 | BIT8)
#define N_PCH_PWRM_GPIO_CFG_GPE0_DW2                        8
#define B_PCH_PWRM_GPIO_CFG_GPE0_DW1                        (BIT7 | BIT6 | BIT5 | BIT4)
#define N_PCH_PWRM_GPIO_CFG_GPE0_DW1                        4
#define B_PCH_PWRM_GPIO_CFG_GPE0_DW0                        (BIT3 | BIT2 | BIT1 | BIT0)
#define N_PCH_PWRM_GPIO_CFG_GPE0_DW0                        0
/// @todo SERVER - CDF - Update SATA 1..3 function disable bits
#define B_CDF_PCH_PWRM_NST_PG_FDIS_1_SATA_1_FDIS_PMC        BIT27  ///< SATA 1 Function Disable (CDF PCH)
#define B_CDF_PCH_PWRM_NST_PG_FDIS_1_SATA_2_FDIS_PMC        BIT27  ///< SATA 2 Function Disable (CDF PCH)
#define B_CDF_PCH_PWRM_NST_PG_FDIS_1_SATA_3_FDIS_PMC        BIT27  ///< SATA 3 Function Disable (CDF PCH)
/// @todo SERVER - CDF - Update SATA 1..3 fuse/ss disable bits
#define B_CDF_PCH_PWRM_FUSE_DIS_RD_2_SATA_1_FUSE_SS_DIS     BIT15  ///< SATA 1 Fuse or Soft Strap Disable (CDF PCH)
#define B_CDF_PCH_PWRM_FUSE_DIS_RD_2_SATA_2_FUSE_SS_DIS     BIT15  ///< SATA 2 Fuse or Soft Strap Disable (CDF PCH)
#define B_CDF_PCH_PWRM_FUSE_DIS_RD_2_SATA_3_FUSE_SS_DIS     BIT15  ///< SATA 3 Fuse or Soft Strap Disable (CDF PCH)


#endif
