/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_DEFINES_H_
#define _GPIO_DEFINES_H_

#define GPIO_PADBAR                     0x500

#define GPIO_NORTH_COMMUNITY_LENGTH     0x76C
#define GPIO_NORTHWEST_COMMUNITY_LENGTH 0x764
#define GPIO_WEST_COMMUNITY_LENGTH      0x674
#define GPIO_SOUTHWEST_COMMUNITY_LENGTH 0x654

// PortID offsets
#define GPIO_MMIO_OFFSET_SW     0xC0
#define GPIO_MMIO_OFFSET_S      0xC2
#define GPIO_MMIO_OFFSET_NW     0xC4
#define GPIO_MMIO_OFFSET_N      0xC5
#define GPIO_MMIO_OFFSET_W      0xC7

#define BXT_GPIO_PAD_CONF0_OFFSET   0x0
#define BXT_GPIO_PAD_CONF1_OFFSET   0x4
#define NA                    0xFF
#define LOW           0
#define HIGH          1
#define MASK_WAKE     0
#define UNMASK_WAKE   1

#define ALL_COMMUNITY_GPIO_PIN_NUM   248
#define GPIO_REGS_SIZE                      8

// GPIO pad DW0 MASK
#define PAD_DW0_MASK  0xF961E0FE  // bit 27, 24,  22, 21, 16:13, 7:1 are RO, mask bit [31:28] as they are modified as usual, special operation for them if need.
// GPIO pad DW1 MASK
#define PAD_DW1_MASK  0xFFFC00FF // bit [17:14],[13:10],[9:8]  are R/W

#define PAD_BASE_ADDRESS      0x0C
#define PAD_OWNERSHIP_0       0x20
#define  V_PAD_OWNERSHIP_MASK 0x03
#define  V_PAD_OWNERSHIP_HOST 0x00
#define HOSTSW_OWNERSHIP_0    0x80
#define GPI_IS_0              0x100
#define GPI_IE_0              0x110
#define GPI_GPE_STS_0         0x120
#define GPI_GPE_EN_0          0x130
#define GPI_SMI_STS_0         0x140
#define GPI_SMI_EN_0          0x150
#define EVOUTEN_0             0x210
#define EVMAP_0               0x220

#define EVENT_MUX_SIZE        16 ///< MUX is 16:1 on Broxton
#define WAKE_EVENT_NUM        16 ///< 16 direct IRQ wake events are supported on Broxton in North and Northwest communities.



