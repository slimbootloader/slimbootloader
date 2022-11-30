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

  Copyright (c) 2021-2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PCH_REGS_PMC_H_
#define _PCH_REGS_PMC_H_

//
// PMC SSRAM Registers (D20:F2)
//
#define PCI_DEVICE_NUMBER_PCH_PMC_SSRAM          20
#define PCI_FUNCTION_NUMBER_PCH_PMC_SSRAM        2

//
// PMC Registers (D31:F2)
//
#define PCI_DEVICE_NUMBER_PCH_PMC                31
#define PCI_FUNCTION_NUMBER_PCH_PMC              2

#define B_PMC_CFG_PWRM_BASE_MASK                 0xFFFF0000                    ///< PWRM must be 64KB alignment to align the source decode.

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

#define R_ACPI_IO_SMI_EN                              0x30
#define S_ACPI_IO_SMI_EN                              4
#define B_ACPI_IO_SMI_EN_LEGACY_USB2                  BIT17
#define B_ACPI_IO_SMI_EN_PERIODIC                     BIT14
#define B_ACPI_IO_SMI_EN_TCO                          BIT13
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
#define N_ACPI_IO_SMI_EN_PERIODIC                     14
#define N_ACPI_IO_SMI_EN_TCO                          13
#define N_ACPI_IO_SMI_EN_MCSMI                        11
#define N_ACPI_IO_SMI_EN_SWSMI_TMR                    6
#define N_ACPI_IO_SMI_EN_APMC                         5
#define N_ACPI_IO_SMI_EN_ON_SLP_EN                    4
#define N_ACPI_IO_SMI_EN_LEGACY_USB                   3

#define R_ACPI_IO_SMI_STS                             0x34
#define S_ACPI_IO_SMI_STS                             4
#define B_ACPI_IO_SMI_STS_GPIO_UNLOCK                 BIT27
#define B_ACPI_IO_SMI_STS_SMBUS                       BIT16
#define B_ACPI_IO_SMI_STS_PERIODIC                    BIT14
#define B_ACPI_IO_SMI_STS_TCO                         BIT13
#define B_ACPI_IO_SMI_STS_MCSMI                       BIT11
#define B_ACPI_IO_SMI_STS_SWSMI_TMR                   BIT6
#define B_ACPI_IO_SMI_STS_APM                         BIT5
#define B_ACPI_IO_SMI_STS_ON_SLP_EN                   BIT4
#define B_ACPI_IO_SMI_STS_BIOS                        BIT2
#define N_ACPI_IO_SMI_STS_LEGACY_USB3                 31
#define N_ACPI_IO_SMI_STS_ESPI                        28
#define N_ACPI_IO_SMI_STS_SPI                         26
#define N_ACPI_IO_SMI_STS_MONITOR                     21
#define N_ACPI_IO_SMI_STS_PCI_EXP                     20
#define N_ACPI_IO_SMI_STS_SMBUS                       16
#define N_ACPI_IO_SMI_STS_SERIRQ                      15
#define N_ACPI_IO_SMI_STS_PERIODIC                    14
#define N_ACPI_IO_SMI_STS_TCO                         13
#define N_ACPI_IO_SMI_STS_MCSMI                       11
#define N_ACPI_IO_SMI_STS_GPIO_SMI                    10
#define N_ACPI_IO_SMI_STS_GPE0                        9
#define N_ACPI_IO_SMI_STS_PM1_STS_REG                 8
#define N_ACPI_IO_SMI_STS_SWSMI_TMR                   6
#define N_ACPI_IO_SMI_STS_APM                         5
#define N_ACPI_IO_SMI_STS_ON_SLP_EN                   4
#define N_ACPI_IO_SMI_STS_LEGACY_USB                  3


