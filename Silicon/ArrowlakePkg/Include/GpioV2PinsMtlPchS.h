/** @file
  This file provides GPIO pins definitions for MTL_PCH_S PCH.

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _GPIOV2_PINS_MTL_PCH_S_H_
#define _GPIOV2_PINS_MTL_PCH_S_H_

#include <Register/GpioV2ChipsetId.h>
#include <GpioAcpiDefinesMtlPchS.h>

//
// Unique IDs used in GpioPad defines
//

//
// Community 0
//
#define MTL_PCH_S_COM_IDX_GPP_D                 0
#define MTL_PCH_S_GRP_IDX_GPP_D                 0
#define MTL_PCH_S_COM_IDX_GPP_R                 0
#define MTL_PCH_S_GRP_IDX_GPP_R                 1
#define MTL_PCH_S_COM_IDX_GPP_J                 0
#define MTL_PCH_S_GRP_IDX_GPP_J                 2

//
// Community 1
//
#define MTL_PCH_S_COM_IDX_GPP_A                 1
#define MTL_PCH_S_GRP_IDX_GPP_A                 0
#define MTL_PCH_S_COM_IDX_GPP_B                 1
#define MTL_PCH_S_GRP_IDX_GPP_B                 1

//
// Community 2
//
#define MTL_PCH_S_COM_IDX_DSW                   2
#define MTL_PCH_S_GRP_IDX_DSW                   0

//
// Community 3
//

#define MTL_PCH_S_COM_IDX_GPP_C                 3
#define MTL_PCH_S_GRP_IDX_GPP_C                 0
#define MTL_PCH_S_COM_IDX_GPP_H                 3
#define MTL_PCH_S_GRP_IDX_GPP_H                 1


//
// Community 4
//
#define MTL_PCH_S_COM_IDX_GPP_S                 4
#define MTL_PCH_S_GRP_IDX_GPP_S                 0
#define MTL_PCH_S_COM_IDX_GPP_E                 4
#define MTL_PCH_S_GRP_IDX_GPP_E                 1
#define MTL_PCH_S_COM_IDX_GPP_K                 4
#define MTL_PCH_S_GRP_IDX_GPP_K                 2
#define MTL_PCH_S_COM_IDX_GPP_F                 4
#define MTL_PCH_S_GRP_IDX_GPP_F                 3

//
// Community 5
//
#define MTL_PCH_S_COM_IDX_GPP_I                 5
#define MTL_PCH_S_GRP_IDX_GPP_I                 0


//
// GPIO Group defines
//
//
// Community 0
//
#define GPIOV2_MTL_PCH_S_GROUP_D        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_R        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_J        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 0xFF)

//
// Community 1
//
#define GPIOV2_MTL_PCH_S_GROUP_A        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A,    MTL_PCH_S_GRP_IDX_GPP_A,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_B        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B,    MTL_PCH_S_GRP_IDX_GPP_B,    0xFF)

//
// Community 2
//
#define GPIOV2_MTL_PCH_S_GROUP_DSW      GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW,    MTL_PCH_S_GRP_IDX_DSW,    0xFF)

//
// Community 3
//
#define GPIOV2_MTL_PCH_S_GROUP_C        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C,    MTL_PCH_S_GRP_IDX_GPP_C,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_H        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H,    MTL_PCH_S_GRP_IDX_GPP_H,    0xFF)

//
// Community 4
//
#define GPIOV2_MTL_PCH_S_GROUP_S        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S,    MTL_PCH_S_GRP_IDX_GPP_S,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_E        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E,    MTL_PCH_S_GRP_IDX_GPP_E,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_K        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K,    MTL_PCH_S_GRP_IDX_GPP_K,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_F        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F,    MTL_PCH_S_GRP_IDX_GPP_F,    0xFF)

//
// Community 5
//
#define GPIOV2_MTL_PCH_S_GROUP_I        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I,    MTL_PCH_S_GRP_IDX_GPP_I,    0xFF)

//
// Group name: GPP_D
// Community index: 0
// Group index: 0
// Number of pads: 24
//


// Pin:                          GPP_D_0 ( Value: 0x0 )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_0
// Native function 6:            IOM_GPPD_0
#define GPIOV2_MTL_PCH_S_GPP_D_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 0)

// Pin:                          GPP_D_1 ( Value: 0x1 )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_1
// Native function 6:            IOM_GPPD_1
#define GPIOV2_MTL_PCH_S_GPP_D_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 1)

// Pin:                          GPP_D_2 ( Value: 0x2 )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_2
// Native function 6:            IOM_GPPD_2
#define GPIOV2_MTL_PCH_S_GPP_D_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 2)

// Pin:                          GPP_D_3 ( Value: 0x3 )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_3
// Native function 6:            IOM_GPPD_3
#define GPIOV2_MTL_PCH_S_GPP_D_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 3)

// Pin:                          GPP_D_4 ( Value: 0x4 )
// Family:                       SML1
// Default function:             GP-In
// Native function 1:            SML1CLK
// Native function 6:            IOM_GPPD_4
#define GPIOV2_MTL_PCH_S_GPP_D_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 4)

// Pin:                          GPP_D_5 ( Value: 0x5 )
// Family:                       I2S2
// Default function:             Function 2
// Native function 1:            I2S2_SFRM
// Native function 2:            CNV_RF_RESET_B
// Native function 5:            DMIC_DATA_0
// Native function 6:            IOM_GPPD_5
#define GPIOV2_MTL_PCH_S_GPP_D_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 5)

// Pin:                          GPP_D_6 ( Value: 0x6 )
// Family:                       I2S2
// Default function:             Function 3
// Native function 1:            I2S2_TXD
// Native function 3:            CRF_CLKREQ
// Native function 5:            DMIC_CLK_B_0
// Native function 6:            IOM_GPPD_6
#define GPIOV2_MTL_PCH_S_GPP_D_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 6)

// Pin:                          GPP_D_7 ( Value: 0x7 )
// Family:                       I2S2
// Default function:             GP-In
// Native function 1:            I2S2_RXD
// Native function 5:            DMIC_DATA_1
// Native function 6:            IOM_GPPD_7
#define GPIOV2_MTL_PCH_S_GPP_D_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 7)

// Pin:                          GPP_D_8 ( Value: 0x8 )
// Family:                       I2S2
// Default function:             GP-In
// Native function 1:            I2S2_SCLK
// Native function 5:            DMIC_CLK_A_0
// Native function 6:            IOM_GPPD_8
#define GPIOV2_MTL_PCH_S_GPP_D_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 8)

// Pin:                          GPP_D_9 ( Value: 0x9 )
// Family:                       SML0
// Default function:             Function 1
// Native function 1:            SML0CLK
// Native function 6:            IOM_GPPD_9
#define GPIOV2_MTL_PCH_S_GPP_D_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 9)

// Pin:                          GPP_D_10 ( Value: 0xa )
// Family:                       SML0
// Default function:             Function 1
// Native function 1:            SML0DATA
// Native function 6:            IOM_GPPD_10
#define GPIOV2_MTL_PCH_S_GPP_D_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 10)

// Pin:                          GPP_D_11 ( Value: 0xb )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_4
// Native function 6:            IOM_GPPD_11
#define GPIOV2_MTL_PCH_S_GPP_D_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 11)

// Pin:                          GPP_D_12 ( Value: 0xc )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_5
// Native function 6:            IOM_GPPD_12
#define GPIOV2_MTL_PCH_S_GPP_D_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 12)

// Pin:                          GPP_D_13 ( Value: 0xd )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_6
// Native function 6:            IOM_GPPD_13
#define GPIOV2_MTL_PCH_S_GPP_D_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 13)

// Pin:                          GPP_D_14 ( Value: 0xe )
// Family:                       MiscD
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_7
// Native function 6:            IOM_GPPD_14
#define GPIOV2_MTL_PCH_S_GPP_D_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 14)

// Pin:                          GPP_D_15 ( Value: 0xf )
// Family:                       SML1
// Default function:             GP-In
// Native function 1:            SML1DATA
// Native function 6:            IOM_GPPD_15
#define GPIOV2_MTL_PCH_S_GPP_D_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 15)

// Pin:                          GPP_D_16 ( Value: 0x10 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            GSPI3_CS0B
// Native function 2:            THC1_SPI2_CSB
// Native function 6:            IOM_GPPD_16
#define GPIOV2_MTL_PCH_S_GPP_D_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 16)

// Pin:                          GPP_D_17 ( Value: 0x11 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            GSPI3_CLK
// Native function 2:            THC1_SPI2_CLK
// Native function 6:            IOM_GPPD_17
#define GPIOV2_MTL_PCH_S_GPP_D_17               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 17)

// Pin:                          GPP_D_18 ( Value: 0x12 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            GSPI3_MISO
// Native function 2:            THC1_SPI2_IO_0
// Native function 6:            IOM_GPPD_18
#define GPIOV2_MTL_PCH_S_GPP_D_18               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 18)

// Pin:                          GPP_D_19 ( Value: 0x13 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            GSPI3_MOSI
// Native function 2:            THC1_SPI2_IO_1
// Native function 6:            IOM_GPPD_19
#define GPIOV2_MTL_PCH_S_GPP_D_19               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 19)

// Pin:                          GPP_D_20 ( Value: 0x14 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            UART3_RXD
// Native function 2:            THC1_SPI2_IO_2
// Native function 6:            IOM_GPPD_20
#define GPIOV2_MTL_PCH_S_GPP_D_20               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 20)

// Pin:                          GPP_D_21 ( Value: 0x15 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            UART3_TXD
// Native function 2:            THC1_SPI2_IO_3
// Native function 6:            IOM_GPPD_21
#define GPIOV2_MTL_PCH_S_GPP_D_21               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 21)

// Pin:                          GPP_D_22 ( Value: 0x16 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            UART3_RTSB
// Native function 2:            THC1_SPI2_RSTB
// Native function 6:            IOM_GPPD_22
#define GPIOV2_MTL_PCH_S_GPP_D_22               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 22)

// Pin:                          GPP_D_23 ( Value: 0x17 )
// Family:                       THC1_GSPI3
// Default function:             GP-In
// Native function 1:            UART3_CTSB
// Native function 2:            THC1_SPI2_INTB
// Native function 6:            IOM_GPPD_23
#define GPIOV2_MTL_PCH_S_GPP_D_23               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 23)


//
// Group name: GPP_R
// Community index: 0
// Group index: 1
// Number of pads: 13
//


// Pin:                          GPP_R_0 ( Value: 0x80 )
// Family:                       AZA_I2S0
// Default function:             Function 1
// Native function 1:            HDA_BCLK
// Native function 2:            I2S0_SCLK
// Native function 4:            HDACPU_BCLK
// Native function 5:            DMIC_CLK_A_1
// Native function 6:            IOM_GPPR_0
#define GPIOV2_MTL_PCH_S_GPP_R_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 0)

// Pin:                          GPP_R_1 ( Value: 0x81 )
// Family:                       AZA_I2S0
// Default function:             Function 1
// Native function 1:            HDA_SYNC
// Native function 2:            I2S0_SFRM
// Native function 5:            DMIC_CLK_B_1
// Native function 6:            IOM_GPPR_1
#define GPIOV2_MTL_PCH_S_GPP_R_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 1)

// Pin:                          GPP_R_2 ( Value: 0x82 )
// Family:                       AZA_I2S0
// Default function:             Function 1
// Native function 1:            HDA_SDO
// Native function 2:            I2S0_TXD
// Native function 4:            HDACPU_SDO
// Native function 6:            IOM_GPPR_2
#define GPIOV2_MTL_PCH_S_GPP_R_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 2)

// Pin:                          GPP_R_3 ( Value: 0x83 )
// Family:                       AZA_I2S0
// Default function:             Function 1
// Native function 1:            HDA_SDI_0
// Native function 2:            I2S0_RXD
// Native function 4:            HDACPU_SDI
// Native function 6:            IOM_GPPR_3
#define GPIOV2_MTL_PCH_S_GPP_R_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 3)

// Pin:                          GPP_R_4 ( Value: 0x84 )
// Family:                       AZA_I2S1
// Default function:             Function 1
// Native function 1:            HDA_RSTB
// Native function 6:            IOM_GPPR_4
#define GPIOV2_MTL_PCH_S_GPP_R_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 4)

// Pin:                          GPP_R_5 ( Value: 0x85 )
// Family:                       AZA_I2S1
// Default function:             GP-In
// Native function 1:            HDA_SDI_1
// Native function 2:            I2S1_RXD
// Native function 6:            IOM_GPPR_5
#define GPIOV2_MTL_PCH_S_GPP_R_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 5)

// Pin:                          GPP_R_6 ( Value: 0x86 )
// Family:                       AZA_I2S1
// Default function:             GP-In
// Native function 2:            I2S1_TXD
// Native function 6:            IOM_GPPR_6
#define GPIOV2_MTL_PCH_S_GPP_R_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 6)

// Pin:                          GPP_R_7 ( Value: 0x87 )
// Family:                       AZA_I2S1
// Default function:             GP-In
// Native function 2:            I2S1_SFRM
// Native function 6:            IOM_GPPR_7
#define GPIOV2_MTL_PCH_S_GPP_R_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 7)

// Pin:                          GPP_R_8 ( Value: 0x88 )
// Family:                       AZA_I2S1
// Default function:             GP-In
// Native function 2:            I2S1_SCLK
// Native function 6:            IOM_GPPR_8
#define GPIOV2_MTL_PCH_S_GPP_R_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 8)

// Pin:                          GPP_R_9 ( Value: 0x89 )
// Family:                       GSPI2
// Default function:             GP-In
// Native function 1:            ISH_SPI_CSB
// Native function 3:            GSPI2_CS0B
// Native function 6:            IOM_GPPR_9
#define GPIOV2_MTL_PCH_S_GPP_R_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 9)

// Pin:                          GPP_R_10 ( Value: 0x8a )
// Family:                       GSPI2
// Default function:             GP-In
// Native function 1:            ISH_SPI_CLK
// Native function 3:            GSPI2_CLK
// Native function 6:            IOM_GPPR_10
#define GPIOV2_MTL_PCH_S_GPP_R_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 10)

// Pin:                          GPP_R_11 ( Value: 0x8b )
// Family:                       GSPI2
// Default function:             GP-In
// Native function 1:            ISH_SPI_MISO
// Native function 3:            GSPI2_MISO
// Native function 6:            IOM_GPPR_11
#define GPIOV2_MTL_PCH_S_GPP_R_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 11)

// Pin:                          GPP_R_12 ( Value: 0x8c )
// Family:                       GSPI2
// Default function:             GP-Out
// Native function 1:            ISH_SPI_MOSI
// Native function 3:            GSPI2_MOSI
// Native function 6:            IOM_GPPR_12
#define GPIOV2_MTL_PCH_S_GPP_R_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 12)


//
// Group name: GPP_J
// Community index: 0
// Group index: 2
// Number of pads: 17
//


// Pin:                          GPP_J_0 ( Value: 0x100 )
// Family:                       CNV
// Default function:             GP-In
// Native function 1:            CNV_PA_BLANKING
// Native function 6:            IOM_GPPJ_0
#define GPIOV2_MTL_PCH_S_GPP_J_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 0)

// Pin:                          GPP_J_1 ( Value: 0x101 )
// Family:                       CNV
// Default function:             Function 1
// Native function 1:            CNV_BRI_DT
// Native function 2:            UART0_RTSB
// Native function 6:            IOM_GPPJ_1
#define GPIOV2_MTL_PCH_S_GPP_J_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 1)

// Pin:                          GPP_J_2 ( Value: 0x102 )
// Family:                       CNV
// Default function:             Function 1
// Native function 1:            CNV_BRI_RSP
// Native function 2:            UART0_RXD
// Native function 6:            IOM_GPPJ_2
#define GPIOV2_MTL_PCH_S_GPP_J_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 2)

// Pin:                          GPP_J_3 ( Value: 0x103 )
// Family:                       CNV
// Default function:             Function 1
// Native function 1:            CNV_RGI_DT
// Native function 2:            UART0_TXD
// Native function 6:            IOM_GPPJ_3
#define GPIOV2_MTL_PCH_S_GPP_J_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 3)

// Pin:                          GPP_J_4 ( Value: 0x104 )
// Family:                       CNV
// Default function:             Function 1
// Native function 1:            CNV_RGI_RSP
// Native function 2:            UART0_CTSB
// Native function 6:            IOM_GPPJ_4
#define GPIOV2_MTL_PCH_S_GPP_J_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 4)

// Pin:                          GPP_J_5 ( Value: 0x105 )
// Family:                       CNV_MFUART2
// Default function:             GP-In
// Native function 1:            CNV_MFUART2_RXD
// Native function 6:            IOM_GPPJ_5
#define GPIOV2_MTL_PCH_S_GPP_J_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 5)

// Pin:                          GPP_J_6 ( Value: 0x106 )
// Family:                       CNV_MFUART2
// Default function:             GP-Out
// Native function 1:            CNV_MFUART2_TXD
// Native function 6:            IOM_GPPJ_6
#define GPIOV2_MTL_PCH_S_GPP_J_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 6)

// Pin:                          GPP_J_7 ( Value: 0x107 )
// Family:                       MiscJ
// Default function:             Function 1
// Native function 1:            BOOTHALT_B
// Native function 6:            IOM_GPPJ_7
#define GPIOV2_MTL_PCH_S_GPP_J_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 7)

// Pin:                          GPP_J_8 ( Value: 0x108 )
// Family:                       MiscJ
// Default function:             Function 1
// Native function 1:            RTCCLKOUT
// Native function 6:            IOM_GPPJ_8
#define GPIOV2_MTL_PCH_S_GPP_J_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 8)

// Pin:                          GPP_J_9 ( Value: 0x109 )
// Family:                       BPKI3C
// Default function:             RW - 4'b0001 [gpcom_strap_ccdif_is_bpki3c?4'b0001: 4'b0010]
// Native function 1:            BPKI3C_SDA
// Native function 2:            BSSB_LS_RX
// Native function 6:            IOM_GPPJ_9
#define GPIOV2_MTL_PCH_S_GPP_J_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 9)

// Pin:                          GPP_J_10 ( Value: 0x10a )
// Family:                       BPKI3C
// Default function:             RW - 4'b0001 [gpcom_strap_ccdif_is_bpki3c?4'b0001: 4'b0010]
// Native function 1:            BPKI3C_SCL
// Native function 2:            BSSB_LS_TX
// Native function 6:            IOM_GPPJ_10
#define GPIOV2_MTL_PCH_S_GPP_J_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 10)

// Pin:                          GPP_J_11 ( Value: 0x10b )
// Family:                       MiscJ
// Default function:             Function 1
// Native function 1:            DAM
#define GPIOV2_MTL_PCH_S_GPP_J_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 11)

// Pin:                          GPP_J_12 ( Value: 0x10c )
// Family:                       AZA_CPU
// Default function:             Function 1
// Native function 4:            HDACPU_SDI
#define GPIOV2_MTL_PCH_S_GPP_J_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 12)

// Pin:                          GPP_J_13 ( Value: 0x10d )
// Family:                       AZA_CPU
// Default function:             Function 1
// Native function 4:            HDACPU_SDO
#define GPIOV2_MTL_PCH_S_GPP_J_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 13)

// Pin:                          GPP_J_14 ( Value: 0x10e )
// Family:                       AZA_CPU
// Default function:             Function 1
// Native function 4:            HDACPU_BCLK
#define GPIOV2_MTL_PCH_S_GPP_J_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 14)

// Pin:                          GPP_J_15 ( Value: 0x10f )
// Family:                       MiscJ
// Default function:             Function 1
// Native function 1:            AUX_PWRGD
#define GPIOV2_MTL_PCH_S_GPP_J_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 15)

// Pin:                          GPP_J_16 ( Value: 0x110 )
// Family:                       MiscJ
// Default function:             Function 1
// Native function 1:            GLB_RST_WARN_B
#define GPIOV2_MTL_PCH_S_GPP_J_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 16)



//
// Group name: GPP_A
// Community index: 1
// Group index: 0
// Number of pads: 14
//


// Pin:                          GPP_A_0 ( Value: 0x400 )
// Family:                       eSPI
// Default function:             RW - 4'b0001 [dfx_dsw_gpcom_strap_espi_disable?4'b0000: 4'b0001]
// Native function 1:            ESPI_IO_0
// Native function 6:            IOM_GPPA_0
#define GPIOV2_MTL_PCH_S_GPP_A_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 0)

// Pin:                          GPP_A_1 ( Value: 0x401 )
// Family:                       eSPI
// Default function:             RW - 4'b0001 [dfx_dsw_gpcom_strap_espi_disable?4'b0000: 4'b0001]
// Native function 1:            ESPI_IO_1
// Native function 6:            IOM_GPPA_1
#define GPIOV2_MTL_PCH_S_GPP_A_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 1)

// Pin:                          GPP_A_2 ( Value: 0x402 )
// Family:                       eSPI
// Default function:             RW - 4'b0001 [dfx_dsw_gpcom_strap_espi_disable?4'b0010: 4'b0001]
// Native function 1:            ESPI_IO_2
// Native function 2:            SUSWARNB_SUSPWRDNACK
// Native function 6:            IOM_GPPA_2
#define GPIOV2_MTL_PCH_S_GPP_A_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 2)

// Pin:                          GPP_A_3 ( Value: 0x403 )
// Family:                       eSPI
// Default function:             RW - 4'b0001 [dfx_dsw_gpcom_strap_espi_disable?4'b0010: 4'b0001]
// Native function 1:            ESPI_IO_3
// Native function 2:            SUSACKB
// Native function 6:            IOM_GPPA_3
#define GPIOV2_MTL_PCH_S_GPP_A_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 3)

// Pin:                          GPP_A_4 ( Value: 0x404 )
// Family:                       eSPI
// Default function:             RW - 4'b0001 [dfx_dsw_gpcom_strap_espi_disable?4'b0000: 4'b0001]
// Native function 1:            ESPI_CS0B
// Native function 6:            IOM_GPPA_4
#define GPIOV2_MTL_PCH_S_GPP_A_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 4)

// Pin:                          GPP_A_5 ( Value: 0x405 )
// Family:                       eSPI
// Default function:             RW - 4'b0001 [dfx_dsw_gpcom_strap_espi_disable?4'b0000: 4'b0001]
// Native function 1:            ESPI_CLK
// Native function 6:            IOM_GPPA_5
#define GPIOV2_MTL_PCH_S_GPP_A_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 5)

// Pin:                          GPP_A_6 ( Value: 0x406 )
// Family:                       eSPI
// Default function:             RW - 4'b0001 [dfx_dsw_gpcom_strap_espi_disable?4'b0000: 4'b0001]
// Native function 1:            ESPI_RESETB
// Native function 6:            IOM_GPPA_6
#define GPIOV2_MTL_PCH_S_GPP_A_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 6)

// Pin:                          GPP_A_7 ( Value: 0x407 )
// Family:                       eSPI
// Default function:             RW - 4'b0000 [dfx_dsw_gpcom_strap_espi_disable ? 4'b0000 : (((gpio_fuse_espi_mdev == 3'b101) | (gpio_fuse_espi_mdev == 3'b110) | (gpio_fuse_espi_mdev == 3'b111)) ? 4'b0001 : 4'b0000)]
// Native function 1:            ESPI_CS1B
// Native function 6:            IOM_GPPA_7
#define GPIOV2_MTL_PCH_S_GPP_A_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 7)

// Pin:                          GPP_A_8 ( Value: 0x408 )
// Family:                       eSPI
// Default function:             RW - 4'b0000 [dfx_dsw_gpcom_strap_espi_disable ? 4'b0000 : (((gpio_fuse_espi_mdev == 3'b110) | (gpio_fuse_espi_mdev == 3'b111)) ? 4'b0001 : 4'b0000)]
// Native function 1:            ESPI_CS2B
// Native function 6:            IOM_GPPA_8
#define GPIOV2_MTL_PCH_S_GPP_A_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 8)

// Pin:                          GPP_A_9 ( Value: 0x409 )
// Family:                       eSPI
// Default function:             RW - 4'b0000 [dfx_dsw_gpcom_strap_espi_disable ? 4'b0000 : ((gpio_fuse_espi_mdev == 3'b111) ? 4'b0001 : 4'b0000)]
// Native function 1:            ESPI_CS3B
// Native function 6:            IOM_GPPA_9
#define GPIOV2_MTL_PCH_S_GPP_A_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 9)

// Pin:                          GPP_A_10 ( Value: 0x40a )
// Family:                       eSPI
// Default function:             RW - 4'b0000 [dfx_dsw_gpcom_strap_espi_disable ? 4'b0000 : (((gpio_fuse_espi_mdev == 3'b100) | (gpio_fuse_espi_mdev == 3'b101) | (gpio_fuse_espi_mdev == 3'b110) | (gpio_fuse_espi_mdev == 3'b111)) ? 4'b0001 : 4'b0000)]
// Native function 1:            ESPI_ALERT0B
// Native function 6:            IOM_GPPA_10
#define GPIOV2_MTL_PCH_S_GPP_A_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 10)

// Pin:                          GPP_A_11 ( Value: 0x40b )
// Family:                       eSPI
// Default function:             RW - 4'b0000 [dfx_dsw_gpcom_strap_espi_disable ? 4'b0000 : (((gpio_fuse_espi_mdev == 3'b101) | (gpio_fuse_espi_mdev == 3'b110) | (gpio_fuse_espi_mdev == 3'b111)) ? 4'b0001 : 4'b0000)]
// Native function 1:            ESPI_ALERT1B
// Native function 6:            IOM_GPPA_11
#define GPIOV2_MTL_PCH_S_GPP_A_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 11)

// Pin:                          GPP_A_12 ( Value: 0x40c )
// Family:                       eSPI
// Default function:             RW - 4'b0000 [dfx_dsw_gpcom_strap_espi_disable ? 4'b0000 : (((gpio_fuse_espi_mdev == 3'b110) | (gpio_fuse_espi_mdev == 3'b111)) ? 4'b0001 : 4'b0000)]
// Native function 1:            ESPI_ALERT2B
// Native function 6:            IOM_GPPA_12
#define GPIOV2_MTL_PCH_S_GPP_A_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 12)

// Pin:                          GPP_A_13 ( Value: 0x40d )
// Family:                       eSPI
// Default function:             RW - 4'b0000 [dfx_dsw_gpcom_strap_espi_disable ? 4'b0000 : ((gpio_fuse_espi_mdev == 3'b111) ? 4'b0001 : 4'b0000)]
// Native function 1:            ESPI_ALERT3B
// Native function 6:            IOM_GPPA_13
#define GPIOV2_MTL_PCH_S_GPP_A_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 13)


//
// Group name: GPP_B
// Community index: 1
// Group index: 1
// Number of pads: 22
//


// Pin:                          GPP_B_0 ( Value: 0x500 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            PCIE_LNK_DOWN
// Native function 6:            IOM_GPPB_0
#define GPIOV2_MTL_PCH_S_GPP_B_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 0)

// Pin:                          GPP_B_1 ( Value: 0x501 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 2:            ISH_UART0_RTSB
// Native function 2:            GSPI2_CS1B
// Native function 6:            IOM_GPPB_1
#define GPIOV2_MTL_PCH_S_GPP_B_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 1)

// Pin:                          GPP_B_2 ( Value: 0x502 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_8
// Native function 6:            IOM_GPPB_2
#define GPIOV2_MTL_PCH_S_GPP_B_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 2)

// Pin:                          GPP_B_3 ( Value: 0x503 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            CPU_GP_2
// Native function 6:            IOM_GPPB_3
#define GPIOV2_MTL_PCH_S_GPP_B_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 3)

// Pin:                          GPP_B_4 ( Value: 0x504 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            CPU_GP_3
// Native function 6:            IOM_GPPB_4
#define GPIOV2_MTL_PCH_S_GPP_B_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 4)

// Pin:                          GPP_B_5 ( Value: 0x505 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            SX_EXIT_HOLDOFFB
// Native function 2:            ISH_GP_6
// Native function 3:            IEH_FATAL_ERR2B
// Native function 6:            IOM_GPPB_5
#define GPIOV2_MTL_PCH_S_GPP_B_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 5)

// Pin:                          GPP_B_6 ( Value: 0x506 )
// Family:                       MiscB
// Default function:             RW - 4'b0000 [gpcom_sstrap_clkout_48? 4'b0001:4'b0000]
// Native function 1:            CLKOUT_48
// Native function 2:            CNV_DEBUG_00
// Native function 6:            IOM_GPPB_6
#define GPIOV2_MTL_PCH_S_GPP_B_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 6)

// Pin:                          GPP_B_7 ( Value: 0x507 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_7
// Native function 2:            CNV_DEBUG_01
// Native function 6:            IOM_GPPB_7
#define GPIOV2_MTL_PCH_S_GPP_B_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 7)

// Pin:                          GPP_B_8 ( Value: 0x508 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_0
// Native function 2:            CNV_DEBUG_02
// Native function 6:            IOM_GPPB_8
#define GPIOV2_MTL_PCH_S_GPP_B_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 8)

// Pin:                          GPP_B_9 ( Value: 0x509 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_1
// Native function 2:            CNV_DEBUG_03
// Native function 6:            IOM_GPPB_9
#define GPIOV2_MTL_PCH_S_GPP_B_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 9)

// Pin:                          GPP_B_10 ( Value: 0x50a )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_2
// Native function 2:            CNV_DEBUG_04
// Native function 6:            IOM_GPPB_10
#define GPIOV2_MTL_PCH_S_GPP_B_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 10)

// Pin:                          GPP_B_11 ( Value: 0x50b )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            I2S_MCLK
// Native function 6:            IOM_GPPB_11
#define GPIOV2_MTL_PCH_S_GPP_B_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 11)

// Pin:                          GPP_B_12 ( Value: 0x50c )
// Family:                       MiscB
// Default function:             Function 1
// Native function 1:            SLP_S0B
// Native function 6:            IOM_GPPB_12
#define GPIOV2_MTL_PCH_S_GPP_B_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 12)

// Pin:                          GPP_B_13 ( Value: 0x50d )
// Family:                       MiscB
// Default function:             Function 1
// Native function 1:            PLTRSTB
// Native function 6:            IOM_GPPB_13
#define GPIOV2_MTL_PCH_S_GPP_B_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 13)

// Pin:                          GPP_B_14 ( Value: 0x50e )
// Family:                       MiscB
// Default function:             GP-Out
// Native function 6:            IOM_GPPB_14
#define GPIOV2_MTL_PCH_S_GPP_B_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 14)

// Pin:                          GPP_B_15 ( Value: 0x50f )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_3
// Native function 2:            CNV_DEBUG_05
// Native function 6:            IOM_GPPB_15
#define GPIOV2_MTL_PCH_S_GPP_B_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 15)

// Pin:                          GPP_B_16 ( Value: 0x510 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_4
// Native function 2:            CNV_DEBUG_06
// Native function 6:            IOM_GPPB_16
#define GPIOV2_MTL_PCH_S_GPP_B_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 16)

// Pin:                          GPP_B_17 ( Value: 0x511 )
// Family:                       MiscB
// Default function:             GP-In
// Native function 1:            ISH_GP_5
// Native function 2:            CNV_DEBUG_07
// Native function 6:            IOM_GPPB_17
#define GPIOV2_MTL_PCH_S_GPP_B_17               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 17)

// Pin:                          GPP_B_18 ( Value: 0x512 )
// Family:                       MiscB
// Default function:             GP-Out
// Native function 1:            PMCALERTB
// Native function 6:            IOM_GPPB_18
#define GPIOV2_MTL_PCH_S_GPP_B_18               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 18)

// Pin:                          GPP_B_19 ( Value: 0x513 )
// Family:                       MiscB
// Default function:             Function 1
// Native function 1:            FUSA_DIAGTEST_EN
// Native function 6:            IOM_GPPB_19
#define GPIOV2_MTL_PCH_S_GPP_B_19               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 19)

// Pin:                          GPP_B_20 ( Value: 0x514 )
// Family:                       MiscB
// Default function:             Function 1
// Native function 1:            FUSA_DIAGTEST_MODE
// Native function 6:            IOM_GPPB_20
#define GPIOV2_MTL_PCH_S_GPP_B_20               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 20)

// Pin:                          GPP_B_21 ( Value: 0x515 )
// Family:                       MiscB
// Default function:             GP-Out
// Native function 1:            SML1ALERTB
// Native function 2:            PCHHOTB
// Native function 6:            IOM_GPPB_21
#define GPIOV2_MTL_PCH_S_GPP_B_21               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B, MTL_PCH_S_GRP_IDX_GPP_B, 21)


//
// Group name: DSW
// Community index: 2
// Group index: 0
// Number of pads: 13
//


// Pin:                          GPD_0 ( Value: 0x800 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            BATLOWB
#define GPIOV2_MTL_PCH_S_GPD_0                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 0)

// Pin:                          GPD_1 ( Value: 0x801 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            ACPRESENT
#define GPIOV2_MTL_PCH_S_GPD_1                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 1)

// Pin:                          GPD_2 ( Value: 0x802 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            LAN_WAKEB
#define GPIOV2_MTL_PCH_S_GPD_2                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 2)

// Pin:                          GPD_3 ( Value: 0x803 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            PWRBTNB
#define GPIOV2_MTL_PCH_S_GPD_3                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 3)

// Pin:                          GPD_4 ( Value: 0x804 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SLP_S3B
#define GPIOV2_MTL_PCH_S_GPD_4                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 4)

// Pin:                          GPD_5 ( Value: 0x805 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SLP_S4B
#define GPIOV2_MTL_PCH_S_GPD_5                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 5)

// Pin:                          GPD_6 ( Value: 0x806 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SLP_AB
#define GPIOV2_MTL_PCH_S_GPD_6                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 6)

// Pin:                          GPD_7 ( Value: 0x807 )
// Family:                       DSW
// Default function:             GP-Out
#define GPIOV2_MTL_PCH_S_GPD_7                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 7)

// Pin:                          GPD_8 ( Value: 0x808 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SUSCLK
#define GPIOV2_MTL_PCH_S_GPD_8                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 8)

// Pin:                          GPD_9 ( Value: 0x809 )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SLP_WLANB
#define GPIOV2_MTL_PCH_S_GPD_9                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 9)

// Pin:                          GPD_10 ( Value: 0x80a )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SLP_S5B
#define GPIOV2_MTL_PCH_S_GPD_10                 GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 10)

// Pin:                          GPD_11 ( Value: 0x80b )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            LANPHYPC
#define GPIOV2_MTL_PCH_S_GPD_11                 GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 11)

// Pin:                          GPD_12 ( Value: 0x80c )
// Family:                       DSW
// Default function:             GP-Out
#define GPIOV2_MTL_PCH_S_GPD_12                 GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 12)



//
// Group name: GPP_C
// Community index: 3
// Group index: 0
// Number of pads: 24
//


// Pin:                          GPP_C_0 ( Value: 0xc80 )
// Family:                       SMBus0
// Default function:             Function 1
// Native function 1:            SMBCLK
// Native function 6:            IOM_GPPC_0
#define GPIOV2_MTL_PCH_S_GPP_C_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 0)

// Pin:                          GPP_C_1 ( Value: 0xc81 )
// Family:                       SMBus0
// Default function:             Function 1
// Native function 1:            SMBDATA
// Native function 6:            IOM_GPPC_1
#define GPIOV2_MTL_PCH_S_GPP_C_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 1)

// Pin:                          GPP_C_2 ( Value: 0xc82 )
// Family:                       SMBus0
// Default function:             RW-4'b0000[gpio_sstrap_smbalertb? 4'b0001:4'b0000]
// Native function 1:            SMBALERTB
// Native function 6:            IOM_GPPC_2
#define GPIOV2_MTL_PCH_S_GPP_C_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 2)

// Pin:                          GPP_C_3 ( Value: 0xc83 )
// Family:                       SML0_I2C2
// Default function:             GP-In
// Native function 2:            ISH_UART0_RXD
// Native function 2:            SML0BDATA
// Native function 3:            I2C2_SDA
// Native function 6:            IOM_GPPC_3
#define GPIOV2_MTL_PCH_S_GPP_C_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 3)

// Pin:                          GPP_C_4 ( Value: 0xc84 )
// Family:                       SML0_I2C2
// Default function:             GP-In
// Native function 2:            ISH_UART0_TXD
// Native function 2:            SML0BCLK
// Native function 3:            I2C2_SCL
// Native function 6:            IOM_GPPC_4
#define GPIOV2_MTL_PCH_S_GPP_C_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 4)

// Pin:                          GPP_C_5 ( Value: 0xc85 )
// Family:                       SMBus0
// Default function:             GP-Out
// Native function 1:            SML0ALERTB
// Native function 6:            IOM_GPPC_5
#define GPIOV2_MTL_PCH_S_GPP_C_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 5)

// Pin:                          GPP_C_6 ( Value: 0xc86 )
// Family:                       I2C3
// Default function:             GP-In
// Native function 1:            ISH_I2C2_SDA
// Native function 2:            I2C3_SDA
// Native function 6:            IOM_GPPC_6
#define GPIOV2_MTL_PCH_S_GPP_C_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 6)

// Pin:                          GPP_C_7 ( Value: 0xc87 )
// Family:                       I2C3
// Default function:             GP-In
// Native function 1:            ISH_I2C2_SCL
// Native function 2:            I2C3_SCL
// Native function 6:            IOM_GPPC_7
#define GPIOV2_MTL_PCH_S_GPP_C_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 7)

// Pin:                          GPP_C_8 ( Value: 0xc88 )
// Family:                       UART012
// Default function:             GP-In
// Native function 2:            UART0_RXD
// Native function 6:            IOM_GPPC_8
#define GPIOV2_MTL_PCH_S_GPP_C_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 8)

// Pin:                          GPP_C_9 ( Value: 0xc89 )
// Family:                       UART012
// Default function:             GP-In
// Native function 2:            UART0_TXD
// Native function 6:            IOM_GPPC_9
#define GPIOV2_MTL_PCH_S_GPP_C_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 9)

// Pin:                          GPP_C_10 ( Value: 0xc8a )
// Family:                       UART012
// Default function:             GP-In
// Native function 2:            UART0_RTSB
// Native function 6:            IOM_GPPC_10
#define GPIOV2_MTL_PCH_S_GPP_C_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 10)

// Pin:                          GPP_C_11 ( Value: 0xc8b )
// Family:                       UART012
// Default function:             GP-In
// Native function 2:            UART0_CTSB
// Native function 6:            IOM_GPPC_11
#define GPIOV2_MTL_PCH_S_GPP_C_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 11)

// Pin:                          GPP_C_12 ( Value: 0xc8c )
// Family:                       UART012
// Default function:             GP-In
// Native function 1:            UART1_RXD
// Native function 2:            ISH_UART1_RXD
// Native function 6:            IOM_GPPC_12
#define GPIOV2_MTL_PCH_S_GPP_C_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 12)

// Pin:                          GPP_C_13 ( Value: 0xc8d )
// Family:                       UART012
// Default function:             GP-In
// Native function 1:            UART1_TXD
// Native function 2:            ISH_UART1_TXD
// Native function 6:            IOM_GPPC_13
#define GPIOV2_MTL_PCH_S_GPP_C_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 13)

// Pin:                          GPP_C_14 ( Value: 0xc8e )
// Family:                       UART012
// Default function:             GP-In
// Native function 1:            UART1_RTSB
// Native function 2:            ISH_UART1_RTSB
// Native function 6:            IOM_GPPC_14
#define GPIOV2_MTL_PCH_S_GPP_C_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 14)

// Pin:                          GPP_C_15 ( Value: 0xc8f )
// Family:                       UART012
// Default function:             GP-In
// Native function 1:            UART1_CTSB
// Native function 2:            ISH_UART1_CTSB
// Native function 6:            IOM_GPPC_15
#define GPIOV2_MTL_PCH_S_GPP_C_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 15)

// Pin:                          GPP_C_16 ( Value: 0xc90 )
// Family:                       I2C4
// Default function:             GP-In
// Native function 1:            USB2_OCB_4
// Native function 2:            I2C4_SDA
// Native function 6:            IOM_GPPC_16
#define GPIOV2_MTL_PCH_S_GPP_C_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 16)

// Pin:                          GPP_C_17 ( Value: 0xc91 )
// Family:                       I2C4
// Default function:             GP-In
// Native function 1:            USB2_OCB_5
// Native function 2:            I2C4_SCL
// Native function 6:            IOM_GPPC_17
#define GPIOV2_MTL_PCH_S_GPP_C_17               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 17)

// Pin:                          GPP_C_18 ( Value: 0xc92 )
// Family:                       I2C5
// Default function:             GP-In
// Native function 1:            USB2_OCB_6
// Native function 2:            I2C5_SDA
// Native function 6:            IOM_GPPC_18
#define GPIOV2_MTL_PCH_S_GPP_C_18               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 18)

// Pin:                          GPP_C_19 ( Value: 0xc93 )
// Family:                       I2C5
// Default function:             GP-In
// Native function 1:            USB2_OCB_7
// Native function 2:            I2C5_SCL
// Native function 6:            IOM_GPPC_19
#define GPIOV2_MTL_PCH_S_GPP_C_19               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 19)

// Pin:                          GPP_C_20 ( Value: 0xc94 )
// Family:                       UART012
// Default function:             GP-In
// Native function 2:            UART2_RXD
// Native function 3:            CNV_MFUART0_RXD
// Native function 6:            IOM_GPPC_20
#define GPIOV2_MTL_PCH_S_GPP_C_20               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 20)

// Pin:                          GPP_C_21 ( Value: 0xc95 )
// Family:                       UART012
// Default function:             GP-In
// Native function 2:            UART2_TXD
// Native function 3:            CNV_MFUART0_TXD
// Native function 6:            IOM_GPPC_21
#define GPIOV2_MTL_PCH_S_GPP_C_21               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 21)

// Pin:                          GPP_C_22 ( Value: 0xc96 )
// Family:                       UART012
// Default function:             GP-In
// Native function 1:            UART2_RTSB
// Native function 3:            CNV_MFUART0_RTS_B
// Native function 6:            IOM_GPPC_22
#define GPIOV2_MTL_PCH_S_GPP_C_22               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 22)

// Pin:                          GPP_C_23 ( Value: 0xc97 )
// Family:                       UART012
// Default function:             GP-In
// Native function 1:            UART2_CTSB
// Native function 3:            CNV_MFUART0_CTS_B
// Native function 6:            IOM_GPPC_23
#define GPIOV2_MTL_PCH_S_GPP_C_23               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C, MTL_PCH_S_GRP_IDX_GPP_C, 23)


//
// Group name: GPP_H
// Community index: 3
// Group index: 1
// Number of pads: 20
//


// Pin:                          GPP_H_0 ( Value: 0xd00 )
// Family:                       MiscH
// Default function:             GP-Out
// Native function 6:            IOM_GPPH_0
#define GPIOV2_MTL_PCH_S_GPP_H_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 0)

// Pin:                          GPP_H_1 ( Value: 0xd01 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_8
// Native function 6:            IOM_GPPH_1
#define GPIOV2_MTL_PCH_S_GPP_H_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 1)

// Pin:                          GPP_H_2 ( Value: 0xd02 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_9
// Native function 6:            IOM_GPPH_2
#define GPIOV2_MTL_PCH_S_GPP_H_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 2)

// Pin:                          GPP_H_3 ( Value: 0xd03 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_10
// Native function 6:            IOM_GPPH_3
#define GPIOV2_MTL_PCH_S_GPP_H_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 3)

// Pin:                          GPP_H_4 ( Value: 0xd04 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_11
// Native function 6:            IOM_GPPH_4
#define GPIOV2_MTL_PCH_S_GPP_H_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 4)

// Pin:                          GPP_H_5 ( Value: 0xd05 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            USB2_OCB_0
// Native function 6:            IOM_GPPH_5
#define GPIOV2_MTL_PCH_S_GPP_H_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 5)

// Pin:                          GPP_H_6 ( Value: 0xd06 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            USB2_OCB_1
// Native function 6:            IOM_GPPH_6
#define GPIOV2_MTL_PCH_S_GPP_H_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 6)

// Pin:                          GPP_H_7 ( Value: 0xd07 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            USB2_OCB_2
// Native function 2:            GMII_MDC_1
// Native function 6:            IOM_GPPH_7
#define GPIOV2_MTL_PCH_S_GPP_H_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 7)

// Pin:                          GPP_H_8 ( Value: 0xd08 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            USB2_OCB_3
// Native function 2:            GMII_MDIO_1
// Native function 6:            IOM_GPPH_8
#define GPIOV2_MTL_PCH_S_GPP_H_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 8)

// Pin:                          GPP_H_9 ( Value: 0xd09 )
// Family:                       SML2
// Default function:             GP-In
// Native function 1:            SML2CLK
// Native function 6:            IOM_GPPH_9
#define GPIOV2_MTL_PCH_S_GPP_H_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 9)

// Pin:                          GPP_H_10 ( Value: 0xd0a )
// Family:                       SML2
// Default function:             GP-In
// Native function 1:            SML2DATA
// Native function 6:            IOM_GPPH_10
#define GPIOV2_MTL_PCH_S_GPP_H_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 10)

// Pin:                          GPP_H_11 ( Value: 0xd0b )
// Family:                       SML2
// Default function:             GP-Out
// Native function 1:            SML2ALERTB
// Native function 6:            IOM_GPPH_11
#define GPIOV2_MTL_PCH_S_GPP_H_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 11)

// Pin:                          GPP_H_12 ( Value: 0xd0c )
// Family:                       SML3
// Default function:             GP-In
// Native function 1:            SML3CLK
// Native function 6:            IOM_GPPH_12
#define GPIOV2_MTL_PCH_S_GPP_H_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 12)

// Pin:                          GPP_H_13 ( Value: 0xd0d )
// Family:                       SML3
// Default function:             GP-In
// Native function 1:            SML3DATA
// Native function 6:            IOM_GPPH_13
#define GPIOV2_MTL_PCH_S_GPP_H_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 13)

// Pin:                          GPP_H_14 ( Value: 0xd0e )
// Family:                       SML3
// Default function:             GP-Out
// Native function 1:            SML3ALERTB
// Native function 6:            IOM_GPPH_14
#define GPIOV2_MTL_PCH_S_GPP_H_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 14)

// Pin:                          GPP_H_15 ( Value: 0xd0f )
// Family:                       SML4
// Default function:             GP-In
// Native function 1:            SML4CLK
// Native function 6:            IOM_GPPH_15
#define GPIOV2_MTL_PCH_S_GPP_H_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 15)

// Pin:                          GPP_H_16 ( Value: 0xd10 )
// Family:                       SML4
// Default function:             GP-In
// Native function 1:            SML4DATA
// Native function 6:            IOM_GPPH_16
#define GPIOV2_MTL_PCH_S_GPP_H_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 16)

// Pin:                          GPP_H_17 ( Value: 0xd11 )
// Family:                       SML4
// Default function:             GP-Out
// Native function 1:            SML4ALERTB
// Native function 6:            IOM_GPPH_17
#define GPIOV2_MTL_PCH_S_GPP_H_17               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 17)

// Pin:                          GPP_H_18 ( Value: 0xd12 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            ISH_I2C1_SDA
// Native function 2:            SMIB
// Native function 6:            IOM_GPPH_18
#define GPIOV2_MTL_PCH_S_GPP_H_18               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 18)

// Pin:                          GPP_H_19 ( Value: 0xd13 )
// Family:                       MiscH
// Default function:             GP-In
// Native function 1:            ISH_I2C1_SCL
// Native function 2:            NMIB
// Native function 6:            IOM_GPPH_19
#define GPIOV2_MTL_PCH_S_GPP_H_19               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H, MTL_PCH_S_GRP_IDX_GPP_H, 19)



//
// Group name: GPP_S
// Community index: 4
// Group index: 0
// Number of pads: 8
//


// Pin:                          GPP_S_0 ( Value: 0x1000 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW0_CLK
// Native function 3:            RGMII_AUXTS_0
#define GPIOV2_MTL_PCH_S_GPP_S_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 0)

// Pin:                          GPP_S_1 ( Value: 0x1001 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW0_DATA0
// Native function 3:            RGMII_INT_0
#define GPIOV2_MTL_PCH_S_GPP_S_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 1)

// Pin:                          GPP_S_2 ( Value: 0x1002 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW1_CLK
// Native function 5:            DMIC_CLK_A_0
// Native function 3:            RGMII_RESET_0
// Native function 4:            SNDW0_DATA1
#define GPIOV2_MTL_PCH_S_GPP_S_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 2)

// Pin:                          GPP_S_3 ( Value: 0x1003 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW1_DATA
// Native function 5:            DMIC_DATA_0
// Native function 3:            RGMII_PPS_0
// Native function 4:            SNDW0_DATA2
#define GPIOV2_MTL_PCH_S_GPP_S_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 3)

// Pin:                          GPP_S_4 ( Value: 0x1004 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW2_CLK
// Native function 5:            DMIC_CLK_B_0
// Native function 3:            RGMII_AUXTS_1
// Native function 4:            SNDW0_DATA3
#define GPIOV2_MTL_PCH_S_GPP_S_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 4)

// Pin:                          GPP_S_5 ( Value: 0x1005 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW2_DATA
// Native function 5:            DMIC_CLK_B_1
// Native function 3:            RGMII_INT_1
#define GPIOV2_MTL_PCH_S_GPP_S_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 5)

// Pin:                          GPP_S_6 ( Value: 0x1006 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW3_CLK
// Native function 5:            DMIC_CLK_A_1
// Native function 3:            RGMII_RESET_1
#define GPIOV2_MTL_PCH_S_GPP_S_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 6)

// Pin:                          GPP_S_7 ( Value: 0x1007 )
// Family:                       SWND
// Default function:             GP-In
// Native function 1:            SNDW3_DATA
// Native function 5:            DMIC_DATA_1
// Native function 3:            RGMII_PPS_1
#define GPIOV2_MTL_PCH_S_GPP_S_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_S, MTL_PCH_S_GRP_IDX_GPP_S, 7)


//
// Group name: GPP_E
// Community index: 4
// Group index: 1
// Number of pads: 22
//


// Pin:                          GPP_E_0 ( Value: 0x1080 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SATAXPCIE_0
// Native function 2:            SATAGP_0
// Native function 6:            IOM_GPPE_0
#define GPIOV2_MTL_PCH_S_GPP_E_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 0)

// Pin:                          GPP_E_1 ( Value: 0x1081 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SATAXPCIE_1
// Native function 2:            SATAGP_1
// Native function 6:            IOM_GPPE_1
#define GPIOV2_MTL_PCH_S_GPP_E_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 1)

// Pin:                          GPP_E_2 ( Value: 0x1082 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SATAXPCIE_2
// Native function 2:            SATAGP_2
// Native function 6:            IOM_GPPE_2
#define GPIOV2_MTL_PCH_S_GPP_E_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 2)

// Pin:                          GPP_E_3 ( Value: 0x1083 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            CPU_GP_0
// Native function 6:            IOM_GPPE_3
#define GPIOV2_MTL_PCH_S_GPP_E_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 3)

// Pin:                          GPP_E_4 ( Value: 0x1084 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SATA_DEVSLP_0
// Native function 6:            IOM_GPPE_4
#define GPIOV2_MTL_PCH_S_GPP_E_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 4)

// Pin:                          GPP_E_5 ( Value: 0x1085 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SATA_DEVSLP_1
// Native function 6:            IOM_GPPE_5
#define GPIOV2_MTL_PCH_S_GPP_E_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 5)

// Pin:                          GPP_E_6 ( Value: 0x1086 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SATA_DEVSLP_2
// Native function 6:            IOM_GPPE_6
#define GPIOV2_MTL_PCH_S_GPP_E_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 6)

// Pin:                          GPP_E_7 ( Value: 0x1087 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            CPU_GP_1
// Native function 6:            IOM_GPPE_7
#define GPIOV2_MTL_PCH_S_GPP_E_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 7)

// Pin:                          GPP_E_8 ( Value: 0x1088 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SATA_LEDB
// Native function 6:            IOM_GPPE_8
#define GPIOV2_MTL_PCH_S_GPP_E_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 8)

// Pin:                          GPP_E_9 ( Value: 0x1089 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_12
// Native function 6:            IOM_GPPE_9
#define GPIOV2_MTL_PCH_S_GPP_E_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 9)

// Pin:                          GPP_E_10 ( Value: 0x108a )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            SRCCLKREQB_13
// Native function 6:            IOM_GPPE_10
#define GPIOV2_MTL_PCH_S_GPP_E_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 10)

// Pin:                          GPP_E_11 ( Value: 0x108b )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            ISH_GP_9
// Native function 6:            IOM_GPPE_11
#define GPIOV2_MTL_PCH_S_GPP_E_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 11)

// Pin:                          GPP_E_12 ( Value: 0x108c )
// Family:                       MiscE
// Default function:             GP-In
// Native function 1:            ISH_GP_10
// Native function 6:            IOM_GPPE_12
#define GPIOV2_MTL_PCH_S_GPP_E_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 12)

// Pin:                          GPP_E_13 ( Value: 0x108d )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_CSB
// Native function 3:            Serial_Blink
// Native function 4:            Blink
// Native function 6:            IOM_GPPE_13
#define GPIOV2_MTL_PCH_S_GPP_E_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 13)

// Pin:                          GPP_E_14 ( Value: 0x108e )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_CLK
// Native function 3:            Serial_Blink
// Native function 4:            Blink
// Native function 6:            IOM_GPPE_14
#define GPIOV2_MTL_PCH_S_GPP_E_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 14)

// Pin:                          GPP_E_15 ( Value: 0x108f )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_IO_1
// Native function 3:            Serial_Blink
// Native function 4:            Blink
// Native function 6:            IOM_GPPE_15
#define GPIOV2_MTL_PCH_S_GPP_E_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 15)

// Pin:                          GPP_E_16 ( Value: 0x1090 )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_IO_0
// Native function 3:            Serial_Blink
// Native function 4:            Blink
// Native function 6:            IOM_GPPE_16
#define GPIOV2_MTL_PCH_S_GPP_E_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 16)

// Pin:                          GPP_E_17 ( Value: 0x1091 )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_IO_2
// Native function 6:            IOM_GPPE_17
#define GPIOV2_MTL_PCH_S_GPP_E_17               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 17)

// Pin:                          GPP_E_18 ( Value: 0x1092 )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_IO_3
// Native function 6:            IOM_GPPE_18
#define GPIOV2_MTL_PCH_S_GPP_E_18               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 18)

// Pin:                          GPP_E_19 ( Value: 0x1093 )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_RSTB
// Native function 6:            IOM_GPPE_19
#define GPIOV2_MTL_PCH_S_GPP_E_19               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 19)

// Pin:                          GPP_E_20 ( Value: 0x1094 )
// Family:                       THC0_SPI1
// Default function:             GP-In
// Native function 2:            THC0_SPI1_INTB
// Native function 6:            IOM_GPPE_20
#define GPIOV2_MTL_PCH_S_GPP_E_20               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 20)

// Pin:                          GPP_E_21 ( Value: 0x1095 )
// Family:                       MiscE
// Default function:             GP-In
// Native function 2:            ISH_UART0_CTSB
// Native function 2:            SML0BALERTB
// Native function 6:            IOM_GPPE_21
#define GPIOV2_MTL_PCH_S_GPP_E_21               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 21)



//
// Group name: GPP_K
// Community index: 4
// Group index: 2
// Number of pads: 11
//


// Pin:                          GPP_K_0 ( Value: 0x1100 )
// Family:                       GSX
// Default function:             GP-In
// Native function 1:            GSXDOUT
// Native function 3:            Serial_Blink
// Native function 4:            Blink
// Native function 6:            IOM_GPPK_0
#define GPIOV2_MTL_PCH_S_GPP_K_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 0)

// Pin:                          GPP_K_1 ( Value: 0x1101 )
// Family:                       GSX
// Default function:             GP-In
// Native function 1:            GSXSLOAD
// Native function 6:            IOM_GPPK_1
#define GPIOV2_MTL_PCH_S_GPP_K_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 1)

// Pin:                          GPP_K_2 ( Value: 0x1102 )
// Family:                       GSX
// Default function:             GP-In
// Native function 1:            GSXDIN
// Native function 6:            IOM_GPPK_2
#define GPIOV2_MTL_PCH_S_GPP_K_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 2)

// Pin:                          GPP_K_3 ( Value: 0x1103 )
// Family:                       GSX
// Default function:             GP-In
// Native function 1:            GSXSRESETB
// Native function 6:            IOM_GPPK_3
#define GPIOV2_MTL_PCH_S_GPP_K_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 3)

// Pin:                          GPP_K_4 ( Value: 0x1104 )
// Family:                       GSX
// Default function:             GP-In
// Native function 1:            GSXCLK
// Native function 6:            IOM_GPPK_4
#define GPIOV2_MTL_PCH_S_GPP_K_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 4)

// Pin:                          GPP_K_5 ( Value: 0x1105 )
// Family:                       MiscK
// Default function:             GP-In
// Native function 1:            ADR_COMPLETE
// Native function 6:            IOM_GPPK_5
#define GPIOV2_MTL_PCH_S_GPP_K_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 5)

// Pin:                          GPP_K_6 ( Value: 0x1106 )
// Family:                       MiscK
// Default function:             Function 2
// Native function 2:            FUSE_SORT_BUMP_0
// Native function 6:            IOM_GPPK_6
#define GPIOV2_MTL_PCH_S_GPP_K_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 6)

// Pin:                          GPP_K_7 ( Value: 0x1107 )
// Family:                       MiscK
// Default function:             Function 2
// Native function 2:            FUSE_SORT_BUMP_1
// Native function 6:            IOM_GPPK_7
#define GPIOV2_MTL_PCH_S_GPP_K_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 7)

// Pin:                          GPP_K_8 ( Value: 0x1108 )
// Family:                       MiscK
// Default function:             Function 1
// Native function 1:            CORE_VID_0
// Native function 6:            IOM_GPPK_8
#define GPIOV2_MTL_PCH_S_GPP_K_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 8)

// Pin:                          GPP_K_9 ( Value: 0x1109 )
// Family:                       MiscK
// Default function:             Function 1
// Native function 1:            CORE_VID_1
// Native function 6:            IOM_GPPK_9
#define GPIOV2_MTL_PCH_S_GPP_K_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 9)

// Pin:                          GPP_K_10 ( Value: 0x110a )
// Family:                       MiscK
// Default function:             Function 2
// Native function 2:            FUSE_SORT_BUMP_2
// Native function 6:            IOM_GPPK_10
#define GPIOV2_MTL_PCH_S_GPP_K_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 10)



//
// Group name: GPP_F
// Community index: 4
// Group index: 3
// Number of pads: 24
//


// Pin:                          GPP_F_0 ( Value: 0x1180 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATAXPCIE_3
// Native function 2:            SATAGP_3
// Native function 3:            VISA2CH1_D0
// Native function 4:            PTI_TRACE_D00
// Native function 5:            VISAFUSECH1_D0
// Native function 6:            IOM_GPPF_0
#define GPIOV2_MTL_PCH_S_GPP_F_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 0)

// Pin:                          GPP_F_1 ( Value: 0x1181 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATAXPCIE_4
// Native function 2:            SATAGP_4
// Native function 3:            VISA2CH1_D1
// Native function 4:            PTI_TRACE_D01
// Native function 5:            VISAFUSECH1_D1
// Native function 6:            IOM_GPPF_1
#define GPIOV2_MTL_PCH_S_GPP_F_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 1)

// Pin:                          GPP_F_2 ( Value: 0x1182 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATAXPCIE_5
// Native function 2:            SATAGP_5
// Native function 3:            VISA2CH1_D2
// Native function 4:            PTI_TRACE_D02
// Native function 5:            VISAFUSECH1_D2
// Native function 6:            IOM_GPPF_2
#define GPIOV2_MTL_PCH_S_GPP_F_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 2)

// Pin:                          GPP_F_3 ( Value: 0x1183 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATAXPCIE_6
// Native function 2:            SATAGP_6
// Native function 3:            VISA2CH1_D3
// Native function 4:            PTI_TRACE_D03
// Native function 5:            VISAFUSECH1_D3
// Native function 6:            IOM_GPPF_3
#define GPIOV2_MTL_PCH_S_GPP_F_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 3)

// Pin:                          GPP_F_4 ( Value: 0x1184 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATAXPCIE_7
// Native function 2:            SATAGP_7
// Native function 3:            VISA2CH1_D4
// Native function 4:            PTI_TRACE_D04
// Native function 5:            VISAFUSECH1_D4
// Native function 6:            IOM_GPPF_4
#define GPIOV2_MTL_PCH_S_GPP_F_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 4)

// Pin:                          GPP_F_5 ( Value: 0x1185 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATA_DEVSLP_3
// Native function 3:            VISA2CH1_D5
// Native function 4:            PTI_TRACE_D05
// Native function 5:            VISAFUSECH1_D5
// Native function 6:            IOM_GPPF_5
#define GPIOV2_MTL_PCH_S_GPP_F_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 5)

// Pin:                          GPP_F_6 ( Value: 0x1186 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATA_DEVSLP_4
// Native function 3:            VISA2CH1_D6
// Native function 4:            PTI_TRACE_D06
// Native function 5:            VISAFUSECH1_D6
// Native function 6:            IOM_GPPF_6
#define GPIOV2_MTL_PCH_S_GPP_F_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 6)

// Pin:                          GPP_F_7 ( Value: 0x1187 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATA_DEVSLP_5
// Native function 3:            VISA2CH1_D7
// Native function 4:            PTI_TRACE_D07
// Native function 5:            VISAFUSECH1_D7
// Native function 6:            IOM_GPPF_7
#define GPIOV2_MTL_PCH_S_GPP_F_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 7)

// Pin:                          GPP_F_8 ( Value: 0x1188 )
// Family:                       VISA2
// Default function:             RW - 4'b0000 [gpcom_strap_dfxtestmode_active_b ? 4'b0000 : 4'b0101]
// Native function 1:            SATA_DEVSLP_6
// Native function 3:            VISA2CH1_CLK
// Native function 4:            PTI_TRACE_CLK
// Native function 5:            VISAFUSECH1_CLK
// Native function 6:            IOM_GPPF_8
#define GPIOV2_MTL_PCH_S_GPP_F_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 8)

// Pin:                          GPP_F_9 ( Value: 0x1189 )
// Family:                       VISA2
// Default function:             GP-In
// Native function 1:            SATA_DEVSLP_7
// Native function 3:            VISA2CH2_D0
// Native function 4:            PTI_TRACE_D08
// Native function 5:            DIR_ESPI_SMI
// Native function 6:            IOM_GPPF_9
#define GPIOV2_MTL_PCH_S_GPP_F_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 9)

// Pin:                          GPP_F_10 ( Value: 0x118a )
// Family:                       VISA2
// Default function:             GP-In
// Native function 1:            SATA_SCLOCK
// Native function 3:            VISA2CH2_D1
// Native function 4:            PTI_TRACE_D09
// Native function 5:            DIR_ESPI_NMI
// Native function 6:            IOM_GPPF_10
#define GPIOV2_MTL_PCH_S_GPP_F_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 10)

// Pin:                          GPP_F_11 ( Value: 0x118b )
// Family:                       VISA2
// Default function:             GP-In
// Native function 1:            SATA_SLOAD
// Native function 3:            VISA2CH2_D2
// Native function 4:            PTI_TRACE_D10
// Native function 5:            DIR_ESPI_IRQ
// Native function 6:            IOM_GPPF_11
#define GPIOV2_MTL_PCH_S_GPP_F_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 11)

// Pin:                          GPP_F_12 ( Value: 0x118c )
// Family:                       VISA2
// Default function:             GP-In
// Native function 1:            SATA_SDATAOUT1
// Native function 3:            VISA2CH2_D3
// Native function 4:            PTI_TRACE_D11
// Native function 5:            DIR_ESPI_WAKE
// Native function 6:            IOM_GPPF_12
#define GPIOV2_MTL_PCH_S_GPP_F_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 12)

// Pin:                          GPP_F_13 ( Value: 0x118d )
// Family:                       VISA2
// Default function:             GP-In
// Native function 1:            SATA_SDATAOUT0
// Native function 3:            VISA2CH2_D4
// Native function 4:            PTI_TRACE_D12
// Native function 5:            DIR_ESPI_SCI
// Native function 6:            IOM_GPPF_13
#define GPIOV2_MTL_PCH_S_GPP_F_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 13)

// Pin:                          GPP_F_14 ( Value: 0x118e )
// Family:                       VISA2
// Default function:             GP-In
// Native function 3:            VISA2CH2_D5
// Native function 4:            PTI_TRACE_D13
// Native function 6:            IOM_GPPF_14
#define GPIOV2_MTL_PCH_S_GPP_F_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 14)

// Pin:                          GPP_F_15 ( Value: 0x118f )
// Family:                       VISA2
// Default function:             GP-In
// Native function 3:            VISA2CH2_D6
// Native function 4:            PTI_TRACE_D14
// Native function 6:            IOM_GPPF_15
#define GPIOV2_MTL_PCH_S_GPP_F_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 15)

// Pin:                          GPP_F_16 ( Value: 0x1190 )
// Family:                       VISA2
// Default function:             GP-In
// Native function 3:            VISA2CH2_D7
// Native function 4:            PTI_TRACE_D15
// Native function 6:            IOM_GPPF_16
#define GPIOV2_MTL_PCH_S_GPP_F_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 16)

// Pin:                          GPP_F_17 ( Value: 0x1191 )
// Family:                       VISA2
// Default function:             GP-In
// Native function 3:            VISA2CH2_CLK
// Native function 4:            PTI_TRACE_VLD
// Native function 6:            IOM_GPPF_17
#define GPIOV2_MTL_PCH_S_GPP_F_17               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 17)

// Pin:                          GPP_F_18 ( Value: 0x1192 )
// Family:                       MiscF
// Default function:             GP-In
// Native function 6:            IOM_GPPF_18
#define GPIOV2_MTL_PCH_S_GPP_F_18               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 18)

// Pin:                          GPP_F_19 ( Value: 0x1193 )
// Family:                       MiscF
// Default function:             Function 1
// Native function 1:            DNX_FORCE_RELOAD
// Native function 6:            IOM_GPPF_19
#define GPIOV2_MTL_PCH_S_GPP_F_19               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 19)

// Pin:                          GPP_F_20 ( Value: 0x1194 )
// Family:                       MiscF
// Default function:             GP-In
// Native function 1:            GMII_MDC_0
// Native function 6:            IOM_GPPF_20
#define GPIOV2_MTL_PCH_S_GPP_F_20               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 20)

// Pin:                          GPP_F_21 ( Value: 0x1195 )
// Family:                       MiscF
// Default function:             GP-In
// Native function 1:            GMII_MDIO_0
// Native function 6:            IOM_GPPF_21
#define GPIOV2_MTL_PCH_S_GPP_F_21               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 21)

// Pin:                          GPP_F_22 ( Value: 0x1196 )
// Family:                       MiscF
// Default function:             GP-In
// Native function 3:            IEH_CORR_ERR0B
// Native function 6:            IOM_GPPF_22
#define GPIOV2_MTL_PCH_S_GPP_F_22               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 22)

// Pin:                          GPP_F_23 ( Value: 0x1197 )
// Family:                       MiscF
// Default function:             GP-In
// Native function 3:            IEH_NONFATAL_ERR1B
// Native function 6:            IOM_GPPF_23
#define GPIOV2_MTL_PCH_S_GPP_F_23               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_F, MTL_PCH_S_GRP_IDX_GPP_F, 23)


//
// Group name: GPP_I
// Community index: 5
// Group index: 0
// Number of pads: 17
//


// Pin:                          GPP_I_0 ( Value: 0x1400 )
// Family:                       GSPI0
// Default function:             GP-In
// Native function 1:            GSPI0_CS1B
// Native function 6:            IOM_GPPI_0
#define GPIOV2_MTL_PCH_S_GPP_I_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 0)

// Pin:                          GPP_I_1 ( Value: 0x1401 )
// Family:                       GSPI1
// Default function:             GP-In
// Native function 1:            GSPI1_CS1B
// Native function 6:            IOM_GPPI_1
#define GPIOV2_MTL_PCH_S_GPP_I_1                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 1)

// Pin:                          GPP_I_2 ( Value: 0x1402 )
// Family:                       I2C0
// Default function:             GP-In
// Native function 1:            I2C0_SDA
// Native function 2:            I3C0_SDA
// Native function 6:            IOM_GPPI_2
#define GPIOV2_MTL_PCH_S_GPP_I_2                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 2)

// Pin:                          GPP_I_3 ( Value: 0x1403 )
// Family:                       I2C0
// Default function:             GP-In
// Native function 1:            I2C0_SCL
// Native function 2:            I3C0_SCL
// Native function 6:            IOM_GPPI_3
#define GPIOV2_MTL_PCH_S_GPP_I_3                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 3)

// Pin:                          GPP_I_4 ( Value: 0x1404 )
// Family:                       I2C1
// Default function:             GP-In
// Native function 1:            I2C1_SDA
// Native function 6:            IOM_GPPI_4
#define GPIOV2_MTL_PCH_S_GPP_I_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 4)

// Pin:                          GPP_I_5 ( Value: 0x1405 )
// Family:                       I2C1
// Default function:             GP-In
// Native function 1:            I2C1_SCL
// Native function 6:            IOM_GPPI_5
#define GPIOV2_MTL_PCH_S_GPP_I_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 5)

// Pin:                          GPP_I_6 ( Value: 0x1406 )
// Family:                       GSPI0
// Default function:             GP-In
// Native function 1:            GSPI0_CS0B
// Native function 6:            IOM_GPPI_6
#define GPIOV2_MTL_PCH_S_GPP_I_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 6)

// Pin:                          GPP_I_7 ( Value: 0x1407 )
// Family:                       GSPI0
// Default function:             GP-In
// Native function 1:            GSPI0_CLK
// Native function 6:            IOM_GPPI_7
#define GPIOV2_MTL_PCH_S_GPP_I_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 7)

// Pin:                          GPP_I_8 ( Value: 0x1408 )
// Family:                       GSPI0
// Default function:             GP-In
// Native function 1:            GSPI0_MISO
// Native function 6:            IOM_GPPI_8
#define GPIOV2_MTL_PCH_S_GPP_I_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 8)

// Pin:                          GPP_I_9 ( Value: 0x1409 )
// Family:                       GSPI0
// Default function:             GP-Out
// Native function 1:            GSPI0_MOSI
// Native function 6:            IOM_GPPI_9
#define GPIOV2_MTL_PCH_S_GPP_I_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 9)

// Pin:                          GPP_I_10 ( Value: 0x140a )
// Family:                       GSPI1
// Default function:             GP-In
// Native function 1:            GSPI1_CS0B
// Native function 6:            IOM_GPPI_10
#define GPIOV2_MTL_PCH_S_GPP_I_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 10)

// Pin:                          GPP_I_11 ( Value: 0x140b )
// Family:                       GSPI1
// Default function:             GP-In
// Native function 1:            GSPI1_CLK
// Native function 2:            NFC_CLK
// Native function 6:            IOM_GPPI_11
#define GPIOV2_MTL_PCH_S_GPP_I_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 11)

// Pin:                          GPP_I_12 ( Value: 0x140c )
// Family:                       GSPI1
// Default function:             GP-In
// Native function 1:            GSPI1_MISO
// Native function 2:            NFC_CLKREQ
// Native function 6:            IOM_GPPI_12
#define GPIOV2_MTL_PCH_S_GPP_I_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 12)

// Pin:                          GPP_I_13 ( Value: 0x140d )
// Family:                       GSPI1
// Default function:             GP-Out
// Native function 1:            GSPI1_MOSI
// Native function 6:            IOM_GPPI_13
#define GPIOV2_MTL_PCH_S_GPP_I_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 13)

// Pin:                          GPP_I_14 ( Value: 0x140e )
// Family:                       ISH_I2C0
// Default function:             GP-In
// Native function 1:            ISH_I2C0_SDA
// Native function 2:            ISH_I3C0_SDA
// Native function 6:            IOM_GPPI_14
#define GPIOV2_MTL_PCH_S_GPP_I_14               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 14)

// Pin:                          GPP_I_15 ( Value: 0x140f )
// Family:                       ISH_I2C0
// Default function:             GP-In
// Native function 1:            ISH_I2C0_SCL
// Native function 2:            ISH_I3C0_SCL
#define GPIOV2_MTL_PCH_S_GPP_I_15               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 15)

// Pin:                          GPP_I_16 ( Value: 0x1410 )
// Family:                       ISH_I2C0
// Default function:             GP-Out
// Native function 1:            ISH_GP_11
#define GPIOV2_MTL_PCH_S_GPP_I_16               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 16)



#endif // _GPIOV2_PINS_MTL_PCH_S_H_
