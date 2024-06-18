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
#define MTL_PCH_S_COM_IDX_VGPIO                 0
#define MTL_PCH_S_GRP_IDX_VGPIO                 3

//
// Community 1
//
#define MTL_PCH_S_COM_IDX_GPP_A                 1
#define MTL_PCH_S_GRP_IDX_GPP_A                 0
#define MTL_PCH_S_COM_IDX_DIR_ESPI              1
#define MTL_PCH_S_GRP_IDX_DIR_ESPI              1
#define MTL_PCH_S_COM_IDX_GPP_B                 1
#define MTL_PCH_S_GRP_IDX_GPP_B                 2

//
// Community 2
//
#define MTL_PCH_S_COM_IDX_DSW                   2
#define MTL_PCH_S_GRP_IDX_DSW                   0

//
// Community 3
//
#define MTL_PCH_S_COM_IDX_SPI0                  3
#define MTL_PCH_S_GRP_IDX_SPI0                  0
#define MTL_PCH_S_COM_IDX_GPP_C                 3
#define MTL_PCH_S_GRP_IDX_GPP_C                 1
#define MTL_PCH_S_COM_IDX_GPP_H                 3
#define MTL_PCH_S_GRP_IDX_GPP_H                 2
#define MTL_PCH_S_COM_IDX_VGPIO_3               3
#define MTL_PCH_S_GRP_IDX_VGPIO_3               3
#define MTL_PCH_S_COM_IDX_VGPIO_0               3
#define MTL_PCH_S_GRP_IDX_VGPIO_0               4
#define MTL_PCH_S_COM_IDX_VGPIO_4               3
#define MTL_PCH_S_GRP_IDX_VGPIO_4               5

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
#define MTL_PCH_S_COM_IDX_JTAG_CPU              5
#define MTL_PCH_S_GRP_IDX_JTAG_CPU              1

//
// GPIO Group defines
//
//
// Community 0
//
#define GPIOV2_MTL_PCH_S_GROUP_D      GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_R      GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_J      GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_VGPIO  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 0xFF)

//
// Community 1
//
#define GPIOV2_MTL_PCH_S_GROUP_A        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A,    MTL_PCH_S_GRP_IDX_GPP_A,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_DIR_ESPI GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_B        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_B,    MTL_PCH_S_GRP_IDX_GPP_B,    0xFF)

//
// Community 2
//
#define GPIOV2_MTL_PCH_S_GROUP_DSW      GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW,    MTL_PCH_S_GRP_IDX_DSW,    0xFF)

//
// Community 3
//
#define GPIOV2_MTL_PCH_S_GROUP_SPI0     GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0,     MTL_PCH_S_GRP_IDX_SPI0,     0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_C        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_C,    MTL_PCH_S_GRP_IDX_GPP_C,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_H        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_H,    MTL_PCH_S_GRP_IDX_GPP_H,    0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_VGPIO_3  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_3,  MTL_PCH_S_GRP_IDX_VGPIO_3,  0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_VGPIO_0  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0,  MTL_PCH_S_GRP_IDX_VGPIO_0,  0xFF)
#define GPIOV2_MTL_PCH_S_GROUP_VGPIO_4  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4,  MTL_PCH_S_GRP_IDX_VGPIO_4,  0xFF)

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
#define GPIOV2_MTL_PCH_S_GROUP_JTAG_CPU GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 0xFF)

//
// Group name: GPP_D
// Community index: 0
// Group index: 0
// Number of pads: 25
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

// Pin:                          GSPI3_THC1_CLK_LOOPBK ( Value: 0x18 )
// Family:                       THC1_GSPI3
// Default function:             RW - 4'b0000 [padmode_value[17][0] ? 4'b0001:4'b0010]
// Native function 1:            GSPI3_CLK_LOOPBK
// Native function 2:            THC1_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_GSPI3_THC1_CLK_LOOPBK  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_D, MTL_PCH_S_GRP_IDX_GPP_D, 24)


//
// Group name: GPP_R
// Community index: 0
// Group index: 1
// Number of pads: 14
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

// Pin:                          GSPI2_CLK_LOOPBK ( Value: 0x8d )
// Family:                       GSPI2
// Default function:             Function 3
// Native function 3:            GSPI2_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_GSPI2_CLK_LOOPBK       GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_R, MTL_PCH_S_GRP_IDX_GPP_R, 13)


//
// Group name: GPP_J
// Community index: 0
// Group index: 2
// Number of pads: 18
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

// Pin:                          RESET_SYNCB ( Value: 0x111 )
// Family:                       MiscJ
// Default function:             Function 1
// Native function 1:            RESET_SYNCB
#define GPIOV2_MTL_PCH_S_RESET_SYNCB            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_J, MTL_PCH_S_GRP_IDX_GPP_J, 17)


//
// Group name: vGPIO
// Community index: 0
// Group index: 3
// Number of pads: 31
//


// Pin:                          vGPIO_0 ( Value: 0x180 )
// Family:                       v_CNV
// Default function:             GP-Out
#define GPIOV2_MTL_PCH_S_VGPIO_0                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 0)

// Pin:                          vGPIO_4 ( Value: 0x181 )
// Family:                       v_CNV
// Default function:             GP-In
#define GPIOV2_MTL_PCH_S_VGPIO_4                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 1)

// Pin:                          vGPIO_5 ( Value: 0x182 )
// Family:                       v_CNV
// Default function:             GP-Out
#define GPIOV2_MTL_PCH_S_VGPIO_5                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 2)

// Pin:                          vGPIO_6 ( Value: 0x183 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            UART0_RXD
// Native function 2:            ISH_UART0_RXD
#define GPIOV2_MTL_PCH_S_VGPIO_6                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 3)

// Pin:                          vGPIO_7 ( Value: 0x184 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            UART0_TXD
// Native function 2:            ISH_UART0_TXD
#define GPIOV2_MTL_PCH_S_VGPIO_7                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 4)

// Pin:                          vGPIO_8 ( Value: 0x185 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            UART0_RTSB
// Native function 2:            ISH_UART0_RTSB
#define GPIOV2_MTL_PCH_S_VGPIO_8                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 5)

// Pin:                          vGPIO_9 ( Value: 0x186 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            UART0_CTSB
// Native function 2:            ISH_UART0_CTSB
#define GPIOV2_MTL_PCH_S_VGPIO_9                GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 6)

// Pin:                          vGPIO_10 ( Value: 0x187 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            ISH_UART0_RXD
// Native function 2:            UART2_RXD
#define GPIOV2_MTL_PCH_S_VGPIO_10               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 7)

// Pin:                          vGPIO_11 ( Value: 0x188 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            ISH_UART0_TXD
// Native function 2:            UART2_TXD
#define GPIOV2_MTL_PCH_S_VGPIO_11               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 8)

// Pin:                          vGPIO_12 ( Value: 0x189 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            ISH_UART0_RTSB
// Native function 2:            UART2_RTS_B
#define GPIOV2_MTL_PCH_S_VGPIO_12               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 9)

// Pin:                          vGPIO_13 ( Value: 0x18a )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            ISH_UART0_CTSB
// Native function 2:            UART2_CTS_B
#define GPIOV2_MTL_PCH_S_VGPIO_13               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 10)

// Pin:                          vGPIO_18 ( Value: 0x18b )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_UART_RXD
#define GPIOV2_MTL_PCH_S_VGPIO_18               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 11)

// Pin:                          vGPIO_19 ( Value: 0x18c )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_UART_TXD
#define GPIOV2_MTL_PCH_S_VGPIO_19               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 12)

// Pin:                          vGPIO_20 ( Value: 0x18d )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_UART_RTS_B
#define GPIOV2_MTL_PCH_S_VGPIO_20               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 13)

// Pin:                          vGPIO_21 ( Value: 0x18e )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_UART_CTS_B
#define GPIOV2_MTL_PCH_S_VGPIO_21               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 14)

// Pin:                          vGPIO_22 ( Value: 0x18f )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_MFUART1_RXD
#define GPIOV2_MTL_PCH_S_VGPIO_22               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 15)

// Pin:                          vGPIO_23 ( Value: 0x190 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_MFUART1_TXD
#define GPIOV2_MTL_PCH_S_VGPIO_23               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 16)

// Pin:                          vGPIO_24 ( Value: 0x191 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_MFUART1_RTS_B
#define GPIOV2_MTL_PCH_S_VGPIO_24               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 17)

// Pin:                          vGPIO_25 ( Value: 0x192 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_MFUART1_CTS_B
#define GPIOV2_MTL_PCH_S_VGPIO_25               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 18)

// Pin:                          vGPIO_30 ( Value: 0x193 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            I2S0_SCLK
// Native function 2:            I2S1_SCLK
// Native function 1:            I2S2_SCLK
#define GPIOV2_MTL_PCH_S_VGPIO_30               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 19)

// Pin:                          vGPIO_31 ( Value: 0x194 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            I2S0_SFRM
// Native function 2:            I2S1_SFRM
// Native function 1:            I2S2_SFRM
#define GPIOV2_MTL_PCH_S_VGPIO_31               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 20)

// Pin:                          vGPIO_32 ( Value: 0x195 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            I2S0_RXD
// Native function 2:            I2S1_RXD
// Native function 1:            I2S2_RXD
#define GPIOV2_MTL_PCH_S_VGPIO_32               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 21)

// Pin:                          vGPIO_33 ( Value: 0x196 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 2:            I2S0_TXD
// Native function 2:            I2S1_TXD
// Native function 1:            I2S2_TXD
#define GPIOV2_MTL_PCH_S_VGPIO_33               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 22)

// Pin:                          vGPIO_34 ( Value: 0x197 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_I2S_BCLK
#define GPIOV2_MTL_PCH_S_VGPIO_34               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 23)

// Pin:                          vGPIO_35 ( Value: 0x198 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_I2S_WS_SYNC
#define GPIOV2_MTL_PCH_S_VGPIO_35               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 24)

// Pin:                          vGPIO_36 ( Value: 0x199 )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_I2S_SDI
#define GPIOV2_MTL_PCH_S_VGPIO_36               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 25)

// Pin:                          vGPIO_37 ( Value: 0x19a )
// Family:                       v_CNV
// Default function:             Function 1
// Native function 1:            CNV_BT_I2S_SDO
#define GPIOV2_MTL_PCH_S_VGPIO_37               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 26)

// Pin:                          vGPIO_THC0 ( Value: 0x19b )
// Family:                       v_CNV
// Default function:             GP-In
#define GPIOV2_MTL_PCH_S_VGPIO_THC0             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 27)

// Pin:                          vGPIO_THC1 ( Value: 0x19c )
// Family:                       v_CNV
// Default function:             GP-In
#define GPIOV2_MTL_PCH_S_VGPIO_THC1             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 28)

// Pin:                          vGPIO_THC2 ( Value: 0x19d )
// Family:                       v_CNV
// Default function:             GP-In
#define GPIOV2_MTL_PCH_S_VGPIO_THC2             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 29)

// Pin:                          vGPIO_THC3 ( Value: 0x19e )
// Family:                       v_CNV
// Default function:             GP-In
#define GPIOV2_MTL_PCH_S_VGPIO_THC3             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO, MTL_PCH_S_GRP_IDX_VGPIO, 30)


//
// Group name: GPP_A
// Community index: 1
// Group index: 0
// Number of pads: 15
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

// Pin:                          ESPI_CLK_LOOPBK ( Value: 0x40e )
// Family:                       eSPI
// Default function:             Function 1
// Native function 1:            ESPI_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_ESPI_CLK_LOOPBK        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_A, MTL_PCH_S_GRP_IDX_GPP_A, 14)


//
// Group name: DIR_ESPI
// Community index: 1
// Group index: 1
// Number of pads: 12
//


// Pin:                          PWRBTNB_OUT ( Value: 0x480 )
// Family:                       DMI
// Default function:             Function 1
// Native function 1:            PWRBTNB_OUT
#define GPIOV2_MTL_PCH_S_PWRBTNB_OUT            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 0)

// Pin:                          DMI_PERSTB ( Value: 0x481 )
// Family:                       DMI
// Default function:             Function 1
// Native function 1:            DMI_PERSTB
#define GPIOV2_MTL_PCH_S_DMI_PERSTB             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 1)

// Pin:                          DMI_CLKREQB ( Value: 0x482 )
// Family:                       DMI
// Default function:             Function 1
// Native function 1:            DMI_CLKREQB
#define GPIOV2_MTL_PCH_S_DMI_CLKREQB            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 2)

// Pin:                          DIR_ESPI_IO_0 ( Value: 0x483 )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_IO_0
#define GPIOV2_MTL_PCH_S_DIR_ESPI_IO_0          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 3)

// Pin:                          DIR_ESPI_IO_1 ( Value: 0x484 )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_IO_1
#define GPIOV2_MTL_PCH_S_DIR_ESPI_IO_1          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 4)

// Pin:                          DIR_ESPI_IO_2 ( Value: 0x485 )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_IO_2
#define GPIOV2_MTL_PCH_S_DIR_ESPI_IO_2          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 5)

// Pin:                          DIR_ESPI_IO_3 ( Value: 0x486 )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_IO_3
#define GPIOV2_MTL_PCH_S_DIR_ESPI_IO_3          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 6)

// Pin:                          DIR_ESPI_CSB ( Value: 0x487 )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_CSB
#define GPIOV2_MTL_PCH_S_DIR_ESPI_CSB           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 7)

// Pin:                          DIR_ESPI_RESETB ( Value: 0x488 )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_RESETB
#define GPIOV2_MTL_PCH_S_DIR_ESPI_RESETB        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 8)

// Pin:                          DIR_ESPI_CLK ( Value: 0x489 )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_CLK
#define GPIOV2_MTL_PCH_S_DIR_ESPI_CLK           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 9)

// Pin:                          DIR_ESPI_RCLK ( Value: 0x48a )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_RCLK
#define GPIOV2_MTL_PCH_S_DIR_ESPI_RCLK          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 10)

// Pin:                          DIR_ESPI_ALERTB ( Value: 0x48b )
// Family:                       DIR_ESPI0
// Default function:             Function 1
// Native function 1:            DIR_ESPI_ALERTB
#define GPIOV2_MTL_PCH_S_DIR_ESPI_ALERTB        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DIR_ESPI, MTL_PCH_S_GRP_IDX_DIR_ESPI, 11)


//
// Group name: GPP_B
// Community index: 1
// Group index: 2
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
// Number of pads: 17
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

// Pin:                          SLP_LANB ( Value: 0x80d )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SLP_LANB
#define GPIOV2_MTL_PCH_S_SLP_LANB               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 13)

// Pin:                          SLP_SUSB ( Value: 0x80e )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            SLP_SUSB
#define GPIOV2_MTL_PCH_S_SLP_SUSB               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 14)

// Pin:                          WAKEB ( Value: 0x80f )
// Family:                       DSW
// Default function:             Function 1
// Native function 1:            WAKEB
#define GPIOV2_MTL_PCH_S_WAKEB                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 15)

// Pin:                          DSW_SPARE ( Value: 0x810 )
// Family:                       DSW
// Default function:             GP-In
// Native function 1:            DSW_SPARE
#define GPIOV2_MTL_PCH_S_DSW_SPARE              GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_DSW, MTL_PCH_S_GRP_IDX_DSW, 16)


//
// Group name: SPI0
// Community index: 3
// Group index: 0
// Number of pads: 9
//


// Pin:                          SPI0_IO_2 ( Value: 0xc00 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_IO_2
#define GPIOV2_MTL_PCH_S_SPI0_IO_2              GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 0)

// Pin:                          SPI0_IO_3 ( Value: 0xc01 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_IO_3
#define GPIOV2_MTL_PCH_S_SPI0_IO_3              GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 1)

// Pin:                          SPI0_MOSI_IO_0 ( Value: 0xc02 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_MOSI_IO_0
#define GPIOV2_MTL_PCH_S_SPI0_MOSI_IO_0         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 2)

// Pin:                          SPI0_MISO_IO_1 ( Value: 0xc03 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_MISO_IO_1
#define GPIOV2_MTL_PCH_S_SPI0_MISO_IO_1         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 3)

// Pin:                          SPI0_TPM_CSB ( Value: 0xc04 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_TPM_CSB
#define GPIOV2_MTL_PCH_S_SPI0_TPM_CSB           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 4)

// Pin:                          SPI0_FLASH_0_CSB ( Value: 0xc05 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_FLASH_0_CSB
#define GPIOV2_MTL_PCH_S_SPI0_FLASH_0_CSB       GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 5)

// Pin:                          SPI0_FLASH_1_CSB ( Value: 0xc06 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_FLASH_1_CSB
#define GPIOV2_MTL_PCH_S_SPI0_FLASH_1_CSB       GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 6)

// Pin:                          SPI0_CLK ( Value: 0xc07 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_CLK
#define GPIOV2_MTL_PCH_S_SPI0_CLK               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 7)

// Pin:                          SPI0_CLK_LOOPBK ( Value: 0xc08 )
// Family:                       SPI
// Default function:             Function 1
// Native function 1:            SPI0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_SPI0_CLK_LOOPBK        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_SPI0, MTL_PCH_S_GRP_IDX_SPI0, 8)


//
// Group name: GPP_C
// Community index: 3
// Group index: 1
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
// Group index: 2
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
// Group name: vGPIO_3
// Community index: 3
// Group index: 3
// Number of pads: 4
//


// Pin:                          vGPIO_PCIE_80 ( Value: 0xd80 )
// Family:                       v_PCIe
// Default function:             Function 1
// Native function 1:            CPU_PCIE_LNK_DN_0
#define GPIOV2_MTL_PCH_S_VGPIO_PCIE_80          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_3, MTL_PCH_S_GRP_IDX_VGPIO_3, 0)

// Pin:                          vGPIO_PCIE_81 ( Value: 0xd81 )
// Family:                       v_PCIe
// Default function:             Function 1
// Native function 1:            CPU_PCIE_LNK_DN_1
#define GPIOV2_MTL_PCH_S_VGPIO_PCIE_81          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_3, MTL_PCH_S_GRP_IDX_VGPIO_3, 1)

// Pin:                          vGPIO_PCIE_82 ( Value: 0xd82 )
// Family:                       v_PCIe
// Default function:             Function 1
// Native function 1:            CPU_PCIE_LNK_DN_2
#define GPIOV2_MTL_PCH_S_VGPIO_PCIE_82          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_3, MTL_PCH_S_GRP_IDX_VGPIO_3, 2)

