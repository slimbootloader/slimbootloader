/** @file
  Register names for PCH LPC/eSPI device

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_LPC_H_
#define _PCH_REGS_LPC_H_


#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define PCI_DEVICE_NUMBER_IOE_PMC       19
#define PCI_FUNCTION_NUMBER_IOE_PMC     2
//typedef UINT16 PCH_STEPPING;
#define R_LPC_CFG_IOD                             0x80
#define N_LPC_CFG_IOD_COMB                        4
#define V_LPC_CFG_IOD_COMB_2F8                    1
#define N_LPC_CFG_IOD_COMA                        0
#define V_LPC_CFG_IOD_COMA_3F8                    0
#define R_LPC_CFG_IOE                             0x82
#define B_LPC_CFG_IOE_ME1                         BIT11           ///< Microcontroller Enable #1, Enables decoding of I/O locations 62h and 66h to LPC.
#define B_LPC_CFG_IOE_CBE                         BIT1            ///< Com Port B Enable, Enables decoding of the COMB range to LPC. Range is selected LIOD.CB.
#define B_LPC_CFG_IOE_CAE                         BIT0            ///< Com Port A Enable, Enables decoding of the COMA range to LPC. Range is selected LIOD.CA.
#define B_LPC_CFG_GENX_DEC_EN                     0x00000001
#define B_LPC_CFG_GENX_DEC_EN                     0x00000001

#define R_LPC_CFG_BDE                             0xD8                          ///< BIOS decode enable


//
// APM Registers
//
#define R_PCH_IO_APM_CNT                             0xB2
#define R_PCH_IO_APM_STS                             0xB3
#define R_LPC_CFG_BC                              0xDC            ///< Bios Control


//
//
// eSPI Responder registers
//
#define R_ESPI_DEVICE_CHA_0_CAP_AND_CONF           0x10            ///< Channel 0 Capabilities and Configurations
#define B_ESPI_DEVICE_BME                          BIT2            ///< Bus Requester Enable

// Processor interface registers
//
#define R_PCH_IO_NMI_EN                              0x70
#define B_PCH_IO_NMI_EN_NMI_EN                       BIT7

#define R_PCH_IO_RST_CNT                             0xCF9
#define V_PCH_IO_RST_CNT_FULLRESET                   0x0E
#define V_PCH_IO_RST_CNT_HARDRESET                   0x06

#define R_RTC_IO_INDEX                           0x70
#define R_RTC_IO_TARGET                          0x71
#define R_RTC_IO_EXT_INDEX                       0x72
#define R_RTC_IO_EXT_TARGET                      0x73
#define R_RTC_IO_INDEX_ALT                       0x74
#define R_RTC_IO_TARGET_ALT                      0x75
#define R_RTC_IO_EXT_INDEX_ALT                   0x76
#define R_RTC_IO_EXT_TARGET_ALT                  0x77
#define R_RTC_PCR_BUC                         0x3414               ///< Backed Up Control
#define B_RTC_PCR_BUC_DSO                     BIT4                 ///< Daylight Savings Override
#define B_RTC_PCR_BUC_TS                      BIT0                 ///< Top Swap
#define R_RTC_PCR_RTCDCG                      0x3418               ///< RTC Dynamic Clock Gating Control
#define R_RTC_IO_REGC                         0x0C
#define R_LPC_PCR_PRC                         0x341C
#define R_ESPI_PCR_SLV_CFG_REG_CTL            0x4000                  ///< Secondary Configuration Register and Link Control
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRE       BIT31                   ///< Secondary Configuration Register Access Enable
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRS       (BIT30 | BIT29 | BIT28) ///< Secondary Configuration Register Access Status
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SCRS       28                      ///< Secondary Configuration Register Access Status bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SBLCL      BIT27                   ///< IOSF-SB eSPI Link Configuration Lock
#define V_ESPI_PCR_SLV_CFG_REG_CTL_SCRS_NOERR 7                       ///< No errors (transaction completed successfully)
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SID        (BIT20 | BIT19)         ///< Secondary ID
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SID        19                      ///< Secondary ID bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRT       (BIT17 | BIT16)         ///< Secondary Configuration Register Access Type
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SCRT       16                      ///< Secondary Configuration Register Access Type bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRA       0x00000FFF              ///< Secondary Configuration Register Address
#define R_ESPI_PCR_SLV_CFG_REG_DATA           0x4004                  ///< Secondary Configuration Register Data
#define R_ESPI_PCR_PCERR_SLV0                 0x4020          ///< Peripheral Channel Error for Secondary 0
#define R_ESPI_PCR_PCERR_SLV1                 0x4024          ///< Peripheral Channel Error for Secondary 1
#define R_ESPI_PCR_VWERR_SLV0                 0x4030          ///< Virtual Wire Channel Error for Secondary 0
#ifdef PCH_ICL
#define B_ESPI_PCR_VWERR_SLV0_VWHC10OE        BIT25           ///< Primary HOST_C10 (Virtual Wire) to Secondary Enable (VWHC10OE)
#endif
#define N_ESPI_PCR_FCERR_SLV0_FCFEE           5               ///< Flash Access Channel Fatal Error Reporting bit position
#define V_ESPI_PCR_FCERR_SLV0_FCFEE_SERR      2               ///< Enable Fatal Error Reporting as SERR
#define N_ESPI_PCR_FCERR_SLV0_FCNFEE          13              ///< Flash Access Channel Non-Fatal Error Reporting Enable bit position
#define V_ESPI_PCR_FCERR_SLV0_FCNFEE_SERR     2               ///< Enable Non-Fatal Error Reporting as SERR
#define R_ESPI_PCR_VWERR_SLV1                 0x4034          ///< Virtual Wire Channel Error for Secondary 1
#define R_ESPI_PCR_FCERR_SLV0                 0x4040          ///< Flash Access Channel Error for Secondary 0
#define B_ESPI_PCR_FCERR_SAFBLK               BIT17           ///< SAF Blocked (SAFBLK)
#define R_ESPI_PCR_LNKERR_SLV0                0x4050          ///< Link Error for Secondary 0
#define S_ESPI_PCR_LNKERR_SLV0                4               ///< Link Error for Secondary 0 register size
#define B_ESPI_PCR_LNKERR_SLV0_SLCRR          BIT31           ///< eSPI Link and Secondary Channel Recovery Required
#define B_ESPI_PCR_LNKERR_SLV0_LFET1E         (BIT22 | BIT21) ///< Fatal Error Type 1 Reporting Enable
#define N_ESPI_PCR_LNKERR_SLV0_LFET1E         21              ///< Fatal Error Type 1 Reporting Enable bit position
#define V_ESPI_PCR_LNKERR_SLV0_LFET1E_SMI     3               ///< Enable Fatal Error Type 1 Reporting as SMI
#define V_ESPI_PCR_LNKERR_SLV0_LFER1E_SERR    2               ///< Enable Fatal Error Type 1 Reporting as SERR
#define N_ESPI_PCR_LNKERR_SLV1_LFET1E         21              ///< Fatal Error Type 1 Reporting Enable bit position
#define V_ESPI_PCR_LNKERR_SLV1_LFET1E_SMI     3               ///< Enable Fatal Error Type 1 Reporting as SMI
#define V_ESPI_PCR_LNKERR_SLV1_LFER1E_SERR    2               ///< Enable Fatal Error Type 1 Reporting as SERR
#define B_ESPI_PCR_LNKERR_SLV0_LFET1S         BIT20           ///< Link Fatal Error Type 1 Status
#define R_ESPI_PCR_LNKERR_SLV1                0x4054          ///< Link Error for Secondary 1
#endif