#define R_ACPI_IO_OC_WDT_CTL                          0x54
#define B_ACPI_IO_OC_WDT_CTL_RLD                      BIT31
#define B_ACPI_IO_OC_WDT_CTL_EN                       BIT14
#define B_ACPI_IO_OC_WDT_CTL_TOV_MASK                 0x3FF
#define B_ACPI_IO_OC_WDT_CTL_FAILURE_STS              BIT23
#define B_ACPI_IO_OC_WDT_CTL_UNXP_RESET_STS           BIT22
#define B_ACPI_IO_OC_WDT_CTL_AFTER_POST               0x3F0000
#define V_ACPI_IO_OC_WDT_CTL_STATUS_FAILURE           1
#define V_ACPI_IO_OC_WDT_CTL_STATUS_OK                0

#define R_ACPI_IO_GPE0_STS_31_0                    0x60
#define R_ACPI_IO_GPE0_STS_127_96                  0x6C
#define S_ACPI_IO_GPE0_STS_127_96                  4
#define B_ACPI_IO_GPE0_STS_127_96_CPU_WAKE_EN      BIT19
#define B_ACPI_IO_GPE0_STS_127_96_WADT             BIT18
#define B_ACPI_IO_GPE0_STS_127_96_USB_CON_DSX_STS  BIT17
#define B_ACPI_IO_GPE0_STS_127_96_LAN_WAKE         BIT16
#define B_ACPI_IO_GPE0_STS_127_96_GPIO_TIER_2      BIT15
#define B_ACPI_IO_GPE0_STS_127_96_PME_B0           BIT13
#define B_ACPI_IO_GPE0_STS_127_96_PME              BIT11
#define B_ACPI_IO_GPE0_STS_127_96_BATLOW           BIT10
#define B_ACPI_IO_GPE0_STS_127_96_PCI_EXP          BIT9
#define B_ACPI_IO_GPE0_STS_127_96_RI               BIT8
#define B_ACPI_IO_GPE0_STS_127_96_SMB_WAK          BIT7
#define B_ACPI_IO_GPE0_STS_127_96_SWGPE            BIT2
#define B_ACPI_IO_GPE0_STS_127_96_HOT_PLUG         BIT1
#define N_ACPI_IO_GPE0_STS_127_96_PME_B0           13
#define N_ACPI_IO_GPE0_STS_127_96_PME              11

#define R_ACPI_IO_GPE0_EN_127_96                   0x7C
#define S_ACPI_IO_GPE0_EN_127_96                   4
#define B_ACPI_IO_GPE0_EN_127_96_WADT              BIT18
#define B_ACPI_IO_GPE0_EN_127_96_USB_CON_DSX_EN    BIT17
#define B_ACPI_IO_GPE0_EN_127_96_LAN_WAKE          BIT16
#define B_ACPI_IO_GPE0_EN_127_96_PME_B0            BIT13
#define B_ACPI_IO_GPE0_EN_127_96_PME               BIT11
#define B_ACPI_IO_GPE0_EN_127_96_PCI_EXP           BIT9
#define N_ACPI_IO_GPE0_EN_127_96_PME_B0            13
#define N_ACPI_IO_GPE0_EN_127_96_PME               11
#define N_ACPI_IO_GPE0_EN_127_96_BATLOW            10
#define N_ACPI_IO_GPE0_EN_127_96_PCI_EXP           9
#define N_ACPI_IO_GPE0_EN_127_96_RI                8
#define N_ACPI_IO_GPE0_EN_127_96_TC0SCI            6
#define N_ACPI_IO_GPE0_EN_127_96_SWGPE             2
#define N_ACPI_IO_GPE0_EN_127_96_HOT_PLUG          1


//
// TCO register I/O map
//
#define R_TCO_IO_TCO1_STS                            0x04


#define R_TCO_IO_TCO1_CNT                            0x08


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

#define R_PMC_PWRM_CRID                                     0x1030           ///< Configured Revision ID
#define B_PMC_PWRM_CRID_RID_SEL                             (BIT0 | BIT1)    ///< RID Select
#define V_PMC_PWRM_CRID_RID_SEL_REVISIONID                  0
#define V_PMC_PWRM_CRID_RID_SEL_CRID0                       1
#define B_PMC_PWRM_CRID_CRID_LK                             BIT31            ///< CRID Lock