// Pin:                          vGPIO_PCIE_83 ( Value: 0xd83 )
// Family:                       v_PCIe
// Default function:             Function 1
// Native function 1:            CPU_PCIE_LNK_DN_3
#define GPIOV2_MTL_PCH_S_VGPIO_PCIE_83          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_3, MTL_PCH_S_GRP_IDX_VGPIO_3, 3)


//
// Group name: vGPIO_0
// Community index: 3
// Group index: 4
// Number of pads: 8
//


// Pin:                          vGPIO_USB_0 ( Value: 0xe00 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            ESPI_USB_OCB_0
#define GPIOV2_MTL_PCH_S_VGPIO_USB_0            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 0)

// Pin:                          vGPIO_USB_1 ( Value: 0xe01 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            ESPI_USB_OCB_1
#define GPIOV2_MTL_PCH_S_VGPIO_USB_1            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 1)

// Pin:                          vGPIO_USB_2 ( Value: 0xe02 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            ESPI_USB_OCB_2
#define GPIOV2_MTL_PCH_S_VGPIO_USB_2            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 2)

// Pin:                          vGPIO_USB_3 ( Value: 0xe03 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            ESPI_USB_OCB_3
#define GPIOV2_MTL_PCH_S_VGPIO_USB_3            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 3)

// Pin:                          vGPIO_USB_8 ( Value: 0xe04 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            USB_CPU_OCB_0
#define GPIOV2_MTL_PCH_S_VGPIO_USB_8            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 4)

// Pin:                          vGPIO_USB_9 ( Value: 0xe05 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            USB_CPU_OCB_1
#define GPIOV2_MTL_PCH_S_VGPIO_USB_9            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 5)

// Pin:                          vGPIO_USB_10 ( Value: 0xe06 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            USB_CPU_OCB_2
#define GPIOV2_MTL_PCH_S_VGPIO_USB_10           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 6)

// Pin:                          vGPIO_USB_11 ( Value: 0xe07 )
// Family:                       v_USB
// Default function:             Function 1
// Native function 1:            USB_CPU_OCB_3
#define GPIOV2_MTL_PCH_S_VGPIO_USB_11           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_0, MTL_PCH_S_GRP_IDX_VGPIO_0, 7)


//
// Group name: vGPIO_4
// Community index: 3
// Group index: 5
// Number of pads: 31
//


// Pin:                          vGPIO_ISCLK_0 ( Value: 0xe80 )
// Family:                       v_ISCLK
// Default function:             Function 1
// Native function 1:            ESPI_ISCLK_XTAL_CLKREQ
#define GPIOV2_MTL_PCH_S_VGPIO_ISCLK_0          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 0)

// Pin:                          vGPIO_ISCLK_1 ( Value: 0xe81 )
// Family:                       v_ISCLK
// Default function:             Function 1
// Native function 1:            ISCLK_ESPI_XTAL_CLKACK
#define GPIOV2_MTL_PCH_S_VGPIO_ISCLK_1          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 1)

// Pin:                          vGPIO_SLPC_0 ( Value: 0xe82 )
// Family:                       v_ISCLK
// Default function:             Function 1
// Native function 1:            ME_SLPC_FTPM_ENABLE
#define GPIOV2_MTL_PCH_S_VGPIO_SLPC_0           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 2)

// Pin:                          vGPIO_SLPC_1 ( Value: 0xe83 )
// Family:                       v_ISCLK
// Default function:             Function 1
// Native function 1:            GP_SLPC_DTFUS_CORE_SPITPM_DIS
#define GPIOV2_MTL_PCH_S_VGPIO_SLPC_1           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 3)

// Pin:                          vGPIO_SLPC_2 ( Value: 0xe84 )
// Family:                       v_ISCLK
// Default function:             Function 1
// Native function 1:            GP_SLPC_SPI_STRAP_TOS
#define GPIOV2_MTL_PCH_S_VGPIO_SLPC_2           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 4)

// Pin:                          vGPIO_SLPC_3 ( Value: 0xe85 )
// Family:                       v_ISCLK
// Default function:             RW - 4'b0001 [gpio_fuse_dtfus_core_spitpm_dis?4'b0010: 4'b0001]
// Native function 1:            GP_SLPC_DTFUS_CORE_SPITPM_DIS_L0
// Native function 2:            GP_SLPC_DTFUS_CORE_SPITPM_DIS_L1
#define GPIOV2_MTL_PCH_S_VGPIO_SLPC_3           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 5)

// Pin:                          vGPIO_SLPC_4 ( Value: 0xe86 )
// Family:                       v_ISCLK
// Default function:             RW - 4'b0001 [gpio_sstrap_spi_strap_tos?4'b0010: 4'b0001]
// Native function 1:            GP_SLPC_SPI_STRAP_TOS_L0
// Native function 2:            GP_SLPC_SPI_STRAP_TOS_L1
#define GPIOV2_MTL_PCH_S_VGPIO_SLPC_4           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 6)

// Pin:                          vGPIO_SLPC_5 ( Value: 0xe87 )
// Family:                       v_ISCLK
// Default function:             Function 1
// Native function 1:            LPC_PRR_TS_OVR
#define GPIOV2_MTL_PCH_S_VGPIO_SLPC_5           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 7)

// Pin:                          vGPIO_SLPC_6 ( Value: 0xe88 )
// Family:                       v_ISCLK
// Default function:             Function 1
// Native function 1:            ITSS_KU1_SHTDWN
#define GPIOV2_MTL_PCH_S_VGPIO_SLPC_6           GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 8)

// Pin:                          vGPIO_SPARE_0 ( Value: 0xe89 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_0
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_0          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 9)

// Pin:                          vGPIO_SPARE_1 ( Value: 0xe8a )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_1
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_1          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 10)

// Pin:                          vGPIO_SPARE_2 ( Value: 0xe8b )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_2
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_2          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 11)

// Pin:                          vGPIO_SPARE_3 ( Value: 0xe8c )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_3
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_3          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 12)

// Pin:                          vGPIO_SPARE_4 ( Value: 0xe8d )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_4
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_4          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 13)

// Pin:                          vGPIO_SPARE_5 ( Value: 0xe8e )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_5
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_5          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 14)

// Pin:                          vGPIO_SPARE_6 ( Value: 0xe8f )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_6
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_6          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 15)

// Pin:                          vGPIO_SPARE_7 ( Value: 0xe90 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_7
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_7          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 16)

// Pin:                          vGPIO_SPARE_8 ( Value: 0xe91 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_8
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_8          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 17)

// Pin:                          vGPIO_SPARE_9 ( Value: 0xe92 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_9
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_9          GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 18)

// Pin:                          vGPIO_SPARE_10 ( Value: 0xe93 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_10
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_10         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 19)

// Pin:                          vGPIO_SPARE_11 ( Value: 0xe94 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_11
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_11         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 20)

// Pin:                          vGPIO_SPARE_12 ( Value: 0xe95 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_12
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_12         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 21)

// Pin:                          vGPIO_SPARE_13 ( Value: 0xe96 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_13
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_13         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 22)

// Pin:                          vGPIO_SPARE_14 ( Value: 0xe97 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_14
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_14         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 23)

// Pin:                          vGPIO_SPARE_15 ( Value: 0xe98 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_15
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_15         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 24)

// Pin:                          vGPIO_SPARE_16 ( Value: 0xe99 )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_16
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_16         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 25)

// Pin:                          vGPIO_SPARE_17 ( Value: 0xe9a )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_17
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_17         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 26)

// Pin:                          vGPIO_SPARE_18 ( Value: 0xe9b )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_18
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_18         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 27)

// Pin:                          vGPIO_SPARE_19 ( Value: 0xe9c )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_19
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_19         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 28)

// Pin:                          vGPIO_SPARE_20 ( Value: 0xe9d )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_20
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_20         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 29)

// Pin:                          vGPIO_SPARE_21 ( Value: 0xe9e )
// Family:                       v_ISCLK
// Default function:             GP-In
// Native function 1:            VWM_SPARE_21
#define GPIOV2_MTL_PCH_S_VGPIO_SPARE_21         GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_VGPIO_4, MTL_PCH_S_GRP_IDX_VGPIO_4, 30)


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
// Number of pads: 23
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

// Pin:                          SPI1_THC0_CLK_LOOPBK ( Value: 0x1096 )
// Family:                       THC0_SPI1
// Default function:             RW - 4'b0000 [padmode_value[22][0] ? 4'b0001:4'b0010]
// Native function 1:            SPI1_CLK_LOOPBK
// Native function 2:            THC0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_SPI1_THC0_CLK_LOOPBK   GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_E, MTL_PCH_S_GRP_IDX_GPP_E, 22)


//
// Group name: GPP_K
// Community index: 4
// Group index: 2
// Number of pads: 14
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

// Pin:                          MISC_SPARE ( Value: 0x110b )
// Family:                       SYS
// Default function:             Function 1
// Native function 1:            MISC_SPARE
#define GPIOV2_MTL_PCH_S_MISC_SPARE             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 11)

// Pin:                          SYS_RESETB ( Value: 0x110c )
// Family:                       SYS
// Default function:             Function 1
// Native function 1:            SYS_RESETB
#define GPIOV2_MTL_PCH_S_SYS_RESETB             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 12)

// Pin:                          MLK_RSTB ( Value: 0x110d )
// Family:                       SYS
// Default function:             Function 1
// Native function 1:            MLK_RSTB
#define GPIOV2_MTL_PCH_S_MLK_RSTB               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_K, MTL_PCH_S_GRP_IDX_GPP_K, 13)


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
// Number of pads: 21
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

// Pin:                          GSPI0_CLK_LOOPBK ( Value: 0x1411 )
// Family:                       GSPI0
// Default function:             Function 1
// Native function 1:            GSPI0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_GSPI0_CLK_LOOPBK       GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 17)

// Pin:                          GSPI1_CLK_LOOPBK ( Value: 0x1412 )
// Family:                       GSPI1
// Default function:             Function 1
// Native function 1:            GSPI1_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_GSPI1_CLK_LOOPBK       GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 18)

// Pin:                          ISH_I3C0_CLK_LOOPBK ( Value: 0x1413 )
// Family:                       ISH_I2C0
// Default function:             Function 1
// Native function 1:            ISH_I3C0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_ISH_I3C0_CLK_LOOPBK    GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 19)

// Pin:                          I3C0_CLK_LOOPBK ( Value: 0x1414 )
// Family:                       I2C0
// Default function:             Function 1
// Native function 1:            I3C0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_I3C0_CLK_LOOPBK        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_GPP_I, MTL_PCH_S_GRP_IDX_GPP_I, 20)


//
// Group name: JTAG_CPU
// Community index: 5
// Group index: 1
// Number of pads: 16
//


// Pin:                          JTAG_TDO ( Value: 0x1480 )
// Family:                       JTAG0
// Default function:             Function 1
// Native function 1:            JTAG_TDO
#define GPIOV2_MTL_PCH_S_JTAG_TDO               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 0)

// Pin:                          JTAGX ( Value: 0x1481 )
// Family:                       JTAG0
// Default function:             Function 1
// Native function 1:            JTAGX
#define GPIOV2_MTL_PCH_S_JTAGX                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 1)

// Pin:                          PRDYB ( Value: 0x1482 )
// Family:                       JTAG2
// Default function:             Function 1
// Native function 1:            PRDYB
#define GPIOV2_MTL_PCH_S_PRDYB                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 2)

// Pin:                          PREQB ( Value: 0x1483 )
// Family:                       JTAG2
// Default function:             Function 1
// Native function 1:            PREQB
#define GPIOV2_MTL_PCH_S_PREQB                  GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 3)

// Pin:                          JTAG_TDI ( Value: 0x1484 )
// Family:                       JTAG0
// Default function:             Function 1
// Native function 1:            JTAG_TDI
#define GPIOV2_MTL_PCH_S_JTAG_TDI               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 4)

// Pin:                          JTAG_TMS ( Value: 0x1485 )
// Family:                       JTAG0
// Default function:             Function 1
// Native function 1:            JTAG_TMS
#define GPIOV2_MTL_PCH_S_JTAG_TMS               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 5)

// Pin:                          JTAG_TCK ( Value: 0x1486 )
// Family:                       JTAG1
// Default function:             Function 1
// Native function 1:            JTAG_TCK
#define GPIOV2_MTL_PCH_S_JTAG_TCK               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 6)

// Pin:                          DBG_PMODE ( Value: 0x1487 )
// Family:                       JTAG3
// Default function:             Function 1
// Native function 1:            DBG_PMODE
#define GPIOV2_MTL_PCH_S_DBG_PMODE              GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 7)

// Pin:                          CPU_TRSTB ( Value: 0x1488 )
// Family:                       JTAG1
// Default function:             Function 1
// Native function 1:            CPU_TRSTB
#define GPIOV2_MTL_PCH_S_CPU_TRSTB              GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 8)

// Pin:                          CPUPWRGD ( Value: 0x1489 )
// Family:                       PM1
// Default function:             Function 1
// Native function 1:            CPUPWRGD
#define GPIOV2_MTL_PCH_S_CPUPWRGD               GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 9)

// Pin:                          PM_SPARE0 ( Value: 0x148a )
// Family:                       PM1
// Default function:             Function 1
// Native function 1:            PM_SPARE0
#define GPIOV2_MTL_PCH_S_PM_SPARE0              GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 10)

// Pin:                          PM_SPARE1 ( Value: 0x148b )
// Family:                       PM1
// Default function:             Function 1
// Native function 1:            PM_SPARE1
#define GPIOV2_MTL_PCH_S_PM_SPARE1              GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 11)

// Pin:                          CRASHLOG_TRIG_N ( Value: 0x148c )
// Family:                       PM2
// Default function:             Function 1
// Native function 1:            CRASHLOG_TRIG_N
#define GPIOV2_MTL_PCH_S_CRASHLOG_TRIG_N        GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 12)

// Pin:                          TRIGGER_IN ( Value: 0x148d )
// Family:                       TRIG
// Default function:             Function 1
// Native function 1:            TRIGGER_IN
#define GPIOV2_MTL_PCH_S_TRIGGER_IN             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 13)

// Pin:                          TRIGGER_OUT ( Value: 0x148e )
// Family:                       TRIG
// Default function:             Function 1
// Native function 1:            TRIGGER_OUT
#define GPIOV2_MTL_PCH_S_TRIGGER_OUT            GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 14)

// Pin:                          FBRK_OUT_N ( Value: 0x148f )
// Family:                       PM
// Default function:             Function 1
// Native function 1:            FBRK_OUT_N
#define GPIOV2_MTL_PCH_S_FBRK_OUT_N             GPIOV2_PAD_ID (0, GPIOV2_MTL_PCH_S_CHIPSET_ID, 0, MTL_PCH_S_COM_IDX_JTAG_CPU, MTL_PCH_S_GRP_IDX_JTAG_CPU, 15)



//
// Pin muxing definitions for Group: GPP_D.
//