// GPIO pad and offset definition as GPIO HAS
// North community GPIO pad definition
#define N_GPIO_0              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0000     //GPIO_0
#define N_GPIO_1              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0008     //GPIO_1
#define N_GPIO_2              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0010     //GPIO_2
#define N_GPIO_3              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0018     //GPIO_3
#define N_GPIO_4              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0020     //GPIO_4
#define N_GPIO_5              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0028     //GPIO_5
#define N_GPIO_6              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0030     //GPIO_6
#define N_GPIO_7              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0038     //GPIO_7
#define N_GPIO_8              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0040     //GPIO_8
#define N_GPIO_9              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0048     //GPIO_9
#define N_GPIO_10             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0050     //GPIO_10
#define N_GPIO_11             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0058     //GPIO_11
#define N_GPIO_12             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0060     //GPIO_12
#define N_GPIO_13             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0068     //GPIO_13
#define N_GPIO_14             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0070     //GPIO_14
#define N_GPIO_15             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0078     //GPIO_15
#define N_GPIO_16             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0080     //GPIO_16
#define N_GPIO_17             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0088     //GPIO_17
#define N_GPIO_18             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0090     //GPIO_18
#define N_GPIO_19             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0098     //GPIO_19
#define N_GPIO_20             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00A0     //GPIO_20
#define N_GPIO_21             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00A8     //GPIO_21
#define N_GPIO_22             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00B0     //GPIO_22
#define N_GPIO_23             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00B8     //GPIO_23
#define N_GPIO_24             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00C0     //GPIO_24
#define N_GPIO_25             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00C8     //GPIO_25
#define N_GPIO_26             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00D0     //GPIO_26
#define N_GPIO_27             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00D8     //GPIO_27
#define N_GPIO_28             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00E0     //GPIO_28
#define N_GPIO_29             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00E8     //GPIO_29
#define N_GPIO_30             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00F0     //GPIO_30
#define N_GPIO_31             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x00F8     //GPIO_31
#define N_GPIO_32             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0100     //GPIO_32
#define N_GPIO_33             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0108     //GPIO_33
#define N_GPIO_34             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0110     //PWM0
#define N_GPIO_35             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0118     //PWM1
#define N_GPIO_36             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0120     //PWM2
#define N_GPIO_37             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0128     //PWM3
#define N_GPIO_38             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0130     //LPSS_UART0_RXD
#define N_GPIO_39             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0138     //LPSS_UART0_TXD
#define N_GPIO_40             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0140     //LPSS_UART0_RTS_B
#define N_GPIO_41             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0148     //LPSS_UART0_CTS_B
#define N_GPIO_42             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0150     //LPSS_UART1_RXD
#define N_GPIO_43             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0158     //LPSS_UART1_TXD
#define N_GPIO_44             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0160     //LPSS_UART1_RTS_B
#define N_GPIO_45             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0168     //LPSS_UART1_CTS_B
#define N_GPIO_46             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0170     //LPSS_UART2_RXD
#define N_GPIO_47             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0178     //LPSS_UART2_TXD
#define N_GPIO_48             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0180     //LPSS_UART2_RTS_B
#define N_GPIO_49             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0188     //LPSS_UART2_CTS_B
#define N_GPIO_62             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0190     //GP_CAMERASB00
#define N_GPIO_63             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0198     //GP_CAMERASB01
#define N_GPIO_64             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01A0     //GP_CAMERASB02
#define N_GPIO_65             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01A8     //GP_CAMERASB03
#define N_GPIO_66             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01B0     //GP_CAMERASB04
#define N_GPIO_67             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01B8     //GP_CAMERASB05
#define N_GPIO_68             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01C0     //GP_CAMERASB06
#define N_GPIO_69             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01C8     //GP_CAMERASB07
#define N_GPIO_70             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01D0     //GP_CAMERASB08
#define N_GPIO_71             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01D8     //GP_CAMERASB09
#define N_GPIO_72             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01E0     //GP_CAMERASB10
#define N_GPIO_73             (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01E8     //GP_CAMERASB11
#define N_TCK                 (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01F0     //TCK
#define N_TRST_B              (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x01F8     //TRST_B
#define N_TMS                 (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0200     //TMS
#define N_TDI                 (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0208     //TDI
#define N_CX_PMODE            (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0210     //CX_PMODE
#define N_CX_PREQ_B           (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0218     //CX_PREQ_B
#define N_JTAGX               (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0220     //JTAGX
#define N_CX_PRDY_B           (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0228     //CXPRDY_B
#define N_TDO                 (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0230     //TDO
#define N_CNV_BRI_DT          (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0238     //CNV_BRI_DT
#define N_CNV_BRI_RSP         (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0240     //CNV_BRI_RSP
#define N_CNV_RGI_DT          (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0248     //CNV_RGI_DT
#define N_CNV_RGI_RSP         (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0250     //CNV_RGI_RSP
#define N_SVID0_ALERT_B       (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0258     //SVID0_ALERT_B
#define N_SVID0_DATA          (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0260     //SVID0_DATA
#define N_SVID0_CLK           (((UINT32)GPIO_MMIO_OFFSET_N)<<16)+GPIO_PADBAR+0x0268     //SVID0_CLK

