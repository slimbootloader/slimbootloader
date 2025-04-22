/** @file
  Register names for SPI device.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, SC registers are denoted by "_SC_" in register names
  - Registers / bits that are different between SC generations are denoted by
    "_SC_<generation_name>_" in register/bit names.
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SC generation will be just named
    as "_SC_" without <generation_name> inserted.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REGS_SPI_H_
#define _REGS_SPI_H_



#define R_SPI_ID                        0x00           ///< Identifiers
#define R_SPI_COMMAND                   0x04           ///< Command
#define R_SPI_BASE                      0x10           ///< 32-bit Memory Base Address Register
#define B_SPI_BASE_BAR                  0xFFFFF000     ///< MEMBAR
#define B_SPI_BAR0_MASK                 0x0FFF
#define B_SPI_BASE_PREF                 BIT3           ///< Prefetchable
#define B_SPI_BASE_MEMI                 BIT0           ///< Memory Space Indicator
#define R_SPI_BDE                       0xD8           ///< BIOS Decode Enable
#define R_SPI_BCR                       0xDC           ///< BIOS Control  Register
#define S_SPI_BCR                       4
#define B_SPI_BC_OSFH                   BIT9           ///< OS Function Hide
#define B_SPI_BC_BILD                   BIT7
#define B_SPI_BC_BBS                    BIT6           ///< Boot BIOS strap
#define N_SPI_BC_BBS                    6
#define V_SPI_BC_BBS_SPI                0              ///< Boot BIOS strapped to SPI
#define V_SPI_BC_BBS_LPC                1              ///< Boot BIOS strapped to LPC
#define B_SPI_BCR_SMM_BWP               BIT5           ///< Enable InSMM.STS
#define B_SPI_BCR_SRC                    (BIT3 | BIT2) ///< SPI Read Configuration (SRC)
#define V_SPI_BCR_SRC_PREF_EN_CACHE_EN   0x08          ///< Prefetch Enable, Cache Enable
#define V_SPI_BCR_SRC_PREF_DIS_CACHE_DIS 0x04          ///< Prefetch Disable, Cache Disable
#define V_SPI_BCR_SRC_PREF_DIS_CACHE_EN  0x00          ///< Prefetch Disable, Cache Enable
#define N_SPI_BCR_SYNC_SS                8
#define B_SPI_BCR_SYNC_SS                BIT8
#define B_SPI_BCR_BILD                   BIT7
#define B_SPI_BCR_EISS                   BIT5          ///< Enable InSMM.STS
#define B_SPI_BCR_BLE                    BIT1          ///< Lock Enable (LE)
#define B_SPI_BCR_BIOSWE                 BIT0          ///< Write Protect Disable (WPD)
#define N_SPI_BCR_BLE                    1
#define N_SPI_BCR_BIOSWE                 0

///
/// SPI Host Interface Registers
#define R_SPI_BFPR                       0x00       ///< BIOS Flash Primary Region Register (32bits)
#define B_SPI_BFPR_PRL                   0x7FFF0000 ///< BIOS Flash Primary Region Limit
#define B_SPI_BFPR_PRB                   0x7FFF     ///< BIOS Flash Primary Region Base
#define R_SPI_HSFS                       0x04          ///< Hardware Sequencing Flash Status and Control Register(32bits)
#define B_SPI_HSFS_FSMIE                 BIT31         ///< Flash SPI SMI# Enable
#define B_SPI_HSFS_FDBC_MASK             0x3F000000    ///< Flash Data Byte Count ( <= 64), Count = (Value in this field) + 1.
#define N_SPI_HSFS_FDBC                  24
#define B_SPI_HSFS_CYCLE_MASK            0x001E0000    ///< Flash Cycle.
#define N_SPI_HSFS_CYCLE                 17
#define V_SPI_HSFS_CYCLE_READ            0             ///< Flash Cycle Read
#define V_SPI_HSFS_CYCLE_WRITE           2             ///< Flash Cycle Write
#define V_SPI_HSFS_CYCLE_4K_ERASE        3             ///< Flash Cycle 4K Block Erase
#define V_SPI_HSFS_CYCLE_64K_ERASE       4             ///< Flash Cycle 64K Sector Erase
#define V_SPI_HSFS_CYCLE_READ_SFDP       5             ///< Flash Cycle Read SFDP
#define V_SPI_HSFS_CYCLE_READ_JEDEC_ID   6             ///< Flash Cycle Read JEDEC ID
#define V_SPI_HSFS_CYCLE_WRITE_STATUS    7             ///< Flash Cycle Write Status
#define V_SPI_HSFS_CYCLE_READ_STATUS     8             ///< Flash Cycle Read Status
#define B_SPI_HSFS_CYCLE_FGO             BIT16         ///< Flash Cycle Go.
#define B_SPI_HSFS_FLOCKDN               BIT15         ///< Flash Configuration Lock-Down
#define B_SPI_HSFS_FDV                   BIT14         ///< Flash Descriptor Valid
#define B_SPI_HSFS_FDOPSS                BIT13         ///< Flash Descriptor Override Pin-Strap Status
#define B_SPI_HSFS_SCIP                  BIT5          ///< SPI Cycle in Progress
#define B_SPI_HSFS_BERASE_MASK           (BIT4 | BIT3) ///< Block/Sector Erase Size
#define V_SPI_HSFS_BERASE_256B           0//0x00       ///< Block/Sector = 256 Bytes
#define V_SPI_HSFS_BERASE_4K             1//0x01       ///< Block/Sector = 4K Bytes
#define V_SPI_HSFS_BERASE_8K             2//0x10       ///< Block/Sector = 8K Bytes
#define V_SPI_HSFS_BERASE_64K            3//0x11       ///< Block/Sector = 64K Bytes
#define B_SPI_HSFS_AEL                   BIT2          ///< Access Error Log
#define B_SPI_HSFS_FCERR                 BIT1          ///< Flash Cycle Error
#define B_SPI_HSFS_FDONE                 BIT0          ///< Flash Cycle Done

#define R_SPI_HSFC                       0x06  ///< Hardware Sequencing Flash Control Register (16bits)
#define B_SPI_HSFC_FSMIE                 BIT15 ///< Flash SPI SMI# Enable
#define B_SPI_HSFC_FDBC_MASK             0x3F00 ///< Flash Data Byte Count ( <= 64), Count = (Value in this field) + 1.
#define B_SPI_HSFC_FCYCLE_MASK           0x001E ///< Flash Cycle.
#define V_SPI_HSFC_FCYCLE_READ           0     ///< Flash Cycle Read
#define V_SPI_HSFC_FCYCLE_WRITE          2     ///< Flash Cycle Write
#define V_SPI_HSFC_FCYCLE_ERASE          3     ///< Flash Cycle 4k Block Erase
#define V_SPI_HSFC_FCYCLE_SERASE         4     ///< Flash Cycle 64k Sector Erase
#define B_SPI_HSFC_FCYCLE_FGO            BIT0  ///< Flash Cycle Go.

#define R_SPI_FADDR                      0x08  ///< SPI Flash Address
#define B_SPI_FADDR_MASK                 0x07FFFFFF ///< SPI Flash Address Mask (0~26bit)

#define R_SPI_DLOCK                     0x0C   ///< Discrete Lock Bits
#define B_SPI_DLOCK_PR0LOCKDN           BIT8   ///< PR0LOCKDN

#define R_SPI_FDATA00                    0x10  ///< SPI Data 00 (32 bits)
#define R_SPI_FDATA01                    0x14  ///< SPI Data 01
#define R_SPI_FDATA02                    0x18  ///< SPI Data 02
#define R_SPI_FDATA03                    0x1C  ///< SPI Data 03
#define R_SPI_FDATA04                    0x20  ///< SPI Data 04
#define R_SPI_FDATA05                    0x24  ///< SPI Data 05
#define R_SPI_FDATA06                    0x28  ///< SPI Data 06
#define R_SPI_FDATA07                    0x2C  ///< SPI Data 07
#define R_SPI_FDATA08                    0x30  ///< SPI Data 08
#define R_SPI_FDATA09                    0x34  ///< SPI Data 09
#define R_SPI_FDATA10                    0x38  ///< SPI Data 10
#define R_SPI_FDATA11                    0x3C  ///< SPI Data 11
#define R_SPI_FDATA12                    0x40  ///< SPI Data 12
#define R_SPI_FDATA13                    0x44  ///< SPI Data 13
#define R_SPI_FDATA14                    0x48  ///< SPI Data 14
#define R_SPI_FDATA15                    0x4C  ///< SPI Data 15

#define R_SPI_FRAP                       0x50  ///< SPI Flash Regions Access Permissions Register
#define B_SPI_FRAP_BMWAG_MASK            0xFF000000 ///< Master Write Access Grant MASK
#define B_SPI_FRAP_BMRAG_MASK            0x00FF0000 ///< Master Read Access Grant Grant MASK
#define B_SPI_FRAP_BRWA_MASK             0x0000FF00 ///< BIOS Region Write Access MASK
#define B_SPI_FRAP_BRWA_PLATFORM         BIT12 //< Region write access for Region4 PlatformData
#define B_SPI_FRAP_BRWA_GBE              BIT11 //< Region write access for Region3 GbE
#define B_SPI_FRAP_BRWA_SEC              BIT10 ///< Region Write Access for Region2 SEC
#define B_SPI_FRAP_BRWA_BIOS             BIT9  ///< Region Write Access for Region1 BIOS
#define B_SPI_FRAP_BRWA_FLASHD           BIT8  ///< Region Write Access for Region0 Flash Descriptor
#define B_SPI_FRAP_BRRA_MASK             0x000000FF ///< BIOS Region Read Access MASK
#define B_SPI_FRAP_BRRA_PLATFORM         BIT4       ///< Region read access for Region4 PlatformData
#define B_SPI_FRAP_BRRA_GBE              BIT3       ///< Region read access for Region3 GbE
#define B_SPI_FRAP_BRRA_SEC              BIT2       ///< Region Read Access for Region2 SEC
#define B_SPI_FRAP_BRRA_BIOS             BIT1       ///< Region Read Access for Region1 BIOS
#define B_SPI_FRAP_BRRA_FLASHD           BIT0       ///< Region Read Access for Region0 Flash Descriptor

#define V_SPI_FLREG_DISABLED            0x00007FFF  ////< Region Base all 1's and Limits all 0's indicates the region is disabled.

#define R_SPI_FREG0_FLASHD               0x54       ///< Flash Region 0 (Flash Descriptor) (32bits)
#define B_SPI_FREG0_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define N_SPI_FREG0_LIMIT                4          ///< Bit 30:16 identifies address bits [26:12]
#define B_SPI_FREG0_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]
#define N_SPI_FREG0_BASE                 12         ///< Bit 14:0 identifies address bits [26:2]

#define R_SPI_FREG1_BIOS                 0x58       ///< Flash Region 1 (BIOS) (32bits)
#define B_SPI_FREG1_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define N_SPI_FREG1_LIMIT                4          ///< Bit 30:16 identifies address bits [26:12]
#define B_SPI_FREG1_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]
#define N_SPI_FREG1_BASE                 12         ///< Bit 14:0 identifies address bits [26:2]

#define R_SPI_FREG2_SEC                  0x5C       ///< Flash Region 2 (SEC) (32bits)
#define B_SPI_FREG2_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define N_SPI_FREG2_LIMIT                4          //< Bit 30:16 identifies address bits [26:12]
#define B_SPI_FREG2_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]
#define N_SPI_FREG2_BASE                 12         //< Bit 14:0 identifies address bits [26:2]

#define R_SPI_FREG3_GBE                  0x60       //< Flash Region 3(GbE)(32bits)
#define B_SPI_FREG3_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define N_SPI_FREG3_LIMIT                4          //< Bit 30:16 identifies address bits [26:12]
#define B_SPI_FREG3_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]
#define N_SPI_FREG3_BASE                 12         //< Bit 14:0 identifies address bits [26:2]

#define R_SPI_FREG4_PLATFORM_DATA        0x64       ///< Flash Region 4 (Platform Data) (32bits)
#define B_SPI_FREG4_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define N_SPI_FREG4_LIMIT                4          ///< Bit 30:16 identifies address bits [26:12]
#define B_SPI_FREG4_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]
#define N_SPI_FREG4_BASE                 12         ///< Bit 14:0 identifies address bits [26:2]

#define R_SPI_FREG5_DEVICE_EXPANSION_1   0x68       ///< Flash Region 5 (Device Expansion) (32bits)
#define B_SPI_FREG5_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define B_SPI_FREG5_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]

#define R_SPI_FREG6_SECONDARY_BIOS       0x6C       ///< Flash Region 6 (Secondary BIOS) (32bits)
#define B_SPI_FREG6_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define B_SPI_FREG6_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]

#define R_SPI_FREG7_UCODE_PATCH          0x70       ///< Flash Region 7 (uCode Patch) (32bits)
#define B_SPI_FREG7_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define B_SPI_FREG7_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]

#define R_SPI_FREG8_EC                   0x74       ///< Flash Region 8 (Embedded Controller) (32bits)
#define B_SPI_FREG8_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define B_SPI_FREG8_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]

#define R_SPI_FREG9_DEVICE_EXPANSION_2   0x78       ///< Flash Region 9 (Device Expansion 2) (32bits)
#define B_SPI_FREG9_LIMIT_MASK           0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define B_SPI_FREG9_BASE_MASK            0x00007FFF ///< Base, [14:0]  here represents base [26:12]

#define R_SPI_FREG10_IE_ENGINE           0x7c       ///< Flash Region 10 (IE Innovation Engine) (32bits)
#define B_SPI_FREG10_LIMIT_MASK          0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define B_SPI_FREG10_BASE_MASK           0x00007FFF ///< Base, [14:0]  here represents base [26:12]

#define R_SPI_FREG11_10_GBE_A            0x80       ///< Flash Region 11 (10 GBE A) (32bits)
#define B_SPI_FREG11_LIMIT_MASK          0x7FFF0000 ///< Size, [30:16] here represents limit[26:12]
#define B_SPI_FREG11_BASE_MASK           0x00007FFF ///< Base, [14:0]  here represents base [26:12]

#define S_SPI_FREGX                      4          ///< Size of Flash Region register
#define B_SPI_FREGX_LIMIT_MASK           0x7FFF0000 ///< Flash Region Limit [30:16] represents [26:12], [11:0] are assumed to be FFFh
#define N_SPI_FREGX_LIMIT                16         ///< Region limit bit position
#define N_SPI_FREGX_LIMIT_REPR           12         ///< Region limit bit represents position
#define B_SPI_FREGX_BASE_MASK            0x00007FFF ///< Flash Region Base, [14:0] represents [26:12]
#define N_SPI_FREGX_BASE                 0          ///< Region base bit position
#define N_SPI_FREGX_BASE_REPR            12         ///< Region base bit represents position

#define R_SPI_PR0                        0x84  ///< Protected Region 0 Register
#define B_SPI_PR0_WPE                    BIT31 ///< Write Protection Enable
#define B_SPI_PR0_PRL_MASK               0x7FFF0000 ///< Protected Range Limit Mask, [30:16] here represents upper limit of address [26:12]
#define B_SPI_PR0_RPE                    BIT15 ///< Read Protection Enable
#define B_SPI_PR0_PRB_MASK               0x00007FFF ///< Protected Range Base Mask, [14:0] here represents base limit of address [26:12]

#define R_SPI_PR1                        0x88  ///< Protected Region 1 Register
#define B_SPI_PR1_WPE                    BIT31 ///< Write Protection Enable
#define B_SPI_PR1_PRL_MASK               0x7FFF0000 ///< Protected Range Limit Mask
#define B_SPI_PR1_RPE                    BIT15 ///< Read Protection Enable
#define B_SPI_PR1_PRB_MASK               0x00007FFF ///< Protected Range Base Mask

#define R_SPI_PR2                        0x8C  ///< Protected Region 2 Register
#define B_SPI_PR2_WPE                    BIT31 ///< Write Protection Enable
#define B_SPI_PR2_PRL_MASK               0x7FFF0000 ///< Protected Range Limit Mask
#define B_SPI_PR2_RPE                    BIT15 ///< Read Protection Enable
#define B_SPI_PR2_PRB_MASK               0x00007FFF ///< Protected Range Base Mask

#define R_SPI_PR3                        0x90  ///< Protected Region 3 Register
#define B_SPI_PR3_WPE                    BIT31 ///< Write Protection Enable
#define B_SPI_PR3_PRL_MASK               0x7FFF0000 ///< Protected Range Limit Mask
#define B_SPI_PR3_RPE                    BIT15 ///< Read Protection Enable
#define B_SPI_PR3_PRB_MASK               0x00007FFF ///< Protected Range Base Mask

#define R_SPI_PR4                        0x94  ///< Protected Region 4 Register
#define B_SPI_PR4_WPE                    BIT31 ///< Write Protection Enable
#define B_SPI_PR4_PRL_MASK               0x7FFF0000 ///< Protected Range Limit Mask
#define B_SPI_PR4_RPE                    BIT15 ///< Read Protection Enable
#define B_SPI_PR4_PRB_MASK               0x00007FFF ///< Protected Range Base Mask

#define S_SPI_PRX                        4     ///< Protected Region X Register size
#define B_SPI_PRX_WPE                    BIT31 ///< Write Protection Enable
#define B_SPI_PRX_PRL_MASK               0x7FFF0000 ///< Protected Range Limit Mask, [30:16] here represents upper limit of address [26:12]
#define N_SPI_PRX_PRL                    16    ///< Protected Range Limit bit position
#define B_SPI_PRX_RPE                    BIT15 ///< Read Protection Enable
#define B_SPI_PRX_PRB_MASK               0x00007FFF ///< Protected Range Base Mask, [14:0] here represents base limit of address [26:12]
#define N_SPI_PRX_PRB                    0     ///< Protected Range Base bit position

#define R_SPI_GPR0                       0x98  ///< Global Protected Range 0 Register
#define B_SPI_GPR0_WPE                   BIT31 ///< Write Protection Enable
#define B_SPI_GPR0_PRL_MASK              0x7FFF0000 ///< Protected Range Limit Mask
#define B_SPI_GPR0_RPE                   BIT15 ///< Read Protection Enable
#define B_SPI_GPR0_PRB_MASK              0x00007FFF ///< Protected Range Base Mask

#define R_SPI_SSFCS                      0xA0  ///< Software Sequencing Flash Control Status Register
#define B_SPI_SSFCS_SCF_MASK             (BIT26 | BIT25 | BIT24) ///< SPI Cycle Frequency
#define V_SPI_SSFCS_SCF_48MHZ            2     ///< SPI Cycle Frequency = 48MHz
#define V_SPI_SSFCS_SCF_30MHZ            4     ///< SPI Cycle Frequency = 30MHz
#define V_SPI_SSFCS_SCF_17MHZ            6     ///< SPI Cycle Frequency = 17MHz

#define B_SPI_SSFCS_SME                  BIT23 ///< SPI SMI# Enable
#define B_SPI_SSFCS_DC                   BIT22 ///< SPI Data Cycle
#define B_SPI_SSFCS_DBC_MASK             0x3F0000 ///< SPI Data Byte Count (value here + 1 = count)
#define B_SPI_SSFCS_COP                  0x7000 ///< Cycle Opcode Pointer
#define B_SPI_SSFCS_SPOP                 BIT11 ///< Sequence Prefix Opcode Pointer
#define B_SPI_SSFCS_ACS                  BIT10 ///< Atomic Cycle Sequence
#define B_SPI_SSFCS_SCGO                 BIT9  ///< SPI Cycle Go
#define B_SPI_SSFCS_FRS                  BIT7  ///< Fast Read Supported
#define B_SPI_SSFCS_DOFRS                BIT6  ///< Dual Output Fast Read Supported
#define B_SPI_SSFCS_AEL                  BIT4  ///< Access Error Log
#define B_SPI_SSFCS_FCERR                BIT3  ///< Flash Cycle Error
#define B_SPI_SSFCS_CDS                  BIT2  ///< Cycle Done Status
#define B_SPI_SSFCS_SCIP                 BIT0  ///< SPI Cycle in Progress

#define R_SPI_PREOP                      0xA4  ///< Prefix Opcode Configuration Register (16 bits)
#define B_SPI_PREOP1_MASK                0xFF00 ///< Prefix Opcode 1 Mask
#define B_SPI_PREOP0_MASK                0x00FF ///< Prefix Opcode 0 Mask

#define R_SPI_OPTYPE                     0xA6  ///< Opcode Type Configuration
#define B_SPI_OPTYPE7_MASK               (BIT15 | BIT14) ///< Opcode Type 7 Mask
#define B_SPI_OPTYPE6_MASK               (BIT13 | BIT12) ///< Opcode Type 6 Mask
#define B_SPI_OPTYPE5_MASK               (BIT11 | BIT10) ///< Opcode Type 5 Mask
#define B_SPI_OPTYPE4_MASK               (BIT9 | BIT8) ///< Opcode Type 4 Mask
#define B_SPI_OPTYPE3_MASK               (BIT7 | BIT6) ///< Opcode Type 3 Mask
#define B_SPI_OPTYPE2_MASK               (BIT5 | BIT4) ///< Opcode Type 2 Mask
#define B_SPI_OPTYPE1_MASK               (BIT3 | BIT2) ///< Opcode Type 1 Mask
#define B_SPI_OPTYPE0_MASK               (BIT1 | BIT0) ///< Opcode Type 0 Mask
#define V_SPI_OPTYPE_RDNOADDR            0x00  ///< Read cycle type without address
#define V_SPI_OPTYPE_WRNOADDR            0x01  ///< Write cycle type without address
#define V_SPI_OPTYPE_RDADDR              0x02  ///< Address required; Read cycle type
#define V_SPI_OPTYPE_WRADDR              0x03  ///< Address required; Write cycle type

#define R_SPI_OPMENU0                    0xA8  ///< Opcode Menu Configuration 0 (32bits)
#define R_SPI_OPMENU1                    0xAC  ///< Opcode Menu Configuration 1 (32bits)

#define R_SPI_SFRAP                       0xB0  ///< Secondary Flash Region Access Permissions (32 bits)

#define R_SPI_FDOC                       0xB4  ///< Flash Descriptor Observability Control Register (32 bits)
#define B_SPI_FDOC_FDSS_MASK             (BIT14 | BIT13 | BIT12) ///< Flash Descriptor Section Select
#define V_SPI_FDOC_FDSS_FSDM             0x0000 ///< Flash Signature and Descriptor Map
#define V_SPI_FDOC_FDSS_COMP             0x1000 ///< Component
#define V_SPI_FDOC_FDSS_REGN             0x2000 ///< Region
#define V_SPI_FDOC_FDSS_MSTR             0x3000 ///< Master
#define V_SPI_FDOC_FDSS_STRP             0x4000 ///< Soft Straps
#define B_SPI_FDOC_FDSI_MASK             0x0FFC ///< Flash Descriptor Section Index

#define R_SPI_FDOD                       0xB8  ///< Flash Descriptor Observability Data Register (32 bits)

#define R_SPI_AFC                        0xC0  ///< Additional Flash Control Register
#define B_SPI_AFC_RRWSP                  0xFE ///< Reserved RW Scratch Pad bit [7:1]
#define B_SPI_AFC_SPFP                   BIT0  ///< Stop Prefetch on Flush Pending

#define R_SPI_LVSCC                      0xC4  ///<Vendor Specific Component Capabilities for Component 0 (32 bits)
#define B_SPI_LVSCC_VCL                  BIT30 ///< Vendor Component Lock
#define B_SPI_LVSCC_EO_64K               BIT29 ///<< 64k Erase valid (EO_64k_valid)
#define B_SPI_LVSCC_64K_EO_MASK          0x00FF0000 ///< 64k Erase Opcode
#define B_SPI_LVSCC_EO_MASK              0x0000FF00 ///<4k Erase Opcode
#define B_SPI_LVSCC_WEWS                 BIT4  ///< Write Enable on Write Status
#define B_SPI_LVSCC_WSR                  BIT3  ///< Write Status Required
#define B_SPI_LVSCC_WG_64B               BIT2  ///< Write Granularity, 0: Reserved; 1: 64 Bytes

#define R_SPI_UVSCC                      0xC8  ///< Vendor Specific Component Capabilities for Component 1 (32 bits)
#define B_SPI_UVSCC_64K_EO_MASK          0x00FF0000 ///< Erase Opcode
#define B_SPI_UVSCC_EO_MASK              0x0000FF00 ///< Erase Opcode
#define B_SPI_UVSCC_WEWS                 BIT4  ///< Write Enable on Write Status
#define B_SPI_UVSCC_WSR                  BIT3  ///< Write Status Required
#define B_SPI_UVSCC_WG_64B               BIT2  ///< Write Granularity, 0: 1 Byte; 1: 64 Bytes

#define R_SPI_PTI                        0xCC  ///< Parameter Table Index
#define R_SPI_PTD                        0xD0  ///< Parameter Table Data
#define R_SPI_BRS                        0xD4  ///< SPI Bus Requester Status

//
// Flash Descriptor Base Address Region (FDBAR) from Flash Region 0
//
#define R_SPI_FDBAR_FLVALSIG             0x10  ///< Flash Valid Signature
#define V_SPI_FDBAR_FLVALSIG             0x0FF0A55A

#define R_SPI_FDBAR_FLASH_MAP0           0x14  ///< Flash MAP 0
#define B_SPI_FDBAR_FRBA                 0x00FF0000 ///< Flash Region Base Address
#define N_SPI_FDBAR_NC                   8       ///<< Number Of Components
#define B_SPI_FDBAR_NC                   0x00000300 ///< Number Of Components
#define V_SPI_FDBAR_NC_2                 0x00000100
#define V_SPI_FDBAR_NC_1                 0x00000000
#define B_SPI_FDBAR_FCBA                 0x000000FF ///< Flash Component Base Address

#define R_SPI_FDBAR_FLASH_MAP1           0x18  ///< Flash MAP 1
#define B_SPI_FDBAR_PSL                  0xFF000000 ///< Strap Length
#define B_SPI_FDBAR_FPSBA                0x00FF0000 ///< Flash Strap Base Address
#define B_SPI_FDBAR_NM                   0x00000700 ///< Number Of Masters
#define B_SPI_FDBAR_FMBA                 0x000000FF ///< Flash Master Base Address

#define R_SPI_FDBAR_FLASH_MAP2           0x1C  ///< Flash Map 2
#define B_SPI_FDBAR_RIL                  0xFF000000 ///< ICC Register Init Length
#define B_SPI_FDBAR_RIBA                 0x00FF0000 ///< ICC Register Init Base Address
#define B_SPI_FDBAR_CPUSL                0x0000FF00 ///< CPU Strap Length
#define B_SPI_FDBAR_FCPUSBA              0x000000FF ///< Flash CPU Strap Base Address

//
// Flash Component Base Address (FCBA) from Flash Region 0
//
#define R_SPI_FCBA_FLCOMP                0x00  ///< Flash Components Register
#define B_SPI_FLCOMP_DOFRS               BIT30 ///< Dual Output Fast Read Support
#define B_SPI_FLCOMP_RIDS_FREQ           (BIT29 | BIT28 | BIT27) ///< Read ID and Read Status Clock Frequency
#define B_SPI_FLCOMP_WE_FREQ             (BIT26 | BIT25 | BIT24) ///< Write and Erase Clock Frequency
#define B_SPI_FLCOMP_FRCF_FREQ           (BIT23 | BIT22 | BIT21) ///< Fast Read Clock Frequency
#define B_SPI_FLCOMP_FR_SUP              BIT20 ///< Fast Read Support.
#define B_SPI_FLCOMP_RC_FREQ             (BIT19 | BIT18 | BIT17) ///< Read Clock Frequency.
#define B_SPI_FLCOMP_COMP1_MASK          0x0F  ///< Flash Component 1 Density
#define V_SPI_FLCOMP_COMP1_512KB         0x00
#define V_SPI_FLCOMP_COMP1_1MB           0x01
#define V_SPI_FLCOMP_COMP1_2MB           0x02
#define V_SPI_FLCOMP_COMP1_4MB           0x03
#define V_SPI_FLCOMP_COMP1_8MB           0x04
#define V_SPI_FLCOMP_COMP1_16MB          0x05
#define V_SPI_FLCOMP_COMP1_32MB          0x06
#define V_SPI_FLCOMP_COMP1_64MB          0x07

///
/// Descriptor Upper Map Section from Flash Region 0
///
#define R_SPI_FLASH_UMAP1                0xEFC ///< Flash Upper Map 1
#define B_SPI_FLASH_UMAP1_VTL            0x0000FF00 ///< VSCC Table Length
#define B_SPI_FLASH_UMAP1_VTBA           0x000000FF ///< VSCC Table Base Address

#define R_SPI_VTBA_JID0                  0x00  ///< JEDEC-ID 0 Register
#define S_SPI_VTBA_JID0                  4
#define B_SPI_VTBA_JID0_DID1             0x00FF0000 ///< SPI Component Device ID 1
#define N_SPI_VTBA_JID0_DID1             0x10
#define B_SPI_VTBA_JID0_DID0             0x0000FF00 ///< SPI Component Device ID 0
#define N_SPI_VTBA_JID0_DID0             0x08
#define B_SPI_VTBA_JID0_VID              0x000000FF ///< SPI Component Vendor ID

#define R_SPI_VTBA_VSCC0                 0x04  ///< Vendor Specific Component Capabilities 0
#define S_SPI_VTBA_VSCC0                 4
#define B_SPI_VTBA_VSCC0_UCAPS           0xFFFF0000
#define B_SPI_VTBA_VSCC0_LCAPS           0x0000FFFF
#define B_SPI_VTBA_VSCC0_EO              0x0000FF00 ///< Erase Opcode
#define B_SPI_VTBA_VSCC0_WEWS            BIT4  ///< Write Enable on Write Status
#define B_SPI_VTBA_VSCC0_WSR             BIT3  ///< Write Status Required
#define B_SPI_VTBA_VSCC0_WG              BIT2  ///< Write Granularity
#define B_SPI_VTBA_VSCC0_BES             (BIT1 | BIT0) ///< Block / Sector Erase Size

#endif