#define R_PMC_PWRM_ETR3                                     0x1048          ///< in CNL this is PWRM register
#define B_PMC_PWRM_ETR3_CF9LOCK                             BIT31           ///< CF9h Lockdown
#define B_PMC_PWRM_ETR3_LATCH_EVENTS_C10_EXIT               BIT30
#define B_PMC_PWRM_ETR3_CF9GR                               BIT20           ///< CF9h Global Reset
#define B_PMC_PWRM_ETR3_CWORWRE                             BIT18
#define B_PMC_PWRM_THROT_1_VR_ALERT                         BIT0

#define R_PMC_PWRM_SSML                                     0x104C           ///< Set Strap Msg Lock
#define B_PMC_PWRM_SSML_SSL                                 BIT0             ///< Set_Strap Lock
#define R_PMC_PWRM_SSMC                                     0x1050           ///< Set Strap Msg Control
#define B_PMC_PWRM_SSMC_SSMS                                BIT0             ///< Set_Strap Mux Select
#define R_PMC_PWRM_SSMD                                     0x1054           ///< Set Strap Msg Data

#define R_PMC_PWRM_CRID_UIP                                 0x10B0           ///< CRID update in progress

#define R_PMC_PWRM_MODPHY_PM_CFG1                           0x10C0
#define R_PMC_PWRM_MODPHY_PM_CFG2                           0x10C4

#define R_PMC_PWRM_MODPHY_PM_CFG3                           0x10C8   ///< modPHY Power Management Configuration Reg 2
#define B_PMC_PWRM_MODPHY_PM_CFG3_MLSPDDGE                  BIT30    ///< ModPHY Lane SUS Power Domain Dynamic Gating Enable

#define R_PMC_PWRM_MODPHY_PM_CFG5                           0x10D0
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_UFS2            BIT26
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_D0              BIT17
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_UFS             BIT16    ///< UFS ModPHY SPD RT Request
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_XDCI            BIT15    ///< xDCI ModPHY SPD RT Request
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_XHCI            BIT14    ///< xHCI ModPHY SPD RT Request
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_GBE             BIT13    ///< GbE ModPHY SPD RT Request
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_SATA            BIT12    ///< SATA ModPHY SPD RT Request
#define B_PMC_PWRM_MODPHY_PM_CFG5_MSPDRTREQ_A0              BIT0
#define R_PMC_PWRM_MODPHY_PM_CFG6                           0x10D4

#define R_PMC_PWRM_CPPM_MPG_POL1A                           0x10E0            ///< CPPM modPHY Gating Policy Reg 1A

#define R_PMC_PWRM_WADT_AC                                  0x1800
#define R_PMC_PWRM_PRSTS                                    0x1810                      ///< Power and Reset Status
#define B_PMC_PWRM_PRSTS_WOL_OVR_WK_STS                     BIT5
#define B_PMC_PWRM_PRSTS_FIELD_1                            BIT4
#define B_PMC_PWRM_PRSTS_ME_WAKE_STS                        BIT0


#define R_PMC_PWRM_S3_PWRGATE_POL                           0x1828                      ///< S3 Power Gating Policies
#define B_PMC_PWRM_S3_PWRGATE_POL_S3DC_GATE_SUS             BIT1                        ///< Deep S3 Enable in DC Mode
#define B_PMC_PWRM_S3_PWRGATE_POL_S3AC_GATE_SUS             BIT0                        ///< Deep S3 Enable in AC Mode

#define R_PMC_PWRM_S4_PWRGATE_POL                           0x182C                      ///< Deep S4 Power Policies
#define B_PMC_PWRM_S4_PWRGATE_POL_S4DC_GATE_SUS             BIT1                        ///< Deep S4 Enable in DC Mode
#define B_PMC_PWRM_S4_PWRGATE_POL_S4AC_GATE_SUS             BIT0                        ///< Deep S4 Enable in AC Mode

