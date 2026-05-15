/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_ESPI_H_
#define _PCH_REGS_ESPI_H_

#define B_ESPI_CFG_DID                    0xFFE0

//
// eSPI controller PCI config registers
//
#define R_ESPI_CFG_ESPI_IOD_IOE           0x80                      ///< I/O Decode Ranges and I/O Enables
#define R_ESPI_CFG_ESPI_IOE               0x82                      /// I/O Decode Enables (second half of R_ESPI_CFG_ESPI_IOD_IOE)
#define N_ESPI_CFG_ESPI_IOD_IOE_LPT       8
#define V_ESPI_CFG_ESPI_IOD_IOE_LPT_378   0
#define B_ESPI_CFG_ESPI_IOD_IOE_CB        (BIT6 | BIT5 |BIT4)       ///< ComB Range
#define N_ESPI_CFG_ESPI_IOD_IOE_CB        4
#define V_ESPI_CFG_ESPI_IOD_IOE_CB_2F8    1
#define V_ESPI_CFG_ESPI_IOD_IOE_CB_3E8    7
#define B_ESPI_CFG_ESPI_IOD_IOE_CA        (BIT2 | BIT1 | BIT0)      ///< ComA Range
#define N_ESPI_CFG_ESPI_IOD_IOE_CA        0
#define V_ESPI_CFG_ESPI_IOD_IOE_CA_3F8    0
#define V_ESPI_CFG_ESPI_IOD_IOE_CA_2F8    1
#define V_ESPI_CFG_ESPI_IOD_IOE_CA_2E8    5
#define V_ESPI_CFG_ESPI_IOD_IOE_CA_3E8    7

#define V_ESPI_CFG_ESPI_IOE_RTCE          BIT14                     ///< Enables decoding of I/O locations 70h-77h to PCH RTC unit
#define V_ESPI_CFG_ESPI_IOE_ME2           BIT13                     ///< Microcontroller Enable #2, Enables decoding of I/O locations 4Eh and 4Fh to LPC
#define V_ESPI_CFG_ESPI_IOE_SE            BIT12                     ///< Super I/O Enable, Enables decoding of I/O locations 2Eh and 2Fh to LPC.
#define V_ESPI_CFG_ESPI_IOE_ME1           BIT11                     ///< Microcontroller Enable #1, Enables decoding of I/O locations 62h and 66h to LPC.
#define V_ESPI_CFG_ESPI_IOE_KE            BIT10                     ///< Keyboard Enable, Enables decoding of the keyboard I/O locations 60h and 64h to LPC.
#define V_ESPI_CFG_ESPI_IOE_HGE           BIT9                      ///< High Gameport Enable, Enables decoding of the I/O locations 208h to 20Fh to LPC.
#define V_ESPI_CFG_ESPI_IOE_LGE           BIT8                      ///< Low Gameport Enable, Enables decoding of the I/O locations 200h to 207h to LPC.
#define V_ESPI_CFG_ESPI_IOE_FDE           BIT3                      ///< Floppy Drive Enable, Enables decoding of the FDD range to LPC. Range is selected by LIOD.FDE
#define V_ESPI_CFG_ESPI_IOE_PPE           BIT2                      ///< Parallel Port Enable, Enables decoding of the LPT range to LPC. Range is selected by LIOD.LPT.
#define V_ESPI_CFG_ESPI_IOE_CBE           BIT1                      ///< ComB Range
#define V_ESPI_CFG_ESPI_IOE_CAE           BIT0                      ///< ComA Range

#define R_ESPI_CFG_ESPI_LGIR1             0x84                      ///< LPC Generic IO Range 1
#define B_ESPI_CFG_ESPI_LGIR1_ADDR_MASK   ( BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 )  ///< Address[7:2] Mask
#define N_ESPI_CFG_ESPI_LGIR1_ADDR_MASK   18
#define B_ESPI_CFG_ESPI_LGIR1_ADDR        0xFFFC
#define N_ESPI_CFG_ESPI_LGIR1_ADDR        2
#define B_ESPI_CFG_ESPI_LGIR1_LDE         BIT0
#define R_ESPI_CFG_ESPI_LGMR              0x98                      ///< LPC Generic Memory Range
#define B_ESPI_CFG_ESPI_LGMR_MADDR        0xFFFF0000                ///< Memory Address[31:16]
#define B_ESPI_CFG_ESPI_LGMR_LGMRD_EN     BIT0                      ///< LPC Memory Range Decode Enable
#define R_ESPI_CFG_ESPI_CS1IORE           0xA0                      ///< eSPI CS1 IO Routing Enables
#define R_ESPI_CFG_ESPI_CS1GIR1           0xA4                      ///< eSPI CS1 Generic IO Range 1
#define R_ESPI_CFG_ESPI_CS1GMR1           0xA8                      ///< eSPI CS1 Generic Memory Range 1

