/** @file
  AlderLake PS RVP GPIO definition table for Post-Memory Initialization

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ALDER_LAKE_PS_DDR5_CRB_POSTMEM_GPIO_TABLE_H_
#define _ALDER_LAKE_PS_DDR5_CRB_POSTMEM_GPIO_TABLE_H_

#include <GpioPinsVer2Lp.h>
#include <GpioConfig.h>

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePostMemAdlPsDdr5Rvp[] =
{
  //CPU M.2 SSD1
  {GPIO_VER2_LP_GPP_D14,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//M.2_CPU_SSD1_PWREN
  {GPIO_VER2_LP_GPP_F7,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//M.2_CPU_SSD1_RESET_N
  // CPU M.2 SSD2
  {GPIO_VER2_LP_GPP_C2,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//M.2_CPU_SSD2_PWREN
  {GPIO_VER2_LP_GPP_H1,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//M2_CPU_SSD2_RESET_N
  // X4 Pcie Slot
  {GPIO_VER2_LP_GPP_A14,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutLow,        GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//X4_SLOT_PWREN
  {GPIO_VER2_LP_GPP_F10,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//X4_Slot_RESET
   //WLAN
  {GPIO_VER2_LP_GPP_A13,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,         GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//BT_RF_KILL_N
  {GPIO_VER2_LP_GPP_H2,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//WLAN_RST_N
  //M.2 PCH SSD
  {GPIO_VER2_LP_GPP_D16,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//M2_PCH_SSD_PWREN
  {GPIO_VER2_LP_GPP_H0,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//M2_SSD_RST_N
  //Audio/ EC
  {GPIO_VER2_LP_GPP_H3,   {GpioPadModeGpio,       GpioHostOwnGpio,        GpioDirInInv,   GpioOutDefault,      GpioIntApic|GpioIntLevel,       GpioPlatformReset,     GpioTermNone} },//CODEC_INT_N
  //CAM
  {GPIO_VER2_LP_GPP_B18,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutLow,        GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//CRD_CAM_STROBE
  //CAM Conn 1
  {GPIO_VER2_LP_GPP_F5,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//CRD1_RST_N_CVF_RST_N
  //cam Conn 1 Audio Input HDR /WLAN
  {GPIO_VER2_LP_GPP_A21,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//CRD1_CLK_EN
  ///Cam Conn1  TCH PNL 1
  //CAM conn1/TYPE A USB3
  {GPIO_VER2_LP_GPP_B23,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//ESPI_CS1_HDR_N
  //Cam Conn 2
  {GPIO_VER2_LP_GPP_E15,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//CRD2_RST_N
  {GPIO_VER2_LP_GPP_E16,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//CRD2_PWREN
  {GPIO_VER2_LP_GPP_F4,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//CRD2_CLK_EN
  //CAM/SATA
  {GPIO_VER2_LP_GPP_B14,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,     GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//CRD_PRIVACY_LED
  //EC
  {GPIO_VER2_LP_GPP_A11,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },  //EC_SLP_S0_CS_N
  {GPIO_VER2_LP_GPP_E7,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,    GpioIntSmi|GpioIntLevel,   GpioPlatformReset,     GpioTermNone} },  //GPPC_E7_EC_SMI_N

  //HDMI Input Detect and Wake
  {GPIO_VER2_LP_GPP_H9,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,     GpioIntSci|GpioIntEdge,     GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock } },//CRD2_HDMI_WAKE_N
  {GPIO_VER2_LP_GPD7,     {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,     GpioIntSci|GpioIntEdge,     GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//CRD1_HDMI_WAKE_N

  //TouchPad
  {GPIO_VER2_LP_GPP_A15,  {GpioPadModeGpio,       GpioHostOwnGpio,       GpioDirInInv,      GpioOutDefault,    GpioIntApic|GpioIntEdge,  GpioPlatformReset,     GpioPadConfigUnlock} },// TCH_PAD_INT_N

  //Touch PNL2 /TSN
  {GPIO_VER2_LP_GPP_F17,  {GpioPadModeGpio,         GpioHostOwnAcpi,     GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//GSPI1_CS_CVF
  {GPIO_VER2_LP_GPP_F18,  {GpioPadModeGpio,         GpioHostOwnGpio,     GpioDirInInv,      GpioOutDefault,    GpioIntApic|GpioIntEdge, GpioPlatformReset,     GpioTermNone} },//GSPI1_CS_CVF

  //WLAN/Flash Dec/ sec /ISH SNSR HDR/EC/MECC
  {GPIO_VER2_LP_GPP_D13,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,     GpioIntDis,              GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//WIFI_WAKE_N

  //UART_BT_WAKE_N
  {GPIO_VER2_LP_GPP_E0,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,      GpioIntDis,              GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//UART_BT_WAKE_N
  //WLAN/ SPI TPM HDR
  {GPIO_VER2_LP_GPP_E3,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//WIFI_RF_KILL_N
  //WWAN/LPC TPM HDR
  {GPIO_VER2_LP_GPP_A6,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,    GpioIntSci|GpioIntLevel,   GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//SAR_DPR_PCH
  {GPIO_VER2_LP_GPP_C5,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//WWAN_PERST_N
  {GPIO_VER2_LP_GPP_D15,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//WWAN_DISABLE_N
  {GPIO_VER2_LP_GPP_D17,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//WWAN_PWREN
  {GPIO_VER2_LP_GPP_D18,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,    GpioIntSci|GpioIntLevel,   GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//WWAN_WAKE_N
  {GPIO_VER2_LP_GPP_F14,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirOut,        GpioOutHigh,       GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//WWAN_RST_N
  {GPIO_VER2_LP_GPP_F15,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//WWAN_FCP_OFF_N

  ///
  ///Others
  ///
  {GPIO_VER2_LP_GPP_E4,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutLow,        GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//TC_RETIMER_FORCE_PWR,TBT Re-Timer
  //Not used
  {GPIO_VER2_LP_GPP_A22,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_A_22_DDPC_CTRLDATA
  {GPIO_VER2_LP_GPP_F6,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_F_6_CNV_PA_BLANKING
  {GPIO_VER2_LP_GPP_F9,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_F_9_BOOTMPC
  {GPIO_VER2_LP_GPP_F20,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_F_20_EXT_PWR_GATEB
  {GPIO_VER2_LP_GPP_F21,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_F_21_EXT_PWR_GATE2B
  {GPIO_VER2_LP_GPP_F22,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_F_22_VNN_CTRL_IEH_CORR_ERR0B
  {GPIO_VER2_LP_GPP_F23,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_F_22_VNN_CTRL_IEH_CORR_ERR0B
  {GPIO_VER2_LP_GPP_T2,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_T_2
  {GPIO_VER2_LP_GPP_T3,   {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirIn,        GpioOutDefault,    GpioIntDefault,            GpioPlatformReset,     GpioTermNone} },//GPPC_T_3
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePostMemAdlPsDdr5eCrb[] =
{
  // TO_DO :: Once ADLPS eCRB is available we need to enable the commented GPIOs and test once
  //{GPIO_VER2_LP_GPP_A0,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_0_ESPI_IO_0
  //{GPIO_VER2_LP_GPP_A1,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_0_ESPI_IO_1
  // {GPIO_VER2_LP_GPP_A4,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_4_ESPI_CS0B // It should ne native
  //{GPIO_VER2_LP_GPP_A5,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_5_ESPI_ALERT0B
  //{GPIO_VER2_LP_GPP_A6,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_6_ESPI_ALERT1B
  //{GPIO_VER2_LP_GPP_A7,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_7_SRCCLK_OEB_7
  //{GPIO_VER2_LP_GPP_A9,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_9_ESPI_CLK
  {GPIO_VER2_LP_GPP_A11,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_A_11_PMC_I2C_SDA
  {GPIO_VER2_LP_GPP_A12,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_12_SATAXPCIE_1_SATAGP_1_SRCCLKREQB_9B,
  {GPIO_VER2_LP_GPP_A13,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_A_13_PMC_I2C_SCL,
  {GPIO_VER2_LP_GPP_A14,  {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,        GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_A_14_USB2_OCB_1_DDSP_HPD_3_DISP_MISC_3,
  {GPIO_VER2_LP_GPP_A15,  {GpioPadModeGpio,    GpioHostOwnGpio,     GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_A_15_USB2_OCB_2_DDSP_HPD_4_DISP_MISC_4,
  //{GPIO_VER2_LP_GPP_A16,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_16_USB2_OCB_3_ISH_GP_5,
  {GPIO_VER2_LP_GPP_A17,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_17_DISP_MISC_C,
  {GPIO_VER2_LP_GPP_A18,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_18_DDSP_HPD_B_DISP_MISC_B,
  {GPIO_VER2_LP_GPP_A20,  {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_20_DDSP_HPD_2_DISP_MISC_2,
  {GPIO_VER2_LP_GPP_A21,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_A_21_DDPC_CTRLCLK,
  {GPIO_VER2_LP_GPP_A23,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_A_23_ESPI_CS1B,
  {GPIO_VER2_LP_GPP_B4,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_B_4_CPU_GP_3_ISH_GP_5B,
 // {GPIO_VER2_LP_GPP_B7,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_B_7_ISH_I2C1_SDA_I2C3_SDA,
  //{GPIO_VER2_LP_GPP_B8,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_B_8_ISH_I2C1_SCL_I2C3_SCL,
  {GPIO_VER2_LP_GPP_B14,  {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_B_14_SPKR_TIME_SYNC_1_SATA_LEDB_ISH_GP_6,CRD_PRIVACY_LED,SATA_LED_R_N
  {GPIO_VER2_LP_GPP_B18,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutLow,        GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_B_18_ADR_COMPLETE,CRD_CAM_STROBE,
  {GPIO_VER2_LP_GPP_B23,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_B_23_SML1ALERTB_PCHHOTB,CRD1_PWREN_CVF_IRQ,
  {GPIO_VER2_LP_GPP_C5,   {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_C_5_SML0ALERTB,
  {GPIO_VER2_LP_GPP_D0,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_0_ISH_GP_0_BK_0_SBK_0,
  {GPIO_VER2_LP_GPP_D1,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_1_ISH_GP_1_BK_1_SBK_1,
  {GPIO_VER2_LP_GPP_D2,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_2_ISH_GP_2_BK_2_SBK_2,
  {GPIO_VER2_LP_GPP_D3,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_3_ISH_GP_3_BK_3_SBK_3,
  //As it as impacting SRCCLKREQ1
  //{GPIO_VER2_LP_GPP_D6,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_6_SRCCLKREQB_1,
  {GPIO_VER2_LP_GPP_D8,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_8_SRCCLKREQB_3,
  {GPIO_VER2_LP_GPP_D13,  {GpioPadModeGpio,     GpioHostOwnAcpi,    GpioDirInInv,    GpioOutDefault,    GpioIntSci , GpioHostDeepReset ,GpioTermNone,  GpioPadConfigUnlock}}, // GPPC_D_13_ISH_UART0_RXD_SML0BDATA_I2C4B_SDA,WIFI_WAKE_N,
  {GPIO_VER2_LP_GPP_D14,  {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_14_ISH_UART0_TXD_SML0BCLK_I2C4B_SCL,M.2_CPU_SSD1_PWREN,
  {GPIO_VER2_LP_GPP_D15,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_15_ISH_UART0_RTSB_I2C7B_SDA,
  {GPIO_VER2_LP_GPP_D16,  {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_16_ISH_UART0_CTSB_SML0BALERTB_I2C7B_SCL,M.2_CPU_SSD2_PWREN,
  {GPIO_VER2_LP_GPP_D19,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_D_19_I2S_MCLK1_OUT,
  {GPIO_VER2_LP_GPP_E0,   {GpioPadModeGpio,     GpioHostOwnAcpi,    GpioDirInInv,    GpioOutDefault,    GpioIntSci , GpioHostDeepReset ,GpioTermNone,  GpioPadConfigUnlock}}, // GPPC_E_0_SATAXPCIE_0_SATAGP_0_SRCCLKREQB_9,UART_BT_WAKE_N,
  {GPIO_VER2_LP_GPP_E3,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_3_CPU_GP_0,WIFI_RF_KILL_N,
  {GPIO_VER2_LP_GPP_E4,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutLow,        GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_4_SATA_DEVSLP_0_SRCCLK_OEB_9,TC_RETIMER_FORCE_PWR,
  {GPIO_VER2_LP_GPP_E5,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_5_SATA_DEVSLP_1_SRCCLK_OEB_6,
  {GPIO_VER2_LP_GPP_E6,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_6_THC0_SPI1_RSTB,
  {GPIO_VER2_LP_GPP_E7,   {GpioPadModeGpio,     GpioHostOwnAcpi,    GpioDirInInv,    GpioOutDefault,    GpioIntSmi , GpioPlatformReset ,GpioTermNone,  GpioPadConfigUnlock}}, // GPPC_E_7_CPU_GP_1,SPI_TPM_INT_N,
  {GPIO_VER2_LP_GPP_E10,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_10_THC0_SPI1_CSB_GSPI0_CS0B,
  {GPIO_VER2_LP_GPP_E11,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_11_THC0_SPI1_CLK_GSPI0_CLK,
  {GPIO_VER2_LP_GPP_E12,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_E_12_THC0_SPI1_IO_1_GSPI0_MISO,DG_S0IX_ENTRY_ACK,
  {GPIO_VER2_LP_GPP_E13,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutLow,        GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_E_13_THC0_SPI1_IO_0_GSPI0_MOSI,PD_AIC_S0IX_ENTRY_REQ,
  {GPIO_VER2_LP_GPP_E15,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_15_SRCCLK_OEB_8,HDMI_CSI_RST_N,
  {GPIO_VER2_LP_GPP_E16,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_16_SRCCLKREQB_8,GPPC_E16_HDMI_CSI_WAKE,
  {GPIO_VER2_LP_GPP_E17,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioResetDefault  ,GpioTermNone}}, // GPPC_E_17_THC0_SPI1_INTB,I2C0_PD_IRQ_N,
  {GPIO_VER2_LP_GPP_E18,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_18_DDP1_CTRLCLK_TBT_LSX0_A,
  {GPIO_VER2_LP_GPP_E19,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_19_DDP1_CTRLDATA_TBT_LSX0_B,GPPC_E19_DDP1_CTRLDATA_TBT_LSX0_RXD,
  {GPIO_VER2_LP_GPP_E20,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_20_DDP2_CTRLCLK_TBT_LSX1_A,
  {GPIO_VER2_LP_GPP_E21,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_21_DDP2_CTRLDATA_TBT_LSX1_B,GPPC_E21_DDP2_CTRLDATA_TBT_LSX1_RXD,
  {GPIO_VER2_LP_GPP_E23,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutLow,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_E_23_DDPA_CTRLDATA,DNX_IN_PROG,
  {GPIO_VER2_LP_GPP_F4,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_4_CNV_RF_RESET_B,HDMI_CSI_INT_GPIO5_3P3,
  {GPIO_VER2_LP_GPP_F5,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_5_MODEM_CLKREQ_CRF_XTAL_CLKREQ,CRD1_RST_N_CVF_RST_N,

  {GPIO_VER2_LP_GPP_F7,   {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_7,M2_CPU_SSD1_RESET_N,

  {GPIO_VER2_LP_GPP_F10,  {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,        GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_10,X4_Slot_RESET,
  {GPIO_VER2_LP_GPP_F14,  {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_14_GSXDIN_THC1_SPI2_IO_2,GPPC_E17_I2C0_PD_IRQ_N,
  {GPIO_VER2_LP_GPP_F15,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_15_GSXSRESETB_THC1_SPI2_IO_3,
  {GPIO_VER2_LP_GPP_F17,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_17_GMII_MDC_THC1_SPI2_RSTB,
  {GPIO_VER2_LP_GPP_F18,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_18_GMII_MDIO_THC1_SPI2_INTB,

  {GPIO_VER2_LP_GPP_H0,   {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_0,
  {GPIO_VER2_LP_GPP_H1,   {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_1,M2_CPU_SSD2_RESET_N,
  {GPIO_VER2_LP_GPP_H2,   {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirOut,      GpioOutHigh,       GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_2,WLAN_RST_N,
  {GPIO_VER2_LP_GPP_H3,   {GpioPadModeGpio,    GpioHostOwnGpio,     GpioDirInInv,    GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_3_SX_EXIT_HOLDOFFB_IEH_FATAL_ERR2B,
  {GPIO_VER2_LP_GPP_H8,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_8_I2C4_SDA_CNV_MFUART2_RXD,
 // {GPIO_VER2_LP_GPP_H9,   {GpioPadModeGpio,    GpioHostOwnAcpi,     GpioDirIn,       GpioOutDefault,    GpioIntSci , GpioHostDeepReset ,GpioTermNone,  GpioPadConfigUnlock}}, // GPPC_H9_HDMI_WAKE_N,CRD2_HDMI_WAKE_N,
  {GPIO_VER2_LP_GPP_H17,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_17_DDPB_CTRLDATA,
  {GPIO_VER2_LP_GPP_H20,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_20_IMGCLKOUT_1,
  {GPIO_VER2_LP_GPP_H22,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_22_IMGCLKOUT_3,
  {GPIO_VER2_LP_GPP_H23,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_H_23_SRCCLKREQB_5,GPPC_H23_SRCCLKREQ5_N,

  /// Pins not in used
  // {GPIO_VER2_LP_GPP_A22,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_A_22_DDPC_CTRLDATA,
  // {GPIO_VER2_LP_GPP_F6,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_6_CNV_PA_BLANKING,Not used / Removed from ADL-PS Package,
  // {GPIO_VER2_LP_GPP_F9,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_9_BOOTMPC,Not used / Removed from ADL-PS Package,
  // {GPIO_VER2_LP_GPP_F20,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_20_EXT_PWR_GATEB,Not used / Removed from ADL-PS Package,
  // {GPIO_VER2_LP_GPP_F21,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_21_EXT_PWR_GATE2B,Not used / Removed from ADL-PS Package,
  // {GPIO_VER2_LP_GPP_F22,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_22_VNN_CTRL_IEH_CORR_ERR0B,Not used / Removed from ADL-PS Package,
  // {GPIO_VER2_LP_GPP_F23,  {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPPC_F_23_V1P05_CTRL_IEH_NONFATAL_ERR1B,Not used / Removed from ADL-PS Package,
  // {GPIO_VER2_LP_GPP_T2,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPIO_VER2_LP_GPP_T2,Not used / Removed from ADL-PS Package,
  // {GPIO_VER2_LP_GPP_T3,   {GpioPadModeGpio,    GpioHostOwnDefault,  GpioDirIn,       GpioOutDefault,    GpioIntDis , GpioPlatformReset ,GpioTermNone}}, // GPIO_VER2_LP_GPP_T2,Not used / Removed from ADL-PS Package,
};

#endif