#define R_PMC_PWRM_S5_PWRGATE_POL                           0x1830                      ///< Deep S5 Power Policies
#define B_PMC_PWRM_S5_PWRGATE_POL_S5DC_GATE_SUS             BIT15                       ///< Deep S5 Enable in DC Mode
#define B_PMC_PWRM_S5_PWRGATE_POL_S5AC_GATE_SUS             BIT14                       ///< Deep S5 Enable in AC Mode

#define R_PMC_PWRM_DSX_CFG                                  0x1834                      ///< Deep SX Configuration
#define B_PMC_PWRM_DSX_CFG_WAKE_PIN_DSX_EN                  BIT2                        ///< WAKE# Pin DeepSx Enable
#define B_PMC_PWRM_DSX_CFG_ACPRES_PD_DSX_DIS                BIT1                        ///< AC_PRESENT pin pulldown in DeepSx disable
#define B_PMC_PWRM_DSX_CFG_LAN_WAKE_EN                      BIT0                        ///< LAN_WAKE Pin DeepSx Enable


#define R_PMC_PWRM_EN_SN_SLOW_RING                          0x1848                      ///< Enable Snoop Request to SLOW_RING
#define R_PMC_PWRM_EN_SN_SLOW_RING2                         0x184C                      ///< Enable Snoop Request to SLOW_RING 2nd Reg
#define R_PMC_PWRM_EN_SN_SA                                 0x1850                      ///< Enable Snoop Request to SA
#define R_PMC_PWRM_EN_SN_SA2                                0x1854                      ///< Enable Snoop Request to SA 2nd Reg
#define R_PMC_PWRM_EN_SN_SLOW_RING_CF                       0x1858                      ///< Enable Snoop Request to SLOW_RING_CF
#define R_PMC_PWRM_EN_NS_SA                                 0x1868                      ///< Enable Non-Snoop Request to SA
#define R_PMC_PWRM_EN_CW_SLOW_RING                          0x1880                      ///< Enable Clock Wake to SLOW_RING
#define R_PMC_PWRM_EN_CW_SLOW_RING2                         0x1884                      ///< Enable Clock Wake to SLOW_RING 2nd Reg
#define R_PMC_PWRM_EN_CW_SA                                 0x1888                      ///< Enable Clock Wake to SA
#define R_PMC_PWRM_EN_CW_SA2                                0x188C                      ///< Enable Clock Wake to SA 2nd Reg
#define R_PMC_PWRM_EN_CW_SLOW_RING_CF                       0x1898                      ///< Enable Clock Wake to SLOW_RING_CF
#define R_PMC_PWRM_EN_PA_SLOW_RING                          0x18A8                      ///< Enable Pegged Active to SLOW_RING
#define R_PMC_PWRM_EN_PA_SLOW_RING2                         0x18AC                      ///< Enable Pegged Active to SLOW_RING 2nd Reg
#define R_PMC_PWRM_EN_MISC_EVENT                            0x18C0                      ///< Enable Misc PM_SYNC Events
#define R_PMC_PWRM_PMSYNC_TPR_CONFIG                        0x18C4


#define R_PMC_PWRM_PM_SYNC_STATE_HYS                        0x18D0                      ///< PM_SYNC State Hysteresis
#define R_PMC_PWRM_PM_SYNC_MODE                             0x18D4                      ///< PM_SYNC Pin Mode


#define B_PMC_PWRM_CFG4_SLPS0_PSON_TMR                      (BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT23)  ///< SLPS0_PSON_TMR
#define N_PMC_PWRM_CFG4_SLPS0_PSON_TMR                      23
#define B_PMC_PWRM_CFG4_PSON_SLPS0_TMR                      (BIT22 | BIT21 | BIT20 | BIT19 | BIT18)                  ///< PSON_SLPS0_TMR
#define N_PMC_PWRM_CFG4_PSON_SLPS0_TMR                      18
#define V_PMC_PWRM_CFG4_PSON_SLPS0_TMR_620MS                0x1F
#define N_PMC_PWRM_CFG4_CPU_IOVR_RAMP_DUR                   0

#define R_PMC_PWRM_CPU_EPOC                                 0x18EC
#define N_PMC_PWRM_CPU_EPOC_DATA_17                         17
#define N_PMC_PWRM_CPU_EPOC_DATA_20                         20

