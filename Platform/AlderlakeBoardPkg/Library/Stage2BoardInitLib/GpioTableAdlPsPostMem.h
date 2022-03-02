/** @file
  AlderLake PS RVP GPIO definition table for Post-Memory Initialization

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
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

  //DNX/DDIA DDC
  {GPIO_VER2_LP_GPP_E23,  {GpioPadModeGpio,       GpioHostOwnDefault,    GpioDirOut,        GpioOutHigh,       GpioIntDefault,           GpioPlatformReset,     GpioTermNone} },//DNX_IN_PROG

  //HDMI Input Detect and Wake
  {GPIO_VER2_LP_GPP_H9,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,     GpioIntSci|GpioIntEdge,     GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock } },//CRD2_HDMI_WAKE_N
  {GPIO_VER2_LP_GPD7,     {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,     GpioIntSci|GpioIntEdge,     GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//CRD1_HDMI_WAKE_N

  //TouchPad
  {GPIO_VER2_LP_GPP_A15,  {GpioPadModeGpio,       GpioHostOwnGpio,       GpioDirInInv,      GpioOutDefault,    GpioIntApic|GpioIntEdge,  GpioPlatformReset,     GpioTermNone} },// TCH_PAD_INT_N

  //Touch PNL2 /TSN
  {GPIO_VER2_LP_GPP_F17,  {GpioPadModeGpio,         GpioHostOwnAcpi,     GpioDirOut,        GpioOutHigh,       GpioIntDefault,          GpioPlatformReset,     GpioTermNone} },//GSPI1_CS_CVF
  {GPIO_VER2_LP_GPP_F18,  {GpioPadModeGpio,         GpioHostOwnGpio,     GpioDirInInv,      GpioOutDefault,    GpioIntApic|GpioIntEdge, GpioPlatformReset,     GpioTermNone} },//GSPI1_CS_CVF

  //WLAN/Flash Dec/ sec /ISH SNSR HDR/EC/MECC
  {GPIO_VER2_LP_GPP_D13,  {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,     GpioIntSci|GpioIntLevel, GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//WIFI_WAKE_N

  //UART_BT_WAKE_N
  {GPIO_VER2_LP_GPP_E0,   {GpioPadModeGpio,       GpioHostOwnAcpi,       GpioDirInInv,      GpioOutDefault,      GpioIntSci|GpioIntLevel, GpioHostDeepReset,     GpioTermNone,  GpioPadConfigUnlock} },//UART_BT_WAKE_N
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

#endif // _ALDER_LAKE_PS_DDR5_CRB_POSTMEM_GPIO_TABLE_H_