// Northwest community GPIO pad definition
#define NW_GPIO_187           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0000    //HV_DDI0_DDC_SDA
#define NW_GPIO_188           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0008    //HV_DDI0_DDC_SCL
#define NW_GPIO_189           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0010    //HV_DDI1_DDC_SDA
#define NW_GPIO_190           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0018    //HV_DDI1_DDC_SCL
#define NW_GPIO_191           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0020    //DBI_SDA
#define NW_GPIO_192           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0028    //DBI_SCL
#define NW_GPIO_193           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0030    //PANEL0_VDDEN
#define NW_GPIO_194           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0038    //PANEL0_BKLTEN
#define NW_GPIO_195           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0040    //PANEL0_BKLTCTL
#define NW_GPIO_196           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0048    //PANEL1_VDDEN
#define NW_GPIO_197           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0050    //PANEL1_BKLTEN
#define NW_GPIO_198           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0058    //PANEL1_BKLTCTL
#define NW_GPIO_199           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0060    //DBI_CSX
#define NW_GPIO_200           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0068    //DBI_RESX
#define NW_GPIO_201           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0070    //GP_INTD_DSI_TE1
#define NW_GPIO_202           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0078    //GP_INTD_DSI_TE2
#define NW_GPIO_203           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0080    //USB_OC0_B
#define NW_GPIO_204           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0088    //USB_OC1_B
#define NW_PMC_SPI_FS0        (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0090    //PMC_SPI_FS0
#define NW_PMC_SPI_FS1        (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0098    //PMC_SPI_FS1
#define NW_PMC_SPI_FS2        (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00A0    //PMC_SPI_FS2
#define NW_PMC_SPI_RXD        (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00A8    //PMC_SPI_RXD
#define NW_PMC_SPI_TXD        (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00B0    //PMC_SPI_TXD
#define NW_PMC_SPI_CLK        (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00B8    //PMC_SPI_CLK
#define NW_PMIC_PWRGOOD       (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00C0    //PMIC_PWRGOOD
#define NW_PMIC_RESET_B       (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00C8    //PMIC_RESET_B
#define NW_GPIO_213           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00D0    //PMIC_SDWN_B
#define NW_GPIO_214           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00D8    //PMIC_BCUDISW2
#define NW_GPIO_215           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00E0    //PMIC_BCUDISCRIT
#define NW_PMIC_THERMTRIP_B   (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00E8    //PMIC_THERMTRIP_B
#define NW_PMIC_STDBY         (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00F0    //PMIC_STDBY
#define NW_PROCHOT_B          (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x00F8    //PROCHOT_B
#define NW_PMIC_I2C_SCL       (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0100    //PMIC_I2C_SCL
#define NW_PMIC_I2C_SDA       (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0108    //PMIC_I2C_SDA
#define NW_GPIO_74            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0110    //AVS_I2S1_MCLK
#define NW_GPIO_75            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0118    //AVS_I2S1_BCLK
#define NW_GPIO_76            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0120    //AVS_I2S1_WS_SYNC
#define NW_GPIO_77            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0128    //AVS_I2S1_SDI
#define NW_GPIO_78            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0130    //AVS_I2S1_SDO
#define NW_GPIO_79            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0138    //AVS_M_CLK_A1
#define NW_GPIO_80            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0140    //AVS_M_CLK_B1
#define NW_GPIO_81            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0148    //AVS_M_DATA_1
#define NW_GPIO_82            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0150    //AVS_M_CLK_AB2
#define NW_GPIO_83            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0158    //AVS_M_DATA_2
#define NW_GPIO_84            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0160    //AVS_I2S2_MCLK
#define NW_GPIO_85            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0168    //AVS_I2S2_BCLK
#define NW_GPIO_86            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0170    //AVS_I2S2_WS_SYNC
#define NW_GPIO_87            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0178    //AVS_I2S2_SDI
#define NW_GPIO_88            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0180    //AVS_I2S2_SDO
#define NW_GPIO_89            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0188    //AVS_I2S3_BCLK
#define NW_GPIO_90            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0190    //AVS_I2S3_WS_SYNC
#define NW_GPIO_91            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0198    //AVS_I2S3_SDI
#define NW_GPIO_92            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01A0    //AVS_I2S3_SDO
#define NW_GPIO_97            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01A8    //FST_SPI_CS0_B
#define NW_GPIO_98            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01B0    //FST_SPI_CS1_B
#define NW_GPIO_99            (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01B8    //FST_SPI_MOSI_IO0
#define NW_GPIO_100           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01C0    //FST_SPI_MISO_IO1
#define NW_GPIO_101           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01C8    //FST_SPI_IO2
#define NW_GPIO_102           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01D0    //FST_SPI_IO3
#define NW_GPIO_103           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01D8    //FST_SPI_CLK
#define NW_FST_SPI_CLK_FB     (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01E0    //FST_SPI_CLK_FB
#define NW_GPIO_104           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01E8    //GP_SSP_0_CLK
#define NW_GPIO_105           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01F0    //GP_SSP_0_FS0
#define NW_GPIO_106           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x01F8    //GP_SSP_0_FS1
#define NW_GPIO_109           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0200    //GP_SSP_0_RXD
#define NW_GPIO_110           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0208    //GP_SSP_0_TXD
#define NW_GPIO_111           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0210    //GP_SSP_1_CLK
#define NW_GPIO_112           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0218    //GP_SSP_1_FS0
#define NW_GPIO_113           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0220    //GP_SSP_1_FS1
#define NW_GPIO_116           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0228    //GP_SSP_1_RXD
#define NW_GPIO_117           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0230    //GP_SSP_1_TXD
#define NW_GPIO_118           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0238    //GP_SSP_2_CLK
#define NW_GPIO_119           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0240    //GP_SSP_2_FS0
#define NW_GPIO_120           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0248    //GP_SSP_2_FS1
#define NW_GPIO_121           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0250    //GP_SSP_2_FS2
#define NW_GPIO_122           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0258    //GP_SSP_2_RXD
#define NW_GPIO_123           (((UINT32)GPIO_MMIO_OFFSET_NW)<<16)+GPIO_PADBAR+0x0260    //GP_SSP_2_TXD