// Group: GPP_D, Pad: GPP_D_0, Function: 1 - SRCCLKREQB_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_0__SRCCLKREQB_0               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_0, 1)
// Group: GPP_D, Pad: GPP_D_0, Function: 6 - IOM_GPPD_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_0__IOM_GPPD_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_0, 6)
// Group: GPP_D, Pad: GPP_D_1, Function: 1 - SRCCLKREQB_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_1__SRCCLKREQB_1               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_1, 1)
// Group: GPP_D, Pad: GPP_D_1, Function: 6 - IOM_GPPD_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_1__IOM_GPPD_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_1, 6)
// Group: GPP_D, Pad: GPP_D_2, Function: 1 - SRCCLKREQB_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_2__SRCCLKREQB_2               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_2, 1)
// Group: GPP_D, Pad: GPP_D_2, Function: 6 - IOM_GPPD_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_2__IOM_GPPD_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_2, 6)
// Group: GPP_D, Pad: GPP_D_3, Function: 1 - SRCCLKREQB_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_3__SRCCLKREQB_3               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_3, 1)
// Group: GPP_D, Pad: GPP_D_3, Function: 6 - IOM_GPPD_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_3__IOM_GPPD_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_3, 6)
// Group: GPP_D, Pad: GPP_D_4, Function: 1 - SML1CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_4__SML1CLK                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_4, 1)
// Group: GPP_D, Pad: GPP_D_4, Function: 6 - IOM_GPPD_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_4__IOM_GPPD_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_4, 6)
// Group: GPP_D, Pad: GPP_D_5, Function: 1 - I2S2_SFRM
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_5__I2S2_SFRM                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_5, 1)
// Group: GPP_D, Pad: GPP_D_5, Function: 2 - CNV_RF_RESET_B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_5__CNV_RF_RESET_B             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_5, 2)
// Group: GPP_D, Pad: GPP_D_5, Function: 5 - DMIC_DATA_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_5__DMIC_DATA_0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_5, 5)
// Group: GPP_D, Pad: GPP_D_5, Function: 6 - IOM_GPPD_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_5__IOM_GPPD_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_5, 6)
// Group: GPP_D, Pad: GPP_D_6, Function: 1 - I2S2_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_6__I2S2_TXD                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_6, 1)
// Group: GPP_D, Pad: GPP_D_6, Function: 3 - CRF_CLKREQ
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_6__CRF_CLKREQ                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_6, 3)
// Group: GPP_D, Pad: GPP_D_6, Function: 5 - DMIC_CLK_B_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_6__DMIC_CLK_B_0               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_6, 5)
// Group: GPP_D, Pad: GPP_D_6, Function: 6 - IOM_GPPD_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_6__IOM_GPPD_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_6, 6)
// Group: GPP_D, Pad: GPP_D_7, Function: 1 - I2S2_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_7__I2S2_RXD                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_7, 1)
// Group: GPP_D, Pad: GPP_D_7, Function: 5 - DMIC_DATA_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_7__DMIC_DATA_1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_7, 5)
// Group: GPP_D, Pad: GPP_D_7, Function: 6 - IOM_GPPD_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_7__IOM_GPPD_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_7, 6)
// Group: GPP_D, Pad: GPP_D_8, Function: 1 - I2S2_SCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_8__I2S2_SCLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_8, 1)
// Group: GPP_D, Pad: GPP_D_8, Function: 5 - DMIC_CLK_A_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_8__DMIC_CLK_A_0               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_8, 5)
// Group: GPP_D, Pad: GPP_D_8, Function: 6 - IOM_GPPD_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_8__IOM_GPPD_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_8, 6)
// Group: GPP_D, Pad: GPP_D_9, Function: 1 - SML0CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_9__SML0CLK                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_9, 1)
// Group: GPP_D, Pad: GPP_D_9, Function: 6 - IOM_GPPD_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_9__IOM_GPPD_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_9, 6)
// Group: GPP_D, Pad: GPP_D_10, Function: 1 - SML0DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_10__SML0DATA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_10, 1)
// Group: GPP_D, Pad: GPP_D_10, Function: 6 - IOM_GPPD_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_10__IOM_GPPD_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_10, 6)
// Group: GPP_D, Pad: GPP_D_11, Function: 1 - SRCCLKREQB_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_11__SRCCLKREQB_4              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_11, 1)
// Group: GPP_D, Pad: GPP_D_11, Function: 6 - IOM_GPPD_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_11__IOM_GPPD_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_11, 6)
// Group: GPP_D, Pad: GPP_D_12, Function: 1 - SRCCLKREQB_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_12__SRCCLKREQB_5              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_12, 1)
// Group: GPP_D, Pad: GPP_D_12, Function: 6 - IOM_GPPD_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_12__IOM_GPPD_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_12, 6)
// Group: GPP_D, Pad: GPP_D_13, Function: 1 - SRCCLKREQB_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_13__SRCCLKREQB_6              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_13, 1)
// Group: GPP_D, Pad: GPP_D_13, Function: 6 - IOM_GPPD_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_13__IOM_GPPD_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_13, 6)
// Group: GPP_D, Pad: GPP_D_14, Function: 1 - SRCCLKREQB_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_14__SRCCLKREQB_7              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_14, 1)
// Group: GPP_D, Pad: GPP_D_14, Function: 6 - IOM_GPPD_14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_14__IOM_GPPD_14               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_14, 6)
// Group: GPP_D, Pad: GPP_D_15, Function: 1 - SML1DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_15__SML1DATA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_15, 1)
// Group: GPP_D, Pad: GPP_D_15, Function: 6 - IOM_GPPD_15
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_15__IOM_GPPD_15               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_15, 6)
// Group: GPP_D, Pad: GPP_D_16, Function: 1 - GSPI3_CS0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_16__GSPI3_CS0B                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_16, 1)
// Group: GPP_D, Pad: GPP_D_16, Function: 2 - THC1_SPI2_CSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_16__THC1_SPI2_CSB             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_16, 2)
// Group: GPP_D, Pad: GPP_D_16, Function: 6 - IOM_GPPD_16
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_16__IOM_GPPD_16               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_16, 6)
// Group: GPP_D, Pad: GPP_D_17, Function: 1 - GSPI3_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_17__GSPI3_CLK                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_17, 1)
// Group: GPP_D, Pad: GPP_D_17, Function: 2 - THC1_SPI2_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_17__THC1_SPI2_CLK             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_17, 2)
// Group: GPP_D, Pad: GPP_D_17, Function: 6 - IOM_GPPD_17
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_17__IOM_GPPD_17               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_17, 6)
// Group: GPP_D, Pad: GPP_D_18, Function: 1 - GSPI3_MISO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_18__GSPI3_MISO                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_18, 1)
// Group: GPP_D, Pad: GPP_D_18, Function: 2 - THC1_SPI2_IO_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_18__THC1_SPI2_IO_0            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_18, 2)
// Group: GPP_D, Pad: GPP_D_18, Function: 6 - IOM_GPPD_18
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_18__IOM_GPPD_18               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_18, 6)
// Group: GPP_D, Pad: GPP_D_19, Function: 1 - GSPI3_MOSI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_19__GSPI3_MOSI                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_19, 1)
// Group: GPP_D, Pad: GPP_D_19, Function: 2 - THC1_SPI2_IO_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_19__THC1_SPI2_IO_1            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_19, 2)
// Group: GPP_D, Pad: GPP_D_19, Function: 6 - IOM_GPPD_19
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_19__IOM_GPPD_19               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_19, 6)
// Group: GPP_D, Pad: GPP_D_20, Function: 1 - UART3_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_20__UART3_RXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_20, 1)
// Group: GPP_D, Pad: GPP_D_20, Function: 2 - THC1_SPI2_IO_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_20__THC1_SPI2_IO_2            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_20, 2)
// Group: GPP_D, Pad: GPP_D_20, Function: 6 - IOM_GPPD_20
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_20__IOM_GPPD_20               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_20, 6)
// Group: GPP_D, Pad: GPP_D_21, Function: 1 - UART3_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_21__UART3_TXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_21, 1)
// Group: GPP_D, Pad: GPP_D_21, Function: 2 - THC1_SPI2_IO_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_21__THC1_SPI2_IO_3            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_21, 2)
// Group: GPP_D, Pad: GPP_D_21, Function: 6 - IOM_GPPD_21
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_21__IOM_GPPD_21               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_21, 6)
// Group: GPP_D, Pad: GPP_D_22, Function: 1 - UART3_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_22__UART3_RTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_22, 1)
// Group: GPP_D, Pad: GPP_D_22, Function: 2 - THC1_SPI2_RSTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_22__THC1_SPI2_RSTB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_22, 2)
// Group: GPP_D, Pad: GPP_D_22, Function: 6 - IOM_GPPD_22
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_22__IOM_GPPD_22               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_22, 6)
// Group: GPP_D, Pad: GPP_D_23, Function: 1 - UART3_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_23__UART3_CTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_23, 1)
// Group: GPP_D, Pad: GPP_D_23, Function: 2 - THC1_SPI2_INTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_23__THC1_SPI2_INTB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_23, 2)
// Group: GPP_D, Pad: GPP_D_23, Function: 6 - IOM_GPPD_23
#define GPIOV2_MTL_PCH_S_MUXING__GPP_D_23__IOM_GPPD_23               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_D_23, 6)
// Group: GPP_D, Pad: GSPI3_THC1_CLK_LOOPBK, Function: 1 - GSPI3_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__GSPI3_THC1_CLK_LOOPBK__GSPI3_CLK_LOOPBK GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GSPI3_THC1_CLK_LOOPBK, 1)
// Group: GPP_D, Pad: GSPI3_THC1_CLK_LOOPBK, Function: 2 - THC1_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__GSPI3_THC1_CLK_LOOPBK__THC1_CLK_LOOPBK GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GSPI3_THC1_CLK_LOOPBK, 2)

//
// Pin muxing definitions for Group: GPP_R.
//

// Group: GPP_R, Pad: GPP_R_0, Function: 1 - HDA_BCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_0__HDA_BCLK                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_0, 1)
// Group: GPP_R, Pad: GPP_R_0, Function: 2 - I2S0_SCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_0__I2S0_SCLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_0, 2)
// Group: GPP_R, Pad: GPP_R_0, Function: 4 - HDACPU_BCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_0__HDACPU_BCLK                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_0, 4)
// Group: GPP_R, Pad: GPP_R_0, Function: 5 - DMIC_CLK_A_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_0__DMIC_CLK_A_1               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_0, 5)
// Group: GPP_R, Pad: GPP_R_0, Function: 6 - IOM_GPPR_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_0__IOM_GPPR_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_0, 6)
// Group: GPP_R, Pad: GPP_R_1, Function: 1 - HDA_SYNC
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_1__HDA_SYNC                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_1, 1)
// Group: GPP_R, Pad: GPP_R_1, Function: 2 - I2S0_SFRM
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_1__I2S0_SFRM                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_1, 2)
// Group: GPP_R, Pad: GPP_R_1, Function: 5 - DMIC_CLK_B_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_1__DMIC_CLK_B_1               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_1, 5)
// Group: GPP_R, Pad: GPP_R_1, Function: 6 - IOM_GPPR_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_1__IOM_GPPR_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_1, 6)
// Group: GPP_R, Pad: GPP_R_2, Function: 1 - HDA_SDO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_2__HDA_SDO                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_2, 1)
// Group: GPP_R, Pad: GPP_R_2, Function: 2 - I2S0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_2__I2S0_TXD                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_2, 2)
// Group: GPP_R, Pad: GPP_R_2, Function: 4 - HDACPU_SDO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_2__HDACPU_SDO                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_2, 4)
// Group: GPP_R, Pad: GPP_R_2, Function: 6 - IOM_GPPR_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_2__IOM_GPPR_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_2, 6)
// Group: GPP_R, Pad: GPP_R_3, Function: 1 - HDA_SDI_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_3__HDA_SDI_0                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_3, 1)
// Group: GPP_R, Pad: GPP_R_3, Function: 2 - I2S0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_3__I2S0_RXD                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_3, 2)
// Group: GPP_R, Pad: GPP_R_3, Function: 4 - HDACPU_SDI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_3__HDACPU_SDI                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_3, 4)
// Group: GPP_R, Pad: GPP_R_3, Function: 6 - IOM_GPPR_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_3__IOM_GPPR_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_3, 6)
// Group: GPP_R, Pad: GPP_R_4, Function: 1 - HDA_RSTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_4__HDA_RSTB                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_4, 1)
// Group: GPP_R, Pad: GPP_R_4, Function: 6 - IOM_GPPR_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_4__IOM_GPPR_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_4, 6)
// Group: GPP_R, Pad: GPP_R_5, Function: 1 - HDA_SDI_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_5__HDA_SDI_1                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_5, 1)
// Group: GPP_R, Pad: GPP_R_5, Function: 2 - I2S1_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_5__I2S1_RXD                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_5, 2)
// Group: GPP_R, Pad: GPP_R_5, Function: 6 - IOM_GPPR_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_5__IOM_GPPR_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_5, 6)
// Group: GPP_R, Pad: GPP_R_6, Function: 2 - I2S1_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_6__I2S1_TXD                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_6, 2)
// Group: GPP_R, Pad: GPP_R_6, Function: 6 - IOM_GPPR_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_6__IOM_GPPR_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_6, 6)
// Group: GPP_R, Pad: GPP_R_7, Function: 2 - I2S1_SFRM
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_7__I2S1_SFRM                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_7, 2)
// Group: GPP_R, Pad: GPP_R_7, Function: 6 - IOM_GPPR_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_7__IOM_GPPR_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_7, 6)
// Group: GPP_R, Pad: GPP_R_8, Function: 2 - I2S1_SCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_8__I2S1_SCLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_8, 2)
// Group: GPP_R, Pad: GPP_R_8, Function: 6 - IOM_GPPR_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_8__IOM_GPPR_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_8, 6)
// Group: GPP_R, Pad: GPP_R_9, Function: 1 - ISH_SPI_CSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_9__ISH_SPI_CSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_9, 1)
// Group: GPP_R, Pad: GPP_R_9, Function: 3 - GSPI2_CS0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_9__GSPI2_CS0B                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_9, 3)
// Group: GPP_R, Pad: GPP_R_9, Function: 6 - IOM_GPPR_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_9__IOM_GPPR_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_9, 6)
// Group: GPP_R, Pad: GPP_R_10, Function: 1 - ISH_SPI_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_10__ISH_SPI_CLK               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_10, 1)
// Group: GPP_R, Pad: GPP_R_10, Function: 3 - GSPI2_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_10__GSPI2_CLK                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_10, 3)
// Group: GPP_R, Pad: GPP_R_10, Function: 6 - IOM_GPPR_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_10__IOM_GPPR_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_10, 6)
// Group: GPP_R, Pad: GPP_R_11, Function: 1 - ISH_SPI_MISO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_11__ISH_SPI_MISO              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_11, 1)
// Group: GPP_R, Pad: GPP_R_11, Function: 3 - GSPI2_MISO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_11__GSPI2_MISO                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_11, 3)
// Group: GPP_R, Pad: GPP_R_11, Function: 6 - IOM_GPPR_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_11__IOM_GPPR_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_11, 6)
// Group: GPP_R, Pad: GPP_R_12, Function: 1 - ISH_SPI_MOSI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_12__ISH_SPI_MOSI              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_12, 1)
// Group: GPP_R, Pad: GPP_R_12, Function: 3 - GSPI2_MOSI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_12__GSPI2_MOSI                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_12, 3)
// Group: GPP_R, Pad: GPP_R_12, Function: 6 - IOM_GPPR_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_R_12__IOM_GPPR_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_R_12, 6)
// Group: GPP_R, Pad: GSPI2_CLK_LOOPBK, Function: 3 - GSPI2_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__GSPI2_CLK_LOOPBK__GSPI2_CLK_LOOPBK  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GSPI2_CLK_LOOPBK, 3)

//
// Pin muxing definitions for Group: GPP_J.
//

// Group: GPP_J, Pad: GPP_J_0, Function: 1 - CNV_PA_BLANKING
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_0__CNV_PA_BLANKING            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_0, 1)
// Group: GPP_J, Pad: GPP_J_0, Function: 6 - IOM_GPPJ_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_0__IOM_GPPJ_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_0, 6)
// Group: GPP_J, Pad: GPP_J_1, Function: 1 - CNV_BRI_DT
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_1__CNV_BRI_DT                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_1, 1)
// Group: GPP_J, Pad: GPP_J_1, Function: 2 - UART0_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_1__UART0_RTSB                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_1, 2)
// Group: GPP_J, Pad: GPP_J_1, Function: 6 - IOM_GPPJ_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_1__IOM_GPPJ_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_1, 6)
// Group: GPP_J, Pad: GPP_J_2, Function: 1 - CNV_BRI_RSP
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_2__CNV_BRI_RSP                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_2, 1)
// Group: GPP_J, Pad: GPP_J_2, Function: 2 - UART0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_2__UART0_RXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_2, 2)
// Group: GPP_J, Pad: GPP_J_2, Function: 6 - IOM_GPPJ_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_2__IOM_GPPJ_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_2, 6)
// Group: GPP_J, Pad: GPP_J_3, Function: 1 - CNV_RGI_DT
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_3__CNV_RGI_DT                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_3, 1)
// Group: GPP_J, Pad: GPP_J_3, Function: 2 - UART0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_3__UART0_TXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_3, 2)
// Group: GPP_J, Pad: GPP_J_3, Function: 6 - IOM_GPPJ_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_3__IOM_GPPJ_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_3, 6)
// Group: GPP_J, Pad: GPP_J_4, Function: 1 - CNV_RGI_RSP
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_4__CNV_RGI_RSP                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_4, 1)
// Group: GPP_J, Pad: GPP_J_4, Function: 2 - UART0_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_4__UART0_CTSB                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_4, 2)
// Group: GPP_J, Pad: GPP_J_4, Function: 6 - IOM_GPPJ_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_4__IOM_GPPJ_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_4, 6)
// Group: GPP_J, Pad: GPP_J_5, Function: 1 - CNV_MFUART2_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_5__CNV_MFUART2_RXD            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_5, 1)
// Group: GPP_J, Pad: GPP_J_5, Function: 6 - IOM_GPPJ_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_5__IOM_GPPJ_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_5, 6)
// Group: GPP_J, Pad: GPP_J_6, Function: 1 - CNV_MFUART2_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_6__CNV_MFUART2_TXD            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_6, 1)
// Group: GPP_J, Pad: GPP_J_6, Function: 6 - IOM_GPPJ_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_6__IOM_GPPJ_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_6, 6)
// Group: GPP_J, Pad: GPP_J_7, Function: 1 - BOOTHALT_B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_7__BOOTHALT_B                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_7, 1)
// Group: GPP_J, Pad: GPP_J_7, Function: 6 - IOM_GPPJ_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_7__IOM_GPPJ_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_7, 6)
// Group: GPP_J, Pad: GPP_J_8, Function: 1 - RTCCLKOUT
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_8__RTCCLKOUT                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_8, 1)
// Group: GPP_J, Pad: GPP_J_8, Function: 6 - IOM_GPPJ_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_8__IOM_GPPJ_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_8, 6)
// Group: GPP_J, Pad: GPP_J_9, Function: 1 - BPKI3C_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_9__BPKI3C_SDA                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_9, 1)
// Group: GPP_J, Pad: GPP_J_9, Function: 2 - BSSB_LS_RX
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_9__BSSB_LS_RX                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_9, 2)
// Group: GPP_J, Pad: GPP_J_9, Function: 6 - IOM_GPPJ_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_9__IOM_GPPJ_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_9, 6)
// Group: GPP_J, Pad: GPP_J_10, Function: 1 - BPKI3C_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_10__BPKI3C_SCL                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_10, 1)
// Group: GPP_J, Pad: GPP_J_10, Function: 2 - BSSB_LS_TX
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_10__BSSB_LS_TX                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_10, 2)
// Group: GPP_J, Pad: GPP_J_10, Function: 6 - IOM_GPPJ_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_10__IOM_GPPJ_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_10, 6)
// Group: GPP_J, Pad: GPP_J_11, Function: 1 - DAM
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_11__DAM                       GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_11, 1)
// Group: GPP_J, Pad: GPP_J_12, Function: 4 - HDACPU_SDI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_12__HDACPU_SDI                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_12, 4)
// Group: GPP_J, Pad: GPP_J_13, Function: 4 - HDACPU_SDO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_13__HDACPU_SDO                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_13, 4)
// Group: GPP_J, Pad: GPP_J_14, Function: 4 - HDACPU_BCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_14__HDACPU_BCLK               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_14, 4)
// Group: GPP_J, Pad: GPP_J_15, Function: 1 - AUX_PWRGD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_15__AUX_PWRGD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_15, 1)
// Group: GPP_J, Pad: GPP_J_16, Function: 1 - GLB_RST_WARN_B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_J_16__GLB_RST_WARN_B            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_J_16, 1)
// Group: GPP_J, Pad: RESET_SYNCB, Function: 1 - RESET_SYNCB
#define GPIOV2_MTL_PCH_S_MUXING__RESET_SYNCB__RESET_SYNCB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_RESET_SYNCB, 1)

//
// Pin muxing definitions for Group: vGPIO.
//

