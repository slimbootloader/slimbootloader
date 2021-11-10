/** @file
  Register names for IGD block
  <b>Conventions</b>:
  - Prefixes:
    - Definitions beginning with "R_" are registers
    - Definitions beginning with "B_" are bits within registers
    - Definitions beginning with "V_" are meaningful values of bits within the registers
    - Definitions beginning with "S_" are register sizes
    - Definitions beginning with "N_" are the bit position
  - In general, SA registers are denoted by "_SA_" in register names
  - Registers / bits that are different between SA generations are denoted by
    "_SA_[generation_name]_" in register/bit names. e.g., "_SA_HSW_"
  - Registers / bits that are different between SKUs are denoted by "_[SKU_name]"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a SA generation will be just named
    as "_SA_" without [generation_name] inserted.

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _IGD_REGS_H_
#define _IGD_REGS_H_

///
/// Device 2 Register Equates
///
//
// The following equates must be reviewed and revised when the specification is ready.
//
#define SA_IGD_BUS           0x00
#define SA_IGD_DEV           0x02
#define SA_IGD_FUN_0         0x00
#define SA_IGD_DEV_FUN       (SA_IGD_DEV << 3)
#define SA_IGD_BUS_DEV_FUN   (SA_MC_BUS << 8) + SA_IGD_DEV_FUN

#define V_SA_IGD_VID         0x8086

#define SA_GT_APERTURE_SIZE_256MB    1      ///< 256MB is the recommanded GT Aperture Size as per BWG.


#define R_SA_IGD_VID               0x00
#define R_SA_IGD_DID               0x02
#define R_SA_IGD_CMD               0x04
///
/// GTTMMADR aligned to 16MB (Base address = [38:24])
///
#define R_SA_IGD_GTTMMADR          0x10
#define R_SA_IGD_GMADR             0x18
#define R_SA_IGD_IOBAR             0x20

#define R_SA_IGD_BSM_OFFSET        0x005C  ///< Base of Stolen Memory
//#define R_SA_IGD_MSAC_OFFSET       0x0060  ///< Multisize Aperture Control
#define R_SA_IGD_MGGC0_OFFSET      0x0050  ///< Mirror of GGC
#define R_SA_IGD_MSICAPID_OFFSET   0x0090  ///< MSI Capabilities ID
#define R_SA_IGD_MC_OFFSET         0x0092  ///< Message Control
#define R_SA_IGD_MA_OFFSET         0x0094  ///< Message Address
#define R_SA_IGD_MD_OFFSET         0x0098  ///< Message Data
#define R_SA_IGD_SWSCI_OFFSET      0x00E8
#define R_SA_IGD_ASLS_OFFSET       0x00FC  ///< ASL Storage

#define R_SA_VTD_IGD_LBAR          0xC8    ///< GFX VTD Bar low
#define R_SA_VTD_IGD_UBAR          0xCC    ///< GFX VTD Bar high


#define R_SA_IGD_AUD_FREQ_CNTRL_OFFSET 0x65900 ///< iDisplay Audio BCLK Frequency Control
//#define B_SA_IGD_AUD_FREQ_CNTRL_TMODE     BIT15 | BIT14   ///< T-Mode
#define B_SA_IGD_AUD_FREQ_CNTRL_96MHZ  BIT4    ///< 96 MHz BCLK
#define B_SA_IGD_AUD_FREQ_CNTRL_48MHZ  BIT3    ///< 48 MHz BCLK

#define R_SA_GTTMMADR_PP_STATUS         0xC7200
#define R_SA_GTTMMADR_PP_CONTROL        0xC7204

#define GSA_TOUCH_BUS   0x0   ///< Bus 0
#define GSA_TOUCH_DEV   0x16  ///< Device 22
#define GSA_TOUCH_FUN   0x4   ///< Functional 4

#define R_SA_GTTMMADR_GTDOORBELL_OFFSET 0x10c008  ///< iTouch GT Doorbell BDF Register
#define R_SA_GTTMMADR_GSA_TOUCH_OFFSET  0x101078  ///< GSA Touch DBF Register
#define R_SA_GTTMMADR_GSA_AUDIO_OFFSET  0x101074  ///< GSA Audio Register
#define GSA_AUDIO_BUS   0x0   ///< Bus 0
#define GSA_AUDIO_DEV   0x1F  ///< Device 31
#define GSA_AUDIO_FUN   0x3   ///< Function 3


///
/// GT MMIO Registers for CD Clock Programming
///
#define R_SA_GTTMMADR_FUSE_STATUS_OFFSET           0x42000  ///< FUSE_STATUS
#define R_SA_GTTMMADR_PWR_WELL_CTL_OFFSET          0x45400  ///< PWR_WELL_CTL
#define R_SA_GTTMMADR_DBUF_CTL_OFFSET            0x45008   ///< DBUF_CTL
#define R_SA_GTTMMADR_CDCLK_CTL_OFFSET           0x46000   ///< CDCLK_CTL
#define R_SA_GTTMMADR_CDCLK_PLL_ENABLE_OFFSET    0x46070   ///< CDCLK_PLL_ENABLE
#define R_SA_GTTMMADR_NDE_RSTWRN_OPT_OFFSET      0x46408   ///< NDE_RSTWRN_OPT
#define R_SA_GTTMMADR_AUDIO_PIN_BUF_CTL_OFFSET   0x48414   ///< AUDIO_PIN_BUF_CTL
#define B_SA_GTTMMADR_AUDIO_PIN_BUF_CTL_ENABLE   BIT31
#define R_SA_GTTMMADR_DSSM_OFFSET                0x51004   ///< DSSM
//#define B_SA_GTTMMADR_DSSM_REFERENCE_FREQ_MASK   (BIT31 + BIT30 + BIT29) // Reference Frequency mask
//#define B_SA_GTTMMADR_DSSM_REFERENCE_FREQ_OFFSET 29
#define R_SA_GTTMMADR_MAILBOX_INTERFACE_OFFSET   0x138124  ///< GTDRIVER_MAILBOX_INTERFACE
#define R_SA_GTTMMADR_MAILBOX_DATA_LOW_OFFSET    0x138128  ///< GTDRIVER_MAILBOX_DATA0
#define R_SA_GTTMMADR_MAILBOX_DATA_HIGH_OFFSET   0x13812c  ///< GTDRIVER_MAILBOX_DATA1
#define V_SA_CDCLK_CTL_CD_FREQ_DECIMAL_168         0x14E
#define V_SA_CDCLK_CTL_CD_FREQ_DECIMAL_307_2       0x264
#define V_SA_CDCLK_CTL_CD_FREQ_DECIMAL_312         0x26E
#define V_SA_CDCLK_CTL_CD_FREQ_DECIMAL_552         0x44E
#define V_SA_CDCLK_CTL_CD_FREQ_DECIMAL_556_8       0x458
#define V_SA_CDCLK_CTL_CD_FREQ_DECIMAL_648         0x50E
#define V_SA_CDCLK_CTL_CD_FREQ_DECIMAL_652_8       0x518
//#define V_SA_CDCLK_CTL_CD2X_PIPE_SELECT_NONE       (BIT21 + BIT20 + BIT19)
#define V_SA_CDCLK_CTL_CD2X_ALL_CDCLK              0x0
#define V_SA_CDCLK_PLL_RATIO_652_8MHZ_REF_19_2MHZ  0x44
#define V_SA_CDCLK_PLL_RATIO_652_8MHZ_REF_38_4MHZ  0x22
#define B_SA_CDCLK_PLL_ENABLE_BIT                  BIT31
#define B_SA_CDCLK_PLL_LOCK_BIT                    BIT30

#define R_SA_GTTMMADR_PSMIBASE_OFFSET            0xBF0     ///< PSMI Base
#define R_SA_GTTMMADR_PSMILIMIT_OFFSET           0xBF8     ///< PSMI Limit
#define B_SA_PSMI_LOCK_MASK                      (0x1)
#define B_SA_PSMIBASE_LSB_MASK                   (0xfffff000)
#define B_SA_PSMILIMIT_LSB_MASK                  (0xffffffc0)
#define B_SA_PSMI_MSB_MASK                       (0x3f)

#define R_SA_GTTMMADR_PROCHOT_0_OFFSET               0x9580   ///< Prochot 0
#define B_SA_GTTMMADR_PROCHOT_0_TEETH_BREAKING_EN    BIT30    ///< Teeth Breaking Control Enable (Prochot)
#define R_SA_GTTMMADR_OFFSET_101014                  0x101014
#endif