// West community GPIO pad definition
#define W_GPIO_124            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0000     //LPSS_I2S0_SDA
#define W_GPIO_125            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0008     //LPSS_I2S0_SCL
#define W_GPIO_126            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0010     //LPSS_I2S1_SDA
#define W_GPIO_127            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0018     //LPSS_I2S1_SCL
#define W_GPIO_128            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0020     //LPSS_I2S2_SDA
#define W_GPIO_129            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0028     //LPSS_I2S2_SCL
#define W_GPIO_130            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0030     //LPSS_I2S3_SDA
#define W_GPIO_131            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0038     //LPSS_I2S3_SCL
#define W_GPIO_132            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0040     //LPSS_I2S4_SDA
#define W_GPIO_133            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0048     //LPSS_I2S4_SCL
#define W_GPIO_134            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0050     //LPSS_I2S5_SDA
#define W_GPIO_135            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0058     //LPSS_I2S5_SCL
#define W_GPIO_136            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0060     //LPSS_I2S6_SDA
#define W_GPIO_137            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0068     //LPSS_I2S6_SCL
#define W_GPIO_138            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0070     //LPSS_I2S7_SDA
#define W_GPIO_139            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0078     //LPSS_I2S7_SCL
#define W_GPIO_146            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0080     //ISH_GPIO_0
#define W_GPIO_147            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0088     //ISH_GPIO_1
#define W_GPIO_148            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0090     //ISH_GPIO_2
#define W_GPIO_149            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0098     //ISH_GPIO_3
#define W_GPIO_150            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00a0     //ISH_GPIO_4
#define W_GPIO_151            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00a8     //ISH_GPIO_5
#define W_GPIO_152            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00b0     //ISH_GPIO_6
#define W_GPIO_153            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00b8     //ISH_GPIO_7
#define W_GPIO_154            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00c0     //ISH_GPIO_8
#define W_GPIO_155            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00c8     //ISH_GPIO_9
#define W_GPIO_209            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00d0     //PCIE_CLKREQ0_B
#define W_GPIO_210            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00d8     //PCIE_CLKREQ1_B
#define W_GPIO_211            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00e0     //PCIE_CLKREQ2_B
#define W_GPIO_212            (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00e8     //PCIE_CLKREQ3_B
#define W_OSC_CLK_OUT_0       (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00f0     //OSC_CLK_OUT0
#define W_OSC_CLK_OUT_1       (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x00f8     //OSC_CLK_OUT1
#define W_OSC_CLK_OUT_2       (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0100     //OSC_CLK_OUT2
#define W_OSC_CLK_OUT_3       (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0108     //OSC_CLK_OUT3
#define W_OSC_CLK_OUT_4       (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0110     //OSC_CLK_OUT4
#define W_PMU_AC_PRESENT      (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0118     //PMU_AC_PRESENT
#define W_PMU_BATLOW_B        (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0120     //PMU_BATLOW_B
#define W_PMU_PLTRST_B        (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0128     //PMU_PLTRST_B
#define W_PMU_PWRBTN_B        (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0130     //PMU_PWRBTN_B
#define W_PMU_RESETBUTTON_B   (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0138     //PMU_RESETBUTTON_B
#define W_PMU_SLP_S0_B        (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0140     //PMU_SLP_S0_B
#define W_PMU_SLP_S3_B        (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0148     //PMU_SLP_S3_B
#define W_PMU_SLP_S4_B        (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0150     //PMU_SLP_S4_B
#define W_PMU_SUSCLK          (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0158     //PMU_SUSCLK
#define W_PMU_WAKE_B          (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0160     //PMU_WAKE_B
#define W_SUS_STAT_B          (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0168     //SUS_STAT_B
#define W_SUSPWRDNACK         (((UINT32)GPIO_MMIO_OFFSET_W)<<16)+GPIO_PADBAR+0x0170     //SUSPWRDNACK