// Group: vGPIO, Pad: vGPIO_6, Function: 2 - UART0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_6__UART0_RXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_6, 2)
// Group: vGPIO, Pad: vGPIO_6, Function: 2 - ISH_UART0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_6__ISH_UART0_RXD              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_6, 2)
// Group: vGPIO, Pad: vGPIO_7, Function: 2 - UART0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_7__UART0_TXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_7, 2)
// Group: vGPIO, Pad: vGPIO_7, Function: 2 - ISH_UART0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_7__ISH_UART0_TXD              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_7, 2)
// Group: vGPIO, Pad: vGPIO_8, Function: 2 - UART0_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_8__UART0_RTSB                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_8, 2)
// Group: vGPIO, Pad: vGPIO_8, Function: 2 - ISH_UART0_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_8__ISH_UART0_RTSB             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_8, 2)
// Group: vGPIO, Pad: vGPIO_9, Function: 2 - UART0_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_9__UART0_CTSB                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_9, 2)
// Group: vGPIO, Pad: vGPIO_9, Function: 2 - ISH_UART0_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_9__ISH_UART0_CTSB             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_9, 2)
// Group: vGPIO, Pad: vGPIO_10, Function: 2 - ISH_UART0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_10__ISH_UART0_RXD             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_10, 2)
// Group: vGPIO, Pad: vGPIO_10, Function: 2 - UART2_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_10__UART2_RXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_10, 2)
// Group: vGPIO, Pad: vGPIO_11, Function: 2 - ISH_UART0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_11__ISH_UART0_TXD             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_11, 2)
// Group: vGPIO, Pad: vGPIO_11, Function: 2 - UART2_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_11__UART2_TXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_11, 2)
// Group: vGPIO, Pad: vGPIO_12, Function: 2 - ISH_UART0_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_12__ISH_UART0_RTSB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_12, 2)
// Group: vGPIO, Pad: vGPIO_12, Function: 2 - UART2_RTS_B
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_12__UART2_RTS_B               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_12, 2)
// Group: vGPIO, Pad: vGPIO_13, Function: 2 - ISH_UART0_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_13__ISH_UART0_CTSB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_13, 2)
// Group: vGPIO, Pad: vGPIO_13, Function: 2 - UART2_CTS_B
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_13__UART2_CTS_B               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_13, 2)
// Group: vGPIO, Pad: vGPIO_18, Function: 1 - CNV_BT_UART_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_18__CNV_BT_UART_RXD           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_18, 1)
// Group: vGPIO, Pad: vGPIO_19, Function: 1 - CNV_BT_UART_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_19__CNV_BT_UART_TXD           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_19, 1)
// Group: vGPIO, Pad: vGPIO_20, Function: 1 - CNV_BT_UART_RTS_B
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_20__CNV_BT_UART_RTS_B         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_20, 1)
// Group: vGPIO, Pad: vGPIO_21, Function: 1 - CNV_BT_UART_CTS_B
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_21__CNV_BT_UART_CTS_B         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_21, 1)
// Group: vGPIO, Pad: vGPIO_22, Function: 1 - CNV_MFUART1_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_22__CNV_MFUART1_RXD           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_22, 1)
// Group: vGPIO, Pad: vGPIO_23, Function: 1 - CNV_MFUART1_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_23__CNV_MFUART1_TXD           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_23, 1)
// Group: vGPIO, Pad: vGPIO_24, Function: 1 - CNV_MFUART1_RTS_B
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_24__CNV_MFUART1_RTS_B         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_24, 1)
// Group: vGPIO, Pad: vGPIO_25, Function: 1 - CNV_MFUART1_CTS_B
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_25__CNV_MFUART1_CTS_B         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_25, 1)
// Group: vGPIO, Pad: vGPIO_30, Function: 2 - I2S0_SCLK
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_30__I2S0_SCLK                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_30, 2)
// Group: vGPIO, Pad: vGPIO_30, Function: 2 - I2S1_SCLK
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_30__I2S1_SCLK                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_30, 2)
// Group: vGPIO, Pad: vGPIO_30, Function: 1 - I2S2_SCLK
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_30__I2S2_SCLK                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_30, 1)
// Group: vGPIO, Pad: vGPIO_31, Function: 2 - I2S0_SFRM
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_31__I2S0_SFRM                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_31, 2)
// Group: vGPIO, Pad: vGPIO_31, Function: 2 - I2S1_SFRM
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_31__I2S1_SFRM                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_31, 2)
// Group: vGPIO, Pad: vGPIO_31, Function: 1 - I2S2_SFRM
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_31__I2S2_SFRM                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_31, 1)
// Group: vGPIO, Pad: vGPIO_32, Function: 2 - I2S0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_32__I2S0_RXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_32, 2)
// Group: vGPIO, Pad: vGPIO_32, Function: 2 - I2S1_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_32__I2S1_RXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_32, 2)
// Group: vGPIO, Pad: vGPIO_32, Function: 1 - I2S2_RXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_32__I2S2_RXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_32, 1)
// Group: vGPIO, Pad: vGPIO_33, Function: 2 - I2S0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_33__I2S0_TXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_33, 2)
// Group: vGPIO, Pad: vGPIO_33, Function: 2 - I2S1_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_33__I2S1_TXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_33, 2)
// Group: vGPIO, Pad: vGPIO_33, Function: 1 - I2S2_TXD
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_33__I2S2_TXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_33, 1)
// Group: vGPIO, Pad: vGPIO_34, Function: 1 - CNV_BT_I2S_BCLK
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_34__CNV_BT_I2S_BCLK           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_34, 1)
// Group: vGPIO, Pad: vGPIO_35, Function: 1 - CNV_BT_I2S_WS_SYNC
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_35__CNV_BT_I2S_WS_SYNC        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_35, 1)
// Group: vGPIO, Pad: vGPIO_36, Function: 1 - CNV_BT_I2S_SDI
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_36__CNV_BT_I2S_SDI            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_36, 1)
// Group: vGPIO, Pad: vGPIO_37, Function: 1 - CNV_BT_I2S_SDO
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_37__CNV_BT_I2S_SDO            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_37, 1)

//
// Pin muxing definitions for Group: GPP_A.
//

// Group: GPP_A, Pad: GPP_A_0, Function: 1 - ESPI_IO_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_0__ESPI_IO_0                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_0, 1)
// Group: GPP_A, Pad: GPP_A_0, Function: 6 - IOM_GPPA_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_0__IOM_GPPA_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_0, 6)
// Group: GPP_A, Pad: GPP_A_1, Function: 1 - ESPI_IO_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_1__ESPI_IO_1                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_1, 1)
// Group: GPP_A, Pad: GPP_A_1, Function: 6 - IOM_GPPA_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_1__IOM_GPPA_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_1, 6)
// Group: GPP_A, Pad: GPP_A_2, Function: 1 - ESPI_IO_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_2__ESPI_IO_2                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_2, 1)
// Group: GPP_A, Pad: GPP_A_2, Function: 2 - SUSWARNB_SUSPWRDNACK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_2__SUSWARNB_SUSPWRDNACK       GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_2, 2)
// Group: GPP_A, Pad: GPP_A_2, Function: 6 - IOM_GPPA_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_2__IOM_GPPA_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_2, 6)
// Group: GPP_A, Pad: GPP_A_3, Function: 1 - ESPI_IO_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_3__ESPI_IO_3                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_3, 1)
// Group: GPP_A, Pad: GPP_A_3, Function: 2 - SUSACKB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_3__SUSACKB                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_3, 2)
// Group: GPP_A, Pad: GPP_A_3, Function: 6 - IOM_GPPA_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_3__IOM_GPPA_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_3, 6)
// Group: GPP_A, Pad: GPP_A_4, Function: 1 - ESPI_CS0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_4__ESPI_CS0B                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_4, 1)
// Group: GPP_A, Pad: GPP_A_4, Function: 6 - IOM_GPPA_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_4__IOM_GPPA_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_4, 6)
// Group: GPP_A, Pad: GPP_A_5, Function: 1 - ESPI_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_5__ESPI_CLK                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_5, 1)
// Group: GPP_A, Pad: GPP_A_5, Function: 6 - IOM_GPPA_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_5__IOM_GPPA_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_5, 6)
// Group: GPP_A, Pad: GPP_A_6, Function: 1 - ESPI_RESETB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_6__ESPI_RESETB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_6, 1)
// Group: GPP_A, Pad: GPP_A_6, Function: 6 - IOM_GPPA_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_6__IOM_GPPA_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_6, 6)
// Group: GPP_A, Pad: GPP_A_7, Function: 1 - ESPI_CS1B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_7__ESPI_CS1B                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_7, 1)
// Group: GPP_A, Pad: GPP_A_7, Function: 6 - IOM_GPPA_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_7__IOM_GPPA_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_7, 6)
// Group: GPP_A, Pad: GPP_A_8, Function: 1 - ESPI_CS2B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_8__ESPI_CS2B                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_8, 1)
// Group: GPP_A, Pad: GPP_A_8, Function: 6 - IOM_GPPA_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_8__IOM_GPPA_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_8, 6)
// Group: GPP_A, Pad: GPP_A_9, Function: 1 - ESPI_CS3B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_9__ESPI_CS3B                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_9, 1)
// Group: GPP_A, Pad: GPP_A_9, Function: 6 - IOM_GPPA_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_9__IOM_GPPA_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_9, 6)
// Group: GPP_A, Pad: GPP_A_10, Function: 1 - ESPI_ALERT0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_10__ESPI_ALERT0B              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_10, 1)
// Group: GPP_A, Pad: GPP_A_10, Function: 6 - IOM_GPPA_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_10__IOM_GPPA_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_10, 6)
// Group: GPP_A, Pad: GPP_A_11, Function: 1 - ESPI_ALERT1B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_11__ESPI_ALERT1B              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_11, 1)
// Group: GPP_A, Pad: GPP_A_11, Function: 6 - IOM_GPPA_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_11__IOM_GPPA_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_11, 6)
// Group: GPP_A, Pad: GPP_A_12, Function: 1 - ESPI_ALERT2B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_12__ESPI_ALERT2B              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_12, 1)
// Group: GPP_A, Pad: GPP_A_12, Function: 6 - IOM_GPPA_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_12__IOM_GPPA_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_12, 6)
// Group: GPP_A, Pad: GPP_A_13, Function: 1 - ESPI_ALERT3B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_13__ESPI_ALERT3B              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_13, 1)
// Group: GPP_A, Pad: GPP_A_13, Function: 6 - IOM_GPPA_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_A_13__IOM_GPPA_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_A_13, 6)
// Group: GPP_A, Pad: ESPI_CLK_LOOPBK, Function: 1 - ESPI_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__ESPI_CLK_LOOPBK__ESPI_CLK_LOOPBK    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_ESPI_CLK_LOOPBK, 1)

//
// Pin muxing definitions for Group: DIR_ESPI.
//

// Group: DIR_ESPI, Pad: PWRBTNB_OUT, Function: 1 - PWRBTNB_OUT
#define GPIOV2_MTL_PCH_S_MUXING__PWRBTNB_OUT__PWRBTNB_OUT            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_PWRBTNB_OUT, 1)
// Group: DIR_ESPI, Pad: DMI_PERSTB, Function: 1 - DMI_PERSTB
#define GPIOV2_MTL_PCH_S_MUXING__DMI_PERSTB__DMI_PERSTB              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DMI_PERSTB, 1)
// Group: DIR_ESPI, Pad: DMI_CLKREQB, Function: 1 - DMI_CLKREQB
#define GPIOV2_MTL_PCH_S_MUXING__DMI_CLKREQB__DMI_CLKREQB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DMI_CLKREQB, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_IO_0, Function: 1 - DIR_ESPI_IO_0
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_IO_0__DIR_ESPI_IO_0        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_IO_0, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_IO_1, Function: 1 - DIR_ESPI_IO_1
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_IO_1__DIR_ESPI_IO_1        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_IO_1, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_IO_2, Function: 1 - DIR_ESPI_IO_2
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_IO_2__DIR_ESPI_IO_2        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_IO_2, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_IO_3, Function: 1 - DIR_ESPI_IO_3
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_IO_3__DIR_ESPI_IO_3        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_IO_3, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_CSB, Function: 1 - DIR_ESPI_CSB
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_CSB__DIR_ESPI_CSB          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_CSB, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_RESETB, Function: 1 - DIR_ESPI_RESETB
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_RESETB__DIR_ESPI_RESETB    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_RESETB, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_CLK, Function: 1 - DIR_ESPI_CLK
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_CLK__DIR_ESPI_CLK          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_CLK, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_RCLK, Function: 1 - DIR_ESPI_RCLK
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_RCLK__DIR_ESPI_RCLK        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_RCLK, 1)
// Group: DIR_ESPI, Pad: DIR_ESPI_ALERTB, Function: 1 - DIR_ESPI_ALERTB
#define GPIOV2_MTL_PCH_S_MUXING__DIR_ESPI_ALERTB__DIR_ESPI_ALERTB    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DIR_ESPI_ALERTB, 1)

//
// Pin muxing definitions for Group: GPP_B.
//

// Group: GPP_B, Pad: GPP_B_0, Function: 1 - PCIE_LNK_DOWN
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_0__PCIE_LNK_DOWN              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_0, 1)
// Group: GPP_B, Pad: GPP_B_0, Function: 6 - IOM_GPPB_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_0__IOM_GPPB_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_0, 6)
// Group: GPP_B, Pad: GPP_B_1, Function: 2 - ISH_UART0_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_1__ISH_UART0_RTSB             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_1, 2)
// Group: GPP_B, Pad: GPP_B_1, Function: 2 - GSPI2_CS1B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_1__GSPI2_CS1B                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_1, 2)
// Group: GPP_B, Pad: GPP_B_1, Function: 6 - IOM_GPPB_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_1__IOM_GPPB_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_1, 6)
// Group: GPP_B, Pad: GPP_B_2, Function: 1 - ISH_GP_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_2__ISH_GP_8                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_2, 1)
// Group: GPP_B, Pad: GPP_B_2, Function: 6 - IOM_GPPB_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_2__IOM_GPPB_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_2, 6)
// Group: GPP_B, Pad: GPP_B_3, Function: 1 - CPU_GP_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_3__CPU_GP_2                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_3, 1)
// Group: GPP_B, Pad: GPP_B_3, Function: 6 - IOM_GPPB_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_3__IOM_GPPB_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_3, 6)
// Group: GPP_B, Pad: GPP_B_4, Function: 1 - CPU_GP_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_4__CPU_GP_3                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_4, 1)
// Group: GPP_B, Pad: GPP_B_4, Function: 6 - IOM_GPPB_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_4__IOM_GPPB_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_4, 6)
// Group: GPP_B, Pad: GPP_B_5, Function: 1 - SX_EXIT_HOLDOFFB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_5__SX_EXIT_HOLDOFFB           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_5, 1)
// Group: GPP_B, Pad: GPP_B_5, Function: 2 - ISH_GP_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_5__ISH_GP_6                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_5, 2)
// Group: GPP_B, Pad: GPP_B_5, Function: 3 - IEH_FATAL_ERR2B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_5__IEH_FATAL_ERR2B            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_5, 3)
// Group: GPP_B, Pad: GPP_B_5, Function: 6 - IOM_GPPB_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_5__IOM_GPPB_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_5, 6)
// Group: GPP_B, Pad: GPP_B_6, Function: 1 - CLKOUT_48
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_6__CLKOUT_48                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_6, 1)
// Group: GPP_B, Pad: GPP_B_6, Function: 2 - CNV_DEBUG_00
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_6__CNV_DEBUG_00               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_6, 2)
// Group: GPP_B, Pad: GPP_B_6, Function: 6 - IOM_GPPB_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_6__IOM_GPPB_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_6, 6)
// Group: GPP_B, Pad: GPP_B_7, Function: 1 - ISH_GP_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_7__ISH_GP_7                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_7, 1)
// Group: GPP_B, Pad: GPP_B_7, Function: 2 - CNV_DEBUG_01
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_7__CNV_DEBUG_01               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_7, 2)
// Group: GPP_B, Pad: GPP_B_7, Function: 6 - IOM_GPPB_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_7__IOM_GPPB_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_7, 6)
// Group: GPP_B, Pad: GPP_B_8, Function: 1 - ISH_GP_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_8__ISH_GP_0                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_8, 1)
// Group: GPP_B, Pad: GPP_B_8, Function: 2 - CNV_DEBUG_02
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_8__CNV_DEBUG_02               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_8, 2)
// Group: GPP_B, Pad: GPP_B_8, Function: 6 - IOM_GPPB_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_8__IOM_GPPB_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_8, 6)
// Group: GPP_B, Pad: GPP_B_9, Function: 1 - ISH_GP_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_9__ISH_GP_1                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_9, 1)
// Group: GPP_B, Pad: GPP_B_9, Function: 2 - CNV_DEBUG_03
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_9__CNV_DEBUG_03               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_9, 2)
// Group: GPP_B, Pad: GPP_B_9, Function: 6 - IOM_GPPB_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_9__IOM_GPPB_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_9, 6)
// Group: GPP_B, Pad: GPP_B_10, Function: 1 - ISH_GP_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_10__ISH_GP_2                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_10, 1)
// Group: GPP_B, Pad: GPP_B_10, Function: 2 - CNV_DEBUG_04
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_10__CNV_DEBUG_04              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_10, 2)
// Group: GPP_B, Pad: GPP_B_10, Function: 6 - IOM_GPPB_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_10__IOM_GPPB_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_10, 6)
// Group: GPP_B, Pad: GPP_B_11, Function: 1 - I2S_MCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_11__I2S_MCLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_11, 1)
// Group: GPP_B, Pad: GPP_B_11, Function: 6 - IOM_GPPB_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_11__IOM_GPPB_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_11, 6)
// Group: GPP_B, Pad: GPP_B_12, Function: 1 - SLP_S0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_12__SLP_S0B                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_12, 1)
// Group: GPP_B, Pad: GPP_B_12, Function: 6 - IOM_GPPB_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_12__IOM_GPPB_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_12, 6)
// Group: GPP_B, Pad: GPP_B_13, Function: 1 - PLTRSTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_13__PLTRSTB                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_13, 1)
// Group: GPP_B, Pad: GPP_B_13, Function: 6 - IOM_GPPB_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_13__IOM_GPPB_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_13, 6)
// Group: GPP_B, Pad: GPP_B_14, Function: 6 - IOM_GPPB_14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_14__IOM_GPPB_14               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_14, 6)
// Group: GPP_B, Pad: GPP_B_15, Function: 1 - ISH_GP_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_15__ISH_GP_3                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_15, 1)
// Group: GPP_B, Pad: GPP_B_15, Function: 2 - CNV_DEBUG_05
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_15__CNV_DEBUG_05              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_15, 2)
// Group: GPP_B, Pad: GPP_B_15, Function: 6 - IOM_GPPB_15
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_15__IOM_GPPB_15               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_15, 6)
// Group: GPP_B, Pad: GPP_B_16, Function: 1 - ISH_GP_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_16__ISH_GP_4                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_16, 1)
// Group: GPP_B, Pad: GPP_B_16, Function: 2 - CNV_DEBUG_06
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_16__CNV_DEBUG_06              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_16, 2)
// Group: GPP_B, Pad: GPP_B_16, Function: 6 - IOM_GPPB_16
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_16__IOM_GPPB_16               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_16, 6)
// Group: GPP_B, Pad: GPP_B_17, Function: 1 - ISH_GP_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_17__ISH_GP_5                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_17, 1)
// Group: GPP_B, Pad: GPP_B_17, Function: 2 - CNV_DEBUG_07
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_17__CNV_DEBUG_07              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_17, 2)
// Group: GPP_B, Pad: GPP_B_17, Function: 6 - IOM_GPPB_17
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_17__IOM_GPPB_17               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_17, 6)
// Group: GPP_B, Pad: GPP_B_18, Function: 1 - PMCALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_18__PMCALERTB                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_18, 1)
// Group: GPP_B, Pad: GPP_B_18, Function: 6 - IOM_GPPB_18
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_18__IOM_GPPB_18               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_18, 6)
// Group: GPP_B, Pad: GPP_B_19, Function: 1 - FUSA_DIAGTEST_EN
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_19__FUSA_DIAGTEST_EN          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_19, 1)
// Group: GPP_B, Pad: GPP_B_19, Function: 6 - IOM_GPPB_19
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_19__IOM_GPPB_19               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_19, 6)
// Group: GPP_B, Pad: GPP_B_20, Function: 1 - FUSA_DIAGTEST_MODE
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_20__FUSA_DIAGTEST_MODE        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_20, 1)
// Group: GPP_B, Pad: GPP_B_20, Function: 6 - IOM_GPPB_20
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_20__IOM_GPPB_20               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_20, 6)
// Group: GPP_B, Pad: GPP_B_21, Function: 1 - SML1ALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_21__SML1ALERTB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_21, 1)
// Group: GPP_B, Pad: GPP_B_21, Function: 2 - PCHHOTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_21__PCHHOTB                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_21, 2)
// Group: GPP_B, Pad: GPP_B_21, Function: 6 - IOM_GPPB_21
#define GPIOV2_MTL_PCH_S_MUXING__GPP_B_21__IOM_GPPB_21               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_B_21, 6)

