/** @file
  Register names for PCH LPC/eSPI device

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

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_REGS_ESPI_H_
#define _PCH_REGS_ESPI_H_

#define B_ESPI_CFG_DID                    0xFFE0

//
// eSPI controller PCI config registers
//
#define R_ESPI_CFG_ESPI_IOD_IOE           0x80                      ///< I/O Decode Ranges and I/O Enables
#define R_LPC_CFG_IOE                     0x82

#define R_LPC_CFG_BDE                     0xD8                      ///< BIOS decode enable

//
// APM Registers
//
#define R_PCH_IO_APM_CNT                  0xB2
#define R_PCH_IO_APM_STS                  0xB3

//
// Processor interface registers
//
#define R_PCH_IO_NMI_EN                   0x70
#define B_PCH_IO_NMI_EN_NMI_EN            BIT7

//
// Reset Generator I/O Port
//
#define R_PCH_IO_RST_CNT                  0xCF9
#define V_PCH_IO_RST_CNT_FULLRESET        0x0E
#define V_PCH_IO_RST_CNT_HARDRESET        0x06

#define R_ESPI_CFG_ESPI_BC                0xDC            ///< BIOS Control

//
// eSPI Responder registers
//
#define R_ESPI_DEVICE_CHA_0_CAP_AND_CONF  0x10            ///< Channel 0 Capabilities and Configurations
#define B_ESPI_DEVICE_BME                 BIT2            ///< Bus Requester Enable

//
// LPC PCR Registers
//
#define R_LPC_PCR_PRC                     0x341C

//
// eSPI PCR Registers
//
#define R_ESPI_PCR_SLV_CFG_REG_CTL                0x4000                  ///< Responder Configuration Register and Link Control
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRE           BIT31                   ///< Responder Configuration Register Access Enable
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRS           (BIT30 | BIT29 | BIT28) ///< Responder Configuration Register Access Status
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SCRS           28                      ///< Responder Configuration Register Access Status bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SBLCL          BIT27                   ///< IOSF-SB eSPI Link Configuration Lock
#define V_ESPI_PCR_SLV_CFG_REG_CTL_SCRS_NOERR     7                       ///< No errors (transaction completed successfully)
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SID            (BIT20 | BIT19)         ///< Responder ID
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SID            19                      ///< Responder ID bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRT           (BIT17 | BIT16)         ///< Responder Configuration Register Access Type
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SCRT           16                      ///< Responder Configuration Register Access Type bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRA           0x00000FFF              ///< Responder Configuration Register Address
#define R_ESPI_PCR_SLV_CFG_REG_DATA               0x4004                  ///< Responder Configuration Register Data

#define R_ESPI_PCR_PCERR_SLV0                     0x4020          ///< Peripheral Channel Error for Responder 0
#define R_ESPI_PCR_PCERR_SLV1                     0x4024          ///< Peripheral Channel Error for Responder 1
#define R_ESPI_PCR_VWERR_SLV0                     0x4030          ///< Virtual Wire Channel Error for Responder 0
#define B_ESPI_PCR_VWERR_SLV0_VWHC10OE            BIT25           ///< Requester HOST_C10 (Virtual Wire) to Responder Enable (VWHC10OE)
#define R_ESPI_PCR_VWERR_SLV1                     0x4034          ///< Virtual Wire Channel Error for Responder 1
#define R_ESPI_PCR_FCERR_SLV0                     0x4040          ///< Flash Access Channel Error for Responder 0
#define N_ESPI_PCR_FCERR_SLV0_FCFEE               5               ///< Flash Access Channel Fatal Error Reporting bit position
#define V_ESPI_PCR_FCERR_SLV0_FCFEE_SERR          2               ///< Enable Fatal Error Reporting as SERR
#define N_ESPI_PCR_FCERR_SLV0_FCNFEE              13              ///< Flash Access Channel Non-Fatal Error Reporting Enable bit position
#define V_ESPI_PCR_FCERR_SLV0_FCNFEE_SERR         2               ///< Enable Non-Fatal Error Reporting as SERR
#define B_ESPI_PCR_FCERR_SAFBLK                   BIT17           ///< SAF Blocked (SAFBLK)
#define R_ESPI_PCR_LNKERR_SLV0                    0x4050          ///< Link Error for Responder 0
#define S_ESPI_PCR_LNKERR_SLV0                    4               ///< Link Error for Responder 0 register size
#define B_ESPI_PCR_LNKERR_SLV0_SLCRR              BIT31           ///< eSPI Link and Responder Channel Recovery Required
#define B_ESPI_PCR_LNKERR_SLV0_LFET1E             (BIT22 | BIT21) ///< Fatal Error Type 1 Reporting Enable
#define N_ESPI_PCR_LNKERR_SLV0_LFET1E             21              ///< Fatal Error Type 1 Reporting Enable bit position
#define V_ESPI_PCR_LNKERR_SLV0_LFET1E_SMI         3               ///< Enable Fatal Error Type 1 Reporting as SMI
#define V_ESPI_PCR_LNKERR_SLV0_LFER1E_SERR        2               ///< Enable Fatal Error Type 1 Reporting as SERR
#define N_ESPI_PCR_LNKERR_SLV1_LFET1E             21              ///< Fatal Error Type 1 Reporting Enable bit position
#define V_ESPI_PCR_LNKERR_SLV1_LFET1E_SMI         3               ///< Enable Fatal Error Type 1 Reporting as SMI
#define V_ESPI_PCR_LNKERR_SLV1_LFER1E_SERR        2               ///< Enable Fatal Error Type 1 Reporting as SERR
#define B_ESPI_PCR_LNKERR_SLV0_LFET1S             BIT20           ///< Link Fatal Error Type 1 Status
#define R_ESPI_PCR_LNKERR_SLV1                    0x4054          ///< Link Error for Responder 1

#endif