// Southwest community GPIO pad definition
#define SW_GPIO_205           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0000    //PCIE_WAKE0_B
#define SW_GPIO_206           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0008    //PCIE_WAKE1_B
#define SW_GPIO_207           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0010    //PCIE_WAKE2_B
#define SW_GPIO_208           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0018    //PCIE_WAKE3_B
#define SW_GPIO_156           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0020    //EMMC0_CLK
#define SW_GPIO_157           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0028    //EMMC0_D0
#define SW_GPIO_158           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0030    //EMMC0_D1
#define SW_GPIO_159           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0038    //EMMC0_D2
#define SW_GPIO_160           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0040    //EMMC0_D3
#define SW_GPIO_161           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0048    //EMMC0_D4
#define SW_GPIO_162           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0050    //EMMC0_D5
#define SW_GPIO_163           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0058    //EMMC0_D6
#define SW_GPIO_164           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0060    //EMMC0_D7
#define SW_GPIO_165           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0068    //EMMC0_CMD0
#define SW_GPIO_166           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0070    //SDIO_CLK
#define SW_GPIO_167           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0078    //SDIO_D0
#define SW_GPIO_168           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0080    //SDIO_D1
#define SW_GPIO_169           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0088    //SDIO_D2
#define SW_GPIO_170           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0090    //SDIO_D3
#define SW_GPIO_171           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0098    //SDIO_CMD
#define SW_GPIO_172           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00A0    //SDCARD_CLK
#define SW_GPIO_173           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00B0    //SDCARD_D0
#define SW_GPIO_174           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00B8    //SDCARD_D1
#define SW_GPIO_175           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00C0    //SDCARD_D2
#define SW_GPIO_176           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00C8    //SDCARD_D3
#define SW_GPIO_177           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00D0    //SDCARD_CD_B
#define SW_GPIO_178           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00D8    //SDCARD_CMD
#define SW_GPIO_186           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00E0    //SDCARD_LVL_WP
#define SW_GPIO_182           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00E8    //EMMC0_STROBE
#define SW_GPIO_183           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00F0    //SDIO_PWR_DOWN_B
#define SW_SMB_ALERTB         (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x00F8    //SMB_ALERTB
#define SW_SMB_CLK            (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0100    //SMB_CLK
#define SW_SMB_DATA           (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0108    //SMB_DATA
#define SW_LPC_ILB_SERIRQ     (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0110    //LPC_ILB_SERIRQ
#define SW_LPC_CLKOUT0        (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0118    //LPC_CLKOUT0
#define SW_LPC_CLKOUT1        (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0120    //LPC_CLKOUT1
#define SW_LPC_AD0            (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0128    //LPC_AD0
#define SW_LPC_AD1            (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0130    //LPC_AD1
#define SW_LPC_AD2            (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0138    //LPC_AD2
#define SW_LPC_AD3            (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0140    //LPC_AD3
#define SW_LPC_CLKRUN         (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0148    //LPC_CLKRUN
#define SW_LPC_FRAMEB         (((UINT32)GPIO_MMIO_OFFSET_SW)<<16)+GPIO_PADBAR+0x0150    //LPC_FRAMEB