#define R_ESPI_CFG_ESPI_BDE               0xD8                      ///< BIOS decode enable
#define B_ESPI_CFG_ESPI_BDE_BLE           BIT31                     ///< BDE Lock Enable

#define R_ESPI_CFG_ESPI_IORE_EXT_2        0x200      ///< eSPI IO Routing Enables for ESPI CS2
#define R_ESPI_CFG_ESPI_GIR1_EXT_2        0x204      ///< eSPI Generic IO Range 1 for ESPI CS2
#define R_ESPI_CFG_ESPI_GMR1_EXT_2        0x208      ///< eSPI Generic Memory Range 1 for ESPI CS2
#define R_ESPI_CFG_ESPI_IORE_EXT_3        0x240      ///< eSPI IO Routing Enables for ESPI CS3
#define R_ESPI_CFG_ESPI_GIR1_EXT_3        0x244      ///< eSPI Generic IO Range 1 for ESPI CS3
#define R_ESPI_CFG_ESPI_GMR1_EXT_3        0x248      ///< eSPI Generic Memory Range 1 for ESPI CS3

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
#define S_ESPI_CFG_ESPI_BC                4               ///< Peripheral Channel BIOS Control register size
#define B_ESPI_CFG_ESPI_BC_BWRE           BIT11           ///< BIOS Write Reporting (Async-SMI) Enable
#define B_ESPI_CFG_ESPI_BC_BWRS           BIT10           ///< BIOS Write Status
#define B_ESPI_CFG_ESPI_BC_BWPDS          BIT8            ///< BIOS Write Protect Disable Status
#define N_ESPI_CFG_ESPI_BC_BWPDS          8               ///< BIOS Write Protect Disable Status bit position
#define B_ESPI_CFG_ESPI_BC_BILD           BIT7            ///< BIOS Interface Lock-Down
#define B_ESPI_CFG_ESPI_BC_EISS           BIT5            ///< Enable InSMM.STS
#define B_ESPI_CFG_ESPI_BC_ESPI           BIT2            ///< eSPI Enable Pin Strap
#define B_ESPI_CFG_ESPI_BC_LE             BIT1            ///< Lock Enable
#define N_ESPI_CFG_ESPI_BC_LE             1
#define B_ESPI_CFG_ESPI_BC_WPD            BIT0            ///< Write Protect Disable

//
// eSPI Device registers
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
#define R_ESPI_PCR_SLV_CFG_REG_CTL                0x4000                  ///< Device Configuration Register and Link Control
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRE           BIT31                   ///< Device Configuration Register Access Enable
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRS           (BIT30 | BIT29 | BIT28) ///< Device Configuration Register Access Status
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SCRS           28                      ///< Device Configuration Register Access Status bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SBLCL          BIT27                   ///< IOSF-SB eSPI Link Configuration Lock
#define B_ESPI_PCR_SLV_CFG_REG_CTL_eSPINC         (BIT25 | BIT26)         ///< eSPI Not Connected
#define N_ESPI_PCR_SLV_CFG_REG_CTL_eSPINC         25                      ///< eSPI Not Connected bit position
#define V_ESPI_PCR_SLV_CFG_REG_CTL_eSPINC_DIS     3                       ///< eSPI interface is disabled through soft strap
#define V_ESPI_PCR_SLV_CFG_REG_CTL_SCRS_NOERR     7                       ///< No errors (transaction completed successfully)
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SID            (BIT20 | BIT19)         ///< Device ID
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SID            19                      ///< Device ID bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRT           (BIT17 | BIT16)         ///< Device Configuration Register Access Type
#define N_ESPI_PCR_SLV_CFG_REG_CTL_SCRT           16                      ///< Device Configuration Register Access Type bit position
#define B_ESPI_PCR_SLV_CFG_REG_CTL_SCRA           0x00000FFF              ///< Device Configuration Register Address
#define R_ESPI_PCR_SLV_CFG_REG_DATA               0x4004                  ///< Device Configuration Register Data