#define R_PMC_PWRM_PM_SYNC_MODE_C0                          0x18F4
#define R_PMC_PWRM_ACPI_TMR_CTL                             0x18FC
#define B_PMC_PWRM_ACPI_TMR_CTL_DIS                         BIT1

#define R_PMC_PWRM_GPIO_CFG                                 0x1920
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW2                        (BIT11 | BIT10 | BIT9 | BIT8)
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW2                        8
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW1                        (BIT7 | BIT6 | BIT5 | BIT4)
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW1                        4
#define B_PMC_PWRM_GPIO_CFG_GPE0_DW0                        (BIT3 | BIT2 | BIT1 | BIT0)
#define N_PMC_PWRM_GPIO_CFG_GPE0_DW0                        0

#define R_PMC_PWRM_PMLDOCTRL                                0x1A00   ///< PMC LDO Control Register
#define B_PMC_PWRM_PMLDOCTRL_CNVIP24LDOLPEN                 BIT9
#define B_PMC_PWRM_PMLDOCTRL_USB2TS1P3LDODSEN               BIT0

#define R_PMC_PWRM_OBFF_CFG                                 0x1B14            ///< OBFF Configuration
#define R_PMC_PWRM_CPPM_MISC_CFG                            0x1B20            ///< CPPM Miscellaneous Configuration
#define R_PMC_PWRM_CPPM_CG_POL1A                            0x1B24            ///< CPPM Clock Gating Policy Reg 1
#define R_PMC_PWRM_CPPM_CG_POL2A                            0x1B40            ///< CPPM Clock Gating Policy Reg 3

#define R_PMC_PWRM_CPPM_CG_POL3A                            0x1BA8            ///< CPPM Clock Gating Policy Reg 5


#define R_PMC_PWRM_BM_CX_CNF                                0x1BDC           ///< in CNL this is PWRM register
#define B_PMC_PWRM_BM_CX_CNF_PHOLD_BM_STS_BLOCK             BIT14
#define B_PMC_PWRM_BM_CX_CNF_BM_STS_ZERO_EN                 BIT10
#define B_PMC_PWRM_BM_CX_CNF_PM_SYNC_MSG_MODE               BIT9

#define R_PMC_PWRM_THROT_1                                  0x1BE0
#define B_PMC_PWRM_THROT_1_VR_ALERT                         BIT0

#define R_PMC_PWRM_CS_SD_CTL1                               0x1BE8            ///< Clock Source Shutdown Control Reg 1
#define B_PMC_PWRM_CS_SD_CTL1_CS5_CTL_CFG                   (BIT22 | BIT21 | BIT20)    ///< Clock Source 5 Control Configuration
#define N_PMC_PWRM_CS_SD_CTL1_CS5_CTL_CFG                   20
#define B_PMC_PWRM_CS_SD_CTL1_CS1_CTL_CFG                   (BIT2 | BIT1 | BIT0)       ///< Clock Source 1 Control Configuration

#define R_PMC_PWRM_CS_SD_CTL2                               0x1BEC ///< Clock Source Shutdown Control Reg 2

#define R_PMC_PWRM_HSWPGCR1                                 0x1DD0
#define B_PMC_PWRM_SW_PG_CTRL_LOCK                          BIT31
#define B_PMC_PWRM_NPK_VNN_SW_PG_CTRL                       BIT0

#define R_PMC_PWRM_1E00                                     0x1E00
#define R_PMC_PWRM_1E04                                     0x1E04

#define R_PMC_PWRM_ST_PG_FDIS_PMC_1                         0x1E20 ///< Static PG Related Function Disable Register 1
#define B_PMC_PWRM_ST_PG_FDIS_PMC_1_ST_FDIS_LK              BIT31 ///< Static Function Disable Lock (ST_FDIS_LK)

#define R_PMC_PWRM_ST_PG_FDIS_PMC_2                         0x1E24 ///< Static Function Disable Control Register 2


#endif
