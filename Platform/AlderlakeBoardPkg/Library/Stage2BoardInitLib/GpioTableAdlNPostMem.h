/** @file
  AlderLake N GPIO definition table for Post-Memory Initialization

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _ALDER_LAKE_N_POSTMEM_GPIO_TABLE_H_
#define _ALDER_LAKE_N_POSTMEM_GPIO_TABLE_H_

#include <GpioPinsVer2Lp.h>
#include <GpioConfig.h>

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePostMemAdlNDdr5Crb[] =
{
  // M.2 Key-E - WLAN/BT
  {GPIO_VER2_LP_GPP_A13,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioHostDeepReset,  GpioTermNone,}},  // BT_RF_KILL_N
  {GPIO_VER2_LP_GPP_E3,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioHostDeepReset,  GpioTermNone,}},  //WIFI_RF_KILL_N
  {GPIO_VER2_LP_GPP_D13,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock }}, // WIFI_WAKE_N
  {GPIO_VER2_LP_GPP_E0,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock }}, // UART_BT_WAKE_N
  {GPIO_VER2_LP_GPP_H2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioResumeReset,    GpioTermNone}},  // WLAN_RST_N

  // PCH M.2 SSD
  //Configured in the pre mem table
  //{GPIO_VER2_LP_GPP_D16, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // M2_PCH_SSD_PWREN
  {GPIO_VER2_LP_GPP_H0,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // M2_SSD_RST_N

  // Camera
  {GPIO_VER2_LP_GPP_R5,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD1_RST_N
  {GPIO_VER2_LP_GPP_A7,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD1_CLK_EN
  {GPIO_VER2_LP_GPP_A11,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD2_RST_N
  {GPIO_VER2_LP_GPP_D15,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD2_PWREN

  {GPIO_VER2_LP_GPP_B14,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutDefault,   GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD_PRIVACY_LED
  {GPIO_VER2_LP_GPP_B18,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,       GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD_CAM_STROBE
  {GPIO_VER2_LP_GPP_B23,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD1_PWREN

  //X1 SLOT
  //Configured in the pre mem table
  //{GPIO_VER2_LP_GPP_A8,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // X1_SLOT_PWREN
  {GPIO_VER2_LP_GPP_F10,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // X1_Slot_RESET
  {GPIO_VER2_LP_GPP_A21,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset, GpioTermNone, GpioPadConfigUnlock}},  // X1_SLOT_WAKE

  // SPI TPM
  {GPIO_VER2_LP_GPP_D14,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock  }},  // SPI_TPM_INT_N

  // Audio
  {GPIO_VER2_LP_GPP_H3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntApic,  GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock  }},  // CODEC_INT_N
  {GPIO_VER2_LP_GPP_H1,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // AUDIO_PWREN
  {GPIO_VER2_LP_GPP_F16,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // PCH_CODEC_PD_N

  //Pnp
  {GPIO_VER2_LP_GPD7,      {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone,}},  // DEBUG_TRACE_PNP

  // TCP0 - TUSB1044 Redriver
  {GPIO_VER2_LP_GPP_E22,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone,}},  // TCP0_DISP_AUX_P_BIAS_GPIO
  {GPIO_VER2_LP_GPP_E23,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,       GpioIntDis,                GpioPlatformReset,  GpioTermNone,}},  // TCP0_DISP_AUX_N_BIAS_GPIO

  //HDMIB ALS
  {GPIO_VER2_LP_GPP_A17,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone,}},  // HDMIB_ALS_PWR_EN


  // TI PDC
  {GPIO_VER2_LP_GPP_F13,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault,   GpioIntEdge|GpioIntApic,   GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock  }},  // PD_CNTL_I2C_IRQ_N

  //I225 FOXVILLE LAN // LAN_FOX_ON not needed in CRB
  {GPIO_VER2_LP_GPP_E5,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone,}},  // LAN_I225_DISABLE_N_R
  {GPIO_VER2_LP_GPP_H23,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone,}},  // LAN_I225_RST_N
  {GPIO_VER2_LP_GPD2,      {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntEdge,               GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}},  // this is needed to be in NF mode1 FOX_WAKE_N. Set to SCI based on the setup option

  // PWR BTN GPIO
//  {GPIO_VER2_LP_GPP_H19,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault,   GpioIntEdge|GpioIntSci,    GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock  }},  // PWRBTN_GPIO_N
  {GPIO_VER2_LP_GPP_H19,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntBothEdge|GpioIntSci,    GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock  }},  // PWRBTN_GPIO_N

  // Unused pins set to high impedance
  {GPIO_VER2_LP_GPP_F11,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault,   GpioIntDefault,            GpioResetDefault,   GpioTermNone}},  // HiZ
  {GPIO_VER2_LP_GPP_S5,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault,   GpioIntDefault,            GpioResetDefault,   GpioTermNone}},  // HiZ
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mGpioTablePostMemAdlNLpddr5Rvp[] =
{
  // M.2 Key-E - WLAN/BT
  {GPIO_VER2_LP_GPP_A13,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioHostDeepReset,  GpioTermNone,}},                          // BT_RF_KILL_N
  {GPIO_VER2_LP_GPP_E3,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioHostDeepReset,  GpioTermNone,}},                          // WIFI_RF_KILL_N
  {GPIO_VER2_LP_GPP_D13,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset,  GpioTermNone,   GpioPadConfigUnlock }},   // WIFI_WAKE_N
  {GPIO_VER2_LP_GPP_E0,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset,  GpioTermNone,   GpioPadConfigUnlock }},   // UART_BT_WAKE_N
  {GPIO_VER2_LP_GPP_H2,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioResumeReset,    GpioTermNone }},                          // WLAN_RST_N

  // PCH M.2 SSD
  //Configured in the pre mem table
  //{GPIO_VER2_LP_GPP_D16, {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // M2_PCH_SSD_PWREN
  {GPIO_VER2_LP_GPP_H0,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // M2_SSD_RST_N

  // Camera
  {GPIO_VER2_LP_GPP_A7,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD1_CLK_EN
  {GPIO_VER2_LP_GPP_E15,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD2_RST_N

  {GPIO_VER2_LP_GPP_B18,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,       GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD_CAM_STROBE
  {GPIO_VER2_LP_GPP_B23,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // CRD1_PWREN

  //x1 SLOT
  //Configured in the pre mem table
  //{GPIO_VER2_LP_GPP_A8,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,       GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // X1_SLOT_PWREN
  {GPIO_VER2_LP_GPP_F10,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // X1_Slot_RESET
  {GPIO_VER2_LP_GPP_D11,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock }},  // X1_SLOT_WAKE

  // Touch Pad
  {GPIO_VER2_LP_GPP_A15,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault,   GpioIntEdge|GpioIntApic,   GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock}}, // TCH_PAD_INT_N
  {GPIO_VER2_LP_GPP_H1,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone  }},                     // TCH_PAD_TCH_PNL2_LS_EN

  // EC
  {GPIO_VER2_LP_GPP_E7,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSmi,   GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock}}, // GPPC_E7_EC_SMI_N
  {GPIO_VER2_LP_GPP_E4,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // EC_SLP_S0_CS_N

  // SPI TPM
  {GPIO_VER2_LP_GPP_D14,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntSci,   GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock}}, // SPI_TPM_INT_N

  // Audio
  {GPIO_VER2_LP_GPP_H3,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault,   GpioIntLevel|GpioIntApic,  GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock}},  // CODEC_INT_N

  //Pnp
  {GPIO_VER2_LP_GPD7,      {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone,}},  // DEBUG_TRACE_PNP

  // TCP0
  {GPIO_VER2_LP_GPP_E22,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,      GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // TCP0_DISP_AUX_P_BIAS_GPIO
  {GPIO_VER2_LP_GPP_E23,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,       GpioIntDis,                GpioPlatformReset,  GpioTermNone}},  // TCP0_DISP_AUX_N_BIAS_GPIO

  // Unused pins set to high impedance
  {GPIO_VER2_LP_GPP_A22,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault,   GpioIntDefault,            GpioResetDefault,   GpioTermNone}}, // HiZ
  {GPIO_VER2_LP_GPP_H23,   {GpioPadModeGpio, GpioHostOwnGpio, GpioDirNone,  GpioOutDefault,   GpioIntDefault,            GpioResetDefault,   GpioTermNone}}, // HiZ
};

#endif // _ALDER_LAKE_N_POSTMEM_GPIO_TABLE_H_
