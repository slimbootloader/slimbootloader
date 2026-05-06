/** @file
  Register names for SPI device.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _SPI_REGS_H_
#define _SPI_REGS_H_

#define SPI_WAIT_PERIOD             10
#define SPI_WAIT_TIME               6000000

//
// SPI Registers
//
#define R_SPI_CFG_BIOS_SPI_UR_STS_CTL                0xD0         ///< SPI Unsupported Request Status
#define B_SPI_CFG_BIOS_SPI_UR_STS_CTL_URRE           BIT0         ///< Unsupported Request Reporting Enabled
#define R_SPI_CFG_BIOS_SPI_BDE                       0xD8         ///< BIOS Decode Enable
#define R_SPI_CFG_BIOS_SPI_BC                        0xDC         ///< BIOS Control
#define S_SPI_CFG_BIOS_SPI_BC                        4
#define B_SPI_CFG_BIOS_SPI_BC_EXT_BIOS_LOCK          BIT28        ///< Extended BIOS Range Lock
#define B_SPI_CFG_BIOS_SPI_BC_EXT_BIOS_EN            BIT27
#define N_SPI_CFG_BIOS_SPI_BC_EXT_BIOS_LIMIT_OFFSET  12
#define B_SPI_CFG_BIOS_SPI_BC_EXT_BIOS_LIMIT_OFFSET  0x7FFF000
#define N_SPI_CFG_BIOS_SPI_BC_ASE_BWP                11
#define B_SPI_CFG_BIOS_SPI_BC_ASE_BWP                BIT11        ///< Async SMI Enable for BIOS Write Protection
#define N_SPI_CFG_BIOS_SPI_BC_SPI_ASYNC_SS           10
#define B_SPI_CFG_BIOS_SPI_BC_SPI_ASYNC_SS           BIT10        ///< Asynchronous SMI Status
#define N_SPI_CFG_BIOS_SPI_BC_SPI_SYNC_SS            8
#define B_SPI_CFG_BIOS_SPI_BC_SPI_SYNC_SS            BIT8         ///< Synchronous SMI Status
#define B_SPI_CFG_BIOS_SPI_BC_BILD                   BIT7         ///< BIOS Interface Lock-Down
#define B_SPI_CFG_BIOS_SPI_BC_BBS                    BIT6         ///< Boot BIOS Strap
#define N_SPI_CFG_BIOS_SPI_BC_BBS                    6
#define V_SPI_CFG_BIOS_SPI_BC_BBS_SPI                0            ///< Boot BIOS strapped to SPI
#define B_SPI_CFG_BIOS_SPI_BC_EISS                   BIT5         ///< Enable InSMM.STS
#define B_SPI_CFG_BIOS_SPI_BC_TSS                    BIT4            ///< Top Swap Status
#define B_SPI_CFG_BIOS_SPI_BC_SRC                    (BIT3 | BIT2)   ///< SPI Read Configuration
#define N_SPI_CFG_BIOS_SPI_BC_SRC                    2
#define V_SPI_CFG_BIOS_SPI_BC_SRCC_PREF_DIS_CACHE_DIS   0x01         ///< No prefetching and no caching
#define B_SPI_CFG_BIOS_SPI_BC_LE                     BIT1         ///< Lock Enable
#define N_SPI_CFG_BIOS_SPI_BC_LE                     1
#define B_SPI_CFG_BIOS_SPI_BC_WPD                    BIT0         ///< Write Protect Disable
#define R_SPI_CFG_BIOS_SPI_BAR1                      0xE0         ///< SPI BAR1 MMIO
#define B_SPI_CFG_BIOS_SPI_BAR0_MEMBAR               0xFFFFF000

//
// BIOS Flash Program Registers (based on SPI_BAR0)
//
#define R_SPI_MEM_BIOS_BFPREG                        0x00         ///< BIOS Flash Primary Region
#define B_SPI_MEM_BIOS_BFPREG_PRL                    0x7FFF0000   ///< BIOS Flash Primary Region Limit
#define B_SPI_MEM_BIOS_BFPREG_PRB                    0x00007FFF   ///< BIOS Flash Primary Region Base
#define B_SPI_MEM_BIOS_BFPREG_SBRS                   BIT31        ///< Shadowed BIOS Region Select
#define R_SPI_MEM_BIOS_HSFSTS_CTL                    0x04         ///< Hardware Sequencing Flash Status and Control
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FDBC               (BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)                    ///< Flash Data Byte Count ( <= 64), Count = (Value in this field) + 1.
#define N_SPI_MEM_BIOS_HSFSTS_CTL_FDBC               24
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE             (BIT20 | BIT19 | BIT18 | BIT17 ) ///< Flash Cycle
#define N_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE             17
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_READ            0        ///< Flash Cycle Read
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_WRITE           2        ///< Flash Cycle Write
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_4K_ERASE        3        ///< Flash Cycle 4K Block Erase
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_64K_ERASE       4        ///< Flash Cycle 64K Sector Erase
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_READ_SFDP       5        ///< Flash Cycle Read SFDP
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_READ_JEDEC_ID   6        ///< Flash Cycle Read JEDEC ID
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_WRITE_STATUS    7        ///< Flash Cycle Write Status
#define V_SPI_MEM_BIOS_HSFSTS_CTL_FCYCLE_READ_STATUS     8        ///< Flash Cycle Read Status
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FGO                BIT16        ///< Flash Cycle Go.
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FLOCKDN            BIT15        ///< Flash Configuration Lock-Down
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FDV                BIT14        ///< Flash Descriptor Valid, once valid software can use hareware sequencing regs
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FDOPSS             BIT13        ///< Flash Descriptor Override Pin-Strap Status
#define B_SPI_MEM_BIOS_HSFSTS_CTL_WRSDIS             BIT11        ///< Write Status Disable
#define B_SPI_MEM_BIOS_HSFSTS_CTL_H_SAF_CE           BIT8         ///< SAF ctype error
#define B_SPI_MEM_BIOS_HSFSTS_CTL_H_SAF_MODE_ACTIVE  BIT7         ///< SAF Mode Active
#define B_SPI_MEM_BIOS_HSFSTS_CTL_H_SAF_LE           BIT6         ///< SAF link Error
#define B_SPI_MEM_BIOS_HSFSTS_CTL_H_SCIP             BIT5         ///< SPI Cycle In Progress
#define B_SPI_MEM_BIOS_HSFSTS_CTL_H_SAF_DLE          BIT4         ///< SAF Data length Error
#define B_SPI_MEM_BIOS_HSFSTS_CTL_H_SAF_ERROR        BIT3         ///< SAF Error
#define B_SPI_MEM_BIOS_HSFSTS_CTL_H_AEL              BIT2         ///< Access Error Log
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FCERR              BIT1         ///< Flash Cycle Error
#define B_SPI_MEM_BIOS_HSFSTS_CTL_FDONE              BIT0         ///< Flash Cycle Done
#define R_SPI_MEM_BIOS_FADDR                         0x08         ///< Flash Address
#define B_SPI_MEM_BIOS_FADDR_FLA                     0x07FFFFFF   ///< Flash Linear Address
#define R_SPI_MEM_BIOS_DLOCK                         0x0C         ///< Discrete Lock Bits
#define B_SPI_MEM_BIOS_DLOCK_PR0LOCKDN               BIT8         ///< PR0 Lock-Down
#define R_SPI_MEM_BIOS_FDATA0                        0x10         ///< Flash Data
#define R_SPI_MEM_BIOS_FRACC                         0x50         ///< Flash Region Access Permissions
#define B_SPI_MEM_BIOS_FRACC_BRWA                    (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8) ///< BIOS Region Write Access
#define N_SPI_MEM_BIOS_FRACC_BRWA                    8            ///< BIOS Region Write Access bit position
#define B_SPI_MEM_BIOS_FRACC_BRRA                    (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0) ///< BIOS Region Read Access
#define B_SPI_MEM_BIOS_FRACC_BMRAG                   (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16) ///< BIOS Host Read Access Grant
#define B_SPI_MEM_BIOS_FRACC_BMWAG                   (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24) ///< BIOS Host Write Access Grant
#define R_SPI_MEM_BIOS_FREG0                         0x54         ///< Flash Region
#define B_SPI_MEM_BIOS_FREG0_RB                      0x00007FFF   ///< Flash Region Base, [14:0] represents [26:12], applies for FREG1-FREG6
#define N_SPI_MEM_BIOS_FREG0_RB                      0            ///< Region base bit position, applies for FREG1-FREG6
#define B_SPI_MEM_BIOS_FREG0_RL                      0x7FFF0000   ///< Flash Region Limit [30:16] represents [26:12], [11:0] are assumed to be FFFh, applies for FREG1-FREG6
#define N_SPI_MEM_BIOS_FREG0_RL                      16           ///< Region limit bit position, applies for FREG1-FREG6
#define R_SPI_MEM_BIOS_FREG1                         0x58         ///< Flash Region
#define R_SPI_MEM_BIOS_FREG2                         0x5C         ///< Flash Region
#define R_SPI_MEM_BIOS_FREG3                         0x60         ///< Flash Region
#define R_SPI_MEM_BIOS_FREG4                         0x64         ///< Flash Region
#define R_SPI_MEM_BIOS_FREG5                         0x68         ///< Flash Region
#define R_SPI_MEM_BIOS_FREG6                         0x6C         ///< Flash Region
#define S_SPI_MEM_BIOS_FREGX                         4            ///< Size of Flash Region register
#define N_SPI_MEM_FREGX_BASE_REPR                    12           ///< Region base bit represents position
#define N_SPI_MEM_FREGX_LIMIT_REPR                   12           ///< Region limit bit represents position
#define R_SPI_MEM_BIOS_FPR0                          0x84         ///< Flash Protected Range
#define B_SPI_MEM_BIOS_FPR0_WPE                      BIT31        ///< Write Protection Enable, applies for FPR1-FPR4
#define B_SPI_MEM_BIOS_FPR0_PRL                      0x7FFF0000   ///< Protected Range Limit Mask, [30:16] here represents upper limit of address [26:12], applies for FPR1-FPR4
#define N_SPI_MEM_BIOS_FPR0_PRL                      16           ///< Protected Range Limit bit position, applies for FPR1-FPR4
#define B_SPI_MEM_BIOS_FPR0_RPE                      BIT15        ///< Read Protection Enable, applies for FPR1-FPR4
#define B_SPI_MEM_BIOS_FPR0_PRB                      0x00007FFF   ///< Protected Range Base Mask, [14:0] here represents base limit of address [26:12], applies for FPR1-FPR4
#define R_SPI_MEM_BIOS_FPR1                          0x88         ///< Flash Protected Range
#define R_SPI_MEM_BIOS_FPR2                          0x8C         ///< Flash Protected Range
#define R_SPI_MEM_BIOS_FPR3                          0x90         ///< Flash Protected Range
#define R_SPI_MEM_BIOS_FPR4                          0x94         ///< Flash Protected Range
#define S_SPI_MEM_BIOS_FPRX                          4            ///< Protected Region X Register size
#define R_SPI_MEM_BIOS_FDOC                          0xB4         ///< Flash Descriptor Observability Control
#define B_SPI_MEM_BIOS_FDOC_FDSS                     (BIT15 | BIT14 | BIT13 | BIT12)       ///< Flash Descritor Section Select
#define V_SPI_MEM_BIOS_FDOC_FDSS_FSDM                0x0000       ///< Flash Signature and Descriptor Map
#define V_SPI_MEM_BIOS_FDOC_FDSS_COMP                0x1000       ///< Component
#define V_SPI_MEM_BIOS_FDOC_FDSS_MSTR                0x3000       ///< Host
#define B_SPI_MEM_BIOS_FDOC_FDSI                     0xFFC       ///< Flash Descriptor Section Index
#define R_SPI_MEM_BIOS_FDOD                          0xB8         ///< Flash Descriptor Observability Data
#define B_SPI_MEM_FMRWA_MASK                         0xFFF00000   ///< Flash BIOS Region Host Write Access mask
#define B_SPI_MEM_FMRRA_MASK                         0x000FFF00   ///< Flash BIOS Region Host Read Access mask
#define R_SPI_MEM_BIOS_SFDP0_VSCC0                   0xC4         ///< Vendor Specific Component Capabilities for Component 0
#define B_SPI_MEM_BIOS_SFDP0_VSCC0_CPPTV             BIT31        ///< Component Property Parameter Table Valid, applies for SFDP1_VSCC1
#define B_SPI_MEM_BIOS_SFDP0_VSCC0_VCL               BIT30        ///< Vendor Component Lock
#define B_SPI_MEM_BIOS_SFDP0_VSCC0_EO_64K_VALID      BIT29        ///< 64k Erase valid (EO_64k_valid)
#define R_SPI_MEM_BIOS_SFDP1_VSCC1                   0xC8         ///< Vendor Specific Component Capabilities for Component 1
#define R_SPI_MEM_BIOS_FREG12                        0xE0         ///< Flash Region

//
// SPI Private Configuration Space Registers
//
#define R_SPI_PCR_SPI_CLK_PWR_GATE_CNTRL             0xC004       ///< SPI Clock and Deep Power Down Control
#define B_SPI_PCR_SPI_CLK_PWR_GATE_CNTRL_CDCPWRGTDIS BIT11        ///< CDC Power Gate Disable
#define B_SPI_PCR_SPI_CLK_PWR_GATE_CNTRL_CLKGATE_HOLDOFF_TYMOUT   (BIT19 | BIT18 | BIT17 | BIT16) ///< Used for the clocks driven out of the AON domain into the Power Gated domain
#define N_SPI_PCR_SPI_CLK_PWR_GATE_CNTRL_CLKGATE_HOLDOFF_TYMOUT   16
#define B_SPI_PCR_SPI_CLK_PWR_GATE_CNTRL_CNTLR_PWRGATE_TYMOUT     (BIT23 | BIT22 | BIT21 | BIT20) ///< Specifies the time a clock domain must be clock-gated before requesting Power Gate
#define N_SPI_PCR_SPI_CLK_PWR_GATE_CNTRL_CNTLR_PWRGATE_TYMOUT     20
#define R_SPI_PCR_SPI_PWRCNTRL_EN                    0xC008       ///< Power Control Enables
#define R_SPI_PCR_SPI_CONFIGURATION_VAL              0xC00C       ///< ESPI Enabled Strap
#define B_SPI_PCR_SPI_CONFIGURATION_VAL_ESPI_EN      BIT0         ///< ESPI Enabled Strap bit position
#define R_SPI_PCR_ESPI_SOFTSTRAP_REG0                0xC210       ///< eSPI Softstraps Register 0
#define B_SPI_PCR_ESPI_SOFTSTRAP_REG0_CS1_EN         BIT12        ///< CS1# Enable
#define R_SPI_PCR_ESPI_SOFTSTRAP_REG_EXT_2           0xC218       ///< eSPI Softstraps Register for eSPI CS2
#define B_SPI_PCR_ESPI_SOFTSTRAP_REG_EXT_2_CS_EN     BIT0         ///< CS# Enable, applies for REG_EXT_3
#define R_SPI_PCR_ESPI_SOFTSTRAP_REG_EXT_3           0xC21C       ///< eSPI Softstraps Register for eSPI CS3


#endif
