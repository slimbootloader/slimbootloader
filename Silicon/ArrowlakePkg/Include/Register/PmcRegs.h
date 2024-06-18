/** @file
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

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_PMC_H_
#define _PCH_REGS_PMC_H_

#define R_PMC_PWRM_TGPIOCTL0                     0x1210                                 ///< Timed GPIO Control 0
#define R_PMC_PWRM_TGPIOCTL1                     0x1310                                 ///< Timed GPIO Control 1

//
// PCIe config space registers
//
#define R_PMC_CFG_BAR                            0x10                                   ///< 32-bit Base Address Register
#define B_PMC_CFG_PWRM_BASE_MASK                 0xFFFF0000                             ///< PWRM must be 64KB alignment to align the source decode.

#define R_PMC_CFG_ACTL                           0x44                                   ///< In IBL the register has been moved from MMIO to PCIe config space
#define B_PMC_CFG_ACTL_SCIS                      (BIT2 | BIT1 | BIT0)                   ///< SCI IRQ select

//
// ACPI and legacy I/O register offsets from ACPIBASE
//
#define R_ACPI_IO_PM1_STS                        0x00
#define S_ACPI_IO_PM1_STS                        2
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
#define B_ACPI_IO_PM1_EN_PCIEXP_WAKE             BIT14
#define B_ACPI_IO_PM1_EN_RTC                     BIT10
#define B_ACPI_IO_PM1_EN_PWRBTN_GBLRST_DIS       BIT9
#define B_ACPI_IO_PM1_EN_PWRBTN                  BIT8
#define N_ACPI_IO_PM1_EN_RTC                     10
#define N_ACPI_IO_PM1_EN_PWRBTN                  8
#define N_ACPI_IO_PM1_EN_TMROF                   0

#define R_ACPI_IO_PM1_CNT                        0x04                                   ///< Power Management 1 Control
#define B_ACPI_IO_PM1_CNT_SLP_EN                 BIT13                                  ///< Sleep Enable
#define B_ACPI_IO_PM1_CNT_SLP_TYP                (BIT12 | BIT11 | BIT10)                ///< Sleep Type
#define V_ACPI_IO_PM1_CNT_S0                     0
#define V_ACPI_IO_PM1_CNT_S1                     BIT10
#define V_ACPI_IO_PM1_CNT_S3                     (BIT12 | BIT10)
#define V_ACPI_IO_PM1_CNT_S4                     (BIT12 | BIT11)
#define V_ACPI_IO_PM1_CNT_S5                     (BIT12 | BIT11 | BIT10)
#define B_ACPI_IO_PM1_CNT_SCI_EN                 BIT0                                   ///< SCI Enable

#define R_ACPI_IO_PM1_TMR                        0x08                                   ///< Power Management 1 Timer
#define B_ACPI_IO_PM1_TMR_TMR_VAL                0xFFFFFF                               ///< Timer Value
#define V_ACPI_IO_PM1_TMR_MAX_VAL                0x1000000                              ///< The timer is 24 bit overflow

#define R_ACPI_IO_THERM_TIMER_DELAY              0x20                                   ///< Therm Timer Delay Register

#define R_ACPI_IO_SMI_EN                                  0x30                          ///< SMI Control and Enable
#define S_ACPI_IO_SMI_EN                                  4
#define B_ACPI_IO_SMI_EN_ESPI_SMI_EN                      BIT28                         ///< eSPI SMI Enable
#define B_ACPI_IO_SMI_EN_THERM_SMI_EN                     BIT18                         ///< THERM SMI Enable
#define B_ACPI_IO_SMI_EN_LEGACY_USB2_EN                   BIT17                         ///< Legacy USB 2 Enable
#define B_ACPI_IO_SMI_EN_PERIODIC_EN                      BIT14                         ///< Periodic Enable
#define B_ACPI_IO_SMI_EN_TCO_EN                           BIT13                         ///< TCO Enable
#define B_ACPI_IO_SMI_EN_BIOS_RLS                         BIT7                          ///< BIOS_RLS
#define B_ACPI_IO_SMI_EN_SWSMI_TMR_EN                     BIT6                          ///< Software SMI Timer Enable
#define B_ACPI_IO_SMI_EN_APMC_EN                          BIT5                          ///< APMC Enable
#define B_ACPI_IO_SMI_EN_SMI_ON_SLP_EN                    BIT4                          ///< SMI On Sleep Enable
#define B_ACPI_IO_SMI_EN_LEGACY_USB_EN                    BIT3                          ///< Legacy USB Enable
#define B_ACPI_IO_SMI_EN_BIOS_EN                          BIT2                          ///< BIOS Enable
#define B_ACPI_IO_SMI_EN_EOS                              BIT1                          ///< End of SMI
#define B_ACPI_IO_SMI_EN_GBL_SMI_EN                       BIT0                          ///< Global SMI Enable
#define N_ACPI_IO_SMI_EN_XHCI_SMI_EN                      31
#define N_ACPI_IO_SMI_EN_ESPI_SMI_EN                      28
#define N_ACPI_IO_SMI_EN_PERIODIC_EN                      14
#define N_ACPI_IO_SMI_EN_TCO_EN                           13
#define N_ACPI_IO_SMI_EN_MCSMI_EN                         11
#define N_ACPI_IO_SMI_EN_SWSMI_TMR_EN                     6
#define N_ACPI_IO_SMI_EN_APMC_EN                          5
#define N_ACPI_IO_SMI_EN_SMI_ON_SLP_EN                    4
#define N_ACPI_IO_SMI_EN_LEGACY_USB_EN                    3

#define R_ACPI_IO_SMI_STS                                 0x34                          ///< SMI Status Register
#define S_ACPI_IO_SMI_STS                                 4
#define B_ACPI_IO_SMI_STS_ESPI_STS                        BIT28
#define B_ACPI_IO_SMI_STS_GPIO_UNLOCK_SMI_STS             BIT27                         ///< GPIO Unlock SMI Status
#define B_ACPI_IO_SMI_STS_SMBUS_SMI_STS                   BIT16                         ///< SMBUS_SMI Status
#define B_ACPI_IO_SMI_STS_PERIODIC_STS                    BIT14                         ///< Periodic Status
#define B_ACPI_IO_SMI_STS_TCO_STS                         BIT13                         ///< TCO Status
#define B_ACPI_IO_SMI_STS_MCSMI_STS                       BIT11                         ///< MCSMI Status
#define B_ACPI_IO_SMI_STS_PM1_STS                         BIT8                          ///< PM1 SMI Status
#define B_ACPI_IO_SMI_STS_SWSMI_TMR_STS                   BIT6                          ///< Software SMI Timer Status
#define B_ACPI_IO_SMI_STS_APM_STS                         BIT5                          ///< APM Status
#define B_ACPI_IO_SMI_STS_SMI_ON_SLP_EN_STS               BIT4                          ///< SMI_ON_SLP_EN Status
#define B_ACPI_IO_SMI_STS_BIOS_STS                        BIT2                          ///< BIOS Status
#define N_ACPI_IO_SMI_STS_XHCI_SMI_STS                    31
#define N_ACPI_IO_SMI_STS_ESPI_SMI_STS                    28
#define N_ACPI_IO_SMI_STS_SPI_SMI_STS                     26
#define N_ACPI_IO_SMI_STS_MONITOR_STS                     21
#define N_ACPI_IO_SMI_STS_PCI_EXP_SMI_STS                 20
#define N_ACPI_IO_SMI_STS_SMBUS_SMI_STS                   16
#define N_ACPI_IO_SMI_STS_SERIRQ_SMI_STS                  15
#define N_ACPI_IO_SMI_STS_PERIODIC_STS                    14
#define N_ACPI_IO_SMI_STS_TCO_STS                         13
#define N_ACPI_IO_SMI_STS_MCSMI_STS                       11
#define N_ACPI_IO_SMI_STS_GPIO_SMI_STS                    10
#define N_ACPI_IO_SMI_STS_GPE0_STS                        9
#define N_ACPI_IO_SMI_STS_PM1_STS_REG                     8
#define N_ACPI_IO_SMI_STS_SWSMI_TMR_STS                   6
#define N_ACPI_IO_SMI_STS_APM_STS                         5
#define N_ACPI_IO_SMI_STS_SMI_ON_SLP_EN_STS               4
#define N_ACPI_IO_SMI_STS_LEGACY_USB_STS                  3

#define R_ACPI_IO_GPE_CTRL                            0x40                              ///< General Purpose Event Control
#define B_ACPI_IO_GPE_CTRL_SWGPE_CTRL                 BIT17                             ///< Software GPE Control

#define R_ACPI_IO_DEVTRAP_STS                         0x44                              ///< Device Trap Status Register
#define B_ACPI_IO_DEVTRAP_STS_D12_TRP_STS             BIT12                             ///< D12 Trap Status
#define B_ACPI_IO_DEVTRAP_STS_D9_TRP_STS              BIT9                              ///< D9 Trap Status
#define B_ACPI_IO_DEVTRAP_STS_D8_TRP_STS              BIT8                              ///< D8 Trap Status
#define B_ACPI_IO_DEVTRAP_STS_D7_TRP_STS              BIT7                              ///< D7 Trap Status
#define B_ACPI_IO_DEVTRAP_STS_D6_TRP_STS              BIT6                              ///< D6 Trap Status

#define R_ACPI_IO_OC_WDT_CTL                          0x54                              ///< Over-Clocking WDT Control
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_RLD               BIT31                             ///< Over-Clocking WDT Reload
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_ICCSURV_STS       BIT25                             ///< Over-Clocking WDT ICC Survivability Impact
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_NO_ICCSURV_STS    BIT24                             ///< Over-Clocking WDT Non-ICC Survivability Mode Timeout Status
#define B_ACPI_IO_OC_WDT_CTL_FAILURE_STS              BIT23
#define B_ACPI_IO_OC_WDT_CTL_UNXP_RESET_STS           BIT22
#define B_ACPI_IO_OC_WDT_CTL_ALLOW_UNXP_RESET_STS     BIT21
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_FORCE_ALL         BIT15                             ///< Over-Clocking WDT Force All
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_EN                BIT14                             ///< Over-Clocking WDT Enable
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_ICCSURV           BIT13                             ///< Over-Clocking WDT ICC Survivability Impact
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_CTL_LCK           BIT12                             ///< OC_WDT_CTL Register Lock
#define B_ACPI_IO_OC_WDT_CTL_OC_WDT_TOV               0x3FF                             ///< Over-Clocking WDT Timeout Value
#define B_ACPI_IO_OC_WDT_CTL_AFTER_POST               0x3F0000
#define V_ACPI_IO_OC_WDT_CTL_STATUS_FAILURE           1
#define V_ACPI_IO_OC_WDT_CTL_STATUS_OK                0

#define R_ACPI_IO_GPE0_STS_31_0                         0x60                            ///< General Purpose Event 0 Status [31:0]
#define R_ACPI_IO_GPE0_STS_127_96                       0x6C                            ///< General Purpose Event 0 Status [127:96]
#define S_ACPI_IO_GPE0_STS_127_96                       4
#define B_ACPI_IO_GPE0_STS_127_96_WADT_STS              BIT18                           ///< Wake Alarm Device Timer Status
#define B_ACPI_IO_GPE0_STS_127_96_USB_CON_DSX_STS       BIT17                           ///< USB Connection in/after DeepSx Status
#define B_ACPI_IO_GPE0_STS_127_96_LANWAKE_STS           BIT16                           ///< GPIO[27] Status
#define B_ACPI_IO_GPE0_STS_127_96_GPIO_TIER2_SCI_STS    BIT15                           ///< GPIO Tier2 SCI Status
#define B_ACPI_IO_GPE0_STS_127_96_ESPI_SCI_STS          BIT14                           ///< eSPI SCI Status
#define B_ACPI_IO_GPE0_STS_127_96_PME_B0_STS            BIT13                           ///< Power Management Event Bus 0 Status
#define B_ACPI_IO_GPE0_STS_127_96_PME_STS               BIT11                           ///< Power Management Event Status
#define B_ACPI_IO_GPE0_STS_127_96_BATLOW_STS            BIT10                           ///< Battery Low Status
#define B_ACPI_IO_GPE0_STS_127_96_PCI_EXP_STS           BIT9                            ///< PCI Express Status
#define B_ACPI_IO_GPE0_STS_127_96_ISH_STS               BIT8                            ///< ISH Status
#define B_ACPI_IO_GPE0_STS_127_96_SMB_WAK_STS           BIT7                            ///< SMBus Wake Status
#define B_ACPI_IO_GPE0_STS_127_96_SWGPE_STS             BIT2                            ///< Software GPE Status
#define B_ACPI_IO_GPE0_STS_127_96_HOT_PLUG_STS          BIT1                            ///< Hot Plug Status
#define N_ACPI_IO_GPE0_STS_127_96_PME_B0_STS            13
#define N_ACPI_IO_GPE0_STS_127_96_PME_STS               11

#define R_ACPI_IO_GPE0_EN_127_96                      0x7C                              ///< General Purpose Event 0 Enable [127:96]
#define S_ACPI_IO_GPE0_EN_127_96                      4
#define B_ACPI_IO_GPE0_EN_127_96_WADT_EN              BIT18                             ///< Wake Alarm Device Timer Enable
#define B_ACPI_IO_GPE0_EN_127_96_USB_CON_DSX_EN       BIT17                             ///< USB Connection in/after DeepSx Enable
#define B_ACPI_IO_GPE0_EN_127_96_LANWAKE_EN           BIT16                             ///< GPIO[27] Enable
#define B_ACPI_IO_GPE0_EN_127_96_GPIO_TIER_2          BIT15                             ///< GPIO Tier2 SCI Enable
#define B_ACPI_IO_GPE0_EN_127_96_ESPI_SCI_EN          BIT14                             ///< eSPI SCI Enable
#define B_ACPI_IO_GPE0_EN_127_96_PME_B0_EN            BIT13                             ///< PME_B0 Enable
#define B_ACPI_IO_GPE0_EN_127_96_PME_EN               BIT11                             ///< Power Management Event Enable
#define B_ACPI_IO_GPE0_EN_127_96_PCI_EXP_EN           BIT9                              ///< PCI Express Enable
#define N_ACPI_IO_GPE0_EN_127_96_PME_B0_EN            13
#define N_ACPI_IO_GPE0_EN_127_96_PME_EN               11
#define N_ACPI_IO_GPE0_EN_127_96_BATLOW_EN            10
#define N_ACPI_IO_GPE0_EN_127_96_PCI_EXP_EN           9
#define N_ACPI_IO_GPE0_EN_127_96_TCOSCI_EN            6
#define N_ACPI_IO_GPE0_EN_127_96_SWGPE_EN             2
#define N_ACPI_IO_GPE0_EN_127_96_HOT_PLUG_EN          1

#define B_ACPI_IO_GPE1_STS_31_0_LPSS_PME_B0_STS    BIT14
#define B_ACPI_IO_GPE1_STS_31_0_CSE_PME_B0_STS     BIT13
#define B_ACPI_IO_GPE1_STS_31_0_XDCI_PME_B0_STS    BIT12
#define B_ACPI_IO_GPE1_STS_31_0_ACE_PME_B0_STS     BIT10
#define B_ACPI_IO_GPE1_STS_31_0_XHCI_PME_B0_STS    BIT9
#define B_ACPI_IO_GPE1_STS_31_0_SATA_PME_B0_STS    BIT8
#define B_ACPI_IO_GPE1_STS_31_0_CSME_PME_B0_STS    BIT7
#define B_ACPI_IO_GPE1_STS_31_0_GBE_PME_B0_STS     BIT6
#define B_ACPI_IO_GPE1_STS_31_0_CNVI_PME_B0_STS    BIT5
#define B_ACPI_IO_GPE1_STS_31_0_TBTLSX_PME_B0_STS  BIT1

#define B_ACPI_IO_GPE1_STS_63_32_IOE_HOT_PLUG_STS  BIT7
#define B_ACPI_IO_GPE1_STS_63_32_SPF_HOT_PLUG_STS  BIT5
#define B_ACPI_IO_GPE1_STS_63_32_SPE_HOT_PLUG_STS  BIT4
#define B_ACPI_IO_GPE1_STS_63_32_SPD_HOT_PLUG_STS  BIT3
#define B_ACPI_IO_GPE1_STS_63_32_SPC_HOT_PLUG_STS  BIT2
#define B_ACPI_IO_GPE1_STS_63_32_SPB_HOT_PLUG_STS  BIT1
#define B_ACPI_IO_GPE1_STS_63_32_SPA_HOT_PLUG_STS  BIT0

#define B_ACPI_IO_GPE1_STS_95_64_IOE_PCI_EXP_STS   BIT7
#define B_ACPI_IO_GPE1_STS_95_64_SPF_PCI_EXP_STS   BIT5
#define B_ACPI_IO_GPE1_STS_95_64_SPE_PCI_EXP_STS   BIT4
#define B_ACPI_IO_GPE1_STS_95_64_SPD_PCI_EXP_STS   BIT3
#define B_ACPI_IO_GPE1_STS_95_64_SPC_PCI_EXP_STS   BIT2
#define B_ACPI_IO_GPE1_STS_95_64_SPB_PCI_EXP_STS   BIT1
#define B_ACPI_IO_GPE1_STS_95_64_SPA_PCI_EXP_STS   BIT0

//
// PWRM Registers for IPC interface
//
#define R_PMC_PWRM_IPC_CMD                                  0x00                        ///< IPC Command
#define N_PMC_PWRM_IPC_CMD_CMD_ID                           12                          ///< IPC command.cmd.ID
#define N_PMC_PWRM_IPC_CMD_SIZE                             16                          ///< IPC command.size
#define N_PMC_PWRM_IPC_CMD_COMMAND                          0                           ///< IPC command.cmd.Command
#define V_PMC_PWRM_IPC_CMD_COMMAND_GEN_COMM                 0xA0                        ///< IPC command for General PMC Communication
#define V_PMC_PWRM_IPC_SUBCMD_GEN_COMM_READ                 2                           ///< IPC subcommand to Read PMC address space using WBUF0
#define V_PMC_PWRM_IPC_CMD_COMMAND_SLP_CTRL                 0xA1                        ///< IPC commmand to control S0ix policies
#define V_PMC_PWRM_IPC_CMD_COMMAND_DFX_OVERRIDES            0xA2                        ///< IPC commmand to control DFX overrides
#define V_PMC_PWRM_IPC_CMD_COMMAND_FIVR                     0xA3                        ///< IPC commmand to control FIVR Configuration
#define V_PMC_PWRM_IPC_SUBCMD_RFI_CTRL0_LOGIC               0                           ///< IPC subcommand to control RFI Control 0 register logic write
#define V_PMC_PWRM_IPC_SUBCMD_RFI_CTRL4_LOGIC               1                           ///< IPC subcommand to control RFI Control 4 register logic write
#define V_PMC_PWRM_IPC_SUBCMD_EMI_CTRL0_LOGIC               2                           ///< IPC subcommand to control EMI Control 0 register logic write
#define V_PMC_PWRM_IPC_SUBCMD_FFFC_FAULT_STATUS             3                           ///< IPC subcommand to control FFFC_FAULT_STATUS register logic read
#define V_PMC_PWRM_IPC_SUBCMD_FFFC_RFI_STATUS               4                           ///< IPC subcommand to control FFFC_RFI_STATUS register logic read
#define V_PMC_PWRM_IPC_CMD_CMD_ID_FIVR_READ                 0x00                        ///< IPC subcommand to read FIVR Register
#define V_PMC_PWRM_IPC_CMD_CMD_ID_FIVR_WRITE                0x01                        ///< IPC subcommand to write FIVR Register and fuse SRAM/DCCM
#define V_PMC_PWRM_IPC_CMD_COMMAND_DFX_PWR_STATE            0xA4                        ///< IPC commmand to control DFX Power State
#define V_PMC_PWRM_IPC_CMD_COMMAND_CRASHLOG                 0xA6                        ///< CrashLog Command
#define V_PMC_PWRM_IPC_CMD_COMMAND_ME_DISABLE               0xA9                        ///< IPC command to disable ME devices
#define V_PMC_PWRM_IPC_CMD_COMMAND_LTR_OVERIDE              0xAA                        ///< LTR Override
#define V_PMC_PWRM_IPC_CMD_COMMAND_PMC_PRV_REG_ACCESS       0xAB                        ///< IPC command from proxy access to PMC private registers
#define V_PMC_PWRM_ID_GBLRST_TRIG_MASK0                     0x0
#define V_PMC_PWRM_ID_GBLRST_EVENT_MASK0                    0x1
#define V_PMC_PWRM_ID_GBLRST_MAX                            0x2
#define V_PMC_PWRM_IPC_CMD_COMMAND_SSC_STATE                0xAE                        ///< IPC command to control SA SSC enable/disable
#define V_PMC_PWRM_IPC_CMD_COMMAND_GENERIC_WORKAROUND       0xAF                        ///< IPC Generic Workaround command
#define V_PMC_PWRM_IPC_CMD_CMD_GENERIC_WORKAROUND_TIME_SYNC_CONTROL 0x1                 ///< PMC time sync control
#define V_PMC_PWRM_IPC_CMD_CMD_GENERIC_WORKAROUND_TIME_SYNC_ENABLE  0x1                 ///< PMC time sync enable
#define V_PMC_PWRM_IPC_CMD_SUBCMD_IO_MARGINING_ENABLE       0x00                        ///< enable I/O Margin tool support
#define V_PMC_PWRM_IPC_CMD_SUBCMD_S3M_ROM_WA_ADD_TIMEOUTS   0x02                        ///< The S3M ROM workaround is to add fixed timeouts of 15 seconds to two of the early handshakes between the CPU and PMC
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_DISCOVERY        0x01                        ///< CrashLog Discovery SubCommand
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_DISABLE          0x02                        ///< Disable CrashLog SubCommand
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_ERASE            0x04                        ///< Clear CrashLog record SubCommand
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_ON_RESET         0x05                        ///< Collect on all resets SubCommand
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_RE_ARM           0x06                        ///< Re-arm SubCommand to ensure that a trigger command only executed on time per boot
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_PROGRAM_TRIGGER_ENABLES  0x8                 ///< Program global reset trigger enable mask
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CRASHLOG_READ_TRIGGER_ENABLES     0x9                 ///< Read global reset trigger enable mask
#define V_PMC_PWRM_IPC_SRC_CLK_CMD                          0xA8                        ///< IPC to PMC SRCCLK command
#define V_PMC_PWRM_IPC_SRC_CLK_PORT_MAPPING_CMD             0xAC                        ///< IPC to PMC SRCCLK and port mapping command
#define V_PMC_PWRM_IPC_CMD_WBUF0_PROXY_NMI                     2                        ///< parameter to access NMI control register
#define V_PMC_PWRM_IPC_CMD_COMMAND_CPU_STRAP                0xB1                        ///< IPC command for CPU straps
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CPU_STRAP_LOCK               0                        ///< IPC subcommand to lock CPU straps
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CPU_STRAP_WRITE_DW1_4        1                        ///< IPC subcommand to write CPU straps DW1-4
#define V_PMC_PWRM_IPC_CMD_CMD_ID_CPU_STRAP_WRITE_DW5_8        3                        ///< IPC subcommand to write CPU straps DW5-8
#define V_PMC_PWRM_IPC_CMD_CMD_BIOS_RST_CMPL_STS            0xD0                        ///< IPC command for Bios Reset Completion Status
#define V_PMC_PWRM_IPC_CMD_CMD_BIOS_RST_CMPL_STS_PCI_ENUM   BIT0                        ///< IPC command for Bios Reset Completion Status Pci Enumeration Done
#define V_PMC_PWRM_IPC_CMD_CMD_BIOS_RST_CMPL_STS_MEM_CAL    BIT1                        ///< IPC command for Bios Reset Completion Status Memory Calibration Done
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
// PMC SSRAM Registers
//
#define R_PMC_MEM_SSRAM_OEM_HSIO_VERSION_BUILD              0x30                        ///< OEM_CHIPSETINIT_VERSION_1 Build
#define R_PMC_MEM_SSRAM_OEM_HSIO_VERSION_HOT_FIX            0x32                        ///< OEM_CHIPSETINIT_VERSION_1 HotFix
#define R_PMC_MEM_SSRAM_OEM_HSIO_VERSION_MINOR              0x34                        ///< OEM_CHIPSETINIT_VERSION_2 Minor Version
#define R_PMC_MEM_SSRAM_OEM_HSIO_VERSION_MAJOR              0x36                        ///< OEM_CHIPSETINIT_VERSION_2 Major Version
#define R_PMC_MEM_SSRAM_INTEL_HSIO_VERSION_BUILD            0x48                        ///< INTEL_CHIPSETINIT_VERSION_1 Build
#define R_PMC_MEM_SSRAM_INTEL_HSIO_VERSION_HOT_FIX          0x4A                        ///< INTEL_CHIPSETINIT_VERSION_1 HotFix
#define R_PMC_MEM_SSRAM_INTEL_HSIO_VERSION_MINOR            0x4C                        ///< INTEL_CHIPSETINIT_VERSION_2 Minor Version
#define R_PMC_MEM_SSRAM_INTEL_HSIO_VERSION_MAJOR            0x4E                        ///< INTEL_CHIPSETINIT_VERSION_2 Major Version

//
// PWRM Registers
//
#define R_PMC_PWRM_GEN_PMCON_A                              0x1020                      ///< General PM Configuration A
#define B_PMC_PWRM_GEN_PMCON_A_DC_PP_DIS                    BIT30                       ///< DC PHY Power Disable
#define B_PMC_PWRM_GEN_PMCON_A_DSX_PP_DIS                   BIT29                       ///< Deep-Sx PHY Power Disable
#define B_PMC_PWRM_GEN_PMCON_A_AG3_PP_EN                    BIT28                       ///< After G3 PHY Power Enable
#define B_PMC_PWRM_GEN_PMCON_A_SX_PP_EN                     BIT27                       ///< Sx PHY Power Enable
#define B_PMC_PWRM_GEN_PMCON_A_GBL_RST_STS                  BIT24                       ///< Global Reset Status
#define B_PMC_PWRM_GEN_PMCON_A_DISB                         BIT23                       ///< DRAM Initialization Scratchpad Bit
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_OPI_PLL_SD_INC0        BIT22                       ///< Allow OPI PLL Shutdown in C0
#define B_PMC_PWRM_GEN_PMCON_A_MEM_SR                       BIT21                       ///< Memory Placed in Self-Refresh
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_SPXB_CG_INC0           BIT20                       ///< Allow SPXB Clock Gating in C0
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_L1LOW_C0               BIT19                       ///< Allow L1.LOW Entry During C0
#define B_PMC_PWRM_GEN_PMCON_A_MS4V                         BIT18                       ///< Minimum SLP_S4# Assertion Width Violation Status
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_L1LOW_OPI_ON           BIT17                       ///< Allow L1.LOW Entry with OPI Voltage On
#define B_PMC_PWRM_GEN_PMCON_A_SUS_PWR_FLR                  BIT16                       ///< SUS Well Power Failure
#define B_PMC_PWRM_GEN_PMCON_A_PME_B0_S5_DIS                BIT15                       ///< PME B0 S5 Disable
#define B_PMC_PWRM_GEN_PMCON_A_PF                           BIT14                       ///< PWR_FLR
#define B_PMC_PWRM_GEN_PMCON_A_ALLOW_L1LOW_BCLKREQ_ON       BIT13                       ///< Allow L1.LOW Entry with CPU BCLK REQ Asserted
#define B_PMC_PWRM_GEN_PMCON_A_DIS_SLP_X_STRCH_SUS_UP       BIT12                       ///< Disable SLP_X Stretching After SUS Well Power Up
#define B_PMC_PWRM_GEN_PMCON_A_HOST_RST_STS                 BIT9                        ///< Host Reset Status
#define B_PMC_PWRM_GEN_PMCON_A_ESPI_SMI_LOCK                BIT8                        ///< ESPI SMI Lock
#define B_PMC_PWRM_GEN_PMCON_A_S4ASE                        BIT3                        ///< SLP_S4# Assertion Stretch Enable
#define B_PMC_PWRM_GEN_PMCON_A_AG3E                         BIT0                        ///< AFTERG3_EN
#define B_PMC_PWRM_GEN_PMCON_A_SLP_S3_MIN_ASST_WDTH         (BIT11|BIT10)               ///< SLP_S3# Minimum Assertion Width
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_60US              0x000
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_1MS               0x400
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_50MS              0x800
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S3_MAW_2S                0xC00
#define B_PMC_PWRM_GEN_PMCON_A_SWSMI_RATESEL                (BIT7|BIT6)                 ///< SWSMI Rate Select
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_64MS              0xC0
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_32MS              0x80
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_16MS              0x40
#define V_PMC_PWRM_GEN_PMCON_A_SWSMI_RTSL_1_5MS             0x00
#define B_PMC_PWRM_GEN_PMCON_A_S4MAW                        (BIT5|BIT4)                 ///< SLP_S4# Minimum Assertion Width
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_1S                0x30
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_2S                0x20
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_3S                0x10
#define V_PMC_PWRM_GEN_PMCON_A_SLP_S4_MAW_4S                0x00
#define B_PMC_PWRM_GEN_PMCON_A_PER_SMI_SEL                  (BIT2|BIT1)                 ///< Period SMI Select
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_64S                  0x0000
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_32S                  0x0002
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_16S                  0x0004
#define V_PMC_PWRM_GEN_PMCON_A_PER_SMI_8S                   0x0006

#define R_PMC_PWRM_GEN_PMCON_B                              0x1024                      ///< General PM Configuration B
#define B_PMC_PWRM_GEN_PMCON_B_CNVLDOCFGLOCK                BIT20                       ///< CNVi LDO Config Lock
#define B_PMC_PWRM_GEN_PMCON_B_CPU_EPOC_LOCK                BIT19                       ///< CPU EPOC Lock
#define B_PMC_PWRM_GEN_PMCON_B_SLPSX_STR_POL_LOCK           BIT18                       ///< SLP_Sx# Stretching Policy Lock-Down
#define B_PMC_PWRM_GEN_PMCON_B_VR_CONFIG_LOCK               BIT17                       ///< ACPI Base Lock
#define B_PMC_PWRM_GEN_PMCON_B_WOL_EN_OVRD                  BIT13                       ///< WOL Enable Override
#define B_PMC_PWRM_GEN_PMCON_B_BIOS_PCI_EXP_EN              BIT10                       ///< BIOS PCI Express Enable
#define B_PMC_PWRM_GEN_PMCON_B_PWRBTN_LVL                   BIT9                        ///< Power Button Level
#define B_PMC_PWRM_GEN_PMCON_B_SMI_LOCK                     BIT4                        ///< SMI Lock
#define B_PMC_PWRM_GEN_PMCON_B_RPS                          BIT2                        ///< RTC_PWR_STS

#define R_PMC_PWRM_CRID                                     0x1030                      ///< Configured Revision ID
#define B_PMC_PWRM_CRID_RID_SEL                             (BIT1 | BIT0)               ///< RID Select(RID_SEL)
#define V_PMC_PWRM_CRID_RID_SEL_REVISIONID                  0
#define V_PMC_PWRM_CRID_RID_SEL_CRID0                       1
#define B_PMC_PWRM_CRID_CRID_LK                             BIT31                       ///< CRID Lock

#define R_PMC_PWRM_ETR3                                     0x1048                      ///< Extended Test Mode Register 3
#define B_PMC_PWRM_ETR3_CF9LOCK                             BIT31                       ///< CF9h Lockdown
#define B_PMC_PWRM_ETR3_LATCH_EVENTS_C10_EXIT               BIT30                       ///< Latch Events C10 exit
#define B_PMC_PWRM_ETR3_CF9GR                               BIT20                       ///< CF9h Global Reset
#define B_PMC_PWRM_ETR3_CWORWRE                             BIT18                       ///< CF9h Without Resume Well Reset Enable
#define B_PMC_PWRM_ETR3_ER_DEBUG_EN                         BIT1                        ///< Energy Reporting Debug Enable
#define B_PMC_PWRM_PMC_THROT_1_VRALERT_EN                   BIT0                        ///< VRAlert# Enable

#define R_PMC_PWRM_SSML                                     0x104C           ///< SET STRAP MSG LOCK
#define B_PMC_PWRM_SSML_SSL                                 BIT0             ///< Set_Strap Lock
#define R_PMC_PWRM_SSMC                                     0x1050           ///< SET STRAP MSG CONTROL
#define B_PMC_PWRM_SSMC_SSMS                                BIT0             ///< Set_Strap Mux Select
#define R_PMC_PWRM_SSMD                                     0x1054           ///< SET STRAP MSG DATA
#define B_PMC_PWRM_SSMD_CORE_HIGH_VOLTAGE_MODE              BIT14            ///< VCCIA Boot Voltage Select
                                                                             ///< 0: Nominal Voltage
                                                                             ///< 1: High Voltage (support voltage higher than 1.65v (max 2.01v))
#define N_PMC_PWRM_SSMD_CORE_HIGH_VOLTAGE_MODE              14

#define R_PMC_PWRM_CRID_UIP                                 0x10B0           ///< Configured Revision ID
#define B_PMC_PWRM_CRID_UIP_CRID_UIP                        BIT0             ///< CRID Update in Progress

#define R_PMC_PWRM_MODPHY_PM_CFG3                           0x10C8           ///< ModPHY Power Management Configuration Reg 3
#define R_PMC_PWRM_MODPHY_PM_CFG5                           0x10D0           ///< ModPHY Power Management Configuration Reg 5
#define R_PMC_PWRM_MODPHY_PM_CFG6                           0x10D4           ///< ModPHY Power Management Configuration Reg 6

#define R_PMC_PWRM_CPPM_MPG_POL1A                           0x10E0           ///< CPPM ModPHY Gating Policy Reg 1
#define B_PMC_PWRM_CPPM_MPG_POL1A_CPPM_MPHY_QUAL            BIT30            ///< CPPM Shutdown Qualifier Enable for ModPHY
#define B_PMC_PWRM_CPPM_MPG_POL1A_LT_MPHY_SEL               BIT29            ///< ASLT/PLT Selection for modPHY
#define B_PMC_PWRM_CPPM_MPG_POL1A_LTR_MPHY_THRESH           0x1FF            ///< LTR Threshold for ModPHY

#define R_PMC_PWRM_EXT_V1P05_VR_CONFIG                      0x11C0
#define R_PMC_PWRM_VCCIN_AUX_CFG1                           0x11D0
#define R_PMC_PWRM_VCCIN_AUX_CFG2                           0x11D4

#define R_PMC_PWRM_WINIDLE_BIOS_ENABLE                      0x1400
#define B_PMC_PWRM_WINIDLE_BIOS_ENABLE_WI_LOCK              BIT31
#define B_PMC_PWRM_WINIDLE_BIOS_ENABLE_WI_ENABLE            BIT0

#define R_PMC_PWRM_WADT_AC                                  0x1800                      ///< Wake Alarm Device Timer: AC
#define R_PMC_PWRM_WADT_DC                                  0x1804                      ///< Wake Alarm Device Timer: DC
#define R_PMC_PWRM_PRSTS                                    0x1810                      ///< Power and Reset Status
#define B_PMC_PWRM_PRSTS_WOL_OVR_WK_STS                     BIT5                        ///< Wake On LAN Override Wake Status
#define B_PMC_PWRM_PRSTS_PMC_HOST_WAKE_STS                  BIT4                        ///< PMC_HOST_WAKE_STS
#define B_PMC_PWRM_PRSTS_ME_HOST_WAKE_STS                   BIT0                        ///< ME_HOST_WAKE_STS

#define R_PMC_PWRM_PMC_PWR_CTL                              0x1814                      ///< PMC Power Control
#define B_PMC_PWRM_PMC_PWR_CTL_CT_EN_PMC                    BIT3                        ///< PMC Clock Throttling Enable
#define R_PMC_PWRM_PM_CFG                                   0x1818                      ///< Power Management Configuration Reg 1
#define B_PMC_PWRM_PM_CFG_ALLOW_24_OSC_SD                   BIT29                       ///< Allow 24MHz Crystal Oscillator Shutdown
#define B_PMC_PWRM_PM_CFG_DBG_MODE_LOCK                     BIT27                       ///< Debug Mode Lock
#define B_PMC_PWRM_PM_CFG_ALLOW_USB2_CORE_PG                BIT25                       ///< Allow USB2 PHY Core Power Gating
#define B_PMC_PWRM_PM_CFG_ER_LOCK                           BIT24                       ///< Energy Reporting Lock
#define B_PMC_PWRM_PM_CFG_PMC_READ_DISABLE                  BIT22                       ///< Disable Reads to PMC
#define B_PMC_PWRM_PM_CFG_SLP_SUS_MIN_ASST_WDTH             (BIT19 | BIT18)             ///< SLP_SUS# Minimum Assertion Width
#define V_PMC_PWRM_PM_CFG_SSMAW_4S                          (BIT19 | BIT18)             ///< 4 seconds
#define V_PMC_PWRM_PM_CFG_SSMAW_1S                          BIT19                       ///< 1 second
#define V_PMC_PWRM_PM_CFG_SSMAW_0_5S                        BIT18                       ///< 0.5 second (500ms)
#define V_PMC_PWRM_PM_CFG_SSMAW_0S                          0                           ///< 0 second
#define B_PMC_PWRM_PM_CFG_SLP_A_MIN_ASST_WDTH               (BIT17 | BIT16)             ///< SLP_A# Minimum Assertion Width
#define V_PMC_PWRM_PM_CFG_SAMAW_2S                          (BIT17 | BIT16)             ///< 2 seconds
#define V_PMC_PWRM_PM_CFG_SAMAW_98ms                        BIT17                       ///< 98ms
#define V_PMC_PWRM_PM_CFG_SAMAW_4S                          BIT16                       ///< 4 seconds
#define V_PMC_PWRM_PM_CFG_SAMAW_0S                          0                           ///< 0 second
#define B_PMC_PWRM_PM_CFG_AG3_LS_EN                         BIT13                       ///< After G3 Last State Enable
#define B_PMC_PWRM_PM_CFG_A8GR_LS_EN                        BIT12                       ///< After Type 8 Global Reset Last State Enable
#define B_PMC_PWRM_PM_CFG_PWR_CYC_DUR                       (BIT9 | BIT8)               ///< Reset Power Cycle Duration
#define V_PMC_PWRM_PM_CFG_RPCD_1S                           (BIT9 | BIT8)               ///< 1-2 seconds
#define V_PMC_PWRM_PM_CFG_RPCD_2S                           BIT9                        ///< 2-3 seconds
#define V_PMC_PWRM_PM_CFG_RPCD_3S                           BIT8                        ///< 3-4 seconds
#define V_PMC_PWRM_PM_CFG_RPCD_4S                           0                           ///< 4-5 seconds (Default)
#define B_PMC_PWRM_PM_CFG_COCS                              BIT5                        ///< CPU OC Strap
#define B_PMC_PWRM_PM_CFG_ER_EN                             BIT2                        ///< Energy Reporting Enable
#define B_PMC_PWRM_PM_CFG_TIMING_T581                       (BIT1 | BIT0)               ///< Timing t581

#define R_PMC_PWRM_S3_PWRGATE_POL                           0x1828                      ///< S3 Power Gating Policies
#define B_PMC_PWRM_S3_PWRGATE_POL_S3DC_GATE_SUS             BIT1                        ///< S3 Power Gate Enable in DC Mode: SUS Well
#define B_PMC_PWRM_S3_PWRGATE_POL_S3AC_GATE_SUS             BIT0                        ///< S3 Power Gate Enable in AC Mode: SUS Well

#define R_PMC_PWRM_S4_PWRGATE_POL                           0x182C                      ///< S4 Power Gating Policies
#define B_PMC_PWRM_S4_PWRGATE_POL_S4DC_GATE_SUS             BIT1                        ///< S4 Power Gate Enable in DC Mode: SUS Well
#define B_PMC_PWRM_S4_PWRGATE_POL_S4AC_GATE_SUS             BIT0                        ///< S4 Power Gate Enable in AC Mode: SUS Well

#define R_PMC_PWRM_S5_PWRGATE_POL                           0x1830                      ///< S5 Power Gating Policies
#define B_PMC_PWRM_S5_PWRGATE_POL_S5DC_GATE_SUS             BIT15                       ///< S5 Power Gate Enable in DC Mode: SUS Well
#define B_PMC_PWRM_S5_PWRGATE_POL_S5AC_GATE_SUS             BIT14                       ///< S5 Power Gate Enable in AC Mode: SUS Well

#define R_PMC_PWRM_DSX_CFG                                  0x1834                      ///< DeepSx Configuration
#define B_PMC_PWRM_DSX_CFG_WAKE_PIN_DSX_EN                  BIT2                        ///< WAKE# Pin DeepSx Enable
#define B_PMC_PWRM_DSX_CFG_ACPRES_PD_DSX_DIS                BIT1                        ///< AC_PRESENT Pin Pulldown in DeepSx Disable
#define B_PMC_PWRM_DSX_CFG_LANWAKE_PIN_DSX_EN               BIT0                        ///< LANWAKE Pin DeepSx Enable

#define R_PMC_PWRM_PM_CFG2                                  0x183C                      ///< Power Management Configuration Reg 2
#define B_PMC_PWRM_PM_CFG2_PBOP                             (BIT31 | BIT30 | BIT29)     ///< Power Button Override Period
#define N_PMC_PWRM_PM_CFG2_PBOP                             29                          ///< Power Button Override Period (PBOP)
#define B_PMC_PWRM_PM_CFG2_PB_DIS                           BIT28                       ///< Power Button Native Mode Disable
#define B_PMC_PWRM_PM_CFG2_EN_DBG_MSG                       BIT27                       ///< Enable Debug Messaging
#define B_PMC_PWRM_PM_CFG2_DRAM_RESET_CTL                   BIT26                       ///< DRAM_RESET# Control
#define N_PMC_PWRM_PM_CFG2_DRAM_RESET_CTL                   26

#define R_PMC_PWRM_SLP_S0_RESIDENCY                         0x193C                      ///< SLP_S0 Residency
#define R_PMC_PWRM_PS_ON_RESIDENCY                          0x18F8                      ///< PS_ON# Residency

#define R_PMC_PWRM_PM_SYNC_MISC_CFG                         0x18C8
#define R_PMC_PWRM_PM_SYNC_MISC_CFG_VER2                    0x18D8
#define B_PMC_PWRM_PM_SYNC_MISC_CFG_PM_SYNC_LOCK            BIT15                       ///< PM_SYNC Configuration Lock

#define R_PMC_PWRM_PM_SYNC_STATE_HYS                        0x18D0                      ///< PM_SYNC State Hysteresis
#define R_PMC_PWRM_PM_SYNC_MODE                             0x18D4                      ///< PM_SYNC Pin Mode

#define R_PMC_PWRM_PM_CFG3                                  0x18E0                      ///< Power Management Configuration Reg 3
#define B_PMC_PWRM_PM_CFG3_SYS_RST_SUP                      BIT22                       ///< Sys Reset# Suppression
#define B_PMC_PWRM_PM_CFG3_HOST_WLAN_PP_EN                  BIT17                       ///< Host Wireless LAN Phy Power Enable
#define B_PMC_PWRM_PM_CFG3_DSX_WLAN_PP_EN                   BIT16                       ///< Deep-Sx WLAN Phy Power Enable
#define B_PMC_PWRM_PM_CFG3_HOST_MISC_CORE_CFG_CPU_VCC_MAP   BIT2                        ///< Enable CPU Vcc Mapping

#define R_PMC_PWRM_PM_DOWN_PPB_CFG                          0x18E4                      ///< PM_DOWN PCH_POWER_BUDGET CONFIGURATION
#define B_PMC_PWRM_PM_DOWN_PPB_CFG_PPB_TSTATE_EN            BIT1                        ///< PCH Power Budget to Thermal Throttle State enable
#define B_PMC_PWRM_PM_DOWN_PPB_CFG_PPB_CFG_LOCK             BIT0                        ///< PCH Power Budget Configuration lock

#define R_PMC_PWRM_PM_CFG4                                  0x18E8                            ///< Power Management Configuration Reg 4
#define B_PMC_PWRM_PM_CFG4_U2_PHY_PG_EN                     BIT30                             ///< USB2 PHY SUS Well Power Gating Enable
#define B_PMC_PWRM_PM_CFG4_VCCST_TMR                        (BIT15 | BIT14 | BIT13 | BIT12)   ///< VccST Ramp Timer
#define B_PMC_PWRM_PM_CFG4_CPU_IOVR_RAMP_DUR                0x1FF                             ///< CPU I/O VR Ramp Duration
#define N_PMC_PWRM_PM_CFG4_CPU_IOVR_RAMP_DUR                0

#define R_PMC_PWRM_CPU_EPOC                                 0x18EC                      ///< CPU Early Power-on Configuration
#define B_PMC_PWRM_CPU_EPOC_EPOC2_EN                        BIT29                       ///< EPOC2_EN
#define N_PMC_PWRM_CPU_EPOC_XTAL_FREQ_LSB                   17
#define N_PMC_PWRM_CPU_EPOC_XTAL_FREQ_MSB                   20

#define R_PMC_PWRM_ADR_EN                                   0x18F0                      ///< ADR Enable
#define B_PMC_PWRM_ADR_EN_ADR_GPIO_SEL                      (BIT31 | BIT30)             ///< ADR GPIO Selection
#define N_PMC_PWRM_ADR_EN_ADR_GPIO_SEL                      30
#define V_PMC_PWRM_ADR_GPIO_B                               0x1
#define V_PMC_PWRM_ADR_GPIO_C                               0x2
#define B_PMC_PWRM_ADR_EN_HPR_ADR_EN                        BIT28                       ///< Host Partition Reset ADR Enable
#define B_PMC_PWRM_ADR_EN_ADR_PLT_ACK_EN                    BIT26                       ///< ADR Pltaform Acknowledge Enable
#define B_PMC_PWRM_ADR_EN_ADR_FEAT_EN                       BIT0                        ///< ADR Feature Enable

#define R_PMC_PWRM_PM_SYNC_MODE_C0                          0x18F4                      ///< PM_SYNC Pin Mode In C0
#define R_PMC_PWRM_ACPI_TMR_CTL                             0x18FC                      ///< ACPI Timer Control
#define B_PMC_PWRM_ACPI_TMR_CTL_ACPI_TIM_DIS                BIT1                        ///< ACPI Timer Disable

#define R_PMC_PWRM_VR_MISC_CTL                              0x1900                      ///< VR Miscellaneous Control

#define R_PMC_PWRM_ADR_GEN_CFG                              0x196C                      ///< ADR General Configuration Register
#define R_PMC_PWRM_ADR_GEN_CFG_V2                           0x1908                      ///< ADR General Configuration
#define B_PMC_PWRM_ADR_GEN_CFG_ADR_P2_TIMER_SCALE           (BIT28|BIT27|BIT26)         ///< ADR Timer Scale
#define N_PMC_PWRM_ADR_GEN_CFG_ADR_P2_TIMER_SCALE           26
#define B_PMC_PWRM_ADR_GEN_CFG_ADR_P2_EN                    BIT19                       ///< ADR Phase 2 Enable
#define B_PMC_PWRM_ADR_GEN_CFG_ADR_P2_TIMER_TOV             (BIT18|BIT17|BIT16|BIT15|BIT14|BIT13|BIT12|BIT11)   ///< ADR Timer Base
#define N_PMC_PWRM_ADR_GEN_CFG_ADR_P2_TIMER_TOV             11
#define B_PMC_PWRM_ADR_GEN_CFG_ADR_P1_TIMER_SCALE           (BIT10|BIT9|BIT8)           ///< ADR Timer Scale (Tick Frequency)
#define N_PMC_PWRM_ADR_GEN_CFG_ADR_P1_TIMER_SCALE           8
#define B_PMC_PWRM_ADR_GEN_CFG_ADR_P1_TIMER_TOV             (BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)           ///< ADR Timer Base
#define N_PMC_PWRM_ADR_GEN_CFG_ADR_P1_TIMER_TOV             0

#define R_PMC_PWRM_GBL2HOST_EN                              0x190C                      ///< Global to Host Reset Enable
#define B_PMC_PWRM_GBL2HOST_EN_HOST_RESET_TIMEOUT           BIT0                        ///< Global to Host for Host Partition Reset Timeout

#define R_PMC_PWRM_GPIO_CFG                                 0x1920                        ///< GPIO Configuration
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW2                        (BIT11 | BIT10 | BIT9 | BIT8) ///< GPIO Group to GPE_DW2 assignment encoding
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW2                        8
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW1                        (BIT7 | BIT6 | BIT5 | BIT4)   ///< GPIO Group to GPE_DW1 assignment encoding
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW1                        4
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW0                        (BIT3 | BIT2 | BIT1 | BIT0)   ///< GPIO Group to GPE_DW0 assignment encoding
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW0                        0

#define R_PMC_PWRM_GBLRST_CAUSE0                            0x1924                      ///< Global Reset Causes 0
#define R_PMC_PWRM_PMLDOCTRL                                0x1A00                      ///< PMC LDO Control Register
#define B_PMC_PWRM_PMLDOCTRL_CNV1P24LDOLPEN                 BIT9                        ///< 1.24V CNVi LDO low power enable
#define B_PMC_PWRM_PMLDOCTRL_USB2TS1P3LDODSEN               BIT0                        ///< 1.3V LDO dynamic shutdown enable

#define R_PMC_PWRM_GBLRST_CAUSE1                            0X1928                      ///< Global Reset Causes 1

#define R_PMC_PWRM_HPR_CAUSE0                               0x192C                      ///< Host Partition Reset Causes
#define B_PMC_PWRM_HPR_CAUSE0_GBL_TO_HOST                   BIT15                       ///< Global Reset Converted to Host Reset
#define B_PMC_PWRM_HPR_CAUSE0_AWR                           BIT0                        ///< Asynchronous Warm Reset Status

#define R_PMC_PWRM_DSW_SPARE_GCR_0                          0x1954                      ///< DSW Well Spare Reg 0

#define R_PMC_PWRM_SUS_SPARE_GCR_0                          0x1958                      ///< GCR SUS_SPARE_GCR_0 register
#define B_PMC_PWRM_SUS_SPARE_GCR_0_TELEMETRY_TYPE           BIT0                        ///< Set by BIOS to indicate the telemetry type for Desktop Psys support

#define R_PMC_PWRM_ADR_CTRL_STS                             0x1960                      ///< ADR Control and Status
#define B_PMC_PWRM_ADR_CTRL_STS_ADR_RST_STS                 BIT0                        ///< ADR Reset Status

#define R_PMC_PWRM_ADR_GBLRST_EN_0                          0x191C                      ///< ADR Global Reset Enable 0

#define R_PMC_PWRM_PMC_S0IX_LDO_OFF_COUNT_REG               0x1A04                          ///< PMC S0IX LDO OFF COUNT Register
#define B_PMC_PWRM_PMC_S0IX_LDO_OFF_COUNT_REG_PMC_S0IX_LDO_OFF_COUNT_REG_LOCK     BIT31     ///< PMC S0IX LDO OFF COUNT Register Lock
#define R_PMC_PWRM_PMC_S0_LDO_OFF_COUNT_REG                 0x1A08                          ///< PMC S0 LDO OFF COUNT Register
#define B_PMC_PWRM_PMC_S0_LDO_OFF_COUNT_REG_PMC_S0_LDO_OFF_COUNT_REG_LOCK         BIT31     ///< PMC S0 LDO OFF COUNT Register Lock

#define R_PMC_PWRM_LTR_IGN                                  0x1B0C                ///< LTR Ignore
#define R_PMC_PWRM_OBFF_CFG                                 0x1B14                ///< OBFF Configuration
#define V_PMC_PWRM_OBFF_CFG_MASK                            0xFFFF4616            ///< OBFF Configuration Mask
#define R_PMC_PWRM_CPPMVRIC                                 0x1B1C                ///< CPPM VR Idle Control 1
#define R_PMC_PWRM_CPPM_MISC_CFG                            0x1B20                ///< CPPM Miscellaneous Configuration
#define V_PMC_PWRM_CPPM_MISC_CFG_MASK                       0xFFFFFF00            ///< CPPM Miscellaneous Configuration Mask
#define R_PMC_PWRM_CPPM_CG_POL1A                            0x1B24                ///< CPPM Clock Gating Policy Reg 1
#define R_PMC_PWRM_CPPM_CG_POL1B                            0x1B28                ///< CPPM Clock Gating Policy Reg 2
#define R_PMC_PWRM_CPPM_CG_POL2B                            0x1B30                ///< CPPM Clock Gating Policy Reg 2
#define R_PMC_PWRM_CPPM_CG_POL2A                            0x1B40                ///< CPPM Clock Gating Policy Reg 2
#define R_PMC_PWRM_CPPM_CG_POL2A_V2                         0x1B2C                ///< CPPM Clock Gating Policy Reg 2

#define R_PMC_PWRM_CPPM_CG_POL3A                            0x1BA8                      ///< CPPM Clock Gating Policy Reg 3
#define R_PMC_PWRM_CPPM_CG_POL3A_V2                         0x1B34                      ///< CPPM Clock Gating Policy Reg 3
#define B_PMC_PWRM_CPPM_CG_POL1A_LT_G1_SEL                  BIT29                       ///< ASLT/PLT Selection for Clock Source Group 1
#define B_PMC_PWRM_CPPM_CG_POL1A_CPPM_G1_QUAL               BIT30                       ///< CPPM Shutdown Qualifier Enable for Clock Source Group 1
#define R_PMC_PWRM_CPPMFALIGNCTL_1                          0x1BD0                      ///< CPPM Forced Alignment Control 1
#define R_PMC_PWRM_CPPM_CG_POL4A                            0x1B3C                      ///< CPPM Clock Gating Policy Reg 4
#define B_PMC_PWRM_CPPMFALIGNCTL_1_MASK                     0xC83F8000                      ///< CPPM MASK

#define R_PMC_PWRM_ACTL                                     0x1BD8                      ///< ACPI Control
#define B_PMC_PWRM_ACTL_SCIS                                (BIT2 | BIT1 | BIT0)        ///< SCI IRQ Select
#define V_PMC_PWRM_ACPI_SCIS_IRQ9                           0
#define V_PMC_PWRM_ACPI_SCIS_IRQ10                          1
#define V_PMC_PWRM_ACPI_SCIS_IRQ11                          2
#define V_PMC_PWRM_ACPI_SCIS_IRQ20                          4
#define V_PMC_PWRM_ACPI_SCIS_IRQ21                          5
#define V_PMC_PWRM_ACPI_SCIS_IRQ23                          7

#define R_PMC_PWRM_BM_CX_CNF                                0x1BDC                      ///< BM_BREAK_EN and Cx State Configuration Register
#define B_PMC_PWRM_BM_CX_CNF_PHOLD_BM_STS_BLOCK             BIT14                       ///< PHOLD to BM_STS Block
#define B_PMC_PWRM_BM_CX_CNF_BM_STS_ZERO_EN                 BIT10                       ///< Bus Master Status Zero Enable
#define B_PMC_PWRM_BM_CX_CNF_PM_SYNC_MSG_MODE               BIT9                        ///< PM_SYNC Serial Message Mode Enable

#define R_PMC_PWRM_PMC_THROT_1                              0x1BE0                      ///< PMC Throttling 1
#define B_PMC_PWRM_PMC_THROT_1_VRALERT_EN                   BIT0                        ///< VRAlert# Enable
#define B_PMC_PWRM_PMC_THROT_1_PMC_THROT_LOCK               BIT15                       ///< PMC THROT 1 Register Lock

#define R_PMC_PWRM_CS_SD_CTL1                               0x1BE8                      ///< Clock Source Shutdown Control Reg 1

#define R_PMC_PWRM_CS_SD_CTL2                               0x1BEC                      ///< Clock Source Shutdown Control Reg 1

#define R_PMC_PWRM_LPM_ADEM_HOST_CTRL                       0x1C20                      ///< Low Power Mode Auto-Demotion Host Control
#define R_PMC_PWRM_LPM_ADEM_HOST_CTRL_V2                    0x1700
#define R_PMC_PWRM_LPM_ADEM_HOST_CTRL_V3                    0x16F4                      ///< Low Power Mode Auto-Demotion Host Control Register
#define B_PMC_PWRM_LPM_ADEM_HOST_CTRL_LPM_ADEM_HOST_ENABLE        BIT0                  ///< Low Power Mode Auto-Demotion Host Enable

#define R_PMC_PWRM_LPM_EN                                   0x1C78                      ///< Low Power Mode Enable
#define R_PMC_PWRM_LPM_EN_V2                                0x179C                      ///< Low Power Mode Enable- version 2
#define R_PMC_PWRM_LPM_EN_V3                                0x1798                      ///< Low Power Mode Enable
#define V_PMC_PWRM_LPM_EN                                   0x7FFFFFFF                  ///< LPM_EN bitmask
#define R_PMC_PWRM_LPM_PRI                                  0x1C7C                      ///< Low Power Mode Priority
#define V_PMC_PWRM_LPM_PRI_MASK                             0x00000FFF                  ///< LPM_PRI bitmask
#define R_PMC_PWRM_LPM_PRI_V2                               0x17A0
#define R_PMC_PWRM_LPM_PRI_V3                               0x179C                      ///< Low Power Mode Priority

#define R_PMC_PWRM_HSWPGCR1                                 0x1DD0                      ///< Host SW PG Control Register 1
#define B_PMC_PWRM_HSWPGCR1_SW_PG_CTRL_LOCK                           BIT31             ///< SW PG Req Control Lock

#define R_PMC_PWRM_FAB_PG_CTL_PSF                                     0x1E00            ///< Fabric Power-gating Control for PSF
#define R_PMC_PWRM_FAB_PG_CTL_SBR                                     0x1E04            ///< Fabric Power-gating Control for IOSF-SB Routers
#define B_PMC_PWRM_FAB_PG_CTL_SBR_CX_QUAL_EN_SBR                      BIT29             ///< CX State Qualifier Enable for IOSF-SB router Power-Gating
#define B_PMC_PWRM_FAB_PG_CTL_SBR_ALL_SBR_IDLE_QUAL_EN_SBR            BIT31             ///< All SBR Idle Qualifier Enable for IOSF-SB Router Power-Gating

#define R_PMC_PWRM_ST_PG_FDIS_PMC_0_V2                      0x1E20                      ///< ST_PG_FDIS_PMC - Register 0

#define R_PMC_PWRM_ST_PG_FDIS_PMC_1                         0x1E20                      ///< Static PG Related Function Disable Register 1
#define R_PMC_PWRM_ST_PG_FDIS_PMC_1_V2                      0x1E24                      ///< ST_PG_FDIS_PMC - Register 1

#define R_PMC_PWRM_ST_PG_FDIS_PMC_2                         0x1E24                      ///< Static Function Disable Control Register 2
#define R_PMC_PWRM_ST_PG_FDIS_PMC_2_V2                      0x1E28                      ///< ST_PG_FDIS_PMC - Register 2

#define R_PMC_PWRM_BCLK_CFG                                 0x1F60                      ///< BCLK PLLs Configuration
#define B_PMC_PWRM_BCLK_CFG_CPU_BCLK_CFG_OVR_VAL            BIT2                        ///< CPU_BCLK_CFG_OVR_VAL
#define B_PMC_PWRM_BCLK_CFG_CPU_BCLK_CFG_OVR_EN             BIT3                        ///< CPU_BCLK_CFG_OVR_EN
#define B_PMC_PWRM_BCLK_CFG_SOC_BCLK_CFG_OVR_VAL            BIT4                        ///< SOC_BCLK_CFG_OVR_VAL
#define B_PMC_PWRM_BCLK_CFG_SOC_BCLK_CFG_OVR_EN             BIT5                        ///< SOC_BCLK_CFG_OVR_EN

#define R_PMC_PWRM_CPU_EPOC2                                0x1F64                      ///< CPU Early Power-on Configuration 2

#define B_PMC_PWRM_CPU_EPOC2_VCCSA_BOOT_VOLTAGE_SEL         BIT0                        ///< VCCSA Boot Voltage Select
                                                                                        ///< 0: EPOC2 SA VID contains Nominal Voltage
                                                                                        ///< 1: EPOC2 SA VID contains Overclocking Voltage
#define N_PMC_PWRM_CPU_EPOC2_VCCSA_BOOT_VOLTAGE_SEL         0

#define B_PMC_PWRM_CPU_EPOC2_CPU_BGREF_MODE_OVR_VAL         BIT1                        ///< CPU Bandgap Reference Mode Override Value
#define N_PMC_PWRM_CPU_EPOC2_CPU_BGREF_MODE_OVR_VAL         1

#define B_PMC_PWRM_CPU_EPOC2_CPU_BGREF_MODE_OVR_EN          BIT2                        ///< CPU Bandgap Reference Mode Override Enable

#define R_PMC_PWRM_VGPIO_BAR                                0x2000
#define R_PMC_PWRM_VGPIO_CFG                                0x2010
#define B_PMC_PWRM_VGPIO_CFG_VGPIO_eSPI_MODE                BIT0

//
// Low Power Mode requirements - 1st  register
//
#define R_PMC_PWRM_LPM0_CSOFF_REQ                           0x1000                      ///< Low Power Mode 0 Requirements Reg 0
//
// Low Power Mode requirements - End register
//
#define R_PMC_PWRM_LPM7_REQ_5                               0x1164                      ///<Low Power Mode 7 Requirements Reg 5

//
// PWRM Registers for IPC interface
//

#define V_PMC_PWRM_IPC_CMD_COMMAND_PMIC                       0xC0                      ///< IPC commmand to configure PMIC
#define V_PMC_PWRM_IPC_CMD_CMD_ID_READ_PMIC1_I2C              0x00                      ///< SubCommand to read PMIC I2C
#define V_PMC_PWRM_IPC_CMD_CMD_ID_WRITE_PMIC1_I2C             0x01                      ///< SubCommand to write PMIC I2C
#define V_PMC_PWRM_IPC_CMD_CMD_ID_READ_PMIC2_I2C              0x03                      ///< SubCommand to read PMIC I2C
#define V_PMC_PWRM_IPC_CMD_CMD_ID_WRITE_PMIC2_I2C             0x04                      ///< SubCommand to read PMIC I2C
#define V_PMC_PWRM_IPC_CMD_CMD_ID_READ_PMIC3_I2C              0x06                      ///< SubCommand to read PMIC I2C
#define V_PMC_PWRM_IPC_CMD_CMD_ID_WRITE_PMIC3_I2C             0x07                      ///< SubCommand to read PMIC I2C

#define V_PMC_PWRM_IPC_CMD_CMD_ID_WRITE_LOCK_ENABLE_PMIC_I2C  0xF                       ///< SubCommand to PMC write lock enable

//
// GPE interrupts definitons
//
#define GPE_PCI_EXP                                     0x69
#define GPE_PME                                         0x6B
#define GPE_WADT_EN                                     0x72
#define GPE_PME_B0                                      0x6D
#define GPE_ESPI_EN                                     0x6E
#define GPE1_IOE_PME_B0                                 0x80
#define GPE1_CNVI_PME_B0                                0x85
#define GPE1_GBE_PME_B0                                 0x86
#define GPE1_XHCI_PME_B0                                0x89
#define GPE1_ACE_PME_B0                                 0x8A
#define GPE1_XDCI_PME_B0                                0x8C
#define GPE1_LPSS_PME_B0_EN                             0x8E

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

#define R_ACPI_IO_GPE1_STS_31_0                         0x10                            ///< General Purpose Event 1 Status [31:0]
#define B_ACPI_IO_GPE1_STS_TBTLSX_PME_B0_STS            BIT1                            ///< TBTLSX_PME_B0_STS
#define R_ACPI_IO_GPE1_EN_31_0                          0x1C                            ///< General Purpose Event 1 Enable [31:0]

// Structs
typedef union {
  struct {
    UINT32 QclkGvLastPoint  : 3;  // Bits 3:0 - Self-Refresh Entry value from PMC register on Warm / S3 flow for SAGV
    UINT32 McFspOp          : 1;  // Bits 4:4 - Self-Refresh Entry value from PMC register on Warm / S3 flow for FspOp
    UINT32 Spare            : 28; // Bits 31:5
  } Bits;
  UINT32 Data;
} DSW_SPARE_GCR_0_STRUCT;

#define R_PMC_PWRM_PMC_SOFT_STRAP_7                                           0x091C                    ///< PMC_SOFT_STRAP_7 Register
#define B_PMC_PWRM_PMC_SOFT_STRAP_7_DEF_CPU_BCLK_CFG                          (BIT15 | BIT14)           ///< This value is used to tell the CPU how it should configure the CPU BCLK PLL.
#define V_PMC_PWRM_PMC_SOFT_STRAP_7_DEF_CPU_BCLK_CFG_INTEGRATED               (0x00008000)              ///< BIT[15:14] = {10b}, BCLK source is integrated. BIT[15:14] = {00b}, BCLK source is external.
#endif