//
// Pin muxing definitions for Group: DSW.
//

// Group: DSW, Pad: GPD_0, Function: 1 - BATLOWB
#define GPIOV2_MTL_PCH_S_MUXING__GPD_0__BATLOWB                      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_0, 1)
// Group: DSW, Pad: GPD_1, Function: 1 - ACPRESENT
#define GPIOV2_MTL_PCH_S_MUXING__GPD_1__ACPRESENT                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_1, 1)
// Group: DSW, Pad: GPD_2, Function: 1 - LAN_WAKEB
#define GPIOV2_MTL_PCH_S_MUXING__GPD_2__LAN_WAKEB                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_2, 1)
// Group: DSW, Pad: GPD_3, Function: 1 - PWRBTNB
#define GPIOV2_MTL_PCH_S_MUXING__GPD_3__PWRBTNB                      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_3, 1)
// Group: DSW, Pad: GPD_4, Function: 1 - SLP_S3B
#define GPIOV2_MTL_PCH_S_MUXING__GPD_4__SLP_S3B                      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_4, 1)
// Group: DSW, Pad: GPD_5, Function: 1 - SLP_S4B
#define GPIOV2_MTL_PCH_S_MUXING__GPD_5__SLP_S4B                      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_5, 1)
// Group: DSW, Pad: GPD_6, Function: 1 - SLP_AB
#define GPIOV2_MTL_PCH_S_MUXING__GPD_6__SLP_AB                       GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_6, 1)
// Group: DSW, Pad: GPD_8, Function: 1 - SUSCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPD_8__SUSCLK                       GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_8, 1)
// Group: DSW, Pad: GPD_9, Function: 1 - SLP_WLANB
#define GPIOV2_MTL_PCH_S_MUXING__GPD_9__SLP_WLANB                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_9, 1)
// Group: DSW, Pad: GPD_10, Function: 1 - SLP_S5B
#define GPIOV2_MTL_PCH_S_MUXING__GPD_10__SLP_S5B                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_10, 1)
// Group: DSW, Pad: GPD_11, Function: 1 - LANPHYPC
#define GPIOV2_MTL_PCH_S_MUXING__GPD_11__LANPHYPC                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPD_11, 1)
// Group: DSW, Pad: SLP_LANB, Function: 1 - SLP_LANB
#define GPIOV2_MTL_PCH_S_MUXING__SLP_LANB__SLP_LANB                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SLP_LANB, 1)
// Group: DSW, Pad: SLP_SUSB, Function: 1 - SLP_SUSB
#define GPIOV2_MTL_PCH_S_MUXING__SLP_SUSB__SLP_SUSB                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SLP_SUSB, 1)
// Group: DSW, Pad: WAKEB, Function: 1 - WAKEB
#define GPIOV2_MTL_PCH_S_MUXING__WAKEB__WAKEB                        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_WAKEB, 1)
// Group: DSW, Pad: DSW_SPARE, Function: 1 - DSW_SPARE
#define GPIOV2_MTL_PCH_S_MUXING__DSW_SPARE__DSW_SPARE                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DSW_SPARE, 1)

//
// Pin muxing definitions for Group: SPI0.
//

// Group: SPI0, Pad: SPI0_IO_2, Function: 1 - SPI0_IO_2
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_IO_2__SPI0_IO_2                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_IO_2, 1)
// Group: SPI0, Pad: SPI0_IO_3, Function: 1 - SPI0_IO_3
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_IO_3__SPI0_IO_3                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_IO_3, 1)
// Group: SPI0, Pad: SPI0_MOSI_IO_0, Function: 1 - SPI0_MOSI_IO_0
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_MOSI_IO_0__SPI0_MOSI_IO_0      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_MOSI_IO_0, 1)
// Group: SPI0, Pad: SPI0_MISO_IO_1, Function: 1 - SPI0_MISO_IO_1
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_MISO_IO_1__SPI0_MISO_IO_1      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_MISO_IO_1, 1)
// Group: SPI0, Pad: SPI0_TPM_CSB, Function: 1 - SPI0_TPM_CSB
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_TPM_CSB__SPI0_TPM_CSB          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_TPM_CSB, 1)
// Group: SPI0, Pad: SPI0_FLASH_0_CSB, Function: 1 - SPI0_FLASH_0_CSB
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_FLASH_0_CSB__SPI0_FLASH_0_CSB  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_FLASH_0_CSB, 1)
// Group: SPI0, Pad: SPI0_FLASH_1_CSB, Function: 1 - SPI0_FLASH_1_CSB
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_FLASH_1_CSB__SPI0_FLASH_1_CSB  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_FLASH_1_CSB, 1)
// Group: SPI0, Pad: SPI0_CLK, Function: 1 - SPI0_CLK
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_CLK__SPI0_CLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_CLK, 1)
// Group: SPI0, Pad: SPI0_CLK_LOOPBK, Function: 1 - SPI0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__SPI0_CLK_LOOPBK__SPI0_CLK_LOOPBK    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI0_CLK_LOOPBK, 1)

//
// Pin muxing definitions for Group: GPP_C.
//

// Group: GPP_C, Pad: GPP_C_0, Function: 1 - SMBCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_0__SMBCLK                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_0, 1)
// Group: GPP_C, Pad: GPP_C_0, Function: 6 - IOM_GPPC_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_0__IOM_GPPC_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_0, 6)
// Group: GPP_C, Pad: GPP_C_1, Function: 1 - SMBDATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_1__SMBDATA                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_1, 1)
// Group: GPP_C, Pad: GPP_C_1, Function: 6 - IOM_GPPC_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_1__IOM_GPPC_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_1, 6)
// Group: GPP_C, Pad: GPP_C_2, Function: 1 - SMBALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_2__SMBALERTB                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_2, 1)
// Group: GPP_C, Pad: GPP_C_2, Function: 6 - IOM_GPPC_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_2__IOM_GPPC_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_2, 6)
// Group: GPP_C, Pad: GPP_C_3, Function: 2 - ISH_UART0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_3__ISH_UART0_RXD              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_3, 2)
// Group: GPP_C, Pad: GPP_C_3, Function: 2 - SML0BDATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_3__SML0BDATA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_3, 2)
// Group: GPP_C, Pad: GPP_C_3, Function: 3 - I2C2_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_3__I2C2_SDA                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_3, 3)
// Group: GPP_C, Pad: GPP_C_3, Function: 6 - IOM_GPPC_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_3__IOM_GPPC_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_3, 6)
// Group: GPP_C, Pad: GPP_C_4, Function: 2 - ISH_UART0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_4__ISH_UART0_TXD              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_4, 2)
// Group: GPP_C, Pad: GPP_C_4, Function: 2 - SML0BCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_4__SML0BCLK                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_4, 2)
// Group: GPP_C, Pad: GPP_C_4, Function: 3 - I2C2_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_4__I2C2_SCL                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_4, 3)
// Group: GPP_C, Pad: GPP_C_4, Function: 6 - IOM_GPPC_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_4__IOM_GPPC_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_4, 6)
// Group: GPP_C, Pad: GPP_C_5, Function: 1 - SML0ALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_5__SML0ALERTB                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_5, 1)
// Group: GPP_C, Pad: GPP_C_5, Function: 6 - IOM_GPPC_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_5__IOM_GPPC_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_5, 6)
// Group: GPP_C, Pad: GPP_C_6, Function: 1 - ISH_I2C2_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_6__ISH_I2C2_SDA               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_6, 1)
// Group: GPP_C, Pad: GPP_C_6, Function: 2 - I2C3_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_6__I2C3_SDA                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_6, 2)
// Group: GPP_C, Pad: GPP_C_6, Function: 6 - IOM_GPPC_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_6__IOM_GPPC_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_6, 6)
// Group: GPP_C, Pad: GPP_C_7, Function: 1 - ISH_I2C2_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_7__ISH_I2C2_SCL               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_7, 1)
// Group: GPP_C, Pad: GPP_C_7, Function: 2 - I2C3_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_7__I2C3_SCL                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_7, 2)
// Group: GPP_C, Pad: GPP_C_7, Function: 6 - IOM_GPPC_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_7__IOM_GPPC_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_7, 6)
// Group: GPP_C, Pad: GPP_C_8, Function: 2 - UART0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_8__UART0_RXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_8, 2)
// Group: GPP_C, Pad: GPP_C_8, Function: 6 - IOM_GPPC_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_8__IOM_GPPC_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_8, 6)
// Group: GPP_C, Pad: GPP_C_9, Function: 2 - UART0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_9__UART0_TXD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_9, 2)
// Group: GPP_C, Pad: GPP_C_9, Function: 6 - IOM_GPPC_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_9__IOM_GPPC_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_9, 6)
// Group: GPP_C, Pad: GPP_C_10, Function: 2 - UART0_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_10__UART0_RTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_10, 2)
// Group: GPP_C, Pad: GPP_C_10, Function: 6 - IOM_GPPC_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_10__IOM_GPPC_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_10, 6)
// Group: GPP_C, Pad: GPP_C_11, Function: 2 - UART0_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_11__UART0_CTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_11, 2)
// Group: GPP_C, Pad: GPP_C_11, Function: 6 - IOM_GPPC_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_11__IOM_GPPC_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_11, 6)
// Group: GPP_C, Pad: GPP_C_12, Function: 1 - UART1_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_12__UART1_RXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_12, 1)
// Group: GPP_C, Pad: GPP_C_12, Function: 2 - ISH_UART1_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_12__ISH_UART1_RXD             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_12, 2)
// Group: GPP_C, Pad: GPP_C_12, Function: 6 - IOM_GPPC_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_12__IOM_GPPC_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_12, 6)
// Group: GPP_C, Pad: GPP_C_13, Function: 1 - UART1_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_13__UART1_TXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_13, 1)
// Group: GPP_C, Pad: GPP_C_13, Function: 2 - ISH_UART1_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_13__ISH_UART1_TXD             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_13, 2)
// Group: GPP_C, Pad: GPP_C_13, Function: 6 - IOM_GPPC_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_13__IOM_GPPC_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_13, 6)
// Group: GPP_C, Pad: GPP_C_14, Function: 1 - UART1_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_14__UART1_RTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_14, 1)
// Group: GPP_C, Pad: GPP_C_14, Function: 2 - ISH_UART1_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_14__ISH_UART1_RTSB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_14, 2)
// Group: GPP_C, Pad: GPP_C_14, Function: 6 - IOM_GPPC_14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_14__IOM_GPPC_14               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_14, 6)
// Group: GPP_C, Pad: GPP_C_15, Function: 1 - UART1_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_15__UART1_CTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_15, 1)
// Group: GPP_C, Pad: GPP_C_15, Function: 2 - ISH_UART1_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_15__ISH_UART1_CTSB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_15, 2)
// Group: GPP_C, Pad: GPP_C_15, Function: 6 - IOM_GPPC_15
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_15__IOM_GPPC_15               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_15, 6)
// Group: GPP_C, Pad: GPP_C_16, Function: 1 - USB2_OCB_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_16__USB2_OCB_4                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_16, 1)
// Group: GPP_C, Pad: GPP_C_16, Function: 2 - I2C4_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_16__I2C4_SDA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_16, 2)
// Group: GPP_C, Pad: GPP_C_16, Function: 6 - IOM_GPPC_16
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_16__IOM_GPPC_16               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_16, 6)
// Group: GPP_C, Pad: GPP_C_17, Function: 1 - USB2_OCB_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_17__USB2_OCB_5                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_17, 1)
// Group: GPP_C, Pad: GPP_C_17, Function: 2 - I2C4_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_17__I2C4_SCL                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_17, 2)
// Group: GPP_C, Pad: GPP_C_17, Function: 6 - IOM_GPPC_17
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_17__IOM_GPPC_17               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_17, 6)
// Group: GPP_C, Pad: GPP_C_18, Function: 1 - USB2_OCB_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_18__USB2_OCB_6                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_18, 1)
// Group: GPP_C, Pad: GPP_C_18, Function: 2 - I2C5_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_18__I2C5_SDA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_18, 2)
// Group: GPP_C, Pad: GPP_C_18, Function: 6 - IOM_GPPC_18
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_18__IOM_GPPC_18               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_18, 6)
// Group: GPP_C, Pad: GPP_C_19, Function: 1 - USB2_OCB_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_19__USB2_OCB_7                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_19, 1)
// Group: GPP_C, Pad: GPP_C_19, Function: 2 - I2C5_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_19__I2C5_SCL                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_19, 2)
// Group: GPP_C, Pad: GPP_C_19, Function: 6 - IOM_GPPC_19
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_19__IOM_GPPC_19               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_19, 6)
// Group: GPP_C, Pad: GPP_C_20, Function: 2 - UART2_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_20__UART2_RXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_20, 2)
// Group: GPP_C, Pad: GPP_C_20, Function: 3 - CNV_MFUART0_RXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_20__CNV_MFUART0_RXD           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_20, 3)
// Group: GPP_C, Pad: GPP_C_20, Function: 6 - IOM_GPPC_20
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_20__IOM_GPPC_20               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_20, 6)
// Group: GPP_C, Pad: GPP_C_21, Function: 2 - UART2_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_21__UART2_TXD                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_21, 2)
// Group: GPP_C, Pad: GPP_C_21, Function: 3 - CNV_MFUART0_TXD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_21__CNV_MFUART0_TXD           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_21, 3)
// Group: GPP_C, Pad: GPP_C_21, Function: 6 - IOM_GPPC_21
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_21__IOM_GPPC_21               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_21, 6)
// Group: GPP_C, Pad: GPP_C_22, Function: 1 - UART2_RTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_22__UART2_RTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_22, 1)
// Group: GPP_C, Pad: GPP_C_22, Function: 3 - CNV_MFUART0_RTS_B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_22__CNV_MFUART0_RTS_B         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_22, 3)
// Group: GPP_C, Pad: GPP_C_22, Function: 6 - IOM_GPPC_22
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_22__IOM_GPPC_22               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_22, 6)
// Group: GPP_C, Pad: GPP_C_23, Function: 1 - UART2_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_23__UART2_CTSB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_23, 1)
// Group: GPP_C, Pad: GPP_C_23, Function: 3 - CNV_MFUART0_CTS_B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_23__CNV_MFUART0_CTS_B         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_23, 3)
// Group: GPP_C, Pad: GPP_C_23, Function: 6 - IOM_GPPC_23
#define GPIOV2_MTL_PCH_S_MUXING__GPP_C_23__IOM_GPPC_23               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_C_23, 6)

//
// Pin muxing definitions for Group: GPP_H.
//