typedef enum
{
    Native =0xff,
    GPIO = 0,      // Native, no need to set PAD_VALUE
    GPI = 1,       // GPI, input only in PAD_VALUE
    GPO = 2,       // GPO, output only in PAD_VALUE
    HI_Z = 3,
} GPIO_En;

typedef enum
{
    LO = 0,
    HI = 1,
} GPO_D4;


// Mapping to CONF0 bit 27:24
// Note: Assume "Direct Irq En" is not set, unless specially notified

typedef enum
{
    P_NONE  = 0,        // Pull None
    P_5K_L  = 2,        // Pull Down  5K
    P_20K_L = 4,        // Pull Down  20K
    P_1K_H  = 9,        // Pull Up 1K
    P_2K_H  = 11,       // Pull Up 2K
    P_20K_H = 12,       // Pull Up 20K
    P_5K_H  = 10,       // Pull Up  5K
    P_1K2K_H  = 13,     // Pull Up  1K
    Native_control = 15
} PULL_TYPE;

typedef enum {
    M0 = 0,
    M1,
    M2,
    M3,
    M4,
    M5
} PAD_MODE;

typedef enum
{
  Level =0,
  Edge =1,
  INT_DIS =2,
  BothEdge = 3,
} INT_Type;

typedef enum
{
  PowerGood = 0,
  DeepGPIO =1,
  GPIOReset =2,
  Inactive =3,
} Reset_Cfg;

typedef enum
{
  Maskable,
  NonMaskable,
} mask;

typedef enum
{
  SOUTH     = GPIO_MMIO_OFFSET_S,
  WEST      = GPIO_MMIO_OFFSET_W,
  NORTHWEST = GPIO_MMIO_OFFSET_NW,
  SOUTHWEST = GPIO_MMIO_OFFSET_SW,
  NORTH     = GPIO_MMIO_OFFSET_N
} coms;

typedef enum
{
  IOAPIC =8 ,
  SCI = 4,
  SMI = 2,
  NMI =1,
  NONE = 0,
} GPIO_Route;

/*
InvertRxTx 7:4
0 - No Inversion
1 - Inversion
[0] RX Enable
[1] TX Enable
[2] RX Data
[3] TX Data
*/
typedef enum
{
    No_invert = 0,
    Inverted  = 1,
} InvertRX;

typedef enum
{
  ACPI_D = 0, // ACPI driver
  GPIO_D = 1, // GPIO driver
} HOSTSW;

