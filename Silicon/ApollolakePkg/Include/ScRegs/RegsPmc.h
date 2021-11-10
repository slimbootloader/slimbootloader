/** @file
  @brief
  Register names for PMC device.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_CHV_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_PCH_" without <generation_name> inserted.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REGS_PMC_H_
#define _REGS_PMC_H_

///
/// PMC Registers
///
#define PMC_BASE_ADDRESS                    0xFE042000
#define GCR_BASE_ADDRESS                    0xFE043000

#define PCI_DEVICE_NUMBER_PMC               13
#define PCI_FUNCTION_NUMBER_PMC_SSRAM       3

///
/// PMC LPC1 Controller Registers (D13:F1)
///
#define PCI_DEVICE_NUMBER_PMC_IPC1          PCI_DEVICE_NUMBER_PMC
#define PCI_FUNCTION_NUMBER_PMC_IPC1        1
#define R_PMC_IPC1_BASE                     0x10  ///< BAR0
#define R_PMC_ACPI_BASE                     0x20  ///< BAR2
#define PMC_GCR_GEN_PMCON1                  0x20

///
///  PMC Controller Registers (D13:F1)
///
#define PCI_FUNCTION_NUMBER_PMC             1
#define R_PMC_BASE                          0x10  ///< BAR0
#define R_PMC_ACPI_BASE                     0x20  ///< BAR2
#define R_PMC_GEN_PMCON_1                   0x1020  ///< General PM Configuration 1
#define R_PMC_GEN_PMCON_2                   0x1024  ///< General PM Configuration 2
#define B_PMC_GEN_PMCON_2_SMI_LOCK          BIT4    ///< SMI LOCK
#define R_PMC_BIOS_SCRATCHPAD               0x1090  ///< BIOS_SCRATCHPAD
#define B_PMC_GEN_PMCON_RTC_PWR_STS         BIT2    ///< RTC Power Status

///
/// PMC PWM Modules
/// PCI Config Space Registers
///
#define PCI_DEVICE_NUMBER_PMC_PWM           26
#define PCI_FUNCTION_NUMBER_PMC_PWM         0

#define B_PMC_IOSF2OCP_PCICFGCTRL3_BAR1_DISABLE3  BIT7

#define R_ACPI_PM1_STS                        0x00  ///< Power Management 1 Status
#define B_ACPI_PM1_STS_WAK                    BIT15 ///< Wake Status

#define R_ACPI_PM1_CNT                        0x04  ///< Power Management 1 Control
#define B_ACPI_PM1_CNT_SLP_TYP                (BIT12 | BIT11 | BIT10) ///< Sleep Type
#define V_ACPI_PM1_CNT_S3                     0x00001400 ///< Suspend-to-RAM (S3)
#define R_ACPI_PM1_TMR                        0x08  ///< Power Management 1 Timer
#define B_ACPI_PM1_TMR_VAL                    0xFFFFFF ///< The timer value mask
#define V_ACPI_PM1_TMR_MAX_VAL                0x1000000 ///< The timer is 24 bit overflow

#define R_SMI_EN                              0x40  ///< SMI Control and Enable
#define B_SMI_EN_GBL_SMI                      BIT0  ///< Global SMI Enable
#define B_SMI_EN_EOS                          BIT1  ///< End Of Service
#define B_SMI_EN_APMC                         BIT5  ///< APMC Enable
#define B_SMI_EN_SWSMI_TMR                    BIT6  ///< Software SMI Timer Enable

#define R_SMI_STS                             0x44  ///< SMI Status
#define B_SMI_STS_APMC                        BIT5  ///< APMC status

#define R_TCO_STS                             0x64  ///< TCO Timer Status
#define B_TCO_STS_SECOND_TO                   BIT17 ///< Second Timeout Status
#define R_TCO1_CNT                            0x68  ///< TCO Control
#define B_TCO1_CNT_TMR_HLT                    BIT11 ///< TCO Timer Halt
#define B_TCO1_CNT_LOCK                       BIT12 ///< TCO Enable Lock

#endif