// Group: GPP_H, Pad: GPP_H_0, Function: 6 - IOM_GPPH_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_0__IOM_GPPH_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_0, 6)
// Group: GPP_H, Pad: GPP_H_1, Function: 1 - SRCCLKREQB_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_1__SRCCLKREQB_8               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_1, 1)
// Group: GPP_H, Pad: GPP_H_1, Function: 6 - IOM_GPPH_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_1__IOM_GPPH_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_1, 6)
// Group: GPP_H, Pad: GPP_H_2, Function: 1 - SRCCLKREQB_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_2__SRCCLKREQB_9               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_2, 1)
// Group: GPP_H, Pad: GPP_H_2, Function: 6 - IOM_GPPH_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_2__IOM_GPPH_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_2, 6)
// Group: GPP_H, Pad: GPP_H_3, Function: 1 - SRCCLKREQB_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_3__SRCCLKREQB_10              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_3, 1)
// Group: GPP_H, Pad: GPP_H_3, Function: 6 - IOM_GPPH_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_3__IOM_GPPH_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_3, 6)
// Group: GPP_H, Pad: GPP_H_4, Function: 1 - SRCCLKREQB_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_4__SRCCLKREQB_11              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_4, 1)
// Group: GPP_H, Pad: GPP_H_4, Function: 6 - IOM_GPPH_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_4__IOM_GPPH_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_4, 6)
// Group: GPP_H, Pad: GPP_H_5, Function: 1 - USB2_OCB_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_5__USB2_OCB_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_5, 1)
// Group: GPP_H, Pad: GPP_H_5, Function: 6 - IOM_GPPH_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_5__IOM_GPPH_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_5, 6)
// Group: GPP_H, Pad: GPP_H_6, Function: 1 - USB2_OCB_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_6__USB2_OCB_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_6, 1)
// Group: GPP_H, Pad: GPP_H_6, Function: 6 - IOM_GPPH_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_6__IOM_GPPH_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_6, 6)
// Group: GPP_H, Pad: GPP_H_7, Function: 1 - USB2_OCB_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_7__USB2_OCB_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_7, 1)
// Group: GPP_H, Pad: GPP_H_7, Function: 2 - GMII_MDC_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_7__GMII_MDC_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_7, 2)
// Group: GPP_H, Pad: GPP_H_7, Function: 6 - IOM_GPPH_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_7__IOM_GPPH_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_7, 6)
// Group: GPP_H, Pad: GPP_H_8, Function: 1 - USB2_OCB_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_8__USB2_OCB_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_8, 1)
// Group: GPP_H, Pad: GPP_H_8, Function: 2 - GMII_MDIO_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_8__GMII_MDIO_1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_8, 2)
// Group: GPP_H, Pad: GPP_H_8, Function: 6 - IOM_GPPH_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_8__IOM_GPPH_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_8, 6)
// Group: GPP_H, Pad: GPP_H_9, Function: 1 - SML2CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_9__SML2CLK                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_9, 1)
// Group: GPP_H, Pad: GPP_H_9, Function: 6 - IOM_GPPH_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_9__IOM_GPPH_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_9, 6)
// Group: GPP_H, Pad: GPP_H_10, Function: 1 - SML2DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_10__SML2DATA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_10, 1)
// Group: GPP_H, Pad: GPP_H_10, Function: 6 - IOM_GPPH_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_10__IOM_GPPH_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_10, 6)
// Group: GPP_H, Pad: GPP_H_11, Function: 1 - SML2ALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_11__SML2ALERTB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_11, 1)
// Group: GPP_H, Pad: GPP_H_11, Function: 6 - IOM_GPPH_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_11__IOM_GPPH_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_11, 6)
// Group: GPP_H, Pad: GPP_H_12, Function: 1 - SML3CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_12__SML3CLK                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_12, 1)
// Group: GPP_H, Pad: GPP_H_12, Function: 6 - IOM_GPPH_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_12__IOM_GPPH_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_12, 6)
// Group: GPP_H, Pad: GPP_H_13, Function: 1 - SML3DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_13__SML3DATA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_13, 1)
// Group: GPP_H, Pad: GPP_H_13, Function: 6 - IOM_GPPH_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_13__IOM_GPPH_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_13, 6)
// Group: GPP_H, Pad: GPP_H_14, Function: 1 - SML3ALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_14__SML3ALERTB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_14, 1)
// Group: GPP_H, Pad: GPP_H_14, Function: 6 - IOM_GPPH_14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_14__IOM_GPPH_14               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_14, 6)
// Group: GPP_H, Pad: GPP_H_15, Function: 1 - SML4CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_15__SML4CLK                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_15, 1)
// Group: GPP_H, Pad: GPP_H_15, Function: 6 - IOM_GPPH_15
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_15__IOM_GPPH_15               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_15, 6)
// Group: GPP_H, Pad: GPP_H_16, Function: 1 - SML4DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_16__SML4DATA                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_16, 1)
// Group: GPP_H, Pad: GPP_H_16, Function: 6 - IOM_GPPH_16
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_16__IOM_GPPH_16               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_16, 6)
// Group: GPP_H, Pad: GPP_H_17, Function: 1 - SML4ALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_17__SML4ALERTB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_17, 1)
// Group: GPP_H, Pad: GPP_H_17, Function: 6 - IOM_GPPH_17
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_17__IOM_GPPH_17               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_17, 6)
// Group: GPP_H, Pad: GPP_H_18, Function: 1 - ISH_I2C1_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_18__ISH_I2C1_SDA              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_18, 1)
// Group: GPP_H, Pad: GPP_H_18, Function: 2 - SMIB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_18__SMIB                      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_18, 2)
// Group: GPP_H, Pad: GPP_H_18, Function: 6 - IOM_GPPH_18
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_18__IOM_GPPH_18               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_18, 6)
// Group: GPP_H, Pad: GPP_H_19, Function: 1 - ISH_I2C1_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_19__ISH_I2C1_SCL              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_19, 1)
// Group: GPP_H, Pad: GPP_H_19, Function: 2 - NMIB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_19__NMIB                      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_19, 2)
// Group: GPP_H, Pad: GPP_H_19, Function: 6 - IOM_GPPH_19
#define GPIOV2_MTL_PCH_S_MUXING__GPP_H_19__IOM_GPPH_19               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_H_19, 6)

//
// Pin muxing definitions for Group: vGPIO_3.
//

// Group: vGPIO_3, Pad: vGPIO_PCIE_80, Function: 1 - CPU_PCIE_LNK_DN_0
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_PCIE_80__CPU_PCIE_LNK_DN_0    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_PCIE_80, 1)
// Group: vGPIO_3, Pad: vGPIO_PCIE_81, Function: 1 - CPU_PCIE_LNK_DN_1
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_PCIE_81__CPU_PCIE_LNK_DN_1    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_PCIE_81, 1)
// Group: vGPIO_3, Pad: vGPIO_PCIE_82, Function: 1 - CPU_PCIE_LNK_DN_2
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_PCIE_82__CPU_PCIE_LNK_DN_2    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_PCIE_82, 1)
// Group: vGPIO_3, Pad: vGPIO_PCIE_83, Function: 1 - CPU_PCIE_LNK_DN_3
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_PCIE_83__CPU_PCIE_LNK_DN_3    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_PCIE_83, 1)

//
// Pin muxing definitions for Group: vGPIO_0.
//

// Group: vGPIO_0, Pad: vGPIO_USB_0, Function: 1 - ESPI_USB_OCB_0
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_0__ESPI_USB_OCB_0         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_0, 1)
// Group: vGPIO_0, Pad: vGPIO_USB_1, Function: 1 - ESPI_USB_OCB_1
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_1__ESPI_USB_OCB_1         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_1, 1)
// Group: vGPIO_0, Pad: vGPIO_USB_2, Function: 1 - ESPI_USB_OCB_2
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_2__ESPI_USB_OCB_2         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_2, 1)
// Group: vGPIO_0, Pad: vGPIO_USB_3, Function: 1 - ESPI_USB_OCB_3
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_3__ESPI_USB_OCB_3         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_3, 1)
// Group: vGPIO_0, Pad: vGPIO_USB_8, Function: 1 - USB_CPU_OCB_0
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_8__USB_CPU_OCB_0          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_8, 1)
// Group: vGPIO_0, Pad: vGPIO_USB_9, Function: 1 - USB_CPU_OCB_1
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_9__USB_CPU_OCB_1          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_9, 1)
// Group: vGPIO_0, Pad: vGPIO_USB_10, Function: 1 - USB_CPU_OCB_2
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_10__USB_CPU_OCB_2         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_10, 1)
// Group: vGPIO_0, Pad: vGPIO_USB_11, Function: 1 - USB_CPU_OCB_3
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_USB_11__USB_CPU_OCB_3         GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_USB_11, 1)

//
// Pin muxing definitions for Group: vGPIO_4.
//

// Group: vGPIO_4, Pad: vGPIO_ISCLK_0, Function: 1 - ESPI_ISCLK_XTAL_CLKREQ
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_ISCLK_0__ESPI_ISCLK_XTAL_CLKREQ GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_ISCLK_0, 1)
// Group: vGPIO_4, Pad: vGPIO_ISCLK_1, Function: 1 - ISCLK_ESPI_XTAL_CLKACK
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_ISCLK_1__ISCLK_ESPI_XTAL_CLKACK GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_ISCLK_1, 1)
// Group: vGPIO_4, Pad: vGPIO_SLPC_0, Function: 1 - ME_SLPC_FTPM_ENABLE
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_0__ME_SLPC_FTPM_ENABLE   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_0, 1)
// Group: vGPIO_4, Pad: vGPIO_SLPC_1, Function: 1 - GP_SLPC_DTFUS_CORE_SPITPM_DIS
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_1__GP_SLPC_DTFUS_CORE_SPITPM_DIS GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_1, 1)
// Group: vGPIO_4, Pad: vGPIO_SLPC_2, Function: 1 - GP_SLPC_SPI_STRAP_TOS
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_2__GP_SLPC_SPI_STRAP_TOS GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_2, 1)
// Group: vGPIO_4, Pad: vGPIO_SLPC_3, Function: 1 - GP_SLPC_DTFUS_CORE_SPITPM_DIS_L0
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_3__GP_SLPC_DTFUS_CORE_SPITPM_DIS_L0 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_3, 1)
// Group: vGPIO_4, Pad: vGPIO_SLPC_3, Function: 2 - GP_SLPC_DTFUS_CORE_SPITPM_DIS_L1
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_3__GP_SLPC_DTFUS_CORE_SPITPM_DIS_L1 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_3, 2)
// Group: vGPIO_4, Pad: vGPIO_SLPC_4, Function: 1 - GP_SLPC_SPI_STRAP_TOS_L0
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_4__GP_SLPC_SPI_STRAP_TOS_L0 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_4, 1)
// Group: vGPIO_4, Pad: vGPIO_SLPC_4, Function: 2 - GP_SLPC_SPI_STRAP_TOS_L1
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_4__GP_SLPC_SPI_STRAP_TOS_L1 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_4, 2)
// Group: vGPIO_4, Pad: vGPIO_SLPC_5, Function: 1 - LPC_PRR_TS_OVR
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_5__LPC_PRR_TS_OVR        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_5, 1)
// Group: vGPIO_4, Pad: vGPIO_SLPC_6, Function: 1 - ITSS_KU1_SHTDWN
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SLPC_6__ITSS_KU1_SHTDWN       GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SLPC_6, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_0, Function: 1 - VWM_SPARE_0
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_0__VWM_SPARE_0          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_0, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_1, Function: 1 - VWM_SPARE_1
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_1__VWM_SPARE_1          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_1, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_2, Function: 1 - VWM_SPARE_2
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_2__VWM_SPARE_2          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_2, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_3, Function: 1 - VWM_SPARE_3
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_3__VWM_SPARE_3          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_3, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_4, Function: 1 - VWM_SPARE_4
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_4__VWM_SPARE_4          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_4, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_5, Function: 1 - VWM_SPARE_5
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_5__VWM_SPARE_5          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_5, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_6, Function: 1 - VWM_SPARE_6
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_6__VWM_SPARE_6          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_6, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_7, Function: 1 - VWM_SPARE_7
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_7__VWM_SPARE_7          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_7, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_8, Function: 1 - VWM_SPARE_8
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_8__VWM_SPARE_8          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_8, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_9, Function: 1 - VWM_SPARE_9
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_9__VWM_SPARE_9          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_9, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_10, Function: 1 - VWM_SPARE_10
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_10__VWM_SPARE_10        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_10, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_11, Function: 1 - VWM_SPARE_11
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_11__VWM_SPARE_11        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_11, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_12, Function: 1 - VWM_SPARE_12
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_12__VWM_SPARE_12        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_12, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_13, Function: 1 - VWM_SPARE_13
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_13__VWM_SPARE_13        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_13, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_14, Function: 1 - VWM_SPARE_14
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_14__VWM_SPARE_14        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_14, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_15, Function: 1 - VWM_SPARE_15
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_15__VWM_SPARE_15        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_15, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_16, Function: 1 - VWM_SPARE_16
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_16__VWM_SPARE_16        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_16, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_17, Function: 1 - VWM_SPARE_17
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_17__VWM_SPARE_17        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_17, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_18, Function: 1 - VWM_SPARE_18
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_18__VWM_SPARE_18        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_18, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_19, Function: 1 - VWM_SPARE_19
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_19__VWM_SPARE_19        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_19, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_20, Function: 1 - VWM_SPARE_20
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_20__VWM_SPARE_20        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_20, 1)
// Group: vGPIO_4, Pad: vGPIO_SPARE_21, Function: 1 - VWM_SPARE_21
#define GPIOV2_MTL_PCH_S_MUXING__VGPIO_SPARE_21__VWM_SPARE_21        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_VGPIO_SPARE_21, 1)

//
// Pin muxing definitions for Group: GPP_S.
//

// Group: GPP_S, Pad: GPP_S_0, Function: 1 - SNDW0_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_0__SNDW0_CLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_0, 1)
// Group: GPP_S, Pad: GPP_S_0, Function: 3 - RGMII_AUXTS_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_0__RGMII_AUXTS_0              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_0, 3)
// Group: GPP_S, Pad: GPP_S_1, Function: 1 - SNDW0_DATA0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_1__SNDW0_DATA0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_1, 1)
// Group: GPP_S, Pad: GPP_S_1, Function: 3 - RGMII_INT_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_1__RGMII_INT_0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_1, 3)
// Group: GPP_S, Pad: GPP_S_2, Function: 1 - SNDW1_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_2__SNDW1_CLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_2, 1)
// Group: GPP_S, Pad: GPP_S_2, Function: 5 - DMIC_CLK_A_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_2__DMIC_CLK_A_0               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_2, 5)
// Group: GPP_S, Pad: GPP_S_2, Function: 3 - RGMII_RESET_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_2__RGMII_RESET_0              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_2, 3)
// Group: GPP_S, Pad: GPP_S_2, Function: 4 - SNDW0_DATA1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_2__SNDW0_DATA1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_2, 4)
// Group: GPP_S, Pad: GPP_S_3, Function: 1 - SNDW1_DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_3__SNDW1_DATA                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_3, 1)
// Group: GPP_S, Pad: GPP_S_3, Function: 5 - DMIC_DATA_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_3__DMIC_DATA_0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_3, 5)
// Group: GPP_S, Pad: GPP_S_3, Function: 3 - RGMII_PPS_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_3__RGMII_PPS_0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_3, 3)
// Group: GPP_S, Pad: GPP_S_3, Function: 4 - SNDW0_DATA2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_3__SNDW0_DATA2                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_3, 4)
// Group: GPP_S, Pad: GPP_S_4, Function: 1 - SNDW2_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_4__SNDW2_CLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_4, 1)
// Group: GPP_S, Pad: GPP_S_4, Function: 5 - DMIC_CLK_B_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_4__DMIC_CLK_B_0               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_4, 5)
// Group: GPP_S, Pad: GPP_S_4, Function: 3 - RGMII_AUXTS_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_4__RGMII_AUXTS_1              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_4, 3)
// Group: GPP_S, Pad: GPP_S_4, Function: 4 - SNDW0_DATA3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_4__SNDW0_DATA3                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_4, 4)
// Group: GPP_S, Pad: GPP_S_5, Function: 1 - SNDW2_DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_5__SNDW2_DATA                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_5, 1)
// Group: GPP_S, Pad: GPP_S_5, Function: 5 - DMIC_CLK_B_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_5__DMIC_CLK_B_1               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_5, 5)
// Group: GPP_S, Pad: GPP_S_5, Function: 3 - RGMII_INT_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_5__RGMII_INT_1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_5, 3)
// Group: GPP_S, Pad: GPP_S_6, Function: 1 - SNDW3_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_6__SNDW3_CLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_6, 1)
// Group: GPP_S, Pad: GPP_S_6, Function: 5 - DMIC_CLK_A_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_6__DMIC_CLK_A_1               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_6, 5)
// Group: GPP_S, Pad: GPP_S_6, Function: 3 - RGMII_RESET_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_6__RGMII_RESET_1              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_6, 3)
// Group: GPP_S, Pad: GPP_S_7, Function: 1 - SNDW3_DATA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_7__SNDW3_DATA                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_7, 1)
// Group: GPP_S, Pad: GPP_S_7, Function: 5 - DMIC_DATA_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_7__DMIC_DATA_1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_7, 5)
// Group: GPP_S, Pad: GPP_S_7, Function: 3 - RGMII_PPS_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_S_7__RGMII_PPS_1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_S_7, 3)

//
// Pin muxing definitions for Group: GPP_E.
//