#define R_ESPI_PCR_PCERR_SLV0                     0x4020          ///< Peripheral Channel Error for Device 0
#define V_ESPI_PCR_PCERR_SLVX_PCFEE_SERR          2               ///< Enable Fatal Error Reporting as SERR
#define V_ESPI_PCR_PCERR_SLVX_PCNFEE_SERR         2               ///< Enable Non-Fatal Error Reporting as SERR
#define B_ESPI_PCR_PCERR_SLV0_PCURD               BIT24           ///< Peripheral Channel Unsupported Request Detected, applies for PCERR_SLV1
#define B_ESPI_PCR_PCERR_SLV0_PCURRE              BIT25           ///< Peripheral Channel Unsupported Request Reporting Enable, applies for PCERR_SLV1
#define N_ESPI_PCR_PCERR_SLV0_PCRMTARE            26              ///< Peripheral Channel Received Target or Target Abort Reporting bit position, applies for PCERR_SLV1
#define V_ESPI_PCR_PCERR_SLVX_PCRMTARE_SERR       2               ///< Enable RMA or RTA Reporting as SERR
#define R_ESPI_PCR_PCERR_SLV1                     0x4024          ///< Peripheral Channel Error for Device 1
#define R_ESPI_PCR_VWERR_SLV0                     0x4030          ///< Virtual Wire Channel Error for Device 0
#define B_ESPI_PCR_VWERR_SLV0_VWHC10OE            BIT25           ///< Requester HOST_C10 (Virtual Wire) to Device Enable (VWHC10OE)
#define V_ESPI_PCR_VWERR_SLVX_VWNFEE_SERR         2               ///< Enable Non-Fatal Error Reporting as SERR
#define V_ESPI_PCR_VWERR_SLVX_VWFEE_SERR          2               ///< Enable Fatal Error Reporting as SERR
#define R_ESPI_PCR_VWERR_SLV1                     0x4034          ///< Virtual Wire Channel Error for Device 1
#define R_ESPI_PCR_FCERR_SLV0                     0x4040          ///< Flash Access Channel Error for Device 0
#define V_ESPI_PCR_FCERR_SLV0_FCFEE_SERR          2               ///< Enable Fatal Error Reporting as SERR
#define V_ESPI_PCR_FCERR_SLV0_FCNFEE_SERR         2               ///< Enable Non-Fatal Error Reporting as SERR
#define B_ESPI_PCR_FCERR_SLV0_SAFBLK              BIT17           ///< SAF Blocked

#define B_ESPI_PCR_XERR_SLVX_XNFEE                (BIT14 | BIT13) ///< Non-Fatal Error Reporting Enable bits
#define N_ESPI_PCR_XERR_SLVX_XNFEE                13              ///< Non-Fatal Error Reporting Enable bit position
#define V_ESPI_PCR_XERR_SLVX_XNFEE_SMI            3               ///< Enable Non-Fatal Error Reporting as SMI
#define B_ESPI_PCR_XERR_SLVX_XNFES                BIT12           ///< Fatal Error Status
#define B_ESPI_PCR_XERR_SLVX_XFEE                 (BIT6 | BIT5)   ///< Fatal Error Reporting Enable bits
#define N_ESPI_PCR_XERR_SLVX_XFEE                 5               ///< Fatal Error Reporting Enable bit position
#define V_ESPI_PCR_XERR_SLVX_XFEE_SMI             3               ///< Enable Fatal Error Reporting as SMI
#define B_ESPI_PCR_XERR_SLVX_XFES                 BIT4            ///< Fatal Error Status
#define S_ESPI_PCR_XERR_SLVX                      4               ///< Channel register sizes

#define R_ESPI_PCR_LNKERR_SLV0                    0x4050          ///< Link Error for Device 0
#define S_ESPI_PCR_LNKERR_SLV0                    4               ///< Link Error for Device 0 register size
#define B_ESPI_PCR_LNKERR_SLV0_SLCRR              BIT31           ///< eSPI Link and Device Channel Recovery Required
#define B_ESPI_PCR_LNKERR_SLV0_LFET1E             (BIT22 | BIT21) ///< Fatal Error Type 1 Reporting Enable
#define N_ESPI_PCR_LNKERR_SLV0_LFET1E             21              ///< Fatal Error Type 1 Reporting Enable bit position
#define V_ESPI_PCR_LNKERR_SLV0_LFET1E_SMI         3               ///< Enable Fatal Error Type 1 Reporting as SMI
#define V_ESPI_PCR_LNKERR_SLV0_LFER1E_SERR        2               ///< Enable Fatal Error Type 1 Reporting as SERR
#define N_ESPI_PCR_LNKERR_SLV1_LFET1E             21              ///< Fatal Error Type 1 Reporting Enable bit position
#define V_ESPI_PCR_LNKERR_SLV1_LFET1E_SMI         3               ///< Enable Fatal Error Type 1 Reporting as SMI
#define V_ESPI_PCR_LNKERR_SLV1_LFER1E_SERR        2               ///< Enable Fatal Error Type 1 Reporting as SERR
#define B_ESPI_PCR_LNKERR_SLV0_LFET1S             BIT20           ///< Link Fatal Error Type 1 Status
#define R_ESPI_PCR_LNKERR_SLV1                    0x4054          ///< Link Error for Device 1
#define R_ESPI_PCR_PCERR_SLV_EXT_2                0x4200          ///< Peripheral Channel Error for ESPI CS 2
#define R_ESPI_PCR_VWERR_SLV_EXT_2                0x4204          ///< Virtual Channel Error for ESPI CS 2
#define R_ESPI_PCR_LNKERR_SLV_EXT_2               0x4208          ///< Link Error for ESPI CS 2
#define S_ESPI_PCR_XERR_SLV_EXT_X                 0x40            ///< Error registers spacing for CS2 and up
#endif
