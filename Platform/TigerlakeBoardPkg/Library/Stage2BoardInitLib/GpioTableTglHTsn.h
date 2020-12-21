/** @file
  Tigerlake U RVP GPIO definition table for TSN Initialization

  Copyright (c) 2019 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _TIGERLAKE_H_TSN_GPIO_TABLE_H_
#define _TIGERLAKE_H_TSN_GPIO_TABLE_H_

static GPIO_INIT_CONFIG mGpioTableTglHDdr4SODimm[] =
{
  //{GPIO_VER2_H_GPP_A8,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,      GpioIntDis,              GpioPlatformReset,  GpioTermNone}},  //TP_PCIE_LNK_DOWN
  {GPIO_VER2_H_GPP_A9,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,     GpioIntDis,              GpioPlatformReset,  GpioTermNone}},  //CAM_CLK_EN
  {GPIO_VER2_H_GPP_A11,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,  GpioIntDis,              GpioPlatformReset,  GpioTermNone}},  //eSPI_Alert1
  {GPIO_VER2_H_GPP_A12,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,  GpioIntDis,              GpioPlatformReset,  GpioTermNone}},  //CAM_RST_N
  {GPIO_VER2_H_GPP_A13,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutDefault,  GpioIntDis,              GpioPlatformReset,  GpioTermNone}},  //CAM_PRIVACY_LED

  {GPIO_VER2_H_GPP_B0,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel|GpioIntApic, GpioHostDeepReset,  GpioTermWpu20K, GpioPadConfigUnlock  }}, //SPI_TPM_INT
  //{GPIO_VER2_H_GPP_B2,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntEdge|GpioIntSci,   GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock  }},  //UART_BT_WAKE_N
  {GPIO_VER2_H_GPP_B3,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioHostDeepReset,  GpioTermNone}},  //BT_RF_KILL
  {GPIO_VER2_H_GPP_B4,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //DGPU_PWR_OK
  {GPIO_VER2_H_GPP_B14,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SPI1_TCH_PNL_PWREN
  {GPIO_VER2_H_GPP_B15,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //M.2_SSD2_PWREN
  {GPIO_VER2_H_GPP_B16,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //M.2_SSD1_PWREN
  // (pre-mem) {GPIO_VER2_H_GPP_B17,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //DGPU_PWREN
  {GPIO_VER2_H_GPP_B18,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //I2C_TCH_PNL_PWREN
  {GPIO_VER2_H_GPP_B23,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioHostDeepReset,  GpioTermNone}},  //EC_SLP_S0_CS_N

  {GPIO_VER2_H_GPP_C2,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntEdge|GpioIntApic,  GpioHostDeepReset,  GpioTermNone,   GpioPadConfigUnlock}},  //BT_UART_WAKE_N
  //{GPIO_VER2_H_GPP_C5,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntSci,  GpioHostDeepReset,  GpioTermNone, GpioPadConfigUnlock}},  //CODEC_INT_N
  {GPIO_VER2_H_GPP_C12,    {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntApic, GpioPlatformReset,  GpioTermWpu20K, GpioPadConfigUnlock}},  //CODEC_INT_N
  {GPIO_VER2_H_GPP_C13,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SPK_PD_N
  {GPIO_VER2_H_GPP_C14,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SSD1_RESET
  {GPIO_VER2_H_GPP_C15,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SSD2_RESET

  //{GPIO_VER2_H_GPP_D7,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //I2S2_RXD
  //{GPIO_VER2_H_GPP_D8,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //I2S2_SCLK
  {GPIO_VER2_H_GPP_D11,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //M.2_SSD2_PWR_EN
  {GPIO_VER2_H_GPP_D12,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SPI1_TCH_PNL_RST_N

  //{GPIO_VER2_H_GPP_E1,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //DGPU_PWR_EN_R_N
  {GPIO_VER2_H_GPP_E2,     {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntSci,  GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock}},  //PCIE_SLOT2_WAKE_N
  {GPIO_VER2_H_GPP_E3,     {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntEdge|GpioIntSmi,   GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock}},  //EC_SMI_N
  {GPIO_VER2_H_GPP_E4,     {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntSci,  GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock}},  //PCIE_SLOT1_WAKE_N
  {GPIO_VER2_H_GPP_E6,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PCIE_SLOT2_RESET_N

  //{GPIO_VER2_H_GPP_F0,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,     GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //TC_RETIMER_FORCE_PWR
  //{GPIO_VER2_H_GPP_F3,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //TC_RETIMER_FORCE_PWR
  {GPIO_VER2_H_GPP_F4,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SATA_HDD_PWR_EN
  {GPIO_VER2_H_GPP_F5,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel|GpioIntSci,  GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock}},  //SATA_ODD_DA_N
  {GPIO_VER2_H_GPP_F7,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //ME_PG_LED
  //{GPIO_VER2_H_GPP_F8,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //VISA2CH1_D7_GPP_F_8
  {GPIO_VER2_H_GPP_F9,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //TCP0_DIRECT_AUX_NBIAS
  {GPIO_VER2_H_GPP_F10,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //VISA2CH2_D1_PCH_CONFIG_JUMPER
  {GPIO_VER2_H_GPP_F11,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntApic, GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock }},  //TCH_PAD_INT_N
  {GPIO_VER2_H_GPP_F12,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioHostDeepReset,  GpioTermNone}},  //WIFI_RF_KILL_N
  {GPIO_VER2_H_GPP_F13,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SATA_ODD_PWRGT
  {GPIO_VER2_H_GPP_F15,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioHostDeepReset,  GpioTermNone}},  //SKT OCCUPIED
  {GPIO_VER2_H_GPP_F16,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SATA_HDD_PWREN
  {GPIO_VER2_H_GPP_F18,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PEG_PWREN
  {GPIO_VER2_H_GPP_F23,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PCIE_SLOT1_RST_N

  {GPIO_VER2_H_GPP_G2,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //TCP0_DIRECT_AUX_PBIAS
  {GPIO_VER2_H_GPP_G3,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //CAM_FLASH_TRIG
  {GPIO_VER2_H_GPP_G4,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //CAM_PWR_ON
  {GPIO_VER2_H_GPP_G6,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //CAM_KL_PM_RST_N
  {GPIO_VER2_H_GPP_G7,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //CAM_PMIC_FLASH_STB

  //{GPIO_VER2_H_GPP_H4,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel|GpioIntSci,  GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock}},  //GPIO_SSD_RST_N
  {GPIO_VER2_H_GPP_H6,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //DPIN0_PRSNT
  {GPIO_VER2_H_GPP_H7,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //DPIN1_PRSNT
  {GPIO_VER2_H_GPP_H8,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //DPIN2_PRSNT
  {GPIO_VER2_H_GPP_H9,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //DPIN3_PRSNT
  {GPIO_VER2_H_GPP_H10,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SLTA_MAIN_PWR_EN
  //{GPIO_VER2_H_GPP_H11,  {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PCH_PCIESLOT2_PWREN
  {GPIO_VER2_H_GPP_H12,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SLTC_MAIN_PWR_EN
  {GPIO_VER2_H_GPP_H13,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PEG_SLOT_RST_N
  {GPIO_VER2_H_GPP_H14,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PCIE_SLOT2_PWREN_N
  {GPIO_VER2_H_GPP_H15,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PEG_WAKE_N
  {GPIO_VER2_H_GPP_H16,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel,             GpioPlatformReset,  GpioTermNone}},  //PEG_SEL_N
  {GPIO_VER2_H_GPP_H17,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //CPU_SSD2_PWR_EN
  {GPIO_VER2_H_GPP_H18,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutLow,     GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PCIE_SLOT1_PWREN_N
  {GPIO_VER2_H_GPP_H23,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //M.2_SSD2_RESET_N

  //{GPIO_VER2_H_GPP_I5,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //GPIO_PCIE_SLOT2_WAKE_N
  //{GPIO_VER2_H_GPP_I6,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntLevel|GpioIntApic, GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock  }},  //GPIO_PCIE_SLOT3_WAKE_N
  {GPIO_VER2_H_GPP_I8,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntLevel|GpioIntApic, GpioPlatformReset,  GpioTermNone,  GpioPadConfigUnlock}},  //FPS_INT_N
  {GPIO_VER2_H_GPP_I9,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PERST_N_M2_SLOTB_N
  {GPIO_VER2_H_GPP_I10,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PERST_SLTA

  {GPIO_VER2_H_GPP_J0,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //FPS_RST_N
  {GPIO_VER2_H_GPP_J8,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //I2C_TCH_PNL_RST_N
  {GPIO_VER2_H_GPP_J9,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntEdge,              GpioPlatformReset,  GpioTermNone}},  //I2C_TCH_PNL_INT_N

  {GPIO_VER2_H_GPP_K0,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PERST_SLTC
  {GPIO_VER2_H_GPP_K1,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //WAKE_SLTC
  {GPIO_VER2_H_GPP_K2,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //WAKE_SLTA
  {GPIO_VER2_H_GPP_K3,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //SLTB_MAIN_PWR_EN
  {GPIO_VER2_H_GPP_K4,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirOut,   GpioOutHigh,    GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //PERST_SLTB
  {GPIO_VER2_H_GPP_K5,     {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //WAKE_SLTB
  //{GPIO_VER2_H_GPP_K6,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //GPIO_WLAN_RST_N
  //{GPIO_VER2_H_GPP_K7,   {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutLow,     GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //GPIO_PEG_RST_N
  {GPIO_VER2_H_GPP_K10,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntDis,               GpioPlatformReset,  GpioTermNone}},  //MFR_MODE_DET_STRAP

  {GPIO_VER2_H_GPP_R10,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirIn,    GpioOutDefault, GpioIntEdge,              GpioPlatformReset,  GpioTermNone}},  //SPI1_TCH_PNL_INT_N
  {GPIO_VER2_H_GPP_R12,    {GpioPadModeGpio, GpioHostOwnGpio, GpioDirInInv, GpioOutDefault, GpioIntLevel|GpioIntApic, GpioHostDeepReset,  GpioTermNone,  GpioPadConfigUnlock }},  //TCH_PAD_INT_N
  //{GPIO_VER2_H_GPD7,     {GpioPadModeGpio, GpioHostOwnAcpi, GpioDirOut,   GpioOutHigh,    GpioIntDis,           GpioDswReset,       GpioTermNone}},  //TCP_RETIMER_PERST_N
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_INIT_CONFIG mTglHTsnDeviceGpioTable[] =
{
  // TSN Device
  //
  {GPIO_VER2_H_GPP_G3,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDC0
  {GPIO_VER2_H_GPP_G4,  {GpioPadModeNative1, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDIO0
  {GPIO_VER2_H_GPP_E11, {GpioPadModeNative2, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDC1
  {GPIO_VER2_H_GPP_E12, {GpioPadModeNative2, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_GMII_MDIO1
  {GPIO_VER2_H_GPP_S0,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW1_CLK_RGMII_AUXTS
  {GPIO_VER2_H_GPP_S1,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirIn,     GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW1_DATA_RGMII_INT
  {GPIO_VER2_H_GPP_S2,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSN_SNDW2_CLK_RGMII_RESET
  {GPIO_VER2_H_GPP_S3,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //TSNSNDW1_DATA_DMIC_CLK_B_1_RGMII_PPS
  {GPIO_VER2_H_GPP_S4,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_AUXTS
  {GPIO_VER2_H_GPP_S5,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirIn,     GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_INT
  {GPIO_VER2_H_GPP_S6,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirOut,    GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_RESET
  {GPIO_VER2_H_GPP_S7,  {GpioPadModeNative3, GpioHostOwnDefault, GpioDirInOut,  GpioOutDefault,   GpioIntDis, GpioResetDefault,  GpioTermNone}},  //RGMII_PPS
};

#endif // _TIGERLAKE_U_TSN_GPIO_TABLE_H_