// Group: GPP_E, Pad: GPP_E_0, Function: 1 - SATAXPCIE_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_0__SATAXPCIE_0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_0, 1)
// Group: GPP_E, Pad: GPP_E_0, Function: 2 - SATAGP_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_0__SATAGP_0                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_0, 2)
// Group: GPP_E, Pad: GPP_E_0, Function: 6 - IOM_GPPE_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_0__IOM_GPPE_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_0, 6)
// Group: GPP_E, Pad: GPP_E_1, Function: 1 - SATAXPCIE_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_1__SATAXPCIE_1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_1, 1)
// Group: GPP_E, Pad: GPP_E_1, Function: 2 - SATAGP_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_1__SATAGP_1                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_1, 2)
// Group: GPP_E, Pad: GPP_E_1, Function: 6 - IOM_GPPE_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_1__IOM_GPPE_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_1, 6)
// Group: GPP_E, Pad: GPP_E_2, Function: 1 - SATAXPCIE_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_2__SATAXPCIE_2                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_2, 1)
// Group: GPP_E, Pad: GPP_E_2, Function: 2 - SATAGP_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_2__SATAGP_2                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_2, 2)
// Group: GPP_E, Pad: GPP_E_2, Function: 6 - IOM_GPPE_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_2__IOM_GPPE_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_2, 6)
// Group: GPP_E, Pad: GPP_E_3, Function: 1 - CPU_GP_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_3__CPU_GP_0                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_3, 1)
// Group: GPP_E, Pad: GPP_E_3, Function: 6 - IOM_GPPE_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_3__IOM_GPPE_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_3, 6)
// Group: GPP_E, Pad: GPP_E_4, Function: 1 - SATA_DEVSLP_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_4__SATA_DEVSLP_0              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_4, 1)
// Group: GPP_E, Pad: GPP_E_4, Function: 6 - IOM_GPPE_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_4__IOM_GPPE_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_4, 6)
// Group: GPP_E, Pad: GPP_E_5, Function: 1 - SATA_DEVSLP_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_5__SATA_DEVSLP_1              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_5, 1)
// Group: GPP_E, Pad: GPP_E_5, Function: 6 - IOM_GPPE_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_5__IOM_GPPE_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_5, 6)
// Group: GPP_E, Pad: GPP_E_6, Function: 1 - SATA_DEVSLP_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_6__SATA_DEVSLP_2              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_6, 1)
// Group: GPP_E, Pad: GPP_E_6, Function: 6 - IOM_GPPE_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_6__IOM_GPPE_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_6, 6)
// Group: GPP_E, Pad: GPP_E_7, Function: 1 - CPU_GP_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_7__CPU_GP_1                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_7, 1)
// Group: GPP_E, Pad: GPP_E_7, Function: 6 - IOM_GPPE_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_7__IOM_GPPE_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_7, 6)
// Group: GPP_E, Pad: GPP_E_8, Function: 1 - SATA_LEDB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_8__SATA_LEDB                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_8, 1)
// Group: GPP_E, Pad: GPP_E_8, Function: 6 - IOM_GPPE_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_8__IOM_GPPE_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_8, 6)
// Group: GPP_E, Pad: GPP_E_9, Function: 1 - SRCCLKREQB_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_9__SRCCLKREQB_12              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_9, 1)
// Group: GPP_E, Pad: GPP_E_9, Function: 6 - IOM_GPPE_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_9__IOM_GPPE_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_9, 6)
// Group: GPP_E, Pad: GPP_E_10, Function: 1 - SRCCLKREQB_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_10__SRCCLKREQB_13             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_10, 1)
// Group: GPP_E, Pad: GPP_E_10, Function: 6 - IOM_GPPE_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_10__IOM_GPPE_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_10, 6)
// Group: GPP_E, Pad: GPP_E_11, Function: 1 - ISH_GP_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_11__ISH_GP_9                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_11, 1)
// Group: GPP_E, Pad: GPP_E_11, Function: 6 - IOM_GPPE_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_11__IOM_GPPE_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_11, 6)
// Group: GPP_E, Pad: GPP_E_12, Function: 1 - ISH_GP_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_12__ISH_GP_10                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_12, 1)
// Group: GPP_E, Pad: GPP_E_12, Function: 6 - IOM_GPPE_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_12__IOM_GPPE_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_12, 6)
// Group: GPP_E, Pad: GPP_E_13, Function: 2 - THC0_SPI1_CSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_13__THC0_SPI1_CSB             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_13, 2)
// Group: GPP_E, Pad: GPP_E_13, Function: 3 - Serial_Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_13__SERIAL_BLINK              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_13, 3)
// Group: GPP_E, Pad: GPP_E_13, Function: 4 - Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_13__BLINK                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_13, 4)
// Group: GPP_E, Pad: GPP_E_13, Function: 6 - IOM_GPPE_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_13__IOM_GPPE_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_13, 6)
// Group: GPP_E, Pad: GPP_E_14, Function: 2 - THC0_SPI1_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_14__THC0_SPI1_CLK             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_14, 2)
// Group: GPP_E, Pad: GPP_E_14, Function: 3 - Serial_Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_14__SERIAL_BLINK              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_14, 3)
// Group: GPP_E, Pad: GPP_E_14, Function: 4 - Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_14__BLINK                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_14, 4)
// Group: GPP_E, Pad: GPP_E_14, Function: 6 - IOM_GPPE_14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_14__IOM_GPPE_14               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_14, 6)
// Group: GPP_E, Pad: GPP_E_15, Function: 2 - THC0_SPI1_IO_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_15__THC0_SPI1_IO_1            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_15, 2)
// Group: GPP_E, Pad: GPP_E_15, Function: 3 - Serial_Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_15__SERIAL_BLINK              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_15, 3)
// Group: GPP_E, Pad: GPP_E_15, Function: 4 - Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_15__BLINK                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_15, 4)
// Group: GPP_E, Pad: GPP_E_15, Function: 6 - IOM_GPPE_15
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_15__IOM_GPPE_15               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_15, 6)
// Group: GPP_E, Pad: GPP_E_16, Function: 2 - THC0_SPI1_IO_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_16__THC0_SPI1_IO_0            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_16, 2)
// Group: GPP_E, Pad: GPP_E_16, Function: 3 - Serial_Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_16__SERIAL_BLINK              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_16, 3)
// Group: GPP_E, Pad: GPP_E_16, Function: 4 - Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_16__BLINK                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_16, 4)
// Group: GPP_E, Pad: GPP_E_16, Function: 6 - IOM_GPPE_16
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_16__IOM_GPPE_16               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_16, 6)
// Group: GPP_E, Pad: GPP_E_17, Function: 2 - THC0_SPI1_IO_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_17__THC0_SPI1_IO_2            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_17, 2)
// Group: GPP_E, Pad: GPP_E_17, Function: 6 - IOM_GPPE_17
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_17__IOM_GPPE_17               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_17, 6)
// Group: GPP_E, Pad: GPP_E_18, Function: 2 - THC0_SPI1_IO_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_18__THC0_SPI1_IO_3            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_18, 2)
// Group: GPP_E, Pad: GPP_E_18, Function: 6 - IOM_GPPE_18
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_18__IOM_GPPE_18               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_18, 6)
// Group: GPP_E, Pad: GPP_E_19, Function: 2 - THC0_SPI1_RSTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_19__THC0_SPI1_RSTB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_19, 2)
// Group: GPP_E, Pad: GPP_E_19, Function: 6 - IOM_GPPE_19
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_19__IOM_GPPE_19               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_19, 6)
// Group: GPP_E, Pad: GPP_E_20, Function: 2 - THC0_SPI1_INTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_20__THC0_SPI1_INTB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_20, 2)
// Group: GPP_E, Pad: GPP_E_20, Function: 6 - IOM_GPPE_20
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_20__IOM_GPPE_20               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_20, 6)
// Group: GPP_E, Pad: GPP_E_21, Function: 2 - ISH_UART0_CTSB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_21__ISH_UART0_CTSB            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_21, 2)
// Group: GPP_E, Pad: GPP_E_21, Function: 2 - SML0BALERTB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_21__SML0BALERTB               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_21, 2)
// Group: GPP_E, Pad: GPP_E_21, Function: 6 - IOM_GPPE_21
#define GPIOV2_MTL_PCH_S_MUXING__GPP_E_21__IOM_GPPE_21               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_E_21, 6)
// Group: GPP_E, Pad: SPI1_THC0_CLK_LOOPBK, Function: 1 - SPI1_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__SPI1_THC0_CLK_LOOPBK__SPI1_CLK_LOOPBK GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI1_THC0_CLK_LOOPBK, 1)
// Group: GPP_E, Pad: SPI1_THC0_CLK_LOOPBK, Function: 2 - THC0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__SPI1_THC0_CLK_LOOPBK__THC0_CLK_LOOPBK GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SPI1_THC0_CLK_LOOPBK, 2)

//
// Pin muxing definitions for Group: GPP_K.
//

// Group: GPP_K, Pad: GPP_K_0, Function: 1 - GSXDOUT
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_0__GSXDOUT                    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_0, 1)
// Group: GPP_K, Pad: GPP_K_0, Function: 3 - Serial_Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_0__SERIAL_BLINK               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_0, 3)
// Group: GPP_K, Pad: GPP_K_0, Function: 4 - Blink
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_0__BLINK                      GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_0, 4)
// Group: GPP_K, Pad: GPP_K_0, Function: 6 - IOM_GPPK_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_0__IOM_GPPK_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_0, 6)
// Group: GPP_K, Pad: GPP_K_1, Function: 1 - GSXSLOAD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_1__GSXSLOAD                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_1, 1)
// Group: GPP_K, Pad: GPP_K_1, Function: 6 - IOM_GPPK_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_1__IOM_GPPK_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_1, 6)
// Group: GPP_K, Pad: GPP_K_2, Function: 1 - GSXDIN
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_2__GSXDIN                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_2, 1)
// Group: GPP_K, Pad: GPP_K_2, Function: 6 - IOM_GPPK_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_2__IOM_GPPK_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_2, 6)
// Group: GPP_K, Pad: GPP_K_3, Function: 1 - GSXSRESETB
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_3__GSXSRESETB                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_3, 1)
// Group: GPP_K, Pad: GPP_K_3, Function: 6 - IOM_GPPK_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_3__IOM_GPPK_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_3, 6)
// Group: GPP_K, Pad: GPP_K_4, Function: 1 - GSXCLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_4__GSXCLK                     GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_4, 1)
// Group: GPP_K, Pad: GPP_K_4, Function: 6 - IOM_GPPK_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_4__IOM_GPPK_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_4, 6)
// Group: GPP_K, Pad: GPP_K_5, Function: 1 - ADR_COMPLETE
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_5__ADR_COMPLETE               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_5, 1)
// Group: GPP_K, Pad: GPP_K_5, Function: 6 - IOM_GPPK_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_5__IOM_GPPK_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_5, 6)
// Group: GPP_K, Pad: GPP_K_6, Function: 2 - FUSE_SORT_BUMP_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_6__FUSE_SORT_BUMP_0           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_6, 2)
// Group: GPP_K, Pad: GPP_K_6, Function: 6 - IOM_GPPK_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_6__IOM_GPPK_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_6, 6)
// Group: GPP_K, Pad: GPP_K_7, Function: 2 - FUSE_SORT_BUMP_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_7__FUSE_SORT_BUMP_1           GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_7, 2)
// Group: GPP_K, Pad: GPP_K_7, Function: 6 - IOM_GPPK_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_7__IOM_GPPK_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_7, 6)
// Group: GPP_K, Pad: GPP_K_8, Function: 1 - CORE_VID_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_8__CORE_VID_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_8, 1)
// Group: GPP_K, Pad: GPP_K_8, Function: 6 - IOM_GPPK_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_8__IOM_GPPK_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_8, 6)
// Group: GPP_K, Pad: GPP_K_9, Function: 1 - CORE_VID_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_9__CORE_VID_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_9, 1)
// Group: GPP_K, Pad: GPP_K_9, Function: 6 - IOM_GPPK_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_9__IOM_GPPK_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_9, 6)
// Group: GPP_K, Pad: GPP_K_10, Function: 2 - FUSE_SORT_BUMP_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_10__FUSE_SORT_BUMP_2          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_10, 2)
// Group: GPP_K, Pad: GPP_K_10, Function: 6 - IOM_GPPK_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_K_10__IOM_GPPK_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_K_10, 6)
// Group: GPP_K, Pad: MISC_SPARE, Function: 1 - MISC_SPARE
#define GPIOV2_MTL_PCH_S_MUXING__MISC_SPARE__MISC_SPARE              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_MISC_SPARE, 1)
// Group: GPP_K, Pad: SYS_RESETB, Function: 1 - SYS_RESETB
#define GPIOV2_MTL_PCH_S_MUXING__SYS_RESETB__SYS_RESETB              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_SYS_RESETB, 1)
// Group: GPP_K, Pad: MLK_RSTB, Function: 1 - MLK_RSTB
#define GPIOV2_MTL_PCH_S_MUXING__MLK_RSTB__MLK_RSTB                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_MLK_RSTB, 1)

//
// Pin muxing definitions for Group: GPP_F.
//

// Group: GPP_F, Pad: GPP_F_0, Function: 1 - SATAXPCIE_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_0__SATAXPCIE_3                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_0, 1)
// Group: GPP_F, Pad: GPP_F_0, Function: 2 - SATAGP_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_0__SATAGP_3                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_0, 2)
// Group: GPP_F, Pad: GPP_F_0, Function: 3 - VISA2CH1_D0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_0__VISA2CH1_D0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_0, 3)
// Group: GPP_F, Pad: GPP_F_0, Function: 4 - PTI_TRACE_D00
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_0__PTI_TRACE_D00              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_0, 4)
// Group: GPP_F, Pad: GPP_F_0, Function: 5 - VISAFUSECH1_D0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_0__VISAFUSECH1_D0             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_0, 5)
// Group: GPP_F, Pad: GPP_F_0, Function: 6 - IOM_GPPF_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_0__IOM_GPPF_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_0, 6)
// Group: GPP_F, Pad: GPP_F_1, Function: 1 - SATAXPCIE_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_1__SATAXPCIE_4                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_1, 1)
// Group: GPP_F, Pad: GPP_F_1, Function: 2 - SATAGP_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_1__SATAGP_4                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_1, 2)
// Group: GPP_F, Pad: GPP_F_1, Function: 3 - VISA2CH1_D1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_1__VISA2CH1_D1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_1, 3)
// Group: GPP_F, Pad: GPP_F_1, Function: 4 - PTI_TRACE_D01
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_1__PTI_TRACE_D01              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_1, 4)
// Group: GPP_F, Pad: GPP_F_1, Function: 5 - VISAFUSECH1_D1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_1__VISAFUSECH1_D1             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_1, 5)
// Group: GPP_F, Pad: GPP_F_1, Function: 6 - IOM_GPPF_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_1__IOM_GPPF_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_1, 6)
// Group: GPP_F, Pad: GPP_F_2, Function: 1 - SATAXPCIE_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_2__SATAXPCIE_5                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_2, 1)
// Group: GPP_F, Pad: GPP_F_2, Function: 2 - SATAGP_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_2__SATAGP_5                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_2, 2)
// Group: GPP_F, Pad: GPP_F_2, Function: 3 - VISA2CH1_D2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_2__VISA2CH1_D2                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_2, 3)
// Group: GPP_F, Pad: GPP_F_2, Function: 4 - PTI_TRACE_D02
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_2__PTI_TRACE_D02              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_2, 4)
// Group: GPP_F, Pad: GPP_F_2, Function: 5 - VISAFUSECH1_D2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_2__VISAFUSECH1_D2             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_2, 5)
// Group: GPP_F, Pad: GPP_F_2, Function: 6 - IOM_GPPF_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_2__IOM_GPPF_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_2, 6)
// Group: GPP_F, Pad: GPP_F_3, Function: 1 - SATAXPCIE_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_3__SATAXPCIE_6                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_3, 1)
// Group: GPP_F, Pad: GPP_F_3, Function: 2 - SATAGP_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_3__SATAGP_6                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_3, 2)
// Group: GPP_F, Pad: GPP_F_3, Function: 3 - VISA2CH1_D3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_3__VISA2CH1_D3                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_3, 3)
// Group: GPP_F, Pad: GPP_F_3, Function: 4 - PTI_TRACE_D03
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_3__PTI_TRACE_D03              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_3, 4)
// Group: GPP_F, Pad: GPP_F_3, Function: 5 - VISAFUSECH1_D3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_3__VISAFUSECH1_D3             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_3, 5)
// Group: GPP_F, Pad: GPP_F_3, Function: 6 - IOM_GPPF_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_3__IOM_GPPF_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_3, 6)
// Group: GPP_F, Pad: GPP_F_4, Function: 1 - SATAXPCIE_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_4__SATAXPCIE_7                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_4, 1)
// Group: GPP_F, Pad: GPP_F_4, Function: 2 - SATAGP_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_4__SATAGP_7                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_4, 2)
// Group: GPP_F, Pad: GPP_F_4, Function: 3 - VISA2CH1_D4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_4__VISA2CH1_D4                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_4, 3)
// Group: GPP_F, Pad: GPP_F_4, Function: 4 - PTI_TRACE_D04
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_4__PTI_TRACE_D04              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_4, 4)
// Group: GPP_F, Pad: GPP_F_4, Function: 5 - VISAFUSECH1_D4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_4__VISAFUSECH1_D4             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_4, 5)
// Group: GPP_F, Pad: GPP_F_4, Function: 6 - IOM_GPPF_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_4__IOM_GPPF_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_4, 6)
// Group: GPP_F, Pad: GPP_F_5, Function: 1 - SATA_DEVSLP_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_5__SATA_DEVSLP_3              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_5, 1)
// Group: GPP_F, Pad: GPP_F_5, Function: 3 - VISA2CH1_D5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_5__VISA2CH1_D5                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_5, 3)
// Group: GPP_F, Pad: GPP_F_5, Function: 4 - PTI_TRACE_D05
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_5__PTI_TRACE_D05              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_5, 4)
// Group: GPP_F, Pad: GPP_F_5, Function: 5 - VISAFUSECH1_D5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_5__VISAFUSECH1_D5             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_5, 5)
// Group: GPP_F, Pad: GPP_F_5, Function: 6 - IOM_GPPF_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_5__IOM_GPPF_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_5, 6)
// Group: GPP_F, Pad: GPP_F_6, Function: 1 - SATA_DEVSLP_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_6__SATA_DEVSLP_4              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_6, 1)
// Group: GPP_F, Pad: GPP_F_6, Function: 3 - VISA2CH1_D6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_6__VISA2CH1_D6                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_6, 3)
// Group: GPP_F, Pad: GPP_F_6, Function: 4 - PTI_TRACE_D06
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_6__PTI_TRACE_D06              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_6, 4)
// Group: GPP_F, Pad: GPP_F_6, Function: 5 - VISAFUSECH1_D6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_6__VISAFUSECH1_D6             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_6, 5)
// Group: GPP_F, Pad: GPP_F_6, Function: 6 - IOM_GPPF_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_6__IOM_GPPF_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_6, 6)
// Group: GPP_F, Pad: GPP_F_7, Function: 1 - SATA_DEVSLP_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_7__SATA_DEVSLP_5              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_7, 1)
// Group: GPP_F, Pad: GPP_F_7, Function: 3 - VISA2CH1_D7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_7__VISA2CH1_D7                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_7, 3)
// Group: GPP_F, Pad: GPP_F_7, Function: 4 - PTI_TRACE_D07
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_7__PTI_TRACE_D07              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_7, 4)
// Group: GPP_F, Pad: GPP_F_7, Function: 5 - VISAFUSECH1_D7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_7__VISAFUSECH1_D7             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_7, 5)
// Group: GPP_F, Pad: GPP_F_7, Function: 6 - IOM_GPPF_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_7__IOM_GPPF_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_7, 6)
// Group: GPP_F, Pad: GPP_F_8, Function: 1 - SATA_DEVSLP_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_8__SATA_DEVSLP_6              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_8, 1)
// Group: GPP_F, Pad: GPP_F_8, Function: 3 - VISA2CH1_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_8__VISA2CH1_CLK               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_8, 3)
// Group: GPP_F, Pad: GPP_F_8, Function: 4 - PTI_TRACE_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_8__PTI_TRACE_CLK              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_8, 4)
// Group: GPP_F, Pad: GPP_F_8, Function: 5 - VISAFUSECH1_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_8__VISAFUSECH1_CLK            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_8, 5)
// Group: GPP_F, Pad: GPP_F_8, Function: 6 - IOM_GPPF_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_8__IOM_GPPF_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_8, 6)
// Group: GPP_F, Pad: GPP_F_9, Function: 1 - SATA_DEVSLP_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_9__SATA_DEVSLP_7              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_9, 1)
// Group: GPP_F, Pad: GPP_F_9, Function: 3 - VISA2CH2_D0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_9__VISA2CH2_D0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_9, 3)
// Group: GPP_F, Pad: GPP_F_9, Function: 4 - PTI_TRACE_D08
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_9__PTI_TRACE_D08              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_9, 4)
// Group: GPP_F, Pad: GPP_F_9, Function: 5 - DIR_ESPI_SMI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_9__DIR_ESPI_SMI               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_9, 5)
// Group: GPP_F, Pad: GPP_F_9, Function: 6 - IOM_GPPF_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_9__IOM_GPPF_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_9, 6)
// Group: GPP_F, Pad: GPP_F_10, Function: 1 - SATA_SCLOCK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_10__SATA_SCLOCK               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_10, 1)
// Group: GPP_F, Pad: GPP_F_10, Function: 3 - VISA2CH2_D1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_10__VISA2CH2_D1               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_10, 3)
// Group: GPP_F, Pad: GPP_F_10, Function: 4 - PTI_TRACE_D09
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_10__PTI_TRACE_D09             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_10, 4)
// Group: GPP_F, Pad: GPP_F_10, Function: 5 - DIR_ESPI_NMI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_10__DIR_ESPI_NMI              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_10, 5)
// Group: GPP_F, Pad: GPP_F_10, Function: 6 - IOM_GPPF_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_10__IOM_GPPF_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_10, 6)
// Group: GPP_F, Pad: GPP_F_11, Function: 1 - SATA_SLOAD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_11__SATA_SLOAD                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_11, 1)
// Group: GPP_F, Pad: GPP_F_11, Function: 3 - VISA2CH2_D2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_11__VISA2CH2_D2               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_11, 3)
// Group: GPP_F, Pad: GPP_F_11, Function: 4 - PTI_TRACE_D10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_11__PTI_TRACE_D10             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_11, 4)
// Group: GPP_F, Pad: GPP_F_11, Function: 5 - DIR_ESPI_IRQ
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_11__DIR_ESPI_IRQ              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_11, 5)
// Group: GPP_F, Pad: GPP_F_11, Function: 6 - IOM_GPPF_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_11__IOM_GPPF_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_11, 6)
// Group: GPP_F, Pad: GPP_F_12, Function: 1 - SATA_SDATAOUT1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_12__SATA_SDATAOUT1            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_12, 1)
// Group: GPP_F, Pad: GPP_F_12, Function: 3 - VISA2CH2_D3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_12__VISA2CH2_D3               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_12, 3)
// Group: GPP_F, Pad: GPP_F_12, Function: 4 - PTI_TRACE_D11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_12__PTI_TRACE_D11             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_12, 4)
// Group: GPP_F, Pad: GPP_F_12, Function: 5 - DIR_ESPI_WAKE
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_12__DIR_ESPI_WAKE             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_12, 5)
// Group: GPP_F, Pad: GPP_F_12, Function: 6 - IOM_GPPF_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_12__IOM_GPPF_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_12, 6)
// Group: GPP_F, Pad: GPP_F_13, Function: 1 - SATA_SDATAOUT0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_13__SATA_SDATAOUT0            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_13, 1)
// Group: GPP_F, Pad: GPP_F_13, Function: 3 - VISA2CH2_D4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_13__VISA2CH2_D4               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_13, 3)
// Group: GPP_F, Pad: GPP_F_13, Function: 4 - PTI_TRACE_D12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_13__PTI_TRACE_D12             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_13, 4)
// Group: GPP_F, Pad: GPP_F_13, Function: 5 - DIR_ESPI_SCI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_13__DIR_ESPI_SCI              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_13, 5)
// Group: GPP_F, Pad: GPP_F_13, Function: 6 - IOM_GPPF_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_13__IOM_GPPF_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_13, 6)
// Group: GPP_F, Pad: GPP_F_14, Function: 3 - VISA2CH2_D5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_14__VISA2CH2_D5               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_14, 3)
// Group: GPP_F, Pad: GPP_F_14, Function: 4 - PTI_TRACE_D13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_14__PTI_TRACE_D13             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_14, 4)
// Group: GPP_F, Pad: GPP_F_14, Function: 6 - IOM_GPPF_14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_14__IOM_GPPF_14               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_14, 6)
// Group: GPP_F, Pad: GPP_F_15, Function: 3 - VISA2CH2_D6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_15__VISA2CH2_D6               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_15, 3)
// Group: GPP_F, Pad: GPP_F_15, Function: 4 - PTI_TRACE_D14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_15__PTI_TRACE_D14             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_15, 4)
// Group: GPP_F, Pad: GPP_F_15, Function: 6 - IOM_GPPF_15
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_15__IOM_GPPF_15               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_15, 6)
// Group: GPP_F, Pad: GPP_F_16, Function: 3 - VISA2CH2_D7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_16__VISA2CH2_D7               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_16, 3)
// Group: GPP_F, Pad: GPP_F_16, Function: 4 - PTI_TRACE_D15
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_16__PTI_TRACE_D15             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_16, 4)
// Group: GPP_F, Pad: GPP_F_16, Function: 6 - IOM_GPPF_16
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_16__IOM_GPPF_16               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_16, 6)
// Group: GPP_F, Pad: GPP_F_17, Function: 3 - VISA2CH2_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_17__VISA2CH2_CLK              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_17, 3)
// Group: GPP_F, Pad: GPP_F_17, Function: 4 - PTI_TRACE_VLD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_17__PTI_TRACE_VLD             GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_17, 4)
// Group: GPP_F, Pad: GPP_F_17, Function: 6 - IOM_GPPF_17
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_17__IOM_GPPF_17               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_17, 6)
// Group: GPP_F, Pad: GPP_F_18, Function: 6 - IOM_GPPF_18
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_18__IOM_GPPF_18               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_18, 6)
// Group: GPP_F, Pad: GPP_F_19, Function: 1 - DNX_FORCE_RELOAD
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_19__DNX_FORCE_RELOAD          GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_19, 1)
// Group: GPP_F, Pad: GPP_F_19, Function: 6 - IOM_GPPF_19
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_19__IOM_GPPF_19               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_19, 6)
// Group: GPP_F, Pad: GPP_F_20, Function: 1 - GMII_MDC_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_20__GMII_MDC_0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_20, 1)
// Group: GPP_F, Pad: GPP_F_20, Function: 6 - IOM_GPPF_20
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_20__IOM_GPPF_20               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_20, 6)
// Group: GPP_F, Pad: GPP_F_21, Function: 1 - GMII_MDIO_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_21__GMII_MDIO_0               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_21, 1)
// Group: GPP_F, Pad: GPP_F_21, Function: 6 - IOM_GPPF_21
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_21__IOM_GPPF_21               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_21, 6)
// Group: GPP_F, Pad: GPP_F_22, Function: 3 - IEH_CORR_ERR0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_22__IEH_CORR_ERR0B            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_22, 3)
// Group: GPP_F, Pad: GPP_F_22, Function: 6 - IOM_GPPF_22
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_22__IOM_GPPF_22               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_22, 6)
// Group: GPP_F, Pad: GPP_F_23, Function: 3 - IEH_NONFATAL_ERR1B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_23__IEH_NONFATAL_ERR1B        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_23, 3)
// Group: GPP_F, Pad: GPP_F_23, Function: 6 - IOM_GPPF_23
#define GPIOV2_MTL_PCH_S_MUXING__GPP_F_23__IOM_GPPF_23               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_F_23, 6)