/*
I/O Standby State
0 = Latch last value driven on TX, TX Enable and RX Enable
1 = Drive 0 with RX disabled and RX drive 0 internally
2 = Drive 0 with RX disabled and RX drive 1 internally
3 = Drive 1 with RX disabled and RX drive 0 internally
4 = Drive 1 with RX disabled and RX drive 1 internally
5 = Drive 0 with RX enabled
6 = Drive 1 with RX enabled
7 = Hi-Z with RX drive 0 internally
8 = Hi-Z with RX drive 1 internally
9 = TX Disabled and RX Enabled (i.e. wake or interrupt)
15 = IO Standby signal is masked for this pad.
*/
typedef enum
{
  Last_Value = 0,
  D0RxDRx0I  = 1,
  D0RxDRx1I  = 2,
  D1RxDRx0I  = 3,
  D1RxDRx1I  = 4,
  D0RxE      = 5,
  D1RxE      = 6,
  HizRx0I    = 7,
  HizRx1I    = 8,
  TxDRxE     = 9,// (i.e. wake or interrupt)
  IOS_Masked = 15,
}IOSSTATE;

/*
I/O Standby Termination
0 = Same as functional pull
1 = Disable Pullup and Pulldown
2 = Enable Pulldown
3 = Enable Pullup
*/
typedef enum
{
  SAME    = 0,
  DisPuPd = 1,
  EnPd    = 2,
  EnPu    = 3,
}IOSTERM;

// Wake enable or disable for direct IRQ GPIO pin
typedef enum
{
  Wake_Disabled = FALSE,
  Wake_Enabled = TRUE,
}WAKE_CAPABLE;

typedef union{
    UINT32 padCnf0;
    struct {
      UINT32 GPIOTxState:1; // 0     GPIO TX State
      UINT32 GPIORxState:1; // 1     GPIO RX State, RO
      UINT32 Reserved1:6;   // 2-7   Reserved, RO
      UINT32 GPIORxTxDis:2; // 8-9  GPIO RX Disable[9], GPIO TX Disable[8]
      UINT32 PMode:3;       // 10-12 Pad Mode, 0h = GPIO Controller controls the Pad; 1h = Native Function 1, if applicable, controls the pad; 2h = Native Function 2, if applicable, controls the pad, etc.
      UINT32 Reserved2:4;   // 13-16 Reserved, RO
      UINT32 GPIRout:4;     // 17-20, Route to IOxAPIC[20], SCI[19], SMI[18], NMI[17]
      UINT32 RXTXEnCfg:2;   // 21-22 RX/TX Enable Config (RXTXEnCfg) RO
      UINT32 RXINV:1;       // 23   RX Invert, 0 = No inversion; 1 = Inversion
      UINT32 PreGfRXSel:1;  // 24 Pre Glitch Filter Stage RX Pad State Select,  RO, not support in BXT
      UINT32 RxEvCfg:2;     // 25-26 0h = Level 1h  = Edge (RxInv=0 for rising edge; 1 for falling edge), 2h  = Disabled ,3h  =  Either rising edge or falling edge
      UINT32 Reserved3:1;   // 27, RO
      UINT32 RXRAW1:1;      // 28 oeverride the RX to 1
      UINT32 RXPadStSel:1;  // 29 RX Pad State Select
      UINT32 PadRstCfg:2;   // 30-31 Pad Reset Config
    }r;
} BXT_CONF_PAD0;

typedef union{
    UINT32 padCnf1;
    struct {
         UINT32 IntSel:7;      // 0-6 Interrupt Select. RO
         UINT32 Reserved:1;    // 7  Reserved.
         UINT32 IOSTerm:2;     // 8-9 I/O Standby Termination (IOSTerm) RW
         UINT32 Term:4;        // 10-13 Termination,
                               // 0 000: none;0 010: 5k wpd;0 100: 20k wpd;1 000: none;1 001: 1k wpu;1 011: 2k wpu;1 010: 5k wpu;
                               // 1 100: 20k wpu;1 101: 1k & 2k wpu;1 111: (optional) Native controller selected by Pad Mode controls the Termination
         UINT32 IOSState:4;    // 14-17 I/O Standby State, I/O Standby is not implemented in BXT, RW
         UINT32 CFIOPadCfg:14; // 18-31 For BXT, this is done thru Family Register if necessary. RO
     }r;
} BXT_CONF_PAD1;



