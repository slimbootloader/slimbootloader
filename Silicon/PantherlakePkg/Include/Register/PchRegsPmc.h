/**@file
  Register names for PCH PMC device

  Conventions:

  - Register definition format:
    Prefix_[GenerationName]_[ComponentName]_SubsystemName_RegisterSpace_RegisterName
  - Prefix:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values within the bits
    Definitions beginning with "S_" are register size
    Definitions beginning with "N_" are the bit position
  - [GenerationName]:
    Three letter acronym of the generation is used (e.g. SKL,KBL,CNL etc.).
    Register name without GenerationName applies to all generations.
  - [ComponentName]:
    This field indicates the component name that the register belongs to (e.g. PCH, SA etc.)
    Register name without ComponentName applies to all components.
    Register that is specific to -H denoted by "_PCH_H_" in component name.
    Register that is specific to -LP denoted by "_PCH_LP_" in component name.
  - SubsystemName:
    This field indicates the subsystem name of the component that the register belongs to
    (e.g. PCIE, USB, SATA, GPIO, PMC etc.).
  - RegisterSpace:
    MEM - MMIO space register of subsystem.
    IO  - IO space register of subsystem.
    PCR - Private configuration register of subsystem.
    CFG - PCI configuration space register of subsystem.
  - RegisterName:
    Full register name.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCH_REGS_PMC_H_
#define _PCH_REGS_PMC_H_

//
// PMC Registers (D31:F2)
//
#define PCI_DEVICE_NUMBER_PCH_PMC                31
#define PCI_FUNCTION_NUMBER_PCH_PMC              2

//
// ACPI and legacy I/O register offsets from ACPIBASE
//
#define R_ACPI_IO_PM1_STS                        0x00
#define S_ACPI_IO_PM1_STS                        2
#define B_ACPI_IO_PM1_EN_PWRBTN_EN               BIT24
#define B_ACPI_IO_PM1_STS_WAK                    BIT15
#define B_ACPI_IO_PM1_STS_PCIEXP_WAKE_STS        BIT14
#define B_ACPI_IO_PM1_STS_PRBTNOR                BIT11
#define B_ACPI_IO_PM1_STS_RTC                    BIT10
#define B_ACPI_IO_PM1_STS_PWRBTN                 BIT8
#define B_ACPI_IO_PM1_STS_GBL                    BIT5
#define B_ACPI_IO_PM1_STS_BM                     BIT4
#define B_ACPI_IO_PM1_STS_TMROF                  BIT0
#define N_ACPI_IO_PM1_STS_RTC                    10
#define N_ACPI_IO_PM1_STS_PWRBTN                 8
#define N_ACPI_IO_PM1_STS_TMROF                  0

#define R_ACPI_IO_PM1_EN                         0x02
#define S_ACPI_IO_PM1_EN                         2
#define B_ACPI_IO_PM1_EN_RTC                     BIT10
#define B_ACPI_IO_PM1_EN_PWRBTN                  BIT8
#define N_ACPI_IO_PM1_EN_RTC                     10
#define N_ACPI_IO_PM1_EN_PWRBTN                  8
#define N_ACPI_IO_PM1_EN_TMROF                   0

#define R_ACPI_IO_PM1_CNT                        0x04
#define B_ACPI_IO_PM1_CNT_SLP_EN                 BIT13
#define B_ACPI_IO_PM1_CNT_SLP_TYP                (BIT12 | BIT11 | BIT10)
#define V_ACPI_IO_PM1_CNT_S0                     0
#define V_ACPI_IO_PM1_CNT_S1                     BIT10
#define V_ACPI_IO_PM1_CNT_S3                     (BIT12 | BIT10)
#define V_ACPI_IO_PM1_CNT_S4                     (BIT12 | BIT11)
#define V_ACPI_IO_PM1_CNT_S5                     (BIT12 | BIT11 | BIT10)
#define B_ACPI_IO_PM1_CNT_SCI_EN                 BIT0

#define R_ACPI_IO_PM1_TMR                        0x08
#define V_ACPI_IO_PM1_TMR_MAX_VAL                0x1000000       ///< The timer is 24 bit overflow

#define R_ACPI_IO_GPE1_STS_31_0                       0x10                            ///< General Purpose Event 1 Status [31:0]
#define B_ACPI_IO_GPE1_STS_31_0_LPSS_PME_B0_STS       BIT14

#define R_ACPI_IO_SMI_EN                              0x30
#define S_ACPI_IO_SMI_EN                              4
#define B_ACPI_IO_SMI_EN_ESPI_SMI_EN                  BIT28                         ///< eSPI SMI Enable
#define B_ACPI_IO_SMI_EN_THERM_SMI_EN                 BIT18                         ///< THERM SMI Enable
#define B_ACPI_IO_SMI_EN_LEGACY_USB2_EN               BIT17                         ///< Legacy USB 2 Enable
#define B_ACPI_IO_SMI_EN_PERIODIC_EN                  BIT14                         ///< Periodic Enable
#define B_ACPI_IO_SMI_EN_TCO_EN                       BIT13                         ///< TCO Enable
#define B_ACPI_IO_SMI_EN_BIOS_RLS                     BIT7                          ///< BIOS_RLS
#define B_ACPI_IO_SMI_EN_SWSMI_TMR_EN                 BIT6                          ///< Software SMI Timer Enable
#define B_ACPI_IO_SMI_EN_APMC_EN                      BIT5                          ///< APMC Enable
#define B_ACPI_IO_SMI_EN_SMI_ON_SLP_EN                BIT4                          ///< SMI On Sleep Enable
#define B_ACPI_IO_SMI_EN_LEGACY_USB_EN                BIT3                          ///< Legacy USB Enable
#define B_ACPI_IO_SMI_EN_BIOS_EN                      BIT2                          ///< BIOS Enable
#define B_ACPI_IO_SMI_EN_EOS                          BIT1                          ///< End of SMI
#define B_ACPI_IO_SMI_EN_GBL_SMI_EN                   BIT0                          ///< Global SMI Enable
#define N_ACPI_IO_SMI_EN_XHCI_SMI_EN                  31
#define N_ACPI_IO_SMI_EN_ESPI_SMI_EN                  28
#define N_ACPI_IO_SMI_EN_PERIODIC_EN                  14
#define N_ACPI_IO_SMI_EN_TCO_EN                       13
#define N_ACPI_IO_SMI_EN_MCSMI_EN                     11
#define N_ACPI_IO_SMI_EN_SWSMI_TMR_EN                 6
#define N_ACPI_IO_SMI_EN_APMC_EN                      5
#define N_ACPI_IO_SMI_EN_SMI_ON_SLP_EN                4
#define N_ACPI_IO_SMI_EN_LEGACY_USB_EN                3

#define R_ACPI_IO_SMI_STS                             0x34
#define S_ACPI_IO_SMI_STS                             4
#define B_ACPI_IO_SMI_STS_ESPI_SMI_STS                BIT28
#define B_ACPI_IO_SMI_STS_GPIO_UNLOCK_SMI_STS         BIT27                         ///< GPIO Unlock SMI Status
#define B_ACPI_IO_SMI_STS_SMBUS_SMI_STS               BIT16                         ///< SMBUS_SMI Status
#define B_ACPI_IO_SMI_STS_PERIODIC_STS                BIT14                         ///< Periodic Status
#define B_ACPI_IO_SMI_STS_TCO_STS                     BIT13                         ///< TCO Status
#define B_ACPI_IO_SMI_STS_MCSMI_STS                   BIT11                         ///< MCSMI Status
#define B_ACPI_IO_SMI_STS_PM1_STS                     BIT8                          ///< PM1 SMI Status
#define B_ACPI_IO_SMI_STS_SWSMI_TMR_STS               BIT6                          ///< Software SMI Timer Status
#define B_ACPI_IO_SMI_STS_APM_STS                     BIT5                          ///< APM Status
#define B_ACPI_IO_SMI_STS_SMI_ON_SLP_EN_STS           BIT4                          ///< SMI_ON_SLP_EN Status
#define B_ACPI_IO_SMI_STS_BIOS_STS                    BIT2                          ///< BIOS Status
#define N_ACPI_IO_SMI_STS_XHCI_SMI_STS                31
#define N_ACPI_IO_SMI_STS_ESPI_SMI_STS                28
#define N_ACPI_IO_SMI_STS_SPI_SMI_STS                 26
#define N_ACPI_IO_SMI_STS_MONITOR_STS                 21
#define N_ACPI_IO_SMI_STS_PCI_EXP_SMI_STS             20
#define N_ACPI_IO_SMI_STS_SMBUS_SMI_STS               16
#define N_ACPI_IO_SMI_STS_SERIRQ_SMI_STS              15
#define N_ACPI_IO_SMI_STS_PERIODIC_STS                14
#define N_ACPI_IO_SMI_STS_TCO_STS                     13
#define N_ACPI_IO_SMI_STS_MCSMI_STS                   11
#define N_ACPI_IO_SMI_STS_GPIO_SMI_STS                10
#define N_ACPI_IO_SMI_STS_GPE0_STS                    9
#define N_ACPI_IO_SMI_STS_PM1_STS_REG                 8
#define N_ACPI_IO_SMI_STS_SWSMI_TMR_STS               6
#define N_ACPI_IO_SMI_STS_APM_STS                     5
#define N_ACPI_IO_SMI_STS_SMI_ON_SLP_EN_STS           4
#define N_ACPI_IO_SMI_STS_LEGACY_USB_STS              3

#define R_ACPI_IO_GPE_CTRL                            0x40                              ///< General Purpose Event Control

#define R_ACPI_IO_OC_WDT_CTL                          0x54
#define B_ACPI_IO_OC_WDT_CTL_RLD                      BIT31
#define B_ACPI_IO_OC_WDT_CTL_EN                       BIT14
#define B_ACPI_IO_OC_WDT_CTL_TOV_MASK                 0x3FF
#define B_ACPI_IO_OC_WDT_CTL_FAILURE_STS              BIT23
#define B_ACPI_IO_OC_WDT_CTL_UNXP_RESET_STS           BIT22
#define B_ACPI_IO_OC_WDT_CTL_AFTER_POST               0x3F0000
#define V_ACPI_IO_OC_WDT_CTL_STATUS_FAILURE           1
#define V_ACPI_IO_OC_WDT_CTL_STATUS_OK                0

#define R_ACPI_IO_GPE0_STS_31_0                       0x60
#define R_ACPI_IO_GPE0_STS_127_96                     0x6C
#define S_ACPI_IO_GPE0_STS_127_96                     4
#define B_ACPI_IO_GPE0_STS_127_96_NAC_SX_WAKE_STS     BIT20
#define B_ACPI_IO_GPE0_STS_127_96_WADT_STS            BIT18                             ///< Wake Alarm Device Timer Status
#define B_ACPI_IO_GPE0_STS_127_96_USB_CON_DSX_STS     BIT17                             ///< USB Connection in/after DeepSx Status
#define B_ACPI_IO_GPE0_STS_127_96_LANWAKE_STS         BIT16                             ///< GPIO[27] Status
#define B_ACPI_IO_GPE0_STS_127_96_GPIO_TIER2_SCI_STS  BIT15                             ///< GPIO Tier2 SCI Status
#define B_ACPI_IO_GPE0_STS_127_96_ESPI_SCI_STS        BIT14                             ///< eSPI SCI Status
#define B_ACPI_IO_GPE0_STS_127_96_PME_B0_STS          BIT13                             ///< Power Management Event Bus 0 Status
#define B_ACPI_IO_GPE0_STS_127_96_PME_STS             BIT11                             ///< Power Management Event Status
#define B_ACPI_IO_GPE0_STS_127_96_BATLOW_STS          BIT10                             ///< Battery Low Status
#define B_ACPI_IO_GPE0_STS_127_96_PCI_EXP_STS         BIT9                              ///< PCI Express Status
#define B_ACPI_IO_GPE0_STS_127_96_ISH_STS             BIT8                              ///< ISH Status
#define B_ACPI_IO_GPE0_STS_127_96_SMB_WAK_STS         BIT7                              ///< SMBus Wake Status
#define B_ACPI_IO_GPE0_STS_127_96_TC_PME_B0_STS       BIT5                              ///< TCSS Power Management Event Bus 0 Status
#define B_ACPI_IO_GPE0_STS_127_96_SWGPE_STS           BIT2                              ///< Software GPE Status
#define B_ACPI_IO_GPE0_STS_127_96_HOT_PLUG_STS        BIT1                              ///< Hot Plug Status
#define N_ACPI_IO_GPE0_STS_127_96_PME_B0_STS          13
#define N_ACPI_IO_GPE0_STS_127_96_PME_STS             11
#define N_ACPI_IO_GPE0_STS_127_96_TC_PME_B0_STS       5

#define R_ACPI_IO_GPE0_EN_127_96                      0x7C
#define S_ACPI_IO_GPE0_EN_127_96                      4
#define B_ACPI_IO_GPE0_EN_127_96_TC_HOT_PLUG_EN       BIT23                             ///< TCSS Hot Plug Enable
#define B_ACPI_IO_GPE0_EN_127_96_TC_PCI_EXP_EN        BIT22                             ///< TC PCI Express Enable
#define B_ACPI_IO_GPE0_EN_127_96_NAC_SX_WAKE_EN       BIT20
#define B_ACPI_IO_GPE0_EN_127_96_TCSS_SX_WAKE_EN      BIT19                             ///< TCSS SX Wake Enable
#define B_ACPI_IO_GPE0_EN_127_96_WADT_EN              BIT18                             ///< Wake Alarm Device Timer Enable
#define B_ACPI_IO_GPE0_EN_127_96_USB_CON_DSX_EN       BIT17                             ///< USB Connection in/after DeepSx Enable
#define B_ACPI_IO_GPE0_EN_127_96_LANWAKE_EN           BIT16                             ///< GPIO[27] Enable
#define B_ACPI_IO_GPE0_EN_127_96_GPIO_TIER2_SCI_EN    BIT15                             ///< GPIO Tier2 SCI Enable
#define B_ACPI_IO_GPE0_EN_127_96_ESPI_SCI_EN          BIT14                             ///< eSPI SCI Enable
#define B_ACPI_IO_GPE0_EN_127_96_PME_B0_EN            BIT13                             ///< PME_B0 Enable
#define B_ACPI_IO_GPE0_EN_127_96_PME_EN               BIT11                             ///< Power Management Event Enable
#define B_ACPI_IO_GPE0_EN_127_96_PCI_EXP_EN           BIT9                              ///< PCI Express Enable
#define B_ACPI_IO_GPE0_EN_127_96_TC_PME_B0_EN         BIT5                              ///< TCSS PME_B0 Enable
#define N_ACPI_IO_GPE0_EN_127_96_PME_B0_EN            13
#define N_ACPI_IO_GPE0_EN_127_96_PME_EN               11
#define N_ACPI_IO_GPE0_EN_127_96_BATLOW_EN            10
#define N_ACPI_IO_GPE0_EN_127_96_PCI_EXP_EN           9
#define N_ACPI_IO_GPE0_EN_127_96_TCOSCI_EN            6
#define N_ACPI_IO_GPE0_EN_127_96_TC_PME_B0_EN         5
#define N_ACPI_IO_GPE0_EN_127_96_SWGPE_EN             2
#define N_ACPI_IO_GPE0_EN_127_96_HOT_PLUG_EN          1

#define R_TCO_IO_TCO2_STS                             0x06
#define B_TCO_IO_TCO2_STS_SECOND_TO                   BIT1

#define R_TCO_IO_TCO1_CNT                             0x08
#define B_TCO_IO_TCO1_CNT_TMR_HLT                     BIT11


//
// PWRM Registers for IPC interface
//
#define R_PMC_PWRM_IPC_CMD                                  0x00                        ///< IPC command
#define N_PMC_PWRM_IPC_CMD_CMD_ID                           12                          ///< IPC command.cmd.ID
#define N_PMC_PWRM_IPC_CMD_SIZE                             16                          ///< IPC command.size
#define N_PMC_PWRM_IPC_CMD_COMMAND                          0                           ///< IPC command.cmd.Command
#define V_PMC_PWRM_IPC_CMD_COMMAND_SLP_CTRL                 0xA1                        ///< IPC commmand to control S0ix policies
#define V_PMC_PWRM_IPC_CMD_COMMAND_SNDW_RCOMP               0xA2                        ///< IPC commmand to control platform Soundwire buffer RCOMP
#define V_PMC_PWRM_IPC_CMD_COMMAND_CRASHLOG                 0xA6                        ///< CrashLog Command
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_DISCOVERY        0x01                        ///< CrashLog Discovery SubCommand
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_DISABLE          0x02                        ///< Disable CrashLog SubCommand
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_ERASE            0x04                        ///< Clear CrashLog record SubCommand
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_ON_RESET         0x05                        ///< Collect on all resets SubCommand
#define V_PMC_PWRM_IPC_SRC_CLK_CMD                          0xA8                        ///< IPC to PMC SRCCLK command
#define R_PMC_PWRM_IPC_STS                                  0x04                        ///< IPC Status
#define B_PMC_PWRM_IPC_STS_BUSY                             BIT0                        ///< IPC Status Busy Bit
#define B_PMC_PWRM_IPC_STS_ERROR                            BIT1                        ///< IPC Status Error Bit
#define R_PMC_PWRM_IPC_WBUF0                                0x80                        ///< IPC Write Buffer
#define R_PMC_PWRM_IPC_WBUF1                                0x84                        ///< IPC Write Buffer
#define R_PMC_PWRM_IPC_WBUF2                                0x88                        ///< IPC Write Buffer
#define R_PMC_PWRM_IPC_WBUF3                                0x8C                        ///< IPC Write Buffer
#define R_PMC_PWRM_IPC_RBUF0                                0x90                        ///< IPC Read Buffer
#define R_PMC_PWRM_IPC_RBUF1                                0x94                        ///< IPC Read Buffer
#define R_PMC_PWRM_IPC_RBUF2                                0x98                        ///< IPC Read Buffer
#define R_PMC_PWRM_IPC_RBUF3                                0x9C                        ///< IPC Read Buffer


//
// PWRM Registers
//
#define R_PMC_PWRM_GEN_PMCON_A                              0x1020                        ///< in CNL located in PWRM
#define B_PMC_PWRM_GEN_PMCON_A_DC_PP_DIS                    BIT30
#define B_PMC_PWRM_GEN_PMCON_A_DSX_PP_DIS                   BIT29
#define B_PMC_PWRM_GEN_PMCON_A_AG3_PP_EN                    BIT28
#define B_PMC_PWRM_GEN_PMCON_A_SX_PP_EN                     BIT27
#define B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS                  BIT24
#define B_PMC_PWRM_GEN_PMCON_A_DISB                         BIT23
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_OPI_PLL_SD_INC0        BIT22
#define B_PMC_PWRM_GEN_PMCON_A_MEM_SR                       BIT21
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_SPXB_CG_INC0           BIT20
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_L1LOW_C0               BIT19
#define B_PMC_PWRM_GEN_PMCON_A_MS4V                         BIT18
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_L1LOW_OPI_ON           BIT17
#define B_PMC_PWRM_GEN_PMCON_A_SUS_PWR_FLR                  BIT16
#define B_PMC_PWRM_GEN_PMCON_A_PME_B0_S5_DIS                BIT15
#define B_PMC_PWRM_GEN_PMCON_A_PWR_FLR                      BIT14
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_L1LOW_BCLKREQ_ON       BIT13
#define B_PMC_PWRM_GEN_PMCON_A_DISABLE_SX_STRETCH           BIT12
#define B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS                 BIT9
#define B_PMC_PWRM_GEN_PMCON_A_ESPI_SMI_LOCK                BIT8
#define B_PMC_PWRM_GEN_PMCON_A_SLP_S4_ASE                   BIT3
#define B_PMC_PWRM_GEN_PMCON_A_AFTERG3_EN                   BIT0
#define B_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW                   0xC00
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_60US              0x000
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_1MS               0x400
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_50MS              0x800
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_2S                0xC00
#define B_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL                   0xC0
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_64MS              0xC0
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_32MS              0x80
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_16MS              0x40
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_1_5MS             0x00
#define B_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW                   0x30
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_1S                0x30
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_2S                0x20
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_3S                0x10
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_4S                0x00
#define B_PMC_PWRM_GEN_PMCON_A_PER_SMI_SEL                  0x6
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_64S                  0x0000
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_32S                  0x0002
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_16S                  0x0004
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_8S                   0x0006

#define R_PMC_PWRM_GEN_PMCON_B                              0x1024
#define B_PMC_PWRM_GEN_PMCON_B_SLPSX_STR_POL_LOCK           BIT18            ///< Lock down SLP_S3/SLP_S4 Minimum Assertion width
#define B_PMC_PWRM_GEN_PMCON_B_WOL_EN_OVRD                  BIT13
#define B_PMC_PWRM_GEN_PMCON_B_BIOS_PCI_EXP_EN              BIT10
#define B_PMC_PWRM_GEN_PMCON_B_PWRBTN_LVL                   BIT9
#define B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK                     BIT4
#define B_PMC_PWRM_GEN_PMCON_B_RTC_PWR_STS                  BIT2

#define R_PMC_PWRM_ETR3                                     0x1048          ///< in CNL this is PWRM register
#define B_PMC_PWRM_ETR3_CF9GR                               BIT20           ///< CF9h Global Reset

#define R_PMC_PWRM_TGPIOCTL0                                0x1210    ///< Timed GPIO Control 0
#define R_PMC_PWRM_TGPIOCTL1                                0x1310    ///< Timed GPIO Control 1

#define R_PMC_PWRM_TSDTR_THRESH                             0x1514                    ///< Thermal Sensor Dynamic Temperature Range PCIe Threshold

#define R_PMC_PWRM_MIN_TEMP                                 0x1500                    ///< Min Temperature

#define R_PMC_PWRM_WADT_AC                                  0x1800

#define R_PMC_PWRM_PM_CFG4                                  0x18E8            ///< Power Management Configuration Reg 4

#define R_PMC_PWRM_PS_ON_RESIDENCY                          0x18F8

#define R_PMC_PWRM_SLP_S0_RESIDENCY                         0x193C

#define R_PMC_PWRM_LTR_IGN                                  0x1B0C            ///< LTR Ignore
#define R_PMC_PWRM_CPPMVRIC                                 0x1B1C            ///< CPPM VR Idle Control 1

#define R_PMC_PWRM_CPPMFALIGNCTL_1                          0x1BD0            ///< CPPM Forced Alignment Control 1

#define R_PMC_PWRM_ST_PG_FDIS_PMC_1                         0x1E20 ///< Static PG Related Function Disable Register 1

//
// GPE interrupts definitons
//
#define GPE_TC_PME_B0                                   0x65
#define GPE_PCI_EXP                                     0x69
#define GPE_PME                                         0x6B
#define GPE_WADT_EN                                     0x72
#define GPE_PME_B0                                      0x6D
#define GPE_ESPI_EN                                     0x6E
#define GPE_TC_PCI_EXP                                  0x76
#define GPE1_IOE_PME_B0                                 0x80
#define GPE1_CNVI_PME_B0                                0x85
#define GPE1_GBE_PME_B0                                 0x86
#define GPE1_XHCI_PME_B0                                0x89
#define GPE1_ACE_PME_B0                                 0x8A
#define GPE1_XDCI_PME_B0                                0x8C
#define GPE1_LPSS_PME_B0_EN                             0x8E
#define GPE1_TC_TBT0_PME_B0_EN                          0x8F
#define GPE1_TC_TBT1_PME_B0_EN                          0x90
#define GPE1_TC_IOM_PME_B0_EN                           0x91

#define GPE1_CNVI_BT_PME_B0                             0x92
#define GPE1_PCI_HOTPLUG_SPA                            0xA0
#define GPE1_PCI_HOTPLUG_SPB                            0xA1
#define GPE1_PCI_HOTPLUG_SPC                            0xA2
#define GPE1_PCI_HOTPLUG_SPD                            0xA3
#define GPE1_IOE_HOTPLUG                                0xA7
#define GPE1_PCI_EXP_SPA                                0xC0
#define GPE1_PCI_EXP_SPB                                0xC1
#define GPE1_PCI_EXP_SPC                                0xC2
#define GPE1_PCI_EXP_SPD                                0xC3
#define GPE1_IOE_PCI_EXP                                0xC7
#define GPE1_TC_PCI0_PCI_EXP                            0xC8
#define GPE1_TC_PCI1_PCI_EXP                            0xC9
#define GPE1_TC_PCI2_PCI_EXP                            0xCA
#define GPE1_TC_PCI3_PCI_EXP                            0xCB

#define GPE1_PG5_PMA0_PCI_EXP_EN_0                      0xDC
#define GPE1_PG5_PMA1_PCI_EXP_EN_0                      0xD8
#define GPE1_PG5_PMA1_PCI_EXP_EN_1                      0xD9

#endif