//
// Pin muxing definitions for Group: GPP_I.
//

// Group: GPP_I, Pad: GPP_I_0, Function: 1 - GSPI0_CS1B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_0__GSPI0_CS1B                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_0, 1)
// Group: GPP_I, Pad: GPP_I_0, Function: 6 - IOM_GPPI_0
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_0__IOM_GPPI_0                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_0, 6)
// Group: GPP_I, Pad: GPP_I_1, Function: 1 - GSPI1_CS1B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_1__GSPI1_CS1B                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_1, 1)
// Group: GPP_I, Pad: GPP_I_1, Function: 6 - IOM_GPPI_1
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_1__IOM_GPPI_1                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_1, 6)
// Group: GPP_I, Pad: GPP_I_2, Function: 1 - I2C0_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_2__I2C0_SDA                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_2, 1)
// Group: GPP_I, Pad: GPP_I_2, Function: 2 - I3C0_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_2__I3C0_SDA                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_2, 2)
// Group: GPP_I, Pad: GPP_I_2, Function: 6 - IOM_GPPI_2
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_2__IOM_GPPI_2                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_2, 6)
// Group: GPP_I, Pad: GPP_I_3, Function: 1 - I2C0_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_3__I2C0_SCL                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_3, 1)
// Group: GPP_I, Pad: GPP_I_3, Function: 2 - I3C0_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_3__I3C0_SCL                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_3, 2)
// Group: GPP_I, Pad: GPP_I_3, Function: 6 - IOM_GPPI_3
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_3__IOM_GPPI_3                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_3, 6)
// Group: GPP_I, Pad: GPP_I_4, Function: 1 - I2C1_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_4__I2C1_SDA                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_4, 1)
// Group: GPP_I, Pad: GPP_I_4, Function: 6 - IOM_GPPI_4
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_4__IOM_GPPI_4                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_4, 6)
// Group: GPP_I, Pad: GPP_I_5, Function: 1 - I2C1_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_5__I2C1_SCL                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_5, 1)
// Group: GPP_I, Pad: GPP_I_5, Function: 6 - IOM_GPPI_5
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_5__IOM_GPPI_5                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_5, 6)
// Group: GPP_I, Pad: GPP_I_6, Function: 1 - GSPI0_CS0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_6__GSPI0_CS0B                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_6, 1)
// Group: GPP_I, Pad: GPP_I_6, Function: 6 - IOM_GPPI_6
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_6__IOM_GPPI_6                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_6, 6)
// Group: GPP_I, Pad: GPP_I_7, Function: 1 - GSPI0_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_7__GSPI0_CLK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_7, 1)
// Group: GPP_I, Pad: GPP_I_7, Function: 6 - IOM_GPPI_7
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_7__IOM_GPPI_7                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_7, 6)
// Group: GPP_I, Pad: GPP_I_8, Function: 1 - GSPI0_MISO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_8__GSPI0_MISO                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_8, 1)
// Group: GPP_I, Pad: GPP_I_8, Function: 6 - IOM_GPPI_8
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_8__IOM_GPPI_8                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_8, 6)
// Group: GPP_I, Pad: GPP_I_9, Function: 1 - GSPI0_MOSI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_9__GSPI0_MOSI                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_9, 1)
// Group: GPP_I, Pad: GPP_I_9, Function: 6 - IOM_GPPI_9
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_9__IOM_GPPI_9                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_9, 6)
// Group: GPP_I, Pad: GPP_I_10, Function: 1 - GSPI1_CS0B
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_10__GSPI1_CS0B                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_10, 1)
// Group: GPP_I, Pad: GPP_I_10, Function: 6 - IOM_GPPI_10
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_10__IOM_GPPI_10               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_10, 6)
// Group: GPP_I, Pad: GPP_I_11, Function: 1 - GSPI1_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_11__GSPI1_CLK                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_11, 1)
// Group: GPP_I, Pad: GPP_I_11, Function: 2 - NFC_CLK
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_11__NFC_CLK                   GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_11, 2)
// Group: GPP_I, Pad: GPP_I_11, Function: 6 - IOM_GPPI_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_11__IOM_GPPI_11               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_11, 6)
// Group: GPP_I, Pad: GPP_I_12, Function: 1 - GSPI1_MISO
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_12__GSPI1_MISO                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_12, 1)
// Group: GPP_I, Pad: GPP_I_12, Function: 2 - NFC_CLKREQ
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_12__NFC_CLKREQ                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_12, 2)
// Group: GPP_I, Pad: GPP_I_12, Function: 6 - IOM_GPPI_12
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_12__IOM_GPPI_12               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_12, 6)
// Group: GPP_I, Pad: GPP_I_13, Function: 1 - GSPI1_MOSI
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_13__GSPI1_MOSI                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_13, 1)
// Group: GPP_I, Pad: GPP_I_13, Function: 6 - IOM_GPPI_13
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_13__IOM_GPPI_13               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_13, 6)
// Group: GPP_I, Pad: GPP_I_14, Function: 1 - ISH_I2C0_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_14__ISH_I2C0_SDA              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_14, 1)
// Group: GPP_I, Pad: GPP_I_14, Function: 2 - ISH_I3C0_SDA
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_14__ISH_I3C0_SDA              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_14, 2)
// Group: GPP_I, Pad: GPP_I_14, Function: 6 - IOM_GPPI_14
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_14__IOM_GPPI_14               GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_14, 6)
// Group: GPP_I, Pad: GPP_I_15, Function: 1 - ISH_I2C0_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_15__ISH_I2C0_SCL              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_15, 1)
// Group: GPP_I, Pad: GPP_I_15, Function: 2 - ISH_I3C0_SCL
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_15__ISH_I3C0_SCL              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_15, 2)
// Group: GPP_I, Pad: GPP_I_16, Function: 1 - ISH_GP_11
#define GPIOV2_MTL_PCH_S_MUXING__GPP_I_16__ISH_GP_11                 GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GPP_I_16, 1)
// Group: GPP_I, Pad: GSPI0_CLK_LOOPBK, Function: 1 - GSPI0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__GSPI0_CLK_LOOPBK__GSPI0_CLK_LOOPBK  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GSPI0_CLK_LOOPBK, 1)
// Group: GPP_I, Pad: GSPI1_CLK_LOOPBK, Function: 1 - GSPI1_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__GSPI1_CLK_LOOPBK__GSPI1_CLK_LOOPBK  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_GSPI1_CLK_LOOPBK, 1)
// Group: GPP_I, Pad: ISH_I3C0_CLK_LOOPBK, Function: 1 - ISH_I3C0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__ISH_I3C0_CLK_LOOPBK__ISH_I3C0_CLK_LOOPBK GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_ISH_I3C0_CLK_LOOPBK, 1)
// Group: GPP_I, Pad: I3C0_CLK_LOOPBK, Function: 1 - I3C0_CLK_LOOPBK
#define GPIOV2_MTL_PCH_S_MUXING__I3C0_CLK_LOOPBK__I3C0_CLK_LOOPBK    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_I3C0_CLK_LOOPBK, 1)

//
// Pin muxing definitions for Group: JTAG_CPU.
//

// Group: JTAG_CPU, Pad: JTAG_TDO, Function: 1 - JTAG_TDO
#define GPIOV2_MTL_PCH_S_MUXING__JTAG_TDO__JTAG_TDO                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_JTAG_TDO, 1)
// Group: JTAG_CPU, Pad: JTAGX, Function: 1 - JTAGX
#define GPIOV2_MTL_PCH_S_MUXING__JTAGX__JTAGX                        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_JTAGX, 1)
// Group: JTAG_CPU, Pad: PRDYB, Function: 1 - PRDYB
#define GPIOV2_MTL_PCH_S_MUXING__PRDYB__PRDYB                        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_PRDYB, 1)
// Group: JTAG_CPU, Pad: PREQB, Function: 1 - PREQB
#define GPIOV2_MTL_PCH_S_MUXING__PREQB__PREQB                        GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_PREQB, 1)
// Group: JTAG_CPU, Pad: JTAG_TDI, Function: 1 - JTAG_TDI
#define GPIOV2_MTL_PCH_S_MUXING__JTAG_TDI__JTAG_TDI                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_JTAG_TDI, 1)
// Group: JTAG_CPU, Pad: JTAG_TMS, Function: 1 - JTAG_TMS
#define GPIOV2_MTL_PCH_S_MUXING__JTAG_TMS__JTAG_TMS                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_JTAG_TMS, 1)
// Group: JTAG_CPU, Pad: JTAG_TCK, Function: 1 - JTAG_TCK
#define GPIOV2_MTL_PCH_S_MUXING__JTAG_TCK__JTAG_TCK                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_JTAG_TCK, 1)
// Group: JTAG_CPU, Pad: DBG_PMODE, Function: 1 - DBG_PMODE
#define GPIOV2_MTL_PCH_S_MUXING__DBG_PMODE__DBG_PMODE                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_DBG_PMODE, 1)
// Group: JTAG_CPU, Pad: CPU_TRSTB, Function: 1 - CPU_TRSTB
#define GPIOV2_MTL_PCH_S_MUXING__CPU_TRSTB__CPU_TRSTB                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_CPU_TRSTB, 1)
// Group: JTAG_CPU, Pad: CPUPWRGD, Function: 1 - CPUPWRGD
#define GPIOV2_MTL_PCH_S_MUXING__CPUPWRGD__CPUPWRGD                  GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_CPUPWRGD, 1)
// Group: JTAG_CPU, Pad: PM_SPARE0, Function: 1 - PM_SPARE0
#define GPIOV2_MTL_PCH_S_MUXING__PM_SPARE0__PM_SPARE0                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_PM_SPARE0, 1)
// Group: JTAG_CPU, Pad: PM_SPARE1, Function: 1 - PM_SPARE1
#define GPIOV2_MTL_PCH_S_MUXING__PM_SPARE1__PM_SPARE1                GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_PM_SPARE1, 1)
// Group: JTAG_CPU, Pad: CRASHLOG_TRIG_N, Function: 1 - CRASHLOG_TRIG_N
#define GPIOV2_MTL_PCH_S_MUXING__CRASHLOG_TRIG_N__CRASHLOG_TRIG_N    GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_CRASHLOG_TRIG_N, 1)
// Group: JTAG_CPU, Pad: TRIGGER_IN, Function: 1 - TRIGGER_IN
#define GPIOV2_MTL_PCH_S_MUXING__TRIGGER_IN__TRIGGER_IN              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_TRIGGER_IN, 1)
// Group: JTAG_CPU, Pad: TRIGGER_OUT, Function: 1 - TRIGGER_OUT
#define GPIOV2_MTL_PCH_S_MUXING__TRIGGER_OUT__TRIGGER_OUT            GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_TRIGGER_OUT, 1)
// Group: JTAG_CPU, Pad: FBRK_OUT_N, Function: 1 - FBRK_OUT_N
#define GPIOV2_MTL_PCH_S_MUXING__FBRK_OUT_N__FBRK_OUT_N              GPIOV2_PAD_SET_NATIVE_FUNCTION (GPIOV2_MTL_PCH_S_FBRK_OUT_N, 1)


#endif // _GPIOV2_PINS_MTL_PCH_S_H_