typedef struct {
        BXT_CONF_PAD0  padConfg0;
        BXT_CONF_PAD1  padConfg1;
        UINT8          Community;
        UINT16         MMIO_ADDRESS;
        BOOLEAN        HostSw;
        BOOLEAN        WakeEnabled; // wake enable for direct IRQ pin
        CHAR16*        pad_name;// GPIO Pin Name for debug purpose
} BXT_GPIO_PAD_INIT;

typedef struct {
        UINT32         CommAndOffset;
        CHAR16*        pad_name;
}BXT_GPIO_PAD_TABLE;

typedef struct {
        UINT32         CommAndOffset;
        BXT_CONF_PAD0  padConfg0;
        BXT_CONF_PAD1  padConfg1;
        UINT8          HostSw;
        BOOLEAN        WakeEnabled;
} BXT_GPIO_CONFIG;

typedef union{
    UINT32 DW;
    struct {
        UINT32         Community:     8;
        UINT32         Offset:        12;
        UINT32         HostSw:        1;
        UINT32         DirectIrqWake: 1;
        UINT32         Reserved0:     10;
    }r;
}GPIO_CONFIG_SMIP_HALF0;

typedef union{
    UINT32 DW;
    struct {
        UINT32         padConfig0:    20; // GPIO Pad DW0's R/W bits: [0], [8:12], [17:26], [28:31]
        UINT32         padConfig1:    10; // GPIO pad DW1's R/W bits:[8:17]
        UINT32         Reserved0:     2;
    }r;
}GPIO_CONFIG_SMIP_HALF1;

typedef struct{
    GPIO_CONFIG_SMIP_HALF0 Half0;
    GPIO_CONFIG_SMIP_HALF1 Half1;
}GPIO_CONFIG_SMIP;

// product all items of BXT_GPIO_PAD_INIT
#define BXT_GPIO_PAD_CONF(pad_name, PMode, GPIO_Config, HostSw, GPO_STATE, INT_Trigger, WakeEnabled, Term_H_L, Inverted, GPI_ROUT, IOSState, IOSTerm, MMIO_Offset, Community ) { \
        {((((INT_Trigger) != NA) ? ((UINT32) INT_Trigger << 25) : (UINT32)INT_DIS << 25 )  | (((Inverted) != NA) ? ((UINT32) Inverted << 23) : (UINT32)No_invert << 23) | (((GPI_ROUT) != NA) ? ((UINT32) GPI_ROUT << 17) : (UINT32)NONE << 17) | ((UINT32) PMode << 10) |  (((GPIO_Config) != NA) ? ((UINT32) GPIO_Config << 8 ): (UINT32)0 << 8) | (((GPO_STATE) == HI ) ? HI : LO)) }, \
        {(((IOSState != NA) ? ((UINT32) IOSState << 14 ) : Last_Value) | (((Term_H_L) != NA) ? ((UINT32) Term_H_L << 10) : P_NONE) | ((IOSTerm != NA) ? ((UINT32) IOSTerm << 8) : SAME))}, \
        Community, \
        (MMIO_Offset != NA) ? (MMIO_Offset) : 0, \
        ((HostSw == NA)||(HostSw == ACPI_D))? ACPI_D : GPIO_D,\
        WakeEnabled,\
        NULL \
        }

#define SMIP_Config0_Convert(SmipPadConfig0) ( \
          (((UINT32)SmipPadConfig0 & (BIT16 | BIT17 | BIT18 | BIT19)) << 12) | \
          (((UINT32)SmipPadConfig0 & (0xFFC0)) << 11) | \
          (((UINT32)SmipPadConfig0 & (BIT1 | BIT2 | BIT3 |BIT4 | BIT5))  << 7) | \
          ((UINT32)SmipPadConfig0 & (BIT0)) \
        )

#define SMIP_Config1_Convert(SmipPadConfig1) ( \
          ((UINT32)SmipPadConfig1 & 0x3FF) << 8 \
        )

#endif
